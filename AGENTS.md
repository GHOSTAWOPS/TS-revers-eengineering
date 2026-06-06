# Agent 工作规范

## 核心路线

本项目目标是《图石钢筋 1 比 1 复刻》。

硬约束：

- 不是用 OCCT 直接重写一个“差不多”的钢筋软件。
- Qt6 只替代 MFC / Codejock 的界面和命令入口。
- OCCT AIS 只替代 HOOPS 的显示、选择、高亮、旋转缩放。
- OCCT 几何 API 只替代 ACIS 的 EDGE / FACE / 曲线 / 求交 / 偏移 / 裁剪 / 距离等能力。
- `LegacyGeometryAdapter` 负责把 OCCT 包装成旧 VisualTS 熟悉的几何语义。
- 钢筋创建、编辑、统计、出图逻辑必须按 VisualTS / IDA / SFL / Detail / 旧图石运行证据 1:1 复刻。
- 父目录旧开发代码只能作为 OCCT 工程写法参考，不能当作旧图石业务真相迁移。

禁止：

- 把父目录 `src/rebar/*`、`RebarCreationCommandService`、`EdgeToRebarFactory`、`FaceRebarGenerator`、`PolylineRebarGenerator` 当主线业务代码迁入。
- 在 `app/src/domain/rebar` 中引入 `TopoDS_`、`AIS_`、`BRep*`、`TopAbs_` 等 OCCT / AIS 细节。
- 不确定旧逻辑时直接写死结论。

## 默认执行闭环

每次开发只推进一个清晰节点：

1. 先读 `todo.csv`，选择 `status=next` 的任务。
2. 读取任务 evidence 指向的文档。
3. 读取相关代码和测试，确认现有模式。
4. 先补测试或测试用例，再改实现。
5. 运行最小相关测试。
6. 运行默认 CTest。
7. 运行 readiness gate 或专项 gate。
8. 涉及代码、测试、构建脚本的节点，验证通过后、commit 前执行 xhigh 只读 review。
9. xhigh 只给审查结论；Critical / Important 必须由主流程 agent 修复或写明技术反驳理由。
10. 修复后重新运行受影响验证。
11. 更新实现记录、build report、追溯矩阵、缺口文档和 `todo.csv`。
12. 节点完成后 commit。
13. commit 后立即打 annotated tag。

如果测试或 gate 失败，不继续堆新功能。先修失败或把阻塞原因写入文档。

## xhigh 只读 Review 规范

涉及代码、测试、构建脚本的节点必须在本地验证通过后、commit 前执行 xhigh 只读 review。

纯文档、todo、证据整理节点不强制 xhigh review，但可以按需执行。

xhigh agent 的权限边界：

- 只能读取 diff、相关文档、测试输出和验证报告。
- 只能输出审查意见和提交判断。
- 不能修改文件。
- 不能 apply patch。
- 不能运行会写回文件的格式化或代码生成。
- 不能 commit、tag、push。

xhigh review 输入必须包含：

- 当前任务目标。
- 相关 goal / todo / evidence 文档。
- base/head diff。
- 本地验证命令和输出。
- 本轮禁止事项，尤其是 OCCT 不得泄漏进 `domain/rebar`。

xhigh review 输出必须包含：

- `Critical`：必须修。
- `Important`：必须修，或由主流程 agent 写明技术反驳理由。
- `Minor`：可以记录到后续 todo 或缺口文档。
- `Verdict`：`block` 或 `allow_commit`。

子代理生命周期：

- xhigh 或其他子代理完成本轮任务并返回结论后，主流程 agent 必须及时调用 `close_agent` 关闭该子代理。
- 关闭范围只限已经完成且不再需要的子代理，目的是减少代理负担；这不是“清空代理池”，也不关闭仍在执行有效任务的代理。
- 如确需继续复用同一个子代理，必须在当前轮说明原因；复用结束后仍要关闭。

修复责任：

- xhigh 只负责 review。
- 主流程 agent 负责修改、再验证、文档更新、commit、tag、push。
- 如果 xhigh agent 不可用，必须在实现记录或 build report 中写明阻塞原因，不能伪造 review 结果。

## Git 节点时间线规范

本仓库必须把工作拆成可回退的时间线节点。

### 什么时候必须 commit

以下情况完成后必须 commit：

- 逆向证据闭合：例如 IDA 函数、字段、常量、调用链确认。
- 运行证据闭合：例如旧图石操作、导出文件、hash、截图、行为确认完成。
- 一个 `TODO-* / M*-App-*` 实现切片完成并通过验证。
- 技术路线、架构边界、命令契约、领域模型、Detail 映射等关键文档完成一次稳定更新。
- 发现路线偏移并完成纠偏文档。
- 修复测试、gate、构建或仓库边界问题。

不要把多个无关节点混在一个 commit 里。

### commit 前检查

每次 commit 前执行：

```powershell
git status --short
git diff --stat
```

涉及代码实现时，至少执行：

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

涉及代码、测试、构建脚本时，还必须在上述验证通过后执行 xhigh 只读 review。
Critical / Important 未处理或未写明技术反驳理由时，不允许 commit。

涉及正式 Phase1 放行时，执行：

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

Node: TODO-012 / M1-App-012
Evidence: E-DEV-036, IDA sub_14059B980
Verify: CTest 8/8 pass; readiness 78/78 pass
Notes: 本节点只做 LegacyGeometryAdapter，不进入钢筋业务层
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
bootstrap/formal-app-m1-app-011
m1-app-012/p3d-edge-group-min-distance
evidence/ida-sub-14059b980
docs/goal-todo-012
fix/readiness-gate-YYYYMMDD
```

tag message 必须写清：

```text
Node: TODO-012 / M1-App-012
What: LegacyGeometryAdapter P3D group minimum distance summary
Evidence: sub_14059B980 / api_entity_point_distance
Verify: CTest 8/8 pass; readiness 78/78 pass
Risk: 仍未等价真实 ACIS topology mutation
```

打 tag 命令示例：

```powershell
git tag -a "m1-app-012/p3d-edge-group-min-distance" -m "Node: TODO-012 / M1-App-012; Verify: CTest + readiness pass"
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
- `99_缺口和待确认项.md`
- `todo.csv`
- 对应 `NN_M1-App-XXX...实现记录.md`
- 对应 `docs/phase1/app_build_reports/*.md`

`46_CSE_v2Goal执行目标与Todo说明.md` 必须始终指向下一轮 `status=next` 的短期 Goal。

## IDA / 旧图石确认规则

旧业务逻辑不确定时，优先级如下：

1. IDA MCP 查询旧 VisualTS 函数、调用链、常量和字段。
2. 旧图石软件运行确认，记录操作步骤、截图、输出文件、hash。
3. SFL / Detail / STP 样本交叉验证。
4. 父目录代码只能作为工程写法参考，不能关闭旧业务证据缺口。

如果 IDA MCP 不可用，必须把阻塞原因写入 `99_缺口和待确认项.md`。
