# M2-Stats-001 Run 001

## 结论

```text
decision = m2-stats-001-pass
```

本轮完成 `TODO-030 / M2-Stats 钢筋统计 / 下料表 P0` 的实现和首轮测试。

最终 xhigh 复审为 `allow_commit`，本报告已收口。

## 范围

```text
IDA MCP 补证 StbTable / MaterialTable Detail writer 链
domain/rebar RebarScheduleService
DetailWriter 接入 RebarScheduleService
rebar_schedule_service_tests
```

本轮没有完成整个工程图专项，也没有闭合旧图石完整材料质量公式。

## 关键实现

```text
app/src/domain/rebar/RebarScheduleService.h
app/src/domain/rebar/RebarScheduleService.cpp
app/tests/unit/rebar_schedule_service_tests.cpp
app/src/drawing/detail/DetailWriter.cpp
app/CMakeLists.txt
tools/phase1_readiness_gate/check_phase1_readiness.py
```

## 验证

窄测：

```text
ctest --test-dir build -C Debug -R "rebar_schedule_service_tests|detail_writer_tests" --output-on-failure
2 / 2 pass
```

全量 CTest：

```text
ctest --test-dir build -C Debug --output-on-failure
16 / 16 pass
```

Readiness gate：

```text
py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
M1-Formal-Ready, 84 / 84 pass, 0 error, 0 warning
```

OCCT / AIS 泄漏扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
no matches
```

## IDA MCP

```text
session = visualts_i64_todo030
database = VisualTS.exe.i64
Hex-Rays = ready
```

确认：

```text
sub_140609690
  -> sub_140607E30 -> sub_140637400 写 StbTable / StbRow
  -> sub_140609190 -> sub_1406382D0 写 MaterialTable / MatRow

sub_140637400
  diameter = a1[8]
  length = sub_140637070(...)
  sameGrpNum = a1[1]
  stbNumSum = a1[3]
  lenSum = *((double*)a1 + 5)

sub_1406382D0
  diameter = *(this + 4)
  lenSum = *(this + 8)
  countSum = *(this + 0)
  singleMass = GetDefaultValue(this)
  massSum = lenSum * singleMass
```

## 边界

本轮不声明：

```text
sameGrpNum 完整合并规则已闭合。
singleMass 旧来源已完整闭合。
Volume722 的旧 ACIS 体积逻辑已复刻。
Excel 输出和 Dialog #427 已运行确认。
AutoCAD L2 / golden 已通过。
```

## xhigh Review

第一轮只读 review：

```text
verdict = block_commit
Critical = 多个 MaterialRow 中部分 deferred 时，可能出现 MaterialTable.Mass = 0
           但局部 MatRow.massSum 非 0 的矛盾输出。
Important = MaterialRow 按 diameter + steelLevel 聚合是 P0 近似；
            MassNum 入口来源未闭合。
```

主流程处理：

```text
已修复：只要任一材料行 deferred，全表 MaterialTable.Mass 和所有 MatRow.massSum
统一 deferred 为 0。
已补测试：多 MaterialRow 中一行可算、一行 deferred 时，所有 massSum 清零。
已补文档：MaterialRow 聚合规则和 MassNum 均标为 P0 近似 / GAP。
```

复审：

```text
verdict = allow_commit
Critical = none
Important = none
Minor = none
```
