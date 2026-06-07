# M2-Drawing-017 旧图石启动阻塞提示链补证 P0 实现记录

## 结论

本轮完成 `TODO-048 / 旧图石启动阻塞提示链补证 P0`。

结果不是解除旧图石启动阻塞，
而是把当前阻塞链静态上补到了可指导人工排查的程度：

- 已确认启动主链：

```text
sub_1406BBFC0
  -> sub_1406BC3B0
  -> sub_14070C760(...)
```

- 已确认错误码分支：

```text
0   -> 启动检查通过
41  -> 许可已过期
其他非 0 -> 请检查网线是否接好
```

- 已确认当前链路和 `ChaspBase / HASP / SuperDog / NetHASP`
  一类许可栈强相关。
- 已形成用户手工解除前置条件清单。

本轮不自动再次启动旧图石，
不实现真实接头线 / Others 几何算法，
不声明 AutoCAD L2 通过，不进入 golden。

## 控制合同

Primary Setpoint：

```text
在 TODO-047 已记录旧图石启动期 stop point 的基础上，
用 IDA MCP / 静态证据查清“提示 / 请检查网线是否...”的来源链、
错误码分支和人工排查方向。
```

Acceptance：

```text
新增 E-IDA-030。
新增 TODO-048 实现记录。
新增 m2_drawing_017_run_001.md / json。
更新 00 / 03 / 11 / 34 / 46 / 99 / todo.csv。
默认 CTest、strict readiness、OCCT 泄漏扫描和 diff --check 通过。
```

Guardrail：

```text
不得把静态证据写成“旧图石已经能正常启动”。
不得再自动启动旧图石。
不得把 OCCT 能力或字段骨架写成旧业务算法证据。
不得实现真实接头线 / Others 几何算法。
```

## 本轮补到的静态事实

### 1. 启动阻塞主链

当前确认：

```text
sub_1406BBFC0
  -> sub_1406BC3B0
  -> sub_14070C760(&qword_140994BF8, a1szphyqnlqkepf, 0)
```

启动主函数分支为：

```text
if sub_1406BC3B0() > 0:
  dword_140994BF0 = 2
  appFlag(+178) = 1
  continue -> sub_1406FF100(this)

else if dword_140994BF0 == -1:
  appFlag(+178) = 0
  return 0

else if dword_140994BF0 == 0:
  appFlag(+178) = 0
  MessageBoxA(..., "请检查网线是否接好", "提示", MB_ICONHAND)
  return 0
```

白话就是：

```text
旧图石一启动先过许可 / 网络相关检查。
没过就先弹框，不让你进主界面。
```

### 2. 错误码 41 的语义

`sub_1406BC3B0` 当前确认：

```text
v0 = sub_14070C760(...)

if v0 == 41:
  dword_140994BF0 = -1
  MessageBoxA(..., "许可已过期", "提示", MB_ICONHAND)
  return 0

return !v0
```

这说明：

```text
41 是明确映射到“许可已过期”。
用户现在看到的“请检查网线是否接好”，
对应的是其他非 0 失败码落入的 fallback 分支。
```

### 3. 启动弹框文案已闭合

本轮同时用两种方式闭合了提示文案：

1. IDA 反编译中的字节地址引用：

```text
byte_14075A738 -> title
byte_14078D7A8 -> expired
byte_14078D820 -> fallback
```

2. `VisualTS.exe` 原始二进制按 GB2312 解码：

```text
0x758F38 -> 提示
0x78BFA8 -> 许可已过期
0x78C020 -> 请检查网线是否接好
```

因此当前启动期三条关键文案已经不是“猜大概”，
而是静态可复核的事实。

### 4. 许可栈侧证据

`sub_14070C428` 当前可确认：

```text
*a1 = &ChaspBase::vftable
```

再结合命中的字符串：

```text
HASP-HL
HASP-SL
SuperDog
sentinelhl,hasphl
NETHASP_00112233445566zz
hasp_enabled
nethasptype
HL_LICENSEDIR
hlrus_license_file.alf
```

以及对应 xref：

```text
NETHASP_00112233445566zz -> sub_140237E8A
nethasptype             -> sub_14018B7D0
hasp_enabled            -> sub_1401882A8
HL_LICENSEDIR           -> sub_14031C11B
hlrus_license_file.alf  -> sub_14031C11B
```

当前可保守写成：

```text
旧图石启动阻塞强相关于 Sentinel / HASP / SuperDog / NetHASP 许可栈。
```

但不能超写成：

```text
本机现在一定就是某个 license file 缺失。
```

这一点还需要人工确认。

### 5. 这条 gate 不是只在启动时用一次

`sub_1404DEA40` 当前也复用了：

```text
sub_14070C760
sub_14070C428
sub_14070C7E0
```

caller 里可见：

```text
sub_14055C1F0
sub_140600AA0
sub_140605B20
...
```

这说明：

```text
就算只从启动角度看它是阻塞框，
从系统角度看它其实还是旧图石多功能共用的许可 gate。
所以不该在启动没过的前提下，继续假设后面的接头命令和工程图命令能可靠运行。
```

## 用户手工确认清单

基于当前静态证据，建议后续人工排查按这个顺序做：

1. 确认旧许可本身没有过期。
2. 确认 Sentinel / HASP / SuperDog 相关许可环境或服务已就绪。
3. 如果是网络许可，确认本机网络可用。
4. 如果是网络许可，确认 license server / 网络狗宿主机可达。
5. 如果仍失败，再检查 `HL_LICENSEDIR` 和 `hlrus_license_file.alf` 相关文件链。

置信度：

- 1~4：高。
- 5：中，当前属于许可栈侧证据，不是已闭合主因。

## 仍未闭合

- 当前这台机器实际命中的非 0 错误码是多少。
- 当前这台机器到底是本地狗、网络许可还是其他同栈模式。
- `HL_LICENSEDIR / hlrus_license_file.alf` 在当前环境里是否真的参与了失败。
- 用户手工处理前置条件后，旧图石是否能稳定进入主界面。
- 进入主界面后，接头相关旧运行非空样例是否可采到。

## 下一步建议

下一步不应该继续自动启动旧图石，
而是转到一个明确的手工节点：

```text
TODO-049 / 旧图石启动前置条件用户手工确认 P0
  -> 用户按手工清单检查许可 / 服务 / 网络 / license file 环境
  -> 用户自己手工重试启动旧图石
  -> 若仍失败，记录完整弹框文本和环境状态
  -> 若成功进入主界面，再回到非空接头样例采集
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

## 边界声明

本记录只证明：

- 启动阻塞链、错误码 41 语义和两条关键中文提示已静态闭合。
- 当前阻塞与 `ChaspBase / HASP / SuperDog / NetHASP` 许可栈强相关。
- 可以形成人工排查清单。

本记录不证明：

- 旧图石已经能正常启动。
- 当前本机真实根因已经被动态验证。
- 真实接头线算法已闭合。
- 真实 Others 几何算法已闭合。
- AutoCAD L2 已通过。
- 旧插件接受新包。
- 完整工程图已完成。
- golden 已采集。
