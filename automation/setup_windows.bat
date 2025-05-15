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

:: Спрашиваем пользователя о сборке
set /p "build_all=Do you want to build the executable and documentation? (yes/no/doc/Enter=no): "

if /i "%build_all%"=="yes" (
    echo Building the executable...
    cmake --build . --config Release --target demo || (
        echo Error when building the executable!
        popd
        pause
        exit /b 1
    )
    echo Executable built successfully.
    echo.
    echo Building the documentation...
    cmake --build . --target docs || (
        echo Error when building the documentation!
        popd
        pause
        exit /b 1
    )
    echo Documentation built successfully.
    pause

) else if /i "%build_all%"=="doc" (
    echo Building the documentation...
    cmake --build . --target docs || (
        echo Error when building the documentation!
        popd
        pause
        exit /b 1
    )
    echo Documentation built successfully.
    pause

) else (
    echo Not building the executable and documentation.
    if not "%build_all%"=="" (
        echo You entered: "%build_all%"
    ) else (
        echo You pressed Enter.
    )
)

popd
echo.
pause
