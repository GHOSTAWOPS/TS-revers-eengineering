# M2-Drawing-022 接头 ContextMenu / Command Dispatch 静态深追 P0

## 结论

本轮执行 `TODO-053 / 接头 context menu / command dispatch 静态深追 P0`。

本轮已经用 IDA MCP 和只读 PE 资源解析继续追查旧图石接头命令入口，当前结论是：

```text
动态右键菜单机制：已确认存在
主要 popup 构造函数：sub_1405C2EF0 / sub_1406BA690
LoadMenuW 菜单资源路径：0xAD / 0xF8 已确认，但不含已知接头 command id
全量 MENU 资源：未发现已知接头 command id
已知接头 command id 普通 immediate 搜索：仍未命中
handler 表：仍只能证明内部命令和 handler 存在
旧 UI caption / 右键菜单项 / dispatch 绑定：仍未闭合
```

大白话说：

```text
旧图石确实有动态右键菜单，也确实有接头内部命令。
但这轮静态证据仍没看到“右键菜单某一项直接挂到 barjointnew / groupjointnew 等接头命令”。
所以不能把接头 UI 入口写成已闭合。
```

本轮不启动旧图石，不安装 HASP，不修改系统目录，不改 app 业务代码，不实现任何接头线 / Others 算法。

## 控制合同

Primary Setpoint：

```text
在 TODO-052 已确认接头命令 ID 未直接挂入 sub_1406F37B0 Ribbon 构造路径后，
继续静态查右键 / 对象上下文菜单 / command dispatch / menu resource 加载路径。
```

Acceptance：

```text
新增 E-IDA-035。
新增 E-DEV-075。
至少覆盖 command dispatch、message map / handler 表、popup/context menu、LoadMenuW 菜单资源、Codejock command bar 相关路径。
如果静态不能闭合，形成明确 stop point 和旧图石运行截图最小清单。
更新 00 / 03 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
```

Guardrail：

```text
不得把内部英文命令名写成中文 UI caption。
不得把 handler 表写成 UI 入口已闭合。
不得把 .pdata unwind metadata 写成 dispatch 证据。
不得声明旧运行样例已采到。
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

IDA MCP 健康检查：

```text
status = ok
module = VisualTS.exe
auto_analysis_ready = true
hexrays_ready = true
strings_cache_size = 16320
```

## 已知接头 command id 复核

本轮继续沿用 `TODO-052` 已确认的接头内部命令：

```text
0x8CCE / 36046 -> barjointnew
0x8CCF / 36047 -> barjointclear
0x8CD0 / 36048 -> barjointmove
0x8CD1 / 36049 -> barjointrev

0x8CDD / 36061 -> groupjointnew
0x8CDE / 36062 -> groupjointclear
0x8CDF / 36063 -> groupjointrev
0x8CE0 / 36064 -> groupjointmove

0x8CD9 / 36057 -> goujianjointnew
0x8CDA / 36058 -> goujianjointclear

0x8D91 / 36241 -> segjointnew
0x8D92 / 36242 -> segjointclear

0x8CD7 / 36055 -> featjointnew
0x8CD8 / 36056 -> featjointclear
```

本轮再次做 immediate 搜索：

```text
36046, 36047, 36048, 36049,
36055, 36056,
36057, 36058,
36061, 36062, 36063, 36064,
36241, 36242

=> no immediate matches
```

邻近值 `0x8CDC / 36060` 有两处命中：

```text
0x1406f4aa8 -> sub_1406F37B0
0x1406f5f6b -> sub_1406F4EC0
```

但 `0x8CDC` 当前不在已确认接头 command id 表中，因此只能写成邻近 / 可疑命令 ID 命中，不能归类为接头 UI 入口证据。

## handler 表 xref 边界

本轮复核 handler xref 时继续确认：

```text
0x14095ae.. = 真实 .data handler 表
0x140a0.... = .pdata unwind metadata
```

重要纠偏：

```text
.pdata 是异常 / unwind 元数据，不是业务 command dispatch 表。
不能把 0x140a0... 的 xref 当成 UI 调度证据。
```

对 `0x14095ae40 / 0x14095ae70 / 0x140959300 / 0x140959320` 查询普通 xref 仍没有直接业务引用。

保守解释：

```text
handler 表可能由 MFC / 框架静态 metadata 或生成结构消费，
也可能经间接扫描 / 注册逻辑使用；
当前没有普通 code xref 能把它直接连到 UI 菜单项。
```

## popup / context menu 调用面

本轮查询菜单 API xref：

```text
CreatePopupMenu -> sub_1405C2EF0, sub_1406BA690
AppendMenuA     -> sub_1405C2EF0, sub_1406BA690
TrackPopupMenu  -> sub_1405C2EF0, sub_1406B1A00, sub_1406BA690, sub_1407072E0
LoadMenuW       -> sub_1406B1A00, sub_1407072E0
```

这能确认旧图石存在多条右键 / 弹出菜单路径。

但这些路径当前只能证明菜单机制存在，不能自动证明接头命令被挂入。

## sub_1406BA690

`sub_1406BA690(__int64 a1, tagPOINT a2, __int64 a3, __int64 a4)` 当前确认：

```text
使用 ClientToScreen
使用 CreatePopupMenu
使用 AppendMenuA
使用 CMenu::TrackPopupMenu
调用 sub_1405C2EF0
按 FACE / EDGE / VERTEX 等选择状态构造动态菜单
```

当前能看到的大量菜单 ID 包括：

```text
0x8927, 0xE120,
0x8CAC, 0x8AAD, 0x8AAF,
0x8D38, 0x8928, 0x8CE1, 0x8CA4, 0x8DD9,
0x8D40, 0x8D41, 0x8D42, 0x8D43, 0x8E2F, 0x8E30,
0x8CED, 0x8D48, 0x8D49, 0x8DDC, 0x8DDE, 0x8DDB, 0x8E27, 0x8E26,
0x8D22, 0x8D23, 36132, 0x8E32, 36164, 0x8DD4,
0x8D36, 0x8DDD, 0x8DFE, 0x8DDA, 0x8DD3, 0x8DDF,
36343, 36150, 0x8D46, 36140
```

保守结论：

```text
sub_1406BA690 证明旧 VisualTS 有几何选择相关的动态 context menu。
但当前静态 decompile / immediate 搜索未发现已知接头 command id。
```

## sub_1405C2EF0

`sub_1405C2EF0(__int64 a1, tagPOINT a2, __int64 a3)` 当前确认：

```text
函数大小约 8651 bytes
switch 约 155 cases
cyclomatic complexity = 171
使用 CreatePopupMenu / AppendMenuA / CMenu::TrackPopupMenu
调用多处钢筋 / EDGE / ENTITY_LIST 相关函数
由 sub_1406BA690 调用
```

它是本轮最重要的动态菜单函数之一。

但当前全局 immediate 搜索显示：

```text
已知接头 command id 没有普通 immediate 命中。
```

因此当前不能把 `sub_1405C2EF0` 写成接头菜单入口闭合。

## LoadMenuW 菜单资源路径

本轮确认两个 `LoadMenuW` 路径：

```text
sub_1406B1A00 -> LoadMenuW(resource 0xAD)
sub_1407072E0 -> LoadMenuW(resource 0xF8)
```

只读解析 `VisualTS.exe` MENU 资源：

```text
menu_resource_count = 15
menu_resource_names =
  0x7e, 0x7f, 0x95, 0x96, 0x97,
  0xa6, 0xa8, 0xa9, 0xad, 0xae,
  0xbc, 0xbd, 0xf8, 0x1ae, 0x3f7
```

资源 `0xAD`：

```text
jointIds = []
cmd-like ids include 0x8969, 0x896a, 0x896b, 0x896c, 0x9664
```

资源 `0xF8`：

```text
jointIds = []
cmd-like ids include 0x8021, 0x8022, 0x8023, 0x8024, 0x8025,
0x8026, 0x85cf, 0x8d1d, 0x8d34, 0x8fb9, 0x9664, 0x9690, 0x9760
```

全量 MENU 资源扫描：

```text
all_menu_joint_hits_count = 0
```

保守结论：

```text
LoadMenuW 菜单资源路径存在，
但 VisualTS.exe 内全量 MENU 资源未直接包含已知接头 command id。
```

## Codejock / Ribbon / icon 路径

`sub_1406F37B0`：

```text
Codejock Ribbon 构造函数。
`0x8CDC` 邻近命令值在该函数有命中。
已知接头 command id 不命中。
```

`sub_1406F4EC0`：

```text
CXTPImageManager::SetIcon / SetIcons 路径。
`0x8CDC` 邻近命令值在该函数有命中。
已知接头 command id 不命中。
```

保守结论：

```text
当前 Codejock / Ribbon / icon 路径没有直接闭合接头 UI 入口。
```

## TODO-053 stop point

本轮最稳妥结论：

```text
1. 接头内部 command id / handler 表已确认。
2. 旧图石动态 context menu 机制已确认。
3. sub_1406BA690 / sub_1405C2EF0 是关键 popup/menu 构造路径。
4. LoadMenuW 的 0xAD / 0xF8 菜单资源路径已确认。
5. 全量 MENU 资源没有已知接头 command id。
6. 已知接头 command id 没有普通 immediate 命中。
7. 旧 UI caption / 右键菜单项 / command dispatch 绑定仍未闭合。
```

这不证明：

```text
旧 UI 没有接头入口。
接头命令无法运行。
接头命令不在动态菜单中。
```

它只证明：

```text
当前静态 immediate / MENU resource / popup constructor 路径不能直接闭合接头 UI 入口。
```

## 用户旧图石运行截图最小清单

后续用户现场插 USB 狗并完成 HASP 后，建议用最小截图闭合 UI 入口：

```text
1. 打开旧图石主界面，切到钢筋页，截图完整 Ribbon。
2. 选中单根钢筋，右键截图菜单。
3. 选中钢筋组，右键截图菜单。
4. 选中构件 / 特征 / 段组对象，右键截图菜单。
5. 找到接头相关入口，截图按钮文字、分组、启用/禁用状态。
6. 点击接头入口，截图弹窗标题和参数字段。
7. 记录对象类型：bar / group / segment / feature / goujian。
8. 导出 Detail 包，记录是否出现非空 `steeljoint-line / joints` 或 `Others / symbolcutIOS`。
```

## 验证

本轮验证结果写入：

```text
docs/phase1/app_build_reports/m2_drawing_022_run_001.md
docs/phase1/app_build_reports/m2_drawing_022_run_001.json
```

本轮是 docs/evidence only：

```text
xhighReview = not_required_docs_only
```

## 下一步

`TODO-053` 完成后，下一步建议执行：

```text
TODO-054 / 接头 handler 业务对象筛选链静态分类 P0
```

原因：

```text
UI 入口静态路径已经追到 stop point。
旧图石运行样例仍依赖用户现场 USB 狗 + HASP。
agent 可继续独立推进的方向，是把 barjoint/groupjoint/segjoint/featjoint/goujianjoint
各 handler 内部的对象筛选、参数入口和业务对象类型再分类。
```

边界：

```text
只读 IDA / 文档。
不实现算法。
不启动旧图石。
不安装 HASP。
不进入 golden。
```
