/**
 * PlayStation Memory Viewer
 * by Giuseppe Gatta a.k.a. nextvolume
 *
 * PSXSDK Example - released to the Public Domain
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <psx.h>

unsigned int cur_addr = 0x80000000;
unsigned int prim_list[0x20000];

int display_is_old = 1;
volatile int should_read_pad = 1;

void pshex_vblank_handler()
{
	should_read_pad = 1;
}

int main()
{
	unsigned short padbuf;
	int y, f, x, x1;
	int highres_mode=0;
	int srp=0;
	int wasup=0, wasdown=0, wasleft=0, wasright=0, wasl1=0, wasr1=0,
		wasl2=0;
	int wascross=0, wascircle=0, wasstart=0,wasselect=0;
	unsigned int go_address = 0x80000000;
	int go_pos = 0;
	GsRectangle pshex_rect;
	

	PSX_InitEx(0);
	
	GsInit();
	GsClearMem();

	GsSetVideoMode(640, 240, VMODE_PAL);
	GsSetList(prim_list);
	GsSetDispEnvSimple(0, 0);
	GsSetDrawEnvSimple(0, 0, 640, 240);
	GsLoadFont(768, 0, 768, 256);

	SetVBlankHandler(pshex_vblank_handler);

	GsPrintFont(16, 16, "PAL/NTSC SELECTION");
	GsPrintFont(16, 32, "X - PAL");
	GsPrintFont(16, 40, "O - NTSC");
	GsDrawList();
	while(GsIsDrawing());

	while(1)
	{
		if(should_read_pad)
		{
			PSX_ReadPad(&padbuf, NULL);

			if((padbuf & PAD_CROSS))
			{
				GsSetVideoModeEx(640, 480, VMODE_PAL, 0, 1, 0);
				wascross=1;
				break;
			}

			if((padbuf & PAD_CIRCLE))
			{
				GsSetVideoModeEx(640, 480, VMODE_NTSC, 0, 1, 0);
				wascircle=1;
				break;
			}

			should_read_pad=0;
		}
	}

	GsSetDrawEnvSimple(0, (GsScreenM==VMODE_PAL?8:0), 640, 480);

	while(1)
	{
		if(display_is_old)
		{
			GsSortCls(0, 0, 255);

			for(y = 0; y < (highres_mode?58:28); y++)
			{
				f = GsPrintFont(24, y*8, "%08x", cur_addr + (y*0x10));
				x1 = prfont_get_fx(f) + 16;

				for(x = 0; x < 16; x++)
				{
					if((x&3 == 0) && x>0)
						x1+=16;

					f = GsPrintFont(x1, y*8, "%02x", *((unsigned char*)(cur_addr + (y*0x10) + x)));
					x1 = prfont_get_fx(f) + 8;
				}
	
				x1+=8;

				for(x=0;x<16;x++)
				{
					GsPrintFont(x1, y*8, "%c", *((unsigned char*)(cur_addr + (y*0x10) + x)));
					x1+=8;
				}
			}

			pshex_rect.x = (go_pos * 8)+24;
			pshex_rect.y = highres_mode?472:232;
			pshex_rect.r = 255;
			pshex_rect.g = 69;
			pshex_rect.b = 0;
			pshex_rect.attribute = 0;
			pshex_rect.w = 8;
			pshex_rect.h = 8;
			GsSortRectangle(&pshex_rect);
	
			GsPrintFont(24, highres_mode?472:232, "%08X", go_address);

			GsDrawList();
			while(GsIsDrawing());
			display_is_old = 0;
		}

		if(should_read_pad)
		{
			PSX_ReadPad(&padbuf, NULL);

			if((padbuf & PAD_UP) && !wasup)
			{
				cur_addr -= 0x10;
				display_is_old = 1;
				wasup = 1;
			}
			
			if((padbuf & PAD_DOWN) && !wasdown)
			{
				cur_addr += 0x10;
				display_is_old = 1;
				wasdown = 1;
			}

			if((padbuf & PAD_LEFT) && !wasleft)
			{
				cur_addr -= 0x100;
				display_is_old = 1;
				wasleft=1;
			}

			if((padbuf & PAD_RIGHT) && !wasright)
			{
				cur_addr += 0x100;
				display_is_old = 1;
				wasright=1;
			}

			if((padbuf & PAD_L1) && !wasl1)
			{
				if(go_pos>0)go_pos--;
				display_is_old = 1;
				wasl1=1;
			}

			if((padbuf & PAD_R1) && !wasr1)
			{
				if(go_pos<7)go_pos++;
				display_is_old = 1;
				wasr1=1;
			}
			
			if((padbuf & PAD_L2) && !wasl2)
			{
				go_pos = 0;
				wasl2 = 1;
			}

			if((padbuf & PAD_CROSS) && !wascross)
			{
				if(((go_address >> ((7-go_pos)<<2))&0xf) < 0xf)
					go_address += 1 << ((7-go_pos)<<2);

				display_is_old = 1;
				wascross=1;
			}

			if((padbuf & PAD_CIRCLE) && !wascircle)
			{
				if(((go_address >> ((7-go_pos)<<2))&0xf) > 0)
					go_address -= 1 << ((7-go_pos)<<2);

				display_is_old = 1;
				wascircle = 1;
			}

			if((padbuf & PAD_START) && !wasstart)
			{
				cur_addr = go_address;
				display_is_old = 1;
				wasstart=1;
			}

			if((padbuf & PAD_SELECT) && !wasselect)
			{
				highres_mode = !highres_mode;

				GsSortCls(0,0,255);
				GsDrawList();
				while(GsIsDrawing());

				if(highres_mode)
				{
					GsSetVideoModeEx(640, 480, GsScreenM, 0, 1, 0);
					GsSetDrawEnvSimple(0, (GsScreenM==VMODE_PAL)?8:0, 640, 480);
				}
				else
				{
					GsSetVideoMode(640, 240, GsScreenM);
					GsSetDrawEnvSimple(0, (GsScreenM==VMODE_PAL)?8:0, 640, 240);
				}

				display_is_old = 1;
				wasselect = 1;
			}

			if(!(padbuf & PAD_UP) || srp==5)wasup=0;
			if(!(padbuf & PAD_DOWN) || srp==5)wasdown=0;
			if(!(padbuf & PAD_LEFT) || srp==5)wasleft=0;
			if(!(padbuf & PAD_RIGHT) || srp==5)wasright=0;
			if(!(padbuf & PAD_L1))wasl1=0;
			if(!(padbuf & PAD_R1))wasr1=0;
			if(!(padbuf & PAD_L2))wasl2=0;
			if(!(padbuf & PAD_CROSS) || srp==5)wascross=0;
			if(!(padbuf & PAD_CIRCLE) || srp==5)wascircle=0;
			if(!(padbuf & PAD_START)) wasstart=0;
			if(!(padbuf & PAD_SELECT)) wasselect=0;

			should_read_pad = 0;
			srp++;
			if(srp > 5) srp=0;
		}
	}
}

//AAAAAAAA 00 11 22 33    44 55 66 77    88 99 AA BB    CC DD EE FF        abcd
