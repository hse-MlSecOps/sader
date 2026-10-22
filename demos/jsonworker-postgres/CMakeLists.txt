cmake_minimum_required(VERSION 3.16)
project(sader_workers CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPQXX REQUIRED IMPORTED_TARGET libpqxx)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(sader_demo
    main.cpp
    JsonWorker.cpp
    WebServer.cpp
)

target_include_directories(sader_demo PRIVATE third_party)

target_link_libraries(sader_demo PRIVATE
    PkgConfig::LIBPQXX
    nlohmann_json::nlohmann_json
)
