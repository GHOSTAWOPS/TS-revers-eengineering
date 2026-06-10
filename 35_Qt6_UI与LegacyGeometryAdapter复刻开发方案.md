# Qt6 UI 与 LegacyGeometryAdapter 复刻开发方案

## 目标

本文件把 Qt6 界面复刻、旧命令映射、OCCT 替代边界和钢筋业务创建责任集中说明。

它回答三个开发问题：

```text
UI 怎么复刻？
OCCT 替代什么、不替代什么？
钢筋到底由谁创建？
```

本文件是开发护栏，不替代以下专项文档：

- `01_功能操作矩阵.md`：旧图石有哪些按钮和入口。
- `02_界面窗口参数矩阵.md`：旧 Dialog 和参数字段。
- `06_技术路线与替代方案.md`：Qt6 / OCCT / 商业库替代边界。
- `08_开发命令契约.md`：命令状态机、dirty、验收方式。
- `17_一期按钮追溯与命令占位矩阵.md`：按钮到 Requirement / CommandId / GAP 的门禁表。
- `23_父目录源码参考边界与路线纠偏.md`：父目录源码只能参考，不能作为业务真相。

## 当前结论

正确路线不是：

```text
OCCT 能做什么
  -> 就用 OCCT 重新写一套钢筋生成器
```

正确路线是：

```text
旧图石 VisualTS 证据需要什么业务语义
  -> Qt6 复刻旧操作入口和参数体验
  -> LegacyUiCommandMap 对齐旧命令
  -> 钢筋业务层按旧 VisualTS 逻辑创建钢筋
  -> LegacyGeometryAdapter 用 OCCT 提供旧 EDGE/FACE/ENTITY_LIST/curve 语义
  -> Detail writer 输出旧 AutoCAD 插件可导入的工程图包
```

一句话：

```text
Qt6 / OCCT 替代旧界面库、显示库、几何库。
钢筋生成、编辑、统计、出图规则必须按旧图石业务逻辑复刻。
```

## 总体架构

```text
用户点击按钮 / 选择边面 / 输入参数
  -> Qt6 MainWindow / RibbonLikeToolbar / DockPanel / Dialog
  -> LegacyUiCommandMap
  -> CommandSession / CommandStateMachine
  -> VisualTSRebarDomain
  -> LegacyGeometryAdapter
  -> OCCT AIS + OCCT Geometry API
  -> SteelBar / SteelBarSegment / SteelBarGroup / SteelData
  -> AIS RebarPresentation
  -> Detail.xml + DetailXX.stl
```

分层职责：

- `Qt6 UI`：主窗口、页签、按钮、参数窗口、模型树、属性面板、状态栏。
- `LegacyUiCommandMap`：旧按钮、旧命令字符串、Dialog、CommandId 的映射。
- `CommandSession`：命令状态、选择流程、取消、失败、dirty、撤销记录边界。
- `VisualTSRebarDomain`：按旧图石证据创建和修改钢筋对象。
- `LegacyGeometryAdapter`：把 OCCT 包装成旧 ACIS / HOOPS 风格的几何语义。
- `OCCT AIS`：显示、选择、高亮、旋转、缩放、平移。
- `OCCT Geometry API`：距离、长度、参数点、相交、裁剪、剖切、扫掠等几何计算。
- `DetailWriter`：输出旧 AutoCAD 插件包，不直接写 DWG / DXF。

## UI 复刻口径

UI 复刻目标是“功能操作 1 比 1”，不是皮肤像素级复刻。

必须复刻：

- 顶部页签：开始、显示、钢筋、查询、工程图。
- 旧按钮名称、分组、启用/禁用条件。
- 参数窗口字段、默认值、单位、灰显状态。
- 选择顺序、状态栏提示、错误提示。
- 模型树、属性面板、选择联动。
- 右键菜单和旧命令入口。
- 操作完成后的 dirty、刷新、工程图过期状态。

首期不追求：

- MFC / Codejock 皮肤像素级一致。
- 旧 `.sfl` 作为主保存格式。
- 新程序直接输出 DWG / DXF。
- 逐 API 复刻 HOOPS / ACIS。

## 钢筋创建责任

老图石里：

```text
图石自己的 steelbar / steelbargroup / seg_steelbargroup 业务代码创建钢筋。
ACIS 只提供 ENTITY_LIST、EDGE、FACE、距离、曲线拆分、样条等几何能力。
HOOPS 主要负责显示、选择、高亮。
```

新系统里：

```text
我们复刻的 VisualTSRebarDomain 创建钢筋。
OCCT 只通过 LegacyGeometryAdapter 提供几何能力。
OCCT AIS 只负责显示和选择。
```

典型证据链：

```text
sgroupbarline / sgroupbararc
  -> sub_1404D10C0
  -> sub_140451730
  -> sub_1405D5670
  -> seg_steelbargroup.cpp
```

这说明线配筋、弧形组等逻辑不是 ACIS 自动生成，而是旧图石自己的分段钢筋组逻辑在做创建、裁剪、分裂、样条和刷新。

TODO-071 已进一步把 `sgroupbarline` 入口契约冻结为：

```text
单选对象
内部子项不少于 2
奇数索引子项抽取为 ENTITY_LIST
四组端点距离候选
10.0 初始最小距离候选
sub_1404D10C0(entityList,objA,objB,minDistance,selectedEndpointDistance,flag)
```

因此新 UI handler 的 P0 可以先接 `LegacySelectionRef + 参数 -> RebarGroupCreator`，
但必须把旧入口契约、低置信字段和 gap 保留到领域对象 evidence/raw 中。

## LegacyGeometryAdapter 边界

钢筋业务层应该依赖旧语义接口：

```text
LegacyEntityRef
LegacyEdgeRef
LegacyFaceRef
LegacyEntityList
LegacyCurveMeasure
LegacyWireChain
LegacySurfaceMeasure
LegacyIntersectionResult
LegacyTrimResult
LegacySweepResult
```

这些接口内部可以使用 OCCT：

```text
TopoDS_Edge
TopoDS_Face
BRepAdaptor_Curve
BRepAdaptor_Surface
BRepAlgoAPI_Section
BRepOffsetAPI_MakePipeShell
```

但钢筋业务层不得直接散落这些 OCCT 类型和 API。

禁止模式：

```text
Rebar.Create.Face 直接接收 TopoDS_Face
Rebar.Create.LineGroup 直接 switch BRepAdaptor_Curve::GetType()
Rebar 业务代码直接调用 BRepAlgoAPI_Section
```

允许模式：

```text
Rebar.Create.LineGroup 接收 LegacyEntityList / LegacyEdgeRef
业务层调用 LegacyCurveMeasure.length()
业务层调用 adapter.splitCurve(...) / adapter.distance(...)
adapter 内部再用 OCCT 实现
```

## 父目录源码使用规则

父目录源码可参考：

- Qt6 / OCCT 工程搭建。
- Viewer 集成方式。
- STEP 导入样例。
- 测试组织方式。
- Detail writer 的局部工程经验。

父目录源码不能作为业务真相：

- `EdgeToRebarFactory` 直接从 `TopoDS_Edge` 创建钢筋。
- `FaceRebarGenerator` 直接从 `TopoDS_Face` 创建面筋。
- 多个 `src/rebar` 模块直接依赖 `BRepAdaptor_* / BRepAlgoAPI_*`。

这些代码更接近“OCCT 直接重写钢筋能力”，不是本项目的 1 比 1 复刻路线。

如后续复用，必须先经过兼容层改造。

## TODO-072 后的 LineGroup 命令接入边界

`TODO-072 / E-DEV-094` 已把 `Rebar.Create.LineGroup` 从 UI 命令入口接到 P0 事务链：

```text
Qt QAction / CommandId::RebarLineCreate
  -> RebarLineGroupCommandHandler
  -> LegacySelectionRef(edge-p0-surrogate)
  -> LegacyRebarGeometryReader
  -> RebarGroupCreator
  -> SteelData append
```

这里有一个明确的 P0 过渡层：

```text
MainWindow / ViewerLegacyRebarGeometryReader
  -> OccViewerWidget.currentSelectionRef()
  -> OccLegacyGeometryAdapter
```

这层属于 app / presentation 桥接，允许内部用 OCCT 读取当前 viewer 的 edge geometry；但 `RebarLineGroupCommandHandler` 和 `domain/rebar` 不直接接收 `TopoDS_Edge / AIS_Shape / BRepAdaptor_Curve`。

当前仍未闭合：

```text
旧 VisualTS 完整 ENTITY_LIST 选择对象语义
旧 UI 参数窗口 / 状态栏提示 / 失败提示
命令完成后的 AIS 钢筋显示刷新
undo / dirty / save / golden
```

## 开发顺序

### 1. Qt6 操作外壳

目标：

- 主窗口。
- 五个一期页签。
- 按 `01` 和 `17` 注册按钮。
- 每个按钮进入统一命令系统。
- 未实现命令只允许明确占位，不能假装成功。

验收：

- 按钮文字和分组与旧图石矩阵一致。
- `CommandId` 唯一。
- 点击后有状态栏反馈或参数窗口。

### 2. 命令状态机

目标：

- 使用 `08` 定义的状态：

```text
Idle -> Armed -> Picking -> ParameterEditing -> Preview -> Applying -> Completed
```

- 支持取消和失败回滚。
- 记录 dirty。
- 选择集可被命令访问。

### 3. OCCT Viewer 与选择

目标：

- STEP/STP 导入。
- AIS 显示、旋转、缩放、平移、全显。
- 面、边、点选择。
- 选择对象转换为 `LegacyEntityRef / LegacyEdgeRef / LegacyFaceRef`。

### 4. LegacyGeometryAdapter

目标：

- 封装曲线长度、参数点、端点、切向、线面求交、距离、裁剪、剖切。
- 对齐旧 `EDGE / FACE / ENTITY_LIST / SPAposition` 语义。
- 不把 OCCT 类型暴露给钢筋业务层。

### 5. 钢筋创建专项

优先顺序：

1. 面配筋：按 `14` 做 UI / 参数 / 选择流程样板。
2. 线配筋：按 `15`、`16` 追 `sgroupbarline` 和 `seg_steelbargroup`。
3. 扇形筋 / 同心圆：按 `sgroupbararc` 公共链闭合。
4. 编辑、检查、查询、工程图按 `17` 和 `99` 逐项推进。

每个钢筋命令必须有：

- 旧按钮 / Dialog / 命令字符串证据。
- 输入选择对象。
- 参数窗口字段。
- 旧业务算法链。
- 新领域对象映射。
- Detail 字段影响。
- golden 或运行确认计划。

## 开发门禁

进入正式业务实现前必须满足：

- 功能入口在 `01`。
- 参数窗口在 `02`。
- CommandId 在 `08` 或 `17`。
- 技术路线符合 `06`。
- 父目录参考边界符合 `23`。
- 未确认项进入 `99`。

业务实现过程中必须满足：

- 旧证据先于实现。
- `TopoDS_* / BRepAdaptor_* / BRepAlgoAPI_*` 不进入钢筋业务层。
- 未闭合旧行为必须标记为 `PARTIAL` 或进入 GAP。
- 不能用父目录 OCCT 直写逻辑关闭旧图石业务缺口。

## 当前状态

当前文档体系已达到：

```text
M1-Formal-Ready
78 / 78 pass
```

当前正式 app 已达到：

```text
M2-RebarCreate-015 / TODO-085 done
  -> Qt6 主窗口和旧图石一期页签
  -> OCCT STEP 导入和 AIS 显示
  -> selection-v1 face / edge / vertex 稳定选择引用
  -> LegacyGeometryAdapter P0 到 P3E
  -> EDGE / FACE bbox、采样、boundary loop、fingerprint 和诊断
  -> edge 切向、selection ref 距离和最近点对
  -> face boundary edge stableId 和 edge-face 接触/重叠代表点
  -> edge 参数区间、子段长度、子段 bbox 和子段采样
  -> edge split / projected split / endpoint trim / wire chain / offset / section / sweep preview
  -> domain SteelData / SteelBarGroup / SteelBar / SteelBarSegment P1
  -> Rebar.Create.LineGroup P0 creator / command handler / UI AIS visible feedback / parameter dialog / selection preflight
  -> 旧 UI 失败提示 / 状态栏口径 IDA 静态 stop point
  -> 旧图石运行确认采样清单和拒收伪工件规则
  -> 旧图石运行工件回填审计：当前只有模板，真实截图 / hash / listing 缺失
  -> sub_1404D10C0 公共创建 gate P0：objB / objA / sub_1405F25F0(objA) / ENTITY_LIST / distanceA
  -> sub_1404D10C0 createdPayload / linkedModelRef / distanceA_4digit / objA-objB role 静态补证
  -> roles DTO / raw evidence P0：LegacyPublicCreateRolesSnapshot、createdPayloadRef、linkedModelRef 低置信、distanceA_4digit normalizer 参数
  -> sub_1405D5670 split / spline / trim trace P0：LegacySegmentCurveNormalizeTrace、api_entity_entity_distance、api_split_curve、api_curve_spline、endpoint trim
  -> sub_1405D5670 group-min-distance / dirty-write gap contract P0：sub_14059B980、sub_1405BD0C0、sub_1405E49D0 链路位置和 deferred 状态
  -> Rebar.Create.LineGroup 成功 dirty/transaction P0：ProjectDirty + RebarDirty + DrawingDirty、E-IDA-049、GAP-REB-C-002；失败 / 取消不 dirty
```

这代表：

- 功能矩阵、命令契约、领域模型、Detail 映射、新工程格式、binding、依赖门禁、runtime gate 已具备进入 Qt6 + OCCT 开发的入口条件。
- OCCT 能力已经开始被封装成旧 VisualTS 可用的几何语义，而不是直接暴露给钢筋业务层。
- 线配筋首条主链已经能从 UI 选择 edge，先通过最小 edge 预检，再打开 P0 参数窗口，进入 `RebarGroupCreator`，生成 domain `SteelBarGroup`，并通过 presentation 层显示到 AIS。
- `TODO-076 / E-IDA-046` 已确认 `sgroupbarline` handler 和 secondary helper 内没有直接中文失败提示字符串；公共创建链只收窄到 ACIS outcome / part state 路径和公共 `Input_float / Dialog #383`。这让旧 UI 失败提示和状态栏口径更窄，但尚未闭合。
- `TODO-077 / E-DEV-099` 已把旧图石现场采样要求落成 `runtime_capture/todo_077_line_group_ui_prompt_capture/` 模板：无选择、选错对象、有效对象、旧主参数窗口、`Dialog #383`、状态栏 pane、模型树 / 输出前后状态和拒收规则都有记录口径。
- `TODO-078 / E-DEV-100` 已确认当前采样目录只有模板，没有真实旧图石截图、hash、listing 或填写后的运行 notes，所以运行回填 blocked；这避免了把空模板误读成旧 UI 已确认。
- `TODO-079 / E-IDA-047 / E-DEV-101` 已完成旧 UI / Dialog 静态资源补证：`sgroupbarline` 表项、公共 `Input_float / Dialog #383` 和状态栏 API 调用层已复核；没有找到直接绑定到线配筋的中文 UI / 状态栏文案，因此旧 UI 真实行为仍需运行确认。
- `TODO-080 / E-DEV-102` 已把 `sub_1404D10C0` 已确认的公共创建 gate 落到 `RebarGroupCreator` P0：在几何查询前校验 `objB / objA / sub_1405F25F0(objA) >= 3 / ENTITY_LIST count >= 1 / distanceA >= 0.002`，并把 gate 快照写入 raw evidence。该实现只对齐旧 gate 和 diagnostic，不关闭完整旧创建对象语义。
- `TODO-081 / E-IDA-048 / E-DEV-103` 已把公共创建对象角色继续收窄：`createdObject + 104` 可作为 `createdPayload`，`createdObject + 112` 只能低置信记为 `linkedModelRef / createdLinkRef`，`sub_1405D5670` 第 4 个 double 是 `distanceA_4digit`，且 `objA / objB` 在线配筋和弧形组入口会交换角色，不能硬编码为固定源/目标组。
- `TODO-082 / E-DEV-104` 已把 TODO-081 的静态证据落进正式 app：`RebarGroupCreator` 新增 `LegacyPublicCreateRolesSnapshot`，在 `createdFromParameters / legacyRaw` 记录 `createdPayloadRef / linkedModelRef / distanceA_4digit / objA-objB role`，并让 normalizer 使用 `distanceA_4digit`。这只是 roles DTO / raw evidence 边界，不是完整旧对象模型。
- `TODO-083 / E-DEV-105` 已把 `sub_1405D5670` 的 split / spline / trim 链落成 P0 trace：`RebarGroupCreator` 只接收 `LegacySegmentCurveNormalizeTrace / Result`，`MainWindow` 的 viewer reader 通过 `OccLegacyGeometryAdapter` 生成 summary，OCCT 没有进入 `domain/rebar`。这只是 `api_entity_entity_distance / api_split_curve / api_curve_spline / endpoint trim` 的 trace，不是完整 topology mutation、group min-distance loop 或 dirty 写回等价。
- `TODO-084 / E-IDA-049 / E-DEV-106` 已把 `sub_14059B980` group min-distance loop、`sub_1405BD0C0` backup/write edge 和 `sub_1405E49D0` dirty call position 写成 P0 gap contract，并新增 false applied claim 防线。它仍不等价旧 ACIS group list、真实 topology mutation、旧 dirty / undo / save parity、旧 UI 或 golden。
- `TODO-085 / E-DEV-107` 已把新 app 线配筋命令成功后的 dirty/transaction P0 接通：成功创建才标记 `ProjectDirty + RebarDirty + DrawingDirty`，并追溯到 `E-IDA-049 / GAP-REB-C-002`；空选择、错类型、取消参数窗口和失败路径不 dirty。这只是 app-level dirty P0，不是旧 `sub_1405E49D0` 完整 dirty / undo / save parity。
- `TODO-086 / E-DEV-108` 已把新 app 的 `Project.Save` P0 保存清除入口接通：保存成功清 `ProjectDirty / RebarDirty / DrawingDirty`，保存失败保持 dirty，并在 app 保存边界把线配筋 binding geometryPath 重写为 runtime 可验证 JSON path。这只是新 app save-clear P0，不是旧图石完整保存 UI、旧 undo/save parity 或关闭提示。

这不代表：

- 旧图石所有钢筋业务已经复刻完成。
- 线配筋旧参数窗口字段 / 默认值 / 灰显状态、旧失败提示、状态栏 pane 文案、完整 `ENTITY_LIST` 选择对象、`createdObject + 112` 准确业务名、`objA/objB` 旧源码真实类名、完整 `sub_1405D5670` topology mutation / dirty 写回等价、旧 dirty / undo / save parity、弧形组、同心圆等复杂算法尚未闭合；这些仍处于待 IDA 或旧图石运行确认状态。
- 当前 `LineGroupParameterDialog` 是 P0 参数输入壳；`Input_float / Dialog #383` 是旧公共浮点输入窗；二者都不能被写成旧线配筋主参数窗口已经 1:1 复刻。
- `TODO-077` 只准备了运行确认包，`TODO-078` 已明确当前没有真实旧图石运行截图或参数窗口字段；`TODO-079` 的静态补证也不能替代现场运行确认。
- `LegacyPublicCreateRolesSnapshot` 只是防止证据丢失和角色误硬编码的 P0 DTO；不能把 `createdObject + 112` 或 `objA/objB` 写成已闭合旧业务名。
- 当前 `DirtyState / CommandTransactionState` 只是新 app 的最小状态污染边界；不能写成旧图石保存提示、撤销栈、关闭提示或保存成功清 dirty 已经 1:1 复刻。
- 父目录源码可以直接拿来当最终业务代码。
- 钢筋创建、编辑、统计、工程图业务规则已经由 OCCT 自动解决。

后续进入开发时，本文件作为架构护栏使用。
