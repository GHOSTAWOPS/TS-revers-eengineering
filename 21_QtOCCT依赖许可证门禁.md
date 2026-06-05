# Qt / OCCT 依赖许可证门禁

## 目标

本文件回答一个开发前必须冻结的问题：

```text
Qt6 + OCCT 可以用，但不能随手加模块、随手静态链接、随手混入商业 SDK。
```

本文件不是法律意见。它是工程门禁，用于约束一期开发、构建和发布包。

## 当前结论

一期建议路线：

```text
Qt6 基础模块
  + OCCT 开源模块
  + 旧图石证据驱动的钢筋复刻层
  + Detail XML writer
```

默认策略：

- Qt 和 OCCT 使用动态链接。
- 不修改 Qt / OCCT 源码。
- 不引入 GPL-only Qt 模块。
- 不引入 OCCT 商业组件。
- 不把 CAD 商业版或旧插件打进新程序基础版发布包。

## 官方来源

Qt：

- `https://doc.qt.io/qt-6/licensing.html`
- `https://www.qt.io/licensing/open-source-lgpl-obligations`
- `https://doc.qt.io/qt-6/licenses-used-in-qt.html`
- `https://doc.qt.io/qt-6/sbom.html`

OCCT：

- `https://dev.opencascade.org/resources/licensing`
- `https://dev.opencascade.org/resources/faq`
- `https://dev.opencascade.org/resources/components`
- `https://dev.opencascade.org/doc/overview/html/index.html`

已确认口径：

- Qt 官方许可页说明 Qt 可按商业许可、LGPLv3 或 GPLv3 使用，且部分模块对开源用户仅 GPL。
- Qt 官方许可页列出 GPL-only 模块，如 `Qt Graphs`、`Qt GRPC`、`Qt HTTP Server`、`Qt MQTT`、`Qt Qml Compiler`、`Qt Quick 3D`、`Qt Virtual Keyboard` 等。
- OCCT 官方许可页说明 Open CASCADE Technology 6.7.0 及以后使用 LGPL 2.1 with additional exception。
- OCCT 官方 components 页列出若干商业组件，不能把它们等同于开源 OCCT。

## 一期允许清单

### Qt 模块

首期允许：

- `Qt Core`
- `Qt GUI`
- `Qt Widgets`
- `Qt OpenGL`
- `Qt OpenGLWidgets`
- `Qt SVG`
- `Qt Test`，仅测试目标使用

按需审查后可用：

- `Qt Network`：只有确有网络功能时才引入。
- `Qt XML`：如果 Detail writer 不用其他 XML 库，可用于 XML 写出。
- `Qt Qml / Qt Quick / Qt Quick Controls`：官方有 LGPL 口径，但一期不建议使用，除非明确放弃纯 Widgets 路线。

一期推荐界面路线：

```text
Qt Widgets + 停靠面板 + 工具栏/Ribbon 类封装 + OCCT Viewer
```

原因：

- 旧图石是复杂桌面工程软件，不是移动端或轻交互页面。
- 参数窗口、对象树、属性表、状态栏更适合 Widgets。
- 可以减少 QML 编译、Quick 3D 等许可证和技术复杂度。

### OCCT 模块

首期允许使用 OCCT 开源能力：

- 基础内核和拓扑数据。
- BRep 建模和拓扑遍历。
- Modeling Algorithms。
- Shape Healing。
- STEP 导入。
- IGES 导入，按需。
- STL / OBJ / glTF，按需。
- Visualization / AIS / 3D Viewer。
- OCAF，若后续决定使用它管理文档、属性或撤销重做。

典型用途：

- STEP/STP 导入。
- 面、边、点拓扑遍历。
- 曲线采样。
- 距离计算。
- 剖切、投影、隐藏线基础。
- 视图选择、高亮、显示模式。

## 禁止或强审查清单

### Qt 禁止直接引入

以下模块对开源用户属于 GPL-only 或合规风险高，禁止进入一期基础版：

- `Qt Canvas Painter`
- `Qt CoAP`
- `Qt Graphs`
- `Qt GRPC`
- `Qt HTTP Server`
- `Qt Lottie Animation`
- `Qt MQTT`
- `Qt Network Authorization`
- `Qt Qml Compiler`
- `Qt Quick 3D`
- `Qt Quick 3D Physics`
- `Qt Quick Timeline`
- `Qt Virtual Keyboard`
- `Qt Wayland Compositor`

以下模块需单独审查：

- `Qt WebEngine`：Chromium 依赖重，notice 和发布材料复杂。
- `Qt Charts`：Qt 6.10 起已 deprecated，且许可证风险不适合一期。
- 任何 Marketplace 组件。
- 任何只以二进制形式提供、license 文件不清的 Qt 插件。

### OCCT 禁止直接引入

禁止把 OCCT 商业组件当作开源 OCCT 使用：

- DXF Import-Export。
- ACIS Import-Export。
- Parasolid Import-Export。
- IFC Import-Export。
- JT Import-Export。
- Express Mesh。
- 商业 Collision Detection 组件。
- 其他 Open Cascade 商业组件。

原因：

```text
用户只有 CAD 商业版。
我们不能把新的商业几何/交换组件混进最快复刻路线。
```

如果未来确实需要这些能力，必须作为单独商业功能门：

- 独立 CMake option。
- 独立授权说明。
- 独立发布包。
- 独立验收。

## 动态链接要求

默认要求：

- Qt 动态链接。
- OCCT 动态链接。
- 不把 Qt / OCCT 静态链接进不可替换的单体 exe。

原因：

- LGPL 场景下，用户需要能替换 LGPL 动态库。
- 动态链接降低发布合规复杂度。
- 便于后续升级和安全修复。

禁止：

- 未审查就启用静态 Qt。
- 未审查就把 OCCT 静态链接进闭源 exe。
- 修改 Qt / OCCT 源码后不记录补丁。

## 第三方 notice 和 SBOM

发布包必须包含：

```text
licenses/
  QT_LICENSES.txt
  OCCT_LICENSE.txt
  THIRD_PARTY_NOTICES.txt
  SOURCE_OFFER.txt
sbom/
  qt-*.spdx.json
  project-sbom.spdx.json
```

至少记录：

- 依赖名称。
- 版本。
- 下载来源。
- 许可证。
- 用途。
- 是否随包分发。
- 是否修改源码。
- 是否动态链接。

Qt 6.8 以后可以利用 Qt 提供的 SBOM 材料，发布包应尽量保留。

## 源码修改规则

首期原则：

```text
不修改 Qt。
不修改 OCCT。
```

如果必须修改：

- 先开 `GAP-TECH`。
- 记录修改原因。
- 记录修改文件和补丁。
- 记录修改日期。
- 准备对应库源码或补丁发布材料。
- 重新审查发布包。

## CMake 门禁

每次新增 Qt 组件，必须更新依赖清单。

示例：

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
```

禁止未经审查出现：

```cmake
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
find_package(Qt6 REQUIRED COMPONENTS Quick3D)
find_package(Qt6 REQUIRED COMPONENTS QmlCompiler)
find_package(Qt6 REQUIRED COMPONENTS HttpServer)
```

每次新增 OCCT toolkit，也必须记录用途和许可证来源。

建议首期按能力分组记录：

```text
OCCT_KERNEL     -> 基础数据和拓扑。
OCCT_MODELING   -> BRep / 几何算法。
OCCT_DATAEX     -> STEP。
OCCT_VISUAL     -> AIS / OpenGL Viewer。
```

具体 toolkit 名称以实际 OCCT 版本和 CMake 配置为准，不在文档里提前写死。

## CAD 商业版隔离

用户已有 CAD 商业版。

一期边界：

- 新系统生成 `Detail.xml + DetailNN.stl` XML 包。
- 用户在已有 AutoCAD 2020 + 旧插件环境导入。
- 新系统不分发 AutoCAD。
- 新系统不把旧插件作为自身基础运行时。
- 新系统不直接写 DWG / DXF。

如果未来做 CAD 自动化：

- 独立模块名。
- 独立开关。
- 独立授权说明。
- 不影响无 CAD 环境下的核心建模、配筋、保存和 Detail 包生成。

## 开发前检查表

进入 Qt6 + OCCT 工程初始化前必须确认：

- Qt 版本已固定。
- OCCT 版本已固定，且为 6.7.0 或以后。
- Qt 模块只包含允许清单。
- OCCT 只使用开源组件。
- 链接方式为动态链接。
- 依赖清单文件已创建。
- notice / license 目录方案已创建。
- 不引入新的商业 SDK。
- 不引入 GPL-only Qt 模块。
- CAD / AutoCAD 插件能力和基础版隔离。

## 新增依赖审查表

后续每加一个依赖，按此模板记录：

```text
Dependency:
Version:
Source:
License:
Purpose:
Runtime distributed: yes or no
Linking: dynamic/static/header-only
Modified source: yes or no
Used by:
Allowed phase:
Review result:
Evidence:
```

## 和复刻路线的关系

本文件只管依赖边界，不改变 1 比 1 复刻目标。

继续成立：

```text
旧图石功能操作和业务结果要 1 比 1。
Qt6 / OCCT 只是新底座。
钢筋业务规则仍靠 IDA + 运行确认 + 样本证据复刻。
```

## 仍需关闭的缺口

- `GAP-TECH-001`：实际 Qt 模块清单和发布包清单。
- `GAP-TECH-002`：OCCT 版本、toolkit 清单和 notice。
- `GAP-TECH-004`：OCCT AIS 大模型显示性能。
- `GAP-TECH-006`：CAD 插件分发和授权边界。
- `GAP-TECH-007`：依赖清单、notice 和 SBOM 文件落地。
