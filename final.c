#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <stdio.h>
#include "virtual_buttons.h"

int main() {
    // Открываем лог
    FILE *log = fopen("ux0:data/final_log.txt", "w");
    fprintf(log, "START\n");
    fflush(log);
    
    // Инициализация железа
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    
    // Инициализация виртуальных кнопок
    virtualButtonsInit();
    
    // Добавляем кнопки
    virtualButtonAdd("CROSS", 700, 400, 80, 80, SCE_CTRL_CROSS, INPUT_METHOD_TRIGGERED);
    virtualButtonAdd("CIRCLE", 800, 400, 80, 80, SCE_CTRL_CIRCLE, INPUT_METHOD_TRIGGERED);
    virtualDPadSetup(200, 400, 150, 1);
    
    fprintf(log, "INIT OK\n");
    fflush(log);
    
    SceTouchData touch;
    SceCtrlData pad;
    
    int frame = 0;
    int running = 1;
    
    while (running) {
        frame++;
        
        // Читаем физические кнопки
        sceCtrlPeekBufferPositive(0, &pad, 1);
        if (pad.buttons & SCE_CTRL_START) {
            running = 0;
        }
        
        // Читаем касания
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
        
        // Обновляем виртуальные кнопки
        virtualButtonsUpdate(&touch, 1);
        
        // Лог каждые 60 кадров
        if (frame % 60 == 0) {
            int dpad_state = 0;
            if (virtualButtonPressed(SCE_CTRL_UP)) dpad_state |= 1;
            if (virtualButtonPressed(SCE_CTRL_DOWN)) dpad_state |= 2;
            if (virtualButtonPressed(SCE_CTRL_LEFT)) dpad_state |= 4;
            if (virtualButtonPressed(SCE_CTRL_RIGHT)) dpad_state |= 8;
            
            fprintf(log, "Frame %d: CROSS=%d CIRCLE=%d DPAD=0x%02X\n",
                    frame,
                    virtualButtonPressed(SCE_CTRL_CROSS),
                    virtualButtonPressed(SCE_CTRL_CIRCLE),
                    dpad_state);
            fflush(log);
        }
        
        sceKernelDelayThread(16 * 1000); // 60 FPS
    }
    
    fprintf(log, "EXIT after %d frames\n", frame);
    fclose(log);
    sceKernelExitProcess(0);
    return 0;
}