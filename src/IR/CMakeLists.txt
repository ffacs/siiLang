file(GLOB siiLang_ir_src
     "*.cpp"
     "Pass/*.cpp")

add_library(sc_ir_obj_lib OBJECT ${siiLang_ir_src})

# shared libraries need PIC
set_property(TARGET sc_ir_obj_lib PROPERTY POSITION_INDEPENDENT_CODE 1)

# shared and static libraries built from the same object files
add_library(sc_ir_lib_shared SHARED $<TARGET_OBJECTS:sc_ir_obj_lib>)

add_library(sc_ir_lib_static STATIC $<TARGET_OBJECTS:sc_ir_obj_lib>)
