#include "command/CommandRegistry.h"
#include "command/LegacyUiCommandMap.h"
#include "ui/MainTabs.h"

#include <iostream>

namespace {

bool fail(const char* message)
{
    std::cerr << message << '\n';
    return false;
}

bool testMainTabs()
{
    const auto tabs = tsrebar::mainTabDefinitions();
    if (tabs.size() != 5) {
        return fail("expected five main tabs");
    }

    const QStringList expected = {
        QStringLiteral("开始"),
        QStringLiteral("显示"),
        QStringLiteral("钢筋"),
        QStringLiteral("查询"),
        QStringLiteral("工程图"),
    };

    for (const QString& name : expected) {
        bool found = false;
        for (const auto& tab : tabs) {
            found = found || tab.title == name;
        }
        if (!found) {
            std::cerr << "missing tab: " << name.toStdString() << '\n';
            return false;
        }
    }

    return true;
}

bool testLegacyCommandMap()
{
    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ExportRebarStep)) {
        return fail("ExportRebarStep command is not registered");
    }

    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::DrawingGeneratePackage)) {
        return fail("DrawingGeneratePackage command is not registered");
    }

    if (!tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectFace) ||
        !tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectEdge) ||
        !tsrebar::hasLegacyUiCommand(tsrebar::CommandId::ViewSelectVertex)) {
        return fail("face/edge/vertex selection commands are not registered");
    }

    return true;
}

bool testLegacyUiCommandCoverageP1()
{
    struct ExpectedCommand
    {
        tsrebar::CommandId id;
        QString commandKey;
        QString caption;
        QString tabObjectName;
        QString groupObjectName;
        QString legacyUiPath;
    };

    const QVector<ExpectedCommand> expected = {
        {tsrebar::CommandId::ExportRebarStep, QStringLiteral("Rebar.Export.SteelStepOrLegacy"), QStringLiteral("输出钢筋"), QStringLiteral("tab_start"), QStringLiteral("group_file"), QStringLiteral("开始/文件/输出钢筋")},
        {tsrebar::CommandId::EditUndo, QStringLiteral("Edit.Undo"), QStringLiteral("放弃"), QStringLiteral("tab_start"), QStringLiteral("group_edit"), QStringLiteral("开始/编辑/放弃")},
        {tsrebar::CommandId::EditRedo, QStringLiteral("Edit.Redo"), QStringLiteral("重做"), QStringLiteral("tab_start"), QStringLiteral("group_edit"), QStringLiteral("开始/编辑/重做")},
        {tsrebar::CommandId::EditDeleteSelection, QStringLiteral("Edit.DeleteSelection"), QStringLiteral("删除"), QStringLiteral("tab_start"), QStringLiteral("group_edit"), QStringLiteral("开始/编辑/删除")},
        {tsrebar::CommandId::SelectionPickMode, QStringLiteral("Selection.PickMode"), QStringLiteral("拾取"), QStringLiteral("tab_start"), QStringLiteral("group_reference"), QStringLiteral("开始/参考/拾取")},
        {tsrebar::CommandId::ReferenceCreatePlane, QStringLiteral("Reference.CreatePlane"), QStringLiteral("参考面"), QStringLiteral("tab_start"), QStringLiteral("group_reference"), QStringLiteral("开始/参考/参考面")},
        {tsrebar::CommandId::ReferenceCreateLine, QStringLiteral("Reference.CreateLine"), QStringLiteral("参考线"), QStringLiteral("tab_start"), QStringLiteral("group_reference"), QStringLiteral("开始/参考/参考线")},
        {tsrebar::CommandId::ViewZoomWindow, QStringLiteral("View.ZoomWindow"), QStringLiteral("窗口缩放"), QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("显示/取景/窗口缩放")},
        {tsrebar::CommandId::ViewIso, QStringLiteral("View.Iso"), QStringLiteral("轴测"), QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("显示/视图/轴测")},
        {tsrebar::CommandId::ViewPerpendicularToSelection, QStringLiteral("View.PerpendicularToSelection"), QStringLiteral("垂直于"), QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("显示/视图/垂直于")},
        {tsrebar::CommandId::ViewRotateDiscrete, QStringLiteral("View.Rotate"), QStringLiteral("旋转"), QStringLiteral("tab_view"), QStringLiteral("group_view"), QStringLiteral("显示/视图/旋转")},
        {tsrebar::CommandId::ViewRenderWireframe, QStringLiteral("View.Render.Wireframe"), QStringLiteral("线框"), QStringLiteral("tab_view"), QStringLiteral("group_display_state"), QStringLiteral("显示/显示状态/线框")},
        {tsrebar::CommandId::RebarViewActivate, QStringLiteral("Rebar.View.Activate"), QStringLiteral("激活"), QStringLiteral("tab_view"), QStringLiteral("group_display_state"), QStringLiteral("显示/显示状态/激活")},
        {tsrebar::CommandId::RebarViewDeactivate, QStringLiteral("Rebar.View.Deactivate"), QStringLiteral("钝化"), QStringLiteral("tab_view"), QStringLiteral("group_display_state"), QStringLiteral("显示/显示状态/钝化")},
        {tsrebar::CommandId::RebarStyleOpenManager, QStringLiteral("RebarStyle.OpenManager"), QStringLiteral("钢筋式样"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_settings"), QStringLiteral("钢筋/文件属性/钢筋式样")},
        {tsrebar::CommandId::ProjectSettings, QStringLiteral("Project.Settings"), QStringLiteral("文件设置"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_settings"), QStringLiteral("钢筋/文件属性/文件设置")},
        {tsrebar::CommandId::ProjectSetElevationDatum, QStringLiteral("Project.SetElevationDatum"), QStringLiteral("标高基准面"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_settings"), QStringLiteral("钢筋/文件属性/标高基准面")},
        {tsrebar::CommandId::AppSystemSettings, QStringLiteral("App.SystemSettings"), QStringLiteral("系统设置"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_settings"), QStringLiteral("钢筋/文件属性/系统设置")},
        {tsrebar::CommandId::RebarSectionRingCreate, QStringLiteral("Rebar.Create.SectionRing"), QStringLiteral("剖面圈筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/剖面圈筋")},
        {tsrebar::CommandId::RebarFaceBoundaryCreate, QStringLiteral("Rebar.Create.FaceBoundary"), QStringLiteral("面周边"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/面周边")},
        {tsrebar::CommandId::RebarConcentricCircleCreate, QStringLiteral("Rebar.Create.ConcentricCircle"), QStringLiteral("同心圆"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/同心圆")},
        {tsrebar::CommandId::RebarAngleCreate, QStringLiteral("Rebar.Create.Angle"), QStringLiteral("角度配筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/角度配筋")},
        {tsrebar::CommandId::RebarCustomCreate, QStringLiteral("Rebar.Create.Custom"), QStringLiteral("自配筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/自配筋")},
        {tsrebar::CommandId::RebarGenericCreate, QStringLiteral("Rebar.Create.Generic"), QStringLiteral("钢筋配筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/主要钢筋/钢筋配筋")},
        {tsrebar::CommandId::RebarFixedCenterCreate, QStringLiteral("Rebar.Create.FixedCenter"), QStringLiteral("固中心"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/固中心")},
        {tsrebar::CommandId::RebarStirrupCreate, QStringLiteral("Rebar.Create.Stirrup"), QStringLiteral("箍筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/箍筋")},
        {tsrebar::CommandId::RebarCornerCreate, QStringLiteral("Rebar.Create.Corner"), QStringLiteral("角筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/角筋")},
        {tsrebar::CommandId::RebarOpeningCreate, QStringLiteral("Rebar.Create.Opening"), QStringLiteral("孔口"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/孔口")},
        {tsrebar::CommandId::RebarInsertCreate, QStringLiteral("Rebar.Create.Insert"), QStringLiteral("插筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/插筋")},
        {tsrebar::CommandId::RebarSpiralCreate, QStringLiteral("Rebar.Create.Spiral"), QStringLiteral("螺旋筋"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_create"), QStringLiteral("钢筋/构造钢筋/螺旋筋")},
        {tsrebar::CommandId::RebarMove, QStringLiteral("Rebar.Edit.Move"), QStringLiteral("钢筋移动"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/钢筋移动")},
        {tsrebar::CommandId::RebarCopy, QStringLiteral("Rebar.Edit.Copy"), QStringLiteral("钢筋拷贝"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/钢筋拷贝")},
        {tsrebar::CommandId::RebarGroupMerge, QStringLiteral("Rebar.Group.Merge"), QStringLiteral("组合并"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/组合并")},
        {tsrebar::CommandId::RebarGroupSplit, QStringLiteral("Rebar.Group.Split"), QStringLiteral("组合开"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/组合开")},
        {tsrebar::CommandId::RebarSegmentConnect, QStringLiteral("Rebar.Segment.Connect"), QStringLiteral("段连接"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/段连接")},
        {tsrebar::CommandId::RebarSegmentDisconnect, QStringLiteral("Rebar.Segment.Disconnect"), QStringLiteral("段断开"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/段断开")},
        {tsrebar::CommandId::RebarAddBarCount, QStringLiteral("Rebar.Edit.AddBarCount"), QStringLiteral("增加钢筋根数"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/增加钢筋根数")},
        {tsrebar::CommandId::RebarAddSegment, QStringLiteral("Rebar.Edit.AddSegment"), QStringLiteral("增加钢筋段"), QStringLiteral("tab_rebar"), QStringLiteral("group_rebar_edit"), QStringLiteral("钢筋/钢筋编辑/增加钢筋段")},
        {tsrebar::CommandId::QueryByDiameter, QStringLiteral("Rebar.Query.ByDiameter"), QStringLiteral("按直径"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_query"), QStringLiteral("查询/查询钢筋/按直径")},
        {tsrebar::CommandId::QueryByPart, QStringLiteral("Rebar.Query.ByPart"), QStringLiteral("按部位"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_query"), QStringLiteral("查询/查询钢筋/按部位")},
        {tsrebar::CommandId::QueryUncutOrUnsectioned, QStringLiteral("Rebar.Check.UncutOrUnsectioned"), QStringLiteral("未割钢筋"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/检查/未割钢筋")},
        {tsrebar::CommandId::QueryOutsideStructure, QStringLiteral("Rebar.Check.OutsideStructure"), QStringLiteral("体外钢筋"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/检查/体外钢筋")},
        {tsrebar::CommandId::MeasureGeneric, QStringLiteral("Measure.Generic"), QStringLiteral("测量"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/测量/测量")},
        {tsrebar::CommandId::MeasureStructureProperty, QStringLiteral("Measure.StructureProperty"), QStringLiteral("结构属性"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/测量/结构属性")},
        {tsrebar::CommandId::QueryMinDistance, QStringLiteral("Rebar.Check.MinDistance"), QStringLiteral("主筋"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/测量/主筋")},
        {tsrebar::CommandId::QueryGroupMinDistance, QStringLiteral("Rebar.Check.GroupMinDistance"), QStringLiteral("箍筋"), QStringLiteral("tab_query"), QStringLiteral("group_rebar_check"), QStringLiteral("查询/测量/箍筋")},
        {tsrebar::CommandId::DrawingDefineProjectPlane, QStringLiteral("Drawing.DefineProjectPlane"), QStringLiteral("投影面"), QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_view"), QStringLiteral("工程图/定义视图/投影面")},
        {tsrebar::CommandId::DrawingDefineAxonometricProjection, QStringLiteral("Drawing.DefineAxonometricProjection"), QStringLiteral("轴测投影"), QStringLiteral("tab_drawing"), QStringLiteral("group_drawing_view"), QStringLiteral("工程图/定义视图/轴测投影")},
    };

    for (const auto& expectedCommand : expected) {
        const auto command = tsrebar::legacyUiCommandDefinition(expectedCommand.id);
        if (!command.has_value()) {
            std::cerr << "missing legacy UI command: "
                      << expectedCommand.commandKey.toStdString() << '\n';
            return false;
        }
        if (command->commandKey != expectedCommand.commandKey ||
            command->caption != expectedCommand.caption ||
            command->tabObjectName != expectedCommand.tabObjectName ||
            command->groupObjectName != expectedCommand.groupObjectName ||
            command->legacyUiPath != expectedCommand.legacyUiPath) {
            std::cerr << "legacy UI metadata mismatch: "
                      << expectedCommand.commandKey.toStdString() << '\n';
            return false;
        }
        if (command->uiSurface != tsrebar::LegacyCommandUiSurface::Ribbon) {
            return fail("P1 UI coverage commands must be ribbon entries");
        }
        if (command->evidenceIds.isEmpty() && command->sourceRefs.isEmpty() &&
            command->gapIds.isEmpty()) {
            std::cerr << "missing traceability for "
                      << command->commandKey.toStdString() << '\n';
            return false;
        }
    }

    return true;
}

bool testLegacyUiCommandTraceabilityP1()
{
    const auto exportRebar =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ExportRebarStep);
    if (!exportRebar.has_value() ||
        !exportRebar->evidenceIds.contains(QStringLiteral("E-IDA-014")) ||
        !exportRebar->evidenceIds.contains(QStringLiteral("E-DEV-048"))) {
        return fail("Rebar.Export.SteelStepOrLegacy must retain IDA and STP witness evidence");
    }

    const auto projectNew = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ProjectNew);
    const auto projectOpen = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ProjectOpen);
    const auto projectSave = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ProjectSave);
    const auto projectClose =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ProjectClose);
    if (!projectNew.has_value() || !projectOpen.has_value() || !projectSave.has_value() ||
        !projectClose.has_value() ||
        !projectNew->gapIds.contains(QStringLiteral("GAP-BTN-START-001")) ||
        !projectOpen->gapIds.contains(QStringLiteral("GAP-BTN-START-002")) ||
        !projectSave->gapIds.contains(QStringLiteral("GAP-BTN-START-003")) ||
        !projectClose->gapIds.contains(QStringLiteral("GAP-BTN-START-004"))) {
        return fail("project file commands must retain button-level GAP IDs");
    }

    const auto undo = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::EditUndo);
    const auto redo = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::EditRedo);
    if (!undo.has_value() || !redo.has_value() ||
        !undo->gapIds.contains(QStringLiteral("GAP-BTN-START-005")) ||
        !redo->gapIds.contains(QStringLiteral("GAP-BTN-START-006"))) {
        return fail("Edit undo/redo must keep button-level GAP IDs");
    }

    const auto fitAll = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ViewFitAll);
    const auto pan = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ViewPan);
    const auto zoom = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ViewZoom);
    const auto rotate = tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ViewRotate);
    if (!fitAll.has_value() || !pan.has_value() || !zoom.has_value() ||
        !rotate.has_value() ||
        fitAll->legacyUiPath != QStringLiteral("显示/取景/全显") ||
        pan->legacyUiPath != QStringLiteral("显示/取景/实时平移") ||
        zoom->legacyUiPath != QStringLiteral("显示/取景/实时缩放") ||
        rotate->legacyUiPath != QStringLiteral("显示/取景/实时转动")) {
        return fail("view navigation commands must preserve the old 取景 UI path");
    }

    const auto trimByLine =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByLine);
    const auto trimByFace =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByFace);
    if (!trimByLine.has_value() || !trimByFace.has_value() ||
        trimByLine->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu ||
        trimByFace->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu) {
        return fail("trim commands must remain context menu commands");
    }

    const auto sectionPlane =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::DrawingDefineSectionPlane);
    if (!sectionPlane.has_value() ||
        !sectionPlane->evidenceIds.contains(QStringLiteral("E-SFL-002")) ||
        !sectionPlane->gapIds.contains(QStringLiteral("GAP-FUNC-005"))) {
        return fail("Drawing.DefineSectionPlane must retain SFL evidence and function GAP");
    }

    return true;
}

bool testLegacyRebarCommandContracts()
{
    const auto lineGroup =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarLineCreate);
    if (!lineGroup.has_value()) {
        return fail("Rebar.Create.LineGroup contract is missing");
    }
    if (lineGroup->commandKey != QStringLiteral("Rebar.Create.LineGroup") ||
        lineGroup->legacyCommand != QStringLiteral("sgroupbarline") ||
        lineGroup->legacyUiPath != QStringLiteral("钢筋/主要钢筋/线配筋")) {
        return fail("Rebar.Create.LineGroup contract metadata mismatch");
    }
    if (!lineGroup->evidenceIds.contains(QStringLiteral("E-IDA-002")) ||
        !lineGroup->evidenceIds.contains(QStringLiteral("E-IDA-010")) ||
        !lineGroup->gapIds.contains(QStringLiteral("GAP-IDA-002")) ||
        !lineGroup->inputSelectionTypes.contains(QStringLiteral("single-selected-entity-list"))) {
        return fail("Rebar.Create.LineGroup evidence, gap or input contract mismatch");
    }
    if (lineGroup->uiSurface != tsrebar::LegacyCommandUiSurface::Ribbon) {
        return fail("Rebar.Create.LineGroup must be a ribbon command");
    }
    if (lineGroup->implementationState !=
        tsrebar::LegacyCommandImplementationState::NotImplemented) {
        return fail("Rebar.Create.LineGroup must remain NotImplemented in M1-App-019");
    }
    if (!lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Idle) ||
        !lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Picking) ||
        !lineGroup->interactionStates.contains(tsrebar::LegacyCommandInteractionState::Completed)) {
        return fail("Rebar.Create.LineGroup interaction state contract mismatch");
    }

    const auto arcGroup =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarArcGroupCreate);
    if (!arcGroup.has_value()) {
        return fail("Rebar.Create.ArcGroup contract is missing");
    }
    if (arcGroup->commandKey != QStringLiteral("Rebar.Create.ArcGroup") ||
        arcGroup->legacyCommand != QStringLiteral("sgroupbararc") ||
        arcGroup->legacyUiPath != QStringLiteral("钢筋/主要钢筋/扇形筋")) {
        return fail("Rebar.Create.ArcGroup contract metadata mismatch");
    }
    if (!arcGroup->evidenceIds.contains(QStringLiteral("E-IDA-001")) ||
        !arcGroup->gapIds.contains(QStringLiteral("GAP-IDA-001")) ||
        !arcGroup->inputSelectionTypes.contains(QStringLiteral("single-selected-chain"))) {
        return fail("Rebar.Create.ArcGroup evidence, gap or input contract mismatch");
    }

    const auto trimByLine =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByLine);
    const auto trimByFace =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::RebarGroupTrimByFace);
    if (!trimByLine.has_value() || !trimByFace.has_value()) {
        return fail("trim command contracts are missing");
    }
    if (trimByLine->legacyContextCommandId != QStringLiteral("0x8939") ||
        trimByLine->commandKey != QStringLiteral("RebarGroup.TrimByLine") ||
        !trimByLine->inputSelectionTypes.contains(QStringLiteral("rebar-group+line"))) {
        return fail("RebarGroup.TrimByLine contract metadata mismatch");
    }
    if (trimByLine->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu ||
        !trimByLine->tabObjectName.isEmpty() || !trimByLine->groupObjectName.isEmpty() ||
        !trimByLine->evidenceIds.contains(QStringLiteral("E-CTX-8939"))) {
        return fail("RebarGroup.TrimByLine placement or evidence contract mismatch");
    }
    if (trimByFace->legacyContextCommandId != QStringLiteral("0x8CE5") ||
        trimByFace->commandKey != QStringLiteral("RebarGroup.TrimByFace") ||
        !trimByFace->inputSelectionTypes.contains(QStringLiteral("rebar-group+face"))) {
        return fail("RebarGroup.TrimByFace contract metadata mismatch");
    }
    if (trimByFace->uiSurface != tsrebar::LegacyCommandUiSurface::ContextMenu ||
        !trimByFace->tabObjectName.isEmpty() || !trimByFace->groupObjectName.isEmpty() ||
        !trimByFace->evidenceIds.contains(QStringLiteral("E-CTX-8CE5"))) {
        return fail("RebarGroup.TrimByFace placement or evidence contract mismatch");
    }

    return true;
}

bool testLegacyPlaceholdersRegisterStableNotImplementedHandlers()
{
    tsrebar::CommandRegistry registry;
    tsrebar::registerLegacyUiCommandPlaceholders(registry);

    if (!registry.hasHandler(tsrebar::CommandId::RebarLineCreate) ||
        !registry.hasHandler(tsrebar::CommandId::RebarArcGroupCreate) ||
        !registry.hasHandler(tsrebar::CommandId::RebarGroupTrimByLine) ||
        !registry.hasHandler(tsrebar::CommandId::RebarGroupTrimByFace)) {
        return fail("legacy rebar command placeholders must register handlers");
    }

    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::RebarGroupTrimByFace);
    if (result.status != tsrebar::CommandStatus::NotImplemented) {
        return fail("legacy command placeholder must return NotImplemented");
    }
    if (!result.message.contains(QStringLiteral("RebarGroup.TrimByFace")) ||
        !result.message.contains(QStringLiteral("旧图石行为待确认"))) {
        return fail("legacy command placeholder message is not stable or traceable");
    }

    return true;
}

bool testLegacyPlaceholdersDoNotOverrideRealHandlers()
{
    tsrebar::CommandRegistry registry;
    registry.registerHandler(tsrebar::CommandId::RebarLineCreate, []() {
        return tsrebar::CommandResult{tsrebar::CommandStatus::Completed,
                                      QStringLiteral("real handler")};
    });

    tsrebar::registerLegacyUiCommandPlaceholders(registry);

    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::RebarLineCreate);
    if (result.status != tsrebar::CommandStatus::Completed ||
        result.message != QStringLiteral("real handler")) {
        return fail("legacy placeholder must not override an existing real handler");
    }

    return true;
}

bool testLegacyEvidenceIdsAreNormalized()
{
    const auto commands = tsrebar::legacyUiCommands();
    for (const auto& command : commands) {
        for (const QString& evidenceId : command.evidenceIds) {
            if (!evidenceId.startsWith(QStringLiteral("E-"))) {
                std::cerr << "non-normalized evidence id in "
                          << command.commandKey.toStdString() << ": "
                          << evidenceId.toStdString() << '\n';
                return false;
            }
        }
    }

    const auto openStep =
        tsrebar::legacyUiCommandDefinition(tsrebar::CommandId::ImportStep);
    if (!openStep.has_value() ||
        !openStep->sourceRefs.contains(QStringLiteral("36"))) {
        return fail("non-evidence source references must remain traceable");
    }

    return true;
}

bool testNotImplementedDefault()
{
    const tsrebar::CommandRegistry registry;
    const tsrebar::CommandResult result =
        registry.execute(tsrebar::CommandId::ExportRebarStep);

    if (result.status != tsrebar::CommandStatus::NotImplemented) {
        return fail("unimplemented command must return NotImplemented");
    }

    return true;
}

} // namespace

int main()
{
    if (!testMainTabs()) {
        return 1;
    }
    if (!testLegacyCommandMap()) {
        return 1;
    }
    if (!testLegacyUiCommandCoverageP1()) {
        return 1;
    }
    if (!testLegacyUiCommandTraceabilityP1()) {
        return 1;
    }
    if (!testLegacyRebarCommandContracts()) {
        return 1;
    }
    if (!testLegacyPlaceholdersRegisterStableNotImplementedHandlers()) {
        return 1;
    }
    if (!testLegacyPlaceholdersDoNotOverrideRealHandlers()) {
        return 1;
    }
    if (!testLegacyEvidenceIdsAreNormalized()) {
        return 1;
    }
    if (!testNotImplementedDefault()) {
        return 1;
    }
    return 0;
}
