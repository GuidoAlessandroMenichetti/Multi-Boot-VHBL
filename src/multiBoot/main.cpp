/* Multi-Boot-VHBL v1.0 by GUIDOBOT */
//This is just a POC, don't expect fancy images/menus

#include "main.h"

void draw(const entry * names, int pos, int count)
{
	int i;
	pspDebugScreenSetXY(0,0);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
	pspDebugScreenPrintf("multi-Boot-VHBL\n\n");
	
	if(!count) pspDebugScreenPrintf("No menu found.");
	for(i=0;i<count;i++)
	{
		unsigned color = (i == pos)? 0xFF00FFFF: 0xFFFFFFFF;
		pspDebugScreenSetTextColor(color);
		pspDebugScreenPrintf("\t%-15s\n", names[i].name);
	};
};

int getNames(const char * file, entry * store)
{
	int count = 0;
	char buffer[BUFFER_LEN], * aux;
	FILE * a, * b;
	
	//open config file
	a = fopen(file, "rb");
	if(!a) return 0;
	
#ifdef DEBUG
	pspDebugScreenPrintf("Reading config file\n");
#endif
	
	//Get names from the config file
	while(fgets(buffer, sizeof(buffer), a) && count<MAX_MENUS)
	{
		//fix fgets extra chars
		aux = strchr(buffer, '\r');
		if(!aux)
			aux = strchr(buffer, '\n');
			
		if(aux) 
			* aux = '\0';
		
		//parse line
		aux = strchr(buffer, ',');
		if(aux)	strcpy(store[count].path, aux+1);
		else continue;
		* aux = '\0';
		
		//check file existence
		b = fopen(store[count].path, "rb");
		if(!b) continue;
		fclose(b);
		
#ifdef DEBUG
		pspDebugScreenPrintf("Adding %s\n", buffer);
#endif
		
		strcpy(store[count].name, buffer);
		count++;
	};
	fclose(a);
	return count;
};

int replace(const char * original, const char * replace, unsigned start, unsigned size, unsigned mask)
{
	//replaces all string ocurrences in a ram range
	char * pointer = (char *)start;
	int count = 0;
	while(size)
	{
		if(!strcmp(pointer, original))
		{
			strcpy((char *)((unsigned)pointer | (unsigned)mask), replace);
			count++;
		};
	
		size--;
		pointer++;
	};
	
	return count;
};


int main(int argc, char * argv[])
{
	int finished = 0, count, pos = 0;
	char exploit[BUFFER_LEN], * ebootPath = NULL;
	SceCtrlData ctrl;
	entry names[MAX_MENUS];
	
	count = getNames(CONFIG_FILE, names); //get names from CONFIG_FILE
	ebootPath = get_launch_address(argc, argv[1]); //get next-to-launch string address (vhbl stuff)
	SetupCallbacks();
	pspDebugScreenInit();
	pspDebugScreenClear();

	while(!finished)
	{	
		draw(names, pos, count);
		
		do //wait button input
		{
			sceCtrlReadBufferPositive(&ctrl, 1); 
		} while(!ctrl.Buttons);
		
		//Button detection
		if((ctrl.Buttons & PSP_CTRL_UP) && pos > 0) pos--;
		if((ctrl.Buttons & PSP_CTRL_DOWN) && pos < count-1) pos++;
		if((ctrl.Buttons & PSP_CTRL_CROSS)) finished = 1;
		
		do //wait button release
		{
			sceCtrlReadBufferPositive(&ctrl, 1); 
		} while(ctrl.Buttons);
	};
	
	//Make the full path
	strcpy(exploit, argv[0]);
	*(strrchr(exploit, '/') + 1) = '\0';
	strcat(exploit, names[pos].path); //changes EBOOT.PBP for selected menu file name

#ifdef DEBUG
	pspDebugScreenPrintf("Full menu path: %s\n", exploit);
#endif
	
	//Overwrite some memory values
	replace(argv[0], exploit, 0x00010000, 0x00004000, 0x40000000); //check scratchpad
	replace(argv[0], exploit, 0x08800000, 0x01800000, 0x00000000); //check user ram

	//Launch the dummy app
	strcpy(strrchr(exploit, '/') + 1, DUMMY_FILE); //changes EBOOT.PBP for dummy file name
	strcpy(ebootPath, exploit);
	
#ifdef DEBUG
	pspDebugScreenPrintf("Launching dummy: %s\n", ebootPath);
#endif
	
	sceKernelExitGame();
	return 0;
};
