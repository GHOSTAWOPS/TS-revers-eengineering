# CSE v2 Goal 执行目标与 Todo 说明

## 一句话结论

后续 goal 模式的主线不是“重新设计一个钢筋软件”，而是继续把正式 `app` 往旧图石 1:1 复刻方向推进：Qt6 / OCCT 只替代界面、显示、选择和几何能力，钢筋业务逻辑按 VisualTS / IDA / SFL / Detail 证据复刻。

## 当前执行口径（2026-06-09）

```text
接头链路现阶段先记录、暂缓。

这不是说接头以后不做，
而是领导已经明确：现阶段先不用考虑接头。

所以当前 goal 主线：
  -> 不再把 TODO-050 / TODO-061 或其他接头运行确认节点设为 next
  -> 只保留已有证据、缺口、采样模板和恢复入口
  -> 当前 next 继续按 todo.csv 执行非接头主线
  -> TODO-065 已完成：生成工程图 / 下料表运行确认模板、
     输出目录 / hash / 覆盖策略记录模板和拒收伪工件规则已落文档
  -> output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
     继续只算可选旁证，不算前置设置窗
  -> TODO-066 已完成真实工件回填；TODO-067 已完成字段对照；TODO-068 已完成最明显旧包格式纠偏；TODO-069 已完成 StbRow 扩展属性骨架；TODO-070 已完成 lineStb StbGeo 字段条件化骨架；TODO-071 已完成线配筋入口契约冻结；TODO-072 已完成 LineGroup command handler P0；TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0；TODO-074 已完成 LineGroup 参数窗口 P0；TODO-075 已完成选择预检与参数窗口顺序对齐 P0；TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point；TODO-077 已完成旧图石运行确认采样清单与拒收规则；现在的主线是 TODO-078
```

## 可直接粘贴到 Goal 模式的目标

### 长期 Goal（可直接复制，适合长期执行）

目标：长期持续推进《图石钢筋 1 比 1 复刻》正式 `app` 开发，直到具备按旧 VisualTS 证据复刻钢筋创建、编辑、统计、出图的工程条件。

本 goal 不是“用 OCCT 重新设计一个差不多的钢筋软件”。

正确路线必须始终保持为：

```text
Qt6
  -> 替代 MFC / Codejock，只负责新界面、菜单、命令入口、窗口状态。

OCCT AIS
  -> 替代 HOOPS，只负责三维显示、旋转缩放、选择、高亮。

OCCT 几何 API
  -> 替代 ACIS，只负责 EDGE / FACE / 曲线 / 距离 / 投影 / split /
     trim / spline / wire chain / offset / section / sweep 等几何能力。

LegacyGeometryAdapter
  -> 把 OCCT 包装成旧 VisualTS 熟悉的 EDGE / FACE / ENTITY_LIST /
     曲线 / 点 / 面 / 选择引用语义。

VisualTS 复刻业务层
  -> 按 IDA / SFL / Detail / 旧图石运行证据 1:1 复刻钢筋创建、
     编辑、统计、出图逻辑。

Detail / 新设计文件格式输出层
  -> 输出 Detail.xml + DetailNN.stl；
     用新的工程格式替代 .sfl 主保存格式；
     新格式结合 SFL 业务语义和 OCCT 几何引用。
```

一句话硬约束：

```text
外壳换 Qt6 + OCCT。
几何能力由 OCCT 提供。
钢筋业务规则按旧图石 VisualTS 证据复刻。
中间必须经过 LegacyGeometryAdapter 隔离。
```

用户要点冻结：

```text
1. 目标是 1:1 复刻旧图石钢筋功能操作，不是做一个“够用的新钢筋软件”。
2. 老图石界面可以更现代、更好看，但菜单入口、命令语义、参数、状态机和输出结果要能追溯到旧图石。
3. OCCT 不能成为钢筋业务真相；OCCT 只替代 ACIS/HOOPS 的几何、显示、选择能力。
4. 钢筋由我们复刻的 VisualTS 业务层创建，不由 OCCT 直接创建业务钢筋。
5. 父目录已有开发只能参考 STEP/STP 导入、XCAF 遍历、AIS viewer、选择 ID、OCCT API 写法。
6. 父目录的 OCCT 直接重写钢筋路线是错误路线，不能迁入正式 app 当主线。
7. 新系统不用 .sfl 做主保存格式；新设计文件格式要结合 SFL 业务语义和 OCCT 几何引用。
8. SFL / STP / Detail / 旧图石运行结果都是证据来源，但不能互相替代。
9. CAD 有商业版，可以作为 Detail / AutoCAD 插件验证环境；但 ACIS / HOOPS / Codejock 不纳入新系统依赖。
10. golden 不是当前前置阻塞，但后续做 1:1 行为闭环时必须逐步补。
11. 不确定旧逻辑时优先用 IDA MCP 或旧图石运行确认，不能凭父目录代码拍脑袋定案。
12. 每次完成一个清晰节点都要 commit、打 tag、push，形成可回退时间线。
13. 涉及代码、测试、构建脚本的节点，在 commit 前必须经过 xhigh 只读 review；修改只能由主流程 agent 完成。
14. 旧图石如果依赖 USB 狗或其他许可前置，那只属于旧系统运行取证条件；新系统复刻目标不继承 USB 狗 / 网络许可依赖，仍要保持开源、无狗。
15. 接头链路现阶段只保留证据和恢复入口，不作为当前 next；除非用户明确恢复优先级，否则不要自动回到 TODO-050 / TODO-061 或其他接头运行确认节点。
```

CSE v2 Control Contract：

```text
Primary Setpoint
  长期推进正式 app，让 Qt6 / OCCT 成为旧商业底座替代层，
  让 VisualTS 复刻业务层逐步具备钢筋创建、编辑、统计、出图能力。

Acceptance
  每个切片有测试、实现记录、build report、追溯矩阵、缺口记录、
  todo 状态、代码节点 xhigh review 结论、commit、annotated tag、push 结果。

Guardrail Metrics
  domain/rebar 不出现 TopoDS_、AIS_、BRep*、TopAbs_；
  不迁入父目录 rebar 业务；
  不把 OCCT API 直接扩散到业务层；
  不把低置信推断写成确定事实。

Sampling Plan
  每轮开始看 todo.csv / 46 / 99 / git status；
  每轮结束跑默认 CTest、readiness gate、domain OCCT 泄漏检查；
  涉及代码、测试、构建脚本的节点，在验证通过后、commit 前执行 xhigh 只读 review；
  涉及旧业务时补 IDA / 旧图石 / SFL / STP / Detail 证据。

Known Delays
  IDA MCP 可能没有绑定数据库；
  旧图石运行确认依赖用户操作；
  golden 采集成本高；
  OCCT 与 ACIS 的容差差异需要后续样本验证。

Recovery Target
  如果路线偏成“OCCT 直接重写钢筋”，立即停止功能开发，
  回到 23 / 35 / 46 文档和 adapter 边界修正。

Rollback Trigger
  测试或 gate 失败仍继续堆功能；
  domain/rebar 引入 OCCT/AIS；
  父目录 rebar 业务被迁入；
  无证据旧逻辑被写死；
  代码节点跳过 xhigh 只读 review；
  todo 与 46 的 next 目标不一致。

Boundary
  每轮只推进一个 M1/M2 切片；
  adapter 切片只改 legacy geometry DTO、OCCT adapter、测试、文档；
  业务切片只通过 legacy interface 使用几何能力；
  UI 切片只对齐旧命令入口和状态，不顺手改业务。
  xhigh agent 只读，只能评审 diff / 文档 / 测试输出，不能修改、格式化、commit、tag、push。

Actuator Budget
  单轮只做一个可验证节点；
  完成后停止复盘，除非用户明确要求继续下一轮。
```

工作目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件
```

主项目目录：

```text
【图石钢筋1比1复刻】
```

正式开发目录：

```text
【图石钢筋1比1复刻】\app
```

先读取以下入口文件，建立当前事实边界：

1. `【图石钢筋1比1复刻】\00_总览.md`
2. `【图石钢筋1比1复刻】\06_技术路线与替代方案.md`
3. `【图石钢筋1比1复刻】\07_1比1复刻实施路线.md`
4. `【图石钢筋1比1复刻】\08_开发命令契约.md`
5. `【图石钢筋1比1复刻】\09_钢筋领域模型草案.md`
6. `【图石钢筋1比1复刻】\11_需求证据追溯矩阵.md`
7. `【图石钢筋1比1复刻】\13_Detail字段映射矩阵.md`
8. `【图石钢筋1比1复刻】\15_线配筋与弧形组专项初稿.md`
9. `【图石钢筋1比1复刻】\16_seg_steelbargroup字段地图初稿.md`
10. `【图石钢筋1比1复刻】\18_新设计文件格式替代SFL策略.md`
11. `【图石钢筋1比1复刻】\23_父目录源码参考边界与路线纠偏.md`
12. `【图石钢筋1比1复刻】\24_新设计文件格式Schema与Fixture草案.md`
13. `【图石钢筋1比1复刻】\32_Validator实现契约与错误码总表.md`
14. `【图石钢筋1比1复刻】\34_Phase1ReadinessGate实际运行记录.md`
15. `【图石钢筋1比1复刻】\35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
16. `【图石钢筋1比1复刻】\36_正式Qt6_OCCT工程架构与首个实现切片.md`
17. `【图石钢筋1比1复刻】\40_M1-App-004LegacyGeometryAdapterP0实现记录.md`
18. `【图石钢筋1比1复刻】\41_M1-App-005LegacyGeometryAdapterP1实现记录.md`
19. `【图石钢筋1比1复刻】\42_M1-App-006LegacyGeometryAdapterP2A实现记录.md`
20. `【图石钢筋1比1复刻】\43_M1-App-007LegacyGeometryAdapterP2B实现记录.md`
21. `【图石钢筋1比1复刻】\44_M1-App-008LegacyGeometryAdapterP2C实现记录.md`
22. `【图石钢筋1比1复刻】\45_M1-App-009LegacyGeometryAdapterP3A实现记录.md`
23. `【图石钢筋1比1复刻】\47_M1-App-010LegacyGeometryAdapterP3B实现记录.md`
24. `【图石钢筋1比1复刻】\48_M1-App-011LegacyGeometryAdapterP3C实现记录.md`
25. `【图石钢筋1比1复刻】\49_M1-App-012LegacyGeometryAdapterP3D实现记录.md`
26. `【图石钢筋1比1复刻】\50_M1-App-013LegacyGeometryAdapterP3E实现记录.md`
27. `【图石钢筋1比1复刻】\51_M1-App-014LegacyWireChain实现记录.md`
28. `【图石钢筋1比1复刻】\52_M1-App-015LegacyGeometryAdapterOffsetSpike实现记录.md`
29. `【图石钢筋1比1复刻】\53_M1-App-016LegacyGeometryAdapterSectionSpike实现记录.md`
30. `【图石钢筋1比1复刻】\54_M1-App-017LegacyGeometryAdapterSweepBoundary实现记录.md`
31. `【图石钢筋1比1复刻】\55_M1-App-018RebarDomainModelFreezeP1实现记录.md`
32. `【图石钢筋1比1复刻】\56_M1-App-019LegacyCommandContractP1实现记录.md`
33. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
34. `【图石钢筋1比1复刻】\todo.csv`

当前已完成状态：

```text
M1-App-001 = done
  -> Qt6 app 骨架、五个旧图石页签、命令注册、STEP import probe。

M1-App-002 = done
  -> 最小 OCCT AIS Viewer 和 STEP 显示。

M1-App-003 = done
  -> face / edge / vertex 选择系统、selection-v1 稳定引用。

M1-App-004 = done
  -> LegacyGeometryAdapter P0，EDGE / FACE 基础几何摘要。

M1-App-005 = done
  -> LegacyGeometryAdapter P1，bbox、采样、boundary loop、
     fingerprint、诊断矩阵。

M1-App-006 = done
  -> LegacyGeometryAdapter P2A，edge 切向、距离、最近点对。

M1-App-007 = done
  -> LegacyGeometryAdapter P2B，face boundary edge stableId、
     edge-face 接触/重叠代表点。

M1-App-008 = done
  -> LegacyGeometryAdapter P2C，edge 参数区间、子段长度、bbox、采样。

M1-App-009 = done
  -> LegacyGeometryAdapter P3A，edge split by parameter。

M1-App-010 = done
  -> edgeProjectPoint + edgeSplitAtPoint。

M1-App-011 = done
  -> edgeTrimEndpoint。

M1-App-012 = done
  -> pointToEdgeGroupDistance。

M1-App-013 = done
  -> buildSplineFromPoints。

M1-App-014 = done
  -> buildWireChain。

M1-App-015 = done
  -> offsetEdgePreview。

M1-App-016 = done
  -> facePlaneSectionPreview。

M1-App-017 = done
  -> edgeCircularSweepPreview。

M1-App-018 = done
  -> domain/rebar 钢筋领域模型冻结 P1。

M1-App-019 = done
  -> 旧命令契约绑定 P1，线筋 / 弧筋 / 与线裁剪 / 与面裁剪进入
     LegacyUiCommandMap 和 CommandRegistry，当前为 NotImplemented 占位。

TODO-020 = done
  -> IDA MCP 旧线筋 / 弧筋链补证据，确认
     sgroupbarline / sgroupbararc -> sub_1404D10C0 -> sub_1405D5670
     的 split / spline / trim / min-distance / backup 写回主规则。

TODO-021 / M1-App-020 = done
  -> 旧线筋 / 弧筋创建算法 P0，新增 RebarGroupCreator、
     LegacyRebarGeometryReader 和 SegmentCurveNormalizer P0 请求，
     可输出 domain SteelBarGroup，但不声明完整旧 UI/golden。

TODO-022 / M1-App-021 = done
  -> AIS 钢筋显示映射 P0，RebarAisPresentationAdapter 可把
     domain SteelBarGroup / SteelBarSegment 映射为 presentation/occ 层
     AIS_Shape，不让 AIS / OCCT 泄漏进 domain/rebar。

TODO-023 / M1-App-022 = done
  -> 新设计文件格式 runtime P1，TsRebarProjectRuntime 可保存 /
     读取 STEP 来源、selection-v1 refs、rebar groups、binding、
     evidence 和 unresolved 字段。

TODO-024 / M1-App-023 = done
  -> DetailWriter P1，DetailWriter 可把 domain SteelData /
     SteelBarGroup / SteelBar / SteelBarSegment 映射为 Detail.xml +
     Detail01.stl 首批字段，并覆盖失败恢复旧 Detail 包。

TODO-025 / Evidence = done
  -> 旧图石输出钢筋 STP 样本入库验证，`123.stp` 已固定为
     `tushi_rebar_123_stp` 几何 witness，记录源 SFL、hash、
     OCCT import probe 和既有 5 轮 STEP selection gate 摘要。

TODO-027 / M2-UI = done
  -> 旧 UI 功能入口 P1，`17` 矩阵一期入口已接入
     CommandId / LegacyUiCommandMap / CommandRegistry / Qt6 QAction，
     `tsrebar_app --smoke` 会校验 QAction 追溯 metadata。

TODO-029 / M2-Edit = done
  -> Rebar.Edit.Move / 钢筋移动 P0，IDA MCP 已补证
     barmove -> Input_Choice -> translate_transf 移动链，
     domain/rebar 新增 RebarEditMoveService，按 copyFlag=0
     实现领域层整体平移，不声明完整旧 ACIS topology mutation。

TODO-032 / M2-Edit = done
  -> Rebar.Edit.Copy / 钢筋拷贝 P0，IDA MCP 已补证
     scopy -> Input_Choice copyFlag=1 -> sub_1405989C0 -> sub_1405AA5D0
     拷贝链，domain/rebar 新增 RebarEditCopyService，按复制后累计平移
     实现领域层 copy，不声明完整旧 ACIS topology clone、旧编号、dirty/undo 或 golden。
```

当前最新验证基线：

```text
app 默认 CTest = 20/20 pass
readiness gate = M1-Formal-Ready, 84/84 pass
domain/rebar + drawing + project OCCT 边界 = pass
TODO-030 xhigh 复审 = allow_commit
TODO-031 xhigh review = allow_commit
TODO-033 xhigh review = allow_commit
TODO-034 IDA MCP 复核 = fdrawing_arx_todo034 active session
TODO-035 验证 = CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-036 验证 = complexSkeleton.passed=true, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2 not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed
TODO-037 验证 = pointFaceEdge.passed=true, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2 not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-038 验证 = sectionLine.passed=true, lineCount=1, arcCount=1, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-039 验证 = sectionLine.passed=true, lineCount=1, arcCount=1, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-040 验证 = lineContainers.passed=true, containerCount=4, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh round2 allow_commit
TODO-041 验证 = lineContainers.passed=true, containerCount=4, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-042 验证 = pointFaceEdge.passed=true, pointGroupCount=2, pointGeoCount=2, faceEdgeCount=2, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-043 验证 = othersSteeljoint.passed=true, Others empty container, steeljoint-line/joints present, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, agent closed
TODO-044 验证 = othersSteeljoint.passed=true, Others empty container, steeljoint-line/joints present, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, agent closed
TODO-057 验证 = owner types traced to steelbar / steelbargroup / seg_steelbargroup, Dialog #428 OK chain traced to sub_14045D720 -> sub_1405CB160 -> sub_1405B7350, child+112 narrowed to phase/position/start offset, phase1 gate unittest 18 tests pass, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh completed important_fixed minor_refined
TODO-069 验证 = StbRow smallTable / mirrorType / mirrorSEFlag 字段骨架已输出，CTest 18/18 pass, readiness unit 28/28 pass, strict readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, one-shot reviewer process exited

latest completed tag = evidence-057/joint-db6c0-owning-dialog-static-trace
planned tag = evidence-058/joint-dialog-message-map-node112-stop-point
```

当前下一步：

```text
TODO-078 / 线配筋旧图石运行确认工件回填 P0
  -> 接头链路现阶段先记录、暂缓。
  -> TODO-071 已完成 sgroupbarline 入口契约冻结。
  -> TODO-072 已完成 LineGroup command handler P0 事务接入。
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0。
  -> TODO-074 已完成 LineGroup 参数窗口 P0。
  -> TODO-075 已完成先选择预检、再打开参数窗口的最小顺序对齐。
  -> TODO-076 已确认旧 handler / helper 无直接中文失败提示，公共链只收窄到 Input_float / Dialog #383。
  -> TODO-077 已完成旧 UI 失败提示 / 状态栏口径 / 参数 Dialog 字段的运行确认清单与工件门禁。
  -> 下一轮只回填和核对用户现场旧图石运行截图、状态栏、参数窗口、Dialog #383、hash 和步骤。
  -> 不自动启动旧图石，不安装 HASP，不用 OCCT 直接重写钢筋业务。
```

长期执行循环：

```text
从 todo.csv 中选择 status=next 的任务。
如果没有 next，选择依赖已满足的最高优先级 P0 pending。
每轮只推进一个明确切片。
每轮必须先读该任务 evidence 指向的文档。
每轮必须先补测试或测试用例，再改实现。
每轮实现后必须跑窄测、默认 CTest、readiness gate 或专项 gate。
涉及代码、测试、构建脚本的节点，验证通过后、commit 前必须执行 xhigh 只读 review。
xhigh 只能审查，不能修改；Critical / Important 必须由主流程 agent 修复或写明技术反驳理由。
每轮必须更新实现记录、build report、追溯矩阵、缺口文档和 todo.csv。
每轮完成后必须 commit、打 annotated tag、push main 和 tags。
完成一个切片后停止复盘，不自动进入下一个切片。
```

每轮开始必须检查：

```text
git status --short --branch
todo.csv 当前 next 项
46_CSE_v2Goal执行目标与Todo说明.md 当前目标
99_缺口和待确认项.md 当前相关缺口
app/src/domain/rebar 是否仍无 OCCT / AIS 类型泄漏
```

每轮默认验证命令：

```text
cd /d "C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\app"

"D:\Work\vcpkg\downloads\tools\cmake-4.3.2-windows\cmake-4.3.2-windows-x86_64\bin\cmake.exe" --build build

"D:\Work\vcpkg\downloads\tools\cmake-4.3.2-windows\cmake-4.3.2-windows-x86_64\bin\ctest.exe" --test-dir build --output-on-failure
```

每轮必须额外运行：

```text
py .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict

rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
```

TDD 规则：

```text
新增功能或行为变更必须先写失败测试。
必须看到 RED。
再写最小实现。
再跑 GREEN。
没有 RED 不能声称 TDD 完成。
```

禁止事项：

```text
禁止把父目录 src/rebar/* 当作旧图石业务真相迁入。
禁止迁入 RebarCreationCommandService。
禁止迁入 EdgeToRebarFactory。
禁止迁入 FaceRebarGenerator。
禁止迁入 PolylineRebarGenerator。
禁止用 OCCT 能怎么做替代旧图石怎么做。
禁止在 domain/rebar 中出现 TopoDS_、AIS_、BRep*、TopAbs_。
禁止业务层直接依赖 OCCT / AIS。
禁止在没有证据时把低置信推断写成确定结论。
禁止 CTest 或 gate 失败时继续堆新功能。
禁止因为没有 golden 就跳过证据追溯。
禁止让 xhigh agent 修改文件、格式化、commit、tag、push。
禁止代码节点跳过 xhigh 只读 review 后提交。
```

允许事项：

```text
允许参考父目录的 STEP/STP 导入、XCAF 遍历、AIS viewer、
选择 ID、OCCT API 写法。

允许在 LegacyGeometryAdapter 内部使用 OCCT。

允许在 presentation / viewer 层使用 AIS。

允许在文档中把低置信结论明确标为 gap。

允许 IDA MCP 可用时优先查旧函数、调用链、常量、字段。
```

IDA / 旧图石确认规则：

```text
遇到旧业务不确定时，优先级如下：

1. IDA MCP 查询旧 VisualTS 函数、调用链、常量和字段。
2. 旧图石软件运行确认，记录截图、操作步骤、输出文件、hash。
3. SFL / Detail / STP 样本交叉验证。
4. 父目录代码只能作为 OCCT 工程写法参考，不能关闭旧业务证据缺口。

如果 IDA MCP 不可用，必须写入 99_缺口和待确认项.md。
```

证据闭环要求：

```text
每个已完成功能必须有：

1. 实现记录 md。
2. build report md。
3. build report json。
4. 需求证据追溯矩阵更新。
5. 99 缺口更新。
6. todo.csv 状态更新。
7. CTest 结果。
8. readiness gate 结果。
9. 代码节点的 xhigh 只读 review 结论和处理结果。
10. commit。
11. annotated tag。
12. push 到 GitHub。
```

xhigh 只读 review 契约：

```text
适用范围：
  涉及代码、测试、构建脚本的节点强制执行。
  纯文档、todo、证据整理节点不强制，但可以按需执行。

执行时机：
  主流程 agent 完成实现和本地验证后、commit 前执行。

xhigh 权限：
  只读。
  只能读取 diff、相关文档、测试输出和验证报告。
  不能修改文件。
  不能 apply patch。
  不能运行格式化写回。
  不能 commit、tag、push。

输入必须包含：
  当前任务目标。
  相关 goal / todo / evidence 文档。
  base/head diff。
  本地验证命令和输出。
  本轮禁止事项，尤其是 OCCT 不得泄漏进 domain/rebar。

输出必须包含：
  Critical：必须修。
  Important：必须修或主流程 agent 写明技术反驳理由。
  Minor：可记录后续处理。
  Verdict：block 或 allow_commit。

子代理生命周期：
  xhigh 或其他子代理完成本轮任务并返回结论后，主流程 agent 必须及时调用 close_agent 关闭该子代理。
  关闭范围只限已经完成且不再需要的子代理，目的是减少代理负担。
  这不是“清空代理池”，也不关闭仍在执行有效任务的代理。
  如确需继续复用同一个子代理，必须在当前轮说明原因；复用结束后仍要关闭。

修复责任：
  xhigh 只给意见。
  主流程 agent 负责修改、再验证、文档更新、commit、tag、push。

不可用处理：
  如果 xhigh agent 不可用，必须在实现记录或 build report 中写明阻塞原因。
  不能伪造 review 结果。
```

Git 节点规则：

```text
每完成一个清晰节点必须 commit。
每个节点必须打 annotated tag。
tag 命名建议：

m1-app-015/offset-curve-preview-spike
m1-app-016/section-intersection-spike
m1-app-017/sweep-capability-boundary
m1-app-018/rebar-domain-model-p1

提交后必须：

git push origin main
git push origin --tags
```

长期成功标准：

```text
功能矩阵完整。
技术路线明确。
命令契约可开发。
钢筋领域模型可编码。
Detail 工程图字段可映射。
新设计文件格式可保存/读取/修复 binding。
需求、证据、缺口、运行确认能互相追溯。
关键缺口通过 IDA 或旧图石运行确认逐步闭合。
Qt6 + OCCT 只替代商业底座。
钢筋业务按旧 VisualTS 证据 1:1 复刻。
```

当前长期风险：

```text
IDA MCP 当前可能没有绑定数据库。
旧图石运行确认依赖用户操作。
OCCT 和 ACIS 几何细节可能有容差差异。
没有 golden 时只能先做结构正确和证据闭环。
父目录旧开发路线容易把项目带回“OCCT 直接重写钢筋”的错误方向。
```

恢复 / 回滚规则：

```text
发现路线偏移时，停止继续开发钢筋业务，先回到文档和 adapter 边界修正。

出现以下情况要停止并修正：

domain/rebar 出现 OCCT include。
父目录 rebar 业务被迁入。
测试失败但继续堆功能。
旧逻辑无证据却写成确定结论。
todo.csv 状态和 46 目标不一致。
文档说已完成但没有 build report / gate / commit / tag。
```

本长期 goal 的执行方式：

```text
不要中途停下来问是否继续。
除非遇到真实阻塞，否则按 todo.csv 顺序持续推进。
每轮只做一个切片。
每轮完成后更新 46，让下一轮 goal 指向新的 next。
每轮完成后输出：

完成了什么
验证了什么
还缺什么
下一阶段建议做什么
commit / tag / push 状态
```

### 短期 Goal（推荐本轮复制）

目标：只完成 `TODO-078 / 线配筋旧图石运行确认工件回填 P0` 这个短期阶段，不自动进入后续长期开发。

当前状态：

```text
接头链路现阶段先记录、暂缓：
  TODO-050 = blocked
  TODO-061 = pending
  不再作为当前 next。

非接头主线已推进到：
  TODO-062 = done
  TODO-063 = done
  TODO-064 = done
  TODO-065 = done
  TODO-066 = done
  TODO-067 = done
  TODO-068 = done
  TODO-069 = done
  TODO-070 = done
  TODO-071 = done
  TODO-072 = done
  TODO-073 = done
  TODO-074 = done
  TODO-075 = done
  TODO-076 = done
  TODO-077 = done
  TODO-078 = next
```

本轮只做：

```text
LineGroupRuntimeCaptureBackfillP0 / TODO-078
  -> 从 03 / 08 / 09 / 15 / 16 / 35 / 46 / 99 出发
  -> 在 TODO-071 已冻结 sgroupbarline 入口契约、TODO-072 已接通 command handler 事务、TODO-073 已接通 UI/AIS 可见反馈之后
  -> 在 TODO-074 已增加最小参数窗口 P0 后
  -> 在 TODO-075 已完成先选择预检、再打开参数窗口后
  -> 在 TODO-076 已确认旧 handler / helper 无直接中文失败提示、公共链只收窄到 Input_float / Dialog #383 后
  -> 在 TODO-077 已形成 docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture 采样模板和拒收规则后
  -> 只接收并核对用户现场旧图石运行工件
  -> 回填无选择、选错对象、有效对象、Dialog #383 标题/标签/默认值、参数窗口字段、状态栏 pane 和确认/取消行为
  -> 使用 hashes.txt / listing / capture_notes.md 校验工件来源和上下文
  -> 不写成完整旧 UI 已闭合，除非真实工件足以闭合对应字段
  -> 同步更新 02 / 08 / 11 / 35 / 46 / 99 / todo / 实现记录 / build report
  -> 跑默认 CTest / readiness gate / OCCT 泄漏检查
  -> 涉及代码、测试、门禁脚本时 commit 前必须 xhigh 只读 review；纯文档回填可记录 not_required_docs_only
  -> 不启动旧图石，不安装 HASP
  -> 不用 OCCT 直接重写钢筋业务
  -> 不迁入父目录 rebar 业务
  -> domain/rebar 不引入 TopoDS / AIS / BRep / TopAbs
  -> 不进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段继续扩张
  -> 不跑 AutoCAD L2，不进入 golden
```

目标语义：

```text
TODO-065 已把当前准备状态推进到：
  36124 / 0x8D1C -> psallc -> sub_140600AA0
  35057 / 0x88F1 -> psexcel -> sub_140605B20
  36124 = 生成工程图
  35057 = 下料表
  sub_140600AA0 里唯一可直接证明的 DoModal
    -> output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT
  docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/
    -> README + capture_notes 模板已落地

TODO-066 已确认：
  - RUN-20260609-001 真实工件已归档
  - Detail.xml 存在但为空 StyleRoot
  - 用户补充多机生成结果一致，Detail.xml 都是 <StyleRoot/>
  - 生成工程图 / CAD 导入时 Detail.xml 修改时间不更新
  - 当前更倾向把 Detail.xml 视为固定空模板 / 占位文件
  - Detail01..04.stl 是非空 DrawingRoot XML 图纸主体
  - 下料表.xls 是非空 Excel workbook
  - 父目录/外部有内容 Detail.xml 可能来自其他项目，不能混入同批证据

TODO-067 已确认字段差距：
  - 真实 Detail.xml 是空 <StyleRoot/>
  - 当前 DetailWriter 会写非空 StyleRoot / Styles / Style1
  - 真实旧包只有 Detail01.stl 带 StbTable / MaterialTable
  - 当前 DetailWriter 每张 DetailNN 都写表格
  - 真实 StbTable 有多个当前缺失的表级属性
  - 真实 StbRow 多 smallTable / mirrorType / mirrorSEFlag
  - 真实 Excel 有三张表，但当前 app 尚无 workbook writer

TODO-068 已完成：
  - Detail.xml 字节级输出 <StyleRoot/>\r\n
  - Detail01.stl 才写 StbTable / MaterialTable
  - Detail02.stl 及后续副图保留空 StbTables 容器
  - StbTable 已补 11 个表级属性骨架
  - 当前仍不声明 AutoCAD L2、Excel writer、隐藏线 / 填充线算法或 golden

TODO-069 已完成：
  - StbRow.smallTable / mirrorType / mirrorSEFlag 字段骨架已输出
  - 三个字段默认值均为 0
  - 字段真实公式和镜像语义仍保留 gap

TODO-070 已完成：
  - lineStb + shapeType=L 的 StbGeo 字段集已收窄到 start/end/offset
  - 不再输出 middle / start_r / end_r / length
  - 点筋、FaceEdge 和弧段既有字段保持不变

TODO-071 已完成：
  - IDA MCP 复核 sgroupbarline 表项、handler 和 secondary
  - sub_1404DE720 入口契约已冻结为：
    单选、选择对象检查、内部 child count >= 2、
    奇数索引 child 抽取、4 个端点距离候选、
    初始最小距离候选 10.0
  - RebarGroupCreator P0 raw/evidence 已写入 E-IDA-045 / E-DEV-093
  - 仍不声明旧 UI 运行流程、完整线配筋算法或 golden

TODO-072 已完成：
  - Rebar.Create.LineGroup 已接入 RebarLineGroupCommandHandler
  - 命令成功时追加 SteelBarGroup / SteelBar / SteelBarSegment 到 SteelData
  - empty selection / wrong type / geometry failure / normalizer failure 不污染 SteelData
  - edge-p0-surrogate 仍不等价旧完整 ENTITY_LIST

TODO-073 已完成：
  - Rebar.Create.LineGroup 成功后进入 RebarAisPresentationAdapter
  - OccViewerWidget::displayRebarPresentation 已显示新增 SteelBarGroup
  - 真实 123.stp smoke 覆盖失败路径不刷新显示、成功路径显示计数增加
  - 仍不声明旧 HOOPS 显示样式、参数窗口、undo/dirty 或 golden

TODO-074 已完成：
  - Rebar.Create.LineGroup 已增加 LineGroupParameterDialog P0
  - 参数窗口可传递直径、间距、数量、钢筋级别、起点距离和终点距离到 handler
  - 取消参数窗口不执行 handler，不污染 SteelData，不刷新 AIS
  - IDA MCP 未找到可直接证明旧参数 Dialog 字段的中文字符串
  - 字段、默认值、单位、灰显状态和提示仍保留 GAP-UI-REB-001
  - 当前流程仍与旧 sub_1404DE720 的先选择预检顺序存在差异，转入 TODO-075

当前仍未闭合：
  - 旧插件是否必须检查 Detail.xml 这个空占位文件
  - Detail.xml + DetailNN.stl 的真实 rerun 覆盖
  - StbTable 表级属性公式
  - StbRow smallTable / mirrorType / mirrorSEFlag 真实公式和镜像语义
  - StbGroup 多 Std# 和复杂钢筋 Detail 字段条件
  - Excel 三表 writer、单位换算和焊接字段
  - AutoCAD L2

本轮只做 TODO-075，
不回接头运行链，
不同时做完整线配筋算法、面配筋、弧筋、接头、Excel writer、真实工程图算法、golden 采集或 AutoCAD L2 通过声明。
```

当前已完成前置：

```text
TODO-010 ~ TODO-017 = done
  -> LegacyGeometryAdapter 已具备 point projection、split、trim、group distance、spline、wire chain、offset、section、sweep preview 等首批几何能力。

TODO-018 / M1-App-018 = done
  -> domain/rebar 钢筋领域模型冻结 P1。

TODO-019 / M1-App-019 = done
  -> 旧命令契约绑定 P1，LineGroup / ArcGroup / TrimByLine / TrimByFace 当前为 NotImplemented placeholder。

TODO-020 / Evidence = done
  -> IDA MCP 已成功补旧线筋 / 弧筋公共链，形成 E-IDA-022 和 57 实现记录。

TODO-021 / M1-App-020 = done
  -> 旧线筋 / 弧筋创建算法 P0，新增 RebarGroupCreator 和 SegmentCurveNormalizer P0 请求。

TODO-022 / M1-App-021 = done
  -> AIS 钢筋显示映射 P0。

TODO-023 / M1-App-022 = done
  -> 新设计文件格式 runtime P1，可保存 / 读取 domain 钢筋对象、binding 和 evidence。

TODO-024 / M1-App-023 = done
  -> DetailWriter P1，可把 domain 钢筋组映射为 Detail.xml + Detail01.stl 首批字段。

TODO-025 / Evidence = done
  -> 旧图石输出钢筋 STP 样本入库验证，`123.stp` 已固定为几何 witness。

TODO-027 / M2-UI = done
  -> 旧 UI 功能入口 P1，`17` 矩阵一期入口已接入正式 app。

TODO-028 / M2-Gate = done
  -> CSE readiness gate 扩展，RouteGuardrail 已自动检查路线护栏。

TODO-029 / M2-Edit = done
  -> Rebar.Edit.Move / 钢筋移动 P0，IDA MCP 已补证 copyFlag=0
     移动原对象链路，domain/rebar 已新增事务式整体平移服务。

TODO-032 / M2-Edit = done
  -> Rebar.Edit.Copy / 钢筋拷贝 P0，IDA MCP 已补证 copyFlag=1
     复制后变换链路，domain/rebar 已新增事务式复制后累计平移服务。

TODO-030 / M2-Stats = done
  -> 钢筋统计 / 下料表 P0，IDA MCP 已补证 Detail / XML writer 侧
     StbTable / MaterialTable 写出链，domain/rebar 已新增
     RebarScheduleService，DetailWriter 已消费同一 schedule service。
     sameGrpNum 完整合并规则、singleMass 旧来源、Volume722 ACIS
     等价、AutoCAD L2 和 golden 仍保留 GAP。

TODO-031 / M2-Drawing-001 = done
  -> DetailWriter 多图纸 P0，IDA MCP 已补证旧 DetailNN 命名规则。
     DetailWriter 可按多个 DrawingView 输出多张 DetailNN.stl，
     L0/L1 校验覆盖所有生成图纸，成功安装会删除旧多余 DetailNN，
     安装失败恢复旧 Detail 包。
     完整工程图、剖切线、隐藏线、填充线、AutoCAD L2 和 golden 仍保留 GAP。

TODO-033 / M2-Drawing-002 = done
  -> AutoCAD L2 导入验证 P0 已生成三图纸验证包和手工清单。
     当前本机 AutoCAD 环境 not_found，所以 autocadL2=not_run。

TODO-034 ~ TODO-044 / M2-Drawing-003..013 = done
  -> 已完成复杂字段静态证据、复杂字段骨架、pointStb / FaceEdge 字段骨架、
     section-line Line/Arc/ZValue 字段骨架、section-line L2 运行确认准备，
     line-containers LineN 字段骨架、line-containers L2 运行确认准备，
     pointStb / FaceEdge L2 运行确认准备、Others / steeljoint-line 字段骨架，
     以及 Others / steeljoint-line L2 运行确认准备。
     这些证据均不声明旧插件接受、真实工程图算法、AutoCAD L2 pass 或 golden。

TODO-026 / Golden = pending
  -> 用户明确说 golden 先不要，所以本轮不进入 golden 采集。

TODO-045 / M2-Drawing-014 = done
  -> 已补 `E-IDA-028 / E-DEV-067`，确认 `steeljoint-line / joints`、
     `Others / symbolcutIOS` 写出链、接头命令 handler 和额外 line/arc writer 分支。
     continuation 开始时旧 session 缺失，已通过 `idb_open` 重新打开
     `visualts_i64_todo045` 并复核关键函数。
     本节点只收敛证据和 GAP，不声明真实接头线算法、Others 几何算法、
     AutoCAD L2、旧插件接受或 golden。

TODO-046 / M2-Drawing-015 = done
  -> 已补 `E-IDA-029 / E-DEV-068`，确认 `JointRuler / JointDistbet / JointWeldLength`
     的对话框 / 配置链、`JointWeldLength / 2000.0` 半长公式、`pattern` raw byte `'L'`、
     `Others / symbolcutIOS` gate、额外弧线 / `DrawTaoTong` 条件和 IDA 止点。
     本节点只补参数绑定和运行触发证据，不声明真实接头线算法、Others 几何算法、
     AutoCAD L2、旧插件接受或 golden。

TODO-047 / M2-Drawing-016 = done
  -> 已补 `E-DEV-069`，确认当前旧图石运行样例采集在启动期就被阻塞：
     `VisualTS.exe` 先弹出标题为 `提示` 的阻塞框，截图可见文本前缀
     `请检查网线是否...`，尚未进入主界面、尚未打开 `SFL`、尚未导出新的 `DetailNN.stl`。
     本节点只记录 stop point，不声明旧运行样例已拿到、真实接头线算法、
     Others 几何算法、AutoCAD L2、旧插件接受或 golden。

TODO-048 / Evidence = done
  -> 已补 `E-IDA-030 / E-DEV-070`，静态闭合旧图石启动阻塞主链、
     `41 -> 许可已过期`、其他非 `0` 失败码回落到 `请检查网线是否接好`，
     并形成用户手工解除前置条件清单。
     本节点只补静态证据和手工清单，不声明旧图石已能正常启动、
     真实接头线算法、Others 几何算法、AutoCAD L2、旧插件接受或 golden。

TODO-049 / Evidence = done
  -> 已补 `E-DEV-071`，完成 agent 侧本机预检：
     网络基础在线、`SafeNet Sentinel / Sentinel LDK` 数据目录存在，
     但本地典型许可服务、监听端口和环境变量仍未出现。
  -> 已补 `E-IDA-031`，确认 `sub_14070C760` 会在许可上下文未 ready
     或对象映射失败时直接走 fallback；
     `请检查网线是否接好` 是宽兜底许可初始化失败文案，
     不能只按“网络断了”理解。
  -> 已补 `E-RUN-003`，用户已明确旧图石启动需要 USB 狗；
     同时明确新系统复刻不能继承 USB 狗依赖，目标仍是开源、无狗。
  -> 已补 `E-RUN-004`，用户确认插狗后可进入主界面、可打开 `SFL`，
     且不需要内网；现场还需要本地 HASP 安装条件。
     工作区 `HASPUserSetup` 已核查到 `HASPUserSetup.exe`、`hasplm.ini`、
     `haspvlib_23520.dll` 和安装说明文档；当前机器目标 HASP 目录尚不存在。
     当前节点已闭环，下一步转回非空运行样例采集。

TODO-050 / Evidence = blocked
  -> 已补 `E-IDA-032 / E-DEV-072`，
     确认当前不能由 agent 单独采到旧图石非空运行样例。
  -> 用户现场需要先完成 USB 狗 + HASP 条件，
     再手工打开旧图石、目标 SFL、接头命令并导出 DetailNN。
  -> 本轮已形成 barjoint / groupjoint / goujianjoint / featjoint
     三类采样入口和回填模板。

TODO-051 / Evidence = done
  -> 已补 `E-IDA-033 / E-DEV-073`，
     确认 `Others / symbolcutIOS` producer 链：
     `sub_14060C940 -> sub_14060A810 -> HVIEWPORT +840/+848
      -> sub_14061F970 -> sub_14053A3F0`。
  -> 本节点只补静态 producer 证据；
     不声明旧 UI caption、非空运行样例、真实接头线算法、
     Others 几何算法、AutoCAD L2 或 golden。

TODO-052 / Evidence = done
  -> 已补 `E-IDA-034 / E-DEV-074`，
     确认接头内部 command id / handler 表更完整，
     `sub_1406F37B0` Ribbon 构造函数未直接命中接头命令 ID。
  -> Dialog #427/#428 只证明接头参数字段存在；
     不能证明旧 UI caption 或 Ribbon / 右键触发路径。
  -> 本节点只补静态 UI/Ribbon stop point；
     不声明旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-053 / Evidence = done
  -> 已补 `E-IDA-035 / E-DEV-075`，确认动态 popup/context menu 机制、
     `sub_1405C2EF0 / sub_1406BA690` 关键路径和 `LoadMenuW` 菜单资源。
  -> 全量 MENU 资源、popup constructor immediate 和已知接头 command id
     普通 immediate 仍不能闭合旧 UI caption / 右键菜单项绑定。
  -> 本节点只补静态 stop point；不声明旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-054 / Evidence = done
  -> 已补 `E-IDA-036 / E-DEV-076`，确认 barjoint / groupjoint / segjoint /
     featjoint 走 selection item +13 业务对象和不同 type id predicate。
  -> 已确认 goujianjoint 直接筛 selection item +80 == 4，
     再遍历 item+120 链取 node+104 对象。
  -> 本节点只补 handler 对象筛选静态分类；
     不声明旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-055 / Evidence = done
  -> 已补 `E-IDA-037 / E-DEV-077`，确认 groupjoint 子链头 obj+80，
     子接头对象 child+72/+88/+96/+108/+112/+116/+128 字段语义。
  -> 已确认 `sub_1405E1D50 / sub_1405E1CC0 / sub_1405E1D20`
     都会先 `ENTITY::backup` 再写回字段。
  -> 已确认 feat/goujian clear adapter 走 obj+192 子链，
     segjoint clear/new dialog 字段入口已分类。
  -> 本节点只补动作函数字段语义；
     不声明 sub_1405DB6C0 完整几何算法、旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-056 / Evidence = done
  -> 已补 `E-IDA-038 / E-DEV-078`，确认 sub_1405DB6C0 的清旧链、
     period/phase/reverse 字段驱动、EDGE 链遍历、bounded curve 取点和
     end_indexed_polygon 调用点寄存器语义。
  -> 已确认 DB6C0 callers 覆盖 barjoint/groupjoint 的 new/move/rev 入口。
  -> 本节点只补静态几何重建主流程；
     不声明 owning 结构名、Dialog #428 确定链、旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-057 / Evidence = done
  -> 已补 `E-IDA-039 / E-DEV-079`，确认 steelbar / steelbargroup / seg_steelbargroup
     owner 类型层、Dialog #428 OK 链和 child+112 的 phase/position 语义。
  -> 本节点只补 owner / dialog 静态证据；
     不声明 generated node+112、standalone Apply、旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-058 / Evidence = done
  -> 已补 `E-IDA-040 / E-DEV-080`，确认 JoingSegDlg 静态 message map
     只含 3 个 radio handler，静态上未命中独立 Apply，
     且 generated node+112 已收紧为生成链上的 int mm 贡献字段。
  -> 本轮又把 done-node gate 补严到：
     run report 同名 json 必须存在，
     且 `pending_before_commit / pending_update_after_verification /
     waiting_rereview` 这类未最终闭环状态不能带着 `done` 落库。
  -> 本节点只补 message map / 字段边界静态证据；
     不声明旧中文 caption、运行期静态表外 Apply、旧运行样例、真实算法、AutoCAD L2 或 golden。

TODO-059 / Evidence = done
  -> 已补 `E-IDA-041 / E-DEV-081`，确认 `sub_1405DC6C0 = sum(generated node+112)`，
     旧 Excel 下料表 `焊头(个)` / `单下料长(mm)` 的落点，
     以及状态栏 pane3 的 `焊接 / 绑扎 / 套筒连接` 展示链。
  -> 本节点把 generated node+112 收紧到
     “接头 / 焊头计数展示链字段”口径；
     不写死最终旧调试名，不声明真实运行截图、AutoCAD L2 或 golden。

TODO-060 / Evidence = done
  -> 已补 `E-DEV-082`，完成现有候选工件排查、无效证据排除和最小手工清单闭环。
  -> 已确认 `1.xls` 是空白工作簿，`screenshot.png` 是 JDK 提示，
     `visualts_prompt_capture.png` 是旧启动阻塞提示，三者都不能当作运行证据。
  -> 本节点不声明真实 pane3 截图、真实 Excel 导出、AutoCAD L2 或 golden。

TODO-061 / Evidence = pending
  -> generated node+112 / 接头相关旧图石真实运行截图与 Excel 导出回填先记录、暂缓。
  -> 已补 `E-DEV-083`：runtime capture gate、README、capture_notes 模板和 xhigh 只读 review 修正。
  -> 当前 gate 已收紧到 hash / basename / 文件头 / Excel 路径口径，但按最新业务指令不再作为当前 next。
  -> 不自动安装 HASP，不自动启动旧图石，不实现真实接头线 / Others 几何算法。

TODO-062 / Evidence = done
  -> 已补 `E-IDA-042 / E-DEV-084`，把“生成工程图”入口收口到可开发 static stop point。
  -> 已确认 `工程图 / 输出 -> {36124, 35057}`；但 TODO-062 初版里 36124/35057 与 psallc/psexcel 的对应关系写反了，后续已由 TODO-063 纠偏。

TODO-063 / Evidence = done
  -> 已补 `E-IDA-043 / E-DEV-085`，完成命令映射纠偏：
     `36124 / 0x8D1C -> psallc -> sub_140600AA0`
     `35057 / 0x88F1 -> psexcel -> sub_140605B20`
     `0x8CD2 -> ysteelout`
  -> `Dialog #274` 已降级为 `Set2Dpage` 属性页，不再直接写成生成工程图设置窗口。
  -> 当前剩余缺口变成 caption / resource 和真正生成工程图弹窗候选。

TODO-064 / Evidence = done
  -> 已补 `E-IDA-044 / E-DEV-086`，静态闭合：
     `36124 = 生成工程图`
     `35057 = 下料表`
  -> 已确认旧资源采用 `描述/help \n caption` 模式；
     `36050 -> 输出钢筋几何 \n 输出钢筋` 为旁证。
  -> 已把 `sub_140600AA0` 里唯一可直接证明的 `DoModal`
     收窄到 `output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT`
     后置报告链。
  -> 已把 `Dialog #427` 降级为 `OptionFactory` 属性页，
     不再作为下料表弹窗真值。

TODO-065 / Evidence = done
  -> 已补 `E-DEV-087`，新增
     `docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/`
     模板目录。
  -> 已把生成工程图 / 下料表的运行确认清单收紧到：
     输出目录 listing / SHA256 / LastWriteTime / rerun 覆盖行为 /
     额外运行时弹窗 / 拒收伪工件规则。
  -> 已明确 `UnCutSteel.TXT` 只算可选旁证，
     不能单独证明 `生成工程图` 成功。

TODO-066 / Evidence = done
  -> 已完成真实工件回填：RUN-20260609-001 记录源 SFL 文件名/hash
     （.sfl 本体按 .gitignore 不纳入 git）、空 StyleRoot Detail.xml、
     非空 Detail01..04.stl、非空 下料表.xls 和截图。

TODO-067 / Evidence = done
TODO-068 / M2-Drawing-037 = done
  -> 已完成 DetailWriter 真实字段差异 P0：
     空 Detail.xml、Detail01 主图表格、副图空 StbTables
     和 StbTable 表级属性骨架。
TODO-069 / M2-Drawing-038 = done
  -> 已完成 StbRow smallTable / mirrorType / mirrorSEFlag 扩展属性骨架。
TODO-070 / M2-Drawing-039 = done
  -> 已完成 lineStb 直线段 StbGeo 字段条件化骨架。
TODO-071 / M2-RebarCreate-001 = done
  -> 已完成线配筋生成旧逻辑证据与 P0 切片准备。
  -> IDA MCP 已冻结 sgroupbarline 入口契约并写入 RebarGroupCreator P0 raw/evidence。
TODO-072 / M2-RebarCreate-002 = done
  -> 线配筋命令 handler P0 已接到 LegacySelectionRef -> LegacyRebarGeometryReader -> RebarGroupCreator -> SteelData 事务边界。
  -> edge-p0-surrogate 只作为 P0 选择代用，不等价旧完整 ENTITY_LIST。
TODO-073 / M2-RebarCreate-003 = done
  -> 线配筋 UI 到 AIS 显示 P0 已完成，命令成功后可显示创建的 SteelBarGroup。
  -> 不声明旧 HOOPS 样式、参数窗口、undo/dirty 或 golden。
TODO-074 / M2-RebarCreate-004 = done
  -> 线配筋参数窗口 P0：旧 UI 字段占位、默认值和 handler 参数接入。
  -> IDA MCP 未确认旧参数 Dialog 字段，继续保留 GAP-UI-REB-001。
TODO-075 / M2-RebarCreate-005 = done
  -> 线配筋选择预检与参数窗口顺序对齐 P0。
  -> 只让有效 edge 选择进入参数窗口；无选择或非 edge 不打开参数窗口。
TODO-076 / M2-RebarCreate-006 = done
  -> 线配筋旧 UI 失败提示与状态栏口径证据 P0。
  -> IDA MCP 已确认 handler / helper 无直接中文失败提示，公共链只收窄到 Input_float / Dialog #383。
  -> 不证明旧运行提示、状态栏 pane 或旧线配筋主参数窗口已闭合。
TODO-077 / M2-RebarCreate-007 = done
  -> 线配筋旧图石运行确认清单与工件门禁 P0。
  -> 只准备用户采样清单和门禁，不自动启动旧图石，不进入完整线配筋算法或 golden。
TODO-078 / M2-RebarCreate-008 = next
  -> 线配筋旧图石运行确认工件回填 P0。
  -> 只核对用户现场旧图石运行截图、状态栏、参数窗口、Dialog #383、hash 和步骤。
  -> 不自动启动旧图石，不安装 HASP，不进入完整线配筋算法或 golden。
```

工作目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件
```

正式开发目录：

```text
【图石钢筋1比1复刻】\app
```

本轮必须先读这些参考文档：

1. `【图石钢筋1比1复刻】\00_总览.md`
2. `【图石钢筋1比1复刻】\03_IDA命令证据.md`
3. `【图石钢筋1比1复刻】\11_需求证据追溯矩阵.md`
4. `【图石钢筋1比1复刻】\34_Phase1ReadinessGate实际运行记录.md`
5. `【图石钢筋1比1复刻】\46_CSE_v2Goal执行目标与Todo说明.md`
6. `【图石钢筋1比1复刻】\83_M2-Drawing-016真实接头线Others旧图石运行样例采集P0实现记录.md`
7. `【图石钢筋1比1复刻】\84_M2-Drawing-017旧图石启动阻塞提示链补证P0实现记录.md`
8. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
9. `【图石钢筋1比1复刻】\todo.csv`

如果任务涉及 UI / 命令入口，补读：

```text
01_功能操作矩阵.md
17_一期按钮追溯与命令占位矩阵.md
```

如果任务涉及旧编号 / 保存 / 新格式，补读：

```text
09_钢筋领域模型草案.md
16_seg_steelbargroup字段地图初稿.md
18_新设计文件格式替代SFL策略.md
```

本轮允许修改：

- 用户手工确认清单、运行记录、证据报告、缺口文档和 todo
- 必要的旧图石启动环境说明，但必须明确它不能替代运行确认
- 对应实现记录、build report、`03 / 11 / 34 / 46 / 84 / 99 / todo.csv`

本轮禁止修改或迁移：

- 父目录 `src/rebar/*`
- 父目录 `RebarCreationCommandService`
- 父目录 `EdgeToRebarFactory`
- 父目录 `FaceRebarGenerator`
- 父目录 `PolylineRebarGenerator`
- 任何 OCCT 直接造钢筋业务逻辑
- `domain/rebar` 中引入 AIS / OCCT / `TopoDS_ / AIS_ / BRep / TopAbs_`
- 把 OCCT HLR / section 结果直接当成旧图石工程图业务真相
- 把 TODO-045 扩成真实算法实现
- 在没有真实运行证据时声明 AutoCAD L2 通过
- 实现真实接头线 / Others 几何算法 / 点筋真实算法 / FaceEdge 生成规则 / 真实连续线 / 隐藏线 / 中心线 / 填充线 / 剖切线算法
- UI 新功能或命令入口扩展同轮实现
- golden 采集同轮实现
- 把缺证剖切线、隐藏线、填充线、标注或 AutoCAD 字段写成确定结论

本轮验收标准：

1. 只执行 TODO-049，不一次铺开完整工程图全部能力。
2. 基于 `E-IDA-030 / E-DEV-070 / E-IDA-031` 的手工清单，由用户自己确认许可 / 服务 / 网络 / license file 环境并手工重试旧图石启动。
   本机预检 `E-DEV-071` 已完成，不需要再重复做 agent 侧环境扫描。
3. 如果旧图石仍无法进入主界面，必须记录完整弹框文本、环境状态和下一轮所需输入。
4. 如果旧图石可以进入主界面，必须记录成功进入主界面的证据，并把下一步切回非空运行样例采集，而不是直接写算法。
5. 不得把字段骨架、OCCT 能力或 AutoCAD L2 清单写成算法证据。
6. 默认 CTest 通过。
7. readiness gate 严格模式通过。
8. domain/rebar + drawing + project OCCT / AIS 泄漏扫描通过。
9. 涉及代码、测试、构建脚本，commit 前必须执行 xhigh 只读 review；若只是 docs / 手工确认节点，可明确写 `xhigh not_required_docs_only`。
10. 更新实现记录、build report、`03 / 11 / 34 / 46 / 84 / 99 / todo.csv`。
11. `todo.csv` 中 `TODO-049` 只在用户手工确认结果和文档闭合后更新；不能假装旧图石已可运行、真实接头线 / Others 几何算法、AutoCAD L2、完整工程图或 golden 已完成。

本轮完成后必须停止，输出阶段复盘：

```text
完成了什么
验证了什么
还缺什么
下一阶段建议回到旧图石非空运行样例采集还是继续补许可环境
commit / tag / push 状态
```

不要在同一个 goal 内继续做真实工程图算法、golden 采集、UI 新功能、AutoCAD L2 pass 声明或多个专项。
### 长期方向（只作护栏，不作为本轮 Goal）

目标：持续推进《图石钢筋 1 比 1 复刻》正式 `app` 开发，直到具备按旧 VisualTS 证据复刻钢筋创建、编辑、统计、出图的工程条件。

工作目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件
```

主项目目录：

```text
【图石钢筋1比1复刻】
```

正式开发目录：

```text
【图石钢筋1比1复刻】\app
```

路线必须保持为：

```text
Qt6
  -> 替代 MFC / Codejock，只负责新界面和命令入口

OCCT AIS
  -> 替代 HOOPS，只负责三维显示、旋转缩放、选择、高亮

OCCT 几何 API
  -> 替代 ACIS，只负责 EDGE / FACE / 曲线 / 求交 / 距离 / 投影 / 裁剪 / 剖切 / 扫掠等几何能力

LegacyGeometryAdapter
  -> 把 OCCT 包装成旧 VisualTS 熟悉的 EDGE / FACE / ENTITY_LIST / 曲线 / 选择引用语义

VisualTS 复刻业务层
  -> 按 IDA / SFL / Detail / 旧图石运行证据 1:1 复刻钢筋创建、编辑、统计、出图逻辑

Detail / 新设计文件格式输出层
  -> 输出 Detail.xml + DetailNN.stl，并用新工程格式替代 .sfl 主保存格式
```

一句话硬约束：

```text
不是用 OCCT 直接重写一个差不多的钢筋软件。
而是用 Qt6 / OCCT 替换旧商业底座，
钢筋业务规则按旧图石 VisualTS 证据复刻。
```

### 启动 Goal 后必须先读的参考文档

先按顺序读取这些文件，建立当前事实边界：

1. `【图石钢筋1比1复刻】\00_总览.md`
   - 看文档体系、当前阶段、已完成 M1-App-001 到 M1-App-012。
2. `【图石钢筋1比1复刻】\06_技术路线与替代方案.md`
   - 看 Qt6 / OCCT 替代边界，确认不是 OCCT 直接重写钢筋。
3. `【图石钢筋1比1复刻】\07_1比1复刻实施路线.md`
   - 看总路线和阶段拆解。
4. `【图石钢筋1比1复刻】\08_开发命令契约.md`
   - 看旧命令如何映射到新系统命令、状态机和 dirty 口径。
5. `【图石钢筋1比1复刻】\09_钢筋领域模型草案.md`
   - 看 SteelBar / SteelBarSegment / SteelBarGroup 等领域模型边界。
6. `【图石钢筋1比1复刻】\11_需求证据追溯矩阵.md`
   - 看 Requirement / Evidence / GAP 如何互相追溯。
7. `【图石钢筋1比1复刻】\13_Detail字段映射矩阵.md`
   - 看 Detail 工程图包字段如何映射到新领域模型。
8. `【图石钢筋1比1复刻】\15_线配筋与弧形组专项初稿.md`
   - 看 sgroupbarline / sgroupbararc 相关旧逻辑。
9. `【图石钢筋1比1复刻】\16_seg_steelbargroup字段地图初稿.md`
   - 看 seg_steelbargroup 字段、常量、ACIS 调用链和旧算法线索。
10. `【图石钢筋1比1复刻】\18_新设计文件格式替代SFL策略.md`
    - 看为什么新系统不用 `.sfl` 做主保存格式，以及新格式如何结合 SFL 业务语义和 OCCT 几何引用。
11. `【图石钢筋1比1复刻】\23_父目录源码参考边界与路线纠偏.md`
    - 看父目录只能当零件库参考，不能迁移父目录钢筋业务逻辑。
12. `【图石钢筋1比1复刻】\24_新设计文件格式Schema与Fixture草案.md`
    - 看 `*.tsrebar`、`legacyObject.raw`、`geometryRef`、`binding`、`evidence`。
13. `【图石钢筋1比1复刻】\32_Validator实现契约与错误码总表.md`
    - 看新工程格式 validator 和错误码口径。
14. `【图石钢筋1比1复刻】\34_Phase1ReadinessGate实际运行记录.md`
    - 看当前 readiness gate 状态和 M1-Formal 放行条件。
15. `【图石钢筋1比1复刻】\35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
    - 看 UI、LegacyUiCommandMap、LegacyGeometryAdapter 和钢筋创建责任。
16. `【图石钢筋1比1复刻】\36_正式Qt6_OCCT工程架构与首个实现切片.md`
    - 看正式 `app` 架构、父目录迁移策略和首批切片。
17. `【图石钢筋1比1复刻】\40_M1-App-004LegacyGeometryAdapterP0实现记录.md`
18. `【图石钢筋1比1复刻】\41_M1-App-005LegacyGeometryAdapterP1实现记录.md`
19. `【图石钢筋1比1复刻】\42_M1-App-006LegacyGeometryAdapterP2A实现记录.md`
20. `【图石钢筋1比1复刻】\43_M1-App-007LegacyGeometryAdapterP2B实现记录.md`
21. `【图石钢筋1比1复刻】\44_M1-App-008LegacyGeometryAdapterP2C实现记录.md`
22. `【图石钢筋1比1复刻】\45_M1-App-009LegacyGeometryAdapterP3A实现记录.md`
23. `【图石钢筋1比1复刻】\47_M1-App-010LegacyGeometryAdapterP3B实现记录.md`
24. `【图石钢筋1比1复刻】\48_M1-App-011LegacyGeometryAdapterP3C实现记录.md`
25. `【图石钢筋1比1复刻】\49_M1-App-012LegacyGeometryAdapterP3D实现记录.md`
26. `【图石钢筋1比1复刻】\50_M1-App-013LegacyGeometryAdapterP3E实现记录.md`
27. `【图石钢筋1比1复刻】\51_M1-App-014LegacyWireChain实现记录.md`
    - 看 adapter 已有能力，避免重复实现。
28. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
    - 看当前缺口，所有不确定项必须回写这里。
29. `【图石钢筋1比1复刻】\todo.csv`
    - 看当前任务看板，只执行 `status=next` 或最高优先级可执行任务。

### 按任务补读的参考文档

如果任务涉及 UI / 菜单 / 命令入口，补读：

- `01_功能操作矩阵.md`
- `02_界面窗口参数矩阵.md`
- `17_一期按钮追溯与命令占位矩阵.md`

如果任务涉及 IDA / 旧 VisualTS 调用链，补读：

- `03_IDA命令证据.md`
- `15_线配筋与弧形组专项初稿.md`
- `16_seg_steelbargroup字段地图初稿.md`

如果任务涉及 SFL / 旧样本 / 新设计文件格式，补读：

- `04_SFL样本证据.md`
- `18_新设计文件格式替代SFL策略.md`
- `24_新设计文件格式Schema与Fixture草案.md`
- `25_新设计文件格式SFL_OCCT联合开发任务拆解.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`
- `31_STEP选择ID实际运行记录模板与样本清单.md`
- `33_Qt6应用SaveOpen与Binding修复契约.md`

如果任务涉及 Detail / 工程图 / 下料表，补读：

- `05_Detail工程图包证据.md`
- `13_Detail字段映射矩阵.md`
- `20_DetailWriter输出事务契约.md`

如果任务涉及 Qt6 / OCCT / 依赖合规，补读：

- `21_QtOCCT依赖许可证门禁.md`
- `27_Qt6_OCCT开发入口门禁与首批工件清单.md`
- `28_QtOCCT依赖清单与发布材料模板.md`
- `DEPENDENCIES.md`
- `THIRD_PARTY_NOTICES.md`

如果任务涉及正式 app 已完成切片，补读：

- `37_M1-App-001实现计划.md`
- `38_M1-App-002最小AISViewer显示实现记录.md`
- `39_M1-App-003选择系统实现记录.md`
- `40_M1-App-004LegacyGeometryAdapterP0实现记录.md`
- `41_M1-App-005LegacyGeometryAdapterP1实现记录.md`
- `42_M1-App-006LegacyGeometryAdapterP2A实现记录.md`
- `43_M1-App-007LegacyGeometryAdapterP2B实现记录.md`
- `44_M1-App-008LegacyGeometryAdapterP2C实现记录.md`
- `45_M1-App-009LegacyGeometryAdapterP3A实现记录.md`
- `47_M1-App-010LegacyGeometryAdapterP3B实现记录.md`
- `48_M1-App-011LegacyGeometryAdapterP3C实现记录.md`
- `49_M1-App-012LegacyGeometryAdapterP3D实现记录.md`
- `50_M1-App-013LegacyGeometryAdapterP3E实现记录.md`
- `51_M1-App-014LegacyWireChain实现记录.md`
- `52_M1-App-015LegacyGeometryAdapterOffsetSpike实现记录.md`
- `53_M1-App-016LegacyGeometryAdapterSectionSpike实现记录.md`
- `54_M1-App-017LegacyGeometryAdapterSweepBoundary实现记录.md`
- `55_M1-App-018RebarDomainModelFreezeP1实现记录.md`

### 当前已知状态

当前正式 `app` 已完成：

- `M1-App-001`：Qt6 app 骨架、五个旧图石页签、命令注册、STEP import probe。
- `M1-App-002`：最小 OCCT AIS Viewer 和 STEP 显示。
- `M1-App-003`：face / edge / vertex 选择系统、`selection-v1` 稳定引用。
- `M1-App-004`：`LegacyGeometryAdapter P0`，EDGE / FACE 基础几何摘要。
- `M1-App-005`：`LegacyGeometryAdapter P1`，bbox、采样、boundary loop、fingerprint、诊断矩阵。
- `M1-App-006`：`LegacyGeometryAdapter P2A`，edge 切向、距离、最近点对。
- `M1-App-007`：`LegacyGeometryAdapter P2B`，face boundary edge stableId、edge-face 接触/重叠代表点。
- `M1-App-008`：`LegacyGeometryAdapter P2C`，edge 参数区间、子段长度、bbox、采样。
- `M1-App-009`：`LegacyGeometryAdapter P3A`，edge split by parameter。
- `M1-App-010`：`LegacyGeometryAdapter P3B`，edge point projection 和 split by projected point。
- `M1-App-011`：`LegacyGeometryAdapter P3C`，endpoint inward trim summary。
- `M1-App-012`：`LegacyGeometryAdapter P3D`，point to edge group minimum distance summary。
- `M1-App-013`：`LegacyGeometryAdapter P3E`，point list spline rebuild summary。
- `M1-App-014`：`LegacyWireChain`，edge refs to wire chain summary。
- `M1-App-015`：`LegacyGeometryAdapter offset preview`，edge ref to offset curve preview summary。
- `M1-App-016`：`LegacyGeometryAdapter section preview`，face-plane section preview summary。
- `M1-App-017`：`LegacyGeometryAdapter sweep preview`，edge circular sweep preview summary。
- `M1-App-018`：`domain/rebar` 钢筋领域模型冻结 P1，SteelData / SteelBarGroup / SteelBar / SteelBarSegment 字段可编码。
- `M1-App-019`：旧命令契约绑定 P1，LineGroup / ArcGroup / TrimByLine / TrimByFace 可查询、可注册稳定 NotImplemented placeholder。
- `TODO-020`：IDA MCP 旧线筋 / 弧筋链补证据，形成 `E-IDA-022`，可支撑 `TODO-021` 的 P0 业务创建 spike。
- `M1-App-020`：旧线筋 / 弧筋创建算法 P0，RebarGroupCreator 可输出 domain SteelBarGroup。
- `M1-App-021`：AIS 钢筋显示映射 P0，RebarAisPresentationAdapter 可输出 AIS_Shape。
- `M1-App-022`：新设计文件格式 runtime P1，TsRebarProjectRuntime 可保存 / 读取 STEP 来源、selection-v1 refs、rebar groups、binding 和 evidence。
- `M1-App-023`：DetailWriter P1，DetailWriter 可把 domain SteelData / SteelBarGroup / SteelBar / SteelBarSegment 映射为 Detail.xml + Detail01.stl 首批字段。
- `TODO-025 / M1-App-024`：旧图石输出钢筋 STP 样本入库验证，`123.stp` 已固定为旧图石钢筋几何 witness；该证据只证明几何可被 OCCT 稳定读取，不证明旧业务算法或新系统可生成同样几何。
- `TODO-027 / M2-UI-001`：旧 UI 功能入口 P1，`17` 矩阵一期入口已接入 `CommandId / LegacyUiCommandMap / CommandRegistry / Qt6 QAction`，并通过 `tsrebar_app --smoke` 校验追溯 metadata；该证据只证明入口占位和命令契约，不证明业务算法已实现。
- `TODO-028 / M2-Gate-001`：CSE readiness gate 扩展，RouteGuardrail 已接入 `Phase1.ReadinessGate`，自动检查 OCCT/AIS 泄漏、父目录 rebar 业务引用、todo 状态和 done 节点报告；该证据只证明路线护栏可自动检查，不证明旧业务算法已完成。
- `TODO-029 / M2-Edit-001`：Rebar.Edit.Move / 钢筋移动 P0，IDA MCP 已补证 `barmove -> Input_Choice -> translate_transf` 移动链，domain/rebar 新增 `RebarEditMoveService`；该证据只证明领域层整体平移 P0，不证明完整旧 ACIS topology mutation、dirty/undo 或 golden。
- `TODO-032 / M2-Edit-002`：Rebar.Edit.Copy / 钢筋拷贝 P0，IDA MCP 已补证 `scopy -> Input_Choice copyFlag=1 -> sub_1405989C0 -> sub_1405AA5D0` 拷贝链，domain/rebar 新增 `RebarEditCopyService`；该证据只证明领域层复制后累计平移 P0，不证明完整旧 ACIS topology clone、旧编号、dirty/undo 或 golden。
- `TODO-030 / M2-Stats-001`：钢筋统计 / 下料表 P0，IDA MCP 已补证 Detail / XML writer 侧 `StbTable / MaterialTable` 写出链，domain/rebar 新增 `RebarScheduleService`，DetailWriter 已消费同一 schedule service；该证据只证明首批统计字段和材料聚合 P0，不证明完整 `sameGrpNum` 合并规则、旧 `singleMass` 来源、`Volume722` ACIS 等价、AutoCAD L2 或 golden。
- `TODO-031 / M2-Drawing-001`：DetailWriter 多图纸 DetailNN P0，IDA MCP 已补证旧 `Detail01..Detail09 / Detail10+` 命名规则；DetailWriter 可按多个 `DrawingView` 输出多张 `DetailNN.stl`，L0/L1 校验覆盖所有生成图纸，成功安装会删除旧多余 DetailNN 并保留非 Detail 文件，安装失败恢复旧 Detail 包；该证据只证明多图纸命名和事务 P0，不证明完整工程图、剖切线、隐藏线、填充线、AutoCAD L2 或 golden。
- `TODO-033 / M2-Drawing-002`：AutoCAD L2 导入验证 P0，新增 `detail_l2_fixture_probe`，用正式 `DetailWriter` 生成 `Detail.xml + Detail01.stl + Detail02.stl + Detail03.stl` 三图纸验证包并记录 hash；旧 `FDrawing.arx / FDrawingObj.dbx` 文件存在且 hash 已记录；当前本机未发现 `acad.exe / accoreconsole.exe`，所以 AutoCAD L2 自动导入未运行；该证据只证明验证包和阻塞记录齐备，不证明旧插件已接受新包、完整工程图、剖切线、隐藏线、填充线或 golden。
- `TODO-034 / M2-Drawing-003`：Detail复杂字段静态证据 P0，已从旧 `Detail01.stl` 样例整理 `continue-line / hidden-line / central-line / section-line / hatch-line / Others / steeljoint-line` 容器、section Line/Arc、pointStb 和 FaceEdge 字段；IDA MCP 复核 `FDrawing.arx` 中 `CViewInfo / CWSNLineDim / CWSNPointDim / CWSNSteelBarTable / CWSNMaterialTable` 等旧插件对象符号；该证据只证明静态字段和插件对象模型存在，不证明 AutoCAD L2 通过、旧插件容忍度、剖切线 / 隐藏线 / 填充线 / 接头线算法或 golden。
- `TODO-035 / M2-Drawing-004`：DetailWriter复杂字段骨架 P0，正式 app `DetailWriter` 已输出 `PartDetailDrawing` 复杂容器骨架、`General-Info` 首批扩展默认字段和 `pointStb StbGeo shapeType=C` 字段骨架；该证据只证明字段骨架可由 writer 输出，不证明 AutoCAD L2 通过、旧插件接受新包、`CompanyName` 旧默认值、FaceEdge、ZValue、剖切线 / 隐藏线 / 填充线 / 接头线算法或 golden。
- `TODO-036 / M2-Drawing-005`：AutoCAD L2复杂字段骨架导入验证 P0，`detail_l2_fixture_probe` 已生成 TODO-036 三图纸复杂骨架包，离线确认 `PartDetailDrawing num=8`、复杂容器和 `General-Info` 首批扩展字段；旧 `FDrawingObj.dbx / FDrawing.arx` 文件存在且 hash 已记录；当前本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD 注册表根键存在但无版本子项，所以 AutoCAD L2 自动导入未运行；xhigh 已指出并修正 pointStb L2 变量污染，本轮不声明旧插件接受、pointStb L2、FaceEdge、完整工程图或 golden。
- `TODO-037 / M2-Drawing-006`：DetailWriter pointStb / FaceEdge 兼容字段骨架 P0，正式 app `DetailWriter` 已输出 `pointStb StbGeo shapeType=C` 和显式 `FaceEdge shapeType=L/A` 字段骨架；`detail_l2_fixture_probe --fixture point-face-edge` 已生成独立三图纸包并离线确认 `pointFaceEdge.passed=true`；当前本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD 注册表根键存在但无版本子项，所以 AutoCAD L2 自动导入仍未运行；本轮不声明旧插件接受、点筋生成算法、FaceEdge 生成规则、完整工程图或 golden。
- `TODO-038 / M2-Drawing-007`：DetailWriter section-line Line/Arc/ZValue 字段骨架 P0，正式 app `DetailWriter` 已输出 `section-line / lines / LineN` 和 `section-line / Arcs / ArcN` 字段骨架，并保留旧样例确认的 `ZValue` 字符串；`detail_l2_fixture_probe --fixture section-line` 已生成独立三图纸包并离线确认 `sectionLine.passed=true`；本轮不声明旧插件接受、真实剖切线算法、隐藏线 / 填充线 / 接头线算法、`ZValue` 语义、完整工程图或 golden。
- `TODO-039 / M2-Drawing-008`：DetailWriter section-line AutoCAD L2 运行确认准备 P0，复用 `section-line` 独立变量生成 TODO-039 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash，并确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD 注册表根键存在但无版本子项，所以 `autocadL2=not_run`；默认 CTest、strict readiness 和保护层 OCCT/AIS 泄漏扫描已通过；本轮不声明旧插件接受、AutoCAD L2 通过、真实剖切线算法、`ZValue` 语义、完整工程图或 golden。
- `TODO-040 / M2-Drawing-009`：DetailWriter 线容器字段骨架 P0，正式 app `DetailWriter` 已输出 `continue-line / hidden-line / central-line / hatch-line` 下的 `LineN start_x/start_y/end_x/end_y/ZValue` 字段骨架；`detail_l2_fixture_probe --fixture line-containers` 已生成独立三图纸包并离线确认 `lineContainers.passed=true`；本轮不声明旧插件接受、AutoCAD L2 通过、真实连续线 / 隐藏线 / 中心线 / 填充线算法、完整工程图或 golden。
- `TODO-041 / M2-Drawing-010`：DetailWriter line-containers AutoCAD L2 运行确认准备 P0，复用 `line-containers` 独立变量生成 TODO-041 三图纸 Detail 包和手工 L2 确认清单，记录 FDrawing 插件 hash、旧样例包 hash 和新包 hash，并确认本机未发现 `acad.exe / accoreconsole.exe`，AutoCAD 注册表根键存在但无版本子项，所以 `autocadL2=not_run`；本轮不声明旧插件接受、AutoCAD L2 通过、真实连续线 / 隐藏线 / 中心线 / 填充线算法、完整工程图或 golden。

当前最新验证状态：

```text
app 默认 CTest = 20/20 pass
readiness gate = M1-Formal-Ready, 84/84 pass
domain/rebar + drawing + project OCCT 边界 = pass
TODO-030 xhigh 复审 = allow_commit
TODO-031 xhigh review = allow_commit
TODO-033 xhigh review = allow_commit
TODO-034 验证 = CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass；xhigh first review found doc-status drift and fix is applied
TODO-035 验证 = CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-036 验证 = complexSkeleton.passed=true, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2 not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed
TODO-037 验证 = pointFaceEdge.passed=true, pointGroupCount=2, pointGeoCount=2, faceEdgeCount=2, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2 not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-038 验证 = sectionLine.passed=true, lineCount=1, arcCount=1, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-039 验证 = sectionLine.passed=true, lineCount=1, arcCount=1, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-040 验证 = lineContainers.passed=true, containerCount=4, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh round2 allow_commit
TODO-041 验证 = lineContainers.passed=true, containerCount=4, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-042 验证 = pointFaceEdge.passed=true, pointGroupCount=2, pointGeoCount=2, faceEdgeCount=2, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh allow_commit
TODO-043 验证 = othersSteeljoint.passed=true, Others empty, steeljoint-line/joints present, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 17/17 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, agent closed
TODO-044 验证 = othersSteeljoint.passed=true, Others empty, steeljoint-line/joints present, acad.exe/accoreconsole.exe not_found, AutoCAD registry root keys exist but no version child groups, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, agent closed
TODO-046 验证 = JointWeldLength half-length formula closed, pattern raw byte = 0x4C('L'), Others gate = *(v8+848)&&a4==0, extra arc branch ties to DrawTaoTong, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh not_required_docs_only
TODO-047 验证 = VisualTS startup stop point recorded, mainWindowTitle=提示, visible text prefix=请检查网线是否..., SFL not opened, runtime non-empty sample not collected, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh not_required_docs_only
TODO-048 验证 = startup chain closed, error41=许可已过期, otherNonZeroFallback=请检查网线是否接好, manualPrereqChecklistReady=true, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh not_required_docs_only
TODO-051 验证 = symbolcutIOS producer chain traced, HVIEWPORT+840/+848 producer confirmed, uiChineseCaptionClosed=false, nonEmptyRuntimeSampleCollected=false, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh not_required_docs_only
TODO-052 验证 = static UI/Ribbon stop point, joint command handler table traced, direct Ribbon immediate hit=false, dialog resources confirmed, uiChineseCaptionClosed=false, nonEmptyRuntimeSampleCollected=false, autocadL2=not_run, CTest 18/18 pass, readiness 84/84 pass, OCCT leak scan pass, xhigh not_required_docs_only
TODO-053 验证 = dynamic context menu mechanism traced, popup constructors=sub_1405C2EF0/sub_1406BA690, LoadMenuW resources=0xAD/0xF8, all MENU known joint hits=0, known joint immediate matches=false, uiChineseCaptionClosed=false, contextMenuBindingClosed=false, nonEmptyRuntimeSampleCollected=false, autocadL2=not_run, xhigh not_required_docs_only
TODO-054 验证 = joint handler object filters classified, bar/group/seg/feat source=selection item +13 with type predicates, goujian source=selection item +80==4 plus item+120 child chain node+104, uiChineseCaptionClosed=false, nonEmptyRuntimeSampleCollected=false, autocadL2=not_run, xhigh not_required_docs_only
TODO-055 验证 = joint handler action field semantics traced, groupjoint obj+80 child chain and child+72/+88/+96/+108/+112/+116/+128 fields classified, backup write helpers closed, feat/goujian obj+192 clear adapter traced, segjoint clear/dialog init classified, full sub_1405DB6C0 geometry algorithm still open, autocadL2=not_run, xhigh not_required_docs_only
TODO-056 验证 = DB6C0 static rebuild core traced, child+88/+108/+112/+116 and node+72/+80/+88/+104/+112 fields classified, reverse traversal and helper chain closed, bounded curve parameter sampling and end_indexed_polygon(rcx=node, rdx=point, r8d=intMmIndex) confirmed, owning structure names/dialog apply/runtime sample still open, autocadL2=not_run, xhigh review completed, critical fixed, important fixed
TODO-069 验证 = StbRow smallTable / mirrorType / mirrorSEFlag 字段骨架已输出，CTest 18/18 pass, readiness unit 28/28 pass, strict readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, one-shot reviewer process exited
TODO-070 验证 = lineStb + shapeType=L 的 StbGeo 字段集已收窄到 start/end/offset，detail_writer_tests TDD red->green, CTest 18/18 pass, readiness unit 29/29 pass, strict readiness 84/84 pass, OCCT leak scan pass, xhigh needs_fix important fixed, one-shot reviewer process exited
TODO-071 验证 = sgroupbarline 表项、sub_1404DE720 入口契约和 sub_1404D10C0 公共创建调用已由 IDA MCP 复核，RebarGroupCreator P0 raw/evidence 已写入 E-IDA-045 / E-DEV-093，rebar_group_creator_tests TDD red->green, CTest 18/18 pass, readiness unit 31/31 pass, strict readiness 84/84 pass, xhigh needs_fix important fixed, agent closed
TODO-072 验证 = RebarLineGroupCommandHandler 已接入 CommandId::RebarLineCreate，测试覆盖 empty selection / wrong type / valid edge / geometry failure / normalizer failure，CTest 19/19 pass, readiness unit 32/32 pass, strict readiness 84/84 pass, domain/rebar + command OCCT leak scan pass, xhigh allow_commit，Franklin reviewer 已关闭
TODO-073 验证 = Rebar.Create.LineGroup 成功后已通过 RebarAisPresentationAdapter / OccViewerWidget 显示新增 SteelBarGroup，line_group_display_smoke_123 覆盖失败路径不刷新、成功路径显示计数增加、连续两次成功创建显示不同新 group，CTest 20/20 pass, readiness unit 33/33 pass, strict readiness 84/84 pass, domain/rebar + command OCCT leak scan pass, xhigh needs_fix 且 Critical / Important 已修复，Socrates reviewer 已关闭
TODO-074 验证 = LineGroupParameterDialog P0 已接入 Rebar.Create.LineGroup，参数传递覆盖默认值和用户修改值，取消/失败不污染 SteelData 且不刷新 AIS，IDA MCP 未确认旧 Dialog 字段字符串，CTest 21/21 pass, readiness unit 34/34 pass, strict readiness 84/84 pass, domain/rebar + command OCCT leak scan pass, xhigh allow_commit，Erdos reviewer 已关闭
TODO-075 验证 = Rebar.Create.LineGroup 已先做当前选择预检再打开 LineGroupParameterDialog，无选择 / 非 edge 不打开参数窗口、不污染 SteelData 且不刷新 AIS，有效 edge 保留 TODO-074 参数传递能力，CTest 21/21 pass, readiness / leak / xhigh 结果见 113 和 m2_rebar_create_005_run_001
TODO-076 验证 = IDA MCP 已确认 sub_1404DE720 / sub_14054B410 无直接中文失败提示字符串，sub_1404D10C0 只收窄到公共 ACIS outcome / part state 链和 Input_float / Dialog #383；Dialog #383 不是旧线配筋主参数窗口；旧运行提示和状态栏 pane 仍需 TODO-077 现场采样清单闭合
```

当前下一步：

```text
TODO-078 / 线配筋旧图石运行确认工件回填 P0
  -> TODO-071 已完成 sgroupbarline 入口契约冻结
  -> TODO-072 已完成 LineGroup command handler P0 事务接入
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0
  -> TODO-074 已完成 LineGroup 参数窗口 P0
  -> TODO-075 已完成选择预检与参数窗口顺序对齐 P0
  -> TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point
  -> TODO-077 已完成旧图石运行确认清单、截图/输出工件模板和拒收伪工件门禁
  -> 下一轮只回填和核对用户现场旧图石运行工件
  -> 不自动安装 HASP，不自动启动旧图石
  -> 不用 OCCT 直接重写钢筋业务，不迁入父目录 rebar 业务
  -> 不声明完整线配筋、AutoCAD L2 通过或 golden
```

原因：

```text
TODO-067 已经把真实字段差距列清楚。
TODO-068 已经把最明显的旧包格式差异先修正。
TODO-069 已经补齐 StbRow 扩展属性骨架。
TODO-070 已经收窄 lineStb 直线段 StbGeo 字段集合。
TODO-071 已经把 sgroupbarline 旧入口契约冻结到 P0 creator raw/evidence。
TODO-072 已经把命令入口接到创建事务边界。
TODO-073 已经把创建结果接到可见 AIS 反馈。
TODO-074 已经把参数窗口和参数传递接到命令入口。
TODO-075 已经把 UI 顺序收窄到“先选择预检，再打开参数窗口”。
TODO-076 已经用 IDA MCP 把旧 handler/helper 的直接失败提示证据收窄到 static stop point：
  handler/helper 内没有直接中文失败提示；
  Dialog #383 是公共 Input_float，不是旧线配筋主参数窗口。

当前最值得继续推进的是线配筋生成主线：
  用 TODO-077 已准备好的采样模板，
  回填真实旧图石运行截图、状态栏 pane、参数窗口字段和 Dialog #383 运行流程。

这比继续抠 Detail 小字段更贴近当前成熟度瓶颈：
  线配筋还没有完整旧运行 UI 提示、状态栏口径和旧默认值链，
  下一步应先准备运行确认清单，而不是用 OCCT 直接写一个差不多的生成器。
golden 采集 TODO-026 仍按用户要求保持 pending。
```

### 执行规则

每轮 goal 模式按这个闭环执行：

1. 先读 `todo.csv`，选择 `status=next` 的任务。
2. 如果没有 `next`，选择依赖已满足的最高优先级 `P0 pending`。
3. 读取该任务 `evidence` 字段指向的文档。
4. 读取相关代码和测试，确认现有模式。
5. 先补测试或测试用例，再改实现。
6. 默认只推进一个 M1 切片，不同时铺开 UI、几何、钢筋业务和工程图。
7. 修改后运行默认 CTest。
8. 运行 readiness gate 或对应专项 gate。
9. 涉及代码、测试、构建脚本的节点，验证通过后、commit 前执行 xhigh 只读 review。
10. xhigh 只给审查结论；Critical / Important 必须由主流程 agent 修复或写明技术反驳理由。
11. 修复后重新运行受影响验证。
12. 更新实现记录文档、build report、`11_需求证据追溯矩阵.md`、`99_缺口和待确认项.md`。
13. 更新 `todo.csv`：完成项改为 `done`，下一个可执行项改为 `next`。

### 禁止事项

禁止：

- 把父目录 `src/rebar/*` 当作旧图石业务真相迁入。
- 把 `RebarCreationCommandService`、`EdgeToRebarFactory`、`FaceRebarGenerator`、`PolylineRebarGenerator` 当主线复用。
- 在 `domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_` 等 OCCT / AIS 细节。
- 用“OCCT 能怎么做”替代“旧图石怎么做”。
- 不确定旧逻辑时直接写死结论。
- CTest 或 gate 失败时继续堆新功能。
- 因为没有 golden 就跳过证据追溯。
- 让 xhigh agent 修改文件、格式化、commit、tag、push。
- 代码节点跳过 xhigh 只读 review 后提交。

允许：

- 参考父目录的 STEP/STP 导入、XCAF 遍历、AIS viewer、选择 ID、OCCT API 写法。
- 在 `LegacyGeometryAdapter` 内部使用 OCCT。
- 在 presentation / viewer 层使用 AIS。
- 在文档中把低置信结论明确标为 gap。
- IDA MCP 可用时优先查旧函数、调用链、常量、字段。

### IDA / 旧图石确认规则

遇到旧业务不确定时，优先级如下：

1. IDA MCP 查询旧 VisualTS 函数、调用链、常量和字段。
2. 旧图石软件运行确认，记录截图、操作步骤、输出文件、hash。
3. SFL / Detail / STP 样本交叉验证。
4. 父目录代码只能作为 OCCT 工程写法参考，不能关闭旧业务证据缺口。

如果 IDA MCP 不可用，要把阻塞原因写入 `99_缺口和待确认项.md`，不能假装已经确认。

成功标准：

- `todo.csv` 中 P0 / P1 任务按依赖顺序完成，状态、证据和验收结果可追溯。
- `app` 默认 CTest 通过，readiness gate 通过。
- `domain/rebar` 不出现 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_` 等 OCCT / AIS 细节。
- 每个已实现功能都有实现记录、测试报告和需求证据追溯更新。
- 每个代码节点都有 xhigh 只读 review 结论；Critical / Important 已处理或有技术反驳记录。
- 旧逻辑不确定时，优先用 IDA MCP 或旧图石运行确认闭合，不用父目录代码替代旧图石证据。

## CSE v2 Control Contract

- **Primary Setpoint**：下一轮只完成 `TODO-078 / 线配筋旧图石运行确认工件回填 P0`，使用 TODO-077 采样模板核对用户现场旧图石运行工件，回填旧提示、状态栏 pane、参数窗口字段和 Dialog #383 运行流程。
- **Acceptance**：`docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture` 中的 `capture_notes.md / hashes.txt / 截图 / listing` 至少覆盖无选择、选错对象、有效对象、Dialog #383 标题/标签/默认值、参数窗口字段、状态栏 pane、确认/取消行为、输出前后截图/hash；拒收伪工件规则生效；更新 02 / 08 / 11 / 35 / 46 / 99 / todo / run report；默认 readiness gate、OCCT 泄漏检查通过；不因部分工件缺失声明旧 UI 全部闭合。
- **Guardrail Metrics**：不能用 OCCT 能怎么做替代旧图石怎么做；不能迁入父目录 rebar 业务；不能让 `domain/rebar` 依赖 TopoDS/AIS/BRep/TopAbs；不能伪造 IDA 或旧运行证据；不能在一个节点里铺开面配筋、接头、Excel、Detail 字段继续扩张或 golden。
- **Sampling Plan**：先读 `todo.csv / 02 / 03 / 08 / 09 / 15 / 16 / 35 / 46 / 99` 和 `docs/phase1/runtime_capture/todo_077_line_group_ui_prompt_capture/README.md`，确认 `TODO-078` 只做旧图石运行工件回填；不自动启动旧图石，不安装 HASP；不得把当前 P0 文案或不完整截图写成旧图石真值；最后运行 readiness gate、OCCT 泄漏检查，若涉及代码/门禁脚本再跑默认 CTest 和 xhigh 只读 review。
- **Known Delays**：旧 UI 参数窗口字段、状态栏流程和失败提示仍依赖用户插狗后的现场运行；没有 golden 时只能准备采样清单，不声明完整 1:1。
- **Recovery Target**：如果运行确认所需工件不齐，就把缺项写成 gap 和拒收规则；不能把现有 `RebarGroupCreator` 或父目录代码当旧图石完整真相。
- **Rollback Trigger**：domain/rebar 出现 OCCT/AIS include；父目录 rebar 业务被迁入；无 IDA/运行证据却写成旧逻辑已确认；测试或 gate 失败仍继续堆功能。
- **Constraints**：不使用 ACIS / HOOPS / Codejock 等商业库；新系统不引入 USB 狗 / 网络许可依赖；旧逻辑不确定时优先查 IDA MCP 或旧图石运行确认；xhigh 只读，修改由主流程 agent 完成；不自动安装 HASP，不自动再次启动旧图石。
- **Boundary**：下一轮优先允许改运行确认工件归档、02/08/11/35/46/99、run report、todo 和必要门禁。禁止自动启动旧图石，禁止改 RebarGroupCreator 算法，禁止改 UI 文案，禁止继续扩张 DetailWriter 字段，禁止实现面配筋/接头/Excel/golden，禁止迁入父目录 rebar 业务。
- **Coupling Notes**：`domain/rebar` 是业务对象边界；`LegacyGeometryAdapter` 是几何能力边界；线配筋创建若需要几何读取，只能通过 legacy 语义接口，不让业务层直接写 OCCT。
- **Approximation Validity**：TODO-078 的目标是回填真实运行工件，不是完整旧参数窗口、完整线配筋、面配筋、弧筋、统计、出图或 golden；若工件只覆盖部分路径，只能关闭对应路径，不能整体关闭旧 UI 缺口。
- **Actuator Budget**：下一轮只推进 `TODO-078`。完成后停止复盘，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段继续扩张或 golden。
- **Risks**：不完整截图容易被误读成旧 UI 已确认；必须保留未覆盖字段的 `GAP-UI-REB-001`，不得伪装成旧图石确认结论。
## Todo CSV 使用方式

`todo.csv` 是后续执行看板。建议每次 goal 模式只拿 `status=next` 或最高优先级 `pending` 的任务推进。

字段说明：

- `id`：任务编号。
- `priority`：`P0` 必须优先，`P1` 近期开发，`P2` 后续增强。
- `phase`：对应 M1 / M2 / evidence / doc 阶段。
- `task`：任务名。
- `status`：`done`、`next`、`pending`、`blocked`。
- `goal_setpoint`：本任务要把系统推进到什么状态。
- `acceptance`：怎样算完成。
- `boundary`：允许碰哪里，禁止碰哪里。
- `evidence`：依赖的证据来源。
- `dependencies`：前置任务。
- `risk`：主要风险。
- `notes`：补充说明。

## 当前执行建议

下一步优先执行：

```text
TODO-078 / 线配筋旧图石运行确认工件回填 P0
  -> TODO-071 已完成 sgroupbarline 入口契约冻结
  -> TODO-072 已完成 LineGroup command handler P0 事务接入
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0
  -> TODO-074 已完成最小参数窗口和参数传递
  -> TODO-075 已完成打开参数窗口前的选择预检
  -> TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point
  -> TODO-077 已完成旧运行采样清单和工件门禁
  -> 下一轮只回填和核对真实旧图石运行工件
  -> 不自动安装 HASP
  -> 不自动启动旧图石
  -> 不用 OCCT 直接重写钢筋业务
  -> 不迁入父目录 rebar 业务
  -> 不声明完整线配筋、AutoCAD L2 通过或 golden
```

原因很简单：TODO-071 已把旧 `sgroupbarline` 入口契约冻结到 P0 creator raw/evidence，TODO-072/073/074/075 已依次接通 handler、AIS 可见反馈、参数窗口和选择预检顺序，TODO-076 已把静态失败提示证据收窄到 stop point，TODO-077 已把静态无法确认的旧运行 UI 信息转成可采样清单。现在要按这个清单核对真实旧图石运行工件，而不是继续抠 Detail 小字段，也不是用 OCCT 直接写一个新的钢筋生成器。
TODO-026 golden 采集暂按用户要求保持 pending。
