# 不可以直接在源码目录里头编译，防止污染源码
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "
    FATAL: In-source builds are not allowed.
           You should create a separate directory for build files.
")
endif()

# 检测一下当前是不是子工程
if (NOT DEFINED SML_IsMaster)
    set(SML_IsMaster OFF)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR) # 若当前所在目录是根目录，则为主工程
        set(SML_IsMaster ON)
        message(STATUS "CMake version: ${CMAKE_VERSION}")
    endif ()
endif ()

cmake_minimum_required(VERSION 3.25)

# 将传入的所有参数连接成一个字符串，随后保存到 ${result_var} 中
function(MergeString OutputVariable)
    set(Result "")
    foreach (i ${ARGN})
        set(Result "${Result}${i}")
    endforeach ()
    set(${OutputVariable} "${Result}" PARENT_SCOPE)
endfunction()

# 把set(...)增强一下
# CMake 3.4的cmake_parse_arguments无法解析CACHE关键字，所以使用list操作手动解析参数
function(SetCacheVariable)
    # 获取第一个参数作为变量名
    list(GET ARGN 0 var)
    list(REMOVE_AT ARGN 0)

    # 获取第二个参数作为值
    list(GET ARGN 0 val)
    list(REMOVE_AT ARGN 0)

    # 获取第三个参数作为关键字
    list(GET ARGN 0 opt)
    list(REMOVE_AT ARGN 0)

    # 获取类型参数（如STRING, BOOL等）
    list(GET ARGN 0 type)
    list(REMOVE_AT ARGN 0)

    # 剩余参数组合成文档字符串
    MergeString(doc ${ARGN})

    # 执行实际的缓存设置
    set(${var} ${val} ${opt} ${type} ${doc})
endfunction()

# 如果是主工程，把默认编译选项设为Release
if (SML_IsMaster AND NOT CMAKE_BUILD_TYPE)
    SetCacheVariable(CMAKE_BUILD_TYPE Release CACHE STRING "项目构建类型，可选项有：Debug Release RelWithDebInfo MinSizeRel")
endif ()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

project(ISO80000SML LANGUAGES CXX DESCRIPTION "ISO 80000-2 Scientific Math Library")

set(SMLVersion "26H2")

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 修改一下目录信息（这里不使用GNU给的目录结构了）
if (NOT CMAKE_INSTALL_INCLUDEDIR OR NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_INCLUDEDIR Headers)
    set(CMAKE_INSTALL_LIBDIR Libraries)
endif ()

SetCacheVariable(SML_HeaderInstallDirectory ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING
    "头文件安装目录，可以是 ${CMAKE_INSTALL_PREFIX} 下的一个相对路径或一个绝对路径")

# 定义源代码根目录
set(SML_HeaderDirs ${CMAKE_SOURCE_DIR}/Headers)
set(SML_SourceDirs ${CMAKE_SOURCE_DIR}/Sources)

# 递归扫描头文件和源文件
file(GLOB_RECURSE SML_HeaderFiles RELATIVE ${CMAKE_SOURCE_DIR} 
    "${SML_HeaderDirs}/*.h" "${SML_HeaderDirs}/*.hpp" "${SML_HeaderDirs}/*.hxx" "${SML_HeaderDirs}/*.inc")

file(GLOB_RECURSE SML_SourceFiles RELATIVE ${CMAKE_SOURCE_DIR} 
    "${SML_SourceDirs}/*.cc")

# 打印找到的文件列表（调试用）
message(STATUS "Found header files:")
foreach(i ${SML_HeaderFiles})
    message(STATUS "  ${i}")
endforeach()

message(STATUS "Found Source files:")
foreach(i ${SML_SourceFiles})
    message(STATUS "  ${i}")
endforeach()

# 强化一下添加目标函数，为后续C++20模块支持预留空间
include(CMakeParseArguments)

function(AddModuleLibrary Name)
    cmake_parse_arguments(AML "" "IF" "FALLBACK" ${ARGN})
    set(ModuleFile ${AML_UNPARSED_ARGUMENTS})

    add_library(${Name})
    set_target_properties(${Name} PROPERTIES LINKER_LANGUAGE CXX)

    #if (NOT ${${AML_IF}})
        target_sources(${Name} PRIVATE ${AML_FALLBACK})
        return()
    #endif ()
endfunction()

AddModuleLibrary(StellarMath Sources/StellarMath.ixx FALLBACK
    ${SML_SourceFiles} ${SML_HeaderFiles}
    IF BuildAsModule)
add_library(StellarDX::ISO80000SML ALIAS StellarMath)

# 添加头文件

target_include_directories(StellarMath PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Headers>
  $<INSTALL_INTERFACE:${SML_HeaderInstallDirectory}>)

# set_target_properties(StellarMath PROPERTIES
#   PUBLIC_HEADER "${SML_HeaderFiles}")

# 安装目标
include(CMakePackageConfigHelpers)

SetCacheVariable(SML_CMakeConfigDirectory ${CMAKE_INSTALL_LIBDIR}/cmake/StellarMath CACHE STRING
    "CMake配置安装目录，可以是 ${CMAKE_INSTALL_PREFIX} 下的一个相对路径或一个绝对路径")

set(VersionConfig ${PROJECT_BINARY_DIR}/StellarMath-config-version.cmake)
set(ProjectConfig ${PROJECT_BINARY_DIR}/StellarMath-config.cmake)
set(PackageConfig ${PROJECT_BINARY_DIR}/StellarMath.pc)
set(TargetsExportName SMLTargets)

SetCacheVariable(SML_LibraryInstallDirectory ${CMAKE_INSTALL_LIBDIR} CACHE STRING
    "库文件安装目录，可以是 ${CMAKE_INSTALL_PREFIX} 下的一个相对路径或一个绝对路径")

SetCacheVariable(SML_PackageConfigDirectory ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE STRING
    "包配置(.pc)安装目录，可以是 ${CMAKE_INSTALL_PREFIX} 下的一个相对路径或一个绝对路径")

write_basic_package_version_file(
    ${VersionConfig}
    VERSION ${SMLVersion}
    COMPATIBILITY AnyNewerVersion)

# 新版CMake似乎移除了这个函数，这里手动添加一下
function(JoinPathsR joined_path first_path_segment)
    set(temp_path "${first_path_segment}")
    foreach(current_segment IN LISTS ARGN)
        if(NOT ("${current_segment}" STREQUAL ""))
            if(IS_ABSOLUTE "${current_segment}")
                set(temp_path "${current_segment}")
            else()
                set(temp_path "${temp_path}/${current_segment}")
            endif()
        endif()
    endforeach()
    set(${joined_path} "${temp_path}" PARENT_SCOPE)
endfunction()

JoinPathsR(PCLibrariesDir "\${exec_prefix}" "${SML_LibraryInstallDirectory}")
JoinPathsR(PCHeadersDir "\${prefix}" "${SML_HeaderInstallDirectory}")

configure_file(
    "${PROJECT_SOURCE_DIR}/CMakeRes/StellarMath.pc.template"
    "${PackageConfig}"
    @ONLY)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/CMakeRes/StellarMath-config.cmake.template
    ${ProjectConfig}
    INSTALL_DESTINATION ${SML_CMakeConfigDirectory})

set(InstallTargets StellarMath)

install(TARGETS ${InstallTargets} EXPORT ${TargetsExportName}
    LIBRARY DESTINATION ${SML_LibraryInstallDirectory}
    ARCHIVE DESTINATION ${SML_LibraryInstallDirectory}
    #PUBLIC_HEADER DESTINATION "${SML_HeaderInstallDirectory}/StellarMath"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(DIRECTORY Headers/StellarMath
    DESTINATION Headers) # 这里因为头文件有子目录，所以另外安装

export(TARGETS ${InstallTargets} NAMESPACE StellarDX::
    FILE ${PROJECT_BINARY_DIR}/${TargetsExportName}.cmake)

install(
    FILES ${ProjectConfig} ${VersionConfig}
    DESTINATION ${SML_CMakeConfigDirectory})

install(EXPORT ${TargetsExportName} DESTINATION ${SML_CMakeConfigDirectory}
          NAMESPACE StellarDX::)

install(FILES "${PackageConfig}" DESTINATION "${SML_PackageConfigDirectory}")
