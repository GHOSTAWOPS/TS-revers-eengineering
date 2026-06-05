#!/usr/bin/env python3
"""Read-only local Qt/OCCT dependency probe for Phase 1 preparation."""

from __future__ import annotations

import argparse
import json
import re
import subprocess
from dataclasses import dataclass, asdict
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any


CN_TZ = timezone(timedelta(hours=8))

QT_RUNTIME_ALLOWLIST = {
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Widgets.dll",
    "Qt6OpenGL.dll",
    "Qt6OpenGLWidgets.dll",
    "Qt6Test.dll",
}

OCCT_RUNTIME_ALLOW_PREFIXES = (
    "TK",
    "TKernel",
)


@dataclass(frozen=True)
class PackageInfo:
    name: str
    version: str
    port_version: str
    architecture: str
    default_features: list[str]
    status: str


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def split_blocks(status_text: str) -> dict[str, str]:
    blocks: dict[str, str] = {}
    for block in re.split(r"\r?\n\r?\n", status_text):
        match = re.search(r"^Package:\s*(.+)$", block, flags=re.MULTILINE)
        if match:
            package_name = match.group(1).strip()
            if re.search(r"^Feature:\s*", block, flags=re.MULTILINE):
                continue
            blocks.setdefault(package_name, block)
    return blocks


def field(block: str, key: str) -> str:
    match = re.search(rf"^{re.escape(key)}:\s*(.+)$", block, flags=re.MULTILINE)
    return match.group(1).strip() if match else ""


def parse_package(status_text: str, package_name: str) -> PackageInfo | None:
    block = split_blocks(status_text).get(package_name)
    if not block:
        return None
    features = field(block, "Default-Features")
    return PackageInfo(
        name=package_name,
        version=field(block, "Version"),
        port_version=field(block, "Port-Version"),
        architecture=field(block, "Architecture"),
        default_features=[item.strip() for item in features.split(",") if item.strip()],
        status=field(block, "Status"),
    )


def list_dlls(bin_dir: Path, pattern: str = "*.dll") -> list[str]:
    if not bin_dir.exists():
        return []
    return sorted(path.name for path in bin_dir.glob(pattern))


def run_command(args: list[str]) -> str:
    try:
        return subprocess.check_output(args, text=True, stderr=subprocess.STDOUT, encoding="utf-8", errors="replace").strip()
    except Exception as exc:
        return f"{type(exc).__name__}: {exc}"


def build_report(vcpkg_root: Path) -> dict[str, Any]:
    installed = vcpkg_root / "installed" / "x64-windows"
    status_path = vcpkg_root / "installed" / "vcpkg" / "status"
    status_text = status_path.read_text(encoding="utf-8", errors="replace") if status_path.exists() else ""

    qt = parse_package(status_text, "qtbase")
    occt = parse_package(status_text, "opencascade")
    bin_dir = installed / "bin"

    qt_dlls = [name for name in list_dlls(bin_dir, "Qt6*.dll")]
    occt_dlls = [
        name
        for name in list_dlls(bin_dir, "*.dll")
        if name.startswith(OCCT_RUNTIME_ALLOW_PREFIXES)
    ]

    qt_runtime_recommended = [name for name in qt_dlls if name in QT_RUNTIME_ALLOWLIST]
    qt_runtime_review = [name for name in qt_dlls if name not in QT_RUNTIME_ALLOWLIST]

    report = {
        "schemaVersion": "dependency-probe/v1",
        "reportId": "dependency_probe_run_001",
        "createdAt": datetime.now(CN_TZ).isoformat(timespec="seconds"),
        "scope": "Local Qt6 + OCCT vcpkg probe",
        "vcpkg": {
            "root": str(vcpkg_root),
            "version": run_command([str(vcpkg_root / "vcpkg.exe"), "version"]) if (vcpkg_root / "vcpkg.exe").exists() else "vcpkg.exe missing",
            "triplet": "x64-windows",
            "installedRoot": str(installed),
            "statusFile": str(status_path),
        },
        "packages": {
            "qtbase": asdict(qt) if qt else None,
            "opencascade": asdict(occt) if occt else None,
        },
        "cmakeConfig": {
            "qt6Config": str(installed / "share" / "Qt6" / "Qt6Config.cmake"),
            "occtConfig": str(installed / "share" / "opencascade" / "OpenCASCADEConfig.cmake"),
            "qt6ConfigExists": (installed / "share" / "Qt6" / "Qt6Config.cmake").exists(),
            "occtConfigExists": (installed / "share" / "opencascade" / "OpenCASCADEConfig.cmake").exists(),
        },
        "runtimeDlls": {
            "qtAll": qt_dlls,
            "qtRecommendedForPhase1": qt_runtime_recommended,
            "qtInstalledButNeedsReviewBeforeLinking": qt_runtime_review,
            "occtAllTk": occt_dlls,
        },
        "sourceArchives": {
            "qtbase": sorted(path.name for path in (vcpkg_root / "downloads").glob("*qtbase*6.10.3*")) if (vcpkg_root / "downloads").exists() else [],
            "opencascade": sorted(path.name for path in (vcpkg_root / "downloads").glob("*OCCT*7_9_3*")) if (vcpkg_root / "downloads").exists() else [],
        },
        "copyrightFiles": {
            "qtbase": str(installed / "share" / "qtbase" / "copyright"),
            "opencascade": str(installed / "share" / "opencascade" / "copyright"),
        },
        "decision": {
            "dependencyCandidateFound": bool(qt and occt),
            "canFreezeFromLocalVcpkg": bool(qt and occt and (installed / "share" / "Qt6" / "Qt6Config.cmake").exists() and (installed / "share" / "opencascade" / "OpenCASCADEConfig.cmake").exists()),
            "notes": [
                "Installed modules are not automatically allowed project link modules.",
                "Phase 1 CMake must still link only the approved Qt/OCCT subset.",
                "This probe does not replace the formal dependency gate.",
            ],
        },
    }
    return report


def write_markdown(report: dict[str, Any], out_path: Path) -> None:
    packages = report["packages"]
    qt = packages["qtbase"] or {}
    occt = packages["opencascade"] or {}
    runtime = report["runtimeDlls"]
    lines = [
        "# Dependency Probe Run 001",
        "",
        "## Summary",
        "",
        f"- Created at: {report['createdAt']}",
        f"- Scope: {report['scope']}",
        f"- vcpkg root: `{report['vcpkg']['root']}`",
        f"- Triplet: `{report['vcpkg']['triplet']}`",
        f"- Qt package: `qtbase {qt.get('version', 'missing')}#{qt.get('port_version', '')}`",
        f"- OCCT package: `opencascade {occt.get('version', 'missing')}#{occt.get('port_version', '')}`",
        f"- Qt CMake config exists: {'yes' if report['cmakeConfig']['qt6ConfigExists'] else 'no'}",
        f"- OCCT CMake config exists: {'yes' if report['cmakeConfig']['occtConfigExists'] else 'no'}",
        f"- Can freeze from local vcpkg: {'yes' if report['decision']['canFreezeFromLocalVcpkg'] else 'no'}",
        "",
        "## Qt Runtime",
        "",
        "Recommended Phase 1 runtime DLLs:",
        "",
    ]
    lines.extend(f"- `{name}`" for name in runtime["qtRecommendedForPhase1"])
    lines.extend(
        [
            "",
            "Installed Qt DLLs requiring review before project linking:",
            "",
        ]
    )
    lines.extend(f"- `{name}`" for name in runtime["qtInstalledButNeedsReviewBeforeLinking"])
    lines.extend(
        [
            "",
            "## OCCT Runtime",
            "",
            "Installed OCCT TK/TKernel DLLs:",
            "",
        ]
    )
    lines.extend(f"- `{name}`" for name in runtime["occtAllTk"])
    lines.extend(
        [
            "",
            "## Source And License Local Evidence",
            "",
            f"- Qt source archives: {', '.join(report['sourceArchives']['qtbase']) or 'none'}",
            f"- OCCT source archives: {', '.join(report['sourceArchives']['opencascade']) or 'none'}",
            f"- Qt copyright file: `{report['copyrightFiles']['qtbase']}`",
            f"- OCCT copyright file: `{report['copyrightFiles']['opencascade']}`",
            "",
            "## Gate Boundary",
            "",
            "- This probe proves local candidate dependencies exist.",
            "- It does not prove final release packaging is complete.",
            "- It does not allow linking installed-but-unapproved modules.",
            "- Formal readiness still depends on `Build.DependencyGate` and Qt6 application runtime reports.",
            "",
        ]
    )
    out_path.write_text("\n".join(lines), encoding="utf-8")


def main() -> int:
    root = repo_root()
    parser = argparse.ArgumentParser()
    parser.add_argument("--vcpkg-root", type=Path, default=Path("D:/Work/vcpkg"))
    parser.add_argument("--json-out", type=Path, default=root / "docs" / "phase1" / "dependency_probe_run_001.json")
    parser.add_argument("--md-out", type=Path, default=root / "docs" / "phase1" / "dependency_probe_run_001.md")
    args = parser.parse_args()

    report = build_report(args.vcpkg_root)
    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.md_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    write_markdown(report, args.md_out)
    print(json.dumps(report["decision"], ensure_ascii=False, indent=2))
    return 0 if report["decision"]["dependencyCandidateFound"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
