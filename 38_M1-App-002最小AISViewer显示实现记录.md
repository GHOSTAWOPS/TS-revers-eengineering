# M1-App-002 最小 AIS Viewer 显示实现记录

## 目标

本文件记录 M1-App-002 的实现边界和实际结果。

目标是：

```text
在正式 app 中接入最小 OCCT AIS Viewer。
导入 123.stp 后能把模型显示到 Viewer。
支持 FitAll。
保留基础选择模式状态。
继续禁止钢筋业务层直接依赖 OCCT。
```

本阶段不是钢筋业务复刻。

## 架构边界

新增模块：

```text
app/src/presentation/occ/OccViewerWidget.h
app/src/presentation/occ/OccViewerWidget.cpp
app/src/geometry/occ/import/OcctImportedDocument.h
```

职责：

- `OcctStepImportService`：读取 STEP，返回纯摘要和 OCCT 私有显示 shape。
- `OcctImportedDocument`：只在 `geometry/occ` / `presentation/occ` 使用，保存 `TopoDS_Shape`。
- `OccViewerWidget`：初始化 AIS viewer、显示 shape、FitAll、保存 selection mode。
- `MainWindow`：通过 `LegacyUiCommandMap -> CommandRegistry` 调用导入和 FitAll。

禁止：

```text
domain/rebar include OcctImportedDocument
domain/rebar include OccViewerWidget
domain/rebar include TopoDS_ / AIS_ / BRep*
Viewer 里出现 RebarCreationCommandService / EdgeToRebarFactory / FaceRebarGenerator
```

## 实现内容

完成项：

- `tsrebar_occ_viewer` 静态库。
- `OccViewerWidget` 最小 AIS 初始化。
- `displayDocument()` 显示导入的 STEP free shapes。
- `fitAll()`。
- `setSelectionMode()` / `selectionMode()` 基础状态。
- `MainWindow` 中心区域替换为 `OccViewerWidget`。
- `导入 STEP` 命令导入后显示模型并刷新模型树。
- `全显` 命令走 `CommandRegistry` 调用 viewer。
- `viewer_smoke_tests` 覆盖 viewer 懒初始化和选择模式状态。
- `tsrebar_app --smoke-display-step <file>` 覆盖真实导入 + AIS 显示 + FitAll 后退出。

未完成项：

- 鼠标点选 face / edge / vertex。
- 稳定 `SelectionRef` 生成。
- 选择高亮。
- 右键菜单。
- 钢筋显示。
- 钢筋反向拾取。

这些进入 M1-App-003。

## 实际验证

构建：

```text
success
```

CTest：

```text
5 / 5 passed
```

通过项：

- `command_registry_tests`
- `app_smoke`
- `viewer_smoke_tests`
- `domain_rebar_boundary`
- `step_import_probe_123`

真实 Viewer 显示 smoke：

```powershell
build\tsrebar_app.exe --smoke-display-step "..\..\123.stp"
```

结果：

```text
exit code = 0
```

这表示：

```text
123.stp 导入成功。
OccViewerWidget 初始化成功。
displayDocument 成功。
FitAll 成功。
```

Readiness gate：

```json
{
  "decision": "M1-Formal-Ready",
  "m1FormalAllowed": true,
  "errorCount": 0,
  "warningCount": 0,
  "passCount": 78,
  "checkCount": 78
}
```

## 123.stp 统计

沿用 M1-App-001 的真实导入统计：

```text
solids=754
faces=3016
edges=9048
vertices=18096
length_unit=METRE
```

## 当前结论

```text
M1-App-002 = pass
```

正式 app 已经从“能导入统计 STEP”推进到“能用 OCCT AIS 显示 STEP”。

下一步：

```text
M1-App-003：face / edge / vertex 选择、SelectionRef 稳定 ID、选择面板摘要。
状态：已完成，见 39_M1-App-003选择系统实现记录.md。
```
