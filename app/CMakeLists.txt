cmake_minimum_required(VERSION 3.22)

project(TsRebarApp LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    add_compile_options(/utf-8 /bigobj)
endif()

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Test)
find_package(OpenCASCADE CONFIG REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TSREBAR_QT_PLATFORM_PLUGIN_PATH "" CACHE PATH
    "Qt platform plugin directory for smoke tests")
if(NOT TSREBAR_QT_PLATFORM_PLUGIN_PATH)
    if(EXISTS "D:/Work/vcpkg/installed/x64-windows/debug/Qt6/plugins/platforms")
        set(TSREBAR_QT_PLATFORM_PLUGIN_PATH
            "D:/Work/vcpkg/installed/x64-windows/debug/Qt6/plugins/platforms")
    elseif(EXISTS "D:/Work/vcpkg/installed/x64-windows/Qt6/plugins/platforms")
        set(TSREBAR_QT_PLATFORM_PLUGIN_PATH
            "D:/Work/vcpkg/installed/x64-windows/Qt6/plugins/platforms")
    endif()
endif()

if(DEFINED OpenCASCADE_INCLUDE_DIR)
    set(TSREBAR_OCCT_INCLUDE_DIR "${OpenCASCADE_INCLUDE_DIR}")
elseif(DEFINED OpenCASCADE_INCLUDE_DIRS)
    set(TSREBAR_OCCT_INCLUDE_DIR "${OpenCASCADE_INCLUDE_DIRS}")
else()
    find_path(TSREBAR_OCCT_INCLUDE_DIR NAMES Standard.hxx)
endif()

if(NOT TSREBAR_OCCT_INCLUDE_DIR)
    message(FATAL_ERROR "Missing OCCT include directory.")
endif()

set(TSREBAR_OCCT_STEP_LIBS
    TKernel
    TKMath
    TKG2d
    TKG3d
    TKGeomBase
    TKBRep
    TKGeomAlgo
    TKTopAlgo
    TKOffset
    TKShHealing
    TKMesh
    TKCDF
    TKLCAF
    TKCAF
    TKVCAF
    TKDE
    TKXSBase
    TKDESTEP
    TKXCAF)

set(TSREBAR_OCCT_VIEWER_LIBS
    TKService
    TKV3d
    TKOpenGl)

add_library(tsrebar_core STATIC
    src/command/CommandRegistry.cpp
    src/command/CommandRegistry.h
    src/command/CommandId.h
    src/command/LegacyUiCommandMap.cpp
    src/command/LegacyUiCommandMap.h
    src/command/RebarLineGroupCommandHandler.cpp
    src/command/RebarLineGroupCommandHandler.h
    src/drawing/detail/DetailWriter.cpp
    src/drawing/detail/DetailWriter.h
    src/geometry/legacy/LegacyGeometry.h
    src/geometry/legacy/LegacySelectionRef.cpp
    src/domain/rebar/RebarGroupCreator.cpp
    src/domain/rebar/RebarGroupCreator.h
    src/domain/rebar/RebarEditCopyService.cpp
    src/domain/rebar/RebarEditCopyService.h
    src/domain/rebar/RebarEditMoveService.cpp
    src/domain/rebar/RebarEditMoveService.h
    src/domain/rebar/RebarScheduleService.cpp
    src/domain/rebar/RebarScheduleService.h
    src/domain/rebar/RebarDomainTypes.h
    src/domain/rebar/SteelBar.h
    src/domain/rebar/SteelBarGroup.h
    src/domain/rebar/SteelBarSegment.h
    src/domain/rebar/SteelData.h
    src/geometry/legacy/LegacySelectionRef.h
    src/project/runtime/TsRebarProjectRuntime.cpp
    src/project/runtime/TsRebarProjectRuntime.h
    src/ui/MainTabs.cpp
    src/ui/MainTabs.h
    src/ui/ModelTreePanel.cpp
    src/ui/ModelTreePanel.h)

target_include_directories(tsrebar_core PUBLIC src)
target_link_libraries(tsrebar_core PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)

add_library(tsrebar_occ_import STATIC
    src/geometry/occ/import/OcctStepImportService.cpp
    src/geometry/occ/import/OcctStepImportService.h
    src/geometry/occ/import/OcctImportedDocument.h
    src/import/StepImportResult.h)

target_include_directories(tsrebar_occ_import PUBLIC
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(tsrebar_occ_import PUBLIC
    Qt6::Core
    tsrebar_core
    ${TSREBAR_OCCT_STEP_LIBS})

add_library(tsrebar_occ_selection STATIC
    src/geometry/occ/selection/OccSelectionIndex.cpp
    src/geometry/occ/selection/OccSelectionIndex.h)

target_include_directories(tsrebar_occ_selection PUBLIC
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(tsrebar_occ_selection PUBLIC
    Qt6::Core
    tsrebar_core
    tsrebar_occ_import
    ${TSREBAR_OCCT_STEP_LIBS})

add_library(tsrebar_occ_legacy_adapter STATIC
    src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.cpp
    src/geometry/occ/legacy_adapter/OccLegacyGeometryAdapter.h)

target_include_directories(tsrebar_occ_legacy_adapter PUBLIC
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(tsrebar_occ_legacy_adapter PUBLIC
    Qt6::Core
    tsrebar_core
    tsrebar_occ_selection
    ${TSREBAR_OCCT_STEP_LIBS})

add_library(tsrebar_occ_rebar_presentation STATIC
    src/presentation/occ/RebarAisPresentationAdapter.cpp
    src/presentation/occ/RebarAisPresentationAdapter.h)

target_include_directories(tsrebar_occ_rebar_presentation PUBLIC
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(tsrebar_occ_rebar_presentation PUBLIC
    Qt6::Core
    tsrebar_core
    ${TSREBAR_OCCT_STEP_LIBS}
    ${TSREBAR_OCCT_VIEWER_LIBS})

add_library(tsrebar_occ_viewer STATIC
    src/presentation/occ/OccViewerWidget.cpp
    src/presentation/occ/OccViewerWidget.h)

target_include_directories(tsrebar_occ_viewer PUBLIC
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(tsrebar_occ_viewer PUBLIC
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    tsrebar_occ_import
    tsrebar_occ_selection
    ${TSREBAR_OCCT_STEP_LIBS}
    ${TSREBAR_OCCT_VIEWER_LIBS})

add_executable(tsrebar_app
    src/app/main.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.h)

target_link_libraries(tsrebar_app PRIVATE tsrebar_core)
target_link_libraries(tsrebar_app PRIVATE tsrebar_occ_import)
target_link_libraries(tsrebar_app PRIVATE tsrebar_occ_legacy_adapter)
target_link_libraries(tsrebar_app PRIVATE tsrebar_occ_rebar_presentation)
target_link_libraries(tsrebar_app PRIVATE tsrebar_occ_viewer)

if(TSREBAR_QT_PLATFORM_PLUGIN_PATH)
    add_custom_command(TARGET tsrebar_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:tsrebar_app>/platforms
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${TSREBAR_QT_PLATFORM_PLUGIN_PATH}
            $<TARGET_FILE_DIR:tsrebar_app>/platforms
        VERBATIM)
endif()

add_executable(command_registry_tests
    tests/unit/command_registry_tests.cpp)

target_link_libraries(command_registry_tests PRIVATE tsrebar_core)

add_executable(legacy_selection_ref_tests
    tests/unit/legacy_selection_ref_tests.cpp)

target_link_libraries(legacy_selection_ref_tests PRIVATE tsrebar_core)

add_executable(rebar_domain_model_tests
    tests/unit/rebar_domain_model_tests.cpp)

target_link_libraries(rebar_domain_model_tests PRIVATE tsrebar_core)

add_executable(rebar_group_creator_tests
    tests/unit/rebar_group_creator_tests.cpp)

target_link_libraries(rebar_group_creator_tests PRIVATE tsrebar_core)

add_executable(rebar_line_group_command_handler_tests
    tests/unit/rebar_line_group_command_handler_tests.cpp)

target_link_libraries(rebar_line_group_command_handler_tests PRIVATE tsrebar_core)

add_executable(rebar_edit_move_tests
    tests/unit/rebar_edit_move_tests.cpp)

target_link_libraries(rebar_edit_move_tests PRIVATE tsrebar_core)

add_executable(rebar_edit_copy_tests
    tests/unit/rebar_edit_copy_tests.cpp)

target_link_libraries(rebar_edit_copy_tests PRIVATE tsrebar_core)

add_executable(rebar_schedule_service_tests
    tests/unit/rebar_schedule_service_tests.cpp)

target_link_libraries(rebar_schedule_service_tests PRIVATE tsrebar_core)

add_executable(rebar_ais_presentation_tests
    tests/unit/rebar_ais_presentation_tests.cpp)

target_include_directories(rebar_ais_presentation_tests PRIVATE
    src
    "${TSREBAR_OCCT_INCLUDE_DIR}")

target_link_libraries(rebar_ais_presentation_tests PRIVATE tsrebar_occ_rebar_presentation)

add_executable(project_runtime_tests
    tests/unit/project_runtime_tests.cpp)

target_link_libraries(project_runtime_tests PRIVATE tsrebar_core)

add_executable(detail_writer_tests
    tests/unit/detail_writer_tests.cpp)

target_link_libraries(detail_writer_tests PRIVATE tsrebar_core)

add_executable(detail_l2_fixture_probe
    tools/detail_l2_fixture_probe.cpp)

target_link_libraries(detail_l2_fixture_probe PRIVATE tsrebar_core)

add_executable(step_import_probe
    src/step/StepImportProbe.cpp)

target_link_libraries(step_import_probe PRIVATE tsrebar_occ_import)

add_executable(viewer_smoke_tests
    tests/integration/viewer_smoke_tests.cpp)

target_link_libraries(viewer_smoke_tests PRIVATE tsrebar_occ_viewer)

add_executable(viewer_selection_tests
    tests/integration/viewer_selection_tests.cpp)

target_link_libraries(viewer_selection_tests PRIVATE tsrebar_occ_viewer tsrebar_occ_selection)

add_executable(line_group_display_smoke_tests
    tests/integration/line_group_display_smoke_tests.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.h)

target_link_libraries(line_group_display_smoke_tests PRIVATE
    tsrebar_core
    tsrebar_occ_import
    tsrebar_occ_legacy_adapter
    tsrebar_occ_rebar_presentation
    tsrebar_occ_viewer)

add_executable(occ_selection_index_tests
    tests/integration/occ_selection_index_tests.cpp)

target_link_libraries(occ_selection_index_tests PRIVATE tsrebar_occ_selection)

add_executable(legacy_geometry_adapter_tests
    tests/integration/legacy_geometry_adapter_tests.cpp)

target_link_libraries(legacy_geometry_adapter_tests PRIVATE tsrebar_occ_legacy_adapter)

add_test(NAME command_registry_tests COMMAND command_registry_tests)
add_test(NAME legacy_selection_ref_tests COMMAND legacy_selection_ref_tests)
add_test(NAME rebar_domain_model_tests COMMAND rebar_domain_model_tests)
add_test(NAME rebar_group_creator_tests COMMAND rebar_group_creator_tests)
add_test(NAME rebar_line_group_command_handler_tests
    COMMAND rebar_line_group_command_handler_tests)
add_test(NAME rebar_edit_move_tests COMMAND rebar_edit_move_tests)
add_test(NAME rebar_edit_copy_tests COMMAND rebar_edit_copy_tests)
add_test(NAME rebar_schedule_service_tests COMMAND rebar_schedule_service_tests)
add_test(NAME rebar_ais_presentation_tests COMMAND rebar_ais_presentation_tests)
add_test(NAME project_runtime_tests COMMAND project_runtime_tests)
add_test(NAME detail_writer_tests COMMAND detail_writer_tests)
add_test(
    NAME detail_l2_fixture_probe_tests
    COMMAND detail_l2_fixture_probe
        ${CMAKE_CURRENT_BINARY_DIR}/detail_l2_fixture_probe_test_package
        --fixture others-steeljoint
        --views 3)
add_test(
    NAME detail_l2_fixture_probe_environment_tests
    COMMAND Python3::Interpreter
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/test_detail_l2_fixture_probe_environment.py
        $<TARGET_FILE:detail_l2_fixture_probe>)
add_test(NAME app_smoke COMMAND tsrebar_app --smoke)
set_tests_properties(app_smoke PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=minimal;QT_QPA_PLATFORM_PLUGIN_PATH=${TSREBAR_QT_PLATFORM_PLUGIN_PATH}")
add_test(NAME viewer_smoke_tests COMMAND viewer_smoke_tests)
set_tests_properties(viewer_smoke_tests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=minimal;QT_QPA_PLATFORM_PLUGIN_PATH=${TSREBAR_QT_PLATFORM_PLUGIN_PATH}")
add_test(
    NAME domain_rebar_boundary
    COMMAND Python3::Interpreter
        ${CMAKE_CURRENT_SOURCE_DIR}/tools/check_domain_rebar_boundary.py
        ${CMAKE_CURRENT_SOURCE_DIR}/src/domain/rebar)

set(TSREBAR_123_STP "${CMAKE_CURRENT_SOURCE_DIR}/../../123.stp")
if(EXISTS "${TSREBAR_123_STP}")
    add_test(
        NAME step_import_probe_123
        COMMAND Python3::Interpreter
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/test_step_import_probe.py
            $<TARGET_FILE:step_import_probe>
            ${TSREBAR_123_STP})
    add_test(
        NAME occ_selection_index_123
        COMMAND occ_selection_index_tests
            ${TSREBAR_123_STP})
    add_test(
        NAME legacy_geometry_adapter_123
        COMMAND legacy_geometry_adapter_tests
            ${TSREBAR_123_STP})
    add_test(
        NAME line_group_display_smoke_123
        COMMAND line_group_display_smoke_tests
            ${TSREBAR_123_STP})
    set_tests_properties(line_group_display_smoke_123 PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=windows;QT_QPA_PLATFORM_PLUGIN_PATH=${TSREBAR_QT_PLATFORM_PLUGIN_PATH}"
        TIMEOUT 120)
endif()
