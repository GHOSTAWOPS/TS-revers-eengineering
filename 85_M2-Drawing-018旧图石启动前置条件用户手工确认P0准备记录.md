# M2-Drawing-018 旧图石启动前置条件用户手工确认 P0 准备记录

## 结论

`TODO-049 / 旧图石启动前置条件用户手工确认 P0`
当前还**不能**闭环。

这轮先补的是：

- agent 不启动旧图石前提下的本机预检；
- 机器侧已确认事实；
- 仍必须由用户现场手工确认的最小动作清单。

当前最重要的结论不是
“已经找到真实根因”，
而是：

```text
本机存在 SafeNet Sentinel 数据目录，
但当前没有看到典型本地 Sentinel / HASP 许可服务或监听端口在工作。
```

以及：

```text
旧图石的“请检查网线是否接好”是宽兜底许可 / 环境初始化失败文案，
不能只按“网络断了”理解。
```

这说明当前问题更像：

```text
许可环境未完整就绪
  或
本地许可服务未提供能力
  或
旧图石实际依赖网络许可 / 网络狗 / USB 狗，
但当前现场状态还没被手工确认
```

因此本轮不能把 `TODO-049` 写成 done。

## 本轮本机预检范围

本轮只做只读探测：

1. 服务名探测；
2. 本地典型许可端口探测；
3. 环境变量探测；
4. SafeNet / Sentinel 安装痕迹探测；
5. 网络连通基础状态探测。

本轮不自动再次启动旧图石，
不改 app 业务代码，
不实现真实接头线 / Others 几何算法，
不声明 AutoCAD L2 通过，
不进入 golden。

## 本轮确认到的机器侧事实

### 1. 典型本地许可服务未发现

直接检查：

```text
hasplms   -> not_installed
aksfridge -> not_installed
```

按关键字扫服务时，当前也没有看到
明确命名为 Sentinel / HASP / SuperDog 的服务。

### 2. 典型本地许可端口未监听

本轮检查：

```text
127.0.0.1:1947 -> False
127.0.0.1:475  -> False
127.0.0.1:6001 -> False
```

白话就是：

```text
本机当前没有表现出“本地许可管理器正在正常监听”的迹象。
```

### 3. 未发现相关环境变量

本轮筛：

```text
HASP / SENTINEL / LICENSE / HL_
```

结果：

```text
NO_MATCH
```

这说明当前至少没有看到
显式暴露在进程环境里的许可变量线索。

### 4. 存在 SafeNet Sentinel 数据目录

本轮确认：

```text
C:\ProgramData\SafeNet Sentinel                -> exists
C:\ProgramData\SafeNet Sentinel\Sentinel LDK  -> exists
```

并且该目录下已有本地数据子树，
最近时间戳为：

```text
2026-06-07 23:23:39
```

这说明本机不是完全没有 Sentinel 痕迹，
至少存在本地数据落地。

### 5. 常见安装目录未发现

本轮确认：

```text
C:\Program Files\SafeNet Sentinel                   -> False
C:\Program Files (x86)\SafeNet Sentinel             -> False
C:\Program Files\Common Files\SafeNet               -> False
C:\Program Files (x86)\Common Files\SafeNet         -> False
```

当前更像是：

```text
有本地数据目录，
但没有明显完整安装目录暴露在常见位置。
```

### 6. 本机基础网络是通的

本轮确认在线网卡：

```text
WLAN
vEthernet (FSE HostVnic)
vEthernet (Default Switch)
```

其中 `WLAN` 当前在线，
说明“机器完全断网”不是当前最直接的结论。

## 本轮追加的 IDA 静态补证

对应证据：

```text
E-IDA-031
```

当前从 `VisualTS.exe.i64` 可进一步确认：

```text
sub_14070C760
  -> 先做许可上下文 ready 检查
  -> ready 且对象映射成功才进入更深的许可处理
  -> 否则直接返回 fallback 路径
```

其中关键点是：

```text
sub_14070CBF0
  -> 取线程级许可上下文
sub_14070D618
  -> 上下文未 ready 时直接返回 0
sub_14070D178
  -> ready 标志为 0 或映射失败时直接返回 0
sub_14070C760
  -> 上述任一失败就返回 fallback
```

白话就是：

```text
旧图石弹“请检查网线是否接好”，
不等于它已经明确证明“网络许可握手失败”。

它也可能只是许可上下文没 ready、
许可对象没映射出来、
本地许可环境没初始化好。
```

另外，本轮复核这些字符串的 xref：

```text
HL_LICENSEDIR              -> sub_14031C11B
hlrus_license_file.alf    -> sub_14031C11B
hasp_enabled               -> sub_1401882A8
nethasptype                -> sub_14018B7D0
NETHASP_00112233445566zz   -> sub_140237E8A
```

当前更像是：

```text
它们属于 Sentinel / HASP / NetHASP 许可栈内部线索，
不是旧图石启动 wrapper 直接暴露出来的 app 级开关。
```

所以 `TODO-049` 的人工清单优先级应保持为：

1. 先确认当前到底是 USB 狗还是网络许可；
2. 再确认服务、内网 / VPN、宿主机可达性；
3. 如果仍失败，再追 `HL_LICENSEDIR / hlrus_license_file.alf` 这条文件链。

## 对 TODO-049 的意义

结合 `TODO-048 / E-IDA-030 / E-DEV-070`
已经闭合的静态证据，
现在可以把范围收窄成：

### 已经不是首要问题的方向

- 不是“完全没有 Sentinel 痕迹”；
- 不是“旧图石阻塞文案来源不明”；
- 不是“只要看到网线文案，就能直接判定是网络许可故障”；
- 不是“agent 还需要继续自动重试启动旧图石”。

### 仍必须人工确认的方向

1. 当前是不是 USB 狗模式；
2. 当前是不是网络许可 / 网络狗模式；
3. 如果是网络许可，当前是否已连到正确内网 / VPN；
4. 当前旧许可本身是否已经过期；
5. 当前用户手工处理前置条件后，旧图石能否进入主界面。

## 用户最小动作清单

当前建议用户按这个最小闭环执行：

1. 确认是否有 USB 加密狗，并确认当前已插好；
2. 如果是网络许可，确认是否已连到正确内网 / VPN；
3. 如果知道 license server / 网络狗宿主机，确认它当前可达；
4. 用户自己手工启动旧图石一次；
5. 把结果按下面两类之一回填。

### A. 仍然阻塞

```text
USB狗：有 / 没有 / 不确定
网络许可：是 / 否 / 不确定
当前是否已连内网/VPN：是 / 否
弹框标题：
弹框完整文本：
是否还是启动前就弹：
截图：有 / 无
```

### B. 已进入主界面

```text
已进入主界面：是
打开到哪一步：
是否能打开 SFL：
主界面截图：有 / 无
```

## 当前仍未闭合

- 本机真实失败码是什么；
- 当前实际是 USB 狗、本地许可还是网络许可；
- `SafeNet Sentinel` 本地数据目录是否足以证明本地许可已可用；
- `HL_LICENSEDIR / hlrus_license_file.alf` 在当前机器上是否真的参与了失败；
- 用户手工处理前置条件后，旧图石是否能稳定进入主界面；
- 进入主界面后，非空 `steeljoint-line / joints / Others` 运行样例能否采到。

## 边界声明

本记录只证明：

- agent 已完成一轮不启动旧图石的本机预检；
- 本机存在 SafeNet Sentinel 数据目录；
- 本机当前没有看到典型本地许可服务或监听端口在工作；
- 启动期 fallback 文案是宽兜底许可初始化失败提示，不是纯网络专用报错；
- `TODO-049` 已经收敛到必须由用户现场确认的最小动作集。

本记录不证明：

- 当前真实根因已经动态确认；
- 旧图石已经可以正常启动；
- 真实接头线算法已闭合；
- 真实 Others 几何算法已闭合；
- AutoCAD L2 已通过；
- 旧插件接受新包；
- 完整工程图已完成；
- golden 已采集。
