# M0-Pivot-002 全量 Markdown 路线审计记录

## 目标

本轮响应“再检查一遍所有 md，确保方向没问题”。

审计目标不是继续开发新功能，而是确认 2026-06-11 路线切换后，
Markdown 文档不会继续把后续开发带回旧 VisualTS UI / 操作流程全量 1 比 1 路线。

当前权威路线：

```text
STEP-only + RebarSmart 钢筋生成逻辑证据 + 图石 Detail 包兼容导出
```

当前 next：

```text
TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0
```

## 审计范围

重点审计入口和高频被 goal 读取的文档：

- `00_总览.md`
- `06_技术路线与替代方案.md`
- `08_开发命令契约.md`
- `09_钢筋领域模型草案.md`
- `18_新设计文件格式替代SFL策略.md`
- `23_父目录源码参考边界与路线纠偏.md`
- `35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`
- `36_正式Qt6_OCCT工程架构与首个实现切片.md`
- `46_CSE_v2Goal执行目标与Todo说明.md`
- `99_缺口和待确认项.md`
- `README.md`
- `AGENTS.md`
- `docs/adr/ADR-20260611-step-only-rebarsmart-detail.md`
- `docs/roadmap/STEP_only_RebarSmart_Detail_refactor_plan.md`
- `TODO-087` 收尾实现记录和 build report 中的历史 next

同时用 `rg --glob "*.md"` 扫描所有 Markdown 的高风险词：

```text
当前主线 / 当前 next / 下一步 / TODO-088 / TODO-090
VisualTS / 1:1 / 1 比 1 / LegacyGeometryAdapter
RebarSmart / STEP-only / DetailPackage
```

## 修正内容

### 1. 总览口径修正

`00_总览.md`：

- 将“历史目标”明确标为“已降级”。
- 删除旧 UI 功能操作 1 比 1 的当前口吻。
- 将 `35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md` 标为旧路线历史方案和局部兼容参考。
- 将 `98_M2-Control-001` 的旧接头暂缓切换记录改为“当时业务纠偏”，并补充 2026-06-11 后当前主线以 `TODO-090` 为准。

### 2. 技术路线修正

`06_技术路线与替代方案.md`：

- 将旧 Qt6 + OCCT + VisualTS 1 比 1 分层标为历史结论。
- 将当前最快路线修正为：

```text
Qt6 + OCCT + RebarSmart 证据驱动的自研钢筋生成层 + 图石 Detail 兼容 writer
```

- 将钢筋业务关键路径修正为 RebarSmart 证据整理、自研算法化和 Detail 兼容闭环。

### 3. 命令契约修正

`08_开发命令契约.md`：

- 顶部新增 2026-06-11 路线状态。
- 明确 TODO-071 到 TODO-087 是旧路线历史成果。
- 明确 `TODO-088` 已降级为 pending。
- 明确当前 next 是 `TODO-090 / DetailPackage 数据模型 P0`。

### 4. 领域模型修正

`09_钢筋领域模型草案.md`：

- 顶部新增路线状态。
- 明确旧 `LegacyGeometryAdapter / LegacyRebarGeometryReader` 链路是旧 VisualTS 线配筋历史成果。
- 后续生成器应转向 RebarSmart 证据驱动的自研领域模型和 `IGeometryEngine` 边界。
- 继续保留 `domain/rebar` 不依赖 OCCT / AIS 的硬约束。

### 5. 新工程格式修正

`18_新设计文件格式替代SFL策略.md`：

- 明确 `.tsrebar` 仍是内部主工程格式。
- 钢筋生成字段和对象关系优先由 RebarSmart 证据驱动。
- Detail / 工程图 / 下料表字段继续由旧图石 Detail 包和运行证据驱动。
- `.sfl` 和 IDA 降级为历史字段、兼容和缺口旁证。

### 6. 父目录参考边界修正

`23_父目录源码参考边界与路线纠偏.md`：

- 保留“父目录源码只能参考，不能作为业务真相”的边界。
- 将业务真相优先级改为：

```text
RebarSmart 证据
  -> 图石 Detail 包 / 下料表 / 旧 CAD 插件
  -> VisualTS / IDA / SFL
  -> 父目录源码只作工程写法参考
```

### 7. UI / 架构旧方案降级

`35_Qt6_UI与LegacyGeometryAdapter复刻开发方案.md`：

- 顶部新增“历史方案 / 已降级”说明。
- 明确旧 VisualTS UI / 操作流程全量 1 比 1 不再是 P0/P1。
- 明确新钢筋生成主线不再叫 `VisualTSRebarDomain`，应转向 RebarSmart 证据驱动的自研结构。

`36_正式Qt6_OCCT工程架构与首个实现切片.md`：

- 顶部新增路线状态。
- 将原“路线总口径”标为历史路线总口径。
- 明确旧 UI 全量 1 比 1 和 TODO-088 都不是当前 next。
- 明确 P0/P1 支撑 STEP-only、DetailPackage、RebarSmart-style 生成器和兼容导出。

### 8. TODO-087 历史 next 收口

`125_M2-RebarCreate-017线配筋保存包Runtime回读验证P0实现记录.md`：

- 顶部新增路线状态。
- 将 `TODO-088` 改为历史下一步，并声明已降级。
- 明确当前 next 是 `TODO-090`。

`docs/phase1/app_build_reports/m2_rebar_create_017_run_001.md`：

- 将路线切换前的旧 next 元数据改为历史 next 元数据。
- 新增 `currentNextAfterPivot = TODO-090 / M1-Detail-001 DetailPackage 数据模型 P0`。

## 审计结论

当前入口文档、goal、ADR、roadmap、缺口表和 todo 看板方向一致：

```text
当前主线 = STEP-only + RebarSmart 证据 + 图石 Detail 兼容
当前 next = TODO-090 / DetailPackage 数据模型 P0
```

旧 VisualTS 1 比 1、LegacyGeometryAdapter、线配筋保存/打开链、接头链路等内容没有删除，
但已经明确标为历史成果、局部兼容参考或 P2 / 专项，不再抢占当前 P0/P1。

## 验证

已执行高风险旧路线短语扫描：

```powershell
rg -n "<stale-route-patterns>" --glob "*.md"
```

结果：

```text
no matches
```

已执行：

```powershell
Import-Csv todo.csv | Where-Object {$_.status -eq 'next'} | Select-Object id,phase,task,status
```

结果：

```text
TODO-090 / M1-Detail-001 / DetailPackage 数据模型 P0 / next
```

已执行：

```powershell
git diff --check
```

结果：

```text
pass
```

备注：`git diff --check` 仅提示部分工作副本 CRLF 将来可能被 Git 触碰时转为 LF，
未报告 whitespace error。

## 残余风险

- 大量旧实现记录仍包含“当时下一步”“当时 nextTodo”等历史语境，不应批量改写。
- 后续 agent 读取旧实现记录时，必须优先读取 `00 / 46 / ADR / roadmap / todo.csv`。
- RebarSmart 证据只能整理为自研算法契约，不得提交二进制、授权文件、客户样本或敏感私有逆向清单。
