# 设置项目名称和支持的 C++ 标准
project(IBLValidation)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)



file(GLOB ALL_SRC
    "include/*.h"
    "src/*.cpp"
)
include_directories(
    ${PROJECT_SOURCE_DIR}/include 
)
include_directories(${PROJECT_SOURCE_DIR}/../3rdParty/include)
include_directories(${PROJECT_SOURCE_DIR}/../SkyEngine/include)
include_directories(${PROJECT_SOURCE_DIR}/../LibAppFramework/include)

# 添加源代码目录，也可以添加多个源代码目录
add_executable(IBLValidation ${ALL_SRC})

if(MSVC)
  # 设置工作目录
  set_property(TARGET IBLValidation PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
  set_property(TARGET IBLValidation PROPERTY VS_RELEASE_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()
