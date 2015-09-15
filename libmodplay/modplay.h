#ifndef _MODPLAY_H
#define _MODPLAY_H

#ifndef NO_PSX_LIB
	#include <psx.h>
#endif

/** Format identification  IDs. */

enum modplay_formats
{
	MOD_FMT_MOD, /** Ultimate SoundTracker / NoiseTracker / ProTracker */
};

typedef struct
{
	char name[32];
	unsigned int length; // Length in bytes
	char finetune;
	unsigned char volume;
	unsigned int repeat_off;
	unsigned int repeat_len;
	unsigned char bits;
	unsigned char data_type;
	unsigned char *data;
}ModSample;

/** Instrument. */

typedef struct
{
	char name[64];
	int sample_num;
	unsigned char sample_ids[8];
}ModInstrument;

/** Music */

typedef struct
{
	/** Music title. */
	char title[32];
	/** Number of samples in the music */
	int sample_num;
	/** Number of channels used by the music */
	int channel_num;
	/** Number of instruments used by the music */
	int instrument_num;
	/** Pointer to an array of ModSample structures. */
	ModSample *sample;
	/** Pointer to an array of ModInstrument structures. */
	ModInstrument *instrument;
	/** Number of song positions. */
	unsigned char song_pos_num;
	/** Pattern table. */
	unsigned char pattern_tbl[256];
	/** Number of rows for each pattern. */
	unsigned char pattern_row_num[256];
	/** ID, such as "M!K!","M.K.","FLT4", etc. */
	char id[4];
	/** Number of patterns. */
	int pattern_num;
	/** Pointer to pattern data */
	unsigned char *pattern_data;
	/** Format of music. */
	int fmt;

	/** [Runtime] Current song position */
	unsigned char song_pos;
	/** [Runtime] Position inside the pattern currently being played */
	unsigned char pat_pos;
	/** [Runtime] Divisions per second */
	int divisions_sec;
	/** [Runtime] Beats per minute */
	unsigned char beats_minute;
	/** [Runtime] Ticks per division */
	unsigned char ticks_division;
	/** [Runtime] Current tick count */
	unsigned char cur_tick;
	/** [Runtime] Old periods for each channel. */
	unsigned short old_periods[8];
	/** [Runtime] Old sample numbers for each channel. */
	unsigned char old_samples[8];
	/** [Runtime] In PlayStation pitch, this is added to the original sample pitch
			      and can be used to change the pitch of the music for special effects */
	short transpose;
}ModMusic;

/** Flags for MODLoad */
enum modload_flags
{
	/** Do not load the samples in memory */
	MODLOAD_NOSAMPLES = 1,
};

/**
 * Allocate a ModMusic structure and copy data to it from 
 * data in memory containing a music module file
 *
 * Almost all data from the music module file is copied into another location
 * in memory for the ModMusic structure.
 *
 * This means your free memory should be roughly the double of 
 * the size of the MOD you're loading.
 *
 * You can avoid loading the samples in the module file's native format,
 * thus saving useful memory, by passing the MODLOAD_NOSAMPLES
 * flag. This is especially the case when you use MOD4PSX_Upload().
 *
 * @param d Pointer to a buffer containing a music module file
 * @param flags Flag bitmask.
 * @return Pointer to newly allocated ModMusic structure.
 */

ModMusic *MODLoadEx(void *d, unsigned int flags);

/**
 * Just like MODLoadEx() but with the flags parameters set to zero, i.e.
 * default behaviour.
 *
 * @param d Pointer to a buffer containing a music module file
 * @return Pointer to newly allocated ModMusic structure.
 */

ModMusic *MODLoad(void *d);

/**
 * Play a tick of a music.
 *
 * This has to be called 60 / 50 times per second.
 *
 * MODPlay decreases the value referenced by t every time
 * the music finishes. 
 *
 * Set the variable pointed by t when you want to set the number of times again!
 *
 * @param m Pointer to ModMusic structure
 * @param t Pointer to an int which contains how many times the music module has to be played.
 *                 i.e. if *t == 1, play once, if *t == 2, play twice, ..., if *t == -1, loop endlessly
 *
 */

void MODPlay(ModMusic *m,int *t);

/**
 * Stop a music.
 * @param m Pointer to ModMusic structure for the music.
 */
void MODStop(ModMusic *m);

/**
 * Rewind music, that is, make it restart from the beginning.
 * @param m Pointer to ModMusic structure for the music.
 */
void MODRewind(ModMusic *m);

/**
 * Upload the samples of the module music to Sound RAM
 * @param m Pointer to ModMusic structure for the music.
 * @param base_addr Sound RAM address to start from when uploading to Sound RAM
 *  If -1 it is interpreted to be the same as the start of the section for sound data in Sound RAM
 * (SPU_DATA_BASE_ADDR). base_addr must be a multiply of 8.
 * @return The sound address after all the uploaded samples
 */

int MODUploadSamples(ModMusic *m, int base_addr);

/**
 * Sets the SPU voice to use as the first channel when playing music.
 *
 * The voice for the second channel will then be this (value+1), and so on...
 *
 * Usually the base voice is 0; a MOD file can have up to eight channels, so take care of that.
 *
 * A 669 file can always have 8 channels.
 * @param base_voice Desired base voice (0-23)
 */

void MODSetBaseVoice(int base_voice);

/**
 * Sets transpose for music
 * 
 * Changing the transpose value for a music shifts the frequency
 * its samples are played at, but the music's tempo is unchanged.
 * @param m Pointer to ModMusic structure
 * @param transpose Transpose value
 */
 
void MODSetTranspose(ModMusic *m, short transpose);

// Internal function... THIS SHOULD NOT BE HERE, GET RID OF THIS HERE!!

/**
 * Upload preconverted ADPCM samples, as generated by the mod4psx tool.
 *
 * @param d Pointer to buffer containing the ADPCM samples archive
 * @param base_addr Base address at which the samples will start to be uploaded.
 * @return The sound address after all the uploaded samples
 */

int MOD4PSX_Upload(void *d, int base_addr);

/**
 * Free memory allocated for music module
 * @param m Pointer to ModMusic structure
 */

void MODUnload(ModMusic *m);

/**
 * Set maximum volume for MODPlay
 * @param max_volume Maximum volume desired (0-0x3FFF)
 */

void MODSetMaxVolume(unsigned short max_volume);

/**
 * Set mono mode 
 * @param value If 0 set stereo mode, if 1 set mono mode
 */

void MODSetMono(int value);

#endif
