# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.13)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Sader")

option(ENABLE_COVERAGE "Build with coverage instrumentation" OFF)

if(ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage -fprofile-abs-path)
    add_link_options(--coverage -lgcov)
endif()

# Включите подпроекты.
add_subdirectory ("Sader")

# Тесты ProcessWorker
add_executable(TestProcessWorker
    tests/test_process_worker.cpp
    workers/process/ProcessWorker.cpp
    src/Worker.cpp
)

target_include_directories(TestProcessWorker PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/workers"
    "${CMAKE_SOURCE_DIR}"
)

target_compile_features(TestProcessWorker PRIVATE cxx_std_20)

target_precompile_headers(Sader PRIVATE
    <iostream>
    <string>
    <vector>
    <memory>
    <filesystem>
)