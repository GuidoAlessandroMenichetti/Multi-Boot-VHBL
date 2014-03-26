#ifndef UTILS_HEADER
#define UTILS_HEADER

#include <pspkernel.h>

typedef struct 
{
	unsigned long APIVersion;
	char Credits[32];
	char VersionName[32];
	char *BackgroundFilename;
	char *filename;
} tMenuApi;

int xstrtoi(char * str, int len);
int exit_callback(int arg1, int arg2, void * common);
int CallbackThread(SceSize args, void * argp);
int SetupCallbacks();
char * get_launch_address(int argc, char * hex_string);

#endif