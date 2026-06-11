# CSE v2 Goal 执行目标与 Todo 说明

## 一句话结论

后续 goal 模式的主线已经调整：

```text
不是继续做“旧 VisualTS UI / 操作流程全量 1:1”。

而是在现有 Qt6 + OCCT app 基础上，
走 STEP-only + RebarSmart 钢筋生成逻辑证据 + 图石 Detail 包兼容导出。
```

白话说：

- RebarSmart3DE 负责给我们“钢筋怎么生成”的证据。
- VisualTS / 老图石负责给我们“Detail 包、工程图、下料表、旧 CAD 插件怎么兼容”的证据。
- Qt6 + OCCT 是新软件唯一长期运行时底座。
- 新系统不继承 USB 狗、HASP、3DE、ACIS、HOOPS、Codejock 等商业运行时依赖。

## 当前路线切换记录

2026-06-11 已建立 pivot 前时间线：

```text
tag: before-rebarsmart-detail-pivot-20260611
```

当前正式路线文档：

- `docs/adr/ADR-20260611-step-only-rebarsmart-detail.md`
- `docs/roadmap/STEP_only_RebarSmart_Detail_refactor_plan.md`

旧 VisualTS 1:1 复刻文档保留为历史证据，但不再作为 P0/P1 主执行路线。

## 长期 Goal（可直接复制）

创建并执行长期 goal：

```text
目标：
长期推进图石钢筋替代系统正式 app 开发。
严格按 46_CSE_v2Goal执行目标与Todo说明.md 和 todo.csv 执行。

核心路线：
外部几何输入只支持 STEP/STP。
Qt6 只负责新界面、菜单、参数面板和命令入口。
OCCT 负责 STEP 导入、几何计算、AIS 显示、选择和高亮。
RebarSmart3DE 只作为钢筋生成逻辑证据源，不作为运行时依赖。
VisualTS / 老图石只作为 Detail 包、工程图字段、下料表、旧 AutoCAD 插件兼容和必要历史证据源。
.tsrebar 是新系统内部主工程格式。
Detail.xml + DetailNN.stl XML 包是旧 AutoCAD 插件兼容导出格式，不是内部主数据格式。

执行规则：
每轮只做 todo.csv 中 status=next 的一个节点。
先读 46 文档、todo.csv 和该任务 evidence 指向的文档。
涉及代码时先补测试，再实现。
实现后运行相关测试、默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查。
涉及代码、测试、构建脚本的节点，commit 前必须执行 xhigh 只读 review。
xhigh 只能 review，不能修改；修改由主流程 agent 完成。
Critical / Important 必须修复或写明技术反驳理由。
完成后更新实现记录、build report、追溯矩阵、99缺口、todo.csv、46文档。
最后 commit、打 annotated tag、push main 和 tags。

禁止：
不要把 RebarSmart DLL / VisualTS / 3DE / ACIS / HOOPS / Codejock 接进新系统运行时。
不要用 OCCT 直接自由发挥写一个“差不多”的钢筋软件。
不要迁入父目录 rebar 业务代码。
不要让 domain/rebar 依赖 TopoDS_ / AIS_ / BRep / TopAbs_。
不要把 DetailNN.stl 当标准 STL 网格；它是旧插件使用的 XML 文本。
不要把旧 VisualTS UI 全量 1:1 当 P0/P1 阻塞项。
不要把公司内部 RebarSmart 二进制、真实客户样本、授权文件或敏感配置提交到公开仓库。

停止条件：
完成一个节点后复盘并停止，不自动进入下一个节点，除非用户明确说继续。
```

## CSE v2 Control Contract

```text
Primary Setpoint
  把正式 app 推进成独立 Qt6 + OCCT 新平台：
  STEP 输入，RebarSmart-style 生成逻辑，自研 RebarModel / DrawingModel，
  并能导出旧图石 CAD 插件可读的 Detail 包。

Acceptance
  每个切片有测试、实现记录、build report、追溯矩阵、缺口记录、
  todo 状态、必要 xhigh review、commit、annotated tag、push 结果。

Guardrail Metrics
  运行时不依赖 RebarSmart / VisualTS / 3DE / ACIS / HOOPS / Codejock。
  domain/rebar 不出现 TopoDS_、AIS_、BRep*、TopAbs_。
  不迁入父目录 rebar 业务。
  Detail 包只作为 exporter，不成为业务模型事实源。
  RebarSmart 只作为证据源，不作为二进制依赖。

Sampling Plan
  每轮开始看 todo.csv / 46 / 99 / git status。
  每轮结束跑默认 CTest、readiness gate、OCCT 泄漏检查。
  涉及代码、测试、构建脚本的节点，在验证通过后、commit 前执行 xhigh 只读 review。
  涉及 RebarSmart 逻辑时读取 INI / 导出符号 / 反编译证据。
  涉及 Detail 时读取旧 Detail 包、AutoCAD 插件运行记录和字段矩阵。

Known Delays
  RebarSmart 反编译证据需要逐步整理。
  RebarSmart INI 可能是 GBK/ANSI 编码，不能假定 UTF-8。
  旧 AutoCAD 插件 L2 验证依赖可运行的 AutoCAD 环境。
  VisualTS / 旧图石运行确认依赖 USB 狗和用户环境，但新软件不继承这些依赖。

Recovery Target
  如果路线滑回“旧 VisualTS UI 全量 1:1”或“OCCT 直接自由写钢筋”，
  立即停止功能开发，回到 ADR / 46 / todo / AGENTS 纠偏。

Rollback Trigger
  RebarSmart DLL 被接入运行时。
  VisualTS / 3DE / ACIS / HOOPS / Codejock 被接入运行时。
  domain/rebar 引入 OCCT/AIS。
  todo 与 46 的 next 目标不一致。
  测试或 gate 失败仍继续堆功能。
  代码节点跳过 xhigh 只读 review。

Boundary
  单轮只推进一个 todo 节点。
  M1 优先 Detail 包 POC。
  M2 再推进 RebarSmart 纯算法层。
  M3 再接 OCCT 几何接口和 FixDistance/FixNumber。
  旧 VisualTS 文档只作为历史证据和 Detail/出图证据，不再驱动 UI 全量 1:1。

Actuator Budget
  单轮只做一个可验证节点。
  完成后停止复盘，除非用户明确要求继续。
```

## 当前已知状态

已保留并可继续使用的资产：

- Qt6 app 骨架。
- STEP import probe。
- 最小 OCCT AIS Viewer。
- face / edge / vertex 选择系统。
- `selection-v1` 稳定引用。
- `LegacyGeometryAdapter` 已有大量几何兼容能力，后续降级为历史 VisualTS 支撑和局部兼容工具。
- `domain/rebar` 基础模型。
- `.tsrebar` 新工程包方向。
- DetailWriter / Detail 字段骨架 / 多图纸输出等历史成果。
- readiness gate、依赖合规、SBOM、测试和文档门禁。

已降级为非当前主线：

- `TODO-088 / Project.Open runtime snapshot 恢复到 app 内存 / 显示入口 P0`。
- 旧 VisualTS 线配筋保存/打开链继续作为历史成果，不作为新路线 next。
- 接头链路继续记录和暂缓。
- 旧 UI / 菜单 / 状态栏 / 弹窗全量 1:1 降为 P2。

## 当前下一步

当前 `todo.csv` 的下一步应是：

```text
TODO-090 / M1-Detail-001
  -> DetailPackage 数据模型 P0
```

原因：

```text
新路线的第一道门槛不是继续旧 VisualTS 保存/打开链，
而是证明旧图石 CAD 插件兼容链路可控。

所以 M1 先做 DetailPackage Reader / Writer / round-trip / 极简包，
再接 RebarModel、DrawingModel 和 RebarSmart-style 生成器。
```

## TODO-090 短期 Goal（下一轮复制）

目标：只完成 `TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0`，不自动进入 Reader / Writer / RebarSmart 算法。

工作目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件
```

正式开发目录：

```text
【图石钢筋1比1复刻】\app
```

本轮必须先读：

1. `46_CSE_v2Goal执行目标与Todo说明.md`
2. `todo.csv`
3. `docs/adr/ADR-20260611-step-only-rebarsmart-detail.md`
4. `docs/roadmap/STEP_only_RebarSmart_Detail_refactor_plan.md`
5. `05_Detail工程图包证据.md`
6. `13_Detail字段映射矩阵.md`
7. `20_DetailWriter输出事务契约.md`
8. `99_缺口和待确认项.md`

目标语义：

```text
建立 Detail 包的内部数据模型，
让后续 Reader / Writer / Exporter 都有统一承载对象。
```

建议输出：

```text
app/src/drawing/detail/DetailPackage.h
app/src/drawing/detail/DetailGeneralInfo.h
app/src/drawing/detail/DetailPrimitive.h
app/src/drawing/detail/DetailRebarGroup.h
app/src/drawing/detail/DetailTable.h
app/tests/... 对应 DetailPackage 模型测试
```

验收标准：

1. 能构造一个空 `DetailPackage`。
2. 能构造一个包含 `DrawingRoot / ViewPort / PartDetailDrawing / General-Info / section-line / StbDetailDrawing / StbGroups` 的最小对象。
3. 能构造钢筋表、材料表、钢筋组、几何段的 P0 字段对象。
4. 字段命名保持新系统语义，不能把业务模型直接命名成 `StbGroup1` 这类 XML 序号。
5. 不写 XML 文件；Writer 是 TODO-092。
6. 默认 CTest 通过。
7. readiness gate 通过，或记录明确失败原因。
8. domain/rebar 无 OCCT/AIS 泄漏。
9. 代码节点完成后必须 xhigh 只读 review。
10. 更新实现记录、build report、11、34、46、99、todo.csv。
11. commit、annotated tag、push main 和 tags。

本轮禁止：

- 不做 Reader。
- 不做 Writer。
- 不做 round-trip。
- 不做 CAD 插件导入验证。
- 不接 RebarModel。
- 不接 RebarSmart 生成器。
- 不迁入 RebarSmart DLL / VisualTS / 3DE / ACIS / HOOPS。
- 不把 Detail 包当内部主数据格式。

## 后续任务队列

推荐新队列：

```text
TODO-090 / M1-Detail-001  DetailPackage 数据模型 P0
TODO-091 / M1-Detail-002  DetailPackageReader P0
TODO-092 / M1-Detail-003  DetailPackageWriter round-trip P0
TODO-093 / M1-Detail-004  极简 Detail 包生成 P0
TODO-094 / M2-RebarSmart-001  GenerateRebarData 参数结构 P0
TODO-095 / M2-RebarSmart-002  RebarSmartIniLoader GBK/ANSI + unit handling P0
TODO-096 / M2-RebarSmart-003  SpaceListParser P0
TODO-097 / M2-RebarSmart-004  PrioritySpace/PriorityList Distributor P0
TODO-098 / M2-RebarSmart-005  GuideCurveZoneCalculator P0
TODO-099 / M3-Geometry-001  IGeometryEngine 接口 P0
```

## 证据优先级

钢筋生成：

1. RebarSmart3DE 类名、导出符号、INI 默认值、参数结构、反编译证据。
2. 旧图石导出的 STP / Detail / 下料表 / 截图等结果证据。
3. VisualTS / IDA 作为补充，不再单独驱动 P0 生成逻辑。
4. 父目录代码只能参考工程写法。

工程图 / Detail：

1. 旧图石真实导出的 `Detail.xml + DetailNN.stl` 包。
2. 旧 AutoCAD 插件实际导入结果。
3. VisualTS / FDrawing 静态证据和 IDA MCP。
4. 当前 DetailWriter / DrawingModel 测试。

## 执行规则

每轮 goal 模式按这个闭环执行：

1. 先读 `todo.csv`，选择唯一 `status=next` 的任务。
2. 如果没有唯一 next，停止并修 todo，不继续开发。
3. 读取该任务 `evidence` 字段指向的文档。
4. 读取相关代码和测试。
5. 涉及代码时先补测试，再改实现。
6. 默认只推进一个切片。
7. 修改后运行相关测试。
8. 运行默认 CTest。
9. 运行 readiness gate 或对应专项 gate。
10. 涉及 `domain/rebar` 时运行 OCCT/AIS 泄漏检查。
11. 涉及代码、测试、构建脚本时执行 xhigh 只读 review。
12. 更新实现记录、build report、追溯矩阵、99 缺口、46、todo.csv。
13. commit、annotated tag、push。

## 旧文档处理原则

不要批量删除旧文档。

旧文档按三类处理：

```text
保留可复用资产：
  Qt6 / OCCT / STEP / AIS / selection / .tsrebar / DetailWriter / gate。

降级为历史证据：
  VisualTS UI 全量 1:1、旧菜单、旧状态栏、旧线配筋保存打开链。

继续作为主证据：
  Detail 包字段、旧图石真实导出、旧 AutoCAD 插件兼容、下料表字段。
```

## 当前风险

- RebarSmart 证据来自公司内部包，不能把二进制和敏感内部资源提交到公开仓库。
- RebarSmart INI 编码可能是 GBK/ANSI，不能用 UTF-8 硬读。
- Detail 包字段容忍度必须靠旧 AutoCAD 插件实测。
- OCCT 的几何结果可能和 3DE / ACIS 有差异，后续要靠 golden 校准。
- 当前仓库如果仍是 public，应避免提交真实客户样本、授权文件、IDA 数据库和私有二进制。

## 完成后复盘格式

每轮完成后输出：

```text
完成了什么
验证了什么
还缺什么
下一阶段建议做哪个 TODO
是否已 commit / tag / push
```
