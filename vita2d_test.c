#include <psp2/kernel/processmgr.h>
#include <vita2d.h>

int main() {
    vita2d_init();
    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}