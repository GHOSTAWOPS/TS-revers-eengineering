# M2-Drawing-004 DetailWriter复杂字段骨架 P0 实现记录

## 结论

本轮完成 `TODO-035 / M2-Drawing-004`：

- `DetailWriter` 已在 `PartDetailDrawing` 下输出旧样例确认的复杂字段容器骨架。
- `General-Info` 已补首批扩展默认字段。
- `shapeType=C` 点筋 `StbGeo` 已按旧样例字段名输出 `point_* / offset_* / offset_*2` 骨架。
- `detail_writer_tests` 新增复杂容器、默认字段和点筋字段断言。

本轮只完成字段骨架，不实现剖切线、隐藏线、填充线、接头线或 `FaceEdge` 生成算法。

## 证据 ID

新增证据：

```text
E-DEV-057
  -> TODO-035 / M2-Drawing-004 DetailWriter 复杂字段骨架 P0。
```

依赖证据：

```text
E-DETAIL-003
  -> 旧 Detail01.stl 复杂字段静态结构。

E-IDA-027
  -> FDrawing.arx 插件侧对象 / 方法符号静态证据。

E-DEV-056
  -> TODO-034 复杂字段静态证据整理。
```

## 实现范围

修改文件：

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

新增文档 / 报告：

```text
71_M2-Drawing-004DetailWriter复杂字段骨架P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_004_run_001.md
docs/phase1/app_build_reports/m2_drawing_004_run_001.json
```

## DetailWriter 输出

`PartDetailDrawing` 现在输出：

```text
PartDetailDrawing num="8"
  General-Info
  continue-line
    lines / circles / Arcs / Ellipses / EllipseArcs / Splines
  hidden-line
    lines / circles / Arcs / Ellipses / EllipseArcs / Splines
  central-line
    lines
  section-line
    lines / circles / Arcs / Ellipses / EllipseArcs / Splines
  hatch-line
    lines
  Others
  steeljoint-line
    joints
```

这些容器当前为空容器，只代表旧 XML 字段结构可输出。

## General-Info 默认字段

本轮补充字段包括：

```text
CompanyName
ExportYesNo / ExpSteelYesNo / ExpSteelMark
DimensionChicunB/T/L/R
DimensionPointBarB/T/L/R
DimensionLineBarB/T/L/R
DimensionLLineBarB/T/L/R
DimensionBDist/TDist/LDist/RDist
Detail
DispCuttedSymb
HalfViewH / HalfViewW
BasePoint_X/Y
Range_Min_X / Range_Max_X / Range_Min_Y / Range_Max_Y
Range_XMLMin_X / Range_XMLMax_X / Range_XMLMin_Y / Range_XMLMax_Y
CutPlaneDirX0/Y0/Z0
CutPlaneDirX/Y/Z
TopDirX/Y/Z
DrawingType
LevelDrawing
CutPlanePosX/Y/Z
DrawTaoTong
```

边界：

- `CompanyName` 当前写 `design-company`，是 ASCII 中性占位。
- 旧样例值是 `设计公司`，但默认值 / 旧插件容忍度未运行确认。
- 因此不能把当前默认值写成旧图石 1:1 默认值已闭合。

## pointStb 字段骨架

点筋分支：

```text
SteelBarSegmentShape::Point
  -> StbGeo shapeType="C"
  -> point_x / point_y / point_z
  -> offset_x / offset_y / offset_z
  -> offset_x2 / offset_y2 / offset_z2
```

当前 P0 映射：

```text
point_*  <- SteelBarSegment.startPoint
offset_* <- SteelBarSegment.offset
offset_*2 = 0
```

边界：

- 点筋真实生成算法未闭合。
- `offset_x2/y2/z2` 第二偏移语义未闭合。
- `FaceEdge` 本轮未实现，仍保留缺口。

## 测试

新增 / 扩展测试：

```text
testDetailWriterWritesComplexPartDrawingSkeletonAndGeneralInfoDefaults
testDetailWriterWritesPointStbGeoFieldSkeleton
```

覆盖：

- `PartDetailDrawing.num = 8`。
- `continue-line / hidden-line / central-line / section-line / hatch-line / Others / steeljoint-line` 存在。
- 复杂容器子节点存在。
- `General-Info` 首批扩展字段存在和关键默认值。
- 点筋 `StbGeo shapeType=C` 输出 `point_* / offset_* / offset_*2`，且不输出线筋 `start_x/end_x`。
- `result.l2 == not_run`，不声明 AutoCAD L2。

原有测试继续覆盖：

- Detail 包首批字段映射。
- 缺 required field 不覆盖旧包。
- 缺 bar / segment ref fail-fast。
- 安装失败恢复旧包。
- 多图纸 DetailNN 命名和旧多余文件清理。

## 验证

已执行：

```text
detail_writer_tests
  -> pass

app 默认 CTest
  -> 17/17 pass

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches

git diff --check
  -> pass
```

## xhigh Review

只读 review：

```text
verdict = allow_commit
Critical = none
Important = none
```

Minor：

```text
CompanyName 使用 design-company，不是旧样例“设计公司”。
FaceEdge 本轮未实现。
review 未跑 CTest/readiness，因为契约要求只读 diff 审查。
```

主流程处理：

- `CompanyName` 已在本记录中明确为中性占位。
- `FaceEdge` 继续写入缺口，不作为 TODO-035 完成条件。
- 主流程已实际运行 CTest / readiness / 泄漏扫描。

## 不声明

本轮不声明：

```text
AutoCAD L2 import 已通过。
旧插件接受当前复杂字段骨架。
剖切线 / 隐藏线 / 填充线 / 接头线算法已实现。
section-line LineN / ArcN 真实几何已生成。
ZValue 语义已闭合。
FaceEdge 生成规则已闭合。
完整工程图已完成。
golden 已采集。
```

## 下一步建议

下一节点建议：

```text
TODO-036 / M2-Drawing-005
  -> AutoCAD L2 复杂字段骨架导入验证 P0
```

目标是在可用 AutoCAD 2020 + FDrawing 插件环境中确认旧插件是否接受当前复杂字段骨架。

如果 AutoCAD 自动环境仍不可用，则继续形成手工运行确认清单和 blocked/gap 记录，不把离线 L0/L1 结果写成 L2 pass。
