# M2-Drawing-033 生成工程图与下料表 Caption 资源闭合及弹窗候选收口 P1 实现记录

## 结论

本轮执行 `TODO-064 / 生成工程图与下料表中文caption资源绑定及真正生成工程图弹窗候选补证 P1`。

本轮继续只读 `IDA MCP`、Win32 资源和既有文档，没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实工程图算法。

核心收口：

```text
1. 36124 的顶部 caption 已可静态闭合为：
   生成工程图

2. 35057 的顶部 caption 已可静态闭合为：
   下料表

3. 旧资源字符串采用：
   描述/help \n caption

4. sub_140600AA0 里唯一可直接证明的 DoModal
   不是前置“生成工程图设置窗”，
   而是：
   output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT

5. Dialog #427 继续降级为：
   OptionFactory 属性页
   不能再当作“下料表弹窗真值”
```

大白话说：

```text
前两轮已经把“谁是工程图、谁是下料表”这两个内部命令链纠正回来了。

这轮把按钮中文名也静态闭合了：
  36124 = 生成工程图
  35057 = 下料表

同时又进一步排除了一个常见误判：
  旧程序里确实会弹一个 Dialog，
  但当前能直接证明的这个 Dialog
  更像生成后的未割钢筋报告，
  不是前置出图设置窗口。
```

## Control Contract

Primary Setpoint：

```text
在 TODO-062 / TODO-063 已把顶部入口和内部命令映射纠偏的基础上，
继续把 36124 / 35057 的中文 caption / resource 最终归属闭合，
并把真正生成工程图弹窗候选进一步收窄。
```

Acceptance：

```text
新增 E-IDA-044。
新增 E-DEV-086。
明确写出 36124 = 生成工程图。
明确写出 35057 = 下料表。
明确写出旧资源采用 描述/help \n caption 模式。
明确写出 sub_140600AA0 里唯一可直接证明的 DoModal
  -> output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT。
明确写出 Dialog #427 -> OptionFactory 属性页。
TODO-064 done，并把下一步切到 TODO-065。
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
不把 output_uncut_steel / Dialog 0x57C 写成前置生成工程图设置窗。
不把 Dialog #427 再写成下料表弹窗真值。
```

## IDA MCP 会话

```text
database = visualts_todo062_generate_drawing
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
hexrays_ready = true
```

## 本轮覆盖对象

```text
sub_140600AA0
sub_140601600
sub_1404FE300
sub_1406AC240
VisualTS.exe string resource ids:
  36124
  35057
  36050
```

## 关键静态收口

### 1. 顶部中文 caption 已静态闭合

本轮用 Win32 `LoadStringW` 资源探针直接读到：

```text
36124 -> 用定义的剖面剖切实体模型\n生成工程图
35057 -> 生成下料表Excel文件\n下料表
36050 -> 输出钢筋几何\n输出钢筋
```

当前高置信工程口径：

```text
旧资源采用：
  描述/help \n caption

所以顶部按钮 caption 可静态闭合为：
  36124 -> 生成工程图
  35057 -> 下料表
```

### 2. 真正生成工程图弹窗候选继续收窄

当前高置信链：

```text
36124 / 0x8D1C
  -> psallc
  -> sub_140600AA0
  -> sub_140601600(v64, &v83)
  -> if (v83) {
       sub_1404FE300(v81, 0)
       CDialog::DoModal(v81)
     }
```

继续收窄：

```text
sub_140601600
  -> GetTempPathA(...)
  -> 拼出 UnCutSteel.TXT
  -> 写未割钢筋 / 未切净类文本
  -> 命中时把 *a2 = 1

sub_1404FE300
  -> CDialog::CDialog(..., 0x57C, ...)
  -> vftable = output_uncut_steel
```

因此当前最稳妥结论是：

```text
sub_140600AA0 里当前能直接证明的 DoModal，
不是前置“生成工程图设置窗”，
而是 output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
这条生成后报告链。
```

### 3. Dialog #427 继续降级

本轮复核：

```text
sub_1406AC240
  -> CPropertyPage(..., 0x1AB, ...)
  -> 0x1AB = 427
  -> vftable = OptionFactory
```

同时它属于：

```text
COptionSheet
  -> Add(Set2Dpage / Set3Dpage / proofwater / OptionDisplay /
         OptionFactory / CsetYJK ...)
```

所以当前只能保守记为：

```text
Dialog #427 = OptionFactory 属性页
```

不能再写成：

```text
Dialog #427 = 下料表弹窗
```

## TODO-064 收口后的静态 stop point

```text
caption / resource：
  36124 -> 生成工程图
  35057 -> 下料表

真正生成工程图弹窗候选：
  当前只收窄到
    output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
  这条后置报告链

不能再写成：
  Dialog #274 = 生成工程图设置窗
  Dialog #427 = 下料表弹窗
```

## 本轮不证明

```text
旧图石点击 生成工程图 时的真实输出目录
Detail.xml + DetailNN.stl 的真实覆盖策略
旧图石点击 下料表 时是否还会出现额外运行时弹窗
旧图石真实运行结果
AutoCAD L2 通过
golden
```

## 下一步

```text
TODO-065 / 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0
```

下一步只继续做这些事：

```text
1. 整理生成工程图真实输出目录 / 文件列表 / hash / 覆盖策略模板
2. 整理下料表真实输出结果和额外运行时弹窗采样门禁
3. 明确哪些截图 / Excel / 输出文件属于有效工件，哪些必须拒收
```
