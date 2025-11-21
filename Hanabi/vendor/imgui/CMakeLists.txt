# --- Source files ---
file(GLOB IMGUI_SRC
    "imgui/imgui.cpp"
    "imgui/imgui_draw.cpp"
    "imgui/imgui_widgets.cpp"
    "imgui/imgui_demo.cpp"
    "imgui/imgui_tables.cpp"
)

file(GLOB IMGUI_HEADERS
    "imgui/imconfig.h"
    "imgui/imgui.h"
    "imgui/imgui_internal.h"
    "imgui/imstb_rectpack.h"
    "imgui/imstb_textedit.h"
    "imgui/imstb_truetype.h"
)

# --- Create static library target ---
add_library(ImGui STATIC ${IMGUI_SRC} ${IMGUI_HEADERS})

set_target_properties(ImGui PROPERTIES
    FOLDER "Dependencies"    
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# --- Configuration-specific compiler flags ---
target_compile_options(ImGui PRIVATE
    $<$<CONFIG:Debug>:/Zi>           # Debug symbols
    $<$<CONFIG:Release>:/O2>         # Optimize
)