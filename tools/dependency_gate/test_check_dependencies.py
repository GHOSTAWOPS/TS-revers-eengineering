import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
GATE_PATH = REPO_ROOT / "tools" / "dependency_gate" / "check_dependencies.py"


def load_gate():
    spec = importlib.util.spec_from_file_location("check_dependencies", GATE_PATH)
    module = importlib.util.module_from_spec(spec)
    sys.modules["check_dependencies"] = module
    spec.loader.exec_module(module)
    return module


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


class DependencyGateTests(unittest.TestCase):
    def test_current_workspace_dependency_candidate_is_frozen(self):
        gate = load_gate()
        root = gate.root_dir()
        findings = gate.collect_findings(
            root=root,
            cmake_path=root / "CMakeLists.txt",
            dependencies_path=root / "DEPENDENCIES.md",
            notices_path=root / "THIRD_PARTY_NOTICES.md",
            licenses_dir=root / "licenses",
            sbom_path=root / "sbom" / "project-sbom.spdx.json",
        )
        summary = gate.summarize(findings)

        self.assertEqual("pass", summary["decision"])
        self.assertTrue(summary["m1FormalAllowed"])
        self.assertNotIn("GAP-TECH-007", summary["blockerGaps"])
        self.assertTrue(
            all(
                finding.ok
                for finding in findings
                if finding.severity == "error"
            )
        )

    def test_concrete_dependency_fixture_passes_without_forbidden_cmake(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            write(
                root / "DEPENDENCIES.md",
                """# Dependencies

## Version Freeze

- Project: fixture
- Date: 2026-06-05
- Freeze status: frozen
- Compiler: MSVC 19.40
- CMake: 3.29.0

## Qt

- Version: 6.8.3
- Source: Official Qt distribution
- License route: commercial
- Linking: dynamic
- Modified source: no
- Distributed runtime: yes

## OCCT

- Version: 7.8.1
- Source: Official OCCT distribution
- License: LGPL 2.1 with additional exception
- Linking: dynamic
- Modified source: no
- Distributed runtime: yes
""",
            )
            write(
                root / "THIRD_PARTY_NOTICES.md",
                """# Third Party Notices

## Qt

- Version: 6.8.3
- Source modified: no

## Open CASCADE Technology

- Version: 7.8.1
- Source modified: no
""",
            )
            write(
                root / "licenses" / "QT_LICENSES.txt",
                "Qt Version: 6.8.3\nSource modified: no\nRuntime DLLs: Qt6Core.dll\n",
            )
            write(
                root / "licenses" / "OCCT_LICENSE.txt",
                "OCCT Version: 7.8.1\nSource modified: no\nCommercial components used: no\n",
            )
            write(
                root / "licenses" / "SOURCE_OFFER.txt",
                "Qt source availability: official source archive\nOCCT source availability: official source archive\n",
            )
            write(
                root / "sbom" / "project-sbom.spdx.json",
                json.dumps(
                    {
                        "spdxVersion": "SPDX-2.3",
                        "packages": [
                            {"name": "Qt", "versionInfo": "6.8.3"},
                            {"name": "Open CASCADE Technology", "versionInfo": "7.8.1"},
                        ],
                    }
                ),
            )
            write(
                root / "CMakeLists.txt",
                "find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)\n",
            )

            findings = gate.collect_findings(
                root=root,
                cmake_path=root / "CMakeLists.txt",
                dependencies_path=root / "DEPENDENCIES.md",
                notices_path=root / "THIRD_PARTY_NOTICES.md",
                licenses_dir=root / "licenses",
                sbom_path=root / "sbom" / "project-sbom.spdx.json",
            )
            summary = gate.summarize(findings)

            self.assertEqual("pass", summary["decision"])
            self.assertTrue(summary["m1FormalAllowed"])

    def test_forbidden_cmake_keyword_blocks_gate(self):
        gate = load_gate()
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            write(root / "DEPENDENCIES.md", "Qt 6.8.3\nOCCT 7.8.1\nSource modified: no\n")
            write(root / "THIRD_PARTY_NOTICES.md", "Source modified: no\nQt 6.8.3\nOCCT 7.8.1\n")
            write(root / "licenses" / "QT_LICENSES.txt", "Qt Version: 6.8.3\nSource modified: no\n")
            write(root / "licenses" / "OCCT_LICENSE.txt", "OCCT Version: 7.8.1\nSource modified: no\n")
            write(root / "licenses" / "SOURCE_OFFER.txt", "Qt source availability: official\nOCCT source availability: official\n")
            write(
                root / "sbom" / "project-sbom.spdx.json",
                json.dumps(
                    {
                        "packages": [
                            {"name": "Qt", "versionInfo": "6.8.3"},
                            {"name": "Open CASCADE Technology", "versionInfo": "7.8.1"},
                        ]
                    }
                ),
            )
            write(root / "CMakeLists.txt", "find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)\n")

            findings = gate.collect_findings(
                root=root,
                cmake_path=root / "CMakeLists.txt",
                dependencies_path=root / "DEPENDENCIES.md",
                notices_path=root / "THIRD_PARTY_NOTICES.md",
                licenses_dir=root / "licenses",
                sbom_path=root / "sbom" / "project-sbom.spdx.json",
            )

            self.assertEqual("fail", gate.summarize(findings)["decision"])
            self.assertTrue(any(finding.check_id == "CMAKE_FORBIDDEN_WEBENGINEWIDGETS" and not finding.ok for finding in findings))


if __name__ == "__main__":
    unittest.main()
