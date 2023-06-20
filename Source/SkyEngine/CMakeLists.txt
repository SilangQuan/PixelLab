# 设置项目名称和支持的 C++ 标准
project(SkyEngine)
set(CMAKE_CXX_STANDARD 11)

file(GLOB ALL_SRC
	"include/Render/*.h"
	"include/PostProcess/*.h"
	"include/Math/*.h"
	"include/Log/*.h"
	"include/Lighting/*.h"
	"include/ImGui/*.h"
    "include/Core/*.h"
	"include/Components/*.h"
    "include/*.h"
	"*.hpp"
    "src/Components/*.cpp"
    "src/Core/*.cpp"
    "src/ImGui/*.cpp"
    "src/Lighting/*.cpp"
    "src/Log/*.cpp"
    "src/Math/*.cpp"
    "src/PostProcess/*.cpp"
    "src/Render/*.cpp"
)

# 添加源代码目录，也可以添加多个源代码目录
add_library(SkyEngine STATIC  ${ALL_SRC})

include_directories(${PROJECT_SOURCE_DIR}/../3rdParty/include)

# 指定库的输出位置（可选）
set_target_properties(SkyEngine 
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

# 添加库的包含路径（可选）
target_include_directories(SkyEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )