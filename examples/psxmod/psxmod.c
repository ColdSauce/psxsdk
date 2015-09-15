#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <psx.h>
#include <modplay.h>

unsigned char fileBuffer[0x80000]; // 512 kilobytes

unsigned int primList[0x8000];
int dbuf = 0;
volatile int display_is_old = 1;
char current_dir[256] = "cdrom:";
char name_buf[24];
char name_buf2[32];
ModMusic *music = NULL;
int music_loop;
char *music_player_fname;
int music_player_time;
short music_player_vol;
int music_player_vol_pc;

GsRectangle prog_rect;

void program_vblank_handler()
{
	display_is_old = 1;
}

struct
{
	char name[20];
	int size;
	int type; /* 0 = file, 1 = directory */
}file_list[256];

int file_list_size = 0;
int file_list_pos = 0;

int wasUp = 0;
int wasDown = 0;
int wasEnter = 0;
int wasLeft = 0;
int wasRight = 0;

void update_file_list()
{
	struct DIRENTRY de;
	struct DIRENTRY *r;
	char *cp;
	int i, k=0;

	if(strcmp(current_dir, "cdrom:") != 0)
	{
		strcpy(file_list[0].name, "..");
		file_list[0].type = 1;
		k=1;
	}
	
	r = firstfile("cdrom:*", &de);
	
	for(i = k; i < 256 && r; i++)
	{
		if(de.name[0] == 0x1)
		{
// Throw away useless entry.			
			r = nextfile(&de);
			i--;
			continue;
		}
		
		memcpy(file_list[i].name, de.name, 20);
		
		if((cp = strrchr(file_list[i].name, ';')))
		{
			*cp = '\0';
			file_list[i].type = 0;
		}
		else
			file_list[i].type = 1;
		
		file_list[i].size = de.size;
		
		r = nextfile(&de);
	}
	
	file_list[i].size = -1;
	file_list_size = i;
}

void file_browser(int redraw)
{
	int i, k, ok, allowed_ext;
	unsigned short padbuf;
	char *cp;
	FILE *f;
	
	PSX_ReadPad(&padbuf, NULL);
			
	if(wasUp)
		wasUp++;
			
	if(wasDown)
		wasDown++;
			
	if((padbuf & PAD_UP) && !wasUp)
	{
		if(file_list_pos > 0)
		{
			file_list_pos--;
			redraw=1;
		}
				
		wasUp=1;
	}
			
	if((padbuf & PAD_DOWN) && !wasDown)
	{
		file_list_pos++;
				
		if(file_list[file_list_pos].size == -1)
			file_list_pos--;
		else
			redraw = 1;
				
		wasDown = 1;
	}
			
	if((padbuf & PAD_CROSS) && !wasEnter)
	{
		if(file_list[file_list_pos].type == 1)
			allowed_ext = 1;
		else
			allowed_ext = 0;
		
		if((cp = strchr(file_list[file_list_pos].name, '.')))
		{
			if(strcmp(cp+1, "MOD") == 0)
				allowed_ext = 1;
		}
		
		if(allowed_ext)
		{
			if(file_list[file_list_pos].type == 0)
			{ 
				if(music != NULL)
				{
					MODUnload(music);
					music = NULL;
				}
					
// It is a file, we will play it
			
// First check if there is a file which contains the
// samples
				strcpy(name_buf, file_list[file_list_pos].name);
				
				if((cp = strchr(name_buf, '.')))
					*(cp+1) = '\0';
				
				strcat(name_buf, "SMP");
			
				ok = 0;
			
				if(!(padbuf & PAD_SELECT))
				{
					for(i = 0; file_list[i].size != -1; i++)
					{
						if(strcmp(name_buf, file_list[i].name) == 0)
						{
							ok = 1;
							break;
						}
					}
				}

				if(ok) 
				{
// We found a sample file			
					printf("Loading sample file for %s...\n", 
						file_list[file_list_pos].name);
				
					sprintf(name_buf2, "cdrom:%s;1", name_buf);
					printf("namebuf2=%s\n",name_buf2);
					
					f = fopen(name_buf2, "rb");
										
				//	printf("SZ = %d\n", file_list[i].size);
					
					fread(fileBuffer, sizeof(char), file_list[i].size, f);
					fclose(f);
					
					memcpy(name_buf, fileBuffer, 8);
					name_buf[8] = 0;
				//	printf("NJUE = %s\n", name_buf);
				//	printf("NUIEI = %d\n", 
					MOD4PSX_Upload(fileBuffer, SPU_DATA_BASE_ADDR);
				}
				else
					printf("No sample file found for %s!\n",
						file_list[file_list_pos].name);
			
				sprintf(name_buf2, "cdrom:%s;1", file_list[file_list_pos].name);
				
				f = fopen(name_buf2, "rb");
				
				fread(fileBuffer, sizeof(char), file_list[file_list_pos].size, f);
				
				fclose(f);
			
				music = MODLoadEx(fileBuffer, ok?MODLOAD_NOSAMPLES:0);

				if(!ok)
					MODUploadSamples(music, SPU_DATA_BASE_ADDR);
				
				music_player_fname = file_list[file_list_pos].name;
				music_player_time = 0;
				music_player_vol = SPU_MAXVOL+1;
				music_player_vol_pc = 100;
				MODSetMaxVolume(music_player_vol-1);
				music_loop = -1;
			}
			else
			{
// It is a directory, we will go inside it
				if(strcmp(file_list[file_list_pos].name, "..") == 0)
				{
					*(strrchr(current_dir, '\\')) = '\0';
						
					if(strcmp(current_dir, "cdrom:") == 0)
						chdir("cdrom:\\");
					else
						chdir(current_dir);
				}
				else
				{
					strcat(current_dir, "\\");
					strcat(current_dir, file_list[file_list_pos].name);
					chdir(current_dir);
				}
					
				file_list_pos = 0;
				update_file_list();
				redraw = 1;
			}
		}
		
		wasEnter = 1;
	}
			
	if(!(padbuf & PAD_UP) || wasUp >= 15)
		wasUp = 0;
	if(!(padbuf & PAD_DOWN) || wasDown >= 15)
		wasDown = 0;
	if(!(padbuf & PAD_CROSS))
		wasEnter = 0;
				
	/* Drawing */
	if(!redraw)
		return;
	

	
	GsSortCls(0, 0, 0);
	GsPrintFont(0, 0, "-= PsxMod =-");
	GsPrintFont(0, 8, "Folder: <root>%s", current_dir + 6);
	GsPrintFont(0, 16, "X to play, hold SELECT to convert SFX");
	
	k=0;
	
	if(file_list_size > 24)
	{
		//if((((file_list_size-1)-k)*8)+32) <= 216)
		
		//printf("IS = %d\n", ((((file_list_size-1)-k)*8)+32));
	
		//if(((((file_list_size-1)-k)*8)+32) <= 216)		
		//	k = file_list_size - 25;
		if(file_list_pos > 12)
		{
			k = file_list_pos - 12;
			
			if(k > (file_list_size - 26))
				k = file_list_size - 26;
		}
	}
	
	prog_rect.x = 0;
	prog_rect.y = 32 + ((file_list_pos-k) * 8);
	prog_rect.w = GsScreenW / 2;
	prog_rect.h = 8;
	prog_rect.r = 0;
	prog_rect.g = 0;
	prog_rect.b = 255;
	prog_rect.attribute = 0;
	GsSortRectangle(&prog_rect);
	
	for(i = 0; file_list[i+k].size != -1; i++)
	{
		if(file_list[i+k].type == 1)
			GsPrintFont(0, 32 + (i * 8), " +- %s", file_list[i+k].name);
		else
			GsPrintFont(0, 32 + (i * 8), "    %s", file_list[i+k].name);
	}
	
	GsDrawList();
	
	dbuf = !dbuf;
	GsSetDispEnvSimple(0, dbuf?256:0);
	GsSetDrawEnvSimple(0, dbuf?0:256, 320, 240);
}

void music_player_draw()
{
	unsigned short padbuf;
	static int old_song_pos = 10000;
	int change_vol = 0;
	
	GsSortCls(0, 0, 0);
	GsPrintFont(0, 0, "-= PsxMod =-");
	GsPrintFont(0, 8, "File: %s", music_player_fname);
	GsPrintFont(0, 16, "Title: %s", music->title);
	GsPrintFont(0, 24, "pat:%03d/%03d pos:%02X spd: %d/%d", 
		music->song_pos, music->song_pos_num - 1, music->pat_pos,
		music->ticks_division, music->beats_minute);
	GsPrintFont(0, 32, "vol: %03d%%/100%% time: %d:%02d chn: %d",
		music_player_vol_pc, music_player_time / 3000, (music_player_time % 3000) / 50,
		music->channel_num);

	GsPrintFont(0, 56, "Press X to change music.");
	
	dbuf = !dbuf;
	GsSetDispEnvSimple(0, dbuf?256:0);
	GsSetDrawEnvSimple(0, dbuf?0:256, 320, 240);
	GsDrawList();
	
	if(old_song_pos > music->song_pos)
		music_player_time = 0;
	
	PSX_ReadPad(&padbuf, NULL);

	if(padbuf & PAD_UP)
	{
		music_player_vol+=32;
		change_vol = 1;
	}
	
	if(padbuf & PAD_DOWN)
	{
		music_player_vol-=32;
		change_vol = 1;
	}
	
	if(padbuf & PAD_LEFT)
	{
		if(!wasLeft)
		{
			
			if(music->song_pos > 0)
			{
				music->pat_pos = 0;
				music->song_pos--;
			}
		}
			
		wasLeft++;
	}
	
	if(padbuf & PAD_RIGHT)
	{
		if(!wasRight)
		{
			if(music->song_pos < (music->song_pos_num - 1))
			{
				music->pat_pos = 0;
				music->song_pos++;
			}
		}
		
		wasRight++;
	}
	
	if(!(padbuf & PAD_LEFT) || wasLeft >= 15)
		wasLeft = 0;
	
	if(!(padbuf & PAD_RIGHT) || wasRight >= 15)
		wasRight = 0;
	
	if((padbuf & PAD_CROSS) && !wasEnter)
	{
		MODStop(music);
		MODUnload(music);
		music = NULL;
		wasEnter = 1;
	}
	
	if(!(padbuf & PAD_CROSS))
		wasEnter = 0;
	
	if(music_player_vol < 0)
		music_player_vol = 0;
	
	if(music_player_vol > (SPU_MAXVOL+1))
		music_player_vol = SPU_MAXVOL+1;
	
	if(change_vol)
	{
		music_player_vol_pc = (int)((float)100 * ((float)music_player_vol / (float)(SPU_MAXVOL+1)));
		MODSetMaxVolume((music_player_vol == 0)?0:(music_player_vol-1));
	}
	
	music_player_time++;
	
	old_song_pos = music->song_pos;
}

int main()
{
	struct DIRENTRY de;
	struct DIRENTRY *r;
	
	unsigned short padbuf;
	int i;
	int redraw=1;
	char *cp;
	FILE *f;
	
	PSX_Init();
	GsInit();
	SsInit();
	
	GsClearMem();
	GsSetAutoWait();
	GsSetList(primList);
	GsSetVideoMode(320, 240, VMODE_PAL);
	GsLoadFont(768, 0, 768, 256);
	
	SetVBlankHandler(program_vblank_handler);

// Open any existing file before calling firstfile() for the first time.
// If before calling firstfile() for the first time, you have never called open() on an existing file before,
// every subsequent read will fail but it will report that files are opened ok!
// This bug happens at least with the SCPH1001 BIOS version.
// We try to open PSX.EXE and SYSTEM.CNF, as one of them must exist in order to boot from CDROM.

	f = fopen("cdrom:PSX.EXE;1", "rb");
	if(f)fclose(f);
	
	f = fopen("cdrom:SYSTEM.CNF;1", "rb");
	if(f)fclose(f);
	
	update_file_list();
	
	dbuf = !dbuf;
	GsSetDispEnvSimple(0, dbuf?256:0);
	GsSetDrawEnvSimple(0, dbuf?0:256, 320, 240);
	
	while(1)
	{
		if(display_is_old)
		{
			/* Update music */
			if(music)
			{
				redraw = 1;
				MODPlay(music, &music_loop);
				music_player_draw();
			}
			else
			{
				file_browser(redraw);
				redraw=0;
			}

			display_is_old = 0;
		}
	}
}
