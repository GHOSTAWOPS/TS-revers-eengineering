# M2-Drawing-030 generated node+112 运行证据 intake gate 实现记录

## 结论

本轮没有闭合 `TODO-061`。

本轮完成的是：

```text
1. 给 TODO-061 补 runtime capture gate。
2. 把 capture README 和 capture_notes 模板与 gate 规则对齐。
3. 用 xhigh 只读 review 收一轮实现边界，再按 review 修正。
4. 证明这轮没有破坏 app 现有基线。
```

大白话说：

```text
旧图石真实 pane3 截图和 Excel 还没拿到。

但现在已经有一套更硬的 intake 入口：
  - 文件名必须是当前目录下的纯文件名
  - hash 必须是有效 SHA256
  - 图片 / Excel 不能只看后缀，至少要过最小文件头校验
  - Excel 导出场景要求补原始导出路径
  - 字段截图场景不再被“导出原始路径”误伤
```

这轮没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / `Others` 几何算法。

## Control Contract

Primary Setpoint：

```text
在 TODO-061 仍缺真实旧图石运行样例的情况下，
先把 runtime capture intake/gate 做到可复核、可拒绝伪工件、
且不会把截图场景误判为失败。
```

Acceptance：

```text
runtime_capture_gate 自测通过；
默认模板目录运行 gate 时按预期 fail；
xhigh 只读 review 完成且 Important 问题已收；
strict readiness gate 通过；
app 默认 CTest 通过；
实现记录 / build report / 追溯 / 46 / 99 / todo 同步更新。
```

Guardrail：

```text
不伪造 E-RUN-005。
不把 TODO-061 标记 done。
不自动安装 HASP。
不自动启动旧图石。
不改 app 业务代码。
不实现真实接头线 / Others 几何算法。
不声明 AutoCAD L2。
```

## 本轮实现

### 1. 新增 runtime capture gate

新增：

```text
tools/runtime_capture_gate/check_todo_061_capture.py
tools/runtime_capture_gate/test_check_todo_061_capture.py
```

当前 gate 覆盖：

```text
- capture_notes.md 存在
- hashes.txt 存在
- pane3 / schedule 文件名、hash、字段存在性
- 文件名必须是纯 basename
- pane3 文本必须是 焊接 / 绑扎 / 套筒连接
- 图片 / Excel 最小文件头校验
- Excel 场景要求导出原始路径
- 常见中文 truthy（是 / 有 / 可见 / 确认 / 已确认）
```

### 2. capture 模板同步收口

更新：

```text
docs/phase1/runtime_capture/todo_061_generated_node112/README.md
docs/phase1/runtime_capture/todo_061_generated_node112/capture_notes.md
```

修正点：

```text
- README 明确了 basename 约束
- README 明确 Excel 导出场景才强制要导出原始路径
- README 增补 --json-out 用法
- capture_notes 模板改回纯空字段，避免建议文案被 gate 误读为真实值
```

## xhigh 只读 review

子代理：

```text
Bernoulli
```

review 结论：

```text
Critical = 0
Important = 3
Minor = 2
decision = needs changes
```

本轮处理结果：

```text
1. 已修正截图场景被“导出原始路径”误伤的问题。
2. 已补最小文件头校验，不再只看扩展名。
3. 已收紧文件名为纯 basename。
4. 已放宽常见中文 truthy，避免 可见 / 已确认 / 有 这类输入被误判。
5. 已删除模板里的无效“文件格式”负担。
```

## 验证

```text
py -3 .\tools\runtime_capture_gate\test_check_todo_061_capture.py
  -> 11 tests, OK

py -3 .\tools\runtime_capture_gate\check_todo_061_capture.py
  -> decision = fail
  -> 这是预期行为：默认目录目前还是空模板，尚未放入真实旧图石运行证据

py -3 .\tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> 22 tests, OK

py -3 .\tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready
  -> 84/84 pass

ctest --test-dir .\app\build --output-on-failure
  -> 18/18 pass

git diff --check
  -> exit 0
  -> `11_需求证据追溯矩阵.md` / `todo.csv` 有 LF 归一化 warning
```

## 本轮不关闭的东西

```text
1. TODO-061 仍然没有真实 pane3 截图。
2. TODO-061 仍然没有真实 Excel 导出或字段截图样例。
3. E-RUN-005 仍未形成。
4. generated node+112 的最终业务名和连接方式完整字段仍是 gap。
```

## 下一步

```text
继续保持 TODO-061 = next。

下一步只接收用户现场旧图石真实输出：
  - pane3 截图
  - Excel 文件或字段截图
  - 对象编号 / 操作步骤 / 输出路径 / hash
```
