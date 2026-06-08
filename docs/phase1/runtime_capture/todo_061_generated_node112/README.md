# TODO-061 Runtime Capture Intake

这个目录只用于收 `TODO-061 / generated node+112` 的真实旧图石运行证据。

## 需要的最小材料

至少放 2 类文件：

```text
1. 状态栏 pane3 截图
2. 下料表 Excel 文件或字段截图
```

推荐文件名：

```text
pane3_statusbar_01.png
barschedule_01.xls
barschedule_01.xlsx
barschedule_01_fields.png
capture_notes.md
```

## 必须记录的信息

请在 `capture_notes.md` 里补这些内容：

```text
旧图石版本：
SFL 文件名：
SFL hash：

选择对象：
  - 对象编号：
  - 是否确认有接头：建议填 `是/否`

状态栏截图：
  - 文件名：
  - pane3 文本：

下料表导出：
  - 文件名：
  - 导出原始路径：如果是 Excel 导出则必填；如果只有字段截图可留空
  - 是否能看到 焊头(个)：建议填 `是/否`
  - 是否能看到 单下料长(mm)：建议填 `是/否`

操作步骤：
  1.
  2.
  3.
```

## 最低验收口径

只要下面两条同时满足，这轮就能继续闭合：

```text
1. 有 1 张真实 pane3 截图
2. 有 1 份真实 Excel 文件或字段截图
```

## Hash

把文件放进来后，执行同目录下：

```powershell
.\collect_hashes.ps1
```

它会生成：

```text
hashes.txt
```

`hashes.txt` 里每行应是：

```text
文件名<TAB>SHA256
```

`文件名` 必须是当前目录下的纯文件名，不要写绝对路径或子目录。

再执行：

```powershell
py -3 ..\..\..\..\tools\runtime_capture_gate\check_todo_061_capture.py
```

如果想把结果落成 JSON：

```powershell
py -3 ..\..\..\..\tools\runtime_capture_gate\check_todo_061_capture.py `
  --json-out .\capture_gate_report.json
```

通过条件：

```text
decision = pass
```

后续直接把这个目录里的文件作为 `TODO-061` 输入即可。
