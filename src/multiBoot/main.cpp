/* Multi-Boot-VHBL v1.0 by GUIDOBOT */

#include <stdio.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <string.h>
#include "vhblBoot.h"

#define NAME_MAX_LEN 15
#define MAX_MENUS 10
#define BUFFER_LEN 200
#define CONFIG_FILE "MBMCFG.TXT"
#define DUMMY_FILE "DUMMY.PBP"

typedef struct 
{
	char name[NAME_MAX_LEN];
	char path[NAME_MAX_LEN];
} entry;

PSP_MODULE_INFO("MULTIBOOT", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

tMenuApi * settings;
void * ebootPath;

int exit_callback(int arg1, int arg2, void *common) 
{
	sceKernelExitGame();
	return 0;
};

int CallbackThread(SceSize args, void *argp) 
{
	int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
};

int SetupCallbacks() 
{
	int thid = 0;
	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0) sceKernelStartThread(thid, 0, 0);
	return thid;
};

void draw(const entry * names, int pos, int count)
{
	int i;
	pspDebugScreenSetXY(0,0);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenPrintf("multi-Boot-VHBL\n\n");
	
	if(!count) pspDebugScreenPrintf("No menu found.");
	for(i=0;i<count;i++)
	{
		if(i==pos) pspDebugScreenSetTextColor(0xFF00FFFF);
		else pspDebugScreenSetTextColor(0xFFFFFFFF);
		pspDebugScreenPrintf("\t%-15s\n", names[i].name);
	};
};

int getNames(const char * file, entry * store)
{
	int count = 0;
	char buffer[BUFFER_LEN], * aux;
	FILE * a, * b;
	a = fopen(file, "rb");
	if(!a) return 0;
	
	//Get names from the config file
	
	while(fgets(buffer, sizeof(buffer), a) && count<MAX_MENUS)
	{
		aux = strchr(buffer, '\r');
		if(aux) * aux = '\0';
		
		aux = strchr(buffer, ',');
		if(aux)	strcpy(store[count].path, aux+1);
		else continue;
		* aux='\0';
		
		b = fopen(store[count].path, "rb");
		if(!b) continue;
		fclose(b);
		
		strcpy(store[count].name, buffer);
		count++;
	};
	fclose(a);
	return count;
};

unsigned getMenuOffset(const char * path)
{
	unsigned aux = 0;
	int i, len = strlen(path), flag = 0;
	for(i=0;i<0x3590;i++)
	{
		if(!strncmp((char*)(0x00010000+i), path, len))
		{
			if(flag)
				return 0x40010000+i;
			else
			{
				aux = 0x40010000+i;
				flag = 1;
			};
		};
	};
	return aux;
};

int main(int argc, char * argv[])
{
	int apiAddr = 0, finished = 0, count, pos = 0;
	char exploit[BUFFER_LEN], * menuOffset = NULL;
	entry names[MAX_MENUS];
	SceCtrlData ctrl;

	//VHBL stuff
	if(argc>1) 
	{
		char * hex = argv[1];
        *(hex + 8 ) = 0;
        apiAddr = xstrtoi(hex, 8);	
		if(apiAddr) 
		{
			settings = (tMenuApi *) apiAddr;
			ebootPath = (void *) settings->filename;
		};
	};
	
	SetupCallbacks();
	pspDebugScreenInit();
	
	count = getNames(CONFIG_FILE, names);
	strcpy(exploit, argv[0]);
	//Get the menu string address in ram
	menuOffset = (char *)(getMenuOffset(exploit));
	draw(names, pos, count);
	
	while(!finished)
	{
		do 
		{
			sceCtrlReadBufferPositive(&ctrl, 1);
		} while(!ctrl.Buttons);
		
		//Button detection
		if((ctrl.Buttons & PSP_CTRL_UP) && pos>0) pos--;
		if((ctrl.Buttons & PSP_CTRL_DOWN) && pos<count-1) pos++;
		if((ctrl.Buttons & PSP_CTRL_CROSS)) finished = 1;
		
		draw(names, pos, count);
		
		do 
		{
			sceCtrlReadBufferPositive(&ctrl, 1);
		} while(ctrl.Buttons);
	};
	
	//Make the full path
	*(strrchr(exploit, '/')+1) = '\0';
	strcat(exploit, names[pos].path);
	
	//Overwrite some scratchpad values
	strcpy(menuOffset, exploit);

	//Launch the dummy app
	strcpy(strrchr(exploit, '/')+1, DUMMY_FILE);
	strcpy((char*)ebootPath, exploit);
	
	sceKernelExitGame();
	return 0;
};
