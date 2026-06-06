# M1-App-016 LegacyGeometryAdapter Section Spike 实现记录

## 结论

本轮完成 `TODO-016 / M1-App-016`。

新增 `LegacyFacePlaneSectionPreview`：

```text
facePlaneSectionPreview
```

它对齐旧 VisualTS / ACIS 语义边界：

```text
FACE
  -> legacy plane
  -> section / intersection preview
  -> sectionEdges / samplePoints / length / bbox / diagnostic
```

本轮实现的是 section / 剖切 preview summary / 能力 spike，不是钢筋业务，
不是工程图业务，也不是 ACIS topology mutation 的完整等价实现。

## 新增能力

新增 DTO：

```text
LegacyPlane
LegacySectionEdge
LegacyFacePlaneSectionPreview
```

新增 adapter API：

```text
facePlaneSectionPreview(faceRef, plane, sampleCount)
```

当前语义：

```text
LegacySelectionRef(face)
  -> validate face ref / plane / sample count
  -> resolve OCCT face inside adapter
  -> run BRepAlgoAPI_Section(face, plane)
  -> return section edge samplePoints / length / bbox / hitCount
  -> if OCCT returns no edge for coplanar face-plane, use boundary-edge preview fallback
```

同时补强真实样本中已有 `edgeFaceIntersections` 的空交集覆盖：

```text
non-touching edge-face pair
  -> ok result
  -> overlap = false
  -> points = empty
```

## 验证覆盖

真实样本：

```text
C:/Users/ghost/Desktop/reverse_engineering/【03】图石软件/123.stp
sha256 = 6a3c4f2044c2cc1f1123f0f58c61b2c869fbad3110a3585fdab49e69daf52a2a
```

已覆盖：

- face sample point + face normal 的 plane section 返回 sectionable summary。
- far plane 返回稳定 empty preview，不崩溃。
- invalid plane 稳定拒绝。
- wrong type / missing ref 返回稳定 diagnostic。
- boundary edge 与所属 face 返回接触/重叠代表点。
- non-touching edge-face pair 返回稳定 empty intersection set。

## 测试结果

RED：

```text
legacy_geometry_adapter_tests.cpp
error C2039: LegacyPlane is not a member of tsrebar
error C2039: facePlaneSectionPreview is not a member of OccLegacyGeometryAdapter
```

GREEN 窄测试：

```text
cmd.exe /c 'call "D:\Visual Studio 2026\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul && cmake --build app\build --target legacy_geometry_adapter_tests && app\build\legacy_geometry_adapter_tests.exe "C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\123.stp"'
result = pass
```

默认 CTest：

```text
8 / 8 passed
Total Test time = 157.10 sec
```

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

`domain/rebar` 边界扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" ".\app\src\domain\rebar"
result = no matches
```

xhigh 只读 review：

```text
Critical = none
Important = none
Minor =
  - BRepAlgoAPI_Section 使用 PerformNow=true 后再调用 Approximation(true)，当前不阻塞。
  - edgeSamplesOnPlane 采样点判断 coplanar fallback，低采样数下理论上可能误收曲边；
    当前 spike / far plane 测试不受影响。
Verdict = allow_commit
```

xhigh 子代理已在 review 结束后关闭。

## IDA 状态

本轮检查 IDA MCP：

```text
idb_list
sessions = []
count = 0
```

因此本轮没有新增 IDA 闭环证据。剖切 / 割切面对应的旧 UI 和旧业务语义
仍需要后续 IDA 或旧图石运行确认闭合。

## 修改范围

```text
app/src/geometry/legacy/LegacyGeometry.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h
app/src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
app/tests/integration/legacy_geometry_adapter_tests.cpp
```

## 边界

本轮没有实现钢筋生成。

本轮没有迁移父目录钢筋业务主链。

本轮没有实现剖切面 UI、工程图割切面、剖面圈筋业务或 Detail writer。

`facePlaneSectionPreview` 是 face ref + legacy plane 到 section preview 的
summary，不等价于 OCCT shape mutation，也不等价于旧 ACIS section API 的完整行为。

OCCT 细节仍限制在：

```text
geometry/occ/legacy_adapter
```

`domain/rebar` 边界扫描通过。

## 下一步建议

下一阶段建议做：

```text
TODO-017：LegacyGeometryAdapter sweep / 扫掠能力边界
```

原因：

```text
旧钢筋最终需要显示、导出和工程图几何表达。section / offset / spline / wire chain
之后，下一块底层能力是确认 OCCT sweep / pipe 能否作为钢筋实体显示和输出的
几何基础；仍然只做 adapter 能力边界，不进入钢筋业务创建。
```

如果 IDA 数据库恢复绑定，也可以先补：

```text
TODO-020：IDA MCP 旧线筋 / 弧筋链证据。
```
