# IDA 命令证据

## 说明

本文件记录 IDA / 二进制确认的旧命令字符串、命令表项和入口函数。

命令表结构当前判断为：

```text
命令字符串地址
  -> 入口函数
  -> 可用性/状态函数
```

表项通常以 3 个 qword 为一组。

## 旧英文命令字符串

IDA `find_regex` 已命中以下旧命令字符串：

| 旧命令 | 字符串地址 | 初步语义 |
|---|---:|---|
| `barjointnew` | `0x140768480` | 新建接头/搭接 |
| `barjointclear` | `0x140768490` | 清除接头 |
| `barjointmove` | `0x1407684A0` | 移动接头 |
| `barjointrev` | `0x1407684B0` | 接头反向 |
| `barmove` | `0x140768828` | 钢筋移动 |
| `steelhide` | `0x140768870` | 隐藏/显示层相关 |
| `steeldis` | `0x140768880` | 显示钢筋 |
| `steeldiam` | `0x1407688B0` | 钢筋属性/直径 |
| `sgroupbararc` | `0x1407689F8` | 弧形钢筋组 |
| `sgroupbarline` | `0x140768A08` | 线性钢筋组 |
| `sbararc2circle` | `0x140768D00` | 弧钢筋转圆/圆处理 |
| `srev_steelbar` | `0x140768DA8` | 钢筋反向 |
| `sexportbar` | `0x140768DE0` | 导出钢筋 |
| `checkoverlapsteel` | `0x140768E00` | 钢筋碰撞/重叠检查 |
| `checkmindststeel` | `0x140768E18` | 最小钢筋间距 |
| `checkgmindststeel` | `0x140768E30` | 组内/箍筋最小间距 |
| `rebars` | `0x140768E44` | 批量钢筋入口 |
| `rebar` | `0x140768E4C` | 普通钢筋入口 |
| `rebarz` | `0x140768ECC` | 语义待确认 |
| `rebarpost` | `0x140768ED8` | 后处理钢筋入口，语义待确认 |
| `ysteelout` | `0x140769008` | 钢筋输出，语义待确认 |
| `refl_createSteel` | `0x1407692A0` | 参考对象创建钢筋 |
| `modify_refl_Steel` | `0x1407692B8` | 修改参考驱动钢筋 |

## 已解析命令表项

| 旧命令 | 表项地址 | 入口函数 | 状态/可用性函数 | 新系统建议命令 |
|---|---:|---:|---:|---|
| `sgroupbararc` | `0x14095B428` | `sub_1404DE110` | `sub_14054AE90` | `Rebar.Create.ArcGroup` |
| `sgroupbarline` | `0x14095B440` | `sub_1404DE720` | `sub_14054B410` | `Rebar.Create.LineGroup` |
| `sexportbar` | `0x14095B638` | `sub_1404D1540` | `sub_14045A020` | `Rebar.Export.SteelStepOrLegacy` |
| `barmove` | `0x14095B230` | `sub_1404D5040` | `0` | `Rebar.Edit.Move` |
| `steelhide` | `0x14095B4E8` | `sub_1404DA2C0` | `0` | `Rebar.View.Hide` |
| `steeldis` | `0x14095B500` | `sub_1404D50F0` | `0` | `Rebar.View.Show` |
| `steeldiam` | `0x14095B548` | `sub_140538DB0` | `0` | `Rebar.Edit.Properties` |
| `checkoverlapsteel` | `0x14095BF68` | `sub_14059BB80` | `0` | `Rebar.Check.Overlap` |
| `checkmindststeel` | `0x14095BF80` | `sub_14059BB00` | `0` | `Rebar.Check.MinDistance` |
| `checkgmindststeel` | `0x14095BF98` | `sub_14059BA80` | `0` | `Rebar.Check.GroupMinDistance` |
| `rebars` | `0x14095AD20` | `sub_1405ADF00` | `0` | `Rebar.Create.BatchFromSelection` |
| `rebar` | `0x14095AAE0` | `sub_14055C1F0` | `sub_14054AE20` | `Rebar.Create.FromSelection` |
| `rebarz` | `0x14095AC18` | `sub_14055C3E0` | `sub_14054AE20` | `Rebar.Create.ZOrPostGroup` |
| `rebarpost` | `0x14095AC30` | `sub_14055C310` | `sub_14054AE20` | `Rebar.Create.PostProcess` |
| `ysteelout` | `0x14095B968` | `sub_140600890` | `sub_14054AC70` | `Rebar.Export.YSteelOut` |
| `refl_createSteel` | `0x14095BC38` | `sub_14055B540` | `0` | `Rebar.Create.FromReference` |
| `modify_refl_Steel` | `0x14095BC50` | `sub_14055C110` | `0` | `Rebar.Edit.ReferenceDriven` |

## `sgroupbarline` 初步反编译

入口：

```text
sgroupbarline -> sub_1404DE720
```

当前 IDA MCP 初步结论：

- 函数要求当前选择集数量为 1。
- 先校验选中对象类型。
- 继续校验选中对象内部结构，要求子项数量不少于 2。
- 从对象结构中按奇数索引抽取一组实体，放入临时 `ENTITY_LIST`。
- 读取两个端点对象的坐标，计算四种端点距离。
- 取最小距离和对应端点距离。
- 最后调用公共生成函数：

```text
sub_1404D10C0(entityList, v6, v4, minDistance, selectedDistance, flag)
```

可确认的几何含义：

- 该入口不像简单 Dialog 弹窗，而是基于当前选择对象生成线性钢筋组。
- 计算逻辑依赖 ACIS `ENTITY_LIST` 和 `SPAposition`。
- `sub_1404D10C0` 很可能是线/弧组共同的钢筋组创建核心。

仍未确认：

- `v4`、`v6` 的业务对象名。
- `flag` 的业务意义。
- UI 上它具体对应 `线配筋` 的哪种选择流程。

## `sgroupbararc` 初步反编译

入口：

```text
sgroupbararc -> sub_1404DE110
```

当前 IDA MCP 初步结论：

- 函数要求当前选择集数量为 1。
- 校验选中对象类型。
- 校验选中对象内部结构，要求元素数量不少于 3。
- 从链式实体关系中收集一组实体到临时 `ENTITY_LIST`。
- 调用 ACIS `api_entity_entity_distance` 计算两个实体之间的距离。
- 默认距离参数初始值为 `0.1`。
- 最后调用公共生成函数：

```text
sub_1404D10C0(entityList, v4, v6, distance, 0.8, 1)
```

可确认的几何含义：

- 该入口处理的是弧形/环形一类钢筋组，不是普通面配筋窗口。
- 它和 `sgroupbarline` 共用 `sub_1404D10C0`。
- `0.8` 可能是默认比例/容差/偏移参数，语义未确认。

仍未确认：

- 它在 UI 上具体对应 `扇形筋`、`同心圆`，还是二者共用的一条底层入口。
- `sub_1405F17C0` / `sub_1405F25F0` 校验的对象类型业务名。
- `sub_1404D10C0` 生成对象的字段含义。

## 线/弧钢筋组公共生成链

入口：

```text
sgroupbarline -> sub_1404DE720 -> sub_1404D10C0
sgroupbararc  -> sub_1404DE110 -> sub_1404D10C0
```

`sub_1404D10C0` 原型：

```text
sub_1404D10C0(ENTITY_LIST *entityList, __int64 objA, __int64 objB, double distanceA, double distanceB, char flag)
```

当前 IDA MCP 初步结论：

- 该函数会校验 `objA`、`objB`、实体列表数量和距离下限。
- 距离小于 `0.002` 时直接失败。
- 会格式化距离参数。
- 会进入 ACIS API block：

```text
api_bb_begin(1)
ACISExceptionCheck("API")
```

- 会复制 `ENTITY_LIST` 并调用：

```text
sub_140451730(*(_QWORD *)(objA + 96), copiedEntityList, &createdObject)
```

- 如果生成成功，会调用：

```text
sub_1405D5670(objB, createdObjectPayload, objA)
sub_1405C7260(objB)
sub_1405E49D0(createdObjectPayload)
```

其中 `sub_140451730` 作用：

- 复制/重建传入实体列表。
- 将实体挂接到当前模型/对象映射。
- 返回新生成对象。

其中 `sub_1405D5670` 作用：

- IDA 反编译命中源码路径：

```text
e:\tushi3d\dam\class\seg_steelbargroup.cpp
```

- 这说明该函数属于旧图石 `seg_steelbargroup` 分段钢筋组逻辑。
- 函数内部调用 ACIS：

```text
api_edge
api_entity_entity_distance
api_split_curve
api_curve_spline
```

- 函数包含边长判断、曲线分割、端部裁剪、样条曲线生成。
- 常量和规则片段包括：
  - 边长阈值 `0.01`。
  - 端点距离阈值 `0.1`。
  - 曲线裁剪步长 `-0.03`。
  - 采样数量约为 `EDGE::length * 50`，且至少 `5`。

工程含义：

```text
线配筋 / 弧形组不是简单画几根线。
旧图石在 seg_steelbargroup.cpp 中做曲线拆分、端部处理和分段组生成。
新系统要把这类逻辑作为钢筋业务层复刻，OCCT 只替代 ACIS 的曲线/距离/分割能力。
```

后续应继续追：

- `sub_1405D5670` 中 `a1/a2/a3` 的业务对象名。
- `sub_1405BD0C0` 是否把生成边写入 steelbar/segment。
- `sub_1405C7260` 是否刷新钢筋组。
- `sub_1405E49D0` 是否刷新显示或保存 dirty。
- `seg_steelbargroup` 与 SFL `seg_steelbargroup` 字段的对应关系。

## 检查命令闭合证据

已确认：

```text
checkoverlapsteel -> sub_14059BB80 -> Dialog #449 钢筋碰撞检查
checkmindststeel  -> sub_14059BB00 -> Dialog #450 平行钢筋最小距离
checkgmindststeel -> sub_14059BA80 -> Dialog #450 平行钢筋最小距离
```

`Dialog #449` 字段：

- 集合1。
- 集合2。
- 碰撞结果。
- 执行。
- 关闭。

`Dialog #450` 字段：

- 钢筋距离。
- 钢筋1。
- 钢筋2。
- 单位 mm。

## 导出命令证据

Evidence ID：

- `E-IDA-014`

`sexportbar` 表项：

```text
0x14095B638: 0x140768DE0 -> "sexportbar"
0x14095B640: 0x1404D1540 -> 入口函数
0x14095B648: 0x14045A020 -> 状态/可用性函数
```

`ysteelout` 表项：

```text
0x14095B968: 0x140769008 -> "ysteelout"
0x14095B970: 0x140600890 -> 入口函数
0x14095B978: 0x14054AC70 -> 状态/可用性函数
```

旧导出字符串证据含：

```text
Rebar:[Count%d]
@Group:[Code%d],[Diameter%d],[Grade%d],[Usage%d],[Segs%d],[Counts%d]
SteelBar:[Seg%d]
$L(%g,%g,%g;%g,%g,%g)
```

运行补证：

- `E-RUN-002`：用户截图确认 `输出钢筋` 位于 `开始 / 文件` 分组。
- 用户运行确认旧图石 `输出钢筋` 可导出钢筋 `.stp`。
- 当前只能确认“旧图石存在钢筋 STP 导出能力”，尚未确认 `.stp` 是 `sexportbar` 还是 `ysteelout` 触发。
- 后续需要保留导出样本，并用 OCCT STEP 导入统计 roots / solids / faces / edges / curves。
- `Rebar/Group/SteelBar` 文本结构和 `.stp` 导出可能是同一入口的不同格式选项，也可能是不同命令链；未闭合前不得合并成确定结论。

## 旧钢筋对象保存字段证据

Evidence ID：

- `E-IDA-015`

本轮用 IDA MCP 追了旧对象字符串和保存函数，确认以下旧图石对象写出骨架。

### `steelData`

字符串和源码路径：

```text
steelData
e:\tushi3d\dam\class\steeldata.cpp
```

关键函数：

```text
sub_14045CCF0
```

已确认写出：

- `write_id_level("steelData", 2, 0)`
- `write_string(*(a1 + 64))`
- `a1 + 72/76/80` 三个 int
- `a1 + 84..120` 十个数值字段，版本 `<831` 写 int，`>=831` 写 real
- `a1 + 124..140` 五个 int
- `a1 + 144/148` 两个 logical
- 版本 `>=838` 时按 flag 追加 `a1 + 152/156`
- 版本 `>=841` 时追加 `a1 + 160/164`

结论：

```text
steelData 保存字段骨架已确认。
字段业务名未闭合，不能直接命名成材料/级别/弯钩等确定语义。
```

### `steelData` 读取函数补证

关键函数：

```text
sub_14045C940
```

已确认读入：

- `read_string` 后写入 `a1 + 64`。
- `a1 + 72/76/80` 三个 int。
- `a1 + 84..120` 十个数值字段，版本 `<831` 读 int 转 float，`>=831` 读 real。
- `a1 + 124..140` 五个 int。
- `a1 + 144/148` 两个 logical。
- 版本 `>=838` 时按 flag 读取 `a1 + 152/156`。
- 版本 `>=841` 时读取 `a1 + 160/164`。

结论：

```text
steelData 的 save/read 顺序成对确认。
新设计文件格式可以按 legacyObject.raw 保存 typed fields。
字段业务名仍未闭合，不能直接提升为确定业务字段。
```

### `steelbar`

字符串和源码路径：

```text
steelbar
e:\tushi3d\dam\class\steelbar.cpp
```

关键函数：

```text
sub_1405E1C10
```

已确认写出：

- `write_id_level("steelbar", 2, 0)`
- `write_string(*(a1 + 64))`
- `a1 + 72/80/88` 三个对象引用
- `a1 + 96/100` 两个 int

结论：

```text
steelbar 保存骨架很短，关键是三个引用的业务含义。
当前不能直接确认它们分别对应 group / segment / style。
```

补充：

```text
sub_1405E0EA0
```

该函数创建 `DEBUG_LIST("steelbar")`，并引用源码路径：

```text
e:\tushi3d\dam\class\steelbar.cpp
```

它是类注册 / 调试列表证据，不是字段读取函数。

### `steelbar` 读取函数补证

关键函数：

```text
sub_1405E1AB0
```

包装入口：

```text
sub_1405DF3F0
```

包装入口行为：

- `ACIS_OBJECT::operator new(136, ..., "e:\tushi3d\dam\class\steelbar.cpp")`
- 调用 `sub_1405DB340(v0, 0)` 初始化对象。
- 调用 `sub_1405E1AB0(v1)` 读取字段。

已确认读入：

- `read_string` 后写入 `a1 + 64`。
- `a1 + 72/80/88` 三个 `read_ptr`。
- `a1 + 96/100` 两个 `read_int`。
- 读完后旧代码把 `a1 + 120 = 0`、`a1 + 104 = 0`。

结论：

```text
steelbar 的 save/read 顺序成对确认。
持久化 raw 字段可按 +64/+72/+80/+88/+96/+100 保存。
但 +72/+80/+88/+96/+100 的业务名仍未闭合。
```

### `steelbargroup`

字符串和源码路径：

```text
steelbargroup
e:\tushi3d\dam\class\steelbargroup.cpp
```

关键函数：

```text
sub_1405F2100
```

已确认写出：

- `write_id_level("steelbargroup", 2, 0)`
- `write_string(*(a1 + 72))`
- `a1 + 80/88/96/104/112` 五个对象引用
- `a1 + 120/128/136` 等基础 int / 版本化字段
- `a1 + 144` vector
- `a1 + 168/192` 两个 position
- `a1 + 224/240/248/256/280..320` 多个 real
- `a1 + 364/368/372/380` logical / flag 字段
- `a1 + 328/332/336/340/344/348/352/356/360` 一组 int 字段
- 版本 `>=842` 写 `a1 + 384`
- 版本 `==805` 写 `a1 + 388`
- 名称包含 `Varies_ShangH_LZ` 时写额外 logical、两个 vector、一个 int 和两个字符串列表

结论：

```text
steelbargroup 是大字段对象。
保存骨架已确认，但大多数字段业务名还不能确定。
新设计文件格式应先保存 typed legacy fields，再逐步提升为业务字段。
```

### `steelbargroup` 读取函数补证

关键函数：

```text
sub_1405F1A90
```

已确认读入：

- `read_string` 后写入 `a1 + 72`。
- `a1 + 80/88/96/104/112` 五个 `read_ptr`。
- `a1 + 120` int。
- 版本 `<807` 从 real 米制读入并换算到 `a1 + 128`，否则读 int。
- `a1 + 136` int。
- `a1 + 224/240` real。
- `a1 + 168/192` 两个 position。
- `a1 + 144` vector，读入后 normalise。
- `a1 + 220/132` 两个 int。
- `a1 + 248/256/280..320` 多个 real。
- `a1 + 364/368/372/380` logical / version branch。
- `a1 + 328/332/336/340/344/348/352/356/360` 编码型 int 字段。
- 版本 `>=842` 读 `a1 + 384`。
- 版本 `==805` 读 `a1 + 388`。
- 名称包含 `Varies_ShangH_LZ` 时读额外 logical、两个 unit_vector、`a1 + 216` 和两个字符串列表。

特别规则：

- `a1 + 328` 读入值 `>2` 时，旧代码设置 `a1 + 340 = 1`，并把 `a1 + 328` 减 `3`。
- `a1 + 332` 读入值 `>2` 时，旧代码设置 `a1 + 344 = 1`，并把 `a1 + 332` 减 `3`。
- 版本 `>=838` 时，上述两个分支还会读取 `a1 + 348/352`。
- 版本 `>=839` 读取 `a1 + 356/360`。

结论：

```text
steelbargroup 的 save/read 顺序成对确认。
但 +328/+332/+340/+344/+348/+352 等仍只能写成编码型 legacy raw 字段。
它们很可能和编号或首末增减根数有关，需运行属性窗口或保存前后差异闭合。
```

### `seg_steelbargroup` 保存函数补证

关键函数：

```text
sub_1405DA720
```

已确认写出：

- `write_id_level("seg_steelbargroup", 2, 0)`。
- `a1 + 68` int。
- `a1 + 72` string。
- `a1 + 80` ptr。
- `a1 + 96` ptr。
- `a1 + 88` ptr。
- `a1 + 64` int。

结论：

```text
seg_steelbargroup 保存顺序不是按偏移升序。
新设计文件格式必须记录 legacyWriteOrder：
+68 -> +72 -> +80 -> +96 -> +88 -> +64。
read 函数已闭合，但 ref80/ref96/ref88/+64 业务名未闭合，仍需运行保存差异和父子关系对照。
```

### `seg_steelbargroup` 读取函数补证

关键函数：

```text
sub_1405DA5D0
```

包装入口：

```text
sub_1405D5600
```

包装入口行为：

- `ACIS_OBJECT::operator new(112, ..., "e:\tushi3d\dam\class\seg_steelbargroup.cpp")`
- 调用 `sub_1405C6FB0(v0, 0)` 初始化对象。
- 调用 `sub_1405DA5D0(v1)` 读取字段。

已确认读入：

- `a1 + 68` int。
- `a1 + 72` string。
- `a1 + 80` ptr。
- `a1 + 96` ptr。
- `a1 + 88` ptr。
- `a1 + 64` int。

结论：

```text
seg_steelbargroup 的 save/read 顺序成对确认：
+68 -> +72 -> +80 -> +96 -> +88 -> +64。

持久化字段里 +72 是字符串，不是 OCCT/ACIS 几何边。
此前算法链中看到的 entity + 72 更可能属于运行期 payload 或其他对象上下文，
不能直接和 SFL 持久化字段 +72 合并解释。
ref80/ref96/ref88/+64 业务名仍未闭合，需要运行保存差异和父子关系对照。
```

### Detail `StbGeo`

字符串：

```text
StbGeo%d
```

关键函数：

```text
sub_14063B010
```

已确认方向：

- 该函数处理 `StbGeo%d` 写出。
- 字符串包含 `_x / _y / _z / Point%d%s`。
- 函数读取 `EDGE::geometry`、`ELLIPSE::centre`、`major_axis`、`radius_ratio` 等几何信息。

结论：

```text
Detail writer 的 StbGeo 是工程图派生几何。
新设计文件格式必须能从 RebarSegment + OCCT geometryRef 派生 StbGeo。
```

### Detail `StbGroup`

字符串：

```text
StbGroup%d
StbGroups
Std%d
```

关键函数：

```text
sub_14063E910
```

包装 / 上游函数：

```text
sub_14061F830
```

上游行为：

- 创建 `StbGroups` 节点。
- 遍历 `a1 + 528` 链表统计钢筋组数量。
- 写 `stbGroupCount`。
- 对每个组调用 `sub_14063E910`。

`sub_14063E910` 已确认写出：

- 创建 `StbGroup%d` 节点。
- 写 `rsdID`。
- 写 `groupID`。
- 写 `diameter`。
- 写 `diameter2`。
- 写 `interval`。
- 写 `barcount`。
- 写 `segcount`。
- 写 `stbNum`。
- 写 `stbNumAct`。
- 写 `stbLevel`。
- 写 `stbLayer`。
- 写 `stbProfile`。
- 写 `stbUse`。
- 写 `RangeLess180`。
- 写 `ComponentName`。
- 写 `PJSteelName`。
- 写 `SteelWay`。
- 写 `stbType`。
- 写 `stbOffsetInOut`。
- 创建 `Std%d` 子节点。
- 对每段调用 `sub_14063B010` 生成 `StbGeo%d`。
- 对 `Std%d` 写 `segCount`。
- 点状钢筋 `pointStb` 时调用 `sub_14063A860` 生成 `FaceEdge`。

关键分支：

- `rsdID` 不是单一 int 规则：
  - `+104 >= 10000` 时格式化为 `%d%c`。
  - 类型 `+376 == 11` 时格式化为 `Y%d`。
  - 类型 `+376 == 12` 时格式化为 `Z%d`。
  - 某些版本 / 类型分支会取字符串字段。
  - 否则写 int。
- `groupID` 来自旧对象 `+4` 的 `%d` 字符串。
- `barcount` 来自同组链上序号最小 / 最大值差值 `max - min + 1`。
- `segcount` 来自 `sub_1405EE850(*(group + 56))`。
- `stbNum` 来自当前 writer 上下文 `this + 40`。
- `stbNumAct` 来自 `sub_1405BD060(pattern + 43)`。
- `SteelWay` 由部位 / 名称字符串匹配后写 `LA / LASAME / XI / GU / OTHER`。
- `stbType` 按 `pattern[192] == 'C'` 区分 `pointStb` / `lineStb`。

结论：

```text
StbGroup 不是独立事实源。
它由旧钢筋组对象、组内链表、Detail writer 上下文和 StbGeo 子节点派生。

新系统 Detail writer 必须从 RebarModel 派生 StbGroup，
并保证 groupID / rsdID / stbNum / StbGeo.segID / StbTable.rsdID 同源。

rsdID、stbNumAct、SteelWay、stbType、stbOffsetInOut 已有 IDA 写出证据，
但业务生成规则仍需运行样本和 SFL 字段对照闭合。
```

### Detail `StbTable` / `MaterialTable`

字符串：

```text
StbTables
StbTable
count
StbRow%d
rsdID
diameter
stbLevel
length
stbNumSum
lenSum
MaterialTable
rowCount
MatRow%d
singleMass
massSum
Mass
Volume722
```

关键函数链：

```text
sub_140605B20
  -> sub_140602F90
```

辅助 helper：

```text
sub_1405259D0
  -> 按名字查找 XML / COM 子节点

sub_140525880
  -> 按名字匹配子字段并写入 VARIANT 值
```

已确认引用点：

| 字段 / 节点 | 引用点 | 行为 |
|---|---:|---|
| `StbTables` | `0x140603902` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `StbTable` | `0x14060394A` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `count` | `0x1406039A7` | 调用 `sub_140525880` 写值 |
| `StbRow%d` | `0x140603A29` | 使用循环序号格式化行节点名 |
| `rsdID` | `0x140603A7D` | 调用 `sub_140525880` 写值 |
| `diameter` | `0x140603B30`, `0x1406041EF` | 调用 `sub_140525880` 写值 |
| `stbLevel` | `0x140603BE2`, `0x14060413C`, `0x1406044B7` | 调用 `sub_140525880` 写值 |
| `length` | `0x140603C94` | 调用 `sub_140525880` 写值 |
| `stbNumSum` | `0x140603D46` | 调用 `sub_140525880` 写值 |
| `lenSum` | `0x140603DF8`, `0x1406042A1` | 调用 `sub_140525880` 写值 |
| `MaterialTable` | `0x140604008` | 构造字段名后调用 `sub_1405259D0` 查找节点 |
| `rowCount` | `0x140604068` | 调用 `sub_140525880` 写值 |
| `MatRow%d` | `0x1406040E8` | 使用循环序号格式化行节点名 |
| `singleMass` | `0x140604353` | 调用 `sub_140525880` 写值 |
| `massSum` | `0x140604405` | 调用 `sub_140525880` 写值 |
| `Mass` | `0x1406045E7` | 调用 `sub_140525880` 写值 |
| `Volume722` | `0x140604649` | 调用 `sub_140525880` 写值 |

补充说明：

- `sub_140605B20` 做权限 / 模型状态检查后调用 `sub_140602F90`。
- `MatRow%d` 另在 `sub_1406382D0` 也有引用，本轮只把 `sub_140602F90` 作为下料导出链证据。
- `diameter / stbLevel / lenSum` 在同一函数中存在多个引用点，说明可能覆盖 `StbRow` 与 `MatRow` 或条件分支，不能仅凭本轮地址判断唯一业务来源。

结论：

```text
StbTable / MaterialTable 字段不是样例包偶然字段。
它们在旧程序导出链中由 sub_140602F90 通过 helper 写出。

新系统 Detail writer 必须把 StbTable.rsdID、StbRow 长度 / 数量汇总、
MaterialTable 质量 / 体积汇总与 RebarModel 中的 RebarGroup / ScheduleRow 同源绑定。

字段值公式仍需继续通过 IDA 和旧图石运行样本闭合。
```

### 下料表内部命令绑定

证据 ID：

```text
E-IDA-020
```

已确认字符串：

```text
psexcel
```

字符串地址：

```text
0x140768FF8 -> "psexcel"
```

命令 ID 表：

```text
0x14095A1C8 -> 0x140768FF8 "psexcel"
0x14095A1D0 -> 0x8D1C
```

同表相邻项：

```text
0x14095A1D8 -> 0x140769000 "psallc"
0x14095A1E0 -> 0x8CD2
0x14095A1E8 -> 0x140769008 "ysteelout"
0x14095A1F0 -> 0x8CDB
```

handler 表：

```text
0x14095B698 -> 0x140768FF8 "psexcel"
0x14095B6A0 -> sub_140605B20
0x14095B6A8 -> 0
```

同表相邻项：

```text
0x14095B6B0 -> 0x140769000 "psallc"
0x14095B6B8 -> sub_140600AA0
0x14095B6C0 -> 0
```

handler 链：

```text
psexcel
  -> sub_140605B20
  -> sub_140602F90
  -> sub_140601D80
```

已确认含义：

- `psexcel` 在内部命令 ID 表中绑定 `0x8D1C`。
- `psexcel` 在内部 handler 表中绑定 `sub_140605B20`。
- `sub_140605B20` 做权限 / 模型状态检查后，调用 `sub_140602AE0` 查找模板，再调用 `sub_140602F90`。
- `sub_140602F90` 已由 `E-IDA-019` 确认写出 `StbTable / MaterialTable` 字段。
- `sub_140601D80` 引用 `钢筋下料表` 字符串，并属于 `excelcommand.cpp` 导出链。

本轮不能过度推断：

- 尚不能证明顶部 Ribbon 文本 `下料表` 一定直接绑定 `psexcel / 0x8D1C`。
- 尚不能证明 Dialog #427 一定由 `psexcel` 打开。
- 尚不能证明运行时点击 `下料表` 的输出目录、文件名和覆盖行为。

结论：

```text
内部命令绑定已确认：

psexcel / 0x8D1C / sub_140605B20

但 UI 顶部按钮、Dialog #427 和运行输出仍需旧图石运行确认。
```

### 下料表 Ribbon UI 绑定补证

证据 ID：

```text
E-IDA-021
```

关键函数：

```text
sub_1406F37B0
```

当前判断：

- 该函数构造 Codejock Ribbon。
- 反编译中出现 `CXTPRibbonBar`、`CXTPRibbonTab`、`CXTPRibbonGroup`、`CXTPControls::Add`。
- 它创建 `工程图` 页签。
- 它在该页签下创建 `输出` 分组。
- 它向 `输出` 分组加入命令 ID `36124`，也就是 `0x8D1C`。
- 同一函数还把 `36124 / 0x8D1C` 加入 QuickAccess 控件。

相关字符串：

```text
0x140796BBC -> 工程图
0x1407969D4 -> 输出
0x14077C328 -> 钢筋下料表
0x14077C32C -> 下料表
```

和 `E-IDA-020` 的关系：

```text
E-IDA-020:
  psexcel -> 0x8D1C -> sub_140605B20

E-IDA-021:
  工程图 / 输出 Ribbon 分组 -> 0x8D1C
```

已确认含义：

- `0x8D1C` 已不是孤立内部命令 ID。
- 它确实被旧程序 Ribbon 构造函数挂到 `工程图 / 输出` 区域。
- 结合 `E-IDA-020`，`工程图 / 输出` 下的该按钮高度可信地指向 `psexcel / sub_140605B20` 下料链。

本轮不能过度推断：

- 还不能单靠本证据证明按钮最终显示标题一定是 `下料表`，需要确认命令 caption / resource 绑定。
- 还不能证明 Dialog #427 一定由 `psexcel` 打开。
- 还不能证明运行时输出目录、文件名、覆盖策略或 Excel / Detail 包写出行为。

结论：

```text
下料表 UI 入口已经从“内部命令候选”推进到：

工程图 / 输出 Ribbon 分组 -> 0x8D1C -> psexcel -> sub_140605B20

但 Dialog #427 和运行输出仍需旧图石运行确认。
```

## 右键命令证据摘要

来自已有迁移资料 `ROOT_CONTEXT_MENU_COMMANDS.csv`：

| 旧菜单行为 | CommandId | 新系统目标 |
|---|---:|---|
| 激活/钝化 | `0x8CA0` | `RebarGroup.ToggleActive` |
| 钢筋组合并 | `0x891F` | `RebarGroup.MergeGroups` |
| 钢筋段连接 | `0x8924` | `RebarGroup.ConnectSegments` |
| 特性 | `0x8933` | `RebarGroup.OpenProperties` |
| 删除 | `0xE120` | `RebarGroup.Delete` |
| 增减根数 | `0x8CEB` | `RebarGroup.EditBarCount` |
| 修改间距 | `0x8CE6` | `RebarGroup.EditSpacing` |
| 复制 | `0x8CE7` | `RebarGroup.Copy` |
| 移动 | `0x8CEC` | `RebarGroup.Move` |
| 与线裁剪 | `0x8939` | `RebarGroup.TrimByLine` |
| 与面裁剪 | `0x8CE5` | `RebarGroup.TrimByFace` |
| 设为箍筋 | `0x8D4A` | `RebarGroup.SetAsStirrup` |

## 待继续分析

- `sub_1404DE110` 和 `sub_1404DE720` 已完成第一轮反编译摘要，公共生成链已追到 `sub_1404D10C0 -> sub_140451730 -> sub_1405D5670`。
- `sub_1405D5670` 已确认命中 `seg_steelbargroup.cpp`，但字段名和对象名仍需继续闭合。
- `rebarz` / `rebarpost` 业务含义未闭合。
- 顶部 Ribbon 按钮和英文命令并非一一对应，需要结合运行界面确认。
- `生成工程图` 顶部按钮的命令表入口仍需继续追。
