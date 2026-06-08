# M2-Drawing-032 生成工程图与下料表命令映射纠偏及 Dialog #274 降级静态补证 P1 实现记录

## 结论

本轮执行 `TODO-063 / 生成工程图与下料表命令映射纠偏及 Dialog #274 降级静态补证 P1`。

本轮只读 `IDA MCP`、既有 `Detail` 证据和现有文档，没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实工程图算法。

核心纠偏：

```text
1. 之前把两个命令认反了。

2. 当前应纠偏为：
   36124 / 0x8D1C -> psallc -> sub_140600AA0
   35057 / 0x88F1 -> psexcel -> sub_140605B20

3. 0x8CD2 实际对应 ysteelout，
   不是 psallc。

4. Dialog #274 不能再直接写成“生成工程图设置窗口”。
   当前只能保守记为：
   Set2Dpage 属性页
```

大白话说：

```text
以前我们知道工程图 / 输出里有两个按钮，
也知道旧程序里有一条写 Detail 包的主链，
还有一条写 Excel / 下料表的链。

问题是中间把谁是谁记反了。

这轮纠偏后，结构更像：
  36124 -> 生成工程图 / Detail 包
  35057 -> 下料表 / Excel

同时 274 也不是“出图设置真相”，
只是一个叫 Set2Dpage 的属性页证据。
```

## Control Contract

Primary Setpoint：

```text
把 TODO-062 里认反的命令映射纠正回来，
并把 Dialog #274 的表述从“生成工程图设置窗口”
降级到可证实的 Set2Dpage 属性页。
```

Acceptance：

```text
新增 E-IDA-043。
新增 E-DEV-085。
明确写出 36124 / 0x8D1C -> psallc -> sub_140600AA0。
明确写出 35057 / 0x88F1 -> psexcel -> sub_140605B20。
明确写出 0x8CD2 -> ysteelout。
明确写出 Dialog #274 -> Set2Dpage。
TODO-063 done，并把下一步切到 TODO-064。
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
不把 36124 / 35057 的中文 caption 在无证据时写死。
```

## IDA MCP 会话

```text
database = visualts_todo062_generate_drawing
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
```

## 本轮覆盖函数

```text
sub_1406F37B0
sub_140600AA0
sub_140605B20
sub_1406AD840
```

## 关键静态收口

### 1. 命令表真实方向要按 `[commandId, string]` 读取

这轮纠偏后，当前高置信映射为：

```text
35057 / 0x88F1 -> psexcel
36124 / 0x8D1C -> psallc
36050 / 0x8CD2 -> ysteelout
36059 / 0x8CDB -> yscreenout
```

### 2. 下料表链是 `psexcel -> sub_140605B20`

```text
35057 / 0x88F1
  -> psexcel
  -> sub_140605B20
  -> sub_140602F90
```

这条链继续对齐：

```text
StbTable / MaterialTable
Excel / 下料表
```

### 3. 生成工程图链是 `psallc -> sub_140600AA0`

```text
36124 / 0x8D1C
  -> psallc
  -> sub_140600AA0
  -> Detail.xml + DetailNN.stl
```

`sub_140600AA0` 内部仍会创建：

```text
StbTables
HViewPorts
ViewPort
PartDetailDrawing
StbDetailDrawing
```

### 4. `Dialog #274` 当前只能降级为 `Set2Dpage`

```text
sub_1406AD840
  -> CPropertyPage(..., 0x112, ...)
  -> vftable = Set2Dpage
```

同时它所在的属性页集合里还有：

```text
Set3Dpage
proofwater
OptionDisplay
OptionFactory
CsetYJK
```

所以当前不能再写成：

```text
Dialog #274 = 生成工程图设置窗口
```

## 本轮不证明

```text
36124 的中文 caption 已完全闭合
35057 的中文 caption 已完全闭合
真正生成工程图弹窗已经找到
旧图石运行时输出目录、覆盖行为和文件列表
AutoCAD L2 通过
golden
```

## 下一步

```text
TODO-064 / 生成工程图与下料表中文caption资源绑定及真正生成工程图弹窗候选补证 P1
```

下一步只继续做两件事：

```text
1. 追 36124 / 35057 的中文 caption / resource
2. 追真正生成工程图弹窗候选
```
