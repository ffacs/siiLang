file(GLOB siiLang_src
     "*.cpp")

add_library(sc_obj_lib OBJECT ${siiLang_src})

# shared libraries need PIC
set_property(TARGET sc_obj_lib PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(sc_lib_shared SHARED $<TARGET_OBJECTS:sc_obj_lib>)
add_library(sc_lib_static STATIC $<TARGET_OBJECTS:sc_obj_lib>)
