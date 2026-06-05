# Dependencies

## Version Freeze

- Project: 图石钢筋 1:1 复刻
- Date: 2026-06-05
- Phase: M1 dependency freeze from local vcpkg candidate
- Freeze status: frozen-local-vcpkg
- Compiler: MSVC toolset 14.50.35717 from Visual Studio 2026 Community
- CMake: Visual Studio bundled CMake 4.2.3.0
- Build generator: Visual Studio bundled Ninja
- Target OS: Windows x64
- Runtime package type: desktop dynamic runtime package
- Package manager: vcpkg 2026-04-08-e0612b42ce44e55a0e630f2ee9d3c533a63d8bc1
- vcpkg root: `D:\Work\vcpkg`
- vcpkg triplet: `x64-windows`

This file freezes the local Phase 1 dependency candidate. It does not allow
linking every module installed by vcpkg. The actual Qt/OCCT link set must still
be checked by CMake and `Build.DependencyGate`.

## Qt

- Package: `qtbase:x64-windows`
- Version: `6.10.3#1`
- Source: vcpkg port `qtbase`
- Source archive: `D:\Work\vcpkg\downloads\qtbase-everywhere-src-6.10.3.tar.xz`
- Install root: `D:\Work\vcpkg\installed\x64-windows`
- CMake config: `D:\Work\vcpkg\installed\x64-windows\share\Qt6\Qt6Config.cmake`
- License route: open-source Qt Base dynamic linking route; external distribution still requires legal review of the copied Qt notices
- Linking: dynamic
- Modified source: no
- Distributed runtime: local Phase 1 package may include only the approved runtime DLL subset below

### Qt Modules

Approved M1 link modules:

- Core
- Gui
- Widgets
- OpenGL
- OpenGLWidgets

Test-only modules:

- Test

Installed but requiring review before project linking:

- Concurrent
- DBus
- Network
- PrintSupport
- Sql
- Xml

Not installed in the frozen candidate:

- Svg

Forbidden modules:

- Canvas Painter
- CoAP
- Graphs
- GRPC
- HTTP Server
- Lottie Animation
- MQTT
- Network Authorization
- Qml Compiler
- Quick
- Quick 3D
- Quick 3D Physics
- Quick Controls
- Quick Timeline
- Virtual Keyboard
- Wayland Compositor
- WebEngine
- Marketplace components

Approved Phase 1 Qt runtime DLL subset:

- `Qt6Core.dll`
- `Qt6Gui.dll`
- `Qt6Widgets.dll`
- `Qt6OpenGL.dll`
- `Qt6OpenGLWidgets.dll`
- `Qt6Test.dll` for tests only

## OCCT

- Package: `opencascade:x64-windows`
- Version: `7.9.3#1`
- Source: vcpkg port `opencascade`
- Source archive: `D:\Work\vcpkg\downloads\Open-Cascade-SAS-OCCT-V7_9_3.tar.gz`
- Install root: `D:\Work\vcpkg\installed\x64-windows`
- CMake config: `D:\Work\vcpkg\installed\x64-windows\share\opencascade\OpenCASCADEConfig.cmake`
- License: LGPL 2.1 with Open CASCADE exception, as recorded by the installed vcpkg copyright file
- Linking: dynamic
- Modified source: no
- Distributed runtime: local Phase 1 package may include the approved toolkit subset needed by the actual CMake targets

Approved capability groups:

- Foundation / Kernel
- Modeling Data
- Modeling Algorithms
- Shape Healing
- Data Exchange: STEP
- Visualization / AIS
- HLR / section projection support

Installed optional features observed:

- FreeType
- FreeImage
- TBB

Capability groups requiring review:

- IGES
- STL / OBJ / glTF
- OCAF

Forbidden commercial components:

- ACIS Import-Export
- Parasolid Import-Export
- DXF Import-Export
- IFC Import-Export
- JT Import-Export
- Express Mesh
- Commercial Collision Detection

Installed OCCT runtime DLLs are recorded by:

- `docs/phase1/dependency_probe_run_001.md`
- `docs/phase1/dependency_probe_run_001.json`

## CAD Boundary

- AutoCAD distributed with this project: no
- Old 图石 AutoCAD plugin distributed with this project: no
- New program writes DWG/DXF directly: no
- Detail package writer enabled: yes, as XML package output only
- Commercial CAD is treated as the user's external runtime boundary

## Dependency Table

| Name | Version | Source | License | Used By | Runtime Distributed | Linking | Modified Source | Evidence |
|---|---|---|---|---|---|---|---|---|
| Qt Base | 6.10.3#1 | vcpkg `qtbase` + local source archive | Qt open-source route, legal review before external distribution | ui/app/tests | approved subset only | dynamic | no | E-TECH-001/E-TECH-002/E-TECH-003/E-TECH-004/E-TECH-005 |
| Open CASCADE Technology | 7.9.3#1 | vcpkg `opencascade` + local source archive | LGPL-2.1 with OCCT exception | geometry/viewer/STEP | approved subset only | dynamic | no | E-TECH-001/E-TECH-002/E-TECH-003/E-TECH-004/E-TECH-005 |

## Gate Decision

- Current dependency candidate: frozen from local vcpkg
- Formal Phase 1 rule: `Build.DependencyGate` must pass after this file, notice
  files, license records, source availability records and SBOM are synchronized.
- CMake rule: the final project must not link installed-but-unapproved Qt
  modules or commercial OCCT components.
