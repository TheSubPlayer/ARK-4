#include <pspsdk.h>
#include <pspinit.h>
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
#include "high_mem.h"
#include "exitgame.h"
#include "libs/graphics/graphics.h"

extern u32 psp_model;
extern ARKConfig* ark_config;
extern STMOD_HANDLER previous;
extern void SetSpeed(int cpuspd, int busspd);

// Return Boot Status
static int isSystemBooted(void)
{

    // Find Function
    int (* _sceKernelGetSystemStatus)(void) = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemForKernel", 0x452E3696);
    
    // Get System Status
    int result = _sceKernelGetSystemStatus();
        
    // System booted
    if(result == 0x20000) return 1;
    
    // Still booting
    return 0;
}

static int _sceKernelBootFromForUmdMan(void)
{
    return 0x20;
}

static void patch_sceUmdMan_driver(SceModule* mod)
{
    int apitype = sceKernelInitApitype();
    if (apitype == 0x152 || apitype == 0x141) {
        hookImportByNID(mod, "InitForKernel", 0x27932388, _sceKernelBootFromForUmdMan);
    }
}

//prevent umd-cache in homebrew, so we can drain the cache partition.
static void patch_umdcache(SceModule2* mod)
{
    int apitype = sceKernelInitApitype();
    if (apitype == 0x152 || apitype == 0x141){
        //kill module start
        u32 text_addr = mod->text_addr;
        MAKE_DUMMY_FUNCTION_RETURN_1(text_addr+0x000009C8);
    }
}

static void patch_sceWlan_Driver(SceModule2* mod)
{
    // disable frequency check
    u32 text_addr = mod->text_addr;
    _sw(NOP, text_addr + 0x000026C0);
}

static void patch_scePower_Service(SceModule2* mod)
{
    // scePowerGetBacklightMaximum always returns 4
    u32 text_addr = mod->text_addr;
    _sw(NOP, text_addr + 0x00000E68);
}

static void disable_PauseGame(SceModule2* mod)
{
    u32 text_addr = mod->text_addr;
    if(psp_model == PSP_GO) {
        for(int i=0; i<2; i++) {
            _sw(NOP, text_addr + 0x00000574 + i * 4);
        }
    }
}

static int is_launcher_mode = 0;
static int use_mscache = 0;
void settingsHandler(char* path){
    if (strcasecmp(path, "overclock") == 0){ // set CPU speed to max
        SetSpeed(333, 166);
    }
    else if (strcasecmp(path, "powersave") == 0){ // underclock to save battery
        int apitype = sceKernelInitApitype();
        if (apitype != 0x144 && apitype != 0x155) // prevent operation in pops
            SetSpeed(133, 66);
    }
    else if (strcasecmp(path, "usbcharge") == 0){
        usb_charge(); // enable usb charging
    }
    else if (strcasecmp(path, "highmem") == 0){ // enable high memory
        if (psp_model > PSP_1000) { // prevent operation in PSP 1K
            int apitype = sceKernelInitApitype();
            if (apitype !=  0x210 && apitype !=  0x220 // prevent operation in VSH
                  && apitype != 0x144 && apitype != 0x155 // prevent operation in pops
              ){
                patch_partitions();
                flushCache();
            }
        }
    }
    else if (strcasecmp(path, "mscache") == 0){
        use_mscache = 1; // enable ms cache for speedup
    }
    else if (strcasecmp(path, "disablepause") == 0){ // disable pause game feature on psp go
        disable_PauseGame(sceKernelFindModuleByName("sceImpose_Driver"));
    }
    else if (strcasecmp(path, "launcher") == 0){ // replace XMB with custom launcher
        is_launcher_mode = 1;
    }
}

void PSPOnModuleStart(SceModule2 * mod){

    // System fully booted Status
    static int booted = 0;
    
    if(strcmp(mod->modname, "sceUmdMan_driver") == 0) {
        patch_sceUmdMan_driver(mod);
        goto flush;
    }

    if(strcmp(mod->modname, "sceUmdCache_driver") == 0) {
        patch_umdcache(mod);
        goto flush;
    }

    if(strcmp(mod->modname, "sceWlan_Driver") == 0) {
        patch_sceWlan_Driver(mod);
        goto flush;
    }

    if(strcmp(mod->modname, "scePower_Service") == 0) {
        patch_scePower_Service(mod);
        goto flush;
    }
    
    if (strcmp(mod->modname, "sceLoadExec") == 0){
        if (psp_model > PSP_1000 && sceKernelApplicationType() == PSP_INIT_KEYCONFIG_GAME) {
            prepatch_partitions();
            goto flush;
        }
    }
    
    if (strcmp(mod->modname, "sceMediaSync") == 0){
        // load and process settings file
        loadSettings(&settingsHandler);
        if (is_launcher_mode){
            strcpy(ark_config->launcher, ARK_MENU); // set CFW in launcher mode
        }
        else{
            ark_config->launcher[0] = 0; // disable launcher mode
        }
        sctrlHENSetArkConfig(ark_config); // notify SystemControl of changes in runtime config
        goto flush;
    }
    
    if(booted == 0)
    {
        // Boot is complete
        if(isSystemBooted())
        {
            if (use_mscache){
                if (psp_model == PSP_GO)
                    msstorCacheInit("eflash0a0f1p", 8 * 1024);
                else
                    msstorCacheInit("msstor0p", 16 * 1024);
            }
            // Boot Complete Action done
            booted = 1;
            goto flush;
        }
    }
    
flush:
    flushCache();

    // Forward to previous Handler
    if(previous) previous(mod);
}

