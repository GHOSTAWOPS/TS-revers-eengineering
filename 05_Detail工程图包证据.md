# Detail 工程图包证据

## 说明

图石旧工程图链路不是新程序直接写 DWG / DXF。

当前确认的交付路线是：

```text
图石主程序
  -> 生成 Detail.xml + DetailNN/DetailNN.stl
  -> AutoCAD 2020 图石旧插件导入
  -> 插件生成 / 编辑 / 打印 CAD 图纸
```

旧插件目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020
```

已确认文件：

- `FDrawing.arx`
- `FDrawingObj.dbx`
- `Detail.xml`
- `Detail01.stl`

## 关键结论

`Detail01.stl` 虽然后缀是 `.stl`，但内容不是网格 STL。

它实际是 XML：

```xml
<DrawingRoot>...</DrawingRoot>
```

因此新系统工程图包 writer 必须按 XML 写 `DetailNN.stl`，不能写成三角网格 STL。

## Detail.xml

样例根节点：

```xml
<StyleRoot CurrPos="2">
```

用途：

- 保存钢筋样式配置。
- 供旧 AutoCAD 插件读取样式。

样例字段：

- `Name`
- `dia`
- `type`
- `joint_type`
- `depth`
- `depth_start`
- `depth_end`
- `depth_to_start`
- `depth_to_end`
- `dis`
- `insert_start`
- `insert_end`
- `m_Anchor1`
- `m_Anchor2`
- `m_BLEND_ANGLE1`
- `m_BLEND_ANGLE2`
- `anchor_start`
- `anchor_end`
- `radio_insert_start`
- `radio_insert_end`
- `radio_layer_InOut`
- `extend1`
- `extend2`
- `m_AddNum_start`
- `m_AddNum_end`

### RUN-20260609-001 真实输出补充

`TODO-066 / TODO-067` 已确认本轮真实旧图石输出中的 `Detail.xml` 不是上面的非空样式表示例。

真实文件为：

```xml
<StyleRoot/>
```

边界：

```text
1. 该文件来自用户现场旧图石真实运行回填。
2. bytes = 14。
3. 用户补充多台电脑生成结果一致，均为 <StyleRoot/>。
4. 生成工程图和 CAD 导入时该文件修改时间不更新。
5. 当前更倾向把它视为固定空模板 / 占位文件。
6. 父目录/外部非空 Detail.xml 可能来自其他项目，不能混入本轮证据。
```

因此新系统后续要补 `GAP-DRAW-006`：

```text
DetailWriter 在旧图石包兼容模式下，应具备输出空 StyleRoot 的策略。
旧插件是否必须检查该文件存在仍需 AutoCAD L2 或运行确认。
```

## Detail01.stl / DrawingRoot

样例根节点：

```xml
<DrawingRoot>
```

主要结构：

```text
DrawingRoot
├── StbTables
│   ├── StbTable
│   │   ├── StbRow1
│   │   │   ├── StbSeg1
│   │   │   └── ...
│   │   └── ...
│   └── MaterialTable
├── HViewPorts
│   └── ViewPort
│       ├── PartDetailDrawing
│       │   ├── General-Info
│       │   ├── continue-line
│       │   ├── hidden-line
│       │   ├── central-line
│       │   ├── section-line
│       │   ├── hatch-line
│       │   ├── Others
│       │   └── steeljoint-line
│       └── StbDetailDrawing
│           └── StbGroups
│               ├── StbGroup1
│               │   ├── Std1
│               │   │   ├── StbGeo1
│               │   │   └── ...
│               │   └── FaceEdge
│               └── ...
```

## 下料表字段

`StbTable` / `StbRow` 样例字段：

- `count`
- `Volume1225`
- `rsdID`
- `ComponentName`
- `SteelWay`
- `diameter`
- `length`
- `segNum`
- `sameGrpNum`
- `stbNumSum`
- `lenSum`
- `stbLevel`
- `stbLayer`
- `stbProfile`
- `stbUse`
- `smallTable`
- `mirrorType`
- `mirrorSEFlag`

`RUN-20260609-001 / Detail01.stl` 真实 `StbTable` 还确认了表级属性：

```text
count
HeightValue0
HeightValueCount
Volume1225
NumCombineGoJians
SteelNetArea
GJTAOTNumber
GJTAOTVolue
LinkTop
LinkDown
DCGQSJ
HYLJJ
```

字段分布：

```text
Detail01.stl 有 StbTable / MaterialTable。
Detail02.stl 到 Detail04.stl 没有 StbTable / MaterialTable，
但仍保留 StbTables 容器。
```

当前 `DetailWriter` 差距：

```text
1. 当前每张 DetailNN.stl 都写 StbTable / MaterialTable。
2. 当前 StbTable 只写 count。
3. 当前 StbRow 缺 smallTable / mirrorType / mirrorSEFlag。
```

这形成 `GAP-DRAW-007`。

IDA 补证：

- `sub_140605B20 -> sub_140602F90` 命中 `StbTables / StbTable / StbRow%d` 导出链。
- `E-IDA-020 / E-IDA-043` 已纠偏：`35057 / 0x88F1 -> psexcel -> sub_140605B20` 才是下料表 / Excel 导出链。
- `E-IDA-042 / E-IDA-043` 已确认 `36124 / 0x8D1C -> psallc -> sub_140600AA0` 是 Detail 包主写出链；`0x8CD2` 实际属于 `ysteelout`，不再把它记成 `psallc`。
- `E-IDA-044 / E-DEV-086` 已把顶部中文 caption / resource 静态闭合：`36124 -> 生成工程图`，`35057 -> 下料表`；`36050 -> 输出钢筋` 作为同类 `描述\ncaption` 资源模式旁证。
- `Dialog #427` 当前只能保守记为 `OptionFactory` 属性页线索，不再把它当成下料表弹窗真值。
- `sub_140600AA0` 里的直接 `DoModal` 当前只收窄到 `output_uncut_steel / Dialog 0x57C / UnCutSteel.TXT` 后置报告链，不再写成前置生成工程图设置窗。
- `E-DEV-087` 已在 `docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/` 落地运行确认模板：要求记录 `Detail.xml / DetailNN.stl` 输出目录 listing / `SHA256` / 最后修改时间 / rerun 覆盖行为，以及下料表结果文件或字段截图、额外运行时弹窗截图；`UnCutSteel.TXT` 只作为可选旁证，不能单独证明 `生成工程图` 成功。
- 运行输出目录、覆盖策略和旧插件导入结果仍需确认。
- `sub_1405259D0` 用于按名字查找 XML / COM 子节点。
- `sub_140525880` 用于按名字匹配子字段并写入值。
- IDA 已确认 `count / rsdID / diameter / stbLevel / length / stbNumSum / lenSum` 走写值 helper。

注意：

- 样例中 `ComponentName / SteelWay / segNum / sameGrpNum` 等字段仍需继续追值来源。
- `StbRow.rsdID` 必须和 `StbGroup.rsdID` 同源，不能由下料表单独生成。

`StbSeg` 样例字段：

- `lenRange`
- `deltaLen`
- `shapeType`

线段：

```xml
<Line start_x="..." start_y="..." end_x="..." end_y="..."/>
```

圆弧：

```xml
<Arc start_x="..." start_y="..." middle_x="..." middle_y="..." end_x="..." end_y="..." start_rad="..." end_rad="..."/>
```

## 材料表字段

`MaterialTable` 样例字段：

- `rowCount`
- `Mass`
- `Volume722`
- `MassNum`

IDA 补证：

- `sub_140602F90` 命中 `MaterialTable / rowCount / MatRow%d / singleMass / massSum / Mass / Volume722`。
- `E-IDA-020/E-IDA-021` 说明这条下料导出链已有内部命令入口和 Ribbon 输出分组绑定，不能替代运行确认。
- `MatRow%d` 另在 `sub_1406382D0` 有引用，本轮不把该函数和 `sub_140602F90` 混成同一个业务结论。

`MatRow` 样例字段：

- `diameter`
- `lenSum`
- `countSum`
- `singleMass`
- `massSum`
- `stbLevel`

## 视口字段

`General-Info` 样例字段：

- `CompanyName`
- `ExportYesNo`
- `ExpSteelYesNo`
- `ExpSteelMark`
- `DimensionChicunB/T/L/R`
- `Model_FileName`
- `DispCuttedSymb`
- `BasePoint_X`
- `BasePoint_Y`
- `Range_Min_X`
- `Range_Max_X`
- `Range_Min_Y`
- `Range_Max_Y`
- `CutPlaneDirX/Y/Z`
- `TopDirX/Y/Z`
- `DrawingName`
- `DrawingUnit`
- `DrawingScale`
- `GeneralScale`
- `DrawingType`
- `LevelDrawing`
- `CutPlanePosX/Y/Z`

## 钢筋详图字段

`StbGroup` 样例字段：

- `rsdID`
- `groupID`
- `diameter`
- `diameter2`
- `interval`
- `barcount`
- `segcount`
- `stbNum`
- `stbNumAct`
- `stbLevel`
- `stbLayer`
- `stbProfile`
- `stbUse`
- `RangeLess180`
- `ComponentName`
- `PJSteelName`
- `SteelWay`
- `stbType`
- `stbOffsetInOut`

IDA 补证：

- `StbGroup%d` 写出函数：`sub_14063E910`。
- 上游 `sub_14061F830` 创建 `StbGroups` 并写 `stbGroupCount`。
- `sub_14063E910` 会继续创建 `Std%d`，并调用 `sub_14063B010` 写 `StbGeo%d`。
- 点状钢筋 `pointStb` 时还会调用 `sub_14063A860` 写 `FaceEdge`。

含义：

```text
StbGroup / StbGeo / StbTable 必须从同一份钢筋模型派生。
Detail 包不能成为独立编号真相。
```

`StbGeo` 样例字段：

- `segID`
- `stbSeqNum`
- `shapeType`
- `start_x/y/z`
- `end_x/y/z`
- `middle_x/y/z`
- `start_r`
- `end_r`
- `offset_x/y/z`

形状类型样例：

- `L`：线段。
- `A`：圆弧。
- `C`：点筋 / 点状钢筋表达。

## 对新系统的硬约束

工程图 writer 必须满足：

- `Detail.xml` 根节点是 `StyleRoot`。
- `DetailNN.stl` 根节点是 `DrawingRoot`。
- `DetailNN.stl` 内容是 XML，不是网格 STL。
- 输出 `StbTables`。
- 输出 `MaterialTable`。
- 输出 `HViewPorts/ViewPort`。
- 输出 `PartDetailDrawing`。
- 输出 `StbDetailDrawing/StbGroups/StbGroup/Std/StbGeo`。
- 钢筋统计和工程图字段必须共用同一份钢筋领域模型，避免下料表和图上钢筋不一致。

writer 事务、字段缺省、覆盖、回滚和 AutoCAD 导入门禁见：

```text
20_DetailWriter输出事务契约.md
```

## 待继续验证

- AutoCAD 2020 插件动态导入新生成包。
- `FDrawing.arx` / `FDrawingObj.dbx` 对字段缺失的容忍度。
- 多图纸 `Detail02`、`Detail09`、`Detail10` 命名规则。
- 真实复杂剖切视图、隐藏线、填充线和尺寸标注的字段保真。
- 真实 Excel `钢筋表 / 钢筋汇总表 / 钢筋下料单` 三表 writer、单位换算、显示格式和焊接字段。

## M2-Drawing-003 复杂字段静态证据

当前已形成：

```text
E-DETAIL-003
E-IDA-027
E-DEV-056
TODO-034 / M2-Drawing-003
```

旧 `Detail01.stl` 样例确认 `PartDetailDrawing` 下存在：

```text
General-Info
continue-line
hidden-line
central-line
section-line
hatch-line
Others
steeljoint-line
```

复杂线容器结构：

```text
continue-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
hidden-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
central-line -> lines
section-line -> lines / circles / Arcs / Ellipses / EllipseArcs / Splines
hatch-line -> lines
steeljoint-line -> joints
```

`section-line` 样例中实际出现：

```text
LineN:
  start_x / start_y / end_x / end_y / ZValue

ArcN:
  center_x / center_y / center_z / radius / start_angle / end_angle / ZValue
```

点筋 / 面边界样例中实际出现：

```text
StbGeo shapeType=C:
  point_x / point_y / point_z
  offset_x / offset_y / offset_z
  offset_x2 / offset_y2 / offset_z2

FaceEdge shapeType=L:
  start_x / start_y / end_x / end_y

FaceEdge shapeType=A:
  m_ArcDotReverse
  start_x / start_y / middle_x / middle_y / end_x / end_y
```

IDA MCP 已用 `fdrawing_arx_todo034` 复核 `FDrawing.arx`：

- 可见 `CViewInfo / CWSNLineDim / CWSNPointDim / CWSNSteelBarTable / CWSNMaterialTable / CWSNSectionTitle` 等旧插件对象符号。
- 可见 `CViewInfo` 的剖切方向、剖切位置、上方向、比例和尺寸相关方法符号。
- 暂未在该插件字符串缓存中直接命中 `continue-line / hidden-line / section-line / hatch-line / steeljoint-line / FaceEdge / StbGroup / StbGeo / ZValue` 等 XML 节点名。

边界：

```text
复杂 XML 字段名 = 旧 Detail01.stl 样例直接证据
FDrawing 对象符号 = 旧插件工程图对象模型存在的静态补强证据
AutoCAD L2 动态导入 = 仍未运行
section / hidden / hatch / joint 算法 = 仍未实现
```

## TODO-066 真实旧图石工程图包与下料表回填

`E-RUN-006 / E-DEV-088` 追加真实运行工件：

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

来源：

```text
用户现场旧图石运行回填。
本轮 agent 没有启动旧图石，没有安装 HASP，没有修改系统目录。
```

归档工件：

```text
消力池下游侧带齿槽底板结构图石钢筋模型.sfl
Detail.xml
Detail01.stl
Detail02.stl
Detail03.stl
Detail04.stl
下料表.xls
PixPin_2026-06-09_09-49-51.png
PixPin_2026-06-09_09-50-13.png
```

关键 hash：

```text
SFL:
  6706F10836CA43D1D3275ACD09B373CAC5AF08C0D7A66B1C7025D30006C4B007

Detail.xml:
  CCBD220D75D7F9C7E26E2540D639FA5956A369A31D7902F75ED36461F778F271
  bytes=14
  content=<StyleRoot/>

Detail01.stl:
  444BE32ED907C0393104F415639CED2FB698F21C93DF7F27F9ED830A70E40BE6

Detail02.stl:
  478D166A5DCE69A3BD8042E0118443780C8C7FBBDB93E7AC910EEB2CCB4E32F6

Detail03.stl:
  2531D11A15184909CC9FB3AA8CC96866F765E8254E443422854C176453A8FB43

Detail04.stl:
  03617911FBFCC2D53BCC310F515F8C375B3C6B78640C638CB2591DA1494D4D0E

下料表.xls:
  A9CF511EF0513DDB45E975A822C19833009AB237C9B0C7E1CA320BC2B5BF5D1E
```

`detail_package_probe_01.txt` 已确认：

```text
Detail01.stl: DrawingRoot=True, StbGroup=12, StbTable=1, section-line=1, pointStb=7, lineStb=5
Detail02.stl: DrawingRoot=True, StbGroup=6,  StbTable=0, section-line=1, pointStb=4, lineStb=2
Detail03.stl: DrawingRoot=True, StbGroup=5,  StbTable=0, section-line=1, pointStb=0, lineStb=5
Detail04.stl: DrawingRoot=True, StbGroup=7,  StbTable=0, section-line=1, pointStb=0, lineStb=7
```

Detail.xml 边界：

```text
本轮外部 staging/source 目录
`C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\docs\phase1\todo66\Detail.xml`
是空 StyleRoot：
  bytes=14
  content=<StyleRoot/>

`C:\Users\ghost\Desktop\reverse_engineering\Detail.xml`
和 `C:\Users\ghost\Desktop\reverse_engineering\autocad2020\Detail.xml`
是父目录/外部历史样式样例，可能来自其他项目：
  bytes=1333
  sha256=B02ABEEB7B9E0AB802566FA617E0627D9883010172EFD9CD50DACF940B5AFEB5
  含 3 个 Style。

两者不是同一次旧图石导出产物。
TODO-066 只能引用空 StyleRoot 作为本次运行事实。
用户补充多台电脑生成结果一致，Detail.xml 都是 <StyleRoot/>；
生成工程图和 CAD 导入时该文件修改时间也不更新。
因此当前更倾向把 Detail.xml 视为固定空模板 / 占位文件，
而不是每次生成的业务字段载体。
父目录有内容的 Detail.xml 只能作为外部参考，不能关闭本轮旧插件接受口径缺口。
```

`schedule_excel_preview_01.txt` 已确认 `下料表.xls` 可只读打开，且包含：

```text
钢筋表        rows=9,  cols=6
钢筋汇总表    rows=4,  cols=6
钢筋下料单    rows=16, cols=8
```

字段样例：

```text
钢筋表：
  编号 / 直径(mm) / 形状 / 单根长(cm) / 根数(根) / 总长(m)

钢筋汇总表：
  直径(mm) / 总长(m) / 单重(Kg/m) / 总重(Kg) / 钢筋合计(T) / 混凝土(m3)

钢筋下料单：
  编号 / 直径(mm) / 单净长(mm) / 焊长(mm) / 焊头(个) /
  单下料长(mm) / 根数(根) / 总根数(根)
```

截图证据：

```text
PixPin_2026-06-09_09-49-51.png
  -> 旧图石工程图页签，可见 生成工程图 / 下料表 按钮。

PixPin_2026-06-09_09-50-13.png
  -> 生成工程图后弹出：
     提示 / 剖切结束,请进入AutoCAD读取工程图!
```

本轮可确认：

```text
1. 旧图石真实生成了 Detail.xml 文件和 Detail01..04.stl。
2. 本轮 Detail.xml 是空 StyleRoot，不含 Style 条目。
3. 多机观察说明 Detail.xml 更像固定空模板 / 占位文件。
4. DetailNN.stl 仍是非空 DrawingRoot XML，不是网格 STL。
5. 旧图石真实生成了非空 下料表.xls。
6. 下料表.xls 里至少有 钢筋表 / 钢筋汇总表 / 钢筋下料单 三张表。
```

本轮仍不证明：

```text
1. AutoCAD L2 或 FDrawing 插件已成功读取该 Detail 包。
2. 新系统 DetailWriter 输出和旧图石输出已经 1:1。
3. rerun 覆盖行为已完全闭合。
4. 真实剖切线 / 隐藏线 / 填充线 / 点筋 / FaceEdge 算法已实现。
5. 接头链路恢复为当前优先级。
```
