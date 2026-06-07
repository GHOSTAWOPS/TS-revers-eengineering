# M2-Drawing-017 Run 001

todoId = TODO-048
phase = M2-Drawing-017
decision = startup-block-chain-closed-manual-prereq-pending
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮完成旧图石启动阻塞提示链的静态补证：

- 补入 `E-IDA-030`。
- 确认启动主链：

```text
sub_1406BBFC0
  -> sub_1406BC3B0
  -> sub_14070C760(...)
```

- 确认 `41 -> 许可已过期`。
- 确认其他非 0 失败码会落到 `请检查网线是否接好`。
- 确认 `ChaspBase`、`HASP / SuperDog / NetHASP`
  许可栈侧证据和共用许可 gate。
- 形成用户手工解除前置条件清单。

本轮不自动再次启动旧图石，
不实现真实接头线 / Others 几何算法，
不声明 AutoCAD L2 通过，不进入 golden。

## IDA / 静态结果

```text
active session:
  database = visualts_i64_todo045
  hexrays_ready = true

startup chain:
  sub_1406BBFC0
    -> sub_1406BC3B0
    -> sub_14070C760(&qword_140994BF8, a1szphyqnlqkepf, 0)

startup mapping:
  v0 == 0
    -> startup check pass
  v0 == 41
    -> MessageBox("许可已过期", "提示")
  other non-zero
    -> MessageBox("请检查网线是否接好", "提示")

message bytes:
  byte_14075A738 -> 提示
  byte_14078D7A8 -> 许可已过期
  byte_14078D820 -> 请检查网线是否接好

raw binary decode:
  0x758F38 -> 提示
  0x78BFA8 -> 许可已过期
  0x78C020 -> 请检查网线是否接好

license object:
  sub_14070C428 -> *a1 = &ChaspBase::vftable

license stack hints:
  HASP-HL / HASP-SL / SuperDog / sentinelhl,hasphl /
  NETHASP_00112233445566zz / hasp_enabled / nethasptype /
  HL_LICENSEDIR / hlrus_license_file.alf

shared gate:
  sub_1404DEA40 also reuses sub_14070C760 / sub_14070C428 / sub_14070C7E0
  callers include sub_14055C1F0 / sub_140600AA0 / sub_140605B20
```

## 手工前置条件清单

```text
1. 确认旧许可没有过期。
2. 确认 Sentinel / HASP / SuperDog 相关许可环境或服务已就绪。
3. 如果是网络许可，确认本机网络可用。
4. 如果是网络许可，确认 license server / 网络狗宿主机可达。
5. 若仍失败，再检查 HL_LICENSEDIR 和 hlrus_license_file.alf 相关文件链。
```

## 已确认事实

```text
startupPrompt.title.closed = true
startupPrompt.expiredText.closed = true
startupPrompt.networkText.closed = true
startupGate.chain.closed = true
startupGate.error41.closed = true
startupGate.otherNonZeroFallback.closed = true
licenseStack.chaspBase.closed = true
licenseStack.haspHints.closed = true
sharedLicenseGate.closed = true
algorithmImplemented = false
oldRuntimeAutomaticallyLaunched = false
autocadL2 = not_run
```

## 明确不声明

```text
旧图石现在已经能正常启动。
当前本机真实根因已经动态确认。
真实接头线算法已实现。
真实 Others 几何算法已实现。
AutoCAD L2 import 已通过。
旧插件接受新包。
完整工程图已完成。
golden 已采集。
```

## 待验证

```text
当前机器真实失败码 / 许可模式 / 服务状态。
用户手工解除前置条件后，旧图石是否能进入主界面。
进入主界面后，非空 steeljoint-line / joints / Others / symbolcutIOS 样例。
```

## 验证

```text
py -3 tools/phase1_readiness_gate/test_phase1_readiness_gate.py
  -> 17/17 pass

ctest --test-dir app/build --output-on-failure
  -> 18/18 pass

py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass

py -3 app/tools/check_domain_rebar_boundary.py app/src/domain/rebar
py -3 app/tools/check_domain_rebar_boundary.py app/src/drawing
py -3 app/tools/check_domain_rebar_boundary.py app/src/project
  -> pass

git diff --check
  -> pass

xhigh read-only review
  -> skipped, docs-only node
```

## 下一步

```text
建议执行 TODO-049：
  -> 旧图石启动前置条件用户手工确认 P0
  -> 不自动再次启动旧图石
  -> 用户手工确认许可 / 服务 / 网络 / license file 环境后重试
```
