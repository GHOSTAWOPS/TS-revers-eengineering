# M1-App-019 Legacy Command Contract P1 实现记录

## 结论

本轮完成 `TODO-019 / M1-App-019`。

本轮只把首批旧图石钢筋命令绑定为可查询、可注册、可追溯的命令契约：

```text
Rebar.Create.LineGroup     -> sgroupbarline
Rebar.Create.ArcGroup      -> sgroupbararc
RebarGroup.TrimByLine      -> 0x8939
RebarGroup.TrimByFace      -> 0x8CE5
```

这些命令当前均为 `NotImplemented` placeholder，不声明线筋、弧筋或裁剪业务算法已经实现。

## 新增能力

`LegacyUiCommandDefinition` 增加：

```text
uiSurface
commandKey
legacyUiPath
legacyCommand
legacyContextCommandId
evidenceIds
sourceRefs
gapIds
inputSelectionTypes
implementationState
interactionStates
```

关键边界：

- Ribbon 命令和右键菜单命令用 `LegacyCommandUiSurface` 区分。
- `MainWindow::buildCommandTabs()` 只渲染 `Ribbon` 命令。
- `RebarGroup.TrimByLine / TrimByFace` 只注册 registry 占位，不出现在 Ribbon。
- `evidenceIds` 只承载 `E-*` 规范证据 ID；文档编号、实现记录号等非证据来源放入 `sourceRefs`。
- placeholder 不覆盖已有真实 handler。
- 未实现命令返回稳定 `CommandStatus::NotImplemented` 和“旧图石行为待确认”提示。

## 证据对齐

命令来源：

```text
01_功能操作矩阵.md
03_IDA命令证据.md
08_开发命令契约.md
17_一期按钮追溯与命令占位矩阵.md
```

已绑定证据：

- `E-IDA-001 / E-IDA-011 / E-IDA-012 / E-IDA-013`：弧形筋入口和公共生成链。
- `E-IDA-002 / E-IDA-010 / E-IDA-012 / E-IDA-013`：线配筋入口和公共生成链。
- `E-CTX-8939`：右键 `与线裁剪` 命令号。
- `E-CTX-8CE5`：右键 `与面裁剪` 命令号。
- `E-DEV-043`：本轮实际工件和验证报告。

## TDD

RED：

```text
cmake --build app\build --target command_registry_tests
result = expected compile failure
failure = LegacyUiCommandDefinition 缺少 uiSurface / implementationState /
          interactionStates；旧裁剪命令未区分 ContextMenu；相关 CommandId
          和 placeholder 注册函数缺失
```

GREEN 窄测试：

```text
cmd.exe /c 'call "D:\Visual Studio 2026\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build app\build --target command_registry_tests && app\build\command_registry_tests.exe'
result = pass
```

## 测试结果

默认 CTest：

```text
9 / 9 passed
Total Test time = 123.53 sec
```

测试覆盖：

- `Rebar.Create.LineGroup` / `Rebar.Create.ArcGroup` 契约 metadata。
- `RebarGroup.TrimByLine` / `RebarGroup.TrimByFace` 右键命令 metadata。
- 右键裁剪命令为 `ContextMenu`，不携带 Ribbon tab/group。
- 未实现命令注册稳定 `NotImplemented` placeholder。
- placeholder 不覆盖已有真实 handler。
- `evidenceIds` 全量规范化为 `E-*`。

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

`domain/rebar` 边界扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
result = no matches
```

## xhigh 只读 review

第一轮 review：

```text
Critical = none
Important =
  1. 右键裁剪命令被放入 tab_rebar/group_rebar_edit，会渲染成 Ribbon 按钮。
  2. handlerState 命名不准确，缺少显式命令交互状态机字段。
  3. evidenceIds 混入未登记 E-CTX 和文件名。
Minor =
  placeholder 不覆盖已有真实 handler 缺少测试。
Verdict = block
```

处理结果：

- 新增 `LegacyCommandUiSurface`，并让 `buildCommandTabs()` 只渲染 Ribbon。
- 右键裁剪改为 `ContextMenu`，只注册占位，不进 Ribbon。
- `LegacyCommandHandlerState` 改为 `LegacyCommandImplementationState`。
- 新增 `LegacyCommandInteractionState` 契约字段。
- `evidenceIds` 只保留规范 ID；`E-CTX-8939 / E-CTX-8CE5` 已在追溯矩阵登记。
- 补 `placeholder must not override real handler` 回归测试。

第二轮 review：

```text
Critical = none
Important =
  - 普通命令仍把 `17 / 36 / M1-App-003` 放进 evidenceIds，
    不符合规范 Evidence ID 形态。
Route Drift = none
Verdict = block
```

处理结果：

- 新增 `LegacyUiCommandDefinition.sourceRefs`，用于承载 `17 / 36 / M1-App-003` 等文档或实现来源引用。
- `evidenceIds` 只保留 `E-*` 规范证据 ID。
- `ViewSelectFace / ViewSelectEdge / ViewSelectVertex` 的证据改为 `E-DEV-027`，`M1-App-003` 只保留为 source ref。
- 补 `testLegacyEvidenceIdsAreNormalized()`，遍历所有命令断言 `evidenceIds` 必须以 `E-` 开头。

第二轮 block 已由主流程修复，并重新通过窄测试、默认 CTest、readiness gate、domain/rebar 边界扫描和 `git diff --check`。

最终复审：

```text
Critical = none
Important = none
Minor =
  - sourceRefs 回归测试只显式断言 ImportStep.sourceRefs == 36，
    未逐项断言 17 / M1-App-003。
  - E-STR-线配筋 / E-STR-扇形筋 符合 E-* 形态，但追溯矩阵索引尚未单独登记。
Route Drift = none
Verdict = allow_commit
```

最终 xhigh 子代理已关闭。

## 修改范围

```text
app/src/app/MainWindow.cpp
app/src/command/CommandId.h
app/src/command/LegacyUiCommandMap.h
app/src/command/LegacyUiCommandMap.cpp
app/tests/unit/command_registry_tests.cpp
```

## 边界

本轮没有实现线筋、弧筋、面筋或裁剪业务算法。

本轮没有迁移父目录钢筋业务主链。

本轮没有实现 Detail writer、AIS 钢筋显示或新工程格式 runtime。

本轮没有让 `domain/rebar` 依赖 OCCT / AIS。

## 下一步建议

下一阶段建议优先做：

```text
TODO-020：IDA MCP 旧线筋 / 弧筋链补证据
```

原因：

```text
命令入口已经可追溯。进入线筋 / 弧筋创建算法前，需要继续用 IDA MCP
闭合 sgroupbarline / sgroupbararc / sub_1405D5670 的关键调用链、
参数语义、字段和常量，避免把 placeholder 直接写成猜测算法。
```
