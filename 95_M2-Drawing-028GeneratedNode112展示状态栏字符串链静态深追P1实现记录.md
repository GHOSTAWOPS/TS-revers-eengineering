# M2-Drawing-028 generated node+112 展示/状态栏字符串链静态深追 P1 实现记录

## 结论

本轮执行 `TODO-059 / generated node+112 展示/状态栏字符串链静态深追 P1`。

本轮只读 `IDA MCP`、二进制字符串和既有文档，没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
1. sub_1405DC6C0(a1)
   = sum(generated node+112) over a1+88 generated chain

2. sub_140601D80 是旧 Excel 下料表写出链：
   E2 = 焊头(个)      = sub_1405DC6C0(obj)
   F2 = 单下料长(mm) = sub_1405DBC20(obj, buffer)

3. sub_1405DBC20 不是直接打印 generated node+112，
   而是用 sum(generated node+112) 决定是否进入
   单下料长(mm) 的分段拼接表达式路径。

4. sub_1406F72A0 是状态栏展示链：
   当 sum(generated node+112) > 0 时，
   pane3 会显示 焊接 / 绑扎 / 套筒连接。

5. sub_1405EAEC0 / sub_1404554B0
   会继续把同一计数向 group / seg / owner 聚合。

6. 因此 generated node+112 当前可收紧到：
   “接头 / 焊头计数展示链字段”。
   但最终旧调试符号名和连接方式来源字段命名仍不写死。
```

大白话说：

```text
现在已经不能只说它是“生成链上的一个 +112”。

更接近事实的是：
它会被汇总成接头 / 焊头数量，
旧系统拿这个数去：
  1. 写 Excel 的 焊头(个)
  2. 决定 单下料长(mm) 要不要拆成 5000+5000+3200 这种串
  3. 在状态栏显示 焊接 / 绑扎 / 套筒连接
```

## Control Contract

Primary Setpoint：

```text
在 TODO-058 已把 generated node+112 收紧到生成链字段边界后，
继续沿 sub_1405DC6C0 callers 深追它的展示 / 状态栏 / 字符串链语义。
```

Acceptance：

```text
新增 E-IDA-041。
新增 E-DEV-081。
覆盖 sub_1405DBC20 / sub_140601D80 / sub_1406F72A0 /
sub_1405EAEC0 / sub_1404554B0。
把 generated node+112 收紧到比“int mm 贡献字段”更接近旧 UI 的展示口径。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
TODO-059 done，并把下一步切到 TODO-060。
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
不把静态字符串直接写成已运行确认的事实。
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
sub_1405DBC20
sub_1406F72A0
sub_1405EAEC0
sub_1405DFEF0
sub_1405E02D0
sub_140601D80
sub_1405DBE50
sub_1405DC6A0
sub_1405EAF40
sub_1405E0660
sub_1405DC6C0
sub_1405DC870
sub_1404554B0
sub_1406B4670
```

## 关键静态收口

### 1. `sub_1405DC6C0` 的真实汇总语义

当前高置信伪代码：

```text
result = 0
for each generatedNode in a1+88 chain:
  result += generatedNode+112
return result
```

所以：

```text
generated node+112
  -> 位于 child+88 生成链节点上
  -> 被整条生成链累加
  -> 不是 child+112 的同一字段
```

### 2. `sub_1405DBC20` 是单下料长分解表达式

当前高置信伪代码：

```text
baseMm = round(sub_1405DC870(a1, 0, 0) * 1000.0)

if sub_1405DC6C0(a1) < 1:
  format "%d"
else:
  for each generatedNode in a1+88 chain:
    for each pieceNode in generatedNode+128 chain:
      pieceMm = *(int *)(pieceNode + 76)
      append "%d+" / "%s%d+" / "%s%d"
```

工程含义：

```text
sub_1405DBC20
  -> 用 sum(generated node+112) 决定是否进入分段拼接模式
  -> 输出更接近“单下料长(mm)”分解表达式
```

### 3. `sub_140601D80` 是旧 Excel 下料表写出链

本轮恢复出的表头：

```text
钢筋下料表
编号
直径(mm)
单净长(mm)
焊长(mm)
焊头(个)
单下料长(mm)
根数(根)
总根数(根)
```

关键列落点：

```text
E 列 = sub_1405DC6C0(obj)
F 列 = sub_1405DBC20(obj, buffer)
```

所以当前可以高置信写成：

```text
sum(generated node+112)
  -> 在旧 Excel 下料表中直接落到 焊头(个)
```

### 4. `sub_1406F72A0` 是状态栏展示链

本轮恢复出的关键字符串：

```text
编号: %d号钢筋
编号: %d%c号钢筋
编号: %d号钢筋 首尾封闭
%s级钢  直径:%s 毫米
型号%d-%g
至中心线距离:  %d毫米
焊接
绑扎
套筒连接
钢筋长度:%g 米
钢筋长度:%g~%g 米
```

最关键结论：

```text
当 sub_1405DC6C0(a2) > 0 时，
状态栏 pane3 会改成：
  焊接 / 绑扎 / 套筒连接
```

连接方式分支来自：

```text
*(sub_1405E0660(a2) + 64)
```

但这个来源字段的最终命名，本轮仍保留为 gap。

### 5. 向上聚合 helper

```text
sub_1405EAEC0(a1)
  -> sum sub_1405DC6C0(child) over a1+80 chain

sub_1404554B0(a1)
  -> sum sub_1405EAEC0(v1) over a1+192 chain
```

这说明旧系统会继续向 group / seg / owner 聚合这个计数。

## 本轮最终口径

当前最稳妥说法：

```text
generated node+112
  = 接头 / 焊头计数展示链字段

它的汇总值：
  -> Excel 下料表里写成 焊头(个)
  -> 控制 单下料长(mm) 是否进入分段拼接表达式
  -> 在状态栏里显示 焊接 / 绑扎 / 套筒连接
```

当前仍不写死：

```text
generated node+112 的最终旧调试符号名
连接方式来源字段 *(sub_1405E0660(a2)+64) 的最终命名和完整 enum
旧图石真实运行截图 / 导出样例
```

## 本轮 stop point

已闭合：

```text
sub_1405DC6C0 = sum(generated node+112)
Excel 下料表 E 列 = 焊头(个)
Excel 下料表 F 列 = 单下料长(mm) 分解表达式
状态栏 pane3 在 sum > 0 时显示 焊接 / 绑扎 / 套筒连接
group / seg / owner 聚合 helper
```

仍未闭合：

```text
generated node+112 的最终业务调试名
连接方式来源字段的最终命名
旧图石真实运行截图和导出样例
AutoCAD L2
真实接头线算法
真实 Others 几何算法
golden
```

## 最小运行确认清单

如果后续用户现场可以运行旧图石，最小补证清单收敛为：

```text
1. 选中一个确定带接头的对象，
   截状态栏，确认是否出现 焊接 / 绑扎 / 套筒连接。

2. 导出一次下料表，
   保留 Excel 文件或至少截出 E 列 焊头(个) 和 F 列 单下料长(mm)。

3. 记录对象编号、操作步骤、导出目录和文件 hash。

4. 如果现场条件仍不满足，
   只记录 blocked 原因，不把运行确认写成 done。
```

## 下一步

```text
TODO-060 / generated node+112 旧图石运行确认与 Excel/状态栏对照 P0
```
