# M2-Drawing-026 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0 实现记录

## 结论

本轮执行 `TODO-057 / 接头 DB6C0 owning 结构与 Dialog #428 确定链补证 P0`。

本轮只读 IDA MCP 和既有文档，没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
1. 类型名补证已经收紧到可开发级：

   dword_140993E74  -> steelbar
   dword_140993EC8 -> steelbargroup
   dword_140993E1C -> seg_steelbargroup

2. DB6C0 的调用层可以区分两类 owner：

   barjoint*
     -> 直接筛 steelbar
     -> 在 obj 本体上写 +108 / +112 / +116
     -> sub_1405DB6C0(obj)

   groupjoint*
     -> 先筛 steelbargroup
     -> groupObj+80 是子接头对象链头
     -> 对每个 child 写 +108 / +112 / +116
     -> sub_1405DB6C0(child)

3. Dialog #428 对应 segjointnew 的对象不是 steelbar / steelbargroup，
   而是 seg_steelbargroup。

4. Dialog #428 的 OK 链已经闭合到可开发 stop point：

   sub_1405D94C0
     -> sub_14045D580
     -> vtable+728(dialog, 428, 0)
     -> ShowWindow(5)
     -> sub_14045D720(OnOK)
     -> sub_1405CB160(segObj, mode, firstOffset, period)
     -> sub_1405B7350(per-node 生成)
     -> CDialog::OnOK
     -> view refresh chain

5. 结合 sub_1405E1CC0 与 Dialog #428 参数链，
   child+112 当前最稳妥的业务名可收紧为：

   phase / position / 起始偏移，单位 int mm，写回时按 period 归一化

6. generated node +112 仍不能命名；
   Dialog #428 的独立 Apply handler 也仍未被静态闭合。
   这两个点本轮保留为明确 stop point。
```

大白话说：

```text
现在已经能把“谁是段组接头对象、谁是钢筋对象、谁是钢筋组对象、Dialog #428
点确定后怎么往下走”讲清楚了。

但还不能说：
  - DB6C0 里的 generated node +112 到底叫啥
  - Dialog #428 是否有独立 Apply，还是只靠默认 OK / message map

所以这轮是“把 owner 和 OK 链钉死”，不是“把所有接头细节全逆出来”。
```

## Control Contract

Primary Setpoint：

```text
在 TODO-056 已确认 DB6C0 静态几何重建主流程后，
继续用 IDA MCP 缩小 owning 结构名、Dialog #428 OK 链和 segjoint 创建链缺口，
并把不能静态闭合的点明确登记为 stop point。
```

Acceptance：

```text
新增 E-IDA-039。
新增 E-DEV-079。
覆盖 steelbar / steelbargroup / seg_steelbargroup 三类 type id 与 debug name。
覆盖 sub_1405D94C0 / sub_14045D580 / sub_14045D650 / sub_14045D720 /
     sub_14045DFF0 / sub_14045DA00 / sub_14045DA10 / sub_14045DA20 /
     sub_1405CB160 / sub_1405B7350 / sub_1405E9640 / sub_1405DFAA0。
明确 child+112 与 generated node+112 的边界。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
TODO-057 done，并把下一步切到 TODO-058，不进入算法实现。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不修改系统目录。
不改 app 业务代码。
不实现真实接头线 / Others 几何算法。
不声明 AutoCAD L2 通过。
不进入 golden。
不把静态链路写成“旧 UI 已完全闭合”。
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
sub_1405E0EA0  -> dword_140993E74 对应 steelbar
sub_1405F1820  -> dword_140993EC8 对应 steelbargroup
sub_1405DA0E0  -> dword_140993E1C 对应 seg_steelbargroup

sub_1405D94C0  -> segjointnew 打开 Dialog #428
sub_14045D580  -> JoingSegDlg 构造 / 初始化
sub_14045D650  -> DDX / DDV
sub_14045D720  -> OnOK
sub_14045DFF0  -> OnInitDialog
sub_14045DA00  -> radio mode = 0
sub_14045DA10  -> radio mode = 1
sub_14045DA20  -> radio mode = 2

sub_1405CB160  -> Dialog #428 确定后的 per-node 分发
sub_1405B7350  -> per-node 接头点列生成

sub_1405DFAA0  -> barjointnew-like 调用层
sub_1405E9640  -> groupjointnew-like 调用层
```

## owning 结构补证

### type id -> 类名

静态 debug name 已补齐：

```text
sub_1405E0EA0:
  dword_140993E74 -> "steelbar"
  source path     -> e:\tushi3d\dam\class\steelbar.cpp

sub_1405F1820:
  dword_140993EC8 -> "steelbargroup"
  source path      -> e:\tushi3d\dam\class\steelbargroup.cpp

sub_1405DA0E0:
  dword_140993E1C -> "seg_steelbargroup"
  source path      -> e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

### DB6C0 调用层 owner 边界

当前最稳妥的静态结构图：

```text
selection item +13
├── barjoint*
│   └── steelbar obj
│       ├── obj+108 / +112 / +116 写回
│       └── sub_1405DB6C0(obj)
├── groupjoint*
│   └── steelbargroup obj
│       ├── obj+80 -> child chain head
│       └── for child:
│             write child+108 / +112 / +116
│             sub_1405DB6C0(child)
└── segjointnew
    └── seg_steelbargroup obj
        └── Dialog #428 / JoingSegDlg
```

工程含义：

```text
DB6C0 不是“只属于一种 owner”的单层结构。

barjoint 路径里，DB6C0 直接吃 steelbar obj。
groupjoint 路径里，DB6C0 吃的是 steelbargroup.obj+80 下的 child。
segjointnew 路径当前还没有直接调用 DB6C0，
而是先进入 Dialog #428，再走 sub_1405CB160 / sub_1405B7350。
```

本轮不能过度推断：

```text
groupjoint child 的精确类名仍未从静态符号里直接恢复。
所以当前只把 owner 层和调用层闭合，
不把 child 的业务类名写死。
```

## Dialog #428 确定链

### 打开链

```text
sub_1405D94C0:
  ctx = sub_1406ED3C0(...)
  first selected item
  obj = *((QWORD*)item + 13)
  if sub_1405DA020(obj):   // seg_steelbargroup
    dialog = operator new(0x148)
    sub_14045D580(dialog, obj, 0)
    vtable+728(dialog, 428, 0)
    ShowWindow(dialog, 5)
```

### 构造 / 字段

```text
sub_14045D580:
  CDialog::CDialog(dialog, 0x1AC, parent)   // 0x1AC = 428
  dialog+320 = segObj
  dialog+304 = 0
  dialog+308 = dword_140994AB8
  dialog+312 = dword_140994AB8
  vtable = JoingSegDlg::vftable
```

### DDX / 参数

```text
sub_14045D650:
  DDX_Radio(..., 1380, dialog+304)
  DDX_Text (..., 1426, dialog+308)
  DDX_Text (..., 1283, dialog+312)
  DDV_MinMaxFloat(..., 0.01, 15000.0)
```

### radio 模式

```text
sub_14045DA00 -> dialog+304 = 0
sub_14045DA10 -> dialog+304 = 1
sub_14045DA20 -> dialog+304 = 2
```

### OnInitDialog

```text
sub_14045DFF0:
  调整窗口尺寸和 OK/Cancel 按钮位置
  隐藏一批控件：
    1261/1262/1263/1264
    1330/1331/1332/1333
    1402/1403/1404/1405
    1481/1479/1480
  最后调用 CDialog::OnInitDialog(this)
```

### OnOK

```text
sub_14045D720:
  ACISExceptionCheck("API")
  sub_1405CB160(
    dialog+320,   // segObj
    dialog+304,   // mode
    dialog+308,   // firstOffset / phase candidate
    dialog+312)   // period
  CDialog::OnOK(this)
  view refresh chain:
    vtable+456(view, segObj, 1)
    sub_1406ED420
    sub_1406ED3C0
    sub_1405F4FB0
    sub_1405F4A90
```

## segjoint 创建链 / 参数语义

### sub_1405CB160

```text
node = *(segObj+80)
for i = 0; node; ++i:
  phase = dialogArg308
  if i is odd:
    phase = 0
  sub_1405B7350(node, dialogArg304, phase, dialogArg312)
  node = *(node+96)
```

工程含义：

```text
Dialog #428 确定后不是直接“整组一次性 rebuild”。
它是沿 segObj+80 子链逐节点分发，
而且会把第 3 个参数按奇偶节点在 {dialogArg308, 0} 之间切换。
```

### sub_1405B7350

关键静态语义：

```text
a3 / a4 先按 /1000.0 转米。
if a3 < 0.02:
  a3 = a4

edge = *(node+72)
edgeLen = EDGE::length(edge, 1)
bounded = get_bounded_curve(edge, 1)

closed edge:
  从 a3 开始，每隔 a4 取一点，end_indexed_polygon(this)

open edge:
  先用 sub_1405D36D0 / sub_14054C2C0 / same_point 判断起点方向
  mode>=1 时按一端起排
  否则按中分偏移起排
  每次取 bounded curve vfunc(+64) 的点
  再 end_indexed_polygon(this)
```

与 DB6C0 的共同常量：

```text
0.02
1000.0
0.5
1.0
```

工程含义：

```text
这说明 Dialog #428 -> segjoint 的确定链
和 DB6C0 主重建链共享同一套“起始偏移 / 周期 / bounded curve 取点”语义，
不是一条完全无关的菜单逻辑。
```

## child+112 与 generated node+112 的边界

本轮把两个 `+112` 明确拆开：

```text
child+112:
  -> sub_1405E1CC0 写回
  -> 按 period 归一化
  -> DB6C0 用 sub_1405DC6E0(child) = child+112 % child+108
  -> 当前最稳妥业务名：
       phase / position / 起始偏移，int mm

generated node+112:
  -> TODO-056 已确认 sub_1405DC6C0 会累加它
  -> 但静态上还不能把它命名成更具体的业务字段
```

工程含义：

```text
TODO-057 关闭的是 child+112 的业务语义收紧，
不是 generated node+112 的最终命名。
后者本轮仍保留为 open item。
```

## Apply / message map stop point

本轮对以下函数做了 xref 复核：

```text
sub_14045D720
sub_14045D650
sub_14045DFF0
sub_14045DA00
sub_14045DA10
sub_14045DA20
```

结果：

```text
xrefs 只稳定命中 data / vtable / message-map-like 数据引用。
当前没有独立代码 xref 把“Apply handler”单独钉出来。
```

结论：

```text
当前能高置信声明：
  sub_14045D720 = JoingSegDlg::OnOK

当前不能高置信声明：
  JoingSegDlg 存在某个已独立定位的 Apply handler
  或 Apply 与 OK 的复用/分离关系已经静态闭合
```

所以本轮 stop point 写实为：

```text
Dialog #428 的 OK 链已闭合。
Apply handler 仍待后续 message map / 资源 / 运行截图证据。
```

## 本轮 stop point

已闭合：

```text
dword_140993E74 / dword_140993EC8 / dword_140993E1C 的 debug 类名。
barjoint / groupjoint / segjoint 的 owner 层边界。
Dialog #428 的打开链、构造字段、DDX、radio、OnInit 和 OnOK。
sub_1405CB160 -> sub_1405B7350 的 segjoint 确定后 per-node 生成链。
child+112 的业务语义已可收紧为 phase / position / 起始偏移，int mm。
```

仍未闭合：

```text
groupjoint child 的精确业务类名。
generated node+112 的业务名。
Dialog #428 独立 Apply handler。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
真实接头线算法。
真实 Others 几何算法。
golden。
```

后续建议：

```text
TODO-058 / JoingSegDlg message map 与 child/node+112 字段收口 P1
```
