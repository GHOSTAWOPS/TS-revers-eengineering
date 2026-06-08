# M2-Drawing-027 JoingSegDlg message map 与 child/node+112 字段收口 P1 实现记录

## 结论

本轮执行 `TODO-058 / JoingSegDlg message map 与 child/node+112 字段收口 P1`。

本轮只读 IDA MCP 和既有文档，没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
1. JoingSegDlg 自身的静态 message map 已闭合：

   sub_14045D6F0
     -> sub_14045D700
     -> off_14075B110
        +0x00 = CDialog::GetThisMessageMap
        +0x08 = 0x14075B090 entries

2. 0x14075B090 只稳定解出 3 个 WM_COMMAND radio handler：

   1380 -> sub_14045DA00 -> dialog+304 = 0
   1384 -> sub_14045DA10 -> dialog+304 = 1
   1381 -> sub_14045DA20 -> dialog+304 = 2

3. JoingSegDlg 自身静态 message map 里没有独立 Apply handler 证据。
   当前最稳妥口径是：
   Dialog #428 走默认 OK / Cancel 框架，
   外加 3 个 radio 模式切换 handler。

4. child+112 的边界维持前一轮结论：

   phase / position / 起始偏移，单位 int mm，
   写回后按 period 归一化。

5. generated node+112 又收紧了一层：

   它位于 child+88 生成链节点上，
   是一个 int mm 贡献字段，
   被 sub_1405DC6C0 汇总，
   并进入字符串格式化、状态显示和 rebuild 前置判断。

6. generated node+112 仍没有稳定调试名；
   旧中文 caption、运行期静态表外 Apply、非空运行样例仍保留为 stop point。
```

大白话说：

```text
现在可以明确地说，Dialog #428 自己不是一大坨“神秘按钮逻辑”。

它静态上就是：
  一个默认对话框
  + 3 个 radio 切换
  + OnOK

而 generated node+112 也不再只是“某个 +112”了，
现在至少知道它属于生成链节点，
而且是会被汇总、拿去做显示和判断的 int mm 字段。

但还不能把它最后的旧业务名写死。
```

## Control Contract

Primary Setpoint：

```text
在 TODO-057 已闭合 owner 类型层和 Dialog #428 OK 链后，
继续用 IDA MCP 缩小 JoingSegDlg message map / Apply
与 child/node+112 的最终静态边界缺口。
```

Acceptance：

```text
新增 E-IDA-040。
新增 E-DEV-080。
覆盖 JoingSegDlg message map / resource xref / radio handler / Apply stop point。
覆盖 child+112 与 generated node+112 的最终静态边界。
形成最小运行确认清单。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
TODO-058 done，并把下一步切到 TODO-059，不进入算法实现。
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
不把静态 message map stop point 写成“运行期按钮绝对不存在”。
```

## 本轮门禁补充

本轮除了补 `IDA` 证据，还顺手把 `TODO-058` 对应的 done-node 门禁补严了一层：

```text
1. RouteGuardrail 现在不仅要求实现记录 + run report md，
   还要求同名 run report json 一起存在。

2. done-node 报告里如果还残留
   pending_before_commit
   pending_update_after_verification
   waiting_rereview
   这类“未最终闭环”状态，
   gate 会直接把它当成错误，不再允许 TODO 先写 done。

3. 第二轮 xhigh 只读 review 完成后，
   本轮 run report 的 xhighReview 已收口到最终完成态。
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
sub_14045D6F0   -> JoingSegDlg GetMessageMap thunk
sub_14045D700   -> 返回 JoingSegDlg message map
sub_14045D580   -> JoingSegDlg 构造 / 初始化
sub_14045D650   -> DDX / DDV
sub_14045D720   -> OnOK
sub_14045DFF0   -> OnInitDialog
sub_14045DA00   -> radio mode = 0
sub_14045DA10   -> radio mode = 1
sub_14045DA20   -> radio mode = 2
sub_1405DC6C0   -> 汇总 generated node+112
sub_1405DC6E0   -> 读取 child+112 % child+108
sub_1405DBC20   -> 格式化字符串时使用 sub_1405DC6C0
sub_1405DFEF0   -> 先判定 sub_1405DC6C0(child) 再 phase 写回 / DB6C0
sub_1405E02D0   -> 先判定 sub_1405DC6C0(child) 再 clear/rebuild
sub_1405EAEC0   -> 对 group/seg 子链继续累加 sub_1405DC6C0
sub_1406F72A0   -> 状态栏路径读取 sub_1405DC6C0
```

## JoingSegDlg message map

### 静态对象

```text
sub_14045D6F0 -> sub_14045D700 -> &off_14075B110

off_14075B110:
  +0x00 -> 0x140709724 = CDialog::GetThisMessageMap
  +0x08 -> 0x14075B090 = JoingSegDlg entries
```

### entries 解码

`0x14075B090` 按 32 字节一项稳定解码为：

```text
entry0:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0564 = 1380
  lastId  = 0x0564 = 1380
  sig     = 0x3A
  handler = sub_14045DA00

entry1:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0568 = 1384
  lastId  = 0x0568 = 1384
  sig     = 0x3A
  handler = sub_14045DA10

entry2:
  message = 0x0111 (WM_COMMAND)
  id      = 0x0565 = 1381
  lastId  = 0x0565 = 1381
  sig     = 0x3A
  handler = sub_14045DA20

terminator:
  all zero
```

工程含义：

```text
JoingSegDlg 自身静态表当前只声明 3 个 radio 模式切换命令。
它们和 DDX_Radio(control 1380) 以及 mode=0/1/2 写回完全对齐。
```

## Apply / OK / Cancel 边界

高置信结论：

```text
sub_14045D720 = JoingSegDlg::OnOK

JoingSegDlg 自身静态 message map 中：
  没有独立命中 IDOK(1)
  没有独立命中 IDCANCEL(2)
  没有独立命中 Apply-like handler

sub_14045DFF0 只明确调整：
  GetDlgItem(1) / GetDlgItem(2) 的位置
  以及一批参数相关控件的 ShowWindow(0)
```

当前最稳妥口径：

```text
Dialog #428 走默认 OK / Cancel 框架，
JoingSegDlg 自己额外声明了 3 个 radio handler，
但静态上没有独立 Apply handler 证据。
```

本轮不夸大为：

```text
运行期绝对没有 Apply 形态按钮
```

## Dialog #428 资源 / 控件 xref

当前已稳定闭合：

```text
sub_14045D580:
  CDialog::CDialog(dialog, 0x1AC, parent)   // 0x1AC = 428

sub_14045D650:
  DDX_Radio(..., 1380, dialog+304)
  DDX_Text (..., 1426, dialog+308)
  DDX_Text (..., 1283, dialog+312)
  DDV_MinMaxFloat(..., 0.01, 15000.0)

sub_14045DFF0:
  GetDlgItem(1) / GetDlgItem(2)
  ShowWindow(0):
    1261/1262/1263/1264
    1330/1331/1332/1333
    1402/1403/1404/1405
    1481/1479/1480
```

工程含义：

```text
Dialog #428 的参数区、radio 控件和一批扩展控件已经有稳定控件 id 证据，
但这些资源级 xref 仍不能直接推出旧中文 caption、
右键菜单文案或运行期按钮文字。
```

## child+112 与 generated node+112 的最终静态边界

### child+112

```text
写回路径：
  sub_1405E1CC0

读取路径：
  sub_1405DC6E0(child) = child+112 % child+108

当前最稳妥业务边界：
  phase / position / 起始偏移，int mm，按 period 归一化
```

### generated node+112

```text
sub_1405DC6C0(a1):
  generated = *(a1+88)
  sum += generatedNode+112

caller coverage:
  sub_1405DBC20
  sub_1405DFEF0
  sub_1405E02D0
  sub_1405EAEC0
  sub_1406F72A0
```

当前可高置信收紧为：

```text
generated node+112:
  -> 位于 child+88 生成链节点上
  -> 是一个 int mm 贡献字段
  -> 会被 sub_1405DC6C0 汇总
  -> 汇总值会进入字符串格式化、状态显示和 rebuild 前置判断
```

当前仍不能高置信声明：

```text
generated node+112 的旧业务调试名
generated node+112 在旧 UI 上的最终中文展示名称
```

## 最小运行确认清单

如果后续用户现场可以运行旧图石，最小补证清单收敛为：

```text
1. 打开 segjointnew / Dialog #428，
   截全窗口，必须包含标题栏和底部按钮区。

2. 确认底部按钮：
   只有 OK/Cancel，还是还有 Apply / 应用 / 预览类按钮。

3. 切换 3 个 radio 模式，
   记录是否存在“不点确定也立即生效”的行为。

4. 输入两个数值框后确定，
   重新打开同一对象，记录参数是否回显，
   并截图状态栏/尺寸串是否变化。
```

## 本轮 stop point

已闭合：

```text
JoingSegDlg 自身静态 message map。
3 个 radio handler 的 id / handler / mode 写回关系。
Dialog #428 资源控件 id 的静态边界。
child+112 与 generated node+112 的分层边界。
standalone Apply 未在 JoingSegDlg 自身静态表中出现。
```

仍未闭合：

```text
generated node+112 的最终业务调试名。
旧中文 caption / 右键菜单项绑定。
运行期是否存在静态表外的 Apply 形态按钮。
旧图石非空运行样例。
AutoCAD L2 通过。
真实接头线算法。
真实 Others 几何算法。
golden。
```

## 后续建议

```text
TODO-059 / generated node+112 展示/状态栏字符串链静态深追 P1
```
