cmake_minimum_required(VERSION 3.8...3.26)
project(ImGui_Cheat)
# Shared library name
set(TARGET MCL)

# For libmain.so
find_package(JNI REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/BNM-Android EXCLUDE_FROM_ALL)
get_property(BNM_INCLUDE_DIRECTORIES TARGET BNM PROPERTY BNM_INCLUDE_DIRECTORIES)

#set(CMAKE_C_FLAGS
#LOCAL_CFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w
#LOCAL_CFLAGS += -fno-rtti -fexceptions -fpermissive
#LOCAL_CPPFLAGS := -Wno-error=format-security -fvisibility=hidden -ffunction-sections -fdata-sections -w -Werror -s -std=c++20
#LOCAL_CPPFLAGS += -Wno-error=c++11-narrowing -fms-extensions -fno-rtti -fexceptions -fpermissive

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# KittyMemory
set(KITTYMEMORY_PATH ${CMAKE_SOURCE_DIR}/third_party/KittyMemory)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)

# xdl
set(XDL_PATH ${CMAKE_SOURCE_DIR}/third_party/xdl)
file(GLOB XDL_SRC ${XDL_PATH}/*.c)

# ImGui
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/third_party/ImGui)
file(GLOB IMGUI_SRC ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/*.cpp)

# Main src
set(MAIN_PATH ${CMAKE_SOURCE_DIR}/Main)
file(GLOB MAIN_SRC ${MAIN_PATH}/*.cpp ${MAIN_PATH}/*.c)

# Misc src
set(MISC_PATH ${CMAKE_SOURCE_DIR}/Misc)
file(GLOB MISC_SRC ${MISC_PATH}/*.cpp ${MISC_PATH}/*.c)

# Static Libs
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)
set(DOBBY_LIB ${CMAKE_SOURCE_DIR}/third_party/Dobby/${ANDROID_ABI}/libdobby.a)

# Source files
add_library(
    ${TARGET}
    SHARED
    ${MAIN_SRC}
    ${MISC_SRC}
    ${IMGUI_SRC}
    ${XDL_SRC}
    ${KITTYMEMORY_SRC}
)

target_compile_options(${TARGET} PRIVATE
    -Wall -Wextra
    -Oz -ffunction-sections -fdata-sections -flto
    -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden -fno-ident -fno-strict-aliasing
    -fno-unwind-tables -fno-asynchronous-unwind-tables
)
target_link_options(${TARGET} PRIVATE
    -Wl,-z,max-page-size=16384
    -s -Oz -Wl,--exclude-libs,ALL -Wl,--gc-sections -Wl,--as-needed -Wl,--icf=all
    -Wl,--strip-all -Wl,--hash-style=sysv -Wl,--build-id=none
    #-Wl,-plugin-opt,-thinlto-index-only
    -Wl,--relax -Wl,--no-rosegment -Wl,--enable-new-dtags
)
add_compile_options(-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=. -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.)

find_program(LLVM_OBJCOPY_PATH NAMES "llvm-objcopy" PATHS ${ANDROID_NDK}/toolchains/llvm/prebuilt/*/bin NO_DEFAULT_PATH)
if(LLVM_OBJCOPY_PATH)
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${LLVM_OBJCOPY_PATH} --remove-section=.note.android.ident --remove-section=.comment --remove-section=.ARM.attributes $<TARGET_FILE:${TARGET}>)
endif()

# RENDER_EGL or RENDER_JAVA_EGL or RENDER_VULKAN
target_compile_definitions(${TARGET} PRIVATE RENDER_EGL)

# INPUT_UNIVERSAL or INPUT_UNITY or INPUT_SDL
target_compile_definitions(${TARGET} PRIVATE INPUT_UNIVERSAL)

target_include_directories(
    BNM
    PUBLIC
    third_party/Dobby/
    third_party/xdl/include/
)

include_directories(
    third_party/Dobby/
    third_party/xdl/include/
)

target_include_directories(
    ${TARGET}
    PUBLIC
    ${BNM_INCLUDE_DIRECTORIES}
    Include/
    third_party/
)

target_link_libraries(
    ${TARGET}
    PUBLIC
    BNM
    ${DOBBY_LIB}
    ${KEYSTONE_LIB}
    
    log
    android
    z
    vulkan
    EGL
    GLESv3
    GLESv2
)

# libLoader.so
# add_library(
#     Loader
#     SHARED
#     Loader/Loader.cpp
# )
# target_include_directories(
#     Loader
#     PUBLIC
#     Include/
# )
# target_link_libraries(
#     Loader
#     PUBLIC
#     log
#     android
# )

# libmain.so
add_library(
    main
    SHARED
    LibMain/main.cpp
)
target_include_directories(
    main
    PUBLIC
    Include/
    ${JNI_INCLUDE_DIRS}
)
target_link_libraries(
    main
    PUBLIC
    log
    android
    ${JNI_LIBRARIES}
)
