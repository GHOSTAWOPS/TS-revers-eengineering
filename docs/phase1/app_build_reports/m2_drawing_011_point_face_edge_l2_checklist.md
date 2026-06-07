# M2-Drawing-011 PointStb / FaceEdge AutoCAD L2 手工确认清单

## 目的

本清单用于在可启动 AutoCAD 2020 的环境中确认旧 FDrawing 插件是否接受 `TODO-042` 的 `pointStb / FaceEdge` Detail 包。

当前本机 probe 结果：

```text
fdrawingPlugin.status = ready
autocadEnvironment.status = not_found
autocadL2 = not_run
```

因此本清单只是运行确认材料，不是 L2 pass 证明。

## 待导入包

新包目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\docs\phase1\app_build_reports\m2_drawing_011_detail_package
```

文件：

```text
Detail.xml
Detail01.stl
Detail02.stl
Detail03.stl
```

旧样例包目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020
```

旧插件：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawingObj.dbx
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawing.arx
```

## 操作步骤

1. 打开 AutoCAD 2020。
2. 在命令行执行：

```text
APPLOAD
```

3. 加载：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawingObj.dbx
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\FDrawing.arx
```

4. 记录命令行是否出现加载失败、版本错误、缺 DLL、签名错误或授权错误。
5. 先导入旧样例包：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\Detail.xml
C:\Users\ghost\Desktop\reverse_engineering\autocad2020\Detail01.stl
```

6. 再导入 TODO-042 新包：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\docs\phase1\app_build_reports\m2_drawing_011_detail_package
```

7. 截图和记录：

```text
APPLOAD 成功 / 失败命令行
FDrawing 插件导入窗口或命令提示
旧样例包导入结果
TODO-042 新包导入结果
模型空间 / 图纸空间生成结果
任何报错弹窗或命令行原文
```

## 重点观察

本包专门观察以下 XML 字段是否会导致旧插件导入失败：

```text
StbGroup.stbType = pointStb
StbGeo shapeType = C
point_x / point_y / point_z
offset_x / offset_y / offset_z
offset_x2 / offset_y2 / offset_z2
FaceEdge shapeType = L
FaceEdge shapeType = A
m_ArcDotReverse
```

## 判定规则

```text
旧样例包失败
  -> 不判断 TODO-042 新包质量；先处理 AutoCAD / FDrawing 环境。

旧样例包成功，TODO-042 新包失败
  -> 记录为 TODO-042 L2 fail；进入 GAP-DRAW-002 字段完整性 / 旧插件容忍度排查。

旧样例包成功，TODO-042 新包成功
  -> 可把本样本记为 pointStb / FaceEdge AutoCAD L2 pass。
     仍不代表完整工程图、点筋真实生成算法、FaceEdge 生成规则、剖切线算法、隐藏线算法、填充线算法、接头线算法或 golden 完成。
```

## 必须回填

运行后回填：

```text
AutoCAD version =
FDrawingObj.dbx loaded = yes/no
FDrawing.arx loaded = yes/no
old sample import = pass/fail
TODO-042 package import = pass/fail
error text =
screenshot paths =
output DWG / generated object summary =
operator =
run time =
```

## 不声明

即使本清单准备完成，也不声明：

```text
AutoCAD L2 已通过。
旧插件接受新包。
pointStb / FaceEdge 字段组合被旧插件接受。
点筋真实生成算法完成。
FaceEdge 生成规则完成。
连续线 / 隐藏线 / 中心线 / 填充线生成算法完成。
剖切线算法完成。
接头线算法完成。
完整工程图完成。
golden 采集完成。
```
