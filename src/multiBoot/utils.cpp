#include "utils.h"

int xstrtoi(char * str, int len) 
{
	int val;
	int c;
	int i;
	val = 0;
	for (i = 0; i < len; i++){
		c = *(str + i);
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else if (c >= 'A' && c <= 'F') {
			c = (c - 'A') + 10;
		} else if (c >= 'a' && c <= 'f') {
			c = (c - 'a') + 10;
		} else {
			return 0;
		}
		val *= 16;
		val += c;
	}
	return val;
};

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

char * get_launch_address(int argc, char * hex_string)
{
	if(argc > 1)
	{
		tMenuApi * settings;
		char * hex = hex_string;
		*(hex + 8 ) = 0;
		int apiAddr = xstrtoi(hex, 8); //transform string address into a hex number
		if(apiAddr) 
		{
			settings = (tMenuApi *)apiAddr;
			return settings->filename;
		};
	};
	
	return NULL;
};