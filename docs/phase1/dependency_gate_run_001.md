# Dependency Gate Run 001

## Summary

- Created at: 2026-06-05T03:28:55+08:00
- Scope: Qt6 + OCCT dependency compliance gate
- Result: pass
- M1-Formal allowed: yes
- Checks: 16/17 pass
- Errors: 0
- Warnings: 1
- Blocker GAPs: none

## Findings

| Check | Severity | OK | File | Evidence | Decision |
|---|---|---|---|---|---|
| DEPENDENCIES_EXISTS | error | yes | `DEPENDENCIES.md` | DEPENDENCIES.md | present |
| NOTICES_EXISTS | error | yes | `THIRD_PARTY_NOTICES.md` | THIRD_PARTY_NOTICES.md | present |
| QT_LICENSE_EXISTS | error | yes | `licenses/QT_LICENSES.txt` | Qt license record | present |
| OCCT_LICENSE_EXISTS | error | yes | `licenses/OCCT_LICENSE.txt` | OCCT license record | present |
| SOURCE_OFFER_EXISTS | error | yes | `licenses/SOURCE_OFFER.txt` | source availability record | present |
| SBOM_EXISTS | error | yes | `sbom/project-sbom.spdx.json` | SPDX SBOM | present |
| DEP_VERSION_FREEZE | error | yes | `DEPENDENCIES.md` | no prep markers | dependency versions frozen |
| DEP_VERSION_FREEZE | error | yes | `THIRD_PARTY_NOTICES.md` | no prep markers | dependency versions frozen |
| DEP_VERSION_FREEZE | error | yes | `licenses/QT_LICENSES.txt` | no prep markers | dependency versions frozen |
| DEP_VERSION_FREEZE | error | yes | `licenses/OCCT_LICENSE.txt` | no prep markers | dependency versions frozen |
| DEP_VERSION_FREEZE | error | yes | `licenses/SOURCE_OFFER.txt` | no prep markers | dependency versions frozen |
| NOTICE_SOURCE_MODIFIED | error | yes | `THIRD_PARTY_NOTICES.md` | Source modified: no | source modification state recorded |
| QT_SOURCE_MODIFIED | error | yes | `licenses/QT_LICENSES.txt` | Source modified: no | source modification state recorded |
| OCCT_SOURCE_MODIFIED | error | yes | `licenses/OCCT_LICENSE.txt` | Source modified: no | source modification state recorded |
| SBOM_REQUIRED_PACKAGES | error | yes | `sbom/project-sbom.spdx.json` | Qt and OCCT packages present | required packages present |
| SBOM_VERSION_FREEZE | error | yes | `sbom/project-sbom.spdx.json` | no prep markers | SBOM versions frozen |
| CMAKE_SCAN | warning | no | `CMakeLists.txt` | missing | CMake project not created yet; dependency module scan skipped |

## Linked Requirements

- REQ-TECH-001
- REQ-TECH-002

## Linked Evidence

- E-TECH-001
- E-TECH-002
- E-TECH-003
- E-TECH-004
- E-TECH-005

## Linked GAP

- GAP-TECH-001
- GAP-TECH-002
- GAP-TECH-007
- GAP-DEV-007

## Gate Boundary

- This run checks the frozen local vcpkg dependency candidate.
- A pass means dependency files, notices, source availability records and SBOM no longer contain preparation placeholders.
- CMake module scanning is still a warning until the Qt6 + OCCT project is created.
- Installed modules are not automatically approved link modules; the CMake allowlist remains authoritative.
