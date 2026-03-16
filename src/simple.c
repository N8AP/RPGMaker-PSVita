#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <stdio.h>

int main() {
    // Открываем лог
    FILE *log = fopen("ux0:data/simple_log.txt", "w");
    fprintf(log, "START\n");
    fflush(log);
    
    // Инициализация
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    
    fprintf(log, "INIT OK\n");
    fflush(log);
    
    SceTouchData touch;
    SceCtrlData pad;
    
    int running = 1;
    int frame = 0;
    
    while (running && frame < 1000) { // 1000 кадров = 16 секунд
        frame++;
        
        // Читаем кнопки
        sceCtrlPeekBufferPositive(0, &pad, 1);
        if (pad.buttons & SCE_CTRL_START) {
            running = 0;
        }
        
        // Читаем касания
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
        
        // Каждые 60 кадров пишем в лог
        if (frame % 60 == 0) {
            fprintf(log, "Frame %d: touches=%d, buttons=0x%08X\n", 
                    frame, touch.reportNum, pad.buttons);
            fflush(log);
        }
        
        sceKernelDelayThread(16 * 1000); // 60 FPS
    }
    
    fprintf(log, "EXIT after %d frames\n", frame);
    fclose(log);
    sceKernelExitProcess(0);
    return 0;
}
