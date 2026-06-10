# M2-RebarCreate-015 线配筋命令成功 Dirty / Transaction 状态 P0 实现记录

todoId = TODO-085
phase = M2-RebarCreate-015
evidenceIds = E-IDA-049, E-DEV-106, E-DEV-107

## 目标

本轮只完成 `TODO-085 / 线配筋命令成功 dirty/transaction 状态 P0`。

目标不是复刻旧图石完整 `sub_1405E49D0` dirty / undo / save 体系，而是在 TODO-084 已确认旧 dirty 调用位置后，给正式 app 补一个最小可测试状态边界：

```text
Rebar.Create.LineGroup 成功创建
  -> app-level ProjectDirty + RebarDirty + DrawingDirty
  -> 记录 dirty transaction commandKey
  -> 记录 E-IDA-049 作为旧 dirty call position 证据
  -> 保留 GAP-REB-C-002 作为旧 dirty parity 未闭合缺口

失败 / 取消 / geometry failure / normalizer failure / mutation-gap false applied
  -> 不提交 transaction
  -> 不污染 dirty flags
  -> 不污染 SteelData
```

能力等级固定为：

```text
p0-app-dirty-transaction
```

## 代码变更

```text
app/src/command/CommandRegistry.h
  -> 新增 CommandDirtyFlags
  -> 新增 CommandTransactionState
  -> CommandResult 带 dirtyFlags 和 transaction

app/src/command/RebarLineGroupCommandHandler.cpp
  -> Rebar.Create.LineGroup 成功后返回 ProjectDirty + RebarDirty + DrawingDirty
  -> transaction.commandKey = Rebar.Create.LineGroup
  -> transaction.legacyDirtyEvidenceId = E-IDA-049
  -> transaction.unresolvedDirtyParityGap = GAP-REB-C-002
  -> 失败路径保持默认 no dirty / no transaction

app/src/app/MainWindow.h / MainWindow.cpp
  -> 新增 DirtyState
  -> 线配筋命令 Completed 后应用 CommandResult dirty flags
  -> 取消、预检失败、命令失败不调用 dirty apply

app/tests/unit/rebar_line_group_command_handler_tests.cpp
  -> 成功路径断言 dirty flags / transaction / evidence / gap
  -> 空选择、错类型、geometry failure、normalizer failure、mutation-gap failure 断言 no dirty transaction

app/tests/integration/line_group_display_smoke_tests.cpp
  -> 真实 123.stp UI 链路断言 no selection / wrong type / cancel 不 dirty
  -> 成功创建后 ProjectDirty + RebarDirty + DrawingDirty
  -> 第二次成功创建累计 transaction count
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
修改旧 UI 文案
实现旧 sub_1405E49D0 完整等价
实现旧 undo / save / close prompt parity
实现保存成功清 dirty
实现保存失败保持 dirty 的 app 保存入口
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
让 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## TDD 记录

```text
rebar_line_group_command_handler_tests
  -> red: CommandResult 不含 dirty / transaction 字段
  -> green: 成功创建返回 dirty flags 和 transaction evidence
  -> green: 失败路径全部保持 no dirty transaction

line_group_display_smoke_123
  -> red: MainWindow 无 app dirty inspection 状态
  -> green: no selection / wrong type / cancel 不 dirty
  -> green: 成功创建后 dirty，第二次成功累计 transaction count
```

## 仍未闭合

```text
旧 sub_1405E49D0 dirty / undo / save 完整语义
旧图石关闭工程 / 保存提示运行确认
旧 undo stack 与 transaction rollback
保存成功后 dirty 清除
保存失败后 dirty 保持
线配筋 / 弧形组 golden 对照
```

## 验证

最终验证记录见：

```text
docs/phase1/app_build_reports/m2_rebar_create_015_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_015_run_001.json
```

## 下一步

建议下一轮执行：

```text
TODO-086 / 线配筋 dirty 状态与保存清除入口准备 P0
```

下一轮只做保存清除 dirty 的 P0 准备，不声明旧图石完整 dirty / undo / save parity。
