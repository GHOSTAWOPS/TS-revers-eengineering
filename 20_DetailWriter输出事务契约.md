# Detail Writer 输出事务契约

## 目标

本文件把 `Drawing.GeneratePackage` 和 `Drawing.GenerateBarSchedule` 从“能写出 XML”升级为可开发、可测试、可回滚的 writer 契约。

核心结论：

```text
Detail writer 只负责把新系统领域模型派生成旧 AutoCAD 插件包。
它不能成为钢筋数据真相源。
它不能在校验失败时覆盖旧输出。
```

## 边界

writer 输出：

- `Detail.xml`
- `Detail01.stl`
- `Detail02.stl`
- 后续更多 `DetailNN.stl`

注意：

- `DetailNN.stl` 是 XML，根节点是 `DrawingRoot`。
- `DetailNN.stl` 不是三角网格 STL。
- 新系统首期不直接写 DWG / DXF。
- AutoCAD 2020 和旧图石插件是导入验证环境，不作为新程序运行时依赖。

## 输入快照契约

writer 必须接收一个只读快照，不能一边写包一边修改领域模型。

首期输入：

- `ProjectDocument`
- `StyleCatalog`
- `RebarModel`
- `DrawingModel`
- `DrawingPackageRequest`

`DrawingPackageRequest` 至少包含：

```json
{
  "packageId": "drawpkg_000001",
  "outputMode": "projectDerived",
  "outputDir": "drawings/",
  "selectedViewIds": ["view_000001"],
  "includeBarSchedule": true,
  "includeMaterialTable": true,
  "overwritePolicy": "replacePackage"
}
```

生成前必须检查：

- `RebarModel` 可读。
- `StyleCatalog` 可读。
- `DrawingModel` 至少有一个视图或剖切/投影视图。
- 被输出的 `RebarGroup`、`Rebar`、`RebarSegment` ID 完整。
- 被输出对象携带 `legacyObject/evidence` 状态，不能脱离旧图石证据单独生成。
- 被输出对象携带 `geometryRef/derived` 几何结果，长度、端点和投影来源可追溯。
- Detail ID 映射能追回同一份 `RebarModel`，不能在 writer 内另起编号真相。
- `StbTable` 和 `StbGroups` 能引用同一套 `RebarModel`。

以下情况必须 fail-fast：

- `groupId / barId / segmentId` 缺失。
- `RebarSegment` 没有关联 `Rebar`。
- `Rebar` 没有关联 `RebarGroup`。
- `DrawingView` 缺少必要的剖切方向、上方向或范围。
- 下料表字段需要的长度、根数、直径无法计算。

## 输出目录布局

工程内派生产物：

```text
新设计文件包/
  drawings/
    Detail.xml
    Detail01.stl
    Detail02.stl
```

`drawings/` 是新设计文件包内的派生产物区。

Detail 包不是工程主数据，不能反向覆盖 SFL 证据、`RebarModel` 或 OCCT `geometryRef`。

临时生成目录：

```text
新设计文件包/
  drawings/.tmp/
    run_YYYYMMDD_HHMMSS/
      Detail.xml
      Detail01.stl
```

审计记录：

```text
新设计文件包/
  audit/
    drawing_writer_runs.jsonl
```

外部导出目录可以由用户选择，但行为必须和工程内派生产物一致：

- 先写临时目录。
- 校验通过后替换目标包。
- 失败时保留原目标包。

## 文件命名规则

样式文件固定：

```text
Detail.xml
```

图纸文件：

```text
Detail01.stl
Detail02.stl
...
Detail99.stl
Detail100.stl
```

IDA 已确认规则：

- `E-IDA-026` 复核 `sub_140635A80(a1, viewIndex)`：
  - `viewIndex < 10` 使用 `\Detail0%d.stl`。
  - `viewIndex >= 10` 使用 `\Detail%d.stl`。
- 因此 `Detail01.stl ... Detail09.stl` 两位补零。
- `Detail10.stl` 以后使用自然数字。
- `Detail100.stl` 不是 `Detail0100.stl`。

实现规则：

- 编号按 `DrawingPackage.views[]` 顺序生成。
- 一个 `DrawingView` 对应一个 `DetailNN.stl`。
- 删除视图后重新生成时，允许重排 `DetailNN` 文件名，但领域模型 ID 不变。

待旧图石运行确认：

- 旧插件是否接受 `Detail100.stl`。
- 多图纸时旧插件是否要求 `CurrPos` 或额外索引字段。

## 字段必填和缺省策略

字段分三类。

### A 类：缺失即失败

- `StyleRoot`
- `DrawingRoot`
- `StbTables`
- `MaterialTable`
- `HViewPorts/ViewPort`
- `PartDetailDrawing/General-Info`
- `StbDetailDrawing/StbGroups`
- `StbGroup.rsdID`
- `StbGroup.groupID`
- `StbGroup.diameter`
- `StbGroup.barcount`
- `StbGroup.segcount`
- `StbGeo.segID`
- `StbGeo.shapeType`
- `StbGeo.start_x/y/z`
- `StbGeo.end_x/y/z`
- `StbTable.StbRow.rsdID`
- `StbTable.StbRow.length`
- `StbTable.StbRow.lenSum`

### B 类：允许默认值，但必须记录

- `CompanyName`
- `DrawingName`
- `DrawingUnit`
- `DrawingScale`
- `GeneralScale`
- `ComponentName`
- `PJSteelName`
- `SteelWay`
- `stbLevel`
- `stbLayer`
- `stbProfile`
- `stbUse`

默认策略：

- 名称类缺失写空字符串或旧样例默认值。
- 单位类默认按工程单位。
- 比例缺失默认 `1`，但写入 warning。
- 枚举类缺失写当前草案默认枚举，并写入 warning。

### C 类：首期可省略

- 隐藏线复杂字段。
- 填充线复杂字段。
- 尺寸标注复杂字段。
- 旧插件可容忍的扩展字段。

C 类字段省略必须有 Evidence / GAP 记录，不能被当作已完整复刻。

## ID 派生和一致性

ID 派生原则：

```text
RebarModel 内部 ID 是主真相。
Detail ID 从内部 ID 派生。
writer 不能重新发明一套独立编号。
```

首期映射：

```text
RebarGroup.groupId
  -> StbGroup.groupID

RebarGroup.rsdId
  -> StbGroup.rsdID
  -> StbRow.rsdID

RebarSegment.segmentId
  -> StbGeo.segID

RebarGroup.detailStdRuns[].segCount
  -> Std%d.segCount
```

IDA 证据补强：

```text
E-IDA-018
  -> StbGroup / Std / StbGeo 写出链

E-IDA-019
  -> StbTable / StbRow / MaterialTable 写值链

E-IDA-020
  -> psexcel / 0x8D1C / sub_140605B20 内部命令绑定

E-IDA-021
  -> 工程图 / 输出 Ribbon 分组加入 0x8D1C
```

说明：

- `E-IDA-020` 只能证明旧程序内部命令名、命令 ID 和 handler 的绑定。
- `E-IDA-021` 进一步证明 `0x8D1C` 位于 `工程图 / 输出` Ribbon 分组。
- 按钮 caption `下料表`、Dialog #427 和运行输出文件仍需运行确认。

校验规则：

- 每个 `StbGroup.rsdID` 必须能在 `StbTable` 找到对应行。
- 每个 `StbRow.rsdID` 必须能追回一个 `RebarGroup`。
- 每个 `StbGeo.segID` 必须能追回一个 `RebarSegment`。
- 每个 `Std%d.segCount` 必须等于该 `Std%d` 下实际 `StbGeo` 数量。
- `barcount` 必须等于派生出的钢筋根数。
- `segcount` 必须等于单根或组内分段规则派生结果。
- `lenSum` 必须等于明细长度汇总，允许浮点容差。

浮点容差首期建议：

```text
长度：1e-6 m
质量：1e-6 kg
```

## 生成事务

生成流程：

```text
1. 冻结输入快照。
2. 创建本次 runId。
3. 写入 drawings/.tmp/runId/。
4. 解析刚写出的 XML。
5. 执行包内一致性校验。
6. 校验通过后替换目标输出。
7. 写入 audit。
8. 成功后更新 DrawingPackage.generatedAt。
9. 成功后可清 DrawingDirty。
```

失败处理：

- 不覆盖旧 `Detail.xml`。
- 不覆盖旧 `DetailNN.stl`。
- 不清 `DrawingDirty`。
- 状态栏显示失败原因。
- audit 记录失败阶段和错误码。

替换策略：

- 同一磁盘内优先原子替换。
- 跨磁盘外部导出时使用“先复制到临时目录，再 rename 到目标名”。
- 如果替换到一半失败，必须回滚到旧包或明确标记目标目录不可用。

## 覆盖和清理策略

`overwritePolicy` 首期只允许：

- `replacePackage`：替换整个 Detail 包。
- `writeNewFolder`：写入新目录，不覆盖旧包。

禁止首期默认启用：

- 只覆盖单个 `DetailNN.stl`。
- 混用旧 `Detail.xml` 和新 `DetailNN.stl`。
- 写失败后保留半新半旧包并标记成功。

`replacePackage` 成功后：

- 删除目标目录中旧的 `DetailNN.stl` 多余文件。
- 保留非 Detail 文件。
- audit 记录删除了哪些旧 `DetailNN.stl`。

## 验证门禁

### L0：XML 文件级

检查：

- 文件存在。
- 编码可解析。
- `Detail.xml` 根节点是 `StyleRoot`。
- `DetailNN.stl` 根节点是 `DrawingRoot`。
- 必需节点存在。
- 数值字段能解析为数字。

### L1：包内一致性

检查：

- `StbGroups` 与 `StbTable` 的 `rsdID` 对齐。
- `StbGeo.segID` 唯一。
- 统计长度、根数、质量可复算。
- 图纸数量和 `DetailNN.stl` 文件数量一致。
- `DrawingView` 范围和剖切方向合法。

### L2：AutoCAD 插件导入

检查：

- AutoCAD 2020 可启动。
- `FDrawing.arx` / `FDrawingObj.dbx` 能加载。
- 旧插件能选择并导入新包。
- 导入后有图形对象、钢筋组和下料表。
- 失败时记录 AutoCAD 命令行、截图或插件报错。

只有 L2 通过，才能把工程图 writer 标为 C3 可验收证据。

## 错误码

首批错误码：

- `DW001_INPUT_MISSING_REBAR_MODEL`
- `DW002_INPUT_MISSING_DRAWING_VIEW`
- `DW003_ID_CROSS_REFERENCE_FAILED`
- `DW004_REQUIRED_FIELD_MISSING`
- `DW005_XML_WRITE_FAILED`
- `DW006_XML_PARSE_FAILED`
- `DW007_PACKAGE_VALIDATION_FAILED`
- `DW008_REPLACE_FAILED`
- `DW009_AUTOCAD_IMPORT_FAILED`

错误信息必须包含：

- `runId`
- 输出目录。
- 失败文件。
- 失败节点或字段。
- 关联 `Requirement ID`。
- 关联 `GAP ID`。

## 审计记录

每次生成写一行 JSONL：

```json
{
  "runId": "DW-20260604-001",
  "createdAt": "2026-06-04T00:00:00+08:00",
  "commandId": "Drawing.GeneratePackage",
  "outputDir": "drawings/",
  "files": ["Detail.xml", "Detail01.stl"],
  "sourceRebarGroupIds": ["grp_000001"],
  "sourceViewIds": ["view_000001"],
  "inputHash": "",
  "writerVersion": "0.1",
  "validation": {
    "l0": "passed",
    "l1": "passed",
    "l2": "not_run"
  },
  "warnings": [],
  "evidenceIds": ["E-DETAIL-001", "E-DETAIL-002"],
  "gapIds": ["GAP-DRAW-001", "GAP-DRAW-002"]
}
```

## 命令职责边界

`Drawing.GeneratePackage`：

- 生成 `Detail.xml`。
- 生成所有 `DetailNN.stl`。
- 可包含下料表和材料表。
- 成功后更新 `DrawingPackage` 派生物记录。

`Drawing.GenerateBarSchedule`：

- 负责弹出或应用下料参数。
- 负责更新下料表中间模型。
- 不单独写一套和工程图包脱节的编号。

首期推荐：

```text
GenerateBarSchedule 更新中间模型。
GeneratePackage 统一写 Detail 包。
```

这样可以避免下料表和图上钢筋双真相。

## 开发前门禁

进入 writer 实现前必须满足：

- `13_Detail字段映射矩阵.md` 的 A 类字段有模型来源。
- `09_钢筋领域模型草案.md` 能表达 `RebarGroup/Rebar/RebarSegment`。
- `18_新设计文件格式替代SFL策略.md` 能记录 `DrawingPackage`，并能把 Detail ID 映射回旧图石业务对象和 OCCT 几何引用。
- 至少能生成一个只有一张图的首期可验证包。
- AutoCAD 2020 + 旧插件导入步骤已记录到 `12_运行确认日志.md`。

未满足时，只能做 L0/L1 离线 writer spike，不能宣称工程图闭环。

## 当前 L0/L1 运行记录

当前已形成实际离线 gate：

```text
tools/detail_writer_gate/detail_writer_gate.py
tools/detail_writer_gate/test_detail_writer_gate.py
docs/phase1/detail_writer_reports/detail_writer_l0_l1_run_001.json
docs/phase1/detail_writer_reports/detail_writer_summary_001.md
docs/phase1/detail_writer_reports/generated_detail_package/
docs/phase1/detail_writer_reports/rollback_probe_package/
```

当前结论：

```text
decision: l0-l1-pass
L0 XML: passed
L1 same-source ID cross reference: passed
negative/detail_rsd_mismatch: DW003_ID_CROSS_REFERENCE_FAILED
rollbackOldPackagePreserved: true
AutoCAD L2 import: not claimed
```

这只说明 Detail writer L0/L1 离线包生成、ID 交叉引用和失败回滚探针通过。

它不说明 AutoCAD 2020 + 旧插件动态导入已经通过。

## M2-Drawing-001 多图纸 P0 状态

当前正式 app 已补：

```text
E-DEV-054
TODO-031 / M2-Drawing-001
```

已完成：

- `DetailWriteOptions.views` 支持多个 `DetailDrawingViewOptions`。
- `views` 为空时保持旧单图纸行为，输出 `Detail01.stl`。
- `views` 非空时按顺序输出 `Detail01.stl ... DetailNN.stl`。
- L0 / L1 校验遍历所有生成的 `DetailNN.stl`。
- `result.files` 返回 `Detail.xml` 和所有 `DetailNN.stl`。
- `replacePackage` 成功后删除旧多余 `DetailNN.stl`，保留非 Detail 文件。
- `replacePackage` 失败时恢复旧 `Detail.xml` 和旧 `DetailNN.stl`。

仍不声明：

```text
AutoCAD L2 import: not_run
剖切线 / 隐藏线 / 填充线: not_implemented
完整工程图: not_complete
旧插件 Detail100.stl 运行确认: not_run
```

## 仍需关闭的缺口

- `GAP-DRAW-001`：新包 AutoCAD 动态导入。
- `GAP-DRAW-002`：复杂字段完整性。
- `GAP-DRAW-004`：下料表和图上钢筋一致性。
- `GAP-IDA-005`：旧图石生成工程图入口。
- `GAP-IDA-006`：旧图石下料表入口。

## M2-Drawing-002 AutoCAD L2 验证准备状态

当前正式 app 已补：

```text
E-DEV-055
TODO-033 / M2-Drawing-002
```

已完成：

- 新增 `detail_l2_fixture_probe`，用正式 `DetailWriter` 生成 AutoCAD L2 验证用包。
- 生成 3 图纸 Detail 包：
  - `Detail.xml`
  - `Detail01.stl`
  - `Detail02.stl`
  - `Detail03.stl`
- `Detail.xml` 根节点是 `StyleRoot`。
- `DetailNN.stl` 根节点是 `DrawingRoot`。
- probe JSON 记录文件 hash、L0/L1/L2 状态和 GAP。
- 旧 FDrawing 插件文件存在，hash 已记录。
- 本机未发现 `acad.exe` / `accoreconsole.exe`，AutoCAD L2 自动导入未运行。
- 已形成手工 L2 导入确认清单。

仍不声明：

```text
AutoCAD L2 import: not_run
FDrawing command name: not_confirmed
section / hidden / hatch line: not_implemented
full drawing generation: not_complete
golden: not_collected
```

## M2-Drawing-003 复杂字段静态证据状态

当前正式证据已补：

```text
E-DETAIL-003
E-IDA-027
E-DEV-056
TODO-034 / M2-Drawing-003
```

已完成：

- 从旧 `Detail01.stl` 样例确认 `PartDetailDrawing` 复杂容器：
  - `continue-line`
  - `hidden-line`
  - `central-line`
  - `section-line`
  - `hatch-line`
  - `Others`
  - `steeljoint-line`
- 确认 `continue-line / hidden-line / section-line` 具备 `lines / circles / Arcs / Ellipses / EllipseArcs / Splines` 子容器。
- 确认 `section-line` 样例中实际出现 `LineN.start_x/start_y/end_x/end_y/ZValue` 和 `ArcN.center_x/center_y/center_z/radius/start_angle/end_angle/ZValue`。
- 确认点筋 `StbGeo shapeType=C` 的 `point_x/y/z`、`offset_x/y/z`、`offset_x2/y2/z2` 字段。
- 确认 `FaceEdge shapeType=L/A` 字段，其中圆弧分支含 `m_ArcDotReverse`。
- 用 IDA MCP 复核 `FDrawing.arx`，确认 `CViewInfo / CWSNLineDim / CWSNPointDim / CWSNSteelBarTable / CWSNMaterialTable / CWSNSectionTitle` 等插件对象符号存在。

后续 writer 扩展顺序：

```text
P0A: 输出复杂容器骨架
  continue-line / hidden-line / central-line / section-line /
  hatch-line / Others / steeljoint-line

P0B: 输出 General-Info 扩展字段
  DimensionPointBar* / DimensionLineBar* / DimensionLLineBar* /
  Range_XML* / CutPlaneDir0* / HalfView* / DrawTaoTong

P0C: 输出 pointStb / FaceEdge 字段骨架
  point_x/y/z / offset_x2/y2/z2 / FaceEdge L/A

P1: 真实剖切线 / 隐藏线 / 填充线 / 接头线算法
  必须另走 OCCT HLR / section 能力、IDA 或旧图石运行对照。
```

仍不声明：

```text
AutoCAD L2 import: not_run
FDrawing acceptance: unknown
section / hidden / hatch / joint algorithm: not_implemented
ZValue semantics: gap
FaceEdge generation rule: gap
full drawing generation: not_complete
golden: not_collected
```

## M2-Drawing-004 复杂字段骨架 P0 状态

当前正式 app 已补：

```text
E-DEV-057
TODO-035 / M2-Drawing-004
```

已完成：

- `DetailWriter` 输出 `PartDetailDrawing num="8"`。
- `DetailWriter` 输出复杂容器骨架：
  - `continue-line`
  - `hidden-line`
  - `central-line`
  - `section-line`
  - `hatch-line`
  - `Others`
  - `steeljoint-line`
- `continue-line / hidden-line / section-line` 输出 `lines / circles / Arcs / Ellipses / EllipseArcs / Splines` 空子容器。
- `central-line / hatch-line` 输出空 `lines`。
- `steeljoint-line` 输出空 `joints`。
- `General-Info` 输出首批扩展默认字段。
- `SteelBarSegmentShape::Point` 输出 `StbGeo shapeType=C` 的 `point_x/y/z`、`offset_x/y/z`、`offset_x2/y2/z2`。
- `detail_writer_tests` 覆盖复杂容器、默认字段、点筋字段、L2 不声明和既有失败回滚。

注意：

- `CompanyName` 当前写 `design-company`，只是中性占位，不是旧图石默认值闭合。
- `FaceEdge` 本轮未实现。
- 空容器不代表旧插件一定接受，也不代表真实工程图算法完成。

仍不声明：

```text
AutoCAD L2 import: not_run
FDrawing acceptance: unknown
section / hidden / hatch / joint algorithm: not_implemented
section-line LineN / ArcN geometry: not_generated
ZValue semantics: gap
FaceEdge generation: not_implemented
full drawing generation: not_complete
golden: not_collected
```

## M2-Drawing-005 AutoCAD L2复杂字段骨架导入验证状态

当前正式证据已补：

```text
E-DEV-058
TODO-036 / M2-Drawing-005
```

已完成：

- `detail_l2_fixture_probe` 生成 TODO-036 专用复杂字段骨架包。
- 输出 `Detail.xml + Detail01.stl + Detail02.stl + Detail03.stl`。
- probe JSON 记录文件 hash、根节点、L0/L1/L2 状态、复杂骨架检查结果和 GAP。
- `complexSkeleton.passed = true`。
- `complexSkeleton.scope = complex containers + General-Info only; pointStb L2 is deferred`。
- `FDrawingObj.dbx / FDrawing.arx` 文件存在，hash 已记录。
- AutoCAD 可执行文件和注册表未发现。

当前结论：

```text
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

注意：

- `fdrawingPlugin.status = ready` 只表示插件文件存在，不表示插件运行成功。
- 当前未执行 APPLOAD，也未执行旧插件导入命令。
- 因此不能把本轮写成 AutoCAD L2 通过。
- xhigh review 已指出点筋变量污染风险，主流程已从 TODO-036 包中移除 `pointStb` fixture。

未确认事项：

```text
AutoCAD L2 import 未通过。
FDrawing accepts complex skeleton 未确认。
pointStb L2 未通过。
FaceEdge generation 未实现。
section / hidden / hatch / joint algorithm 未实现。
full drawing generation 未完成。
golden 未采集。
```
