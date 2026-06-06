#pragma once

namespace tsrebar {

enum class CommandId {
    ProjectNew,
    ProjectOpen,
    ProjectSave,
    ProjectClose,
    ImportStep,
    ExportRebarStep,
    ViewFitAll,
    ViewSelectFace,
    ViewSelectEdge,
    ViewSelectVertex,
    ViewPan,
    ViewZoom,
    ViewRotate,
    RebarFaceCreate,
    RebarLineCreate,
    RebarArcGroupCreate,
    RebarGroupTrimByLine,
    RebarGroupTrimByFace,
    QueryByNumber,
    QueryCollision,
    DrawingDefineSectionPlane,
    DrawingGeneratePackage,
    DrawingGenerateBarSchedule
};

} // namespace tsrebar
