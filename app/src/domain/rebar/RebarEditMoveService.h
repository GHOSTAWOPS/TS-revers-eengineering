#pragma once

#include "domain/rebar/RebarDomainTypes.h"
#include "domain/rebar/SteelData.h"

#include <string>
#include <vector>

namespace tsrebar {

struct RebarMoveRequest
{
    std::vector<std::string> groupIds;
    DomainPoint3d translation;
    std::string commandId = "Rebar.Edit.Move";
    std::vector<RebarEvidenceRef> evidence;
};

struct RebarMoveResult
{
    bool ok = false;
    std::string diagnostic;
    int movedGroupCount = 0;
    int movedBarCount = 0;
    int movedSegmentCount = 0;
};

class RebarEditMoveService
{
public:
    // 对齐旧 barmove copyFlag=0：只移动原钢筋对象，保留 group/bar/segment 身份。
    [[nodiscard]] RebarMoveResult moveGroups(SteelData& steelData,
                                             const RebarMoveRequest& request) const;
};

} // namespace tsrebar
