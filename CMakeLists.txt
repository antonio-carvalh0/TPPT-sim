#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(sim)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#Uncomment the folowing line to activate GDML support!
#add_compile_definitions(USE_GDML)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 and ROOT libraries
#
add_executable(sim sim.cc ${sources} ${headers})
target_link_libraries(sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy run-time resources to the build directory
#
set(EXTRA_FILES
  vis.mac
  ProductionCrossSections.txt
  CT2Density.dat
  Materials.dat
  ColorMap.dat
  EnergyRangeSigma.txt
  BeamletCalibration.txt
  )

foreach(_file ${EXTRA_FILES})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
    )
endforeach()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/detector.gdml.tar.gz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS sim DESTINATION bin)

