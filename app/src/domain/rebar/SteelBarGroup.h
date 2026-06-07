#pragma once

#include "domain/rebar/RebarDomainTypes.h"

#include <string>
#include <vector>

namespace tsrebar {

struct SteelBarGroup
{
    std::string id; // Backward-compatible alias until older callers move to groupId.
    std::string groupId;
    std::string rsdId;
    std::string name;
    std::string displayNumber;
    std::string actualNumber;
    std::string componentName;
    std::string projectSteelName;
    std::string legacyCommand;
    std::string createCommand;
    std::string styleId;
    std::string steelDataId;
    double diameter = 0.0;
    double secondaryDiameter = 0.0;
    double interval = 0.0;
    int barCount = 0;
    int segmentCount = 0;
    std::string steelLevel;
    std::string layer;
    std::string profile;
    std::string use;
    bool rangeLess180 = false;
    std::string steelWay;
    std::string rebarType;
    double offsetInOut = 0.0;
    bool active = true;
    bool visible = true;
    std::string sourceFaceId;
    std::string sourceLineId;
    std::vector<std::string> sourceCurveIds;
    std::vector<std::string> referenceIds;
    std::vector<std::string> barIds;
    LegacyRawBlock createdFromParameters;
    LegacyRawBlock legacyRaw;
    GeometryReference geometryRef;
    BindingAnchor binding;
    std::vector<RebarFaceEdgeGeometry> faceEdges;
    std::vector<UnresolvedLegacyField> unresolvedLegacyFields;
    std::vector<RebarEvidenceRef> evidence;
};

} // namespace tsrebar
