#!/usr/bin/env python3
"""Offline Detail writer L0/L1 gate for tsrebar fixtures."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import shutil
import xml.etree.ElementTree as ET
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any


CN_TZ = timezone(timedelta(hours=8))
ROOT = Path(__file__).resolve().parents[2]


def load_json(path: Path) -> Any:
    return json.loads(path.read_text(encoding="utf-8-sig"))


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def directory_hash(path: Path) -> str:
    digest = hashlib.sha256()
    if not path.exists():
        return "sha256:missing"
    for item in sorted(path.rglob("*")):
        if not item.is_file():
            continue
        rel = item.relative_to(path).as_posix()
        digest.update(rel.encode("utf-8"))
        digest.update(b"\0")
        digest.update(item.read_bytes())
        digest.update(b"\0")
    return f"sha256:{digest.hexdigest()}"


def package_documents(package_path: Path) -> dict[str, Any]:
    rels = [
        "drawing/detail_mapping.json",
        "rebar/groups.json",
        "rebar/bars.json",
        "rebar/segments.json",
        "rebar/steel_data.json",
    ]
    return {rel: load_json(package_path / rel) for rel in rels}


def items_by_id(doc: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {item["id"]: item for item in doc.get("items", [])}


def group_points(group: dict[str, Any]) -> tuple[list[float], list[float]]:
    value = (
        group.get("legacyObject", {})
        .get("raw", {})
        .get("fields", {})
        .get("positions168_192", {})
        .get("value", [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0]])
    )
    if len(value) < 2:
        return [0.0, 0.0, 0.0], [1.0, 0.0, 0.0]
    return [float(v) for v in value[0]], [float(v) for v in value[1]]


def distance(a: list[float], b: list[float]) -> float:
    return math.sqrt(sum((a[index] - b[index]) ** 2 for index in range(3)))


def build_style_xml() -> ET.ElementTree:
    root = ET.Element("StyleRoot")
    styles = ET.SubElement(root, "Styles")
    ET.SubElement(
        styles,
        "Style",
        {
            "Name": "default",
            "dia": "25",
            "type": "HRB",
            "source": "E-DETAIL-001",
        },
    )
    return ET.ElementTree(root)


def build_drawing_xml(package_path: Path) -> tuple[ET.ElementTree, list[str]]:
    docs = package_documents(package_path)
    mapping = docs["drawing/detail_mapping.json"]
    groups = items_by_id(docs["rebar/groups.json"])
    segments = items_by_id(docs["rebar/segments.json"])
    warnings: list[str] = []

    root = ET.Element("DrawingRoot")
    view_ports = ET.SubElement(root, "HViewPorts")
    ET.SubElement(view_ports, "ViewPort", {"id": "view_000001", "DrawingName": "fixture_detail_l0_l1", "DrawingUnit": "m", "DrawingScale": "1"})
    part = ET.SubElement(root, "PartDetailDrawing")
    ET.SubElement(part, "General-Info", {"Model_FileName": package_path.name, "DrawingName": "fixture_detail_l0_l1"})

    stb_detail = ET.SubElement(root, "StbDetailDrawing")
    stb_groups = ET.SubElement(stb_detail, "StbGroups", {"stbGroupCount": str(len(mapping.get("groups", [])))})
    for group_map in mapping.get("groups", []):
        group = groups[group_map["rebarGroupId"]]
        segment_ids = group.get("segmentIds", [])
        bar_count = len(group.get("barIds", []))
        start, end = group_points(group)
        length = distance(start, end)
        attrs = {
            "rsdID": group_map["rsdId"],
            "groupID": group_map["groupId"],
            "diameter": "25",
            "interval": "0",
            "barcount": str(bar_count),
            "segcount": str(len(segment_ids)),
            "stbNum": group.get("id", ""),
            "stbNumAct": group.get("id", ""),
            "ComponentName": "fixture",
            "PJSteelName": group.get("id", ""),
            "SteelWay": "OTHER",
            "stbType": "lineStb",
        }
        stb_group = ET.SubElement(stb_groups, "StbGroup", attrs)
        std = ET.SubElement(stb_group, "Std", {"segCount": str(len(segment_ids))})
        for segment_id in segment_ids:
            segment = segments[segment_id]
            ET.SubElement(
                std,
                "StbGeo",
                {
                    "segID": segment["id"],
                    "shapeType": "L",
                    "start_x": str(start[0]),
                    "start_y": str(start[1]),
                    "start_z": str(start[2]),
                    "end_x": str(end[0]),
                    "end_y": str(end[1]),
                    "end_z": str(end[2]),
                    "length": f"{length:.6f}",
                },
            )
        if "diameter" not in group:
            warnings.append("DW-WARN-DEFAULT_DIAMETER")

    tables = ET.SubElement(root, "StbTables")
    table = ET.SubElement(tables, "StbTable", {"count": str(len(mapping.get("tables", [])))})
    for table_map in mapping.get("tables", []):
        group = groups[table_map["sourceGroupId"]]
        start, end = group_points(group)
        length = distance(start, end)
        ET.SubElement(
            table,
            "StbRow",
            {
                "rsdID": table_map["rsdId"],
                "diameter": "25",
                "length": f"{length:.6f}",
                "segNum": str(len(group.get("segmentIds", []))),
                "stbNumSum": str(len(group.get("barIds", []))),
                "lenSum": f"{length * max(1, len(group.get('barIds', []))):.6f}",
                "stbLevel": "HRB",
            },
        )

    material = ET.SubElement(root, "MaterialTable", {"rowCount": "1", "Mass": "0", "Volume722": "0"})
    ET.SubElement(material, "MatRow", {"diameter": "25", "lenSum": "0", "countSum": "0", "singleMass": "0", "massSum": "0", "stbLevel": "HRB"})

    return ET.ElementTree(root), warnings


def write_xml(tree: ET.ElementTree, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    tree.write(path, encoding="utf-8", xml_declaration=True)


def validate_l0(output_dir: Path) -> list[dict[str, str]]:
    findings: list[dict[str, str]] = []
    expected = {
        "Detail.xml": "StyleRoot",
        "Detail01.stl": "DrawingRoot",
    }
    for filename, root_name in expected.items():
        path = output_dir / filename
        if not path.exists():
            findings.append({"code": "DW004_REQUIRED_FIELD_MISSING", "file": filename, "message": "required Detail file missing"})
            continue
        try:
            root = ET.parse(path).getroot()
        except ET.ParseError as exc:
            findings.append({"code": "DW006_XML_PARSE_FAILED", "file": filename, "message": str(exc)})
            continue
        if root.tag != root_name:
            findings.append({"code": "DW006_XML_PARSE_FAILED", "file": filename, "message": f"root is {root.tag}, expected {root_name}"})
    return findings


def validate_l1(output_dir: Path) -> list[dict[str, str]]:
    findings: list[dict[str, str]] = []
    root = ET.parse(output_dir / "Detail01.stl").getroot()
    group_rsd = {item.get("rsdID") for item in root.findall(".//StbGroup")}
    row_rsd = {item.get("rsdID") for item in root.findall(".//StbRow")}
    if group_rsd != row_rsd:
        findings.append(
            {
                "code": "DW003_ID_CROSS_REFERENCE_FAILED",
                "file": "Detail01.stl",
                "message": f"StbGroup rsdID {sorted(group_rsd)} != StbRow rsdID {sorted(row_rsd)}",
            }
        )
    seg_ids = [item.get("segID") for item in root.findall(".//StbGeo")]
    if len(seg_ids) != len(set(seg_ids)):
        findings.append({"code": "DW003_ID_CROSS_REFERENCE_FAILED", "file": "Detail01.stl", "message": "StbGeo.segID is not unique"})
    for std in root.findall(".//Std"):
        expected = int(std.get("segCount", "0"))
        actual = len(std.findall("./StbGeo"))
        if expected != actual:
            findings.append({"code": "DW003_ID_CROSS_REFERENCE_FAILED", "file": "Detail01.stl", "message": f"Std.segCount {expected} != StbGeo count {actual}"})
    return findings


def replace_output(tmp_dir: Path, output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    for old in output_dir.glob("Detail*.stl"):
        old.unlink()
    for filename in ["Detail.xml", "Detail01.stl"]:
        shutil.copy2(tmp_dir / filename, output_dir / filename)


def write_detail_package(package_path: Path, output_dir: Path, run_id: str = "DW-RUN-001") -> dict[str, Any]:
    package_path = Path(package_path)
    output_dir = Path(output_dir)
    before_hash = directory_hash(output_dir)
    tmp_dir = output_dir.parent / f".tmp_{run_id}"
    if tmp_dir.exists():
        shutil.rmtree(tmp_dir)
    tmp_dir.mkdir(parents=True)

    warnings: list[str] = []
    findings: list[dict[str, str]] = []
    try:
        write_xml(build_style_xml(), tmp_dir / "Detail.xml")
        drawing_xml, build_warnings = build_drawing_xml(package_path)
        warnings.extend(build_warnings)
        write_xml(drawing_xml, tmp_dir / "Detail01.stl")
        findings.extend(validate_l0(tmp_dir))
        if not findings:
            findings.extend(validate_l1(tmp_dir))
        if findings:
            after_hash = directory_hash(output_dir)
            return {
                "schemaVersion": "detail-writer-run/v1",
                "runId": run_id,
                "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
                "inputPackage": package_path.as_posix(),
                "outputDir": output_dir.as_posix(),
                "decision": "fail",
                "l0": "failed" if any(item["code"].startswith("DW006") or item["code"] == "DW004_REQUIRED_FIELD_MISSING" for item in findings) else "passed",
                "l1": "failed",
                "l2": "not_run",
                "files": [],
                "findings": findings,
                "warnings": warnings,
                "errorCodes": [item["code"] for item in findings],
                "guardrails": {
                    "oldPackagePreserved": before_hash == after_hash,
                    "drawingDirtyPreserved": True,
                },
                "linkedEvidence": ["E-DETAIL-001", "E-DETAIL-002", "E-IDA-018", "E-IDA-019"],
                "linkedGaps": ["GAP-DRAW-005"],
            }
        replace_output(tmp_dir, output_dir)
        after_hash = directory_hash(output_dir)
        return {
            "schemaVersion": "detail-writer-run/v1",
            "runId": run_id,
            "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
            "inputPackage": package_path.as_posix(),
            "outputDir": output_dir.as_posix(),
            "decision": "l0-l1-pass",
            "l0": "passed",
            "l1": "passed",
            "l2": "not_run",
            "files": ["Detail.xml", "Detail01.stl"],
            "findings": [],
            "warnings": warnings,
            "errorCodes": [],
            "outputHashBefore": before_hash,
            "outputHashAfter": after_hash,
            "guardrails": {
                "replacedOnlyAfterValidation": True,
                "drawingDirtyCanBeClearedAfterL1": True,
                "autoCadImportNotClaimed": True,
            },
            "linkedEvidence": ["E-DETAIL-001", "E-DETAIL-002", "E-IDA-018", "E-IDA-019"],
            "linkedGaps": ["GAP-DRAW-001", "GAP-DRAW-002", "GAP-DRAW-005"],
        }
    finally:
        if tmp_dir.exists():
            shutil.rmtree(tmp_dir)


def write_summary(report: dict[str, Any], path: Path) -> None:
    summary = report["summary"]
    lines = [
        "# Detail Writer L0/L1 Summary 001",
        "",
        "## Summary",
        "",
        f"- Created at: {report['createdAt']}",
        f"- Decision: {report['decision']}",
        f"- L0: {summary['l0']}",
        f"- L1: {summary['l1']}",
        f"- L2 AutoCAD run: {'yes' if summary['l2AutoCadRun'] else 'no'}",
        f"- Positive package: `{summary['positiveOutputDir']}`",
        f"- Rollback old package preserved: {'yes' if summary['rollbackOldPackagePreserved'] else 'no'}",
        "",
        "## Boundary",
        "",
        "- This proves offline Detail XML L0/L1 only.",
        "- This does not prove AutoCAD 2020 plugin import.",
        "- Detail writer remains a derived output path; it is not the rebar data truth source.",
        "",
    ]
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines), encoding="utf-8")


def run_detail_writer_gate(root: Path, report_dir: Path) -> dict[str, Any]:
    root = Path(root)
    report_dir = Path(report_dir)
    report_dir.mkdir(parents=True, exist_ok=True)
    positive_out = report_dir / "generated_detail_package"
    rollback_out = report_dir / "rollback_probe_package"
    rollback_out.mkdir(parents=True, exist_ok=True)
    (rollback_out / "Detail.xml").write_text("<StyleRoot><Old /></StyleRoot>", encoding="utf-8")
    (rollback_out / "Detail01.stl").write_text("<DrawingRoot><Old /></DrawingRoot>", encoding="utf-8")

    positive = write_detail_package(root / "fixtures" / "tsrebar" / "fixture_b_single_group" / "project.tsrebar", positive_out, "DW-RUN-001-POSITIVE")
    rollback = write_detail_package(root / "fixtures" / "tsrebar" / "negative" / "detail_rsd_mismatch" / "project.tsrebar", rollback_out, "DW-RUN-001-ROLLBACK")
    decision = "l0-l1-pass" if positive["decision"] == "l0-l1-pass" and rollback["guardrails"]["oldPackagePreserved"] else "fail"
    report = {
        "schemaVersion": "detail-writer-l0-l1-run/v1",
        "reportId": "detail_writer_l0_l1_run_001",
        "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
        "decision": decision,
        "summary": {
            "l0": positive["l0"],
            "l1": positive["l1"],
            "l2AutoCadRun": False,
            "positiveOutputDir": positive_out.as_posix(),
            "rollbackOldPackagePreserved": rollback["guardrails"]["oldPackagePreserved"],
            "autoCadImportNotClaimed": True,
        },
        "positiveRun": positive,
        "rollbackRun": rollback,
        "linkedRequirements": ["REQ-DRAW-003", "REQ-DRAW-004"],
        "linkedEvidence": ["E-DEV-001", "E-DETAIL-001", "E-DETAIL-002", "E-IDA-018", "E-IDA-019"],
        "linkedGaps": ["GAP-DRAW-001", "GAP-DRAW-002", "GAP-DRAW-005"],
    }
    write_json(report_dir / "detail_writer_l0_l1_run_001.json", report)
    write_summary(report, report_dir / "detail_writer_summary_001.md")
    return report


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--out-dir", type=Path, default=ROOT / "docs" / "phase1" / "detail_writer_reports")
    args = parser.parse_args()
    report = run_detail_writer_gate(args.root, args.out_dir)
    print(json.dumps({"decision": report["decision"], **report["summary"]}, ensure_ascii=False, indent=2))
    return 0 if report["decision"] == "l0-l1-pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())

