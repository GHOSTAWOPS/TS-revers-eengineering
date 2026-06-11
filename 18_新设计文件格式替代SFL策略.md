# 新设计文件格式替代 SFL 主保存格式策略

## 2026-06-11 路线状态

本文件早期围绕“新格式承载旧 VisualTS 业务语义”展开。
路线切换后，`.tsrebar` 仍是新系统内部主工程格式，但业务语义来源调整为：

```text
STEP 几何输入
  + RebarSmart 钢筋生成逻辑证据
  + 图石 Detail 包 / 下料表 / 旧 CAD 插件兼容证据
```

`.sfl` 仍可作为历史样本和字段旁证，但不再是当前 P0/P1 的主保存格式、
主输入格式或钢筋生成逻辑主证据源。

## 目标

本文件回答：

```text
既然不把老 .sfl 作为新系统主保存格式，
新设计文件格式应该怎么设计，
才不会偏成“重新开发一套新钢筋软件”？
```

结论：

```text
格式用新的。
业务骨架必须能追溯证据。
钢筋生成字段和对象关系优先由 RebarSmart 证据驱动。
Detail / 工程图 / 下料表字段继续由旧图石 Detail 包和运行证据驱动。
几何引用和拓扑承载必须结合 OCCT / STEP 设计。
```

## 核心边界

新系统采用新的设计文件格式替代老 `.sfl` 主保存格式。

但这不是缩小复刻范围，也不是自由设计。

这里的“替代”只替代 `.sfl` 文件壳和主保存职责。

它不替代 RebarSmart / Detail / SFL / IDA 对业务语义、字段、编号和工程图映射的证据职责。

不能做：

- 不直接复刻 `.sfl` 的 ACIS BinaryFile 二进制格式。
- 不把完整读取 / 写回老 `.sfl` 作为一期主线。
- 不默认继承父目录已有工程格式。
- 不按 OCCT 能力重新设计一套钢筋业务模型。
- 不因为新格式能保存对象，就宣称旧图石复刻完成。

必须做：

- 用 RebarSmart 证据反推钢筋生成参数、筋型、分布和导引规则。
- 用 Detail 包反推工程图、下料、钢筋组 ID 映射。
- 用 `.sfl` 样本和 IDA 作为历史字段、兼容和缺口旁证。
- 用 OCCT / STEP / BRep 设计新系统的几何引用、拓扑选择 ID、剖切、投影和距离计算承载方式。
- 用旧图石运行确认补齐 dirty、保存提示、导出行为。
- 新格式字段必须能追溯到证据或明确标记为待确认。

## SFL 与新格式的关系

```text
老 .sfl
  -> 证据源
  -> 反推旧图石 steelbar / steelbargroup / steelData / 工程图对象
  -> 反推对象保存顺序、字段关系和引用关系
  -> 后续可作为兼容导入专项

新设计文件格式
  -> 新系统主保存格式
  -> 保存复刻后的结构模型、钢筋模型、工程图模型、查询状态
  -> 核心字段必须由旧图石证据驱动
  -> 未闭合字段进入 legacyObject.raw 或 pendingLegacyEvidence
  -> 几何引用和拓扑选择 ID 结合 OCCT / STEP 保存
  -> 不承担 ACIS BinaryFile 兼容负担
```

白话说：

```text
不复刻老文件壳。
要复刻老业务骨架。
```

更准确地说：

```text
新格式不是“脱离 SFL 的全新设计”。
新格式也不是“继续使用 SFL 的二进制壳”。

新格式 = SFL/IDA/Detail 反推的旧业务语义
       + OCCT/STEP 提供的几何拓扑承载
       + 新工程包自己的保存事务和证据追溯。
```

这里的“一比一”指旧图石业务对象、字段顺序、编号、统计和工程图映射按证据复刻；
不是要求新系统继续使用 `.sfl` 的 ACIS 二进制文件壳。

## SFL 与 OCCT 的分工

新设计文件格式要同时结合 `.sfl` 和 OCCT。

不是二选一。

```text
SFL / IDA / Detail
  -> 定义旧图石业务对象
  -> 定义钢筋组、钢筋段、钢筋参数、编号、统计、工程图映射

OCCT / STEP / BRep
  -> 定义新系统几何承载
  -> 定义 face / edge / vertex 选择引用
  -> 提供剖切、投影、距离、相交、显示

新设计文件格式
  -> 把旧业务对象和新几何引用绑定起来
```

字段上要明确区分：

- `legacyObject`：来自旧图石 SFL / IDA 的业务对象信息。
- `geometryRef`：来自 OCCT / STEP 的几何拓扑引用。
- `binding`：旧业务对象与新几何引用的绑定关系。
- `evidence`：字段证据来源。
- `derived`：可由几何或业务对象重新计算的派生值。

不能混淆：

- 不能用 OCCT face / edge ID 替代旧图石钢筋组编号。
- 不能用旧 SFL 的 ACIS 引用直接当新系统几何 ID。
- 不能把 Detail 包派生产物当工程主数据。

## SFL + OCCT 联合绑定原则

新格式不能只是“左边存一份 SFL raw，右边存一份 OCCT ref”。

它必须保存二者的绑定关系：

```text
legacyObject.raw
  -> 旧图石对象、字段顺序、编号、Detail ID、生成规则证据

geometryRef
  -> STEP / OCCT face、edge、curve、section、projection、fingerprint

binding
  -> 哪个旧业务对象依附哪个几何对象
  -> 用什么角色依附
  -> 绑定证据是什么
  -> 当前是否可恢复
```

白话说：

```text
SFL 告诉我们“这是什么钢筋对象”。
OCCT 告诉我们“它挂在哪个几何上、怎么算”。
binding 告诉程序“这两件事是同一个对象的两半”。
```

首期至少要支持三种绑定状态：

- `resolved`：旧业务对象和 OCCT 几何引用都能恢复。
- `unresolvedGeometry`：旧业务对象存在，但 STEP 缺失或拓扑匹配失败。
- `pendingLegacyEvidence`：几何引用存在，但旧字段语义还没闭合。

开发禁令：

- 不能让没有 `legacyObject.raw` 的纯 OCCT 曲线进入正式 `RebarModel`。
- 不能让没有 `geometryRef` 或明确 `unresolvedGeometry` 状态的钢筋对象进入正式保存验收。
- 不能把绑定失败静默修正到“看起来接近”的面或边。
- 不能把 `binding.status = pendingLegacyEvidence` 的对象显示成旧图石字段已完全确认。

## 文件形态和扩展名策略

扩展名不是业务证据，不能用扩展名反推业务模型。

本目录文档先使用工作名：

```text
*.tsrebar
```

它只表示“图石钢筋新设计工程包”。

Phase 1 工程初始化前再冻结正式扩展名。

硬边界：

- 不沿用父目录已有 `.tsrproj`。
- 不沿用父目录已有 `.vtsproj`。
- 不把父目录工程格式的 manifest、字段名、对象名自动带入本项目。
- 扩展名怎么定，不影响业务字段证据来源。

推荐文件形态：

```text
工程包 zip 或目录
├── manifest.json
├── project.json
├── geometry/
│   ├── source.step.ref.json
│   └── topology_fingerprints.json
├── rebar/
│   ├── steel_data.json
│   ├── groups.json
│   ├── bars.json
│   └── segments.json
├── drawing/
│   ├── views.json
│   └── detail_mapping.json
└── evidence/
    ├── evidence_index.json
    └── unresolved_fields.json
```

这个工程包有三类职责：

- `rebar/*` 保存 SFL / IDA / Detail 反推的旧业务对象和字段。
- `geometry/*` 保存 OCCT / STEP 的几何、拓扑和选择引用。
- `evidence/*` 保存证据、置信度和未闭合字段。

它不是纯 OCCT 工程包，也不是 SFL 二进制克隆。

说明：

- `manifest.json` 管版本、单位、依赖、schema 版本和包完整性。
- `project.json` 管工程级信息、dirty 摘要和模型入口。
- `geometry/*` 管 OCCT / STEP 几何引用和拓扑 fingerprint。
- `rebar/*` 管旧图石证据驱动的钢筋对象。
- `drawing/*` 管 Detail writer 所需视图、编号和字段映射。
- `evidence/*` 管证据 ID、置信度和未确认字段。

## 证据优先级

新格式字段设计按以下顺序取证：

1. `.sfl` 样本中的对象记录、对象名、引用关系、字段痕迹。
2. IDA 中的 save / restore / read / write 函数。
3. IDA 中的命令入口、Dialog、对象创建函数。
4. Detail 工程图包中的 `StbGroup / StbGeo / StbTable / MaterialTable`。
5. OCCT / STEP / BRep 中可稳定保存和恢复的几何引用。
6. 旧图石运行后的保存前后差异。

父目录源码不进入业务证据优先级。

父目录源码只能进入“工程实现参考”：

- 序列化组织方式。
- 保存事务。
- schema 校验。
- 失败回滚测试。
- CMake / CTest 组织方式。

它不能决定业务字段，不能单独关闭 GAP。

## 新格式应保存什么

第一层：工程级信息

- 工程名称、单位、版本。
- 源 STEP/STP 路径、hash、导入时间。
- dirty 状态和保存状态。
- 旧图石证据版本或逆向批次。

第二层：结构模型

- STEP/STP 引用。
- 拓扑对象 fingerprint。
- OCCT shape / face / edge / vertex 的稳定引用。
- 参考面、参考线、剖切面、投影面。
- face / edge / vertex 稳定选择 ID。

第三层：钢筋模型

- `steelData` 对应的钢筋级别、直径、样式、材料数据。
- `steelbargroup` 对应的组、编号、部位、参数、统计字段。
- `steelbar` 对应的单根钢筋、段、长度、形状、显示编号。
- `seg_steelbargroup` 对应的线/弧组段、曲线、采样、裁剪和链表关系。
- 接头、搭接、组合、段连接等对象。
- 钢筋业务对象与 OCCT 几何引用之间的绑定关系。

第四层：工程图和下料

- 剖切面、投影面、轴测投影。
- Detail 包输出配置。
- `StbGroup / StbGeo / StbTable` ID 映射。
- 下料表字段和图上编号一致性。

第五层：审计和证据

- 每个核心字段的证据 ID。
- 未确认字段的 `pendingLegacyEvidence`。
- 保存失败和 writer 输出 audit。

## 首期可验证联合格式

Phase 1 不能只交空壳格式。

首期可验证联合格式必须能证明以下内容：

```text
Project.Save
  -> 写临时工程包
  -> 写 manifest / project / geometry / rebar / evidence
  -> schema 校验
  -> 引用完整性校验
  -> legacyObject.raw / geometryRef / binding / evidence 绑定校验
  -> 原子替换
  -> 失败回滚

Project.Open
  -> 读 manifest
  -> 读 geometryRef
  -> 读 legacyObject.raw
  -> 校验旧业务证据、OCCT topologyRef 和二者绑定关系
  -> 重建领域对象
  -> pendingLegacyEvidence 给开发期警告
```

首期验证包里必须包含：

- 一个 STEP/STP 源文件引用和 hash。
- 一个拓扑 fingerprint 示例。
- 一个 `steelData` 的 `legacyObject.raw`。
- 一个 `steelbargroup` 的 `legacyObject.raw`。
- 一个 `steelbar` 的 `legacyObject.raw`。
- 一个 `seg_steelbargroup` 的 `legacyObject.raw` 或 `pendingLegacyEvidence` 占位。
- 一个 `geometryRef`，绑定到 STEP face / edge / curve。
- 一个 `Detail` 映射占位，能追到 `groupId / rsdId / segmentId`。
- 一个 `evidence[]` 列表，明确来自 `E-IDA / E-SFL / E-DETAIL / E-DEV`。

不满足以上条件，只能叫“保存壳”，不能叫新设计文件格式可开发。

schema、fixture、validator 和保存 / 读取事务草案见：

```text
24_新设计文件格式Schema与Fixture草案.md
```

联合开发任务拆解见：

```text
25_新设计文件格式SFL_OCCT联合开发任务拆解.md
```

Fixture A/B/C、negative fixture、validator 错误码、失败输出格式和 Phase 1 前实际工件清单见：

```text
26_首期可验证联合格式Fixture与Validator执行清单.md
```

## 字段设计规则

每个核心字段都要有以下信息：

```text
字段名
  -> 新系统用途
  -> 对应旧对象或旧字段
  -> 证据来源
  -> 置信度
  -> 是否允许暂定
```

字段状态分三类：

- `confirmed`：已有 SFL / IDA / Detail / 运行证据交叉确认。
- `inferred`：有证据迹象，但语义还需要运行确认。
- `pendingLegacyEvidence`：新系统暂需字段，旧图石证据未闭合。

不允许把 `pendingLegacyEvidence` 写成旧图石确定字段。

## 对象统一结构

每个可保存业务对象统一拆成四块：

```json
{
  "id": "stable-new-id",
  "kind": "RebarGroup",
  "legacyObject": {
    "type": "steelbargroup",
    "sourceClass": "e:\\tushi3d\\dam\\class\\steelbargroup.cpp",
    "readWriteEvidence": ["E-IDA-015", "E-IDA-016"],
    "sflObjectName": "旧对象名或段_N",
    "raw": {
      "schema": "legacy-steelbargroup-v1",
      "versionRules": [],
      "fields": []
    }
  },
  "geometryRef": {
    "kernel": "OCCT",
    "sourceStep": "source.step",
    "topologyRefs": [],
    "curveRefs": []
  },
  "binding": {
    "state": "resolved",
    "items": [
      {
        "role": "sourceGeometry",
        "legacyPath": "legacyObject.raw.fields",
        "geometryPath": "geometryRef.topologyRefs[0]",
        "status": "pendingLegacyEvidence",
        "evidence": ["GAP-DEV-002"]
      }
    ]
  },
  "derived": {},
  "evidence": ["E-IDA-016", "E-SFL-001", "GAP-DEV-002"]
}
```

白话解释：

- `id` 是新系统稳定 ID，用于保存、撤销、查询和工程图映射。
- `legacyObject` 是旧图石对象证据，先保真，后命名。
- `geometryRef` 是 OCCT / STEP 几何承载，负责恢复和计算。
- `binding` 是旧图石对象与 OCCT 几何引用之间的绑定契约。
- `derived` 是可重新计算值，例如长度、端点、包围盒。
- `evidence` 记录每个核心字段从哪里来。

核心钢筋对象不允许空 `evidence`。

空证据只能用于非核心临时对象，不能进入正式保存验收。

开发禁令：

- 不能把 `legacyObject.raw` 直接删掉，只保留漂亮业务字段。
- 不能用 `geometryRef` 替代旧图石业务编号。
- 不能让 `derived` 成为唯一主数据。

## 首批对象字段契约

本节把新设计文件格式的首批可开发字段分成两半：

```text
legacyObject
  -> 旧图石业务对象字段
  -> 来源是 SFL / IDA / Detail / 运行确认

geometryRef
  -> 新系统几何引用字段
  -> 来源是 OCCT / STEP / BRep / topology fingerprint
```

### steelData -> RebarSteelData

旧证据：

- 字符串：`steelData`。
- 源码路径：`e:\tushi3d\dam\class\steeldata.cpp`。
- IDA 保存函数：`sub_14045CCF0`。
- IDA 读取函数：`sub_14045C940`。
- IDA 类注册 / 调试列表函数：`sub_14045C750`。
- SFL 样本存在 `steelData`，但复杂样本数量和 `SteelStyle` 不完全等价。

已确认读写骨架：

| 旧偏移 / 字段 | 新格式字段建议 | 类型 | 状态 | 证据 |
|---|---|---|---|---|
| id level `steelData` | `legacyObject.type` | string | confirmed | `sub_14045CCF0` 写 |
| `a1 + 64` | `legacyObject.name` | string | confirmed | `sub_14045CCF0` 写，`sub_14045C940` 读 |
| `a1 + 72/76/80` | `legacyObject.raw.intFields[]` | int[3] | confirmed | 读写均连续 int |
| `a1 + 84..120` | `legacyObject.raw.realFields[]` | real[10] | confirmed | 版本 `<831` 读写 int 转 float，`>=831` 读写 real |
| `a1 + 124..140` | `legacyObject.raw.tailIntFields[]` | int[5] | confirmed | 读写均连续 int |
| `a1 + 144/148` | `legacyObject.raw.flags[]` | bool[2] | confirmed | `read/write_logical(F/T)` |
| `a1 + 152/156` | `legacyObject.raw.conditionalIntFields[]` | int[0..2] | confirmed | 版本 `>=838` 且 flag 为真才读写 |
| `a1 + 160/164` | `legacyObject.raw.version841Fields[]` | int[2] | confirmed | 版本 `>=841` 才读写 |

开发约束：

- 暂时不要把这些字段命名成“直径、级别、弯钩”等确定业务名。
- 可先保存为 typed legacy fields，并挂 `pendingLegacyEvidence`。
- 当 Dialog / SFL save-restore / 运行属性窗口闭合后，再把字段提升为业务名。

### steelbar -> Rebar

旧证据：

- 字符串：`steelbar`。
- 源码路径：`e:\tushi3d\dam\class\steelbar.cpp`。
- IDA 保存函数：`sub_1405E1C10`。
- IDA 读取函数：`sub_1405E1AB0`。
- IDA restore 包装入口：`sub_1405DF3F0`。
- IDA 类注册 / 调试列表函数：`sub_1405E0EA0`。
- SFL 样本存在大量 `steelbar`，名称可见 `1根钢筋 / 2根钢筋 / 3根钢筋`。

已确认读写骨架：

| 旧偏移 / 字段 | 新格式字段建议 | 类型 | 状态 | 证据 |
|---|---|---|---|---|
| id level `steelbar` | `legacyObject.type` | string | confirmed | `sub_1405E1C10` |
| `a1 + 64` | `legacyObject.name` | string | confirmed | `write_string` / `read_string` |
| `a1 + 72` | `legacyObject.raw.ref72` | ref | confirmed raw / inferred meaning | `write_ptr` / `read_ptr` |
| `a1 + 80` | `legacyObject.raw.ref80` | ref | confirmed raw / inferred meaning | `write_ptr` / `read_ptr` |
| `a1 + 88` | `legacyObject.raw.ref88` | ref | confirmed raw / inferred meaning | `write_ptr` / `read_ptr` |
| `a1 + 96` | `legacyObject.raw.int96` | int | confirmed raw / inferred meaning | `write_int` / `read_int` |
| `a1 + 100` | `legacyObject.raw.int100` | int | confirmed raw / inferred meaning | `write_int` / `read_int` |
| `a1 + 104/120` | `legacyObject.raw.runtimeResetFields` | int/ref | runtime only | 读取后置零，保存函数不写 |

新格式补充字段：

| 新格式字段 | 来源 | 状态 | 说明 |
|---|---|---|---|
| `barId` | 新系统稳定 ID | required | 不等于旧图石显示编号。 |
| `groupId` | `steelbargroup` 关系 / Detail 映射 | pendingLegacyEvidence | 需闭合旧父子关系。 |
| `segmentIds` | `seg_steelbargroup` / Detail `StbGeo` | pendingLegacyEvidence | 需闭合 `steelbar` 与 `seg` 对应关系。 |
| `geometryRef.curves[]` | OCCT / STEP | required | 保存新系统中钢筋几何曲线引用。 |

开发约束：

- `ref1/ref2/ref3` 不能直接命名为 group/segment/style，除非 IDA 或 SFL 对照闭合。
- `geometryRef` 不能用旧 ACIS 指针；必须保存 OCCT 可恢复的曲线或拓扑引用。

### steelbargroup -> RebarGroup

旧证据：

- 字符串：`steelbargroup`。
- 源码路径：`e:\tushi3d\dam\class\steelbargroup.cpp`。
- IDA 保存函数：`sub_1405F2100`。
- IDA 读取函数：`sub_1405F1A90`。
- IDA 类注册 / 调试列表函数：`sub_1405F1820`。
- Detail `StbGroup` 已确认有 `rsdID / groupID / diameter / interval / barcount / segcount / stbLevel / stbLayer / stbProfile / stbUse / ComponentName / SteelWay` 等字段。

已确认读写骨架：

| 旧偏移 / 字段 | 新格式字段建议 | 类型 | 状态 | 证据 |
|---|---|---|---|---|
| id level `steelbargroup` | `legacyObject.type` | string | confirmed | `sub_1405F2100` |
| `a1 + 72` | `legacyObject.name` | string | confirmed | 读写均为 string |
| `a1 + 80/88/96/104/112` | `legacyObject.raw.refs[]` | ref[5] | confirmed | 读写均连续 ptr |
| `a1 + 120` | `legacyObject.raw.int120` | int | confirmed | 读写均 int |
| `a1 + 128` | `legacyObject.raw.versionedIntOrReal128` | int/real | confirmed | 版本 `<807` 读写 real 并按 1000 换算，否则 int |
| `a1 + 136` | `legacyObject.raw.int136` | int | confirmed | 读写均 int |
| `a1 + 144` | `legacyObject.raw.vector144` | vector | confirmed | 读写 vector，读入后 normalise |
| `a1 + 168/192` | `legacyObject.raw.positions[]` | position[2] | confirmed | 读写 position |
| `a1 + 220/132` | `legacyObject.raw.int220And132` | int[2] | confirmed | 读写均 int，顺序在 vector 后 |
| `a1 + 224/240/248/256/280..320` | `legacyObject.raw.realFields[]` | real[] | confirmed | 多个 real 读写成对 |
| `a1 + 364/368/372/380` | `legacyObject.raw.flags[]` | bool/int | confirmed | logical / version branch |
| `a1 + 328/332/336/340/344/348/352/356/360` | `legacyObject.raw.indexOrNumberFields[]` | int[] | inferred | 读写有编码规则，业务名未闭合 |
| `a1 + 384/388` | `legacyObject.raw.versionedTailFields[]` | int[] | confirmed | 版本 `>=842` 或 `==805` |
| `name contains Varies_ShangH_LZ` | `legacyObject.raw.variesShanghLzBlock` | object | confirmed | 特殊名称触发附加字段 |
| `a1 + 396` | `legacyObject.raw.variesFlag` | bool | confirmed | 特殊块内 logical |
| `a1 + 400/424` | `legacyObject.raw.variesUnitVectors[]` | vector[2] | confirmed | 读 `unit_vector`，写 `vector` |
| `a1 + 216` | `legacyObject.raw.variesInt216` | int | confirmed | 特殊块内 int |
| `a1 + 504..512` / `528..536` | `legacyObject.raw.variesStringLists[]` | string[][] | confirmed | 特殊块内写两个字符串列表 |

旧版本编码规则：

- 版本 `<807`：`+128` 以 real 米制读写，内存为 `int(mm)`。
- 版本 `802/803`：`+372` 读写 int。
- 版本 `<804`：旧文件会额外读写两个 real，占位兼容。
- 版本 `>=827`：读写 `+376`。
- 版本 `>=828`：读写 `+124` logical。
- 版本 `>=833`：读写 `+380` logical。
- 版本 `>=842`：读写 `+384`。
- 版本 `==805`：读写 `+388`。
- `+328/+332` 如果读入值 `>2`，旧代码会减 `3` 并设置 `+340/+344` 标志；版本 `>=838` 还会读 `+348/+352`。
- 版本 `>=839`：读写 `+356/+360`。

新格式补充字段：

| 新格式字段 | 来源 | 状态 | 说明 |
|---|---|---|---|
| `groupId` | 新系统稳定 ID / Detail `groupID` | required | 生成规则待和旧图石对齐。 |
| `rsdId` | Detail `rsdID` | required | 下料表和工程图必须共用。 |
| `displayNumber` | Detail `stbNum` / 旧运行确认 | pendingLegacyEvidence | 编号空号、加撇规则未闭合。 |
| `sourceSelection` | OCCT / STEP | required | 保存来源 face / edge / reference object。 |
| `geometryRef.groupCurves[]` | OCCT / BRep | required | 保存组级几何引用或可重建参数。 |
| `legacyEvidence[]` | 文档证据 ID | required | 至少记录 SFL / IDA / Detail 来源。 |

开发约束：

- Detail 字段可作为 `RebarGroup` 的输出映射，但不能反过来当完整旧对象字段。
- `a1 + 328/332/340/344/...` 很可能和编号或首末增减根数有关，但还不能命名成确定业务字段。
- `Varies_ShangH_LZ` 是特殊分支，必须保留 raw block，不能丢弃。

### seg_steelbargroup -> RebarSegmentGroup / SegmentCurveRule

旧证据：

- 字符串：`seg_steelbargroup`。
- 源码路径：`e:\tushi3d\dam\class\seg_steelbargroup.cpp`。
- IDA 生成链：`sub_1404D10C0 -> sub_140451730 -> sub_1405D5670`。
- IDA 保存函数：`sub_1405DA720`。
- IDA 读取函数：`sub_1405DA5D0`。
- IDA restore 包装入口：`sub_1405D5600`。
- IDA 类注册 / 调试列表函数：`sub_1405DA0E0`。
- SFL 可见记录头基本稳定。

已确认运行内存 / 算法字段：

| 旧偏移 / 规则 | 新格式字段建议 | 类型 | 状态 | 证据 |
|---|---|---|---|---|
| `+72` | `legacyObject.edgeHandleRole` / `geometryRef.curve` | ref | confirmed in memory | `api_edge(entity+72)`、`sub_1405BD0C0` 写回 |
| `+80` | `legacyObject.ownerOrAttachNode` | ref | inferred | 多处父级 / 链表入口 |
| `+88` | `legacyObject.nextOrChildNode` | ref | inferred | 遍历组内边、链表尾节点 |
| `+96` | `legacyObject.nextRefNode` | ref | inferred | `sub_1405D5670` 循环 |
| `+104` | `legacyObject.createdPayload` | ref | inferred | 创建后传入修正链 |
| `0.002` | `rules.minCreateDistance` | real | confirmed | `sub_1404D10C0` |
| `0.01` | `rules.minEdgeLength` | real | confirmed | split 后边长阈值 |
| `0.1` | `rules.nearEndpointDistance` | real | confirmed | 两段端点接近阈值 |
| `-0.03` | `rules.trimDelta` | real | confirmed | 端部裁剪 |
| `0.001 / 0.999` | `rules.safeCurveParams` | real[2] | confirmed | 避开端点采样 |
| `max(5, length * 50)` | `rules.splineSampleCount` | expression | confirmed | 样条重建采样 |

SFL 可见记录头：

| 旧保存偏移 / SFL 可见字段 | 新格式字段建议 | 状态 | 证据 |
|---|---|---|---|
| `a1 + 68` / `segIndex` | `legacyObject.raw.segmentIndex` | confirmed | `sub_1405DA720` 先写 int，与 `段_N` 对应 |
| `a1 + 72` / `str("段_N")` | `legacyObject.raw.displayName` | confirmed | `sub_1405DA720` 写 string，SFL 可见 |
| `a1 + 80` | `legacyObject.raw.ref80` | pendingLegacyEvidence | 保存函数第 1 个 ptr |
| `a1 + 96` | `legacyObject.raw.ref96` | pendingLegacyEvidence | 保存函数第 2 个 ptr |
| `a1 + 88` | `legacyObject.raw.ref88` | pendingLegacyEvidence | 保存函数第 3 个 ptr，注意顺序在 `+96` 后 |
| `a1 + 64` / `tailInt` | `legacyObject.raw.segmentRoleOrState` | pendingLegacyEvidence | 保存函数最后写 int，只见 `0..3` |

注意：

- `seg_steelbargroup` 的保存和读取顺序均为 `+68 -> +72 -> +80 -> +96 -> +88 -> +64`。
- 这和直观看偏移大小排序不同，新格式必须保存 `legacyWriteOrder`。
- 持久化 `+72` 已确认是字符串字段，对应 `段_N`。
- 算法链中看到的 `entity + 72` 几何边用法，不能直接和持久化字段 `+72` 合并解释。
- `ref80/ref96/ref88/+64` 业务名仍不能写死。

新格式补充字段：

| 新格式字段 | 来源 | 状态 | 说明 |
|---|---|---|---|
| `segmentId` | 新系统稳定 ID / Detail `segID` | required | 必须能映射 `StbGeo.segID`。 |
| `barId` | `steelbar` 关系 | pendingLegacyEvidence | 父子关系未全闭合。 |
| `geometryRef.curve` | OCCT curve / edge | required | 不能保存旧 ACIS 指针。 |
| `rules` | IDA 算法证据 | required | 线/弧组复刻必须保存或可由命令参数重建。 |
| `derived.length` | OCCT 计算 | derived | 可重新计算，但要和下料一致。 |

开发约束：

- `seg_steelbargroup` 不是普通字段表，它包含分段曲线修正规则。
- 新格式要保存足够信息，使 OCCT 可重建 split / spline / trim 后的曲线。
- 业务规则来自旧 IDA，几何曲线承载来自 OCCT。
- `legacyObject.raw` 记录旧保存顺序，`geometryRef.curve` 记录 OCCT 曲线承载，两者不能互相替代。
- 持久化 `legacyObject.raw.string72DisplayName` 不能被当作几何边引用；几何引用必须进入 `geometryRef.curveRefs`。

### Detail StbGroup / StbGeo -> Drawing Mapping

旧证据：

- Detail 样例包存在 `StbGroup%d`、`StbGeo%d`。
- IDA 写出函数命中 `StbGeo%d`：`sub_14063B010`。
- `13_Detail字段映射矩阵.md` 已列出 `StbGroup -> RebarGroup`、`StbGeo -> RebarSegment`。

新格式必须保存的映射：

| Detail 字段族 | 新格式字段 | 状态 | 说明 |
|---|---|---|---|
| `StbGroup.groupID` | `RebarGroup.groupId` | required | 工程图组 ID。 |
| `StbGroup.rsdID` | `RebarGroup.rsdId` | required | 下料表关联主键。 |
| `StbGeo.segID` | `RebarSegment.segmentId` | required | 图形段 ID。 |
| `StbGeo.start/end/middle` | `RebarSegment.geometryRef` / `derived.points` | required | 由 OCCT 曲线或保存点派生。 |
| `StbTable.rsdID` | `ScheduleRow.rsdId` | required | 下料表必须追回组。 |

开发约束：

- Detail writer 只能从新设计文件中的 `RebarModel` 派生。
- 不能让工程图 writer 重新生成一套与业务模型无关的钢筋编号。

## 与父目录源码的关系

父目录已有工程保存代码可以参考：

- JSON schema 组织方式。
- 保存事务。
- 失败回滚。
- 单元测试。
- 工程包目录组织。

不能参考成事实：

- 不能默认沿用父目录扩展名。
- 不能默认沿用父目录字段名。
- 不能默认沿用父目录钢筋对象语义。
- 不能用父目录测试关闭旧图石证据缺口。

正确做法：

```text
旧图石证据决定业务字段。
OCCT / STEP 决定几何引用和拓扑承载。
父目录源码只帮我们少写底层样板代码。
```

## 保存事务原则

保存必须是事务式：

```text
1. 写临时文件
2. 校验 JSON / 包结构可读
3. 校验核心对象引用完整
4. 原子替换正式文件
5. 成功后清 dirty
6. 失败时保留旧文件和 dirty
```

这个原则可以参考父目录实现，但字段和对象语义仍以旧图石证据为准。

## 读取原则

读取流程：

```text
1. 读取 manifest
2. 读取工程数据
3. 校验版本和必填字段
4. 读取 STEP/STP 或提示模型缺失
5. 重建拓扑 fingerprint
6. 恢复结构模型、钢筋模型、工程图模型
7. 对 pendingLegacyEvidence 字段给出开发期警告
```

如果 STEP 拓扑匹配失败：

- 工程仍可打开。
- 依赖 face / edge 的钢筋编辑必须禁用或进入修复流程。
- 不能静默把钢筋挂到错误几何对象上。

## 验收口径

Phase 1 前：

- 能保存和读取工程元信息、STEP/STP 引用、参考对象。
- 能记录字段证据状态。
- 保存失败不破坏旧文件。

Phase 2 前：

- 能保存和读取 `steelData / steelbargroup / steelbar / segment` 的复刻模型。
- 字段能追溯到 SFL / IDA / Detail 证据。
- 内部 ID 能映射 Detail writer 所需 ID。

Phase 3 前：

- 创建命令生成的钢筋对象能完整保存和恢复。
- 面配筋、线配筋、弧形组至少各有专项字段映射。
- 未确认字段不能影响旧图石对照验收。

## 仍需关闭的缺口

- `GAP-DEV-001`：新设计文件格式字段继续按旧图石 SFL / IDA / Detail 证据和 OCCT / STEP 几何承载能力校准。
- `GAP-DEV-002`：STEP face / edge 稳定选择 ID。
- `GAP-DEV-003`：撤销 / 重做事务日志是否保存。
- `GAP-DEV-004`：dirty 标记与旧图石保存提示口径。
- `GAP-DEV-005`：内部 ID 与旧编号、Detail ID 的关系。
- `GAP-SFL-003`：`steelData` 字段业务名。
- `GAP-SFL-004`：`steelbargroup` 大字段表。
- `GAP-SFL-005`：`seg_steelbargroup` SFL 字段名。

## 当前结论

新设计文件格式是必要的。

但它不能成为“重新开发一套新钢筋软件”的入口。

后续所有工程文件设计按这条线走：

```text
新文件格式替代 .sfl 主保存文件壳。
SFL / IDA / Detail 决定旧图石业务语义和字段证据。
OCCT / STEP 决定几何引用和拓扑承载。
新工程包负责保存读取、事务回滚和证据追溯。
父目录源码只能参考事务和测试写法。
```
