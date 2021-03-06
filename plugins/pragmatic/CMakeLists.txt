#Add the directory containing all header files from pragmatic (note that pragmatic is header only!)
set(PRAGMATIC_INCLUDES "${CMAKE_SOURCE_DIR}/plugins/pragmatic/headers")
include_directories(${PRAGMATIC_INCLUDES})
message (STATUS "Pragmatic includes: ${PRAGMATIC_INCLUDES}")

#find METIS package
find_package(METIS QUIET)
if (METIS_FOUND)
  message(STATUS "Found METIS")
  message(STATUS "METIS include directories: ${METIS_INCLUDE_DIRS}")
  message(STATUS "METIS libraries: ${METIS_LIBRARIES}")
  include_directories(${METIS_INCLUDE_DIRS})
else()
  message(STATUS "METIS not found")
endif()

#Set flag if OpenMP has been found on the system or output an error if not, since it is required for the use of pragmatic
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-DHAVE_OPENMP)
    message(STATUS "Found OPENMP")
    message(STATUS "${CMAKE_C_FLAGS}")
    message(STATUS "${CMAKE_CXX_FLAGS}")
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

#find boost packages
find_package(Boost COMPONENTS REQUIRED timer)
#include_directories( ${Boost_INCLUDE_DIRS} )

#Add plugin to ViennaMesh
VIENNAMESH_ADD_PLUGIN(viennamesh-module-pragmatic 	plugin.cpp
							pragmatic_coarsen.cpp
							pragmatic_refine.cpp
							pragmatic_smooth.cpp
							pragmatic_swapping.cpp
              pragmatic_metis_partitioning.cpp              
							pragmatic_refine_multi_region.cpp)
if (METIS_FOUND)
  target_link_libraries(viennamesh-module-pragmatic ${METIS_LIBRARIES})
endif()

target_link_libraries(viennamesh-module-pragmatic ${Boost_LIBRARIES})

#set(VTK_DIR "/home/lgnam/Desktop/software/VTK-build")
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
target_link_libraries(viennamesh-module-pragmatic ${VTK_LIBRARIES})