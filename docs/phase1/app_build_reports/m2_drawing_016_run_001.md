# M2-Drawing-016 Run 001

todoId = TODO-047
phase = M2-Drawing-016
decision = old-runtime-startup-stop-point-recorded
algorithmImplemented = false
autocadL2 = not_run

## 结论

本轮没有拿到旧图石非空接头运行样例。

已确认的 stop point 是：

- `VisualTS.exe` 当前可启动进程；
- 但启动后立即弹出标题为 `提示` 的阻塞框；
- 截图可见文本前缀为 `请检查网线是否...`；
- 阻塞发生在打开 `SFL` 之前。

因此本轮没有：

- 打开旧 `SFL`
- 执行新建接头 / 移动接头 / 接头反向 / 清除接头
- 导出新的 `DetailNN.stl`
- 收集到非空 `steeljoint-line / joints / Others / symbolcutIOS` 节点样例

本轮把 `TODO-047` 闭合为运行 stop point 证据节点，
不把它写成样例采集成功。

## 已确认事实

```text
oldRuntime.startupBlocked = true
oldRuntime.mainWindowTitle = 提示
oldRuntime.promptVisibleTextPrefix = 请检查网线是否...
oldRuntime.sflOpened = false
oldRuntime.detailExportGenerated = false
oldRuntime.nonEmptyJointOthersSampleCollected = false
algorithmImplemented = false
autocadL2 = not_run
```

## 运行工件

```text
VisualTS.exe
  SHA256 = 8233C1BE3702202D9AB5610700463B8BDC2DAD888993B76A6AC3C4344837C046

startup prompt screenshot
  docs/phase1/app_build_reports/m2_drawing_016_visualts_startup_prompt.png
  SHA256 = 56C44CEF1FB7211131A0239264AD4BE931C7BB487E9676D59488C3A346B3E101
```

## 影响判断

```text
TODO-046 已闭合参数绑定和 gate 止点，
但 TODO-047 证明当前旧图石运行入口本身仍被启动期提示阻塞。

所以后续不能直接进入：
  - 真实接头线算法实现
  - 真实 Others 几何算法实现
  - 旧运行样例等价声明
```

## 明确不声明

```text
旧图石已成功打开可创建接头的 SFL。
新建 / 移动 / 反向 / 清除接头路径已确认。
非空 steeljoint-line / joints / Others / symbolcutIOS 样例已拿到。
真实接头线算法已实现。
真实 Others 几何算法已实现。
AutoCAD L2 import 已通过。
旧插件接受新包。
完整工程图已完成。
golden 已采集。
```

## 待验证

```text
旧图石启动阻塞提示的完整文本、来源函数和配置 / 许可 / 网络前置条件。
旧图石可进入主界面后的接头菜单路径、参数窗口和状态提示。
旧图石非空 Detail 样例和截图。
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
建议执行 TODO-048：
  -> 旧图石启动阻塞提示链补证 P0
  -> 用 IDA MCP / 静态证据定位“提示 / 请检查网线是否...”阻塞链
  -> 形成后续用户手工解除前置条件清单
  -> 不自动再次启动旧图石
```
