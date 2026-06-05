#!/usr/bin/env python3
"""Dependency compliance gate for the Qt6 + OCCT preparation package."""

from __future__ import annotations

import argparse
import json
import re
from dataclasses import dataclass
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any


CN_TZ = timezone(timedelta(hours=8))

FORBIDDEN_CMAKE_KEYWORDS = [
    "QtWebEngine",
    "WebEngineWidgets",
    "QtQuick3D",
    "Quick3D",
    "QmlCompiler",
    "QtHttpServer",
    "HttpServer",
    "QtGraphs",
    "QtGrpc",
    "QtMqtt",
    "QtVirtualKeyboard",
    "ACIS",
    "Parasolid",
    "DXF",
    "JT",
    "IFC",
    "ExpressMesh",
]

PREP_MARKERS = [
    r"\bTBD\b",
    r"\bnot frozen\b",
    r"\byes/no\b",
]


@dataclass(frozen=True)
class Finding:
    check_id: str
    severity: str
    ok: bool
    file: str
    evidence: str
    decision: str

    def to_json(self) -> dict[str, Any]:
        return {
            "checkId": self.check_id,
            "severity": self.severity,
            "ok": self.ok,
            "file": self.file,
            "evidence": self.evidence,
            "decision": self.decision,
        }


def rel(root: Path, path: Path) -> str:
    try:
        return path.resolve().relative_to(root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8-sig")


def exists_finding(root: Path, check_id: str, path: Path, evidence: str) -> Finding:
    ok = path.exists()
    return Finding(
        check_id=check_id,
        severity="error",
        ok=ok,
        file=rel(root, path),
        evidence=evidence if ok else "missing",
        decision="present" if ok else "required artifact missing",
    )


def marker_hits(text: str) -> list[str]:
    hits: list[str] = []
    for marker in PREP_MARKERS:
        if re.search(marker, text, flags=re.IGNORECASE):
            hits.append(marker.strip(r"\b"))
    return hits


def check_no_prep_markers(root: Path, path: Path) -> Finding:
    if not path.exists():
        return exists_finding(root, "DEP_VERSION_FREEZE", path, "")
    hits = marker_hits(read_text(path))
    ok = not hits
    return Finding(
        check_id="DEP_VERSION_FREEZE",
        severity="error",
        ok=ok,
        file=rel(root, path),
        evidence=", ".join(sorted(set(hits))) if hits else "no prep markers",
        decision="dependency versions frozen" if ok else "M1-Prep markers remain; M1-Formal blocked",
    )


def check_source_modified_no(root: Path, path: Path, component: str) -> Finding:
    if not path.exists():
        return exists_finding(root, f"{component.upper()}_SOURCE_MODIFIED", path, "")
    text = read_text(path).lower()
    ok = "source modified: no" in text
    return Finding(
        check_id=f"{component.upper()}_SOURCE_MODIFIED",
        severity="error",
        ok=ok,
        file=rel(root, path),
        evidence="Source modified: no" if ok else "source modified marker missing",
        decision="source modification state recorded" if ok else "source modification state must be explicit",
    )


def check_sbom(root: Path, path: Path) -> list[Finding]:
    if not path.exists():
        return [exists_finding(root, "SBOM_EXISTS", path, "")]
    try:
        payload = json.loads(read_text(path))
    except Exception as exc:
        return [
            Finding(
                "SBOM_JSON",
                "error",
                False,
                rel(root, path),
                type(exc).__name__,
                "SBOM JSON unreadable",
            )
        ]

    findings: list[Finding] = []
    packages = payload.get("packages", [])
    package_names = {str(package.get("name", "")) for package in packages if isinstance(package, dict)}
    required = {"Qt", "Open CASCADE Technology"}
    missing = sorted(required - package_names)
    findings.append(
        Finding(
            "SBOM_REQUIRED_PACKAGES",
            "error",
            not missing,
            rel(root, path),
            ", ".join(missing) if missing else "Qt and OCCT packages present",
            "required packages present" if not missing else "required packages missing",
        )
    )

    markers = marker_hits(json.dumps(payload, ensure_ascii=False))
    findings.append(
        Finding(
            "SBOM_VERSION_FREEZE",
            "error",
            not markers,
            rel(root, path),
            ", ".join(sorted(set(markers))) if markers else "no prep markers",
            "SBOM versions frozen" if not markers else "SBOM still contains M1-Prep markers",
        )
    )
    return findings


def check_cmake(root: Path, cmake_path: Path) -> list[Finding]:
    if not cmake_path.exists():
        return [
            Finding(
                "CMAKE_SCAN",
                "warning",
                False,
                rel(root, cmake_path),
                "missing",
                "CMake project not created yet; dependency module scan skipped",
            )
        ]

    text = read_text(cmake_path)
    findings: list[Finding] = []
    for keyword in FORBIDDEN_CMAKE_KEYWORDS:
        hit = keyword in text
        findings.append(
            Finding(
                f"CMAKE_FORBIDDEN_{keyword.upper()}",
                "error",
                not hit,
                rel(root, cmake_path),
                keyword if hit else "not found",
                "manual dependency review required" if hit else "forbidden keyword absent",
            )
        )
    return findings


def collect_findings(
    root: Path,
    cmake_path: Path,
    dependencies_path: Path,
    notices_path: Path,
    licenses_dir: Path,
    sbom_path: Path,
) -> list[Finding]:
    findings: list[Finding] = []

    required_files = [
        ("DEPENDENCIES_EXISTS", dependencies_path, "DEPENDENCIES.md"),
        ("NOTICES_EXISTS", notices_path, "THIRD_PARTY_NOTICES.md"),
        ("QT_LICENSE_EXISTS", licenses_dir / "QT_LICENSES.txt", "Qt license record"),
        ("OCCT_LICENSE_EXISTS", licenses_dir / "OCCT_LICENSE.txt", "OCCT license record"),
        ("SOURCE_OFFER_EXISTS", licenses_dir / "SOURCE_OFFER.txt", "source availability record"),
        ("SBOM_EXISTS", sbom_path, "SPDX SBOM"),
    ]
    for check_id, path, evidence in required_files:
        findings.append(exists_finding(root, check_id, path, evidence))

    findings.append(check_no_prep_markers(root, dependencies_path))
    findings.append(check_no_prep_markers(root, notices_path))
    findings.append(check_no_prep_markers(root, licenses_dir / "QT_LICENSES.txt"))
    findings.append(check_no_prep_markers(root, licenses_dir / "OCCT_LICENSE.txt"))
    findings.append(check_no_prep_markers(root, licenses_dir / "SOURCE_OFFER.txt"))
    findings.append(check_source_modified_no(root, notices_path, "notice"))
    findings.append(check_source_modified_no(root, licenses_dir / "QT_LICENSES.txt", "qt"))
    findings.append(check_source_modified_no(root, licenses_dir / "OCCT_LICENSE.txt", "occt"))
    findings.extend(check_sbom(root, sbom_path))
    findings.extend(check_cmake(root, cmake_path))

    return findings


def summarize(findings: list[Finding]) -> dict[str, Any]:
    errors = [finding for finding in findings if not finding.ok and finding.severity == "error"]
    warnings = [finding for finding in findings if not finding.ok and finding.severity == "warning"]
    return {
        "decision": "pass" if not errors else "fail",
        "m1FormalAllowed": not errors,
        "errorCount": len(errors),
        "warningCount": len(warnings),
        "passCount": sum(1 for finding in findings if finding.ok),
        "checkCount": len(findings),
        "blockerGaps": [] if not errors else ["GAP-TECH-007", "GAP-DEV-007"],
    }


def build_report(root: Path, findings: list[Finding]) -> dict[str, Any]:
    return {
        "schemaVersion": "dependency-gate/v1",
        "reportId": "dependency_gate_run_001",
        "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
        "scope": "Qt6 + OCCT dependency compliance gate",
        "summary": summarize(findings),
        "findings": [finding.to_json() for finding in findings],
        "linkedRequirements": ["REQ-TECH-001", "REQ-TECH-002"],
        "linkedEvidence": ["E-TECH-001", "E-TECH-002", "E-TECH-003", "E-TECH-004", "E-TECH-005"],
        "linkedGaps": ["GAP-TECH-001", "GAP-TECH-002", "GAP-TECH-007", "GAP-DEV-007"],
        "notes": [
            "TBD/not frozen markers are allowed only in M1-Prep.",
            "Qt and OCCT versions must be frozen before M1-Formal.",
            "This gate scans CMake only after the Qt6 + OCCT project exists.",
        ],
    }


def write_markdown(report: dict[str, Any], out_path: Path) -> None:
    summary = report["summary"]
    lines = [
        "# Dependency Gate Run 001",
        "",
        "## Summary",
        "",
        f"- Created at: {report['createdAt']}",
        f"- Scope: {report['scope']}",
        f"- Result: {summary['decision']}",
        f"- M1-Formal allowed: {'yes' if summary['m1FormalAllowed'] else 'no'}",
        f"- Checks: {summary['passCount']}/{summary['checkCount']} pass",
        f"- Errors: {summary['errorCount']}",
        f"- Warnings: {summary['warningCount']}",
        f"- Blocker GAPs: {', '.join(summary['blockerGaps']) if summary['blockerGaps'] else 'none'}",
        "",
        "## Findings",
        "",
        "| Check | Severity | OK | File | Evidence | Decision |",
        "|---|---|---|---|---|---|",
    ]
    for finding in report["findings"]:
        lines.append(
            "| {check} | {severity} | {ok} | `{file}` | {evidence} | {decision} |".format(
                check=finding["checkId"],
                severity=finding["severity"],
                ok="yes" if finding["ok"] else "no",
                file=finding["file"],
                evidence=finding["evidence"].replace("|", "/"),
                decision=finding["decision"].replace("|", "/"),
            )
        )
    lines.extend(
        [
            "",
            "## Linked Requirements",
            "",
            "- REQ-TECH-001",
            "- REQ-TECH-002",
            "",
            "## Linked Evidence",
            "",
            "- E-TECH-001",
            "- E-TECH-002",
            "- E-TECH-003",
            "- E-TECH-004",
            "- E-TECH-005",
            "",
            "## Linked GAP",
            "",
            "- GAP-TECH-001",
            "- GAP-TECH-002",
            "- GAP-TECH-007",
            "- GAP-DEV-007",
            "",
            "## Gate Boundary",
            "",
            "- This run checks the frozen local vcpkg dependency candidate.",
            "- A pass means dependency files, notices, source availability records and SBOM no longer contain preparation placeholders.",
            "- CMake module scanning is still a warning until the Qt6 + OCCT project is created.",
            "- Installed modules are not automatically approved link modules; the CMake allowlist remains authoritative.",
            "",
        ]
    )
    out_path.write_text("\n".join(lines), encoding="utf-8")


def root_dir() -> Path:
    return Path(__file__).resolve().parents[2]


def main() -> int:
    root = root_dir()
    parser = argparse.ArgumentParser()
    parser.add_argument("--cmake", type=Path, default=root / "CMakeLists.txt")
    parser.add_argument("--dependencies", type=Path, default=root / "DEPENDENCIES.md")
    parser.add_argument("--notices", type=Path, default=root / "THIRD_PARTY_NOTICES.md")
    parser.add_argument("--licenses", type=Path, default=root / "licenses")
    parser.add_argument("--sbom", type=Path, default=root / "sbom" / "project-sbom.spdx.json")
    parser.add_argument("--report", type=Path, default=root / "docs" / "phase1" / "dependency_gate_run_001.md")
    parser.add_argument("--json-report", type=Path)
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args()

    findings = collect_findings(
        root=root,
        cmake_path=args.cmake,
        dependencies_path=args.dependencies,
        notices_path=args.notices,
        licenses_dir=args.licenses,
        sbom_path=args.sbom,
    )
    report = build_report(root, findings)

    args.report.parent.mkdir(parents=True, exist_ok=True)
    write_markdown(report, args.report)
    if args.json_report:
        args.json_report.parent.mkdir(parents=True, exist_ok=True)
        args.json_report.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    print(json.dumps(report["summary"], ensure_ascii=False, indent=2))
    if args.strict and not report["summary"]["m1FormalAllowed"]:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
