# TODO-065 Runtime Capture Intake

这个目录只用于收 `TODO-065 / 生成工程图 + 下料表` 的真实旧图石运行证据。

这轮不是要你证明新系统已经 1:1。

这轮只做两件事：

```text
1. 把旧图石真实输出目录 / 文件 / hash / 覆盖行为留证据
2. 把下料表真实输出结果 / 额外运行时弹窗留证据
```

## 推荐文件

推荐至少放这些工件：

```text
capture_notes.md

generate_package_dir_listing_01.txt
generate_package_hashes_01.txt
generate_package_dir_01.png

schedule_dir_listing_01.txt
schedule_hashes_01.txt
schedule_result_01.xls
schedule_result_01.xlsx
schedule_result_01.png
schedule_dialog_01.png

uncut_steel_01.txt
```

不是每个文件都必须有。

但后续要闭 `TODO-066`，
至少要同时覆盖：

```text
1. 生成工程图输出目录 / 文件 / hash / 覆盖行为
2. 下料表输出结果或额外运行时弹窗
```

## 必须记录的信息

请在 `capture_notes.md` 里补这些内容：

```text
旧图石版本：
SFL 文件名：
SFL hash：

生成工程图：
  - 点击前样本状态：
  - 输出目录：
  - 是否出现前置弹窗：建议填 是/否
  - 是否出现 UnCutSteel.TXT：建议填 是/否
  - 生成文件：
    - Detail.xml：
    - Detail01.stl：
    - 其他 DetailNN.stl：
  - 是否重复生成到同一目录：建议填 是/否
  - 第二次生成是否覆盖旧文件：建议填 是/否/未知

下料表：
  - 输出目录：
  - 是否生成 Excel：建议填 是/否
  - 是否只改 Detail 包：建议填 是/否
  - 是否同时生成 Excel 和 Detail 包：建议填 是/否
  - 是否出现额外运行时弹窗：建议填 是/否
  - 若有弹窗，标题：

目录 / 文件留证：
  - 目录 listing 文件名：
  - hash 文件名：
  - 代表截图文件名：

操作步骤：
  1.
  2.
  3.
```

## 目录 listing 与 hash

如果真实输出目录不方便直接拷进仓库，
至少要把目录 listing 和 hash 文本留在这里。

推荐 PowerShell：

```powershell
Get-ChildItem -LiteralPath "实际输出目录" -File |
  Sort-Object Name |
  Select-Object Name, Length, LastWriteTime |
  Format-Table -AutoSize |
  Out-File .\generate_package_dir_listing_01.txt -Encoding utf8
```

```powershell
Get-ChildItem -LiteralPath "实际输出目录" -File |
  Get-FileHash -Algorithm SHA256 |
  ForEach-Object {
    "{0}`t{1}" -f $_.Path.Split('\')[-1], $_.Hash
  } |
  Out-File .\generate_package_hashes_01.txt -Encoding utf8
```

如果是下料表目录，也同样留一份：

```powershell
schedule_dir_listing_01.txt
schedule_hashes_01.txt
```

## 最低验收口径

只要下面两条同时满足，这轮就能继续闭合：

```text
1. 至少有 1 组真实生成工程图目录 listing / hash / rerun 覆盖记录
2. 至少有 1 组真实下料表结果文件或弹窗截图
```

## 拒收规则

下面这些一律不能当 `TODO-066` 有效工件：

```text
1. 只有 UnCutSteel.TXT，没有 Detail.xml / DetailNN.stl / 目录 listing / hash。
2. 只有菜单截图，没有输出目录 / 文件 / hash。
3. 空白 Excel 工作簿。
4. 无关提示窗口截图。
5. 只有 hash，没有对应文件名 / 路径 / 操作步骤。
6. 没有 rerun 前后记录，却直接写“已确认覆盖行为”。
```

## 额外说明

`UnCutSteel.TXT` 的口径这里再强调一遍：

```text
它只算可选旁证，
不能单独证明 生成工程图 成功。
```
