# M2-Drawing-029 generated node+112 旧图石运行确认与 Excel/状态栏对照 P0 实现记录

## 结论

本轮执行 `TODO-060 / generated node+112 旧图石运行确认与 Excel/状态栏对照 P0`。

这轮没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实接头线 / `Others` 几何算法。

本轮完成的是：

```text
1. 排查当前工作区内可能被误当成运行证据的候选工件。
2. 逐个确认这些工件为什么不能用于 TODO-060 闭环。
3. 固化最小人工采样清单、输出路径和 hash 要求。
4. 把 docs-only 的 blocked facts 收成 E-DEV-082。
```

核心结论：

```text
当前并没有拿到真实旧图石 pane3 截图或真实下料表 Excel 导出。

现有候选工件：
  1.xls
  screenshot.png
  visualts_prompt_capture.png

全部都不能当作 TODO-060 的运行证据。
```

所以本轮关闭的是：

```text
候选工件排查 + 无效证据排除 + 最小人工清单
```

不是：

```text
真实旧图石运行样例已采到
```

真实运行截图 / Excel 导出回填转入：

```text
TODO-061
```

## Control Contract

Primary Setpoint：

```text
如果当前现场还没有真实旧图石截图和导出文件，
就不要假装 TODO-060 已经拿到运行样例；
而是把现有候选工件逐个排除，
并把后续最小人工采样动作写清楚。
```

Acceptance：

```text
新增 E-DEV-082。
新增 96 实现记录。
新增 m2_drawing_029_run_001.md / json。
更新 00 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、strict readiness gate、OCCT 泄漏扫描通过。
```

Guardrail：

```text
不把空白工作簿写成旧下料表样例。
不把无关 PNG 写成 pane3 运行截图。
不自动安装 HASP。
不自动启动旧图石。
不实现真实接头线 / Others 几何算法。
不声明 AutoCAD L2 通过。
不进入 golden。
```

## 本轮工件排查

### 1. `1.xls`

路径：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\1.xls
```

hash：

```text
SHA256 = DBCF9E3226EF111DD0FFE712EC247A0568B28191FAD0ECB55D5759C83E958049
```

只读检查结果：

```text
Excel workbook
sheet = Sheet1
usedRows = 1
usedCols = 1
A1..H2 全空
```

结论：

```text
它不是我们要的旧图石下料表导出样例。
至少当前这个文件里没有
焊头(个) / 单下料长(mm) / 数据行。
```

### 2. `screenshot.png`

路径：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\screenshot.png
```

hash：

```text
SHA256 = 1FFF0DB032A81DA95D73DDEC2C6ED5D431D4B81AD7DE4F1F9EC4FF9E4699480C
```

图像内容：

```text
cmd 窗口
JDK 21+ could not be found...
```

结论：

```text
这是 JDK 提示，不是旧图石状态栏 pane3 截图。
```

### 3. `visualts_prompt_capture.png`

路径：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\visualts_prompt_capture.png
```

hash：

```text
SHA256 = 56C44CEF1FB7211131A0239264AD4BE931C7BB487E9676D59488C3A346B3E101
```

图像内容：

```text
旧图石启动阻塞提示
标题 = 提示
文本前缀 = 请检查网线是否...
```

结论：

```text
它只能证明旧启动阻塞提示，
不能证明 pane3 的 焊接 / 绑扎 / 套筒连接，
也不能证明 Excel E/F 列。
```

## 本轮新增证据

对应证据：

```text
E-DEV-082
```

口径：

```text
TODO-060 已完成：
  当前候选工件排查
  无效证据排除
  最小人工采样清单

TODO-060 未完成：
  真实 pane3 截图
  真实 Excel 导出文件或截图
```

## 最小人工采样清单

后续用户现场只需要回填这四类信息：

```text
1. 打开一个确定带接头的对象。
2. 截状态栏 pane3，确认是：
     焊接 / 绑扎 / 套筒连接
3. 导出一次下料表，
   保留 Excel 文件或至少截出：
     焊头(个)
     单下料长(mm)
4. 记录：
   - 对象编号
   - 操作步骤
   - 输出路径
   - 文件 hash
```

最小回填模板：

```text
旧图石版本：
SFL 文件名：
SFL hash：

选择对象：
  - 对象编号：
  - 是否确认有接头：

状态栏截图：
  - 文件路径：
  - pane3 文本：
  - 截图 hash：

下料表导出：
  - 文件路径：
  - 文件格式：
  - E 列标题和值：
  - F 列标题和值：
  - 文件 hash：
```

## 本轮 stop point

已闭合：

```text
现有 1.xls 不是可用下料表样例
现有 screenshot.png 不是可用 pane3 截图
现有 visualts_prompt_capture.png 不是可用 pane3 / Excel 证据
最小人工采样清单已固定
```

仍未闭合：

```text
真实旧图石 pane3 截图
真实旧图石 Excel 下料表导出
generated node+112 的最终旧调试名
连接方式来源字段最终命名
AutoCAD L2
真实接头线算法
真实 Others 几何算法
golden
```

## 对 TODO-060 的结论

`TODO-060` 现在可以按下面口径收尾：

```text
done
```

原因不是“真实运行样例已拿到”，而是：

```text
本节点的 docs-only 部分已经闭环，
把当前伪候选工件排除掉了，
并把后续必须由用户现场回填的最小样例口径固定下来了。
```

## 下一步

```text
TODO-061 / generated node+112 旧图石真实运行截图与 Excel 导出回填 P0
```

白话就是：

```text
这轮先把“哪些材料不算证据”讲清楚。
下一轮不再分析现有目录，
只等真实旧图石截图和真实 Excel 导出。
```
