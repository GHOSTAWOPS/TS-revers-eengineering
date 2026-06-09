# TODO-077 Runtime Capture Intake

这个目录只用于收 `TODO-077 / 线配筋旧图石运行确认清单与工件门禁 P0`
后续真实旧图石运行证据。

这轮不是采 golden，也不是证明新系统已经 1:1。

这轮只把 TODO-076 没法靠 IDA 静态闭合的内容，
变成现场可执行的采样清单：

```text
1. 旧失败提示和状态栏 pane 口径。
2. 旧线配筋主参数窗口字段、默认值、单位和灰显状态。
3. 公共 Input_float / Dialog #383 是否会在真实流程出现。
4. 无选择、选错对象、有效对象三条点击路径。
5. 保存 / 导出前后是否产生可对照输出工件。
```

## 推荐文件

推荐至少放这些工件：

```text
capture_notes.md
hashes.txt

case_no_selection_before.png
case_no_selection_after.png
case_no_selection_prompt.png
case_no_selection_statusbar.png

case_wrong_object_selection.png
case_wrong_object_prompt.png
case_wrong_object_statusbar.png

case_valid_selection_before.png
case_valid_parameter_dialog.png
case_valid_dialog383.png
case_valid_statusbar_before.png
case_valid_statusbar_after.png
case_valid_model_tree_before.png
case_valid_model_tree_after.png
case_valid_after_apply.png
case_valid_after_cancel.png

line_group_save_before_listing.txt
line_group_save_after_listing.txt
line_group_save_hashes.txt
```

不是每个文件都必须同时存在。

但后续要闭合旧 UI / 状态栏缺口，
至少要覆盖：

```text
1. 无选择点击线配筋的结果。
2. 选错对象点击线配筋的结果。
3. 有效对象点击线配筋的参数窗口和提交 / 取消结果。
4. 状态栏截图，必须能看到 pane 区域。
5. 若出现 Dialog #383，必须记录标题、标签、默认值和按钮行为。
```

## 必须记录的信息

请在 `capture_notes.md` 里补这些内容：

```text
旧图石版本：
VisualTS.exe hash：
SFL 文件名：
SFL hash：
是否插入 USB 狗：
是否安装 HASP：

命令入口：
  - 页签 / 分组：
  - 按钮文字：
  - 是否从右键菜单触发：

无选择点击：
  - 点击前选择状态：
  - 是否弹窗：
  - 弹窗标题：
  - 弹窗正文：
  - 弹窗按钮：
  - 状态栏 pane 文本：
  - 截图文件名：

选错对象点击：
  - 选中对象类型：
  - 是否弹窗：
  - 弹窗标题：
  - 弹窗正文：
  - 弹窗按钮：
  - 状态栏 pane 文本：
  - 截图文件名：

有效对象点击：
  - 选中对象类型：
  - 对象编号 / 模型树位置：
  - 是否出现主参数窗口：
  - 主参数窗口标题：
  - 主参数窗口截图：
  - 是否出现 Dialog #383：
  - Dialog #383 标题：
  - Dialog #383 标签：
  - Dialog #383 默认值：
  - Dialog #383 单位：
  - Dialog #383 OK 结果：
  - Dialog #383 Cancel 结果：
  - 状态栏点击前文本：
  - 状态栏点击后文本：
  - 模型树点击前状态：
  - 模型树点击后状态：

主参数窗口字段：
  - 字段清单：
  - 默认值：
  - 单位：
  - 灰显 / 禁用字段：
  - 页签：
  - OK 行为：
  - Cancel 行为：
  - Apply 行为：

保存 / 输出工件：
  - 是否保存 SFL：
  - 保存前 listing 文件名：
  - 保存后 listing 文件名：
  - 保存前后 hash 文件名：
  - 是否导出 STP：
  - 是否生成 Detail 包：

操作步骤：
  1.
  2.
  3.
```

## Hash

把截图、listing 和输出工件放进来后，执行：

```powershell
.\collect_hashes.ps1
```

它会生成：

```text
hashes.txt
```

`hashes.txt` 每行应是：

```text
文件名<TAB>SHA256
```

文件名必须是当前目录下的纯文件名，不要写绝对路径。

## 最低验收口径

后续 `TODO-078` 要闭合真实运行回填时，最低需要：

```text
1. `capture_notes.md` 写清旧图石版本、SFL 文件名/hash、操作步骤。
2. 至少一张无选择点击结果截图，能看到弹窗或状态栏。
3. 至少一张选错对象点击结果截图，能看到弹窗或状态栏。
4. 至少一张有效对象点击后的主参数窗口截图。
5. 至少一张有效对象提交或取消后的状态栏 / 模型树截图。
6. 所有截图和输出工件都有 SHA256。
```

## 拒收规则

下面这些一律不能当有效旧图石运行证据：

```text
1. 只有菜单 / Ribbon 截图，没有点击结果。
2. 截图没有状态栏或弹窗上下文，却声称关闭状态栏 / 提示缺口。
3. Dialog #383 截图没有标题、标签、默认值或按钮区域。
4. 新 Qt6 app 的 P0 参数窗口截图，不能当旧 VisualTS 证据。
5. 没有 SFL 文件名 / hash / 操作步骤。
6. 只有启动许可提示或 HASP 阻塞提示。
7. 只有输出文件 hash，没有文件名、路径和操作步骤。
8. 没有点击前后对照，却声称模型树或输出发生变化。
9. 父目录旧工程截图或其他项目 Detail / STP，不能混入本轮证据。
10. 没有说明是否插 USB 狗 / HASP 状态，却把失败归因写死。
```

## 口径提醒

本目录里的材料只用于关闭运行确认缺口：

```text
GAP-UI-REB-001
GAP-IDA-002
```

它不能单独证明：

```text
完整线配筋算法已经 1:1。
完整 sub_1405D5670 已等价。
线配筋 golden 已完成。
面配筋、弧筋、接头、Excel 或 Detail 已完成。
```
