# M2-Drawing-016 真实接头线 / Others 旧图石运行样例采集 P0 实现记录

## 结论

本轮尝试执行 `TODO-047 / M2-Drawing-016` 的旧图石运行样例采集。

结果不是拿到非空 `steeljoint-line / joints / Others / symbolcutIOS`，
而是先确认了一个新的运行 stop point：

- `VisualTS.exe` 当前可以启动进程，但启动后立即弹出标题为 `提示` 的阻塞框。
- 截图中可见文本前缀为 `请检查网线是否...`。
- 阻塞发生在打开旧 `SFL` 之前，因此本轮：
  - 未进入旧图石主工作界面；
  - 未打开任何 `SFL`；
  - 未执行新建接头 / 移动接头 / 接头反向 / 清除接头；
  - 未导出新的 `DetailNN.stl`；
  - 未修改任何模型或工程文件。

因此，`TODO-047` 以“运行样例 stop point 已记录”的形式闭环，
而不是以“拿到非空旧运行样例”的形式闭环。

本轮不实现真实接头线算法，不实现 `Others` 几何算法，
不声明 AutoCAD L2 通过，不声明旧插件接受新包，不进入 golden。

## 控制合同

Primary Setpoint：

```text
在 TODO-046 已补齐参数绑定和 gate 止点后，
优先尝试旧图石运行采样；
如果旧图石仍无法进入可操作状态，
必须把 stop point、外部前置条件和后续输入要求记录清楚。
```

Acceptance：

```text
形成 TODO-047 实现记录。
形成 m2_drawing_016_run_001.md / json。
把旧图石启动阻塞提示截图纳入 build report 工件目录。
更新 00 / 11 / 13 / 20 / 34 / 46 / 99 / todo.csv。
默认 CTest、strict readiness、OCCT/AIS 泄漏扫描和 diff --check 通过。
```

Guardrail：

```text
不得把本轮 stop point 写成旧图石运行样例已闭合。
不得把字段骨架、OCCT 能力或 AutoCAD L2 清单写成算法证据。
不得继续自动启动旧图石。
不得实现真实接头线 / Others 几何算法。
```

## 本轮实际运行探测

### 1. 最小启动探测

本轮只做了一次最小旧图石启动探测：

```text
启动文件：
  C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe

SHA256：
  8233C1BE3702202D9AB5610700463B8BDC2DAD888993B76A6AC3C4344837C046
```

探测到的进程信息：

```text
processId        = 59928
processName      = VisualTS
mainWindowTitle  = 提示
responding       = true
startTime        = 2026-06-07 23:23:39 +08:00
```

### 2. 阻塞提示

已归档截图：

```text
docs/phase1/app_build_reports/m2_drawing_016_visualts_startup_prompt.png
SHA256 = 56C44CEF1FB7211131A0239264AD4BE931C7BB487E9676D59488C3A346B3E101
```

截图可见事实：

```text
window title = 提示
visible text prefix = 请检查网线是否...
```

当前只能确认该提示和网络 / 许可 / 外部环境前置条件相关；
本轮没有继续点击或交互该窗口。

### 3. 为何本轮停止

`TODO-047` 的目标是收集旧图石运行样例。

但本轮 stop point 出现在：

```text
旧图石启动期
  -> 阻塞提示框
  -> 尚未进入主界面
  -> 尚未打开 SFL
  -> 尚未执行接头命令
```

所以本轮继续推进已经没有意义。

继续推进只会把“程序可启动”误写成“样例已采到”，
这是路线错误。

### 4. 用户协作边界

本轮运行探测之后，用户已明确表达：

```text
不要默认自动启动旧图石。
```

因此后续任何旧图石运行节点都必须满足其一：

1. 用户明确确认可以再次启动旧图石；
2. 用户自己手工操作旧图石，我们只做证据接收和整理。

## 本轮新增事实

- 当前本机 `VisualTS.exe` 会在启动期进入阻塞提示，而不是直接进入可操作界面。
- 当前旧图石运行样例采集的首要前置条件，已经从“找到合适 SFL 和操作路径”变成了“先解除启动期阻塞提示”。
- `TODO-047` 当前拿到的是运行 stop point，不是非空 `DetailNN.stl` 样例。

## 仍未闭合

- 阻塞提示的完整文本。
- 该提示背后的旧图石启动链、配置链、许可链或网络检查链。
- 旧 `SFL` 打开后的接头菜单路径、参数窗口、状态提示。
- 新建 / 移动 / 反向 / 清除接头后的非空 `DetailNN.stl` 样例。
- `steeljoint-line / joints / Others / symbolcutIOS` 的旧运行非空节点。
- AutoCAD 2020 + FDrawing 是否接受当前新包。

## 下一步建议

建议下一轮不要再直接做旧图石运行采样，
而是先做一个新的证据节点：

```text
TODO-048 / 旧图石启动阻塞提示链补证 P0
  -> 用 IDA MCP / 静态证据追启动期“提示 / 请检查网线是否...”阻塞链
  -> 形成用户手工解除前置条件清单
  -> 不自动启动旧图石
```

等启动阻塞证据补清、或用户手工确认可进入旧图石后，
再回到接头样例采集。

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

- 旧图石运行样例采集在启动期就遇到 stop point；
- 这个 stop point 已被记录、截图和归档；
- 当前不能再把 `TODO-047` 当成“非空样例尚未采但程序路径没问题”。

本记录不证明：

- 真实接头线算法已闭合；
- 真实 `Others` 几何算法已闭合；
- 旧图石运行样例已拿到；
- AutoCAD L2 通过；
- 旧插件接受新包；
- 完整工程图完成；
- golden 完成。
