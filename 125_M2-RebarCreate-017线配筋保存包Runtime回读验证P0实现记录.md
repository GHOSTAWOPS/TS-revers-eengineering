# M2-RebarCreate-017 线配筋保存包 Runtime 回读验证 P0 实现记录

## 2026-06-11 路线状态

本文件记录的是路线切换前的旧 VisualTS 线配筋保存包回读节点。
`TODO-088` 曾是当时建议的下一步，但 2026-06-11 pivot 后已经降级为
旧路线 pending，不再作为当前 next。

当前 next 以 `todo.csv` 和 `46_CSE_v2Goal执行目标与Todo说明.md` 为准：

```text
TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0
```

todoId = TODO-087
phase = M2-RebarCreate-017
evidenceIds = E-DEV-046, E-DEV-108, E-DEV-109

## 目标

本轮只完成 `TODO-087 / 线配筋保存包 runtime 回读验证 P0`。

目标不是旧图石完整保存 / 打开 UI，也不是 `.sfl` 兼容。

本轮只证明正式 app 通过 `Project.Save` 写出的 `.tsrebar` 包，可以被
`TsRebarProjectRuntime.open()` 回读，并恢复线配筋对象的最小结构：

```text
Project.Save 成功
  -> 写出 .tsrebar package
  -> 清 ProjectDirty / RebarDirty / DrawingDirty

TsRebarProjectRuntime.open(package)
  -> OpenedWarning
  -> bindingDecision != blocked
  -> 恢复 SteelBarGroup / SteelBar / SteelBarSegment
  -> 恢复 binding geometryPath
  -> 恢复 evidence_index.json 里的 evidence / gap id
```

能力等级固定为：

```text
p0-save-package-runtime-openback
```

## 代码变更

```text
app/src/project/runtime/TsRebarProjectRuntime.cpp
  -> 新增 evidenceIdsFromIndex(...)
  -> readSnapshot(...) 打开包时读取 evidence/evidence_index.json
  -> snapshot.evidenceIds 恢复保存包里的 evidence / gap id

app/tests/unit/project_runtime_tests.cpp
  -> 新增 testOpenRestoresEvidenceIndexIds
  -> 覆盖 runtime saveSnapshot -> open 后恢复 E-DEV-004 / E-IDA-016 / GAP-SFL-004

app/tests/integration/line_group_display_smoke_tests.cpp
  -> 在真实 123.stp 的 Rebar.Create.LineGroup + Project.Save 成功后
  -> 用 TsRebarProjectRuntime.open(packagePath) 回读保存包
  -> 断言 OpenedWarning / bindingDecision != blocked
  -> 断言 group/bar/segment 数量恢复为 1
  -> 断言 group binding geometryPath 指向 runtime topology_refs JSON path
  -> 断言 E-DEV-108 / E-IDA-049 / GAP-REB-C-002 从 evidence index 恢复

tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 新增 TODO-087 done-report JSON 合同
  -> 要求 targetedRuntimeOpenBackTests 等验证项闭合
```

## TDD 记录

```text
project_runtime_tests
  -> red: runtime.open(...) 不恢复 evidence_index.json 中的 id
  -> green: readSnapshot(...) 从 evidence/evidence_index.json 恢复 snapshot.evidenceIds

line_group_display_smoke_123
  -> red: 保存后 open.snapshot.evidenceIds 缺少 E-DEV-108 / E-IDA-049 / GAP-REB-C-002
  -> green: 真实 123.stp 保存包可 open-back，且 SteelData / binding / evidence 基本结构恢复

readiness gate unit
  -> red: TODO-087 done report 未纳入 gate 合同
  -> green: TODO-087 正例通过，缺 targetedRuntimeOpenBackTests 反例被拦截
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
继承 USB 狗 / 网络许可依赖
实现旧图石完整保存 UI
实现 Project.OpenTsRebar 完整 Qt UI
实现 SaveAs
实现关闭提示
实现 undo / redo parity
实现旧 sub_1405E49D0 完整 dirty / save 等价
实现 .sfl 兼容打开
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
让 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## 仍未闭合

```text
Project.OpenTsRebar Qt6 命令入口恢复到 app 内存 / AIS 显示
旧图石保存提示 / 关闭提示运行确认
旧 undo stack 与 transaction rollback
旧 sub_1405E49D0 dirty / undo / save 完整语义
完整 binding repair UI
线配筋 / 弧形组 golden 对照
```

## 验证

最终验证记录见：

```text
docs/phase1/app_build_reports/m2_rebar_create_017_run_001.md
docs/phase1/app_build_reports/m2_rebar_create_017_run_001.json
```

## 历史下一步（已降级）

当时建议下一轮执行：

```text
TODO-088 / Project.Open runtime snapshot 恢复到 app 内存 / 显示入口 P0
```

2026-06-11 路线切换后，`TODO-088` 已降级，不再作为当前 next。
当前 next 是 `TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0`。
