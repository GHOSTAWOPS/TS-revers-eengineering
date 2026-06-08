# M2-Drawing-031 生成工程图命令入口 / Ribbon 绑定静态补证 P0 实现记录

## 结论

本轮执行 `TODO-062 / 生成工程图命令入口 / Ribbon 绑定静态补证 P0`。

本轮只读 `IDA MCP`、既有 `Detail` 证据和现有文档，没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实工程图算法。

更正说明（后续 `TODO-063` 已落文）：

```text
本记录初版把 36124/35057 与 psallc/psexcel 的对应关系写反了。

当前应以 101_M2-Drawing-032... / E-IDA-043 为准：
  36124 / 0x8D1C -> psallc -> sub_140600AA0
  35057 / 0x88F1 -> psexcel -> sub_140605B20
  Dialog #274 -> 只能保守记为 Set2Dpage 属性页
```

核心结论：

```text
1. sub_1406F37B0 就是旧 VisualTS 的 Codejock Ribbon 构造函数。

2. 在 sub_1406F37B0 里：
   工程图 / 输出 -> {36124, 35057}

3. 其中 36124 已由既有证据闭合到：
   psexcel / 0x8D1C / sub_140605B20
   -> 下料表链

4. sub_140600AA0 已继续确认是：
   psallc / 0x8CD2 / sub_140600AA0
   -> Detail.xml + DetailNN.stl 主写出链

5. 因此当前可以把 35057 收紧为：
   高置信“生成工程图”候选入口

6. 但这轮还没有完全闭合：
   35057 -> dispatch -> psallc / 0x8CD2
   Dialog #274 -> 最终 command binding
```

大白话说：

```text
以前“生成工程图”这件事是两头都知道一点，中间还空着：

上面知道工程图页有这个按钮，
下面知道旧程序内部确实有写 Detail 包的主链，
但不知道这两个是不是同一条。

这轮至少把边界收紧成了：
  顶部按钮组里就两个核心入口：36124 和 35057
  36124 是下料表
  35057 大概率就是生成工程图
  而 psallc / 0x8CD2 / sub_140600AA0 确实是旧 Detail 包主写出链

剩下要追的就是中间那一跳：
  35057 最终怎么 dispatch 到内部导出链
  Dialog #274 到底绑到哪条命令
```

## Control Contract

Primary Setpoint：

```text
在接头链路暂缓后，先把“生成工程图”顶层命令入口和内部 writer 主链
收口到可开发 static stop point。
```

Acceptance：

```text
新增 E-IDA-042。
新增 E-DEV-084。
确认 sub_1406F37B0 的 工程图 / 输出 分组 command ids。
确认 sub_140600AA0 的 Detail 包主写出链。
TODO-062 done，并把下一步切到 TODO-063。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
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
不把 35057 直接写死成已完全闭合真相。
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
```

## 关键静态收口

### 1. `sub_1406F37B0` 是顶层 Ribbon 构造函数

当前高置信静态结论：

```text
工程图页签
  -> 输出分组
     -> 36124
     -> 35057
```

其中：

```text
36124
  -> 同时被加入 QuickAccess
35057
  -> 当前未在同一处 QuickAccess immediate 列表命中
```

### 2. `36124` 继续保持为下料表链

本轮没有推翻既有事实：

```text
36124 / 0x8D1C
  -> psexcel
  -> sub_140605B20
  -> sub_140602F90
```

所以当前顶层 `工程图 / 输出` 分组里，至少一个按钮已经确定是 `下料表`。

### 3. `sub_140600AA0` 是旧 Detail 包主写出链

本轮复核到的关键调用：

```text
sub_140609690   -> StbTable / MaterialTable
sub_140609A50   -> 一类视图生成
sub_14060B010   -> 一类视图生成
sub_14060A230   -> 一类视图生成
sub_14060DC00   -> 一类视图生成
sub_14061F970   -> PartDetailDrawing 复杂线容器
sub_14061F830   -> StbGroups / StbGroup / Std / StbGeo
sub_140635A80   -> 保存 DetailNN.stl
```

并且函数内部会创建：

```text
StbTables
HViewPorts
ViewPort
PartDetailDrawing
StbDetailDrawing
```

这足够把它收口成：

```text
旧生成工程图内部 writer 主链
```

### 4. 本轮形成的 static stop point

当前可以保守写成：

```text
顶层 Ribbon：
  工程图 / 输出 -> {36124, 35057}

内部 writer：
  psallc / 0x8CD2 / sub_140600AA0

中间待补：
  35057 -> dispatch -> psallc / 0x8CD2
  Dialog #274 -> 最终 binding
```

## 本轮不证明

```text
35057 已经 100% 等于生成工程图最终命令
35057 -> dispatch -> psallc / 0x8CD2 已完全闭合
Dialog #274 最终一定由 35057 打开
旧图石运行时输出目录、覆盖行为和文件列表
AutoCAD L2 通过
golden
```

## 下一步

```text
TODO-063 / 生成工程图 35057 到内部导出 dispatch / Dialog #274 静态补证 P1
```

下一步只继续做两件事：

```text
1. 追 35057 的 dispatch
2. 追 Dialog #274 的最终命令绑定
```
