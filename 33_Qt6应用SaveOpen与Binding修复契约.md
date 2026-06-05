# Qt6 应用 Save/Open 与 Binding 修复契约

## 目标

本文件补齐 `GAP-DEV-001` 中两个仍然卡住 Phase 1 的开发缺口：

```text
Qt6 应用内 Save/Open 回滚测试
binding 修复流程
```

它回答：

```text
Qt6 应用里的 Project.Save / Project.Open 到底怎么调 validator？
哪些状态可以进入正式编辑，哪些只能查看或修复？
保存失败时 UI、dirty、旧包、临时包和 audit 怎么处理？
geometryRef / binding 断了以后怎么修？
什么报告可以作为 M1-Formal 的证据？
```

本文件新增开发证据：

```text
E-DEV-018
  -> Qt6 应用 Save/Open 与 binding 修复契约
  -> 来源：33_Qt6应用SaveOpen与Binding修复契约.md

E-DEV-020
  -> Qt6 应用 Save/Open 与 binding repair gate simulation、测试和首批报告
  -> 来源：
     tools/app_save_open_gate/
     docs/phase1/app_save_open_reports/
```

注意：

- `E-DEV-018` 是开发契约，不是实际 Qt6 运行报告。
- `E-DEV-020` 是应用层状态机 simulation，不是实际 Qt6 C++ / GUI 运行报告。
- `E-DEV-016` 已证明 standalone 事务 gate，不等于 Qt6 应用内 Save/Open。
- `E-DEV-017` 已证明当前 seed STEP 选择 ID gate，不等于 binding 修复 UI。
- 没有实际 Qt6 测试和报告前，`GAP-DEV-001 / GAP-DEV-007` 不能关闭。

## 控制合同

| 字段 | 约束 |
|---|---|
| Primary Setpoint | Qt6 应用层的保存、打开、验证、binding 修复和 dirty 状态必须是同一条事务链，不能各做各的。 |
| Acceptance | 本文件给出状态机、命令契约、保存/打开流程、binding 修复流程、报告 schema、测试场景和 readiness 接入口。 |
| Guardrail Metrics | validator error 不能覆盖旧包；不能清 dirty；不能静默改绑到相似面/边；不能把父目录源码当旧图石业务证据。 |
| Boundary | 只定义 Qt6 应用层契约和报告模板；不实现 C++ 代码；不声明 M1-Formal 已通过。 |
| Rollback Trigger | 临时包验证失败、写入失败、替换失败、binding 修复候选验证失败时，正式包必须保持原 hash，应用保持 dirty 或 repairRequired。 |
| Approximation Validity | standalone gate 只可作为事务语义样本；正式验收必须由 Qt6 应用命令、项目状态和报告共同证明。 |

## 主边界

### 可以依赖

- `tools/tsrebar_validator/validate_tsrebar.py` 当前错误码和报告口径。
- `tools/tsrebar_transaction_gate/transaction_gate.py` 的 standalone 事务语义。
- `docs/phase1/step_selection/` 的 STEP selection diff 报告。
- `fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/`
- `fixtures/tsrebar/fixture_b_single_group/project.tsrebar/`
- `fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/`
- `fixtures/tsrebar/negative/`

### 不能依赖

- 父目录 `.tsrproj / .vtsproj` 作为正式格式。
- OCCT transient index 作为长期保存 ID。
- 相似 bbox / 距离最近面作为自动改绑依据。
- 没有 evidence 的手工字段命名。
- standalone gate 直接冒充 Qt6 应用运行报告。

## 应用层状态机

### ProjectDocumentState

```text
NoProject
  -> Opening
  -> OpenedFormal
  -> OpenedWarning
  -> OpenedRepairRequired
  -> OpenBlocked

OpenedFormal
  -> Dirty
  -> Saving
  -> NoProject

OpenedWarning
  -> Dirty
  -> Saving
  -> OpenedRepairRequired
  -> NoProject

OpenedRepairRequired
  -> BindingRepairPreview
  -> BindingRepairApplying
  -> Dirty
  -> Saving
  -> NoProject

Dirty
  -> Saving
  -> SaveFailed
  -> OpenedFormal
  -> OpenedWarning

Saving
  -> OpenedFormal
  -> OpenedWarning
  -> SaveFailed

SaveFailed
  -> Dirty
  -> BindingRepairPreview
  -> NoProject
```

规则：

- `OpenedFormal`：允许钢筋编辑、工程图生成、保存。
- `OpenedWarning`：允许编辑，但保存时 warning 必须进入 audit。
- `OpenedRepairRequired`：禁止依赖 geometryRef 的钢筋编辑、剖切、出图和下料；允许查看、选择新 STEP、进入修复预览。
- `OpenBlocked`：不构建正式 RebarModel，只显示错误报告和原始包摘要。
- `SaveFailed`：旧包 hash 不变，dirty 不清，UI 必须保留失败报告入口。

### BindingState

```text
resolved
warningOnly
pendingLegacyEvidence
unresolvedGeometry
repairRequired
repairPreview
repairAppliedPendingSave
blocked
```

进入规则：

- `resolved`：legacyPath、geometryPath、evidence、STEP diff 全部可追溯。
- `warningOnly`：字段仍有 pending，但不影响当前 geometryRef 恢复。
- `pendingLegacyEvidence`：旧字段语义未闭合，但 raw 可保存。
- `unresolvedGeometry`：几何引用缺失或 STEP 来源缺失，但对象 raw 可保留。
- `repairRequired`：打开时发现 topology mismatch、sourceStep 不匹配、binding geometryPath 断裂。
- `repairPreview`：已选新几何候选，但还没有写入工程包。
- `repairAppliedPendingSave`：候选已应用到内存快照，必须经过 Save 事务才能落盘。
- `blocked`：证据伪造、包半写、requiredFiles 缺失等不能进入工程对象。

禁止规则：

- 禁止从 `repairRequired` 直接跳到 `resolved`。
- 禁止在没有用户确认和报告的情况下改写 `geometryRef`。
- 禁止把低置信匹配写成 confirmed。
- 禁止修复流程改写 `legacyObject.raw` 字段语义。

## Qt6 应用命令契约

### Project.OpenTsRebar

输入：

- `*.tsrebar` 工程包路径。

应用流程：

```text
1. 进入 Opening
2. 读取 manifest 和 requiredFiles
3. 构建 PackageSnapshot，只读
4. 跑 ProjectValidationRunner
5. 读取 source_step 和 topology_refs
6. 跑 STEP selection diff 索引校验
7. 构建 RebarModelSnapshot
8. 恢复 legacyObject.raw
9. 恢复 geometryRef
10. 计算 BindingState
11. 根据 finding 决定 OpenedFormal / OpenedWarning / OpenedRepairRequired / OpenBlocked
12. 写 app open report
```

输出：

- `ProjectDocumentState`
- `ValidationReport`
- `BindingRecoveryReport`
- UI 命令可用性矩阵

验收：

- `fixture_a_empty_step` 打开为 `OpenedFormal`。
- `fixture_b_single_group` 可打开，warning 必须可见。
- `fixture_c_pending_legacy` 不能被误判为完全 confirmed。
- `negative/missing_required_file` 打开为 `OpenBlocked`。
- `negative/broken_binding` 打开为 `OpenedRepairRequired` 或 `OpenBlocked`，不能静默改绑。

### Project.Save

输入：

- 当前 `ProjectDocument` 内存快照。
- 当前 dirty 状态。
- 当前 validation / binding repair 状态。

应用流程：

```text
1. UI 禁用重复保存入口，进入 Saving
2. 从 UI / model 层冻结 ProjectSnapshot
3. 写临时工程包目录或临时 zip
4. 写 manifest/project/geometry/rebar/drawing/evidence
5. 写 save_transaction.json，状态为 writing
6. 跑完整 validator
7. 跑 binding 状态检查
8. 写 validation report 和 app save report
9. 无 error 时把 save_transaction.json 标为 validated
10. 原子替换正式包
11. 替换成功后写 audit，清 ProjectDirty
12. 替换失败或 validator error 时进入 SaveFailed
```

失败必须满足：

- 旧工程包 hash 不变。
- ProjectDirty 不清。
- 内存对象不丢。
- 临时包保留或归档到失败报告。
- UI 显示失败摘要和报告路径。

成功必须满足：

- 正式包 hash 改变或明确记录 no-op save。
- ProjectDirty 清除。
- app save report 写入 `docs/phase1/app_save_open_reports/` 或正式工程报告目录。
- audit 记录 validator 决策、binding 决策、旧 hash、新 hash。

### Project.SaveAs

规则：

- 和 `Project.Save` 共享同一事务链。
- 目标路径不能覆盖当前打开包，除非走同一原子替换流程。
- SaveAs 成功后更新当前工程路径和 MRU。
- SaveAs 失败时当前工程路径不变。

### Binding.Repair.Begin

触发条件：

- `OpenedRepairRequired`。
- `GeometryRefValidator` 或 `LegacyGeometryBindingValidator` 出现 error。
- 用户选择“修复几何引用”入口。

输入：

- 当前 broken binding 列表。
- 原 `source_step` 记录。
- 用户选择的新 STEP/STP 或同名修复路径。

流程：

```text
1. 进入 BindingRepairPreview
2. 导入候选 STEP
3. 跑 STEP selection gate
4. 用 stableRef + step-shape-fingerprint/v1 建候选映射
5. 生成 oldRef -> candidateRef 对照表
6. 标记 match / ambiguous / missing / typeMismatch
7. 生成 preview report
8. UI 展示候选，不写工程包
```

禁止：

- preview 阶段写正式包。
- 自动把 ambiguous 当 resolved。
- 自动把 missing 绑定到最近几何。

### Binding.Repair.Apply

输入：

- 用户确认的 repair preview。

流程：

```text
1. 只应用 match 且 evidence 完整的映射
2. ambiguous / missing 保持 unresolvedGeometry
3. 更新 geometry/source_step.json 候选来源
4. 更新 geometry/topology_refs.json
5. 更新 binding.items[].geometryPath
6. 对每个更新写 repair evidence
7. 状态变为 repairAppliedPendingSave
8. 立即触发 Project.Save 或提示用户保存
```

验收：

- 修复应用后仍必须跑完整 Save 事务。
- validator error 时回到 `OpenedRepairRequired` 或 `SaveFailed`。
- 修复报告必须列出每条 binding 的旧路径、新路径、证据、结果。

### Binding.Repair.Cancel

规则：

- 丢弃 preview，不改内存 RebarModel。
- `OpenedRepairRequired` 状态保持。
- 不清 dirty。
- 写 cancel audit。

## UI 命令可用性矩阵

| 状态 | 查看 | 选择几何 | 钢筋编辑 | 工程图 | 保存 | 修复 binding |
|---|---|---|---|---|---|---|
| OpenedFormal | enabled | enabled | enabled | enabled | enabled when dirty | disabled |
| OpenedWarning | enabled | enabled | enabled | warning | enabled when dirty | optional |
| OpenedRepairRequired | enabled | limited | disabled | disabled | enabled only after repair/apply or raw-only audit | enabled |
| OpenBlocked | report only | disabled | disabled | disabled | disabled | disabled |
| SaveFailed | enabled | current state | disabled until decision | disabled | retry enabled | if binding error |

说明：

- `limited` 表示只能选择候选几何用于修复，不能执行正式钢筋生成。
- raw-only audit 只允许保存“明确仍 unresolved 的包”，不能假装 resolved。

## Binding 修复报告 schema

正式工程报告建议写到：

```text
docs/phase1/app_save_open_reports/
```

报告文件：

```text
app_save_open_run_001.json
binding_repair_run_001.json
binding_repair_preview_001.md
```

### app_save_open_run JSON

```json
{
  "reportId": "app_save_open_run_001",
  "scope": "Qt6 application Save/Open gate",
  "createdAt": "YYYY-MM-DDTHH:mm:ss+08:00",
  "appVersion": "phase1-dev",
  "schemaVersion": "tsrebar/v1",
  "cases": [
    {
      "caseId": "ASO-001",
      "command": "Project.OpenTsRebar",
      "inputPackage": "fixtures/tsrebar/fixture_a_empty_step/project.tsrebar",
      "initialState": "NoProject",
      "finalState": "OpenedFormal",
      "dirtyBefore": false,
      "dirtyAfter": false,
      "oldPackageHash": "",
      "newPackageHash": "",
      "validationDecision": "pass",
      "bindingDecision": "resolved",
      "reportPath": "docs/phase1/app_save_open_reports/ASO-001_open_fixture_a.json",
      "decision": "pass"
    }
  ],
  "summary": {
    "passCount": 0,
    "failCount": 0,
    "blockedCount": 0,
    "m1FormalCandidate": false
  }
}
```

### binding_repair_run JSON

```json
{
  "reportId": "binding_repair_run_001",
  "scope": "Binding repair preview and apply gate",
  "createdAt": "YYYY-MM-DDTHH:mm:ss+08:00",
  "sourcePackage": "fixtures/tsrebar/negative/broken_binding/project.tsrebar",
  "sourceStepBefore": "geometry/source_step.json#sourceSteps[0]",
  "sourceStepAfter": "geometry/source_step.json#sourceSteps[0]",
  "stepSelectionEvidence": [
    "docs/phase1/step_selection/test_stp/step_selection_diff.json"
  ],
  "bindings": [
    {
      "objectId": "group-001",
      "role": "hostFace",
      "oldGeometryPath": "geometryRef.hostFaces[0]",
      "candidateGeometryPath": "geometryRef.hostFaces[0]",
      "matchState": "match",
      "evidence": ["E-DEV-017", "E-DEV-018"],
      "decision": "apply"
    }
  ],
  "validationDecisionAfterApply": "pass",
  "saveDecision": "SaveSucceeded",
  "oldPackageHashPreservedOnFailure": true
}
```

## 当前 M1-Formal 实际运行

当前已落地：

```text
tools/app_save_open_gate/app_gate.py
tools/app_save_open_gate/test_app_gate.py
tools/qt6_runtime_gate/run_qt6_runtime_gate.py
tools/qt6_runtime_gate/test_qt6_runtime_gate.py
tools/qt6_runtime_gate/cpp/qt6_runtime_gate.cpp
docs/phase1/app_save_open_reports/app_save_open_run_001.json
docs/phase1/app_save_open_reports/binding_repair_run_001.json
docs/phase1/app_save_open_reports/binding_repair_preview_001.md
```

Run 001 结果：

```text
app_save_open_run_001.json:
  scope: Qt6 C++ runtime gate
  decision: qt6-app-pass
  passCount: 8
  blockedCount: 2
  m1FormalCandidate: true
  qApplicationCreated: true
  guiCommandActionsCreated: true

binding_repair_run_001.json:
  decision: qt6-binding-repair-pass
  saveDecision: SaveSucceeded
  validationDecisionAfterApply: warning-only
  m1FormalCandidate: true
```

这说明：

- `OpenedFormal / OpenedWarning / OpenedRepairRequired / OpenBlocked` 状态映射已可跑。
- Save 失败时旧包 hash 保持、dirty 不清已可验证。
- Save 成功时包替换、dirty 清除已可验证。
- Binding repair preview 不写正式包，apply 后仍走 Save 事务已可验证。
- Qt6 runtime 已实际创建 `QApplication` 和 GUI command `QAction`，不再是 pre-Qt6 simulation 报告。

边界仍然要写清楚：

- `E-DEV-022` 只关闭 Phase1 开发入口的 Qt6 runtime 证据缺口。
- 它不代表旧图石钢筋生成、编辑、统计等完整业务算法已经复刻。
- 后续正式产品工程仍要把这里的状态机迁入真实 Qt6 + OCCT 代码结构，并继续用旧图石运行、IDA、SFL、Detail 证据校准。

## 必跑场景

| Case ID | 命令 | 输入 | 期望 |
|---|---|---|---|
| ASO-001 | Open | Fixture A | `OpenedFormal`，无 error。 |
| ASO-002 | Open | Fixture B | `OpenedWarning` 或 warning-only，不能丢 pending evidence。 |
| ASO-003 | Open | Fixture C | `OpenedWarning` 或 `OpenedRepairRequired`，不能标 fully confirmed。 |
| ASO-004 | Open | missing required file | `OpenBlocked`，不构建正式 RebarModel。 |
| ASO-005 | Save | broken binding candidate | `SaveFailed`，旧 hash 不变，dirty 不清。 |
| ASO-006 | Save | valid dirty package | `SaveSucceeded`，清 dirty，audit 存在。 |
| ASO-007 | Save | 写临时包后失败注入 | 正式包 hash 不变，失败报告存在。 |
| ASO-008 | Repair Preview | broken binding + candidate STEP | 只生成 preview，不写正式包。 |
| ASO-009 | Repair Apply | confirmed repair preview | 进入 `repairAppliedPendingSave` 并跑 Save 事务。 |
| ASO-010 | Repair Apply | ambiguous candidate | 保持 unresolved，禁止自动 resolved。 |

## 建议工程位置

正式 Qt6 + OCCT 工程初始化时建议建立：

```text
src/project/
  ProjectDocument.h
  ProjectDocumentState.h
  ProjectSaveController.h
  ProjectOpenController.h
  ProjectSaveAudit.h

src/project/binding/
  BindingState.h
  BindingIssue.h
  BindingRepairController.h
  BindingRepairPreview.h
  BindingRepairReport.h

src/project/validation/
  ProjectValidationRunner.h

tests/integration/project/
  test_qt_app_save_open.cpp
  test_binding_repair.cpp

docs/phase1/app_save_open_reports/
```

工程边界：

- `ProjectSaveController` 可以调用 validator，但不能自己发明证据。
- `BindingRepairController` 可以生成候选映射，但不能越过用户确认。
- Viewer 只提供几何选择和高亮，不拥有钢筋业务真相。
- RebarModel 只接受 `legacyObject.raw + geometryRef + binding + evidence` 同时成立的对象。

## Readiness 接入口

`M1-Formal` 申请时必须补充：

```text
docs/phase1/app_save_open_reports/app_save_open_run_001.json
docs/phase1/app_save_open_reports/binding_repair_run_001.json
docs/phase1/app_save_open_reports/binding_repair_preview_001.md
```

并在 `docs/phase1/readiness_run_001.md` 中记录：

```text
Qt6 Application Save/Open Gate:
  result:
  app version:
  report:
  pass cases:
  fail cases:
  old package hash preserved on failed save:
  dirty preserved on failed save:

Binding Repair Gate:
  result:
  report:
  preview generated:
  ambiguous candidates blocked:
  silent rebind forbidden:
```

## 关闭 GAP 的边界

本文件可以减少 `GAP-DEV-001` 的“契约不清”风险，但不能关闭它。

关闭 `GAP-DEV-001` 至少还需要：

- Qt6 应用内 `Project.OpenTsRebar` 集成实现。
- Qt6 应用内 `Project.Save / SaveAs` 集成实现。
- Qt6 应用内失败保存回滚测试报告。
- binding repair preview / apply / cancel 实现。
- binding repair 报告。
- readiness run 明确记录 M1-Formal 是否允许。

关闭 `GAP-DEV-007` 还需要：

- 依赖门禁实际报告。
- Qt6 应用 Save/Open gate 实际报告。
- Binding Repair gate 实际报告。
- Detail writer L0/L1 实际报告。
- `readiness_run_001.md` 结果达到 `M1-Formal-Ready`。
