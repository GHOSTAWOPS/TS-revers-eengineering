# Qt6 + OCCT 开发入口门禁与首批工件清单

## 目标

本文件回答一个很具体的问题：

```text
什么时候可以进入 Qt6 + OCCT 开发？
进入开发前必须先准备哪些工件？
哪些只能算 UI demo 或技术 spike，不能叫正式 Phase 1？
```

核心结论：

```text
可以准备 Qt6 + OCCT 外壳。
但正式 Phase 1 不能只靠“能打开窗口、能显示 STEP”。

正式 Phase 1 必须同时具备：
旧图石功能入口证据、
Qt / OCCT 依赖门禁、
新设计文件格式首期可验证包契约、
STEP 选择 ID 验证计划、
父目录源码参考边界、
以及不能关闭的 GAP 清单。
```

本文件新增开发证据：

```text
E-DEV-006
  -> Qt6 + OCCT 开发入口门禁与首批工件清单
  -> 来源：27_Qt6_OCCT开发入口门禁与首批工件清单.md
```

注意：

- `E-DEV-006` 只证明开工门禁清单已经明确。
- 它不能证明 Phase 1 已经开工完成。
- 它不能关闭 `GAP-DEV-007 / GAP-TECH-007 / GAP-DEV-001 / GAP-DEV-002`。

## 分层定义

### M1-Prep：允许准备

允许做：

- Qt6 工程壳。
- 主窗口。
- 5 个一期页签。
- 按钮占位。
- CommandId 注册表。
- OCCT Viewer。
- STEP 导入显示 spike。
- 选择、高亮、状态栏、模型树框架。

限制：

- `Open` 缺口按钮只能显示未实现或待确认状态。
- 不得把父目录 UI / 命令表当旧图石证据。
- 不得把 OCCT 生成的几何对象直接当正式钢筋模型。

### M1-Formal：正式 Phase 1

正式 Phase 1 不是“窗口能跑”。

正式 Phase 1 必须具备本文件列出的首批工件，并且每个工件能追到文档和 GAP。

判断标准：

```text
M1-Prep 可以边做边补。
M1-Formal 必须有可审计门禁。
```

## 首批工件清单

### A. 复刻范围工件

必须存在：

- `01_功能操作矩阵.md`
- `02_界面窗口参数矩阵.md`
- `08_开发命令契约.md`
- `17_一期按钮追溯与命令占位矩阵.md`

必须证明：

- 一期页签是 `开始 / 显示 / 钢筋 / 查询 / 工程图`。
- 一期按钮都有旧图石入口证据或明确 GAP。
- 每个按钮都有新系统 `CommandId` 或占位策略。
- Open 状态不能写成已实现。

阻塞信号：

- 按钮文字没有旧图石截图、字符串、Dialog 或 IDA 证据，却写成 confirmed。
- 只按父目录 UI 表生成按钮。
- 钢筋页只保留面配筋，漏掉线配筋、扇形筋、箍筋、插筋、螺旋筋等一期入口。

### B. 技术路线工件

必须存在：

- `06_技术路线与替代方案.md`
- `21_QtOCCT依赖许可证门禁.md`
- `23_父目录源码参考边界与路线纠偏.md`

必须证明：

- Qt6 只替代 MFC / Codejock 类界面底座。
- OCCT 只替代 ACIS / HOOPS 类几何和显示底座。
- 钢筋业务规则仍由旧图石运行、IDA、SFL、Detail 证据驱动。
- 父目录源码只能参考 CMake、Qt/OCCT 集成、事务、测试写法。

阻塞信号：

- 写成“OCCT 负责钢筋生成逻辑”。
- 写成“父目录钢筋算法可作为旧图石业务真相”。
- 引入新的商业 3D SDK。

### C. 依赖合规工件

模板和门禁说明见：

- `21_QtOCCT依赖许可证门禁.md`
- `28_QtOCCT依赖清单与发布材料模板.md`

正式 Phase 1 工程初始化前必须产出：

```text
DEPENDENCIES.md
THIRD_PARTY_NOTICES.md
licenses/
  QT_LICENSES.txt
  OCCT_LICENSE.txt
sbom/
  project-sbom.spdx.json
```

至少记录：

- Qt 版本。
- OCCT 版本。
- Qt 模块清单。
- OCCT toolkit / 能力清单。
- 动态链接策略。
- 禁止模块检查结果。
- 是否随包分发。
- 是否修改源码。

首期允许：

- Qt Core / Gui / Widgets / OpenGL / OpenGLWidgets。
- Qt Test 仅限测试目标。
- OCCT 开源内核、BRep、STEP、AIS Viewer、几何算法。

首期禁止：

- GPL-only Qt 模块。
- OCCT 商业组件。
- ACIS / Parasolid / DXF / JT 等商业转换组件。
- 把 AutoCAD 或旧插件作为新程序基础运行时。

关联 GAP：

- `GAP-TECH-001`
- `GAP-TECH-002`
- `GAP-TECH-007`

### D. 新设计文件格式工件

必须存在：

- `18_新设计文件格式替代SFL策略.md`
- `24_新设计文件格式Schema与Fixture草案.md`
- `25_新设计文件格式SFL_OCCT联合开发任务拆解.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`
- `30_Phase1工程初始化Runbook与Readiness记录模板.md`
- `32_Validator实现契约与错误码总表.md`

正式 Phase 1 前必须产出实际工程工件：

```text
fixtures/tsrebar/fixture_a_empty_step/project.tsrebar/
fixtures/tsrebar/fixture_b_single_group/project.tsrebar/
fixtures/tsrebar/fixture_c_pending_legacy/project.tsrebar/
fixtures/tsrebar/negative/
tests/fixtures/
```

必须证明：

- `legacyObject.raw` 存在。
- `geometryRef` 存在或明确 `unresolvedGeometry`。
- `binding` 存在。
- evidence ID 可追溯。
- Detail mapping 能追回 `groupId / rsdId / segmentId`。
- validator 失败不能覆盖旧工程包，不能清 dirty。
- validator finding 必须符合 `32` 的错误码、报告 schema 和 Save/Open gate。
- Fixture A/B/C 和 negative fixture 有明确 golden 断言。
- Save/Open 事务状态矩阵可验证。
- Phase 1 初始化有 `readiness_run_001.md` 记录模板和状态判定口径。

阻塞信号：

- 只有 manifest，没有 `legacyObject.raw`。
- 只有 OCCT face / edge，没有旧图石对象证据。
- 保存两堆数据但没有 binding。
- 低置信字段没有进入 `unresolved_fields.json`。

关联 GAP：

- `GAP-DEV-001`
- `GAP-DEV-002`
- `GAP-SFL-003`
- `GAP-SFL-004`
- `GAP-SFL-005`

### E. STEP 选择 ID 工件

必须存在：

- `19_STEP选择ID稳定性Spike计划.md`
- `31_STEP选择ID实际运行记录模板与样本清单.md`

正式 Phase 1 前建议产出：

```text
step_selection_run_001.json
step_selection_run_002.json
step_selection_run_003.json
step_selection_copied_path.json
step_selection_restart.json
step_selection_diff.md
```

必须证明：

- 同一个 STEP 多轮导入后 face / edge 可匹配。
- `occtTransientIndex` 不能作为稳定 ID。
- 匹配失败时进入修复状态，不静默改绑。
- run JSON、diff、hash、OCCT 版本、样本路径和 decision 都有记录。

阻塞信号：

- 把 OCCT 遍历顺序当长期保存 ID。
- STEP 缺失时直接丢钢筋对象。
- topology 匹配失败后自动挂到相似面。

关联 GAP：

- `GAP-DEV-002`
- `GAP-TECH-003`

### F. 钢筋领域模型工件

必须存在：

- `09_钢筋领域模型草案.md`
- `13_Detail字段映射矩阵.md`
- `16_seg_steelbargroup字段地图初稿.md`

正式 Phase 1 允许先做内存模型骨架，但必须遵守：

- `RebarGroup` 能追回 `steelbargroup`。
- `Rebar` 能追回 `steelbar`。
- `RebarSegment` 能追回 `seg_steelbargroup`。
- `SteelData` 能追回 `steelData`。
- Detail `StbGroup / StbGeo / StbTable` 从同一份 `RebarModel` 派生。

阻塞信号：

- 纯 OCCT 曲线进入正式 `RebarModel`。
- Detail writer 重新生成一套编号。
- `StbTable` 和图上 `StbGroup` 不同源。

关联 GAP：

- `GAP-DEV-005`
- `GAP-SFL-003`
- `GAP-SFL-004`
- `GAP-SFL-005`
- `GAP-DRAW-004`

### G. Detail writer 工件

必须存在：

- `05_Detail工程图包证据.md`
- `13_Detail字段映射矩阵.md`
- `20_DetailWriter输出事务契约.md`

正式 Phase 1 可做 L0/L1 离线 writer spike：

```text
drawings/.tmp/run_*/
audit/drawing_writer_runs.jsonl
Detail.xml
Detail01.stl
```

必须证明：

- `DetailNN.stl` 是 XML，不是三角网格 STL。
- writer 接收只读快照。
- writer 失败不覆盖旧包。
- `StbGroup.rsdID` 与 `StbTable.rsdID` 同源。

不能宣称：

- 未经 AutoCAD 2020 + 旧插件导入，不得宣称工程图闭环完成。

关联 GAP：

- `GAP-DRAW-001`
- `GAP-DRAW-004`
- `GAP-DRAW-005`
- `GAP-IDA-005`
- `GAP-IDA-006`

## 首批工程目录建议

正式工程初始化时建议建立：

```text
src/
  app/
  ui/
  commands/
  geometry/
  project/
  rebar/
  drawing/
  evidence/
tests/
  unit/
  integration/
  fixtures/
fixtures/
  tsrebar/
docs/
  phase1/
licenses/
sbom/
```

说明：

- `ui/` 负责 Qt Widgets 界面。
- `commands/` 负责 `CommandId`、状态机、dirty 规则。
- `geometry/` 负责 OCCT / STEP / topology fingerprint。
- `project/` 负责新设计文件格式保存、读取、事务。
- `rebar/` 负责旧图石证据驱动的钢筋领域模型。
- `drawing/` 负责 Detail writer。
- `evidence/` 负责 evidence / gap / unresolved 字段校验。

禁止：

- 让 `geometry/` 反向定义钢筋业务字段。
- 让 `drawing/` 反向覆盖 `RebarModel`。
- 让 `ui/` 直接修改项目文件而绕过 command / transaction。

## 首批命令注册门禁

正式 Phase 1 至少注册这些命令族：

```text
Project.New
Project.OpenStep
Project.Save

View.FitAll
View.Pan
View.Zoom
View.Rotate
View.SectionX/Y/Z

Rebar.Create.Face
Rebar.Create.LineGroup
Rebar.Create.ArcGroup
Rebar.Edit.Move
Rebar.Group.Merge
Rebar.Group.Split
Rebar.Check.Overlap
Rebar.Check.MinDistance

Drawing.DefineSectionPlane
Drawing.DefineProjectPlane
Drawing.GeneratePackage
Drawing.GenerateBarSchedule
```

Open 缺口命令规则：

- 可以注册。
- 可以显示未实现。
- 可以记录 GAP。
- 不允许执行伪逻辑并写入正式工程包。

## UI Demo 与正式 Phase 1 的区别

### 只能叫 UI demo

满足任一条，就只能叫 UI demo：

- 只有窗口和按钮，没有 `CommandId`。
- 只有 STEP Viewer，没有新设计文件格式。
- 只有 OCCT 选择结果，没有 topology fingerprint 策略。
- 只有内存钢筋对象，没有 `legacyObject.raw + geometryRef + binding + evidence`。
- 只有 Detail XML writer，没有失败回滚和 ID 同源校验。
- 只复用了父目录工程格式或字段名。

### 可以叫正式 Phase 1 开工

必须同时满足：

- 复刻范围文档完整。
- Qt / OCCT 依赖门禁文件实际落地。
- CommandId 注册表可从 `17` 追溯。
- 新设计文件格式 fixture / validator / transaction 工件可创建。
- STEP 选择 ID spike 输入样本明确。
- 父目录源码参考边界明确。
- `99` 中 P0/P1 未关闭项仍保留关闭条件。

## Phase 1 不变量

这些不变量不能被后续实现破坏：

1. `.sfl` 不是新系统首期主保存格式。
2. 新格式必须保存 `legacyObject.raw + geometryRef + binding + evidence`。
3. OCCT 不产生旧图石钢筋业务规则。
4. Detail writer 不是钢筋数据真相源。
5. 父目录源码不是业务证据源。
6. validator 失败不能清 dirty，不能覆盖旧包。
7. 低置信字段不能自动晋级 confirmed。
8. AutoCAD L2 未通过前，工程图只能称为离线 writer spike。

## 进入开发的推荐顺序

```text
1. 创建依赖清单和 notice/SBOM 骨架。
2. 创建 Qt6 + OCCT 工程壳。
3. 注册一期 CommandId。
4. 接入 OCCT Viewer 和 STEP 导入。
5. 产出 Fixture A/B/C 与 negative fixture。
6. 实现 validators。
7. 实现 Project.Save / Project.Open 事务。
8. 跑 STEP topology diff。
9. 接入 Detail writer L0/L1。
10. 再推进面配筋、线配筋、弧形组等业务专项。
```

注意：

- 第 10 步之前可以做专项 spike。
- 但不能把专项 spike 变成“旧图石钢筋算法已复刻完成”的说法。

## 仍不能关闭的缺口

本文件不会关闭以下缺口：

- `GAP-DEV-001`：已补实际 fixture 包清单和 golden 断言，Fixture A/B/C、首批 negative fixture、validator 脚本、首批 validation reports、standalone Save/Open reports、首批 STEP diff reports 和 Qt6 应用 Save/Open / binding 修复契约已落地，但 Qt6 应用代码、应用级保存回滚测试和 binding 修复运行报告仍缺。
- `GAP-DEV-002`：已有真实 STEP 多轮导入 diff；`test.stp` 与 5 个 complex curve STEP 样本均通过，`step-shape-fingerprint/v1` 已消解复杂 edge/vertex raw Explorer 重复噪声；当前降为 P1 residual，新增工程 STEP 样本仍需继续跑同一 gate。
- `GAP-DEV-007`：已补 Phase 1 初始化 runbook 和 readiness 记录模板，但还没有工程初始化后的真实 readiness 打勾记录。
- `GAP-TECH-007`：已补依赖清单、notice、license、SBOM 和 `Build.DependencyGate` 模板，但还没有正式工程里的实际文件和运行记录。
- `GAP-DRAW-005`：还没有 writer L0/L1 实现和失败回滚测试。

## 当前结论

现在可以继续准备开发入口。

但正式进入 Phase 1 前，必须把本文件的工件清单从“文档要求”变成“工程内实际文件和测试”。

白话说：

```text
可以开始搭台子。
但台子要能验收，不能只是窗口亮起来。
```
