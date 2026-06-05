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

IDA 补证：

- `sub_140605B20 -> sub_140602F90` 命中 `StbTables / StbTable / StbRow%d` 导出链。
- `E-IDA-020` 已确认内部命令绑定 `psexcel -> 0x8D1C -> sub_140605B20`。
- `E-IDA-021` 已确认 `0x8D1C` 被挂到 `工程图 / 输出` Ribbon 分组。
- 按钮 caption、Dialog #427 和运行输出仍需确认。
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
