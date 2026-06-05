# Dependency Probe Run 001

## Summary

- Created at: 2026-06-05T03:17:24+08:00
- Scope: Local Qt6 + OCCT vcpkg probe
- vcpkg root: `D:\Work\vcpkg`
- Triplet: `x64-windows`
- Qt package: `qtbase 6.10.3#1`
- OCCT package: `opencascade 7.9.3#1`
- Qt CMake config exists: yes
- OCCT CMake config exists: yes
- Can freeze from local vcpkg: yes

## Qt Runtime

Recommended Phase 1 runtime DLLs:

- `Qt6Core.dll`
- `Qt6Gui.dll`
- `Qt6OpenGL.dll`
- `Qt6OpenGLWidgets.dll`
- `Qt6Test.dll`
- `Qt6Widgets.dll`

Installed Qt DLLs requiring review before project linking:

- `Qt6Concurrent.dll`
- `Qt6DBus.dll`
- `Qt6Network.dll`
- `Qt6PrintSupport.dll`
- `Qt6Sql.dll`
- `Qt6Xml.dll`

## OCCT Runtime

Installed OCCT TK/TKernel DLLs:

- `TKBO.dll`
- `TKBRep.dll`
- `TKBin.dll`
- `TKBinL.dll`
- `TKBinTObj.dll`
- `TKBinXCAF.dll`
- `TKBool.dll`
- `TKCAF.dll`
- `TKCDF.dll`
- `TKDE.dll`
- `TKDECascade.dll`
- `TKDEGLTF.dll`
- `TKDEIGES.dll`
- `TKDEOBJ.dll`
- `TKDEPLY.dll`
- `TKDESTEP.dll`
- `TKDESTL.dll`
- `TKDEVRML.dll`
- `TKFeat.dll`
- `TKFillet.dll`
- `TKG2d.dll`
- `TKG3d.dll`
- `TKGeomAlgo.dll`
- `TKGeomBase.dll`
- `TKHLR.dll`
- `TKLCAF.dll`
- `TKMath.dll`
- `TKMesh.dll`
- `TKMeshVS.dll`
- `TKOffset.dll`
- `TKOpenGl.dll`
- `TKPrim.dll`
- `TKRWMesh.dll`
- `TKService.dll`
- `TKShHealing.dll`
- `TKStd.dll`
- `TKStdL.dll`
- `TKTObj.dll`
- `TKTopAlgo.dll`
- `TKV3d.dll`
- `TKVCAF.dll`
- `TKXCAF.dll`
- `TKXMesh.dll`
- `TKXSBase.dll`
- `TKXml.dll`
- `TKXmlL.dll`
- `TKXmlTObj.dll`
- `TKXmlXCAF.dll`
- `TKernel.dll`

## Source And License Local Evidence

- Qt source archives: qtbase-everywhere-src-6.10.3.tar.xz
- OCCT source archives: Open-Cascade-SAS-OCCT-V7_9_3.tar.gz
- Qt copyright file: `D:\Work\vcpkg\installed\x64-windows\share\qtbase\copyright`
- OCCT copyright file: `D:\Work\vcpkg\installed\x64-windows\share\opencascade\copyright`

## Gate Boundary

- This probe proves local candidate dependencies exist.
- It does not prove final release packaging is complete.
- It does not allow linking installed-but-unapproved modules.
- Formal readiness still depends on `Build.DependencyGate` and Qt6 application runtime reports.
