# 新设计文件格式 Schema 与 Fixture 草案

## 目标

本文件把 `18_新设计文件格式替代SFL策略.md` 落到可开发层。

它回答：

```text
Project.Save / Project.Open 第一版到底写什么文件？
每个文件有哪些必填字段？
怎么证明它不是空壳格式？
怎么证明它同时结合了 SFL 旧业务对象和 OCCT / STEP 几何承载？
```

核心结论：

```text
新格式不是 SFL 二进制复刻。
新格式也不是纯 OCCT 自研模型。
新格式必须保存 legacyObject.raw + geometryRef + binding + evidence。
```

开发期工作名暂用：

```text
*.tsrebar
```

正式扩展名 Phase 1 初始化前冻结。

硬边界：

- 不沿用父目录 `.tsrproj`。
- 不沿用父目录 `.vtsproj`。
- 不把父目录工程格式字段当业务事实。
- 字段证据以旧图石运行、IDA、SFL、Detail 为准。

## 联合格式定位

本格式按三层理解：

```text
legacyObject.raw
  -> SFL / IDA / Detail 反推的旧图石业务对象和字段保真区

geometryRef
  -> OCCT / STEP / BRep 提供的几何、拓扑、曲线和选择引用区

binding
  -> 旧图石业务对象与 OCCT 几何引用的绑定区

evidence / unresolved_fields
  -> 证据追溯、低置信字段和开发期缺口治理区
```

因此首期 schema 不能只验证 JSON 能保存。

它必须验证：

- 旧图石对象骨架没有被新业务字段覆盖。
- OCCT 几何引用没有被旧 ACIS 指针替代。
- `legacyObject.raw` 和 `geometryRef` 不是孤立保存，而是有可校验绑定。
- 每个核心字段能追到证据或进入 unresolved。
- Detail 映射从同一份 `RebarModel` 派生。

## 工程包结构

首期按 zip 包或同构目录开发，目录结构固定：

```text
project.tsrebar
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

首期不写：

- 老 `.sfl` ACIS BinaryFile。
- HOOPS 私有显示状态。
- DWG / DXF。
- 父目录旧工程格式文件。

## 统一对象结构

所有可保存领域对象都必须遵守这个外形：

```json
{
  "id": "stable-new-id",
  "kind": "RebarGroup",
  "legacyObject": {
    "type": "steelbargroup",
    "sourceClass": "e:\\tushi3d\\dam\\class\\steelbargroup.cpp",
    "readWriteEvidence": ["E-IDA-015", "E-IDA-016"],
    "sflObjectName": "面配筋_1",
    "raw": {
      "schema": "legacy-steelbargroup-v1",
      "legacyVersion": 842,
      "legacyWriteOrder": [],
      "fields": {}
    }
  },
  "geometryRef": {
    "kernel": "OCCT",
    "sourceStepId": "step-main",
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

字段含义：

- `id`：新系统稳定 ID，用于保存、撤销、查询、工程图映射。
- `kind`：新系统领域对象类型。
- `legacyObject.type`：旧图石对象名，如 `steelData / steelbar / steelbargroup / seg_steelbargroup`。
- `legacyObject.raw`：旧图石读写顺序和字段保真区。
- `geometryRef`：OCCT / STEP 几何引用区。
- `binding`：旧图石对象与 OCCT 几何引用的绑定、状态和证据。
- `derived`：长度、端点、包围盒等可重算值。
- `evidence`：字段级证据索引。

核心钢筋对象不允许空 `evidence`。

空 `evidence` 只允许非核心临时对象，且不能进入正式保存验收。

开发禁令：

- 不能删除 `legacyObject.raw`，只留漂亮业务字段。
- 不能用 `geometryRef` 替代旧图石业务编号。
- 不能只保存 `legacyObject.raw` 和 `geometryRef` 两堆数据，却没有 `binding` 说明二者如何对应。
- 不能把 `derived` 当唯一主数据。
- 不能用 Detail 派生产物反向覆盖业务对象。

## manifest.json

用途：

- 判断包版本。
- 判断 schema 版本。
- 判断必需文件是否齐全。
- 判断保存是否完成。

草案：

```json
{
  "format": "tsrebar",
  "formatVersion": 1,
  "schemaVersion": "2026-06-04-draft",
  "createdBy": "TushiRebarClone",
  "units": {
    "length": "m",
    "angle": "rad"
  },
  "requiredFiles": [
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
  ],
  "evidenceBaseline": {
    "ida": ["E-IDA-015", "E-IDA-016"],
    "sfl": ["E-SFL-001"],
    "detail": ["E-DETAIL-001", "E-DETAIL-002"],
    "dev": ["E-DEV-002", "E-DEV-003", "E-DEV-004", "E-DEV-005"]
  },
  "saveTransaction": {
    "state": "complete",
    "dirtyCleared": true
  }
}
```

必填校验：

- `format == "tsrebar"`。
- `formatVersion >= 1`。
- `requiredFiles` 全存在。
- `saveTransaction.state == "complete"`。

## project.json

用途：

- 工程级入口。
- dirty 摘要。
- 顶层对象索引。

草案：

```json
{
  "projectId": "proj-001",
  "projectName": "边墩样例",
  "sourceModelId": "step-main",
  "dirtyFlags": {
    "projectDirty": false,
    "geometryDirty": false,
    "rebarDirty": false,
    "drawingDirty": true
  },
  "models": {
    "structure": "geometry/source_step.json",
    "rebar": {
      "steelData": "rebar/steel_data.json",
      "groups": "rebar/groups.json",
      "bars": "rebar/bars.json",
      "segments": "rebar/segments.json"
    },
    "drawing": "drawing/detail_mapping.json"
  }
}
```

开发约束：

- `projectId` 是新系统 ID，不是旧图石编号。
- `dirtyFlags` 的旧图石保存提示口径仍在 `GAP-DEV-004`。
- 打开工程时不能因为 STEP 缺失而丢失钢筋业务对象。

## geometry/source_step.json

用途：

- 保存 STEP/STP 源模型引用。
- 保存 hash。
- 给 `geometryRef` 提供来源锚点。

草案：

```json
{
  "sourceStepId": "step-main",
  "path": "models/source.step",
  "originalPath": "C:/project/source.step",
  "sha256": "pending-real-hash",
  "importKernel": "OCCT",
  "importOptions": {
    "unitMode": "from-file",
    "readNames": true,
    "readColors": true
  }
}
```

缺口：

- 真实 STEP hash 和路径策略需要 Phase 1 spike。
- STEP 缺失时的修复流程需要 UI 契约。

## geometry/topology_refs.json

用途：

- 保存 face / edge / vertex 的稳定引用。
- 给钢筋来源面、方向边、曲线段提供恢复锚点。

草案：

```json
{
  "topologyRefs": [
    {
      "topologyId": "face-001",
      "shapeType": "face",
      "sourceStepId": "step-main",
      "occtTransientIndex": 123,
      "fingerprint": {
        "bbox": [0.0, 0.0, 0.0, 12.0, 4.0, 2.0],
        "area": 48.0,
        "center": [6.0, 2.0, 1.0],
        "normalHint": [0.0, 0.0, 1.0]
      },
      "evidence": ["GAP-DEV-002"]
    }
  ]
}
```

开发约束：

- `occtTransientIndex` 只能做调试辅助，不能当稳定 ID。
- `fingerprint` 需要通过 `19_STEP选择ID稳定性Spike计划.md` 验证。
- 拓扑匹配失败时，不能静默把钢筋挂到错误面或边。

## rebar/steel_data.json

用途：

- 保存旧 `steelData` 业务对象保真字段。

首期 schema：

```json
{
  "items": [
    {
      "id": "steel-data-001",
      "kind": "RebarSteelData",
      "legacyObject": {
        "type": "steelData",
        "sourceClass": "e:\\tushi3d\\dam\\class\\steeldata.cpp",
        "readWriteEvidence": ["E-IDA-015", "E-IDA-016"],
        "raw": {
          "schema": "legacy-steelData-v1",
          "legacyVersion": 841,
          "fields": {
            "name": "HPB300",
            "int72": 0,
            "int76": 0,
            "int80": 0,
            "real84_120": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            "int124_140": [0, 0, 0, 0, 0],
            "flag144": false,
            "flag148": false,
            "conditionalInt152": null,
            "conditionalInt156": null,
            "version841Int160": 0,
            "version841Int164": 0
          }
        }
      },
      "business": {
        "gradeName": "pendingLegacyEvidence",
        "diameterSet": []
      },
      "evidence": ["E-IDA-015", "E-IDA-016", "GAP-SFL-003"]
    }
  ]
}
```

注意：

- `business.gradeName` 暂不能反推旧字段。
- 字段命名先保留偏移名。
- 运行窗口闭合后再把 raw 字段提升成业务字段。

## rebar/groups.json

用途：

- 保存旧 `steelbargroup`。
- 保存新系统 `RebarGroup`。
- 连接 Detail `StbGroup` 和 OCCT 几何引用。

首期 schema：

```json
{
  "items": [
    {
      "id": "group-001",
      "kind": "RebarGroup",
      "groupId": "detail-group-001",
      "rsdId": "rsd-001",
      "legacyObject": {
        "type": "steelbargroup",
        "sourceClass": "e:\\tushi3d\\dam\\class\\steelbargroup.cpp",
        "readWriteEvidence": ["E-IDA-015", "E-IDA-016"],
        "sflObjectName": "面配筋_1",
        "raw": {
          "schema": "legacy-steelbargroup-v1",
          "legacyVersion": 842,
          "fields": {
            "name72": "面配筋_1",
            "refs80_112": ["ref80", "ref88", "ref96", "ref104", "ref112"],
            "int120": 0,
            "versioned128": 0,
            "int136": 0,
            "vector144": [0.0, 0.0, 1.0],
            "positions168_192": [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0]],
            "int220": 0,
            "int132": 0,
            "real224_320": [],
            "flags364_380": {},
            "indexOrNumberFields328_360": {},
            "versionedTail384_388": {},
            "variesShanghLzBlock": null
          }
        }
      },
      "geometryRef": {
        "kernel": "OCCT",
        "sourceStepId": "step-main",
        "topologyRefs": ["face-001"],
        "curveRefs": []
      },
      "binding": {
        "state": "resolved",
        "items": [
          {
            "role": "sourceFace",
            "legacyPath": "legacyObject.raw.fields.positions168_192",
            "geometryPath": "geometryRef.topologyRefs[0]",
            "status": "pendingLegacyEvidence",
            "evidence": ["E-IDA-016", "GAP-DEV-002"]
          }
        ]
      },
      "detailMapping": {
        "stbGroupId": "detail-group-001",
        "rsdId": "rsd-001",
        "componentName": "pendingLegacyEvidence",
        "stbType": "pendingLegacyEvidence",
        "steelWay": "pendingLegacyEvidence",
        "stdRuns": []
      },
      "evidence": ["E-IDA-015", "E-IDA-016", "E-IDA-018", "E-DETAIL-001", "GAP-SFL-004"]
    }
  ]
}
```

旧版本规则必须进入 validator：

- 版本 `<807` 时 `+128` 是 real 米制和 int 毫米制换算。
- `+328/+332` 读入值 `>2` 时，旧代码减 `3` 并设置 `+340/+344`。
- 版本 `>=838` 时读写 `+348/+352`。
- 版本 `>=839` 时读写 `+356/+360`。
- `Varies_ShangH_LZ` 特殊块不能丢。

## rebar/bars.json

用途：

- 保存旧 `steelbar`。
- 保存新系统 `Rebar`。

首期 schema：

```json
{
  "items": [
    {
      "id": "bar-001",
      "kind": "Rebar",
      "groupId": "group-001",
      "legacyObject": {
        "type": "steelbar",
        "sourceClass": "e:\\tushi3d\\dam\\class\\steelbar.cpp",
        "readWriteEvidence": ["E-IDA-015", "E-IDA-017"],
        "sflObjectName": "1根钢筋",
        "raw": {
          "schema": "legacy-steelbar-v1",
          "fields": {
            "name64": "1根钢筋",
            "ref72": "pendingLegacyEvidence",
            "ref80": "pendingLegacyEvidence",
            "ref88": "pendingLegacyEvidence",
            "int96": 0,
            "int100": 0
          }
        }
      },
      "geometryRef": {
        "kernel": "OCCT",
        "curveRefs": ["curve-seg-001"]
      },
      "binding": {
        "state": "resolved",
        "items": [
          {
            "role": "barCurve",
            "legacyPath": "legacyObject.raw.fields.ref72",
            "geometryPath": "geometryRef.curveRefs[0]",
            "status": "pendingLegacyEvidence",
            "evidence": ["E-IDA-017", "GAP-SFL-005"]
          }
        ]
      },
      "segmentIds": ["segment-001"],
      "evidence": ["E-IDA-015", "E-IDA-017", "GAP-SFL-005"]
    }
  ]
}
```

缺口：

- `steelbar` save/read 顺序已闭合。
- `ref72/ref80/ref88/int96/int100` 不能命名为 group/style/segment/编号字段。

## rebar/segments.json

用途：

- 保存 `seg_steelbargroup`。
- 保存段级曲线引用和线/弧组规则。

首期 schema：

```json
{
  "items": [
    {
      "id": "segment-001",
      "kind": "RebarSegment",
      "legacyObject": {
        "type": "seg_steelbargroup",
        "sourceClass": "e:\\tushi3d\\dam\\class\\seg_steelbargroup.cpp",
        "readWriteEvidence": ["E-IDA-013", "E-IDA-016", "E-IDA-017"],
        "sflObjectName": "段_1",
        "raw": {
          "schema": "legacy-seg-steelbargroup-v1",
          "legacyWriteOrder": ["int68", "string72", "ptr80", "ptr96", "ptr88", "int64"],
          "fields": {
            "int68SegmentIndex": 1,
            "string72DisplayName": "段_1",
            "ptr80": "pendingLegacyEvidence",
            "ptr96": "pendingLegacyEvidence",
            "ptr88": "pendingLegacyEvidence",
            "int64RoleOrState": 0
          }
        }
      },
      "geometryRef": {
        "kernel": "OCCT",
        "curveRefs": [
          {
            "curveId": "curve-seg-001",
            "type": "edge",
            "sourceTopologyId": "edge-001"
          }
        ]
      },
      "binding": {
        "state": "resolved",
        "items": [
          {
            "role": "segmentCurve",
            "legacyPath": "legacyObject.raw.fields.string72DisplayName",
            "geometryPath": "geometryRef.curveRefs[0]",
            "status": "pendingLegacyEvidence",
            "evidence": ["E-IDA-013", "E-IDA-017", "GAP-IDA-007"]
          }
        ]
      },
      "rules": {
        "minCreateDistance": 0.002,
        "minEdgeLength": 0.01,
        "nearEndpointDistance": 0.1,
        "trimDelta": -0.03,
        "safeCurveParams": [0.001, 0.999],
        "splineSampleCount": "max(5, length * 50)"
      },
      "derived": {
        "length": "computed-by-OCCT"
      },
      "evidence": ["E-IDA-013", "E-IDA-016", "E-IDA-017", "GAP-IDA-007"]
    }
  ]
}
```

强约束：

- `legacyWriteOrder` 必须保留，不能按偏移或 JSON 字段名排序。
- `geometryRef.curveRefs` 承载 OCCT 曲线，不保存旧 ACIS 指针。
- `binding.items` 必须说明段对象与 OCCT 曲线的对应角色。
- `rules` 来自旧 IDA 证据，不是 OCCT 自由设计。
- 持久化 `string72DisplayName` 是段名，不是几何边；几何边必须放入 `geometryRef.curveRefs`。

## drawing/detail_mapping.json

用途：

- 连接新模型和旧 AutoCAD 插件 Detail 包。
- 保证工程图编号、下料表和模型对象同源。

首期 schema：

```json
{
  "groups": [
    {
      "rebarGroupId": "group-001",
      "stbGroupId": "detail-group-001",
      "rsdId": "rsd-001",
      "detailFields": {
        "diameter": "pendingLegacyEvidence",
        "interval": "pendingLegacyEvidence",
        "barcount": "pendingLegacyEvidence",
        "segcount": 1,
        "stbLevel": "pendingLegacyEvidence",
        "stbLayer": "pendingLegacyEvidence",
        "stbProfile": "pendingLegacyEvidence",
        "stbUse": "pendingLegacyEvidence"
      }
    }
  ],
  "segments": [
    {
      "rebarSegmentId": "segment-001",
      "stbGeoSegId": "segment-001",
      "sourceCurveRef": "curve-seg-001"
    }
  ],
  "tables": [
    {
      "rsdId": "rsd-001",
      "sourceGroupId": "group-001"
    }
  ]
}
```

强约束：

- Detail writer 只能从 `RebarModel` 派生。
- 下料表不能另起一套编号。
- `StbGroup.groupID / StbTable.rsdID / StbGeo.segID` 必须能追回新系统对象。

## evidence/evidence_index.json

用途：

- 保存字段证据和置信度。
- 让低置信字段不能伪装成已确认。

草案：

```json
{
  "items": [
    {
      "id": "E-IDA-016",
      "type": "IDA",
      "summary": "steelData/steelbargroup read 顺序，seg_steelbargroup 保存顺序",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-IDA-017",
      "type": "IDA",
      "summary": "steelbar read 函数 sub_1405E1AB0 与 seg_steelbargroup read 函数 sub_1405DA5D0，确认二者 save/read 顺序成对",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-IDA-018",
      "type": "IDA",
      "summary": "Detail StbGroup 写出函数 sub_14063E910，确认 StbGroup 字段、Std%d 子节点、StbGeo 调用和 FaceEdge 点状分支",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-IDA-019",
      "type": "IDA",
      "summary": "Detail StbTable / MaterialTable 导出链 sub_140605B20 -> sub_140602F90，确认 StbRow 和材料汇总字段写值 helper",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-IDA-020",
      "type": "IDA",
      "summary": "下料表内部命令绑定 psexcel -> 0x8D1C -> sub_140605B20，Ribbon UI 和运行输出仍需补强",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-IDA-021",
      "type": "IDA",
      "summary": "Ribbon 构造函数确认 工程图 / 输出 分组加入 0x8D1C；结合 E-IDA-020 可追到 psexcel / sub_140605B20，caption、Dialog 和运行输出仍需确认",
      "sourceDoc": "03_IDA命令证据.md",
      "confidence": "C2"
    },
    {
      "id": "E-DEV-002",
      "type": "DevelopmentContract",
      "summary": "新设计文件格式 schema / fixture / 回滚契约",
      "sourceDoc": "24_新设计文件格式Schema与Fixture草案.md",
      "confidence": "C2"
    },
    {
      "id": "E-DEV-004",
      "type": "DevelopmentContract",
      "summary": "新设计文件格式 legacyObject.raw 与 OCCT geometryRef 绑定契约，确认 binding.state、binding.items、legacyPath、geometryPath 和绑定失败处理",
      "sourceDoc": "18_新设计文件格式替代SFL策略.md, 24_新设计文件格式Schema与Fixture草案.md",
      "confidence": "C2"
    },
    {
      "id": "E-DEV-005",
      "type": "DevelopmentContract",
      "summary": "首期可验证联合格式 Fixture A/B/C、negative fixture、validator 错误码、失败输出格式、Save/Open 事务测试和 STEP diff 工件清单",
      "sourceDoc": "26_首期可验证联合格式Fixture与Validator执行清单.md",
      "confidence": "C2"
    }
  ]
}
```

## evidence/unresolved_fields.json

用途：

- 集中记录不能命名的字段。
- 防止开发中把猜测写成确定业务名。

首批：

```json
{
  "items": [
    {
      "objectType": "steelData",
      "fields": ["int72", "int76", "int80", "real84_120", "int124_140"],
      "gap": "GAP-SFL-003",
      "neededEvidence": "运行钢筋式样/材料窗口字段 + SFL 保存前后差异"
    },
    {
      "objectType": "steelbargroup",
      "fields": ["indexOrNumberFields328_360", "flags364_380", "variesShanghLzBlock"],
      "gap": "GAP-SFL-004",
      "neededEvidence": "Detail 字段对照 + 运行属性窗口 + 保存前后差异"
    },
    {
      "objectType": "seg_steelbargroup",
      "fields": ["ptr80", "ptr96", "ptr88", "int64RoleOrState"],
      "gap": "GAP-IDA-007",
      "neededEvidence": "线配筋/弧形筋保存前后差异 + 父子关系和 Detail 对照"
    },
    {
      "objectType": "steelbar",
      "fields": ["ref72", "ref80", "ref88", "int96", "int100"],
      "gap": "GAP-SFL-005",
      "neededEvidence": "Detail StbGroup/StbGeo 对照 + SFL 保存前后差异"
    }
  ]
}
```

## 首批 Fixture

### Fixture A：空工程 + STEP 引用

目的：

- 验证 `manifest/project/geometry/evidence` 首期保存。
- 不包含钢筋。

定位：

- Fixture A 只是包基础设施 fixture。
- 它不能单独证明新格式路线成立。
- 真正证明 SFL + OCCT 联合格式，必须依赖 Fixture B/C 的 `legacyObject.raw + geometryRef + binding + evidence`。

必须覆盖：

- `Project.Save` 原子写入。
- `Project.Open` 读回 STEP 引用。
- STEP 文件缺失时工程仍能打开。

### Fixture B：一个钢筋组 + 一根钢筋 + 一段线

目的：

- 验证 `steelData / steelbargroup / steelbar / seg_steelbargroup` 同时保存。
- 验证 `legacyObject.raw` 和 `geometryRef` 绑定。
- 验证 Detail `groupId / rsdId / segmentId` 映射。

必须覆盖：

- `groups.json` 引用 `bars.json`。
- `bars.json` 引用 `segments.json`。
- `segments.json` 引用 `geometryRef.curveRefs`。
- `detail_mapping.json` 追回 `group-001 / segment-001`。

### Fixture C：含 pendingLegacyEvidence 的复杂组

目的：

- 验证低置信字段治理。
- 验证 `Varies_ShangH_LZ` raw block 不丢。
- 验证打开时给开发期警告，但不阻塞读工程。

必须覆盖：

- `unresolved_fields.json` 至少含 `GAP-SFL-004 / GAP-IDA-007`。
- 读回后字段仍保留 raw 值。
- 不能把 pending 字段显示成已确认业务名。

## Validator 草案

首期至少需要这些校验：

```text
PackageValidator
  -> required files exist
  -> manifest format/version valid
  -> JSON parse valid
  -> schema version supported

ProjectReferenceValidator
  -> project references existing package files
  -> rebar group/bar/segment ID references valid
  -> detail mapping references valid rebar objects

LegacyObjectValidator
  -> legacyObject.type known
  -> legacyObject.raw.schema known
  -> evidence IDs exist
  -> pending fields listed in unresolved_fields.json

GeometryRefValidator
  -> sourceStepId exists
  -> topologyRefs exist or are explicitly unresolved
  -> no old ACIS pointer used as stable geometry ID

LegacyGeometryBindingValidator
  -> binding.state in resolved / unresolvedGeometry / pendingLegacyEvidence
  -> binding.items legacyPath points to an existing legacyObject.raw path or explicit unresolved field
  -> binding.items geometryPath points to an existing geometryRef path or explicit unresolved geometry state
  -> binding evidence IDs exist
  -> unresolvedGeometry cannot silently bind to another face / edge / curve

CoreObjectBindingValidator
  -> core rebar objects have legacyObject.raw
  -> core rebar objects have evidence IDs
  -> objects that need geometry have geometryRef or explicit unresolved geometry state
  -> pure geometryRef objects cannot enter formal RebarModel

DetailMappingValidator
  -> StbGroup.groupID maps back to RebarGroup.groupId
  -> StbGroup.rsdID and StbTable.rsdID are same-source
  -> StbGeo.segID maps back to RebarSegment.segmentId
  -> schedule rows cannot become orphan rows

EvidenceValidator
  -> evidence IDs exist
  -> GAP IDs are traceable
  -> pendingLegacyEvidence cannot be marked confirmed
  -> confirmed fields need SFL / IDA / Detail / runtime evidence

TransactionValidator
  -> temp package can be read before replace
  -> failed save preserves old package
  -> success clears dirty according to command contract
```

不能通过 validator 的包：

- 不能清 dirty。
- 不能覆盖旧工程包。
- 不能进入正式 Phase 1 开发验收。

## 保存 / 读取事务

保存：

```text
1. 写入临时目录或临时 zip
2. 写 manifest/project/geometry/rebar/drawing/evidence
3. 跑 PackageValidator
4. 跑 ProjectReferenceValidator
5. 跑 LegacyObjectValidator
6. 跑 GeometryRefValidator
7. 跑 LegacyGeometryBindingValidator
8. 跑 CoreObjectBindingValidator
9. 跑 DetailMappingValidator
10. 跑 EvidenceValidator
11. 跑 TransactionValidator
12. 临时包通过后原子替换正式包
13. 成功后清 ProjectDirty
14. 失败保留旧包和 dirty
```

读取：

```text
1. 读 manifest
2. 校验 requiredFiles
3. 读 project
4. 读 geometry/source_step 和 topology_refs
5. 读 rebar 对象
6. 恢复 legacyObject.raw
7. 绑定 geometryRef
8. 读 drawing/detail_mapping
9. 对 pendingLegacyEvidence 输出开发期警告
10. STEP 拓扑匹配失败时进入修复状态
```

## Phase 1 进入条件

进入 Qt6 + OCCT 工程开发前，`GAP-DEV-001` 至少要做到：

- 本文件 schema 草案冻结为 `schemaVersion`。
- Fixture A/B/C 有样例包。
- validator 清单变成开发任务。
- 保存失败回滚测试有用例设计。
- `legacyObject.raw` 字段不被业务字段替代。
- `geometryRef` 至少能挂 STEP face / edge / curve 的占位引用。
- `binding` 能证明 `legacyObject.raw` 与 `geometryRef` 不是孤立保存。

## 仍未关闭的缺口

- `GAP-DEV-001`：schema 草案、Fixture A/B/C、negative fixture、validator 脚本、首批 validation reports、standalone Save/Open reports 和首批 STEP diff reports 已落地；但 Qt6 应用内 Save/Open 和 binding 修复流程仍未实际实现。
- `GAP-DEV-002`：`E-DEV-017` 已产出首批真实 STEP 多轮导入 diff；`test.stp` 与 5 个 complex curve STEP 样本均通过，`step-shape-fingerprint/v1` 已消解复杂 edge/vertex raw Explorer 重复噪声；当前 Phase1 seed 样本不再阻塞 M1，但新增工程 STEP 样本仍需按同一 gate 运行后才能作为 binding 证据。
- `GAP-SFL-003`：`steelData` 字段业务名未闭合。
- `GAP-SFL-004`：`steelbargroup` 大字段业务名未闭合。
- `GAP-SFL-005`：`steelbar / seg_steelbargroup` 字段业务名未闭合。
- `GAP-DEV-004`：dirty 清除和旧图石保存提示口径未运行确认。

## 当前结论

这份 schema 草案已经足够指导 `Project.Save / Project.Open` 的第一轮设计。

但它仍只是开发契约，不是运行闭环。

下一步应做：

```text
24 schema 草案
  -> schema 文件 / fixture 样例
  -> validator 实现与 reports
  -> standalone Save/Open reports
  -> Qt6 application Save/Open 契约与 reports
  -> binding 修复契约与 reports
  -> 保存失败回滚测试
  -> STEP 选择 ID spike
```

具体开发任务拆解见：

```text
25_新设计文件格式SFL_OCCT联合开发任务拆解.md
```

Fixture 与 validator 执行清单见：

```text
26_首期可验证联合格式Fixture与Validator执行清单.md
```

Validator 实现契约、错误码总表、报告 schema 和 Save/Open gate 见：

```text
32_Validator实现契约与错误码总表.md
```
