#include "command/LegacyUiCommandMap.h"

#include "command/CommandRegistry.h"

namespace tsrebar {

namespace {

LegacyUiCommandDefinition command(CommandId id,
                                  LegacyCommandUiSurface uiSurface,
                                  QString commandKey,
                                  QString caption,
                                  QString objectName,
                                  QString tabObjectName,
                                  QString groupObjectName,
                                  QString legacyUiPath,
                                  QVector<QString> evidenceIds,
                                  QVector<QString> sourceRefs = {},
                                  QVector<QString> gapIds = {},
                                  QVector<QString> inputSelectionTypes = {},
                                  QString legacyCommand = {},
                                  QString legacyContextCommandId = {},
                                  LegacyCommandImplementationState implementationState =
                                      LegacyCommandImplementationState::NotImplemented,
                                  QVector<LegacyCommandInteractionState> interactionStates =
                                      {LegacyCommandInteractionState::Idle,
                                       LegacyCommandInteractionState::Armed,
                                       LegacyCommandInteractionState::Picking,
                                       LegacyCommandInteractionState::Previewing,
                                       LegacyCommandInteractionState::Completed,
                                       LegacyCommandInteractionState::Cancelled,
                                       LegacyCommandInteractionState::Failed})
{
    return {id,
            uiSurface,
            std::move(commandKey),
            std::move(caption),
            std::move(objectName),
            std::move(tabObjectName),
            std::move(groupObjectName),
            std::move(legacyUiPath),
            std::move(legacyCommand),
            std::move(legacyContextCommandId),
            std::move(evidenceIds),
            std::move(sourceRefs),
            std::move(gapIds),
            std::move(inputSelectionTypes),
            implementationState,
            std::move(interactionStates)};
}

LegacyUiCommandDefinition ribbonCommand(CommandId id,
                                        QString commandKey,
                                        QString caption,
                                        QString objectName,
                                        QString tabObjectName,
                                        QString groupObjectName,
                                        QString legacyUiPath,
                                        QVector<QString> evidenceIds,
                                        QVector<QString> sourceRefs = {},
                                        QVector<QString> gapIds = {},
                                        QVector<QString> inputSelectionTypes = {},
                                        QString legacyCommand = {},
                                        QString legacyContextCommandId = {},
                                        LegacyCommandImplementationState implementationState =
                                            LegacyCommandImplementationState::NotImplemented)
{
    return command(id,
                   LegacyCommandUiSurface::Ribbon,
                   std::move(commandKey),
                   std::move(caption),
                   std::move(objectName),
                   std::move(tabObjectName),
                   std::move(groupObjectName),
                   std::move(legacyUiPath),
                   std::move(evidenceIds),
                   std::move(sourceRefs),
                   std::move(gapIds),
                   std::move(inputSelectionTypes),
                   std::move(legacyCommand),
                   std::move(legacyContextCommandId),
                   implementationState);
}

LegacyUiCommandDefinition contextMenuCommand(CommandId id,
                                             QString commandKey,
                                             QString caption,
                                             QString objectName,
                                             QString legacyUiPath,
                                             QVector<QString> evidenceIds,
                                             QVector<QString> sourceRefs = {},
                                             QVector<QString> gapIds = {},
                                             QVector<QString> inputSelectionTypes = {},
                                             QString legacyContextCommandId = {})
{
    return command(id,
                   LegacyCommandUiSurface::ContextMenu,
                   std::move(commandKey),
                   std::move(caption),
                   std::move(objectName),
                   {},
                   {},
                   std::move(legacyUiPath),
                   std::move(evidenceIds),
                   std::move(sourceRefs),
                   std::move(gapIds),
                   std::move(inputSelectionTypes),
                   {},
                   std::move(legacyContextCommandId));
}

QString placeholderMessage(const LegacyUiCommandDefinition& definition)
{
    return QStringLiteral("%1：该功能已登记，旧图石行为待确认，当前版本暂未实现。")
        .arg(definition.commandKey);
}

} // namespace

QVector<LegacyUiCommandDefinition> legacyUiCommands()
{
    return {
        ribbonCommand(CommandId::ProjectNew, QStringLiteral("Project.New"), QStringLiteral("新建"),
                QStringLiteral("cmd_project_new"), QStringLiteral("tab_start"),
                QStringLiteral("group_file"), QStringLiteral("开始/文件/新建"),
                {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ProjectOpen, QStringLiteral("Project.Open"), QStringLiteral("打开"),
                QStringLiteral("cmd_project_open"), QStringLiteral("tab_start"),
                QStringLiteral("group_file"), QStringLiteral("开始/文件/打开"),
                {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ProjectSave, QStringLiteral("Project.Save"), QStringLiteral("保存"),
                QStringLiteral("cmd_project_save"), QStringLiteral("tab_start"),
                QStringLiteral("group_file"), QStringLiteral("开始/文件/保存"),
                {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ProjectClose, QStringLiteral("Project.Close"), QStringLiteral("关闭"),
                QStringLiteral("cmd_project_close"), QStringLiteral("tab_start"),
                QStringLiteral("group_file"), QStringLiteral("开始/文件/关闭"),
                {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ImportStep, QStringLiteral("Project.OpenStep"),
                QStringLiteral("导入 STEP"), QStringLiteral("cmd_import_step"),
                QStringLiteral("tab_start"), QStringLiteral("group_file"),
                QStringLiteral("开始/文件/打开"), {}, {QStringLiteral("36")}, {}, {},
                {}, {}, LegacyCommandImplementationState::Implemented),
        ribbonCommand(CommandId::ExportRebarStep, QStringLiteral("Rebar.Export.SteelStepOrLegacy"),
                QStringLiteral("输出钢筋"), QStringLiteral("cmd_export_rebar_step"),
                QStringLiteral("tab_start"), QStringLiteral("group_file"),
                QStringLiteral("开始/文件/输出钢筋"),
                {QStringLiteral("E-RUN-002"), QStringLiteral("E-DEV-023")},
                {}, {QStringLiteral("GAP-IDA-008")}, {},
                QStringLiteral("sexportbar/ysteelout")),
        ribbonCommand(CommandId::ViewFitAll, QStringLiteral("View.FitAll"), QStringLiteral("全显"),
                QStringLiteral("cmd_view_fit_all"), QStringLiteral("tab_view"),
                QStringLiteral("group_view"), QStringLiteral("显示/取景/全显"),
                {}, {QStringLiteral("17")}, {}, {}, {}, {},
                LegacyCommandImplementationState::Implemented),
        ribbonCommand(CommandId::ViewSelectFace, QStringLiteral("View.SelectFace"),
                QStringLiteral("选面"), QStringLiteral("cmd_view_select_face"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/选择/选面"), {QStringLiteral("E-DEV-027")},
                {QStringLiteral("M1-App-003")},
                {}, {QStringLiteral("face")}, {}, {},
                LegacyCommandImplementationState::Implemented),
        ribbonCommand(CommandId::ViewSelectEdge, QStringLiteral("View.SelectEdge"),
                QStringLiteral("选边"), QStringLiteral("cmd_view_select_edge"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/选择/选边"), {QStringLiteral("E-DEV-027")},
                {QStringLiteral("M1-App-003")},
                {}, {QStringLiteral("edge")}, {}, {},
                LegacyCommandImplementationState::Implemented),
        ribbonCommand(CommandId::ViewSelectVertex, QStringLiteral("View.SelectVertex"),
                QStringLiteral("选点"), QStringLiteral("cmd_view_select_vertex"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/选择/选点"), {QStringLiteral("E-DEV-027")},
                {QStringLiteral("M1-App-003")},
                {}, {QStringLiteral("vertex")}, {}, {},
                LegacyCommandImplementationState::Implemented),
        ribbonCommand(CommandId::ViewPan, QStringLiteral("View.PanRealtime"),
                QStringLiteral("实时平移"), QStringLiteral("cmd_view_pan"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/取景/实时平移"), {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ViewZoom, QStringLiteral("View.ZoomRealtime"),
                QStringLiteral("实时缩放"), QStringLiteral("cmd_view_zoom"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/取景/实时缩放"), {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::ViewRotate, QStringLiteral("View.OrbitRealtime"),
                QStringLiteral("实时转动"), QStringLiteral("cmd_view_rotate"),
                QStringLiteral("tab_view"), QStringLiteral("group_view"),
                QStringLiteral("显示/取景/实时转动"), {}, {QStringLiteral("17")}),
        ribbonCommand(CommandId::RebarFaceCreate, QStringLiteral("Rebar.Create.Face"),
                QStringLiteral("面配筋"), QStringLiteral("cmd_rebar_face_create"),
                QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"),
                QStringLiteral("钢筋/主要钢筋/面配筋"),
                {QStringLiteral("E-DLG-265"), QStringLiteral("E-DLG-266"),
                 QStringLiteral("E-DLG-268"), QStringLiteral("E-SFL-001"),
                 QStringLiteral("E-RUN-001")},
                {},
                {QStringLiteral("GAP-DEV-004"), QStringLiteral("GAP-DEV-005"),
                 QStringLiteral("GAP-DEV-006")},
                {QStringLiteral("face"), QStringLiteral("direction-or-guide-line")}),
        ribbonCommand(CommandId::RebarLineCreate, QStringLiteral("Rebar.Create.LineGroup"),
                QStringLiteral("线配筋"), QStringLiteral("cmd_rebar_line_create"),
                QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"),
                QStringLiteral("钢筋/主要钢筋/线配筋"),
                {QStringLiteral("E-IDA-002"), QStringLiteral("E-IDA-010"),
                 QStringLiteral("E-IDA-012"), QStringLiteral("E-IDA-013"),
                 QStringLiteral("E-STR-线配筋")},
                {},
                {QStringLiteral("GAP-IDA-002"), QStringLiteral("GAP-IDA-007")},
                {QStringLiteral("single-selected-entity-list")},
                QStringLiteral("sgroupbarline")),
        ribbonCommand(CommandId::RebarArcGroupCreate, QStringLiteral("Rebar.Create.ArcGroup"),
                QStringLiteral("扇形筋"), QStringLiteral("cmd_rebar_arc_group_create"),
                QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"),
                QStringLiteral("钢筋/主要钢筋/扇形筋"),
                {QStringLiteral("E-IDA-001"), QStringLiteral("E-IDA-011"),
                 QStringLiteral("E-IDA-012"), QStringLiteral("E-IDA-013"),
                 QStringLiteral("E-STR-扇形筋")},
                {},
                {QStringLiteral("GAP-IDA-001"), QStringLiteral("GAP-IDA-007")},
                {QStringLiteral("single-selected-chain")},
                QStringLiteral("sgroupbararc")),
        contextMenuCommand(CommandId::RebarGroupTrimByLine,
                           QStringLiteral("RebarGroup.TrimByLine"),
                           QStringLiteral("与线裁剪"),
                           QStringLiteral("cmd_rebar_group_trim_by_line"),
                           QStringLiteral("钢筋/右键菜单/与线裁剪"),
                           {QStringLiteral("E-CTX-8939")},
                           {},
                           {QStringLiteral("GAP-IDA-007")},
                           {QStringLiteral("rebar-group+line")},
                           QStringLiteral("0x8939")),
        contextMenuCommand(CommandId::RebarGroupTrimByFace,
                           QStringLiteral("RebarGroup.TrimByFace"),
                           QStringLiteral("与面裁剪"),
                           QStringLiteral("cmd_rebar_group_trim_by_face"),
                           QStringLiteral("钢筋/右键菜单/与面裁剪"),
                           {QStringLiteral("E-CTX-8CE5")},
                           {},
                           {QStringLiteral("GAP-IDA-007")},
                           {QStringLiteral("rebar-group+face")},
                           QStringLiteral("0x8CE5")),
        ribbonCommand(CommandId::QueryByNumber, QStringLiteral("Rebar.Query.ByNumber"),
                QStringLiteral("按编号"), QStringLiteral("cmd_query_by_number"),
                QStringLiteral("tab_query"), QStringLiteral("group_rebar_query"),
                QStringLiteral("查询/查询钢筋/按编号"), {QStringLiteral("E-DLG-303")}),
        ribbonCommand(CommandId::QueryCollision, QStringLiteral("Rebar.Check.Overlap"),
                QStringLiteral("碰撞检测"), QStringLiteral("cmd_query_collision"),
                QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"),
                QStringLiteral("查询/测量/碰撞检测"), {QStringLiteral("E-DLG-449")}),
        ribbonCommand(CommandId::DrawingDefineSectionPlane,
                QStringLiteral("Drawing.DefineSectionPlane"), QStringLiteral("剖切面"),
                QStringLiteral("cmd_drawing_section_plane"), QStringLiteral("tab_drawing"),
                QStringLiteral("group_drawing_view"),
                QStringLiteral("工程图/定义视图/剖切面"),
                {QStringLiteral("E-DLG-316"), QStringLiteral("E-DLG-485")}),
        ribbonCommand(CommandId::DrawingGeneratePackage,
                QStringLiteral("Drawing.GeneratePackage"), QStringLiteral("生成工程图"),
                QStringLiteral("cmd_drawing_generate_package"),
                QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_output"),
                QStringLiteral("工程图/输出/生成工程图"), {QStringLiteral("E-DLG-274")}),
        ribbonCommand(CommandId::DrawingGenerateBarSchedule,
                QStringLiteral("Drawing.GenerateBarSchedule"), QStringLiteral("下料表"),
                QStringLiteral("cmd_drawing_bar_schedule"), QStringLiteral("tab_drawing"),
                QStringLiteral("group_drawing_output"), QStringLiteral("工程图/输出/下料表"),
                {QStringLiteral("E-DLG-427"), QStringLiteral("E-IDA-019"),
                 QStringLiteral("E-IDA-020"), QStringLiteral("E-IDA-021")},
                {},
                {QStringLiteral("GAP-IDA-006"), QStringLiteral("GAP-DRAW-004")},
                {QStringLiteral("rebar-model")}, QStringLiteral("psexcel"),
                QStringLiteral("0x8D1C")),
    };
}

bool hasLegacyUiCommand(CommandId id)
{
    const auto commands = legacyUiCommands();
    for (const auto& command : commands) {
        if (command.id == id) {
            return true;
        }
    }
    return false;
}

std::optional<LegacyUiCommandDefinition> legacyUiCommandDefinition(CommandId id)
{
    const auto commands = legacyUiCommands();
    for (const auto& command : commands) {
        if (command.id == id) {
            return command;
        }
    }
    return std::nullopt;
}

void registerLegacyUiCommandPlaceholders(CommandRegistry& registry)
{
    const auto commands = legacyUiCommands();
    for (const auto& commandDefinition : commands) {
        if (registry.hasHandler(commandDefinition.id) ||
            commandDefinition.implementationState == LegacyCommandImplementationState::Implemented) {
            continue;
        }

        registry.registerHandler(commandDefinition.id, [commandDefinition]() {
            return CommandResult{CommandStatus::NotImplemented,
                                 placeholderMessage(commandDefinition)};
        });
    }
}

} // namespace tsrebar
