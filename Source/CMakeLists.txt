cmake_minimum_required(VERSION 3.7)
project(PixelLab)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 添加子目录
add_subdirectory(SkyEngine)
add_subdirectory(LibAppFramework)
add_subdirectory(ShaderTest)
add_subdirectory(HDRRendering)
add_subdirectory(IBLTools)
add_subdirectory(IBLValidation)
add_subdirectory(MultithreadedRendering)

# 设置依赖关系
add_dependencies(LibAppFramework SkyEngine)
add_dependencies(ShaderTest SkyEngine LibAppFramework)
add_dependencies(HDRRendering SkyEngine LibAppFramework)
add_dependencies(IBLTools SkyEngine LibAppFramework)
add_dependencies(MultithreadedRendering SkyEngine LibAppFramework)

file(GLOB DLLS_TO_COPY 
"3rdParty/libs/SDL/x64/*.dll"
"3rdParty/libs/SDL_ttf/x64/*.dll"
"3rdParty/libs/GLEW/x64/*.dll"
"3rdParty/dlls/assimp/bin64/*.dll"
)


# 复制所有文件
install(FILES ${DLLS_TO_COPY} DESTINATION  ${CMAKE_BINARY_DIR}/bin)
