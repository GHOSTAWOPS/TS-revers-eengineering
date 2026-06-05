#!/usr/bin/env python3
"""Phase 1 readiness gate for the Tushi rebar reproduction docs.

This gate is intentionally conservative. It verifies that required evidence
artifacts exist and reports why the current state can or cannot be called
M1-Formal-Ready. It does not replace Qt6 application runtime tests.
"""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from datetime import datetime, timezone, timedelta
from pathlib import Path
from typing import Any


CN_TZ = timezone(timedelta(hours=8))


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


def build_report(root: Path, checks: list[GateCheck]) -> dict[str, Any]:
    summary = summarize(checks)
    return {
        "schemaVersion": "phase1-readiness-gate/v1",
        "reportId": "readiness_gate_run_001",
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
        "# Phase 1 Readiness Gate Run 001",
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
    parser.add_argument("--strict", action="store_true", help="exit 1 when M1-Formal is blocked")
    args = parser.parse_args()

    root = root_dir()
    checks = collect_checks(root)
    report = build_report(root, checks)

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
