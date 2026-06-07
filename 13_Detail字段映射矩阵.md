# Detail 字段映射矩阵

## 目标

本文件把旧 AutoCAD 插件包字段映射到新系统钢筋领域模型。

核心约束：

```text
下料表和图上钢筋必须共用同一份 RebarModel。
不能工程图一套编号，下料表另一套编号。
```

## 总体结构映射

```text
Detail.xml / StyleRoot
  -> RebarStyleCatalog

DetailNN.stl / DrawingRoot
  -> DrawingPackage

StbTables / StbTable
  -> RebarSchedule

MaterialTable
  -> MaterialSummary

HViewPorts / ViewPort
  -> DrawingView

PartDetailDrawing
  -> StructureDrawingGeometry

StbDetailDrawing / StbGroups
  -> DrawingRebarGroups
```

## StyleRoot -> RebarStyle

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `Name` | `RebarStyle.name` | 已确认 | 钢筋式样名称。 |
| `dia` | `RebarStyle.diameter` | 已确认 | 直径。 |
| `type` | `RebarStyle.steelType` | 已确认 | 钢筋类型/级别，枚举待闭合。 |
| `joint_type` | `RebarStyle.jointType` | 部分确认 | 接头类型。 |
| `depth` | `RebarStyle.coverDepth` | 部分确认 | 保护层或深度，需和 Dialog #268 对齐。 |
| `depth_start` | `RebarStyle.startCoverDepth` | 部分确认 | 起点保护层/深度。 |
| `depth_end` | `RebarStyle.endCoverDepth` | 部分确认 | 终点保护层/深度。 |
| `m_Anchor1` | `RebarStyle.startAnchor` | 部分确认 | 首端锚固。 |
| `m_Anchor2` | `RebarStyle.endAnchor` | 部分确认 | 尾端锚固。 |
| `anchor_start` | `RebarStyle.startHook` | 部分确认 | 首端弯钩/锚固，具体语义待验证。 |
| `anchor_end` | `RebarStyle.endHook` | 部分确认 | 尾端弯钩/锚固，具体语义待验证。 |
| `extend1` | `RebarStyle.extendStart` | 部分确认 | 起点延长。 |
| `extend2` | `RebarStyle.extendEnd` | 部分确认 | 终点延长。 |
| `m_AddNum_start` | `RebarStyle.addNumStart` | 部分确认 | 首端增减根数。 |
| `m_AddNum_end` | `RebarStyle.addNumEnd` | 部分确认 | 末端增减根数。 |

## StbGroup -> RebarGroup

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `rsdID` | `RebarGroup.rsdId` | 已确认字段存在 | 生成规则未闭合。 |
| `groupID` | `RebarGroup.groupId` | 已确认字段存在 | 内部 ID 与旧 ID 的关系待验证。 |
| `diameter` | `RebarGroup.diameter` | 已确认 | 直径。 |
| `diameter2` | `RebarGroup.secondaryDiameter` | 部分确认 | 双直径或附加直径，语义待验证。 |
| `interval` | `RebarGroup.interval` | 已确认 | 间距。 |
| `barcount` | `RebarGroup.barCount` | 已确认 | 根数。 |
| `segcount` | `RebarGroup.segmentCount` | 已确认 | 段数。 |
| `stbNum` | `RebarGroup.displayNumber` | 部分确认 | 图上钢筋编号。 |
| `stbNumAct` | `RebarGroup.actualNumber` | 部分确认 | 实际编号/有效编号，语义待验证。 |
| `stbLevel` | `RebarGroup.steelLevel` | 部分确认 | 钢筋级别枚举待闭合。 |
| `stbLayer` | `RebarGroup.layer` | 部分确认 | 内外层或图层，语义待验证。 |
| `stbProfile` | `RebarGroup.profile` | 部分确认 | 断面/式样类别，语义待验证。 |
| `stbUse` | `RebarGroup.use` | 部分确认 | 用途枚举待闭合。 |
| `RangeLess180` | `RebarGroup.rangeLess180` | IDA 确认写出 | 写 `T/F`，来自 writer 上下文布尔字段，语义待运行确认。 |
| `ComponentName` | `RebarGroup.componentName` | 已确认 | 构件或部位名称。 |
| `PJSteelName` | `RebarGroup.projectSteelName` | 部分确认 | 工程图钢筋名。 |
| `SteelWay` | `RebarGroup.steelWay` | 部分确认 | 布筋方向/方式，需和创建命令对齐。 |
| `stbType` | `RebarGroup.rebarType` | 部分确认 | 钢筋类型。 |
| `stbOffsetInOut` | `RebarGroup.offsetInOut` | 部分确认 | 内外偏移。 |
| `Std%d/segCount` | `RebarGroup.detailStdRuns[].segCount` | IDA 确认写出 | `Std%d` 是 `StbGroup` 下的分段容器，`segCount` 由实际写入的 `StbGeo` 数量派生。 |
| `FaceEdge` | `RebarGroup.faceEdgeGeometry` | IDA 确认点状分支 | 仅 `pointStb` 分支写出，用于面边界 / 点状钢筋附加几何。 |

IDA 补证：

- `sub_14061F830` 创建 `StbGroups`，写 `stbGroupCount`，逐组调用 `sub_14063E910`。
- `sub_14063E910` 创建 `StbGroup%d`，写出本节字段，并创建 `Std%d`。
- `sub_14063E910` 对每段调用 `sub_14063B010` 写 `StbGeo%d`。
- `pointStb` 分支调用 `sub_14063A860` 写 `FaceEdge`。

关键规则：

- `rsdID` 有多分支格式：普通 int、`Y%d`、`Z%d`、`%d%c` 或字符串字段，不能只按 int 设计。
- `barcount` 来自组内链表序号范围 `max - min + 1`。
- `SteelWay` 输出 `LA / LASAME / XI / GU / OTHER`，由部位 / 名称字符串匹配决定。
- `stbType` 输出 `pointStb` 或 `lineStb`。

开发约束：

- `StbGroup` 只能从 `RebarModel` 派生。
- `rsdID / groupID / stbNum / StbGeo.segID / StbTable.rsdID` 必须同源。
- Detail writer 不能重新生成一套与模型无关的钢筋编号。

M1-App-018 状态：

- `SteelBarGroup` P1 已可表达 `rsdID / groupID / diameter / diameter2 / interval / barcount / segcount / stbNum / stbNumAct / stbLevel / stbLayer / stbProfile / stbUse / RangeLess180 / ComponentName / PJSteelName / SteelWay / stbType / stbOffsetInOut`。
- `rsdID` 在代码中使用 string 承载，避免把 `Y%d / Z%d / %d%c` 等旧分支误压成 int。
- Detail writer 仍未接入 `domain/rebar`；本状态只说明字段可编码，不说明写出值公式或 AutoCAD 导入已闭合。

## StbGeo -> RebarSegment

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `segID` | `RebarSegment.segmentId` | 已确认字段存在 | ID 生成规则未闭合。 |
| `stbSeqNum` | `RebarSegment.sequenceNo` | 部分确认 | 序号。 |
| `shapeType` | `RebarSegment.shapeType` | 已确认 | `L` / `A` / `C`。 |
| `start_x/y/z` | `RebarSegment.startPoint` | 已确认 | 起点。 |
| `end_x/y/z` | `RebarSegment.endPoint` | 已确认 | 终点。 |
| `middle_x/y/z` | `RebarSegment.middlePoint` | 已确认 | 圆弧中点或辅助点。 |
| `start_r` | `RebarSegment.startRadius` | 部分确认 | 起点半径。 |
| `end_r` | `RebarSegment.endRadius` | 部分确认 | 终点半径。 |
| `offset_x/y/z` | `RebarSegment.offset` | 部分确认 | 偏移。 |

M1-App-018 状态：

- `SteelBarSegment` P1 已可表达 `segID / stbSeqNum / shapeType / startPoint / endPoint / middlePoint / startRadius / endRadius / offset / length`。
- `shapeType` 使用 `SteelBarSegmentShape`，并提供 `detailShapeTypeCode()` 输出 `L / A / C`。
- `startRadius / endRadius / offset` 字段已存在，但旧值来源、半径公式和点状 `C` 的完整语义仍未闭合。

## StbRow -> ScheduleRow

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `rsdID` | `ScheduleRow.rsdId` | IDA 确认写值 | 必须能追回 RebarGroup。 |
| `ComponentName` | `ScheduleRow.componentName` | 已确认 | 构件名。 |
| `SteelWay` | `ScheduleRow.steelWay` | 部分确认 | 布筋方向/方式。 |
| `diameter` | `ScheduleRow.diameter` | IDA 确认写值 | 直径；同函数内另有材料表相关引用，值来源仍需分支拆解。 |
| `length` | `ScheduleRow.length` | IDA 确认写值 | 单根长度。 |
| `segNum` | `ScheduleRow.segmentCount` | 已确认 | 段数。 |
| `sameGrpNum` | `ScheduleRow.sameGroupCount` | 部分确认 | 同组数量，合并规则待验证。 |
| `stbNumSum` | `ScheduleRow.barNumberSum` | IDA 确认写值 | 编号汇总，值公式待闭合。 |
| `lenSum` | `ScheduleRow.lengthSum` | IDA 确认写值 | 总长度；同函数内另有材料表相关引用。 |
| `stbLevel` | `ScheduleRow.steelLevel` | IDA 确认写值 | 钢筋级别；枚举待闭合。 |
| `stbLayer` | `ScheduleRow.layer` | 部分确认 | 层。 |
| `stbProfile` | `ScheduleRow.profile` | 部分确认 | 断面/式样。 |
| `stbUse` | `ScheduleRow.use` | 部分确认 | 用途。 |

IDA 补证：

- `sub_140605B20` 检查模型状态后调用 `sub_140602F90`。
- `E-IDA-020` 已确认内部命令绑定 `psexcel -> 0x8D1C -> sub_140605B20`。
- `E-IDA-021` 已确认 `0x8D1C` 被挂到 `工程图 / 输出` Ribbon 分组。
- `sub_140602F90` 查找 `StbTables / StbTable`，格式化 `StbRow%d`，并通过 `sub_140525880` 写 `count / rsdID / diameter / stbLevel / length / stbNumSum / lenSum`。
- 本轮确认字段写出，不等于字段值公式已经闭合。
- 按钮 caption `下料表`、Dialog #427 和运行输出仍需旧图石确认。

## StbSeg -> ScheduleSegment

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `lenRange` | `ScheduleSegment.lengthRange` | 部分确认 | 长度范围。 |
| `deltaLen` | `ScheduleSegment.deltaLength` | 部分确认 | 长度差。 |
| `shapeType` | `ScheduleSegment.shapeType` | 已确认 | 线段/圆弧/点状表达。 |
| `Line` | `ScheduleSegment.line2d` | 已确认 | 下料表二维线段。 |
| `Arc` | `ScheduleSegment.arc2d` | 已确认 | 下料表二维圆弧。 |

## MatRow -> MaterialSummaryRow

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `MaterialTable.rowCount` | `MaterialSummary.rowCount` | IDA 确认写值 | 材料汇总行数。 |
| `MatRow%d` | `MaterialSummary.rows[]` | IDA 确认行节点 | 使用循环序号格式化。 |
| `diameter` | `MaterialSummaryRow.diameter` | IDA 确认写值 | 直径；同函数内也用于下料行。 |
| `lenSum` | `MaterialSummaryRow.lengthSum` | IDA 确认写值 | 总长度；同函数内也用于下料行。 |
| `countSum` | `MaterialSummaryRow.countSum` | 已确认 | 总根数。 |
| `singleMass` | `MaterialSummaryRow.singleMass` | IDA 确认写值 | 单位质量。 |
| `massSum` | `MaterialSummaryRow.massSum` | IDA 确认写值 | 总质量。 |
| `Mass` | `MaterialSummary.totalMass` | IDA 确认写值 | 材料表总质量。 |
| `Volume722` | `MaterialSummary.volume722` | IDA 确认写值 | 材料表体积字段，业务含义待确认。 |
| `stbLevel` | `MaterialSummaryRow.steelLevel` | IDA 确认写值 | 钢筋级别；枚举待闭合。 |

IDA 补证：

- `sub_140602F90` 查找 `MaterialTable`，格式化 `MatRow%d`，并写 `rowCount / singleMass / massSum / Mass / Volume722`。
- `MatRow%d` 另在 `sub_1406382D0` 被引用，后续需判断它是否是另一个 XML table writer。

## General-Info -> DrawingView

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `Model_FileName` | `DrawingView.modelFileName` | 已确认 | 源模型名。 |
| `DrawingName` | `DrawingView.name` | 已确认 | 图名。 |
| `DrawingUnit` | `DrawingView.unit` | 已确认 | 单位。 |
| `DrawingScale` | `DrawingView.scale` | 已确认 | 比例。 |
| `GeneralScale` | `DrawingView.generalScale` | 部分确认 | 总比例。 |
| `DrawingType` | `DrawingView.type` | 部分确认 | 图纸类型。 |
| `CutPlaneDirX/Y/Z` | `SectionPlane.normal` | 已确认字段存在 | 剖切方向。 |
| `TopDirX/Y/Z` | `DrawingView.topDirection` | 已确认字段存在 | 视图上方向。 |
| `CutPlanePosX/Y/Z` | `SectionPlane.origin` | 已确认字段存在 | 剖切位置。 |
| `Range_Min_X/Y` | `DrawingView.rangeMin` | 已确认字段存在 | 视图范围。 |
| `Range_Max_X/Y` | `DrawingView.rangeMax` | 已确认字段存在 | 视图范围。 |

M2-Drawing-003 补充：

- `E-DETAIL-003` 已从旧样例补齐 `DimensionPointBarB/T/L/R`、`DimensionLineBarB/T/L/R`、`DimensionLLineBarB/T/L/R`、`Range_XMLMin_X/Y`、`Range_XMLMax_X/Y`、`CutPlaneDirX0/Y0/Z0`、`HalfViewH/W`、`DrawTaoTong` 等复杂视图字段存在性。
- `E-IDA-027` 已确认 `FDrawing.arx` 中 `CViewInfo` 存在视图范围、剖切方向、剖切位置、上方向、比例和尺寸相关方法符号。
- 字段默认值、写入公式和旧插件容忍度仍需 AutoCAD L2 或进一步 IDA 反编译确认。

M2-Drawing-004 补充：

- `E-DEV-057` 已让正式 app `DetailWriter` 输出 `General-Info` 首批扩展字段骨架。
- `CompanyName` 当前使用 `design-company` 作为 ASCII 中性占位，不是旧样例 `设计公司` 的 1:1 默认值确认。
- 视图范围、剖切方向、上方向、剖切位置等字段当前是骨架默认值；真实公式和旧插件容忍度仍需 AutoCAD L2 或进一步 IDA / 运行确认。

M2-Drawing-005 补充：

- `E-DEV-058` 已用 `detail_l2_fixture_probe` 生成 TODO-036 复杂字段骨架包，并离线确认 `General-Info` 首批扩展字段存在。
- 本机未发现 `acad.exe / accoreconsole.exe` 或 AutoCAD 注册表键，所以 AutoCAD L2 自动导入未运行。
- 当前只证明复杂骨架包可生成并可离线检查，不证明旧插件接受默认字段。

## PartDetailDrawing 复杂线容器

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `continue-line` | `DrawingView.continueLineGeometry` | 样例确认容器 | 旧样例确认容器和子容器存在，多数为空；不等于算法已实现。 |
| `hidden-line` | `DrawingView.hiddenLineGeometry` | 样例确认容器 | 旧样例确认 `lines / circles / Arcs / Ellipses / EllipseArcs / Splines` 容器存在；隐藏线算法未实现。 |
| `central-line` | `DrawingView.centralLines` | 样例确认容器 | 当前样例仅确认 `lines` 容器。 |
| `section-line` | `DrawingView.sectionLineGeometry` | 样例确认字段 | 样例含 Line 和 Arc 实际几何字段；剖切线算法未实现。 |
| `hatch-line` | `DrawingView.hatchLines` | 样例确认容器 | 当前样例确认 `lines` 容器存在；填充线算法未实现。 |
| `Others` | `DrawingView.otherDrawingGeometry` | 样例确认容器 | 样例为空，语义未闭合。 |
| `steeljoint-line` | `DrawingView.steelJointLines` | 样例确认容器 | 当前样例确认 `joints` 容器存在；接头线生成规则未闭合。 |

子字段：

```text
LineN:
  start_x / start_y / end_x / end_y / ZValue

ArcN:
  center_x / center_y / center_z / radius / start_angle / end_angle / ZValue
```

边界：

```text
这些字段目前是 DetailWriter 复杂字段骨架输入。
不得把容器存在写成完整剖切线、隐藏线或填充线算法完成。
```

M2-Drawing-004 状态：

- `E-DEV-057` 已让正式 app `DetailWriter` 输出这些复杂容器空骨架。
- `section-line` 当前只输出空 `lines / circles / Arcs / Ellipses / EllipseArcs / Splines` 容器，不输出真实 `LineN / ArcN / ZValue` 几何。
- `steeljoint-line` 当前只输出空 `joints` 容器，不输出真实接头线。
- 该状态不等价于 AutoCAD L2 导入通过，也不等价于工程图算法完成。

M2-Drawing-005 状态：

- `E-DEV-058` 已离线检查 TODO-036 包中的 `PartDetailDrawing num="8"`、复杂容器和子容器存在。
- 检查范围固定为 `complex containers + General-Info only; pointStb L2 is deferred`。
- 旧 FDrawing 插件文件存在并记录 hash，但 AutoCAD L2 仍是 `not_run`。

## pointStb / FaceEdge 补充字段

| Detail 字段 | 新模型字段 | 状态 | 说明 |
|---|---|---|---|
| `StbGeo.shapeType=C` | `RebarSegment.shapeType = Point` | 样例确认字段 | 点状钢筋表达存在；生成规则未闭合。 |
| `point_x/y/z` | `RebarSegment.point` | 样例确认字段 | 点筋位置字段。 |
| `offset_x/y/z` | `RebarSegment.offset` | 样例确认字段 | 偏移字段，来源未完全闭合。 |
| `offset_x2/y2/z2` | `RebarSegment.secondaryOffset` | 样例确认字段 | 第二偏移字段，语义未闭合。 |
| `FaceEdge shapeType=L` | `RebarGroup.faceEdgeGeometry.line` | 样例确认字段 | 面边界线段表达。 |
| `FaceEdge shapeType=A` | `RebarGroup.faceEdgeGeometry.arc` | 样例确认字段 | 面边界圆弧表达，含 `m_ArcDotReverse`。 |

M2-Drawing-004 状态：

- `E-DEV-057` 已让正式 app `DetailWriter` 在 `SteelBarSegmentShape::Point` 时输出 `shapeType=C`、`point_x/y/z`、`offset_x/y/z`、`offset_x2/y2/z2` 字段骨架。
- 当前 P0 用 `SteelBarSegment.startPoint` 作为 `point_*`，`SteelBarSegment.offset` 作为 `offset_*`，`offset_*2` 默认 0。
- `FaceEdge` 本轮未实现；线段 / 圆弧 FaceEdge 生成规则仍是缺口。

M2-Drawing-005 状态：

- xhigh review 指出把 `pointStb` 混入 TODO-036 L2 包会污染变量。
- 主流程已从 TODO-036 包中移除点筋 fixture。
- `pointStb / FaceEdge` 后续必须作为独立干净变量专项验证，不能借 TODO-036 的复杂容器骨架结论关闭。

## 必须保持一致的字段

这些字段后续开发时不能各算各的：

- `RebarGroup.groupId`
- `RebarGroup.rsdId`
- `RebarGroup.displayNumber`
- `RebarSegment.segmentId`
- `RebarSegment.sequenceNo`
- `ScheduleRow.rsdId`
- `StbGroup.groupID`
- `StbGeo.segID`

规则：

```text
RebarModel 是唯一事实源。
Detail writer 只能从 RebarModel 派生字段。
下料表不能重新生成一套独立编号。
```

writer 级 ID 校验、缺省字段策略和失败回滚见：

```text
20_DetailWriter输出事务契约.md
```

## 待验证字段

优先验证：

- `rsdID` 的生成和复用规则。
- `groupID` 是否等于旧图石内部组 ID。
- `stbNum` / `stbNumAct` 的关系。
- `stbLayer` 是否等于内外层、显示层或业务层。
- `stbProfile` 的枚举。
- `stbUse` 的枚举。
- `sameGrpNum` 的合并规则。
- `diameter2` 的使用场景。
