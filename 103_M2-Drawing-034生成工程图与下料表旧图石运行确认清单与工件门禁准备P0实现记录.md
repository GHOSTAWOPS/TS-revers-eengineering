# M2-Drawing-034 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0 实现记录

## 结论

本轮执行 `TODO-065 / 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0`。

本轮不新增 `IDA` 结论，不启动旧图石，不安装 `HASP`，不修改 `app` 业务代码，也没有实现真实工程图算法。

本轮做的事情很直接：

```text
1. 把 TODO-064 已确认的静态边界，
   落成可执行的 runtime_capture 模板目录。

2. 把“生成工程图 / 下料表”需要回填的真实工件
   收紧到固定字段、固定目录和固定拒收规则。

3. 明确：
   output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
   只算可选旁证，
   不能单独证明 生成工程图 成功。
```

大白话说：

```text
上一轮我们已经知道：
  哪个按钮是 生成工程图，
  哪个按钮是 下料表，
  以及当前唯一能直接证明的 Dialog
  更像生成后的未割钢筋报告。

这轮不再继续猜，
而是把后面真跑旧图石时
“要留什么证据、什么算有效、什么必须拒收”
全写清楚。
```

## Control Contract

Primary Setpoint：

```text
在 TODO-064 已静态闭合 36124 / 35057 顶部 caption，
并把 output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
收窄成后置报告链的基础上，
把生成工程图 / 下料表的旧图石运行确认清单、
输出目录 / 文件 / hash / 覆盖策略模板、
以及拒收伪工件规则落到文档和 runtime_capture 目录。
```

Acceptance：

```text
新增 E-DEV-087。
新增 runtime_capture 模板目录：
  docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/
新增 README.md 和 capture_notes.md。
明确生成工程图需要记录：
  输出目录 listing / Detail.xml / DetailNN.stl / SHA256 /
  LastWriteTime / rerun 覆盖行为 / 可选 UnCutSteel.TXT。
明确下料表需要记录：
  输出路径 / 文件名 / SHA256 / 结果截图 /
  额外运行时弹窗截图。
明确至少 5 条拒收规则。
TODO-065 done，并把下一步切到 TODO-066。
默认 CTest、readiness gate、git diff --check 通过。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不修改系统目录。
不改 app 业务代码。
不实现真实工程图算法。
不声明 AutoCAD L2 通过。
不进入 golden。
不把 UnCutSteel.TXT 单独当成生成工程图成功证据。
不把 Dialog #427 再写回下料表弹窗真值。
不把无关截图 / 空白 Excel / 旧阻塞提示图收成有效工件。
```

## 本轮新增工件

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/
  README.md
  capture_notes.md
```

## 本轮收口内容

### 1. 生成工程图真实工件要求已固定

模板要求后续至少记录：

```text
输出目录
目录 listing
Detail.xml
DetailNN.stl
每个文件的 SHA256
每个文件的 LastWriteTime
rerun 覆盖行为
若存在 UnCutSteel.TXT，则额外记录
```

如果真实输出目录不方便直接拷进仓库，
当前也至少要留下：

```text
目录 listing 文本
hash 文本
目录截图
capture_notes.md
```

### 2. 下料表真实工件要求已固定

模板要求后续至少记录：

```text
是否生成 Excel
是否只改 Detail 包
是否二者同时生成
输出路径
文件名
SHA256
是否弹额外运行时窗口
若有弹窗，记录窗口标题和截图
```

### 3. 拒收规则已固定

本轮明确拒收：

```text
1. 只有 UnCutSteel.TXT，没有目录 listing / Detail.xml / DetailNN.stl。
2. 只有菜单截图，没有输出目录 / 文件 / hash。
3. 空白 Excel 工作簿。
4. 无关提示窗口截图。
5. 只有 hash，没有文件名 / 路径 / 操作步骤。
6. 未记录 rerun 前后差异，却直接声明覆盖行为已确认。
```

## 本轮不证明

```text
旧图石真实输出目录已经拿到
Detail.xml + DetailNN.stl 覆盖策略已经实跑确认
下料表真实输出路径和文件结果已经拿到
额外运行时弹窗已经实跑确认
AutoCAD L2 通过
golden
```

## 下一步

```text
TODO-066 / 生成工程图与下料表旧图石真实运行工件回填 P0
```

下一步只继续做这些事：

```text
1. 接收真实生成工程图输出目录 / 文件 / hash / rerun 覆盖工件
2. 接收真实下料表输出结果或额外运行时弹窗截图
3. 用 capture_notes 把样本、步骤、路径、文件名、时间和 hash 对齐
```
