
include_directories(pugixml-1.5/src)
add_library(viennamesh-pugixml SHARED pugixml-1.5/src/pugixml.cpp)
set(LOCAL_VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES ${LOCAL_VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/pugixml-1.5/src)
set(LOCAL_VIENNAMESH_EXTERNAL_LIBRARIES viennamesh-pugixml)
install(TARGETS viennamesh-pugixml DESTINATION lib)

include_directories(tclap-1.2.1/include)
set(LOCAL_VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES ${LOCAL_VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_SOURCE_DIR}/tclap-1.2.1/include)

SET(VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES ${LOCAL_VIENNAMESH_EXTERNAL_INCLUDE_DIRECTORIES} PARENT_SCOPE)
SET(VIENNAMESH_EXTERNAL_LIBRARIES ${LOCAL_VIENNAMESH_EXTERNAL_LIBRARIES} PARENT_SCOPE)
