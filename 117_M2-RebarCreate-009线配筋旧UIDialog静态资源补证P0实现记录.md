# M2-RebarCreate-009 线配筋旧 UI / Dialog 静态资源补证 P0 实现记录

todoId = TODO-079
phase = M2-RebarCreate-009
evidenceIds = E-IDA-047, E-DEV-101

## 目标

本轮只完成 `TODO-079 / 线配筋旧 UI / Dialog 静态资源补证 P0`。

背景是：`TODO-078` 已确认当前运行采样目录只有模板，没有真实旧图石截图、hash、listing 或填写后的 notes。因此本轮不启动旧图石、不安装 HASP，只用 IDA MCP 和静态资源继续收窄旧线配筋 UI / Dialog / 状态栏证据。

## 本轮查证

IDA MCP 会话：

```text
database = visualts_todo079
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
strings_cache_size = 16320
```

本轮复核对象：

```text
sgroupbarline / sgroupbararc 字符串
sub_1404DE720     // sgroupbarline handler
sub_14054B410     // secondary / availability helper
sub_1404D10C0     // line / arc group common creation core
sub_14058B8D0     // Input_float modal wrapper
sub_1404F5120     // Input_float constructor
sub_1404F5380     // Input_float init / title / label binding
CStatusBar::SetPaneText imports and xrefs
```

## 结论

新增 `E-IDA-047`：

```text
sgroupbarline 表项仍绑定到 sub_1404DE720 + sub_14054B410。

中文字符串静态扫查没有直接命中：
  线配筋 / 请选择 / 选择边 / 状态栏 / 失败 / 参数 / 钢筋组

sub_1404DE720 和 sub_14054B410 内仍没有直接中文提示、AfxMessageBox 或 SetPaneText 调用。

sub_1404D10C0 的公共创建 gate 继续收窄为：
  objA / objB / entityList / distance
  distance >= 0.002
  sub_1405F25F0(objA) >= 3
  ENTITY_LIST count >= 1

Dialog #383 是公共 Input_float 浮点输入窗：
  CDialog::CDialog(..., 0x17F, ...)
  source = e:\tushi3d\dam\tool\input_float.cpp
  object+304 = double value
  object+320 = title
  object+328 = label

CStatusBar::SetPaneText 的集中调用层存在，
但本轮没有找到直接绑定到 sgroupbarline 的旧状态栏文案。
```

新增 `E-DEV-101`：

```text
TODO-079 已把上述 IDA / 静态资源结论回写到证据文档、追溯矩阵、缺口文档、run report、readiness gate 映射和 todo.csv。
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
修改系统目录
修改 RebarGroupCreator
修改 LineGroupParameterDialog 文案
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
用 OCCT 直接重写钢筋业务
```

## 仍未闭合

```text
旧线配筋真实运行提示
旧状态栏 pane 文案
旧主参数窗口字段、默认值、单位、灰显状态
Dialog #383 在真实线配筋流程中的标题、标签、默认值和 OK/Cancel 行为
旧选择对象业务名
完整 ENTITY_LIST 输入对象语义
完整 sub_1405D5670 等价和 golden
```

这些继续保留在：

```text
GAP-UI-REB-001
GAP-IDA-002
GAP-IDA-007
GAP-REB-C-002
```

## 下一步

下一轮建议执行：

```text
TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0
```

建议原因：

```text
TODO-079 已把旧 UI 静态证据收窄到 stop point。
在没有真实旧图石运行工件前，下一步不宜继续猜 UI。
更稳妥的是把 sub_1404D10C0 已确认的公共创建 gate 和 diagnostic 语义落到 P0 代码/测试，
继续保证新系统靠近旧 VisualTS 行为，而不是用 OCCT 自己发挥。
```
