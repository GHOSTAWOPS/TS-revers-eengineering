# 1 比 1 复刻开工 Readiness 审计

## 目标

本文件回答：

```text
现在到底能不能按 1 比 1 复刻路线开工？
能开发哪一层？
哪些内容只能借鉴父目录源码？
哪些必须继续 IDA / 运行确认？
```

结论先说：

```text
可以准备 1 比 1 复刻外壳和底层 spike。
不能把父目录现有源码路线当成权威路线。
不能沿用“基于 OCCT 自研一套钢筋软件”的口径。
钢筋业务必须以旧图石证据为主，以父目录源码为参考。
```

## 父目录源码边界

父目录 `C:\Users\ghost\Desktop\reverse_engineering` 下已有 CMake、Qt、OCCT、viewer、工程保存、Detail writer 和大量测试。

这些内容只能作为参考：

- CMake 组织方式。
- Qt / OCCT 集成方式。
- STEP 导入经验。
- AIS Viewer 交互经验。
- Detail XML writer 的字段写法和测试思路。
- 工程保存、失败回滚、单元测试写法。

不能作为权威：

- 不能把父目录路线视为本项目路线。
- 不能用父目录的钢筋生成算法替代旧图石算法。
- 不能用父目录的工程格式命名覆盖本目录文档决策。
- 不能因为父目录已有实现，就把本目录缺口标为关闭。
- 不能把“OCCT 重开发一套钢筋软件”写成 1 比 1 复刻。

判断规则：

```text
旧图石证据能证明的，按旧图石。
旧图石证据不足的，进 GAP。
父目录源码只能提供实现参考，不能提供业务真相。
```

完整边界见 `23_父目录源码参考边界与路线纠偏.md`。

## 当前 Readiness 总表

| 阶段 | 状态 | 可以做 | 不能做 |
|---|---|---|---|
| M0 证据闭合 | 进行中 | 继续补 IDA、运行确认、SFL 字段地图 | 把 Open 缺口写成确定结论 |
| M1 Qt6 + OCCT 外壳 | 可准备 | 主窗口、页签、按钮、命令注册、OCCT Viewer、STEP 导入 spike | 把父目录 UI/命令口径当旧图石口径 |
| M2 设计文件格式 / 选择 ID | 可设计和 spike | SFL 证据驱动的新格式、STEP face/edge fingerprint diff、SFL + OCCT 联合包任务、legacyObject.raw 与 geometryRef 的 binding 校验 | 只看父目录格式或只看 OCCT，忽略 SFL 业务对象证据，或只并列保存两堆数据不做绑定 |
| M3 钢筋核心创建 | 仅专项 spike | 面配筋、线配筋、弧形组专项验证 | 用父目录算法替代旧图石生成逻辑 |
| M4 编辑 / 查询 / 检查 | 部分可 spike | 查询、碰撞、最小距离的模型和几何验证 | 完整编号、组合开、dirty 口径宣称闭合 |
| M5 工程图 / 下料 | 可离线 spike | Detail writer L0/L1、字段映射、失败回滚 | 没有 AutoCAD 导入就宣称 C3 闭环 |
| M6 老 SFL 兼容 | 后置 | 样本证据和字段地图 | 直接把 `.sfl` 当新系统主格式 |

## 可以做的准备工作

### 1. Qt6 外壳

允许内容：

- 5 个一期页签：开始、显示、钢筋、查询、工程图。
- 按 `17_一期按钮追溯与命令占位矩阵.md` 注册按钮。
- 每个按钮都有 `CommandId`。
- `Open` 状态按钮只显示未实现提示。
- 状态栏、模型树、属性面板、选择集框架。
- 可以参考父目录 Qt Widgets / viewer 组织方式。

必须遵守：

- 依赖按 `21_QtOCCT依赖许可证门禁.md`。
- 不能引入 GPL-only Qt 模块。
- 不能引入新的商业 3D SDK。
- 不能把父目录按钮表、命令表替代 `17_一期按钮追溯与命令占位矩阵.md`。

### 2. OCCT Viewer / STEP 导入 spike

允许内容：

- 导入 STEP/STP。
- 显示模型。
- 选择 face / edge / vertex。
- 输出选择对象描述。
- 跑 `19_STEP选择ID稳定性Spike计划.md`。
- 可以参考父目录 `StepImportService`、`SelectionRef` 的技术做法。

通过前限制：

- 不允许把 face/edge 导入顺序当可保存 ID。
- 不允许把依赖选择 ID 的钢筋编辑做成正式功能。
- 不允许把父目录 `SelectionRef` 直接视为已验证的 1 比 1 复刻选择 ID。

### 3. 新设计文件格式 spike

允许内容：

- 创建新设计文件格式样例。
- 保存 `manifest.json / project.json`。
- 保存 STEP 路径和 hash。
- 保存旧图石证据驱动的 `RebarModel` 字段草案。
- 保存一个参考面。
- 保存 OCCT / STEP 几何引用和拓扑 fingerprint。
- 失败回滚测试。
- 可以参考父目录序列化代码，但只参考事务、schema、回滚、测试组织。
- 扩展名、字段、目录结构不得默认继承父目录。

对应文档：

- `18_新设计文件格式替代SFL策略.md`
- `GAP-DEV-001`

当前注意：

- 新格式要结合 SFL 业务对象和 OCCT 几何拓扑。
- SFL / IDA / Detail 决定业务字段。
- OCCT / STEP 决定几何引用、拓扑选择 ID、剖切和投影承载。
- `binding` 决定旧业务对象与新几何引用如何对应，以及失败时进入修复/只读状态。
- 父目录源码只能参考事务和测试写法。

### 4. Detail writer 离线 spike

允许内容：

- 生成首期可验证 `Detail.xml`。
- 生成首期可验证 `Detail01.stl` XML。
- L0 XML 校验。
- L1 包内 ID 一致性校验。
- 失败不覆盖旧包。
- audit 记录。
- 可以参考父目录 `DetailXmlWriter / DetailStlXmlWriter / PluginPackageExporter`。
- 字段语义和闭环状态只以旧 Detail 样例、字段矩阵和 AutoCAD 旧插件动态导入为准。

限制：

- 未通过 AutoCAD 2020 + 旧插件导入前，只能标 C2。
- 不能宣称工程图链路闭环。
- 不能因为父目录测试通过，就关闭旧插件动态导入缺口。

对应文档：

- `20_DetailWriter输出事务契约.md`
- `GAP-DRAW-005`

## 不能直接做的工作

### 1. 全量钢筋算法复刻

原因：

- 面配筋只是第一个样板。
- 线配筋和弧形组已有 IDA spike，但选择流程、参数窗口、SFL 字段仍未全闭合。
- 剖面圈筋、固中心、组合开仍是运行确认缺口。

正确做法：

```text
一个命令一个专项。
每个专项先补证据，再 spike，再实现。
```

### 2. 老 SFL 直接导入

原因：

- SFL 是 `ACIS BinaryFile + 图石私有对象`。
- 当前没有 ACIS 授权。
- `.sfl` 首期只作为证据，不作为主工程格式。

正确做法：

- 新系统先支持 STEP/STP 和新设计文件格式。
- 旧 SFL 后置兼容专项。

但这不等于把 SFL 证据后置。

首期新格式仍必须保存 SFL / IDA / Detail 反推的 `legacyObject.raw` 和 evidence。

### 3. 直接写 DWG / DXF

原因：

- DWG/DXF 会引入新的商业库风险。
- 用户已有 CAD 商业版。
- 当前最快闭环是写旧插件可导入的 Detail XML 包。

正确做法：

- 新系统生成 `Detail.xml + DetailNN.stl` XML 包。
- 用 AutoCAD 2020 + 旧插件导入验证。

## Phase 1 开工门禁

进入 1 比 1 复刻工程开工前必须有：

- Qt 版本选择。
- OCCT 版本选择。
- `DEPENDENCIES.md` 草案。
- `THIRD_PARTY_NOTICES.md` 草案。
- `Build.DependencyGate` 命令或脚本计划。
- 新设计文件格式首期可验证联合契约：`legacyObject.raw + geometryRef + binding + evidence`。
- 新设计文件格式 schema、读写 fixture 和保存失败回滚测试计划。
- schema / fixture 草案见 `24_新设计文件格式Schema与Fixture草案.md`。
- SFL + OCCT 联合开发任务拆解见 `25_新设计文件格式SFL_OCCT联合开发任务拆解.md`。
- Fixture A/B/C、negative fixture、validator 错误码、失败输出格式和 Save/Open 事务测试执行清单见 `26_首期可验证联合格式Fixture与Validator执行清单.md`。
- UI demo、M1-Prep 和正式 Phase 1 的区别，以及首批工程工件清单见 `27_Qt6_OCCT开发入口门禁与首批工件清单.md`。
- Qt / OCCT 依赖清单、notice、license、SBOM 和 `Build.DependencyGate` 模板见 `28_QtOCCT依赖清单与发布材料模板.md`。
- Fixture A/B/C、negative fixture、golden 断言、Save/Open 状态矩阵和 STEP diff 占位见 `29_首期tsrebar实际Fixture包清单与Golden断言.md`。
- Phase 1 初始化顺序、门禁运行和 `readiness_run_001.md` 记录模板见 `30_Phase1工程初始化Runbook与Readiness记录模板.md`。
- STEP 选择 ID 样本清单、5 轮运行矩阵、run JSON、diff 模板和 `unresolvedGeometry` 判定见 `31_STEP选择ID实际运行记录模板与样本清单.md`。
- Validator 接口、执行顺序、错误码、报告 schema、Save/Open gate 和 readiness 接入见 `32_Validator实现契约与错误码总表.md`。
- 新格式工作名 `*.tsrebar` 仅作开发期占位，不沿用父目录 `.tsrproj/.vtsproj`。
- 父目录源码参考清单：哪些复用思想，哪些禁止复用口径。

未满足也可以写 UI demo，但不能叫正式 Phase 1。

## Phase 2 钢筋模型门禁

进入钢筋领域模型正式开发前必须有：

- `RebarGroup / Rebar / RebarSegment` 保存恢复 spike。
- `groupId / barId / segmentId` 不变量测试。
- `sourceSelection.faceId / edgeId` 保存恢复策略。
- Detail `StbGroup / StbGeo / StbRow` 能从同一模型派生。

缺失时只能做领域模型草案或内存原型。

## Phase 3 钢筋创建门禁

进入每个创建命令正式实现前，至少需要：

- 旧按钮入口。
- `CommandId`。
- 参数窗口字段。
- 选择流程。
- 输出对象结构。
- dirty 规则。
- 撤销/取消口径。
- 至少一个验收样本。

缺任一项：

- 可以做 UI 占位。
- 可以做 IDA / 运行确认。
- 不进入正式业务实现。

## Phase 5 工程图门禁

进入工程图 writer 正式验收前必须有：

- L0 XML 文件级校验通过。
- L1 包内一致性校验通过。
- 失败回滚测试通过。
- AutoCAD 2020 + 旧插件 L2 导入记录。
- `StbTable` 和 `StbGroups` ID 对齐。

当前证据状态：

- `E-IDA-018` 已补强 `StbGroup / Std / StbGeo` 写出字段链。
- `E-IDA-019` 已补强 `StbTable / StbRow / MaterialTable` 写值字段链。
- `E-IDA-043` 已把下料表内部命令纠偏为 `35057 / 0x88F1 / psexcel / sub_140605B20`。
- `E-IDA-042` 已补强 `工程图 / 输出 -> {36124, 35057}` 的 UI 区域绑定。
- 仍需旧图石运行或继续 IDA 闭合按钮 caption `下料表` / Dialog #427 对应、字段值公式、输出文件和 AutoCAD 导入结果。

没有 L2：

```text
只能说 writer 离线 spike 通过。
不能说工程图复刻闭环完成。
```

## 当前最建议下一步

按工程收益排序：

1. 按 `27` 冻结 Phase 1 入口工件清单，避免把 UI demo 当正式开工。
2. 用 SFL / IDA / Detail 继续校准新设计文件格式字段，推进 `GAP-DEV-001`。
3. 按 `26`、`29` 和 `32` 产出 Fixture A/B/C、negative fixture、validator 任务、golden 断言和 Save/Open 回滚测试。
4. 按 `30` 生成 Phase 1 初始化 runbook 记录模板，推进 `GAP-DEV-007`。
5. 按 `31` 做 STEP 选择 ID diff，推进 `GAP-DEV-002`。
6. 准备 Qt6 + OCCT 外壳，但所有 Open 按钮只做占位。
7. 做 Detail writer L0/L1 离线 spike，推进 `GAP-DRAW-005`。
8. 用户运行旧图石补 `剖面圈筋 / 固中心 / 组合开 / 生成工程图 / 下料表`。

## 白话总结

现在不是“不能开发”，而是不能跑偏。

现在能做的是：

- 软件壳。
- 工程格式。
- 选择 ID。
- Detail writer 离线链路。

暂时不能硬上的，是：

- 大而全钢筋算法。
- 老 SFL 直接导入。
- DWG/DXF 直接写出。
- 没 AutoCAD 验证就宣称工程图完成。
- 把父目录 OCCT 自研钢筋路线当成 1 比 1 复刻路线。
