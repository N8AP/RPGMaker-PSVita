cmake_minimum_required(VERSION 3.10)
project(VirtualButtons)

set(CMAKE_SYSTEM_NAME Generic)

# Путь к VitaSDK
if(NOT DEFINED ENV{VITASDK})
    message(FATAL_ERROR "VITASDK not set")
endif()
set(VITASDK $ENV{VITASDK})

set(CMAKE_C_COMPILER ${VITASDK}/bin/arm-vita-eabi-gcc)
set(CMAKE_CXX_COMPILER ${VITASDK}/bin/arm-vita-eabi-g++)

link_directories(${VITASDK}/arm-vita-eabi/lib)
include_directories(${VITASDK}/arm-vita-eabi/include)

# Библиотеки
set(LIBS
    -lSceLibKernel_stub
    -lSceCtrl_stub
    -lSceTouch_stub
)

add_executable(virtual_buttons.elf main.c virtual_buttons.c)
target_link_libraries(virtual_buttons.elf ${LIBS})

# Конвертация в VPK
add_custom_command(TARGET virtual_buttons.elf POST_BUILD
    COMMAND ${VITASDK}/bin/vita-elf-create virtual_buttons.elf virtual_buttons.velf
    COMMAND ${VITASDK}/bin/vita-make-fself -s virtual_buttons.velf eboot.bin
    COMMAND ${VITASDK}/bin/vita-mksfoex -s TITLE_ID="VBTN00001" "Virtual Buttons" param.sfo
    COMMENT "Creating eboot.bin and param.sfo"
)
