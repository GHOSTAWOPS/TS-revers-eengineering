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
- `TODO-036 / M2-Drawing-005` AutoCAD L2复杂字段骨架导入验证 P0 已落地并通过离线检查：`detail_l2_fixture_probe` 生成 TODO-036 三图纸复杂骨架包，离线确认 `PartDetailDrawing num=8`、复杂容器和 `General-Info` 首批扩展字段；本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD registry 根键存在但无版本子项，所以 `autocadL2=not_run`；xhigh 已指出并修正 pointStb L2 变量污染。
- `TODO-037 / M2-Drawing-006` DetailWriter pointStb / FaceEdge 兼容字段骨架 P0 已落地并通过离线检查：`DetailWriter` 输出 `pointStb StbGeo shapeType=C` 和显式 `FaceEdge shapeType=L/A` 字段骨架；`detail_l2_fixture_probe --fixture point-face-edge` 生成独立三图纸包并确认 `pointFaceEdge.passed=true`；当前 CTest 17/17 pass，strict readiness gate 84/84 pass，保护层 OCCT/AIS 泄漏扫描无匹配；该阶段不是 AutoCAD L2 通过、旧插件接受新包、点筋生成算法、FaceEdge 生成规则、完整工程图或 golden 证明。
- `TODO-038 / M2-Drawing-007` DetailWriter section-line Line/Arc/ZValue 字段骨架 P0 已落地并通过离线检查：`DetailWriter` 输出 `section-line / lines / LineN` 和 `section-line / Arcs / ArcN` 字段骨架，并保留旧样例确认的 `ZValue` 字符串；`detail_l2_fixture_probe --fixture section-line` 生成独立三图纸包并确认 `sectionLine.passed=true`；当前 AutoCAD L2 仍为 `not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实剖切线算法、隐藏线 / 填充线 / 接头线算法、`ZValue` 语义或 golden 证明。
- `TODO-039 / M2-Drawing-008` DetailWriter section-line AutoCAD L2 运行确认准备 P0 已落地：复用 `section-line` 独立变量生成 TODO-039 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash，并再次确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD registry 根键存在但无版本子项，所以 `autocadL2=not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实剖切线算法、`ZValue` 语义、完整工程图或 golden 证明。
- `TODO-040 / M2-Drawing-009` DetailWriter 线容器字段骨架 P0 已落地并通过离线检查：`DetailWriter` 输出 `continue-line / hidden-line / central-line / hatch-line` 下的 `LineN start_x/start_y/end_x/end_y/ZValue` 字段骨架；`detail_l2_fixture_probe --fixture line-containers` 生成独立三图纸包并确认 `lineContainers.passed=true`；当前 AutoCAD L2 仍为 `not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实连续线 / 隐藏线 / 中心线 / 填充线算法、完整工程图或 golden 证明。
- `TODO-041 / M2-Drawing-010` DetailWriter line-containers AutoCAD L2 运行确认准备 P0 已落地：复用 `line-containers` 独立变量生成 TODO-041 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash、旧样例包 hash，并再次确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD registry 根键存在但无版本子项，所以 `autocadL2=not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实连续线 / 隐藏线 / 中心线 / 填充线算法、完整工程图或 golden 证明。
- `TODO-042 / M2-Drawing-011` DetailWriter pointStb / FaceEdge AutoCAD L2 运行确认准备 P0 已落地：复用 `point-face-edge` 独立变量生成 TODO-042 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash、旧样例包 hash，并再次确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD registry 根键存在但无版本子项，所以 `autocadL2=not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、点筋生成规则、FaceEdge 生成规则、真实工程图算法或 golden 证明。
- `TODO-043 / M2-Drawing-012` DetailWriter Others / steeljoint-line 字段骨架 P0 已落地：复用 `others-steeljoint` 独立变量生成 TODO-043 三图纸 Detail 包并离线确认 `Others` 空容器和 `steeljoint-line / joints` 容器存在；当前 `autocadL2=not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实接头线算法、Others 几何算法、完整工程图或 golden 证明。
- `TODO-044 / M2-Drawing-013` DetailWriter Others / steeljoint-line AutoCAD L2 运行确认准备 P0 已落地：复用 `others-steeljoint` 独立变量生成 TODO-044 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash、旧样例包 hash、新包 hash，并再次确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD registry 根键存在但无版本子项，所以 `autocadL2=not_run`。该阶段不是 AutoCAD L2 通过、旧插件接受新包、真实接头线算法、Others 几何算法、完整工程图或 golden 证明。
- `TODO-046 / M2-Drawing-015` 真实接头线 / Others 旧图石运行确认与参数绑定追踪 P0 已落地：IDA MCP 已把 `JointRuler / JointDistbet / JointWeldLength` 的对话框 / 配置链闭合到旧全局值，确认 `sub_1406107F0` 使用 `JointWeldLength / 2000.0` 对称半长写 `LineN`，确认 `pattern` raw byte `0x4C ('L')`、`Others / symbolcutIOS` gate 和额外弧线 / `DrawTaoTong` 关系；当前仍未收集旧图石非空运行样例，`autocadL2=not_run`。该阶段不是真实接头线算法、Others 几何算法、AutoCAD L2、旧插件接受或完整工程图证明。
- `TODO-047 / M2-Drawing-016` 真实接头线 / Others 旧图石运行样例采集 P0 已以 stop point 形式落地：本轮旧图石启动探测确认 `VisualTS.exe` 会先弹出标题为 `提示` 的阻塞框，截图可见文本前缀 `请检查网线是否...`；阻塞发生在打开 `SFL` 之前，因此未进入旧图石主界面、未执行接头命令、未导出新的 `DetailNN.stl`，也未采到非空 `steeljoint-line / joints / Others / symbolcutIOS` 节点样例。该阶段只证明当前旧运行入口存在启动期阻塞，不是真实接头线算法、Others 几何算法、AutoCAD L2、旧插件接受或完整工程图证明。
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
5. 后续开发仍按旧图石运行、IDA、SFL、Detail 证据闭合业务缺口。`TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0` 已纳入 readiness gate done-report 映射：要求存在 `118_M2-RebarCreate-010线配筋公共创建Core参数Gate与Diagnostic对齐P0实现记录.md`、`docs/phase1/app_build_reports/m2_rebar_create_010_run_001.md` 和同名 `.json`，并拦截 `pending_readonly_review` 这类未最终收口状态。当前下一步切到 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0`，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 或 golden。
   注：第 5 条是历史长链归档，其中“下一步建议继续执行 TODO-064”是当时状态；当前权威 next 以第 12 条和 `todo.csv` 的 `TODO-071` 为准。
6. `TODO-064 / 生成工程图与下料表中文caption资源绑定及真正生成工程图弹窗候选补证 P1` 已形成 `E-IDA-044 / E-DEV-086`：`LoadStringW` 资源探针已把 `36124 = 生成工程图`、`35057 = 下料表` 静态闭合，并确认旧资源采用 `描述/help \n caption` 模式；`sub_140600AA0` 里唯一可直接证明的 `DoModal` 已收窄到 `output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT` 后置报告链；`Dialog #427` 已降级为 `OptionFactory` 属性页，不再作为下料表弹窗真值。当前下一步切到 `TODO-065 / 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0`，继续补运行确认模板和拒收伪工件门禁，不自动启动旧图石、不声明 AutoCAD L2 或 golden 通过。
7. `TODO-065 / 生成工程图与下料表旧图石运行确认清单与工件门禁准备 P0` 已形成 `E-DEV-087`：新增 `docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/` 模板目录和 `capture_notes` 模板，把生成工程图 / 下料表的运行确认清单收紧到输出目录 listing / `SHA256` / 最后修改时间 / rerun 覆盖行为 / 额外运行时弹窗 / 拒收伪工件规则；同时明确 `UnCutSteel.TXT` 只算可选旁证，不能单独证明 `生成工程图` 成功。
8. `TODO-066 / 生成工程图与下料表旧图石真实运行工件回填 P0` 已形成 `E-RUN-006 / E-DEV-088`：用户现场回填旧图石真实工件已归档到 `docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/`，包含源 `SFL` 文件名/hash（`.sfl` 本体按 `.gitignore` 不纳入 git）、空样式根 `Detail.xml`、非空 `Detail01..04.stl`、非空 `下料表.xls` 和两张运行截图；用户补充多台电脑生成结果一致，`Detail.xml` 都是 `<StyleRoot/>`，且生成工程图 / CAD 导入时修改时间不更新，当前更倾向记录为固定空模板 / 占位文件；readiness gate 已把 `TODO-066` 纳入 done-report 映射，并新增 `pending_final` 拦截，防止 done 节点报告带未收尾状态。当前下一步切到 `TODO-067 / 旧图石真实 Detail 与下料表字段对照 P0`，只做字段差距报告，不自动启动旧图石、不声明 AutoCAD L2 或 golden 通过。
9. `TODO-067 / 旧图石真实 Detail 与下料表字段对照 P0` 已形成 `E-DETAIL-004 / E-DEV-089`：本轮只解析 `RUN-20260609-001` 的真实 `Detail01..04.stl` 和 `下料表.xls`，对照当前 `DetailWriter / RebarScheduleService` 字段覆盖，确认 `Detail.xml` 空模板、主图才有 `StbTable / MaterialTable`、`StbTable` 扩展属性、`StbRow` 三个缺失属性和 Excel 三表 writer 缺口；readiness gate 已把 `TODO-067` 纳入 done-report 映射。下一步切到 `TODO-068 / DetailWriter 真实字段差异 P0：空 Detail.xml 与主图表格策略`。
10. `TODO-068 / DetailWriter 真实字段差异 P0：空 Detail.xml 与主图表格策略` 已形成 `E-DEV-090`：本轮让正式 `DetailWriter` 输出空 `<StyleRoot/>\r\n`、只在 `Detail01.stl` 输出 `StbTable / MaterialTable`、副图保留空 `StbTables`，并补 `StbTable` 表级属性骨架；readiness gate 已把 `TODO-068` 纳入 done-report 映射。下一步切到 `TODO-069 / DetailWriter 真实字段差异 P1：StbRow 扩展属性骨架`。
11. `TODO-069 / DetailWriter 真实字段差异 P1：StbRow 扩展属性骨架` 已形成 `E-DEV-091`：本轮让正式 `DetailWriter` 在 `StbRow#` 输出 `smallTable / mirrorType / mirrorSEFlag` 三个扩展属性默认骨架，值均为 `0`；readiness gate 已把 `TODO-069` 纳入 done-report 映射，并补充拦截 `not_run_before_commit_review / not_run_until_docs_mapping` 这类 done report 未收口占位状态。当前 readiness unit 28/28 pass，strict gate 84/84 pass。下一步切到 `TODO-070 / DetailWriter 真实字段差异 P2：lineStb StbGeo 字段条件化骨架`。
12. `TODO-070 / DetailWriter 真实字段差异 P2：lineStb StbGeo 字段条件化骨架` 已形成 `E-DEV-092`：本轮让正式 `DetailWriter` 对 `lineStb + shapeType=L` 的 `StbGeo#` 只输出 `segID / stbSeqNum / shapeType / start / end / offset`，不再输出 `middle / start_r / end_r / length`；readiness gate 已把 `TODO-070` 纳入 done-report 映射。当前 readiness unit 29/29 pass，strict gate 84/84 pass。下一步切到 `TODO-071 / 线配筋生成旧逻辑证据与 P0 切片准备`，回到钢筋生成主线。
13. `TODO-071 / 线配筋生成旧逻辑证据与 P0 切片准备` 已形成 `E-IDA-045 / E-DEV-093`：本轮用 IDA MCP 复核 `sgroupbarline` 表项、`sub_1404DE720` 入口契约和 `sub_1404D10C0` 公共创建调用，并把单选、`sub_1405C6820 / sub_1405DA020` 选择 gate、内部子项下限、奇数索引实体抽取、4 个端点距离候选、初始最小距离候选 `10.0` 写入正式 `RebarGroupCreator` P0 raw/evidence；readiness gate 已把 `TODO-071` 纳入 done-report 映射并新增 `pending_after_docs` 拦截。该轮 CTest 18/18 pass，readiness unit 31/31 pass，strict gate 84/84 pass，xhigh 只读 review 为 `needs_fix` 且 Important 已由主流程修复。当时下一步切到 `TODO-072 / 线配筋命令 handler P0：LegacySelection 到 RebarGroupCreator 事务接入`，不自动进入面配筋、接头、Excel、Detail 字段继续扩张或 golden。
14. `TODO-072 / 线配筋命令 handler P0：LegacySelection 到 RebarGroupCreator 事务接入` 已形成 `E-DEV-094`：新增 `RebarLineGroupCommandHandler`，把 `Rebar.Create.LineGroup` 从命令入口接到 `LegacySelectionRef -> LegacyRebarGeometryReader -> RebarGroupCreator -> SteelData` P0 事务边界；测试覆盖 empty selection、wrong type、valid edge、geometry failure 和 normalizer failure；`LegacyUiCommandMap` 已把 `Rebar.Create.LineGroup` 标记为 `Implemented`，并明确 `edge-p0-surrogate` 只是 P0 选择代用，不等价旧完整 `ENTITY_LIST` 语义。readiness gate 已把 `TODO-072` 纳入 done-report 映射。当前下一步切到 `TODO-073 / 线配筋 UI 到 AIS 显示 P0：命令完成后显示创建的 SteelBarGroup`，不自动进入完整参数弹窗、面配筋、弧筋、接头、Excel 或 golden。
15. `TODO-073 / 线配筋 UI 到 AIS 显示 P0：命令完成后显示创建的 SteelBarGroup` 已形成 `E-DEV-095`：新增 `OccViewerWidget::displayRebarPresentation` 和 `displayedRebarShapeCount`，`MainWindow` 在 `Rebar.Create.LineGroup` 成功后通过 `RebarAisPresentationAdapter` 显示新增 `SteelBarGroup`；新增 `line_group_display_smoke_123` 在真实 `123.stp` 上覆盖失败路径不刷新显示、成功路径显示计数增加；readiness gate 已把 `TODO-073` 纳入 done-report 映射。当前下一步切到 `TODO-074 / 线配筋参数窗口 P0`，不自动进入面配筋、弧筋、接头、Excel 或 golden。
16. `TODO-074 / 线配筋参数窗口 P0：旧 UI 字段占位、默认值和 handler 参数接入` 已形成 `E-DEV-096`：新增 `LineGroupParameterDialog`，`MainWindow` 在 `Rebar.Create.LineGroup` 前收集直径、间距、数量、钢筋级别、起点距离和终点距离，并传给 `RebarLineGroupCommandHandler`；取消或失败不污染 `SteelData` 且不刷新 AIS；readiness gate 已把 `TODO-074` 纳入 done-report 映射。当前下一步切到 `TODO-075 / 线配筋选择预检与参数窗口顺序对齐 P0`，只对齐旧 `sub_1404DE720` 的先选择预检顺序，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel 或 golden。
17. `TODO-075 / 线配筋选择预检与参数窗口顺序对齐 P0` 已形成 `E-DEV-097`：`MainWindow` 现在先用 `lineGroupSelectionPreflightForCommand` 检查当前选择，再打开 `LineGroupParameterDialog`；无选择或非 edge 时不打开参数窗口、不执行 handler、不污染 `SteelData` 且不刷新 AIS；有效 edge 时继续保留参数窗口和参数传递能力。readiness gate 已把 `TODO-075` 纳入 done-report 映射。当前下一步切到 `TODO-076 / 线配筋旧 UI 失败提示与状态栏口径证据 P0`，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel 或 golden。
18. `TODO-076 / 线配筋旧 UI 失败提示与状态栏口径证据 P0` 已形成 `E-IDA-046 / E-DEV-098`：IDA MCP 确认 `sub_1404DE720` 和 `sub_14054B410` 内没有直接中文失败提示字符串，`sub_1404D10C0` 只收窄到公共 ACIS outcome / part state 链和公共 `Input_float / Dialog #383`；`Dialog #383` 不是旧线配筋主参数窗口字段清单。readiness gate 已把 `TODO-076` 纳入 done-report 映射。当前下一步切到 `TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0`，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段或 golden。
19. `TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0` 已形成 `E-DEV-099`：新增 `docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/` 模板目录、`capture_notes.md` 和 `collect_hashes.ps1`，把无选择、选错对象、有效对象、旧主参数窗口、公共 `Dialog #383`、状态栏 pane、模型树 / 输出前后状态、截图 / listing / hash / 操作步骤和拒收伪工件规则落成可执行采样清单；readiness gate 已把 `TODO-077` 纳入 done-report 映射并新增 pending 报告拦截测试。该节点当时切到 `TODO-078 / 线配筋旧图石运行确认工件回填 P0`，只接收和核对真实旧图石运行工件，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段或 golden；后续 `TODO-078` 已因缺真实运行工件改为 blocked。
20. `TODO-078 / 线配筋旧图石运行确认工件回填 P0` 已形成 `E-DEV-100` 阻塞审计：`docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/` 当前只有 `README.md / capture_notes.md / collect_hashes.ps1` 模板，没有 `hashes.txt`、旧图石运行截图、listing 或填写后的旧图石运行 notes。因此 `TODO-078` 不能纳入 done-report 映射，状态改为 `blocked`；`GAP-UI-REB-001 / GAP-IDA-002` 继续 open。当前下一步切到 `TODO-079 / 线配筋旧 UI / Dialog 静态资源补证 P0`，只做 IDA MCP / 静态资源补证，不替代真实运行确认，不自动启动旧图石、不安装 HASP、不进入完整线配筋算法或 golden。
21. `TODO-079 / 线配筋旧 UI / Dialog 静态资源补证 P0` 已形成 `E-IDA-047 / E-DEV-101`：IDA MCP 复核 `sgroupbarline` 表项、公共 `Input_float / Dialog #383` 构造和初始化链、`SetPaneText` 调用层；直接字符串扫查没有命中线配筋中文 UI / 状态栏文案。readiness gate 已把 `TODO-079` 纳入 done-report 映射。当前下一步切到 `TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0`，不自动启动旧图石、不安装 HASP、不进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 或 golden。
22. `TODO-080 / 线配筋公共创建 core 参数 gate 与 diagnostic 对齐 P0` 已形成 `E-DEV-102`：`RebarGroupCreator` 新增 `LegacyPublicCreateGateSnapshot`，在几何查询前执行 `sub_1404D10C0` 已确认的 `objB / objA / sub_1405F25F0(objA) >= 3 / ENTITY_LIST count >= 1 / distanceA >= 0.002` gate，并把 gate 快照写入 raw evidence；readiness gate 已把 `TODO-080` 纳入 done-report 映射。当前下一步切到 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0`，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 或 golden。
