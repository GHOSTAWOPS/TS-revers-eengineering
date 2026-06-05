# 图石钢筋 1 比 1 复刻

本仓库是旧图石钢筋模块的 1:1 复刻工程。

核心路线：

- Qt6 替代 MFC / Codejock 的界面和命令入口。
- OCCT AIS 替代 HOOPS 的三维显示、选择和高亮。
- OCCT 几何 API 替代 ACIS 的几何计算能力。
- `LegacyGeometryAdapter` 把 OCCT 包装成旧 VisualTS 的 EDGE / FACE / 曲线 / 实体语义。
- 钢筋创建、编辑、统计、出图按 VisualTS / IDA / SFL / Detail 证据复刻。

入口文档：

- `00_总览.md`：文档体系和当前阶段。
- `46_CSE_v2Goal执行目标与Todo说明.md`：当前 goal 模式执行目标。
- `todo.csv`：任务看板。
- `AGENTS.md`：agent 工作规范、commit/tag 时间线规范。

当前主线不是“OCCT 直接重写钢筋软件”，而是用 Qt6 / OCCT 替换旧商业底座，
业务规则按旧图石证据逐步复刻。
