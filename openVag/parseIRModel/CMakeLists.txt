set(Sources
	IRXmlRep.h
	parseIRModel.cpp
	parseIRModel.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Sources})

add_library(parseIRModel ${Sources})

find_package(tinyxml2 REQUIRED)
target_link_libraries(parseIRModel PRIVATE tinyxml2)
target_include_directories(parseIRModel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


set(SourcesExe
	main.cpp)

add_executable(parseIRModelExe ${SourcesExe})
target_link_libraries(parseIRModelExe PRIVATE parseIRModel)