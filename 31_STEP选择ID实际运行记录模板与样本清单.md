# STEP 选择 ID 实际运行记录模板与样本清单

## 目标

本文件把 `19_STEP选择ID稳定性Spike计划.md` 从“计划”推进到“可执行记录模板”。

它回答：

```text
用哪些 STEP 样本跑？
每个样本跑几轮？
每轮记录哪些字段？
diff 怎么写？
什么时候允许进入正式保存 / 打开开发？
什么时候必须进入 unresolvedGeometry？
```

本文件新增开发证据：

```text
E-DEV-009
  -> STEP 选择 ID 实际运行记录模板与样本清单
  -> 来源：31_STEP选择ID实际运行记录模板与样本清单.md
```

注意：

- `E-DEV-009` 只证明 STEP 选择 ID 运行方案、样本清单、记录格式和判定门禁已经明确。
- 它不证明具体样本已经通过；当前 seed 样本通过情况另由 `E-DEV-017` 证明。
- 它不能单独关闭 `GAP-DEV-002`。
- 只有对应样本的真实 `step_selection_run_*.json` 和 `step_selection_diff.md` 存在并通过门禁后，才能把该样本作为 binding 证据。

## 控制合同

| 字段 | 约束 |
|---|---|
| Primary Setpoint | 保存工程后重新打开，旧业务对象绑定的 STEP face / edge 必须能稳定恢复。 |
| Acceptance | 本文件给出样本清单、运行矩阵、JSON 记录格式、diff 模板、通过/失败门禁和 readiness 接入点。 |
| Guardrail Metrics | 不把 OCCT 遍历顺序当稳定 ID；不把 SFL 当 Phase 1 几何主载体；不在匹配失败时静默改绑。 |
| Boundary | 只定义 STEP 选择 ID 运行记录；不定义钢筋业务生成规则，也不关闭旧图石运行缺口。 |
| Rollback Trigger | 若 diff 发现 fingerprint 不稳定或有歧义，相关钢筋对象必须进入 `unresolvedGeometry` 或人工修复状态。 |

## 白话解释

旧图石用 ACIS / 私有对象保存，可能天然知道“这是哪个面”。

新系统不用 ACIS 商业库，也不把 `.sfl` 当首期主工程格式。

所以我们必须给 STEP 里的面、边、体做一套可复验的身份识别。

这件事不是优化项，而是保存/打开能不能靠谱的底座：

```text
旧图石业务对象
  -> legacyObject.raw

OCCT / STEP 几何对象
  -> geometryRef + topology fingerprint

二者绑定关系
  -> binding

如果 STEP 重新导入后找不到同一个面
  -> unresolvedGeometry
  -> 不能静默挂到另一个相似面
```

## 样本清单

### STEP 候选样本

已在本机确认存在的 STEP / STP 候选：

```text
C:\Users\ghost\Desktop\reverse_engineering\test.stp
C:\Users\ghost\Desktop\reverse_engineering\tests\golden\complex_curve_rebar\CC-001-bspline-edge-to-rebar\input\input.step
C:\Users\ghost\Desktop\reverse_engineering\tests\golden\complex_curve_rebar\CC-002-bezier-edge-to-rebar\input\input.step
C:\Users\ghost\Desktop\reverse_engineering\tests\golden\complex_curve_rebar\CC-003-real-length-vs-polyline-length\input\input.step
C:\Users\ghost\Desktop\reverse_engineering\tests\golden\complex_curve_rebar\CC-004-hostref-and-source-curve-preservation\input\input.step
C:\Users\ghost\Desktop\reverse_engineering\tests\golden\complex_curve_rebar\CC-005-sampling-policy\input\input.step
```

用途分层：

- `test.stp`：首个简单样本，适合人工核对 face / edge 数量和 fingerprint。
- `CC-001`：B-spline 边界曲线样本，验证曲线类型和边长计算。
- `CC-002`：Bezier 边界曲线样本，验证非直线 edge 的稳定性。
- `CC-003`：真实长度与折线长度差异样本，验证不能用显示折线近似当几何真值。
- `CC-004`：hostRef / source curve 保留样本，验证来源引用能进入 `geometryRef`。
- `CC-005`：采样策略样本，验证 curve sampling 不能污染 topology fingerprint。

### 旧图石导出钢筋 STP 样本

用户运行确认旧图石 `开始 / 文件 / 输出钢筋` 可以导出钢筋 `.stp`。

这类 STP 不只是结构 STEP 输入，还可以作为钢筋几何 golden：

```text
旧 SFL
  -> 旧图石输出钢筋 .stp
  -> OCCT 导入统计
  -> 新系统生成钢筋后导出 / 显示 / 统计对照
```

已登记样本：

- `tushi_rebar_123_stp`
  - 源 SFL：`消力池下游侧带齿槽底板结构图石钢筋模型.sfl`。
  - 导出文件：`C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp`。
  - 文件大小：87,746,388 bytes。
  - SHA256：`6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a`。
  - STEP 导出器：`Spatial InterOp 3D`。
  - OCCT 导入统计：1 root、1 assembly、754 references、754 solids、3016 faces、9048 raw edges、18096 raw vertices、length unit `METRE`。
  - STEP selection gate：5 轮通过，copy-path run 通过，decision `pass`。
  - 稳定引用：faces 3016、edges 4524、vertices 3016，min match rate 均为 1.000000，duplicate 0。
  - 报告：`docs/phase1/step_selection/tushi_rebar_123_stp/step_selection_diff.md`。

待补样本：

- 复杂样本钢筋 STP：建议来自 4#塔、14#坝段、尾水调压室或边墩样本之一。

记录要求：

- 导出窗口截图。
- 源 SFL 路径。
- 导出 STP 路径、文件大小、SHA256。
- OCCT 导入 roots / solids / faces / edges / vertices / units。
- 判断 STP 表达的是钢筋扫掠实体、中心线，还是混合表达。

注意：

- 旧图石导出的钢筋 STP 可以作为几何对照，但不能替代 IDA 对钢筋业务规则的复刻。
- 旧 STP 只能证明“结果长什么样”，不能单独证明“算法为什么这么生成”。

### SFL 关联样本

当前 6 个 `.sfl` 样本用于业务关联和复杂工程语义参考，不作为 Phase 1 几何主载体：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\14#坝段表孔闸墩及流道2877.00m高程以上钢筋图.sfl
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\4#塔图石模型7.16最终.sfl
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\边墩1107（底板+边墩配筋+接头坝段)替换模型后-未合并钢筋-修改廊道位置-替换后.sfl
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\尾水调压室闸墩钢筋模型（2705m以上）_校审后修改.sfl
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\尾水调压室闸墩结构图石钢筋模型_替换前.sfl
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\消力池下游侧带齿槽底板结构图石钢筋模型.sfl
```

本运行模板里的 SFL 用法：

- 给 fixture 选择真实工程语境。
- 关联 `legacyObject.raw`、`evidence`、`unresolved_fields`。
- 校准钢筋对象和 Detail 字段关系。

禁止用法：

- 不把 `.sfl` 当 Phase 1 主保存格式。
- 不直接从 `.sfl` 解析 ACIS BinaryFile 作为首期几何输入。
- 不用 SFL 里的私有指针替代 `geometryRef`。

## 运行矩阵

每个 STEP 样本至少跑 5 轮：

```text
R1 same-path-001      原路径导入第 1 次
R2 same-path-002      原路径导入第 2 次
R3 same-path-003      原路径导入第 3 次
R4 copied-path-001    复制到临时目录后导入 1 次
R5 restart-001        重启程序后导入 1 次
```

推荐输出路径：

```text
docs/phase1/step_selection/
  test_stp/
    step_selection_run_r1.json
    step_selection_run_r2.json
    step_selection_run_r3.json
    step_selection_run_r4.json
    step_selection_run_r5.json
    step_selection_diff.md
  cc_001_bspline_edge/
    step_selection_run_r1.json
    step_selection_diff.md
```

Phase 1 最低要求：

- `test.stp` 必须跑满 5 轮。
- 至少选 1 个 `CC-00x` 曲线样本跑满 5 轮。
- Fixture B/C 对应的 STEP 样本必须产出 diff。

建议完整要求：

- 上述 6 个 STEP/STP 全部跑满 5 轮。
- 每个样本都保留 run JSON 和 diff。

## Run JSON 结构

每轮导入输出一个 JSON。

建议 schema：

```json
{
  "schemaVersion": "step-selection-run/v1",
  "runId": "test_stp-r1",
  "createdAt": "YYYY-MM-DDTHH:mm:ss+08:00",
  "tool": {
    "app": "tsrebar-phase1-spike",
    "commit": "TBD",
    "occtVersion": "TBD",
    "qtVersion": "TBD"
  },
  "source": {
    "sourceStepId": "step-main",
    "path": "C:/Users/ghost/Desktop/reverse_engineering/test.stp",
    "copiedFrom": null,
    "sha256": "TBD",
    "fileSize": 0
  },
  "importOptions": {
    "unit": "TBD",
    "sewing": false,
    "healing": false,
    "tolerance": {
      "length": 0.000001,
      "area": 0.00000001,
      "angleRad": 0.000001
    }
  },
  "summary": {
    "solidCount": 0,
    "shellCount": 0,
    "faceCount": 0,
    "edgeCount": 0,
    "vertexCount": 0
  },
  "faces": [
    {
      "occtTransientIndex": 1,
      "topologyPath": "solid[0]/shell[0]/face[0]",
      "fingerprint": "face-sha256:TBD",
      "surfaceType": "plane",
      "area": 0.0,
      "center": [0.0, 0.0, 0.0],
      "normal": [0.0, 0.0, 1.0],
      "bbox": {
        "min": [0.0, 0.0, 0.0],
        "max": [0.0, 0.0, 0.0]
      },
      "edgeCount": 0,
      "edgeLengthSet": [],
      "adjacentFaceFingerprints": [],
      "warnings": []
    }
  ],
  "edges": [
    {
      "occtTransientIndex": 1,
      "topologyPath": "solid[0]/shell[0]/edge[0]",
      "fingerprint": "edge-sha256:TBD",
      "curveType": "line",
      "length": 0.0,
      "start": [0.0, 0.0, 0.0],
      "end": [0.0, 0.0, 0.0],
      "midPoint": [0.0, 0.0, 0.0],
      "adjacentFaceFingerprints": [],
      "warnings": []
    }
  ],
  "diagnostics": {
    "duplicateFaceFingerprints": [],
    "duplicateEdgeFingerprints": [],
    "ambiguousTopologyPaths": [],
    "importWarnings": []
  }
}
```

硬规则：

- `occtTransientIndex` 只能记录用于观察，不能作为长期保存 ID。
- `fingerprint` 必须可由几何特征重新计算。
- `topologyPath` 只能作为辅助，不作为唯一稳定 ID。
- 任何 `warnings` 都必须进入 diff。

## Diff 模板

每个样本产出一个 `step_selection_diff.md`。

模板：

```markdown
# STEP Selection Diff: <sample-id>

## Summary

- Date:
- Operator:
- Tool commit:
- OCCT version:
- Source STEP:
- Source hash:
- Runs:
- Result: pass / fail / blocked
- Decision:

## Run Matrix

| Run | Input path | Restarted | Source hash | Face count | Edge count | Notes |
|---|---|---|---|---|---|---|
| R1 |  | no |  |  |  |  |
| R2 |  | no |  |  |  |  |
| R3 |  | no |  |  |  |  |
| R4 |  | no |  |  |  |  |
| R5 |  | yes |  |  |  |  |

## Match Result

- Face match rate:
- Edge match rate:
- Duplicate face fingerprints:
- Duplicate edge fingerprints:
- Unmatched faces:
- Unmatched edges:
- Ambiguous matches:

## Drift

| Kind | Stable fingerprint | Transient index changed | Count | Decision |
|---|---|---|---|---|
| face |  |  |  |  |
| edge |  |  |  |  |

## Binding Impact

- Affected geometryRef:
- Affected binding:
- Required state:
- Can auto-repair:
- Human confirmation required:

## Decision

- Can use fingerprint for save/open:
- Need second-level adjacency disambiguation:
- Must enter unresolvedGeometry:
- Blocks M1-Formal:

## Evidence

- Requirement:
- Evidence:
- GAP:
- Related fixture:
```

## 通过门禁

简单 STEP：

- face 匹配率必须 100%。
- edge 匹配率必须 100%。
- 重复 fingerprint 必须为 0。
- R1/R2/R3/R4/R5 的对象数量必须一致。

曲线 STEP：

- 曲线 edge 必须记录 `curveType`。
- B-spline / Bezier 不得退化成折线近似保存。
- edge 长度必须来自几何曲线真实长度，不来自显示采样折线。

复杂 STEP：

- face 匹配率目标 99.9% 以上。
- edge 匹配率目标 99.5% 以上。
- 重复 fingerprint 必须有二级消歧策略。
- 无法消歧的对象必须进入 `unresolvedGeometry`。

正式 Phase 1 保存/打开门禁：

- 不能只因简单 STEP 通过，就开放所有复杂结构可保存配筋。
- 每个可保存 `geometryRef` 必须能追到本 diff。
- `Project.Open` 发现 topology mismatch 时，不能自动改绑到相似面。

## 失败分级

### F1：导入失败

表现：

- STEP 文件无法导入。
- shape 为空。
- face / edge 数量为 0。

处理：

- 样本标为 blocked。
- 不能关闭 `GAP-DEV-002`。
- 记录 OCCT 版本、导入选项和错误信息。

### F2：导入数量漂移

表现：

- 同一文件多轮导入 face / edge 数量不同。

处理：

- 禁止对该样本启用正式保存配筋。
- 检查导入 healing/sewing 选项是否不一致。
- 若选项一致仍漂移，升级为拓扑命名专项。

### F3：fingerprint 重复但可消歧

表现：

- 多个 face / edge fingerprint 相同。
- 邻接关系或父子 topology 上下文可区分。

处理：

- 增加二级 fingerprint。
- diff 记录消歧字段。
- 相关 validator 增加 duplicate 检查。

### F4：fingerprint 无法消歧

表现：

- 多个对象几何上等价，邻接关系也无法区分。

处理：

- `geometryRef.state = "ambiguous"`。
- `binding.state = "unresolvedGeometry"`。
- UI 打开后要求人工选择或重新绑定。
- 不允许静默改绑。

### F5：运行通过但没有记录

表现：

- 口头说跑过。
- 没有 run JSON、diff、hash、版本、命令记录。

处理：

- 一律视为未跑。
- 不允许推进 `GAP-DEV-002`。

## 与新设计文件格式的关系

`geometry/topology_refs.json` 至少需要承载：

```json
{
  "geometryRefs": [
    {
      "geometryRefId": "geom-face-001",
      "sourceStepId": "step-main",
      "kind": "face",
      "fingerprint": "face-sha256:TBD",
      "occtTransientIndexHint": 1,
      "topologyPathHint": "solid[0]/shell[0]/face[0]",
      "state": "resolved",
      "evidence": ["E-DEV-009", "GAP-DEV-002"]
    }
  ]
}
```

`rebar/groups.json` 或 `rebar/segments.json` 的 binding 必须引用它：

```json
{
  "binding": {
    "state": "resolved",
    "items": [
      {
        "legacyPath": "legacyObject.raw.steelbargroup[0]",
        "geometryPath": "geometryRefs[geom-face-001]",
        "source": "step-fingerprint",
        "evidence": ["E-DEV-009"]
      }
    ]
  }
}
```

如果恢复失败：

```json
{
  "binding": {
    "state": "unresolvedGeometry",
    "items": [
      {
        "legacyPath": "legacyObject.raw.steelbargroup[0]",
        "geometryPath": null,
        "lastKnownFingerprint": "face-sha256:TBD",
        "reason": "fingerprint not found after STEP re-import",
        "evidence": ["GAP-DEV-002", "E-DEV-009"]
      }
    ]
  }
}
```

## 与 readiness 的关系

`docs/phase1/readiness_run_001.md` 里的 STEP Selection Gate 至少要填：

```text
sample:
run_r1:
run_r2:
run_r3:
run_r4:
run_r5:
diff:
face match rate:
edge match rate:
duplicates:
unresolvedGeometry cases:
decision:
```

判定：

- 没有真实 diff：`GAP-DEV-002` 保持 P0。
- 简单样本通过但复杂样本未跑：只能支持 M1-Prep 或有限 spike。
- Fixture B/C 绑定的 STEP 通过：可支持对应 fixture 的 Save/Open 开发。
- 任一正式可保存 binding 无 diff 证据：不能申请 M1-Formal。

## 关联追溯

Requirement：

- `REQ-PROJ-001`：新设计文件格式。
- `REQ-TECH-002`：Qt6 + OCCT 开发入口门禁。

Evidence：

- `E-DEV-009`：STEP 选择 ID 实际运行记录模板与样本清单。
- `E-DEV-017`：STEP selection gate、unittest 和首批真实 OCCT 多轮 diff reports。

GAP：

- `GAP-DEV-002`：STEP 选择 ID 稳定性。
- `GAP-DEV-001`：新设计文件格式实际 fixture / validator / binding 验证。
- `GAP-DEV-007`：开发前 readiness 实际打勾。

关联文档：

- `19_STEP选择ID稳定性Spike计划.md`
- `24_新设计文件格式Schema与Fixture草案.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `27_Qt6_OCCT开发入口门禁与首批工件清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`
- `30_Phase1工程初始化Runbook与Readiness记录模板.md`
- `99_缺口和待确认项.md`

## 当前结论

现在 STEP 选择 ID 已经从“要跑一个 spike”细化成“按哪些样本、跑几轮、输出什么、怎么判定”的执行模板，并已跑出第一批真实 OCCT diff。

当前真实运行结论：

- `test_stp`：5 轮同/复制路径通过，face/edge/vertex 引用稳定。
- `cc001_bspline_edge`、`cc002_bezier_edge`、`cc003_bspline_length`、`cc004_hostref_source_curve`、`cc005_sampling_policy`：均 5 轮通过。
- `step-shape-fingerprint/v1` 已接入 `visualts_step_probe --refs-json` 和 gate 比较逻辑，复杂 edge/vertex raw Explorer 重复噪声已被唯一 subshape map + fingerprint 消解。

所以当前状态仍是：

```text
GAP-DEV-002 = P1 residual / current seed samples passed
下一步 = 后续真实工程 STEP 样本进入时继续跑同一 gate
```
