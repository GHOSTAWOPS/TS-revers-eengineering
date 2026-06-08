#!/usr/bin/env python3
"""Validate TODO-061 runtime capture intake artifacts."""

from __future__ import annotations

import argparse
import json
import re
from datetime import datetime, timezone, timedelta
from pathlib import Path
from typing import Any


CHINA_TZ = timezone(timedelta(hours=8))
IMAGE_SUFFIXES = {".png", ".jpg", ".jpeg", ".bmp"}
EXCEL_SUFFIXES = {".xls", ".xlsx"}
VALID_PANE3_TEXTS = {"焊接", "绑扎", "套筒连接"}
TRUTHY = {"1", "true", "yes", "y", "是", "有", "可见", "确认", "已确认"}
HASH_PATTERN = re.compile(r"^[0-9A-Fa-f]{64}$")


NOTE_PATTERNS = {
    "visualts_version": re.compile(r"^旧图石版本：\s*(.+?)\s*$"),
    "sfl_name": re.compile(r"^SFL 文件名：\s*(.+?)\s*$"),
    "sfl_hash": re.compile(r"^SFL hash：\s*(.+?)\s*$"),
    "object_id": re.compile(r"^-\s*对象编号：\s*(.+?)\s*$"),
    "has_joint_confirmed": re.compile(r"^-\s*是否确认有接头：\s*(.+?)\s*$"),
    "pane3_file": re.compile(r"^-\s*文件名：\s*(.+?)\s*$"),
    "pane3_text": re.compile(r"^-\s*pane3 文本：\s*(.+?)\s*$"),
    "schedule_file": re.compile(r"^-\s*文件名：\s*(.+?)\s*$"),
    "schedule_source_path": re.compile(r"^-\s*导出原始路径：\s*(.+?)\s*$"),
    "weld_head_visible": re.compile(r"^-\s*是否能看到 `焊头\(个\)`：\s*(.+?)\s*$"),
    "single_length_visible": re.compile(r"^-\s*是否能看到 `单下料长\(mm\)`：\s*(.+?)\s*$"),
}


def repo_root() -> Path:
    return Path(__file__).resolve().parents[2]


def default_capture_dir() -> Path:
    return repo_root() / "docs" / "phase1" / "runtime_capture" / "todo_061_generated_node112"


def parse_hashes(path: Path) -> dict[str, str]:
    entries: dict[str, str] = {}
    if not path.exists():
        return entries
    for line in path.read_text(encoding="utf-8-sig").splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        parts = stripped.split("\t")
        if len(parts) >= 2:
            entries[parts[0].strip()] = parts[1].strip()
    return entries


def truthy(value: str) -> bool:
    return value.strip().lower() in TRUTHY


def looks_like_sha256(value: str) -> bool:
    normalized = value.strip()
    if normalized.lower().startswith("sha256:"):
        normalized = normalized.split(":", 1)[1].strip()
    return bool(HASH_PATTERN.fullmatch(normalized))


def is_plain_file_name(value: str) -> bool:
    normalized = value.strip()
    return normalized == Path(normalized).name and not Path(normalized).is_absolute()


def has_expected_signature(path: Path) -> bool:
    suffix = path.suffix.lower()
    with path.open("rb") as handle:
        header = handle.read(16)

    if suffix == ".png":
        return header.startswith(b"\x89PNG\r\n\x1a\n")
    if suffix in {".jpg", ".jpeg"}:
        return header.startswith(b"\xff\xd8\xff")
    if suffix == ".bmp":
        return header.startswith(b"BM")
    if suffix == ".xls":
        return header.startswith(bytes.fromhex("D0CF11E0A1B11AE1"))
    if suffix == ".xlsx":
        return header.startswith(b"PK\x03\x04")
    return False


def parse_capture_notes(path: Path) -> dict[str, str]:
    if not path.exists():
        return {}

    result: dict[str, str] = {}
    in_status_section = False
    in_schedule_section = False
    for raw_line in path.read_text(encoding="utf-8-sig").splitlines():
        line = raw_line.rstrip()
        if line.startswith("状态栏截图："):
            in_status_section = True
            in_schedule_section = False
            continue
        if line.startswith("下料表导出："):
            in_status_section = False
            in_schedule_section = True
            continue
        if line.startswith("操作步骤：") or line.startswith("补充说明："):
            in_status_section = False
            in_schedule_section = False

        if "visualts_version" not in result:
            match = NOTE_PATTERNS["visualts_version"].match(line)
            if match:
                result["visualts_version"] = match.group(1).strip()
                continue
        if "sfl_name" not in result:
            match = NOTE_PATTERNS["sfl_name"].match(line)
            if match:
                result["sfl_name"] = match.group(1).strip()
                continue
        if "sfl_hash" not in result:
            match = NOTE_PATTERNS["sfl_hash"].match(line)
            if match:
                result["sfl_hash"] = match.group(1).strip()
                continue
        if "object_id" not in result:
            match = NOTE_PATTERNS["object_id"].match(line)
            if match:
                result["object_id"] = match.group(1).strip()
                continue
        if "has_joint_confirmed" not in result:
            match = NOTE_PATTERNS["has_joint_confirmed"].match(line)
            if match:
                result["has_joint_confirmed"] = match.group(1).strip()
                continue

        if in_status_section:
            if "pane3_file" not in result:
                match = NOTE_PATTERNS["pane3_file"].match(line)
                if match:
                    result["pane3_file"] = match.group(1).strip()
                    continue
            if "pane3_text" not in result:
                match = NOTE_PATTERNS["pane3_text"].match(line)
                if match:
                    result["pane3_text"] = match.group(1).strip()
                    continue

        if in_schedule_section:
            if "schedule_file" not in result:
                match = NOTE_PATTERNS["schedule_file"].match(line)
                if match:
                    result["schedule_file"] = match.group(1).strip()
                    continue
            if "schedule_source_path" not in result:
                match = NOTE_PATTERNS["schedule_source_path"].match(line)
                if match:
                    result["schedule_source_path"] = match.group(1).strip()
                    continue
            if "weld_head_visible" not in result:
                match = NOTE_PATTERNS["weld_head_visible"].match(line)
                if match:
                    result["weld_head_visible"] = match.group(1).strip()
                    continue
            if "single_length_visible" not in result:
                match = NOTE_PATTERNS["single_length_visible"].match(line)
                if match:
                    result["single_length_visible"] = match.group(1).strip()
                    continue

    return result


def add_finding(
    findings: list[dict[str, Any]],
    error_code: str,
    message: str,
    *,
    severity: str = "error",
    path: str = "",
) -> None:
    findings.append(
        {
            "severity": severity,
            "errorCode": error_code,
            "message": message,
            "path": path,
        }
    )


def validate_capture_dir(capture_dir: str | Path) -> dict[str, Any]:
    capture_path = Path(capture_dir)
    findings: list[dict[str, Any]] = []
    notes_path = capture_path / "capture_notes.md"
    hashes_path = capture_path / "hashes.txt"
    notes = parse_capture_notes(notes_path)
    hashes = parse_hashes(hashes_path)

    if not capture_path.exists():
        add_finding(findings, "RCV000_CAPTURE_DIR_MISSING", "capture directory is missing", path=str(capture_path))
    if not notes_path.exists():
        add_finding(findings, "RCV001_CAPTURE_NOTES_MISSING", "capture_notes.md is missing", path=str(notes_path))

    for key, label in [
        ("visualts_version", "旧图石版本"),
        ("sfl_name", "SFL 文件名"),
        ("sfl_hash", "SFL hash"),
        ("object_id", "对象编号"),
        ("has_joint_confirmed", "是否确认有接头"),
        ("pane3_file", "状态栏截图文件名"),
        ("pane3_text", "pane3 文本"),
        ("schedule_file", "下料表文件名"),
        ("weld_head_visible", "焊头(个) 可见性"),
        ("single_length_visible", "单下料长(mm) 可见性"),
    ]:
        if not notes.get(key):
            add_finding(findings, "RCV002_CAPTURE_NOTE_FIELD_MISSING", f"{label} 未填写", path="capture_notes.md")

    sfl_hash = notes.get("sfl_hash", "")
    if sfl_hash and not looks_like_sha256(sfl_hash):
        add_finding(findings, "RCV012_SFL_HASH_INVALID", "SFL hash 不是有效的 SHA256", path="capture_notes.md")

    pane3_file_name = notes.get("pane3_file", "")
    pane3_file = capture_path / pane3_file_name
    if pane3_file_name:
        if not is_plain_file_name(pane3_file_name):
            add_finding(findings, "RCV013_FILE_NAME_NOT_BASENAME", "状态栏截图文件名必须是当前目录下的纯文件名", path="capture_notes.md")
        elif not pane3_file.exists():
            add_finding(findings, "RCV003_PANE3_FILE_NOT_FOUND", "状态栏截图文件不存在", path=notes["pane3_file"])
        elif pane3_file.suffix.lower() not in IMAGE_SUFFIXES:
            add_finding(findings, "RCV003_PANE3_FILE_NOT_IMAGE", "状态栏截图文件不是图片", path=notes["pane3_file"])
        elif not has_expected_signature(pane3_file):
            add_finding(findings, "RCV003_PANE3_FILE_SIGNATURE_INVALID", "状态栏截图文件头与扩展名不匹配", path=notes["pane3_file"])

    pane3_text = notes.get("pane3_text", "")
    if pane3_text and pane3_text not in VALID_PANE3_TEXTS:
        add_finding(
            findings,
            "RCV004_PANE3_TEXT_INVALID",
            "pane3 文本不是 焊接 / 绑扎 / 套筒连接 之一",
            path="capture_notes.md",
        )

    schedule_file_name = notes.get("schedule_file", "")
    schedule_file = capture_path / schedule_file_name
    if schedule_file_name:
        if not is_plain_file_name(schedule_file_name):
            add_finding(findings, "RCV013_FILE_NAME_NOT_BASENAME", "下料表文件名必须是当前目录下的纯文件名", path="capture_notes.md")
        elif not schedule_file.exists():
            add_finding(findings, "RCV005_SCHEDULE_FILE_NOT_FOUND", "下料表文件不存在", path=notes["schedule_file"])
        elif schedule_file.suffix.lower() not in (IMAGE_SUFFIXES | EXCEL_SUFFIXES):
            add_finding(
                findings,
                "RCV005_SCHEDULE_FILE_INVALID_TYPE",
                "下料表文件必须是 Excel 或字段截图图片",
                path=notes["schedule_file"],
            )
        elif not has_expected_signature(schedule_file):
            add_finding(
                findings,
                "RCV005_SCHEDULE_FILE_SIGNATURE_INVALID",
                "下料表文件头与扩展名不匹配",
                path=notes["schedule_file"],
            )

    if schedule_file_name and schedule_file.suffix.lower() in EXCEL_SUFFIXES and not notes.get("schedule_source_path"):
        add_finding(findings, "RCV002_CAPTURE_NOTE_FIELD_MISSING", "下料表导出原始路径 未填写", path="capture_notes.md")

    if not hashes_path.exists():
        add_finding(findings, "RCV006_HASH_FILE_MISSING", "hashes.txt 缺失", path="hashes.txt")
    else:
        for key in ["pane3_file", "schedule_file"]:
            file_name = notes.get(key, "")
            if file_name and not is_plain_file_name(file_name):
                continue
            if file_name and file_name not in hashes:
                add_finding(findings, "RCV007_HASH_ENTRY_MISSING", f"{file_name} 未出现在 hashes.txt 中", path="hashes.txt")
            elif file_name and not looks_like_sha256(hashes[file_name]):
                add_finding(findings, "RCV008_HASH_ENTRY_INVALID", f"{file_name} 的 hash 不是有效的 SHA256", path="hashes.txt")

    if notes.get("has_joint_confirmed") and not truthy(notes["has_joint_confirmed"]):
        add_finding(findings, "RCV010_JOINT_NOT_CONFIRMED", "未确认选中对象确实有接头", path="capture_notes.md")
    if notes.get("weld_head_visible") and not truthy(notes["weld_head_visible"]):
        add_finding(findings, "RCV009_WELD_HEAD_FIELD_NOT_CONFIRMED", "未确认焊头(个) 可见", path="capture_notes.md")
    if notes.get("single_length_visible") and not truthy(notes["single_length_visible"]):
        add_finding(findings, "RCV011_SINGLE_LENGTH_FIELD_NOT_CONFIRMED", "未确认单下料长(mm) 可见", path="capture_notes.md")

    error_count = sum(1 for item in findings if item["severity"] == "error")
    warning_count = sum(1 for item in findings if item["severity"] == "warning")
    decision = "fail" if error_count else "pass"

    return {
        "decision": decision,
        "captureDir": capture_path.as_posix(),
        "checkedAt": datetime.now(CHINA_TZ).isoformat(timespec="seconds"),
        "errorCount": error_count,
        "warningCount": warning_count,
        "notes": notes,
        "hashEntryCount": len(hashes),
        "findings": findings,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Validate TODO-061 runtime capture intake.")
    parser.add_argument("--capture-dir", default=str(default_capture_dir()))
    parser.add_argument("--json-out", default="")
    args = parser.parse_args()

    report = validate_capture_dir(args.capture_dir)
    if args.json_out:
        output_path = Path(args.json_out)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding="utf-8")

    print(json.dumps(report, ensure_ascii=False, indent=2))
    return 0 if report["decision"] == "pass" else 1


if __name__ == "__main__":
    raise SystemExit(main())
