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

#include "common.h"

const char *g_messages_en[] = {
	"Default",
	"Disable",
	//"Hidden",
	"Enable",
	"Normal",
	"Toggle",
	"X Prim",
	"O Prim",
	"Sony NP9660 (PBP)",
	"Inferno 2 (ISO/CSO)",
	//"CPU CLOCK XMB  ",
	//"CPU CLOCK GAME ",
	"USB DEVICE     ",
	"USB READONLY   ",
	"UMD ISO MODE   ",
	"ISO VIDEO MOUNT",
	"COLORS         ",
	"CONVERT BATTERY",
	"SWAP X/O BTNS  ",
	"ACTIVATE FLASH AND WMA",
	"DELETE HIBERNATION",
	"CUSTOM LAUNCHER ->",
	"RECOVERY MENU  ->",
	"SHUTDOWN DEVICE",
	"SUSPEND DEVICE",
	"RESET DEVICE",
	"RESET VSH",
	"EXIT",
	" Advanced VSH Menu ",
	"Flash",
	"UMD Disc",
	"Memory Stick",
	"None",
	"Red",
	"Light Red",
	"Orange",
	"Light Orange",
	"Yellow",
	"Light Yellow",
	"Green",
	"Light Green",
	"Blue",
	"Light Blue",
	"Indigo",
	"Light Indigo",
	"Violet",
	"Light Violet",
	"Pink",
	"Light Pink",
	"Purple",
	"Light Purple",
	"Teal",
	"Light Teal",
	"Aqua",
	"Light Aqua",
	"Grey",
	"Light Grey",
	"Black",
	"Light Black",
};
u8 message_test_en[NELEMS(g_messages_en) == MSG_END ? 0 : -1];
