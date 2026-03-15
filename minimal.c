#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <stdio.h>

int main() {
    FILE *log = fopen("ux0:data/minimal_log.txt", "w");
    fprintf(log, "Start\n");
    fflush(log);
    
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
    fprintf(log, "Ctrl OK\n");
    fflush(log);
    
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    fprintf(log, "Touch OK\n");
    fflush(log);
    
    for (int i = 0; i < 10; i++) {
        fprintf(log, "Loop %d\n", i);
        fflush(log);
        sceKernelDelayThread(500 * 1000);
    }
    
    fprintf(log, "Exit\n");
    fclose(log);
    sceKernelExitProcess(0);
    return 0;
}