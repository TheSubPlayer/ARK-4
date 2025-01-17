/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

/*
	PSP VSH MENU controll
	based Booster's vshex
*/

#include "common.h"
#include <psputility.h>

const char **g_messages = g_messages_en;

void change_clock(int dir, int a);

extern int pwidth;
extern char umd_path[72];
extern SEConfig cnf;

char freq_buf[3+3+2] = "";
char freq2_buf[3+3+2] = "";
char device_buf[13] = "";
char umdvideo_path[256] = "";

#define TMENU_MAX 16

enum{
	//TMENU_XMB_CLOCK,
	//TMENU_GAME_CLOCK,
	TMENU_USB_DEVICE,
	TMENU_USB_READONLY,
	TMENU_UMD_MODE,
	TMENU_UMD_VIDEO,
	TMENU_COLORS,
	TMENU_CONVERT_BATTERY,
	TMENU_SWAP_XO_BUTTONS,
	TMENU_ACTIVATE_FLASH_WMA,
	TMENU_DELETE_HIBERNATION,
//	TMENU_XMB_PLUGINS,
//	TMENU_GAME_PLUGINS,
//	TMENU_POPS_PLUGINS,
	TMENU_CUSTOM_LAUNCHER,
//	TMENU_USB_CHARGE,
	TMENU_RECOVERY_MENU,
//	TMENU_SKIP_GAMEBOOT,
//	TMENU_HIDE_PIC,
//	TMENU_FLASH_PROT,
//	TMENU_FAKE_REGION,
	TMENU_SHUTDOWN_DEVICE,
	TMENU_SUSPEND_DEVICE,
	TMENU_RESET_DEVICE,
	TMENU_RESET_VSH,
	TMENU_EXIT
};

int item_fcolor[TMENU_MAX];
const char *item_str[TMENU_MAX];

static int menu_sel = TMENU_USB_DEVICE;

const int xyPoint[] ={0x98, 0x30, 0xC0, 0xA0, 0x70, 0x08, 0x0E, 0xA8};//data243C=
const int xyPoint2[] ={0xB0, 0x30, 0xD8, 0xB8, 0x88, 0x08, 0x11, 0xC0};//data2458=

int is_pandora = 0;
int colors_dir = 0;

int menu_draw(void)
{
	u32 fc,bc;
	const char *msg;
	int max_menu, cur_menu;
	const int *pointer;
	int xPointer;
	
	// ARK Version
	const char ark_version[24];
	int ver = sctrlHENGetMinorVersion();
 	int major = (ver&0xFF0000)>>16;
	int minor = (ver&0xFF00)>>8;
	int micro = (ver&0xFF);

	#ifdef DEBUG
    if (micro>0) snprintf(ark_version, sizeof(ark_version), " ARK %d.%d.%d DEBUG ", major, minor, micro);
    else snprintf(ark_version, sizeof(ark_version), " ARK %d.%d DEBUG ", major, minor);
    #else
    if (micro>0) snprintf(ark_version, sizeof(ark_version), "    ARK %d.%d.%d    ", major, minor, micro);
    else snprintf(ark_version, sizeof(ark_version), "    ARK %d.%d    ", major, minor); 
	#endif

	// check & setup video mode
	if( blit_setup() < 0) return -1;

	if(pwidth==720) {
		pointer = xyPoint;
	} else {
		pointer = xyPoint2;
	}

	// show menu list
	blit_set_color(0xffffff,0x8000ff00);
	blit_string(pointer[0], pointer[1], g_messages[MSG_PRO_VSH_MENU]);
	blit_string(pointer[0], 56, ark_version);
//	blit_string(168, (pointer[1] + sizeof(ark_version))*8, ark_version);
 
	for(max_menu=0;max_menu<TMENU_MAX;max_menu++) {
		fc = 0xffffff;
										    // 0xc00000ff original     abgr
		msg = g_messages[MSG_USB_DEVICE + max_menu];
		switch(cnf.vsh_colors) {
						// Red
						case 0: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x000000ff;
							blit_set_color(fc,bc);
							break;
						// Light Red
						case 1: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa00000ff;
							blit_set_color(fc,bc);
							break;
						// Orange
						case 2: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x0000a5ff;
							blit_set_color(fc,bc);
							break;
						// Light Orange
						case 3: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa000a5ff;
							blit_set_color(fc,bc);
							break;
						// Yellow
						case 4: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x0000e6e6;
							blit_set_color(fc,bc);
							break;
						// Light Yellow
						case 5: 
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa000e6e6;
							blit_set_color(fc,bc);
							break;
						// Green
						case 6:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x0000b300;
							blit_set_color(fc,bc);
						// Light Green
						case 7:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa000ff00;
							blit_set_color(fc,bc);
							break;
						// Blue
						case 8:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00ff0000;
							blit_set_color(fc,bc);
							break;
						// Light Blue
						case 9:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0ff0000;
							blit_set_color(fc,bc);
							break;
						// Indigo
						case 10:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x0082004b;
							blit_set_color(fc,bc);
							break;
						// Light Indigo
						case 11:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa082004b;
							blit_set_color(fc,bc);
							break;
						// Violet
						case 12:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00ee82ee;
							blit_set_color(fc,bc);
							break;
						// Light Violet
						case 13:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0ee82ee;
							blit_set_color(fc,bc);
							break;
						// Pink 
						case 14:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00cbc0ff;
							blit_set_color(fc,bc);
							break;
						// Light Pink 
						case 15:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0cbc0ff;
							blit_set_color(fc,bc);
							break;
						// Purple 
						case 16:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00993366;
							blit_set_color(fc,bc);
							break;
						// Light Purple 
						case 17:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0993366;
							blit_set_color(fc,bc);
							break;
						// Teal 
						case 18:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00808000;
							blit_set_color(fc,bc);
							break;
						// Light Teal 
						case 19:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0808000;
							blit_set_color(fc,bc);
							break;
						// Aqua 
						case 20:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00cccc00;
							blit_set_color(fc,bc);
							break;
						// Light Aqua 
						case 21:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0cccc00;
							blit_set_color(fc,bc);
							break;
						// Grey 
						case 22:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00737373;
							blit_set_color(fc,bc);
							break;
						// Light Grey 
						case 23:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0737373;
							blit_set_color(fc,bc);
							break;
						// Black 
						case 24:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0x00000000;
							blit_set_color(fc,bc);
							break;
						// Light Black 
						case 25:
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa0000000;
							blit_set_color(fc,bc);
							break;
						default:	
							bc = (max_menu==menu_sel) ? 0xff8080 : 0xa00000ff;
							blit_set_color(fc,bc);
					}


		if(msg) {
				bc = (max_menu==menu_sel) ? 0xff8080 : 0x0000ff00;
			switch(max_menu) {
				case TMENU_EXIT:
					xPointer = pointer[2];
					break;
				case TMENU_RESET_DEVICE:
					if (cur_language == PSP_SYSTEMPARAM_LANGUAGE_GERMAN) {
						xPointer = pointer[3] - 2 * 8 - 1;
					} else {
						xPointer = pointer[3];
					}
					
					break;
				case TMENU_RESET_VSH:
					if (cur_language == PSP_SYSTEMPARAM_LANGUAGE_GERMAN) {
						xPointer = pointer[7] - 2 * 8 - 1;
					} else {
						xPointer = pointer[7];
					}
					
					break;
				case TMENU_DELETE_HIBERNATION:
				case TMENU_CUSTOM_LAUNCHER:
				case TMENU_RECOVERY_MENU:
					xPointer = 168;
					break;
				case TMENU_ACTIVATE_FLASH_WMA:
					xPointer = 153;
					break;
				case TMENU_SHUTDOWN_DEVICE:
					xPointer = 176;
					break;
				case TMENU_SUSPEND_DEVICE:
					xPointer = 176;
					break;
				default:
					xPointer=pointer[4];
					break;
			}



			cur_menu = max_menu;
			blit_string(xPointer, (pointer[5] + cur_menu)*8, msg);
			msg = item_str[max_menu];
			if (max_menu == TMENU_CONVERT_BATTERY){
				if (is_pandora){
					if (is_pandora < 0) msg = "Unsupported";
					else msg = "Pandora -> Normal";
				}
				else{
					msg = "Normal -> Pandora";
				}
				blit_string(xPointer+0x80, (pointer[5] + cur_menu)*8, msg);
			}
			else if(msg) {
				//blit_set_color(item_fcolor[max_menu],bc);
				//blit_set_color(fc,bc);
				blit_string( (pointer[6] * 8) + 128, (pointer[5] + cur_menu)*8, msg);
			}

		}

	}

	blit_set_color(0x00ffffff,0x00000000);

	return 0;
}

static inline const char *get_enable_disable(int opt)
{
	if(opt) {
		return g_messages[MSG_ENABLE];
	}

	return g_messages[MSG_DISABLE];
}

int menu_setup(void)
{
	int i;
	const char *bridge;
	const char *umdvideo_disp;

	// preset
	for(i=0;i<TMENU_MAX;i++) {
		item_str[i] = NULL;
		item_fcolor[i] = RGB(255,255,255);
	}

	/*//xmb clock
	if( cpu2no(cnf.vshcpuspeed) && ( bus2no(cnf.vshbusspeed)))	{		

#ifdef CONFIG_639
		if(psp_fw_version == FW_639)
			scePaf_sprintf(freq_buf, "%d/%d", cnf.vshcpuspeed, cnf.vshbusspeed);
#endif

#ifdef CONFIG_635
		if(psp_fw_version == FW_635)
			scePaf_sprintf(freq_buf, "%d/%d", cnf.vshcpuspeed, cnf.vshbusspeed);
#endif

#ifdef CONFIG_620
		if (psp_fw_version == FW_620)
			scePaf_sprintf_620(freq_buf, "%d/%d", cnf.vshcpuspeed, cnf.vshbusspeed);
#endif
		
#if defined(CONFIG_660) || defined(CONFIG_661)
		if ((psp_fw_version == FW_660) || (psp_fw_version == FW_661))
			scePaf_sprintf_660(freq_buf, "%d/%d", cnf.vshcpuspeed, cnf.vshbusspeed);
#endif
		
		bridge = freq_buf;
	} else {
		bridge = g_messages[MSG_DEFAULT];
	}

	item_str[TMENU_XMB_CLOCK] = bridge;

	//game clock
	if(cpu2no(cnf.umdisocpuspeed) && (bus2no(cnf.umdisobusspeed))) {		
#ifdef CONFIG_639
		if(psp_fw_version == FW_639)
			scePaf_sprintf(freq2_buf, "%d/%d", cnf.umdisocpuspeed, cnf.umdisobusspeed);
#endif

#ifdef CONFIG_635
		if(psp_fw_version == FW_635)
			scePaf_sprintf(freq2_buf, "%d/%d", cnf.umdisocpuspeed, cnf.umdisobusspeed);
#endif

#ifdef CONFIG_620
		if (psp_fw_version == FW_620)
			scePaf_sprintf_620(freq2_buf, "%d/%d", cnf.umdisocpuspeed, cnf.umdisobusspeed);
#endif
		
#if defined(CONFIG_660) || defined(CONFIG_661)
		if ((psp_fw_version == FW_660) || (psp_fw_version == FW_661))
			scePaf_sprintf_660(freq2_buf, "%d/%d", cnf.umdisocpuspeed, cnf.umdisobusspeed);
#endif
		
		bridge = freq2_buf;
	} else {
		bridge = g_messages[MSG_DEFAULT];
	}

	item_str[TMENU_GAME_CLOCK] = bridge;
*/
	//usb device
	if((cnf.usbdevice>0) && (cnf.usbdevice<5)) {
#ifdef CONFIG_639
		if(psp_fw_version == FW_639)
			scePaf_sprintf(device_buf, "%s %d", g_messages[MSG_FLASH], cnf.usbdevice-1);	
#endif

#ifdef CONFIG_635
		if(psp_fw_version == FW_635)
			scePaf_sprintf(device_buf, "%s %d", g_messages[MSG_FLASH], cnf.usbdevice-1);	
#endif

#ifdef CONFIG_620
		if (psp_fw_version == FW_620)
			scePaf_sprintf_620(device_buf, "%s %d", g_messages[MSG_FLASH], cnf.usbdevice-1);	
#endif

#if defined(CONFIG_660) || defined(CONFIG_661)
		if ((psp_fw_version == FW_660) || (psp_fw_version == FW_661))
			scePaf_sprintf_660(device_buf, "%s %d", g_messages[MSG_FLASH], cnf.usbdevice-1);	
#endif

		bridge = device_buf;
	} else {
		const char *device;

		if(cnf.usbdevice==5)
			device= g_messages[MSG_UMD_DISC];
		else
			device= g_messages[MSG_MEMORY_STICK];

		bridge = device;
	}

	umdvideo_disp = strrchr(umdvideo_path, '/');

	if(umdvideo_disp == NULL) {
		umdvideo_disp = umdvideo_path;
	} else {
		umdvideo_disp++;
	}

	item_str[TMENU_UMD_VIDEO] = umdvideo_disp;
	item_str[TMENU_USB_DEVICE] = bridge;

	switch(cnf.umdmode) {
		case MODE_NP9660:
			item_str[TMENU_UMD_MODE] = g_messages[MSG_NP9660];
			break;
		case MODE_INFERNO:
			item_str[TMENU_UMD_MODE] = g_messages[MSG_INFERNO];
			break;
		default:
			item_str[TMENU_UMD_MODE] = g_messages[MSG_INFERNO];
	}

	switch(cnf.usbdevice_rdonly) {
		case 0:
			item_str[TMENU_USB_READONLY] = g_messages[MSG_DISABLE];
			break;
		case 1:
			item_str[TMENU_USB_READONLY] = g_messages[MSG_ENABLE];
			break;
		default:
			item_str[TMENU_USB_READONLY] = g_messages[MSG_ENABLE];
	}

	switch(cnf.swap_xo) {
		case 0:
			item_str[TMENU_SWAP_XO_BUTTONS] = g_messages[MSG_DISABLE];
			break;
		case 1:
			item_str[TMENU_SWAP_XO_BUTTONS] = g_messages[MSG_ENABLE];
			break;
	}

	switch(cnf.vsh_colors) {
		case _RED:
			item_str[TMENU_COLORS] = g_messages[MSG_RED];
			break;
		case _LITE_RED:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_RED];
			break;
		case _ORANGE:
			item_str[TMENU_COLORS] = g_messages[MSG_ORANGE];
			break;
		case _LITE_ORANGE:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_ORANGE];
			break;
		case _YELLOW:
			item_str[TMENU_COLORS] = g_messages[MSG_YELLOW];
			break;
		case _LITE_YELLOW:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_YELLOW];
			break;
		case _GREEN:
			item_str[TMENU_COLORS] = g_messages[MSG_GREEN];
			break;
		case _LITE_GREEN:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_GREEN];
			break;
		case _BLUE:
			item_str[TMENU_COLORS] = g_messages[MSG_BLUE];
			break;
		case _LITE_BLUE:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_BLUE];
			break;
		case _INDIGO:
			item_str[TMENU_COLORS] = g_messages[MSG_INDIGO];
			break;
		case _LITE_INDIGO:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_INDIGO];
			break;
		case _VIOLET:
			item_str[TMENU_COLORS] = g_messages[MSG_VIOLET];
			break;
		case _LITE_VIOLET:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_VIOLET];
			break;
		case _PINK:
			item_str[TMENU_COLORS] = g_messages[MSG_PINK];
			break;
		case _LITE_PINK:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_PINK];
			break;
		case _PURPLE:
			item_str[TMENU_COLORS] = g_messages[MSG_PURPLE];
			break;
		case _LITE_PURPLE:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_PURPLE];
			break;
		case _TEAL:
			item_str[TMENU_COLORS] = g_messages[MSG_TEAL];
			break;
		case _LITE_TEAL:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_TEAL];
			break;
		case _AQUA:
			item_str[TMENU_COLORS] = g_messages[MSG_AQUA];
			break;
		case _LITE_AQUA:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_AQUA];
			break;
		case _GREY:
			item_str[TMENU_COLORS] = g_messages[MSG_GREY];
			break;
		case _LITE_GREY:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_GREY];
			break;
		case _BLACK:
			item_str[TMENU_COLORS] = g_messages[MSG_BLACK];
			break;
		case _LITE_BLACK:
			item_str[TMENU_COLORS] = g_messages[MSG_LITE_BLACK];
			break;
		default:
			item_str[TMENU_COLORS] = g_messages[MSG_RED];
	}

	return 0;
}

int menu_ctrl(u32 button_on)
{
	int direction;

	if( (button_on & PSP_CTRL_SELECT) ||
		(button_on & PSP_CTRL_HOME)) {
		menu_sel = TMENU_EXIT;
		return 1;
	}

	// change menu select
	direction = 0;

	if(button_on & PSP_CTRL_DOWN) direction++;
	if(button_on & PSP_CTRL_UP) direction--;

	menu_sel = limit(menu_sel+direction, 0, TMENU_MAX-1);

	// LEFT & RIGHT
	direction = -2;

	if(button_on & PSP_CTRL_LEFT)   direction = -1;
	if(button_on & PSP_CTRL_CROSS) direction = 0;
	if(button_on & PSP_CTRL_CIRCLE) direction = 0;
	if(button_on & PSP_CTRL_RIGHT)  direction = 1;

	if(direction <= -2)
		return 0;

	switch(menu_sel) {
/*		case TMENU_XMB_CLOCK:
			if(direction) change_clock( direction, 0);
			break;
		case TMENU_GAME_CLOCK:
			if(direction) change_clock( direction, 1);
			break;
*/
		case TMENU_USB_DEVICE:
			if(direction) change_usb( direction );
			break;
		case TMENU_USB_READONLY:
			// Other logic will go here
			if (direction) swap_readonly(direction);
			//else if(direction==0) return 11;
			break;
		case TMENU_UMD_MODE:
			if(direction) change_umd_mode( direction );
			break;
		case TMENU_UMD_VIDEO:
			if(direction) {
			   	change_umd_mount_idx(direction);

				if(umdvideo_idx != 0) {
					char *umdpath;

					umdpath = umdvideolist_get(&g_umdlist, umdvideo_idx-1);

					if(umdpath != NULL) {
						strncpy(umdvideo_path, umdpath, sizeof(umdvideo_path));
						umdvideo_path[sizeof(umdvideo_path)-1] = '\0';
					} else {
						goto none;
					}
				} else {
none:
					strcpy(umdvideo_path, g_messages[MSG_NONE]);
				}
			} else {
				return 6; // Mount UMDVideo ISO flag
			}
			break;
		case TMENU_DELETE_HIBERNATION:
			if(direction==0) {
				return 10; // Delete Hibernation flag 
			}
			break;
		case TMENU_ACTIVATE_FLASH_WMA:
			if(direction==0) {
				return 11; // Delete Hibernation flag 
			}
			break;
		case TMENU_SWAP_XO_BUTTONS:
			if(direction) { swap_xo( direction ); } // Swap X/O Buttons flag  
			else if (direction==0 && cnf.swap_xo == 1 ) return 12;
			break;
		case TMENU_CONVERT_BATTERY:
			if(direction==0) {
				return 9; // Convert Battery flag
			}
			break;
		case TMENU_COLORS:
			// This will be where I will be adding to set the color
			if(direction) change_colors(direction);
			break;
		case TMENU_CUSTOM_LAUNCHER:
			if(direction==0) {
				return 7; // Custom Launcher menu flag
			}
			break;
		case TMENU_RECOVERY_MENU:
			if(direction==0) {
				return 8; // Recovery menu flag
			}
			break;
		case TMENU_SHUTDOWN_DEVICE:			
			if(direction==0) {
				return 3; // SHUTDOWN flag
			}
			break;
		case TMENU_RESET_DEVICE:	
			if(direction==0) {
				return 2; // RESET flag
			}
			break;
		case TMENU_RESET_VSH:	
			if(direction==0) {
				return 4; // RESET VSH flag
			}
			break;
		case TMENU_SUSPEND_DEVICE:	
			if(direction==0) {
				return 5; // SUSPEND flag
			}
			break;
		case TMENU_EXIT:
			if(direction==0) return 1; // finish
			break;
	}

	return 0; // continue
}
