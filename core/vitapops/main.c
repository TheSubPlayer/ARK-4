#include <pspsdk.h>
#include <globals.h>
#include <graphics.h>
#include <macros.h>
#include <module2.h>
#include <pspdisplay_kernel.h>
#include <pspsysmem_kernel.h>
#include <systemctrl.h>
#include <systemctrl_private.h>
#include <pspiofilemgr.h>
#include <pspgu.h>
#include <functions.h>
#include "popsdisplay.h"

PSP_MODULE_INFO("ARKVitaPopsCompat", 0x3007, 1, 0);

// Previous Module Start Handler
STMOD_HANDLER previous = NULL;

static ARKConfig _ark_conf;
ARKConfig* ark_config = &_ark_conf;

extern void patchVitaPopsDisplay(SceModule2* mod);

KernelFunctions _ktbl = {
    .KernelDcacheInvalidateRange = &sceKernelDcacheInvalidateRange,
    .KernelIcacheInvalidateAll = &sceKernelIcacheInvalidateAll,
    .KernelDcacheWritebackInvalidateAll = &sceKernelDcacheWritebackInvalidateAll,
    .KernelIOOpen = &sceIoOpen,
    .KernelIORead = &sceIoRead,
    .KernelIOClose = &sceIoClose,
    .KernelDelayThread = &sceKernelDelayThread,
}; // for vita flash patcher

// Flush Instruction and Data Cache
void flushCache()
{
    // Flush Instruction Cache
    sceKernelIcacheInvalidateAll();
    
    // Flush Data Cache
    sceKernelDcacheWritebackInvalidateAll();
}

// Return Boot Status
static int isSystemBooted(void)
{

    // Find Function
    int (* _sceKernelGetSystemStatus)(void) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemForKernel", 0x452E3696);
    
    // Get System Status
    int result = _sceKernelGetSystemStatus();
        
    // System booted
    if(result == 0x20000) return 1;
    
    // Still booting
    return 0;
}

void* sctrlARKSetPSXVramHandler(void (*handler)(u32* psp_vram, u16* ps1_vram)){
    int k1 = pspSdkSetK1(0);
    void* prev = registerPSXVramHandler(handler);
    pspSdkSetK1(k1);
    return prev;
}

// patch pops display to set up our own screen handler
void patchVitaPopsDisplay(SceModule2* mod){
    u32 display_func = FindFunction("sceDisplay_Service", "sceDisplay_driver", 0x3E17FE8D);
    if (display_func){
        // protect vita pops vram
        sceKernelAllocPartitionMemory(6, "POPS VRAM CONFIG", 2, 0x1B0, (void *)0x09FE0000);
        sceKernelAllocPartitionMemory(6, "POPS VRAM", 2, 0x3C0000, (void *)0x090C0000);
        memset((void *)0x49FE0000, 0, 0x1B0);
        memset((void *)0x490C0000, 0, 0x3C0000);
        // register default screen handler
        registerPSXVramHandler(&SoftRelocateVram);
        // initialize screen configuration
        initVitaPopsVram();
        // patch display function
        HIJACK_FUNCTION(display_func, sceDisplaySetFrameBufferInternalHook,
            _sceDisplaySetFrameBufferInternal);
    }
}

static void ARKVitaPopsOnModuleStart(SceModule2 * mod){

    static int booted = 0;

    // Patch display in PSX exploits
    if(strcmp(mod->modname, "sceDisplay_Service") == 0) {
        patchVitaPopsDisplay(mod);
        goto flush;
    }
    
    // Kermit Peripheral Patches
    if(strcmp(mod->modname, "sceKermitPeripheral_Driver") == 0)
    {
        // Patch Kermit Peripheral Module to load flash0
        patchKermitPeripheral(&_ktbl);
        // Exit Handler
        goto flush;
    }
    
    // Boot Complete Action not done yet
    if(booted == 0)
    {
        // Boot is complete
        if(isSystemBooted())
        {
            // Allow exiting through key combo
            patchExitGame();
            // Boot Complete Action done
            booted = 1;
            goto flush;
        }
    }

flush:
    flushCache();

exit:

    // Forward to previous Handler
    if(previous) previous(mod);
}

// Boot Time Entry Point
int module_start(SceSize args, void * argp)
{
    // copy configuration
    sctrlHENGetArkConfig(ark_config);
    // Register Module Start Handler
    previous = sctrlHENSetStartModuleHandler(ARKVitaPopsOnModuleStart);
    // Return Success
    return 0;
}