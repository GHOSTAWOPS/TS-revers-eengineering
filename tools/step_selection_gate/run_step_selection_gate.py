#!/usr/bin/env python3
"""Run multi-pass STEP stable selection reference checks.

This gate consumes visualts_step_probe --refs-json output. It proves whether
the current OCCT import path gives stable selection-v1 refs across repeated
imports for a specific STEP sample.
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
from dataclasses import dataclass
from pathlib import Path
from typing import Any


KINDS = ("faces", "edges", "vertices")


@dataclass(frozen=True)
class KindComparison:
    kind: str
    baseline_count: int
    min_match_rate: float
    duplicate_count: int
    mismatched_runs: list[str]


def read_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def item_identity(item: dict[str, Any]) -> str:
    stable_ref = item.get("stableRef", "")
    if not stable_ref:
        return ""
    fingerprint = item.get("fingerprint", {})
    fingerprint_key = fingerprint.get("key", "") if isinstance(fingerprint, dict) else ""
    if fingerprint_key:
        return f"{stable_ref}|fingerprint={fingerprint_key}"
    return stable_ref


def refs_by_kind(data: dict[str, Any]) -> dict[str, list[str]]:
    refs: dict[str, list[str]] = {kind: [] for kind in KINDS}
    for part in data.get("parts", []):
        subshape_refs = part.get("subShapeRefs", {})
        for kind in KINDS:
            for item in subshape_refs.get(kind, []):
                identity = item_identity(item)
                if identity:
                    refs[kind].append(identity)
    return refs


def duplicate_count(values: list[str]) -> int:
    return len(values) - len(set(values))


def match_rate(baseline: list[str], current: list[str]) -> float:
    denominator = max(len(baseline), len(current))
    if denominator == 0:
        return 1.0
    return len(set(baseline).intersection(current)) / denominator


def compare_runs(run_paths: list[Path]) -> tuple[str, list[KindComparison]]:
    if not run_paths:
        raise ValueError("no run files provided")

    run_data = [(path.stem, read_json(path)) for path in run_paths]
    failed_imports = [
        name
        for name, data in run_data
        if not data.get("readOk") or not data.get("transferOk") or data.get("errors")
    ]
    baseline = refs_by_kind(run_data[0][1])
    comparisons: list[KindComparison] = []

    for kind in KINDS:
        mismatched_runs: list[str] = []
        rates: list[float] = []
        duplicates = duplicate_count(baseline[kind])
        for name, data in run_data[1:]:
            current = refs_by_kind(data)[kind]
            rate = match_rate(baseline[kind], current)
            rates.append(rate)
            if current != baseline[kind]:
                mismatched_runs.append(name)
            duplicates += duplicate_count(current)
        min_rate = min(rates) if rates else 1.0
        comparisons.append(
            KindComparison(
                kind=kind,
                baseline_count=len(baseline[kind]),
                min_match_rate=min_rate,
                duplicate_count=duplicates,
                mismatched_runs=mismatched_runs,
            )
        )

    decision = "pass"
    if failed_imports:
        decision = "fail"
    for comparison in comparisons:
        if comparison.duplicate_count or comparison.mismatched_runs or comparison.min_match_rate < 1.0:
            decision = "fail"
    return decision, comparisons


def run_probe(probe: Path, sample: Path, output_path: Path) -> dict[str, str | int]:
    command = [
        str(probe),
        "--input",
        str(sample),
        "--refs-json",
        str(output_path),
    ]
    result = subprocess.run(command, text=True, capture_output=True, check=False)
    return {
        "returnCode": result.returncode,
        "stdout": result.stdout,
        "stderr": result.stderr,
    }


def write_diff_json(
    path: Path,
    sample_id: str,
    sample: Path,
    run_paths: list[Path],
    probe_results: list[dict[str, str | int]],
    run_inputs: list[str],
    decision: str,
    comparisons: list[KindComparison],
) -> None:
    first_run = read_json(run_paths[0])
    payload = {
        "schemaVersion": "step-selection-diff/v1",
        "sampleId": sample_id,
        "sourceStep": str(sample),
        "sourceSha256": first_run.get("sourceSha256"),
        "probeRunCount": len(run_paths),
        "decision": decision,
        "occtSource": "visualts_step_probe via StepImportService",
        "runs": [str(path) for path in run_paths],
        "runInputs": run_inputs,
        "probeResults": probe_results,
        "comparisons": [
            {
                "kind": item.kind,
                "baselineCount": item.baseline_count,
                "minMatchRate": item.min_match_rate,
                "duplicateCount": item.duplicate_count,
                "mismatchedRuns": item.mismatched_runs,
            }
            for item in comparisons
        ],
        "remainingLimitations": [
            "This proves stable selection-v1 refs for the tested STEP sample only.",
            "It does not prove old Tushi SFL geometry compatibility.",
            "It does not replace Qt6 application Save/Open or binding repair tests.",
        ],
    }
    path.write_text(json.dumps(payload, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def write_diff_md(
    path: Path,
    sample_id: str,
    sample: Path,
    decision: str,
    comparisons: list[KindComparison],
) -> None:
    lines = [
        f"# STEP Selection Diff: {sample_id}",
        "",
        f"- Source STEP: `{sample}`",
        "- Runner: `tools/step_selection_gate/run_step_selection_gate.py`",
        "- Probe: `visualts_step_probe --refs-json`",
        f"- Decision: `{decision}`",
        "",
        "## Stable Ref Comparison",
        "",
        "| Kind | Baseline count | Min match rate | Duplicate count | Mismatched runs |",
        "|---|---:|---:|---:|---|",
    ]
    for item in comparisons:
        mismatched = ", ".join(item.mismatched_runs) if item.mismatched_runs else "-"
        lines.append(
            f"| {item.kind} | {item.baseline_count} | {item.min_match_rate:.6f} | "
            f"{item.duplicate_count} | {mismatched} |"
        )
    lines.extend(
        [
            "",
            "## Boundary",
            "",
            "- This is real OCCT STEP import evidence for the listed sample.",
            "- It does not close Qt6 application Save/Open or binding repair gaps.",
            "- Complex project-level samples still need the same gate before GAP-DEV-002 can be closed.",
            "",
        ]
    )
    path.write_text("\n".join(lines), encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run STEP stable selection ref gate.")
    parser.add_argument("--probe", required=True, type=Path)
    parser.add_argument("--sample", required=True, type=Path)
    parser.add_argument("--sample-id", required=True)
    parser.add_argument("--out-dir", required=True, type=Path)
    parser.add_argument("--runs", default=5, type=int)
    parser.add_argument(
        "--copy-final-run",
        action="store_true",
        help="Use a copied STEP path for the final run.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    run_paths: list[Path] = []
    probe_results: list[dict[str, str | int]] = []
    run_inputs: list[str] = []
    copied_sample = args.out_dir / f"{args.sample_id}_copied_input{args.sample.suffix}"
    if args.copy_final_run:
        shutil.copy2(args.sample, copied_sample)

    for index in range(1, args.runs + 1):
        run_path = args.out_dir / f"step_selection_run_r{index}.json"
        run_sample = copied_sample if args.copy_final_run and index == args.runs else args.sample
        run_inputs.append(str(run_sample))
        probe_results.append(run_probe(args.probe, run_sample, run_path))
        run_paths.append(run_path)

    decision, comparisons = compare_runs(run_paths)
    diff_json = args.out_dir / "step_selection_diff.json"
    diff_md = args.out_dir / "step_selection_diff.md"
    write_diff_json(
        diff_json,
        args.sample_id,
        args.sample,
        run_paths,
        probe_results,
        run_inputs,
        decision,
        comparisons,
    )
    write_diff_md(diff_md, args.sample_id, args.sample, decision, comparisons)
    print(f"decision={decision}")
    print(f"diff={diff_json}")
    return 0 if decision == "pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())
