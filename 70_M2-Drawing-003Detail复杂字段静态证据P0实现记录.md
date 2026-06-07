# M2-Drawing-003 Detail复杂字段静态证据 P0 实现记录

## 结论

本轮完成 `TODO-034 / M2-Drawing-003` 的静态证据整理：

- 已从旧样例 `autocad2020/Detail01.stl` 提取复杂工程图字段结构。
- 已用 IDA MCP 重新打开 `FDrawing.arx`，确认旧插件侧存在视图、点筋标注、线筋标注、钢筋表、材料表和剖面标题相关类 / 方法符号。
- 已明确区分 XML 样例证据、FDrawing 插件静态符号证据和仍需 AutoCAD L2 运行确认的缺口。

本轮不实现剖切线、隐藏线、填充线、接头线或 AutoCAD 导入算法。

## 证据 ID

新增证据：

```text
E-DETAIL-003
  -> 旧样例 Detail01.stl 复杂字段静态结构。

E-IDA-027
  -> FDrawing.arx 插件侧工程图对象类 / 方法符号静态证据。

E-DEV-056
  -> TODO-034 静态字段证据整理、报告和追溯更新。
```

关联缺口：

```text
GAP-DRAW-001
GAP-DRAW-002
GAP-DRAW-003
```

## 样例来源

旧样例包：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\Detail.xml
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\Detail01.stl
```

旧插件：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawing.arx
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawingObj.dbx
```

已知 hash：

```text
autocad2020/Detail.xml
  B02ABEEB7B9E0AB802566FA617E0627D9883010172EFD9CD50DACF940B5AFEB5

autocad2020/Detail01.stl
  E3BB220E39F5657A38BFC6793E246208760816913520DAF9C97C3544D0421C3E

FDrawing.arx
  30DB8152DA1D42DE0F2550C6A4242BCC4781F8D5B649428C6DDCBF647A29D428

FDrawingObj.dbx
  A7C8493507DE802F4881C0B60DB3DBBE4C5982CDF9F9C42A9C805EC2FEEC97B0
```

## XML复杂字段结构

旧样例 `DrawingRoot / HViewPorts / ViewPort / PartDetailDrawing` 下包含：

```text
General-Info
continue-line
hidden-line
central-line
section-line
hatch-line
Others
steeljoint-line
```

字段容器结构：

```text
continue-line
  -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines

hidden-line
  -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines

central-line
  -> lines

section-line
  -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines

hatch-line
  -> lines

Others
  -> 样例为空

steeljoint-line
  -> joints
```

本样例中 `continue-line / hidden-line / hatch-line / steeljoint-line` 容器存在，但多数为空。
因此本轮只能确认字段结构存在，不能确认旧插件对缺失 / 空节点的完整容忍规则。

## General-Info字段

旧样例 `General-Info` 属性：

```text
CompanyName
ExportYesNo
ExpSteelYesNo
ExpSteelMark
DimensionChicunB / T / L / R
DimensionPointBarB / T / L / R
DimensionLineBarB / T / L / R
DimensionLLineBarB / T / L / R
DimensionBDist / TDist / LDist / RDist
Detail
Model_FileName
DispCuttedSymb
HalfViewH
HalfViewW
BasePoint_X / BasePoint_Y
Range_Min_X / Range_Max_X / Range_Min_Y / Range_Max_Y
Range_XMLMin_X / Range_XMLMax_X / Range_XMLMin_Y / Range_XMLMax_Y
CutPlaneDirX0 / CutPlaneDirY0 / CutPlaneDirZ0
CutPlaneDirX / CutPlaneDirY / CutPlaneDirZ
TopDirX / TopDirY / TopDirZ
DrawingName
DrawingUnit
DrawingScale
GeneralScale
DrawingType
LevelDrawing
CutPlanePosX / CutPlanePosY / CutPlanePosZ
DrawTaoTong
```

证据分级：

- XML 样例确认字段名和样例值。
- FDrawing 符号确认 `CViewInfo` 存在 `Set/GetCutPlaneDir`、`Set/GetCutPlanePos`、`Set/GetTopDir`、`Set/GetGeneralScale`、`Set/GetDimensionChicun*`、`Set/GetDimension*Dist`、`SetBoxXmlMinMax`、`SetLevelDrawing`、`SetDispCuttedSymb`、`SetTaotongCircle` 等方法。
- 字段写入公式、默认值和旧插件容忍度仍需 AutoCAD L2 或进一步反编译。

## section-line字段

旧样例 `section-line` 中有实际几何：

```text
lines / LineN
  -> start_x
  -> start_y
  -> end_x
  -> end_y
  -> ZValue

Arcs / ArcN
  -> center_x
  -> center_y
  -> center_z
  -> radius
  -> start_angle
  -> end_angle
  -> ZValue
```

其中 `ZValue` 是字符串形式的三段数值，例如：

```text
12.239266:9.333333:12.239266
```

边界：

- 本轮确认 section-line 里线和圆弧字段结构。
- 不确认 `ZValue` 三段值的完整业务语义。
- 不实现 OCCT section / HLR / hidden line 算法。

## line容器字段

当前样例出现的线字段：

```text
LineN.start_x
LineN.start_y
LineN.end_x
LineN.end_y
LineN.ZValue
```

`continue-line / hidden-line / hatch-line` 中的 `lines` 容器存在但样例为空。
后续 DetailWriter 可以先补结构骨架，但不能把空容器当作完整剖切、隐藏线或填充线算法完成。

## arc容器字段

当前样例出现的圆弧字段：

```text
ArcN.center_x
ArcN.center_y
ArcN.center_z
ArcN.radius
ArcN.start_angle
ArcN.end_angle
ArcN.ZValue
```

`continue-line / hidden-line` 的 `Arcs` 容器存在但样例为空。
后续仍需更多旧包或 AutoCAD L2 结果确认插件是否要求空容器必须存在。

## pointStb / FaceEdge字段

旧样例 `StbDetailDrawing / StbGroups` 中存在 `pointStb` 分支：

```text
StbGroup.stbType = pointStb
```

点筋 `StbGeo` 字段：

```text
segID
stbSeqNum
shapeType = C
point_x / point_y / point_z
offset_x / offset_y / offset_z
offset_x2 / offset_y2 / offset_z2
```

`FaceEdge` 线段字段：

```text
shapeType = L
start_x / start_y
end_x / end_y
```

`FaceEdge` 圆弧字段：

```text
shapeType = A
m_ArcDotReverse
start_x / start_y
middle_x / middle_y
end_x / end_y
```

边界：

- 本轮确认点筋 / FaceEdge 的 XML 字段结构。
- 不确认点筋生成算法、面边界绑定规则、`offset_x2` 语义或 `m_ArcDotReverse` 业务来源。

## FDrawing静态符号证据

IDA MCP 新 session：

```text
fdrawing_arx_todo034
```

确认 `FDrawing.arx` 字符串缓存可用：

```text
strings_cache_ready = true
strings_cache_size = 27731
```

可见插件侧类 / 方法符号：

```text
CViewInfo
  GetViewID / SetViewID
  GetDetail / SetDetail
  GetModelFile / SetModelFile
  GetViewRange
  GetBoxXmlMinMax / SetBoxXmlMinMax
  GetTopDir / SetTopDir
  GetCutPlanePos / SetCutPlanePos
  GetCutPlaneDir / SetCutPlaneDir
  GetCutPlaneDir0 / SetCutPlaneDir0
  GetGeneralScale / SetGeneralScale
  GetDimensionChicunB/T/L/R
  SetDimensionChicunB/T/L/R
  GetDimensionBDist/TDist/LDist/RDist
  SetDimensionBDist/TDist/LDist/RDist

CWSNLineDim
  GetSteelBarVec / GetBulgeVec
  AppendLines / AppendLines0 / AppendPts
  GetSegID / SetSegID
  GetSteelWay / SetSteelWay
  GetPjSteelName / SetPjSteelName
  explodeForACAD

CWSNPointDim
  GetPosVec / SetPosVec
  GetAnchorPosVec / SetAnchorPosVec
  GetOffsetDir
  explodeForACAD

CWSNSteelBarTable
  SetTitleVec / GetTitleVec
  SetSketchPtVec / GetSketchPtVec
  SetSketchBulgeVec / GetSketchBulgeVec
  SetSketchNoteVec / GetSketchNoteVec
  SetVolume
  explodeForACAD

CWSNMaterialTable
  SetRowsVec / GetRowsVec
  SetTotalSum / GetTotalSum
  SetVolume / GetVolume
  SetMassNum / GetMassNum
  explodeForACAD

CWSNSectionTitle
  SetTitle / GetTitle
  SetScale / GetScale
  SetDetail / GetDetail
  explodeForACAD

CWSNArchInfo
  GetHiddenLayer
  GetSectionSymbolTextStyle
```

未命中的明文字段：

```text
DrawingRoot
PartDetailDrawing
General-Info
continue-line
hidden-line
section-line
hatch-line
steeljoint-line
FaceEdge
StbGroup
StbGeo
ZValue
```

解释：

- `FDrawing.arx` 中能看到插件对象类名和方法名。
- 暂未在该插件字符串缓存中直接找到 XML 节点名。
- 因此本轮不能声称 FDrawing 已静态确认所有 XML 字段名；XML 字段名主要来自旧样例包。

## 对后续开发的约束

后续 `DetailWriter` 扩展应分层实现：

```text
P0A: 输出容器骨架
  continue-line / hidden-line / central-line / section-line /
  hatch-line / Others / steeljoint-line

P0B: 输出 General-Info 扩展字段
  Dimension* / Range_XML* / CutPlaneDir0 / HalfView / DrawTaoTong

P0C: 输出 pointStb / FaceEdge 字段
  point_x/y/z / offset_x2/y2/z2 / FaceEdge L/A

P1: 真实剖切线 / 隐藏线 / 填充线算法
  必须另走 OCCT HLR / section 能力和旧图石运行对照。
```

禁止把 P0A/P0B/P0C 写成完整工程图完成。

## 验证

本轮是静态证据节点，没有修改 DetailWriter 业务算法。

已执行：

```text
git status --short --branch
todo.csv / 46 / 99 读取确认
旧 Detail01.stl XML 字段提取
IDA MCP idb_open FDrawing.arx -> fdrawing_arx_todo034
IDA MCP find_regex / find 静态字符串查询
IDA MCP idb_list -> fdrawing_arx_todo034 active
git diff --check -> pass
py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning
rg TopoDS_|AIS_|BRep|TopAbs_ app/src/domain/rebar app/src/drawing app/src/project
  -> no matches
app 默认 CTest
  -> 17/17 pass
```

验证结论：

```text
TODO-034 静态证据节点可收口。
当前验证不改变 AutoCAD L2 / 工程图算法 / golden 的未闭合边界。
```

## 不声明

本轮不声明：

```text
AutoCAD L2 import 已通过。
旧插件接受新包。
section-line / hidden-line / hatch-line 算法已实现。
ZValue 语义已闭合。
FaceEdge / pointStb 生成规则已闭合。
完整工程图已完成。
golden 已采集。
```

## 下一步建议

下一节点建议：

```text
TODO-035 / M2-Drawing-004
  -> DetailWriter 复杂字段骨架 P0
  -> 只输出容器骨架和可追溯默认字段
  -> 不实现剖切线、隐藏线、填充线算法
```
