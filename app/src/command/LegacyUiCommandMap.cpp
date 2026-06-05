#include "command/LegacyUiCommandMap.h"

namespace tsrebar {

QVector<LegacyUiCommandDefinition> legacyUiCommands()
{
    return {
        {CommandId::ProjectNew, QStringLiteral("新建"), QStringLiteral("cmd_project_new"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("17")},
        {CommandId::ProjectOpen, QStringLiteral("打开"), QStringLiteral("cmd_project_open"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("17")},
        {CommandId::ProjectSave, QStringLiteral("保存"), QStringLiteral("cmd_project_save"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("17")},
        {CommandId::ProjectClose, QStringLiteral("关闭"), QStringLiteral("cmd_project_close"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("17")},
        {CommandId::ImportStep, QStringLiteral("导入 STEP"), QStringLiteral("cmd_import_step"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("36")},
        {CommandId::ExportRebarStep, QStringLiteral("输出钢筋"), QStringLiteral("cmd_export_rebar_step"),
         QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("E-RUN-002/E-DEV-023")},
        {CommandId::ViewFitAll, QStringLiteral("全显"), QStringLiteral("cmd_view_fit_all"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("17")},
        {CommandId::ViewSelectFace, QStringLiteral("选面"), QStringLiteral("cmd_view_select_face"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("M1-App-003")},
        {CommandId::ViewSelectEdge, QStringLiteral("选边"), QStringLiteral("cmd_view_select_edge"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("M1-App-003")},
        {CommandId::ViewSelectVertex, QStringLiteral("选点"), QStringLiteral("cmd_view_select_vertex"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("M1-App-003")},
        {CommandId::ViewPan, QStringLiteral("实时平移"), QStringLiteral("cmd_view_pan"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("17")},
        {CommandId::ViewZoom, QStringLiteral("实时缩放"), QStringLiteral("cmd_view_zoom"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("17")},
        {CommandId::ViewRotate, QStringLiteral("实时转动"), QStringLiteral("cmd_view_rotate"),
         QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("17")},
        {CommandId::RebarFaceCreate, QStringLiteral("面配筋"), QStringLiteral("cmd_rebar_face_create"),
         QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("14")},
        {CommandId::RebarLineCreate, QStringLiteral("线配筋"), QStringLiteral("cmd_rebar_line_create"),
         QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("15")},
        {CommandId::RebarArcGroupCreate, QStringLiteral("扇形筋"), QStringLiteral("cmd_rebar_arc_group_create"),
         QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("15")},
        {CommandId::QueryByNumber, QStringLiteral("按编号"), QStringLiteral("cmd_query_by_number"),
         QStringLiteral("tab_query"), QStringLiteral("group_rebar_query"), QStringLiteral("E-DLG-303")},
        {CommandId::QueryCollision, QStringLiteral("碰撞检测"), QStringLiteral("cmd_query_collision"),
         QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("E-DLG-449")},
        {CommandId::DrawingDefineSectionPlane, QStringLiteral("剖切面"), QStringLiteral("cmd_drawing_section_plane"),
         QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_view"), QStringLiteral("E-DLG-316/E-DLG-485")},
        {CommandId::DrawingGeneratePackage, QStringLiteral("生成工程图"), QStringLiteral("cmd_drawing_generate_package"),
         QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_output"), QStringLiteral("E-DLG-274")},
        {CommandId::DrawingGenerateBarSchedule, QStringLiteral("下料表"), QStringLiteral("cmd_drawing_bar_schedule"),
         QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_output"), QStringLiteral("E-DLG-427")},
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

} // namespace tsrebar
