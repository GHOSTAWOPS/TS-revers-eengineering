#!/usr/bin/env python3
import json
import subprocess
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RUNNER = ROOT / "tools" / "qt6_runtime_gate" / "run_qt6_runtime_gate.py"
APP_REPORT = ROOT / "docs" / "phase1" / "app_save_open_reports" / "app_save_open_run_001.json"
BINDING_REPORT = ROOT / "docs" / "phase1" / "app_save_open_reports" / "binding_repair_run_001.json"
PREVIEW_REPORT = ROOT / "docs" / "phase1" / "app_save_open_reports" / "binding_repair_preview_001.md"


class Qt6RuntimeGateTests(unittest.TestCase):
    def test_qt6_cpp_runtime_gate_generates_formal_readiness_reports(self):
        completed = subprocess.run(
            [sys.executable, str(RUNNER), "--skip-configure-if-built"],
            cwd=ROOT,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            timeout=120,
        )

        self.assertEqual(completed.returncode, 0, completed.stdout)

        app_report = json.loads(APP_REPORT.read_text(encoding="utf-8"))
        binding_report = json.loads(BINDING_REPORT.read_text(encoding="utf-8"))
        preview_text = PREVIEW_REPORT.read_text(encoding="utf-8")

        self.assertEqual("qt6-app-pass", app_report["summary"]["decision"])
        self.assertTrue(app_report["summary"]["m1FormalCandidate"])
        self.assertEqual("Qt6 C++ runtime gate", app_report["scope"])
        self.assertTrue(app_report["qtRuntime"]["qApplicationCreated"])
        self.assertTrue(app_report["qtRuntime"]["guiCommandActionsCreated"])
        self.assertEqual("qt6-binding-repair-pass", binding_report["decision"])
        self.assertTrue(binding_report["m1FormalCandidate"])
        self.assertIn("Qt6 C++ runtime", preview_text)
        self.assertNotIn("pre-Qt6", preview_text)


if __name__ == "__main__":
    unittest.main()
