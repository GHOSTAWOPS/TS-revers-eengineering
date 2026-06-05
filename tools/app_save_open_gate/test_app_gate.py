import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
GATE_PATH = REPO_ROOT / "tools" / "app_save_open_gate" / "app_gate.py"
FIXTURE_ROOT = REPO_ROOT / "fixtures" / "tsrebar"


def load_gate():
    spec = importlib.util.spec_from_file_location("app_gate", GATE_PATH)
    module = importlib.util.module_from_spec(spec)
    sys.modules["app_gate"] = module
    spec.loader.exec_module(module)
    return module


class AppSaveOpenGateTests(unittest.TestCase):
    def test_open_maps_validator_and_binding_state_to_qt_application_state(self):
        gate = load_gate()

        cases = {
            "fixture_a_empty_step/project.tsrebar": ("OpenedFormal", "resolved"),
            "fixture_b_single_group/project.tsrebar": ("OpenedWarning", "warningOnly"),
            "fixture_c_pending_legacy/project.tsrebar": ("OpenedRepairRequired", "pendingLegacyEvidence"),
            "negative/broken_binding/project.tsrebar": ("OpenedRepairRequired", "repairRequired"),
            "negative/missing_required_file/project.tsrebar": ("OpenBlocked", "blocked"),
        }

        for rel, expected in cases.items():
            with self.subTest(rel=rel):
                result = gate.open_project(FIXTURE_ROOT / rel)
                self.assertEqual(expected[0], result["finalState"])
                self.assertEqual(expected[1], result["bindingDecision"])
                if expected[0] == "OpenedRepairRequired":
                    self.assertFalse(result["commandAvailability"]["rebarEdit"])
                    self.assertTrue(result["commandAvailability"]["bindingRepair"])

    def test_save_failure_preserves_old_package_hash_and_dirty_state(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            current = tmp_path / "current.tsrebar"
            gate.copy_package(FIXTURE_ROOT / "fixture_a_empty_step" / "project.tsrebar", current)
            before_hash = gate.package_hash(current)

            result = gate.save_project(
                current_package=current,
                candidate_package=FIXTURE_ROOT / "negative" / "broken_binding" / "project.tsrebar",
                report_dir=tmp_path / "reports",
                dirty_before=True,
            )

            self.assertEqual("SaveFailed", result["finalState"])
            self.assertTrue(result["dirtyAfter"])
            self.assertEqual(before_hash, gate.package_hash(current))
            self.assertTrue(result["guardrails"]["oldPackagePreserved"])
            self.assertTrue(result["guardrails"]["dirtyPreserved"])
            self.assertIn("LGV004_GEOMETRY_PATH_BROKEN", result["errorCodes"])

    def test_save_success_replaces_package_and_clears_dirty_state(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            tmp_path = Path(tmp)
            current = tmp_path / "current.tsrebar"
            gate.copy_package(FIXTURE_ROOT / "fixture_a_empty_step" / "project.tsrebar", current)
            before_hash = gate.package_hash(current)

            result = gate.save_project(
                current_package=current,
                candidate_package=FIXTURE_ROOT / "fixture_b_single_group" / "project.tsrebar",
                report_dir=tmp_path / "reports",
                dirty_before=True,
            )

            self.assertEqual("OpenedWarning", result["finalState"])
            self.assertFalse(result["dirtyAfter"])
            self.assertNotEqual(before_hash, gate.package_hash(current))
            self.assertTrue(result["guardrails"]["dirtyCleared"])

    def test_binding_repair_preview_apply_and_cancel_are_explicit_state_transitions(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            report_dir = Path(tmp) / "reports"
            source = FIXTURE_ROOT / "negative" / "broken_binding" / "project.tsrebar"
            candidate = FIXTURE_ROOT / "fixture_b_single_group" / "project.tsrebar"

            preview = gate.binding_repair_preview(source, candidate, report_dir)
            self.assertEqual("repairPreview", preview["bindingStateAfter"])
            self.assertTrue(preview["guardrails"]["previewOnly"])
            self.assertTrue(preview["guardrails"]["silentRebindForbidden"])
            self.assertEqual(1, preview["summary"]["matchCount"])

            cancel = gate.binding_repair_cancel(preview, report_dir)
            self.assertEqual("OpenedRepairRequired", cancel["finalState"])
            self.assertFalse(cancel["dirtyAfter"])

            apply_result = gate.binding_repair_apply(source, candidate, preview, report_dir)
            self.assertEqual("repairAppliedPendingSave", apply_result["bindingStateBeforeSave"])
            self.assertEqual("SaveSucceeded", apply_result["saveDecision"])
            self.assertFalse(apply_result["dirtyAfter"])
            self.assertTrue(Path(apply_result["savedPackage"]).exists())

    def test_run_writes_readiness_reports_with_non_formal_decision(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            out_dir = Path(tmp) / "reports"
            report = gate.run_app_gate(REPO_ROOT, out_dir)

            self.assertEqual("app-save-open-run/v1", report["schemaVersion"])
            self.assertFalse(report["summary"]["m1FormalCandidate"])
            self.assertTrue((out_dir / "app_save_open_run_001.json").exists())
            self.assertTrue((out_dir / "binding_repair_run_001.json").exists())
            self.assertTrue((out_dir / "binding_repair_preview_001.md").exists())

            run_json = json.loads((out_dir / "app_save_open_run_001.json").read_text(encoding="utf-8"))
            self.assertEqual("needs-real-qt6-runtime", run_json["summary"]["decision"])


if __name__ == "__main__":
    unittest.main()

