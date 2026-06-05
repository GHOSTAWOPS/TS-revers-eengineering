# CSE v2 Goal 执行目标与 Todo 说明

## 一句话结论

后续 goal 模式的主线不是“重新设计一个钢筋软件”，而是继续把正式 `app` 往旧图石 1:1 复刻方向推进：Qt6 / OCCT 只替代界面、显示、选择和几何能力，钢筋业务逻辑按 VisualTS / IDA / SFL / Detail 证据复刻。

## 可直接粘贴到 Goal 模式的目标

### 短期 Goal（推荐本轮复制）

目标：只完成 `TODO-012 / M1-App-012` 这个短期阶段，不自动进入后续长期开发。

本轮要在正式 `app` 中完成 `LegacyGeometryAdapter P3D`：

```text
组内最小距离检查
point / edge group minimum distance summary
```

目标语义：

```text
对齐旧 VisualTS / ACIS 链路：

sub_14059B980
  -> 从组节点链表遍历已有 edge
  -> 对每个 edge 调 api_entity_point_distance
  -> 返回输入点到组内所有 edge 的最小距离
```

本轮只做几何兼容层能力，不做钢筋创建业务。

当前已完成前置：

```text
TODO-010 / M1-App-010 = done
  -> edgeProjectPoint
  -> edgeSplitAtPoint

TODO-011 / M1-App-011 = done
  -> edgeTrimEndpoint
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
17. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
18. `【图石钢筋1比1复刻】\todo.csv`

本轮允许修改：

- `app/src/geometry/legacy/LegacyGeometry.h`
- `app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h`
- `app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp`
- `app/tests/integration/legacy_geometry_adapter_tests.cpp`
- `【图石钢筋1比1复刻】` 下对应实现记录、build report、追溯矩阵、缺口文档和 `todo.csv`

本轮禁止修改或迁移：

- 父目录 `src/rebar/*`
- 父目录 `RebarCreationCommandService`
- 父目录 `EdgeToRebarFactory`
- 父目录 `FaceRebarGenerator`
- 父目录 `PolylineRebarGenerator`
- 任何 OCCT 直接造钢筋业务逻辑
- `domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_`

本轮验收标准：

1. 新增组内最小距离相关 legacy DTO，命名可在实现时微调，但必须表达：
   - 输入点。
   - 候选 edge refs。
   - 最小距离。
   - 命中的 edge stableId。
   - 点到命中 edge 的最近点。
   - 是否命中阈值。
2. 新增 adapter API，命名可在实现时微调，例如：
   - `pointToEdgeGroupDistance(point, edgeRefs, threshold)`
   - 或 `edgeGroupMinDistance(point, edgeRefs, threshold)`
3. API 输入必须是 `LegacyPoint3d + std::vector<LegacySelectionRef>` 或等价 legacy 引用列表，不能引入钢筋业务对象。
4. 真实 `123.stp` 上至少覆盖：
   - 单 edge candidate：点到 edge 的最小距离和最近点可用。
   - 多 edge candidates：返回最小距离所属 edge stableId。
   - 空 group：稳定拒绝。
   - wrong type / missing ref：返回稳定 diagnostic。
   - threshold 命中：距离小于等于 `0.002` 时可标记为 tooClose / hit。
5. 默认 CTest 通过。
6. readiness gate 严格模式通过，或记录明确失败原因。
7. 新增 `49_M1-App-012LegacyGeometryAdapterP3D实现记录.md`。
8. 新增 `docs/phase1/app_build_reports/m1_app_012_run_001.md` 和必要 JSON。
9. 更新：
   - `00_总览.md`
   - `11_需求证据追溯矩阵.md`
   - `34_Phase1ReadinessGate实际运行记录.md`
   - `99_缺口和待确认项.md`
   - `todo.csv`
10. `todo.csv` 中 `TODO-012` 改为 `done`；只把下一个明确可执行任务改为 `next`，但不继续实现。

本轮完成后必须停止，输出阶段复盘：

```text
完成了什么
验证了什么
还缺什么
下一阶段建议做 TODO-013 还是先补 IDA / 运行证据
```

不要在同一个 goal 内继续做 `TODO-013`、钢筋领域模型、Detail writer 或 UI 复刻。

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
   - 看文档体系、当前阶段、已完成 M1-App-001 到 M1-App-011。
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
    - 看 adapter 已有能力，避免重复实现。
23. `【图石钢筋1比1复刻】\99_缺口和待确认项.md`
    - 看当前缺口，所有不确定项必须回写这里。
24. `【图石钢筋1比1复刻】\todo.csv`
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

当前最新验证状态：

```text
app 默认 CTest = 8/8 pass
readiness gate = M1-Formal-Ready, 78/78 pass
domain/rebar OCCT 边界 = pass
```

当前下一步：

```text
TODO-012 / M1-App-012
  -> LegacyGeometryAdapter P3D
  -> 组内最小距离检查
```

原因：

```text
旧 sub_1405D5670 在 split / trim 后还会判断新边端点
到已有钢筋组的最小距离。
这个能力来自 sub_14059B980 / api_entity_point_distance。
所以 M1-App-012 必须先补点到 edge group 的最小距离 summary。
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
9. 更新实现记录文档、build report、`11_需求证据追溯矩阵.md`、`99_缺口和待确认项.md`。
10. 更新 `todo.csv`：完成项改为 `done`，下一个可执行项改为 `next`。

### 禁止事项

禁止：

- 把父目录 `src/rebar/*` 当作旧图石业务真相迁入。
- 把 `RebarCreationCommandService`、`EdgeToRebarFactory`、`FaceRebarGenerator`、`PolylineRebarGenerator` 当主线复用。
- 在 `domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_` 等 OCCT / AIS 细节。
- 用“OCCT 能怎么做”替代“旧图石怎么做”。
- 不确定旧逻辑时直接写死结论。
- CTest 或 gate 失败时继续堆新功能。
- 因为没有 golden 就跳过证据追溯。

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
- 旧逻辑不确定时，优先用 IDA MCP 或旧图石运行确认闭合，不用父目录代码替代旧图石证据。

## CSE v2 Control Contract

- **Primary Setpoint**：本轮只完成 `TODO-012 / M1-App-012`，让 `LegacyGeometryAdapter` 具备点到 edge group 的最小距离 legacy 几何语义。
- **Acceptance**：组内最小距离 DTO/API、真实 `123.stp` 集成测试、默认 CTest、readiness gate、实现记录、build report、`todo.csv` 和追溯文档全部闭合。
- **Guardrail Metrics**：不能让 OCCT 细节泄漏进 `domain/rebar`；不能把父目录钢筋生成器当业务真相；不能用“OCCT 能做什么”替代“旧图石怎么做”。
- **Sampling Plan**：先跑/补 integration test，再改 adapter；实现后运行默认 CTest；最后运行 readiness gate；完成后更新 evidence / gap / todo。
- **Known Delays**：IDA MCP 当前可能没有绑定数据库；旧图石运行确认依赖用户操作；真实 golden 对照要等旧软件可稳定导出。
- **Recovery Target**：发现路线偏移时，停止继续开发钢筋业务，先回到文档和 adapter 边界修正。
- **Rollback Trigger**：`domain/rebar` 出现 OCCT include、父目录 rebar 业务被迁入、测试失败但继续堆功能、旧逻辑无证据却写成确定结论。
- **Constraints**：不使用 ACIS / HOOPS / Codejock 等商业库；不读取完整私有 SFL 作为新主格式；新工程格式结合 SFL 业务语义和 OCCT 几何引用设计。
- **Boundary**：本轮只允许修改 geometry legacy DTO、OCCT adapter、adapter 集成测试、M1-App-012 文档和任务看板；父目录只读参考。
- **Coupling Notes**：`LegacyGeometryAdapter` 是几何能力边界；`domain/rebar` 是业务对象边界；`DetailWriter` 和新设计文件格式是输出 / 持久化边界。
- **Approximation Validity**：当前 adapter 的 split / interval 是 legacy summary，不等价于真实 topology mutation；必须在文档中标明能力等级。
- **Actuator Budget**：本轮只推进 `TODO-012`。完成后停止复盘，不自动进入 `TODO-013`。
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
TODO-012 / M1-App-012
  -> 组内最小距离检查
  -> 对齐旧 sub_14059B980 / api_entity_point_distance 链
```

原因很简单：旧图石在 split / trim 之后还会判断新边端点到已有 group 边的最小距离，
避免生成太近、重复或不稳定的边。这个能力不补，后面线筋、弧筋、裁剪、
端点修正和组内去重都会缺一个关键判断。
