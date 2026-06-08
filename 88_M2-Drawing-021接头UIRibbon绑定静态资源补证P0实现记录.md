# M2-Drawing-021 接头 UI caption / Ribbon 绑定静态资源补证 P0

## 结论

本轮执行 `TODO-052 / 接头 UI caption / Ribbon 绑定静态资源补证 P0`。

本轮已经用 IDA MCP 和静态资源复核，把接头 UI 触发路径推进到这个状态：

```text
内部命令 / handler 表：已确认
中文 UI caption：未从普通字符串直接命中
Ribbon 构造函数 sub_1406F37B0：未看到接头命令 ID 作为直接 top-level Ribbon 控件加入
资源对话框：能确认接头参数字段存在，但不能证明顶部按钮 caption / Ribbon 绑定
旧图石运行样例：未采集
```

大白话说：

```text
旧程序里面确实有“新建接头 / 清除接头 / 移动接头 / 接头反向”等内部命令和 handler。
但静态看不到它们直接挂在顶部 Ribbon 的哪个中文按钮上。
它们更可能是右键 / 对象上下文 / 动态菜单 / 其他 Codejock 资源路径触发。
```

本轮不启动旧图石，不安装 HASP，不修改系统目录，不改 app 业务代码，不实现任何接头线 / Others 算法。

## 控制合同

Primary Setpoint：

```text
在 TODO-051 已闭合 symbolcutIOS producer 静态链后，
继续用 IDA MCP / 静态资源缩小接头 UI caption / Ribbon 绑定缺口。
```

Acceptance：

```text
新增 E-IDA-034。
新增 E-DEV-074。
复核接头 command id 表、handler 表、Ribbon 构造函数、普通字符串和静态资源。
若静态不能闭合，形成旧图石运行截图最小清单。
更新 03 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
```

Guardrail：

```text
不得把英文内部命令名写成中文 UI caption。
不得把 dialog 参数字段写成 Ribbon 按钮绑定。
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

复核函数 / 路径：

```text
sub_1406F37B0
  -> Codejock Ribbon 构造函数
  -> CXTPRibbonBar / CXTPRibbonTab / CXTPRibbonGroup / CXTPControls::Add
```

## 接头内部命令 ID 表

本轮复核到接头内部命令 ID：

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

邻近上下文：

```text
0x8919 / 35097 -> pgetmass
0x891C / 35100 -> bdiv
0x891F / 35103 -> gcom
0x8920 / 35104 -> spload
0x8923 / 35107 -> spcombine
```

## 接头 handler 表

本轮复核到 handler 绑定：

```text
barjointnew       -> sub_1405DFAA0, aux 0
barjointclear     -> sub_1405DF710, aux 0
barjointmove      -> sub_1405DFEF0, aux 0
barjointrev       -> sub_1405E02D0, aux 0

groupjointnew     -> sub_1405F0060, aux sub_14054AF20
groupjointclear   -> sub_1405EFCC0, aux sub_14054AF20
groupjointrev     -> sub_1405F0850, aux sub_14054AF20
groupjointmove    -> sub_1405F0430, aux sub_14054AF20

featjointnew      -> sub_1405EF140, aux sub_14054AE20
featjointclear    -> sub_1405EEDA0, aux sub_14054AE20

segjointnew       -> sub_1405D94C0, aux sub_14054B410
segjointclear     -> sub_1405D9450, aux sub_14054B410

goujianjointnew   -> sub_1405EF8D0, aux sub_14045A020
goujianjointclear -> sub_1405EF510, aux sub_14045A020
```

重要 xref：

```text
0x140768480 "barjointnew" -> 0x140959328, 0x14095ae70
0x1407684c0 "groupjointnew" -> 0x140959368, 0x14095aed0
0x140768508 "segjointnew" -> 0x1409593a8, 0x14095af30
0x140768528 "featjointnew" -> 0x1409593c8, 0x14095af00
0x140768548 "groupjointrev" -> 0x1409593e8, 0x14095af90

sub_1405DFAA0 -> 0x14095ae78, 0x140a0af5c, 0x140a0af68
sub_1405F0060 -> 0x14095aed8, 0x140a0b5e0
sub_1405D94C0 -> 0x14095af38, 0x140a0ab90
sub_1405EF140 -> 0x14095af08, 0x140a0b5b0
sub_1405EF8D0 -> 0x14095af68, 0x140a0b5c8
```

保守解释：

```text
接头命令和 handler 确实存在于内部命令 / handler 表。
这些证据只能证明内部命令入口，不等于旧界面中文按钮 caption 或顶部 Ribbon 绑定。
```

## Ribbon 构造函数复核

`sub_1406F37B0` 已确认构造 Codejock Ribbon：

```text
CXTPRibbonBar::AddTab
CXTPRibbonTab::AddGroup
CXTPRibbonGroup::Add
CXTPControls::Add
```

已知对照：

```text
36124 / 0x8D1C 在 sub_1406F37B0 中出现
35100 / bdiv 在 sub_1406F37B0 中出现
35103 / gcom 在 sub_1406F37B0 中出现
```

本轮对接头命令 ID 做 immediate 搜索：

```text
36046, 36047, 36048, 36049,
36061, 36062, 36063, 36064,
36057, 36058,
36241, 36242,
36055, 36056

=> no immediate matches
```

保守结论：

```text
在已检查的 Ribbon 构造函数 sub_1406F37B0 中，
未看到接头命令 ID 作为直接 Ribbon 控件加入。

这不证明旧 UI 没有接头入口；只能说明这些接头命令不像 0x8D1C / 35100 / 35103 那样，
以普通 immediate 形式直接出现在该 Ribbon 构造函数里。
```

可能路径：

```text
右键 / 对象上下文菜单。
运行期动态菜单。
Codejock 外部资源或命令栏持久化数据。
其他未覆盖构造函数。
```

## 普通字符串和静态资源复核

IDA 字符串搜索未直接命中：

```text
接头
搭接
焊接
新建接头
移动接头
清除接头
钢筋接头
```

直接扫描 `VisualTS.exe` 字节时，普通资源能看到接头相关参数字段，尤其：

```text
Dialog #427 title = 下料
  定尺长度(米)
  焊接长度(厘米)
  接头相间距离(米)

Dialog #428 title = 创建段组接头
  确定
  取消
  居中
  首端始
  尾端始
  起始位置
  起点距离
  定尺长度
```

另外 `Dialog #268 / #292` 有 `接头类型` 字段，但属于钢筋信息 / 样式类 dialog，不是顶部 UI 触发按钮。

资源证据的边界：

```text
这些 dialog / 字段能证明旧程序有接头参数界面和接头相关配置项。
但它们不能证明顶部 Ribbon caption，也不能证明哪个 UI 按钮绑定到 barjointnew / groupjointnew。
```

## 用户旧图石运行截图最小清单

如果后续用户现场已经插 USB 狗并完成 HASP，本缺口建议用最小截图闭合：

```text
1. 打开旧图石主界面，切到钢筋相关页签，截图完整 Ribbon。
2. 右键选中一根钢筋 / 一组钢筋，截图上下文菜单。
3. 在菜单或 Ribbon 中找到接头相关入口，截图按钮文字、分组、启用/禁用状态。
4. 点击“新建接头 / 创建段组接头 / 清除接头 / 移动接头 / 接头反向”等入口，截图弹窗标题和参数字段。
5. 记录操作对象类型：单根钢筋、钢筋组、构件、特征、段组。
6. 如能导出 Detail 包，保留 Detail.xml + DetailNN.stl，并记录文件 hash。
```

其中最关键的是：

```text
菜单路径 / 按钮文字 / 对象选择类型 / 弹窗标题 / 导出文件。
```

## 本轮已闭合 / 未闭合

已闭合：

```text
接头内部命令 ID 表更完整。
接头 command -> handler 表更完整。
sub_1406F37B0 是 Ribbon 构造函数。
已知 Ribbon ID 可在 sub_1406F37B0 中找到，对照方法有效。
接头命令 ID 未作为普通 immediate 出现在 sub_1406F37B0。
静态资源确认接头参数 dialog 字段存在。
```

未闭合：

```text
旧 UI 中文 caption。
顶部 Ribbon 是否存在接头按钮。
右键 / 对象上下文菜单是否触发这些命令。
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

本轮计划验证：

```text
git diff --check
todo.csv / json parse
readiness gate strict
domain boundary scan:
  app/src/domain/rebar
  app/src/drawing
  app/src/project
CTest
```

最终结果见：

```text
docs/phase1/app_build_reports/m2_drawing_021_run_001.md
docs/phase1/app_build_reports/m2_drawing_021_run_001.json
```

## 边界声明

本记录证明：

```text
TODO-052 已完成静态资源 / Ribbon 绑定补证尝试。
IDA MCP 已使用。
接头内部命令和 handler 表已进一步完整。
静态证据显示接头命令 ID 未直接挂在 sub_1406F37B0 的 Ribbon 构造路径中。
```

本记录不证明：

```text
旧图石运行样例已采到。
旧 UI caption 已闭合。
旧 UI 没有接头入口。
AutoCAD L2 已通过。
真实接头线算法已实现。
真实 Others 几何算法已实现。
完整工程图已完成。
golden 已采集。
```

## 下一步

建议下一轮进入：

```text
TODO-053 / M2-Drawing-022
  -> 接头 context menu / command dispatch 静态深追 P0
```

目标：

```text
不启动旧图石、不实现算法，继续查 handler 表之外的 command dispatch、popup/context menu、ON_COMMAND/message map 或 Codejock command bar 加载路径。
如果仍不能闭合，则保持旧图石运行截图清单为必要证据。
```
