# M2-Drawing-020 Others / symbolcutIOS producer 与接头 UI 触发静态补证 P0

## 结论

本轮执行 `TODO-051 / Others symbolcutIOS producer 与接头 UI 触发静态补证 P0`。

本轮已经用 IDA MCP 把 `Others / symbolcutIOS` 的上游 producer 继续追深一层：

```text
sub_14060C940
  -> api_entity_entity_distance
  -> api_mk_ed_line
  -> sub_14060A810
  -> HVIEWPORT +840 / +848
  -> sub_14061F970
  -> sub_14053A3F0
  -> Others / symbolcutIOS / SymbolCutIOSN
```

工程含义：

```text
旧图石的 Others / symbolcutIOS 不是 Detail writer 凭空生成。
它来自旧工程图 / 视口上下文里的一个 ring/list。
该 list 的节点内容是 code + SPAposition center。
sub_14061F970 只是在出图写 XML 时消费这个旧上下文。
```

本轮仍然没有闭合：

```text
旧 UI 中文 caption / Ribbon 按钮路径。
旧图石真实非空运行样例。
AutoCAD L2 接受度。
真实接头线算法。
真实 Others 几何算法。
```

本轮不启动旧图石，不安装 HASP，不修改系统目录，不改 app 业务代码，不实现任何接头线 / Others 算法。

## 控制合同

Primary Setpoint：

```text
在 TODO-050 受用户现场运行条件阻塞时，
用 IDA MCP 静态缩小 Others / symbolcutIOS producer 和接头 UI 触发缺口。
```

Acceptance：

```text
新增 E-IDA-033。
新增 E-DEV-073。
至少追一层 HVIEWPORT +840 / +848 producer。
记录接头命令表静态线索和 UI caption stop point。
更新 03 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
```

Guardrail：

```text
不得把静态 producer 证据写成旧运行样例。
不得把英文内部命令表写成旧 UI 中文 caption。
不得声明 AutoCAD L2 通过。
不得实现真实接头线 / Others 几何算法。
不得让 OCCT 直接进入钢筋业务层。
```

## IDA MCP 会话

本轮使用：

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
```

复核函数：

```text
sub_14061F970
sub_14060A810
sub_14060C940
sub_14060E740
```

## writer 链复核

`sub_14061F970` 仍是 `Others / symbolcutIOS` 写出主链。

当前确认：

```text
callers:
  sub_140600AA0
  sub_140621470
  sub_140625760

callee:
  sub_14053A3F0
```

已有证据表明 `sub_14053A3F0` 写出：

```text
SymbolCutIOS%ld
center_x
center_y
center_z
code
```

本轮继续确认：

```text
sub_14061F970 消费 HVIEWPORT 上下文中的 +840 list 和 +848 count / marker。
当 +848 非空且 writer 条件满足时，才创建 Others / symbolcutIOS。
```

## HVIEWPORT 初始化证据

`sub_14060E740` 是一个 `HVIEWPORT` 风格对象构造函数。

IDA 复核要点：

```text
*a1 = HVIEWPORT::vftable
ENTITY_LIST init:
  +776
  +792
  +800
  +808
  +816
  +824

+840:
  先清零，再调用 sub_14060E470 初始化 ring/list sentinel

+848:
  构造期清零
```

工程含义：

```text
+840 是 symbolcutIOS 候选点链表。
+848 是该链表的 count / non-empty marker。
```

## producer 证据

### sub_14060A810

`sub_14060A810` 是当前确认的直接 producer。

反编译要点：

```text
node = sub_14060E470(a1)
*(node + 16) = *a4
SPAposition(node + 24, a4 + 8)
```

字段含义：

```text
node + 16 = code
node + 24 = center SPAposition
```

这和 `sub_14061F970 -> sub_14053A3F0` 读取的 `code + center` 对齐。

### sub_14060C940

`sub_14060A810` 的唯一 caller 是 `sub_14060C940`。

`sub_14060C940` 的关键链路：

```text
api_entity_entity_distance(...)
if distance <= 0.001:
  api_get_edges(...)
  api_get_vertices(...)
  api_mk_ed_line(center, projected-center, &edge)
  ENTITY_LIST::add(HVIEWPORT +824, edge)
  prepare var:
    code   = *(source + 72)
    center = SPAposition(...)
  sub_14060A810(HVIEWPORT +840, sentinel, tail, &var)
  ++*(HVIEWPORT +848)
```

反编译地址线索：

```text
api_entity_entity_distance at 0x14060d5e3
api_mk_ed_line at 0x14060d78a
sub_14060A810 call at 0x14060d7ea
+848 increment at 0x14060d80f
```

`sub_14060C940` 还带有源文件字符串：

```text
e:\tushi3d\dam\yxj\yxt.cpp
```

保守解释：

```text
这段逻辑很可能属于旧图石工程图 / 视图 / 切口符号相关生成链。
当前证据足以说明 symbolcutIOS producer 的数据结构和几何触发条件，
但还不足以命名完整旧业务功能或 UI 按钮。
```

## 接头命令表补证

本轮继续沿用并复核 `TODO-050 / E-IDA-032` 的接头内部命令边界。

静态命令表线索：

```text
0x140959328 -> 0x140768480 -> "barjointnew"
0x14095ae70 -> 0x140768480 -> "barjointnew"

0x1409593e8 -> 0x140768548 -> "groupjointrev"
0x14095af90 -> 0x140768548 -> "groupjointrev"

0x14095ae78 -> sub_1405DFAA0
0x14095af98 -> sub_1405F0850
0x14095afa0 -> sub_14054AF20
```

当前仍不能写成：

```text
这些表项已经等价于中文 Ribbon caption。
这些表项已经闭合旧 UI 真实点击路径。
```

原因：

```text
本轮继续搜索 接头 / 搭接 / 焊接 / 新建接头 / 移动接头 / 清除接头 等普通字符串，
仍没有直接命中中文 UI caption。
```

保守结论：

```text
旧 UI caption 可能来自资源表、Codejock / Ribbon 资源、外部 UI 数据或运行期构造。
下一步需要继续做静态资源 / Ribbon 绑定补证，或由用户运行旧图石截图确认。
```

## 本轮已闭合 / 未闭合

已闭合：

```text
Others / symbolcutIOS writer = sub_14061F970 -> sub_14053A3F0。
HVIEWPORT +840 是 producer list sentinel。
HVIEWPORT +848 是 count / non-empty marker。
sub_14060A810 写入 list node 的 code + center。
sub_14060C940 在距离判断和短线生成后调用 sub_14060A810 并递增 +848。
```

未闭合：

```text
旧 UI 中文 caption。
Codejock / Ribbon 资源到内部命令的完整绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 动态导入。
真实接头线算法。
真实 Others 几何算法。
```

## xhigh review

```text
xhighReview = not_required_docs_only
```

原因：

```text
本轮只新增 / 更新文档、报告和 todo.csv。
没有修改 app 代码、测试代码或构建脚本。
```

## 验证结果

本轮验证：

```text
git diff --check = pass
  -> 仅 CRLF/LF 换行提示，无 whitespace error

todo.csv / json parse = pass
  -> next = TODO-052
  -> blocked = TODO-050
  -> m2_drawing_020_run_001.json valid

readiness gate strict = pass
  -> M1-Formal-Ready
  -> 84 / 84 pass
  -> 0 error
  -> 0 warning

domain boundary scan = pass
  -> app/src/domain/rebar
  -> app/src/drawing
  -> app/src/project

CTest = pass
  -> 18 / 18 pass
  -> total real time 161.94 sec
```

## 边界声明

本记录证明：

```text
TODO-051 已完成静态补证。
IDA MCP 已使用。
symbolcutIOS producer 链已从 writer 继续追到 HVIEWPORT +840 / +848 的直接生产者。
```

本记录不证明：

```text
旧图石运行样例已采到。
旧 UI caption 已闭合。
AutoCAD L2 已通过。
真实接头线算法已实现。
真实 Others 几何算法已实现。
完整工程图已完成。
golden 已采集。
```

## 下一步

建议下一轮进入：

```text
TODO-052 / M2-Drawing-021
  -> 接头 UI caption / Ribbon 绑定静态资源补证 P0
```

目标：

```text
继续追中文 caption、Ribbon / Codejock 资源、command id 到 handler 的绑定。
如果静态仍不能闭合，则形成用户旧图石运行截图最小清单。
```
