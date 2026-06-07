# M2-Drawing-014 Run 001

todoId = TODO-045
phase = M2-Drawing-014
decision = evidence-gap-recorded
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮完成真实接头线 / Others 几何算法的证据审计和缺口收敛：

- 复核 `E-DETAIL-003 / E-DEV-065 / E-DEV-066`。
- 补入 `E-IDA-028`，确认旧 VisualTS `steeljoint-line / joints` 写出链和 `Others / symbolcutIOS` 写出链。
- 确认接头对象来自旧 VisualTS steeljoint / steelbar 业务层，Detail writer 在出图阶段消费旧对象 / 渲染上下文。
- continuation 开始时探测到旧 session 已退出，随后用 `idb_open` 重新打开 `visualts_i64_todo045` 并二次复核旧 VisualTS 接头线 / Others 写出链和 `barjoint*` handler。
- 输出旧图石运行确认清单，用来后续验证接头线、额外 arc 分支和 `Others / symbolcutIOS` 的运行触发路径。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## IDA MCP 结果

```text
E-IDA-028 recorded session:
  database = visualts_i64_todo045
  input = VisualTS.exe.i64
  Hex-Rays = ready

continuation probe before reopen:
  idb_list -> sessions = []
  server_health(visualts_i64_todo045) -> Session not found

reopen result:
  idb_open -> success = true
  survey_binary(minimal) -> total_functions = 17182, total_strings = 16320
  server_health -> status = ok, auto_analysis_ready = true, hexrays_ready = true

E-IDA-028 confirmed:
  barjointnew -> sub_1405DFAA0
  barjointclear -> sub_1405DF710
  barjointmove -> sub_1405DFEF0
  barjointrev -> sub_1405E02D0
  groupjointnew -> sub_1405F0060
  groupjointclear -> sub_1405EFCC0
  steeljoint-line / joints writer chain = sub_14061F970 -> sub_1406107F0 -> sub_1405398F0
  joints LineN fields = start_x / start_y / end_x / end_y / ZValue
  Others / symbolcutIOS writer chain = sub_14061F970 -> sub_14053A3F0
  SymbolCutIOSN fields = center_x / center_y / center_z / code
  extra geometry writer branch = sub_14053A6B0
  flags getter = sub_140610AA0 -> *(uint16 *)(context + 232)
  rechecked handlers = sub_1405DFAA0 / sub_1405DF710 / sub_1405DFEF0 / sub_1405E02D0

rechecked functions:
  sub_14061F970
  sub_1406107F0
  sub_1405DFAA0
```

结论：

```text
E-IDA-028 已提供并复核接头线 / Others 写出链证据。
接头线半长配置绑定、额外 arc 分支、Others 运行触发路径和旧插件接受度仍未闭合。
```

## 已确认事实

```text
Others present = true
Others current sample children = 0
steeljoint-line present = true
steeljoint-line / joints present = true
barjoint* handler = partially confirmed by E-IDA-028
joints LineN schema = start_x / start_y / end_x / end_y / ZValue
Others symbolcutIOS schema = center_x / center_y / center_z / code
IDA MCP session active = true
algorithmImplemented = false
```

## 明确不声明

```text
真实接头线算法已实现。
Others 几何算法已实现。
接头线半长配置绑定已确认。
额外 arc 分支已实现。
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
  搜索 DetailNN.stl 中 steeljoint-line / joints 子节点
  保存 SFL 前后 hash

IDA 后续追踪：
  继续追 JointDistbet / JointWeldLength / JointRuler 配置绑定
  继续追 pattern + 192 == 76 的业务枚举名
  继续追额外 api_curve_arc_center_edge 分支输出条件
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
  -> pass with CRLF normalization warnings only

xhigh read-only review
  -> needs_fix, Important x2 fixed, agent closed
```

## 下一步

```text
建议执行 TODO-046，继续补 IDA / 旧图石运行证据，不直接进入真实接头线 / Others 算法实现。
```
