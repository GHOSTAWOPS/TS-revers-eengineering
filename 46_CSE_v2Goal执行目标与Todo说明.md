# CSE v2 Goal 执行目标与 Todo 说明

## 一句话结论

后续 goal 模式的主线不是“重新设计一个钢筋软件”，而是继续把正式 `app` 往旧图石 1:1 复刻方向推进：Qt6 / OCCT 只替代界面、显示、选择和几何能力，钢筋业务逻辑按 VisualTS / IDA / SFL / Detail 证据复刻。

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
31. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
32. `【图石钢筋1比1复刻】\todo.csv`

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
```

当前最新验证基线：

```text
app 默认 CTest = 8/8 pass
readiness gate = M1-Formal-Ready, 78/78 pass
domain/rebar OCCT 边界 = pass

latest commit = 本文件所在 TODO-017 节点提交
latest tag = m1-app-017/sweep-pipe-preview
```

当前下一步：

```text
TODO-018 / M1-App-018
  -> 钢筋领域模型冻结 P1
  -> 把 SteelBar / SteelBarSegment / SteelBarGroup 字段按旧证据固化成可编码模型
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

目标：只完成 `TODO-018 / M1-App-018` 这个短期阶段，不自动进入后续长期开发。

本轮要在正式 `app` 中完成 `钢筋领域模型冻结 P1`：

```text
SteelBar
SteelBarSegment
SteelBarGroup
SteelData
```

目标语义：

```text
对齐旧 VisualTS / Detail / SFL 证据：

旧 steelbar / steelbargroup / seg_steelbargroup / steelData 字段线索
  -> Detail StbGroup / StbGeo / StbTable 字段映射
  -> 新 domain/rebar DTO 字段和 ID 关系
  -> 单测验证字段默认值、引用关系和无 OCCT 泄漏
  -> 只冻结可编码领域模型，不实现钢筋创建算法
```

本轮只做领域模型字段和测试，不做线筋 / 弧筋创建业务，不做 UI，不做 Detail writer。

当前已完成前置：

```text
TODO-010 / M1-App-010 = done
  -> edgeProjectPoint
  -> edgeSplitAtPoint

TODO-011 / M1-App-011 = done
  -> edgeTrimEndpoint

TODO-012 / M1-App-012 = done
  -> pointToEdgeGroupDistance

TODO-013 / M1-App-013 = done
  -> buildSplineFromPoints

TODO-014 / M1-App-014 = done
  -> buildWireChain

TODO-015 / M1-App-015 = done
  -> offsetEdgePreview

TODO-016 / M1-App-016 = done
  -> facePlaneSectionPreview

TODO-017 / M1-App-017 = done
  -> edgeCircularSweepPreview
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
2. `【图石钢筋1比1复刻】\06_技术路线与替代方案.md`
3. `【图石钢筋1比1复刻】\11_需求证据追溯矩阵.md`
4. `【图石钢筋1比1复刻】\15_线配筋与弧形组专项初稿.md`
5. `【图石钢筋1比1复刻】\16_seg_steelbargroup字段地图初稿.md`
6. `【图石钢筋1比1复刻】\23_父目录源码参考边界与路线纠偏.md`
7. `【图石钢筋1比1复刻】\34_Phase1ReadinessGate实际运行记录.md`
8. `【图石钢筋1比1复刻】\35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
9. `【图石钢筋1比1复刻】\40_M1-App-004LegacyGeometryAdapterP0实现记录.md`
10. `【图石钢筋1比1复刻】\41_M1-App-005LegacyGeometryAdapterP1实现记录.md`
11. `【图石钢筋1比1复刻】\42_M1-App-006LegacyGeometryAdapterP2A实现记录.md`
12. `【图石钢筋1比1复刻】\43_M1-App-007LegacyGeometryAdapterP2B实现记录.md`
13. `【图石钢筋1比1复刻】\44_M1-App-008LegacyGeometryAdapterP2C实现记录.md`
14. `【图石钢筋1比1复刻】\45_M1-App-009LegacyGeometryAdapterP3A实现记录.md`
15. `【图石钢筋1比1复刻】\47_M1-App-010LegacyGeometryAdapterP3B实现记录.md`
16. `【图石钢筋1比1复刻】\48_M1-App-011LegacyGeometryAdapterP3C实现记录.md`
17. `【图石钢筋1比1复刻】\49_M1-App-012LegacyGeometryAdapterP3D实现记录.md`
18. `【图石钢筋1比1复刻】\50_M1-App-013LegacyGeometryAdapterP3E实现记录.md`
19. `【图石钢筋1比1复刻】\51_M1-App-014LegacyWireChain实现记录.md`
20. `【图石钢筋1比1复刻】\52_M1-App-015LegacyGeometryAdapterOffsetSpike实现记录.md`
21. `【图石钢筋1比1复刻】\53_M1-App-016LegacyGeometryAdapterSectionSpike实现记录.md`
22. `【图石钢筋1比1复刻】\54_M1-App-017LegacyGeometryAdapterSweepBoundary实现记录.md`
23. `【图石钢筋1比1复刻】\09_钢筋领域模型草案.md`
24. `【图石钢筋1比1复刻】\13_Detail字段映射矩阵.md`
25. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
26. `【图石钢筋1比1复刻】\todo.csv`

本轮允许修改：

- `app/src/domain/rebar/SteelData.h`
- `app/src/domain/rebar/SteelBar.h`
- `app/src/domain/rebar/SteelBarSegment.h`
- `app/src/domain/rebar/SteelBarGroup.h`
- `app/tests/unit/`
- 必要时修改 `app/CMakeLists.txt` 以接入领域模型单测
- `【图石钢筋1比1复刻】` 下对应实现记录、build report、追溯矩阵、缺口文档和 `todo.csv`

本轮禁止修改或迁移：

- 父目录 `src/rebar/*`
- 父目录 `RebarCreationCommandService`
- 父目录 `EdgeToRebarFactory`
- 父目录 `FaceRebarGenerator`
- 父目录 `PolylineRebarGenerator`
- 任何 OCCT 直接造钢筋业务逻辑
- `domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_`
- 线筋 / 弧筋 / 面筋创建算法
- Detail writer 输出逻辑
- UI 菜单和命令 handler

本轮验收标准：

1. `domain/rebar` 中的 SteelData / SteelBar / SteelBarSegment / SteelBarGroup 字段与 `09/13/16` 的证据口径对齐。
2. 字段需要区分：
   - 已有高置信字段。
   - 低置信但需要保留的 legacy raw / unresolved 字段。
   - Detail writer 需要的映射字段。
3. 领域模型只依赖 STL / Qt 基础类型或本项目纯领域类型，不依赖 OCCT / AIS。
4. 新增或补强单测，覆盖：
   - 默认值。
   - group -> bar -> segment 引用关系。
   - Detail 关键字段映射所需 ID / 数量 / 直径 / 间距 / 长度 / 类型字段。
   - 低置信字段必须能标注 unresolved / evidence。
5. 默认 CTest 通过。
6. readiness gate 严格模式通过，或记录明确失败原因。
7. domain/rebar OCCT 泄漏检查通过。
8. 涉及代码、测试、构建脚本时，commit 前必须执行 xhigh 只读 review，并关闭已完成子代理。
9. 新增 `55_M1-App-018RebarDomainModelFreezeP1实现记录.md`。
10. 新增 `docs/phase1/app_build_reports/m1_app_018_run_001.md` 和必要 JSON。
11. 更新：
   - `00_总览.md`
   - `09_钢筋领域模型草案.md`
   - `11_需求证据追溯矩阵.md`
   - `13_Detail字段映射矩阵.md`
   - `34_Phase1ReadinessGate实际运行记录.md`
   - `99_缺口和待确认项.md`
   - `46_CSE_v2Goal执行目标与Todo说明.md`
   - `todo.csv`
12. `todo.csv` 中 `TODO-018` 改为 `done`；只把下一个明确可执行任务改为 `next`，但不继续实现。

本轮完成后必须停止，输出阶段复盘：

```text
完成了什么
验证了什么
还缺什么
下一阶段建议做 TODO-019 / TODO-020 / TODO-021 中哪一个
```

不要在同一个 goal 内继续做 `TODO-019`、旧命令契约、钢筋创建算法、Detail writer 或 UI 复刻。

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

当前最新验证状态：

```text
app 默认 CTest = 8/8 pass
readiness gate = M1-Formal-Ready, 78/78 pass
domain/rebar OCCT 边界 = pass
```

当前下一步：

```text
TODO-018 / M1-App-018
  -> 钢筋领域模型冻结 P1
  -> SteelBar / SteelBarSegment / SteelBarGroup / SteelData 字段可编码
```

原因：

```text
LegacyGeometryAdapter 的首批几何能力边界已覆盖到 sweep / pipe preview。
继续堆 adapter spike 的收益下降。下一步应把旧 steelbar / steelbargroup /
seg_steelbargroup / steelData 证据沉到 domain/rebar 领域模型里，
为后续线筋 / 弧筋创建算法提供稳定对象边界。
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

- **Primary Setpoint**：本轮只完成 `TODO-018 / M1-App-018`，让 `domain/rebar` 的 SteelData / SteelBar / SteelBarSegment / SteelBarGroup 字段按旧证据冻结到 P1。
- **Acceptance**：领域模型 DTO、单测、默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查、xhigh 只读 review、实现记录、build report、`todo.csv` 和追溯文档全部闭合。
- **Guardrail Metrics**：不能让 OCCT 细节泄漏进 `domain/rebar`；不能把父目录钢筋生成器当业务真相；不能用“OCCT 能做什么”替代“旧图石怎么做”。
- **Sampling Plan**：先读 `09/13/16` 字段证据，再补 domain/rebar 单测；实现后运行默认 CTest；运行 readiness gate；运行 domain/rebar OCCT 泄漏检查；代码节点 commit 前执行 xhigh 只读 review；完成后更新 evidence / gap / todo。
- **Known Delays**：IDA MCP 当前可能没有绑定数据库；旧图石运行确认依赖用户操作；真实 golden 对照要等旧软件可稳定导出。
- **Recovery Target**：发现路线偏移时，停止继续开发钢筋业务，先回到文档和 adapter 边界修正。
- **Rollback Trigger**：`domain/rebar` 出现 OCCT include、父目录 rebar 业务被迁入、测试失败但继续堆功能、旧逻辑无证据却写成确定结论、代码节点跳过 xhigh 只读 review。
- **Constraints**：不使用 ACIS / HOOPS / Codejock 等商业库；不读取完整私有 SFL 作为新主格式；新工程格式结合 SFL 业务语义和 OCCT 几何引用设计。
- **Boundary**：本轮只允许修改 domain/rebar DTO、领域模型单测、必要 CMake 测试接入、M1-App-018 文档和任务看板；父目录只读参考；xhigh agent 只读 review，不负责修改。
- **Coupling Notes**：`LegacyGeometryAdapter` 是几何能力边界；`domain/rebar` 是业务对象边界；`DetailWriter` 和新设计文件格式是输出 / 持久化边界。
- **Approximation Validity**：本轮领域模型冻结 P1 只确认可编码字段、ID 关系和低置信字段承载方式；不代表旧图石所有业务字段语义完全闭合，也不代表线筋 / 弧筋创建算法已实现。
- **Actuator Budget**：本轮只推进 `TODO-018`。完成后停止复盘，不自动进入 `TODO-019`。
- **Risks**：旧图石业务逻辑证据不足；OCCT 几何结果和 ACIS 存在细节差异；没有 golden 时只能先做结构正确和证据闭环。

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
TODO-018 / M1-App-018
  -> 钢筋领域模型冻结 P1
  -> 把 SteelBar / SteelBarSegment / SteelBarGroup / SteelData 字段按旧证据固化
```

原因很简单：几何 adapter 的首批能力边界已经够支撑下一步建业务对象。
如果不先冻结领域模型，后面线筋 / 弧筋创建、Detail writer 和新工程格式
都会各自发明字段，路线会变成多套事实源。
