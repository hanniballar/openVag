set(Sources
	main.cpp
	IRXmlRep.h
	LayerNode.h
	parseIRModel.cpp
)

message(STATUS "CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_executable(parseIRModel ${Sources})

find_package(tinyxml2 REQUIRED)
target_link_libraries(parseIRModel PRIVATE tinyxml2)