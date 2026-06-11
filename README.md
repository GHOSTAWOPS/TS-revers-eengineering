# 图石钢筋新平台

本仓库是图石钢筋替代系统的正式开发仓库。

当前主线已在 2026-06-11 调整为：

```text
STEP-only + RebarSmart 钢筋生成逻辑证据 + 图石 Detail 包兼容导出
```

## 核心路线

- Qt6：新界面、参数面板、命令入口。
- OCCT：STEP/STP 导入、几何计算、AIS 显示、选择和高亮。
- RebarSmart3DE：只作为钢筋生成逻辑证据源，不作为运行时依赖。
- VisualTS / 老图石：主要作为 Detail 包、工程图字段、下料表和旧 AutoCAD 插件兼容证据源。
- `.tsrebar`：新系统内部主工程格式。
- `Detail.xml + DetailNN.stl`：兼容旧 AutoCAD 插件的导出格式，其中 `DetailNN.stl` 是 XML 文本，不是标准 STL 网格。

## 不再作为 P0 主线

- 旧 VisualTS UI / 操作流程全量 1:1。
- 老 `.sfl` 完整打开和完整兼容。
- SAT / SAB / DWG / DXF 直接兼容。
- 直接调用 RebarSmart、VisualTS、3DE、ACIS、HOOPS、Codejock 等商业或闭源运行时。

旧 VisualTS 相关文档不会删除，但降级为历史证据和 Detail/出图兼容证据。

## 入口文档

- `00_总览.md`：文档体系和当前路线。
- `docs/adr/ADR-20260611-step-only-rebarsmart-detail.md`：路线调整 ADR。
- `docs/roadmap/STEP_only_RebarSmart_Detail_refactor_plan.md`：新路线 roadmap。
- `46_CSE_v2Goal执行目标与Todo说明.md`：goal 模式执行目标。
- `todo.csv`：任务看板。
- `AGENTS.md`：agent 工作规范、commit/tag 时间线规范。
