# M2-Drawing-018 Run 002

todoId = TODO-049
phase = M2-Drawing-018
decision = local-precheck-plus-ida-fallback-supplement-recorded-user-manual-confirmation-pending
userActionRequired = true
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮仍然没有自动再次启动旧图石。

这轮新增的不是运行结果，
而是对 `TODO-049` 的一层 IDA 补证：

```text
旧图石的“请检查网线是否接好”
不是纯网络专用报错，
而是宽兜底的许可 / 环境初始化失败文案。
```

因此当前人工排查优先级应保持为：

1. 先确认许可模式；
2. 再确认服务 / 内网 / VPN / 宿主机可达性；
3. 最后再查 `HL_LICENSEDIR / hlrus_license_file.alf` 文件链。

本轮仍不能把 `TODO-049` 写成 done。

## 本轮新增 IDA 事实

### 启动 fallback 的更精确语义

`sub_14070C760` 当前确认：

```text
sub_14070CBF0(&v9)
if (licenseContextReady && sub_14070D178(&v9, a1 + 8) != 0)
  return sub_14070D454(...)
else
  return 500
```

继续拆开：

```text
sub_14070CBF0
  -> 取线程级许可上下文
  -> 用 sub_14070D618(v2 + 24) 判断 ready

sub_14070D618
  -> *(byte *)(ctx + 40) == 0 时直接返回 0

sub_14070D178
  -> ready 标志为 0 或对象映射失败时直接返回 0
```

白话结论：

```text
只要许可上下文没 ready，
或者许可对象没映射出来，
旧图石也会走“请检查网线是否接好”这条 fallback。
```

### 许可关键词的当前定位

本轮复核 xref：

```text
HL_LICENSEDIR             -> sub_14031C11B
hlrus_license_file.alf   -> sub_14031C11B
hasp_enabled              -> sub_1401882A8
nethasptype               -> sub_14018B7D0
NETHASP_00112233445566zz  -> sub_140237E8A
```

当前更适合解释成：

```text
它们是 Sentinel / HASP / NetHASP 许可栈内部线索，
不是旧图石启动 wrapper 已闭合的 app 级配置结论。
```

## 当前状态汇总

```text
localPrecheck.completed = true
idaFallbackSupplement.completed = true
userManualConfirmation.completed = false
oldRuntimeAutomaticallyLaunched = false
algorithmImplemented = false
autocadL2 = not_run
```

## 本轮仍未闭合

```text
当前机器真实失败码
当前机器真实许可模式
用户手工处理前置条件后的启动结果
是否能进入主界面并打开 SFL
是否能回到非空 steeljoint-line / Others 运行样例采集
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
