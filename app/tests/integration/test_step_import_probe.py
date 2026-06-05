#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys


def parse_probe_output(output: str) -> dict[str, str]:
    values: dict[str, str] = {}
    for line in output.splitlines():
        if "=" not in line:
            continue
        key, value = line.split("=", 1)
        values[key.strip()] = value.strip()
    return values


def main() -> int:
    if len(sys.argv) != 3:
        print("usage: test_step_import_probe.py <probe.exe> <sample.stp>", file=sys.stderr)
        return 2

    probe = Path(sys.argv[1])
    sample = Path(sys.argv[2])
    result = subprocess.run(
        [str(probe), str(sample)],
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )

    if result.returncode != 0:
        print(result.stdout, file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        return result.returncode

    values = parse_probe_output(result.stdout)
    expected = {
        "ok": "true",
        "read_ok": "true",
        "transfer_ok": "true",
        "solids": "754",
        "faces": "3016",
        "length_unit": "METRE",
    }

    errors: list[str] = []
    for key, expected_value in expected.items():
        actual = values.get(key)
        if actual != expected_value:
            errors.append(f"{key}: expected {expected_value}, got {actual}")

    if errors:
        print(result.stdout, file=sys.stderr)
        print("\n".join(errors), file=sys.stderr)
        return 1

    print("step_import_probe_123 ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

