import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("check_todo_061_capture.py")
SPEC = importlib.util.spec_from_file_location("check_todo_061_capture", MODULE_PATH)
assert SPEC and SPEC.loader
gate = importlib.util.module_from_spec(SPEC)
sys.modules["check_todo_061_capture"] = gate
SPEC.loader.exec_module(gate)


class Todo061CaptureGateTests(unittest.TestCase):
    @staticmethod
    def file_bytes_for_name(name: str) -> bytes:
        suffix = Path(name).suffix.lower()
        if suffix == ".png":
            return b"\x89PNG\r\n\x1a\nfake"
        if suffix in {".jpg", ".jpeg"}:
            return b"\xff\xd8\xff\xe0fake"
        if suffix == ".bmp":
            return b"BMfake"
        if suffix == ".xls":
            return bytes.fromhex("D0CF11E0A1B11AE1") + b"fake"
        if suffix == ".xlsx":
            return b"PK\x03\x04fake"
        return b"fake"

    def make_capture_dir(self) -> tuple[tempfile.TemporaryDirectory[str], Path]:
        tmp = tempfile.TemporaryDirectory()
        root = Path(tmp.name)
        return tmp, root

    def write_notes(
        self,
        root: Path,
        *,
        pane3_name: str = "pane3_statusbar_01.png",
        pane3_text: str = "焊接",
        schedule_name: str = "barschedule_01.xls",
        schedule_source_path: str = r"C:\temp\barschedule_01.xls",
        has_joint_confirmed: str = "是",
        weld_head_visible: str = "是",
        single_length_visible: str = "是",
    ) -> None:
        schedule_source_path_line = f"- 导出原始路径：{schedule_source_path}" if schedule_source_path else "- 导出原始路径："
        text = f"""# TODO-061 Capture Notes

旧图石版本：VisualTS 2024

SFL 文件名：sample.sfl

SFL hash：{'A' * 64}

选择对象：

- 对象编号：12A
- 是否确认有接头：{has_joint_confirmed}

状态栏截图：

- 文件名：{pane3_name}
- pane3 文本：{pane3_text}

下料表导出：

- 文件名：{schedule_name}
{schedule_source_path_line}
- 是否能看到 `焊头(个)`：{weld_head_visible}
- 是否能看到 `单下料长(mm)`：{single_length_visible}

操作步骤：

1. 打开样本
2. 选中对象
3. 导出下料表
"""
        (root / "capture_notes.md").write_text(text, encoding="utf-8")

    def write_hashes(self, root: Path, *names: str) -> None:
        lines = ["# TODO-061 hashes", ""]
        for name in names:
            lines.append(f"{name}\t{'A' * 64}")
        (root / "hashes.txt").write_text("\n".join(lines), encoding="utf-8")

    def test_complete_capture_with_excel_passes(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("pass", report["decision"])
            self.assertEqual(0, report["errorCount"])

    def test_schedule_field_screenshot_without_excel_also_passes(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_fields_01.png"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("pass", report["decision"])
            self.assertEqual(0, report["errorCount"])

    def test_missing_hashes_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV006_HASH_FILE_MISSING", {f["errorCode"] for f in report["findings"]})

    def test_invalid_pane3_text_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(
                root,
                pane3_name=pane3_name,
                pane3_text="未知文本",
                schedule_name=schedule_name,
            )
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV004_PANE3_TEXT_INVALID", {f["errorCode"] for f in report["findings"]})

    def test_joint_not_confirmed_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(
                root,
                pane3_name=pane3_name,
                schedule_name=schedule_name,
                has_joint_confirmed="否",
            )
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV010_JOINT_NOT_CONFIRMED", {f["errorCode"] for f in report["findings"]})

    def test_invalid_hash_format_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)
            (root / "hashes.txt").write_text(
                "\n".join(
                    [
                        "# TODO-061 hashes",
                        "",
                        f"{pane3_name}\t{'A' * 64}",
                        f"{schedule_name}\tINVALID_HASH",
                    ]
                ),
                encoding="utf-8",
            )

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV008_HASH_ENTRY_INVALID", {f["errorCode"] for f in report["findings"]})

    def test_missing_schedule_source_path_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(
                root,
                pane3_name=pane3_name,
                schedule_name=schedule_name,
                schedule_source_path="",
            )
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV002_CAPTURE_NOTE_FIELD_MISSING", {f["errorCode"] for f in report["findings"]})

    def test_schedule_field_screenshot_without_source_path_passes(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_fields_01.png"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(
                root,
                pane3_name=pane3_name,
                schedule_name=schedule_name,
                schedule_source_path="",
            )
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("pass", report["decision"])

    def test_invalid_schedule_file_signature_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(b"not-an-excel-file")
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV005_SCHEDULE_FILE_SIGNATURE_INVALID", {f["errorCode"] for f in report["findings"]})

    def test_common_chinese_truthy_values_pass(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = "pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / pane3_name).write_bytes(self.file_bytes_for_name(pane3_name))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(
                root,
                pane3_name=pane3_name,
                schedule_name=schedule_name,
                has_joint_confirmed="已确认",
                weld_head_visible="可见",
                single_length_visible="有",
            )
            self.write_hashes(root, pane3_name, schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("pass", report["decision"])

    def test_path_in_file_name_fails(self):
        tmp, root = self.make_capture_dir()
        with tmp:
            pane3_name = r"nested\pane3_statusbar_01.png"
            schedule_name = "barschedule_01.xls"
            (root / "pane3_statusbar_01.png").write_bytes(self.file_bytes_for_name("pane3_statusbar_01.png"))
            (root / schedule_name).write_bytes(self.file_bytes_for_name(schedule_name))
            self.write_notes(root, pane3_name=pane3_name, schedule_name=schedule_name)
            self.write_hashes(root, "pane3_statusbar_01.png", schedule_name)

            report = gate.validate_capture_dir(root)

            self.assertEqual("fail", report["decision"])
            self.assertIn("RCV013_FILE_NAME_NOT_BASENAME", {f["errorCode"] for f in report["findings"]})


if __name__ == "__main__":
    unittest.main()
