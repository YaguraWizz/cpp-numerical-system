cmake_minimum_required(VERSION 3.16)


# === 1. Project configuration and parameters ===
# -----------------------------------------------------------------------
set(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/numerical-system")
set(DEMO_MAIN_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp")
set(UNITTESTS_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/unittests/unittests.cpp")
set(GTEST_DIR          "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest")
set(DOXYFILE_INPUT     "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen")

set(PROJECT_NAME_ "cpp-numerical-system")
set(MAIN_EXECUTABLE_NAME "demo")
set(UNITTEST_EXECUTABLE_NAME "run_unit_tests")
set(LIBRARY_NAME "numerical_system_lib")


project(${PROJECT_NAME_} VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' by default.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

option(NUMSYSTEM_BUILD_MAIN_EXECUTABLE "Build the main executable 'demo'" ON)
option(NUMSYSTEM_BUILD_TESTS "Build unit tests" ON)
option(NUMSYSTEM_BUILD_DOCS "Generate Doxygen documentation" ON)

message(STATUS "\n--- Build Options ---")
message(STATUS "Build main executable: ${NUMSYSTEM_BUILD_MAIN_EXECUTABLE}")
message(STATUS "Build unit tests: ${NUMSYSTEM_BUILD_TESTS}")
message(STATUS "Generate documentation: ${NUMSYSTEM_BUILD_DOCS}")
message(STATUS "----------------------\n")

# === 2. Static library ===
# -----------------------------------------------------------------------
file(GLOB_RECURSE SOURCE_FILES "${PROJECT_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${PROJECT_DIR}/*.h")

add_library(${LIBRARY_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${PROJECT_DIR})
set_target_properties(${LIBRARY_NAME} PROPERTIES
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

# === 3. Main executable ===
# -----------------------------------------------------------------------
if(NUMSYSTEM_BUILD_MAIN_EXECUTABLE)
    if(EXISTS ${DEMO_MAIN_PATH})
        add_executable(${MAIN_EXECUTABLE_NAME} ${DEMO_MAIN_PATH})
        target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})
        set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            CXX_STANDARD ${CMAKE_CXX_STANDARD}
            CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
            CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
        )
        message(STATUS "Main executable '${MAIN_EXECUTABLE_NAME}' will be built.")
    else()
        message(WARNING "File ${DEMO_MAIN_PATH} not found. '${MAIN_EXECUTABLE_NAME}' will not be built.")
    endif()
endif()

# === 4. Unit tests ===
# -----------------------------------------------------------------------
if(NUMSYSTEM_BUILD_TESTS)
    if(EXISTS "${GTEST_DIR}/CMakeLists.txt")
        message(STATUS "Google Test found. Unit tests will be built.")
        set(INSTALL_GTEST OFF CACHE INTERNAL "Disable GTest installation")
        set(BUILD_GMOCK OFF CACHE INTERNAL "Disable GMock build")
        add_subdirectory(${GTEST_DIR} EXCLUDE_FROM_ALL)

        if(EXISTS ${UNITTESTS_PATH})
            add_executable(${UNITTEST_EXECUTABLE_NAME} ${UNITTESTS_PATH})
            target_link_libraries(${UNITTEST_EXECUTABLE_NAME} PRIVATE
                ${LIBRARY_NAME}
                GTest::gtest
                GTest::gtest_main
            )
            set_target_properties(${UNITTEST_EXECUTABLE_NAME} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
            )
            include(GoogleTest)
            gtest_discover_tests(${UNITTEST_EXECUTABLE_NAME})
            message(STATUS "Tests configured. Run with 'ctest'.")
        else()
            message(WARNING "Unit tests file not found: ${UNITTESTS_PATH}")
        endif()
    else()
        message(WARNING "Google Test not found at ${GTEST_DIR}")
        message(WARNING "Add Google Test: git submodule add https://github.com/google/googletest.git external/googletest && git submodule update --init")
    endif()
endif()

# === 5. Documentation generation ===
# -----------------------------------------------------------------------
if(NUMSYSTEM_BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        if(EXISTS ${DOXYFILE_INPUT})
            file(COPY ${DOXYFILE_INPUT} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
            configure_file(${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

            add_custom_target(docs
                COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating Doxygen documentation"
                VERBATIM
            )
            message(STATUS "Target 'docs' added. Run with: cmake --build . --target docs")
        else()
            message(WARNING "Doxyfile.in not found: ${DOXYFILE_INPUT}")
        endif()
    else()
        message(WARNING "Doxygen not found. Documentation will not be generated.")
    endif()
endif()

# === 6. Summary ===
# -----------------------------------------------------------------------
message(STATUS "\n--- Configuration of ${PROJECT_NAME} completed ---")
message(STATUS "  Main executable: ${NUMSYSTEM_BUILD_MAIN_EXECUTABLE}")
message(STATUS "  Unit tests: ${NUMSYSTEM_BUILD_TESTS}")
message(STATUS "  Documentation: ${NUMSYSTEM_BUILD_DOCS}\n")

if(TARGET ${MAIN_EXECUTABLE_NAME})
    message(STATUS "To run the main executable: ${CMAKE_BINARY_DIR}/bin/${MAIN_EXECUTABLE_NAME}")
endif()
if(TARGET ${UNITTEST_EXECUTABLE_NAME})
    message(STATUS "Run unit tests with: ctest")
endif()
if(TARGET docs)
    message(STATUS "Documentation will be in: ${DOXYGEN_OUTPUT_DIR}/html/index.html")
endif()

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${MAIN_EXECUTABLE_NAME})
endif()

# === 7. Full build-and-run pipeline ===
# -----------------------------------------------------------------------
if(NUMSYSTEM_BUILD_TESTS AND NUMSYSTEM_BUILD_MAIN_EXECUTABLE)
    add_custom_target(full_run
        # Комментарий, который будет отображаться при сборке этой цели
        COMMENT "Running unit tests and launching demo if successful..."

        # Команда 1: Запустить исполняемый файл тестов.
        # Если тесты пройдут успешно, он вернет код завершения 0.
        # Используем генераторное выражение $<TARGET_FILE:...> для надежного получения пути.
        COMMAND $<TARGET_FILE:${UNITTEST_EXECUTABLE_NAME}>

        # Команда 2: Запустить исполняемый файл демо.
        # Эта команда выполнится ТОЛЬКО если предыдущая команда (запуск тестов) завершилась успешно (с кодом 0).
        COMMAND $<TARGET_FILE:${MAIN_EXECUTABLE_NAME}>

        # Указываем рабочий каталог для выполнения этих команд.
        # ${CMAKE_BINARY_DIR} - это корень каталога сборки.
        # Если исполняемые файлы тестов и демо находятся в подкаталогах bin/ и tests/
        # относительно build, то рабочим каталогом можно оставить build, а пути к exe
        # будут полными благодаря $<TARGET_FILE:...>.
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

        # Это не 'ALL', потому что это пользовательская цель, которая выполняет действия,
        # а не является частью основной сборки.
    )

    # Убедимся, что тесты и демо собраны до попытки выполнения цели full_run
    add_dependencies(full_run ${UNITTEST_EXECUTABLE_NAME} ${MAIN_EXECUTABLE_NAME})

    # УДАЛИТЕ старую add_custom_command с POST_BUILD для full_run
    # add_custom_command(TARGET full_run POST_BUILD ...)
    # ... этот блок команд больше не нужен
endif()