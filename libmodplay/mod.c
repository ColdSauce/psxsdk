// Ultimate SoundTracker / NoiseTracker / ProTracker module file support for MODPlay

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "modplay.h"
#include "modplay_int.h"
#include "modtbl.h" // Period -> frequency table for ProTracker MODs

ModMusic *MODLoad_MOD(void *d)
{
	unsigned char *c = d;
	ModMusic *m;
	int x;
	int mp=0;
	int y;
	
// Allocate memory for mod structure	
	m = (ModMusic*)malloc(sizeof(ModMusic));

// Get title	
	memcpy(m->title, &c[0], 20);

// For now let's assume there are 32 samples...	
	mp+=20;	

	memcpy(m->id, &c[0x438], 4);
	
// If there is not a valid ID, this is the start of pattern data,
// otherwise jump four bytes	
	
	if(strncmp(m->id, "M.K.",4) == 0 || strncmp(m->id, "FLT4", 4) == 0 ||
		strncmp(m->id, "M!K!",4) == 0 || strncmp(m->id, "4CHN", 4) == 0 ||
			strncmp(m->id, "6CHN", 4) == 0 || strncmp(m->id, "8CHN", 4) == 0)
		m->sample_num = 31;
	else
		m->sample_num = 15;

// Standard channel number is 4	
	
	if(strncmp(m->id, "6CHN", 4) == 0)
		m->channel_num = 6;
	else if(strncmp(m->id, "8CHN", 4) == 0)
		m->channel_num = 8;
	else
		m->channel_num = 4;
	
// Get sample information
	
	m->sample = malloc(sizeof(ModSample) * m->sample_num);
	
	for(x = 0; x < m->sample_num; x++)
	{
	// Get sample name
		memcpy(m->sample[x].name, &c[mp], 22);
		mp+=22;
	// Get sample length	
		m->sample[x].length = (c[mp] << 8) | c[mp+1];
		m->sample[x].length*= 2;
		mp+=2;
	// Get finetune value
		m->sample[x].finetune = c[mp] & 0xf;
		
		if(m->sample[x].finetune & 0x8)
			m->sample[x].finetune|=0xf0;
	
		mp++;
	// Get sample volume
		m->sample[x].volume = c[mp];
		mp++;
	
	// Get sample repeat offset 
		m->sample[x].repeat_off = (c[mp] << 8) | c[mp+1];
		m->sample[x].repeat_off *= 2;
		mp+=2;
		
	// Get sample repeat length
		m->sample[x].repeat_len = (c[mp] << 8) | c[mp+1];
		m->sample[x].repeat_len *= 2;
		mp+=2;

	// Samples are always 8-bit
		m->sample[x].bits = 8;
		
	// Data type is always 0
		m->sample[x].data_type = 0;
	}
	
// Get number of song positons
	m->song_pos_num = c[mp++];
	
// Ignore this value...	
	mp++;

// Get pattern table	
	memcpy(m->pattern_tbl, &c[mp], 128);
	mp+=128;
	

	
// Get ID (it is not assured that this value will be valid)
	memcpy(m->id, &c[0x438], 4);
// If there is not a valid ID, this is the start of pattern data,
// otherwise jump four bytes	
	
	if(strncmp(m->id, "M.K.",4) == 0 || strncmp(m->id, "FLT4", 4) == 0 ||
		strncmp(m->id, "M!K!",4) == 0 || strncmp(m->id, "4CHN", 4) == 0 ||
			strncmp(m->id, "6CHN", 4) == 0 || strncmp(m->id, "8CHN", 4) == 0)
		mp+=4;
	
// Get number of patterns
// This is actually done by scanning the pattern table for the highest value
	y = 0;
	
	for(x=0;x<128;x++)
	{
		//printf("%x, \n", m->pattern_tbl[x]);
		if(m->pattern_tbl[x] > y)
			y = m->pattern_tbl[x];
	}
	
	//printf("\n");
	
	m->pattern_num = y+1;
	
// Pattern row sizes are always 64
	for(x = 0; x < m->pattern_num; x++)
		m->pattern_row_num[x] = 64;
		
// Allocate memory for patterns...
	m->pattern_data = malloc(m->pattern_num * ((4*m->channel_num)*64));
	
// Get pattern data
	memcpy(m->pattern_data,&c[mp],  m->pattern_num * ((4*m->channel_num)*64));
	mp += m->pattern_num * ((4*m->channel_num)*64);
		
// Allocate & Get sample data
	for(x = 0; x < m->sample_num; x++)
	{
		if(m->sample[x].length < 32 || (modload_flags & MODLOAD_NOSAMPLES))
			m->sample[x].data = NULL;
		else
		{
			m->sample[x].data = malloc(m->sample[x].length);
			memcpy(m->sample[x].data,  &c[mp], m->sample[x].length);

			// Convert to unsigned 8-bit format
			// Most sound cards/programs nowadays want data in this format		
			for(y = 0; y < m->sample[x].length; y++)
				m->sample[x].data[y] ^= 0x80;
		}
		
		mp += m->sample[x].length;
	}
	
	m->song_pos = 0;
	m->pat_pos = 0;
	m->divisions_sec = 7;
	m->beats_minute = 125;
	m->ticks_division = 6;
	
	for(x = 0; x<8;x++)
	{
		m->old_samples[x] = 1;
		m->old_periods[x] = 0;
	}
	
	m->cur_tick = 0;
	m->fmt = MOD_FMT_MOD;
// MOD has no instruments!	
	m->instrument_num = 0;
	
	return m;
}

void MODPlay_MOD(ModMusic *m,int *t)
{
	int cur_pat = m->pattern_tbl[m->song_pos];
	int cur_pat_pos = m->pat_pos;
	unsigned char b[4];
	int s, p, e,x,y;
	int do_not_increase_pat = 0;
	int v1,f;
	
	if(*t == 0)
		return;

		
	m->cur_tick++;

	if(m->cur_tick != (50 / m->divisions_sec))
		return;
	
	for(x = 0; x < m->channel_num; x++)
	{
		memcpy(b, &m->pattern_data[(cur_pat * ((4*m->channel_num)*64)) + (cur_pat_pos * (4*m->channel_num)) + (x*4)], 4);
		
	// Get sample
		s = (b[2] & 0xf0)>>4;
		s |= b[0] & 0xf0;
		
	// Get period
		p = b[1];
		p|= (b[0] & 0xf)<<8;
		p&=~(2048|1024);
	
	// Get effect
		e = b[3];
		e|= (b[2] & 0xf)<<8;

		if(s != 0 && p==0)
			p = m->old_periods[x];	
		
		if(s == 0 && p != 0)
			s=m->old_samples[x];

		v1 = m->sample[s-1].volume;
		
		switch(e & 0xf00)
		{
			case 0xc00: // Set volume
				v1 = e & 0xff;
			break;
		}
				
		f = -1;
		
		for(y = 0; y < sizeof(modplay_pitch_per_tbl) / 4; y++)
		{
			if(modplay_pitch_per_tbl[y<<1] == p)
			{
				f = modplay_pitch_per_tbl[(y<<1)+1];
				break;
			}
		}
		
		if(f==-1 && p!=0)
		{
			printf("Couldn't find period %d in table. Calculating it.\n", p);
			f = SsFreqToPitch(7159090/(p*2));
		}

		f+=m->transpose;

		if(f<0)f=0;
		else if(f>0x3fff)f=0x3fff;

		v1 <<= 8;
		
		if(v1 >= 0x4000)
			v1 = 0x3fff;
		

		if(s && p!=0)
		{
			if(x == 0 || x == 3 || x == 4 || x == 7)
				MODPlay_func(m, x, s-1, f, v1, 0);
			else
				MODPlay_func(m, x, s-1, f, 0, v1);
		}
			
		switch(e & 0xf00)
		{
			case 0xb00: // Position Jump
				m->song_pos = e & 0xff;
				m->pat_pos = 0;
				// printf("Jump to song pos %d\n", m->song_pos);
			
				// this fixes some mods which jump over the mod itself
			
				if(m->song_pos >= m->song_pos_num)
					m->song_pos = 0;
			
				do_not_increase_pat = 1;
			break;
			case 0xd00: // Pattern break
				m->song_pos++;
				m->pat_pos = (((e&0xf0)>>4)*10)+(e&0xf);
				// printf("Pattern break, newpatpos=%d\n", m->pat_pos);
			
				// this fixes some mods which jump over themselves
			
				if(m->song_pos >= m->song_pos_num)
					m->song_pos = 0;
			
				do_not_increase_pat = 1;
			break;
			case 0xf00: // Tempo
				/*v1 = (e & 0xf0) >> 4;
				v2 = e & 0xf;
				v3 = (v1*16)+v2;*/
			
				if((e & 0xff) <= 32)
				{
					if((e&0xff) == 0)e++;
					m->ticks_division = e & 0xff;
				}
				else
					m->beats_minute = e & 0xff;
			
				m->divisions_sec = 24 * m->beats_minute;
				m->divisions_sec /= m->ticks_division;
				m->divisions_sec /= 60;
			break;
		}
		
		if(s) m->old_samples[x] = s;
		if(p) m->old_periods[x] = p;

		m->cur_tick = 0;
	}
	
	if(!do_not_increase_pat)m->pat_pos++;
	
	if(m->pat_pos == 64)
	{
		m->song_pos++;
		if(m->song_pos >= m->song_pos_num)
		{
			*t-=1;

			MODRewind(m);
		}
			
		m->pat_pos = 0;
	}
}
