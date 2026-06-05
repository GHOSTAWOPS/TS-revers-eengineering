# Third Party Notices

This product includes third-party open-source components.

This notice file records the local M1 dependency candidate frozen from vcpkg.
External distribution still requires copying the full upstream/vcpkg notice
files into the release package.

## Qt

- Component: Qt Base
- Package: `qtbase:x64-windows`
- Version: `6.10.3#1`
- Project URL: https://www.qt.io/
- Source route: `D:\Work\vcpkg\downloads\qtbase-everywhere-src-6.10.3.tar.xz`
- License route: open-source Qt Base dynamic linking route; legal review before external distribution
- License files:
  - licenses/QT_LICENSES.txt
- Linking: dynamic
- Source modified: no

## Open CASCADE Technology

- Component: Open CASCADE Technology
- Package: `opencascade:x64-windows`
- Version: `7.9.3#1`
- Project URL: https://dev.opencascade.org/
- Source route: `D:\Work\vcpkg\downloads\Open-Cascade-SAS-OCCT-V7_9_3.tar.gz`
- License: LGPL 2.1 with Open CASCADE exception, as recorded by the installed vcpkg copyright file
- License files:
  - licenses/OCCT_LICENSE.txt
- Linking: dynamic
- Source modified: no

## Compiler / Runtime Libraries

- Component: MSVC runtime from Visual Studio 2026 Community
- Version: MSVC toolset 14.50.35717
- License: Microsoft runtime redistribution terms apply
- Distributed files: to be listed by the first real CMake/package run

## Project Notes

- AutoCAD is not distributed with this package.
- Old 图石 AutoCAD plugin is not distributed as a base runtime of this package.
- Detail XML package output is a compatibility output, not a bundled CAD runtime.
- Commercial CAD owned by the user is treated as an external runtime boundary,
  not as a dependency bundled by this reproduction project.
