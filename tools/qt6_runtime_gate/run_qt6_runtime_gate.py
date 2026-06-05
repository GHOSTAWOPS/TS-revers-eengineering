#!/usr/bin/env python3
"""Build and run the real Qt6 C++ runtime gate."""

from __future__ import annotations

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
TOOL_DIR = ROOT / "tools" / "qt6_runtime_gate"
CPP_DIR = TOOL_DIR / "cpp"
STAGE_DIR = Path(tempfile.gettempdir()) / "tsrebar_qt6_runtime_gate"
STAGED_CPP_DIR = STAGE_DIR / "cpp"
BUILD_DIR = STAGE_DIR / "build"
REPORT_DIR = ROOT / "docs" / "phase1" / "app_save_open_reports"

VCPKG_PREFIX = Path("D:/Work/vcpkg/installed/x64-windows")
VS_DEV_CMD = Path("D:/Visual Studio 2026/Community/Common7/Tools/VsDevCmd.bat")
VS_CMAKE = Path("D:/Visual Studio 2026/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe")
VS_NINJA = Path("D:/Visual Studio 2026/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe")


def exe_path() -> Path:
    return BUILD_DIR / "qt6_runtime_gate.exe"


def stage_cpp_sources() -> None:
    if STAGED_CPP_DIR.exists():
        shutil.rmtree(STAGED_CPP_DIR)
    shutil.copytree(CPP_DIR, STAGED_CPP_DIR)


def q(path: Path) -> str:
    return f'"{path}"'


def run_vs_command(args: list[str], *, cwd: Path) -> None:
    if not VS_DEV_CMD.exists():
        raise FileNotFoundError(f"VsDevCmd.bat not found: {VS_DEV_CMD}")
    STAGE_DIR.mkdir(parents=True, exist_ok=True)
    command = " ".join(args)
    script = STAGE_DIR / "run_vs_command.cmd"
    script.write_text(
        "\n".join(
            [
                "@echo off",
                f"call {q(VS_DEV_CMD)} -no_logo -arch=x64 -host_arch=x64",
                "if errorlevel 1 exit /b %ERRORLEVEL%",
                command,
                "exit /b %ERRORLEVEL%",
                "",
            ]
        ),
        encoding="ascii",
    )
    completed = subprocess.run(
        ["cmd", "/d", "/c", str(script)],
        cwd=cwd,
        text=True,
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    if completed.returncode != 0:
        raise RuntimeError(
            "command failed with exit code {code}:\n{command}\n{output}".format(
                code=completed.returncode,
                command=f"{script}\n{command}",
                output=completed.stdout or "<no output>",
            )
        )
    if completed.stdout.strip():
        print(completed.stdout)


def configure() -> None:
    if not VCPKG_PREFIX.exists():
        raise FileNotFoundError(f"vcpkg prefix not found: {VCPKG_PREFIX}")
    if not VS_CMAKE.exists():
        raise FileNotFoundError(f"cmake not found: {VS_CMAKE}")
    if not VS_NINJA.exists():
        raise FileNotFoundError(f"ninja not found: {VS_NINJA}")

    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    run_vs_command(
        [
            q(VS_CMAKE),
            "-S",
            q(STAGED_CPP_DIR),
            "-B",
            q(BUILD_DIR),
            "-G",
            '"Ninja"',
            "-DCMAKE_BUILD_TYPE=Release",
            f"-DCMAKE_PREFIX_PATH={q(VCPKG_PREFIX)}",
            f"-DCMAKE_MAKE_PROGRAM={q(VS_NINJA)}",
        ],
        cwd=ROOT,
    )


def build() -> None:
    run_vs_command([q(VS_CMAKE), "--build", q(BUILD_DIR), "--config", "Release"], cwd=ROOT)


def build_with_cl() -> None:
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    include_root = VCPKG_PREFIX / "include"
    qt_include = include_root / "Qt6"
    qt_lib = VCPKG_PREFIX / "lib"
    source = STAGED_CPP_DIR / "qt6_runtime_gate.cpp"
    exe = exe_path()

    include_args = []
    for include_dir in [
        include_root,
        qt_include,
        qt_include / "QtCore",
        qt_include / "QtGui",
        qt_include / "QtWidgets",
        VCPKG_PREFIX / "share" / "Qt6" / "mkspecs" / "win32-msvc",
    ]:
        include_args.extend(["/I", q(include_dir)])

    command = [
        "cl.exe",
        "/nologo",
        "/std:c++17",
        "/Zc:__cplusplus",
        "/permissive-",
        "/utf-8",
        "/EHsc",
        "/MD",
        "/O2",
        "/DQT_NO_DEBUG",
        "/DQT_CORE_LIB",
        "/DQT_GUI_LIB",
        "/DQT_WIDGETS_LIB",
        *include_args,
        q(source),
        f"/Fe:{q(exe)}",
        f"/Fo:{q(BUILD_DIR / 'qt6_runtime_gate.obj')}",
        "/link",
        f"/LIBPATH:{q(qt_lib)}",
        "Qt6Widgets.lib",
        "Qt6Gui.lib",
        "Qt6Core.lib",
        "user32.lib",
        "gdi32.lib",
        "shell32.lib",
        "ole32.lib",
        "uuid.lib",
        "advapi32.lib",
        "ws2_32.lib",
    ]
    run_vs_command(command, cwd=ROOT)


def run_gate() -> None:
    exe = exe_path()
    if not exe.exists():
        raise FileNotFoundError(f"runtime gate executable not found: {exe}")

    env = os.environ.copy()
    env["PATH"] = str(VCPKG_PREFIX / "bin") + os.pathsep + env.get("PATH", "")
    env["QT_PLUGIN_PATH"] = str(VCPKG_PREFIX / "Qt6" / "plugins")
    env.setdefault("QT_QPA_PLATFORM", "minimal")

    completed = subprocess.run(
        [
            str(exe),
            "--root",
            str(ROOT),
            "--out-dir",
            str(REPORT_DIR),
        ],
        cwd=ROOT,
        env=env,
        text=True,
        encoding="utf-8",
        errors="replace",
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    print(completed.stdout)
    if completed.returncode != 0:
        raise RuntimeError(f"Qt6 runtime gate failed with exit code {completed.returncode}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--skip-configure-if-built", action="store_true")
    args = parser.parse_args()

    try:
        stage_cpp_sources()
        build_with_cl()
        run_gate()
    except Exception as error:  # noqa: BLE001 - command-line gate should print one clear failure.
        print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
