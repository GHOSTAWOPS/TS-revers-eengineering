#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys
import unittest


class DomainRebarBoundaryTests(unittest.TestCase):
    def test_real_domain_rebar_has_no_occt_tokens(self):
        app_root = Path(__file__).resolve().parents[2]
        script = app_root / "tools" / "check_domain_rebar_boundary.py"
        domain = app_root / "src" / "domain" / "rebar"

        result = subprocess.run(
            [sys.executable, str(script), str(domain)],
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )

        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == "__main__":
    unittest.main()

