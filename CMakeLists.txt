#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(musrSim_mst)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#----------------------------------------------------------------------------
# Use the JP/SMS root-config discovery path.
include(FindROOT.cmake)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/third_party/EcoMug)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
option(ENABLE_CRY "Enable CRY cosmic shower source" OFF)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*.cpp)
if(ENABLE_CRY)
  list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/FTFP_BERT.cc ${PROJECT_SOURCE_DIR}/src/G4EmExtraPhysics.cc)
endif()
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh ${PROJECT_SOURCE_DIR}/include/*.h)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(musrSim_mst musrSim.cc ${sources} ${headers})
target_link_libraries(musrSim_mst ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
set(CRY_ROOT "/home/liyifei/cry/cry_v1.7" CACHE PATH "CRY 1.7 source and data")
if(ENABLE_CRY)
  if(NOT EXISTS "${CRY_ROOT}/src/CRYGenerator.h" OR NOT EXISTS "${CRY_ROOT}/data/cosmics_0.data")
    message(FATAL_ERROR "CRY source or sea-level data missing under CRY_ROOT")
  endif()
  file(GLOB CRY_SOURCES "${CRY_ROOT}/src/*.cc")
  add_library(cry_source STATIC ${CRY_SOURCES})
  target_include_directories(cry_source PUBLIC "${CRY_ROOT}/src")
  target_compile_definitions(musrSim_mst PRIVATE ENABLE_CRY CRY_DATA_DIR="${CRY_ROOT}/data")
  target_link_libraries(musrSim_mst cry_source)
endif()

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(EXAMPLEB1_SCRIPTS
  run/101.mac
  run/102.mac
  )

foreach(_script ${EXAMPLEB1_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(musrSim DEPENDS musrSim)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS musrSim_mst DESTINATION bin)

option(BUILD_PLANE_TRUTH_TESTS "Build the independent four-plane crossing test" ON)
if(BUILD_PLANE_TRUTH_TESTS)
  enable_testing()
  add_executable(primary_muon_plane_truth_test tests/primary_muon_plane_truth_test.cc)
  add_test(NAME primary_muon_plane_truth COMMAND primary_muon_plane_truth_test)
  add_executable(ecomug_api_test tests/ecomug_api_test.cc)
  add_test(NAME ecomug_api COMMAND ecomug_api_test)
endif()



add_executable(material_boundary_truth_test tests/material_boundary_truth_test.cc)
target_compile_options(material_boundary_truth_test PRIVATE -UNDEBUG)
if(BUILD_PLANE_TRUTH_TESTS)
  add_test(NAME material_boundary_truth COMMAND material_boundary_truth_test)
endif()

add_executable(roi_truth_test tests/roi_truth_test.cc)
target_compile_options(roi_truth_test PRIVATE -UNDEBUG)
if(BUILD_PLANE_TRUTH_TESTS)
  add_test(NAME roi_truth COMMAND roi_truth_test)
endif()

add_executable(cry_truth_test tests/cry_truth_test.cc)
target_compile_options(cry_truth_test PRIVATE -UNDEBUG)
if(BUILD_PLANE_TRUTH_TESTS)
  add_test(NAME cry_truth COMMAND cry_truth_test)
endif()
