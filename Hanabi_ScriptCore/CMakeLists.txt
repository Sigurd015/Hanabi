# === Source files ===
file(GLOB_RECURSE SCRIPTCORE_SRC
    "Source/*.cs"
    "Properties/*.cs"
)

# === Shared library target ===
add_library(Hanabi_ScriptCore SHARED ${SCRIPTCORE_SRC})

# Automatically assign source groups
assign_source_groups("${CMAKE_CURRENT_SOURCE_DIR}/Hanabi/src" HANABI_SRC)

set_target_properties(Hanabi_ScriptCore PROPERTIES
    FOLDER "Core"
    DOTNET_TARGET_FRAMEWORK "net472"    # equivalent to dotnetframework "4.7.2"
    OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/Hanabi_Editor/Resources/Scripts"
    # Optional: intermediate directory (like objdir)
    VS_DOTNET_REFERENCES ""
)

# === Configuration-specific compiler options ===
target_compile_options(Hanabi_ScriptCore PRIVATE
    $<$<CONFIG:Debug>:/optimize->
    $<$<CONFIG:Release>:/optimize+>
    $<$<CONFIG:Dist>:/optimize+>
)

# === Symbols / debugging ===
# CMake automatically handles symbols for Debug/Release
# If you want to explicitly disable for Dist:
set_property(TARGET Hanabi_ScriptCore PROPERTY VS_DOTNET_SYMBOLS $<$<CONFIG:Dist>:false>)

# === Optional: namespace ===
# In C#, namespace is defined in code, not in CMake. No action needed here.