# 设置项目名称和支持的 C++ 标准
project(LibAppFramework)
set(CMAKE_CXX_STANDARD 11)

file(GLOB ALL_SRC
    "include/*.h"
    "src/*.cpp"
)

# 添加源代码目录，也可以添加多个源代码目录
add_library(LibAppFramework STATIC  ${ALL_SRC})

include_directories(${PROJECT_SOURCE_DIR}/../3rdParty/include)
include_directories(${PROJECT_SOURCE_DIR}/../SkyEngine/include)

# 指定库的输出位置（可选）
set_target_properties(LibAppFramework 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

# 添加库的包含路径（可选）
target_include_directories(LibAppFramework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )