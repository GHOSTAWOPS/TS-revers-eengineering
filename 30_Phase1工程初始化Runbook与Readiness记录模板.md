# Phase 1 工程初始化 Runbook 与 Readiness 记录模板

## 目标

本文件把 `22 / 27 / 28 / 29` 的开工门禁串成一次可执行的工程初始化流程。

它回答：

```text
正式创建 Qt6 + OCCT 工程时，第一轮应该按什么顺序做？
哪些工件必须真实落地？
readiness_run_001.md 应该记录什么？
哪些失败只能允许停在 UI demo / M1-Prep，不能叫 M1-Formal？
```

本文件新增开发证据：

```text
E-DEV-008
  -> Phase 1 工程初始化 Runbook 与 Readiness 记录模板
  -> 来源：30_Phase1工程初始化Runbook与Readiness记录模板.md
```

注意：

- `E-DEV-008` 证明工程初始化 runbook 和记录模板已经明确。
- 它不证明工程已经初始化。
- 它不证明任何测试已经跑过。
- 它不能单独关闭 `GAP-DEV-007`。

## 控制合同

| 字段 | 约束 |
|---|---|
| Primary Setpoint | Phase 1 初始化必须从文档门禁变成可审计运行记录。 |
| Acceptance | 本文件给出初始化顺序、产物清单、readiness 记录模板、失败分级和关闭条件。 |
| Guardrail Metrics | 不把 UI demo 当正式 Phase 1；不把父目录源码当业务证据；不关闭未实际运行的 GAP。 |
| Boundary | 只定义初始化流程和记录模板；不声明真实工程已完成。 |
| Rollback Trigger | 若初始化产物绕过 `legacyObject.raw + geometryRef + binding + evidence`，必须退回 M1-Prep。 |

## 总体流程

Phase 1 初始化按下面顺序执行：

```text
0. 冻结复刻边界
1. 建立工程目录
2. 落依赖合规材料
3. 注册 CommandId
4. 落 tsrebar fixture A/B/C 与 negative fixture
5. 实现或接入 validators
6. 跑 Save/Open 事务测试
7. 跑 STEP 选择 ID diff
8. 准备 Detail writer L0/L1 工件
9. 生成 readiness_run_001.md
10. 决定 M1-Prep / M1-Formal 状态
```

判断规则：

```text
任一 P0 工件缺失
  -> 只能 M1-Prep

所有 P0 工件有实际文件和运行记录
  -> 可申请 M1-Formal
```

## 目录初始化清单

正式工程建议创建：

```text
src/
  app/
  ui/
  commands/
  geometry/
  project/
  rebar/
  drawing/
  evidence/
tests/
  unit/
  integration/
  fixtures/
fixtures/
  tsrebar/
docs/
  phase1/
licenses/
sbom/
tools/
  dependency_gate/
```

禁止：

- 把父目录已有 `.tsrproj / .vtsproj` 当正式格式。
- 把父目录钢筋算法复制为旧图石业务真相。
- 让 `geometry/` 反向定义钢筋业务字段。
- 让 `drawing/` 反向覆盖 `RebarModel`。
- 让 `ui/` 绕过 command / transaction 直接写工程文件。

## Step 0：冻结复刻边界

输入文档：

- `00_总览.md`
- `17_一期按钮追溯与命令占位矩阵.md`
- `22_开发前Readiness审计.md`
- `23_父目录源码参考边界与路线纠偏.md`
- `27_Qt6_OCCT开发入口门禁与首批工件清单.md`

必须记录：

```text
Phase1.scope.tabs = 开始 / 显示 / 钢筋 / 查询 / 工程图
Phase1.excluded = 堆石坝 / 老 SFL 直接导入 / 直接 DWG DXF 写出
ParentSourcePolicy = reference-only
```

失败条件：

- 以父目录 UI 表替代 `17`。
- 以父目录工程格式替代 `18/24/29`。
- 以 OCCT 曲线生成规则替代旧图石钢筋规则。

## Step 1：建立工程目录

必须产出：

```text
docs/phase1/readiness_run_001.md
docs/phase1/dependency_gate_run_001.md
docs/phase1/readiness_gate_reports/readiness_gate_run_001.json
docs/phase1/readiness_gate_reports/readiness_gate_run_001.md
docs/phase1/step_selection/
fixtures/tsrebar/
tests/fixtures/
```

允许：

- 空目录先创建。
- `readiness_run_001.md` 先写 `pending`。

不允许：

- 目录缺失但 readiness 写 pass。
- 工程目录用父目录旧格式名表达正式保存格式。

## Step 2：落依赖合规材料

依据：

- `21_QtOCCT依赖许可证门禁.md`
- `28_QtOCCT依赖清单与发布材料模板.md`

必须产出：

```text
DEPENDENCIES.md
THIRD_PARTY_NOTICES.md
licenses/
  QT_LICENSES.txt
  OCCT_LICENSE.txt
  SOURCE_OFFER.txt
sbom/
  project-sbom.spdx.json
docs/phase1/dependency_gate_run_001.md
```

`dependency_gate_run_001.md` 至少记录：

```text
Qt version:
OCCT version:
Qt modules:
OCCT capability groups:
Forbidden Qt module scan:
Forbidden OCCT commercial component scan:
Dynamic linking:
Result: pass/fail
```

失败条件：

- Qt / OCCT 版本仍是 `TBD`，但申请 M1-Formal。
- 出现 GPL-only Qt 模块。
- 出现 OCCT 商业交换组件。
- 没有 notice / license / SBOM。

## Step 3：注册 CommandId

依据：

- `08_开发命令契约.md`
- `17_一期按钮追溯与命令占位矩阵.md`
- `27_Qt6_OCCT开发入口门禁与首批工件清单.md`

首期至少注册：

```text
Project.New
Project.OpenStep
Project.Save
View.FitAll
View.Pan
View.Zoom
View.Rotate
View.SectionX
View.SectionY
View.SectionZ
Rebar.Create.Face
Rebar.Create.LineGroup
Rebar.Create.ArcGroup
Rebar.Edit.Move
Rebar.Group.Merge
Rebar.Group.Split
Rebar.Check.Overlap
Rebar.Check.MinDistance
Drawing.DefineSectionPlane
Drawing.DefineProjectPlane
Drawing.GeneratePackage
Drawing.GenerateBarSchedule
```

必须记录：

```text
CommandId:
Old UI path:
Evidence IDs:
GAP IDs:
Status: enabled / disabled / placeholder / open-gap
```

失败条件：

- `Open` 缺口按钮执行伪逻辑并写正式工程包。
- 按钮没有 evidence 或 GAP，却标 confirmed。
- 钢筋页只保留面配筋，漏掉线配筋、弧形组等入口。

## Step 4：落 tsrebar fixture

依据：

- `24_新设计文件格式Schema与Fixture草案.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`

必须产出：

```text
fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/
fixtures/tsrebar/fixture_b_single_group/project.tsrebar/
fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/
fixtures/tsrebar/negative/missing_required_file/
fixtures/tsrebar/negative/broken_binding/
fixtures/tsrebar/negative/acis_pointer_as_geometry_id/
fixtures/tsrebar/negative/transient_index_only/
fixtures/tsrebar/negative/forged_evidence/
fixtures/tsrebar/negative/pending_field_marked_confirmed/
fixtures/tsrebar/negative/detail_rsd_mismatch/
fixtures/tsrebar/negative/half_written_package/
```

每个正向 fixture 必须包含 10 个必需文件：

```text
manifest.json
project.json
geometry/source_step.json
geometry/topology_refs.json
rebar/steel_data.json
rebar/groups.json
rebar/bars.json
rebar/segments.json
drawing/detail_mapping.json
evidence/evidence_index.json
evidence/unresolved_fields.json
```

注意：上面实际是 11 个文件。
`29` 的 requiredFiles 列表中 `project.json` 之外有 10 个相对文件。
readiness 记录必须按实际文件数校验，不能靠口头描述通过。

通过标准：

- Fixture A/B/C 符合 `29` 的 golden 断言。
- negative fixture 每个只破坏一个规则。
- `legacyObject.raw`、`geometryRef`、`binding`、`evidence` 同时存在。

失败条件：

- 只有 manifest，没有旧业务对象。
- 只有 OCCT 几何，没有 `legacyObject.raw`。
- 保存两堆数据但没有 `binding`。
- pending 字段没有进入 `unresolved_fields.json`。

## Step 5：实现或接入 validators

必须有：

```text
PackageValidator
ProjectReferenceValidator
LegacyObjectValidator
GeometryRefValidator
LegacyGeometryBindingValidator
CoreObjectBindingValidator
DetailMappingValidator
EvidenceValidator
TransactionValidator
```

实现契约和错误码依据：

```text
32_Validator实现契约与错误码总表.md
```

必须输出结构化结果：

```json
{
  "validatorName": "LegacyGeometryBindingValidator",
  "objectId": "group-001",
  "jsonPath": "rebar/groups.json#/items/0/binding/items/0/geometryPath",
  "errorCode": "LGV004_GEOMETRY_PATH_BROKEN",
  "message": "binding geometryPath points to a missing geometryRef target",
  "evidenceHint": ["GAP-DEV-002", "E-DEV-004"],
  "severity": "error"
}
```

readiness 记录必须列出：

```text
validator:
input fixture:
result:
errorCount:
warningCount:
report path:
```

失败条件：

- validator 只返回 true/false，没有字段级错误。
- validator finding 不符合 `32` 的错误码和报告 schema。
- validator 失败仍清 dirty。
- validator 失败仍覆盖旧工程包。

## Step 6：跑 Save/Open 事务测试

依据：

- `08_开发命令契约.md`
- `20_DetailWriter输出事务契约.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`

必须覆盖：

```text
Save success
Save validator failure
Save interrupted / half-written package
Open success
Open source STEP missing
Open topology mismatch
```

必须记录：

```text
oldPackage hash before:
tmpPackage path:
validator report:
audit record:
dirty before:
dirty after:
oldPackage changed: yes/no
```

通过标准：

- 成功保存才清 dirty。
- 失败保存不覆盖旧包。
- 半写包不能作为正式包打开。
- STEP 缺失时业务对象仍可读。
- topology mismatch 不静默改绑。

## Step 7：跑 STEP 选择 ID diff

依据：

- `19_STEP选择ID稳定性Spike计划.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`
- `31_STEP选择ID实际运行记录模板与样本清单.md`

必须产出：

```text
docs/phase1/step_selection/fixture_b_run_r1.json
docs/phase1/step_selection/fixture_b_run_r2.json
docs/phase1/step_selection/fixture_b_run_r3.json
docs/phase1/step_selection/fixture_b_run_r4_copied_path.json
docs/phase1/step_selection/fixture_b_run_r5_restart.json
docs/phase1/step_selection/fixture_b_diff.md
docs/phase1/step_selection/fixture_c_run_r1.json
docs/phase1/step_selection/fixture_c_run_r2.json
docs/phase1/step_selection/fixture_c_run_r3.json
docs/phase1/step_selection/fixture_c_run_r4_copied_path.json
docs/phase1/step_selection/fixture_c_run_r5_restart.json
docs/phase1/step_selection/fixture_c_diff.md
```

`*_diff.md` 必须记录：

```text
face match rate:
edge match rate:
duplicate fingerprints:
unmatched faces:
unmatched edges:
unresolvedGeometry cases:
decision:
```

失败条件：

- 把 `occtTransientIndex` 当长期保存 ID。
- 匹配失败后自动挂到相似面。
- 在新增工程 STEP diff 未通过，或新增样本仍存在重复 fingerprint / unmatched 拓扑时，把该样本作为正式 binding 证据。

## Step 8：准备 Detail writer L0/L1

依据：

- `05_Detail工程图包证据.md`
- `13_Detail字段映射矩阵.md`
- `20_DetailWriter输出事务契约.md`

必须准备：

```text
drawings/.tmp/run_*/
audit/drawing_writer_runs.jsonl
Detail.xml
Detail01.stl
```

M1 阶段只要求 L0/L1：

- L0：XML 文件结构可解析。
- L1：包内 ID 同源，`StbGroup.rsdID` 与 `StbTable.rsdID` 可追到同一 `RebarGroup`。

不能宣称：

- 未通过 AutoCAD 2020 + 旧插件导入，不得说工程图闭环完成。

## Step 9：生成 readiness_run_001.md

建议路径：

```text
docs/phase1/readiness_run_001.md
```

模板：

```markdown
# Phase 1 Readiness Run 001

## Summary

- Date:
- Commit:
- Operator:
- Result: M1-Prep / M1-Formal-Ready / Failed
- Decision:

## Scope

- Tabs:
- Excluded:
- Parent source policy:

## Linked Requirements

- REQ-PROJ-001
- REQ-TECH-001
- REQ-TECH-002

## Linked Evidence

- E-DEV-005
- E-DEV-006
- E-DEV-007
- E-DEV-008
- E-DEV-009
- E-DEV-010
- E-TECH-002

## Linked GAP

- GAP-DEV-001
- GAP-DEV-002
- GAP-DEV-007
- GAP-TECH-007
- GAP-DRAW-005

## Dependency Gate

- DEPENDENCIES.md:
- THIRD_PARTY_NOTICES.md:
- licenses:
- sbom:
- dependency gate report:
- result:

## Command Gate

- Command registry:
- Open-gap commands:
- placeholder behavior:
- result:

## Fixture Gate

- Fixture A:
- Fixture B:
- Fixture C:
- Negative fixtures:
- golden assertion report:
- result:

## Validator Gate

- PackageValidator:
- ProjectReferenceValidator:
- LegacyObjectValidator:
- GeometryRefValidator:
- LegacyGeometryBindingValidator:
- CoreObjectBindingValidator:
- DetailMappingValidator:
- EvidenceValidator:
- TransactionValidator:
- result:

## Save/Open Gate

- Save success:
- Save validator failure:
- Save interrupted:
- Open success:
- Open STEP missing:
- Open topology mismatch:
- result:

## STEP Selection Gate

- run_001:
- run_002:
- diff:
- face match rate:
- edge match rate:
- decision:

## Detail Writer Gate

- L0 XML:
- L1 ID same-source:
- rollback:
- AutoCAD L2:
- result:

## Findings

| ID | Severity | Gate | Evidence | Decision |
|---|---|---|---|---|

## Final Decision

- M1-Prep allowed:
- M1-Formal allowed:
- Remaining blockers:
```

## Step 10：状态判定

### 只能叫 UI demo

满足任一条：

- 只有窗口和按钮，没有 `CommandId`。
- 只有 STEP Viewer，没有新设计文件格式。
- 只有内存钢筋对象，没有 `legacyObject.raw + geometryRef + binding + evidence`。
- 没有 fixture / validator。
- 没有 readiness run 记录。

### 可以叫 M1-Prep

满足：

- Qt6 + OCCT 外壳可以准备。
- CommandId 可注册。
- STEP Viewer 可接入。
- Open 缺口可占位。
- P0 工件仍未全部实际落地。

### 可以申请 M1-Formal

必须同时满足：

- 依赖门禁实际落地。
- Fixture A/B/C 实际存在。
- negative fixture 实际存在。
- validators 有运行记录。
- Save/Open 事务测试有运行记录。
- STEP diff 有真实报告。
- `readiness_run_001.md` 结果为 `M1-Formal-Ready`。
- `99` 中未关闭 GAP 仍保留关闭条件。

## 后续残余跟踪

M1-Formal 已可放行，但这些事项进入后续开发阶段继续跟踪：

- `GAP-DEV-001`：M1 入口阻塞已解除；M2 中把 Qt6 runtime gate 状态机迁入正式 Qt6 + OCCT 工程，并用更多真实样本扩展。
- `GAP-DEV-002`：已有真实 STEP diff；`test_stp` 与 5 个 complex curve STEP 样本均通过，`step-shape-fingerprint/v1` 已消解复杂 edge/vertex raw Explorer 重复噪声；当前 Phase1 seed 样本不再阻塞 M1，但新增工程 STEP 样本仍按 P1 residual gate 继续验证。
- `GAP-DEV-007`：已闭合 M1；后续作为 readiness gate 回归门禁保留。
- `GAP-TECH-007`：M1 依赖材料已通过；正式工程创建后继续跑 CMake 模块 allowlist 扫描。
- `GAP-DRAW-005`：Detail writer L0/L1 已通过；AutoCAD L2 动态导入另走工程图专项。

## 当前结论

现在文档已经能指导 Phase 1 初始化。

实际工件进度已经推进到：

```text
fixture A/B/C: present
negative fixture: present
validator reports: present
standalone Save/Open reports: present
Qt6 application Save/Open contract: present
Qt6 application Save/Open reports: present, qt6-app-pass
binding repair contract: present
binding repair reports: present, qt6-binding-repair-pass
STEP diff reports: present, current seed samples pass
readiness gate script/report: present, M1-Formal-Ready
```

下一步如果进入工程阶段，第一件事不是直接写钢筋算法，
而是按本文档生成：

```text
readiness_run_001.md
dependency_gate_run_001.md
fixture A/B/C
negative fixture
validator reports
Qt6 application Save/Open reports
binding repair reports
STEP diff reports for any new engineering STEP samples
readiness gate reports
```

这些真实记录出现之前，只能说：

```text
Phase 1 可准备。
Phase 1 还未正式具备可审计开工条件。
```
