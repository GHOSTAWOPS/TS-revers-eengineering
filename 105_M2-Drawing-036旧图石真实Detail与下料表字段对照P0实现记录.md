# M2-Drawing-036 旧图石真实 Detail 与下料表字段对照 P0 实现记录

## 结论

本轮执行 `TODO-067 / 旧图石真实 Detail 与下料表字段对照 P0`。

本轮没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实工程图算法。

本轮只做字段级对照：

```text
旧图石 RUN-20260609-001 真实输出
  -> Detail.xml
  -> Detail01.stl 到 Detail04.stl
  -> 下料表.xls

对照当前 app
  -> DetailWriter
  -> RebarScheduleService
```

核心结论：

```text
1. 真实 Detail.xml 是 14 bytes 的 <StyleRoot/>。
   当前 DetailWriter 会写非空 StyleRoot/Styles/Style1。
   这是当前最明显的格式差异。

2. 真实 Detail01..04.stl 都有 StbTables 容器，
   但只有 Detail01.stl 有 StbTable 和 MaterialTable。
   当前 DetailWriter 会给每张 DetailNN.stl 都写 StbTable 和 MaterialTable。

3. 真实 StbTable 除 count 外，还有 HeightValue0 / HeightValueCount /
   Volume1225 / NumCombineGoJians / SteelNetArea / GJTAOTNumber /
   GJTAOTVolue / LinkTop / LinkDown / DCGQSJ / HYLJJ。
   当前 DetailWriter 只写 count。

4. 真实 StbRow 字段比当前 writer 多 smallTable / mirrorType / mirrorSEFlag。

5. 真实 lineStb 的 StbGeo 只出现 start/end/offset 字段；
   当前 DetailWriter 对非点筋统一写 middle/start_r/end_r。
   这可能需要后续按 shapeType 做字段条件化。

6. 真实 StbGroup 支持多个 Std# 容器；
   当前 DetailWriter 只写 Std1，且只用第一根 bar 的 segment 列表。

7. 真实 General-Info 字段结构当前基本有骨架，
   但多个默认值和真实值不一致：
   CompanyName、DrawingUnit、DrawingScale、GeneralScale、
   DimensionChicunB/L、BasePoint、Range、CutPlaneDir、CutPlanePos 等。

8. 真实下料表.xls 是三张 Excel 表：
   钢筋表、钢筋汇总表、钢筋下料单。
   当前 RebarScheduleService 只提供 XML schedule/material 数据，
   没有真实 Excel writer。

9. 真实 Excel 需要单位换算和显示格式：
   [3]20、单根长(cm)、总长(m)、单净长(mm)、总重(Kg)、钢筋合计(T)、
   混凝土(m3) 等。
   当前这些公式和格式仍未闭合。

10. 真实 Excel 第三张表含 焊长(mm) / 焊头(个) / 单下料长(mm)。
    当前接头链路按用户要求暂缓，因此本轮只记录字段缺口，不实现接头算法。
```

大白话说：

```text
TODO-066 解决的是“旧图石到底真实导出了什么文件”。

TODO-067 解决的是“这些真实文件里到底有哪些字段，
我们的新 app 现在哪些有，哪些没有，哪些只是空骨架”。

结论很清楚：
当前 app 的 DetailWriter / RebarScheduleService 已经有主骨架，
但还不是旧图石真实输出格式。

下一步应该先做一个很小的格式纠偏切片：
  -> Detail.xml 空模板策略
  -> StbTable / MaterialTable 只在主图 Detail01 输出的策略
  -> StbTable 缺失属性补齐为可追溯默认值

不要直接跳到隐藏线、填充线、点筋、FaceEdge 或 AutoCAD L2。
```

## Control Contract

Primary Setpoint：

```text
把 RUN-20260609-001 的真实 Detail / Excel 字段，
和当前 DetailWriter / RebarScheduleService 的字段能力做静态对照，
形成可开发的缺口清单。
```

Acceptance：

```text
新增 E-DETAIL-004。
新增 E-DEV-089。
新增 M2-Drawing-036 实现记录。
新增 m2_drawing_036_run_001.md/json。
更新 05 / 11 / 12 / 13 / 20 / 34 / 46 / 99 / 00 / todo.csv。
TODO-067 标记 done。
新增 TODO-068 作为下一轮 next。
默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查通过。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不修改系统目录。
不改 app 业务算法。
不实现真实工程图算法。
不声明 AutoCAD L2 通过。
不进入 golden。
不把父目录/外部非空 Detail.xml 混成本轮真实工件。
不把 Detail.xml 空模板推断写成旧插件已验证要求。
```

## 输入证据

真实运行工件目录：

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

本轮读取：

```text
Detail.xml
Detail01.stl
Detail02.stl
Detail03.stl
Detail04.stl
下料表.xls
detail_package_probe_01.txt
schedule_excel_preview_01.txt
```

对照代码：

```text
app/src/drawing/detail/DetailWriter.cpp
app/src/drawing/detail/DetailWriter.h
app/src/domain/rebar/RebarScheduleService.cpp
app/src/domain/rebar/RebarScheduleService.h
```

## Detail.xml 对照

真实旧图石输出：

```xml
<StyleRoot/>
```

当前事实：

```text
bytes = 14
sha256 = CCBD220D75D7F9C7E26E2540D639FA5956A369A31D7902F75ED36461F778F271
用户补充多台电脑生成结果一致，均为 <StyleRoot/>。
生成工程图和 CAD 导入时 Detail.xml 修改时间不更新。
```

当前 `DetailWriter`：

```text
writeStyleXml()
  -> StyleRoot CurrPos="1"
  -> Styles
  -> Style1 Name / dia / type / source
```

差距：

```text
GAP-DRAW-006:
  真实旧输出是空 StyleRoot。
  当前新 writer 是非空 StyleRoot。
  后续需要决定：
    A. 默认按旧真实输出写空 StyleRoot；
    B. 或保留兼容开关，但旧图石包模式必须能输出空 StyleRoot。
```

本轮不证明：

```text
旧 FDrawing 插件必须存在 Detail.xml 才能导入。
旧插件一定拒绝非空 StyleRoot。
父目录/外部非空 Detail.xml 属于 RUN-20260609-001。
```

## DetailNN.stl 字段对照

### 文件分布

真实旧图石输出：

```text
Detail01.stl:
  DrawingRoot = true
  StbGroup = 12
  StbTable = 1
  pointStb = 7
  lineStb = 5

Detail02.stl:
  DrawingRoot = true
  StbGroup = 6
  StbTable = 0
  pointStb = 4
  lineStb = 2

Detail03.stl:
  DrawingRoot = true
  StbGroup = 5
  StbTable = 0
  pointStb = 0
  lineStb = 5

Detail04.stl:
  DrawingRoot = true
  StbGroup = 7
  StbTable = 0
  pointStb = 0
  lineStb = 7
```

当前 `DetailWriter`：

```text
每个 view 调一次 writeDrawingXml()。
每个 DetailNN.stl 都写 StbTable 和 MaterialTable。
```

差距：

```text
GAP-DRAW-007:
  真实旧输出只在 Detail01 写 StbTable / MaterialTable。
  当前 writer 每张图都写表格。
  下一轮应先改成主图表格策略，副图保留空 StbTables 容器。
```

### StbTable

真实 `StbTable` 属性：

```text
count
HeightValue0
HeightValueCount
Volume1225
NumCombineGoJians
SteelNetArea
GJTAOTNumber
GJTAOTVolue
LinkTop
LinkDown
DCGQSJ
HYLJJ
```

当前 `DetailWriter` 支持：

```text
count
```

缺失：

```text
HeightValue0 / HeightValueCount / Volume1225 / NumCombineGoJians /
SteelNetArea / GJTAOTNumber / GJTAOTVolue / LinkTop / LinkDown /
DCGQSJ / HYLJJ
```

后续策略：

```text
先按真实样例默认值补齐字段骨架，
但 Volume1225 / SteelNetArea / GJTAOT* 等真实公式仍标 gap，
不能把默认值当作算法完成。
```

### StbRow

真实 `StbRow#` 属性：

```text
rsdID
ComponentName
SteelWay
diameter
length
segNum
sameGrpNum
stbNumSum
lenSum
stbLevel
stbLayer
stbProfile
stbUse
smallTable
mirrorType
mirrorSEFlag
```

当前 `DetailWriter + RebarScheduleService` 支持：

```text
rsdID
ComponentName
SteelWay
diameter
length
segNum
sameGrpNum
stbNumSum
lenSum
stbLevel
stbLayer
stbProfile
stbUse
```

缺失：

```text
smallTable
mirrorType
mirrorSEFlag
```

低置信：

```text
sameGrpNum 当前 P0 固定为 1，真实合并规则未闭合。
length / lenSum 的 cm/m 单位换算与旧 Excel 显示还未闭合。
```

### MaterialTable

真实 `MaterialTable` 属性：

```text
rowCount
Mass
Volume722
MassNum
```

真实 `MatRow#` 属性：

```text
diameter
lenSum
countSum
singleMass
massSum
stbLevel
```

当前 `DetailWriter + RebarScheduleService` 结构支持这些字段。

仍未闭合：

```text
singleMass 来源。
Mass / massSum 质量公式。
MassNum 入口参数来源。
Volume722 / Volume1225 与混凝土体积字段的关系。
```

### General-Info

真实 `General-Info` 与当前 writer 都有以下字段骨架：

```text
CompanyName
ExportYesNo
ExpSteelYesNo
ExpSteelMark
DimensionChicunB/T/L/R
DimensionPointBarB/T/L/R
DimensionLineBarB/T/L/R
DimensionLLineBarB/T/L/R
DimensionBDist/TDist/LDist/RDist
Detail
Model_FileName
DispCuttedSymb
HalfViewH/W
BasePoint_X/Y
Range_Min_X/Max_X/Min_Y/Max_Y
Range_XMLMin_X/XMLMax_X/XMLMin_Y/XMLMax_Y
CutPlaneDirX0/Y0/Z0
CutPlaneDirX/Y/Z
TopDirX/Y/Z
DrawingName
DrawingUnit
DrawingScale
GeneralScale
DrawingType
LevelDrawing
CutPlanePosX/Y/Z
DrawTaoTong
```

差距：

```text
CompanyName:
  真实 = 设计公司
  当前 = design-company

DrawingUnit:
  真实 = 1000
  当前默认 = m

DrawingScale / GeneralScale:
  真实样例 = 100 / 50
  当前默认 = 1 / 1

PartDetailDrawing.num:
  真实样例 = 8 / 4 / 10
  当前 = 8

BasePoint / Range / CutPlaneDir / TopDir / CutPlanePos:
  真实按视图变化。
  当前大多是 0 或固定方向。

DimensionChicunB/L:
  真实样例为 F。
  当前 writer 默认 B/L 为 T。
```

### PartDetailDrawing 复杂容器

真实四张图均有：

```text
continue-line
hidden-line
central-line
section-line
hatch-line
Others
steeljoint-line
```

真实 `section-line`：

```text
四张图合计 32 个 Line#。
Line# 字段为 start_x / start_y / end_x / end_y / ZValue。
本轮样例未观察到 section Arc#。
```

当前 `DetailWriter`：

```text
已能输出容器骨架。
已能在输入 view.sectionLines / view.sectionArcs 时写 Line# / Arc#。
但当前没有真实剖切线生成算法。
```

本轮结论：

```text
字段骨架已有，不等于真实 section-line 算法完成。
隐藏线、填充线、连续线在本轮真实样例中为空，不能据此关闭算法缺口。
```

### StbGroup / Std / StbGeo

真实 `StbGroup#` 属性和当前 writer 基本一致：

```text
rsdID
groupID
diameter
diameter2
interval
barcount
segcount
stbNum
stbNumAct
stbLevel
stbLayer
stbProfile
stbUse
RangeLess180
ComponentName
PJSteelName
SteelWay
stbType
stbOffsetInOut
```

差距：

```text
1. 真实旧输出支持一个 StbGroup 下多个 Std#。
   当前 writer 只写 Std1。

2. 真实旧输出的 Std# 更接近“组内代表钢筋 / 首末根 / 分布端点”语义。
   当前 writer 只用第一根 bar 的 segments。

3. 真实 lineStb 的 StbGeo# 只见：
     segID / stbSeqNum / shapeType /
     start_x/y/z / end_x/y/z / offset_x/y/z
   当前 writer 对非点筋额外写：
     middle_x/y/z / start_r / end_r

4. 真实 pointStb 的 StbGeo# 见：
     point_x/y/z / offset_x/y/z / offset_x2/y2/z2
   当前 writer 已有点筋字段骨架，但生成规则和 FaceEdge 来源仍未闭合。
```

## Excel 下料表对照

真实 `下料表.xls`：

```text
钢筋表:
  rows = 9
  cols = 6
  header = 编号 / 直径(mm) / 形状 / 单根长(cm) / 根数(根) / 总长(m)

钢筋汇总表:
  rows = 4
  cols = 6
  header = 直径(mm) / 总长(m) / 单重(Kg/m) / 总重(Kg) /
           钢筋合计(T) / 混凝土(m3)

钢筋下料单:
  rows = 16
  cols = 8
  header = 编号 / 直径(mm) / 单净长(mm) / 焊长(mm) /
           焊头(个) / 单下料长(mm) / 根数(根) / 总根数(根)
```

当前 `RebarScheduleService`：

```text
只生成内存 RebarSchedule。
当前输出被 DetailWriter 写入 DetailNN.stl 的 StbTable / MaterialTable。
没有生成真实 Excel workbook。
```

字段支持情况：

```text
可间接支持：
  编号 -> row.rsdId / group.displayNumber 仍需最终口径
  直径 -> row.diameter
  单根长 -> row.length
  根数 -> row.barNumberSum
  总长 -> row.lengthSum
  单重 -> material.singleMass，来源未闭合
  总重 -> material.massSum，公式未闭合
  混凝土 -> schedule.volume722，公式未闭合

缺失或低置信：
  Excel 三 sheet writer
  [3]20 这种级别+直径显示格式
  形状列
  单根长(cm) / 单净长(mm) / 总长(m) 单位换算
  钢筋合计(T) 只显示在汇总表首行的布局规则
  焊长(mm)
  焊头(个)
  单下料长(mm)
  总根数(根)
  sameGrpNum 完整合并规则
```

## 新增缺口

```text
GAP-DRAW-006:
  Detail.xml 空 StyleRoot 策略和旧插件存在性要求未闭合。

GAP-DRAW-007:
  StbTable / MaterialTable 主图输出策略和 StbTable 扩展属性未实现。

GAP-DRAW-008:
  真实 Excel 下料表三 sheet 输出、单位换算、显示格式和焊接字段未实现。
```

## 下一步

本轮把 `TODO-067` 标记为 `done`，并新增下一阶段：

```text
TODO-068 / DetailWriter 真实字段差异 P0：空 Detail.xml 与主图表格策略

建议范围：
  -> 只改 DetailWriter 和对应测试
  -> 让旧图石包模式输出空 <StyleRoot/>
  -> 让 StbTable / MaterialTable 只在 Detail01.stl 输出
  -> 让副图保留空 StbTables 容器
  -> 补 StbTable 缺失属性的可追溯默认值
  -> 不做 Excel writer
  -> 不做隐藏线 / 填充线 / 点筋 / FaceEdge 算法
  -> 不跑 AutoCAD L2
```

本轮不自动进入 `TODO-068`。
