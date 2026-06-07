# M2-Drawing-001 DetailWriter 多图纸 P0 实现记录

## 结论

本轮完成：

```text
TODO-031 / M2-Drawing-001
DetailWriter 多图纸 DetailNN.stl P0
```

完成范围是：

```text
按 IDA MCP 证据确认旧 DetailNN.stl 命名规则。
让 DetailWriter 支持一个 DrawingView 对应一个 DetailNN.stl。
补齐多图纸 L0/L1 校验、成功替换时旧 DetailNN 清理、失败恢复旧包。
```

本轮不是完整工程图生成专项完成，也不是 AutoCAD L2 动态导入通过。

## IDA MCP 证据

新增证据：

```text
E-IDA-026
```

IDA MCP 会话：

```text
session = visualts_i64_todo031_recheck
database = VisualTS.exe.i64
Hex-Rays = ready
```

已确认旧命名函数：

```text
sub_140635A80(a1, viewIndex)
```

关键规则：

```text
GetTempPathA(...)
  -> 追加 msohtmplcllip

if viewIndex < 10:
  "\\Detail0%d.stl"
else:
  "\\Detail%d.stl"

然后调用 COM / XML 保存接口写出该 DetailNN.stl。
```

含义：

```text
Detail01.stl ... Detail09.stl 使用两位补零。
Detail10.stl 以后使用自然数字。
Detail100.stl 不是 Detail0100.stl。
```

和工程图主链关系：

```text
sub_140600AA0
  -> 创建 StbTables / HViewPorts / ViewPort / PartDetailDrawing / StbDetailDrawing
  -> 调用 sub_140609690 写表
  -> 调用 sub_14061F830 写 StbGroups
  -> 调用 sub_140635A80(ppv, viewIndex) 保存 DetailNN.stl
```

## 实现内容

新增 / 修改：

```text
app/src/drawing/detail/DetailWriter.h
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

新增 DTO：

```text
DetailDrawingViewOptions
  viewId
  drawingName
  modelFileName
  drawingUnit
  drawingScale
  generalScale
```

`DetailWriteOptions` 新增：

```text
QVector<DetailDrawingViewOptions> views
```

兼容规则：

```text
options.views 为空：
  保持旧 P1 行为，生成 Detail.xml + Detail01.stl。

options.views 非空：
  按 views 顺序生成 Detail01.stl、Detail02.stl ... DetailNN.stl。
```

## 事务语义

本轮把 package file list 从固定：

```text
Detail.xml
Detail01.stl
```

改为：

```text
Detail.xml
Detail01.stl
Detail02.stl
...
DetailNN.stl
```

并让以下流程消费同一份文件列表：

```text
候选包写入
L0 root 校验
L1 StbGroup / StbRow / StbGeo 交叉校验
目标包替换
result.files
```

成功替换时：

```text
复制新 Detail.xml + 所有新 DetailNN.stl。
删除目标目录中旧的多余 DetailNN.stl。
保留非 Detail 文件。
```

安装失败时：

```text
删除半安装 Detail 文件。
从 backup 恢复旧 Detail.xml 和旧 DetailNN.stl。
保持 dirtyAfter = true。
返回 DW008_REPLACE_FAILED。
```

## TDD

新增测试：

```text
testDetailWriterWritesMultipleDetailViewsWithLegacyNames
testDetailWriterRemovesStaleDetailViewsOnSuccessfulInstall
```

覆盖：

```text
Detail01.stl / Detail02.stl / Detail09.stl / Detail10.stl / Detail100.stl 命名。
禁止 Detail010.stl / Detail0100.stl。
每个 DetailNN.stl 根节点都是 DrawingRoot。
Detail02 / Detail10 / Detail100 的 ViewPort 和 General-Info 来自对应 view。
成功两视图安装后删除旧 Detail03.stl。
成功安装保留 keep.txt 等非 Detail 文件。
安装失败仍恢复旧 Detail01.stl / Detail02.stl。
```

## 验证结果

窄测：

```text
cmake --build app\build --target detail_writer_tests
app\build\detail_writer_tests.exe

pass
```

全量构建：

```text
cmake --build app\build

pass
```

全量 CTest：

```text
ctest --test-dir app\build --output-on-failure

16 / 16 pass
```

Readiness gate：

```text
py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict

M1-Formal-Ready
84 / 84 pass
0 error
0 warning
```

OCCT / AIS 泄漏扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project

no matches
```

Diff 检查：

```text
git diff --check

pass
```

## xhigh Review

本轮代码节点已执行 xhigh 只读 review。

第一轮 review 代理卡住，已关闭以减少代理负担：

```text
agent = xhigh_readonly_detail_multiview_reviewer
previous_status = running
action = closed
```

第二轮 review：

```text
agent = xhigh_readonly_detail_multiview_reviewer_2
verdict = allow_commit
Critical = none
Important = none
Minor = 输入校验失败时 diagnostics 仍固定标记 Detail01.stl；
        多图纸场景下不够精确，但不影响本轮 P0 提交。
```

处理结论：

```text
Minor 记录到 build report。
本轮不扩大修改面。
```

## 边界

本轮不声明：

```text
完整工程图生成已完成。
剖切线 / 隐藏线 / 填充线算法已复刻。
AutoCAD L2 动态导入已通过。
旧插件多图纸 Detail100.stl 运行确认已通过。
生成工程图 UI / Dialog / 输出目录行为已运行确认。
```

仍保留缺口：

```text
GAP-DRAW-001 AutoCAD 动态导入。
GAP-DRAW-002 DetailNN.stl 复杂字段完整性。
GAP-DRAW-003 隐藏线 / 剖切线 / 填充线保真。
GAP-DRAW-005 Detail writer 输出事务后续复杂字段和 AutoCAD L2。
GAP-IDA-005 生成工程图顶部命令入口和运行确认。
```
