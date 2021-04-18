/*

Pandora Creator for 3.72 HX -  par Roomain
http://www.romain-dev.fr.nf

LICENCE GNU/GPL :
-----------------

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

-----------------

Vive le libre !

*/


#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <string.h>
#include <pspgu.h>
#include <pspuser.h>



#include "pspcrypt.h"
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h> 
#include <psploadexec.h>

#include "psploadexec_kernel.h"

#include "kstuff.h"



PSP_MODULE_INFO("pandoriseur", 0, 0, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);


// Définition de printf
#define printf pspDebugScreenPrintf

int kstuffBatMakeService(void);
int kstuffBatMakeNormal(void);

int loadStartModule(const char *path)
{

	printf("\n\n\n");
 //  printf("Chargement du PRX... ", path);
   int mid = sceKernelLoadModule(path, 1, 0);
   if(mid == 0x80020139)
   {
      // module is already loaded
     // printf("already loaded.");
      return 0;
   }
   
   if(mid < 0)
   {
      // unable to load module
     // printf("unable to load.");
      return mid;
   }
   else
   {
   //   printf("Ok !");
   }
   
  printf("\n Modification... ");
   int result = sceKernelStartModule(mid, 1, 0, 0, 0);
   if(result < 0)
   {
      // Unable to start
     printf("Echec !");
      return result;
   }
   else
   {
      printf("Ok !");
   }
   
   return mid;
}


void pandor()
{

	
	pspDebugScreenInit();// On initialise l'écran
	
	pspDebugScreenSetTextColor(0xFF0000); // La couleur du texte: il faut un logiciel nommé Fast color codes pour obtenir ce code couleur c++
	pspDebugScreenClear(); // On efface l'écran
	printf("\n X : Pandoriser la batterie \n");
	printf(" O : Normaliser la batterie \n");
	//	flipScreen();
	/*// write battery EEPROM
	u32 sceSyscon_driver_1165C864(u8 addr, u16 data); 
	// read battery EEPROM
	u32 sceSyscon_driver_68EF0BEF(u8 addr); 
	// same for 3.71
	u32 sceSyscon_driver_40CBBD46(u8 addr, u16 data);
	u32 sceSyscon_driver_B9C1B0AC(u8 addr);*/
	int r=0;
	
	while(1)
	{
		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1);
		
		// X
		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			

//vshKernelLoadExecVSHMs1("ms0:/PSP/GAME/M33CREATOR/EBOOT.PBP", NULL);
	
	
	kstuffBatMakeService();
	loadStartModule("kstuff.prx");
		pspDebugScreenSetTextColor(0x00FF00);
	printf("\n Batterie pandorisee... Extinction dans 2 secondes !\n\n Application codee par Roomain de Gx-Mod");
	sceKernelDelayThread(2*1000*1000);
	sceKernelExitGame();
	

		}
		if (pad.Buttons & PSP_CTRL_CIRCLE)
		{
		
			loadStartModule("kstuff.prx");
			kstuffBatMakeNormal();
			pspDebugScreenSetTextColor(0x00FF00);
	printf("\n Batterie normalisee... Extinction dans 2 secondes !\n\n Application codee par Roomain de Gx-Mod");
	sceKernelDelayThread(2*1000*1000);
	sceKernelExitGame();

		}
		

	}
	}




	// Le main qui se lance au démarrage du programme.
	int main(void)
	{


		pandor();
	}



