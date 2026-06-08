# M2-Drawing-018 Run 003

todoId = TODO-049
phase = M2-Drawing-018
evidenceId = E-RUN-003
decision = usb-dongle-prerequisite-user-confirmed-main-window-result-pending
userActionRequired = true
algorithmImplemented = false
autocadL2 = not_run

## 结论

用户本轮明确回填了两件关键事实：

1. 旧图石启动需要 USB 狗；
2. 新系统复刻目标不继承 USB 狗依赖，要保持开源、无狗。

这会直接改变 `TODO-049` 的剩余问题形状：

```text
不再优先问：
  旧图石到底是不是 USB 狗模式

而是改成：
  USB 狗插好后能否进入主界面
  USB 狗插好后能否打开 SFL
  如果仍失败，是否还叠加网络许可 / VPN / license file 条件
```

因此本轮仍不能把 `TODO-049` 写成 done。

## 当前收敛结果

### 旧图石侧

```text
legacyStartup.usbDongleRequired = true
legacyStartup.mainWindowReachedAfterDongle = pending
legacyStartup.sflOpenAfterDongle = pending
```

### 新系统侧

```text
newSystem.usbDongleDependencyAllowed = false
newSystem.openSourceTarget = true
```

### 执行边界

```text
oldRuntimeAutomaticallyLaunched = false
algorithmImplemented = false
autocadL2 = not_run
```

## 对 TODO-049 的实际影响

当前 `TODO-049` 只新增了用户现场事实，
没有新增自动运行结果。

所以它只能证明：

```text
旧图石启动前置条件里至少包含 USB 狗
新系统不能继承这个硬件许可依赖
```

还不能证明：

```text
插狗后旧图石已经能正常进入主界面
插狗后已经能打开 SFL
当前机器的其余许可 / 网络条件已经满足
```

## 用户下一步最小回填

```text
USB狗已插好：是 / 否
启动后是否进入主界面：
是否能打开 SFL：
如仍失败，弹框完整文本：
如仍失败，是否还需要内网/VPN：
截图：有 / 无
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
