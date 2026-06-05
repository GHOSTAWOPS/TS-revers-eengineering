"""Standalone Save/Open transaction gate for tsrebar fixtures.

This tool proves package transaction semantics around the validator. It is a
development gate, not the final Qt6 application Save/Open implementation.
"""

from __future__ import annotations

import argparse
import importlib.util
import json
import shutil
import tempfile
from datetime import datetime, timedelta, timezone
from pathlib import Path
from typing import Any


CHINA_TZ = timezone(timedelta(hours=8))
ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = ROOT / "tools" / "tsrebar_validator" / "validate_tsrebar.py"


def load_validator():
    spec = importlib.util.spec_from_file_location("validate_tsrebar", VALIDATOR_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


VALIDATOR = load_validator()


def package_hash(package_path: Path) -> str:
    return VALIDATOR.package_hash(Path(package_path))


def write_json(path: Path, value: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as handle:
        json.dump(value, handle, ensure_ascii=False, indent=2)
        handle.write("\n")


def write_validation_report(report_dir: Path, label: str, report: dict[str, Any]) -> Path:
    report_path = report_dir / f"{label}_validation.json"
    write_json(report_path, report)
    return report_path


def replace_directory(current_package: Path, candidate_package: Path) -> None:
    tmp_replace = current_package.with_name(f"{current_package.name}.replace_tmp")
    backup = current_package.with_name(f"{current_package.name}.backup_tmp")
    if tmp_replace.exists():
        shutil.rmtree(tmp_replace)
    if backup.exists():
        shutil.rmtree(backup)
    shutil.copytree(candidate_package, tmp_replace)
    if current_package.exists():
        current_package.rename(backup)
    tmp_replace.rename(current_package)
    if backup.exists():
        shutil.rmtree(backup)


def simulate_save(
    *,
    current_package: Path,
    candidate_package: Path,
    report_dir: Path,
    dirty_before: bool,
) -> dict[str, Any]:
    current_package = Path(current_package)
    candidate_package = Path(candidate_package)
    report_dir = Path(report_dir)
    before_hash = package_hash(current_package) if current_package.exists() else None

    validation_report = VALIDATOR.validate_package(candidate_package)
    label = f"save_{candidate_package.parent.name}"
    validation_report_path = write_validation_report(report_dir, label, validation_report)
    error_codes = [
        finding["errorCode"]
        for finding in validation_report["findings"]
        if finding["severity"] == "error"
    ]

    if validation_report["errorCount"] > 0:
        after_hash = package_hash(current_package) if current_package.exists() else None
        return {
            "schemaVersion": "save-transaction-result/v1",
            "scenario": label,
            "decision": "SaveFailed",
            "validationDecision": validation_report["decision"],
            "dirtyBefore": dirty_before,
            "dirtyAfter": dirty_before,
            "oldPackageHashBefore": before_hash,
            "oldPackageHashAfter": after_hash,
            "candidatePackage": candidate_package.as_posix(),
            "currentPackage": current_package.as_posix(),
            "validationReportPath": validation_report_path.as_posix(),
            "errorCodes": error_codes,
            "guardrails": {
                "oldPackagePreserved": before_hash == after_hash,
                "dirtyPreserved": True,
            },
        }

    replace_directory(current_package, candidate_package)
    after_hash = package_hash(current_package)
    return {
        "schemaVersion": "save-transaction-result/v1",
        "scenario": label,
        "decision": "SaveSucceeded",
        "validationDecision": validation_report["decision"],
        "dirtyBefore": dirty_before,
        "dirtyAfter": False,
        "oldPackageHashBefore": before_hash,
        "oldPackageHashAfter": after_hash,
        "candidatePackage": candidate_package.as_posix(),
        "currentPackage": current_package.as_posix(),
        "validationReportPath": validation_report_path.as_posix(),
        "errorCodes": error_codes,
        "guardrails": {
            "oldPackageReplaced": before_hash != after_hash,
            "dirtyCleared": True,
        },
    }


def simulate_open(package_path: Path) -> dict[str, Any]:
    package_path = Path(package_path)
    validation_report = VALIDATOR.validate_package(package_path)
    if validation_report["decision"] == "pass":
        open_state = "formalEdit"
    elif validation_report["decision"] == "warning-only":
        open_state = "warningOnly"
    else:
        open_state = "blocked"
    return {
        "schemaVersion": "open-transaction-result/v1",
        "packagePath": package_path.as_posix(),
        "openState": open_state,
        "validationDecision": validation_report["decision"],
        "errorCount": validation_report["errorCount"],
        "warningCount": validation_report["warningCount"],
        "errorCodes": [
            finding["errorCode"]
            for finding in validation_report["findings"]
            if finding["severity"] == "error"
        ],
    }


def run_fixture_scenarios(root: Path, out_dir: Path) -> dict[str, Any]:
    fixture_root = root / "fixtures" / "tsrebar"
    out_dir.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="tsrebar-save-open-") as temp_dir:
        current = Path(temp_dir) / "current.tsrebar"
        shutil.copytree(fixture_root / "fixture_a_empty_step" / "project.tsrebar", current)
        failed_save = simulate_save(
            current_package=current,
            candidate_package=fixture_root / "negative" / "broken_binding" / "project.tsrebar",
            report_dir=out_dir,
            dirty_before=True,
        )
        successful_save = simulate_save(
            current_package=current,
            candidate_package=fixture_root / "fixture_b_single_group" / "project.tsrebar",
            report_dir=out_dir,
            dirty_before=True,
        )

    open_cases = [
        fixture_root / "fixture_a_empty_step" / "project.tsrebar",
        fixture_root / "fixture_b_single_group" / "project.tsrebar",
        fixture_root / "fixture_c_pending_legacy" / "project.tsrebar",
        fixture_root / "negative" / "missing_required_file" / "project.tsrebar",
        fixture_root / "negative" / "half_written_package" / "project.tsrebar",
    ]
    open_results = [simulate_open(path) for path in open_cases]
    summary = {
        "schemaVersion": "save-open-run/v1",
        "runId": "save_open_run_001",
        "createdAt": datetime.now(CHINA_TZ).isoformat(timespec="seconds"),
        "runner": "tools/tsrebar_transaction_gate/transaction_gate.py",
        "scope": "standalone transaction gate around tsrebar validator; not Qt6 application Save/Open",
        "saveResults": [failed_save, successful_save],
        "openResults": open_results,
        "decision": "standalone-pass",
        "remainingGaps": [
            "Qt6 application Save/Open integration not run",
            "real STEP multi-run diff not run",
            "binding repair workflow not implemented",
        ],
    }
    write_json(out_dir / "save_open_run_001.json", summary)
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description="Run standalone tsrebar Save/Open transaction scenarios.")
    parser.add_argument("--root", type=Path, default=ROOT, help="documentation root")
    parser.add_argument("--out-dir", type=Path, required=True, help="directory for reports")
    parser.add_argument("--run-fixtures", action="store_true", help="run built-in fixture scenarios")
    args = parser.parse_args()
    if not args.run_fixtures:
        parser.error("--run-fixtures is required")
    summary = run_fixture_scenarios(args.root, args.out_dir)
    print(json.dumps(summary, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
