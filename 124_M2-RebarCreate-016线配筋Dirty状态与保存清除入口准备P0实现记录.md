# M2-RebarCreate-016 线配筋 Dirty 状态与保存清除入口准备 P0 实现记录

todoId = TODO-086
phase = M2-RebarCreate-016
evidenceIds = E-IDA-049, E-DEV-107, E-DEV-108

## 目标

本轮只完成 `TODO-086 / 线配筋 dirty 状态与保存清除入口准备 P0`。

目标不是旧图石完整 `sub_1405E49D0` dirty / undo / save / close prompt parity。

本轮只建立正式 app 的最小保存状态边界：

```text
Rebar.Create.LineGroup 成功
  -> ProjectDirty + RebarDirty + DrawingDirty

Project.Save 成功
  -> 调用 TsRebarProjectRuntime.saveSnapshot(...)
  -> 清除 ProjectDirty / RebarDirty / DrawingDirty
  -> 保留 dirty transaction evidence / gap 追溯

Project.Save 失败
  -> dirty 保持
  -> 旧 dirty parity gap 保持
  -> 不把失败伪装成成功
```

能力等级固定为：

```text
p0-app-dirty-save-clear-entry
```

## 代码变更

```text
app/src/app/MainWindow.h / MainWindow.cpp
  -> 新增 setProjectPackagePathForInspection
  -> 新增 lastSaveResultForInspection
  -> 注册 Project.Save P0 handler
  -> 新增 currentProjectSnapshotForSave
  -> 新增 applyDirtyStateFromSaveResult
  -> 保存成功只清 ProjectDirty / RebarDirty / DrawingDirty
  -> 保存失败不清 dirty

app/src/command/LegacyUiCommandMap.cpp
  -> Project.Save 标记为 Implemented P0
  -> 补 E-DEV-108 和 GAP-REB-C-002 追溯

app/tests/integration/line_group_display_smoke_tests.cpp
  -> 真实 123.stp 上触发 Rebar.Create.LineGroup
  -> dirty 后触发 Project.Save 成功路径
  -> 断言 dirtyBefore=true / dirtyAfter=false
  -> 断言 ProjectDirty / RebarDirty / DrawingDirty 被清除
  -> 再创建第二个 LineGroup 重新 dirty
  -> 用文件路径阻塞保存安装，断言失败且 dirty 保持

tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 新增 TODO-086 done-report JSON 合同
```

## Binding 处理边界

`RebarGroupCreator` 在 domain 层只保存 legacy 语义下的相对 binding 描述，例如：

```text
geometryRef.curveStableIds[0]
```

`TsRebarProjectRuntime` 的 validator 要求保存包里的 binding 指向真实 JSON path。

所以本轮只在 `MainWindow::currentProjectSnapshotForSave()` 这个 app 保存边界中，把当前线配筋对象的 binding 重写成：

```text
geometry/topology_refs.json#/topologyRefs/N
```

这不改变 domain/rebar 的业务模型，也不让 domain/rebar 依赖 OCCT。

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
继承 USB 狗 / 网络许可依赖
实现旧图石完整保存 UI
实现 SaveAs
实现关闭提示
实现 undo / redo parity
实现旧 sub_1405E49D0 完整 dirty 等价
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
让 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## TDD 记录

```text
line_group_display_smoke_123
  -> red: MainWindow 无 Project.Save P0 保存路径、保存结果和 dirty clear 入口
  -> green: 成功创建 LineGroup 后 Project.Save 可写 runtime 包并清 dirty
  -> green: 保存安装失败时 dirty 保持
  -> green: 保存后 legacyDirtyEvidenceId / unresolvedDirtyParityGap 保留

readiness gate unit
  -> red: TODO-086 done report 未纳入 gate 合同
  -> green: TODO-086 正例通过，pending xhigh 反例被拦截
```

## 仍未闭合

```text
旧 sub_1405E49D0 dirty / undo / save 完整语义
旧图石保存提示 / 关闭提示运行确认
旧 undo stack 与 transaction rollback
新 app 完整 Save/Open UI
保存包 runtime open 回读 smoke
线配筋 / 弧形组 golden 对照
```

## 验证

最终验证记录见：

```text
docs/phase1/app_build_reports/m2_rebar_create_016_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_016_run_001.json
```

## 下一步

建议下一轮执行：

```text
TODO-087 / 线配筋保存包 runtime 回读验证 P0
```

下一轮只做保存包回读验证，不声明旧图石完整保存 UI、SFL 兼容或旧 dirty / undo / save parity。
