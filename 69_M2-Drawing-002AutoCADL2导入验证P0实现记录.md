# M2-Drawing-002 AutoCAD L2 导入验证 P0 实现记录

## 结论

本轮完成 `TODO-033 / M2-Drawing-002` 的 P0 验证准备和阻塞闭环：

- 已用正式 `DetailWriter` 生成一个新 Detail 多图纸包。
- 已记录文件列表、hash、XML 根节点、L0/L1 结果和插件文件 hash。
- 已检查本机 AutoCAD 自动验证环境。
- 当前未发现 `acad.exe` 或 `accoreconsole.exe`，所以 AutoCAD L2 自动导入未运行。
- 本轮不声明旧 AutoCAD 2020 + FDrawing 插件导入通过。

本轮不是完整工程图专项，也不实现剖切线、隐藏线、填充线、标注或 golden。

## 证据

关联证据：

```text
E-DEV-054
E-DEV-055
E-IDA-026
GAP-DRAW-001
GAP-DRAW-002
GAP-DRAW-005
```

本轮新增实际工件证据：

```text
E-DEV-055
```

含义：

- `detail_l2_fixture_probe` 可调用正式 `DetailWriter` 生成 AutoCAD L2 验证用 Detail 包。
- 本轮生成包为 3 图纸：
  - `Detail.xml`
  - `Detail01.stl`
  - `Detail02.stl`
  - `Detail03.stl`
- `Detail.xml` 根节点为 `StyleRoot`。
- `DetailNN.stl` 根节点为 `DrawingRoot`。
- `DetailWriteResult`：
  - `decision = l0-l1-pass`
  - `l0 = passed`
  - `l1 = passed`
  - `l2 = not_run`
  - `autocadL2 = not_run`

## 新增工具

新增：

```text
app/tools/detail_l2_fixture_probe.cpp
```

注册：

```text
app/CMakeLists.txt
  -> detail_l2_fixture_probe
  -> detail_l2_fixture_probe_tests
```

工具职责：

- 只生成可复现的 L2 验证用 Detail 包。
- 只使用正式 `DetailWriter` 和 domain `SteelData`。
- 不手写 Detail XML。
- 不调用 AutoCAD。
- 不引入 OCCT / AIS 到 `domain/rebar`。
- 输出 JSON 摘要，包含文件 hash、根节点、L0/L1/L2 状态、warning 和 GAP。

示例：

```powershell
app\build\detail_l2_fixture_probe.exe `
  docs\phase1\app_build_reports\m2_drawing_002_detail_package `
  --views 3 `
  > docs\phase1\app_build_reports\m2_drawing_002_detail_probe.json
```

## 新 Detail 包

输出目录：

```text
docs/phase1/app_build_reports/m2_drawing_002_detail_package
```

文件：

```text
Detail.xml     186 bytes
Detail01.stl  2542 bytes
Detail02.stl  2542 bytes
Detail03.stl  2542 bytes
```

SHA256：

```text
Detail.xml
  CBF196DBF3E68FB41486D3CD365F27FDADB394CFF6A114EBE4DD020138B56E4D

Detail01.stl
  7F3D8B57F3E22AA3C1777033A268EBD6128398F1680C2CC6BF72097D88221F73

Detail02.stl
  512082F29863382929ED15B369A86B4AFB3C99FDFFE900AFA07AF294278AC5E4

Detail03.stl
  2C98ABBEE840D687979619A013B9FB89E54E0EA37E8AD51303FE329FE90671B2
```

probe 摘要：

```text
docs/phase1/app_build_reports/m2_drawing_002_detail_probe.json
```

关键字段：

```text
schemaVersion = detail-l2-fixture-probe/v1
runId = DW-L2-TODO033-001
viewCount = 3
decision = l0-l1-pass
l0 = passed
l1 = passed
l2 = not_run
autocadL2 = not_run
warnings = DW-WARN-MATERIAL_MASS_FORMULA_DEFERRED
```

## 旧插件文件

旧插件目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\autocad2020
```

存在文件：

```text
FDrawing.arx
FDrawingObj.dbx
Detail.xml
Detail01.stl
切剖面.dvb
unlk.lsp
toolbarNew/
```

插件 hash：

```text
FDrawing.arx
  30DB8152DA1D42DE0F2550C6A4242BCC4781F8D5B649428C6DDCBF647A29D428

FDrawingObj.dbx
  A7C8493507DE802F4881C0B60DB3DBBE4C5982CDF9F9C42A9C805EC2FEEC97B0
```

旧样例包 hash：

```text
autocad2020/Detail.xml
  B02ABEEB7B9E0AB802566FA617E0627D9883010172EFD9CD50DACF940B5AFEB5

autocad2020/Detail01.stl
  E3BB220E39F5657A38BFC6793E246208760816913520DAF9C97C3544D0421C3E
```

## AutoCAD 环境探测

本轮检查：

```powershell
Get-Command acad.exe
Get-Command accoreconsole.exe
注册表 HKLM/HKCU Autodesk AutoCAD
有限磁盘扫描 acad.exe / accoreconsole.exe
```

结果：

```text
acad.exe not in PATH
accoreconsole.exe not in PATH
HKLM:\SOFTWARE\Autodesk\AutoCAD missing
HKLM:\SOFTWARE\WOW6432Node\Autodesk\AutoCAD missing
HKCU:\SOFTWARE\Autodesk\AutoCAD missing
limited scan: acad.exe not found
limited scan: accoreconsole.exe not found
```

因此：

```text
AutoCAD L2 import = not_run
原因 = 本机当前未发现 AutoCAD 可执行文件，无法自动加载 FDrawing 插件并导入新包。
```

## IDA / 静态确认

本轮使用 IDA MCP 做了轻量确认：

```text
VisualTS.exe.i64 session = visualts_todo033
FDrawing.arx session = fdrawing_arx_todo033
```

VisualTS 字符串可见：

```text
Detail.xml
DrawingRoot
\Detail0%d.stl
\Detail%d.stl
msohtmplcllip
AutoCAD
```

FDrawing.arx 字符串可见：

```text
FDrawing.arx
FDrawingObj.dbx
AcRxArxApp
AcRxDbxApp
LoadLibraryW
acrxLoadModule
Detail 相关类名
XML 相关字符串
```

边界：

- 本轮没有确认旧插件具体导入命令名。
- 本轮没有确认旧插件接受新包。
- IDA 静态证据只说明插件和 Detail 相关结构存在，不能替代 AutoCAD L2 运行确认。

## 手工 L2 运行确认清单

当 AutoCAD 2020 可用后，按以下步骤执行：

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

4. 确认命令行没有加载失败、签名错误、版本错误、缺 DLL 或授权错误。
5. 使用旧插件 UI 或命令选择 Detail 包目录：

```text
C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\【图石钢筋1比1复刻】\docs\phase1\app_build_reports\m2_drawing_002_detail_package
```

6. 记录是否能导入：
   - 图形对象
   - 钢筋组
   - 下料表
   - 材料表
   - 多图纸视图
7. 截图：
   - APPLOAD 成功 / 失败命令行
   - 插件导入窗口或命令行提示
   - 导入后模型空间 / 图纸空间结果
   - 任何报错弹窗
8. 若失败，记录：
   - AutoCAD 版本
   - FDrawing.arx / FDrawingObj.dbx 是否成功加载
   - 失败命令原文
   - 是否能导入旧 `autocad2020/Detail.xml + Detail01.stl`
   - 是否只新包失败

判定：

```text
旧样例包可导入，新包失败
  -> 优先补 Detail 字段完整性，更新 GAP-DRAW-002。

旧样例包也失败
  -> 优先处理 AutoCAD / 插件加载环境，更新 GAP-DRAW-001。

新包可导入但对象缺失
  -> 记录缺失对象和字段，进入后续 Detail 复杂字段专项。

新包完全导入
  -> 可把本样本记为 AutoCAD L2 pass，但仍不代表完整工程图/golden。
```

## 验证

窄构建与生成：

```text
cmd /c "call ""D:\Visual Studio 2026\Community\VC\Auxiliary\Build\vcvars64.bat"" >nul &&
cmake --build app\build --target detail_l2_fixture_probe detail_writer_tests &&
app\build\detail_writer_tests.exe &&
app\build\detail_l2_fixture_probe.exe docs\phase1\app_build_reports\m2_drawing_002_detail_package --views 3 >
docs\phase1\app_build_reports\m2_drawing_002_detail_probe.json"

result = pass
```

完整 CTest：

```text
100% tests passed, 0 tests failed out of 17
Total Test time = 125.48 sec
```

readiness gate：

```text
py -3 tools/phase1_readiness_gate/check_phase1_readiness.py --strict

decision = M1-Formal-Ready
checks = 84 / 84 pass
errors = 0
warnings = 0
```

OCCT / AIS 泄漏扫描：

```text
rg -n "TopoDS_|AIS_|BRep|TopAbs_" app/src/domain/rebar app/src/drawing app/src/project

no matches
```

diff check：

```text
git diff --check

pass
```

## 边界

本轮不声明：

```text
AutoCAD L2 动态导入已通过。
旧插件导入命令名已完全确认。
旧插件接受 Detail100.stl。
完整工程图已完成。
剖切线 / 隐藏线 / 填充线已实现。
下料表完整公式已闭合。
golden 已采集。
```

## 下一步建议

下一阶段优先级：

1. 如果用户能打开 AutoCAD 2020：按本清单执行 L2 手工确认。
2. 如果旧样例包能导入但新包失败：进入 Detail 复杂字段补齐专项。
3. 如果 AutoCAD / 插件无法加载：先闭合 AutoCAD 插件环境和授权缺口。
4. 不建议在没有 L2 反馈前直接实现完整剖切线 / 隐藏线 / 填充线。
