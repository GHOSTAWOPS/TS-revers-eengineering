# 新设计文件格式 SFL + OCCT 联合开发任务拆解

## 目标

本文件把 `18` 和 `24` 的策略继续落到开发任务。

核心纠偏：

```text
新设计文件格式要结合 SFL 和 OCCT 来做。

SFL / IDA / Detail
  -> 决定旧图石钢筋业务语义、字段、编号、统计、工程图映射

OCCT / STEP / BRep
  -> 承载新系统几何、拓扑选择、剖切、投影、距离和显示

*.tsrebar 工程包
  -> 把旧业务对象和新几何引用绑定起来
  -> 负责保存、读取、binding 校验、事务、证据追溯和缺口治理
```

一句白话：

```text
SFL 管“图石以前到底怎么记钢筋”。
OCCT 管“新程序怎么稳定找到几何对象并计算”。
新格式管“把二者锁在同一个可开发工程包里”。
```

## 三层结构

```text
旧图石证据层                         新几何承载层
SFL / IDA / Detail                    OCCT / STEP / BRep
  │                                      │
  ├── steelData                          ├── source STEP
  ├── steelbargroup                      ├── face / edge / vertex
  ├── steelbar                           ├── topology fingerprint
  ├── seg_steelbargroup                  ├── curve / section / projection
  └── StbGroup / StbGeo / StbTable       └── distance / intersection
              │                         │
              └──────────┬──────────────┘
                         ↓
              新设计文件格式 *.tsrebar
              legacyObject.raw
              geometryRef
              binding
              detailMapping
              evidence / unresolved_fields
```

这三层不能互相替代：

- `legacyObject.raw` 不能被 OCCT `geometryRef` 替代。
- `geometryRef` 不能被旧 SFL 的 ACIS 指针替代。
- `binding` 不能省略，否则旧业务对象和新几何只是两堆孤立数据。
- `Detail` ID 不能另起一套工程图真相，必须从 `RebarModel` 派生。
- 父目录已有工程格式不能替代本项目新格式。

## 核心设计决策

### 决策 1：SFL 是业务证据源，不是一期主工程壳

首期不直接保存为老 `.sfl`。

但 `.sfl` 里的对象名、对象数量、字段痕迹、保存顺序，是新格式业务字段的重要来源。

落地要求：

- `steelData / steelbargroup / steelbar / seg_steelbargroup` 都要有 `legacyObject.raw`。
- 暂不能命名的字段必须保留偏移名或 raw 名。
- 低置信字段必须进入 `evidence/unresolved_fields.json`。

### 决策 2：OCCT 是几何承载，不是钢筋业务规则来源

OCCT 提供 STEP、BRep、选择、曲线、剖切、投影、距离。

但钢筋生成逻辑、编号、统计、工程图映射，仍按旧图石证据复刻。

落地要求：

- `geometryRef.kernel = "OCCT"`。
- `geometryRef.sourceStepId` 必须能追到 `geometry/source_step.json`。
- face / edge / curve 必须通过 `topology_refs.json` 或明确 unresolved。
- 不能把 OCCT 导入顺序当长期稳定 ID。

### 决策 3：新格式负责把业务对象和几何引用绑定

每个核心对象必须同时说明：

```text
我是谁           -> id / kind
旧图石怎么存     -> legacyObject.raw
我挂在哪个几何上 -> geometryRef
二者如何对应     -> binding
哪些值可重算     -> derived
证据从哪里来     -> evidence
还有什么没闭合   -> pendingLegacyEvidence / unresolved_fields
```

## 首批开发任务

### T25-001：冻结开发期包结构

输入文档：

- `18_新设计文件格式替代SFL策略.md`
- `24_新设计文件格式Schema与Fixture草案.md`

交付物：

- `manifest.json`
- `project.json`
- `geometry/source_step.json`
- `geometry/topology_refs.json`
- `rebar/steel_data.json`
- `rebar/groups.json`
- `rebar/bars.json`
- `rebar/segments.json`
- `drawing/detail_mapping.json`
- `evidence/evidence_index.json`
- `evidence/unresolved_fields.json`

验收：

- 包结构能被 `PackageValidator` 枚举。
- 不出现父目录 `.tsrproj/.vtsproj` 作为正式格式。
- 所有 rebar 文件都能同时出现 `legacyObject.raw`、必要 `geometryRef` 或明确 unresolved 状态、以及 `binding`。

### T25-002：产出 Fixture A/B/C

Fixture A：空工程 + STEP 引用。

必须证明：

- 没有钢筋时也能保存工程入口。
- STEP 缺失时工程仍可打开。
- `geometry/source_step.json` 和 `evidence/evidence_index.json` 可读。

Fixture B：一个钢筋组 + 一根钢筋 + 一段线。

必须证明：

- `steelData / steelbargroup / steelbar / seg_steelbargroup` 同包保存。
- `legacyObject.raw` 记录旧图石对象骨架。
- `geometryRef.curveRefs` 记录 OCCT 曲线引用。
- `binding.items` 记录旧业务对象和 OCCT 几何引用之间的角色关系。
- `drawing/detail_mapping.json` 能追回 `groupId / rsdId / segmentId`。

Fixture C：含低置信字段的复杂组。

必须证明：

- `Varies_ShangH_LZ` raw block 不丢。
- `pendingLegacyEvidence` 打开后仍保留。
- validator 不允许把低置信字段伪装成 confirmed。

### T25-003：实现 Validator 任务清单

第一轮 validator 以包结构、旧对象、几何、binding、Detail 映射、证据和事务为边界：

```text
PackageValidator
  -> 检查包结构、manifest、requiredFiles、JSON 可读性

ProjectReferenceValidator
  -> 检查 project、rebar、drawing、evidence 之间的 ID 引用

LegacyObjectValidator
  -> 检查 legacyObject.type、raw.schema、legacyWriteOrder、证据 ID

GeometryRefValidator
  -> 检查 sourceStepId、topologyRefs、curveRefs、禁止旧 ACIS 指针当稳定 ID

LegacyGeometryBindingValidator
  -> 检查 binding.state、legacyPath、geometryPath、evidence
  -> 检查旧业务对象和 OCCT 几何引用是否真正绑定
  -> 检查 unresolvedGeometry 不能静默改绑到其他面、边、曲线

CoreObjectBindingValidator
  -> 检查核心钢筋对象是否同时具备 legacyObject.raw、证据 ID、必要 geometryRef 或明确 unresolved 状态
  -> 禁止纯 geometryRef 对象进入正式 RebarModel

DetailMappingValidator
  -> 检查 StbGroup.groupID、StbGroup.rsdID、StbGeo.segID、StbTable.rsdID 都能追回同一份 RebarModel
  -> 禁止下料表和图上钢筋形成两套编号真相

EvidenceValidator
  -> 检查 evidence ID、GAP ID、pendingLegacyEvidence 与 confirmed 字段状态
  -> 禁止低置信字段伪装成已确认业务字段

TransactionValidator
  -> 检查临时包可读、失败不覆盖旧包、成功后 dirty 清除
```

validator 错误码、失败输出格式和 negative fixture 清单见：

```text
26_首期可验证联合格式Fixture与Validator执行清单.md
```

验收：

- Fixture A/B/C 均可通过预期 validator。
- 人为删文件、断引用、伪造 evidence、写旧 ACIS 指针、删除 binding 时必须失败。
- validator 失败不能清 dirty，不能覆盖旧工程包。

### T25-004：实现 Save / Open 事务契约

保存流程：

```text
内存 RebarModel / GeometryModel / DrawingModel
  -> 临时工程包
  -> 写 manifest/project/geometry/rebar/drawing/evidence
  -> 写 legacyObject.raw / geometryRef / binding
  -> validators
  -> 原子替换正式包
  -> 成功后清 dirty
```

读取流程：

```text
打开工程包
  -> 读 manifest/project
  -> 读 legacyObject.raw
  -> 读 geometryRef
  -> 读 binding 并校验绑定状态
  -> 读 detailMapping
  -> 重建 RebarModel
  -> STEP 缺失或拓扑失败时进入修复状态
```

强约束：

- STEP 缺失不能导致钢筋业务对象丢失。
- topology 匹配失败不能静默挂到错误面或边。
- binding 校验失败不能清 dirty，也不能进入正式编辑状态。
- `pendingLegacyEvidence` 必须给开发期警告。

### T25-005：实现 OCCT 几何引用解析 Spike

输入：

- `19_STEP选择ID稳定性Spike计划.md`
- Fixture A/B/C
- 至少一个真实 STEP/STP 模型

任务：

- 导入 STEP。
- 遍历 face / edge / vertex。
- 生成 fingerprint。
- 保存 `topology_refs.json`。
- 重新打开后匹配 topologyRef。
- 输出 diff 报告。

通过口径：

- 简单 STEP：face/edge 匹配 100%。
- 复杂 STEP：按 `19` 的门禁执行。
- 匹配失败时进入修复状态，不允许静默错误绑定。

### T25-006：继续补旧图石字段证据

已补强证据：

- `steelbar` restore/read 函数：`E-IDA-017`。
- `seg_steelbargroup` restore/read 函数：`E-IDA-017`。
- `StbGroup%d -> sub_14063E910` 详细字段写出：`E-IDA-018`。
- `StbTable / MaterialTable -> sub_140602F90` 下料字段写值链：`E-IDA-019`。
- `35057 / 0x88F1 -> psexcel -> sub_140605B20` 下料表内部命令绑定：`E-IDA-043`。
- `工程图 / 输出` Ribbon 分组包含 `{36124, 35057}`：`E-IDA-042`。

仍需继续用 IDA 或旧图石运行闭合：

- `steelData` 字段业务名。
- `steelbargroup` 大字段业务名。
- `groupId / rsdId / stbNum / segID` 的生成和映射规则。
- `StbTable / MaterialTable` 字段值公式。
- 按钮 caption `下料表` / Dialog #427 到 `35057 / 0x88F1 / psexcel` 的对应关系。
- 旧图石运行输出目录、文件名和覆盖行为。

这些不阻塞 `legacyObject.raw` 保存。

但会阻塞字段从 `pendingLegacyEvidence` 晋级为 confirmed。

## 需求追溯

本任务拆解支撑：

- `REQ-PROJ-001`：新设计文件格式。
- `GAP-DEV-001`：schema / fixture / validator / 回滚测试。
- `GAP-DEV-002`：STEP 选择 ID 稳定性。
- `GAP-SFL-003`：`steelData` 字段业务名。
- `GAP-SFL-004`：`steelbargroup` 大字段业务名。
- `GAP-SFL-005`：`steelbar / seg_steelbargroup` 字段业务名。

新增证据建议：

```text
E-DEV-003
  -> 新设计文件格式 SFL + OCCT 联合开发任务拆解
  -> 来源：25_新设计文件格式SFL_OCCT联合开发任务拆解.md

E-DEV-004
  -> 新设计文件格式 legacyObject.raw + geometryRef + binding 绑定契约
  -> 来源：18_新设计文件格式替代SFL策略.md / 24_新设计文件格式Schema与Fixture草案.md

E-DEV-005
  -> 首期可验证联合格式 Fixture A/B/C 与 Validator 执行清单
  -> 来源：26_首期可验证联合格式Fixture与Validator执行清单.md
```

## 当前结论

新设计文件格式的准确路线是：

```text
不是老 SFL 文件壳。
不是 OCCT 自由建模工程。

而是：
旧图石业务语义保真层
  + OCCT 几何拓扑承载层
  + 新工程包事务和证据追溯层
```

开发时先做四件套的保存读取闭环：

```text
legacyObject.raw + geometryRef + binding + evidence
```

然后再逐个钢筋命令补旧图石字段和行为证据。
