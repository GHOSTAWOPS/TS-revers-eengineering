# M2-Drawing-015 真实接头线 / Others 旧图石运行确认与参数绑定追踪 P0 实现记录

## 结论

本轮完成 `TODO-046 / M2-Drawing-015` 的参数绑定和运行触发补证：

- 用 IDA MCP 把 `JointRuler / JointDistbet / JointWeldLength` 的旧对话框、配置读写和消费链继续闭合。
- 确认 `sub_1406107F0` 用 `dword_14095D62C / 2000.0` 作为接头线对称半长，沿归一化 edge 方向写 `steeljoint-line / joints / LineN`。
- 确认 `sub_1407306A0` 旧 HOOPS 显示链也使用同一 `/2000.0` 长度公式，作为独立旁证。
- 把 `pattern + 192 == 76` 从“裸常量”收敛成字节字面量 `0x4C ('L')`。
- 把 `Others / symbolcutIOS` 的 gate 收敛到 `*(v8 + 848)` 非空且 `a4 == 0`，并补出 node 上的 `code + center` 写出链。
- 把额外 `api_curve_arc_center_edge` 分支的条件继续收窄到 `flags==0`、plane distance `< 0.4`、可见性 / 方向 gate，并确认该分支会置位 `DrawTaoTong=T`。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## 控制合同

Primary Setpoint：

```text
在 TODO-045 已确认 writer 链的基础上，把旧接头参数、
半长数学关系、额外弧线 gate 和 Others 触发 gate 补成可追溯证据，
明确 IDA 已经能闭合到哪里、还缺什么旧运行样例。
```

Acceptance：

```text
新增 E-IDA-029。
新增 E-DEV-068。
新增 82 实现记录。
新增 m2_drawing_015_run_001.md / json。
更新 03 / 11 / 13 / 20 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT / AIS 泄漏扫描通过。
```

Guardrail：

```text
不得实现真实接头线 / Others 几何算法。
不得把字段骨架或 OCCT 能力写成旧业务规则。
不得声明 AutoCAD L2 pass、旧插件接受、完整工程图或 golden。
不得改钢筋创建业务。
```

## 新增关键证据

### 1. 三个旧参数和全局值已闭合

通过 `sub_1406ED5E0 / sub_1406EDF80 / sub_1406AC410 / sub_1406AC6A0`，
当前已明确：

```text
JointRuler      <-> dword_140994AB8
JointDistbet    <-> dword_14095D628
JointWeldLength <-> dword_14095D62C
```

其中：

```text
JointRuler      -> dialogValue * 1000 -> dword_140994AB8
JointDistbet    -> dialogValue * 1000 -> dword_14095D628
JointWeldLength -> dialogValue * 10   -> dword_14095D62C
```

对话框 apply 还会强制：

```text
if (JointRuler < JointDistbet)
  JointDistbet = JointRuler / 2
```

### 2. 接头线半长公式已闭合

`sub_1406107F0` 汇编明确为：

```text
movd xmm6, dword_14095D62C
cvtdq2pd xmm6, xmm6
divsd xmm6, 2000.0
```

随后 `xmm6` 被作为沿归一化 edge 方向的正负标量，
分别生成：

```text
center + dir * halfLength
center - dir * halfLength
```

再写入 `LineN.start_x/start_y/end_x/end_y/ZValue`。

因此当前可以明确：

```text
halfLength_model = dword_14095D62C / 2000.0
fullLineLength_model = dword_14095D62C / 1000.0

又因为：
dword_14095D62C = JointWeldLength_dialog * 10

所以：
halfLength_model = JointWeldLength_dialog / 200.0
fullLineLength_model = JointWeldLength_dialog / 100.0
```

这已经把“旧接头线半长到底怎么从 UI 参数落到 writer”闭合了。
但旧 UI 的单位标签仍需运行确认，当前不能擅自写成 mm / cm / m 的确定结论。

### 3. 旧显示链也使用同一长度公式

`sub_1407306A0` 中存在：

```text
v7 = (double)dword_14095D62C / 2000.0;
```

说明 `JointWeldLength` 不只影响 Detail writer，
也影响旧 HOOPS 渲染侧的接头相关可视长度。

### 4. `JointRuler / JointDistbet` 已落到旧对象字段

`sub_1405DB340`（`__SteelBar` 构造）会把：

```text
field +108 = dword_140994AB8
field +112 = dword_140994AB8
```

`sub_1405E9640`、`sub_1405DFAA0(barjointnew)` 和 `sub_1405EBA30`
会根据 `JointDistbet` 做交错端点修正：

```text
odd/even parity
  -> end = JointRuler
  -> or end = JointRuler - JointDistbet
```

`sub_1405DFEF0(barjointmove)` 还会把 `field112`
归一化到 `field112 mod field108`。

当前可以保守表达为：

```text
JointRuler 已落到旧对象的周期 / 基础长度字段
JointDistbet 已落到旧对象的交错偏移 / 相位修正规则
```

字段的精确业务名还没拿到，但不是“只停留在对话框全局值”。

### 5. `pattern + 192` 当前至少能确认是字节 `'L'`

`sub_1406107F0` 汇编不是抽象的 `76`，
而是：

```text
cmp byte ptr [pattern + 0xC0], 0x4C
```

所以当前能确认：

```text
pattern discriminant raw byte = 0x4C = 'L'
```

owning enum / 结构名仍未闭合，
但“它只是一个神秘的 76 常量”这个状态已经结束。

### 6. `Others / symbolcutIOS` 的 gate 更明确了

`sub_14061F970` 当前已能确认：

```text
if (*(v8 + 848) && a4 == 0)
  -> create Others/symbolcutIOS
  -> iterate ring list *(v8 + 840)
```

单个 node 至少包含：

```text
node + 16 -> int code
node + 24 -> SPAposition center
```

写出前会经过当前 `SPAtransf` 变换，
然后调用 `sub_14053A3F0` 写：

```text
SymbolCutIOSN
  center_x
  center_y
  center_z
  code
```

### 7. 额外弧线分支和 `DrawTaoTong` 有直接关系

`sub_14061F970` 中额外 `api_curve_arc_center_edge` 分支当前可确认：

```text
sub_140610AA0(v8) == 0
pattern[43] != 0
distance_to_plane(firstPoint, viewPlane) < 0.4
visibility / direction gate passes
```

进入后会：

```text
*(byte *)(v8 + 832) = 1
api_curve_arc_center_edge(...)
sub_14053A6B0(...)
++*(v8 + 1052)
```

随后函数尾部会把：

```text
General-Info/DrawTaoTong = "T"
```

所以当前可以确认：

```text
额外弧线分支 != 普通 joints/LineN writer
额外弧线分支 -> 直接参与 DrawTaoTong 置位
```

## 当前仍未闭合

以下几点仍需要旧图石运行确认或更多 IDA 命名闭合：

1. `JointWeldLength` 的旧 UI 单位标签。
2. `pattern / pointNode` 的 owning 结构名和业务枚举名。
3. `sub_14061F970` 额外弧线分支最终落到哪类工程图业务容器。
4. `sub_1406BA2C0` 中 `Varies_ShangH_LZ -> dword_14095D62C = 800`
   到底是专项默认值还是导入覆盖规则。
5. `Others / symbolcutIOS` 的旧 UI 触发路径和非空运行样例。
6. AutoCAD 2020 + FDrawing 是否接受当前新包。

## IDA MCP 会话

本轮使用的 IDA MCP 会话为：

```text
database = visualts_i64_todo045
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
server_health = ok
hexrays_ready = true
```

本轮重点复核函数：

```text
sub_1406ED5E0
sub_1406EDF80
sub_1406AC410
sub_1406AC6A0
sub_1406107F0
sub_14061F970
sub_1407306A0
sub_1405DB340
sub_1405E9640
sub_1405EBA30
sub_1405DFEF0
sub_1406BA2C0
```

## 运行确认清单

由于 IDA 当前已把数学绑定和主要 gate 查到止点，
下一阶段建议转旧图石运行确认：

1. 打开含接头或可创建接头的 SFL。
2. 执行 `新建接头/搭接`，记录菜单路径、状态栏提示、参数窗口截图。
3. 保存前后 SFL，记录 hash 和修改时间。
4. 执行 `移动接头`，记录同样信息。
5. 执行 `接头反向`，记录同样信息。
6. 执行 `清除接头`，记录同样信息。
7. 生成 Detail 包。
8. 在 `DetailNN.stl` 中搜索：

```text
steeljoint-line
joints
Others
symbolcutIOS
DrawTaoTong
```

9. 如出现非空节点，记录完整节点名、属性名、数值样例和所属图纸编号。
10. 如仍为空，记录操作对象、参数和 stop point，避免后续误写成算法已闭合。

## 本轮能力边界

本轮新增的是参数绑定和 gate 证据，不是功能能力：

```text
能确定：
  - JointWeldLength 如何变成 writer 里的对称半长
  - JointRuler / JointDistbet 如何落到旧对象字段和交错规则
  - pattern raw byte 至少是 'L'
  - Others / symbolcutIOS 进入 writer 的 gate
  - 额外弧线分支和 DrawTaoTong 的关系

不能确定：
  - 真实接头线算法已经完成
  - 真实 Others 几何算法已经完成
  - 旧 UI 单位名和触发路径
  - 旧插件接受当前新包
  - AutoCAD L2 pass
  - 完整工程图算法
```

## 验证记录

验证结果回填于：

```text
docs/phase1/app_build_reports/m2_drawing_015_run_001.md
docs/phase1/app_build_reports/m2_drawing_015_run_001.json
```

## 下一步

建议下一阶段执行：

```text
TODO-047 / M2-Drawing-016
  -> 真实接头线 / Others 旧图石运行样例采集 P0
  -> 目标是拿到接头创建 / 移动 / 反向 / 清除后的 Detail 非空样例、截图和 hash
```
