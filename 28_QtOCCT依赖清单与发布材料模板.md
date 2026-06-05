# Qt / OCCT 依赖清单与发布材料模板

## 目标

本文件把 `21_QtOCCT依赖许可证门禁.md` 和
`27_Qt6_OCCT开发入口门禁与首批工件清单.md` 里的原则，
落成工程初始化时必须创建的文件模板。

它回答：

```text
Qt6 + OCCT 工程一创建，依赖、license、notice、SBOM 应该长什么样？
Build.DependencyGate 应该检查什么？
当前本机 Qt / OCCT 候选依赖是否已经冻结？
GAP-TECH-007 的 M1 阻塞是否仍然存在？
```

本文件关联技术证据：

```text
E-TECH-002
  -> Qt / OCCT 依赖清单与发布材料模板
  -> 来源：28_QtOCCT依赖清单与发布材料模板.md

E-TECH-005
  -> 本机 vcpkg 依赖探测和冻结报告
  -> 来源：
     tools/dependency_gate/probe_local_dependencies.py
     docs/phase1/dependency_probe_run_001.md
     docs/phase1/dependency_probe_run_001.json
```

注意：

- `E-TECH-002` 证明依赖发布材料模板已经明确。
- `E-TECH-005` 证明当前本机 vcpkg 候选依赖已可探测并已用于冻结依赖文件。
- 当前 `Build.DependencyGate` 已经 pass，`GAP-TECH-007` 不再作为 M1 readiness 阻塞。
- 正式 Qt6 + OCCT 工程创建后，仍要继续跑 CMake 模块 allowlist 扫描。

## 官方口径快照

本文件按以下官方页面作为依赖合规来源：

```text
Qt:
  https://doc.qt.io/qt-6/licensing.html
  https://www.qt.io/licensing/open-source-lgpl-obligations
  https://doc.qt.io/qt-6/licenses-used-in-qt.html
  https://doc.qt.io/qt-6/sbom.html

OCCT:
  https://dev.opencascade.org/resources/licensing
  https://dev.opencascade.org/resources/components
  https://dev.opencascade.org/doc/overview/html/index.html
```

当前工程口径：

- Qt6 可以走商业许可、LGPL 或 GPL 路线，但部分模块对开源用户是 GPL-only。
- Qt 6.8 起官方提供 SBOM 相关能力和材料。
- OCCT 6.7.0 及以后是 LGPL 2.1 with additional exception。
- OCCT 商业组件不能当作开源 OCCT 使用。

发布前必须重新核对官方页面。
许可证不是写一次就永远安全，尤其 Qt 模块授权会随版本变化。

## Phase 1 必须落地的文件

正式工程初始化时至少创建：

```text
DEPENDENCIES.md
THIRD_PARTY_NOTICES.md
licenses/
  QT_LICENSES.txt
  OCCT_LICENSE.txt
  SOURCE_OFFER.txt
sbom/
  project-sbom.spdx.json
docs/phase1/
  dependency_gate_run_001.md
```

可选但建议创建：

```text
tools/dependency_gate/
  README.md
  allowlist.json
  denylist.json
```

说明：

- `DEPENDENCIES.md` 记录实际依赖事实。
- `THIRD_PARTY_NOTICES.md` 记录随包分发需要给用户看的 notice。
- `licenses/` 放 Qt、OCCT 和其他运行库的 license 材料。
- `sbom/` 放项目 SBOM。
- `docs/phase1/dependency_gate_run_001.md` 记录第一次门禁运行结果。

## DEPENDENCIES.md 模板

工程初始化时按下面结构创建。

```markdown
# Dependencies

## Version Freeze

- Project:
- Date:
- Compiler:
- CMake:
- Target OS:
- Runtime package type:

## Qt

- Version:
- Source:
- Install root:
- License route: LGPL / Commercial / Other
- Linking: dynamic
- Modified source: no
- Distributed runtime: yes or no

### Qt Modules

Allowed runtime modules:

- Core
- Gui
- Widgets
- OpenGL
- OpenGLWidgets

Test-only modules:

- Test

Modules requiring review:

- Svg
- Network
- Xml
- Qml
- Quick
- QuickControls2

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
- Quick 3D
- Quick 3D Physics
- Quick Timeline
- Virtual Keyboard
- Wayland Compositor
- WebEngine
- Marketplace components

## OCCT

- Version:
- Source:
- Install root:
- License: LGPL 2.1 with additional exception
- Linking: dynamic
- Modified source: no
- Distributed runtime: yes or no

Allowed capability groups:

- Kernel / Foundation
- Modeling Data
- Modeling Algorithms
- Shape Healing
- Data Exchange: STEP
- Visualization / AIS

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

## CAD Boundary

- AutoCAD distributed with this project: no
- Old plugin distributed with this project: no
- New program writes DWG/DXF directly: no
- Detail package writer enabled: yes or no

## Dependency Table

| Name | Version | Source | License | Used By | Runtime Distributed | Linking | Modified Source | Evidence |
|---|---|---|---|---|---|---|---|---|
| Qt | fill version | Official Qt or vcpkg Qt port | fill license route | ui/app | yes or no | dynamic | no | E-TECH-001/E-TECH-002 |
| OCCT | fill version | Official OCCT or vcpkg OCCT port | LGPL-2.1-exception | geometry/viewer | yes or no | dynamic | no | E-TECH-001/E-TECH-002 |
```

门禁要求：

- 版本占位只能出现在 M1-Prep 模板，不能进入实际依赖文件。
- 正式 Phase 1 开工前，Qt 和 OCCT 版本必须填实。
- 新增依赖必须补 `Dependency Table`。
- 新增 Qt 模块必须先过 allowlist / denylist。

## THIRD_PARTY_NOTICES.md 模板

```markdown
# Third Party Notices

This product includes third-party open-source components.

## Qt

- Component: Qt
- Version:
- Project URL:
- License route:
- License files:
  - licenses/QT_LICENSES.txt
- Linking:
- Source modified:

## Open CASCADE Technology

- Component: Open CASCADE Technology
- Version:
- Project URL:
- License:
- License files:
  - licenses/OCCT_LICENSE.txt
- Linking:
- Source modified:

## Compiler / Runtime Libraries

- Component:
- Version:
- License:
- Distributed files:

## Project Notes

- AutoCAD is not distributed with this package.
- Old 图石 AutoCAD plugin is not distributed as a base runtime of this package.
- Detail XML package output is a compatibility output, not a bundled CAD runtime.
```

门禁要求：

- notice 文件必须随发布包一起走。
- 如果 Qt / OCCT 源码未修改，要明确写 `Source modified: no`。
- 如果未来修改源码，必须开新的 `GAP-TECH` 并补源码/patch 分发材料。

## licenses/ 模板

目录结构：

```text
licenses/
  QT_LICENSES.txt
  OCCT_LICENSE.txt
  SOURCE_OFFER.txt
```

`QT_LICENSES.txt` 至少记录：

```text
Qt Version:
Qt Source:
License route:
Modules:
Runtime DLLs:
Official license URL:
Bundled license files copied from Qt install:
Source modified: no
```

`OCCT_LICENSE.txt` 至少记录：

```text
OCCT Version:
OCCT Source:
License:
Toolkit / capability groups:
Runtime DLLs:
Official license URL:
Source modified: no
Commercial components used: no
```

`SOURCE_OFFER.txt` 至少记录：

```text
Qt source availability:
OCCT source availability:
Patch files:
Local modifications:
Contact / internal location:
```

禁止：

- 只写一句“Qt/OCCT 开源”。
- 只放链接，不记录实际版本。
- 只放 license，不记录随包 DLL。

## SBOM 模板

首期可以先用 SPDX JSON 骨架。

`sbom/project-sbom.spdx.json`：

```json
{
  "spdxVersion": "SPDX-2.3",
  "dataLicense": "CC0-1.0",
  "SPDXID": "SPDXRef-DOCUMENT",
  "name": "tushi-rebar-project-sbom",
  "documentNamespace": "https://local.tushi-rebar/sbom/fill-versioned-namespace",
  "creationInfo": {
    "created": "fill-created-time",
    "creators": [
      "Organization: fill-organization",
      "Tool: Build.DependencyGate"
    ]
  },
  "packages": [
    {
      "name": "Qt",
      "SPDXID": "SPDXRef-Package-Qt",
      "versionInfo": "fill-qt-version",
      "downloadLocation": "fill-qt-source-location",
      "licenseConcluded": "NOASSERTION",
      "licenseDeclared": "NOASSERTION",
      "copyrightText": "NOASSERTION"
    },
    {
      "name": "Open CASCADE Technology",
      "SPDXID": "SPDXRef-Package-OCCT",
      "versionInfo": "fill-occt-version",
      "downloadLocation": "fill-occt-source-location",
      "licenseConcluded": "LGPL-2.1-only WITH OCCT-exception-1.0",
      "licenseDeclared": "LGPL-2.1-only WITH OCCT-exception-1.0",
      "copyrightText": "NOASSERTION"
    }
  ],
  "relationships": [
    {
      "spdxElementId": "SPDXRef-DOCUMENT",
      "relationshipType": "DESCRIBES",
      "relatedSpdxElement": "SPDXRef-Package-Qt"
    },
    {
      "spdxElementId": "SPDXRef-DOCUMENT",
      "relationshipType": "DESCRIBES",
      "relatedSpdxElement": "SPDXRef-Package-OCCT"
    }
  ]
}
```

门禁要求：

- 正式 Phase 1 前不得保留版本占位。
- SPDX license 表达式要在发布前由实际工具或人工审查确认。
- 如果 Qt 官方 SBOM 可用，应把 Qt 官方 SBOM 文件保留到发布材料中。

## Build.DependencyGate 契约

建议命令名：

```text
Build.DependencyGate
```

建议命令：

```powershell
python tools/dependency_gate/check_dependencies.py `
  --cmake CMakeLists.txt `
  --dependencies DEPENDENCIES.md `
  --notices THIRD_PARTY_NOTICES.md `
  --licenses licenses `
  --sbom sbom/project-sbom.spdx.json `
  --report docs/phase1/dependency_gate_run_001.md
```

首期可以先手工执行同等检查。
但正式工程里必须逐步变成脚本。

### 输入

- `CMakeLists.txt`
- `DEPENDENCIES.md`
- `THIRD_PARTY_NOTICES.md`
- `licenses/`
- `sbom/project-sbom.spdx.json`

### 输出

- `docs/phase1/dependency_gate_run_001.md`
- 非零退出码表示门禁失败。

### 必查项

```text
Qt:
  - find_package(Qt6 ...) 中组件只能来自允许清单或审查清单。
  - 禁止出现 GPL-only / 高风险模块。
  - Qt Test 只能出现在测试 target。
  - 不能静态链接 Qt，除非另开合规专项。

OCCT:
  - 只允许开源 OCCT 能力组。
  - 禁止出现商业交换组件。
  - 不能静态链接 OCCT，除非另开合规专项。

发布材料:
  - DEPENDENCIES.md 存在。
  - THIRD_PARTY_NOTICES.md 存在。
  - licenses/QT_LICENSES.txt 存在。
  - licenses/OCCT_LICENSE.txt 存在。
  - sbom/project-sbom.spdx.json 存在。
  - Qt / OCCT 版本不为空。
  - 不出现未审查的商业 SDK。
```

### 禁止关键词

依赖门禁脚本至少扫描这些关键词：

```text
QtWebEngine
WebEngineWidgets
QtQuick3D
Quick3D
QmlCompiler
QtHttpServer
HttpServer
QtGraphs
QtGrpc
QtMqtt
QtVirtualKeyboard
ACIS
Parasolid
DXF
JT
IFC
ExpressMesh
```

命中后不一定代表违法，但必须中断门禁，人工确认后才能继续。

## CMake 允许写法

首期推荐：

```cmake
find_package(Qt6 REQUIRED COMPONENTS
  Core
  Gui
  Widgets
  OpenGL
  OpenGLWidgets
)
```

测试目标可单独：

```cmake
find_package(Qt6 REQUIRED COMPONENTS Test)
```

禁止未经审查：

```cmake
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
find_package(Qt6 REQUIRED COMPONENTS Quick3D)
find_package(Qt6 REQUIRED COMPONENTS QmlCompiler)
find_package(Qt6 REQUIRED COMPONENTS HttpServer)
```

OCCT 侧不提前写死 toolkit 名称。
正式工程应按实际 OCCT 版本导出的 CMake 包确定。

但能力边界必须保持：

```text
允许：STEP、BRep、几何算法、AIS Viewer、Shape Healing。
禁止：ACIS、Parasolid、DXF、JT、IFC 等商业交换组件。
```

## dependency_gate_run_001.md 模板

```markdown
# Dependency Gate Run 001

## Summary

- Date:
- Commit:
- Result: pass/fail
- Operator:

## Version Freeze

- Qt:
- OCCT:
- Compiler:
- CMake:

## Checks

- Qt allowlist:
- Qt denylist:
- Qt Test only in tests:
- Qt dynamic linking:
- OCCT open-source capabilities:
- OCCT commercial components absent:
- CAD runtime isolated:
- DEPENDENCIES.md complete:
- THIRD_PARTY_NOTICES.md complete:
- licenses complete:
- SBOM complete:

## Findings

- Finding ID:
- Severity:
- File:
- Evidence:
- Decision:

## Linked Requirements

- REQ-TECH-001
- REQ-TECH-002

## Linked Evidence

- E-TECH-001
- E-TECH-002
- E-TECH-003
- E-TECH-004

## Linked GAP

- GAP-TECH-001
- GAP-TECH-002
- GAP-TECH-007
```

门禁失败时：

- 不允许删除失败报告。
- 不允许把失败项写成通过。
- 失败报告必须回链到 `99_缺口和待确认项.md`。

## 当前 M1-Prep 实际运行

当前已落地：

```text
DEPENDENCIES.md
THIRD_PARTY_NOTICES.md
licenses/QT_LICENSES.txt
licenses/OCCT_LICENSE.txt
licenses/SOURCE_OFFER.txt
sbom/project-sbom.spdx.json
tools/dependency_gate/check_dependencies.py
tools/dependency_gate/test_check_dependencies.py
docs/phase1/dependency_gate_run_001.md
docs/phase1/dependency_gate_run_001.json
```

Run 001 结果：

```text
Result: pass
M1-Formal allowed: yes
Checks: 16 / 17 pass
Errors: 0
Warnings: 1
Blocker GAPs: none
```

当前依赖文件已经用本机 vcpkg 候选版本冻结：

- Qt Base：`6.10.3#1`
- OCCT：`7.9.3#1`
- vcpkg root：`D:\Work\vcpkg`
- triplet：`x64-windows`
- source availability 已写入 `licenses/SOURCE_OFFER.txt`
- SBOM versionInfo / downloadLocation 已填实

剩余 warning 是 Qt6 + OCCT CMake 工程壳还没创建，所以 CMake 模块扫描暂时跳过。

这说明 `E-TECH-003/E-TECH-004/E-TECH-005` 已能关闭当前
`GAP-TECH-007` 的 M1 阻塞；后续正式工程创建后仍必须继续跑
CMake allowlist 扫描。

## GAP-TECH-007 关闭条件

`GAP-TECH-007` 不能因为本文件存在就关闭。

必须同时满足：

1. 正式工程目录中存在实际 `DEPENDENCIES.md`。当前已满足。
2. 正式工程目录中存在实际 `THIRD_PARTY_NOTICES.md`。当前已满足。
3. 正式工程目录中存在实际 `licenses/`。当前已满足。
4. 正式工程目录中存在实际 `sbom/project-sbom.spdx.json`。当前已满足。
5. Qt 版本和 OCCT 版本已填实。当前已满足。
6. 动态链接策略已写入发布材料。当前已满足。
7. `Build.DependencyGate` 有一次 pass 记录。当前已满足。
8. CMake 里 Qt 模块清单与 `DEPENDENCIES.md` 一致。工程创建后继续验证。
9. 未出现禁止 Qt 模块。工程创建后继续验证。
10. 未出现 OCCT 商业组件。工程创建后继续验证。

因此当前 `GAP-TECH-007` 不再阻塞 M1 readiness。

但不能因此写成整个项目 `ReadyForDev / Closed`，因为真实 Qt6 应用
Save/Open 与 binding repair 仍未跑通。

## 和 1 比 1 复刻路线的关系

依赖门禁只解决“用什么底座、怎么合法发布”的问题。

它不负责：

- 反推钢筋生成规则。
- 解释 SFL 私有对象。
- 生成 Detail 字段映射。
- 判断旧图石按钮行为。

继续成立：

```text
Qt6 负责界面底座。
OCCT 负责几何、显示、STEP、剖切、距离、投影。
旧图石钢筋业务规则仍由运行确认、IDA、SFL、Detail 包证据驱动。
新设计文件格式仍必须保存 legacyObject.raw + geometryRef + binding + evidence。
```

## 下一步执行顺序

建议正式工程初始化第一轮按下面顺序执行：

```text
1. 冻结 Qt / OCCT 具体版本。
2. 填实 `DEPENDENCIES.md`、notice、license、source availability 和 SBOM。
3. 创建 CMake 工程壳。
4. 写 Qt / OCCT find_package。
5. 重跑 DependencyGate，直到 `Result: pass`。
6. 把 pass 运行结果写入 `docs/phase1/dependency_gate_run_001.md`。
7. 再进入 Qt6 主窗口和 OCCT Viewer。
```

这样做的目的很简单：

```text
先把依赖边界锁住。
再搭界面和几何底座。
避免开发半个月后发现混进了不能发布的模块。
```
