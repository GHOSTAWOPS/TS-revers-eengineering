# 首期可验证联合格式 Fixture 与 Validator 执行清单

## 目标

本文件把 `24` 和 `25` 中的 schema、fixture、validator 草案继续压实到开发执行层。

Validator 的 C++ 接口、执行顺序、错误码总表、报告 schema、Save/Open gate 和 readiness 接入见 `32_Validator实现契约与错误码总表.md`。

它回答：

```text
Phase 1 前到底要产出哪些 fixture？
每个 fixture 要证明什么？
validator 要检查什么？
哪些反例必须失败？
哪些缺口仍不能关闭？
```

核心结论：

```text
首期可验证联合格式不是保存壳。

它必须同时证明四件事：

1. SFL / IDA / Detail 反推的旧业务对象没有丢。
2. OCCT / STEP 的几何引用能被保存和恢复。
3. legacyObject.raw 与 geometryRef 有 binding。
4. 保存、读取、失败回滚、证据追溯可被 validator 验证。
```

本文件新增开发证据：

```text
E-DEV-005
  -> 首期可验证联合格式 Fixture A/B/C 与 Validator 执行清单
  -> 来源：26_首期可验证联合格式Fixture与Validator执行清单.md
```

注意：

- `E-DEV-005` 不能单独关闭 `GAP-DEV-001`。
- `GAP-DEV-001` 只有在实际 fixture 包、validator 实现、保存回滚测试、STEP 几何引用验证都产出后才能降级或关闭。
- `E-DEV-010` 只证明 validator 实现契约明确，不能替代实际 validator 代码和运行报告。
- `E-DEV-015` 已补充实际 validator 脚本和首批 reports，但不能替代 Qt6 应用内 Save/Open 回滚测试和 binding 修复运行报告。
- `E-DEV-016` 已补充 standalone Save/Open transaction gate 和首批 reports，但不能替代 Qt6 应用内 Save/Open 集成和 binding 修复运行报告。
- `E-DEV-018` 已补充 Qt6 应用 Save/Open 与 binding 修复契约，但不能替代应用代码、应用级运行报告和 M1-Formal readiness。

## 控制合同

| 字段 | 本轮约束 |
|---|---|
| Primary Setpoint | 把新设计文件格式从策略推进到可开发的 fixture / validator 执行清单。 |
| Acceptance | 文档中能明确 Fixture A/B/C 的目录、对象数量、必填字段、通过断言、失败反例和回链 GAP。 |
| Guardrail Metrics | 不把新格式写成 SFL 二进制复刻；不把 OCCT 几何对象写成钢筋业务真相；不关闭仍缺真实实现的 GAP。 |
| Boundary | 本文件定义开发清单和验收断言；实际 fixture 包、validator 脚本、reports、standalone Save/Open reports、STEP selection reports 和 Qt6 应用 Save/Open 契约分别由 `E-DEV-011` 到 `E-DEV-018` 追溯。 |
| Rollback Trigger | 若发现任何条目要求用父目录字段替代旧图石证据，必须退回 `18/23` 的路线边界。 |

## 总体交付物

Phase 1 工程初始化前，建议在新工程内产出以下工件。

目录名可以在工程初始化时调整，但语义不变：

```text
fixtures/
└── tsrebar/
    ├── fixture_a_empty_step/
    │   └── project.tsrebar/
    ├── fixture_b_single_group/
    │   └── project.tsrebar/
    ├── fixture_c_pending_legacy/
    │   └── project.tsrebar/
    └── negative/
        ├── missing_required_file/
        ├── broken_binding/
        ├── acis_pointer_as_geometry_id/
        ├── forged_evidence/
        ├── pending_field_marked_confirmed/
        └── half_written_package/

tests/
└── fixtures/
    ├── test_fixture_a_empty_step.*
    ├── test_fixture_b_single_group.*
    ├── test_fixture_c_pending_legacy.*
    ├── test_negative_packages.*
    └── test_save_open_transaction.*
```

每个 `project.tsrebar/` 必须是同构目录包：

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

禁止出现：

- `.sfl` 作为主保存文件。
- `.tsrproj / .vtsproj` 作为正式格式。
- 旧 ACIS 指针作为稳定几何 ID。
- 没有 `legacyObject.raw` 的正式 `RebarGroup / Rebar / RebarSegment`。
- 没有 `binding` 的核心钢筋对象。

## Fixture A：空工程 + STEP 引用

### 目的

Fixture A 只证明工程包基础设施成立：

- 空工程也能保存和打开。
- STEP 引用和 hash 能记录。
- STEP 缺失时不丢工程元信息。
- evidence 文件可读。

它不能单独证明 SFL + OCCT 联合格式成立。

### 必填内容

`manifest.json`：

- `format = "tsrebar"`。
- `formatVersion >= 1`。
- `schemaVersion` 与当前工程代码支持版本一致。
- `requiredFiles` 全列出。
- `saveTransaction.state = "complete"`。

`project.json`：

- `projectId`。
- `sourceModelId = "step-main"`。
- `dirtyFlags.projectDirty = false`。
- `models.structure = "geometry/source_step.json"`。

`geometry/source_step.json`：

- `sourceStepId = "step-main"`。
- `path`。
- `sha256`。
- `importKernel = "OCCT"`。

`geometry/topology_refs.json`：

- 允许 `topologyRefs = []`。
- 但必须包含 `sourceStepId` 或全局来源说明。

`rebar/*.json`：

- `items = []`。
- 空列表必须合法。

`drawing/detail_mapping.json`：

- `groups = []`。
- `segments = []`。
- `tables = []`。

`evidence/evidence_index.json`：

- 至少包含 `E-DEV-002 / E-DEV-003 / E-DEV-004 / E-DEV-005`。

### 通过断言

- `PackageValidator` 通过。
- `ProjectReferenceValidator` 通过。
- `GeometryRefValidator` 在空拓扑模式下通过。
- `CoreObjectBindingValidator` 不要求空 rebar 集合有核心对象。
- 打开时如果 STEP 文件缺失，进入 `GeometryMissing` 或同等修复状态，但 `project.json` 和 evidence 仍可读。

### 必须失败的反例

- 删除 `geometry/source_step.json`，`PackageValidator` 必须失败。
- `manifest.requiredFiles` 未列出 `evidence/unresolved_fields.json`，`PackageValidator` 必须失败。
- `project.sourceModelId` 指向不存在的 STEP 来源，`ProjectReferenceValidator` 必须失败。
- `saveTransaction.state = "partial"` 却尝试清 dirty，`TransactionValidator` 必须失败。

## Fixture B：一个钢筋组 + 一根钢筋 + 一段线

### 目的

Fixture B 是首个真正证明联合格式成立的样本。

它必须同时覆盖：

- `steelData`。
- `steelbargroup`。
- `steelbar`。
- `seg_steelbargroup`。
- OCCT / STEP face、edge、curve 引用。
- `binding`。
- Detail `groupId / rsdId / segmentId` 映射。

### 对象数量

```text
steel_data.json       -> 1 个 RebarSteelData
groups.json           -> 1 个 RebarGroup
bars.json             -> 1 个 Rebar
segments.json         -> 1 个 RebarSegment
topology_refs.json    -> 至少 1 个 face、1 个 edge
detail_mapping.json   -> 1 个 group、1 个 segment、1 个 table row 映射
```

### RebarSteelData 必填

- `id`。
- `kind = "RebarSteelData"`。
- `legacyObject.type = "steelData"`。
- `legacyObject.raw.schema = "legacy-steelData-v1"`。
- `legacyObject.raw.fields.name`。
- `legacyObject.raw.fields.int72/int76/int80`。
- `legacyObject.raw.fields.real84_120`。
- `evidence` 包含 `E-IDA-015 / E-IDA-016 / GAP-SFL-003`。

不要求：

- 把 `int72/int76/int80` 命名成确定业务字段。

### RebarGroup 必填

- `id = "group-001"` 或等价稳定 ID。
- `kind = "RebarGroup"`。
- `groupId`。
- `rsdId`。
- `legacyObject.type = "steelbargroup"`。
- `legacyObject.raw.schema = "legacy-steelbargroup-v1"`。
- `legacyObject.raw.fields.name72`。
- `legacyObject.raw.fields.refs80_112`。
- `legacyObject.raw.fields.vector144`。
- `legacyObject.raw.fields.positions168_192`。
- `geometryRef.kernel = "OCCT"`。
- `geometryRef.sourceStepId = "step-main"`。
- `geometryRef.topologyRefs` 至少引用 `face-001`。
- `binding.state = "resolved"`。
- `binding.items[]` 至少有一条 `role = "sourceFace"`。
- `detailMapping.rsdId` 与根字段 `rsdId` 一致。
- `evidence` 包含 `E-IDA-015 / E-IDA-016 / E-IDA-018 / E-DETAIL-001`。

### Rebar 必填

- `id = "bar-001"` 或等价稳定 ID。
- `kind = "Rebar"`。
- `groupId` 指向 `groups.json` 中的组。
- `legacyObject.type = "steelbar"`。
- `legacyObject.raw.schema = "legacy-steelbar-v1"`。
- `legacyObject.raw.fields.name64`。
- `legacyObject.raw.fields.ref72/ref80/ref88/int96/int100`。
- `geometryRef.curveRefs` 至少引用 `curve-seg-001`。
- `binding.state = "resolved"`。
- `segmentIds` 指向 `segments.json` 中的段。
- `evidence` 包含 `E-IDA-015 / E-IDA-017 / GAP-SFL-005`。

### RebarSegment 必填

- `id = "segment-001"` 或等价稳定 ID。
- `kind = "RebarSegment"`。
- `legacyObject.type = "seg_steelbargroup"`。
- `legacyObject.raw.schema = "legacy-seg-steelbargroup-v1"`。
- `legacyObject.raw.legacyWriteOrder = ["int68", "string72", "ptr80", "ptr96", "ptr88", "int64"]`。
- `legacyObject.raw.fields.string72DisplayName`。
- `geometryRef.curveRefs[0].curveId = "curve-seg-001"`。
- `geometryRef.curveRefs[0].sourceTopologyId` 指向 `topology_refs.json` 里的 edge。
- `binding.state = "resolved"`。
- `rules.minCreateDistance = 0.002`。
- `rules.minEdgeLength = 0.01`。
- `rules.nearEndpointDistance = 0.1`。
- `rules.trimDelta = -0.03`。
- `rules.safeCurveParams = [0.001, 0.999]`。
- `rules.splineSampleCount = "max(5, length * 50)"`。
- `evidence` 包含 `E-IDA-016 / E-IDA-017 / GAP-IDA-007`。

### DetailMapping 必填

必须证明同源：

```text
RebarGroup.groupId
  -> detail_mapping.groups[].stbGroupId

RebarGroup.rsdId
  -> detail_mapping.groups[].rsdId
  -> detail_mapping.tables[].rsdId

RebarSegment.segmentId
  -> detail_mapping.segments[].stbGeoSegId
```

必须有断言：

- `groups[].rebarGroupId` 能追回 `groups.json`。
- `segments[].rebarSegmentId` 能追回 `segments.json`。
- `tables[].sourceGroupId` 能追回 `groups.json`。
- `tables[].rsdId` 必须等于对应 `RebarGroup.rsdId`。

### 必须失败的反例

- 删除 `groups[0].legacyObject.raw`，`CoreObjectBindingValidator` 必须失败。
- 删除 `groups[0].binding`，`LegacyGeometryBindingValidator` 必须失败。
- `binding.items[].legacyPath` 或 `geometryPath` 只是字符串占位、不能真实解析到对象字段，`LegacyGeometryBindingValidator` 必须失败。
- `bars[0].groupId` 指向不存在的组，`ProjectReferenceValidator` 必须失败。
- `segments[0].geometryRef.curveRefs[0].sourceTopologyId` 指向不存在 edge，`GeometryRefValidator` 必须失败。
- `detail_mapping.tables[0].rsdId` 与 `groups[0].rsdId` 不一致，`DetailMappingValidator` 必须失败。
- `RebarGroup` 只有 `geometryRef`，没有 `legacyObject.raw` 和 evidence，`CoreObjectBindingValidator` 必须失败。

## Fixture C：含 pendingLegacyEvidence 的复杂组

### 目的

Fixture C 用来证明低置信字段不会被误写成确定结论。

它重点覆盖：

- `Varies_ShangH_LZ` raw block。
- `steelbargroup` 大字段中未命名字段。
- `seg_steelbargroup` 的 `ptr80/ptr96/ptr88/int64RoleOrState`。
- `unresolvedGeometry`。
- `pendingLegacyEvidence`。

### 必填内容

`groups.json` 中至少 1 个复杂组：

- `legacyObject.raw.fields.variesShanghLzBlock` 不为 null。
- `legacyObject.raw.fields.indexOrNumberFields328_360` 保留 raw 值。
- `legacyObject.raw.fields.flags364_380` 保留 raw 值。
- `binding.state` 可为 `resolved` 或 `pendingLegacyEvidence`，但不能缺失。
- `evidence` 包含 `GAP-SFL-004`。

`segments.json` 中至少 1 个段：

- `legacyObject.raw.fields.ptr80/ptr96/ptr88/int64RoleOrState` 保留。
- 若 STEP 拓扑故意缺失，则 `binding.state = "unresolvedGeometry"`。
- `geometryRef.curveRefs` 可为空，但必须写明 unresolved 原因。
- `evidence` 包含 `GAP-IDA-007`。

`evidence/unresolved_fields.json`：

- 必须列出 `steelbargroup.indexOrNumberFields328_360`。
- 必须列出 `steelbargroup.variesShanghLzBlock`。
- 必须列出 `seg_steelbargroup.ptr80/ptr96/ptr88/int64RoleOrState`。
- 每项必须绑定 GAP：`GAP-SFL-004 / GAP-SFL-005 / GAP-IDA-007`。

### 通过断言

- `LegacyObjectValidator` 通过，且保留 raw block。
- `EvidenceValidator` 允许 pending 字段存在，但要求它们列入 `unresolved_fields.json`。
- `LegacyGeometryBindingValidator` 允许 `unresolvedGeometry`，但要求对象进入修复或只读状态。
- 打开后不能丢失 raw 字段。
- 打开后不能把 pending 字段显示成 confirmed。

### 必须失败的反例

- `variesShanghLzBlock` 被删除，`LegacyObjectValidator` 必须失败。
- `pendingLegacyEvidence` 字段没有进入 `unresolved_fields.json`，`EvidenceValidator` 必须失败。
- `binding.state = "resolved"` 但 `geometryRef.curveRefs` 指向不存在拓扑，`LegacyGeometryBindingValidator` 必须失败。
- 未确认字段被标记为 `confirmed` 且没有运行或 IDA 证据，`EvidenceValidator` 必须失败。

## Validator 输出格式

所有 validator 失败结果必须结构化输出。

首期统一字段：

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

字段要求：

- `validatorName`：必须能定位是哪一个 validator 失败。
- `objectId`：必须能定位到具体工程对象；包级错误可写 `project.tsrebar`。
- `jsonPath`：必须能定位到失败字段。
- `errorCode`：必须来自本文件定义的错误码。
- `message`：给开发者看的短说明，不替代 errorCode。
- `evidenceHint`：必须指向相关 Evidence ID 或 GAP ID。
- `severity`：首期只允许 `error / warning`。

输出规则：

- 阻塞保存、覆盖、进入正式编辑态的问题必须是 `error`。
- `pendingLegacyEvidence` 可作为 `warning`，但如果字段没有进入 `unresolved_fields.json`，必须升级为 `error`。
- 任何 `error` 都不能清 dirty，不能覆盖旧工程包。

## Validator 责任边界

### PackageValidator

输入：

- `manifest.json`。
- 工程包根目录。

检查：

- 必需文件存在。
- JSON 可解析。
- `format / formatVersion / schemaVersion` 合法。
- `saveTransaction.state = "complete"`。

错误码：

- `PV001_REQUIRED_FILE_MISSING`
- `PV002_JSON_PARSE_FAILED`
- `PV003_UNSUPPORTED_SCHEMA_VERSION`
- `PV004_SAVE_TRANSACTION_INCOMPLETE`

### ProjectReferenceValidator

输入：

- `project.json`。
- 全部 `geometry / rebar / drawing / evidence` 文件。

检查：

- `project.models.*` 指向存在文件。
- `groups -> bars -> segments` 引用完整。
- `detail_mapping` 能追回 rebar 对象。
- evidence ID 均能在 `evidence_index.json` 找到，GAP ID 可在 `99` 追溯。

错误码：

- `PRV001_MODEL_FILE_NOT_FOUND`
- `PRV002_REBAR_REF_BROKEN`
- `PRV003_DETAIL_MAPPING_REF_BROKEN`
- `PRV004_EVIDENCE_REF_BROKEN`

### LegacyObjectValidator

输入：

- `rebar/*.json`。
- `evidence/unresolved_fields.json`。

检查：

- 核心对象有 `legacyObject.raw`。
- `legacyObject.type` 属于已知旧图石对象：`steelData / steelbargroup / steelbar / seg_steelbargroup`。
- `legacyObject.raw.schema` 合法。
- `seg_steelbargroup.legacyWriteOrder` 顺序必须是 `+68 -> +72 -> +80 -> +96 -> +88 -> +64` 对应字段顺序。
- 低置信 raw 字段必须列入 unresolved。

错误码：

- `LOV001_LEGACY_RAW_MISSING`
- `LOV002_UNKNOWN_LEGACY_TYPE`
- `LOV003_LEGACY_SCHEMA_INVALID`
- `LOV004_LEGACY_WRITE_ORDER_INVALID`
- `LOV005_UNRESOLVED_FIELD_NOT_LISTED`

### GeometryRefValidator

输入：

- `geometry/source_step.json`。
- `geometry/topology_refs.json`。
- `rebar/*.json`。

检查：

- `geometryRef.kernel = "OCCT"`。
- `sourceStepId` 存在。
- `topologyRefs / curveRefs` 指向存在对象，或明确 `unresolvedGeometry`。
- 禁止使用旧 ACIS 指针、内存地址、`ENTITY*` 字符串作为稳定几何 ID。
- `occtTransientIndex` 只能作为 debug 字段，不能作为唯一 key。

错误码：

- `GRV001_SOURCE_STEP_NOT_FOUND`
- `GRV002_TOPOLOGY_REF_BROKEN`
- `GRV003_CURVE_REF_BROKEN`
- `GRV004_ACIS_POINTER_USED_AS_STABLE_ID`
- `GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID`

### LegacyGeometryBindingValidator

输入：

- 核心 rebar 对象。
- `geometry/*`。
- `evidence/*`。

检查：

- `binding.state` 只能是 `resolved / unresolvedGeometry / pendingLegacyEvidence`。
- `binding.items[].legacyPath` 指向存在 raw 字段，或该字段明确在 unresolved 中。
- `binding.items[].geometryPath` 指向存在几何引用，或状态为 `unresolvedGeometry`。
- `binding.items[].evidence` 必须存在。
- `unresolvedGeometry` 不能静默改绑到相似面、边、曲线。

错误码：

- `LGV001_BINDING_MISSING`
- `LGV002_BINDING_STATE_INVALID`
- `LGV003_LEGACY_PATH_BROKEN`
- `LGV004_GEOMETRY_PATH_BROKEN`
- `LGV005_BINDING_EVIDENCE_MISSING`
- `LGV006_SILENT_REBIND_FORBIDDEN`

### CoreObjectBindingValidator

输入：

- `rebar/steel_data.json`。
- `rebar/groups.json`。
- `rebar/bars.json`。
- `rebar/segments.json`。

检查：

- `RebarGroup / Rebar / RebarSegment` 必须有 `legacyObject.raw`。
- 需要几何的对象必须有 `geometryRef` 或明确 `unresolvedGeometry`。
- `SteelData` 可无 `geometryRef`，但必须有 `legacyObject.raw` 和 evidence。
- 纯 `geometryRef` 对象不能进入正式 `RebarModel`。

错误码：

- `COB001_CORE_LEGACY_RAW_MISSING`
- `COB002_CORE_GEOMETRY_REF_MISSING`
- `COB003_STEEL_DATA_EVIDENCE_MISSING`
- `COB004_PURE_GEOMETRY_OBJECT_FORBIDDEN`

### DetailMappingValidator

输入：

- `drawing/detail_mapping.json`。
- `rebar/groups.json`。
- `rebar/segments.json`。

检查：

- `StbGroup.groupID` 映射回 `RebarGroup.groupId`。
- `StbGroup.rsdID` 与 `StbTable.rsdID` 同源。
- `StbGeo.segID` 映射回 `RebarSegment.segmentId`。
- `StbTable` 不能引用不存在的 `RebarGroup`。

错误码：

- `DMV001_GROUP_ID_NOT_FOUND`
- `DMV002_RSD_ID_MISMATCH`
- `DMV003_SEGMENT_ID_NOT_FOUND`
- `DMV004_SCHEDULE_ROW_ORPHANED`

### EvidenceValidator

输入：

- `evidence/evidence_index.json`。
- `evidence/unresolved_fields.json`。
- 所有对象的 `evidence` 字段。

检查：

- evidence ID 存在。
- GAP ID 能追到 `99_缺口和待确认项.md`。
- `pendingLegacyEvidence` 不能被标记为 confirmed。
- confirmed 字段必须有 SFL / IDA / Detail / 运行确认中的至少一种证据。

错误码：

- `EV001_EVIDENCE_ID_NOT_FOUND`
- `EV002_GAP_ID_NOT_FOUND`
- `EV003_PENDING_MARKED_CONFIRMED`
- `EV004_CONFIRMED_WITHOUT_EVIDENCE`

### TransactionValidator

输入：

- 临时工程包。
- 原工程包。
- dirty 状态。
- 保存 audit。

检查：

- 临时包必须能完整打开。
- validator 失败不能覆盖原工程包。
- validator 失败不能清 dirty。
- 成功替换后必须写 audit。
- 成功后 dirty 清除必须符合 `08_开发命令契约.md`。

错误码：

- `TXV001_TEMP_PACKAGE_INVALID`
- `TXV002_FAILED_SAVE_OVERWROTE_OLD_PACKAGE`
- `TXV003_FAILED_SAVE_CLEARED_DIRTY`
- `TXV004_SUCCESS_WITHOUT_AUDIT`
- `TXV005_DIRTY_CLEAR_RULE_BROKEN`

## 验收矩阵

| 用例 | Package | ProjectRef | Legacy | Geometry | Binding | Core | Detail | Evidence | Transaction |
|---|---|---|---|---|---|---|---|---|---|
| Fixture A | 通过 | 通过 | 空集合通过 | 空拓扑通过 | 空集合通过 | 空集合通过 | 空映射通过 | 通过 | 通过 |
| Fixture B | 通过 | 通过 | 通过 | 通过 | 通过 | 通过 | 通过 | 通过 | 通过 |
| Fixture C | 通过 | 通过 | 通过 | unresolved 允许 | pending 允许 | 通过 | 通过或警告 | 通过并警告 | 通过 |
| 删除必需文件 | 失败 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不覆盖 |
| 删除 binding | 通过 | 通过 | 通过 | 通过 | 失败 | 失败 | 不执行 | 通过 | 不覆盖 |
| ACIS 指针当 ID | 通过 | 通过 | 通过 | 失败 | 不执行 | 不执行 | 不执行 | 通过 | 不覆盖 |
| 伪造 evidence | 通过 | 失败 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 失败 | 不覆盖 |
| 半写包 | 失败 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不执行 | 不清 dirty |

## 开发任务拆解

### T26-001：冻结 fixture 目录

创建：

- `fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/`
- `fixtures/tsrebar/fixture_b_single_group/project.tsrebar/`
- `fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/`
- `fixtures/tsrebar/negative/*/`

验收：

- 每个正向 fixture 都包含完整 10 个必需文件。
- 每个 negative fixture 都只破坏一个规则，便于定位 validator。

### T26-002：实现 package 和引用 validator

实现：

- `PackageValidator`
- `ProjectReferenceValidator`

验收：

- Fixture A/B/C 通过。
- `missing_required_file` 失败。
- `forged_evidence` 失败。

### T26-003：实现 legacy validator

实现：

- `LegacyObjectValidator`
- `EvidenceValidator`

验收：

- `seg_steelbargroup.legacyWriteOrder` 顺序错误时失败。
- `pending_field_marked_confirmed` 失败。
- `Varies_ShangH_LZ` block 丢失时失败。

### T26-004：实现 geometry 和 binding validator

实现：

- `GeometryRefValidator`
- `LegacyGeometryBindingValidator`
- `CoreObjectBindingValidator`

验收：

- `broken_binding` 失败。
- `acis_pointer_as_geometry_id` 失败。
- `unresolvedGeometry` 只能进入修复或只读状态。

### T26-005：实现 Detail 映射 validator

实现：

- `DetailMappingValidator`

验收：

- `rsdId` 不一致时失败。
- `StbGeo.segID` 找不到 `RebarSegment.segmentId` 时失败。
- `StbTable` 行不能成为孤儿行。

### T26-006：实现 Save / Open 事务测试

实现测试：

- 正常保存后临时包替换正式包。
- validator 失败不覆盖旧包。
- validator 失败不清 dirty。
- 保存成功写 audit。

验收：

- `TXV001..TXV005` 都有对应失败用例。

### T26-007：接入 STEP 选择 ID spike

输入：

- Fixture A/B/C。
- 至少一个真实 STEP/STP 模型。
- `19_STEP选择ID稳定性Spike计划.md`。
- `31_STEP选择ID实际运行记录模板与样本清单.md`。

输出：

- `step_selection_run_001.json`。
- `step_selection_run_002.json`。
- 需要时补 `step_selection_run_003.json / copied-path / restart`。
- `step_selection_diff.md`。

验收：

- 简单 STEP face / edge 匹配 100%。
- 复杂 STEP 匹配失败时，相关钢筋对象进入 `unresolvedGeometry`，不静默改绑。

### T26-008：更新 Phase 1 readiness

只有以下工件真实存在后，才可以把 `GAP-DEV-001` 从“文档草案”推进为“开发 spike 已产出”：

- Fixture A/B/C 实际目录包。
- negative fixtures。
- validator 代码。
- validator reports。
- Save / Open 事务测试。
- STEP 选择 ID diff 报告。
- `DEPENDENCIES.md / THIRD_PARTY_NOTICES.md` 与 Qt6 / OCCT 版本清单。

## 与缺口的关系

本文件支撑：

- `REQ-PROJ-001`：新设计文件格式。
- `GAP-DEV-001`：新设计文件格式 schema / fixture / validator / 回滚测试。
- `GAP-DEV-002`：STEP 选择 ID 稳定性。
- `GAP-SFL-003`：`steelData` 字段业务名。
- `GAP-SFL-004`：`steelbargroup` 大字段业务名。
- `GAP-SFL-005`：`steelbar / seg_steelbargroup` 字段业务名。
- `GAP-DRAW-004`：下料表和图上钢筋一致性。
- `GAP-DRAW-005`：Detail writer 输出事务。

不能关闭：

- `GAP-DEV-001`：Fixture A/B/C、首批 negative fixture、validator 脚本、首批 validation reports、standalone Save/Open reports、首批 STEP diff reports 和 Qt6 应用 Save/Open / binding 修复契约已落地，但 Qt6 应用代码、应用级 Save/Open 测试、binding 修复实际运行报告仍缺。
- `GAP-DEV-002`：首批真实 STEP 多轮导入 diff 已落地；`test.stp` 与 5 个 complex curve STEP 样本均通过，`step-shape-fingerprint/v1` 已消解复杂 edge/vertex raw Explorer 重复噪声；当前 Phase1 seed 样本不再阻塞 M1，后续新增真实工程 STEP 仍按 P1 residual gate 继续验证。
- `GAP-SFL-003/004/005`：字段业务名仍需 IDA 或旧图石运行确认。
- `GAP-DRAW-004/005`：Detail writer 还没有实际 L0/L1 输出测试。

## 当前结论

可以进入的下一步不是“自由写工程保存”。

正确顺序是：

```text
26 执行清单
  -> 29 golden 断言和实际包清单
  -> fixture A/B/C 实际包
  -> negative fixture
  -> validators
  -> Save/Open 回滚测试
  -> STEP topology diff
  -> Phase 1 readiness 打勾
```

只要其中任一环节缺失，新格式都只能算开发契约，不能算已闭环。

实际 fixture 包清单、对象 ID、golden 字段断言、negative fixture 失败期望、
Save/Open 状态矩阵和 STEP diff 占位见：

```text
29_首期tsrebar实际Fixture包清单与Golden断言.md
```
