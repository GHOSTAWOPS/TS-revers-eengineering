#include "ui/MainTabs.h"

namespace tsrebar {

QVector<MainTabDefinition> mainTabDefinitions()
{
    return {
        {QStringLiteral("开始"),
         QStringLiteral("tab_start"),
         {{QStringLiteral("文件"), QStringLiteral("group_file")},
          {QStringLiteral("编辑"), QStringLiteral("group_edit")},
          {QStringLiteral("参考"), QStringLiteral("group_reference")}}},
        {QStringLiteral("显示"),
         QStringLiteral("tab_view"),
         {{QStringLiteral("视图"), QStringLiteral("group_view")},
          {QStringLiteral("显示状态"), QStringLiteral("group_display_state")}}},
        {QStringLiteral("钢筋"),
         QStringLiteral("tab_rebar"),
         {{QStringLiteral("设置"), QStringLiteral("group_rebar_settings")},
          {QStringLiteral("创建"), QStringLiteral("group_rebar_create")},
          {QStringLiteral("编辑"), QStringLiteral("group_rebar_edit")}}},
        {QStringLiteral("查询"),
         QStringLiteral("tab_query"),
         {{QStringLiteral("查询钢筋"), QStringLiteral("group_rebar_query")},
          {QStringLiteral("检查"), QStringLiteral("group_rebar_check")}}},
        {QStringLiteral("工程图"),
         QStringLiteral("tab_drawing"),
         {{QStringLiteral("定义视图"), QStringLiteral("group_drawing_view")},
          {QStringLiteral("输出"), QStringLiteral("group_drawing_output")}}},
    };
}

} // namespace tsrebar

