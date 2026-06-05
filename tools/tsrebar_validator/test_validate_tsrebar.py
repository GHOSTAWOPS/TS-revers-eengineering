import importlib.util
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_PATH = REPO_ROOT / "tools" / "tsrebar_validator" / "validate_tsrebar.py"
FIXTURE_ROOT = REPO_ROOT / "fixtures" / "tsrebar"


def load_validator():
    spec = importlib.util.spec_from_file_location("validate_tsrebar", VALIDATOR_PATH)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def error_codes(report):
    return {finding["errorCode"] for finding in report["findings"]}


class TsrebarValidatorTests(unittest.TestCase):
    def test_positive_fixtures_validate_without_errors(self):
        validator = load_validator()

        expected = {
            "fixture_a_empty_step": "pass",
            "fixture_b_single_group": "warning-only",
            "fixture_c_pending_legacy": "warning-only",
        }

        for fixture_name, expected_decision in expected.items():
            with self.subTest(fixture_name=fixture_name):
                package_path = FIXTURE_ROOT / fixture_name / "project.tsrebar"
                report = validator.validate_package(package_path)

                self.assertEqual("validation-report/v1", report["schemaVersion"])
                self.assertEqual(expected_decision, report["decision"])
                self.assertEqual(0, report["errorCount"])

    def test_negative_fixtures_emit_expected_error_codes(self):
        validator = load_validator()

        expected = {
            "missing_required_file": {"PV001_REQUIRED_FILE_MISSING"},
            "broken_binding": {"LGV004_GEOMETRY_PATH_BROKEN"},
            "acis_pointer_as_geometry_id": {"GRV004_ACIS_POINTER_USED_AS_STABLE_ID"},
            "transient_index_only": {"GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID"},
            "forged_evidence": {"EV001_EVIDENCE_ID_NOT_FOUND", "PRV004_EVIDENCE_REF_BROKEN"},
            "pending_field_marked_confirmed": {"EV003_PENDING_MARKED_CONFIRMED", "EV004_CONFIRMED_WITHOUT_EVIDENCE"},
            "detail_rsd_mismatch": {"DMV002_RSD_ID_MISMATCH"},
            "half_written_package": {
                "PV004_SAVE_TRANSACTION_INCOMPLETE",
                "TXV001_TEMP_PACKAGE_INVALID",
                "TXV006_HALF_WRITTEN_PACKAGE_OPENED_AS_FORMAL",
            },
            "missing_step_diff": {"GRV006_STEP_DIFF_MISSING"},
        }

        for fixture_name, expected_codes in expected.items():
            with self.subTest(fixture_name=fixture_name):
                package_path = FIXTURE_ROOT / "negative" / fixture_name / "project.tsrebar"
                report = validator.validate_package(package_path)

                self.assertEqual("fail", report["decision"])
                self.assertTrue(expected_codes.issubset(error_codes(report)))


if __name__ == "__main__":
    unittest.main()
