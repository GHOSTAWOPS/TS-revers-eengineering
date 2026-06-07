# Phase 1 Readiness Gate 实际运行记录

## 目标

本文件记录 `Phase1.ReadinessGate` 从人工清单推进到可运行检查器的状态。

它回答：

```text
M1-Formal 现在到底能不能放行？
哪些证据已经存在？
哪些证据缺失会阻止正式进入 Qt6 + OCCT 开发？
这个判断能不能重复运行？
```

本文件新增开发证据：

```text
E-DEV-019
  -> Phase 1 readiness gate 脚本、测试和运行报告
  -> 来源：
     tools/phase1_readiness_gate/
     docs/phase1/readiness_gate_reports/
     34_Phase1ReadinessGate实际运行记录.md

E-DEV-022
  -> Qt6 C++ runtime gate、测试和真实运行报告
  -> 来源：
     tools/qt6_runtime_gate/
     docs/phase1/app_save_open_reports/

E-DEV-025
  -> M1-App-001 正式 app 工程骨架、CTest 和 123.stp 导入统计报告
  -> 来源：
     app/
     docs/phase1/app_build_reports/m1_app_001_run_001.md
     docs/phase1/app_build_reports/m1_app_001_run_001.json

E-DEV-026
  -> M1-App-002 最小 AIS Viewer、STEP 显示和 viewer smoke 报告
  -> 来源：
     app/
     38_M1-App-002最小AISViewer显示实现记录.md
     docs/phase1/app_build_reports/m1_app_002_run_001.md
     docs/phase1/app_build_reports/m1_app_002_run_001.json

E-DEV-027
  -> M1-App-003 face / edge / vertex 选择、selection-v1 稳定引用和 viewer selection 报告
  -> 来源：
     app/
     39_M1-App-003选择系统实现记录.md
     docs/phase1/app_build_reports/m1_app_003_run_001.md
     docs/phase1/app_build_reports/m1_app_003_run_001.json

E-DEV-028
  -> M1-App-004 LegacyGeometryAdapter P0、EDGE / FACE 几何摘要查询报告
  -> 来源：
     app/
     40_M1-App-004LegacyGeometryAdapterP0实现记录.md
     docs/phase1/app_build_reports/m1_app_004_run_001.md
     docs/phase1/app_build_reports/m1_app_004_run_001.json

E-DEV-029
  -> M1-App-005 LegacyGeometryAdapter P1、EDGE / FACE bbox、采样、boundary loop、fingerprint 和诊断报告
  -> 来源：
     app/
     41_M1-App-005LegacyGeometryAdapterP1实现记录.md
     docs/phase1/app_build_reports/m1_app_005_run_001.md
     docs/phase1/app_build_reports/m1_app_005_run_001.json

E-DEV-030
  -> M1-App-006 LegacyGeometryAdapter P2A、edge 切向、selection ref 距离和最近点报告
  -> 来源：
     app/
     42_M1-App-006LegacyGeometryAdapterP2A实现记录.md
     docs/phase1/app_build_reports/m1_app_006_run_001.md
     docs/phase1/app_build_reports/m1_app_006_run_001.json

E-DEV-031
  -> M1-App-007 LegacyGeometryAdapter P2B、face boundary edge stableId 和 edge-face 接触/重叠代表点报告
  -> 来源：
     app/
     43_M1-App-007LegacyGeometryAdapterP2B实现记录.md
     docs/phase1/app_build_reports/m1_app_007_run_001.md
     docs/phase1/app_build_reports/m1_app_007_run_001.json

E-DEV-032
  -> M1-App-008 LegacyGeometryAdapter P2C、edge 参数区间、子段长度和子段采样报告
  -> 来源：
     app/
     44_M1-App-008LegacyGeometryAdapterP2C实现记录.md
     docs/phase1/app_build_reports/m1_app_008_run_001.md
     docs/phase1/app_build_reports/m1_app_008_run_001.json

E-DEV-033
  -> M1-App-009 LegacyGeometryAdapter P3A、edge split 语义和前后子段报告
  -> 来源：
     app/
     45_M1-App-009LegacyGeometryAdapterP3A实现记录.md
     docs/phase1/app_build_reports/m1_app_009_run_001.md
     docs/phase1/app_build_reports/m1_app_009_run_001.json

E-DEV-034
  -> M1-App-010 LegacyGeometryAdapter P3B、edge point projection 和 split by projected point 报告
  -> 来源：
     app/
     47_M1-App-010LegacyGeometryAdapterP3B实现记录.md
     docs/phase1/app_build_reports/m1_app_010_run_001.md
     docs/phase1/app_build_reports/m1_app_010_run_001.json

E-DEV-035
  -> M1-App-011 LegacyGeometryAdapter P3C、endpoint inward trim summary 报告
  -> 来源：
     app/
     48_M1-App-011LegacyGeometryAdapterP3C实现记录.md
     docs/phase1/app_build_reports/m1_app_011_run_001.md
     docs/phase1/app_build_reports/m1_app_011_run_001.json

E-DEV-036
  -> M1-App-012 LegacyGeometryAdapter P3D、point to edge group minimum distance summary 报告
  -> 来源：
     app/
     49_M1-App-012LegacyGeometryAdapterP3D实现记录.md
     docs/phase1/app_build_reports/m1_app_012_run_001.md
     docs/phase1/app_build_reports/m1_app_012_run_001.json

E-DEV-037
  -> M1-App-013 LegacyGeometryAdapter P3E、point list spline rebuild summary 报告
  -> 来源：
     app/
     50_M1-App-013LegacyGeometryAdapterP3E实现记录.md
     docs/phase1/app_build_reports/m1_app_013_run_001.md
     docs/phase1/app_build_reports/m1_app_013_run_001.json

E-DEV-038
  -> M1-App-014 LegacyWireChain、edge refs to wire chain summary 报告
  -> 来源：
     app/
     51_M1-App-014LegacyWireChain实现记录.md
     docs/phase1/app_build_reports/m1_app_014_run_001.md
     docs/phase1/app_build_reports/m1_app_014_run_001.json

E-DEV-039
  -> M1-App-015 LegacyGeometryAdapter offset curve preview summary 报告
  -> 来源：
     app/
     52_M1-App-015LegacyGeometryAdapterOffsetSpike实现记录.md
     docs/phase1/app_build_reports/m1_app_015_run_001.md
     docs/phase1/app_build_reports/m1_app_015_run_001.json

E-DEV-040
  -> M1-App-016 LegacyGeometryAdapter section / 剖切 preview summary 报告
  -> 来源：
     app/
     53_M1-App-016LegacyGeometryAdapterSectionSpike实现记录.md
     docs/phase1/app_build_reports/m1_app_016_run_001.md
     docs/phase1/app_build_reports/m1_app_016_run_001.json

E-DEV-041
  -> M1-App-017 LegacyGeometryAdapter sweep / 扫掠 preview summary 报告
  -> 来源：
     app/
     54_M1-App-017LegacyGeometryAdapterSweepBoundary实现记录.md
     docs/phase1/app_build_reports/m1_app_017_run_001.md
     docs/phase1/app_build_reports/m1_app_017_run_001.json

E-DEV-042
  -> M1-App-018 钢筋领域模型冻结 P1 报告
  -> 来源：
     app/
     55_M1-App-018RebarDomainModelFreezeP1实现记录.md
     docs/phase1/app_build_reports/m1_app_018_run_001.md
     docs/phase1/app_build_reports/m1_app_018_run_001.json

E-DEV-043
  -> M1-App-019 旧命令契约绑定 P1 报告
  -> 来源：
     app/
     56_M1-App-019LegacyCommandContractP1实现记录.md
     docs/phase1/app_build_reports/m1_app_019_run_001.md
     docs/phase1/app_build_reports/m1_app_019_run_001.json

E-DEV-044
  -> M1-App-020 旧线筋 / 弧筋创建算法 P0 报告
  -> 来源：
     app/
     58_M1-App-020旧线筋弧筋创建算法P0实现记录.md
     docs/phase1/app_build_reports/m1_app_020_run_001.md

E-DEV-045
  -> M1-App-021 AIS 钢筋显示映射 P0 报告
  -> 来源：
     app/
     59_M1-App-021AIS钢筋显示映射实现记录.md
     docs/phase1/app_build_reports/m1_app_021_run_001.md
     docs/phase1/app_build_reports/m1_app_021_run_001.json

E-DEV-046
  -> M1-App-022 新设计文件格式 runtime P1 报告
  -> 来源：
     app/
     60_M1-App-022新设计文件格式RuntimeP1实现记录.md
     docs/phase1/app_build_reports/m1_app_022_run_001.md
     docs/phase1/app_build_reports/m1_app_022_run_001.json
     docs/phase1/validator_reports/runtime_m1_app_022/build_validation_001.json

E-DEV-047
  -> M1-App-023 DetailWriter P1 报告
  -> 来源：
     app/
     61_M1-App-023DetailWriterP1实现记录.md
     docs/phase1/app_build_reports/m1_app_023_run_001.md
     docs/phase1/app_build_reports/m1_app_023_run_001.json

E-DEV-048
  -> TODO-025 / M1-App-024 旧图石输出钢筋 STP 样本入库验证报告
  -> 来源：
     62_TODO-025旧图石输出钢筋STP样本入库验证记录.md
     docs/phase1/step_selection/tushi_rebar_123_stp/witness_todo_025.md
     docs/phase1/step_selection/tushi_rebar_123_stp/witness_todo_025.json
     docs/phase1/app_build_reports/m1_app_024_run_001.md
     docs/phase1/app_build_reports/m1_app_024_run_001.json

E-DEV-049
  -> TODO-027 / M2-UI-001 旧 UI 功能入口 P1 报告
  -> 来源：
     app/
     63_M2-UI-001旧UI功能入口P1实现记录.md
     docs/phase1/app_build_reports/m2_ui_001_run_001.md
     docs/phase1/app_build_reports/m2_ui_001_run_001.json

E-DEV-050
  -> TODO-028 / M2-Gate-001 CSE readiness gate 扩展报告
  -> 来源：
     tools/phase1_readiness_gate/
     64_M2-Gate-001CSEReadinessGate扩展实现记录.md
     docs/phase1/readiness_gate_reports/readiness_gate_run_002.json
     docs/phase1/readiness_gate_reports/readiness_gate_run_002.md
     docs/phase1/app_build_reports/m2_gate_001_run_001.md
     docs/phase1/app_build_reports/m2_gate_001_run_001.json
```

注意：

- `E-DEV-019` 证明 readiness gate 已经可运行并产出报告。
- `E-DEV-022` 证明 Qt6 C++ runtime 已实际创建 `QApplication`、GUI 命令对象，并跑通 Save/Open 与 binding repair gate。
- 当前报告结论是 `M1-Formal-Ready`。
- 这只代表开发入口门禁通过，不代表旧图石全部钢筋业务已经复刻完成。

## 工件

```text
tools/phase1_readiness_gate/check_phase1_readiness.py
tools/phase1_readiness_gate/test_phase1_readiness_gate.py
docs/phase1/readiness_gate_reports/readiness_gate_run_001.json
docs/phase1/readiness_gate_reports/readiness_gate_run_001.md
docs/phase1/readiness_gate_reports/readiness_gate_run_002.json
docs/phase1/readiness_gate_reports/readiness_gate_run_002.md
```

## 当前运行结果

```text
Decision: M1-Formal-Ready
M1-Formal allowed: yes
Checks: 84 / 84 pass
Errors: 0
Warnings: 0
Blocker GAPs: none
```

这说明：

- 文档、fixture、validator、standalone Save/Open、seed STEP selection 已经具备第一批证据。
- 依赖合规文件、dependency probe 和 dependency gate 已落地，dependency gate 当前为 `pass`。
- Qt6 C++ runtime gate 已落地并通过，`app_save_open_run_001.json` 决策为 `qt6-app-pass`。
- Binding repair 真实 runtime 报告已落地并通过，`binding_repair_run_001.json` 决策为 `qt6-binding-repair-pass`。
- 正式 `app` 工程骨架已落地并通过 M1-App-001：CTest 4/4 pass，`123.stp` 导入统计为 754 solids / 3016 faces / METRE。
- 最小 AIS Viewer 已落地并通过 M1-App-002：CTest 5/5 pass，`tsrebar_app --smoke-display-step ..\..\123.stp` 返回 0。
- 选择系统已落地并通过 M1-App-003：默认 CTest 7/7 pass，`123.stp` 的 `selection-v1` face / edge / vertex 引用可生成和解析；真实 viewer selection 作为手动 OpenGL smoke 保留，避免日常测试反复弹窗。
- `LegacyGeometryAdapter P0` 已落地并通过 M1-App-004：默认 CTest 8/8 pass，可从 `LegacySelectionRef` 查询 EDGE 长度/端点/曲线类型和 FACE 面积/法向/边界数。
- `LegacyGeometryAdapter P1` 已落地并通过 M1-App-005：默认 CTest 8/8 pass，可查询 EDGE bbox/参数点/采样点/fingerprint，以及 FACE bbox/boundary loops/fingerprint，并补齐 wrong type/missing part/out-of-range 诊断。
- `LegacyGeometryAdapter P2A` 已落地并通过 M1-App-006：默认 CTest 8/8 pass，可查询 EDGE 参数点切向、selection ref 距离和最近点对；真实 `123.stp` 上 edge 自距为 0、edge-face 距离非负。
- `LegacyGeometryAdapter P2B` 已落地并通过 M1-App-007：默认 CTest 8/8 pass，可从 face boundary loop 取得 edge stableId，并查询 edge-face 接触/重叠代表点和 edge 参数；该阶段不是完整布尔 section 或裁剪。
- `LegacyGeometryAdapter P2C` 已落地并通过 M1-App-008：默认 CTest 8/8 pass，可从 edge 参数区间查询子段长度、起止点、bbox 和采样点；该阶段不是 topology trim、offset 或扫掠。
- `LegacyGeometryAdapter P3A` 已落地并通过 M1-App-009：默认 CTest 8/8 pass，可按内部参数把 edge 拆成前后两个 `LegacyCurveInterval`；该阶段不是 topology split 或 spline 重建。
- `LegacyGeometryAdapter P3B` 已落地并通过 M1-App-010：默认 CTest 8/8 pass，可把输入点投影到 edge 参数并按投影点拆成前后两个 `LegacyCurveInterval`；该阶段不是 topology split、trim 或钢筋生成。
- `LegacyGeometryAdapter P3C` 已落地并通过 M1-App-011：默认 CTest 8/8 pass，可按旧 `delta=-0.03,endFlag=0/1` 输出端点 inward trim 后的 `LegacyCurveInterval`；该阶段不是 topology trim、curve mutation 或钢筋生成。
- `LegacyGeometryAdapter P3D` 已落地并通过 M1-App-012：默认 CTest 8/8 pass，可对输入点和候选 edge refs 返回组内最小距离、nearest stableId 和 `0.002` 阈值命中标记；该阶段不是旧 group 链表完整复刻或钢筋生成。
- `LegacyGeometryAdapter P3E` 已落地并通过 M1-App-013：默认 CTest 8/8 pass，可把点列重建为 BSpline legacy summary，记录输入点数、采样数、`max(5,length*50)` 建议值、长度、bbox 和失败原因；该阶段不是旧 ACIS `api_curve_spline` topology mutation 或钢筋生成。
- `LegacyWireChain` 已落地并通过 M1-App-014：默认 CTest 8/8 pass，可把 edge refs 构造成 ordered wire chain summary，并记录总长度、起止点、bbox、connected、closed 和断链诊断；该阶段不是旧 `ENTITY_LIST` 对象完整复刻、OCCT wire mutation 或钢筋生成。
- `LegacyGeometryAdapter offset preview` 已落地并通过 M1-App-015：默认 CTest 8/8 pass，可把 edge ref 生成 offset curve preview summary，并记录 samplePoints、length、bbox、sourceCurveKind、sourceLength、offsettable 和失败诊断；该阶段不是完整 ACIS offset 等价、OCCT topology mutation 或钢筋生成。
- `LegacyGeometryAdapter section preview` 已落地并通过 M1-App-016：默认 CTest 8/8 pass，可把 face ref + legacy plane 生成 section preview summary，并补充 far plane empty、invalid plane、wrong type/missing ref 和 edge-face empty intersection set 覆盖；该阶段不是完整 ACIS section 等价、OCCT topology mutation、剖切面 UI 或钢筋生成。
- `LegacyGeometryAdapter sweep preview` 已落地并通过 M1-App-017：默认 CTest 8/8 pass，可把 edge ref + circular radius 生成 sweep / pipe preview summary，并记录 pathLength、samplePoints、bbox、shape face/edge/vertex count、zero/non-finite radius 和 wrong type/missing ref 诊断；该阶段不是完整 ACIS sweep 等价、OCCT topology mutation、钢筋实体持久化或钢筋生成。
- `domain/rebar` 钢筋领域模型 P1 已落地并通过 M1-App-018：默认 CTest 9/9 pass，可用 `SteelData / SteelBarGroup / SteelBar / SteelBarSegment` 承载 Detail 关键字段、group -> bar -> segment 引用、legacy raw、geometryRef、binding、evidence 和 unresolved 字段；该阶段不是钢筋创建算法、Detail writer 接入、UI handler 或旧 SFL 全字段语义闭合。
- `旧命令契约绑定 P1` 已落地并通过 M1-App-019：默认 CTest 9/9 pass，可查询和注册 `Rebar.Create.LineGroup / Rebar.Create.ArcGroup / RebarGroup.TrimByLine / RebarGroup.TrimByFace`，右键裁剪命令只作为 `ContextMenu` 占位，不渲染到 Ribbon；该阶段不是线筋 / 弧筋创建算法或裁剪业务算法。
- `TODO-020 IDA 旧线筋 / 弧筋链补证据` 已落地：IDA MCP 成功打开 `VisualTS.exe.i64`，补证 `sub_1404DE720 / sub_1404DE110 -> sub_1404D10C0 -> sub_140451730 -> sub_1405D5670 -> sub_1405BD0C0 / sub_1405C7260 / sub_1405E49D0`，形成 `E-IDA-022`；该阶段不是线筋 / 弧筋创建算法，也不是完整旧 UI/golden 闭合。
- `旧线筋 / 弧筋创建算法 P0` 已落地并通过 M1-App-020：默认 CTest 10/10 pass，可通过 `RebarGroupCreator` 把 legacy edge ref / old parameters 输出到 `SteelData -> SteelBarGroup -> SteelBar -> SteelBarSegment`，并显式调用 `SegmentCurveNormalizer P0` 接口；该阶段不是 AIS 显示、Detail writer、完整旧 UI/golden 或裁剪编辑算法。
- `AIS 钢筋显示映射 P0` 已落地并通过 M1-App-021：默认 CTest 11/11 pass，可通过 `RebarAisPresentationAdapter` 把 domain `SteelBarGroup / SteelBarSegment` 映射成 presentation/occ 层 `AIS_Shape`；该阶段不是旧 HOOPS 显示样式 1:1、UI handler、Detail writer 或新设计文件 runtime。
- `新设计文件格式 runtime P1` 已落地并通过 M1-App-022：默认 CTest 12/12 pass，可通过 `TsRebarProjectRuntime` 保存 / 读取 STEP 来源、selection-v1 refs、rebar groups、legacyObject.raw、geometryRef、binding、evidence 和 unresolved 字段；runtime 导出包外部 validator 为 warning-only / 0 error；该阶段不是完整 UI Save/Open、旧 SFL 兼容或 Detail writer。
- `DetailWriter P1` 已落地并通过 M1-App-023：默认 CTest 13/13 pass，可通过 `DetailWriter` 把 domain `SteelData / SteelBarGroup / SteelBar / SteelBarSegment` 输出为 `Detail.xml + Detail01.stl` 首批字段；安装失败会恢复旧 Detail 包；MaterialTable 质量公式 deferred；该阶段不是 AutoCAD L2 动态导入、完整工程图或完整下料统计公式。
- `TODO-025 / M1-App-024` 旧图石输出钢筋 STP 样本入库验证已完成：`123.stp` 已固定为 `tushi_rebar_123_stp` 几何 witness，当前 import probe 为 754 solids / 3016 faces / 9048 raw edges / 18096 raw vertices / METRE，既有 5 轮 STEP selection gate 为 pass；该阶段不是旧钢筋创建算法、旧参数、统计公式、Detail 正确性或新系统可生成同样几何的证明。
- `TODO-027 / M2-UI-001` 旧 UI 功能入口 P1 已落地并通过：默认 CTest 13/13 pass，`tsrebar_app --smoke` 校验 Ribbon QAction 追溯 metadata，`LegacyUiCommandMap` 覆盖 `17` 的一期入口，ContextMenu 命令不渲染到 Ribbon；该阶段不是旧弹窗字段、状态栏提示、钢筋业务算法或 golden。
- `TODO-028 / M2-Gate-001` CSE readiness gate 扩展已落地并通过：新增 RouteGuardrail，当前 readiness gate 为 84/84 pass、0 error、0 warning；新增检查覆盖 domain/rebar OCCT/AIS 泄漏、保护层泄漏、父目录 rebar 业务引用、todo 状态和 done 节点报告存在性；该阶段不是旧业务算法或输出结果 1:1 证明。
- `TODO-029 / M2-Edit-001` Rebar.Edit.Move / 钢筋移动 P0 已落地并通过：IDA MCP 已补证 `barmove -> Input_Choice copyFlag=0 -> translate_transf` 移动链，domain/rebar 新增 `RebarEditMoveService`；当前 CTest 14/14 pass，readiness gate 84/84 pass；该阶段不是旧 ACIS topology mutation、dirty/undo 或 golden 证明。
- `TODO-032 / M2-Edit-002` Rebar.Edit.Copy / 钢筋拷贝 P0 已落地并通过：IDA MCP 已补证 `scopy -> Input_Choice copyFlag=1 -> sub_1405989C0 -> sub_1405AA5D0` 拷贝链，domain/rebar 新增 `RebarEditCopyService`；当前 CTest 15/15 pass，readiness gate 84/84 pass；该阶段不是旧 ACIS topology clone、旧编号、dirty/undo 或 golden 证明。
- `TODO-030 / M2-Stats-001` 钢筋统计 / 下料表 P0 已落地并通过：IDA MCP 已补证 Detail / XML writer 侧 `StbTable / MaterialTable` 写出链，domain/rebar 新增 `RebarScheduleService`，DetailWriter 改为消费同一 schedule service；当前 CTest 16/16 pass，strict readiness gate 84/84 pass，xhigh 复审 `allow_commit`；该阶段不是完整下料合并规则、旧 `singleMass` 来源、`Volume722` ACIS 等价、AutoCAD L2 或 golden 证明。
- `TODO-031 / M2-Drawing-001` DetailWriter 多图纸 P0 已落地并通过：IDA MCP 已补证 `sub_140600AA0 -> sub_140635A80` 的旧 `Detail01..Detail09 / Detail10+` 命名规则，DetailWriter 可按多个 `DrawingView` 输出多张 `DetailNN.stl`，并覆盖多文件 L0/L1 校验、成功清理旧多余 DetailNN、失败恢复旧包；当前 CTest 16/16 pass，strict readiness gate 84/84 pass，保护层 OCCT/AIS 泄漏扫描无匹配；该阶段不是完整工程图、剖切线、隐藏线、填充线、AutoCAD L2 或 golden 证明。
- `TODO-033 / M2-Drawing-002` AutoCAD L2 导入验证 P0 已落地验证准备和阻塞记录：新增 `detail_l2_fixture_probe`，用正式 `DetailWriter` 生成 `Detail.xml + Detail01.stl + Detail02.stl + Detail03.stl` 三图纸包并记录 hash；旧 `FDrawing.arx / FDrawingObj.dbx` 文件存在且 hash 已记录；当前本机未发现 `acad.exe / accoreconsole.exe`，所以 AutoCAD L2 自动导入未运行；当前 CTest 17/17 pass，strict readiness gate 84/84 pass，保护层 OCCT/AIS 泄漏扫描无匹配；该阶段不是 AutoCAD L2 通过、完整工程图、剖切线、隐藏线、填充线或 golden 证明。
- `TODO-034 / M2-Drawing-003` Detail复杂字段静态证据 P0 已落地：从旧 `Detail01.stl` 样例整理 `continue-line / hidden-line / central-line / section-line / hatch-line / Others / steeljoint-line` 容器、section Line/Arc、pointStb 和 FaceEdge 字段；IDA MCP 复核 `FDrawing.arx` 中 `CViewInfo / CWSNLineDim / CWSNPointDim / CWSNSteelBarTable / CWSNMaterialTable` 等旧插件对象符号，并明确 XML 节点名主要来自旧样例而不是 FDrawing 明文字段；该阶段不是 AutoCAD L2 通过、旧插件容忍度、完整工程图、剖切线、隐藏线、填充线、接头线或 golden 证明。
- `TODO-035 / M2-Drawing-004` DetailWriter复杂字段骨架 P0 已落地并通过：正式 app `DetailWriter` 输出 `PartDetailDrawing` 复杂容器骨架、`General-Info` 首批扩展默认字段和 `pointStb StbGeo shapeType=C` 字段骨架；当前 CTest 17/17 pass，strict readiness gate 84/84 pass，保护层 OCCT/AIS 泄漏扫描无匹配，xhigh 只读 review 为 `allow_commit`；该阶段不是 AutoCAD L2 通过、旧插件接受新包、`CompanyName` 旧默认值、完整工程图、FaceEdge、ZValue、剖切线、隐藏线、填充线、接头线或 golden 证明。
- Detail writer L0/L1 离线 gate 已落地并通过，不再作为当前 M1 阻塞。
- `GAP-DEV-001/GAP-DEV-007` 不再阻塞进入 Qt6 + OCCT 开发入口，但旧图石业务复刻缺口仍按功能专项继续闭合。

## 当前失败项

readiness gate 当前没有 error：

```text
none
```

## 门禁边界

本 gate 只检查证据工件存在性和关键决策字段。

它不会：

- 替代完整 Qt6 + OCCT 产品工程。
- 替代旧图石钢筋生成、编辑、统计等业务算法复刻。
- 替代 Detail writer AutoCAD L2 动态导入验证。
- 用父目录源码关闭旧图石业务缺口。

它会：

- 阻止缺失关键报告时误写 `M1-Formal-Ready`。
- 阻止 dependency gate 或 Qt6 application gate 未通过时误写 `M1-Formal-Ready`。
- 固化当前 `M1-Formal-Ready` 的证据路径。
- 给后续工程初始化提供可重复检查入口。

## 使用方式

生成报告：

```powershell
& 'C:\Users\ghost\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' `
  'C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\tools\phase1_readiness_gate\check_phase1_readiness.py' `
  --json-out 'C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\docs\phase1\readiness_gate_reports\readiness_gate_run_001.json' `
  --md-out 'C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\docs\phase1\readiness_gate_reports\readiness_gate_run_001.md'
```

测试：

```powershell
& 'C:\Users\ghost\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' `
  'C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\tools\phase1_readiness_gate\test_phase1_readiness_gate.py'
```

严格模式：

```powershell
& 'C:\Users\ghost\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' `
  'C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\tools\phase1_readiness_gate\check_phase1_readiness.py' `
  --strict
```

当前严格模式应返回成功，因为 M1-Formal 已可放行。

## 下一步

进入 Qt6 + OCCT 工程初始化前继续保留这些护栏：

1. 正式工程创建后继续跑 CMake 模块 allowlist 扫描。
2. 保留 Qt6 runtime gate，防止 Save/Open 和 binding repair 报告退回 simulation。
3. 保留 Detail writer L0/L1 报告；AutoCAD L2 导入另走工程图专项。
4. 新增真实工程 STEP 样本时继续跑 STEP selection gate。
5. 后续开发仍按旧图石运行、IDA、SFL、Detail 证据闭合业务缺口；`TODO-029 / Rebar.Edit.Move P0`、`TODO-032 / Rebar.Edit.Copy P0`、`TODO-030 / M2-Stats-001`、`TODO-031 / M2-Drawing-001`、`TODO-033 / M2-Drawing-002`、`TODO-034 / M2-Drawing-003` 和 `TODO-035 / M2-Drawing-004` 已纳入 done-report 映射。下一步建议按 `TODO-036 / M2-Drawing-005` 做 AutoCAD L2 复杂字段骨架导入验证 P0，或在用户能启动 AutoCAD 2020 时继续执行 `69/71` 的手工 L2 导入清单；`TODO-026` golden 采集保持 pending，不自动进入。
