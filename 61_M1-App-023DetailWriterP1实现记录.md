# M1-App-023 DetailWriter P1 实现记录

## 结论

本轮完成 `TODO-024 / M1-App-023`。

正式 `app` 新增 `DetailWriter P1`，可以把已有 domain 钢筋对象：

```text
SteelData
SteelBarGroup
SteelBar
SteelBarSegment
```

映射为旧 AutoCAD 插件兼容方向的首批 Detail 包：

```text
Detail.xml
Detail01.stl
```

注意：这里的 `Detail01.stl` 是旧图石 Detail 包里的 `DrawingRoot` XML，不是 mesh STL。

本轮不是 AutoCAD L2 动态导入验证，不是完整工程图生成，不是下料统计公式闭合，也不新增钢筋创建 / 编辑算法。

## 新增能力

新增文件：

```text
app/src/drawing/detail/DetailWriter.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

`DetailWriter::writePackage(outputDir, SteelData, DetailWriteOptions)` 当前输出：

```text
Detail.xml
  -> root = StyleRoot

Detail01.stl
  -> root = DrawingRoot
```

首批字段映射：

```text
StbGroups / StbGroup1
  -> rsdID
  -> groupID
  -> diameter / diameter2
  -> interval
  -> barcount
  -> segcount
  -> stbNum
  -> stbNumAct
  -> stbLevel
  -> stbLayer / stbProfile / stbUse
  -> RangeLess180
  -> ComponentName
  -> PJSteelName
  -> SteelWay
  -> stbType
  -> stbOffsetInOut

Std1 / StbGeo1..n
  -> segID
  -> stbSeqNum
  -> shapeType
  -> start / middle / end
  -> start_r / end_r
  -> offset
  -> length

StbTables / StbTable / StbRow1
  -> rsdID
  -> diameter
  -> length
  -> segNum
  -> stbNumSum
  -> lenSum
  -> stbLevel

MaterialTable / MatRow1
  -> diameter
  -> lenSum
  -> countSum
  -> singleMass
  -> massSum
  -> Mass
  -> Volume722
```

`MaterialTable` 的质量公式本轮不闭合。当前输出 `0`，并显式返回：

```text
DW-WARN-MATERIAL_MASS_FORMULA_DEFERRED
```

## 输入校验

当前拒绝：

```text
缺 groups
缺 groupID / rsdID / diameter
group 引用不存在的 bar
bar 引用不存在的 segment
缺 segment id
缺 segment shapeType
```

错误码：

```text
DW004_REQUIRED_FIELD_MISSING
DW003_ID_CROSS_REFERENCE_FAILED
```

## 输出事务

事务口径：

```text
先写 candidate 包
  -> L0 校验 Detail.xml / Detail01.stl root
  -> L1 校验 StbGroup / StbRow / StbGeo 关键交叉关系
  -> 校验通过后安装到正式输出目录
  -> 安装失败恢复旧 Detail 包
```

安装失败稳定返回：

```text
DW008_REPLACE_FAILED
```

失败时：

```text
dirtyAfter = true
oldPackagePreserved = true
```

成功时：

```text
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
dirtyAfter = false
```

`l2 = not_run` 是故意的。本轮不声明 AutoCAD 动态导入通过。

## TDD

RED：

```text
detail_writer_tests 初始编译失败：
  drawing/detail/DetailWriter.h missing
```

GREEN：

```text
cmake --build app\build --target detail_writer_tests
app\build\detail_writer_tests.exe
result = pass
```

单测覆盖：

```text
DetailWriter maps domain rebar to Detail package
DetailWriter failure preserves existing package
DetailWriter rejects broken rebar references before writing
DetailWriter install failure restores existing package
```

## IDA MCP 证据

本轮涉及 Detail 首批字段写出链，因此使用 IDA MCP 补了旧 VisualTS 证据。

IDA 数据库：

```text
visualts_i64_todo024
VisualTS.exe.i64
```

确认函数：

```text
sub_14061F830
  -> 创建 StbGroups
  -> 写 stbGroupCount
  -> 循环调用 sub_14063E910

sub_14063E910
  -> 命中 StbGroup%d
  -> 字符串含 rsdID / groupID / stbNum / stbNumAct / SteelWay
  -> 调用 sub_14063B010

sub_14063B010
  -> 命中 StbGeo%d

sub_140605B20
  -> 调用 sub_140602F90

sub_140602F90
  -> 命中 DrawingRoot / StbTables / StbTable / StbRow%d
  -> 命中 MaterialTable / MatRow%d / singleMass / massSum / Mass / Volume722
```

结论边界：

```text
本轮只确认 Detail 首批字段写出链。
不确认完整质量公式。
不确认完整下料合并规则。
不确认 AutoCAD L2 导入结果。
```

## 验证结果

窄测：

```text
detail_writer_tests = pass
```

默认 CTest：

```text
13 / 13 passed
Total Test time = 126.35 sec
```

Readiness gate strict：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

OCCT / AIS 泄漏扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
result = no matches
```

`git diff --check`：

```text
pass
```

## xhigh 只读 review

本轮代码节点执行了 xhigh 只读 review，且子代理只审查、不修改。

第一轮：

```text
Verdict = block
Critical:
  replacePackage 在复制 candidate 前删除旧文件，安装失败可能破坏旧包。
  replace failure 错误码不稳定。
Important:
  缺 bar / segment 交叉引用会被静默跳过。
```

处理：

```text
补输入交叉引用校验。
补安装失败稳定 DW008。
补旧包恢复逻辑。
补失败保持 dirty。
```

第二轮：

```text
Verdict = block
Critical:
  backup 失败后仍可能进入部分删除。
  缺 replace-stage failure 测试。
```

处理：

```text
backup 阶段失败直接退出并清理 backup tmp。
新增 install failure 注入测试。
验证旧 Detail.xml / Detail01.stl / Detail02.stl 全部恢复。
```

第三轮：

```text
Verdict = allow_commit
Critical = none
Important = none
Minor:
  testInjectInstallFailureAfterFirstCopy 是公开测试注入开关，后续可收窄。
  oldPackagePreserved=true 在成功路径语义略宽，但不阻塞。
Route Drift = none
```

子代理完成后已关闭，避免代理负担。

## 边界

本轮没有：

```text
实现 AutoCAD L2 动态导入。
生成完整工程图。
闭合 MaterialTable 质量公式。
实现完整下料统计合并规则。
修改钢筋创建算法。
把父目录 rebar 业务迁入正式 app。
让 domain/rebar 依赖 OCCT / AIS。
```

## 下一步

本轮完成后只把 `todo.csv` 的下一项标为 `next`，不继续实现。

建议下一阶段：

```text
TODO-025 / 旧图石输出钢筋 STP 样本入库验证
```

原因：

```text
123.stp 是旧图石真实导出的钢筋几何 witness。
先把它的 hash、OCCT 导入统计和样本报告固定下来，
后续才能更稳地做钢筋几何输出和 golden 辅助对照。
```
