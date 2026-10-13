# CMakeList.txt: проект CMake для Sader; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
cmake_minimum_required(VERSION 3.20)

# ========================================
# Prod
# ========================================
add_executable (Sader 
    "${CMAKE_SOURCE_DIR}/src"

    "${CMAKE_SOURCE_DIR}/src/main.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/Executor.h"
    "${CMAKE_SOURCE_DIR}/src/Executor.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/WorkerRegistry.h"
    "${CMAKE_SOURCE_DIR}/src/WorkerRegistry.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/WorkerFactory.h"
    "${CMAKE_SOURCE_DIR}/src/WorkerFactory.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/Worker.h"
    "${CMAKE_SOURCE_DIR}/src/Worker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/csv/CsvWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/text/TextWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/process/ProcessWorker.cpp"
)

target_include_directories(Sader
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
)

target_precompile_headers(Sader PRIVATE
    "${CMAKE_SOURCE_DIR}/src/stdafx.h"
)

# ========================================
# Test
# ========================================

add_executable(test_worker_registry
    "${CMAKE_SOURCE_DIR}/tests/test_worker_registry.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerRegistry.cpp"
    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"
)

target_include_directories(test_worker_registry
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}"
)

add_executable(testWorkerFabric
    "${CMAKE_SOURCE_DIR}/tests/testWorkerFabric.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerFactory.cpp"
    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/csv/CsvWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/process/ProcessWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/text/TextWorker.cpp"
)

target_include_directories(testWorkerFabric
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}"
)

add_executable (workers_test 
    "${CMAKE_SOURCE_DIR}/tests/HttpWorkerTest.cpp"
    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"
   
)

target_include_directories(workers_test PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}"
)


add_executable (file_worker_test
    "${CMAKE_SOURCE_DIR}/tests/FileWorkerTest.cpp"
    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"
)

target_include_directories(file_worker_test PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}"
)
add_executable(ModelWorkerTest
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorkerTest.cpp"
)

target_include_directories(ModelWorkerTest
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
)

enable_testing()

add_test(
    NAME HttpWOrkerTest
    COMMAND workers_test
)

add_test(
    NAME FileWorkerTest
    COMMAND file_worker_test
)
add_test(
    NAME ModelWorkerTest 
    COMMAND ModelWorkerTest
)

add_test(
    NAME WorkerRegistryTest
    COMMAND test_worker_registry
)

add_test(
    NAME WorkerFabricTest
    COMMAND testWorkerFabric
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sader PROPERTY CXX_STANDARD 20)
  set_property(TARGET workers_test PROPERTY CXX_STANDARD 20)
  set_property(TARGET file_worker_test PROPERTY CXX_STANDARD 20)
  set_property(TARGET ModelWorkerTest PROPERTY CXX_STANDARD 20)
  set_property(TARGET test_worker_registry PROPERTY CXX STANDARD 20)
  set_property(TARGET testWorkerFabric PROPERTY CXX STANDARD 20)
endif()
