@echo off
chcp 65001

:: Проверка установки CMake
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: CMake not found. Install it from https://cmake.org/download/
    pause
    exit /b 1
)

:: Проверка установки Doxygen
where doxygen >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: Doxygen not found. Install it and make sure it's in your PATH.
    pause
    exit /b 1
)

:: Задаём путь к каталогу сборки
set "PROJECT_DIR=..\project"

:: Удаляем папку сборки, если существует
if exist "%PROJECT_DIR%" (
    echo Cleaning previous build directory...
    rmdir /s /q "%PROJECT_DIR%"
)

:: Создаём новую папку сборки
mkdir "%PROJECT_DIR%"

:: Переход в папку проекта и генерация CMake-файлов
pushd "%PROJECT_DIR%"
cmake .. || (
    echo Error when running CMake!
    popd
    pause
    exit /b 1
)

echo.
echo Build files generated in "%PROJECT_DIR%"
echo.

:: Сборка тестов
echo Building the tests...
cmake --build . --target run_unit_tests || (
    echo Error when building or running the unit tests!
    popd
    pause
    exit /b 1
)

echo Unit tests passed successfully.
echo.

:: Сборка исполнимого файла
echo Building the executable...
cmake --build . --config Release --target demo || (
    echo Error when building the executable!
    popd
    pause
    exit /b 1
)
echo Executable built successfully.
echo.

:: Сборка документации
echo Building the documentation...
cmake --build . --target docs || (
    echo Error when building the documentation!
    popd
    pause
    exit /b 1
)
echo Documentation built successfully.

popd
echo.
echo Build and documentation generation complete.
pause
