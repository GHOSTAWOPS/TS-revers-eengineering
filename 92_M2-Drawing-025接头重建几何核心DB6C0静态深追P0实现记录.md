# M2-Drawing-025 接头重建几何核心 DB6C0 静态深追 P0 实现记录

## 结论

本轮执行 `TODO-056 / 接头重建几何核心 sub_1405DB6C0 静态深追 P0`。

本轮只读 IDA MCP 和既有文档，没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
sub_1405DB6C0(child) 是接头生成链的几何重建核心。

它的输入不是 OCCT/ACIS 直接生成规则，而是旧 VisualTS 接头业务对象字段：
  child+88  = 已生成接头几何 / 显示链头
  child+108 = JointRuler / period，int mm
  child+112 = phase / position，int mm，按 period 取模
  child+116 = reverse flag

主流程：
  1. 先 sub_1405DBE20(child)，清 child+88 旧生成链。
  2. 读取 period 和 phase，按 /1000.0 转成米。
  3. phaseM < 0.02 时回退为 periodM。
  4. reverse flag 为真时，从 child+88 链尾开始反向遍历。
  5. 遍历 node+72 的 EDGE，按 EDGE::length 累计距离。
  6. 到达 phase/period 节点时，用 get_bounded_curve + bounded_curve vfunc(+64) 取 SPAposition。
  7. 调 direct_render_mesh_manager::end_indexed_polygon(node, point, int_mm_index) 写回生成点。
```

复刻影响：

```text
旧图石接头不是 ACIS/HOOPS 自动创建业务钢筋。
旧 VisualTS 自己根据 period / phase / reverse 和 EDGE 链计算点位，再调用几何/显示库能力。

新系统后续要复刻的是：
  VisualTS 接头业务字段和重建流程
  -> 通过 LegacyGeometryAdapter 取得 EDGE 长度、端点、连接点、曲线参数点
  -> 生成新系统 domain / drawing 所需接头语义

不能把本轮证据理解成“直接用 OCCT 写一个差不多的接头线算法”。
```

## Control Contract

Primary Setpoint：

```text
在不自动启动旧图石、不改 app 代码的前提下，
用 IDA MCP 深追 sub_1405DB6C0 的曲线取点、反向遍历、清旧链和输出调用语义。
```

Acceptance：

```text
新增 E-IDA-038。
新增 E-DEV-078。
覆盖 sub_1405DB6C0 / sub_1405DBE20 / sub_1405DC6E0 / sub_1405DC840 /
     sub_1405E06F0 / sub_1405E05A0 / sub_1405E1E70 / sub_1405D36D0 /
     sub_14054C2C0 / sub_14054C820 / sub_1405BBEF0 / sub_1405DC870 /
     sub_1405DC6C0。
记录字段偏移、遍历方向、距离累计、曲线参数取点、输出调用寄存器和剩余 stop point。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
TODO-056 done，下一步只切到 TODO-057，不进入算法实现。
```

Guardrail：

```text
不实现真实接头线 / Others 几何算法。
不声明 AutoCAD L2 通过。
不启动旧图石。
不安装 HASP。
不改 app 业务代码。
不进入 golden。
不把静态几何重建证据写成旧插件接受度或完整工程图算法。
```

## IDA MCP 会话

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
strings_cache_size = 16320
```

## 本轮覆盖函数

```text
sub_1405DB6C0  -> 接头生成链几何重建核心
sub_1405DBE20  -> 清 child+88 旧生成链
sub_1405DC6E0  -> 读取 child+112 % child+108
sub_1405DC840  -> 沿 child+88 / node+88 链找尾节点
sub_1405E06F0  -> 统计 child+88 / node+88 链节点数
sub_1405E05A0  -> 取末端参考点
sub_1405E1E70  -> 取起端参考点
sub_1405D36D0  -> 多段链连接点 / 位置分类 helper
sub_14054C2C0  -> 两条 EDGE 的连接点 / 最近点 helper
sub_14054C820  -> 两条 EDGE 的保守端点连接 helper
sub_1405BBEF0  -> 沿 owner/head 的 node+88 链找当前节点前驱
sub_1405DC870  -> 旧链总长度估算 helper
sub_1405DC6C0  -> 累加生成链节点 +112
```

## sub_1405DB6C0 主流程

静态流程：

```text
sub_1405DBE20(child)
node = *(child+88)
period = *(int *)(child+108)
if !node or period < 1:
  return

periodM = period / 1000.0
phaseM = sub_1405DC6E0(child) / 1000.0
if phaseM < 0.02:
  phaseM = periodM

accumulated = 0.0
if *(byte *)(child+116):
  node = sub_1405DC840(child)

while node:
  edge = *(node+72)
  directionFlag = old connection / dot-product / endpoint-distance logic
  edgeLen = EDGE::length(edge, 1)

  if phaseM <= accumulated + edgeLen:
    bounded = get_bounded_curve(edge, 1)
    scale = 1.0 if is_linear_edge(edge) else (bounded.paramEnd - bounded.paramStart) / edgeLen
    distanceOnEdge = phaseM - accumulated
    if directionFlag < 1:
      curveParam = bounded.paramStart + distanceOnEdge * scale
    else:
      curveParam = bounded.paramEnd - distanceOnEdge * scale
    point = bounded.vfunc(+64)(curveParam)
    intMmIndex = int(phaseM * 1000.0 + 0.5)
    direct_render_mesh_manager::end_indexed_polygon(node, &point, intMmIndex)
    phaseM = periodM
    repeat while remaining accumulated distance >= periodM

  node = reverse ? sub_1405BBEF0(node) : *(node+88)
```

关键常量：

```text
0x1408bbab8 = 1000.0  // mm <-> m
0x14075a798 = 0.02    // phaseM 下限
0x1408bba30 = 0.5     // int mm 四舍五入偏移
0x1408bba40 = 1.0     // linear edge scale
```

## 输出调用寄存器事实

Hex-Rays 对 `direct_render_mesh_manager::end_indexed_polygon` 的原型不完整，伪代码显示为只传 `v2`。

以 `sub_1405DB6C0` 调用点反汇编为准：

```text
0x1405dbb15  SPAposition copy ctor
0x1405dbb1b  mov r8d, ebx       // int mm index
0x1405dbb1e  mov rcx, rsi       // this / generated-chain node
0x1405dbb21  mov rdx, rax       // SPAposition*
0x1405dbb24  call direct_render_mesh_manager::end_indexed_polygon
```

结论：

```text
rcx = generated-chain node
rdx = SPAposition*
r8d = int mm index
```

本轮不能声明该函数完整类原型已恢复，只能声明 `DB6C0` 调用点寄存器语义已确认。

## helper 语义

### sub_1405DBE20

```text
for node = *(child+88); node; node = *(node+88):
  sub_1405B9640(node)
```

含义：

```text
重建前先清旧生成链。
```

### sub_1405DC6E0

```text
period = *(int *)(child+108)
phase = *(uint *)(child+112)
return period ? phase % period : phase
```

含义：

```text
child+112 是周期内相位 / 位置，单位仍是 int mm。
```

### sub_1405DC840 / sub_1405BBEF0

```text
sub_1405DC840(child):
  从 child+88 沿 node+88 找尾节点

sub_1405BBEF0(node):
  从 *(node+80)+88 的链头开始找当前 node 的前驱
```

含义：

```text
child+116 reverse flag 会把遍历方向从 head->next 改成 tail->prev。
```

### sub_1405E06F0

```text
for node = *(child+88); node; node = *(node+88):
  count++
```

含义：

```text
`DB6C0` 用它区分单段链和多段链。
```

### sub_1405E05A0 / sub_1405E1E70

```text
sub_1405E05A0(child):
  无生成链时返回 0,0,0
  单节点时返回 *(child+80)+192
  多节点时用最后节点 node+104 调 sub_1405D36D0 取末端连接点

sub_1405E1E70(child):
  无生成链时返回 0,0,0
  单节点时返回 *(child+80)+168
  多节点时用首节点 node+104 调 sub_1405D36D0 取起端连接点
```

含义：

```text
单段链使用 owning 对象中的起止参考点。
多段链通过连接点 helper 推导端点。
```

### sub_1405D36D0 / sub_14054C820 / sub_14054C2C0

```text
sub_1405D36D0:
  返回 0/1/2/3 或 -1，表示当前 node 在多段链中的位置分类。
  分类 1/2 时调 sub_14054C820。
  分类 3 且有当前 node 时，根据 a4 选择前驱或后继 EDGE，再调 sub_14054C2C0。

sub_14054C820:
  只在两条 EDGE 端点之间做保守连接点选择。

sub_14054C2C0:
  先判断四种端点 same_point。
  a4 为真时可调用 api_entity_entity_distance 得到最近点。
  a4 为假时在四个端点距离里选最近端点。
```

含义：

```text
多段链连接点不是随便取端点。
旧逻辑会按 node 在链中的位置、前驱/后继 EDGE 和端点距离选择连接点。
```

### sub_1405DC870 / sub_1405DC6C0

```text
sub_1405DC870(child, a2, a3):
  累加 child+88 链上每个 node 的 vfunc(+344)(a2,a3)。
  特定模型名 / 配置分支可能额外加长度修正。

sub_1405DC6C0(child):
  累加 child+88 链上每个 node+112。
```

含义：

```text
这两个 helper 支持旧参数窗口和动作函数做长度 / phase 相关判断。
本轮不命名完整 owning 结构。
```

## DB6C0 callers

`sub_1405DB6C0` 的 code xrefs：

```text
sub_1405DFAA0  -> barjointnew 类批量新建入口，写 +108/+112/+116 后 rebuild
sub_1405DFEF0  -> barjointmove 类单对象移动入口，写 +112 后 rebuild
sub_1405E02D0  -> barjointrev 类单对象反向入口，切换 +116 后 rebuild
sub_1405E9640  -> groupjointnew，逐 child 写 period/phase/reverse 后 rebuild
sub_1405EBA30  -> groupjointmove，逐 child 写 phase/reverse 后 rebuild
sub_1405ED6C0  -> groupjointrev，逐 child 切换 reverse 后 rebuild
```

含义：

```text
DB6C0 是 barjoint/groupjoint 新建、移动、反向动作后的共同重建入口。
```

## 字段语义更新

```text
child+88  = generated chain head
child+108 = period / JointRuler, int mm
child+112 = phase / position, int mm modulo period
child+116 = reverse flag

node+72   = EDGE*
node+80   = owner/head for predecessor lookup
node+88   = next generated-chain node
node+104  = multi-edge connection point participant
node+112  = summed by sub_1405DC6C0; business name still open
```

## 本轮 stop point

已闭合：

```text
DB6C0 的清旧链、period/phase/reverse 字段驱动。
正向 / 反向遍历方式。
单段 / 多段链连接点 helper。
EDGE::length + get_bounded_curve + bounded_curve vfunc(+64) 取点。
linear edge scale = 1.0，非线性 edge 用参数区间 / 长度换算。
end_indexed_polygon 调用点的 rcx/rdx/r8d 语义。
barjoint/groupjoint new/move/rev caller 集合。
```

仍未闭合：

```text
DB6C0 owning 结构名和 node+112 业务名。
Dialog #428 确定按钮后的 segjoint 创建链。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
真实接头线算法。
真实 Others 几何算法。
golden。
```

后续建议：

```text
TODO-057 / 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0
```

