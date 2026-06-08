# M2-Drawing-023 接头 Handler 业务对象筛选链静态分类 P0

## 结论

本轮执行 `TODO-054 / 接头 handler 业务对象筛选链静态分类 P0`。

本轮只读 IDA MCP 和既有文档，没有启动旧图石，没有安装 HASP，没有修改 `app` 业务代码，也没有实现真实接头线 / Others 几何算法。

核心结论：

```text
barjoint*   -> selection item +13 取业务对象，按 dword_140993E74 过滤。
groupjoint* -> selection item +13 取业务对象，按 dword_140993EC8 过滤。
segjoint*   -> selection item +13 取业务对象，按 dword_140993E1C 过滤。
featjoint*  -> selection item +13 取业务对象，按 dword_1409931D0 过滤。
goujianjoint* -> 直接筛 selection item，自身 +80 == 4，再遍历 item +120 链取 node +104 业务对象。
```

大白话说：

```text
旧图石不是所有接头命令都操作同一种钢筋对象。
bar / group / seg / feat 四类从选中项背后的业务对象走；
goujianjoint 走构件选择项自身，再遍历其内部链表。
后续复刻接头命令时，选择适配层不能只做一套“选中钢筋对象”的通用逻辑。
```

## 控制合同

Primary Setpoint：

```text
在 TODO-053 UI/context menu 静态 stop point 后，
继续用 IDA MCP 分类 barjoint / groupjoint / segjoint / featjoint / goujianjoint
handler 的选择对象来源、对象类型判断、参数入口、失败口径和可继续追踪字段。
```

Acceptance：

```text
新增 E-IDA-036。
新增 E-DEV-076。
至少覆盖 5 组 handler 的对象来源、类型 predicate、参数入口、空选择 / 失败口径。
形成 TODO-055 下一步，不进入算法实现。
默认 CTest、readiness gate、OCCT 泄漏检查通过。
docs-only 节点记录 xhighReview = not_required_docs_only。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP，不拷贝 DLL 到系统目录。
不改 app 业务代码。
不实现真实接头线 / Others 几何算法。
不声明旧 UI caption / 右键菜单项绑定已闭合。
不声明 AutoCAD L2 通过。
不进入 golden。
```

## IDA MCP 会话

```text
database = visualts_i64_todo051
input = C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\VisualTS.exe.i64
module = VisualTS.exe
auto_analysis_ready = true
hexrays_ready = true
strings_cache_size = 16320
```

## Handler 表

本轮复用并复核以下内部命令 handler：

```text
barjointnew       -> sub_1405DFAA0
barjointclear     -> sub_1405DF710
barjointmove      -> sub_1405DFEF0
barjointrev       -> sub_1405E02D0

groupjointnew     -> sub_1405F0060
groupjointclear   -> sub_1405EFCC0
groupjointrev     -> sub_1405F0850
groupjointmove    -> sub_1405F0430

featjointnew      -> sub_1405EF140
featjointclear    -> sub_1405EEDA0

segjointnew       -> sub_1405D94C0
segjointclear     -> sub_1405D9450

goujianjointnew   -> sub_1405EF8D0
goujianjointclear -> sub_1405EF510
```

## 共同入口

多数 handler 共同结构：

```text
sub_1406ED3C0(...) -> 当前 selection/context
*(ctx + 8)          -> ENTITY_LIST
ENTITY_LIST::init / next / count / operator[]
sub_1405C6820(item) -> 选择实体有效性过滤
sub_14054C720()     -> 上下文可用性检查，部分批量 handler 使用
sub_14054C760(...)  -> 视图 / ACIS 操作上下文
sub_1406B8140(...)  -> view/update/mark 准备
ACISExceptionCheck("API")
update_from_bb()
view vtable +456(view, obj, 1) -> 触发视图对象更新 / 标记
```

失败 / 空选择口径：

```text
ctx null              -> return 0
ctx + 8 ENTITY_LIST null -> return 0
过滤后对象数 < 1     -> return 0
view/context null     -> return 0
单选命令 count != 1  -> return 0
对象类型 predicate 失败 -> return 0 或跳过
```

## 类型 Predicate

四类 `selection item +13` 业务对象都走相同模式：

```text
obj && obj->vfunc(+24)(obj, 2) == 某个全局 type id
```

已确认：

```text
sub_1405E0E70(obj) -> obj type == dword_140993E74  // barjoint 对象
sub_1405F17C0(obj) -> obj type == dword_140993EC8  // groupjoint 对象
sub_1405DA020(obj) -> obj type == dword_140993E1C  // segjoint 对象
sub_14045A7F0(obj) -> obj type == dword_1409931D0  // featjoint 对象
sub_1405C6F90(item) -> *(uint32_t *)(item + 80)     // goujianjoint selection item 类型
```

## barjoint 分类

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405E0E70(obj)
```

参数 / 字段入口：

```text
dword_140994AB8 -> JointRuler / 定尺长度，单位 mm，handler 内常转成 /1000.0 米。
dword_14095D628 -> JointDistbet / 接头相间距离，参与交错起点。
obj + 96         -> 用奇偶控制初始接头位置偏移。
obj + 108        -> 接头周期 / 定尺长度字段，sub_1405E1D50 写入。
obj + 112        -> 当前接头位置字段，sub_1405E1CC0 写入并做周期归一化。
obj + 116        -> 反向 / reverse flag，sub_1405E1D20 写入。
obj + 88         -> 旧几何 / 显示链头，sub_1405DB6C0 / sub_1405DBE20 遍历。
```

handler 动作：

```text
barjointnew / sub_1405DFAA0
  要求 dword_140994AB8 >= 1。
  过滤所有 barjoint 对象。
  如果 JointRuler/1000 < sub_1405DC870(obj,0,0)：
    sub_1405E1D50(obj, dword_140994AB8)       -> 写 obj+108
    sub_1405E1CC0(obj, dword_140994AB8 或 dword_140994AB8-dword_14095D628) -> 写 obj+112
    sub_1405E1D20(obj, 0)                     -> 写 obj+116
    sub_1405DB6C0(obj)                        -> 重建 / 刷新接头显示链
    view+456(view,obj,1)

barjointclear / sub_1405DF710
  过滤所有 barjoint 对象。
  对每个对象：
    sub_1405E1D20(obj, 0)
    sub_1405DBE20(obj)

barjointmove / sub_1405DFEF0
  要求 selection count == 1。
  要求 sub_1405DC6C0(obj) >= 1。
  初值 sub_1405DC6E0(obj) / 1000.0。
  通过 sub_14058B8D0(dword_140779F08, dword_140779EF8, &distance) 让用户调整距离。
  用 obj+108 做周期归一化。
  调 sub_1405E1CC0(obj, distance_mm) + sub_1405DB6C0(obj) + view+456。

barjointrev / sub_1405E02D0
  要求 selection count == 1。
  要求 sub_1405DC6C0(obj) >= 1。
  读取 obj+116，取反后 sub_1405E1D20(obj, flag)，再 sub_1405DB6C0(obj)。
```

## groupjoint 分类

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405F17C0(obj)
```

handler 动作：

```text
groupjointnew / sub_1405F0060
  要求 dword_140994AB8 >= 1。
  过滤 groupjoint 对象列表。
  对每个对象调用 sub_1405E9640(obj) + view+456。

groupjointclear / sub_1405EFCC0
  过滤 groupjoint 对象列表。
  对每个对象调用 sub_1405E7960(obj) + view+456。

groupjointrev / sub_1405F0850
  过滤 groupjoint 对象列表。
  对每个对象调用 sub_1405ED6C0(obj) + view+456。

groupjointmove / sub_1405F0430
  过滤 groupjoint 对象列表。
  首个对象上从 *(obj + 80) 再取 sub_1405DC6E0(...) / 1000.0 作为输入初值。
  通过 sub_14058B8D0(...) 调整距离。
  对所有 groupjoint 对象调用 sub_1405EBA30(obj, distance) + view+456。
```

可继续追踪字段：

```text
obj + 80 在 groupjointmove 中指向可取 sub_1405DC6E0 的子对象 / 关联对象。
sub_1405E9640 / sub_1405E7960 / sub_1405ED6C0 / sub_1405EBA30 是 TODO-055 的动作语义深追重点。
```

## segjoint 分类

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_1405DA020(obj)
```

handler 动作：

```text
segjointnew / sub_1405D94C0
  只取 selection 第一个 item。
  类型通过后 operator new(0x148)。
  sub_14045D580(newDialog, obj, 0)。
  vtable +728(dialog, 428, 0)，打开 Dialog #428。
  CWnd::ShowWindow(dialog, 5)。

segjointclear / sub_1405D9450
  只取 selection 第一个 item。
  类型通过后 sub_1405CEB60(obj)。
```

边界：

```text
segjointnew 目前是参数窗口入口，不是立即生成接头线。
Dialog #428 title = 创建段组接头。
```

## featjoint 分类

对象来源：

```text
selection item -> *((QWORD*)item + 13)
```

类型过滤：

```text
sub_14045A7F0(obj)
```

handler 动作：

```text
featjointnew / sub_1405EF140
  要求 dword_140994AB8 >= 1。
  过滤 featjoint 对象列表。
  对每个对象调用 direct_render_mesh_manager::end_indexed_polygon(obj) + view+456。

featjointclear / sub_1405EEDA0
  过滤 featjoint 对象列表。
  对每个对象调用 sub_140446AE0(obj) + view+456。
```

边界：

```text
Hex-Rays 把对象显示为 direct_render_mesh_manager*，但当前只能确认调用名和动作链，
不能直接把 owning 结构名写死为最终业务类型名。
```

## goujianjoint 分类

对象来源和其它四类不同：

```text
直接使用 selection item。
不取 *((QWORD*)item + 13)。
```

选择过滤：

```text
sub_1405C6820(item)
sub_1405C6F90(item) == 4
sub_1405C6F90(item) = *(uint32_t *)(item + 80)
```

内部链：

```text
for (node = *(QWORD *)(item + 120); node; node = *(QWORD *)(node + 128)) {
    obj = *(QWORD *)(node + 104);
    ...
}
```

handler 动作：

```text
goujianjointnew / sub_1405EF8D0
  要求 dword_140994AB8 >= 1。
  过滤 selection item 自身 +80 == 4。
  遍历 item+120 链。
  对 node+104 对象调用 direct_render_mesh_manager::end_indexed_polygon(obj) + view+456。

goujianjointclear / sub_1405EF510
  过滤 selection item 自身 +80 == 4。
  遍历 item+120 链。
  对 node+104 对象调用 sub_140446AE0(obj) + view+456。
```

复刻影响：

```text
新系统的选择适配层需要支持“选中构件 item -> 展开内部对象链 -> 对每个子对象执行接头动作”的语义。
不能把 goujianjoint 简化成普通钢筋对象直接选择。
```

## 动作函数初步字段

本轮只追到可分类级别，字段完整命名留给 TODO-055。

已确认：

```text
sub_1405E1D50(obj, value)
  ENTITY::backup(obj)
  *(int *)(obj + 108) = value

sub_1405E1CC0(obj, value)
  读取 *(int *)(obj + 108)
  对输入 value 做 1..period 归一化
  ENTITY::backup(obj)
  *(int *)(obj + 112) = normalizedValue

sub_1405E1D20(obj, flag)
  ENTITY::backup(obj)
  *(byte *)(obj + 116) = flag

sub_1405DBE20(obj)
  for (p = *(QWORD *)(obj + 88); p; p = *(QWORD *)(p + 88))
    sub_1405B9640(p)

sub_1405DB6C0(obj)
  先 sub_1405DBE20(obj)
  再遍历 obj + 88 几何 / 显示链
  使用 EDGE::length / get_bounded_curve / distance_to_point / end_indexed_polygon 等 ACIS / 显示能力重建接头表现
```

架构含义：

```text
旧 VisualTS 业务对象保存接头规则字段。
ACIS / HOOPS 负责长度、曲线点、显示 mesh 等能力。
新系统中这些能力应继续由 LegacyGeometryAdapter / OCCT AIS 承接，业务层不能直接写 TopoDS / AIS。
```

## 本轮 Stop Point

已经闭合：

```text
五组 handler 的选择对象来源。
四个业务对象 predicate 的 type id 比较方式。
goujianjoint 的 selection item + 内部链表特殊路径。
barjoint 的主要接头字段 offset 108 / 112 / 116。
groupjointmove 的 obj+80 子对象线索。
segjointnew 到 Dialog #428 的参数入口。
```

仍未闭合：

```text
sub_1405E9640 / sub_1405E7960 / sub_1405ED6C0 / sub_1405EBA30 的完整字段语义。
sub_140446AE0 / direct_render_mesh_manager::end_indexed_polygon 在 feat/goujian 路径中的准确业务命名。
goujianjoint item+120 / node+104 / node+128 的 owning 结构名。
旧 UI caption / 右键菜单项绑定。
旧图石非空 steeljoint-line / Others 运行样例。
AutoCAD L2 接受度。
真实接头线 / Others 几何算法。
```

## 验证

最终验证写入：

```text
docs/phase1/app_build_reports/m2_drawing_023_run_001.md
docs/phase1/app_build_reports/m2_drawing_023_run_001.json
```

本轮是 docs-only / evidence-only：

```text
xhighReview = not_required_docs_only
```

## 下一步

下一步建议：

```text
TODO-055 / 接头 handler 动作函数字段语义深追 P0
```

只继续做 IDA / 静态证据，把以下动作函数的字段语义追清楚：

```text
sub_1405E9640
sub_1405E7960
sub_1405ED6C0
sub_1405EBA30
sub_140446AE0
sub_1405CEB60
sub_14045D580
```

仍然不进入真实接头线 / Others 算法实现。
