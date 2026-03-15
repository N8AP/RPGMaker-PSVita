#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <vita2d.h>
#include <stdio.h>
#include "virtual_buttons.h"

int main() {
    // Открываем лог-файл
    FILE *log = fopen("ux0:data/vbtns_debug.txt", "w");
    fprintf(log, "=== VIRTUAL BUTTONS DEBUG ===\n");
    fflush(log);
    
    // 1. Инициализация графики
    fprintf(log, "1. vita2d_init()...\n");
    fflush(log);
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));
    fprintf(log, "   OK\n");
    fflush(log);
    
    // 2. Контроллер
    fprintf(log, "2. sceCtrlSetSamplingMode()...\n");
    fflush(log);
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    fprintf(log, "   OK\n");
    fflush(log);
    
    // 3. Тачпад
    fprintf(log, "3. sceTouchSetSamplingState()...\n");
    fflush(log);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    fprintf(log, "   OK\n");
    fflush(log);
    
    // 4. Инициализация виртуальных кнопок
    fprintf(log, "4. virtualButtonsInit()...\n");
    fflush(log);
    virtualButtonsInit();
    fprintf(log, "   OK\n");
    fflush(log);
    
    // 5. Добавление кнопок
    fprintf(log, "5. Adding CROSS button...\n");
    fflush(log);
    int res1 = virtualButtonAdd("CROSS", 700, 400, 80, 80, SCE_CTRL_CROSS, INPUT_METHOD_TRIGGERED);
    fprintf(log, "   result: %d\n", res1);
    fflush(log);
    
    fprintf(log, "5b. Adding CIRCLE button...\n");
    fflush(log);
    int res2 = virtualButtonAdd("CIRCLE", 800, 400, 80, 80, SCE_CTRL_CIRCLE, INPUT_METHOD_TRIGGERED);
    fprintf(log, "   result: %d\n", res2);
    fflush(log);
    
    fprintf(log, "5c. virtualDPadSetup()...\n");
    fflush(log);
    virtualDPadSetup(200, 400, 150, 1);
    fprintf(log, "   OK\n");
    fflush(log);
    
    SceTouchData touch[2];
    SceCtrlData pad;
    
    int frame = 0;
    
    fprintf(log, "6. Entering main loop\n");
    fflush(log);
    
    while (1) {
        frame++;
        
        // Каждые 60 кадров пишем в лог
        if (frame % 60 == 0) {
            fprintf(log, "Frame %d\n", frame);
            fflush(log);
        }
        
        // Читаем касания
        sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch[0], 1);
        sceTouchPeek(SCE_TOUCH_PORT_BACK, &touch[1], 1);
        
        // Обновляем виртуальные кнопки
        virtualButtonsUpdate(touch, 2);
        
        // Читаем физические кнопки
        sceCtrlPeekBufferPositive(0, &pad, 1);
        if (pad.buttons & SCE_CTRL_START) {
            fprintf(log, "START pressed, exiting...\n");
            fflush(log);
            break;
        }
        
        // Рисование
        vita2d_start_drawing();
        vita2d_clear_screen();
        
        // Отображаем счётчик кадров
        char frame_text[64];
        sprintf(frame_text, "Frame: %d", frame);
        vita2d_font_draw_text(NULL, 10, 30, RGBA8(255, 255, 255, 255), 24, frame_text);
        
        // Рисуем D-Pad
        vita2d_draw_rectangle(125, 325, 150, 150, RGBA8(100, 100, 100, 255));
        
        // Рисуем кнопки
        vita2d_draw_rectangle(700, 400, 80, 80, RGBA8(100, 100, 100, 255));
        vita2d_draw_rectangle(800, 400, 80, 80, RGBA8(100, 100, 100, 255));
        
        // Подписи
        vita2d_font_draw_text(NULL, 720, 420, RGBA8(255, 255, 255, 255), 20, "X");
        vita2d_font_draw_text(NULL, 825, 420, RGBA8(255, 255, 255, 255), 20, "O");
        
        // Отображение нажатий
        if (virtualButtonPressed(SCE_CTRL_CROSS)) {
            vita2d_draw_rectangle(700, 400, 80, 80, RGBA8(0, 255, 0, 255));
        }
        if (virtualButtonPressed(SCE_CTRL_CIRCLE)) {
            vita2d_draw_rectangle(800, 400, 80, 80, RGBA8(0, 255, 0, 255));
        }
        
        // Отображение направления D-Pad
        if (virtualButtonPressed(SCE_CTRL_UP)) {
            vita2d_font_draw_text(NULL, 170, 340, RGBA8(0, 255, 0, 255), 20, "UP");
        }
        if (virtualButtonPressed(SCE_CTRL_DOWN)) {
            vita2d_font_draw_text(NULL, 170, 460, RGBA8(0, 255, 0, 255), 20, "DOWN");
        }
        if (virtualButtonPressed(SCE_CTRL_LEFT)) {
            vita2d_font_draw_text(NULL, 110, 400, RGBA8(0, 255, 0, 255), 20, "LEFT");
        }
        if (virtualButtonPressed(SCE_CTRL_RIGHT)) {
            vita2d_font_draw_text(NULL, 230, 400, RGBA8(0, 255, 0, 255), 20, "RIGHT");
        }
        
        vita2d_end_drawing();
        vita2d_swap_buffers();
        
        sceKernelDelayThread(16 * 1000);
    }
    
    fprintf(log, "7. Cleaning up...\n");
    fflush(log);
    
    vita2d_fini();
    
    fprintf(log, "8. Exit\n");
    fclose(log);
    
    sceKernelExitProcess(0);
    return 0;
}