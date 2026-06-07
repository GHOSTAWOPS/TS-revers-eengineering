# M2-Drawing-012 DetailWriter Others / steeljoint-line 字段骨架 P0 实现记录

## 结论

本轮完成 `TODO-043 / M2-Drawing-012`：

- `DetailWriter` 保持输出 `Others` 空容器和 `steeljoint-line / joints` 容器骨架。
- `detail_l2_fixture_probe` 新增 `--fixture others-steeljoint` 独立变量。
- 新增 `othersSteeljoint` probe，离线确认：
  - `Others` 存在且为空。
  - `steeljoint-line` 存在。
  - `steeljoint-line / joints` 存在。
  - `algorithmImplemented=false`。
  - `autocadL2Claimed=false`。
- 新增 DetailWriter 单测，明确该字段骨架不得声明 AutoCAD L2 或真实接头线算法。
- readiness gate 新增 `TODO-043` done-report 映射和 RED/GREEN 单测。

本轮不实现真实接头线算法，不实现 `Others` 真实几何规则，不声明旧插件接受，不声明 AutoCAD L2 通过，不进入 golden。

## Control Contract

```text
Primary Setpoint
  TODO-043 只补 Others / steeljoint-line 字段骨架和独立 probe。

Acceptance
  DetailWriter 单测覆盖 Others / steeljoint-line 容器骨架。
  detail_l2_fixture_probe --fixture others-steeljoint 生成三图纸包并通过。
  CTest / readiness / OCCT 泄漏扫描 / xhigh / 文档 / commit / tag / push 闭合后才算完成。

Guardrail
  不写真实接头线算法。
  不写 Others 真实几何算法。
  不声明 AutoCAD L2 pass。
  不迁入父目录 rebar 业务。
  不让 domain/rebar、drawing、project 泄漏 OCCT/AIS 类型。
```

## TDD 记录

RED：

```text
app\build\detail_l2_fixture_probe.exe --fixture others-steeljoint --views 3 --run-id DW-L2-TODO043-RED -o docs\phase1\app_build_reports\m2_drawing_012_red_probe_tmp
  -> fixture must be complex-skeleton, point-face-edge, section-line, or line-containers
  -> exit 1
```

GREEN：

```text
cmd /c '"D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat" && cmake --build app\build --target detail_writer_tests detail_l2_fixture_probe'
  -> build pass

app\build\detail_writer_tests.exe
  -> pass

ctest --test-dir app\build -R "detail_l2_fixture_probe_tests|detail_writer_tests" --output-on-failure
  -> 2/2 pass
```

Gate RED/GREEN：

```text
py -3 tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> RED before TODO-043 known_reports mapping
  -> GREEN after mapping, 15/15 OK
```

最终验证：

```text
py -3 -m json.tool docs\phase1\app_build_reports\m2_drawing_012_detail_probe.json
py -3 -m json.tool docs\phase1\app_build_reports\m2_drawing_012_run_001.json
  -> json ok

cmd /c '"D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat" && cmake --build app\build'
  -> build pass

ctest --test-dir app\build --output-on-failure
  -> 17/17 pass

py -3 tools\phase1_readiness_gate\test_phase1_readiness_gate.py
  -> 15/15 OK

py -3 tools\phase1_readiness_gate\check_phase1_readiness.py --strict
  -> M1-Formal-Ready, 84/84 pass

rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project
  -> no matches

git diff --check
  -> pass
```

xhigh 只读 review：

```text
reviewer = Sartre / 019ea1dd-e207-7132-bd09-eccf2173670a
verdict = needs_fix
critical = none
important = 34 / 99 曾把 79 写成手工 L2 清单编号
fix = 主流程已改为 TODO-044 将单独生成 Others / steeljoint-line L2 清单
route drift = no
AutoCAD L2 misclaim = no
algorithm misclaim = no
evidence consistency = pass
post-review verification = json ok; gate unit 15/15 OK; strict readiness 84/84 pass; OCCT leak scan pass; diff check pass
agent closed = true
```

## 实现内容

### DetailWriter 单测

新增：

```text
app/tests/unit/detail_writer_tests.cpp
  -> testDetailWriterWritesOthersAndSteeljointContainerSkeleton
```

断言：

```text
PartDetailDrawing contains Others
PartDetailDrawing contains steeljoint-line
Others has no children
steeljoint-line has child joints
joints has no children
result.l2 == not_run
```

### detail_l2_fixture_probe

新增 fixture：

```text
--fixture others-steeljoint
```

新增 JSON 节点：

```text
othersSteeljoint:
  scope = Others empty container + steeljoint-line/joints container skeleton only
  others.present = true
  others.actualChildren = []
  steeljointLine.present = true
  steeljointLine.jointsPresent = true
  algorithmImplemented = false
  autocadL2Claimed = false
  passed = true
```

### CTest 覆盖

`detail_l2_fixture_probe_tests` 改为默认跑：

```text
--fixture others-steeljoint --views 3
```

这样 TODO-043 的独立 probe 会进入默认 CTest。

### readiness gate

新增 done-report 映射：

```text
TODO-043:
  79_M2-Drawing-012DetailWriterOthersSteeljoint字段骨架P0实现记录.md
  docs/phase1/app_build_reports/m2_drawing_012_run_001.md
```

## Probe 摘要

正式命令：

```text
app\build\detail_l2_fixture_probe.exe \
  --fixture others-steeljoint \
  --views 3 \
  --run-id DW-L2-TODO043-001 \
  --plugin-dir C:\Users\ghost\Desktop\reverse_engineering\autocad2020 \
  -o docs\phase1\app_build_reports\m2_drawing_012_detail_package \
  > docs\phase1\app_build_reports\m2_drawing_012_detail_probe.json
```

结果：

```text
fixture = others-steeljoint
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
othersSteeljoint.passed = true
others.present = true
others.actualChildren = 0
steeljointLine.present = true
steeljointLine.jointsPresent = true
algorithmImplemented = false
autocadEnvironment.status = not_found
fdrawingPlugin.status = ready
fdrawingPlugin.allRequiredPresent = true
```

文件 hash：

```text
Detail.xml    cbf196dbf3e68fb41486d3cd365f27fdadb394cff6a114ebe4dd020138b56e4d
Detail01.stl  7e25983036b3150021f42ba47c608340b4ba868a42e88931140105bd061f484a
Detail02.stl  d54fbdce40da3487fe524bd028a1c149cb121631b09e37f488a10cf2768a116d
Detail03.stl  f2964b47a639f1234158917ef9b29109f99bb9bc997196f8aa6192b6dc4af993
```

## IDA MCP 状态

本轮没有新增 IDA 证据。

原因：

```text
TODO-043 只把旧 Detail 样例已确认的 Others / steeljoint-line 容器拆成独立字段骨架和 probe。
旧样例中 Others 为空，steeljoint-line 仅确认 joints 容器存在。
没有实现接头线生成规则或 Others 几何规则，因此不新增旧业务算法判断。
```

后续若要实现真实接头线 / Others 几何算法，必须继续用 IDA MCP 或旧图石运行确认闭合。

## 明确不声明

```text
AutoCAD L2 import pass
FDrawing accepts TODO-043 package
real joint-line generation algorithm
real Others geometry algorithm
hidden / hatch / section drawing algorithm
complete drawing generation
golden collected
```

## 输出工件

```text
79_M2-Drawing-012DetailWriterOthersSteeljoint字段骨架P0实现记录.md
docs/phase1/app_build_reports/m2_drawing_012_detail_probe.json
docs/phase1/app_build_reports/m2_drawing_012_detail_package/Detail.xml
docs/phase1/app_build_reports/m2_drawing_012_detail_package/Detail01.stl
docs/phase1/app_build_reports/m2_drawing_012_detail_package/Detail02.stl
docs/phase1/app_build_reports/m2_drawing_012_detail_package/Detail03.stl
docs/phase1/app_build_reports/m2_drawing_012_run_001.md
docs/phase1/app_build_reports/m2_drawing_012_run_001.json
```

## 下一步

```text
TODO-044 / M2-Drawing-013
  -> DetailWriter Others / steeljoint-line AutoCAD L2 运行确认准备 P0
  -> 把 TODO-043 独立包转成手工 L2 确认清单和环境阻塞记录
  -> 不实现真实接头线 / Others 几何算法，不声明 AutoCAD L2 pass，不进入 golden
```
