# M2-RebarCreate-012 线配筋公共创建 Roles DTO 与 Raw Evidence 对齐 P0 实现记录

todoId = TODO-082
phase = M2-RebarCreate-012
evidenceIds = E-IDA-048, E-DEV-104

## 目标

本轮只完成 `TODO-082 / 线配筋公共创建 roles DTO 与 raw evidence 对齐 P0`。

目标不是实现完整 `sub_1405D5670` 算法，也不是把 `objA / objB` 起成固定中文业务名，而是把 TODO-081 已收窄的静态证据落到正式 app 的可测试 DTO 和 raw evidence：

```text
createdObject + 104 = createdPayload
createdObject + 112 = linkedModelRef / createdLinkRef，低置信
sub_1405D5670 arg4 = distanceA_4digit
objA / objB 在线配筋和弧形组入口会交换角色
```

## 代码变更

新增 / 调整：

```text
app/src/domain/rebar/RebarGroupCreator.h
  -> 新增 LegacyPublicCreateRolesSnapshot
  -> RebarGroupCreationRequest 增加 publicCreateRoles

app/src/domain/rebar/RebarGroupCreator.cpp
  -> 新增 distanceA4Digit(distanceA)
  -> creationParameters 写入 roles DTO / raw evidence
  -> normalizeSegmentCurve 使用 distanceA_4digit，而不是未截断 distanceA
  -> 移除过期 unresolved field: sub_1405D5670.arg4
  -> 保留真正未闭合项：sub_1405D5670.fullEquivalence、objA/objB.oldClassNames、createdObject+112.businessMeaning

app/tests/unit/rebar_group_creator_tests.cpp
  -> 新增 roles DTO / raw evidence 测试
  -> 覆盖 distanceA_4digit、createdPayloadRef、linkedModelRef 低置信、objA/objB 不硬编码
```

## DTO 字段

`LegacyPublicCreateRolesSnapshot` 当前只作为旧公共创建角色快照，不是完整旧对象模型：

```text
objARole = createContextOwner/sourceContextObject
objBRole = refreshTargetObject/counterpartObject
roleConfidence = candidate
objAObjBMaySwapByEntryPoint = true
createdPayloadRef = createdObject+104
linkedModelRef = createdObject+112
linkedModelRefConfidence = low
```

这些名字是工程边界名，不是旧 VisualTS 源码类名。

## Raw Evidence 字段

`createdFromParameters / legacyRaw` 新增：

```text
sub_1405D5670.distanceA4Digit
sub_1404D10C0.objA.roleCandidate
sub_1404D10C0.objB.roleCandidate
sub_1404D10C0.objAObjB.roleConfidence
sub_1404D10C0.objAObjB.rolesMaySwapByEntryPoint
sub_140451730.createdPayloadRef
sub_140451730.linkedModelRef
sub_140451730.linkedModelRef.confidence
```

其中 `distanceA4Digit` 按旧汇编证据：

```text
distanceA4Digit = (int)(distanceA * 10000.0) / 10000.0
```

## TDD 记录

先补测试后实现：

```text
cmake --build .\app\build --target rebar_group_creator_tests
  -> red：RebarGroupCreationRequest 没有 publicCreateRoles

新增 DTO 和 raw evidence 后：
  -> green

继续补 normalizer 参数测试：
  -> red：normalizerRequest 仍使用未截断 distanceA

改为 normalizerRequest(distanceA4Digit(request.distanceA)) 后：
  -> green
```

## 验证

```text
targetedTest = pass
  cmd /c "call vcvars64.bat >nul && cmake --build .\app\build --target rebar_group_creator_tests && .\app\build\rebar_group_creator_tests.exe"

targetedCTest = pass
  ctest --test-dir .\app\build -R rebar_group_creator_tests --output-on-failure

defaultCTest = pass
  21/21 pass, 205.66 sec

readinessGateUnit = pass
  50/50 pass

readinessGateStrict = pass
  M1-Formal-Ready, 84/84 pass

domainRebarCommandOCCLeak = pass
  no matches; rg exit code 1 is expected

gitDiffCheck = pass

xhighReadonlyReview = needs_fix_then_fixed_by_main_flow
  xhigh 发现 run report 仍有未完成 verification marker；
  主流程已修复报告并重新跑 readiness gate 通过；
  reviewer agent 已关闭。
```

## 没有做的事

本轮没有：

```text
启动旧图石
安装 HASP
修改系统目录
修改 UI 文案
实现完整线配筋算法
实现面配筋 / 弧筋 / 接头 / Excel / Detail / golden
迁入父目录 rebar 业务代码
用 OCCT 直接创建钢筋业务对象
在 domain/rebar 引入 TopoDS_ / AIS_ / BRep / TopAbs_
```

## 仍未闭合

```text
createdObject + 112 的准确旧业务含义
objA / objB 的旧源码真实类名和 UI 业务名
object + 80 / 88 / 96 在不同对象上的准确含义
完整 sub_1405D5670 split / spline / trim / dirty 写回等价
旧线配筋真实运行提示、状态栏、主参数窗口字段
线配筋 / 弧形组 golden 对照
```

## 下一步

建议下一轮执行：

```text
TODO-083 / 线配筋公共创建 sub_1405D5670 split-spline-trim 等价切片 P0
```

下一轮才开始继续拆 `sub_1405D5670` 行为，不在 TODO-082 内扩张完整算法。
