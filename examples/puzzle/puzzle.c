#include <psx.h>
#include <stdio.h>
#include <stdlib.h>


unsigned char puzzle_pieces[7][7];
unsigned char piece_taken[7 * 7];

int puzzle_x = 0, puzzle_y = 0;

volatile int display_is_old = 1;

void prog_vblank_handler()
{
	display_is_old=1;
}

GsImage puzzle_image;
unsigned int prim_list[0x4000];
unsigned char data_buffer[0x40000]; // 256 kilobytes

int main()
{
	int x, y, x2, y2, z, ex, ey;
	int wasleft=0;
	int wasright=0;
	int wasup=0;
	int wasdown=0;
	int wasexh=0;
	int wasexv=0;
	int dbuf=0;
	unsigned char b;
	unsigned short padbuf;
	FILE *f;
	GsSprite tile_sprite;
	GsRectangle tile_rectangle;

	PSX_Init();
	GsInit();
	GsClearMem();
	GsSetList(prim_list);
	GsSetVideoMode(320, 240, VMODE_PAL);

	f = fopen("cdrom:\\PUZZLE.TIM;1", "rb");
	fseek(f, 0, SEEK_END);
	x = ftell(f);
	fseek(f, 0, SEEK_SET);

	fread(data_buffer, sizeof(char), x, f);
	fclose(f);

	GsImageFromTim(&puzzle_image, data_buffer);

	GsUploadImage(&puzzle_image);
	while(GsIsDrawing());

	GsLoadFont(768, 0, 768, 256);
	while(GsIsDrawing());

	tile_sprite.tpage = 5;
	tile_sprite.w = 32;
	tile_sprite.h = 32;
	tile_sprite.attribute = COLORMODE(COLORMODE_16BPP);
	tile_sprite.r = tile_sprite.g = tile_sprite.b = NORMAL_LUMINOSITY;

	SetVBlankHandler(prog_vblank_handler);

//puzzle_pieces[y][x] = (y*7)+x;

	for(x=0;x<sizeof(piece_taken);x++)
		piece_taken[x] = 0;

	for(y = 0; y < 7; y++)
	{
		for(x = 0; x < 7; x++)
		{
			do
			{
				puzzle_pieces[y][x] = (rand() % 49) + 1;
			}while(piece_taken[puzzle_pieces[y][x]] == 1);

			piece_taken[puzzle_pieces[y][x]] = 1;
		}
	}
			
	while(1)
	{
		if(display_is_old)
		{
			GsSetDispEnvSimple(0, dbuf?256:0);
			GsSetDrawEnvSimple(0, dbuf?0:256, 320, 240);
			dbuf=!dbuf;

			PSX_ReadPad(&padbuf, NULL);

			if((padbuf & PAD_LEFT) && !wasleft)
			{
				if(puzzle_x>0)
					puzzle_x--;
	
				wasleft=1;	
			}

			if((padbuf & PAD_RIGHT) && !wasright)
			{
				if(puzzle_x<6)
					puzzle_x++;
		
				wasright=1;
			}

			if((padbuf & PAD_UP) && !wasup)
			{
				if(puzzle_y>0)
					puzzle_y--;

				wasup=1;
			}

			if((padbuf & PAD_DOWN) && !wasdown)
			{
				if(puzzle_y<6)
					puzzle_y++;
	
				wasdown=1;
			}

			if(!(padbuf & PAD_LEFT))
				wasleft=0;
	
			if(!(padbuf & PAD_RIGHT))
				wasright=0;

			if(!(padbuf & PAD_UP))
				wasup=0;
		
			if(!(padbuf & PAD_DOWN))
				wasdown=0;

			if((padbuf & PAD_CROSS) && !wasexh) // x <--> y
			{
				ey = puzzle_y;
				if(puzzle_x < 6)
					ex = puzzle_x+1;
				else 
					ex = puzzle_x-1;

				b = puzzle_pieces[puzzle_y][puzzle_x];
				puzzle_pieces[puzzle_y][puzzle_x] =
					puzzle_pieces[ey][ex];
				puzzle_pieces[ey][ex] = b;

				wasexh=1;
			}

			if((padbuf & PAD_CIRCLE) && !wasexv)
			{
				ex = puzzle_x;
				if(puzzle_y < 6)
					ey = puzzle_y+1;
				else 
					ey = puzzle_y-1;

				b = puzzle_pieces[puzzle_y][puzzle_x];
				puzzle_pieces[puzzle_y][puzzle_x] =
					puzzle_pieces[ey][ex];
				puzzle_pieces[ey][ex] = b;

				wasexv=1;
			}

			if(!(padbuf & PAD_CROSS))
				wasexh=0;

			if(!(padbuf & PAD_CIRCLE))
				wasexv=0;
		
			GsSortCls(0, 0, 255);

			for(y = 0; y < 7; y++)
			{
				for(x = 0; x < 7; x++)
				{
					tile_sprite.x = x * 32;
					tile_sprite.y = y * 32;
					tile_sprite.u = ((puzzle_pieces[y][x]-1) % 7) * 32;
					tile_sprite.v = ((puzzle_pieces[y][x]-1) / 7) * 32;
					GsSortSimpleSprite(&tile_sprite);

					if(x == puzzle_x && y == puzzle_y)
					{
						tile_rectangle.x = x * 32;
						tile_rectangle.y = y * 32;
						tile_rectangle.r = 255;
						tile_rectangle.g = 0;
						tile_rectangle.b = 255;
						tile_rectangle.w = 32;
						tile_rectangle.h = 32;
						tile_rectangle.attribute = ENABLE_TRANS;
						GsSortRectangle(&tile_rectangle);
					}
				}
			}

		/*	z = 1;
			
			for(y=0;y<7;y++)
				for(x=0;x<7;x++)
					if(puzzle_pieces[y][x] != ((y*7)+x+1))
						z=0;
		*/

/*			if(z==1)
				GsPrintFont(232, 16, "Puzzle\nResolved!");
			else*/
				GsPrintFont(232, 16, "Resolve\nPuzzle\n\n"
								 "Exchange:\n"
                                                                 "\nX\nHorizontal\n"
								 "\nO\nVertical");

			GsDrawList();	
			while(GsIsDrawing());

			display_is_old = 0;
		}
	}
}
