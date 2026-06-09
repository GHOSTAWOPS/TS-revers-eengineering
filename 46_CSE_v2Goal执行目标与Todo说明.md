# CSE v2 Goal 执行目标与 Todo 说明

## 一句话结论

后续 goal 模式的主线不是“重新设计一个钢筋软件”，而是继续把正式 `app` 往旧图石 1:1 复刻方向推进：Qt6 / OCCT 只替代界面、显示、选择和几何能力，钢筋业务逻辑按 VisualTS / IDA / SFL / Detail 证据复刻。

## 当前执行口径（2026-06-10）

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
  -> TODO-066 已完成真实工件回填；TODO-067 已完成字段对照；TODO-068 已完成最明显旧包格式纠偏；TODO-069 已完成 StbRow 扩展属性骨架；TODO-070 已完成 lineStb StbGeo 字段条件化骨架；TODO-071 已完成线配筋入口契约冻结；TODO-072 已完成 LineGroup command handler P0；TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0；TODO-074 已完成 LineGroup 参数窗口 P0；TODO-075 已完成选择预检与参数窗口顺序对齐 P0；TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point；TODO-077 已完成旧图石运行确认采样清单与拒收规则；TODO-078 已审计为 blocked，因为当前只有模板、没有真实旧图石运行截图/hash/listing/填写后的 notes；TODO-079 已完成旧 UI / Dialog 静态资源补证；TODO-080 已完成公共创建 core gate 与 diagnostic P0 对齐；现在的主线是 TODO-081
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

latest pushed baseline tag = m2-rebar-create-009-todo-079-linegroup-ui-static-evidence
current node tag to create after verification = m2-rebar-create-010-todo-080-linegroup-core-gate-diagnostics
```

当前下一步：

```text
TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0
  -> 接头链路现阶段先记录、暂缓。
  -> TODO-071 已完成 sgroupbarline 入口契约冻结。
  -> TODO-072 已完成 LineGroup command handler P0 事务接入。
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0。
  -> TODO-074 已完成 LineGroup 参数窗口 P0。
  -> TODO-075 已完成先选择预检、再打开参数窗口的最小顺序对齐。
  -> TODO-076 已确认旧 handler / helper 无直接中文失败提示，公共链只收窄到 Input_float / Dialog #383。
  -> TODO-077 已完成旧 UI 运行确认清单与工件门禁。
  -> TODO-078 已确认采样目录只有模板、缺真实旧图石运行工件，因此 blocked。
  -> TODO-079 已完成旧 UI / Dialog 静态资源补证，仍没有直接旧中文 UI / 状态栏文案绑定。
  -> TODO-080 已完成旧 sub_1404D10C0 已确认公共创建 gate 与 diagnostic P0 对齐。
  -> 下一轮只补 sub_1404D10C0 -> sub_140451730 -> createdObject / payload 字段语义。
  -> 不自动启动旧图石，不安装 HASP，不用 OCCT 直接重写钢筋业务。
  -> 不替代 TODO-078 的真实运行确认。
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

### 短期 Goal（下一轮推荐复制）

目标：只完成 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0` 这个短期阶段，不自动进入后续长期开发。

当前状态：

```text
接头链路现阶段先记录、暂缓：
  TODO-050 = blocked
  TODO-061 = pending
  不再作为当前 next。

非接头主线已推进到：
  TODO-071 = done  线配筋入口契约冻结
  TODO-072 = done  LineGroup command handler P0
  TODO-073 = done  LineGroup UI/AIS 可见反馈 P0
  TODO-074 = done  LineGroup 参数窗口 P0
  TODO-075 = done  选择预检与参数窗口顺序对齐 P0
  TODO-076 = done  旧 UI 失败提示与状态栏口径静态 stop point
  TODO-077 = done  旧图石运行确认采样清单与拒收规则
  TODO-078 = blocked  当前只有模板，没有真实旧图石运行截图/hash/listing/填写后的 notes
  TODO-079 = done  旧 UI / Dialog 静态资源补证 static stop point
  TODO-080 = done  公共创建 core gate 与 diagnostic P0 对齐
  TODO-081 = next
```

本轮只做：

```text
LineGroupCreatedPayloadEvidenceP0 / TODO-081
  -> 从 todo.csv / 03 / 08 / 11 / 15 / 16 / 35 / 46 / 99 出发
  -> 基于 E-IDA-045 / E-IDA-047 / E-DEV-102 已确认的 sub_1404D10C0 公共创建 gate
  -> 优先用 IDA MCP 追 sub_1404D10C0 -> sub_140451730 -> createdObject / payload 字段
  -> 只做静态证据、字段语义、可开发边界和缺口收口
  -> 不直接实现完整线配筋算法
  -> 更新实现记录、run report、追溯矩阵、缺口文档、46 和 todo
  -> 如本轮只改文档/证据，运行 readiness gate 和 git diff --check
  -> 如涉及代码、测试或门禁脚本，必须补测试并在 commit 前执行 xhigh 只读 review
  -> 不启动旧图石，不安装 HASP
  -> 不改 LineGroupParameterDialog 文案
  -> 不用 OCCT 直接重写钢筋业务
  -> 不迁入父目录 rebar 业务
  -> domain/rebar 不引入 TopoDS / AIS / BRep / TopAbs
  -> 不进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段继续扩张
  -> 不跑 AutoCAD L2，不进入 golden
```

目标语义：

```text
TODO-078 不是完成了，而是 blocked：
  runtime_capture/todo_077_line_group_ui_prompt_capture 目前只有模板。
  缺 hashes.txt、旧图石运行截图、listing、输出 hash 和填写后的 capture_notes。

TODO-079 已完成 static stop point：
  旧 UI / Dialog / 状态栏静态证据已继续收窄。
  仍不能替代旧图石真实运行确认。

TODO-080 已完成：
  不再继续猜 UI 文案。
  已把旧 sub_1404D10C0 已确认的公共创建 gate 和失败诊断对齐到 P0 代码/测试。

TODO-080 仍不能证明：
  完整线配筋算法已完成。
  旧图石真实运行提示已闭合。
  Dialog #383 一定出现在真实线配筋流程。
  当前 Qt6 LineGroupParameterDialog 已 1:1 等于旧图石窗口。
  面配筋、弧筋、接头、Excel、Detail 或 golden 已完成。

TODO-081 的价值是：
  在继续编码完整线配筋算法前，
  先把 objA / objB / createdPayload 字段语义继续从 IDA 静态证据里收窄。
  这能防止后续把 P0 gate 快照误当成完整旧对象模型。
```

本轮完成后必须停止并复盘：

```text
完成了哪些 gate / diagnostic 对齐
验证了哪些门禁
还缺哪些旧图石运行工件
TODO-078 需要用户补什么
下一阶段建议继续 TODO-081 还是回填 TODO-078
```
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
TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0
  -> TODO-071 已完成 sgroupbarline 入口契约冻结
  -> TODO-072 已完成 LineGroup command handler P0 事务接入
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0
  -> TODO-074 已完成 LineGroup 参数窗口 P0
  -> TODO-075 已完成选择预检与参数窗口顺序对齐 P0
  -> TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point
  -> TODO-077 已完成旧图石运行确认清单、截图/输出工件模板和拒收伪工件门禁
  -> TODO-078 已确认当前只有模板、缺真实旧图石运行工件，因此 blocked
  -> TODO-079 已完成旧 UI / Dialog 静态资源补证
  -> TODO-080 已完成公共创建 core gate 与 diagnostic P0 对齐
  -> 下一轮只用 IDA MCP / 静态证据补证 createdPayload 与 objA/objB 字段语义
  -> 不自动安装 HASP，不自动启动旧图石
  -> 不用 OCCT 直接重写钢筋业务，不迁入父目录 rebar 业务
  -> 不替代 TODO-078 的真实运行确认
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
TODO-077 已经把静态无法确认的旧运行 UI 信息转成可采样清单。
TODO-078 已审计当前采样目录只有模板，无法回填真实运行结果，因此 blocked。

当前最值得继续推进的是线配筋生成主线：
  在等待 TODO-078 用户现场运行工件期间，
  TODO-079 已经完成旧 UI / Dialog / 状态栏的静态资源补证。
  TODO-080 已经把公共创建 gate 和 diagnostic 落到 P0。

这比继续抠 Detail 小字段更贴近当前成熟度瓶颈：
  线配筋还没有完整 objA / objB / createdPayload 业务对象字段语义，
  下一步应先继续收窄静态证据，而不是用 OCCT 直接写一个差不多的生成器。
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

- **Primary Setpoint**：下一轮只完成 `TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0`，把旧 `sub_1404D10C0 -> sub_140451730 -> createdObject / payload` 字段语义继续收窄到可开发边界。
- **Acceptance**：用 IDA MCP 或等价静态证据补齐 `createdPayload / objA / objB` 字段语义、仍未闭合项和下一步可编码边界；更新实现记录、run report、03/08/11/15/16/35/46/99/todo；readiness gate、git diff --check 通过；若涉及代码、测试或门禁脚本，默认 CTest、OCCT 泄漏检查和 xhigh 只读 review 必须通过。
- **Guardrail Metrics**：不能用 OCCT 能怎么做替代旧图石怎么做；不能迁入父目录 rebar 业务；不能让 `domain/rebar` 依赖 TopoDS/AIS/BRep/TopAbs；不能把 P0 diagnostic 写成完整旧运行行为；不能在一个节点里铺开面配筋、接头、Excel、Detail 字段继续扩张或 golden。
- **Sampling Plan**：先读 `todo.csv / 03 / 08 / 11 / 15 / 16 / 35 / 46 / 99`，确认 `TODO-081` 只做公共创建 payload / 字段语义补证；优先查 IDA MCP；最后运行 readiness gate、git diff --check，代码节点再跑默认 CTest、OCCT 泄漏检查和 xhigh 只读 review。
- **Known Delays**：旧 UI 参数窗口字段、状态栏流程和失败提示最终仍依赖用户插狗后的现场运行；TODO-081 只能继续收窄静态字段语义，不能替代 TODO-078。
- **Recovery Target**：如果 `createdPayload / objA / objB` 语义仍不足以编码，就只记录 static stop point 和缺口，不编造完整旧算法。
- **Rollback Trigger**：domain/rebar 出现 OCCT/AIS include；父目录 rebar 业务被迁入；无 IDA/运行证据却写成旧逻辑已确认；测试或 gate 失败仍继续堆功能。
- **Constraints**：不使用 ACIS / HOOPS / Codejock 等商业库；新系统不引入 USB 狗 / 网络许可依赖；旧逻辑不确定时优先查 IDA MCP 或旧图石运行确认；xhigh 只读，修改由主流程 agent 完成；不自动安装 HASP，不自动再次启动旧图石。
- **Boundary**：下一轮优先只改 IDA / 静态证据文档、run report、todo 和必要门禁；若证据足够且范围清楚，才允许补极小测试或 DTO。禁止改 UI 文案，禁止实现完整线配筋算法，禁止继续扩张 DetailWriter 字段，禁止实现面配筋/接头/Excel/golden，禁止迁入父目录 rebar 业务。
- **Coupling Notes**：`domain/rebar` 是业务对象边界；`LegacyGeometryAdapter` 是几何能力边界；线配筋创建若需要几何读取，只能通过 legacy 语义接口，不让业务层直接写 OCCT。
- **Approximation Validity**：TODO-081 的目标是静态字段语义补证，不是旧图石真实运行确认、完整旧参数窗口、完整线配筋、面配筋、弧筋、统计、出图或 golden。
- **Actuator Budget**：下一轮只推进 `TODO-081`。完成后停止复盘，不自动进入完整线配筋算法、面配筋、弧筋、接头、Excel、Detail 字段继续扩张或 golden。
- **Risks**：静态字段语义容易被误读成完整旧算法；必须保留 `GAP-IDA-002 / GAP-IDA-007 / GAP-UI-REB-001`，不得伪装成旧图石运行确认结论。
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
TODO-081 / 线配筋公共创建 createdPayload 与 objA/objB 字段语义补证 P0
  -> TODO-071 已完成 sgroupbarline 入口契约冻结
  -> TODO-072 已完成 LineGroup command handler P0 事务接入
  -> TODO-073 已完成 LineGroup UI/AIS 可见反馈 P0
  -> TODO-074 已完成最小参数窗口和参数传递
  -> TODO-075 已完成打开参数窗口前的选择预检
  -> TODO-076 已完成旧 UI 失败提示与状态栏口径静态 stop point
  -> TODO-077 已完成旧运行采样清单和工件门禁
  -> TODO-078 已因缺真实旧图石运行工件 blocked
  -> TODO-079 已完成旧 UI / Dialog 静态资源补证 static stop point
  -> TODO-080 已完成 sub_1404D10C0 公共创建 gate / diagnostic 对齐
  -> 下一轮只做 createdPayload / objA / objB 字段语义补证
  -> 不自动安装 HASP
  -> 不自动启动旧图石
  -> 不用 OCCT 直接重写钢筋业务
  -> 不迁入父目录 rebar 业务
  -> 不替代 TODO-078 的真实运行确认
  -> 不声明完整线配筋、AutoCAD L2 通过或 golden
```

原因很简单：TODO-079 已把旧 UI / Dialog 静态证据收窄到 stop point，继续猜中文 UI 文案收益很低。在没有真实旧图石运行工件前，下一步更稳的是把 `sub_1404D10C0` 已确认的公共创建 gate 和失败诊断先落到 P0 代码/测试，继续保持“按 VisualTS 证据复刻”，而不是用 OCCT 直接写一个新的钢筋生成器。
TODO-026 golden 采集暂按用户要求保持 pending。
