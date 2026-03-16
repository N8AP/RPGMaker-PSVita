#include "virtual_buttons.h"
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <string.h>
#include <stdio.h>

#define MAX_BUTTONS 32
#define MAX_NAME_LEN 32
#define REPEAT_DELAY 24      // Кадров до первого повтора
#define REPEAT_INTERVAL 6     // Кадров между повторами

// Глобальное состояние
static VirtualButton g_buttons[MAX_BUTTONS];
static int g_buttonCount = 0;
static VirtualDPad g_dpad;
static int g_hasDPad = 0;

// Вспомогательная функция: проверка попадания в прямоугольник
static int pointInRect(int px, int py, int rx, int ry, int rw, int rh) {
    int left = rx - rw/2;
    int right = rx + rw/2;
    int top = ry - rh/2;
    int bottom = ry + rh/2;
    
    return (px >= left && px <= right && py >= top && py <= bottom);
}

// Инициализация
void virtualButtonsInit(void) {
    g_buttonCount = 0;
    g_hasDPad = 0;
    memset(g_buttons, 0, sizeof(g_buttons));
    memset(&g_dpad, 0, sizeof(g_dpad));
}

// Добавление кнопки
int virtualButtonAdd(const char* name, int x, int y, int width, int height, 
                     int inputTrigger, InputMethod method) {
    if (!name) return -1;
    if (g_buttonCount >= MAX_BUTTONS) return -1;
    
    VirtualButton* btn = &g_buttons[g_buttonCount];
    strncpy(btn->name, name, MAX_NAME_LEN - 1);
    btn->name[MAX_NAME_LEN - 1] = '\0';
    btn->type = BUTTON_TYPE_KEY;
    btn->x = x;
    btn->y = y;
    btn->width = width;
    btn->height = height;
    btn->inputTrigger = inputTrigger;
    btn->method = method;
    btn->state = 0;
    btn->pressFrame = 0;
    btn->lastPressFrame = 0;
    btn->enabled = 1;
    
    return g_buttonCount++;
}

// Настройка D-Pad
void virtualDPadSetup(int x, int y, int size, int diagonalEnabled) {
    g_dpad.x = x;
    g_dpad.y = y;
    g_dpad.size = size;
    g_dpad.diagonalEnabled = diagonalEnabled;
    g_dpad.lastDirection = 0;
    g_dpad.enabled = 1;
    g_hasDPad = 1;
}

// Обработка D-Pad
static void processDPad(int x, int y) {
    if (!g_hasDPad) return;
    if (!g_dpad.enabled) return;
    
    // Проверяем, попадает ли точка в область D-Pad
    int half = g_dpad.size / 2;
    if (!pointInRect(x, y, g_dpad.x, g_dpad.y, g_dpad.size, g_dpad.size)) {
        // Если палец убрали, очищаем направление
        if (g_dpad.lastDirection) {
            g_dpad.lastDirection = 0;
        }
        return;
    }
    
    // Определяем сектор (как в оригинальном плагине)
    int relX = x - (g_dpad.x - half);
    int relY = y - (g_dpad.y - half);
    
    int col = relX > g_dpad.size / 3 ? (relX > g_dpad.size * 2 / 3 ? 2 : 1) : 0;
    int row = relY > g_dpad.size / 3 ? (relY > g_dpad.size * 2 / 3 ? 2 : 1) : 0;
    int index = row * 3 + col;
    
    int newDir = 0;
    
    // Конвертируем индекс в направление
    switch (index) {
        case 0: newDir = g_dpad.diagonalEnabled ? (SCE_CTRL_UP | SCE_CTRL_LEFT) : SCE_CTRL_UP; break;
        case 1: newDir = SCE_CTRL_UP; break;
        case 2: newDir = g_dpad.diagonalEnabled ? (SCE_CTRL_UP | SCE_CTRL_RIGHT) : SCE_CTRL_UP; break;
        case 3: newDir = SCE_CTRL_LEFT; break;
        case 4: newDir = 0; break;  // Центр
        case 5: newDir = SCE_CTRL_RIGHT; break;
        case 6: newDir = g_dpad.diagonalEnabled ? (SCE_CTRL_DOWN | SCE_CTRL_LEFT) : SCE_CTRL_DOWN; break;
        case 7: newDir = SCE_CTRL_DOWN; break;
        case 8: newDir = g_dpad.diagonalEnabled ? (SCE_CTRL_DOWN | SCE_CTRL_RIGHT) : SCE_CTRL_DOWN; break;
    }
    
    g_dpad.lastDirection = newDir;
}

// Обработка обычной кнопки
static void processButton(VirtualButton* btn, int pressed, int frame) {
    if (!btn) return;
    if (!btn->enabled) return;
    
    switch (btn->method) {
        case INPUT_METHOD_TRIGGERED:
            // Только в момент нажатия
            btn->state = pressed && !btn->lastPressFrame;
            break;
            
        case INPUT_METHOD_PRESSED:
            // Пока нажато
            btn->state = pressed;
            break;
            
        case INPUT_METHOD_REPEATED:
            // С задержкой и повтором
            if (pressed) {
                btn->pressFrame++;
                if (btn->pressFrame == 1) {
                    btn->state = 1;  // Первое нажатие
                } else if (btn->pressFrame > REPEAT_DELAY && 
                           (btn->pressFrame - REPEAT_DELAY) % REPEAT_INTERVAL == 0) {
                    btn->state = 1;  // Повторы
                } else {
                    btn->state = 0;
                }
            } else {
                btn->pressFrame = 0;
                btn->state = 0;
            }
            break;
            
        case INPUT_METHOD_LONG_PRESSED:
            // После задержки каждый кадр
            if (pressed) {
                btn->pressFrame++;
                btn->state = (btn->pressFrame > REPEAT_DELAY);
            } else {
                btn->pressFrame = 0;
                btn->state = 0;
            }
            break;
            
        case INPUT_METHOD_RELEASED:
            // При отпускании
            btn->state = btn->lastPressFrame && !pressed;
            break;
    }
    
    btn->lastPressFrame = pressed;
}

// Обновление состояния (вызывать каждый кадр)
void virtualButtonsUpdate(SceTouchData* touch, int touchNum) {
    if (!touch) return;
    if (touchNum <= 0) return;
    if (g_buttonCount == 0 && !g_hasDPad) return;
    
    // Сначала сбрасываем состояния (кроме тех, что будут установлены)
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        if (btn->method != INPUT_METHOD_PRESSED &&
            btn->method != INPUT_METHOD_REPEATED &&
            btn->method != INPUT_METHOD_LONG_PRESSED) {
            btn->state = 0;
        }
    }
    
    // Обрабатываем все касания
    for (int t = 0; t < touchNum; t++) {
        if (!touch[t].report) continue;
        
        for (int r = 0; r < touch[t].reportNum; r++) {
            int x = touch[t].report[r].x;
            int y = touch[t].report[r].y;
            
            // Сначала D-Pad
            if (g_hasDPad) {
                processDPad(x, y);
            }
            
            // Потом обычные кнопки
            for (int i = 0; i < g_buttonCount; i++) {
                VirtualButton* btn = &g_buttons[i];
                if (!btn) continue;
                
                if (btn->type == BUTTON_TYPE_KEY) {
                    int pressed = pointInRect(x, y, btn->x, btn->y, 
                                             btn->width, btn->height);
                    processButton(btn, pressed, 0);
                }
            }
        }
    }
}

// Проверка состояния кнопки
int virtualButtonPressed(int inputTrigger) {
    if (g_buttonCount == 0 && !g_hasDPad) return 0;
    
    // Проверяем D-Pad
    if (g_hasDPad && g_dpad.lastDirection) {
        if (inputTrigger == SCE_CTRL_UP && (g_dpad.lastDirection & SCE_CTRL_UP)) return 1;
        if (inputTrigger == SCE_CTRL_DOWN && (g_dpad.lastDirection & SCE_CTRL_DOWN)) return 1;
        if (inputTrigger == SCE_CTRL_LEFT && (g_dpad.lastDirection & SCE_CTRL_LEFT)) return 1;
        if (inputTrigger == SCE_CTRL_RIGHT && (g_dpad.lastDirection & SCE_CTRL_RIGHT)) return 1;
    }
    
    // Проверяем обычные кнопки
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        if (btn->inputTrigger == inputTrigger && btn->state) {
            return 1;
        }
    }
    return 0;
}

// Показать/скрыть кнопки
void virtualButtonShow(int inputTrigger) {
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        if (btn->inputTrigger == inputTrigger) {
            btn->enabled = 1;
        }
    }
}

void virtualButtonHide(int inputTrigger) {
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        if (btn->inputTrigger == inputTrigger) {
            btn->enabled = 0;
        }
    }
}

void virtualButtonsShowAll(void) {
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        btn->enabled = 1;
    }
    if (g_hasDPad) g_dpad.enabled = 1;
}

void virtualButtonsHideAll(void) {
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        btn->enabled = 0;
    }
    if (g_hasDPad) g_dpad.enabled = 0;
}

// Для отладки
void virtualButtonsDrawDebug(void) {
    printf("=== Virtual Buttons ===\n");
    if (g_hasDPad) {
        printf("DPad: %s, dir=%d\n", 
               g_dpad.enabled ? "ON" : "OFF", 
               g_dpad.lastDirection);
    }
    for (int i = 0; i < g_buttonCount; i++) {
        VirtualButton* btn = &g_buttons[i];
        if (!btn) continue;
        
        printf("%s: state=%d, frame=%d\n", 
               btn->name, 
               btn->state,
               btn->pressFrame);
    }
}