import json
import subprocess
import sys
import tempfile
from pathlib import Path


AUTO_CAD_REGISTRY_KEYS = [
    ("HKEY_LOCAL_MACHINE\\SOFTWARE\\Autodesk\\AutoCAD", "HKLM:\\SOFTWARE\\Autodesk\\AutoCAD"),
    (
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Autodesk\\AutoCAD",
        "HKLM:\\SOFTWARE\\WOW6432Node\\Autodesk\\AutoCAD",
    ),
    ("HKEY_CURRENT_USER\\SOFTWARE\\Autodesk\\AutoCAD", "HKCU:\\SOFTWARE\\Autodesk\\AutoCAD"),
]


def powershell_registry_probe(ps_key: str) -> dict[str, object]:
    script = (
        "$key = '" + ps_key.replace("'", "''") + "'; "
        "$exists = Test-Path -LiteralPath $key; "
        "$children = @(); "
        "if ($exists) { "
        "$children = @(Get-ChildItem -LiteralPath $key -ErrorAction SilentlyContinue | "
        "Select-Object -ExpandProperty PSChildName) "
        "}; "
        "[pscustomobject]@{exists=$exists; childGroupCount=$children.Count} | ConvertTo-Json"
    )
    completed = subprocess.run(
        ["powershell", "-NoProfile", "-Command", script],
        check=True,
        capture_output=True,
        text=True,
    )
    return json.loads(completed.stdout)


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: test_detail_l2_fixture_probe_environment.py <detail_l2_fixture_probe.exe>")
        return 2

    probe = Path(sys.argv[1])
    if not probe.exists():
        print(f"probe executable not found: {probe}")
        return 2

    with tempfile.TemporaryDirectory() as temp_dir:
        package_dir = Path(temp_dir) / "detail_package"
        completed = subprocess.run(
            [
                str(probe),
                "-o",
                str(package_dir),
                "--fixture",
                "others-steeljoint",
                "--views",
                "1",
                "--run-id",
                "DW-L2-ENV-TEST",
            ],
            check=True,
            capture_output=True,
            text=True,
        )
        probe_json = json.loads(completed.stdout)

    registry = {
        item["key"]: item for item in probe_json["autocadEnvironment"]["registry"]
    }
    failures: list[str] = []
    for json_key, ps_key in AUTO_CAD_REGISTRY_KEYS:
        expected = powershell_registry_probe(ps_key)
        actual = registry[json_key]
        if actual["exists"] != expected["exists"]:
            failures.append(
                f"{json_key}: exists probe mismatch; "
                f"probe={actual['exists']} powershell={expected['exists']}"
            )
        if actual["childGroupCount"] != expected["childGroupCount"]:
            failures.append(
                f"{json_key}: childGroupCount mismatch; "
                f"probe={actual['childGroupCount']} powershell={expected['childGroupCount']}"
            )

    if failures:
        print("\n".join(failures))
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
