# M1-App-022 新设计文件格式 Runtime P1 实现记录

## 结论

本轮完成 `TODO-023 / M1-App-022`。

正式 `app` 新增项目 runtime：

```text
TsRebarProjectRuntime
TsRebarProjectSnapshot
TsRebarProjectOpenResult
TsRebarProjectSaveResult
```

它可以把当前 domain 快照保存为目录形态 `project.tsrebar`，并重新打开恢复：

```text
STEP 来源
selection-v1 refs / topology refs
SteelData / SteelBarGroup / SteelBar / SteelBarSegment
legacyObject.raw
geometryRef
binding
evidence / unresolved_fields
Detail 映射占位
```

本轮不是 Detail writer，不接完整 UI Save/Open 菜单，不新增钢筋创建 / 编辑 / 统计 / 出图业务规则。

## 新增能力

### Save runtime P1

`saveSnapshot()` 当前按目录包写出：

```text
manifest.json
project.json
geometry/source_step.json
geometry/topology_refs.json
rebar/steel_data.json
rebar/groups.json
rebar/bars.json
rebar/segments.json
drawing/detail_mapping.json
evidence/evidence_index.json
evidence/unresolved_fields.json
```

保存事务语义：

```text
写 candidate 包
  -> C++ runtime validation
  -> validator error 时不替换正式包
  -> validator error 时不清 dirty
  -> validator error 时清理 candidate 包
  -> 无 error 时替换正式包并清 dirty
```

### Open runtime P1

`open()` 当前可恢复：

- STEP source id / path / originalPath / sha256。
- topology refs。
- steelData / groups / bars / segments。
- `legacyObject.raw`。
- `geometryRef.curveStableIds` 中的 `selection-v1` 引用。
- `binding.state` 和 `binding.items`。

打开状态当前覆盖：

```text
OpenedFormal
OpenedWarning
OpenedRepairRequired
OpenBlocked
```

### Runtime validator P1

C++ runtime validator 当前覆盖本轮必须守住的事务口径：

- required files 存在。
- `saveTransaction.state == complete`。
- unresolved fields 进入 `EVW001_PENDING_LEGACY_FIELD` warning。
- group / bar / segment 引用链断裂进入 `PRV002_REBAR_REF_BROKEN`。
- binding `geometryPath` 断裂进入 `LGV004_GEOMETRY_PATH_BROKEN`。
- resolved binding 缺 `geometryPath` 进入 `LGV004_GEOMETRY_PATH_BROKEN`。
- `unresolvedGeometry` 缺 `openMode=repairRequired/readOnlyGeometry` 进入 `LGV007_UNRESOLVED_STATE_NOT_ENFORCED`。

外部 Python validator 对 runtime 导出包的结果：

```text
decision = warning-only
errorCount = 0
warningCount = 2
```

2 个 warning 是预期低置信旧字段：

```text
GAP-SFL-004 -> steelbargroup positions168_192
GAP-SFL-005 -> seg_steelbargroup ptr80
```

## TDD

本轮先补测试，再实现。

RED 覆盖：

```text
segment.groupId 初始为空，project_runtime_tests 失败：
segment must preserve owning groupId for ProjectReferenceValidator

broken rebar references 初始会被保存，project_runtime_tests 失败：
broken rebar references must not save

sourceStepId 初始硬编码 step-main，project_runtime_tests 失败：
group geometryRef sourceStepId must not be hardcoded
```

后续根据 xhigh 第一轮 block 又补充：

```text
resolved binding without geometryPath -> LGV004
unresolvedGeometry without repair openMode -> LGV007
failed save must clean candidate package
relative package path -> must not hang final packageHash
save -> open -> save must preserve unresolved_fields warnings
install failure must keep dirty and clean candidate / replace tmp
```

GREEN：

```text
cmake --build app\build --target project_runtime_tests
TSREBAR_RUNTIME_FIXTURE_OUT=app\build\runtime_validator_fixture.tsrebar app\build\project_runtime_tests.exe
result = pass
```

补充稳定性修复：

```text
验证时发现 TSREBAR_RUNTIME_FIXTURE_OUT 使用相对路径导出会卡在最终 packageHash。
根因是 runtime 对相对 packagePath 没有统一规范化，packageHash 的遍历根和相对路径计算不稳定。
已在 saveSnapshot / open / packageHash 入口统一转为 absolutePackagePath，并新增相对路径保存测试。

xhigh 复审又发现 readSnapshot 未恢复 evidence/unresolved_fields.json，
会导致 save -> open -> save 静默丢失 pending legacy fields。
已恢复 group / bar / segment unresolvedLegacyFields，并新增 round-trip 测试。

xhigh 复审同时指出 replaceDir 失败时缺少 backup 恢复和异常兜底。
已补 replace_tmp / backup_tmp 清理和 backup restore，并让 saveSnapshot 异常进入 SaveFailed、保持 dirty。

后续复审继续指出 hash 异常兜底不完整：
oldPackageHash 在 try 外可能逃逸，catch 内二次 packageHash 可能再抛，成功替换后再算 final hash 已无法恢复旧包。
已改成先计算 candidate hash，成功安装后直接使用 candidate hash；catch 不再重新 hash current。
```

## 验证结果

默认 CTest：

```text
12 / 12 passed
Total Test time = 125.49 sec
```

外部 validator：

```text
python tools\tsrebar_validator\validate_tsrebar.py app\build\runtime_validator_fixture.tsrebar --out-dir docs\phase1\validator_reports\runtime_m1_app_022
result = exit 0
report = docs/phase1/validator_reports/runtime_m1_app_022/build_validation_001.json
```

Readiness gate：

```text
decision = M1-Formal-Ready
78 / 78 pass
0 error
0 warning
```

OCCT / AIS 泄漏扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app\src\domain\rebar app\src\project
result = no matches
```

`git diff --check`：

```text
pass
```

## xhigh 只读 review

第一轮：

```text
Verdict = block_commit
Critical:
  C++ runtime validator 与 Python validator 的 LGV004 / LGV007 口径不一致。
  外部导出验证曾被认为卡住，需要重新证明。
Important:
  文件名大小写不一致。
  SaveFailed 未清理 candidate 包。
```

处理：

```text
文件名统一为 TsRebarProjectRuntime.*。
补齐 LGV004 / LGV007 runtime validation。
SaveFailed 分支清理 candidate 包。
带 TSREBAR_RUNTIME_FIXTURE_OUT 的 project_runtime_tests 重新运行并退出 0。
补齐相对 packagePath 规范化，避免 runtime fixture 导出卡在 packageHash。
重新运行外部 validator、CTest、readiness gate、泄漏扫描、diff check。
```

第三轮复审前修复：

```text
readSnapshot 恢复 unresolved_fields。
replaceDir 安装失败时恢复 backup。
saveSnapshot 捕获写包 / 替换 / hash 异常，清理 candidate 和 replace_tmp，保持 dirty。
hash 异常兜底改为 candidate hash 预计算，避免替换后 final hash 失败。
```

第二轮复审：

```text
Verdict = allow_commit
Critical = none
Important = none
Minor = 2 个 EVW001 pending legacy field warning 属于预期，不阻止提交
```

两个子代理均已关闭。

## IDA / 旧图石确认判断

本轮不新增 IDA MCP 查询。

原因：

```text
TODO-023 是新设计文件格式 runtime P1。
本轮只保存 / 读取已有 domain 对象、binding、geometryRef 和 evidence。
不新增旧图石钢筋生成、编辑、统计、出图业务规则判断。
```

旧业务依据仍来自既有证据：

```text
E-IDA-015 / E-IDA-016 / E-IDA-017
  -> steelData / steelbar / steelbargroup / seg_steelbargroup 读写骨架。

E-IDA-018 / E-IDA-019
  -> Detail 字段映射依据。

E-IDA-022
  -> 旧线筋 / 弧筋公共创建链依据。
```

## 修改范围

```text
app/CMakeLists.txt
app/src/project/runtime/TsRebarProjectRuntime.h
app/src/project/runtime/TsRebarProjectRuntime.cpp
app/tests/unit/project_runtime_tests.cpp
docs/phase1/validator_reports/runtime_m1_app_022/build_validation_001.json
docs/phase1/readiness_gate_reports/readiness_gate_run_001.json
docs/phase1/readiness_gate_reports/readiness_gate_run_001.md
```

以及本轮文档 / 看板更新。

## 未完成项

本轮不声明以下事项完成：

- 完整旧 `.sfl` 兼容读取 / 写回。
- zip 形态工程包。
- UI 菜单 `Project.Save / Project.Open` 完整接线。
- binding repair 用户交互流程。
- Detail writer P1。
- AutoCAD L2 动态验证。
- 旧图石 golden 输出对照。
- 旧钢筋编辑 / 统计 / 出图业务规则。

## 下一步建议

下一阶段建议做：

```text
TODO-024 / M1-App-023
DetailWriter P1 接入 domain rebar
```

原因：

```text
领域模型、旧线筋 / 弧筋 P0、AIS 显示映射、新设计文件 runtime P1 已经具备。
下一步需要把 domain 钢筋组映射到 Detail.xml + DetailNN.stl 首批字段，
为旧 AutoCAD 插件兼容包打通 L0/L1 输出链路。
```

如果想先补证据，也可以先做：

```text
TODO-025 / 旧图石输出钢筋 STP 样本入库验证
```
