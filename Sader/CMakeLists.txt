# CMakeList.txt: проект CMake для Sader; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Sader 
    "${CMAKE_SOURCE_DIR}/src"

    "${CMAKE_SOURCE_DIR}/src/main.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/Executor.h"
    "${CMAKE_SOURCE_DIR}/src/Executor.cpp"

    "${CMAKE_SOURCE_DIR}/include/sader/Worker.h"
    "${CMAKE_SOURCE_DIR}/src/Worker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/file/FileWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/hash/HashWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/csv/CsvWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/http/HttpWorker.cpp"

    "${CMAKE_SOURCE_DIR}/workers/json/JsonWorker.cpp"
   
)

target_include_directories(Sader
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sader PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

target_precompile_headers(Sader PRIVATE
    "${CMAKE_SOURCE_DIR}/src/stdafx.h"
)