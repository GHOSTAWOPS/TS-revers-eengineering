# M2-Drawing-035 Run 001

todoId = TODO-066
phase = M2-Drawing-035
evidenceId = E-RUN-006, E-DEV-088
decision = generate-package-and-schedule-runtime-artifacts-accepted

## Summary

本轮完成 `TODO-066 / 生成工程图与下料表旧图石真实运行工件回填 P0`。

用户提供的真实旧图石运行工件已归档到：

```text
docs/phase1/runtime_capture/todo_065_generate_package_and_schedule/run_20260609_001/
```

核心结论：

```text
acceptedArtifacts:
  sourceSfl: 消力池下游侧带齿槽底板结构图石钢筋模型.sfl
    tracking: local staging file; sha256 recorded; *.sfl is gitignored and not committed
  detailPackage:
    Detail.xml
    Detail01.stl
    Detail02.stl
    Detail03.stl
    Detail04.stl
  scheduleWorkbook:
    下料表.xls
  screenshots:
    PixPin_2026-06-09_09-49-51.png
    PixPin_2026-06-09_09-50-13.png

runtimeObserved:
  工程图页签可见 生成工程图 / 下料表 按钮
  生成工程图后提示: 剖切结束,请进入AutoCAD读取工程图!
  Detail.xml 存在但为空 StyleRoot: bytes=14, content=<StyleRoot/>
  用户补充多台电脑生成结果一致，Detail.xml 都是 <StyleRoot/>
  生成工程图和 CAD 导入时 Detail.xml 修改时间不更新
  Detail01..04.stl 均为 DrawingRoot XML
  下料表.xls 为非空 Excel workbook
```

## Evidence Files

```text
capture_notes.md
generate_package_dir_listing_01.txt
generate_package_hashes_01.txt
schedule_dir_listing_01.txt
schedule_hashes_01.txt
schedule_excel_preview_01.txt
detail_package_probe_01.txt
```

## Key Hashes

```text
SFL = 6706F10836CA43D1D3275ACD09B373CAC5AF08C0D7A66B1C7025D30006C4B007
下料表.xls = A9CF511EF0513DDB45E975A822C19833009AB237C9B0C7E1CA320BC2B5BF5D1E
Detail.xml = CCBD220D75D7F9C7E26E2540D639FA5956A369A31D7902F75ED36461F778F271
Detail.xml bytes = 14
Detail.xml content = <StyleRoot/>
Detail01.stl = 444BE32ED907C0393104F415639CED2FB698F21C93DF7F27F9ED830A70E40BE6
Detail02.stl = 478D166A5DCE69A3BD8042E0118443780C8C7FBBDB93E7AC910EEB2CCB4E32F6
Detail03.stl = 2531D11A15184909CC9FB3AA8CC96866F765E8254E443422854C176453A8FB43
Detail04.stl = 03617911FBFCC2D53BCC310F515F8C375B3C6B78640C638CB2591DA1494D4D0E
```

## Detail.xml Boundary

```text
todo66 Detail.xml = 当前真实回填工件，空 StyleRoot。
multi-machine observation = 多机生成均为 <StyleRoot/>，生成 / CAD 导入时修改时间不更新。
current interpretation = fixed empty template / placeholder, not drawing business payload.
reverse_engineering/Detail.xml = 父目录/外部历史样式样例，含 3 个 Style，可能来自其他项目。
autocad2020/Detail.xml = 同 hash 父目录/外部历史样式样例，可能来自其他项目。

本轮不把父目录/外部样式表混入 RUN-20260609-001。
```

## Guardrails

```text
oldRuntimeAutomaticallyLaunched = false
haspInstalledAutomatically = false
systemDirectoryModifiedAutomatically = false
appBusinessCodeModified = false
algorithmImplemented = false
autocadL2 = not_run
golden = not_started
xhighReview = needs_fix_then_fixed; Important on 99 GAP-IDA-006 top schedule row resolved
```

## Verification

```text
detailPackageProbe = pass; Detail01..04 DrawingRoot XML; total StbGroup count observed
styleRootProbe = pass; Detail.xml is empty StyleRoot; multi-machine observation suggests fixed empty placeholder
scheduleWorkbookProbe = pass; Excel COM read-only open; 3 non-empty sheets
artifactHashListing = pass; SHA256 recorded
gitDiffCheck = pass
ctest = pass; 18/18
readinessGateStrict = pass; M1-Formal-Ready; 84/84
domainRebarOCCLeak = pass; no matches
```

## Residual Gaps

```text
rerunOverwriteBehavior = partial; missing before/after two-run listing
detailXmlRole = likely fixed empty template / placeholder; old plugin existence requirement still unknown
AutoCADL2 = not_run
newSystemComparison = not_run
scheduleFullRules = not_closed
```

## Next

```text
nextTodo = TODO-067
action = compare real old VisualTS Detail01..04.stl and 下料表.xls fields against current DetailWriter / RebarScheduleService
scope = field gap report only; no algorithm implementation; no AutoCAD L2
```
