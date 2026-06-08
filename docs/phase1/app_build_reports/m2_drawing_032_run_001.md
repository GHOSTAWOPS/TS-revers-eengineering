# M2-Drawing-032 Run 001

todoId = TODO-063
phase = M2-Drawing-032
evidenceId = E-DEV-085
idaEvidenceId = E-IDA-043
decision = generate-package-command-map-correction-and-dialog274-downgrade

## Summary

本轮完成 `TODO-063 / 生成工程图与下料表命令映射纠偏及 Dialog #274 降级静态补证 P1`。

本轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，没有实现真实工程图算法。

核心结论：

```text
correctedCommandMap:
  36124 / 0x8D1C
    -> psallc
    -> sub_140600AA0
    -> Detail.xml + DetailNN.stl

  35057 / 0x88F1
    -> psexcel
    -> sub_140605B20
    -> sub_140602F90
    -> StbTable / MaterialTable

extraCorrection:
  0x8CD2 -> ysteelout

dialog274:
  sub_1406AD840
    -> Set2Dpage
    -> not direct GeneratePackage settings truth
```

## IDA Evidence

```text
database = visualts_todo062_generate_drawing
module = VisualTS.exe
hexraysReady = true
coveredFunctions:
  sub_1406F37B0
  sub_140600AA0
  sub_140605B20
  sub_1406AD840
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
xhighReview = not_required_docs_only
```

## Verification

```text
todoCsvParse = pass; TODO-063=done; next=TODO-064
gitDiffCheck = pass
readinessGateStrict = pass; M1-Formal-Ready; 84/84 pass
ctest = pass; 18/18 pass
relationshipTrace = pass; REQ-DRAW-003/004 include E-IDA-043/E-DEV-085 and corrected command mapping
```

同名 JSON 已与以上验证结果同步。

## Next

```text
nextTodo = TODO-064
task = 生成工程图与下料表中文caption资源绑定及真正生成工程图弹窗候选补证 P1
```

说明：

```text
TODO-064 不再去追一个已经被纠偏掉的假链路，
而是继续收窄：
  36124 / 35057 的中文 caption / resource
  真正生成工程图弹窗候选
```
