#ifndef _SPU_H
#define _SPU_H

/** Start address of sound data in Sound RAM */
#define SPU_DATA_BASE_ADDR		0x1010
/** Maximum volume. */
#define SPU_MAXVOL				0x3FFF

/** VAG file */

typedef struct
{
	/** Version. */
	unsigned int version;
	/** Data size. */
	unsigned int data_size;
	/** Sample rate. */
	unsigned int sample_rate;
	/** Name */
	unsigned char name[16];
	/** Pointer to sound data */
	void *data;
	/** Address in Sound RAM where the sound data was uploaded */
	unsigned int spu_addr;
	/** [Runtime] Voice this VAG is currently being played on */
	char cur_voice;
}SsVag;

/**
 * Set voice volume.
 * @param voice Voice number (0-23)
 * @param left Left channel volume
 * @param right Right channel volume
 */

void SsVoiceVol(int voice, unsigned short left, unsigned short right);

/**
 * Set voice pitch.
 * @param voice Voice
 * @param pitch Pitch.
 */

void SsVoicePitch(int voice, unsigned short pitch);


/**
 * Set start Sound RAM address for voice.
 * @param voice Voice
 * @param addr Start address in Sound RAM (multiplier of 8)
 */

void SsVoiceStartAddr(int voice, unsigned int addr);

/**
 * Set ADSR level for voice
 * @param voice Voice
 * @param level ADSR level
 * @param rate ADSR rate
 */

void SsVoiceADSRRaw(int voice, unsigned short level, unsigned short rate);

/**
 * Set repeat address for voice
 * @param voice Voice
 * @param addr Address in Sound RAM (multiplier of 8)
 */

void SsVoiceRepeatAddr(int voice, unsigned int addr);

/**
 * Set a voice to 'on'. This has the effect of playing the sound specified for the voice.
 * @param voice Voice
 */

void SsKeyOn(int voice);

/**
 * Set a voice to 'off'. This stops the sound specified for the voice.
 * @param voice Voice
 */

void SsKeyOff(int voice);

/**
 * Set the voices specified by the bitmask to 'on'. Like SsKeyOn()
 * @param mask Bitmask
 */
 
void SsKeyOnMask(int mask);

/**
 * Set the voices specified by the bitmask to 'off'. Like SsKeyOff()
 * @param mask Bitmask
 */

void SsKeyOffMask(int mask);

/**
 * Wait for the SPU to be ready.
 */

void SsWait(void);

/**
 * Intialize the SPU.
 */

void SsInit(void);

/**
 * Uploads sound data in PSX ADPCM format to Sound RAM.
 * @param addr Pointer to PSX ADPCM sound data in main RAM
 * @param size Sound data size
 * @param spu_addr Destination address in Sound RAM (multiplier of 8).
 */

void SsUpload(void *addr, int size, int spu_addr);

/**
 * Converts a sampling rate in hertz to PlayStation pitch rate used by the SPU.
 * @param hz Sampling rate in hertz.
 * @return PlayStation pitch rate
 */

unsigned short SsFreqToPitch(int hz);

/**
 * Reads information from a buffer containg a VAG file and stores it inside a SsVag structure.
 * @param vag Pointer to structure in which to store information.
 * @param data Pointer to VAG file data
 */

int SsReadVag(SsVag *vag, void *data);

/**
 * Uploads the sound data specified by a SsVag structure to the specified address in Sound RAM.
 * The SsVag structure can then be used for playing with SsPlayVag()
 * @param vag Pointer to SsVag structure
 * @param spu_addr Destination address in Sound RAM (multiplier of 8)
 */

void SsUploadVagEx(SsVag *vag, int spu_addr);


/**
 * Uploads the sound data specified by a SsVag structure to Sound RAM, beginning from the
 * base of usable Sound RAM and continuing from there, in an automatic fashion.
 * @param vag Pointer to SsVag structure
 */

void SsUploadVag(SsVag *vag);

/**
 * Plays the sound specified by the SsVag structure at specified voice and volume.
 * @param vag Pointer to SsVag structure
 * @param voice Voice
 * @param vl Left channel volume
 * @param vr Right channel volume
 */

void SsPlayVag(SsVag *vag, unsigned char voice, unsigned short vl, 
	unsigned short vr);

/**
 * Stops the sound specified by the SsVag structure
 * @param vag Pointer to SsVag structure
 */

void SsStopVag(SsVag *vag);

/**
 * Tell SsUploadVag() to start uploading from the base of usable Sound RAM again.
 */

void SsResetVagAddr(void);

/**
 * Enable CD Audio.
 */

void SsEnableCd(void);

/**
 * Enable External audio. (???)
 */

void SsEnableExt(void);

/**
 * Set CD Audio volume.
 * @param left Left channel volume
 * @param right Right channel volume
 */

void SsCdVol(unsigned short left, unsigned short right);

#endif
