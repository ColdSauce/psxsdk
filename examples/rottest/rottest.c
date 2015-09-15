#include <psx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int prim_list[0x4000];
unsigned char filebuf[0x40000];

volatile int display_is_old = 0;

void prog_vblank_handler()
{
	display_is_old = 1;
}

int main()
{
	FILE *f;
	GsSprite my_sprite;
	GsImage my_image;
	int y = 0;
	int ro = 0;
	int sc_x = SCALE_ONE;
	int sc_y = SCALE_ONE;
	unsigned short padbuf;

	PSX_Init();
		
	GsInit();
	GsSetList(prim_list);
	GsSetVideoMode(320, 240, VMODE_PAL);

	GsClearMem();
	GsSetDrawEnvSimple(0, 256, 320, 240);
	GsSetDispEnvSimple(0, 0);

	f = fopen("cdrom:\\IMAGE.TIM;1", "rb");
	fseek(f, 0, SEEK_END);
	y = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(filebuf, sizeof(char), y, f);
	fclose(f);

	GsImageFromTim(&my_image, filebuf);
	GsSpriteFromImage(&my_sprite, &my_image, 1);

	SetVBlankHandler(prog_vblank_handler);

	GsLoadFont(768, 0, 768, 128);
	while(GsIsWorking());

	while(1)
	{
		if(display_is_old)
		{
			PSX_ReadPad(&padbuf, NULL);
			if(padbuf & PAD_LEFT)
			{
				ro--;
				if(ro < 0) ro = 359;
			}

			if(padbuf & PAD_RIGHT)
			{
				ro++;
				if(ro>=360) ro = 0;
			}

			if(padbuf & PAD_UP)
				sc_x+=128;
			
			if(padbuf & PAD_DOWN)
				sc_x-=128;

			if(padbuf & PAD_CROSS)
				sc_y-=128;
			
			if(padbuf & PAD_CIRCLE)
				sc_y+=128;

			GsSortCls(0, 0, 0);

			my_sprite.x = 100;
			my_sprite.y = 100;
			my_sprite.r = my_sprite.g = my_sprite.b = NORMAL_LUMINOSITY;
			my_sprite.rotate = ROTATE_ONE*ro;
			my_sprite.scalex = sc_x;
			my_sprite.scaley = sc_y;
			my_sprite.mx = (my_sprite.w/2) * (my_sprite.scalex / 4096);
			my_sprite.my = (my_sprite.h/2) * (my_sprite.scaley / 4096);
			GsSortSprite(&my_sprite);

			GsPrintFont(0, 0, "ro=%d\nsc_x=%d\nsc_y=%d", ro, sc_x, sc_y);

			GsDrawList();
			while(GsIsWorking());
	
			y=!y;
			GsSetDrawEnvSimple(0, y?0:256, 320, 240);
			GsSetDispEnvSimple(0, y?256:0);
			display_is_old = 0;
		}
	}
}
