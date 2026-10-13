
cmake_minimum_required(VERSION 3.20)

# ========================================
# Configuration
# ========================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)

# ========================================
# Production
# ========================================

add_executable(Sader
    "${CMAKE_SOURCE_DIR}/src/main.cpp"

    "${CMAKE_SOURCE_DIR}/src/Executor.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerRegistry.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerFactory.cpp"
    "${CMAKE_SOURCE_DIR}/src/Worker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/csv/CsvWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/text/TextWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/process/ProcessWorker.cpp"
)

target_include_directories(Sader PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(Sader PRIVATE CURL::libcurl)

target_precompile_headers(Sader PRIVATE
    "${CMAKE_SOURCE_DIR}/src/stdafx.h"
)

# ========================================
# Tests
# ========================================

enable_testing()

# WorkerRegistry tests

add_executable(test_worker_registry
    "${CMAKE_SOURCE_DIR}/tests/test_worker_registry.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerRegistry.cpp"
    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"
)

target_include_directories(test_worker_registry PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

add_test(
    NAME WorkerRegistryTest
    COMMAND test_worker_registry
)

# WorkerFactory tests

add_executable(testWorkerFabric
    "${CMAKE_SOURCE_DIR}/tests/testWorkerFabric.cpp"
    "${CMAKE_SOURCE_DIR}/src/WorkerFactory.cpp"
    "${CMAKE_SOURCE_DIR}/src/Worker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/csv/CsvWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/process/ProcessWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/text/TextWorker.cpp"
)

target_include_directories(testWorkerFabric PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(testWorkerFabric PRIVATE CURL::libcurl)

add_test(
    NAME WorkerFabricTest
    COMMAND testWorkerFabric
)

# HttpWorker tests

add_executable(workers_test
    "${CMAKE_SOURCE_DIR}/tests/HttpWorkerTest.cpp"
    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"
)

target_include_directories(workers_test PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(workers_test PRIVATE CURL::libcurl)

add_test(
    NAME HttpWorkerTest
    COMMAND workers_test
)

# FileWorker tests

add_executable(file_worker_test
    "${CMAKE_SOURCE_DIR}/tests/FileWorkerTest.cpp"
    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"
)

target_include_directories(file_worker_test PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

add_test(
    NAME FileWorkerTest
    COMMAND file_worker_test
)

# ModelWorker tests

add_executable(ModelWorkerTest
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorker.cpp"
    "${CMAKE_SOURCE_DIR}/workers/model/ModelWorkerTest.cpp"
)

target_include_directories(ModelWorkerTest PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}"
)

add_test(
    NAME ModelWorkerTest
    COMMAND ModelWorkerTest
)