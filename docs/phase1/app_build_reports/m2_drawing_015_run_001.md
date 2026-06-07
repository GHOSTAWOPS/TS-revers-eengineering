# M2-Drawing-015 Run 001

todoId = TODO-046
phase = M2-Drawing-015
decision = ida-binding-closed-runtime-sample-pending
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮完成真实接头线 / Others 的参数绑定和运行触发补证：

- 补入 `E-IDA-029`，把 `JointRuler / JointDistbet / JointWeldLength`
  的对话框 / 配置链闭合到旧全局值。
- 确认 `sub_1406107F0` 用 `JointWeldLength / 2000.0`
  作为接头线对称半长写 `LineN`。
- 确认 `sub_1407306A0` 旧显示链也使用同一 `/2000.0` 公式。
- 确认 `pattern + 192` 的 raw byte 是 `0x4C ('L')`。
- 确认 `Others / symbolcutIOS` gate = `*(v8+848) && a4==0`，
  节点来自 `*(v8+840)` 环形链。
- 确认额外 `api_curve_arc_center_edge` 分支与 `flags==0`、
  plane distance `< 0.4` 和 `DrawTaoTong` 置位相关。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，
不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## IDA MCP 结果

```text
active session:
  database = visualts_i64_todo045
  server_health = ok
  hexrays_ready = true

parameter mapping:
  JointRuler      <-> dword_140994AB8
  JointDistbet    <-> dword_14095D628
  JointWeldLength <-> dword_14095D62C

writer formula:
  sub_1406107F0
    -> halfLength = dword_14095D62C / 2000.0
    -> write joints LineN around center +/- dir * halfLength

rendering corroboration:
  sub_1407306A0
    -> v7 = (double)dword_14095D62C / 2000.0

pattern discriminant:
  cmp byte ptr [pattern + 0xC0], 0x4C
    -> raw byte = 'L'

Others gate:
  if (*(v8 + 848) && a4 == 0)
    -> create Others/symbolcutIOS
    -> iterate ring list *(v8 + 840)
    -> node + 16 = code
    -> node + 24 = center

extra arc branch:
  sub_140610AA0(v8) == 0
  distance_to_plane(firstPoint, viewPlane) < 0.4
  visibility / direction gate passes
  -> *(byte *)(v8 + 832) = 1
  -> api_curve_arc_center_edge(...)
  -> sub_14053A6B0(...)
  -> DrawTaoTong = T
```

结论：

```text
IDA 已把参数数学关系和主要 gate 基本查到止点。
下一步应转旧图石运行确认，收集非空样例和截图，
而不是直接进入真实算法实现。
```

## 已确认事实

```text
jointParameterMapping.closed = true
jointHalfLengthFormula.closed = true
patternRawByte.closed = true
othersGate.closed = true
drawTaoTongArcRelation.closed = true
algorithmImplemented = false
oldRuntimeSample.nonEmpty = false
autocadL2 = not_run
```

## 明确不声明

```text
真实接头线算法已实现。
真实 Others 几何算法已实现。
旧 UI 单位名和触发路径已确认。
AutoCAD L2 import 已通过。
旧 FDrawing 插件接受新包。
完整工程图已完成。
golden 已采集。
```

## 待验证

```text
旧图石运行确认：
  新建接头 / 移动接头 / 接头反向 / 清除接头
  生成 Detail 包
  搜索 DetailNN.stl 中 steeljoint-line / joints / Others / symbolcutIOS / DrawTaoTong
  记录前后 SFL hash、截图和 stop point

IDA 残余：
  pattern / pointNode owning struct names
  JointWeldLength 旧 UI 单位名
  Varies_ShangH_LZ -> dword_14095D62C = 800 的专项语义
```

## 验证

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 17/17 pass

ctest --test-dir app/build --output-on-failure
  -> 18/18 pass

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass

py -3 app/tools/check_domain_rebar_boundary.py app/src/domain/rebar
py -3 app/tools/check_domain_rebar_boundary.py app/src/drawing
py -3 app/tools/check_domain_rebar_boundary.py app/src/project
  -> pass

git diff --check
  -> pass

xhigh read-only review
  -> skipped, docs-only node
```

## 下一步

```text
建议执行 TODO-047，转旧图石运行确认收集非空样例，
不直接进入真实接头线 / Others 算法实现。
```
