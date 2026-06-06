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
```

当前最新验证基线：

```text
app 默认 CTest = 9/9 pass
readiness gate = M1-Formal-Ready, 78/78 pass
domain/rebar OCCT 边界 = pass

latest commit = 本文件所在 TODO-019 节点提交
latest tag = m1-app-019/legacy-command-contract-p1
```

当前下一步：

```text
TODO-020 / Evidence
  -> IDA MCP 旧线筋 / 弧筋链补证据
  -> 补 sgroupbarline / sgroupbararc / sub_1405D5670 的关键调用链、
     参数语义、字段和常量
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

目标：只完成 `TODO-020 / Evidence` 这个短期阶段，不自动进入后续长期开发。

本轮要用 IDA MCP 补旧线筋 / 弧筋链证据：

```text
sgroupbarline
sgroupbararc
sub_1404DE720
sub_1404DE110
sub_1404D10C0
sub_140451730
sub_1405D5670
```

目标语义：

```text
旧命令契约已经绑定，但业务创建算法不能靠猜。
本轮只做反编译证据闭合：

旧命令入口
  -> handler 函数
  -> 公共生成链
  -> seg_steelbargroup 关键函数
  -> 参数语义 / 字段 / 常量 / ACIS 调用
  -> 可开发的业务算法约束
```

本轮只做 IDA / 文档证据整理，不做线筋 / 弧筋创建业务，不做 Detail writer，不做 AIS 钢筋显示，不修改正式 app 业务代码。

当前已完成前置：

```text
TODO-010 ~ TODO-017 = done
  -> LegacyGeometryAdapter 已具备 point projection、split、trim、group distance、spline、wire chain、offset、section、sweep preview 等首批几何能力。

TODO-018 / M1-App-018 = done
  -> domain/rebar 钢筋领域模型冻结 P1。

TODO-019 / M1-App-019 = done
  -> 旧命令契约绑定 P1，LineGroup / ArcGroup / TrimByLine / TrimByFace 当前为 NotImplemented placeholder。
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
4. `【图石钢筋1比1复刻】\15_线配筋与弧形组专项初稿.md`
5. `【图石钢筋1比1复刻】\16_seg_steelbargroup字段地图初稿.md`
6. `【图石钢筋1比1复刻】\35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
7. `【图石钢筋1比1复刻】\56_M1-App-019LegacyCommandContractP1实现记录.md`
8. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
9. `【图石钢筋1比1复刻】\todo.csv`

本轮允许修改：

- `03_IDA命令证据.md`
- `15_线配筋与弧形组专项初稿.md`
- `16_seg_steelbargroup字段地图初稿.md`
- `11_需求证据追溯矩阵.md`
- `99_缺口和待确认项.md`
- `46_CSE_v2Goal执行目标与Todo说明.md`
- `todo.csv`
- 必要时新增 `57_TODO-020_IDA旧线筋弧筋链补证据记录.md`
- 必要时新增 `docs/phase1/ida_reports/todo_020_*.md/json`

本轮禁止修改或迁移：

- `app/src/domain/rebar/*`
- `app/src/geometry/*`
- `app/src/command/*`
- 父目录 `src/rebar/*`
- 任何 OCCT 直接造钢筋业务逻辑
- 线筋 / 弧筋 / 面筋创建算法
- Detail writer 输出逻辑
- AIS 钢筋显示逻辑

本轮验收标准：

1. 优先用 IDA MCP 查询旧 VisualTS 数据库。
2. 如果 IDA MCP 可用，至少记录：
   - 目标函数地址 / 名称。
   - 调用链。
   - 关键参数和返回值推断。
   - 关键常量。
   - ACIS 调用点。
   - 仍不确定的字段 / 分支。
3. 如果 IDA MCP 没有绑定数据库或不可用，必须把真实阻塞原因写入 `99_缺口和待确认项.md` 和本轮报告。
4. 不得用父目录代码替代旧 VisualTS 证据。
5. 不得把低置信推断写成确定业务规则。
6. 更新 `03 / 15 / 16 / 11 / 99 / 46 / todo.csv`。
7. `todo.csv` 中 `TODO-020` 改为 `done` 或 `blocked`；如果 done，只把下一个明确可执行任务改为 `next`，但不继续实现。
8. 纯证据节点不强制 xhigh；如本轮修改代码、测试或构建脚本，则必须执行 xhigh 只读 review。

本轮完成后必须停止，输出阶段复盘：

```text
完成了什么
IDA MCP 是否可用
确认了哪些旧逻辑
还缺什么
下一阶段建议做 TODO-021 还是先继续补 IDA / 旧图石运行证据
```

不要在同一个 goal 内继续做 `TODO-021`、钢筋创建算法、Detail writer、AIS 钢筋显示或新工程格式 runtime。
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

当前最新验证状态：

```text
app 默认 CTest = 9/9 pass
readiness gate = M1-Formal-Ready, 78/78 pass
domain/rebar OCCT 边界 = pass
```

当前下一步：

```text
TODO-020 / Evidence
  -> IDA MCP 旧线筋 / 弧筋链补证据
  -> 补 sgroupbarline / sgroupbararc / sub_1405D5670 的关键调用链、参数语义、字段和常量
```

原因：

```text
旧命令契约已完成，LineGroup / ArcGroup / TrimByLine / TrimByFace 当前都是可追溯占位。
进入业务创建算法前，必须先用 IDA MCP 补足旧 VisualTS 调用链和字段语义，
避免把 NotImplemented placeholder 直接写成猜测算法。
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

- **Primary Setpoint**：本轮只完成 `TODO-020 / Evidence`，用 IDA MCP 补旧线筋 / 弧筋链证据，把 `sgroupbarline / sgroupbararc / sub_1405D5670` 关键调用链推进到可开发级证据。
- **Acceptance**：IDA MCP 可用时记录函数、调用链、关键参数、常量、ACIS 调用点和字段推断；IDA MCP 不可用时记录真实 blocked 原因；更新 `03/15/16/11/99/46/todo.csv`；不修改正式 app 代码。
- **Guardrail Metrics**：不能用父目录代码替代旧 VisualTS 证据；不能把低置信推断写成确定算法；不能让 OCCT 能力反推旧业务规则。
- **Sampling Plan**：先读 `03/15/16/56/99/todo.csv`；调用 IDA MCP 查询旧函数和调用链；把结果写入证据文档；最后更新 todo 状态。
- **Known Delays**：IDA MCP 可能没有绑定数据库；旧图石运行确认依赖用户操作；真实 golden 对照后续再补。
- **Recovery Target**：如果 IDA 不可用，本轮应标记为 `blocked` 或只记录阻塞事实，不能转去猜业务算法。
- **Rollback Trigger**：无 IDA / 运行证据却开始实现线筋 / 弧筋创建算法；父目录 rebar 业务被迁入；低置信字段被写成确定事实。
- **Constraints**：不使用 ACIS / HOOPS / Codejock 等商业库；本轮不修改 app 业务代码；新工程格式结合 SFL 业务语义和 OCCT 几何引用设计。
- **Boundary**：本轮只允许修改 IDA / 线筋弧筋专项 / 追溯矩阵 / 缺口 / 46 / todo 文档；如必须新增报告，只放在 `docs/phase1/ida_reports/`。
- **Coupling Notes**：`LegacyGeometryAdapter` 是几何能力边界；`domain/rebar` 是业务对象边界；TODO-020 只决定后续业务算法的证据输入，不直接施加代码控制输入。
- **Approximation Validity**：IDA 反编译结论必须标置信度；未确认字段保持 GAP，不得作为实现事实。
- **Actuator Budget**：本轮只推进 `TODO-020`。完成后停止复盘，不自动进入 `TODO-021` 或钢筋创建算法。
- **Risks**：IDA 数据库未绑定；旧函数反编译噪声高；函数名 / 字段偏移可能需要多轮交叉验证。
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
TODO-020 / Evidence
  -> IDA MCP 旧线筋 / 弧筋链补证据
  -> 补 sgroupbarline / sgroupbararc / sub_1405D5670 的关键调用链、参数语义、字段和常量
```

原因很简单：旧命令入口已经绑定，下一步不能直接写创建算法。
必须先用 IDA MCP 把旧 VisualTS 线筋 / 弧筋公共生成链补到可开发级，
否则后续 `TODO-021` 会从命令占位滑向猜测实现。
