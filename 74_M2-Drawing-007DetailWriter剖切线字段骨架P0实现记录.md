# M2-Drawing-007 DetailWriter section-line 字段骨架 P0 实现记录

## 结论

本轮完成 `TODO-038 / M2-Drawing-007` 的 P0 字段骨架：

- `DetailDrawingViewOptions` 新增视图级 `sectionLines / sectionArcs` 字段载体。
- `DetailWriter` 可在 `PartDetailDrawing / section-line / lines` 下输出 `LineN`。
- `DetailWriter` 可在 `PartDetailDrawing / section-line / Arcs` 下输出 `ArcN`。
- `LineN` 覆盖旧样例确认的 `start_x / start_y / end_x / end_y / ZValue` 字段。
- `ArcN` 覆盖旧样例确认的 `center_x / center_y / center_z / radius / start_angle / end_angle / ZValue` 字段。
- `detail_l2_fixture_probe --fixture section-line` 可生成独立三图纸 Detail 包，并离线确认 `sectionLine.passed=true`。

本轮只做旧样例字段骨架，不声明剖切线生成算法、隐藏线算法、填充线算法、接头线算法、旧插件接受度、AutoCAD L2 或 golden 已完成。

## 证据 ID

新增证据：

```text
E-DEV-060
  -> TODO-038 / M2-Drawing-007 DetailWriter section-line Line/Arc/ZValue 字段骨架 P0。
```

依赖证据：

```text
E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 section-line / LineN / ArcN / ZValue 字段。

E-DEV-056
  -> Detail 复杂字段静态证据。

E-DEV-057
  -> DetailWriter 复杂容器骨架。

E-DEV-059
  -> 上一轮 pointStb / FaceEdge 独立字段骨架已闭合。
```

## 实现范围

修改文件：

```text
app/src/drawing/detail/DetailWriter.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
app/tools/detail_l2_fixture_probe.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

新增文档 / 报告：

```text
74_M2-Drawing-007DetailWriter剖切线字段骨架P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_007_run_001.md
docs/phase1/app_build_reports/m2_drawing_007_run_001.json
docs/phase1/app_build_reports/m2_drawing_007_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_007_detail_package/
```

## DTO

新增视图级 DTO：

```text
DetailSectionLineGeometry
  startX / startY
  endX / endY
  zValue

DetailSectionArcGeometry
  centerX / centerY / centerZ
  radius
  startAngle / endAngle
  zValue
```

边界：

- DTO 位于 `drawing/detail` 输出选项层，不进入 `domain/rebar`。
- DTO 只是显式字段载体，不负责从 OCCT 或钢筋领域对象生成剖切线。
- `ZValue` 只按旧样例字符串透传，业务语义继续保留为缺口。

## DetailWriter 输出

输出字段：

```text
section-line / lines / LineN
  start_x
  start_y
  end_x
  end_y
  ZValue

section-line / Arcs / ArcN
  center_x
  center_y
  center_z
  radius
  start_angle
  end_angle
  ZValue
```

## 独立 Probe

`detail_l2_fixture_probe` 新增：

```text
--fixture section-line
```

输出：

```text
runId = DW-L2-TODO038-001
fixture = section-line
sectionLine.passed = true
lineCount = 1
arcCount = 1
lineFieldsPassed = true
arcFieldsPassed = true
autocadL2 = not_run
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
```

## IDA MCP 状态

本轮 `idb_list` 结果：

```text
sessions = []
count = 0
```

本轮没有新增 IDA 证据。原因是 `section-line / LineN / ArcN / ZValue` 字段名和样例值已有旧 `Detail01.stl` 直接证据，且本轮只做字段骨架，不推断旧剖切线生成算法或 `ZValue` 业务语义。

## 验证

已执行：

```text
cmake --build app\build --target detail_writer_tests detail_l2_fixture_probe --config Debug
  -> pass

app\build\detail_writer_tests.exe
  -> pass

detail_l2_fixture_probe --fixture section-line
  -> pass

app 默认 CTest
  -> 17/17 pass

py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass, 0 error, 0 warning

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> pass / no matches

xhigh 只读 review
  -> allow_commit; Critical=0; Important=0
```

## 不声明

未确认事项：

```text
AutoCAD L2 import 未通过。
旧插件接受 section-line 新包未确认。
section-line 生成规则未闭合。
ZValue 三段值业务语义未闭合。
真实剖切线算法未实现。
隐藏线 / 填充线 / 接头线算法未实现。
完整工程图未完成。
golden 未采集。
```
