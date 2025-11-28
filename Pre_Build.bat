@echo off

SET ASSIMP_SOURCE_DIR="Hanabi/vendor/assimp"
SET PRE_BUILD_DIR="Prebuilt"
SET GENERATOR=Visual Studio 17 2022

SET BINARIES_DIR="build/assimp/x64"
cmake . -G "%GENERATOR%" -A x64 -S %ASSIMP_SOURCE_DIR% -B %BINARIES_DIR%
cmake --build %BINARIES_DIR% --config debug
cmake --build %BINARIES_DIR% --config release

mkdir "Prebuilt\assimp\Debug" /p 2>nul
mkdir "Prebuilt\assimp\Release" /p 2>nul

copy "build\assimp\x64\bin\Debug\assimp-*.dll" "Prebuilt\assimp\Debug"
copy "build\assimp\x64\lib\Debug\assimp-*.lib" "Prebuilt\assimp\Debug"
copy "build\assimp\x64\bin\Release\assimp-*.dll" "Prebuilt\assimp\Release"
copy "build\assimp\x64\lib\Release\assimp-*.lib" "Prebuilt\assimp\Release"
copy "build\assimp\x64\include\assimp\config.h" "Hanabi\vendor\assimp\include\assimp"
copy "build\assimp\x64\include\assimp\revision.h" "Hanabi\vendor\assimp\include\assimp"

PAUSE