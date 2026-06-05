# STEP 选择 ID 稳定性 Spike 计划

## 目标

本文件定义推进 `GAP-DEV-002` 的验证计划。

本 Spike 只解决几何拓扑选择的稳定恢复。

它不定义钢筋业务编号、旧图石字段语义、钢筋组关系或工程图 ID 规则。

问题很直接：

```text
用户选了一个面做面配筋。
保存工程后重新打开。
新程序必须还能找到同一个面。
```

如果 STEP 导入后 face / edge 顺序不稳定，钢筋参数就会失效。

## 为什么这是 P0

以下功能都依赖稳定选择 ID：

- 面配筋：保存 `sourceFaceId`。
- 线配筋：保存 `sourceLineId / edgeIds`。
- 面周边：保存面边界。
- 参考面 / 参考线：保存来源选择。
- 剖切面 / 投影面：可能引用结构面或参考对象。
- 工程图和下料：需要钢筋来源可追溯。

因此不能只用 OCCT 导入顺序当长期 ID。

## Spike 输入

首批输入：

- 一份简单 STEP：几何面少，便于人工核对。
- 一份复杂 STEP：来自 6 个 SFL 样本引用的真实结构模型。
- 若 STEP 原文件缺失，先用旧图石导出或用户提供对应 `.stp/.step`。

说明：

- 这里的复杂 STEP 可以来自旧图石导出或用户提供的对应结构模型。
- SFL 在本 Spike 中提供样本关联和业务证据，不作为新格式几何主载体。

每个 STEP 至少跑 3 轮：

```text
同一路径导入 3 次
换路径导入 1 次
重启程序后导入 1 次
```

## 需要记录的数据

每个 face 记录：

```json
{
  "sourceIndex": 1,
  "area": 12.345678,
  "center": [1.0, 2.0, 3.0],
  "normal": [0.0, 0.0, 1.0],
  "edgeCount": 4,
  "edgeLengthSet": [1.2, 1.2, 3.4, 3.4],
  "bbox": {
    "min": [0, 0, 0],
    "max": [1, 1, 0]
  }
}
```

每个 edge 记录：

```json
{
  "sourceIndex": 1,
  "length": 3.45,
  "start": [0, 0, 0],
  "end": [1, 0, 0],
  "curveType": "line",
  "adjacentFaceFingerprint": []
}
```

## 候选 fingerprint

### Face fingerprint

建议组合：

- 面积。
- 重心。
- 法向。
- 包围盒。
- 边数量。
- 边长集合。
- 曲面类型：plane / cylinder / cone / sphere / bspline。

初步 key：

```text
faceKey = hash(
  surfaceType,
  rounded(area),
  rounded(center),
  rounded(normal),
  edgeCount,
  rounded(sorted(edgeLengths))
)
```

### Edge fingerprint

建议组合：

- 长度。
- 起点和终点。
- 曲线类型。
- 相邻面 faceKey。
- 中点。

初步 key：

```text
edgeKey = hash(
  curveType,
  rounded(length),
  rounded(unordered(start, end)),
  rounded(midPoint),
  adjacentFaceKeys
)
```

## 容差建议

第一轮容差：

```text
坐标：1e-6 m
长度：1e-6 m
面积：1e-8 m2
法向夹角：1e-6 rad
```

如果真实 STEP 精度不稳定，允许按模型尺度放宽，但必须记录原因。

## 验证流程

```text
1. 导入 STEP。
2. 遍历 TopoDS_Shape，提取 face / edge 描述。
3. 生成 fingerprint。
4. 保存为 run_001.json。
5. 重复导入，保存 run_002.json / run_003.json。
6. 对比 sourceIndex 是否稳定。
7. 对比 fingerprint 是否一一匹配。
8. 找出重复 fingerprint 和无法匹配项。
```

## 通过标准

简单 STEP：

- face 匹配率 100%。
- edge 匹配率 100%。
- 无重复 fingerprint。

复杂 STEP：

- face 匹配率至少 99.9%。
- edge 匹配率至少 99.5%。
- 对重复 fingerprint 必须能用父子关系或相邻面二次消歧。

若达不到：

- 不能进入正式钢筋保存开发。
- 只能进入 UI 外壳和临时选择。
- 必须升级为自建拓扑命名服务。

## 失败分级

### A 类：导入顺序稳定，fingerprint 也稳定

处理：

- 首期可保存 `sourceIndex + fingerprint`。
- 恢复时先按 fingerprint 匹配，sourceIndex 只作加速。

### B 类：导入顺序不稳定，但 fingerprint 稳定

处理：

- 保存 fingerprint。
- 恢复时全量匹配。
- sourceIndex 不能用于长期定位。

### C 类：fingerprint 有重复，但能靠邻接关系消歧

处理：

- 增加相邻 face / edge 上下文。
- 保存二级 fingerprint。

### D 类：fingerprint 无法稳定区分

处理：

- 首期不允许对该类几何做可保存配筋。
- UI 上提示“该选择无法稳定保存”。
- 后续考虑用户命名面、参考对象锚点或导入时写入扩展属性。

## 输出物

Spike 完成后必须产出：

```text
step_selection_run_001.json
step_selection_run_002.json
step_selection_diff.md
```

实际运行样本、5 轮运行矩阵、run JSON schema、diff 模板和 readiness 接入口见：

```text
31_STEP选择ID实际运行记录模板与样本清单.md
```

文档要更新：

- `18_新设计文件格式替代SFL策略.md`：确认 OCCT / STEP 几何引用和 `fingerprint` 字段。
- `09_钢筋领域模型草案.md`：确认 `sourceSelection` 保存策略。
- `99_缺口和待确认项.md`：只有真实 run JSON 和 diff 通过后，才能关闭或降级 `GAP-DEV-002`。

## 与旧图石 1:1 的关系

旧图石用 ACIS / 私有对象保存，可能不需要我们这种 fingerprint。

但新系统不使用 ACIS 商业库，也不把 `.sfl` 当一期主格式。
所以必须用 OCCT + 自建稳定 ID 解决保存恢复。

这不是优化项，是功能复刻的基础设施。
