# 首期 tsrebar 实际 Fixture 包清单与 Golden 断言

## 目标与边界

本文件把 `24` 的 schema 草案和 `26` 的执行清单继续压实到
“实际 fixture 包应该长什么样”的层级。

它回答：

```text
Fixture A/B/C 每个 project.tsrebar 目录里到底有哪些文件？
每个文件必须有哪些对象 ID 和字段？
每个 validator 的通过/失败 golden 断言是什么？
Save/Open 事务失败时，dirty、旧包、audit 应该是什么状态？
STEP 拓扑 diff 在 fixture 中怎么占位？
```

本文件新增开发证据：

```text
E-DEV-007
  -> 首期 tsrebar 实际 Fixture 包清单与 Golden 断言
  -> 来源：29_首期tsrebar实际Fixture包清单与Golden断言.md
```

注意：

- `E-DEV-007` 证明 fixture 包清单和 golden 断言已经明确。
- 它不证明 fixture 目录已经真实创建。
- 它不证明 validator 已实现。
- 实际 fixture、validator 和 standalone Save/Open 的当前状态分别由 `E-DEV-011` 到 `E-DEV-016` 追溯。
- 它不能单独关闭 `GAP-DEV-001 / GAP-DEV-002 / GAP-DEV-007`。

## 和关闭 GAP 的关系

本文件推进这些缺口：

- `GAP-DEV-001`：新设计文件格式 fixture、validator、Save/Open 回滚。
- `GAP-DEV-002`：STEP 选择 ID diff 输入、输出占位和 31 号实际运行记录模板。
- `GAP-DEV-007`：Phase 1 readiness 实际打勾口径。
- `E-DEV-010`：validator 实现契约、错误码总表、报告 schema 和 Save/Open gate。
- `GAP-SFL-003/004/005`：低置信旧字段必须进入 unresolved。
- `GAP-DRAW-004/005`：Detail ID 同源断言。

但关闭条件仍然是实际工件：

```text
有真实 fixture 包
+ 有 negative fixture
+ 有 validator 代码
+ 有 standalone Save/Open 回滚测试
+ 有 Qt6 应用内 Save/Open 测试
+ 有 STEP diff 报告
+ 有 readiness run 记录
```

只写本文档，不允许把 `99` 中对应 GAP 标成关闭。

## 公共包结构冻结

所有正向 fixture 的目录结构固定：

```text
project.tsrebar/
├── manifest.json
├── project.json
├── geometry/
│   ├── source_step.json
│   └── topology_refs.json
├── rebar/
│   ├── steel_data.json
│   ├── groups.json
│   ├── bars.json
│   └── segments.json
├── drawing/
│   └── detail_mapping.json
└── evidence/
    ├── evidence_index.json
    └── unresolved_fields.json
```

`manifest.requiredFiles` 必须逐项列出：

```json
[
  "project.json",
  "geometry/source_step.json",
  "geometry/topology_refs.json",
  "rebar/steel_data.json",
  "rebar/groups.json",
  "rebar/bars.json",
  "rebar/segments.json",
  "drawing/detail_mapping.json",
  "evidence/evidence_index.json",
  "evidence/unresolved_fields.json"
]
```

禁止：

- 缺文件但依然通过 `PackageValidator`。
- 只在 `project.json` 引用文件，`manifest.requiredFiles` 不列出。
- 使用 `.sfl`、`.tsrproj`、`.vtsproj` 作为主工程文件。
- 把 `occtTransientIndex` 作为唯一稳定 ID。

## 公共 ID 规则

Fixture 里先使用稳定、可读、可断言的 ID。

```text
projectId        = proj-fixture-a / proj-fixture-b / proj-fixture-c
sourceStepId     = step-main
steelDataId      = steel-data-001
rebarGroupId     = group-001
groupId          = detail-group-001
rsdId            = rsd-001
barId            = bar-001
segmentId        = segment-001
topologyFaceId   = face-001
topologyEdgeId   = edge-001
curveId          = curve-seg-001
```

Golden 断言：

- 同一 ID 在所有文件中必须完全一致。
- `RebarGroup.rsdId` 必须能追回 `detail_mapping.tables[].rsdId`。
- `RebarSegment.id` 必须能追回 `detail_mapping.segments[].stbGeoSegId`。
- `curveId` 必须能追回 `segments.geometryRef.curveRefs[]`。
- `topologyFaceId / topologyEdgeId` 必须能追回 `geometry/topology_refs.json`。

## Fixture A：空工程 + STEP 引用

路径：

```text
fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/
```

目的：

- 验证空工程包结构。
- 验证 STEP 引用可保存。
- 验证无钢筋时 rebar 文件空数组合法。
- 验证 STEP 缺失时不丢工程和 evidence。

### A-Manifest Golden

`manifest.json` 必须断言：

```text
format = tsrebar
formatVersion = 1
schemaVersion = 2026-06-05-draft 或工程冻结值
saveTransaction.state = complete
saveTransaction.dirtyCleared = true
requiredFiles 完整列出 10 个文件
```

### A-Project Golden

`project.json` 必须断言：

```text
projectId = proj-fixture-a
sourceModelId = step-main
dirtyFlags.projectDirty = false
dirtyFlags.geometryDirty = false
dirtyFlags.rebarDirty = false
dirtyFlags.drawingDirty = false
models.structure = geometry/source_step.json
```

### A-Geometry Golden

`geometry/source_step.json` 必须断言：

```text
sourceStepId = step-main
path = models/fixture_a.step
sha256 非空；开发期允许 fixture-a-step-sha256-placeholder
importKernel = OCCT
```

`geometry/topology_refs.json` 必须断言：

```text
sourceStepId = step-main
topologyRefs = []
```

### A-Rebar / Drawing Golden

必须全部为空集合：

```text
rebar/steel_data.json.items = []
rebar/groups.json.items = []
rebar/bars.json.items = []
rebar/segments.json.items = []
drawing/detail_mapping.json.groups = []
drawing/detail_mapping.json.segments = []
drawing/detail_mapping.json.tables = []
```

### A-Evidence Golden

`evidence/evidence_index.json` 至少包含：

```text
E-DEV-002
E-DEV-003
E-DEV-004
E-DEV-005
E-DEV-007
```

`evidence/unresolved_fields.json.items = []`。

### A-Validator Golden

应通过：

- `PackageValidator`
- `ProjectReferenceValidator`
- `GeometryRefValidator`
- `EvidenceValidator`
- `TransactionValidator`

允许空集合通过：

- `LegacyObjectValidator`
- `LegacyGeometryBindingValidator`
- `CoreObjectBindingValidator`
- `DetailMappingValidator`

## Fixture B：单组单筋单段线

路径：

```text
fixtures/tsrebar/fixture_b_single_group/project.tsrebar/
```

目的：

```text
这是首个证明联合格式成立的 fixture。
它必须同时出现 legacyObject.raw、geometryRef、binding、evidence、Detail 映射。
```

### B-对象数量 Golden

```text
steel_data.json.items      = 1
groups.json.items          = 1
bars.json.items            = 1
segments.json.items        = 1
topology_refs.topologyRefs >= 2，至少 face-001 + edge-001
detail_mapping.groups      = 1
detail_mapping.segments    = 1
detail_mapping.tables      = 1
```

### B-Topology Golden

`geometry/topology_refs.json` 至少包含：

```json
{
  "topologyId": "face-001",
  "shapeType": "face",
  "sourceStepId": "step-main",
  "occtTransientIndex": 1,
  "fingerprint": {
    "surfaceType": "plane",
    "area": 48.0,
    "center": [6.0, 2.0, 1.0],
    "normalHint": [0.0, 0.0, 1.0],
    "edgeCount": 4
  },
  "evidence": ["GAP-DEV-002"]
}
```

以及：

```json
{
  "topologyId": "edge-001",
  "shapeType": "edge",
  "sourceStepId": "step-main",
  "occtTransientIndex": 2,
  "fingerprint": {
    "curveType": "line",
    "length": 12.0,
    "midPoint": [6.0, 0.0, 1.0],
    "adjacentFaceIds": ["face-001"]
  },
  "evidence": ["GAP-DEV-002"]
}
```

Golden 断言：

- `occtTransientIndex` 允许存在，但不能是唯一 key。
- `fingerprint` 必须存在。
- `sourceStepId` 必须等于 `step-main`。

### B-SteelData Golden

`rebar/steel_data.json.items[0]` 必须断言：

```text
id = steel-data-001
kind = RebarSteelData
legacyObject.type = steelData
legacyObject.raw.schema = legacy-steelData-v1
legacyObject.raw.fields.name 非空
legacyObject.raw.fields.int72/int76/int80 存在
legacyObject.raw.fields.real84_120 存在
evidence 包含 E-IDA-015 / E-IDA-016 / GAP-SFL-003
```

不允许：

- 把 `int72/int76/int80` 提升成 confirmed 业务名。
- 删除 `legacyObject.raw`，只保留漂亮字段。

### B-Group Golden

`rebar/groups.json.items[0]` 必须断言：

```text
id = group-001
kind = RebarGroup
groupId = detail-group-001
rsdId = rsd-001
legacyObject.type = steelbargroup
legacyObject.raw.schema = legacy-steelbargroup-v1
legacyObject.raw.fields.name72 非空
legacyObject.raw.fields.refs80_112 存在
legacyObject.raw.fields.vector144 存在
legacyObject.raw.fields.positions168_192 存在
geometryRef.kernel = OCCT
geometryRef.sourceStepId = step-main
geometryRef.topologyRefs 包含 face-001
binding.state = resolved
binding.items 至少包含 role = sourceFace
evidence 包含 E-IDA-015 / E-IDA-016 / E-IDA-018 / E-DETAIL-001 / GAP-SFL-004
```

`binding.items[role=sourceFace]` 必须断言：

```text
legacyPath 可解析到 legacyObject.raw.fields.positions168_192 或明确 unresolved 字段
geometryPath 可解析到 geometryRef.topologyRefs[0]
geometryPath 最终引用 face-001
evidence 包含 E-DEV-004 / GAP-DEV-002
```

### B-Bar Golden

`rebar/bars.json.items[0]` 必须断言：

```text
id = bar-001
kind = Rebar
groupId = group-001
legacyObject.type = steelbar
legacyObject.raw.schema = legacy-steelbar-v1
legacyObject.raw.fields.name64 非空
legacyObject.raw.fields.ref72/ref80/ref88/int96/int100 存在
geometryRef.kernel = OCCT
geometryRef.curveRefs 包含 curve-seg-001
binding.state = resolved
segmentIds 包含 segment-001
evidence 包含 E-IDA-015 / E-IDA-017 / GAP-SFL-005
```

### B-Segment Golden

`rebar/segments.json.items[0]` 必须断言：

```text
id = segment-001
kind = RebarSegment
legacyObject.type = seg_steelbargroup
legacyObject.raw.schema = legacy-seg-steelbargroup-v1
legacyObject.raw.legacyWriteOrder = ["int68", "string72", "ptr80", "ptr96", "ptr88", "int64"]
legacyObject.raw.fields.string72DisplayName 非空
geometryRef.kernel = OCCT
geometryRef.curveRefs[0].curveId = curve-seg-001
geometryRef.curveRefs[0].sourceTopologyId = edge-001
binding.state = resolved
rules.minCreateDistance = 0.002
rules.minEdgeLength = 0.01
rules.nearEndpointDistance = 0.1
rules.trimDelta = -0.03
rules.safeCurveParams = [0.001, 0.999]
rules.splineSampleCount = max(5, length * 50)
evidence 包含 E-IDA-013 / E-IDA-016 / E-IDA-017 / GAP-IDA-007
```

### B-DetailMapping Golden

`drawing/detail_mapping.json` 必须断言：

```text
groups[0].rebarGroupId = group-001
groups[0].stbGroupId = detail-group-001
groups[0].rsdId = rsd-001
segments[0].rebarSegmentId = segment-001
segments[0].stbGeoSegId = segment-001
segments[0].sourceCurveRef = curve-seg-001
tables[0].sourceGroupId = group-001
tables[0].rsdId = rsd-001
```

强断言：

```text
groups[0].rsdId == tables[0].rsdId
tables[0].sourceGroupId 存在于 groups.json
segments[0].rebarSegmentId 存在于 segments.json
```

### B-Validator Golden

全部通过：

- `PackageValidator`
- `ProjectReferenceValidator`
- `LegacyObjectValidator`
- `GeometryRefValidator`
- `LegacyGeometryBindingValidator`
- `CoreObjectBindingValidator`
- `DetailMappingValidator`
- `EvidenceValidator`
- `TransactionValidator`

## Fixture C：pendingLegacyEvidence 复杂组

路径：

```text
fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/
```

目的：

- 证明复杂 raw 字段不丢。
- 证明低置信字段不会自动变 confirmed。
- 证明 `unresolvedGeometry` 可打开，但进入修复/只读状态。

### C-Group Golden

`rebar/groups.json.items[0]` 必须断言：

```text
legacyObject.raw.fields.variesShanghLzBlock 存在且不为 null
legacyObject.raw.fields.indexOrNumberFields328_360 存在
legacyObject.raw.fields.flags364_380 存在
binding.state = pendingLegacyEvidence 或 resolved
evidence 包含 GAP-SFL-004
```

如果 `binding.state = resolved`：

- `geometryPath` 必须能解析到真实 `geometryRef`。

如果 `binding.state = pendingLegacyEvidence`：

- 相关字段必须在 `unresolved_fields.json` 中列出。

### C-Segment Golden

`rebar/segments.json.items[0]` 必须断言：

```text
legacyObject.raw.fields.ptr80 存在
legacyObject.raw.fields.ptr96 存在
legacyObject.raw.fields.ptr88 存在
legacyObject.raw.fields.int64RoleOrState 存在
binding.state = unresolvedGeometry 或 pendingLegacyEvidence
evidence 包含 GAP-IDA-007
```

如果 `binding.state = unresolvedGeometry`：

```text
geometryRef.curveRefs 可以为空
unresolvedReason 必须存在
openMode = repairRequired 或 readOnlyGeometry
```

### C-Unresolved Golden

`evidence/unresolved_fields.json` 必须列出：

```text
steelbargroup.indexOrNumberFields328_360 -> GAP-SFL-004
steelbargroup.variesShanghLzBlock -> GAP-SFL-004
seg_steelbargroup.ptr80 -> GAP-IDA-007
seg_steelbargroup.ptr96 -> GAP-IDA-007
seg_steelbargroup.ptr88 -> GAP-IDA-007
seg_steelbargroup.int64RoleOrState -> GAP-IDA-007
steelbar.ref72/ref80/ref88/int96/int100 -> GAP-SFL-005
```

Golden 断言：

- pending 字段不能标 `confirmed`。
- pending 字段不能用于 Detail writer 的最终业务字段。
- pending 字段打开后必须仍保留 raw 值。

### C-Validator Golden

应通过但带 warning：

- `LegacyObjectValidator`
- `LegacyGeometryBindingValidator`
- `EvidenceValidator`

warning 示例：

```json
{
  "validatorName": "EvidenceValidator",
  "objectId": "group-001",
  "jsonPath": "rebar/groups.json#/items/0/legacyObject/raw/fields/variesShanghLzBlock",
  "errorCode": "EVW001_PENDING_LEGACY_FIELD",
  "message": "pending legacy field is preserved and listed in unresolved_fields",
  "evidenceHint": ["GAP-SFL-004"],
  "severity": "warning"
}
```

## Negative Fixture 清单

negative fixture 路径：

```text
fixtures/tsrebar/negative/
```

每个 negative fixture 只破坏一个规则。
这样 validator 失败才能定位。

### N1：missing_required_file

破坏：

```text
删除 geometry/source_step.json
```

预期：

```text
PackageValidator -> PV001_REQUIRED_FILE_MISSING
TransactionValidator -> 不覆盖旧包，不清 dirty
```

### N2：broken_binding

破坏：

```text
groups[0].binding.items[0].geometryPath 指向不存在路径
```

预期：

```text
LegacyGeometryBindingValidator -> LGV004_GEOMETRY_PATH_BROKEN
CoreObjectBindingValidator -> 不进入正式编辑态
```

### N3：acis_pointer_as_geometry_id

破坏：

```text
geometryRef.topologyRefs = ["ENTITY* 0x12345678"]
```

预期：

```text
GeometryRefValidator -> GRV004_ACIS_POINTER_USED_AS_STABLE_ID
```

### N4：transient_index_only

破坏：

```text
topologyRefs[0] 只有 occtTransientIndex，没有 fingerprint
```

预期：

```text
GeometryRefValidator -> GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID
```

### N5：forged_evidence

破坏：

```text
对象 evidence 包含 E-FAKE-999
```

预期：

```text
EvidenceValidator -> EV001_EVIDENCE_ID_NOT_FOUND
ProjectReferenceValidator -> PRV004_EVIDENCE_REF_BROKEN
```

### N6：pending_field_marked_confirmed

破坏：

```text
seg_steelbargroup.ptr80 标记 confirmed
但没有 IDA / SFL / 运行确认支撑
```

预期：

```text
EvidenceValidator -> EV003_PENDING_MARKED_CONFIRMED
EvidenceValidator -> EV004_CONFIRMED_WITHOUT_EVIDENCE
```

### N7：detail_rsd_mismatch

破坏：

```text
detail_mapping.tables[0].rsdId != groups[0].rsdId
```

预期：

```text
DetailMappingValidator -> DMV002_RSD_ID_MISMATCH
```

### N8：half_written_package

破坏：

```text
manifest.saveTransaction.state = partial
或缺少保存 audit
```

预期：

```text
PackageValidator -> PV004_SAVE_TRANSACTION_INCOMPLETE
TransactionValidator -> TXV001_TEMP_PACKAGE_INVALID
```

## Golden Validator 结果

正向 fixture 预期：

| Fixture | Package | ProjectRef | Legacy | Geometry | Binding | Core | Detail | Evidence | Transaction |
|---|---|---|---|---|---|---|---|---|---|
| A | pass | pass | pass-empty | pass-empty | pass-empty | pass-empty | pass-empty | pass | pass |
| B | pass | pass | pass | pass | pass | pass | pass | pass | pass |
| C | pass | pass | pass-warning | pass-warning | pass-warning | pass | pass | pass-warning | pass |

negative fixture 预期：

| Negative | 主要失败 | errorCode | 保存后状态 |
|---|---|---|---|
| missing_required_file | PackageValidator | PV001_REQUIRED_FILE_MISSING | 不覆盖旧包，不清 dirty |
| broken_binding | LegacyGeometryBindingValidator | LGV004_GEOMETRY_PATH_BROKEN | 不覆盖旧包，不清 dirty |
| acis_pointer_as_geometry_id | GeometryRefValidator | GRV004_ACIS_POINTER_USED_AS_STABLE_ID | 不覆盖旧包，不清 dirty |
| transient_index_only | GeometryRefValidator | GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID | 不覆盖旧包，不清 dirty |
| forged_evidence | EvidenceValidator | EV001_EVIDENCE_ID_NOT_FOUND | 不覆盖旧包，不清 dirty |
| pending_field_marked_confirmed | EvidenceValidator | EV003 / EV004 | 不覆盖旧包，不清 dirty |
| detail_rsd_mismatch | DetailMappingValidator | DMV002_RSD_ID_MISMATCH | 不覆盖旧包，不清 dirty |
| half_written_package | Package / Transaction | PV004 / TXV001 | 不覆盖旧包，不清 dirty |

## Save/Open 事务状态矩阵

### Save 成功

```text
输入状态：
  ProjectDirty = true
  oldPackage exists

动作：
  写入 .tmp/save-{timestamp}/project.tsrebar
  跑全部 validator
  写 audit/save_runs.jsonl
  原子替换正式包

输出状态：
  ProjectDirty = false
  oldPackage replaced
  audit 有 success 记录
```

### Save 失败：validator error

```text
输入状态：
  ProjectDirty = true
  oldPackage exists

动作：
  写入 .tmp/save-{timestamp}/project.tsrebar
  validator 失败

输出状态：
  ProjectDirty = true
  oldPackage unchanged
  tmpPackage preserved for debug 或按策略归档
  audit 有 failed 记录
```

### Save 失败：写入中断

```text
输入状态：
  ProjectDirty = true
  oldPackage exists

动作：
  写到一半进程中断或磁盘失败

输出状态：
  ProjectDirty = true
  oldPackage unchanged
  manifest.saveTransaction.state != complete 的包不能被打开为正式包
  audit 可缺失，但下次启动要能识别 orphan tmp
```

### Open 成功

```text
输入：
  project.tsrebar 完整
  STEP 存在
  topology fingerprint 匹配

输出：
  ProjectOpenState = editable
  BindingState = resolved
  Dirty = false
```

### Open：STEP 缺失

```text
输入：
  project.tsrebar 完整
  source_step.path 缺失

输出：
  ProjectOpenState = repairRequired
  Legacy objects loaded = true
  RebarModel business data loaded = true
  Geometry editable = false
  Dirty = false，直到用户执行修复或重绑
```

### Open：topology mismatch

```text
输入：
  STEP 存在
  fingerprint 无法匹配原 face / edge

输出：
  ProjectOpenState = repairRequired
  BindingState = unresolvedGeometry
  不静默改绑
  Dirty = false，直到用户确认重绑
```

## Binding 修复状态机

```text
resolved
  -> STEP missing
      -> unresolvedGeometry
      -> repairRequired

resolved
  -> topology mismatch
      -> unresolvedGeometry
      -> repairRequired

unresolvedGeometry
  -> user selects replacement face/edge
      -> pendingUserConfirmation
      -> resolved
      -> Dirty = true

pendingLegacyEvidence
  -> IDA / 运行确认 / SFL diff 补证据
      -> resolved 或 confirmedLegacyField

pendingUserConfirmation
  -> cancel
      -> unresolvedGeometry
      -> Dirty unchanged

pendingUserConfirmation
  -> confirm
      -> resolved
      -> Dirty = true
```

开发禁令：

- 不允许 topology mismatch 后自动绑定到“看起来相似”的面。
- 不允许 unresolvedGeometry 状态下写出 Detail 正式包。
- 不允许 pendingLegacyEvidence 字段参与 confirmed 业务计算。

## STEP diff 工件占位

Fixture B/C 要预留 STEP diff 输出目录：

```text
docs/phase1/step_selection/
  fixture_b_run_001.json
  fixture_b_run_002.json
  fixture_b_diff.md
  fixture_c_run_001.json
  fixture_c_run_002.json
  fixture_c_diff.md
```

`*_run_001.json` 至少包含：

```json
{
  "sourceStepId": "step-main",
  "runId": "fixture-b-run-001",
  "occtVersion": "TBD",
  "shapeStats": {
    "faceCount": 0,
    "edgeCount": 0
  },
  "faces": [],
  "edges": []
}
```

`*_diff.md` 至少包含：

```markdown
# STEP Selection Diff

## Input

- STEP:
- OCCT:
- Run A:
- Run B:

## Result

- Face match rate:
- Edge match rate:
- Duplicate fingerprints:
- Unmatched faces:
- Unmatched edges:

## Decision

- Can save resolved geometryRef: yes/no
- Requires unresolvedGeometry: yes/no
- Follow-up GAP:
```

注意：

- 新增工程 STEP 样本没有真实 diff 通过，或新增样本仍存在重复 fingerprint / unmatched 拓扑时，不能把该样本作为正式 binding 证据。
- Fixture 里只能放占位 hash 或开发期 hash，不能伪造真实 diff 结果。

## Phase 1 Readiness 勾选口径

正式 Phase 1 前，readiness 记录必须逐项打勾：

```text
[ ] DEPENDENCIES.md exists
[ ] THIRD_PARTY_NOTICES.md exists
[ ] licenses/ exists
[ ] sbom/project-sbom.spdx.json exists
[ ] Fixture A exists
[ ] Fixture B exists
[x] Fixture C exists
[x] negative fixtures exist
[ ] validators implemented
[ ] Save/Open transaction tests pass
[ ] STEP diff report exists
[ ] CommandId registry exists
[ ] Detail writer L0/L1 plan exists
[ ] parent directory usage reviewed as reference-only
```

记录文件建议：

```text
docs/phase1/readiness_run_001.md
```

完整初始化顺序、运行记录模板和 M1-Prep / M1-Formal 判定见：

```text
30_Phase1工程初始化Runbook与Readiness记录模板.md
```

该记录必须引用：

- `REQ-PROJ-001`
- `REQ-TECH-001`
- `REQ-TECH-002`
- `E-DEV-005`
- `E-DEV-006`
- `E-DEV-007`
- `E-DEV-008`
- `E-DEV-009`
- `E-DEV-010`
- `E-DEV-014`
- `E-DEV-015`
- `E-DEV-016`
- `E-TECH-002`
- `GAP-DEV-001`
- `GAP-DEV-002`
- `GAP-DEV-007`
- `GAP-TECH-007`

## 残余 GAP

本文档完成后仍不能关闭：

- `GAP-DEV-001`：Fixture A/B/C、首批 negative fixture、validator 脚本、首批 validation reports、standalone Save/Open reports 和首批 STEP diff reports 已落地，仍缺 Qt6 应用内 Save/Open 测试和 binding 修复流程。
- `GAP-DEV-002`：已有真实 STEP 多轮导入 diff；`test_stp` 与 5 个 complex curve STEP 样本均通过，`step-shape-fingerprint/v1` 已消解复杂 edge/vertex raw Explorer 重复噪声；当前 Phase1 seed 样本不再阻塞 M1，但新增工程 STEP 样本仍按 P1 residual 管控。
- `GAP-DEV-007`：仍缺工程初始化后的 readiness run 记录。
- `GAP-TECH-007`：仍缺正式工程内实际依赖材料和门禁运行记录。
- `GAP-SFL-003/004/005`：旧字段业务名仍需 IDA 或旧图石运行确认。
- `GAP-DRAW-004/005`：Detail writer 同源验证和回滚测试仍需实现。

## 当前结论

`32` 已经把 validator 实现契约补齐。

下一步不再继续扩写理论 validator，而是转向真实工件。

正确推进顺序是：

```text
29 golden 断言
  -> 32 validator 实现契约
  -> 创建真实 fixture A/B/C
  -> 创建 negative fixture
  -> 实现 validators
  -> 跑 Save/Open 回滚测试
  -> 跑 STEP diff
  -> 记录 Phase 1 readiness run
```

这才是把 `GAP-DEV-001` 从“文档很清楚”推进到“工程可验证”的路径。
