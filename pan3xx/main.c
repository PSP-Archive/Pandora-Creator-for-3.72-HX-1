/*  ==========================================  *\
 *                                              *
 *  PANDORA INSTALLER FOR 3.XX KERNELS          *
 *  smashed together by ---==> HELLCAT <==---   *
 *                                              *
\*  ==========================================  */

//  uses PSAR extraction based on PSPet's code  //
//  uses battery knowledge enginiered by Nem    //
//  uses a bit of common sense by.... me ;-)    //
//////////////////////////////////////////////////

//  this is a "get it done FFS" project,        //
//  don't expect the cleanest code ;-)))        //
//////////////////////////////////////////////////

#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>

#include "kstuff.h"

PSP_MODULE_INFO("PandoraForKernel3xx", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(21000);

#define printf pspDebugScreenPrintf  // as usual *lol*

u8 *dataPSAR, *dataOut, *dataOut2;
char *dataMisc;

u8 sha1_150[20] =
{
	0x1A, 0x4C, 0x91, 0xE5, 0x2F, 0x67, 0x9B, 0x8B, 
	0x8B, 0x29, 0xD1, 0xA2, 0x6A, 0xF8, 0xC5, 0xCA, 
	0xA6, 0x04,	0xD3, 0x30
};

u8 sha1[20];

int InitFWInstall(void)
{
	SceUID fd;
	int size; //, read, i;
	// int r;
	
	printf("Reading and checking ms0:/UPDATE.PBP\n");
	fd = sceIoOpen("ms0:/UPDATE.PBP", PSP_O_RDONLY, 0777);
	if (fd < 0)
  {
	  return 0x8CA10000 + (fd & 0xFFFF);
  }
  size = sceIoLseek(fd, 0, SEEK_END);
  sceIoLseek(fd, 0, PSP_SEEK_SET);
  sceIoRead(fd, dataOut, 200);
  if (memcmp(dataOut, "\0PBP", 4) != 0)
  {
	  sceIoClose(fd);
	  return 0x80CA0002;
  }
  size = size - *(u32 *)&dataOut[0x24];
  sceIoLseek(fd, *(u32 *)&dataOut[0x24], PSP_SEEK_SET);
  if (size != 10149440)
  {
	  sceIoClose(fd);
	  return 0x80CA0003;
  }
  if (sceIoRead(fd, dataPSAR, 10149440) != 10149440)
  {
	  sceIoClose(fd);
	  return 0x80CA0004;
 	}
  sceIoClose(fd);
  
  sceKernelUtilsSha1Digest(dataPSAR, 10149440, sha1);
	if (memcmp(sha1, sha1_150, 20) != 0)
	{
	  return 0x80CA0005;
	}
  
  return 0;
}

void Perform150Patches()
{
	/*pspSdkInstallNoDeviceCheckPatch();
  pspSdkInstallNoPlainModuleCheckPatch();
  pspSdkInstallKernelLoadModulePatch();*/
}

void ClearScreen(void)
{
	pspDebugScreenSetBackColor(0x00402000);
  pspDebugScreenSetTextColor(0x00FFFFFF);
  pspDebugScreenClear();
  printf("*** Pandora Installer for 3.xx Kernels ***     Rev2b\n");
  printf("******************************************\n\n");
  pspDebugScreenSetTextColor(0x00FFFF80);
  printf("Installer by ---==> HELLCAT <==---\n");
  printf("uses code for PSAR extraction from PSPPet (who else ;-))\n");
  printf("uses knownledge for battery patching engeniered by Nem\n\n");
  pspDebugScreenSetTextColor(0x00FFFFFF);
  printf("-----------------------------------------------------------------\n");
  printf("\n");
  pspDebugScreenSetTextColor(0x0080FFFF);
}

void InitMainScreen(void)
{
	pspDebugScreenInit();
	pspDebugScreenSetBackColor(0x00402000);
  pspDebugScreenSetTextColor(0x00FFFFFF);
  pspDebugScreenClear();
  printf("*** Pandora Installer for 3.xx Kernels ***     Rev2b\n");
  printf("******************************************\n\n");
  pspDebugScreenSetTextColor(0x00FFFF80);
  printf("Installer by ---==> HELLCAT <==---\n");
  printf("uses code for PSAR extraction from PSPPet (who else ;-))\n");
  printf("uses knownledge for battery patching engeniered by Nem\n\n");
  pspDebugScreenSetTextColor(0x00FFFFFF);
  printf("-----------------------------------------------------------------\n");
  printf("The maker of this installer takes:\n");
  printf("  a) no credit for the Pandora itself\n");
  printf("  b) no responsibility for anything\n\n");
  printf("Have fun!\n");
  printf("-----------------------------------------------------------------\n\n");
  pspDebugScreenSetTextColor(0x0080FFFF);
  printf("Press X to install a 1.50 firmware, containing Pandora, to the\n");
  printf("        memory stick\n\n");
  printf("Press  O to make battery a Pandora one\n");
  printf("   or [] to make battery a normal one\n\n");
  printf("Press R+X to repartition and format(!) memstick for Pandora\n");
  printf("      L+X to install ms0:/MSIPL.BIN to the memstick\n\n");
  printf("Press /\\ to exit and do nothing\n\n");
}

static int WriteFile(char* file, void* buffer, int size)
{
	scePowerTick(0);

	int i;
	int pathlen = 0;
	char path[128];
	
	// because I'm so lazy, I need folders created "on the fly"
	// this does so, create every folder in the path of the
	// file we are to save.
	// A little bruty forcy, yah, but it does the job :-D
	for(i=1; i<(strlen(file)); i++)
	{
		if(strncmp(file+i-1, "/", 1) == 0)
		{
			pathlen=i-1;
			strncpy(path, file, pathlen);
		  path[pathlen] = 0;
		  sceIoMkdir(path, 0777);
		}
	}
	
	// now up to the file write....
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0)
	{
		return -1;
	}
	
  scePowerTick(0);
	
	int written = sceIoWrite(fd, buffer, size);
	
	scePowerTick(0);
	
	if (sceIoClose(fd) < 0)
		return -1;
  
	return written;
}

void RepartitionMemstick(void)
{
	char* MsMbr;
	char* MsBRec;
	// int PartPosOld;
	// int PartPosNew;
	int PartSizeOld;
	int PartSizeNew;
	int PartPreceedOld;
	int PartPreceedNew;
	int PartClusterSize;
	SceUID f;
	int r;
	int o;
	int PartStartSize;
	int btn;
	SceCtrlData pad;
	
	ClearScreen();
	printf("  ***  Repartitioning and formatting of the memorystick  ***\n\n");
	printf("WARNING!!!\nThis procedure WILL DELETE ALL DATA currently on the stick!\n");
	printf("Make sure you have backed up everything you like to keep!!!\n\n\n");
	printf("To continue with the repartitioning, and so completely erasing\n");
	printf("all contents of the memorystick, press L + R + X....\n....or O to quit.\n");
	
	sceCtrlReadBufferPositive(&pad, 1);
  r=0;
  while(r==0)
  { 
    btn=0;
    while(btn==0)
    {
      sceCtrlReadBufferPositive(&pad, 1);
      btn = pad.Buttons & 0xFFFF;
    }
    if(btn & PSP_CTRL_CIRCLE) { printf("saved at last second ;-)"); sceKernelDelayThread(2000000); sceKernelExitGame(); }
    if(btn == (PSP_CTRL_CROSS | PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER)) { r=1; }
  }
	
	ClearScreen();
	printf("Repartitioning and formatting the memorystick....\n(say good bye to whatever was on it)\n\n");
	
	// allocate some buffers
	MsMbr = (char *)malloc(512);
  if (!MsMbr) { printf("Cannot allocate memory for MBR data.... (0x%08x)\n", (int)MsMbr); return; }
  MsBRec = (char *)malloc(512);
  if (!MsMbr) { printf("Cannot allocate memory for Bootrecord data.... (0x%08x)\n", (int)MsBRec); return; }
  
  // read everything we need to know about current layout
  r = kstuffGetMsPartitionStart();
  if (r<0)
  {
  	printf("ERROR FETCHING PARTITION START BLOCKS!\n");
  	return;
  }
	PartStartSize = r;
	
	f = sceIoOpen( "msstor0:", PSP_O_RDONLY | PSP_O_WRONLY, 0);
	if (f<0)
	{
		printf("ERROR OPENING MSSTOR BLOCKDEVICE! (0x%08x)\n", f);
		return;
	}
	
	r = sceIoRead(f, MsMbr, 512);
	if(r!=512)
	{
		printf("ERROR READING MBR!\n");
		return;
	}
	PartPreceedOld = (MsMbr[454] & 0xFF) + ((MsMbr[455] & 0xFF)*0x100) + ((MsMbr[456] & 0xFF)*0x10000) + ((MsMbr[457] & 0xFF)*0x1000000);
	
	o = PartPreceedOld * 512;
	r = sceIoLseek(f, o, SEEK_SET);
	if(r!=o)
	{
		printf("ERROR SEEKING TO BOOTRECORD!\nShould be 0x%08x => is 0x%08x\n", o, r);
		return;
	}
	r = sceIoRead(f, MsBRec, 512);
	if(r!=512)
	{
		printf("ERROR READING BOOTRECORD!\n");
		return;
	}
	PartSizeOld = (MsBRec[32] & 0xFF) + ((MsBRec[33] & 0xFF)*0x100) + ((MsBRec[34] & 0xFF)*0x10000) + ((MsBRec[35] & 0xFF)*0x1000000);
	// printf("\n\n %x %x %x %x %x\n", (MsBRec[32] & 0xFF) , ((MsBRec[33] & 0xFF)*0x100) , ((MsBRec[34] & 0xFF)*0x10000) , ((MsBRec[35] & 0xFF)*0x1000000), PartSizeOld);
	
	/*  OK, now we have all the information we need to actually  *
	 *  patch the partitioning information on the stick          */
	
	// move start of data partition to ~1MB from the physical start
	PartPreceedNew = 2064;
	// change size of partition accordingly + some spare space, just in case....
	PartSizeNew = PartSizeOld + PartPreceedOld - PartPreceedNew - 64;
	
	// try to get a decent cluster size
	r = (PartSizeOld * 512) / 1024 / 1024;
	PartClusterSize = 40;
	if( r > 55 ) { PartClusterSize = 0x10; }
	if( r > 110 ) { PartClusterSize = 0x20; }
	if( r > 900 ) { PartClusterSize = 0x40; }
	if( r > 3500 ) { PartClusterSize = 0x80; }
	
	// now patch the new information into MBR and bootrecord
	MsMbr[454] = PartPreceedNew & 0xFF;
	MsMbr[455] = (PartPreceedNew & 0xFF00) / 0x100;
	MsMbr[456] = (PartPreceedNew & 0xFF0000) / 0x10000;
	MsMbr[457] = (PartPreceedNew & 0xFF000000) / 0x1000000;
	
	MsMbr[458] = PartSizeNew & 0xFF;
	MsMbr[459] = (PartSizeNew & 0xFF00) / 0x100;
	MsMbr[460] = (PartSizeNew & 0xFF0000) / 0x10000;
	MsMbr[461] = (PartSizeNew & 0xFF000000) / 0x1000000;
	
	// dummying out CHS stuff (=> use LBA only, I'm lazy ;-))
	MsMbr[447] = 255;
	MsMbr[448] = 255;
	MsMbr[449] = 255;
	MsMbr[450] = 14;
	MsMbr[451] = 255;
	MsMbr[452] = 255;
	MsMbr[453] = 255;
	
	dataOut[13] = PartClusterSize;
	dataOut[32] = PartSizeNew & 0xFF;
	dataOut[33] = (PartSizeNew & 0xFF00) / 0x100;
	dataOut[34] = (PartSizeNew & 0xFF0000) / 0x10000;
	dataOut[35] = (PartSizeNew & 0xFF000000) / 0x1000000;
	
	// time to write everything back to the stick....
	r = sceIoLseek(f, 0, SEEK_SET);
	r = sceIoWrite(f, MsMbr, 512);
	if(r!=512)
	{
		printf("Warning: possible failure at writing new MBR\n");
	}
	
	o = PartPreceedNew * 512;
	r = sceIoLseek(f, o, SEEK_SET);
	if(r!=o)
	{
		printf("Warning: possible failure at seeking to new partition start.\n\nShould be 0x%08x => is 0x%08x\n", o, r);
	}
	r = sceIoWrite(f, dataOut, PartStartSize);
	if(r!=PartStartSize)
	{
		printf("Warning: possible failure at writing new partition start blocks\n");
	}
	//printf("\n\n %x %x %x %x\n", PartSizeOld, PartSizeNew, PartPreceedOld, PartPreceedNew);
	sceKernelDelayThread(1500000);
	
	printf("\n\nDONE! :-)\n\nReboot PSP for recognizing the new memstick layout!\n\n");
	printf("After rebooting, recopy UPDATE.PBP and this installer\n");
	printf("back on the stick to install the Pandora files to it.\n\n\n");
	printf("Press X to turn off the PSP now....");
	
	sceIoClose(f);
	
	r=0;
	while(r==0)
	{
    sceCtrlReadBufferPositive(&pad, 1);
    while((pad.Buttons & 0xFFFF) != 0){ sceCtrlReadBufferPositive(&pad, 1); }
      
    btn=0;
    while(btn==0)
    {
      sceCtrlReadBufferPositive(&pad, 1);
      btn = pad.Buttons & 0xFFFF;
    }
    if(btn & PSP_CTRL_CROSS) { r=1; }
  }
  scePowerRequestStandby();
  while(1==1) { sceKernelDelayThread(500000); }
}

void InstallMsIpl(void)
{
	SceUID f1;
	SceUID f2;
	int r;
	int l;
	
  printf("\nInstalling ms0:/MSIPL.BIN to reserved area on memstick....\n");
  
  f1 = sceIoOpen( "msstor0:", PSP_O_RDONLY | PSP_O_WRONLY, 0);
	if (f1<0)
	{
		printf("ERROR OPENING MSSTOR BLOCKDEVICE! (0x%08x)\n", f1);
		return;
	}
	
	f2 = sceIoOpen( "ms0:/msipl.bin", PSP_O_RDONLY, 0777);
	if (f2<0)
	{
		printf("ERROR OPENING MSIPL.BIN! (0x%08x)\n", f2);
		return;
	}
	
	l = sceIoRead(f2, dataOut, 1000000); // + 512;
	printf("read %i bytes, ", l);
	r = sceIoLseek(f1, 0x2000, SEEK_SET);
	printf("seek = 0x%x, ", r);
	l = ( l + 512 ) & 0xFFFFFE00;   // bring "l" to a round multiple of 512
	r = sceIoWrite(f1, dataOut, l);
	printf("written %i bytes.... ", r);
	
	sceIoClose(f1);
	sceIoClose(f2);
	
	sceKernelDelayThread(1500000);
	printf("\nDONE! :-)\nExiting....");
	sceKernelDelayThread(3000000);
	sceKernelExitGame();
}

int main(int argc, char *argv[])
{
	int btn;
	SceCtrlData pad;
	int quit = 0;
	int r;
	
	InitMainScreen();
	
	//Perform150Patches();
	
	SceUID mod;
	mod = pspSdkLoadStartModule("kstuff.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0) { printf(" Error 0x%08X loading/starting kstuff.prx.\n", mod); }
  
  dataPSAR = (u8 *)memalign(0x40, 16703136+256);
  if (!dataPSAR) { printf("Cannot allocate memory for PSAR data.... (0x%08x)\n", (int)dataPSAR); }

  dataOut = (u8 *)memalign(0x40,  2000000);
  if (!dataOut) { printf("Cannot allocate memory for buffer 1.... (0x%08x)\n", (int)dataOut); }

  dataOut2 = (u8 *)memalign(0x40, 2000000);
  if (!dataOut2) { printf("Cannot allocate memory for buffer 2.... (0x%08x)\n", (int)dataOut2); }
  	
  dataMisc = (char *)memalign(0x40, 1024);
  if (!dataOut2) { printf("Cannot allocate memory for buffer 3.... (0x%08x)\n", (int)dataOut2); }
	
	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
	
	while(quit==0)
	{
    sceCtrlReadBufferPositive(&pad, 1);
    //while((pad.Buttons & 0xFFFF) != 0){ sceCtrlReadBufferPositive(&pad, 1); }
      
    btn=0;
    while(btn==0)
    {
      sceCtrlReadBufferPositive(&pad, 1);
      btn = pad.Buttons & 0xFFFF;
    }
    if(btn == (PSP_CTRL_RTRIGGER | PSP_CTRL_CROSS))
    {
  	  kstuffInitBuffers(dataPSAR, dataOut, dataOut2, dataMisc);
  	  RepartitionMemstick();
    }
    if(btn == (PSP_CTRL_LTRIGGER | PSP_CTRL_CROSS))
    {
      InstallMsIpl();
    }
    if(btn & PSP_CTRL_TRIANGLE) { quit=1; }
    if(btn & PSP_CTRL_CIRCLE) { r=kstuffBatMakeService(); printf("Battery pandorized....\n"); }
    if(btn & PSP_CTRL_SQUARE) { r=kstuffBatMakeNormal(); printf("Battery normalized....\n"); }
    if(btn & PSP_CTRL_CROSS) { // setup "callback" for the kernel module to channel file writes through userland
    	                         //kstuffSetUserFileWriteFunc((void*)&WriteFile);
    	                         ClearScreen();
    	                         r=InitFWInstall();
    	                         if(r!=0) { printf("\nERROR WHILE INITIALIZING: 0x%08x\n", r); }
    	                         kstuffInitBuffers(dataPSAR, dataOut, dataOut2, dataMisc);
    	                         printf("Installing firmware to memorystick.\nHang on, this will take a wile, time to get a coffee.... ;-)");
    	                         r=1;
    	                         while(r!=0)
    	                         {
    	                           r=kstuffMsInstallFW();
    	                           if(r>0){ WriteFile(dataMisc, dataOut2, r); }
    	                         }
    	                         printf("\nInstalling additional files....");
    	                         r=1;
    	                         while(r!=0)
    	                         {
    	                         	 r=kstuffMsInstallAdditional();
    	                         	 if(r>0){ WriteFile(dataMisc, dataOut2, r); }
    	                         }
    	                         printf("\n\nOK, if all went well we now have Pandora on the stick.\nDon't forget to run msipl installer if needed!\n");
    	                         printf("\nPress /\\ to quit....\n\n");
    	                         printf("....or L+X to install MSIPL.BIN to the memorystick.\n");
    	                         quit=0; }
  }

  printf("\n\nFINISHED! Autoexit in 5 seconds....");
  sceKernelDelayThread(5000000);
	sceKernelExitGame();
	
	return 0;
}
