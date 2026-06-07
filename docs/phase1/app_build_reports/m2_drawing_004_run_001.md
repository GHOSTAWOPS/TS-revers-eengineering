# M2-Drawing-004 Run 001

## Summary

```text
todoId = TODO-035
phase = M2-Drawing-004
decision = detail-complex-skeleton-p0-pass
```

本轮把 TODO-034 的静态复杂字段证据落到正式 app `DetailWriter`：

- 输出 `PartDetailDrawing` 复杂容器骨架。
- 补 `General-Info` 首批扩展字段。
- 补 `pointStb` 的 `StbGeo shapeType=C` 字段骨架。

## Evidence

```text
E-DETAIL-003 = old Detail01.stl complex field structure
E-IDA-027 = FDrawing.arx object / method static symbols
E-DEV-056 = TODO-034 static evidence package
E-DEV-057 = TODO-035 DetailWriter complex skeleton P0
```

## Changed Files

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
tools/phase1_readiness_gate/check_phase1_readiness.py
```

## Validation

Narrow test:

```text
detail_writer_tests
  -> pass
```

Default CTest:

```text
17 / 17 pass
```

Readiness gate:

```text
M1-Formal-Ready
84 / 84 pass
0 error
0 warning
```

Protected-layer leak scan:

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches
```

Diff check:

```text
git diff --check
  -> pass
```

xhigh read-only review:

```text
verdict = allow_commit
Critical = none
Important = none
Minor = CompanyName placeholder; FaceEdge not implemented; review did not run tests
```

## Boundary

```text
AutoCAD L2 import = not_run
FDrawing accepts complex skeleton = not_confirmed
section / hidden / hatch / joint algorithm = not_implemented
section-line LineN / ArcN geometry = not_generated
ZValue semantics = gap
FaceEdge generation = not_implemented
golden = not_collected
```

## Next

```text
TODO-036 / M2-Drawing-005
  -> AutoCAD L2 complex skeleton import verification P0
```
