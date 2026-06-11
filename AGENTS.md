# Agent 工作规范

## 核心路线

本项目当前主线已在 2026-06-11 调整为：

```text
STEP-only + RebarSmart 钢筋生成逻辑证据 + 图石 Detail 包兼容导出
```

白话说：

- 新软件只吃 `STEP/STP` 作为外部几何输入。
- Qt6 负责新界面、菜单、参数面板、命令入口。
- OCCT 负责 STEP 导入、几何计算、显示、选择、高亮。
- RebarSmart3DE 只作为钢筋生成逻辑的证据源，用来复刻参数、默认值、分布、导引线/面、弯钩、11 类筋型等规则。
- VisualTS / 老图石只降级为 Detail 包、工程图字段、下料表、旧 AutoCAD 插件兼容和必要历史证据源。
- `.tsrebar` 是新系统内部主工程格式。
- `Detail.xml + DetailNN.stl` XML 包只是兼容旧 AutoCAD 插件的导出格式。

这不是继续做“旧 VisualTS UI / 操作路径全量 1:1 复刻”。旧路线保留为历史证据，但不再作为 P0/P1 主线。

## 硬约束

新软件运行时允许依赖：

- C++17 / C++20
- Qt6
- OCCT
- 自研 RebarModel / DrawingModel / DetailPackageExporter
- 必要的开源 XML / JSON / CSV / XLSX 辅助库

新软件运行时禁止依赖：

- RebarSmart DLL / EXE / 资源包运行时
- VisualTS EXE / DLL
- 3DE / CAA / CATIA / 3DEXPERIENCE runtime
- ACIS / Spatial
- HOOPS
- Codejock
- USB 狗、HASP、授权服务、内网许可作为新系统运行条件

禁止：

- 把 RebarSmart DLL 直接链接、加载或包装成后端。
- 把 VisualTS 旧程序当作新系统后端。
- 把父目录旧 `src/rebar/*`、`RebarCreationCommandService`、`EdgeToRebarFactory`、`FaceRebarGenerator`、`PolylineRebarGenerator` 当主线业务代码迁入。
- 在 `app/src/domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_` 等 OCCT / AIS 细节。
- 把 `DetailNN.stl` 当成标准 STL 网格处理；它在旧图石出图链里是 XML 文本。
- 把低置信逆向推断写成确定事实。
- 把公司内部 RebarSmart 二进制、真实客户样本、授权文件、详细私有符号或敏感配置提交到公开 GitHub。

允许：

- 参考既有 app 的 Qt6 / OCCT / STEP / AIS viewer / selection / `.tsrebar` / DetailWriter / test gate 资产。
- 参考父目录的 OCCT 工程写法，但不能把父目录 rebar 业务当事实源。
- 用 RebarSmart 的 INI、导出符号、类名、参数名和反编译证据整理算法规则。
- 用 VisualTS / Detail / 旧 AutoCAD 插件证据整理工程图包字段和兼容导出规则。

## 默认执行闭环

每次开发只推进一个清晰节点：

1. 先读 `46_CSE_v2Goal执行目标与Todo说明.md` 和 `todo.csv`。
2. 选择唯一 `status=next` 的任务。
3. 读取任务 `evidence` 指向的文档。
4. 读取相关代码和测试，确认现有模式。
5. 涉及代码时先补测试或测试用例，再改实现。
6. 运行最小相关测试。
7. 运行默认 CTest。
8. 运行 readiness gate 或专项 gate。
9. 涉及代码、测试、构建脚本的节点，验证通过后、commit 前执行 xhigh 只读 review。
10. xhigh 只给审查结论；Critical / Important 必须由主流程 agent 修复或写明技术反驳理由。
11. 修复后重新运行受影响验证。
12. 更新实现记录、build report、追溯矩阵、缺口文档、`46` 和 `todo.csv`。
13. 节点完成后 commit。
14. commit 后立即打 annotated tag。
15. push `main` 和 tags。

如果测试或 gate 失败，不继续堆新功能。先修失败或把阻塞原因写入文档。

## xhigh 只读 Review 规范

涉及代码、测试、构建脚本的节点必须在本地验证通过后、commit 前执行 xhigh 只读 review。

纯文档、todo、证据整理、路线治理节点不强制 xhigh review，但可以按需执行。

xhigh agent 的权限边界：

- 只能读取 diff、相关文档、测试输出和验证报告。
- 只能输出审查意见和提交判断。
- 不能修改文件。
- 不能 apply patch。
- 不能运行会写回文件的格式化或代码生成。
- 不能 commit、tag、push。

xhigh review 输出必须包含：

- `Critical`：必须修。
- `Important`：必须修，或由主流程 agent 写明技术反驳理由。
- `Minor`：可以记录到后续 todo 或缺口文档。
- `Verdict`：`block` 或 `allow_commit`。

子代理生命周期：

- xhigh 或其他子代理完成本轮任务并返回结论后，主流程 agent 必须及时关闭该子代理。
- 关闭范围只限已经完成且不再需要的子代理，目的是减少代理负担。
- 这不是“清空代理池”，也不关闭仍在执行有效任务的代理。

## Git 节点时间线规范

本仓库必须把工作拆成可回退的时间线节点。

### 什么时候必须 commit

以下情况完成后必须 commit：

- 路线、ADR、roadmap、todo 或 agent 执行规则完成一次稳定更新。
- RebarSmart 证据被整理成可开发算法契约。
- Detail 包字段、Reader / Writer / Exporter 契约完成一个闭合节点。
- 一个 `TODO-* / M*-*` 实现切片完成并通过验证。
- 发现路线偏移并完成纠偏文档。
- 修复测试、gate、构建或仓库边界问题。

不要把多个无关节点混在一个 commit 里。

### commit 前检查

每次 commit 前执行：

```powershell
git status --short
git diff --stat
git diff --check
```

涉及代码实现时，至少执行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

涉及正式 Phase1 / route gate 时，执行：

```powershell
py .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict
```

涉及 `domain/rebar` 边界时，执行：

```powershell
rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
```

### commit message 格式

使用这个结构：

```text
<type>(<scope>): <short summary>

Node: TODO-089 / M0-Pivot-001
Evidence: ADR-20260611-step-only-rebarsmart-detail
Verify: readiness gate pass; git diff --check pass
Notes: 本节点只做路线治理，不迁入 RebarSmart 二进制
```

常用 `type`：

- `docs`：文档、证据、方案、追溯矩阵。
- `feat`：新增能力。
- `test`：测试补充。
- `fix`：修复实现或文档错误。
- `chore`：仓库、脚本、构建、忽略规则。

### tag 规则

每个稳定 commit 后必须打 annotated tag。

命名建议：

```text
docs/rebarsmart-detail-pivot-20260611
m1-detail-001/detail-package-model
m2-rebarsmart-001/generate-rebar-data
fix/readiness-gate-YYYYMMDD
```

tag message 必须写清：

```text
Node: TODO-089 / M0-Pivot-001
What: STEP-only + RebarSmart evidence + Detail compatibility route pivot
Evidence: ADR / roadmap / todo / goal docs
Verify: readiness gate pass; git diff --check pass
Risk: RebarSmart 只能作为证据源，不能进入运行时
```

推送时同时推送 commit 和 tag：

```powershell
git push origin main
git push origin --tags
```

### 跑偏回退方式

先看时间线：

```powershell
git log --oneline --decorate --graph --all -20
git tag --sort=creatordate
```

不要直接 `reset --hard`。

优先从 tag 拉出恢复分支：

```powershell
git switch -c recovery/<tag-name> <tag-name>
```

确认恢复点正确后，再决定 cherry-pick、merge 或新建纠偏节点。

## 文档同步要求

每个实现节点至少同步：

- `00_总览.md`
- `11_需求证据追溯矩阵.md`
- `34_Phase1ReadinessGate实际运行记录.md`
- `46_CSE_v2Goal执行目标与Todo说明.md`
- `99_缺口和待确认项.md`
- `todo.csv`
- 对应实现记录
- 对应 `docs/phase1/app_build_reports/*.md`

`46_CSE_v2Goal执行目标与Todo说明.md` 必须始终指向下一轮 `status=next` 的短期 Goal。

## RebarSmart / VisualTS 确认规则

钢筋生成逻辑优先级：

1. RebarSmart3DE 的类名、导出符号、INI 默认值、参数结构和反编译证据。
2. 旧图石运行结果、STP / Detail / 下料表 / 截图等结果证据。
3. VisualTS / IDA 只在 RebarSmart 证据不足或需要对齐旧 Detail / 工程图时补充。
4. 父目录代码只能作为工程写法参考，不能关闭业务证据缺口。

工程图 / Detail / AutoCAD 插件兼容优先级：

1. 旧图石真实导出的 `Detail.xml + DetailNN.stl` 包。
2. 旧 AutoCAD 插件实际导入结果。
3. VisualTS / FDrawing 静态证据和 IDA MCP。
4. 当前 DetailWriter / DrawingModel 测试。

如果 IDA MCP、旧图石或 AutoCAD 插件不可用，必须把阻塞原因写入 `99_缺口和待确认项.md`，不能假装已经确认。
