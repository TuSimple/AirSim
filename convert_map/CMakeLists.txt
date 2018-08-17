CMAKE_MINIMUM_REQUIRED(VERSION 3.9.0)

project(road)

add_definitions("-std=c++11")

add_subdirectory(map-lane-map2)
# add_subdirectory(rapidjson)

set(exeLoadMap "myLoadMap.app")
add_executable( ${exeLoadMap}
	testMap.cpp
)
target_link_libraries( ${exeLoadMap}
	tsmap2
)
target_include_directories( ${exeLoadMap}
	PUBLIC
	${RAPIDJSON_INCLUDE_DIRS}
)


# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
# set(exeJson "myJson.app")
# add_executable( ${exeJson}
# 	testJson.cpp
# )
# target_link_libraries( ${exeJson}
# 	tsmap2
# )
# target_include_directories( ${exeJson}
# 	PUBLIC
# 	${RAPIDJSON_INCLUDE_DIRS}
# )



include_directories(${CMAKE_CURRENT_SOURCE_DIR}/json/include)
set(eJson "myjson.app")
add_executable( ${eJson}
	json.cpp
)



set(exeConvertMap "myConvertMap.app")
add_executable( ${exeConvertMap}
	convertMap.cpp
)
target_link_libraries( ${exeConvertMap}
	tsmap2
)
target_include_directories( ${exeConvertMap}
	PUBLIC
	${RAPIDJSON_INCLUDE_DIRS}
)


