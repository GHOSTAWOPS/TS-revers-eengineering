# M2-Drawing-035 生成工程图与下料表旧图石真实运行工件回填 P0 实现记录

## 结论

本轮执行 `TODO-066 / 生成工程图与下料表旧图石真实运行工件回填 P0`。

用户已把旧图石真实运行工件放到正式 repo 外层 staging/source 目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\docs\phase1\todo66
```

本轮没有启动旧图石，没有安装 `HASP`，没有修改 `app` 业务代码，也没有实现真实工程图算法。

本轮做的是接收和核对真实工件，并归档到项目证据目录：

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

核心结论：

```text
1. 旧图石真实输出链会带出 Detail.xml 文件和 Detail01.stl 到 Detail04.stl。
2. 本轮 Detail.xml 只有 14 bytes，内容为 <StyleRoot/>，是空样式根。
3. 用户补充多台电脑生成结果一致，Detail.xml 均为 <StyleRoot/>；
   生成工程图和 CAD 导入时该文件修改时间也不更新。
4. 因此 Detail.xml 更像固定空模板 / 占位文件，不是本轮业务字段载体。
5. Detail01.stl 到 Detail04.stl 均为非空 DrawingRoot XML，是本轮图纸主体。
6. 旧图石真实生成了非空 下料表.xls。
7. 下料表.xls 只读打开后包含：
   - 钢筋表
   - 钢筋汇总表
   - 钢筋下料单
8. 截图确认工程图页签下存在 生成工程图 / 下料表 按钮，
   且生成工程图后出现提示：
   剖切结束,请进入AutoCAD读取工程图!
```

大白话说：

```text
这轮终于不是“准备清单”了，
而是拿到了旧图石真实跑出来的一包东西。

它证明旧图石的生成工程图链会产出 DetailNN.stl 图纸主体，
下料表链会产出 Excel，
并且 Detail01..04.stl 与 Excel 不是空文件。

注意：本轮 `Detail.xml` 本身不是非空样式表，而是空的 `<StyleRoot/>`。
用户补充：换了几台电脑观察到的 `Detail.xml` 内容仍然一致，
都是 `<StyleRoot/>`，且生成工程图 / CAD 导入时修改时间不更新。
所以本轮把它记录为高度疑似固定空模板 / 占位文件。

`C:\Users\ghost\Desktop\reverse_engineering\Detail.xml`
和 `autocad2020\Detail.xml` 是父目录/外部历史样式样例，可能来自其他项目；
hash 与本轮不同，不能混入 `TODO-066` 当作同一次运行输出。
```

## Control Contract

Primary Setpoint：

```text
在 TODO-065 已固定 runtime_capture 门禁的基础上，
接收用户回填的旧图石真实运行工件，
核对 Detail 包、下料表、截图、hash、LastWriteTime，
并把已确认事实和剩余缺口写回文档。
```

Acceptance：

```text
新增 E-RUN-006。
新增 E-DEV-088。
归档 run_20260609_001 工件目录。
记录源 SFL 文件名和 hash；源 `.sfl` 本体按 `.gitignore` 不纳入 git。
归档并提交 Detail.xml、Detail01..04.stl、下料表.xls、截图和探针摘要。
记录 Detail.xml 多机观察均为空 StyleRoot 的事实边界。
生成 generate_package_dir_listing_01.txt。
生成 generate_package_hashes_01.txt。
生成 schedule_dir_listing_01.txt。
生成 schedule_hashes_01.txt。
生成 schedule_excel_preview_01.txt。
生成 detail_package_probe_01.txt。
更新 05 / 11 / 12 / 34 / 46 / 99 / todo.csv / 00。
默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查通过。
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
不把本轮样例写成新系统和旧图石已经 1:1。
```

## 本轮归档工件

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
  capture_notes.md
  generate_package_dir_listing_01.txt
  generate_package_hashes_01.txt
  schedule_dir_listing_01.txt
  schedule_hashes_01.txt
  schedule_excel_preview_01.txt
  detail_package_probe_01.txt
  消力池下游侧带齿槽底板结构图石钢筋模型.sfl
    -> 本地回填文件，记录 hash；因 *.sfl 忽略规则不纳入 git。
  Detail.xml
  Detail01.stl
  Detail02.stl
  Detail03.stl
  Detail04.stl
  下料表.xls
  PixPin_2026-06-09_09-49-51.png
  PixPin_2026-06-09_09-50-13.png
```

## 关键文件 hash

```text
消力池下游侧带齿槽底板结构图石钢筋模型.sfl
  6706F10836CA43D1D3275ACD09B373CAC5AF08C0D7A66B1C7025D30006C4B007

Detail.xml
  CCBD220D75D7F9C7E26E2540D639FA5956A369A31D7902F75ED36461F778F271
  bytes=14
  content=<StyleRoot/>

Detail01.stl
  444BE32ED907C0393104F415639CED2FB698F21C93DF7F27F9ED830A70E40BE6

Detail02.stl
  478D166A5DCE69A3BD8042E0118443780C8C7FBBDB93E7AC910EEB2CCB4E32F6

Detail03.stl
  2531D11A15184909CC9FB3AA8CC96866F765E8254E443422854C176453A8FB43

Detail04.stl
  03617911FBFCC2D53BCC310F515F8C375B3C6B78640C638CB2591DA1494D4D0E

下料表.xls
  A9CF511EF0513DDB45E975A822C19833009AB237C9B0C7E1CA320BC2B5BF5D1E
```

## Detail 包核对

`detail_package_probe_01.txt` 记录：

```text
Detail01.stl: DrawingRoot=True, StbGroup=12, StbTable=1, section-line=1
Detail02.stl: DrawingRoot=True, StbGroup=6,  StbTable=0, section-line=1
Detail03.stl: DrawingRoot=True, StbGroup=5,  StbTable=0, section-line=1
Detail04.stl: DrawingRoot=True, StbGroup=7,  StbTable=0, section-line=1
```

当前可确认：

```text
1. 本轮 Detail.xml 存在，但只是空 StyleRoot，不含 Style 条目。
2. 多机观察补强了 “Detail.xml 是固定空模板 / 占位文件” 的判断。
3. DetailNN.stl 继续不是网格 STL，而是 DrawingRoot XML。
4. 真实旧图石输出中存在 StbGroups、StbTable、section-line、pointStb、lineStb。
5. Detail01.stl 是本轮样例中包含 StbTables 的主图纸文件。
```

对比说明：

```text
C:\Users\ghost\Desktop\reverse_engineering\Detail.xml
  bytes=1333
  lastWrite=2024-06-20 18:40:06
  sha256=B02ABEEB7B9E0AB802566FA617E0627D9883010172EFD9CD50DACF940B5AFEB5
  含 3 个 Style，是父目录/外部历史样式样例，可能来自其他项目。

C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\docs\phase1\todo66\Detail.xml
  bytes=14
  lastWrite=2026-06-09 10:10:04
  sha256=CCBD220D75D7F9C7E26E2540D639FA5956A369A31D7902F75ED36461F778F271
  内容为 <StyleRoot/>，是本轮外部 staging/source 目录中的真实回填工件。

两者不是同一次旧图石导出产物，不能互相替换。
```

## 下料表核对

`下料表.xls` 通过本机 Excel COM 只读打开，`schedule_excel_preview_01.txt` 记录：

```text
SHEET=钢筋表
ROWS=9;COLS=6

SHEET=钢筋汇总表
ROWS=4;COLS=6

SHEET=钢筋下料单
ROWS=16;COLS=8
```

表头可见：

```text
钢筋表：编号 / 直径(mm) / 形状 / 单根长(cm) / 根数(根) / 总长(m)
钢筋汇总表：直径(mm) / 总长(m) / 单重(Kg/m) / 总重(Kg) / 钢筋合计(T) / 混凝土(m3)
钢筋下料单：编号 / 直径(mm) / 单净长(mm) / 焊长(mm) / 焊头(个) / 单下料长(mm) / 根数(根) / 总根数(根)
```

当前可确认：

```text
1. 旧图石可生成非空 Excel 下料表。
2. Excel 的第三张表含 焊头(个) / 单下料长(mm) 字段。
3. 本轮样例中 焊头(个) 多行为 0，但这不等价于接头链路运行确认。
```

## 截图核对

本轮归档两张截图：

```text
PixPin_2026-06-09_09-49-51.png
  -> 旧图石打开 SFL 后的工程图页签，能看到 生成工程图 / 下料表 按钮。

PixPin_2026-06-09_09-50-13.png
  -> 生成工程图后弹出 提示：剖切结束,请进入AutoCAD读取工程图!
```

该提示可作为生成工程图运行完成的运行证据，
但不能替代 AutoCAD 插件导入验证。

## 本轮不证明

```text
旧 AutoCAD 插件已经成功读取该 Detail 包。
AutoCAD L2 已通过。
新系统 DetailWriter 输出和旧图石输出已经 1:1。
旧插件是否要求 Detail.xml 这个空占位文件必须存在。
隐藏线 / 填充线 / 剖切线真实算法已经实现。
接头链路已经恢复优先级。
旧图石 rerun 覆盖行为已经完全闭合。
```

## 剩余缺口

```text
1. 缺少 rerun before/after 两次目录 listing，
   所以 Detail.xml + DetailNN.stl 的覆盖行为仍只能记为未完全确认。

2. 本轮 Detail.xml 是空 StyleRoot，
   用户补充多机结果也都是 <StyleRoot/>；
   当前更倾向判断为固定空模板 / 占位文件，
   但旧插件是否必须检查该文件存在仍需后续确认。

3. 旧插件 AutoCAD L2 仍未运行，
   不能声明旧插件接受该包或能生成最终 DWG 图纸。

4. 下料表 Excel 已有真实样例，
   但完整统计合并规则、质量公式、Volume722 体积等价仍需后续继续闭合。
```

## 下一步

本轮把 `TODO-066` 标记为 `done`，并新增下一阶段：

```text
TODO-067 / 旧图石真实 Detail 与下料表字段对照 P0
  -> 解析 run_20260609_001 的 Detail01..04.stl 和 下料表.xls
  -> 对照当前 DetailWriter / RebarScheduleService 已支持和缺失字段
  -> 只做字段差距报告
  -> 不实现算法，不跑 AutoCAD L2
```

本轮不自动进入 TODO-067。
