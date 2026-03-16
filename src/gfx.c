#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>
#include <psp2/types.h>
#include <string.h>

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 544
#define PIXEL_SIZE 4 // 32-bit color
#define FB_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT * PIXEL_SIZE)

// Простая функция для рисования пикселя
void draw_pixel(void *base, int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *pixel = (uint32_t *)((uint8_t *)base + (y * SCREEN_WIDTH + x) * PIXEL_SIZE);
    *pixel = color;
}

// Рисуем прямоугольник
void draw_rect(void *base, int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            draw_pixel(base, x + i, y + j, color);
        }
    }
}

int main() {
    // Получаем информацию о дисплее
    SceDisplayFrameBuf frameBuf;
    memset(&frameBuf, 0, sizeof(frameBuf));
    frameBuf.size = sizeof(frameBuf);
    
    // Инициализируем фреймбуфер
    sceDisplaySetFrameBuf(NULL, SCE_DISPLAY_SETBUF_NEXTFRAME);
    sceDisplayWaitVblankStart();
    
    // Получаем текущий фреймбуфер
    sceDisplayGetFrameBuf(&frameBuf, SCE_DISPLAY_SETBUF_NEXTFRAME);
    
    if (!frameBuf.base) {
        // Если не получили — выходим
        sceKernelExitProcess(0);
        return 1;
    }
    
    // Рисуем что-нибудь простенькое
    // Белый прямоугольник в центре
    draw_rect(frameBuf.base, 480 - 100, 272 - 50, 200, 100, 0xFFFFFFFF);
    
    // Жёлтая рамка вокруг
    for (int i = 0; i < 200; i++) {
        draw_pixel(frameBuf.base, 480 - 100 + i, 272 - 50, 0xFFFFFF00);
        draw_pixel(frameBuf.base, 480 - 100 + i, 272 + 49, 0xFFFFFF00);
    }
    for (int i = 0; i < 100; i++) {
        draw_pixel(frameBuf.base, 480 - 100, 272 - 50 + i, 0xFFFFFF00);
        draw_pixel(frameBuf.base, 480 + 99, 272 - 50 + i, 0xFFFFFF00);
    }
    
    // Ждём 5 секунд, чтобы увидеть результат
    sceKernelDelayThread(5 * 1000 * 1000);
    
    sceKernelExitProcess(0);
    return 0;
}