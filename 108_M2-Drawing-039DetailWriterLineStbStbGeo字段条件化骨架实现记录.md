# M2-Drawing-039 DetailWriter lineStb StbGeo 字段条件化骨架实现记录

## 结论

本轮执行 `TODO-070 / DetailWriter 真实字段差异 P2：lineStb StbGeo 字段条件化骨架`。

本轮只收窄真实旧包已经暴露的 `lineStb + shapeType=L` 字段集合：

```text
lineStb 直线段 StbGeo# 继续输出：
  segID
  stbSeqNum
  shapeType
  start_x / start_y / start_z
  end_x / end_y / end_z
  offset_x / offset_y / offset_z

lineStb 直线段 StbGeo# 不再输出：
  middle_x / middle_y / middle_z
  start_r
  end_r
  length
```

本轮不声明圆弧、点筋、FaceEdge、StbGroup 多 `Std#`、Excel writer、AutoCAD L2 或完整工程图算法闭合。

## Control Contract

Primary Setpoint：

```text
让正式 app 的 DetailWriter 对 lineStb 直线段 StbGeo# 输出更接近
RUN-20260609-001 真实旧包字段集合。
```

Acceptance：

```text
1. 先补测试，确认旧实现会因为 lineStb line StbGeo.middle_x 仍存在而失败。
2. DetailWriter 对 lineStb + SteelBarSegmentShape::Line 只写 start/end/offset 字段集。
3. 点筋 StbGeo shapeType=C 既有字段不变。
4. FaceEdge 既有字段不变。
5. 弧段仍保留 middle/start_r/end_r/length，避免本轮误伤圆弧语义。
6. 默认 CTest、readiness gate、domain/rebar OCCT 泄漏检查和 git diff check 通过。
7. xhigh 只读 review 完成，Critical / Important 已修复或有技术反驳。
```

Guardrail：

```text
不启动旧图石。
不安装 HASP。
不运行 AutoCAD L2。
不实现 Excel writer。
不实现真实工程图算法。
不改钢筋创建业务。
不迁入父目录 rebar 业务代码。
domain/rebar 不引入 OCCT / AIS。
```

## 输入证据

本轮基于：

```text
E-DETAIL-004
E-DEV-091
105_M2-Drawing-036旧图石真实Detail与下料表字段对照P0实现记录.md
107_M2-Drawing-038DetailWriterStbRow扩展属性骨架实现记录.md
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

真实旧包事实：

```text
RUN-20260609-001 的 lineStb StbGeo# 当前只见：
  segID / stbSeqNum / shapeType /
  start_x/y/z / end_x/y/z / offset_x/y/z

未见：
  middle_x/y/z / start_r / end_r / length
```

该事实来自当前真实样例，不等于所有旧包、所有圆弧或所有钢筋类型的最终字段规则。

## 实现内容

代码改动：

```text
app/src/drawing/detail/DetailWriter.cpp
app/tests/unit/detail_writer_tests.cpp
```

实现细节：

```text
usesRuntimeLineStbLineGeoFieldSet(rebarType, shapeType)
  -> rebarType == "lineStb"
  -> shapeType == SteelBarSegmentShape::Line

writeSegmentGeo()
  -> pointStb 点筋保持 point/offset/offset2 字段骨架
  -> lineStb 直线段只写 start/end/offset 字段集
  -> 其他非点筋保留 start/middle/end/radius/offset/length 既有行为
```

## 测试

扩展测试：

```text
testDetailWriterMapsDomainRebarToDetailPackage()
```

新增覆盖：

```text
1. lineStb 直线段仍输出 start/end/offset。
2. lineStb 直线段不输出 middle_x/y/z。
3. lineStb 直线段不输出 start_r / end_r。
4. lineStb 直线段不输出 length。
5. 弧段仍输出 middle_x / start_r / length。
```

TDD 记录：

```text
修改实现前：
  detail_writer_tests 失败
  失败信息 = lineStb line StbGeo must not emit middle_x in TODO-070 field set

修改实现后：
  detail_writer_tests 通过
```

## 验证

```text
detail_writer_tests = pass
full build = pass
CTest = pass, 18/18 pass, total 172.89 sec
readiness gate unit = pass, 29 tests OK
strict readiness gate = pass, M1-Formal-Ready, 84/84 pass
domain/rebar OCCT leak scan = pass
git diff --check = pass
xhigh review = needs_fix_important_fixed; Critical none; Important doc/report xhigh status placeholders fixed by main flow
```

## 本轮不关闭的缺口

```text
GAP-DRAW-002:
  lineStb 直线段字段集合已收窄。
  但 StbGroup 多 Std#、真实工程图算法、旧插件导入接受度、
  圆弧/复杂钢筋字段条件和完整旧包兼容仍未闭合。

GAP-DRAW-007:
  StbRow 扩展属性公式和镜像语义仍未闭合。

GAP-DRAW-008:
  Excel writer 未实现。
```

## 下一步

```text
TODO-071 / 线配筋生成旧逻辑证据与 P0 切片准备
```

完成 TODO-070 后，下一轮建议从 Detail 字段收窄切回钢筋生成主线：先用 IDA / 15 / 16 / 旧运行证据确认 `sgroupbarline` 或等价线配筋生成入口、参数、对象创建和字段写入，再决定 P0 实现切片。
