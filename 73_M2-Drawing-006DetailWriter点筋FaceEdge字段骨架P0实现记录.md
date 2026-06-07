# M2-Drawing-006 DetailWriter 点筋 FaceEdge 字段骨架 P0 实现记录

## 结论

本轮完成 `TODO-037 / M2-Drawing-006` 的 P0 字段骨架闭环：

- `DetailWriter` 可输出 `pointStb` 的 `StbGeo shapeType=C` 字段骨架。
- 新增 `RebarFaceEdgeGeometry` 作为纯领域 DTO，承载旧样例确认的 `FaceEdge shapeType=L/A` 字段。
- `DetailWriter` 只在 `StbGroup.stbType = pointStb` 时输出显式 `FaceEdge` 字段。
- 新增 `detail_l2_fixture_probe --fixture point-face-edge` 独立变量包。
- probe 离线确认 `pointStb / FaceEdge L/A` 字段存在。

本轮不声明旧 AutoCAD 2020 + FDrawing 插件已经接受当前包，也不声明点筋或 FaceEdge 生成算法已完成。

## 证据 ID

新增证据：

```text
E-DEV-059
  -> TODO-037 / M2-Drawing-006 DetailWriter pointStb / FaceEdge 字段骨架 P0。
```

依赖证据：

```text
E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 pointStb / FaceEdge L/A 字段。

E-DEV-056
  -> 复杂字段静态证据。

E-DEV-057
  -> DetailWriter 复杂容器、General-Info 和 pointStb 字段骨架。

E-DEV-058
  -> TODO-036 复审确认 pointStb 不应混入复杂骨架 L2 变量。
```

## 实现范围

修改文件：

```text
app/src/domain/rebar/RebarDomainTypes.h
app/src/domain/rebar/SteelBarGroup.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

新增文档 / 报告：

```text
73_M2-Drawing-006DetailWriter点筋FaceEdge字段骨架P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_006_run_001.md
docs/phase1/app_build_reports/m2_drawing_006_run_001.json
docs/phase1/app_build_reports/m2_drawing_006_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_006_detail_package/
```

## FaceEdge DTO

新增纯领域 DTO：

```text
RebarFaceEdgeGeometry
  shapeType = Line / Arc
  startPoint
  middlePoint
  endPoint
  arcDotReverse
  unresolvedLegacyFields
  evidence
```

边界：

- DTO 不依赖 OCCT / AIS。
- DTO 只是字段载体，不负责从面、边、点自动生成 FaceEdge。
- `FaceEdge generation rule` 继续保留为 `GAP-DRAW-002`。

## DetailWriter 输出

当前输出规则：

```text
仅当 StbGroup.stbType = pointStb 时：
  输出 group.faceEdges 中的 FaceEdge 字段。
```

线段：

```text
FaceEdge shapeType="L"
  start_x / start_y
  end_x / end_y
```

圆弧：

```text
FaceEdge shapeType="A"
  m_ArcDotReverse
  start_x / start_y
  middle_x / middle_y
  end_x / end_y
```

## 独立 Probe

`detail_l2_fixture_probe` 新增：

```text
--fixture point-face-edge
```

输出：

```text
runId = DW-L2-TODO037-001
fixture = point-face-edge
pointFaceEdge.passed = true
pointGroupCount = 2
pointGeoCount = 2
faceEdgeCount = 2
lineFaceEdgePassed = true
arcFaceEdgePassed = true
autocadL2 = not_run
```

## IDA MCP 状态

本轮 `idb_list` 结果：

```text
sessions = []
count = 0
```

本轮没有新增 IDA 证据。原因是 `TODO-037` 的字段名和位置已有旧 `Detail01.stl` 样例直接证据，且本轮只做字段骨架，不推断旧 FaceEdge 生成算法。

## 验证

已执行：

```text
cmake --build app\build --target detail_writer_tests detail_l2_fixture_probe --config Debug
  -> pass

app\build\detail_writer_tests.exe
  -> pass

detail_l2_fixture_probe --fixture point-face-edge
  -> pass

app 默认 CTest
  -> 17/17 pass

py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches
```

## Xhigh 只读 Review

结果：

```text
Initial Verdict = needs_fix
Critical = none
Important = fixed:
  - run_001.json xhighReview 从 pending 收口；
  - 11_需求证据追溯矩阵.md 的 REQ-PROJ-001 / REQ-TECH-002
    汇总行纳入 E-DEV-059。
Minor = pointFaceEdgeProbe 当前按全局计数检查 FaceEdge；
        后续可增强为每个 pointStb StbGroup 内部归属断言。
Final Verdict = allow_commit
```

处理：

- Important 已由主流程修复。
- Minor 不影响本轮 P0 字段骨架验收。
- 本轮继续保持 `FaceEdge` 只是显式 fixture 字段骨架，不声明生成算法。

## 不声明

未确认事项：

```text
AutoCAD L2 import 未通过。
旧插件接受 pointStb / FaceEdge 新包未确认。
FaceEdge 生成规则未闭合。
m_ArcDotReverse 业务来源未闭合。
点筋真实生成算法未实现。
剖切线 / 隐藏线 / 填充线 / 接头线算法未实现。
完整工程图未完成。
golden 未采集。
```
