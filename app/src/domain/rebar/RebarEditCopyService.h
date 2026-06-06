#pragma once

#include "domain/rebar/RebarDomainTypes.h"
#include "domain/rebar/SteelData.h"

#include <string>
#include <vector>

namespace tsrebar {

struct RebarCopyIdMap
{
    std::string sourceId;
    std::string newId;
};

struct RebarCopyIdSet
{
    int copyIndex = 0;
    std::string sourceGroupId;
    std::string newGroupId;
    std::vector<RebarCopyIdMap> barIds;
    std::vector<RebarCopyIdMap> segmentIds;
};

struct RebarCopyRequest
{
    std::vector<std::string> groupIds;
    DomainPoint3d translation;
    int copyCount = 1;
    std::string commandId = "Rebar.Edit.Copy";
    std::vector<RebarCopyIdSet> idSets;
    std::vector<RebarEvidenceRef> evidence;
};

struct RebarCopyResult
{
    bool ok = false;
    std::string diagnostic;
    int copiedGroupCount = 0;
    int copiedBarCount = 0;
    int copiedSegmentCount = 0;
    std::vector<std::string> newGroupIds;
};

class RebarEditCopyService
{
public:
    // 对齐旧 scopy copyFlag=1 P0：先复制钢筋组对象，再对复制件应用平移变换。
    [[nodiscard]] RebarCopyResult copyGroups(SteelData& steelData,
                                             const RebarCopyRequest& request) const;
};

} // namespace tsrebar
