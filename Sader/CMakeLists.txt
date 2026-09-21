# CMakeList.txt: проект CMake для Sader; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (Sader 
    "Sader.cpp" 
    "Sader.h" 

    "CWorker.h"
    "CWorker.cpp"

    "CHashWorker.h" 
    "CHashWorker.cpp" 
    
    "CFileWorker.h"
    "CFileWorker.cpp"

    "CExecutor.h"
    "CExecutor.cpp"

    "Command.h"

    "pch.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sader PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.

target_precompile_headers(Sader PRIVATE
    <iostream>
    <string>
    <vector>
    <memory>
    <filesystem>
)