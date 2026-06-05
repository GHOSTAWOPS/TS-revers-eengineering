# Validator 实现契约与错误码总表

## 目标

本文件把 `24 / 26 / 29 / 30` 中的 validator 清单压成可编码契约。

它回答：

```text
validator 在工程里怎么组织？
每个 validator 输入什么、输出什么？
错误码在哪里统一？
Save/Open 如何调用 validator？
validator 报告怎么写入 readiness？
什么情况下不能覆盖旧包、不能清 dirty、不能进入正式编辑态？
```

本文件新增开发证据：

```text
E-DEV-010
  -> Validator 实现契约与错误码总表
  -> 来源：32_Validator实现契约与错误码总表.md

E-DEV-015
  -> tsrebar validator 脚本、unittest 和首批 validation reports
  -> 来源：tools/tsrebar_validator/ 与 docs/phase1/validator_reports/

E-DEV-016
  -> standalone Save/Open transaction gate、unittest 和首批 Save/Open reports
  -> 来源：tools/tsrebar_transaction_gate/ 与 docs/phase1/save_open_reports/
```

注意：

- `E-DEV-010` 只证明 validator 的接口、错误码、执行顺序和报告契约已经明确。
- `E-DEV-015` 证明首批独立 validator 已实现并跑出 reports。
- `E-DEV-016` 证明独立 Save/Open transaction gate 已经围绕 validator 跑通。
- 它不能单独关闭 `GAP-DEV-001 / GAP-DEV-007`。
- `E-DEV-018` 已补充 Qt6 应用 Save/Open 与 binding 修复契约。
- 只有 Qt6 应用内 Save/Open 代码、应用级运行报告、binding 修复运行报告和 readiness 记录存在后，才能继续推进缺口状态。

## 控制合同

| 字段 | 约束 |
|---|---|
| Primary Setpoint | 新设计文件格式的包、引用、旧对象、几何引用、binding、Detail 映射、证据和事务状态都必须可被统一 validator 判定。 |
| Acceptance | 本文件给出接口、执行顺序、错误码总表、报告 schema、Save/Open gate 和 readiness 接入方式。 |
| Guardrail Metrics | validator 失败不能清 dirty；不能覆盖旧包；不能把 OCCT 几何对象反向当钢筋业务真相；不能用父目录测试结果关闭业务缺口。 |
| Boundary | 只定义新格式 validator 工程契约；不实现代码；不声明真实 fixture 已通过。 |
| Rollback Trigger | 任何 `error` 级 finding 出现，保存必须停止在临时包，正式包不变。 |

## 设计原则

### 1. 单一事实源

validator 只检查工程包事实，不创造业务事实。

```text
旧图石业务语义
  -> 来自 SFL / IDA / Detail / 运行确认

OCCT / STEP 几何语义
  -> 来自 source_step / topology_refs / STEP diff

二者关系
  -> 来自 binding

validator
  -> 只判断这些关系是否一致、可追溯、可保存
```

### 2. 字段级错误

validator 不能只返回 `true / false`。

每个失败必须定位到：

- validator 名称。
- objectId。
- package 文件。
- jsonPath。
- errorCode。
- evidenceHint。
- severity。

### 3. 保存事务优先

Save/Open 不是“写完再说”。

保存必须按下面顺序：

```text
写临时包
  -> 跑全部 validator
  -> 写 validator report
  -> 无 error 才原子替换正式包
  -> 成功才清 dirty
```

有任何 `error`：

```text
正式包不变
ProjectDirty 不清
tmp 包保留或归档
audit 写 failed
```

## 建议工程位置

正式工程初始化时建议建立：

```text
src/project/validation/
  ValidationFinding.h
  ValidationReport.h
  ValidationContext.h
  IProjectValidator.h
  PackageValidator.h
  ProjectReferenceValidator.h
  LegacyObjectValidator.h
  GeometryRefValidator.h
  LegacyGeometryBindingValidator.h
  CoreObjectBindingValidator.h
  DetailMappingValidator.h
  EvidenceValidator.h
  TransactionValidator.h
  ProjectValidationRunner.h

tests/unit/project/validation/
tests/integration/project/validation/
docs/phase1/validator_reports/
```

说明：

- 文件名只是建议，正式工程可按 CMake 目录调整。
- 职责边界不能变。
- validator 不直接依赖 Qt UI。
- validator 可以依赖 JSON 解析、工程包读取、hash 和路径解析。
- validator 不依赖 AutoCAD、旧图石运行时、ACIS、HOOPS。

## 核心接口

### ValidationFinding

```cpp
struct ValidationFinding {
  std::string validatorName;
  std::string objectId;
  std::string packageFile;
  std::string jsonPath;
  std::string errorCode;
  std::string message;
  std::vector<std::string> evidenceHint;
  std::string severity; // error / warning
};
```

硬规则：

- `errorCode` 必须来自本文错误码总表。
- `severity` 首期只允许 `error / warning`。
- `jsonPath` 包级错误可写 `/`。
- `objectId` 包级错误可写 `project.tsrebar`。

### ValidationReport

```cpp
struct ValidationReport {
  std::string reportId;
  std::string packagePath;
  std::string packageHash;
  std::string createdAt;
  std::string runnerVersion;
  std::vector<ValidationFinding> findings;
  int errorCount;
  int warningCount;
  std::string decision; // pass / fail / blocked
};
```

排序规则：

```text
severity(error first)
  -> validatorName
  -> errorCode
  -> packageFile
  -> jsonPath
  -> objectId
```

这样报告可稳定 diff，不会因为遍历顺序抖动。

### ValidationContext

```cpp
struct ValidationContext {
  PackageSnapshot package;
  EvidenceIndex evidenceIndex;
  GapIndex gapIndex;
  StepSelectionIndex stepSelectionIndex;
  std::optional<SaveTransactionSnapshot> saveTransaction;
};
```

说明：

- `PackageSnapshot` 是只读工程包快照。
- `EvidenceIndex` 来自 `evidence/evidence_index.json` 和 `11`。
- `GapIndex` 来自 `99_缺口和待确认项.md` 的 GAP ID 清单或工程内同步索引。
- `StepSelectionIndex` 来自 `31` 的 STEP diff 报告索引。
- validator 不修改工程包。

### IProjectValidator

```cpp
class IProjectValidator {
public:
  virtual ~IProjectValidator() = default;
  virtual std::string name() const = 0;
  virtual std::vector<ValidationFinding>
  validate(const ValidationContext& context) const = 0;
};
```

## 执行顺序

固定顺序：

```text
1. PackageValidator
2. ProjectReferenceValidator
3. LegacyObjectValidator
4. GeometryRefValidator
5. LegacyGeometryBindingValidator
6. CoreObjectBindingValidator
7. DetailMappingValidator
8. EvidenceValidator
9. TransactionValidator
```

为什么按这个顺序：

- 先确认包能读。
- 再确认引用链存在。
- 再确认旧业务对象和几何引用。
- 再确认二者 binding。
- 再确认 Detail 同源。
- 最后确认证据和事务后果。

执行策略：

- `PackageValidator` 如果发现 JSON 无法解析，可跳过依赖该文件的后续字段级检查，但仍输出包级错误。
- 其他 validator 尽量继续收集错误，不因第一个错误提前退出。
- `TransactionValidator` 必须看到前面所有 findings，才能判断保存后果。

## 报告落盘

建议输出：

```text
docs/phase1/validator_reports/
  fixture_a_validation_001.json
  fixture_b_validation_001.json
  fixture_c_validation_001.json
  negative_missing_required_file_001.json
  negative_broken_binding_001.json
  save_failure_validation_001.json

audit/
  validation_runs.jsonl
```

报告 JSON：

```json
{
  "schemaVersion": "validation-report/v1",
  "reportId": "fixture_b_validation_001",
  "packagePath": "fixtures/tsrebar/fixture_b_single_group/project.tsrebar",
  "packageHash": "sha256:TBD",
  "createdAt": "YYYY-MM-DDTHH:mm:ss+08:00",
  "runnerVersion": "TBD",
  "decision": "fail",
  "errorCount": 1,
  "warningCount": 0,
  "findings": [
    {
      "validatorName": "LegacyGeometryBindingValidator",
      "objectId": "group-001",
      "packageFile": "rebar/groups.json",
      "jsonPath": "#/items/0/binding/items/0/geometryPath",
      "errorCode": "LGV004_GEOMETRY_PATH_BROKEN",
      "message": "binding geometryPath points to a missing geometryRef target",
      "evidenceHint": ["GAP-DEV-002", "E-DEV-004"],
      "severity": "error"
    }
  ]
}
```

## 错误码总表

### PackageValidator

- `PV001_REQUIRED_FILE_MISSING`：`manifest.requiredFiles` 中的文件缺失。
- `PV002_JSON_PARSE_FAILED`：JSON 无法解析。
- `PV003_UNSUPPORTED_SCHEMA_VERSION`：schema 或 formatVersion 不支持。
- `PV004_SAVE_TRANSACTION_INCOMPLETE`：工程包处于半写或未完成事务状态。
- `PV005_REQUIRED_FILE_NOT_DECLARED`：实际必需文件存在但未列入 manifest。

### ProjectReferenceValidator

- `PRV001_MODEL_FILE_NOT_FOUND`：`project.json` 指向的模型文件不存在。
- `PRV002_REBAR_REF_BROKEN`：group / bar / segment 引用断裂。
- `PRV003_DETAIL_MAPPING_REF_BROKEN`：Detail mapping 指向不存在 rebar 对象。
- `PRV004_EVIDENCE_REF_BROKEN`：对象 evidence ID 在 evidence index 中不存在。
- `PRV005_SOURCE_STEP_REF_BROKEN`：`sourceStepId` 指向不存在的 `source_step`。

### LegacyObjectValidator

- `LOV001_LEGACY_RAW_MISSING`：核心对象缺少 `legacyObject.raw`。
- `LOV002_UNKNOWN_LEGACY_TYPE`：旧对象类型不在允许清单中。
- `LOV003_LEGACY_SCHEMA_INVALID`：raw schema 不合法。
- `LOV004_LEGACY_WRITE_ORDER_INVALID`：旧对象写出顺序与 IDA 证据不一致。
- `LOV005_UNRESOLVED_FIELD_NOT_LISTED`：低置信字段没有进入 `unresolved_fields.json`。
- `LOV006_LEGACY_EVIDENCE_MISSING`：旧对象没有证据来源。

### GeometryRefValidator

- `GRV001_SOURCE_STEP_NOT_FOUND`：STEP 来源文件或记录缺失。
- `GRV002_TOPOLOGY_REF_BROKEN`：face / shell / solid 引用断裂。
- `GRV003_CURVE_REF_BROKEN`：edge / curve 引用断裂。
- `GRV004_ACIS_POINTER_USED_AS_STABLE_ID`：把 ACIS 指针、内存地址或 `ENTITY*` 当稳定 ID。
- `GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID`：只用 `occtTransientIndex` 当稳定 ID。
- `GRV006_STEP_DIFF_MISSING`：正式可保存 geometryRef 缺少 `31` 要求的 STEP diff 证据。
- `GRV007_FINGERPRINT_DUPLICATE_UNRESOLVED`：重复 fingerprint 未消歧也未进入 unresolved。

### LegacyGeometryBindingValidator

- `LGV001_BINDING_MISSING`：核心对象缺少 binding。
- `LGV002_BINDING_STATE_INVALID`：binding state 不合法。
- `LGV003_LEGACY_PATH_BROKEN`：binding 的 legacyPath 指向不存在 raw 字段。
- `LGV004_GEOMETRY_PATH_BROKEN`：binding 的 geometryPath 指向不存在 geometryRef。
- `LGV005_BINDING_EVIDENCE_MISSING`：binding 缺少 evidence。
- `LGV006_SILENT_REBIND_FORBIDDEN`：topology mismatch 后静默改绑到相似面/边/曲线。
- `LGV007_UNRESOLVED_STATE_NOT_ENFORCED`：恢复失败但对象仍进入正式编辑态。

### CoreObjectBindingValidator

- `COB001_CORE_LEGACY_RAW_MISSING`：核心 rebar 对象缺少 `legacyObject.raw`。
- `COB002_CORE_GEOMETRY_REF_MISSING`：需要几何的对象没有 geometryRef，也没有 unresolved 状态。
- `COB003_STEEL_DATA_EVIDENCE_MISSING`：SteelData 缺少 evidence。
- `COB004_PURE_GEOMETRY_OBJECT_FORBIDDEN`：纯 OCCT 几何对象进入正式 RebarModel。
- `COB005_DETAIL_SOURCE_NOT_REBAR_MODEL`：Detail 映射不是从同一 RebarModel 派生。

### DetailMappingValidator

- `DMV001_GROUP_ID_NOT_FOUND`：`StbGroup.groupID` 找不到 RebarGroup。
- `DMV002_RSD_ID_MISMATCH`：`StbGroup.rsdID` 与 `StbTable.rsdID` 不同源。
- `DMV003_SEGMENT_ID_NOT_FOUND`：`StbGeo.segID` 找不到 RebarSegment。
- `DMV004_SCHEDULE_ROW_ORPHANED`：下料表行找不到对应钢筋组。
- `DMV005_DETAIL_FIELD_CONFIDENCE_INVALID`：低置信 Detail 字段被标为 confirmed。

### EvidenceValidator

- `EV001_EVIDENCE_ID_NOT_FOUND`：evidence ID 不存在。
- `EV002_GAP_ID_NOT_FOUND`：GAP ID 不存在。
- `EV003_PENDING_MARKED_CONFIRMED`：pending 字段被标成 confirmed。
- `EV004_CONFIRMED_WITHOUT_EVIDENCE`：confirmed 字段没有 SFL / IDA / Detail / 运行证据。
- `EV005_PARENT_SOURCE_USED_AS_BUSINESS_TRUTH`：父目录源码被当成旧图石业务证据。
- `EVW001_PENDING_LEGACY_FIELD`：低置信旧字段存在，但已正确进入 unresolved。

### TransactionValidator

- `TXV001_TEMP_PACKAGE_INVALID`：临时包无法完整打开或验证失败。
- `TXV002_FAILED_SAVE_OVERWROTE_OLD_PACKAGE`：失败保存覆盖了旧包。
- `TXV003_FAILED_SAVE_CLEARED_DIRTY`：失败保存清除了 dirty。
- `TXV004_SUCCESS_WITHOUT_AUDIT`：成功保存没有 audit。
- `TXV005_DIRTY_CLEAR_RULE_BROKEN`：dirty 清除规则违反 `08_开发命令契约.md`。
- `TXV006_HALF_WRITTEN_PACKAGE_OPENED_AS_FORMAL`：半写包被当正式包打开。

## Severity 判定

必须是 `error`：

- 会导致保存覆盖旧包。
- 会导致 dirty 错误清除。
- 会导致旧业务对象丢失。
- 会导致 geometryRef 静默改绑。
- 会导致 Detail ID 不同源。
- 会导致父目录源码被当业务证据。

允许是 `warning`：

- 字段低置信，但已经进入 `unresolved_fields.json`。
- geometryRef 暂时 unresolved，但对象被正确标为只读/修复状态。
- 旧图石运行确认缺口存在，但命令仍保持 Open/placeholder。

warning 不能自动升级为 ReadyForDev。

## Save Gate

保存调用：

```text
Project.Save
  -> write tmp package
  -> ProjectValidationRunner.validate(tmp)
  -> if errorCount > 0:
       keep old package
       keep dirty
       write failed audit
       keep report
       return SaveFailed
  -> atomic replace
  -> write success audit
  -> clear dirty
```

禁止：

- validator error 后继续覆盖正式包。
- validator error 后清 dirty。
- validator warning 被当成已闭合证据。
- 临时包未验证就替换正式包。

## Open Gate

打开调用：

```text
Project.Open
  -> read manifest
  -> run PackageValidator
  -> read package snapshot
  -> run validators
  -> if package errors:
       open as blocked / repair mode
  -> if only warnings:
       open with warnings and unresolved panel
  -> if no findings:
       open formal edit mode
```

状态映射：

- `pass`：正式编辑态。
- `warning-only`：可读，可编辑范围受 unresolved 状态限制。
- `error`：只读或修复态，不允许保存覆盖原包。
- `blocked`：包级损坏，不能进入 RebarModel。

## Negative Fixture 对应关系

| Negative fixture | 主 validator | 期望 errorCode | Save 后状态 |
|---|---|---|---|
| missing_required_file | PackageValidator | PV001_REQUIRED_FILE_MISSING | 不覆盖旧包，不清 dirty |
| broken_binding | LegacyGeometryBindingValidator | LGV004_GEOMETRY_PATH_BROKEN | 不覆盖旧包，不清 dirty |
| acis_pointer_as_geometry_id | GeometryRefValidator | GRV004_ACIS_POINTER_USED_AS_STABLE_ID | 不覆盖旧包，不清 dirty |
| transient_index_only | GeometryRefValidator | GRV005_TRANSIENT_INDEX_USED_AS_ONLY_ID | 不覆盖旧包，不清 dirty |
| forged_evidence | EvidenceValidator | EV001_EVIDENCE_ID_NOT_FOUND | 不覆盖旧包，不清 dirty |
| pending_field_marked_confirmed | EvidenceValidator | EV003 / EV004 | 不覆盖旧包，不清 dirty |
| detail_rsd_mismatch | DetailMappingValidator | DMV002_RSD_ID_MISMATCH | 不覆盖旧包，不清 dirty |
| half_written_package | PackageValidator / TransactionValidator | PV004 / TXV001 / TXV006 | 不覆盖旧包，不清 dirty |
| missing_step_diff | GeometryRefValidator | GRV006_STEP_DIFF_MISSING | 不覆盖旧包，不清 dirty |

## Readiness 接入

`docs/phase1/readiness_run_001.md` 的 Validator Gate 必须记录：

```text
validator runner:
runner version:
input fixture:
report path:
error count:
warning count:
decision:
blocking error codes:
```

正式 M1-Formal 前必须至少有：

```text
fixture_a_validation_001.json
fixture_b_validation_001.json
fixture_c_validation_001.json
negative_*_validation_001.json
save_success_validation_001.json
save_failure_validation_001.json
open_topology_mismatch_validation_001.json
```

没有这些实际报告：

```text
GAP-DEV-001 不能关闭
GAP-DEV-007 不能关闭
M1-Formal 不能申请
```

## 与旧图石 1:1 的边界

validator 不负责生成钢筋。

它负责保证：

- 钢筋对象能追回旧图石证据。
- 几何引用能追回 OCCT / STEP 证据。
- binding 能说明旧对象和新几何如何对应。
- Detail 字段来自同一个 RebarModel。
- 低置信字段不会被偷偷写成 confirmed。
- 父目录源码不会变成业务真相。

白话说：

```text
validator 是门卫。
它不生产业务规则。
它只挡住不能进入正式工程包的坏状态。
```

## 关联追溯

Requirement：

- `REQ-PROJ-001`：新设计文件格式。
- `REQ-TECH-002`：Qt6 + OCCT 开发入口门禁。

Evidence：

- `E-DEV-010`：Validator 实现契约与错误码总表。

GAP：

- `GAP-DEV-001`：新设计文件格式实际 fixture / validator / binding / 回滚测试。
- `GAP-DEV-007`：开发前 readiness 实际打勾。

关联文档：

- `24_新设计文件格式Schema与Fixture草案.md`
- `26_首期可验证联合格式Fixture与Validator执行清单.md`
- `29_首期tsrebar实际Fixture包清单与Golden断言.md`
- `30_Phase1工程初始化Runbook与Readiness记录模板.md`
- `31_STEP选择ID实际运行记录模板与样本清单.md`
- `99_缺口和待确认项.md`

## 当前结论

现在 validator 已从“有哪些检查项”推进到“工程里怎么实现、怎么报告、怎么挡保存”的契约。

独立 validator 的首批真实实现已经发生，见 `E-DEV-015`。

standalone Save/Open transaction gate 的首批运行报告已经发生，见 `E-DEV-016`。

当前状态仍是：

```text
GAP-DEV-001 = P0 / Open
下一步 = Qt6 应用内 Save/Open 实现与回滚测试、binding 修复实现与运行报告
```
