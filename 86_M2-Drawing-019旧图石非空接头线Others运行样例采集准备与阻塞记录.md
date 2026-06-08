# M2-Drawing-019 旧图石非空接头线 / Others 运行样例采集准备与阻塞记录

## 结论

本轮继续执行 `TODO-050 / 旧图石非空接头线 Others 运行样例采集 P0`。

当前不能把 `TODO-050` 写成 done。

原因很直接：

```text
真实非空样例必须来自旧图石运行输出；
但当前用户已说明 USB 狗尚未插入，HASP 也尚未安装。
agent 不能自动安装驱动、不能拷贝系统目录、不能自动启动旧图石。
```

因此本轮把 `TODO-050` 收敛为：

```text
blocked_by_user_runtime
```

这不是路线失败，而是证据节点的真实边界。

本轮已经完成的是：

1. 复核 `TODO-049 / E-RUN-004` 已关闭启动前置条件。
2. 复核工作区 `HASPUserSetup` 目录和目标 HASP 目录状态。
3. 用 IDA MCP 重新打开 `VisualTS.exe.i64`，补强接头命令触发线索。
4. 形成用户后续手工采样清单。
5. 新增下一轮可继续推进的静态补证任务 `TODO-051`。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，
不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## 控制合同

Primary Setpoint：

```text
确认 TODO-050 是否能由 agent 在当前环境完成；
若不能完成真实旧运行样例采集，则把阻塞条件、IDA 可补证部分、
手工采样模板和下一步可执行任务落成文档。
```

Acceptance：

```text
新增 E-IDA-032。
新增 E-DEV-072。
新增 86 实现记录。
新增 m2_drawing_019_run_001.md / json。
更新 03 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、readiness gate、OCCT / AIS 泄漏扫描通过。
```

Guardrail：

```text
不得把“采样准备完成”写成“非空样例已采到”。
不得自动安装 HASP 驱动或复制系统目录。
不得自动启动旧图石。
不得实现真实接头线 / Others 几何算法。
不得用 OCCT 或字段骨架替代旧图石运行证据。
```

## 当前旧运行阻塞

用户已经确认：

```text
USB 狗当前尚未插入。
HASPUserSetup.exe 尚未安装。
hasplm.ini / haspvlib_23520.dll 尚未复制到目标 HASP 目录。
```

本轮只读复核：

```text
.\HASPUserSetup
  HASPUserSetup.exe
  hasplm.ini
  haspvlib_23520.dll
  驱动安装步骤.doc

C:\Program Files (x86)\Common Files\Aladdin Shared\HASP\
  -> 目标目录当前不存在
```

注意：

```text
用户消息里提到 haspvlib_23250.dll，
工作区实际文件名是 haspvlib_23520.dll。
```

该差异继续保留为现场安装复核项。

## 本轮 IDA MCP 补证

本轮重新打开：

```text
database = visualts_i64_todo050
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
hexrays_ready = true
strings_cache_size = 16320
```

### 接头内部命令清单

IDA 字符串缓存确认接头相关内部命令：

```text
barjointnew
barjointclear
barjointmove
barjointrev
groupjointnew
groupjointclear
goujianjointnew
goujianjointclear
featjointnew
featjointclear
groupjointrev
groupjointmove
```

其中 `barjointnew` 的字符串 xref 落到两组命令表：

```text
0x140959328
0x14095ae70
```

`groupjointrev` 同样落到两组命令表：

```text
0x1409593e8
0x14095af90
```

当前未从这些表项继续追到中文 Ribbon caption。

### barjointnew

`sub_1405DFAA0(barjointnew)` 当前确认：

```text
1. 检查 JointRuler / JointDistbet 全局参数。
2. 从当前选择集取对象。
3. 通过 sub_1405C6820 和 sub_1405E0E70 筛选旧钢筋对象。
4. 进入 api_bb_begin / update_from_bb / api_bb_end ACIS transaction。
5. 调用旧 VisualTS steeljoint / steelbar 业务函数。
```

工程含义：

```text
barjointnew 适合采“单根 / 单段钢筋对象接头”样例。
```

### groupjointnew

`sub_1405F0060(groupjointnew)` 当前确认：

```text
1. 从当前选择集取对象。
2. 通过 sub_1405C6820 和 sub_1405F17C0 筛选旧钢筋组对象。
3. 进入 ACIS transaction。
4. 调用 sub_1405E9640 等旧组接头业务函数。
```

工程含义：

```text
groupjointnew 适合采“钢筋组接头”样例。
```

### goujian / feat 接头入口

`sub_1405EF140` 对应 `goujianjointnew / featjointnew` 系列当前确认：

```text
1. 从当前选择集取对象。
2. 通过 sub_1405C6820 和 sub_14045A7F0 筛选对象。
3. 进入 ACIS transaction。
```

工程含义：

```text
这些入口可能面向构件 / 特征对象接头。
当前不应和 barjointnew / groupjointnew 混成同一个采样动作。
```

### 中文 UI caption

本轮在 IDA 字符串缓存中搜索：

```text
接头
搭接
焊接
相间
定尺
下料
钢筋接头
接头反向
移动接头
清除接头
新建接头
```

结果：

```text
no direct string hit
```

保守结论：

```text
接头相关中文 UI caption 可能来自资源表、Codejock/Ribbon 资源、
外部 UI 数据或非普通字符串缓存。
后续旧界面运行确认仍必须记录真实菜单路径 / 按钮文字 / 状态栏提示。
```

## 用户后续手工采样清单

当旧图石现场条件满足后，按这个最小流程采样：

```text
前置：
  1. 插入 USB 狗。
  2. 安装 HASPUserSetup.exe。
  3. 复核 hasplm.ini 和实际 haspvlib_*.dll 文件名。
  4. 确认旧图石能进入主界面并打开目标 SFL。
```

### 样例 A：barjointnew

```text
1. 打开一个包含可选钢筋对象的 SFL。
2. 选中单根钢筋 / 单段钢筋。
3. 尝试旧 UI 中的“接头 / 搭接 / 焊接 / 新建接头”相关按钮。
4. 如果旧图石支持命令输入或快捷入口，尝试 barjointnew。
5. 记录：
   - 菜单路径或按钮位置
   - 状态栏提示
   - 弹窗标题和字段
   - 参数值，尤其是定尺长度 / 焊接长度 / 接头相间距离
   - 成功或失败截图
```

### 样例 B：groupjointnew

```text
1. 选中钢筋组。
2. 尝试组接头相关入口。
3. 如果有命令入口，尝试 groupjointnew。
4. 记录同样的信息。
```

### 样例 C：goujian / feat 接头

```text
1. 选中构件 / 特征相关对象。
2. 尝试构件接头 / 特征接头入口。
3. 如果有命令入口，尝试 goujianjointnew 或 featjointnew。
4. 记录是否可触发、是否提示对象类型不对。
```

### 工程图输出

每次成功触发后：

```text
1. 保存前后 SFL，记录 hash 和修改时间。
2. 执行生成工程图。
3. 找到输出的 DetailNN.stl。
4. 搜索并记录：
   Others
   symbolcutIOS
   SymbolCutIOS
   steeljoint-line
   joints
   Line
5. 记录目标 XML 节点完整片段、文件 hash、截图。
```

## 最小回填模板

```text
旧图石版本：
USB 狗是否插入：
HASP 是否已安装：
打开的 SFL 文件名：
SFL 文件 hash：

尝试入口：
  - UI 菜单路径：
  - 按钮文字：
  - 内部命令名（如尝试）：

选择对象：
  - 单根钢筋 / 钢筋组 / 构件 / 特征：
  - 对象编号或截图：

弹窗：
  - 标题：
  - 字段：
  - 参数值：

结果：
  - 成功 / 失败：
  - 失败提示完整文本：
  - 截图路径：

输出：
  - 是否生成 Detail 包：
  - DetailNN.stl 文件名：
  - DetailNN.stl hash：
  - 是否出现 steeljoint-line / joints：
  - 是否出现 Others / symbolcutIOS：
  - XML 片段：
```

## 下一步

因为 `TODO-050` 当前需要用户现场运行证据，
下一轮不应继续围着旧图石启动条件空转。

建议新增并执行：

```text
TODO-051 / M2-Drawing-020
  -> Others / symbolcutIOS producer 与接头 UI 触发静态补证 P0
```

目标：

```text
在等待用户现场运行样例期间，
继续用 IDA MCP 查 *(v8 + 840 / 848) 的 producer、
接头命令表注册链、可能的 Ribbon / command id 绑定，
减少后续手工采样试错。
```

## 边界声明

本记录证明：

```text
TODO-050 当前不能由 agent 单独完成。
接头相关内部命令和 handler 类型已进一步确认。
用户手工采样模板已明确。
```

本记录不证明：

```text
非空 steeljoint-line / joints 样例已采到。
非空 Others / symbolcutIOS 样例已采到。
真实接头线算法已实现。
真实 Others 几何算法已实现。
AutoCAD L2 已通过。
旧插件已接受新包。
完整工程图已完成。
golden 已采集。
```
