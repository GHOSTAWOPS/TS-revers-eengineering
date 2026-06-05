import importlib.util
import json
import shutil
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
GATE_PATH = REPO_ROOT / "tools" / "tsrebar_transaction_gate" / "transaction_gate.py"
FIXTURE_ROOT = REPO_ROOT / "fixtures" / "tsrebar"


def load_gate():
    spec = importlib.util.spec_from_file_location("transaction_gate", GATE_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def copy_package(src: Path, dst: Path):
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)


def read_project_id(package: Path) -> str:
    return json.loads((package / "project.json").read_text(encoding="utf-8-sig"))["projectId"]


class TransactionGateTests(unittest.TestCase):
    def test_failed_save_preserves_old_package_and_dirty_state(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            current = tmp_path / "current.tsrebar"
            reports = tmp_path / "reports"
            copy_package(FIXTURE_ROOT / "fixture_a_empty_step" / "project.tsrebar", current)
            before_hash = gate.package_hash(current)

            result = gate.simulate_save(
                current_package=current,
                candidate_package=FIXTURE_ROOT / "negative" / "broken_binding" / "project.tsrebar",
                report_dir=reports,
                dirty_before=True,
            )

            self.assertEqual("SaveFailed", result["decision"])
            self.assertTrue(result["dirtyBefore"])
            self.assertTrue(result["dirtyAfter"])
            self.assertEqual(before_hash, gate.package_hash(current))
            self.assertEqual("proj-fixture-a", read_project_id(current))
            self.assertIn("LGV004_GEOMETRY_PATH_BROKEN", result["errorCodes"])
            self.assertTrue(Path(result["validationReportPath"]).exists())

    def test_successful_save_replaces_package_and_clears_dirty_state(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            current = tmp_path / "current.tsrebar"
            reports = tmp_path / "reports"
            copy_package(FIXTURE_ROOT / "fixture_a_empty_step" / "project.tsrebar", current)
            before_hash = gate.package_hash(current)

            result = gate.simulate_save(
                current_package=current,
                candidate_package=FIXTURE_ROOT / "fixture_b_single_group" / "project.tsrebar",
                report_dir=reports,
                dirty_before=True,
            )

            self.assertEqual("SaveSucceeded", result["decision"])
            self.assertTrue(result["dirtyBefore"])
            self.assertFalse(result["dirtyAfter"])
            self.assertNotEqual(before_hash, gate.package_hash(current))
            self.assertEqual("proj-fixture-b", read_project_id(current))
            self.assertEqual("warning-only", result["validationDecision"])

    def test_open_maps_validation_decision_to_project_state(self):
        gate = load_gate()

        cases = {
            "fixture_a_empty_step/project.tsrebar": "formalEdit",
            "fixture_b_single_group/project.tsrebar": "warningOnly",
            "negative/missing_required_file/project.tsrebar": "blocked",
        }

        for rel, expected_state in cases.items():
            with self.subTest(rel=rel):
                result = gate.simulate_open(FIXTURE_ROOT / rel)
                self.assertEqual(expected_state, result["openState"])


if __name__ == "__main__":
    unittest.main()
