# M2-Drawing-018 Run 004

todoId = TODO-049
phase = M2-Drawing-018
evidenceId = E-RUN-004
decision = startup-prereq-closed-sample-collection-next
userActionRequired = true
algorithmImplemented = false
autocadL2 = not_run
oldRuntimeAutomaticallyLaunched = false

## 结论

用户本轮把 `TODO-049` 剩余的启动前置条件补齐到了可收尾状态：

1. 旧图石启动仍需要 USB 狗；
2. USB 狗插入后，旧图石可以进入主界面；
3. 旧图石可以打开 `SFL`；
4. 不需要内网；
5. 现场仍需要本地 HASP 安装条件；
6. 工作区 `HASPUserSetup` 已核查到安装包和运行文件，但当前机器目标目录
   `C:\Program Files (x86)\Common Files\Aladdin Shared\HASP\` 尚不存在。

这说明 `TODO-049` 可以结束，下一步应切回真正有开发价值的
`TODO-050 / 旧图石非空接头线 Others 运行样例采集 P0`。

## 当前收敛结果

### 旧图石侧

```text
legacyStartup.usbDongleRequired = true
legacyStartup.mainWindowReachableWithDongle = true
legacyStartup.sflOpenWithDongle = true
legacyStartup.intranetRequired = false
legacyStartup.localHaspInstallRequired = true
```

### 工作区 HASP 安装包目录

```text
workspaceHaspSetup.path = .\HASPUserSetup
workspaceHaspSetup.files =
  HASPUserSetup.exe
  hasplm.ini
  haspvlib_23520.dll
  驱动安装步骤.doc
workspaceHaspSetup.targetPathExists = false
```

### 文件名差异

```text
userStatement.dllName = haspvlib_23250.dll
workspaceActual.dllName = haspvlib_23520.dll
```

当前只记录差异，不把任一文件名擅自提升为
"旧图石唯一正确版本" 的确定结论。

### 新系统边界

```text
newSystem.usbDongleDependencyAllowed = false
newSystem.haspDependencyAllowed = false
newSystem.openSourceTarget = true
```

## 这次节点真正关闭了什么

现在已经可以明确：

```text
旧图石启动前置条件节点已闭合
启动问题不再是当前主阻塞
下一阶段该回到真实运行样例采集
```

但仍然不能声明：

```text
当前机器已经完成 HASP 安装
真实接头线算法已实现
真实 Others 几何算法已实现
AutoCAD L2 import 已通过
旧插件已接受新包
完整工程图已完成
golden 已采集
```

## Agent 边界

```text
oldRuntimeAutomaticallyLaunched = false
driverInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
algorithmImplemented = false
autocadL2 = not_run
xhigh = not_required_docs_only
```

## 下一步建议

```text
TODO-050
  -> 在旧图石已可进入主界面并打开 SFL 的前提下，
     采至少 1 个非空 steeljoint-line / Others 运行样例，
     记录最小操作、截图、输出文件 hash、目标 XML 节点内容和 stop point。
```
