#!/usr/bin/env python3
"""Phase 1 readiness gate for the Tushi rebar reproduction docs.

This gate is intentionally conservative. It verifies that required evidence
artifacts exist and reports why the current state can or cannot be called
M1-Formal-Ready. It does not replace Qt6 application runtime tests.
"""

from __future__ import annotations

import argparse
import csv
import json
import re
from dataclasses import dataclass
from datetime import datetime, timezone, timedelta
from pathlib import Path
from typing import Any


CN_TZ = timezone(timedelta(hours=8))
PENDING_REPORT_MARKERS = (
    "pending_after_docs",
    "pending_before_final_verification",
    "pending_before_commit",
    "pending_final",
    "pending_update_after_verification",
    "pending_readonly_review",
    "waiting_rereview",
    "not_run_before_commit_review",
    "not_run_until_docs_mapping",
)
JSONLESS_DONE_NODE_REPORT_TODOS = {
    # Historical app report created before JSON sibling became mandatory.
    "TODO-021",
}
DONE_NODE_JSON_CONTRACTS = {
    "TODO-081": {
        "todoId": "TODO-081",
        "decision": "done_static_payload_role_evidence",
        "nextTodoId": "TODO-082",
        "requiredVerification": ("readinessGateUnit", "readinessGateStrict", "xhighReview"),
    },
}


@dataclass(frozen=True)
class GateCheck:
    gate: str
    item: str
    severity: str
    ok: bool
    path: str
    evidence: list[str]
    gap: list[str]
    message: str

    def to_json(self) -> dict[str, Any]:
        return {
            "gate": self.gate,
            "item": self.item,
            "severity": self.severity,
            "ok": self.ok,
            "path": self.path,
            "evidence": self.evidence,
            "gap": self.gap,
            "message": self.message,
        }


def root_dir() -> Path:
    return Path(__file__).resolve().parents[2]


def rel(root: Path, path: Path) -> str:
    try:
        return path.relative_to(root).as_posix()
    except ValueError:
        return path.as_posix()


def exists_check(
    root: Path,
    gate: str,
    item: str,
    path: Path,
    evidence: list[str],
    gap: list[str],
    severity: str = "error",
    message_ok: str = "present",
    message_missing: str = "missing",
) -> GateCheck:
    ok = path.exists()
    return GateCheck(
        gate=gate,
        item=item,
        severity=severity,
        ok=ok,
        path=rel(root, path),
        evidence=evidence,
        gap=gap,
        message=message_ok if ok else message_missing,
    )


def read_json(path: Path) -> dict[str, Any] | None:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return None


def read_text_lossy(path: Path) -> str:
    for encoding in ("utf-8-sig", "utf-8", "gb18030"):
        try:
            return path.read_text(encoding=encoding)
        except UnicodeDecodeError:
            continue
        except Exception:
            return ""
    try:
        return path.read_text(errors="ignore")
    except Exception:
        return ""


def iter_text_files(base: Path) -> list[Path]:
    if not base.exists():
        return []
    suffixes = {
        ".bat",
        ".cmake",
        ".cpp",
        ".csv",
        ".h",
        ".hpp",
        ".json",
        ".md",
        ".py",
        ".txt",
        ".xml",
    }
    ignored_dirs = {"build", "__pycache__", ".git", ".vs"}
    files: list[Path] = []
    for path in base.rglob("*"):
        if not path.is_file():
            continue
        if any(part in ignored_dirs for part in path.parts):
            continue
        if path.suffix.lower() in suffixes:
            files.append(path)
    return files


def find_pattern_hits(root: Path, bases: list[Path], pattern: re.Pattern[str]) -> list[str]:
    hits: list[str] = []
    for base in bases:
        for path in iter_text_files(base):
            text = read_text_lossy(path)
            if not text:
                continue
            if pattern.search(text):
                hits.append(rel(root, path))
    return sorted(set(hits))


def read_todo_rows(root: Path) -> list[dict[str, str]] | None:
    todo = root / "todo.csv"
    if not todo.exists():
        return None
    try:
        with todo.open("r", encoding="utf-8-sig", newline="") as handle:
            return [dict(row) for row in csv.DictReader(handle)]
    except Exception:
        return None


def done_node_json_contract_issues(todo_id: str, report_json: dict[str, Any], report_path: str) -> list[str]:
    contract = DONE_NODE_JSON_CONTRACTS.get(todo_id)
    if not contract:
        return []

    issues: list[str] = []
    if report_json.get("todoId") != contract["todoId"]:
        issues.append(f"{todo_id}:{report_path}:json_todoId_mismatch")
    if report_json.get("decision") != contract["decision"]:
        issues.append(f"{todo_id}:{report_path}:json_decision_mismatch")

    next_report = report_json.get("next")
    if not isinstance(next_report, dict) or next_report.get("todoId") != contract["nextTodoId"]:
        issues.append(f"{todo_id}:{report_path}:json_nextTodoId_mismatch")

    verification = report_json.get("verification")
    if not isinstance(verification, dict):
        issues.append(f"{todo_id}:{report_path}:json_verification_missing")
        return issues

    for key in contract["requiredVerification"]:
        if key not in verification:
            issues.append(f"{todo_id}:{report_path}:json_verification_{key}_missing")
    return issues


def done_node_report_requirements(root: Path, rows: list[dict[str, str]]) -> tuple[list[str], list[str], list[str]]:
    known_reports = {
        "TODO-010": ("47_M1-App-010LegacyGeometryAdapterP3B实现记录.md", "docs/phase1/app_build_reports/m1_app_010_run_001.md"),
        "TODO-011": ("48_M1-App-011LegacyGeometryAdapterP3C实现记录.md", "docs/phase1/app_build_reports/m1_app_011_run_001.md"),
        "TODO-012": ("49_M1-App-012LegacyGeometryAdapterP3D实现记录.md", "docs/phase1/app_build_reports/m1_app_012_run_001.md"),
        "TODO-013": ("50_M1-App-013LegacyGeometryAdapterP3E实现记录.md", "docs/phase1/app_build_reports/m1_app_013_run_001.md"),
        "TODO-014": ("51_M1-App-014LegacyWireChain实现记录.md", "docs/phase1/app_build_reports/m1_app_014_run_001.md"),
        "TODO-015": ("52_M1-App-015LegacyGeometryAdapterOffsetSpike实现记录.md", "docs/phase1/app_build_reports/m1_app_015_run_001.md"),
        "TODO-016": ("53_M1-App-016LegacyGeometryAdapterSectionSpike实现记录.md", "docs/phase1/app_build_reports/m1_app_016_run_001.md"),
        "TODO-017": ("54_M1-App-017LegacyGeometryAdapterSweepBoundary实现记录.md", "docs/phase1/app_build_reports/m1_app_017_run_001.md"),
        "TODO-018": ("55_M1-App-018RebarDomainModelFreezeP1实现记录.md", "docs/phase1/app_build_reports/m1_app_018_run_001.md"),
        "TODO-019": ("56_M1-App-019LegacyCommandContractP1实现记录.md", "docs/phase1/app_build_reports/m1_app_019_run_001.md"),
        "TODO-020": ("57_TODO-020_IDA旧线筋弧筋链补证据记录.md", "docs/phase1/ida_reports/todo_020_ida_line_arc_chain_run_001.md"),
        "TODO-021": ("58_M1-App-020旧线筋弧筋创建算法P0实现记录.md", "docs/phase1/app_build_reports/m1_app_020_run_001.md"),
        "TODO-022": ("59_M1-App-021AIS钢筋显示映射实现记录.md", "docs/phase1/app_build_reports/m1_app_021_run_001.md"),
        "TODO-023": ("60_M1-App-022新设计文件格式RuntimeP1实现记录.md", "docs/phase1/app_build_reports/m1_app_022_run_001.md"),
        "TODO-024": ("61_M1-App-023DetailWriterP1实现记录.md", "docs/phase1/app_build_reports/m1_app_023_run_001.md"),
        "TODO-025": ("62_TODO-025旧图石输出钢筋STP样本入库验证记录.md", "docs/phase1/app_build_reports/m1_app_024_run_001.md"),
        "TODO-027": ("63_M2-UI-001旧UI功能入口P1实现记录.md", "docs/phase1/app_build_reports/m2_ui_001_run_001.md"),
        "TODO-028": ("64_M2-Gate-001CSEReadinessGate扩展实现记录.md", "docs/phase1/app_build_reports/m2_gate_001_run_001.md"),
        "TODO-029": ("65_M2-Edit-001钢筋移动P0实现记录.md", "docs/phase1/app_build_reports/m2_edit_001_run_001.md"),
        "TODO-030": ("67_M2-Stats-001钢筋统计下料表P0实现记录.md", "docs/phase1/app_build_reports/m2_stats_001_run_001.md"),
        "TODO-031": ("68_M2-Drawing-001DetailWriter多图纸P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_001_run_001.md"),
        "TODO-032": ("66_M2-Edit-002钢筋拷贝P0实现记录.md", "docs/phase1/app_build_reports/m2_edit_002_run_001.md"),
        "TODO-033": ("69_M2-Drawing-002AutoCADL2导入验证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_002_run_001.md"),
        "TODO-034": ("70_M2-Drawing-003Detail复杂字段静态证据P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_003_run_001.md"),
        "TODO-035": ("71_M2-Drawing-004DetailWriter复杂字段骨架P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_004_run_001.md"),
        "TODO-036": ("72_M2-Drawing-005AutoCADL2复杂字段骨架导入验证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_005_run_001.md"),
        "TODO-037": ("73_M2-Drawing-006DetailWriter点筋FaceEdge字段骨架P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_006_run_001.md"),
        "TODO-038": ("74_M2-Drawing-007DetailWriter剖切线字段骨架P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_007_run_001.md"),
        "TODO-039": ("75_M2-Drawing-008DetailWriterSectionLineAutoCADL2运行确认准备P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_008_run_001.md"),
        "TODO-040": ("76_M2-Drawing-009DetailWriter线容器字段骨架P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_009_run_001.md"),
        "TODO-041": ("77_M2-Drawing-010DetailWriter线容器AutoCADL2运行确认准备P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_010_run_001.md"),
        "TODO-042": ("78_M2-Drawing-011DetailWriter点筋FaceEdgeAutoCADL2运行确认准备P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_011_run_001.md"),
        "TODO-043": ("79_M2-Drawing-012DetailWriterOthersSteeljoint字段骨架P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_012_run_001.md"),
        "TODO-044": ("80_M2-Drawing-013DetailWriterOthersSteeljointAutoCADL2运行确认准备P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_013_run_001.md"),
        "TODO-045": ("81_M2-Drawing-014真实接头线Others几何算法证据补齐P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_014_run_001.md"),
        "TODO-046": ("82_M2-Drawing-015真实接头线Others旧图石运行确认与参数绑定追踪P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_015_run_001.md"),
        "TODO-047": ("83_M2-Drawing-016真实接头线Others旧图石运行样例采集P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_016_run_001.md"),
        "TODO-048": ("84_M2-Drawing-017旧图石启动阻塞提示链补证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_017_run_001.md"),
        "TODO-049": ("85_M2-Drawing-018旧图石启动前置条件用户手工确认P0准备记录.md", "docs/phase1/app_build_reports/m2_drawing_018_run_001.md"),
        "TODO-051": ("87_M2-Drawing-020OthersSymbolcutIOSProducer与接头UI触发静态补证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_020_run_001.md"),
        "TODO-052": ("88_M2-Drawing-021接头UIRibbon绑定静态资源补证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_021_run_001.md"),
        "TODO-053": ("89_M2-Drawing-022接头ContextMenuCommandDispatch静态深追P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_022_run_001.md"),
        "TODO-054": ("90_M2-Drawing-023接头Handler业务对象筛选链静态分类P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_023_run_001.md"),
        "TODO-055": ("91_M2-Drawing-024接头Handler动作函数字段语义深追P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_024_run_001.md"),
        "TODO-056": ("92_M2-Drawing-025接头重建几何核心DB6C0静态深追P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_025_run_001.md"),
        "TODO-057": ("93_M2-Drawing-026接头DB6C0owning结构与Dialog428确定链补证P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_026_run_001.md"),
        "TODO-058": ("94_M2-Drawing-027JoingSegDlgMessageMap与ChildNode112字段收口P1实现记录.md", "docs/phase1/app_build_reports/m2_drawing_027_run_001.md"),
        "TODO-066": ("104_M2-Drawing-035生成工程图与下料表旧图石真实运行工件回填P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_035_run_001.md"),
        "TODO-067": ("105_M2-Drawing-036旧图石真实Detail与下料表字段对照P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_036_run_001.md"),
        "TODO-068": ("106_M2-Drawing-037DetailWriter真实字段差异P0实现记录.md", "docs/phase1/app_build_reports/m2_drawing_037_run_001.md"),
        "TODO-069": ("107_M2-Drawing-038DetailWriterStbRow扩展属性骨架实现记录.md", "docs/phase1/app_build_reports/m2_drawing_038_run_001.md"),
        "TODO-070": ("108_M2-Drawing-039DetailWriterLineStbStbGeo字段条件化骨架实现记录.md", "docs/phase1/app_build_reports/m2_drawing_039_run_001.md"),
        "TODO-071": ("109_M2-RebarCreate-001线配筋生成旧逻辑证据与P0切片准备实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_001_run_001.md"),
        "TODO-072": ("110_M2-RebarCreate-002线配筋命令HandlerP0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_002_run_001.md"),
        "TODO-073": ("111_M2-RebarCreate-003线配筋UI到AIS显示P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_003_run_001.md"),
        "TODO-074": ("112_M2-RebarCreate-004线配筋参数窗口P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_004_run_001.md"),
        "TODO-075": ("113_M2-RebarCreate-005线配筋选择预检与参数窗口顺序对齐P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_005_run_001.md"),
        "TODO-076": ("114_M2-RebarCreate-006线配筋旧UI失败提示与状态栏口径证据P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_006_run_001.md"),
        "TODO-077": ("115_M2-RebarCreate-007线配筋旧图石运行确认清单与工件门禁P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_007_run_001.md"),
        "TODO-079": ("117_M2-RebarCreate-009线配筋旧UIDialog静态资源补证P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_009_run_001.md"),
        "TODO-080": ("118_M2-RebarCreate-010线配筋公共创建Core参数Gate与Diagnostic对齐P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_010_run_001.md"),
        "TODO-081": ("119_M2-RebarCreate-011线配筋公共创建CreatedPayload与ObjAB字段语义补证P0实现记录.md", "docs/phase1/app_build_reports/m2_rebar_create_011_run_001.md"),
    }
    missing: list[str] = []
    checked: list[str] = []
    issues: list[str] = []
    for row in rows:
        if row.get("status") != "done":
            continue
        todo_id = row.get("id", "")
        if todo_id not in known_reports:
            continue
        checked.append(todo_id)
        implementation_record, report_md = known_reports[todo_id]
        required_paths = [implementation_record, report_md]
        if (
            report_md.startswith("docs/phase1/app_build_reports/")
            and todo_id not in JSONLESS_DONE_NODE_REPORT_TODOS
        ):
            required_paths.append(str(Path(report_md).with_suffix(".json")).replace("\\", "/"))
        for required_path in required_paths:
            path = root / required_path
            if not path.exists():
                missing.append(f"{todo_id}:{required_path}")
                continue
            if required_path.endswith(".json"):
                report_json = read_json(path)
                if report_json is None:
                    issues.append(f"{todo_id}:{required_path}:invalid_json")
                    continue
                issues.extend(done_node_json_contract_issues(todo_id, report_json, required_path))
            if required_path.startswith("docs/phase1/app_build_reports/"):
                text = read_text_lossy(path)
                marker = next((candidate for candidate in PENDING_REPORT_MARKERS if candidate in text), "")
                if marker:
                    issues.append(f"{todo_id}:{required_path}:{marker}")
    return missing, checked, issues


def collect_route_guardrail_checks(root: Path) -> list[GateCheck]:
    checks: list[GateCheck] = []
    occt_pattern = re.compile(r"\b(TopoDS_|AIS_|BRep|TopAbs_)")
    forbidden_parent_rebar_pattern = re.compile(
        r"\b(RebarCreationCommandService|EdgeToRebarFactory|FaceRebarGenerator|PolylineRebarGenerator)\b"
    )

    domain_hits = find_pattern_hits(root, [root / "app" / "src" / "domain" / "rebar"], occt_pattern)
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "domain_rebar_occt_boundary",
            "error",
            not domain_hits,
            "app/src/domain/rebar",
            ["E-DEV-050"],
            ["GAP-ROUTE-001"],
            "no OCCT/AIS symbols in domain/rebar"
            if not domain_hits
            else "OCCT/AIS symbol leak: " + ", ".join(domain_hits[:8]),
        )
    )

    boundary_hits = find_pattern_hits(
        root,
        [root / "app" / "src" / "domain" / "rebar", root / "app" / "src" / "drawing", root / "app" / "src" / "project"],
        occt_pattern,
    )
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "domain_drawing_project_occt_boundary",
            "error",
            not boundary_hits,
            "app/src/domain/rebar;app/src/drawing;app/src/project",
            ["E-DEV-050"],
            ["GAP-ROUTE-001"],
            "no OCCT/AIS symbols in protected layers"
            if not boundary_hits
            else "OCCT/AIS symbol leak: " + ", ".join(boundary_hits[:8]),
        )
    )

    parent_rebar_hits = find_pattern_hits(root, [root / "app" / "src"], forbidden_parent_rebar_pattern)
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "parent_rebar_business_reference",
            "error",
            not parent_rebar_hits,
            "app/src",
            ["E-DEV-050", "23_父目录源码参考边界与路线纠偏.md"],
            ["GAP-ROUTE-002"],
            "no parent rebar business factories referenced"
            if not parent_rebar_hits
            else "parent rebar business reference: " + ", ".join(parent_rebar_hits[:8]),
        )
    )

    rows = read_todo_rows(root)
    if rows is None:
        checks.append(
            GateCheck(
                "RouteGuardrail",
                "todo_status_single_next",
                "error",
                False,
                "todo.csv",
                ["E-DEV-050"],
                ["GAP-ROUTE-003"],
                "todo.csv missing or unreadable",
            )
        )
        checks.append(
            GateCheck(
                "RouteGuardrail",
                "done_node_reports",
                "warning",
                False,
                "todo.csv",
                ["E-DEV-050"],
                ["GAP-ROUTE-004"],
                "todo.csv missing or unreadable",
            )
        )
        return checks

    next_ids = [row.get("id", "") for row in rows if row.get("status") == "next"]
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "todo_status_single_next",
            "error",
            len(next_ids) == 1,
            "todo.csv",
            ["E-DEV-050"],
            ["GAP-ROUTE-003"],
            f"next_count={len(next_ids)} next_ids={','.join(next_ids) if next_ids else 'none'}",
        )
    )

    allowed_statuses = {"done", "next", "pending", "blocked"}
    invalid_statuses = [
        f"{row.get('id', '<missing-id>')}:{row.get('status', '<missing-status>')}"
        for row in rows
        if row.get("status") not in allowed_statuses
    ]
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "todo_status_values",
            "error",
            not invalid_statuses,
            "todo.csv",
            ["E-DEV-050"],
            ["GAP-ROUTE-003"],
            "todo status values valid"
            if not invalid_statuses
            else "invalid todo status: " + ", ".join(invalid_statuses[:8]),
        )
    )

    missing_reports, checked_done_ids, report_issues = done_node_report_requirements(root, rows)
    checks.append(
        GateCheck(
            "RouteGuardrail",
            "done_node_reports",
            "error",
            not missing_reports and not report_issues,
            "todo.csv;docs/phase1/app_build_reports",
            ["E-DEV-050"],
            ["GAP-ROUTE-004"],
            f"checked_done_nodes={len(checked_done_ids)}"
            if not missing_reports and not report_issues
            else "done node report issue: " + ", ".join((missing_reports + report_issues)[:8]),
        )
    )

    return checks


def decision_check(
    root: Path,
    gate: str,
    item: str,
    path: Path,
    expected_decisions: set[str],
    evidence: list[str],
    gap: list[str],
    severity: str = "error",
) -> GateCheck:
    payload = read_json(path)
    if payload is None:
        return GateCheck(
            gate,
            item,
            severity,
            False,
            rel(root, path),
            evidence,
            gap,
            "JSON missing or unreadable",
        )
    decision = str(payload.get("decision", ""))
    ok = decision in expected_decisions
    return GateCheck(
        gate,
        item,
        severity,
        ok,
        rel(root, path),
        evidence,
        gap,
        f"decision={decision}" if decision else "decision missing",
    )


def markdown_result_check(
    root: Path,
    gate: str,
    item: str,
    path: Path,
    expected_results: set[str],
    evidence: list[str],
    gap: list[str],
    severity: str = "error",
) -> GateCheck:
    if not path.exists():
        return GateCheck(
            gate,
            item,
            severity,
            False,
            rel(root, path),
            evidence,
            gap,
            "markdown report missing",
        )
    try:
        text = path.read_text(encoding="utf-8-sig")
    except Exception:
        return GateCheck(
            gate,
            item,
            severity,
            False,
            rel(root, path),
            evidence,
            gap,
            "markdown report unreadable",
        )
    match = re.search(r"^- Result:\s*([A-Za-z0-9_-]+)\s*$", text, flags=re.MULTILINE)
    result = match.group(1) if match else ""
    ok = result in expected_results
    return GateCheck(
        gate,
        item,
        severity,
        ok,
        rel(root, path),
        evidence,
        gap,
        f"result={result}" if result else "result missing",
    )


def json_field_decision_check(
    root: Path,
    gate: str,
    item: str,
    path: Path,
    field_path: list[str],
    expected_decisions: set[str],
    evidence: list[str],
    gap: list[str],
    severity: str = "error",
) -> GateCheck:
    payload = read_json(path)
    if payload is None:
        return GateCheck(
            gate,
            item,
            severity,
            False,
            rel(root, path),
            evidence,
            gap,
            "JSON missing or unreadable",
        )
    value: Any = payload
    for part in field_path:
        if not isinstance(value, dict) or part not in value:
            return GateCheck(
                gate,
                item,
                severity,
                False,
                rel(root, path),
                evidence,
                gap,
                ".".join(field_path) + " missing",
            )
        value = value[part]
    decision = str(value)
    ok = decision in expected_decisions
    return GateCheck(
        gate,
        item,
        severity,
        ok,
        rel(root, path),
        evidence,
        gap,
        f"decision={decision}",
    )


def collect_checks(root: Path) -> list[GateCheck]:
    docs = root / "docs" / "phase1"
    fixtures = root / "fixtures" / "tsrebar"
    checks: list[GateCheck] = []

    required_docs = [
        "00_总览.md",
        "08_开发命令契约.md",
        "11_需求证据追溯矩阵.md",
        "24_新设计文件格式Schema与Fixture草案.md",
        "30_Phase1工程初始化Runbook与Readiness记录模板.md",
        "31_STEP选择ID实际运行记录模板与样本清单.md",
        "32_Validator实现契约与错误码总表.md",
        "33_Qt6应用SaveOpen与Binding修复契约.md",
        "34_Phase1ReadinessGate实际运行记录.md",
        "99_缺口和待确认项.md",
    ]
    for doc in required_docs:
        checks.append(
            exists_check(
                root,
                "Documentation",
                doc,
                root / doc,
                ["E-DEV-018"] if doc.startswith("33_") else [],
                ["GAP-DEV-001", "GAP-DEV-007"],
                message_missing="required contract document missing",
            )
        )

    required_fixture_files = [
        "manifest.json",
        "project.json",
        "geometry/source_step.json",
        "geometry/topology_refs.json",
        "rebar/steel_data.json",
        "rebar/groups.json",
        "rebar/bars.json",
        "rebar/segments.json",
        "drawing/detail_mapping.json",
        "evidence/evidence_index.json",
        "evidence/unresolved_fields.json",
    ]
    for fixture in [
        "fixture_a_empty_step",
        "fixture_b_single_group",
        "fixture_c_pending_legacy",
    ]:
        base = fixtures / fixture / "project.tsrebar"
        checks.append(
            exists_check(
                root,
                "Fixture",
                fixture,
                base,
                ["E-DEV-011", "E-DEV-012", "E-DEV-013"],
                ["GAP-DEV-001"],
                message_missing="fixture package directory missing",
            )
        )
        for req_file in required_fixture_files:
            checks.append(
                exists_check(
                    root,
                    "Fixture",
                    f"{fixture}/{req_file}",
                    base / req_file,
                    ["E-DEV-011", "E-DEV-012", "E-DEV-013"],
                    ["GAP-DEV-001"],
                    message_missing="fixture required file missing",
                )
            )

    negative_cases = [
        "missing_required_file",
        "broken_binding",
        "acis_pointer_as_geometry_id",
        "transient_index_only",
        "forged_evidence",
        "pending_field_marked_confirmed",
        "detail_rsd_mismatch",
        "half_written_package",
        "missing_step_diff",
    ]
    for case in negative_cases:
        checks.append(
            exists_check(
                root,
                "NegativeFixture",
                case,
                fixtures / "negative" / case / "project.tsrebar",
                ["E-DEV-014"],
                ["GAP-DEV-001"],
                message_missing="negative fixture missing",
            )
        )

    validator_reports = docs / "validator_reports"
    checks.append(
        exists_check(
            root,
            "Validator",
            "validation_summary_001.md",
            validator_reports / "validation_summary_001.md",
            ["E-DEV-015"],
            ["GAP-DEV-001"],
            message_missing="validator summary missing",
        )
    )
    for report in [
        "fixture_a_empty_step_validation_001.json",
        "fixture_b_single_group_validation_001.json",
        "fixture_c_pending_legacy_validation_001.json",
        "negative_missing_required_file_validation_001.json",
        "negative_broken_binding_validation_001.json",
    ]:
        checks.append(
            exists_check(
                root,
                "Validator",
                report,
                validator_reports / report,
                ["E-DEV-015"],
                ["GAP-DEV-001"],
                message_missing="validator report missing",
            )
        )

    checks.append(
        decision_check(
            root,
            "StandaloneSaveOpen",
            "save_open_run_001",
            docs / "save_open_reports" / "save_open_run_001.json",
            {"standalone-pass"},
            ["E-DEV-016"],
            ["GAP-DEV-001"],
            severity="warning",
        )
    )

    step_samples = [
        "test_stp",
        "cc001_bspline_edge",
        "cc002_bezier_edge",
        "cc003_bspline_length",
        "cc004_hostref_source_curve",
        "cc005_sampling_policy",
    ]
    for sample in step_samples:
        checks.append(
            decision_check(
                root,
                "StepSelection",
                sample,
                docs / "step_selection" / sample / "step_selection_diff.json",
                {"pass"},
                ["E-DEV-017"],
                ["GAP-DEV-002"],
                severity="warning",
            )
        )

    checks.append(
        markdown_result_check(
            root,
            "Dependency",
            "dependency_gate_run_001.md",
            docs / "dependency_gate_run_001.md",
            {"pass"},
            ["E-TECH-002"],
            ["GAP-TECH-007", "GAP-DEV-007"],
        )
    )
    for path, item in [
        (root / "DEPENDENCIES.md", "DEPENDENCIES.md"),
        (root / "THIRD_PARTY_NOTICES.md", "THIRD_PARTY_NOTICES.md"),
        (root / "licenses", "licenses/"),
        (root / "sbom" / "project-sbom.spdx.json", "sbom/project-sbom.spdx.json"),
    ]:
        checks.append(
            exists_check(
                root,
                "Dependency",
                item,
                path,
                ["E-TECH-002"],
                ["GAP-TECH-007", "GAP-DEV-007"],
                message_missing="dependency artifact missing",
            )
        )

    app_reports = docs / "app_save_open_reports"
    checks.append(
        json_field_decision_check(
            root,
            "Qt6Application",
            "app_save_open_run_001.json",
            app_reports / "app_save_open_run_001.json",
            ["summary", "decision"],
            {"qt6-app-pass"},
            ["E-DEV-022"],
            ["GAP-DEV-001", "GAP-DEV-007"],
        )
    )
    checks.append(
        json_field_decision_check(
            root,
            "Qt6Application",
            "binding_repair_run_001.json",
            app_reports / "binding_repair_run_001.json",
            ["decision"],
            {"qt6-binding-repair-pass"},
            ["E-DEV-022"],
            ["GAP-DEV-001", "GAP-DEV-007"],
        )
    )
    checks.append(
        exists_check(
            root,
            "Qt6Application",
            "binding_repair_preview_001.md",
            app_reports / "binding_repair_preview_001.md",
            ["E-DEV-022"],
            ["GAP-DEV-001", "GAP-DEV-007"],
            message_missing="Qt6 application gate report missing",
        )
    )

    detail_reports = docs / "detail_writer_reports"
    checks.append(
        json_field_decision_check(
            root,
            "DetailWriter",
            "detail_writer_l0_l1_run_001.json",
            detail_reports / "detail_writer_l0_l1_run_001.json",
            ["decision"],
            {"l0-l1-pass"},
            ["E-DEV-001"],
            ["GAP-DRAW-005", "GAP-DEV-007"],
        )
    )
    checks.append(
        exists_check(
            root,
            "DetailWriter",
            "detail_writer_summary_001.md",
            detail_reports / "detail_writer_summary_001.md",
            ["E-DEV-001"],
            ["GAP-DRAW-005", "GAP-DEV-007"],
            message_missing="Detail writer L0/L1 report missing",
        )
    )

    checks.extend(collect_route_guardrail_checks(root))

    return checks


def summarize(checks: list[GateCheck]) -> dict[str, Any]:
    errors = [c for c in checks if not c.ok and c.severity == "error"]
    warnings = [c for c in checks if not c.ok and c.severity == "warning"]
    blocker_gaps = sorted({gap for c in errors for gap in c.gap})
    m1_formal = not errors
    return {
        "decision": "M1-Formal-Ready" if m1_formal else "M1-Prep-Blocked",
        "m1FormalAllowed": m1_formal,
        "errorCount": len(errors),
        "warningCount": len(warnings),
        "passCount": sum(1 for c in checks if c.ok),
        "checkCount": len(checks),
        "blockerGaps": blocker_gaps,
    }


def build_report(root: Path, checks: list[GateCheck], report_id: str = "readiness_gate_run_001") -> dict[str, Any]:
    summary = summarize(checks)
    return {
        "schemaVersion": "phase1-readiness-gate/v1",
        "reportId": report_id,
        "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
        "scope": "M1-Formal readiness artifact gate",
        "summary": summary,
        "checks": [c.to_json() for c in checks],
        "notes": [
            "This gate checks evidence artifacts. It does not execute Qt6 application code.",
            "Missing Qt6 application reports keep GAP-DEV-001 and GAP-DEV-007 open.",
            "Parent directory source remains reference-only and cannot close business gaps.",
        ],
    }


def write_markdown(report: dict[str, Any], out_path: Path) -> None:
    summary = report["summary"]
    lines: list[str] = [
        f"# Phase 1 Readiness Gate {report['reportId']}",
        "",
        "## Summary",
        "",
        f"- Created at: {report['createdAt']}",
        f"- Scope: {report['scope']}",
        f"- Decision: {summary['decision']}",
        f"- M1-Formal allowed: {'yes' if summary['m1FormalAllowed'] else 'no'}",
        f"- Checks: {summary['passCount']}/{summary['checkCount']} pass",
        f"- Errors: {summary['errorCount']}",
        f"- Warnings: {summary['warningCount']}",
        f"- Blocker GAPs: {', '.join(summary['blockerGaps']) if summary['blockerGaps'] else 'none'}",
        "",
        "## Failed Checks",
        "",
        "| Gate | Item | Severity | GAP | Message | Path |",
        "|---|---|---|---|---|---|",
    ]
    for check in report["checks"]:
        if check["ok"]:
            continue
        lines.append(
            "| {gate} | {item} | {severity} | {gap} | {message} | `{path}` |".format(
                gate=check["gate"],
                item=check["item"],
                severity=check["severity"],
                gap=", ".join(check["gap"]),
                message=check["message"],
                path=check["path"],
            )
        )
    lines.extend(
        [
            "",
            "## Gate Boundary",
            "",
            "- This is an artifact gate, not a Qt6 runtime test.",
            "- Passing standalone Save/Open does not close Qt6 application Save/Open.",
            "- Current seed STEP samples pass, but new engineering STEP samples still need the same gate.",
            "- M1-Formal cannot be allowed while any `error` check is missing.",
            "",
        ]
    )
    out_path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json-out", type=Path)
    parser.add_argument("--md-out", type=Path)
    parser.add_argument("--report-id")
    parser.add_argument("--strict", action="store_true", help="exit 1 when M1-Formal is blocked")
    args = parser.parse_args()

    root = root_dir()
    checks = collect_checks(root)
    report_id = args.report_id
    if not report_id and args.json_out:
        report_id = args.json_out.stem
    if not report_id and args.md_out:
        report_id = args.md_out.stem
    report = build_report(root, checks, report_id or "readiness_gate_run_001")

    if args.json_out:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    if args.md_out:
        args.md_out.parent.mkdir(parents=True, exist_ok=True)
        write_markdown(report, args.md_out)

    print(json.dumps(report["summary"], ensure_ascii=False, indent=2))
    if args.strict and not report["summary"]["m1FormalAllowed"]:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
