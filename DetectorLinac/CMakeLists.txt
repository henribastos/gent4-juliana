#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(DetectorLinac)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()


#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})
                    
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(DetectorLinac DetectorLinac.cc ${headers} ${sources})

target_link_libraries(DetectorLinac ${Geant4_LIBRARIES} )


#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build DetectorLinac. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#

# the macros
set(DetectorLinac_MACROS
    run.mac vis.mac
    )
 
# original set of DetectorLinac data
set(DetectorLinac_SCRIPTS
   
  )
  
# new DetectorLinac data (in share directory)
set(DetectorLinac_SHARE
   
  )

foreach(_script ${DetectorLinac_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

foreach(_script ${DetectorLinac_SHARE})
  configure_file(
    ${PROJECT_SOURCE_DIR}/share/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

foreach(_script ${DetectorLinac_MACROS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS DetectorLinac DESTINATION bin)

