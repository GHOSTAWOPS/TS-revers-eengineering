import importlib.util
import shutil
import sys
import tempfile
import unittest
import xml.etree.ElementTree as ET
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
GATE_PATH = REPO_ROOT / "tools" / "detail_writer_gate" / "detail_writer_gate.py"
FIXTURE_ROOT = REPO_ROOT / "fixtures" / "tsrebar"


def load_gate():
    spec = importlib.util.spec_from_file_location("detail_writer_gate", GATE_PATH)
    module = importlib.util.module_from_spec(spec)
    sys.modules["detail_writer_gate"] = module
    spec.loader.exec_module(module)
    return module


class DetailWriterGateTests(unittest.TestCase):
    def test_writes_detail_package_with_l0_l1_consistent_ids(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            out_dir = Path(tmp) / "detail_out"
            report = gate.write_detail_package(
                package_path=FIXTURE_ROOT / "fixture_b_single_group" / "project.tsrebar",
                output_dir=out_dir,
                run_id="DW-TEST-001",
            )

            self.assertEqual("l0-l1-pass", report["decision"])
            self.assertTrue((out_dir / "Detail.xml").exists())
            self.assertTrue((out_dir / "Detail01.stl").exists())

            style_root = ET.parse(out_dir / "Detail.xml").getroot()
            drawing_root = ET.parse(out_dir / "Detail01.stl").getroot()
            self.assertEqual("StyleRoot", style_root.tag)
            self.assertEqual("DrawingRoot", drawing_root.tag)

            stb_group = drawing_root.find(".//StbGroup")
            stb_row = drawing_root.find(".//StbRow")
            stb_geo = drawing_root.find(".//StbGeo")
            self.assertIsNotNone(stb_group)
            self.assertIsNotNone(stb_row)
            self.assertIsNotNone(stb_geo)
            self.assertEqual(stb_group.get("rsdID"), stb_row.get("rsdID"))
            self.assertEqual("segment-001", stb_geo.get("segID"))

    def test_l1_failure_preserves_existing_detail_package(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            out_dir = Path(tmp) / "detail_out"
            out_dir.mkdir()
            (out_dir / "Detail.xml").write_text("<StyleRoot><Old /></StyleRoot>", encoding="utf-8")
            (out_dir / "Detail01.stl").write_text("<DrawingRoot><Old /></DrawingRoot>", encoding="utf-8")
            before_hash = gate.directory_hash(out_dir)

            report = gate.write_detail_package(
                package_path=FIXTURE_ROOT / "negative" / "detail_rsd_mismatch" / "project.tsrebar",
                output_dir=out_dir,
                run_id="DW-TEST-FAIL",
            )

            self.assertEqual("fail", report["decision"])
            self.assertIn("DW003_ID_CROSS_REFERENCE_FAILED", report["errorCodes"])
            self.assertEqual(before_hash, gate.directory_hash(out_dir))
            self.assertTrue(report["guardrails"]["oldPackagePreserved"])

    def test_run_writes_readiness_reports_for_l0_l1_gate(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            report_dir = Path(tmp) / "reports"
            report = gate.run_detail_writer_gate(REPO_ROOT, report_dir)

            self.assertEqual("detail-writer-l0-l1-run/v1", report["schemaVersion"])
            self.assertEqual("l0-l1-pass", report["decision"])
            self.assertFalse(report["summary"]["l2AutoCadRun"])
            self.assertTrue((report_dir / "detail_writer_l0_l1_run_001.json").exists())
            self.assertTrue((report_dir / "detail_writer_summary_001.md").exists())


if __name__ == "__main__":
    unittest.main()

