#ifndef VIRTUAL_BUTTONS_H
#define VIRTUAL_BUTTONS_H

#include <psp2/types.h>
#include <psp2/touch.h>

// Режимы ввода (как в плагине)
typedef enum {
    INPUT_METHOD_TRIGGERED = 0,  // Однократно при нажатии
    INPUT_METHOD_PRESSED = 1,     // Каждый кадр пока нажато
    INPUT_METHOD_REPEATED = 2,    // С задержкой и повтором
    INPUT_METHOD_LONG_PRESSED = 3, // После задержки каждый кадр
    INPUT_METHOD_RELEASED = 4     // При отпускании
} InputMethod;

// Типы кнопок
typedef enum {
    BUTTON_TYPE_DPAD,      // D-Pad (специальная обработка)
    BUTTON_TYPE_KEY        // Обычная кнопка
} ButtonType;

// Структура для обычной кнопки
typedef struct {
    char name[32];          // Имя кнопки (для отладки)
    ButtonType type;
    int x, y;               // Центр кнопки
    int width, height;      // Размер
    int inputTrigger;       // Какую кнопку эмулировать (SCE_CTRL_CROSS и т.д.)
    InputMethod method;     // Режим ввода
    int state;              // 0 = отжата, 1 = нажата
    int pressFrame;         // Сколько кадров нажата
    int lastPressFrame;     // Для повторов
    int enabled;            // 1 = активна, 0 = скрыта
} VirtualButton;

// Структура для D-Pad (специальный случай)
typedef struct {
    int x, y;               // Центр D-Pad
    int size;               // Размер (ширина и высота одинаковые)
    int lastDirection;      // Последнее направление (для очистки)
    int enabled;            // 1 = активен, 0 = скрыт
    int diagonalEnabled;    // Разрешить диагонали
} VirtualDPad;

// Инициализация
void virtualButtonsInit(void);

// Добавление кнопок
int virtualButtonAdd(const char* name, int x, int y, int width, int height, 
                     int inputTrigger, InputMethod method);

// Настройка D-Pad
void virtualDPadSetup(int x, int y, int size, int diagonalEnabled);

// Обновление состояния (вызывать каждый кадр)
void virtualButtonsUpdate(SceTouchData* touch, int touchNum);

// Проверка состояния кнопок (аналог Input.isPressed)
int virtualButtonPressed(int inputTrigger);

// Показать/скрыть кнопки
void virtualButtonShow(int inputTrigger);
void virtualButtonHide(int inputTrigger);
void virtualButtonsShowAll(void);
void virtualButtonsHideAll(void);

// Для отладки
void virtualButtonsDrawDebug(void);

#endif