# TODO-025 旧图石输出钢筋 STP 样本入库验证记录

## 结论

本轮完成 `TODO-025 / 旧图石输出钢筋 STP 样本入库验证`。

`123.stp` 已被固定为首个旧图石钢筋几何 witness：

```text
sampleId = tushi_rebar_123_stp
sourceSfl = 消力池下游侧带齿槽底板结构图石钢筋模型.sfl
sourceStep = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp
```

本轮只读样本并生成报告，不修改钢筋业务代码，不新增钢筋创建算法，不扩展 DetailWriter，不做 AutoCAD L2 动态导入。

## 文件身份

源 SFL：

```text
path = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\消力池下游侧带齿槽底板结构图石钢筋模型.sfl
size = 563,051 bytes
sha256 = a5645635ff77346e1dacccbac57b7f594d1278d74bf70f4be762dd93ced90515
```

旧图石导出钢筋 STP：

```text
path = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp
size = 87,746,388 bytes
sha256 = 6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
lastWriteTime = 2026-06-05 10:02:17 +08:00
```

STEP 头部：

```text
FILE_DESCRIPTION(('STEP AP214'),'1')
FILE_NAME('123.stp','2026-06-05T02:02:12',...,'Spatial InterOp 3D',...)
FILE_SCHEMA(('AUTOMOTIVE_DESIGN { 1 0 10303 214 1 1 1 1 }'))
```

## 当前 OCCT 导入统计

本轮重新运行：

```powershell
app\build\step_import_probe.exe "..\123.stp"
```

结果：

```text
ok = true
read_ok = true
transfer_ok = true
roots = 1
free_shapes = 1
solids = 754
faces = 3016
edges = 9048
vertices = 18096
length_unit = METRE
```

这说明当前正式 app 的 STEP import probe 仍可稳定读取该样本。

## STEP selection gate 证据

既有 5 轮 selection gate 报告：

```text
docs/phase1/step_selection/tushi_rebar_123_stp/step_selection_diff.json
docs/phase1/step_selection/tushi_rebar_123_stp/step_selection_diff.md
```

摘要：

```text
decision = pass
probeRunCount = 5
copy-path run = included
sourceSha256 = sha256:6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
```

稳定引用统计：

```text
faces = 3016
edges = 4524
vertices = 3016
minMatchRate = 1.000000
duplicateCount = 0
mismatchedRuns = none
```

说明：

```text
raw edges = 9048
stable unique edges = 4524
```

这是 OCCT raw explorer 与 selection-v1 去重口径差异，不是本轮错误。

## 证据边界

`123.stp` 可以证明：

```text
旧图石存在输出钢筋 STP 能力。
当前样本可被 OCCT 导入。
当前样本可生成稳定 selection-v1 face / edge / vertex 引用。
当前样本几何结构是 754 solids / 3016 faces / 9048 raw edges / 18096 raw vertices。
```

`123.stp` 不能证明：

```text
旧图石钢筋创建算法。
旧图石钢筋参数和字段值。
线筋 / 弧筋 / 面筋规则。
钢筋编号、统计、下料合并规则。
新系统已 1:1 生成同样钢筋。
```

所以它只能作为后续几何 golden 辅助证据，不能替代 IDA / SFL / Detail / 旧图石运行证据。

## 关联证据

Requirement：

```text
REQ-PROJ-005
REQ-PROJ-001
REQ-TECH-002
```

Evidence：

```text
E-RUN-002
E-DEV-023
E-DEV-048
```

GAP：

```text
GAP-IDA-008
GAP-DEV-002
```

## 验证结果

默认 CTest：

```text
13 / 13 passed
Total Test time = 125.71 sec
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

## xhigh 说明

本轮没有修改代码、测试或构建脚本，只新增和更新样本 witness / 文档 / todo。

按 `46` 的 TODO-025 控制合同：

```text
纯样本报告节点可记录不需要 xhigh 的理由。
```

因此本轮不启动 xhigh 代码 review。

## 下一步

本轮完成后只把 `todo.csv` 的下一项标为 `next`，不继续实现。

建议下一阶段：

```text
TODO-026 / 旧图石运行 golden 采集方案
```

但用户之前明确说 golden 先不要，所以更现实的下一轮建议是：

```text
TODO-027 / 旧 UI 功能入口 1:1 复刻 P1
```

如果继续严格按 `todo.csv`，则 `TODO-026` 只是 P2 golden 方案，不应自动采集 golden。
