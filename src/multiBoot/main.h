/* Multi-Boot-VHBL v1.0 by GUIDOBOT */
//This is just a POC, don't expect fancy images/menus

#ifndef MAIN_HEADER
#define MAIN_HEADER

#include <stdio.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <string.h>
#include "utils.h"

#define NAME_MAX_LEN 15
#define MAX_MENUS 10
#define BUFFER_LEN 200
#define CONFIG_FILE "MBMCFG.TXT"
#define DUMMY_FILE "DUMMY.PBP"
//define DEBUG for on-screen debugging
//#define DEBUG

typedef struct s_entry
{
	char name[NAME_MAX_LEN];
	char path[NAME_MAX_LEN];
} entry;

PSP_MODULE_INFO("MULTIBOOT", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

#endif