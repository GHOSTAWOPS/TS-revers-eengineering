# M2-Drawing-014 真实接头线 / Others 几何算法证据补齐 P0 实现记录

## 结论

本轮完成 `TODO-045 / M2-Drawing-014` 的证据审计和缺口收敛：

- 复核 `Others / steeljoint-line` 的旧 Detail 样例证据、字段骨架证据和 L2 准备证据。
- 补入 `E-IDA-028`：旧 VisualTS 接头命令入口、`steeljoint-line / joints` 写出链、`Others / symbolcutIOS` 写出链。
- continuation 开始时发现 `visualts_i64_todo045` session 不存在，随后用 `idb_open` 重新打开并复核 `sub_14061F970 / sub_1406107F0 / sub_1405398F0 / sub_14053A3F0 / sub_14053A6B0` 和 `barjoint*` handler。
- 明确旧图石运行确认清单，作为后续闭合真实接头线 / Others 规则的输入。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## 控制合同

Primary Setpoint：

```text
在实现真实接头线 / Others 几何算法前，先把当前已知事实、
IDA 写出链证据、当前 session 阻塞事实、旧图石运行确认清单和剩余 GAP 落成可追溯证据。
```

Acceptance：

```text
新增 E-DEV-067。
新增 81 实现记录。
新增 m2_drawing_014_run_001.md / json。
更新 03 / 11 / 13 / 20 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT/AIS 泄漏扫描和 xhigh 只读 review 闭合。
```

Guardrail：

```text
不得用 OCCT 或字段骨架直接推断旧业务规则。
不得实现真实接头线 / Others 几何算法。
不得声明 AutoCAD L2 pass、旧插件接受、完整工程图或 golden。
不得改钢筋创建业务。
```

## 证据输入

```text
E-DETAIL-003
  -> 旧 Detail01.stl 样例确认 PartDetailDrawing 下存在 Others / steeljoint-line。

E-IDA-027
  -> FDrawing.arx 静态符号只补强旧插件对象模型；
     未直接命中 steeljoint-line / Others XML 节点名。

E-IDA-028
  -> TODO-045 通过 IDA MCP 记录旧 VisualTS 接头命令入口、
     steeljoint-line / joints 写出链和 Others / symbolcutIOS 写出链。

E-DEV-065
  -> TODO-043 DetailWriter Others / steeljoint-line 字段骨架。

E-DEV-066
  -> TODO-044 Others / steeljoint-line AutoCAD L2 运行确认准备。

GAP-DRAW-001
  -> AutoCAD L2 动态导入仍未运行。

GAP-DRAW-002
  -> DetailNN.stl 字段完整性、旧插件容忍度和真实接头线 / Others 规则未闭合。

GAP-DRAW-003
  -> 剖切线 / 隐藏线 / 填充线保真未闭合。
```

## 已确认事实

从旧 Detail 样例和当前新包可以确认：

```text
PartDetailDrawing
  -> Others
  -> steeljoint-line
       -> joints
```

当前旧样例与新包共同边界：

- `Others` 是容器；当前样例为空。
- `steeljoint-line` 是容器；当前已确认子容器 `joints` 存在。
- `E-DEV-065 / E-DEV-066` 只证明字段骨架和 L2 准备材料可生成。
- `E-DEV-066` 的 probe 明确记录 `algorithmImplemented=false`、`autocadL2Claimed=false`。

从 `E-IDA-028` 当前可确认：

- `barjointnew / barjointclear / barjointmove / barjointrev` 等旧命令已追到 handler。
- 接头对象由旧 VisualTS steeljoint / steelbar 业务层维护，ACIS 参与几何和事务。
- `sub_14061F970` 会创建 / 使用 `steeljoint-line / joints`。
- `sub_1406107F0` 从 `ha_rendering_context` pattern 中取 joint 点并生成接头线端点。
- `sub_1405398F0` 向 `joints` 下写 `LineN`，字段为 `start_x / start_y / end_x / end_y / ZValue`。
- `sub_14061F970` 还存在 `Others / symbolcutIOS / SymbolCutIOSN` 写出链。
- `sub_14053A3F0` 写 `center_x / center_y / center_z / code`。
- `sub_14053A6B0` 是通用线 / 弧等几何 writer，`sub_14061F970` 在特定 flags 下还会从接头相关 edge 走额外 line/arc 输出分支。
- `barjointnew / barjointclear / barjointmove / barjointrev` handler 均显示接头由旧 VisualTS steeljoint / steelbar 业务逻辑维护，外层套 ACIS transaction 和视图刷新。

当前仍不能确认：

- `JointDistbet / JointWeldLength / JointRuler` 等配置与接头线半长的具体绑定。
- `pattern + 192 == 76` 的业务枚举名。
- `pattern + 344 / +128 / pointNode + 64 / pointNode + 88` 的结构名。
- `sub_14061F970` 额外 `api_curve_arc_center_edge` 分支的完整输出条件和旧 UI 关系。
- `Others / symbolcutIOS` 的旧 UI 触发路径和运行样例。
- 旧插件是否接受当前空 `Others / steeljoint-line / joints` 新包。

## IDA MCP 复核记录

当前 `03_IDA命令证据.md` 已有 `E-IDA-028` 证据，记录的 IDA MCP 会话为：

```text
database = visualts_i64_todo045
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
Hex-Rays = ready
```

本轮 continuation 先探测到旧 session 已退出：

```text
idb_list
  -> sessions = []

server_health(database="visualts_i64_todo045")
  -> Session not found
```

随后用 IDA MCP 重新打开同名 session：

```text
idb_open(
  input_path = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64,
  preferred_session_id = visualts_i64_todo045,
  mode = prefer_headless,
  init_hexrays = true
)
  -> success = true

survey_binary(minimal)
  -> total_functions = 17182
  -> total_strings = 16320

server_health(database="visualts_i64_todo045")
  -> status = ok
  -> auto_analysis_ready = true
  -> hexrays_ready = true
```

本轮实际复核函数：

```text
sub_14061F970  -> PartDetailDrawing 复杂线容器 writer
sub_1406107F0  -> steeljoint-line / joints LineN 生成辅助函数
sub_1405398F0  -> LineN XML 写出
sub_14053A3F0  -> SymbolCutIOSN XML 写出
sub_14053A6B0  -> 通用 line / arc / circle 等几何 writer
sub_140610AA0  -> flags getter, return *(uint16 *)(a1 + 232)
sub_1405DFAA0  -> barjointnew
sub_1405DF710  -> barjointclear
sub_1405DFEF0  -> barjointmove
sub_1405E02D0  -> barjointrev
```

结论：

```text
本轮复核时 IDA MCP 已重开并可用，E-IDA-028 已被二次复核。
旧 writer 链和接头 handler 足以证明接头线 / Others 不是 OCCT 能力直接决定，
而是旧 VisualTS 业务对象和出图上下文共同驱动。
后续若要实现真实算法，仍必须继续追半长参数绑定、pattern 枚举、额外 arc 分支和旧运行输出。
```

## 旧命令线索

`03_IDA命令证据.md` 已有旧接头命令字符串：

```text
barjointnew      -> sub_1405DFAA0
barjointclear    -> sub_1405DF710
barjointmove     -> sub_1405DFEF0
barjointrev      -> sub_1405E02D0
groupjointnew    -> sub_1405F0060
groupjointclear  -> sub_1405EFCC0
```

当前仍未闭合：

- 接头命令 UI 运行流程、选择提示和参数窗口。
- handler 到具体 SFL 字段 / Detail 输出数据结构的完整字段名。
- 接头线半长参数来源。
- 额外 arc 分支和 `Others / symbolcutIOS` 的运行触发路径。

## 旧图石运行确认清单

后续用户能运行旧图石时，建议优先用非 golden 的小范围运行确认闭合：

1. 打开包含接头或可创建接头的 SFL。
2. 执行 `新建接头/搭接`，记录菜单路径、弹窗、状态栏、选择对象和参数。
3. 保存前后 SFL，记录文件 hash 和修改时间。
4. 执行 `移动接头`，记录同样信息。
5. 执行 `接头反向`，记录同样信息。
6. 执行 `清除接头`，记录同样信息。
7. 生成工程图 / Detail 包。
8. 打开生成的 `DetailNN.stl`，搜索：

```text
Others
steeljoint-line
joints
joint
```

9. 如果 `joints` 出现子节点，记录完整节点名、属性名、数值样例和所属图纸编号。
10. 如 AutoCAD 2020 + FDrawing 可用，按 `80` 的清单导入新包和旧包，记录 APPLOAD、导入命令、截图、错误文本或成功结果。

## 本轮能力边界

本轮新增的是证据状态，不是功能能力：

```text
能确定：
  - Others / steeljoint-line / joints 容器存在。
  - E-IDA-028 已确认 steeljoint-line/joints 的旧写出链。
  - E-IDA-028 已确认 Others/symbolcutIOS 的旧写出链。
  - 本轮已重新打开 IDA MCP session，并已复核接头命令 handler 与额外 line/arc writer 分支。
  - 当前样例和新包中它们为空或只有空容器。

不能确定：
  - 完整真实接头线算法。
  - 完整 Others 几何算法。
  - 接头线半长参数绑定。
  - 额外 arc 分支和旧 UI 的关系。
  - 旧插件接受当前新包。
  - AutoCAD L2 pass。
  - 完整工程图算法。
```

## 验证记录

验证结果回填于：

```text
docs/phase1/app_build_reports/m2_drawing_014_run_001.md
docs/phase1/app_build_reports/m2_drawing_014_run_001.json
```

## 下一步

建议下一阶段不要直接实现真实算法，而是继续补证据：

```text
TODO-046 / M2-Drawing-015
  -> 真实接头线 / Others 旧图石运行确认与参数绑定追踪 P0
  -> 目标是拿到 JointDistbet / JointWeldLength / JointRuler 绑定、pattern 枚举名、额外 arc 分支条件或旧运行 Detail 输出
```
