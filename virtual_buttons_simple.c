#include "virtual_buttons.h"
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <string.h>

static int g_cross_state = 0;
static int g_circle_state = 0;
static int g_dpad_state = 0;

void virtualButtonsInit(void) {
    g_cross_state = 0;
    g_circle_state = 0;
    g_dpad_state = 0;
}

void virtualButtonsUpdate(SceTouchData* touch, int touchNum) {
    if (!touch) return;
    
    g_cross_state = 0;
    g_circle_state = 0;
    g_dpad_state = 0;
    
    for (int t = 0; t < touchNum; t++) {
        for (int r = 0; r < touch[t].reportNum; r++) {
            int x = touch[t].report[r].x;
            int y = touch[t].report[r].y;
            
            // Кнопка CROSS (700,400,80,80)
            if (x >= 660 && x <= 740 && y >= 360 && y <= 440) {
                g_cross_state = 1;
            }
            
            // Кнопка CIRCLE (800,400,80,80)
            if (x >= 760 && x <= 840 && y >= 360 && y <= 440) {
                g_circle_state = 1;
            }
            
            // D-Pad (200,400,150,150)
            if (x >= 125 && x <= 275 && y >= 325 && y <= 475) {
                if (x < 170) g_dpad_state |= SCE_CTRL_LEFT;
                else if (x > 230) g_dpad_state |= SCE_CTRL_RIGHT;
                if (y < 350) g_dpad_state |= SCE_CTRL_UP;
                else if (y > 450) g_dpad_state |= SCE_CTRL_DOWN;
            }
        }
    }
}

int virtualButtonPressed(int inputTrigger) {
    if (inputTrigger == SCE_CTRL_CROSS) return g_cross_state;
    if (inputTrigger == SCE_CTRL_CIRCLE) return g_circle_state;
    if (inputTrigger == SCE_CTRL_UP && (g_dpad_state & SCE_CTRL_UP)) return 1;
    if (inputTrigger == SCE_CTRL_DOWN && (g_dpad_state & SCE_CTRL_DOWN)) return 1;
    if (inputTrigger == SCE_CTRL_LEFT && (g_dpad_state & SCE_CTRL_LEFT)) return 1;
    if (inputTrigger == SCE_CTRL_RIGHT && (g_dpad_state & SCE_CTRL_RIGHT)) return 1;
    return 0;
}

// Заглушки для остальных функций
int virtualButtonAdd(const char* name, int x, int y, int width, int height, int inputTrigger, InputMethod method) { return 0; }
void virtualDPadSetup(int x, int y, int size, int diagonalEnabled) {}
void virtualButtonShow(int inputTrigger) {}
void virtualButtonHide(int inputTrigger) {}
void virtualButtonsShowAll(void) {}
void virtualButtonsHideAll(void) {}
void virtualButtonsDrawDebug(void) {}