# M2-Drawing-018 Run 001

todoId = TODO-049
phase = M2-Drawing-018
decision = local-precheck-recorded-user-manual-confirmation-pending
userActionRequired = true
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮没有自动再次启动旧图石。

本轮只完成：

- `TODO-049` 的本机许可环境预检；
- 用户手工确认动作最小化；
- 文档和缺口回填准备。

当前关键结论：

```text
本机存在 SafeNet Sentinel 数据目录，
但当前没有看到典型本地 Sentinel / HASP 许可服务或监听端口在工作。
```

因此 `TODO-049` 仍然不能写成 done，
下一步必须由用户现场确认 USB 狗 / 网络许可 / 内网或 VPN / 手工启动结果。

## 本机预检结果

### 服务

```text
hasplms   -> not_installed
aksfridge -> not_installed
```

关键字扫服务时，
没有发现明确命名为 Sentinel / HASP / SuperDog 的本地许可服务。

### 本地端口

```text
127.0.0.1:1947 -> False
127.0.0.1:475  -> False
127.0.0.1:6001 -> False
```

### 环境变量

```text
HASP / SENTINEL / LICENSE / HL_ -> NO_MATCH
```

### 目录痕迹

```text
C:\ProgramData\SafeNet Sentinel                -> True
C:\ProgramData\SafeNet Sentinel\Sentinel LDK  -> True

C:\Program Files\SafeNet Sentinel             -> False
C:\Program Files (x86)\SafeNet Sentinel       -> False
C:\Program Files\Common Files\SafeNet         -> False
C:\Program Files (x86)\Common Files\SafeNet   -> False
```

### 网络基础状态

```text
Up adapters:
  WLAN
  vEthernet (FSE HostVnic)
  vEthernet (Default Switch)
```

## 当前判断

更接近下面几类之一：

```text
1. 许可环境未完整就绪
2. 本地许可服务未工作
3. 实际依赖网络许可 / 网络狗，但当前现场未连到正确环境
4. 实际依赖 USB 狗，但当前未识别或未插好
```

当前不应直接下结论为：

```text
就是 license file 缺失
或
就是单纯断网
```

## 需要用户回填的最小结果

### 仍阻塞

```text
USB狗：有 / 没有 / 不确定
网络许可：是 / 否 / 不确定
当前是否已连内网/VPN：是 / 否
弹框标题：
弹框完整文本：
是否还是启动前就弹：
截图：有 / 无
```

### 已进入主界面

```text
已进入主界面：是
打开到哪一步：
是否能打开 SFL：
主界面截图：有 / 无
```

## 已确认事实

```text
localPrecheck.completed = true
localNetwork.basicUp = true
localSentinelDataDir.present = true
localSentinelService.present = false
localSentinelLocalPorts.open = false
licenseEnvVars.present = false
userManualConfirmation.completed = false
oldRuntimeAutomaticallyLaunched = false
algorithmImplemented = false
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
