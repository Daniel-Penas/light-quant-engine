cmake_minimum_required(VERSION 3.18)
project(quantengine LANGUAGES CXX)

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_quantengine
    src/quantengine/bindings.cpp
)

target_include_directories(_quantengine PRIVATE src/quantengine)

set_target_properties(_quantengine PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME "_quantengine"            # the filename is _quantengine.* (no dots in module name)
)

# ⬇️ This line is what makes the .so end up in quantengine/_quantengine.* inside the wheel
install(TARGETS _quantengine
        LIBRARY DESTINATION quantengine     # macOS/Linux
        RUNTIME DESTINATION quantengine     # Windows
        ARCHIVE DESTINATION quantengine)    # (kept for completeness)