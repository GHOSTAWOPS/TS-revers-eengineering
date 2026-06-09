# M2-Drawing-037 DetailWriter 真实字段差异 P0 实现记录

## 结论

本轮执行 `TODO-068 / DetailWriter 真实字段差异 P0：空 Detail.xml 与主图表格策略`。

本轮是实现切片，不是算法切片：

```text
已实现：
  -> Detail.xml 按旧图石真实包输出空 <StyleRoot/>
  -> StbTable / MaterialTable 只在 Detail01.stl 输出
  -> Detail02.stl 及后续副图保留空 StbTables 容器
  -> StbTable 补表级属性骨架

未实现：
  -> Excel workbook writer
  -> 隐藏线 / 填充线 / 点筋 / FaceEdge 真实算法
  -> AutoCAD L2 动态导入
  -> golden 对照
  -> 钢筋创建业务
```

## Control Contract

Primary Setpoint：

```text
让正式 app 的 DetailWriter 在旧图石包兼容输出上，
先对齐 RUN-20260609-001 中最明显的真实格式差异。
```

Acceptance：

```text
1. detail_writer_tests 新增旧运行包策略断言。
2. Detail.xml 字节级输出 <StyleRoot/>\r\n。
3. Detail01.stl 输出 StbTable / MaterialTable。
4. Detail02.stl 及后续副图输出空 StbTables，且仍保留 StbDetailDrawing/StbGroups。
5. StbTable 输出 HeightValue0 / HeightValueCount / Volume1225 /
   NumCombineGoJians / SteelNetArea / GJTAOTNumber / GJTAOTVolue /
   LinkTop / LinkDown / DCGQSJ / HYLJJ。
6. 默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查和 git diff check 通过。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不运行 AutoCAD L2。
不实现 Excel writer。
不实现真实工程图算法。
不改钢筋创建、编辑、统计业务。
不迁入父目录 rebar 业务代码。
domain/rebar 不引入 OCCT / AIS。
```

## 输入证据

本轮基于：

```text
E-DETAIL-004
E-DEV-089
105_M2-Drawing-036旧图石真实Detail与下料表字段对照P0实现记录.md
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

旧图石真实包事实：

```text
Detail.xml = <StyleRoot/>
Detail01.stl = StbTables / StbTable / MaterialTable
Detail02.stl 到 Detail04.stl = StbTables 空容器，无 StbTable / MaterialTable
```

## 实现内容

代码改动：

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

实现细节：

```text
writeStyleXml()
  -> 不再写 StyleRoot / Styles / Style1
  -> 改为字节级写出 <StyleRoot/>\r\n

writeDrawingXml(..., includeScheduleTables)
  -> Detail01.stl 调用时 includeScheduleTables=true
  -> Detail02.stl 及后续 includeScheduleTables=false

StbTable
  -> count 继续来自 RebarScheduleService
  -> Volume1225 暂与 schedule.volume722 对齐
  -> 其余旧表级属性按当前真实样例和未闭合公式写默认骨架

validateL1File()
  -> 对没有 StbRow 的副图不再强制 StbGroup.rsdID == StbRow.rsdID
  -> 仍保留 Std / StbGeo 数量一致性检查
```

## 测试

新增测试：

```text
testDetailWriterMatchesRealRuntimeTablePackageStrategy()
```

覆盖：

```text
1. Detail.xml 字节级等于 <StyleRoot/>\r\n。
2. Detail01.stl 的 StbTables 直接子节点包含 StbTable / MaterialTable。
3. StbTable 含 TODO-067 确认的 11 个表级属性骨架。
4. Detail02.stl 的 StbTables 为空。
5. Detail02.stl 不含 StbTable / MaterialTable。
6. Detail02.stl 仍含 StbGroups，保证副图钢筋图形主体没有被误删。
```

## 本轮不关闭的缺口

```text
GAP-DRAW-006:
  已实现空 Detail.xml 输出策略。
  但旧插件是否必须检查该文件存在、是否忽略非空样式表，仍需 AutoCAD L2。

GAP-DRAW-007:
  已实现主图表格策略和 StbTable 表级属性骨架。
  但表级属性公式、StbRow smallTable / mirrorType / mirrorSEFlag、
  Std# 多容器和 lineStb 字段条件化仍未闭合。

GAP-DRAW-008:
  Excel writer 未实现。
```

## 下一步

```text
TODO-069 / DetailWriter 真实字段差异 P1：StbRow 扩展属性骨架
```

下一轮只建议补：

```text
StbRow.smallTable
StbRow.mirrorType
StbRow.mirrorSEFlag
```

不要在同一轮混入 Excel writer、隐藏线 / 填充线算法、AutoCAD L2 或钢筋创建业务。
