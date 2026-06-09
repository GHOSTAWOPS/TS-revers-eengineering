# M2-Drawing-038 DetailWriter StbRow 扩展属性骨架实现记录

## 结论

本轮执行 `TODO-069 / DetailWriter 真实字段差异 P1：StbRow 扩展属性骨架`。

本轮是字段骨架切片，不是公式或算法切片：

```text
已实现：
  -> StbRow.smallTable 默认输出 0
  -> StbRow.mirrorType 默认输出 0
  -> StbRow.mirrorSEFlag 默认输出 0

未实现：
  -> smallTable / mirrorType / mirrorSEFlag 的真实公式或镜像语义
  -> Excel workbook writer
  -> 隐藏线 / 填充线 / 点筋 / FaceEdge 真实算法
  -> lineStb StbGeo 字段条件化
  -> AutoCAD L2 动态导入
  -> golden 对照
  -> 钢筋创建业务
```

## Control Contract

Primary Setpoint：

```text
让正式 app 的 DetailWriter 继续按 RUN-20260609-001 真实旧包字段，
补齐 StbRow 上明确存在但当前缺失的 3 个扩展属性骨架。
```

Acceptance：

```text
1. detail_writer_tests 在旧运行包策略断言里覆盖 StbRow 三个扩展属性。
2. 修改前测试失败，失败点为 StbRow.smallTable 缺失。
3. StbRow1 输出 smallTable / mirrorType / mirrorSEFlag，默认值均为 0。
4. 默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查和 git diff check 通过。
5. xhigh 只读 review 完成，Critical / Important 已修复或有技术反驳。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不运行 AutoCAD L2。
不实现 Excel writer。
不实现真实工程图算法。
不改 lineStb 几何字段条件化。
不改钢筋创建、编辑、统计业务。
不迁入父目录 rebar 业务代码。
domain/rebar 不引入 OCCT / AIS。
```

## 输入证据

本轮基于：

```text
E-DETAIL-004
E-DEV-090
105_M2-Drawing-036旧图石真实Detail与下料表字段对照P0实现记录.md
106_M2-Drawing-037DetailWriter真实字段差异P0实现记录.md
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

旧图石真实包事实：

```text
真实 StbRow# 属性包含：
  smallTable
  mirrorType
  mirrorSEFlag

RUN-20260609-001 当前样例值均为 0。
字段语义和公式未闭合，不能把默认值当成算法完成。
```

## 实现内容

代码改动：

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

实现细节：

```text
writeStbRowLegacyAttributes()
  -> smallTable = 0
  -> mirrorType = 0
  -> mirrorSEFlag = 0

writeDrawingXml()
  -> 在每个主图 StbRow# 写完现有 schedule 字段后，
     追加旧样例确认过的扩展属性骨架。
```

本轮没有把这三个字段加入 `RebarScheduleRow`，原因是：

```text
字段存在性已由真实旧包确认；
但字段公式、镜像语义和业务来源未闭合。
直接进入 domain model 会把未知语义伪装成已建模字段。
```

## 测试

扩展测试：

```text
testDetailWriterMatchesRealRuntimeTablePackageStrategy()
```

新增覆盖：

```text
1. StbRow1.smallTable = 0。
2. StbRow1.mirrorType = 0。
3. StbRow1.mirrorSEFlag = 0。
```

TDD 记录：

```text
修改实现前：
  detail_writer_tests 失败
  失败信息 = StbRow.smallTable default mismatch

修改实现后：
  detail_writer_tests 通过
```

## xhigh review 与最终验证

```text
xhigh reviewer = needs_fix
Critical = 无
Important =
  1. build report 仍残留 not_run 占位状态。
  2. readiness gate 未拦本轮两类 not-run 占位状态。

主流程处理：
  -> 回填 m2_drawing_038_run_001.md / json 最终状态。
  -> 扩展 readiness gate pending marker 覆盖。
  -> 新增 TODO-069 not_run placeholder 负例测试。
  -> 子代理为一次性 codex exec 进程，已退出，无常驻代理需要关闭。
```

最终验证：

```text
app Debug build = pass; ninja no work to do
default CTest = pass; 18/18 pass; total 129.43 sec
readiness gate unit = pass; 28 tests OK
readiness gate strict = pass; M1-Formal-Ready; 84/84 pass
domain/rebar OCCT leak scan = pass; no matches
git diff --check = pass
```

## 本轮不关闭的缺口

```text
GAP-DRAW-007:
  已实现 StbRow smallTable / mirrorType / mirrorSEFlag 字段骨架。
  但三个字段的真实公式和镜像语义仍未闭合。

GAP-DRAW-002:
  lineStb StbGeo 字段条件化、StbGroup 多 Std#、真实工程图算法仍未闭合。

GAP-DRAW-008:
  Excel writer 未实现。

AutoCAD L2:
  未运行。
```

## 下一步

```text
TODO-070 / DetailWriter 真实字段差异 P2：lineStb StbGeo 字段条件化骨架
```

下一轮只建议处理真实旧包 lineStb `StbGeo#` 字段集合和当前 writer 额外字段的条件化差异。

不要在同一轮混入 Excel writer、隐藏线 / 填充线算法、AutoCAD L2、StbGroup 多 Std# 真实算法或钢筋创建业务。
