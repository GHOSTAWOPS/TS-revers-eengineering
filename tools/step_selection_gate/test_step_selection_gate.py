#!/usr/bin/env python3

import json
import tempfile
import unittest
from pathlib import Path

import run_step_selection_gate as gate


def write_run(path: Path, face_refs, edge_refs):
    def to_items(values):
        items = []
        for value in values:
            if isinstance(value, tuple):
                stable_ref, fingerprint_key = value
                items.append({
                    "stableRef": stable_ref,
                    "fingerprint": {"key": fingerprint_key},
                })
            else:
                items.append({"stableRef": value})
        return items

    payload = {
        "readOk": True,
        "transferOk": True,
        "errors": [],
        "parts": [
            {
                "subShapeRefs": {
                    "faces": to_items(face_refs),
                    "edges": to_items(edge_refs),
                    "vertices": [],
                }
            }
        ],
    }
    path.write_text(json.dumps(payload), encoding="utf-8")


class StepSelectionGateTests(unittest.TestCase):
    def test_compare_runs_passes_when_refs_are_identical(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            first = root / "run1.json"
            second = root / "run2.json"
            write_run(first, ["face-1"], ["edge-1", "edge-2"])
            write_run(second, ["face-1"], ["edge-1", "edge-2"])

            decision, comparisons = gate.compare_runs([first, second])

        self.assertEqual(decision, "pass")
        self.assertEqual(comparisons[0].min_match_rate, 1.0)

    def test_compare_runs_fails_when_refs_change_order_or_identity(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            first = root / "run1.json"
            second = root / "run2.json"
            write_run(first, ["face-1"], ["edge-1", "edge-2"])
            write_run(second, ["face-1"], ["edge-2", "edge-1"])

            decision, comparisons = gate.compare_runs([first, second])

        self.assertEqual(decision, "fail")
        edge_comparison = [item for item in comparisons if item.kind == "edges"][0]
        self.assertEqual(edge_comparison.mismatched_runs, ["run2"])

    def test_compare_runs_fails_when_fingerprint_changes_under_same_ref(self):
        with tempfile.TemporaryDirectory() as temp_dir:
            root = Path(temp_dir)
            first = root / "run1.json"
            second = root / "run2.json"
            write_run(first, [], [("edge-1", "fp-a")])
            write_run(second, [], [("edge-1", "fp-b")])

            decision, comparisons = gate.compare_runs([first, second])

        self.assertEqual(decision, "fail")
        edge_comparison = [item for item in comparisons if item.kind == "edges"][0]
        self.assertEqual(edge_comparison.mismatched_runs, ["run2"])


if __name__ == "__main__":
    unittest.main()
