# 正式 Qt6 + OCCT 工程架构与首个实现切片

## 2026-06-11 路线状态

本文件冻结的是旧 VisualTS 1 比 1 路线阶段的首版 `app` 架构。
路线切换后，`app`、Qt6、OCCT、STEP 导入、AIS 显示、选择系统、
`.tsrebar` 和既有测试门禁继续保留。

但后续主线已经调整为：

```text
STEP-only + RebarSmart 钢筋生成逻辑证据 + 图石 Detail 包兼容导出
```

因此，本文中 `VisualTSRebarDomain / LegacyUiCommandMap / LegacyGeometryAdapter`
作为历史结构和局部兼容资产保留；新钢筋生成层应逐步改为 RebarSmart
证据驱动的自研 `RebarModel / DrawingModel / IGeometryEngine` 边界。
当前下一步以 `46` 和 `todo.csv` 的 `TODO-090 / DetailPackage 数据模型 P0`
为准，不回到旧 UI 全量 1 比 1 或 TODO-088。

## 目标

本文件冻结正式开发工程的第一版架构。

它回答四个问题：

```text
新项目放在哪里？
父目录已有开发实现怎么用？
Qt6 + OCCT 在新系统里承担什么职责？
第一刀实现到什么程度才算可验收？
```

本文件不是旧图石业务规则的替代证据。

旧图石功能真相仍以运行确认、IDA、SFL、Detail 包和本目录追溯矩阵为准。

## 历史路线总口径（已降级）

旧路线当时认为正确的口径：

```text
Qt6 / OCCT 替代旧商业底座能力。
VisualTSRebarDomain 按旧图石证据 1 比 1 复刻钢筋业务。
LegacyGeometryAdapter 把 OCCT 包装成旧系统熟悉的 EDGE / FACE / 曲线 / 实体语义。
```

错误路线：

```text
OCCT 能怎么生成钢筋
  -> 就按 OCCT 思路直接重写一套钢筋软件
```

一句话：

```text
OCCT 是几何发动机，不是钢筋业务规则来源。
父目录源码是零件库，不是本项目主工程。
```

## 新项目位置

正式新项目放在：

```text
【图石钢筋1比1复刻】/app
```

后续所有新增产品代码、测试、工程配置默认进入 `app`。

父目录：

```text
C:\Users\ghost\Desktop\reverse_engineering
```

只作为参考来源：

- 可借 Qt6 / OCCT / STEP / Viewer / CMake / 测试基础设施。
- 不把父目录钢筋算法当旧图石业务真相。
- 不在父目录继续推进正式产品代码。

## 总体架构

```text
Qt6 MainWindow / Dock / Dialog
  -> LegacyUiCommandMap
  -> CommandSession
  -> VisualTSRebarDomain
  -> LegacyGeometryAdapter
  -> OCCT Geometry API
  -> OCCT AIS Viewer
  -> SteelBar / SteelBarSegment / SteelBarGroup / SteelData
  -> .tsrebar 新设计文件
  -> Detail.xml + DetailXX.stl
  -> 后续钢筋 STP 导出
```

职责边界：

- `Qt6 UI`：新界面、按钮、参数窗口、状态栏、模型树、属性面板。
- `LegacyUiCommandMap`：旧按钮、旧命令字符串、CommandId、Dialog 的映射。
- `CommandSession`：一次命令的选择、参数、预览、应用、取消、失败回滚。
- `VisualTSRebarDomain`：旧路线下用于复刻旧图石 steelbar / steelbargroup 业务逻辑；路线切换后作为历史成果保留。
- `LegacyGeometryAdapter`：提供旧系统语义的几何接口；路线切换后作为历史 VisualTS 兼容工具和局部几何参考保留。
- `OCCT Geometry API`：实现长度、参数点、法向、求交、距离、裁剪、剖切、扫掠。
- `OCCT AIS`：显示、选择、高亮、旋转、缩放、平移。
- `.tsrebar`：新系统主设计文件格式，结合 SFL 证据和 OCCT 几何引用。
- `Detail writer`：继续服务旧 AutoCAD 插件的工程图包。

## 界面复刻口径

老图石界面审美不作为复刻目标。

旧路线要求 1 比 1 对齐的是：

- 功能入口。
- 菜单层级。
- 按钮名称。
- 参数字段。
- 默认值。
- 灰显和启用条件。
- 选择顺序。
- 状态提示。
- 命令结果。
- 工程 dirty / 刷新 / 出图状态。

不复刻的是：

- MFC / Codejock 旧皮肤。
- 老式拥挤布局。
- 低可读性的图标和按钮堆叠。
- 纯粹历史包袱造成的视觉丑陋。

新界面原则：

```text
P0/P1 支撑 STEP-only、DetailPackage、RebarSmart-style 生成器和兼容导出。
旧图石功能入口和 UI 细节降为 P2 / 历史专项。
```

## 推荐目录结构

```text
app/
  CMakeLists.txt
  src/
    app/
      main.cpp
      MainWindow.h
      MainWindow.cpp
    ui/
      MainTabs.h
      MainTabs.cpp
      ModelTreePanel.h
      ModelTreePanel.cpp
      PropertyPanel.h
      PropertyPanel.cpp
    command/
      CommandId.h
      LegacyUiCommandMap.h
      LegacyUiCommandMap.cpp
      CommandSession.h
      CommandSession.cpp
      CommandRegistry.h
      CommandRegistry.cpp
    geometry/
      legacy/
        LegacyEntityRef.h
        LegacyEdgeRef.h
        LegacyFaceRef.h
        LegacyGeometryAdapter.h
      occ/
        OccGeometryAdapter.h
        OccGeometryAdapter.cpp
        import/
          OcctStepImportService.h
          OcctStepImportService.cpp
          OcctImportedDocument.h
        selection/
          OcctImportedModelIndex.h
          SelectionRef.h
          SelectionService.h
          OccSelectionAdapter.h
          OccSelectionAdapter.cpp
    import/
      StepImportResult.h
      ImportedModelSummary.h
    presentation/
      occ/
        OcctViewerWidget.h
        OcctViewerWidget.cpp
        OccShapePresenter.h
        OccShapePresenter.cpp
    domain/
      rebar/
        SteelBar.h
        SteelBarSegment.h
        SteelBarGroup.h
        SteelData.h
      drawing/
      project/
    io/
      tsrebar/
      detail/
      step/
  tests/
    unit/
    integration/
```

硬边界：

```text
domain/rebar 不允许 include OCCT 头文件。
domain/rebar 不允许出现 TopoDS_ / BRepAdaptor_ / BRepAlgoAPI_ / AIS_。
```

允许 OCCT 出现的位置：

- `geometry/occ`
- `presentation/occ`
- `geometry/occ/import`
- `geometry/occ/selection`
- `io/step`
- 面向 OCCT 的测试辅助代码

## 父目录迁移策略

迁移原则：

```text
先迁基础设施。
再迁接口形状。
最后才评估业务代码。
钢筋生成业务代码默认不迁，除非能证明它只是 DTO / 序列化 / 测试夹具。
```

优先可迁方向：

- STEP / STP 导入。
- XCAF 装配分解。
- ImportedDocument / ImportedModelIndex。
- Viewer 初始化和 AIS 显示基础。
- face / edge / vertex 选择引用的稳定 ID 思路。
- CMake 找 Qt6 / OCCT 的经验。
- STEP probe / selection gate 的测试方式。
- Detail writer 的 XML 和包结构经验。

默认禁迁方向：

- `EdgeToRebarFactory`。
- `FaceRebarGenerator`。
- `PolylineRebarGenerator`。
- 直接从 `TopoDS_Edge` / `TopoDS_Face` 创建钢筋的业务实现。
- 以 OCCT 几何能力倒推钢筋规则的代码。

原因：

```text
这些代码更像“基于 OCCT 重写钢筋功能”。
本项目要的是“旧 VisualTS 业务逻辑复刻 + OCCT 兼容层”。
```

## 父目录专项审计结论

本轮已用 5 个 xhigh 只读子代理并行复盘父目录旧开发实现。

审计口径：

```text
只判断父目录代码能否作为新 app 的零件。
不把父目录代码当旧图石业务真相。
不允许子代理修改文件。
```

### STEP / STP 导入

迁移价值：

```text
4 / 5
```

可迁：

- `src/import/StepImportService.h/.cpp`：改名为 `OcctStepImportService`。
- `src/import/ImportedDocument.h`：改为 `OcctImportedDocument`，只在 OCCT 层使用。
- `src/import/ImportedModelIndex.h`：放入 `geometry/occ/selection`。
- `src/step/StepImportProbe.cpp`：可迁为 smoke probe。
- STEP 导入单测：保留缺失文件、扩展名、stable key、真实 STEP 样本断言。

改造点：

- 不能让 `domain/rebar` include `ImportedDocument` / `ImportedModelIndex`。
- `123.stp` 单位为 `METRE`，新工程必须明确 mm 归一化或长度单位策略。
- stable key 只承诺同一文件、同一导入逻辑下稳定；重新导出或拓扑变化后必须重新跑选择 ID gate。
- 大文件导入需要后续补耗时、内存、取消和错误诊断。

落点：

```text
app/src/geometry/occ/import/OcctStepImportService.*
app/src/geometry/occ/import/OcctImportedDocument.h
app/src/geometry/occ/selection/OcctImportedModelIndex.h
app/src/import/StepImportResult.h
```

### Viewer / AIS / 选择

迁移价值：

```text
4 / 5
```

可迁：

- `src/occ/SelectionRef.h`：stable selection string 思路可迁。
- `src/occ/SelectionService.h`：选择过滤、Replace / Append / Toggle 可迁。
- `src/occ/SelectionRayCandidateProvider.h`：透选候选可后置迁。
- `src/occ/SelectionDescriptionService.h`：选择面板描述可后置迁。
- `src/occ/ReferenceGeometryBinder.h`：适合承接 `SelectionRef -> LegacySelectionRef`。
- `src/occ/OcctViewerService.h`：先去掉 `rebar/SteelBarSegment.h` 依赖后再迁。

只抽取：

- AIS viewer 初始化。
- STEP 部件显示。
- `FitAll`。
- Body / Face / Edge / Vertex 选择模式。
- 鼠标 hover / click 到选择候选。
- `SelectionRef::toStableString()`。

禁止整体迁移：

- `src/ui/OcctViewerWidget.*`

原因：

```text
这个类已经混入钢筋创建、编辑、检查、项目文档、钢筋高亮和反向拾取。
新项目只能切片抽取几何显示/选择部分，不能把它当新 Viewer 基类。
```

必须剥离的关键词：

```text
RebarEditCommandService
RebarCreationCommandSession
RebarCheckService
ProjectRebarMutationService
RebarPresentation
rebarPickFromOwner
syncRelatedRebarSelectionForGeometrySelection
```

### UI / 命令

迁移价值：

```text
3.5 / 5
```

可迁：

- `src/ui/LegacyUiCommandMap.*`：命令证据表雏形可迁。
- `RebarContextCommandId`：右键/编辑命令枚举参考。
- `currentContextCommandModel()`：命令显示文本、enabled 状态、分组模型参考。
- `dispatchContextCommand()`：dispatch 表思路可迁，但目标必须改成 `CommandSession`。
- `CommandAvailabilityService`：启用/禁用条件计算思路可迁。
- `LegacyUiCommandMapTests`：`PARTIAL` / 不能虚假 `CONFIRMED_1TO1` 的门禁思路可迁。

不能照搬：

- 父目录 `MainWindow` 只有 4 个页签，缺 `工程图`。
- 父目录测试固定 18 个 QAction，低于 `17_一期按钮追溯与命令占位矩阵.md` 要求。
- `viewer_main.cpp` 存在 QAction 直连 viewer / service 的模式。
- 旧 demo 布局和英文调试按钮不能作为新 UI 基准。

新口径：

```text
UI 功能入口 1 比 1。
视觉现代化。
所有 QAction 必须进入 LegacyUiCommandMap -> CommandSession。
未实现命令返回 NotImplemented，不假装成功。
```

### 钢筋业务

迁移价值：

```text
DTO / 枚举 / 统计候选可谨慎迁。
钢筋生成主链禁止迁。
```

父目录钢筋实现性质：

```text
高风险混合体。
创建主链、EDGE/FACE 生成、面偏移、局部移动、碰撞、保护层、剖切修复
明显属于 OCCT 直接重写路线。
```

禁止直接迁移：

- `RebarCreationCommandService.h`
- `RebarCreationCommandSession.h`
- `ProjectRebarMutationService.h`
- `EdgeToRebarFactory.h`
- `FaceRebarGenerator.h`
- `PolylineRebarGenerator.h`
- `RebarFaceOffsetService.h`
- `RebarLocalMoveService.h`
- `RebarCollisionDetector.h`
- `RebarCoverGeometryValidator.h`
- `RebarSectionRepairService.h`

可作为候选、但必须证据校准：

- `SteelBarSegment.h`
- `SteelBar.h`
- `SteelBarGroup.h`
- `SteelData.h`
- `SteelJoint.h`
- `SteelStyle.h`
- `RebarStatistics.h`
- `SteelCatalog.h`
- 纯数学编辑服务，如 move / transform / trim / bend / hook 类工具。

迁移条件：

```text
字段名、默认值、长度/重量/分组口径必须逐项对齐 IDA、SFL 或旧图石运行确认。
父目录 NEW_SPEC golden 只能当静态回归夹具，不能当旧图石 1:1 证据。
```

`domain/rebar` 禁止关键词：

```text
TopoDS_, TopoDS::, TopAbs_, TopExp_, BRep, BRepAdaptor_,
BRepAlgoAPI_, BRepBuilderAPI_, BRep_Tool, BRepGProp, BRepClass3d,
Geom_, Geom2d_, GeomAbs_, GCPnts_, gp_, AIS_, Standard_, Handle(Geom
```

### 导出 / 工程 IO

迁移价值：

```text
协议、IO 模式、验证资产：4 / 5
旧代码直接搬迁：2 / 5
```

可迁候选：

- `DetailPackageNaming.h`：`Detail01` / `Detail01.stl` 命名规则。
- `DetailXmlWriter.h`：UTF-8 BOM、CRLF、`QSaveFile` 原子写入。
- `DetailStlXmlWriter.h`：旧插件 XML 结构 DTO。
- `PluginPackageExporter.h`：包布局和导出结果模式，清目录逻辑需安全降级。
- `ProjectSerializer.h` / `ProjectDeserializer.h`：schema、源 STP 指纹、原子保存经验。
- `StepRebarExporter.*`：只迁 OCCT STEP 写出后端，入口改成新 `RebarExportModel`。
- `RebarExportDiagnostics.h`：统一导出诊断模式。
- XML normalizer / golden diff / STEP exporter tests：迁为验证基础设施。

不可迁候选：

- 旧 ACIS / MFC 保存链。
- C# `SteelBar.cs` 链表/指针式二进制读写。
- `ProjectRebarMutationService`、`DrawingViewGenerator`、`RebarDrawingGenerator`、HLR / Section 全链路。
- SAT 导出。
- 旧 `sexportbar` 文本完整复刻。
- STEP 文件字节级 diff。

源格式和导出关系：

```text
.tsrebar 是源事实。
Detail.xml + DetailXX.stl 是派生兼容包。
钢筋 STP 也是派生产物。
二者都由规范化 RebarExportModel 导出。
```

`123.stp` 用途：

```text
登记为旧图石真实导出的几何 golden。
只做几何指标对照，不做 STEP 文本 diff。
初期比对：可读、非空、bbox、实体数量级、总长度/直径分布。
```

## 首个实现切片

第一刀命名：

```text
M1-App-001
```

目标：

```text
新工程能启动。
能导入并显示 123.stp。
能注册旧图石一期命令入口。
能证明 OCCT 没有污染钢筋领域层。
```

范围内：

- 创建 `app` 工程。
- Qt6 主窗口。
- 顶部页签：开始、显示、钢筋、查询、工程图。
- 状态栏。
- 模型树面板。
- 属性面板。
- OCCT viewer。
- STP 导入服务。
- 导入 `123.stp` 后显示模型。
- 统计 solids / faces / edges / vertices。
- 注册 `CommandId` 和 `LegacyUiCommandMap`。
- 未实现命令统一返回 `NotImplemented` 状态，不假装成功。
- 建立 `domain/rebar` 的 OCCT include 扫描测试。

范围外：

- 不实现真实钢筋生成。
- 不实现面配筋、线配筋、弧形组。
- 不实现 Detail 包真实导出。
- 不实现 `.tsrebar` 完整保存打开。
- 不导入 `.sfl` 作为新系统主格式。
- 不把父目录 RebarCreation 体系迁进新工程。

## M1-App-001 验收标准

必须通过：

```text
CMake configure 成功。
CMake build 成功。
应用可启动。
应用能打开 123.stp。
导入统计能看到 754 solids、3016 faces 量级。
主界面存在五个一期页签。
命令注册表包含“输出钢筋”等已知入口。
domain/rebar 扫描不到 TopoDS_ / BRepAdaptor_ / BRepAlgoAPI_ / AIS_。
Phase1 readiness gate 仍然通过。
```

推荐验证命令：

```powershell
py .\【图石钢筋1比1复刻】\tools\phase1_readiness_gate\check_phase1_readiness.py --strict
```

新工程验证命令在 `app` 初始化后补入本文和运行日志。

## 控制合同

Primary Setpoint：

```text
在本目录创建干净 Qt6 + OCCT 新工程，跑通 123.stp 导入显示，同时保持钢筋领域层不依赖 OCCT。
```

Acceptance：

```text
构建通过、应用启动、123.stp 导入显示、统计正确、命令注册存在、OCCT 边界扫描通过。
```

Guardrail Metrics：

- 父目录源码只能单向参考，不在父目录继续产品开发。
- `domain/rebar` 不出现 OCCT 类型。
- 未实现命令不能返回成功。
- 旧图石业务缺口不能用父目录实现直接关闭。

Sampling Plan：

- 每个实现切片运行构建。
- 每个涉及选择或 STEP 导入的切片用 `123.stp` 复测。
- 每个钢筋业务切片追加 IDA / 运行确认 / SFL / Detail 证据。

Rollback Trigger：

- 如果 `domain/rebar` 出现 OCCT 依赖，停止迁移并回退该实现。
- 如果父目录钢筋生成代码被直接搬入新项目，停止并重新走 LegacyGeometryAdapter 设计。
- 如果新项目无法独立构建，停止继续扩功能，先修工程骨架。

Boundary：

- 本轮允许新增 `【图石钢筋1比1复刻】/app`。
- 本轮允许新增或更新本目录文档。
- 本轮不修改父目录旧工程源码。

Risks：

- 父目录 Viewer 文件可能过大且混入钢筋业务，需要拆最小切片。
- STEP 导入能跑不代表选择语义已完全稳定，需要继续跑 selection gate。
- UI 功能入口可先占位，但参数窗口和业务行为必须后续逐项由证据闭合。

## 后续开发顺序

推荐顺序：

1. 并行复盘父目录可迁实现。已完成。
2. 汇总可迁 / 禁迁清单到本文。已完成。
3. 创建 `app` 工程骨架。
4. 迁入最小 STEP 导入服务。
5. 迁入或重写最小 OCCT viewer。
6. 注册一期命令入口。
7. 加 OCCT 边界扫描测试。
8. 用 `123.stp` 跑真实导入显示。
9. 再进入 LegacyGeometryAdapter 第一批接口。
10. 最后才开始钢筋业务命令专项。

## 与其他文档关系

- `01_功能操作矩阵.md`：决定旧图石有哪些功能入口。
- `02_界面窗口参数矩阵.md`：决定参数窗口字段。
- `06_技术路线与替代方案.md`：决定商业库替代边界。
- `08_开发命令契约.md`：决定命令状态和返回契约。
- `17_一期按钮追溯与命令占位矩阵.md`：决定一期按钮是否可注册。
- `18_新设计文件格式替代SFL策略.md`：决定 `.tsrebar` 方向。
- `23_父目录源码参考边界与路线纠偏.md`：决定父目录参考边界。
- `31_STEP选择ID实际运行记录模板与样本清单.md`：决定 STEP 选择 ID 验证方式。
- `35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`：决定 UI 和兼容层总原则。

## 当前状态

本文已冻结正式工程入口设计。

并行审计已完成，审计对象包括：

- STEP/STP 导入。
- Viewer / AIS / 选择。
- UI / 命令。
- 钢筋业务路线风险。
- 导出 / 工程 IO / Detail / STEP rebar export。

当前结论：

```text
父目录可作为 Qt6 / OCCT / STEP / Viewer / IO 零件库。
父目录钢筋生成主链属于 OCCT 直接重写路线，禁止作为业务真相迁移。
M1-App-001 app 工程初始化已通过。
```

M1-App-001 实际结果：

```text
CTest: 4 / 4 passed
app_smoke: passed
domain_rebar_boundary: passed
step_import_probe_123: passed
123.stp: 754 solids / 3016 faces / 9048 edges / 18096 vertices / METRE
readiness gate: M1-Formal-Ready, 78 / 78 pass
```

运行报告：

```text
docs/phase1/app_build_reports/m1_app_001_run_001.md
```
