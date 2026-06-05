#!/usr/bin/env python3
import importlib.util
import sys
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("check_phase1_readiness.py")
SPEC = importlib.util.spec_from_file_location("check_phase1_readiness", MODULE_PATH)
assert SPEC and SPEC.loader
gate = importlib.util.module_from_spec(SPEC)
sys.modules["check_phase1_readiness"] = gate
SPEC.loader.exec_module(gate)


class Phase1ReadinessGateTests(unittest.TestCase):
    def test_current_workspace_is_m1_formal_ready_without_blocker_gaps(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        summary = gate.summarize(checks)
        self.assertEqual(summary["decision"], "M1-Formal-Ready")
        self.assertTrue(summary["m1FormalAllowed"])
        self.assertEqual([], summary["blockerGaps"])
        self.assertNotIn("GAP-DRAW-005", summary["blockerGaps"])
        self.assertNotIn("GAP-TECH-007", summary["blockerGaps"])

    def test_seed_step_and_standalone_gate_are_not_reported_as_errors(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        failing_error_items = {
            (check.gate, check.item)
            for check in checks
            if not check.ok and check.severity == "error"
        }
        self.assertNotIn(("StandaloneSaveOpen", "save_open_run_001"), failing_error_items)
        for sample in [
            "test_stp",
            "cc001_bspline_edge",
            "cc002_bezier_edge",
            "cc003_bspline_length",
            "cc004_hostref_source_curve",
            "cc005_sampling_policy",
        ]:
            self.assertNotIn(("StepSelection", sample), failing_error_items)

    def test_dependency_gate_pass_removes_tech_blocker(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        dependency_gate_checks = [
            check
            for check in checks
            if check.gate == "Dependency" and check.item == "dependency_gate_run_001.md"
        ]

        self.assertEqual(1, len(dependency_gate_checks))
        self.assertTrue(dependency_gate_checks[0].ok)
        self.assertEqual("error", dependency_gate_checks[0].severity)
        self.assertEqual("result=pass", dependency_gate_checks[0].message)

    def test_app_save_open_reports_are_real_qt6_runtime(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        app_checks = {
            check.item: check
            for check in checks
            if check.gate == "Qt6Application"
        }

        self.assertTrue(app_checks["app_save_open_run_001.json"].ok)
        self.assertEqual("decision=qt6-app-pass", app_checks["app_save_open_run_001.json"].message)
        self.assertTrue(app_checks["binding_repair_run_001.json"].ok)
        self.assertEqual("decision=qt6-binding-repair-pass", app_checks["binding_repair_run_001.json"].message)
        self.assertTrue(app_checks["binding_repair_preview_001.md"].ok)

    def test_detail_writer_l0_l1_report_must_pass_not_merely_exist(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        detail_checks = {
            check.item: check
            for check in checks
            if check.gate == "DetailWriter"
        }

        self.assertTrue(detail_checks["detail_writer_l0_l1_run_001.json"].ok)
        self.assertEqual("decision=l0-l1-pass", detail_checks["detail_writer_l0_l1_run_001.json"].message)
        self.assertTrue(detail_checks["detail_writer_summary_001.md"].ok)


if __name__ == "__main__":
    unittest.main()
