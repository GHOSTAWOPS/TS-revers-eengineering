# M2-Drawing-024 接头 Handler 动作函数字段语义深追 P0 实现记录

## 结论

本轮执行 `TODO-055 / 接头 handler 动作函数字段语义深追 P0`。

本轮只读 IDA MCP 和既有文档，没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
groupjoint 对象自身：
  obj+80  -> 子接头对象链头。

子接头对象：
  child+72  -> 同组下一个子接头对象。
  child+88  -> 已生成接头几何 / 显示链头，clear / rebuild 会删除。
  child+96  -> 奇偶错位选择字段，影响 JointDistbet 修正。
  child+108 -> JointRuler / 接头周期，单位按旧代码以 mm 存储。
  child+112 -> 当前接头相位 / 位置，写入前会按周期归一化。
  child+116 -> reverse flag，控制反向遍历和接头生成方向。
  child+128 -> 低层 ACIS/HOOPS 生成实体链，删除时会 api_del_entity。

写回函数：
  sub_1405E1D50(child, value) -> backup 后写 child+108。
  sub_1405E1CC0(child, value) -> backup 后归一化并写 child+112。
  sub_1405E1D20(child, flag)  -> backup 后写 child+116。
```

复刻影响：

```text
后续新系统不能只做“视图刷新”。
这些接头动作包含三层语义：
1. 旧业务字段写回。
2. ENTITY::backup 触发的备份 / dirty / undo 语义。
3. 删除旧接头几何链并按字段重建 indexed polygon 的几何刷新语义。
```

## Control Contract

Primary Setpoint：

```text
在不自动启动旧图石、不改 app 代码的前提下，
用 IDA MCP 深追 TODO-055 指定动作函数的字段访问、写回规则和 stop point。
```

Acceptance：

```text
新增 E-IDA-037。
新增 E-DEV-077。
覆盖 sub_1405E9640 / sub_1405E7960 / sub_1405ED6C0 / sub_1405EBA30 /
     sub_140446AE0 / sub_1405CEB60 / sub_14045D580。
记录字段偏移、写回函数、清除 / 新建 / 移动 / 反向规则和剩余 stop point。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
TODO-055 done，下一步只切到 TODO-056，不进入算法实现。
```

Guardrail：

```text
不实现真实接头线 / Others 几何算法。
不声明 AutoCAD L2 通过。
不启动旧图石。
不安装 HASP。
不改 app 代码。
不进入 golden。
不把静态字段语义写成旧 UI caption 或完整运行行为。
```

## IDA MCP 会话

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
base = 0x140000000
hexrays_ready = true
strings_cache_size = 16320
```

## 本轮覆盖函数

```text
sub_1405E9640  -> groupjointnew 动作函数。
sub_1405E7960  -> groupjointclear / feat-goujian 间接 clear 共用函数。
sub_1405ED6C0  -> groupjointrev 动作函数。
sub_1405EBA30  -> groupjointmove 动作函数。
sub_140446AE0  -> featjointclear / goujianjointclear 的子对象 clear adapter。
sub_1405CEB60  -> segjointclear 动作函数。
sub_14045D580  -> segjointnew Dialog #428 初始化函数。
```

## groupjoint 动作函数

### sub_1405E9640 / groupjointnew

伪代码要点：

```text
if dword_140994AB8 < 1:
  return 0

if dword_140994AB8 < dword_14095D628:
  dword_14095D628 = dword_140994AB8 / 2

periodM = dword_140994AB8 / 1000.0
sub_1405E7960(groupObj)   // 先清旧接头几何链

for child = *(groupObj+80); child; child = *(child+72):
  if periodM < sub_1405DC870(child, 0, 0):
    sub_1405E1D50(child, dword_140994AB8)      // 写 child+108
    phase = dword_140994AB8
    if *(int*)(child+96) % 2 < 1:
      phase = dword_140994AB8 - dword_14095D628
    sub_1405E1CC0(child, phase)                // 写 child+112，归一化
    sub_1405E1D20(child, 0)                    // 写 child+116 = false
    sub_1405DB6C0(child)                       // 按字段重建接头几何
return 1
```

字段语义：

```text
child+80 不是 groupjoint 子链；groupjoint 的子链从 groupObj+80 开始。
child+72 是 groupjoint 子接头对象 next。
child+96 控制 JointDistbet 是否参与奇偶错位。
child+108 接 JointRuler。
child+112 接归一化相位 / 位置。
child+116 接 reverse flag。
```

### sub_1405E7960 / groupjointclear 共用函数

伪代码要点：

```text
for child = *(groupObj+80); child; child = *(child+72):
  sub_1405E1D20(child, 0)  // backup 后写 child+116 = false
  sub_1405DBE20(child)     // 删除 child+88 下的旧接头几何 / 显示链
return 1
```

复刻影响：

```text
clear 不是删除 groupjoint 对象本身。
它清的是 groupjoint 下每个子接头对象的 reverse flag 和已生成接头几何链。
```

### sub_1405ED6C0 / groupjointrev

伪代码要点：

```text
for child = *(groupObj+80); child; child = *(child+72):
  sub_1405E1D20(child, *(byte*)(child+116) == 0)
  sub_1405DB6C0(child)
return 1
```

复刻影响：

```text
rev 是逐子对象切换 child+116，然后重新生成接头几何。
```

### sub_1405EBA30 / groupjointmove

伪代码要点：

```text
child = *(groupObj+80)
if !child:
  return 0

while child:
  period = *(int*)(child+108)
  if period >= 1 and period/1000.0 < sub_1405DC870(child, 0, 0):
    phase = inputDistanceM * 1000.0 + 0.5
    if *(int*)(child+96) % 2 < 1:
      phase -= dword_14095D628
    sub_1405E1CC0(child, (int)phase)
    sub_1405E1D20(child, 0)
    sub_1405DB6C0(child)
  child = *(child+72)
return 1
```

handler `sub_1405F0430` 的入口补证：

```text
首个 groupjoint 对象：
  initialDistance = sub_1405DC6E0(*(groupObj+80)) / 1000.0
  sub_14058B8D0(..., &initialDistance)  // 旧输入窗口调整距离

随后对所有 groupjoint 对象调用：
  sub_1405EBA30(groupObj, initialDistance)
```

复刻影响：

```text
groupjointmove 的用户输入单位是米口径，写入字段时乘 1000 转成 int。
写 child+112 前同样应用 child+96 与 JointDistbet 的奇偶错位修正。
```

## 写回 helper 字段语义

### sub_1405E1D50

```text
ENTITY::backup(child)
*(int*)(child+108) = value
```

含义：

```text
写 JointRuler / 接头周期字段。
单位按旧代码：全局 dword_140994AB8 是 mm，几何比较时除以 1000.0。
```

### sub_1405E1CC0

```text
period = *(int*)(child+108)
phase = input

if abs(input) >= 1:
  if period > 1:
    while phase < 1:
      phase += period
    while phase > period:
      phase -= period
else:
  phase = period

ENTITY::backup(child)
*(int*)(child+112) = phase
```

含义：

```text
写归一化接头相位 / 位置。
输入小于 1 时回退为 period。
负数会通过加 period 拉回正区间。
大于 period 时按周期扣减。
```

### sub_1405E1D20

```text
ENTITY::backup(child)
*(byte*)(child+116) = flag
```

含义：

```text
写 reverse flag。
```

### sub_1405DC6E0

```text
period = *(int*)(child+108)
phase = *(uint32*)(child+112)
if period:
  return phase % period
return phase
```

含义：

```text
读取当前归一化相位 / 位置。
```

## 几何清理与重建函数

### sub_1405DBE20

```text
for geom = *(child+88); geom; geom = *(geom+88):
  sub_1405B9640(geom)
```

含义：

```text
删除 child+88 下的已生成接头几何 / 显示链。
```

### sub_1405B9640

关键行为：

```text
for node = *(obj+128); node; node = *(node+88):
  HA_Delete_Entity_Geometry(node)
  api_del_entity(*(node+64))
  backup(node); *(node+64) = 0
  backup(node); *(node+80) = 0
  backup(node); *(node+88) = 0
  api_del_entity(node)

backup(obj); *(obj+128) = 0
backup(obj); *(obj+112) = 0
```

含义：

```text
清理低层 ACIS/HOOPS 几何实体链，并重置 obj+128 / obj+112。
这说明旧 clear / rebuild 包含底层 entity 删除，不只是 UI 隐藏。
```

### sub_1405DB6C0

关键行为：

```text
sub_1405DBE20(child)       // 先清旧链
period = *(int*)(child+108)
if period < 1: stop
periodM = period / 1000.0
phaseM = sub_1405DC6E0(child) / 1000.0
if phaseM < 0.02:
  phaseM = periodM

if *(byte*)(child+116):
  从反向端开始遍历

遍历 child+88 几何链 / EDGE：
  EDGE::length(...)
  get_bounded_curve(...)
  按 phaseM / periodM 在曲线上取点
  direct_render_mesh_manager::end_indexed_polygon(...)
```

含义：

```text
这是按 child+108 / +112 / +116 重建接头显示 / 几何的核心函数。
本轮只确认字段驱动与重建入口，不声明完整曲线取点、接头线几何算法已复刻。
```

## featjoint / goujianjoint 清除路径

### sub_140446AE0

伪代码要点：

```text
for subObj = *(obj+192); subObj; subObj = *(subObj+88):
  sub_1405E7960(subObj)
return lastResult
```

含义：

```text
featjointclear / goujianjointclear 不是直接对选中对象调用 groupjoint clear。
它先从 obj+192 遍历子对象链，再对每个子对象调用 sub_1405E7960。
该路径说明 feat/goujian 对象内部挂着可复用 groupjoint-like 子结构。
```

handler 入口差异：

```text
featjointclear:
  selection item +13 -> feat object -> sub_140446AE0(featObj) -> view+456。

goujianjointclear:
  selection item +80 == 4
  item+120 child node chain
  node+104 -> action object -> sub_140446AE0(actionObj) -> view+456。
```

### featjointnew / sub_1405EF140 补证

```text
featjointnew 过滤 feat object 后，直接调用：
  direct_render_mesh_manager::end_indexed_polygon(featObj)
  view+456(featObj, 1)

它不经过 sub_140446AE0。
```

复刻影响：

```text
featjointnew 和 featjointclear 不是同一条内部动作链。
```

## segjoint 路径

### sub_1405CEB60 / segjointclear

伪代码要点：

```text
for item = *(segObj+80); item; item = *(item+96):
  sub_1405B9640(item)
return lastResult
```

含义：

```text
segjointclear 清理 segObj+80 下的段组接头子链。
该子链 next 字段是 +96，不是 groupjoint 子对象的 +72。
每个子节点通过 sub_1405B9640 删除底层几何实体链。
```

### sub_14045D580 / segjointnew Dialog #428 初始化

伪代码要点：

```text
CDialog::CDialog(dialog, 0x1AC, parent)   // 0x1AC = 428
*(dialog+320) = segObj
vtable = JoingSegDlg::vftable
*(int*)(dialog+308) = dword_140994AB8
*(int*)(dialog+304) = 0
*(int*)(dialog+312) = dword_140994AB8
```

含义：

```text
segjointnew 当前可确认是创建 JoingSegDlg / Dialog #428 参数窗口。
Dialog 保存 segObj 到 +320，并把 JointRuler 默认值写入 +308 / +312。
本轮不追 Dialog 确定按钮后的真实生成逻辑。
```

## ACIS / View Transaction 模式

多数组 / feat handler 入口共同包裹：

```text
sub_14054C720()
sub_1406ED3C0(...)
selection ENTITY_LIST -> filter -> local ENTITY_LIST
sub_14054C760(...)
sub_1406B8140(...)
set_global_error_info(0)
outcome / problems_list_prop / exception_save
api_bb_begin(1)
exception_save::begin(...)
ACISExceptionCheck("API")
action function
view vtable +456(view, obj, 1)
if outcome::ok: update_from_bb()
```

复刻影响：

```text
后续新系统接头动作也需要事务边界：
选择过滤 -> 业务字段写回 -> 几何清理/重建 -> 视图更新 -> dirty/undo。
OCCT 只能替代几何计算和显示能力，不能替代这些旧业务状态规则。
```

## Stop Point

已经闭合：

```text
TODO-055 指定 7 个函数的字段访问和写回规则。
groupjoint 子链头 / next / phase / period / reverse 字段。
feat/goujian clear 的 obj+192 子链 adapter。
segjoint clear 的 obj+80 / +96 子链。
segjointnew Dialog #428 初始化字段。
写回 helper 会调用 ENTITY::backup。
```

仍未闭合：

```text
sub_1405DB6C0 内部完整几何算法：曲线取点、端点处理、pattern 生成、end_indexed_polygon 实际输出形态。
JoingSegDlg Dialog #428 确定按钮后的 segjoint 创建链。
feat/goujian obj+192 子链 owning 结构名。
goujian item+120 / node+104 / node+128 owning 结构名。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
golden。
```

## 验证

最终验证写入：

```text
docs/phase1/app_build_reports/m2_drawing_024_run_001.md
docs/phase1/app_build_reports/m2_drawing_024_run_001.json
```

本轮 docs-only：

```text
xhighReview = not_required_docs_only
```

## 下一步

```text
TODO-056 / 接头重建几何核心 sub_1405DB6C0 静态深追 P0
```

建议下一轮仍只做 IDA / 静态证据：继续拆 `sub_1405DB6C0` 的曲线取点、反向遍历、`end_indexed_polygon` 输出语义和相关 helper，不进入真实 OCCT 接头线算法实现。
