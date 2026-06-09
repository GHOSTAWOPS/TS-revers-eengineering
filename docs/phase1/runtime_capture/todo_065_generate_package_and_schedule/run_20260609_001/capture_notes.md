# RUN-20260609-001 Capture Notes

旧图石版本：

- 用户本机旧图石软件，具体版本号待补。

SFL 文件名：

- 消力池下游侧带齿槽底板结构图石钢筋模型.sfl

SFL hash：

- SHA256: 6706F10836CA43D1D3275ACD09B373CAC5AF08C0D7A66B1C7025D30006C4B007
- Git tracking: `.sfl` 本体按 `.gitignore` 不纳入 git；本轮只提交 hash、文件名、
  Detail/Excel/截图和探针摘要。

生成工程图：

- 点击前样本状态：已打开 `消力池下游侧带齿槽底板结构图石钢筋模型.sfl`，截图可见工程图页签与输出分组。
- 输出目录：正式 repo 外层用户回填 staging/source 目录
  `C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\docs\phase1\todo66`，
  已归档到本目录。
- 是否出现前置弹窗：截图未见前置设置弹窗，当前记为未知。
- 是否出现 `UnCutSteel.TXT`：未提供，当前记为否/未见。
- `Detail.xml`：存在，14 bytes，内容为 `<StyleRoot/>`。
  用户补充多台电脑生成结果一致，`Detail.xml` 都是 `<StyleRoot/>`；
  生成工程图和 CAD 导入时该文件修改时间不更新。
  当前更倾向记录为固定空模板 / 占位文件，不作为业务字段载体。
- `Detail01.stl`：存在，17085 bytes，非空 `DrawingRoot` XML，含 `StbTables / HViewPorts / StbGroups`。
- `Detail02.stl`：存在，6604 bytes，非空 `DrawingRoot` XML。
- `Detail03.stl`：存在，8179 bytes，非空 `DrawingRoot` XML。
- `Detail04.stl`：存在，9717 bytes，非空 `DrawingRoot` XML。
- 是否重复生成到同一目录：文件 LastWriteTime 分布在 10:09:37 到 10:10:04，用户提供了同一批生成结果；缺少 rerun 前后两次 listing，当前只能记为未完全确认。
- 第二次生成是否覆盖旧文件：缺少 rerun before/after 证据，当前记为未知。

下料表：

- 输出目录：正式 repo 外层用户回填 staging/source 目录
  `C:\Users\ghost\Desktop\reverse_engineering\【03】图石软件\docs\phase1\todo66`，
  已归档到本目录。
- 是否生成 Excel：是，`下料表.xls`。
- 是否只改 Detail 包：否，已提供独立 Excel 文件。
- 是否同时生成 Excel 和 Detail 包：本次回填同时包含 `Detail.xml + Detail01..04.stl` 和 `下料表.xls`，但两者是否来自同一次点击序列仍按用户回填证据记录，不推断成单事务。
- 是否出现额外运行时弹窗：截图显示 `提示 / 剖切结束,请进入AutoCAD读取工程图!`。
- 若有弹窗，标题：提示。

目录 / 文件留证：

- 目录 listing 文件名：generate_package_dir_listing_01.txt
  说明：该 listing 是归档目录 listing，包含 agent 生成的 notes / probe / listing / hash
  等元数据，不等同于旧图石真实输出清单。
- hash 文件名：generate_package_hashes_01.txt
- 代表截图文件名：PixPin_2026-06-09_09-49-51.png, PixPin_2026-06-09_09-50-13.png
- `generate_package_hashes_01.txt` 只记录用户回填 / 旧图石输出工件：
  源 SFL、Detail.xml、Detail01..04.stl、下料表.xls 和截图。
  其中源 SFL 只保留 hash 证据，`.sfl` 本体不纳入 git。
  agent 生成的 listing / probe / preview / notes 元数据不参与真实输出 hash 清单。

操作步骤：

1. 用户在旧图石中打开 `消力池下游侧带齿槽底板结构图石钢筋模型.sfl`。
2. 进入 `工程图` 页签，截图可见 `生成工程图` 与 `下料表` 按钮。
3. 执行生成工程图，截图可见提示：`剖切结束,请进入AutoCAD读取工程图!`。
4. 用户回填 `Detail.xml`、`Detail01.stl` 到 `Detail04.stl`、`下料表.xls`、两张截图和源 SFL。
5. Agent 归档文件并生成 listing / SHA256。

补充说明：

- 本轮确认真实旧图石可生成非空 Detail 包和非空 Excel 下料表。
- `下料表.xls` 通过本机 Excel COM 只读打开，包含 `钢筋表`、`钢筋汇总表`、`钢筋下料单` 三个工作表，非空。
- 本轮不声明 AutoCAD L2 已通过。
- 本轮不声明旧插件已成功读取该 Detail 包。
- 本轮不声明 rerun 覆盖行为已完全闭合。
- 本轮不声明新系统输出已经与旧图石 1:1。
