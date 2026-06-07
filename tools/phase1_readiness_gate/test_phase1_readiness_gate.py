#!/usr/bin/env python3
import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("check_phase1_readiness.py")
SPEC = importlib.util.spec_from_file_location("check_phase1_readiness", MODULE_PATH)
assert SPEC and SPEC.loader
gate = importlib.util.module_from_spec(SPEC)
sys.modules["check_phase1_readiness"] = gate
SPEC.loader.exec_module(gate)


class Phase1ReadinessGateTests(unittest.TestCase):
    def make_guardrail_root(self) -> tuple[tempfile.TemporaryDirectory[str], Path]:
        tmp = tempfile.TemporaryDirectory()
        root = Path(tmp.name)
        (root / "app" / "src" / "domain" / "rebar").mkdir(parents=True)
        (root / "app" / "src" / "drawing").mkdir(parents=True)
        (root / "app" / "src" / "project").mkdir(parents=True)
        (root / "docs" / "phase1" / "app_build_reports").mkdir(parents=True)
        return tmp, root

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

    def test_current_workspace_route_guardrails_are_present_and_clean(self):
        root = gate.root_dir()
        checks = gate.collect_checks(root)
        route_checks = {
            check.item: check
            for check in checks
            if check.gate == "RouteGuardrail"
        }

        for item in [
            "domain_rebar_occt_boundary",
            "domain_drawing_project_occt_boundary",
            "parent_rebar_business_reference",
            "todo_status_single_next",
            "todo_status_values",
            "done_node_reports",
        ]:
            self.assertIn(item, route_checks)

        route_errors = [
            check
            for check in route_checks.values()
            if not check.ok and check.severity == "error"
        ]
        self.assertEqual([], route_errors)

    def test_route_guardrail_detects_occt_leak_in_domain_rebar(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            leak = root / "app" / "src" / "domain" / "rebar" / "BadLeak.h"
            leak.write_text("#include <TopoDS_Edge.hxx>\nAIS_Shape* shape;\n", encoding="utf-8")

            checks = gate.collect_route_guardrail_checks(root)
            leak_checks = [check for check in checks if check.item == "domain_rebar_occt_boundary"]

            self.assertEqual(1, len(leak_checks))
            self.assertFalse(leak_checks[0].ok)
            self.assertEqual("error", leak_checks[0].severity)
            self.assertIn("GAP-ROUTE-001", leak_checks[0].gap)
            self.assertIn("BadLeak.h", leak_checks[0].message)

    def test_route_guardrail_detects_parent_rebar_business_import(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            bad_file = root / "app" / "src" / "command" / "BadFactoryUse.cpp"
            bad_file.parent.mkdir(parents=True)
            bad_file.write_text("auto x = EdgeToRebarFactory{};\n", encoding="utf-8")

            checks = gate.collect_route_guardrail_checks(root)
            parent_checks = [check for check in checks if check.item == "parent_rebar_business_reference"]

            self.assertEqual(1, len(parent_checks))
            self.assertFalse(parent_checks[0].ok)
            self.assertEqual("error", parent_checks[0].severity)
            self.assertIn("GAP-ROUTE-002", parent_checks[0].gap)
            self.assertIn("BadFactoryUse.cpp", parent_checks[0].message)

    def test_route_guardrail_requires_exactly_one_next_todo(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-001","P1","A","one","next","","","","","","",""\n'
                '"TODO-002","P1","A","two","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            todo_checks = [check for check in checks if check.item == "todo_status_single_next"]

            self.assertEqual(1, len(todo_checks))
            self.assertFalse(todo_checks[0].ok)
            self.assertEqual("error", todo_checks[0].severity)
            self.assertIn("GAP-ROUTE-003", todo_checks[0].gap)
            self.assertIn("next_count=2", todo_checks[0].message)

    def test_route_guardrail_requires_done_node_reports(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-027","P1","M2-UI","旧 UI 功能入口 1:1 复刻 P1","done","","","","","","",""\n'
                '"TODO-028","P1","Gate","CSE readiness gate 扩展","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-027", report_checks[0].message)

    def test_route_guardrail_requires_todo039_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-039","P2","M2-Drawing-008","DetailWriter section-line AutoCAD L2 运行确认准备 P0","done","","","","","","",""\n'
                '"TODO-040","P2","M2-Drawing-009","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-039", report_checks[0].message)
            self.assertIn("75_M2-Drawing-008DetailWriterSectionLineAutoCADL2运行确认准备P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo040_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-040","P2","M2-Drawing-009","DetailWriter line container field skeleton P0","done","","","","","","",""\n'
                '"TODO-041","P2","M2-Drawing-010","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-040", report_checks[0].message)
            self.assertIn("76_M2-Drawing-009DetailWriter线容器字段骨架P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo041_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-041","P2","M2-Drawing-010","DetailWriter line-containers AutoCAD L2 confirmation preparation P0","done","","","","","","",""\n'
                '"TODO-042","P2","M2-Drawing-011","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-041", report_checks[0].message)
            self.assertIn("77_M2-Drawing-010DetailWriter线容器AutoCADL2运行确认准备P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo042_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-042","P2","M2-Drawing-011","DetailWriter pointStb / FaceEdge AutoCAD L2 confirmation preparation P0","done","","","","","","",""\n'
                '"TODO-043","P2","M2-Drawing-012","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-042", report_checks[0].message)
            self.assertIn("78_M2-Drawing-011DetailWriter点筋FaceEdgeAutoCADL2运行确认准备P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo043_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-043","P2","M2-Drawing-012","DetailWriter Others / steeljoint-line field skeleton P0","done","","","","","","",""\n'
                '"TODO-044","P2","M2-Drawing-013","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-043", report_checks[0].message)
            self.assertIn("79_M2-Drawing-012DetailWriterOthersSteeljoint字段骨架P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo044_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-044","P2","M2-Drawing-013","DetailWriter Others / steeljoint-line AutoCAD L2 confirmation preparation P0","done","","","","","","",""\n'
                '"TODO-045","P2","M2-Drawing-014","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-044", report_checks[0].message)
            self.assertIn("80_M2-Drawing-013DetailWriterOthersSteeljointAutoCADL2运行确认准备P0实现记录.md",
                          report_checks[0].message)

    def test_route_guardrail_requires_todo045_done_report(self):
        tmp, root = self.make_guardrail_root()
        with tmp:
            (root / "todo.csv").write_text(
                '"id","priority","phase","task","status","goal_setpoint","acceptance","boundary","evidence","dependencies","risk","notes"\n'
                '"TODO-045","P2","M2-Drawing-014","真实接头线 / Others 几何算法证据补齐 P0","done","","","","","","",""\n'
                '"TODO-046","P2","M2-Drawing-015","next task","next","","","","","","",""\n',
                encoding="utf-8",
            )

            checks = gate.collect_route_guardrail_checks(root)
            report_checks = [check for check in checks if check.item == "done_node_reports"]

            self.assertEqual(1, len(report_checks))
            self.assertFalse(report_checks[0].ok)
            self.assertEqual("warning", report_checks[0].severity)
            self.assertIn("GAP-ROUTE-004", report_checks[0].gap)
            self.assertIn("TODO-045", report_checks[0].message)
            self.assertIn("81_M2-Drawing-014真实接头线Others几何算法证据补齐P0实现记录.md",
                          report_checks[0].message)


if __name__ == "__main__":
    unittest.main()
