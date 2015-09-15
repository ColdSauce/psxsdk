/**
 * MODPlay: internal prototypes
 */

#ifndef _MODPLAY_INT_H
#define _MODPLAY_INT_H

void MODPlay_func(ModMusic *m, int c, int s, int p, int vl, int vr);
extern int modplay_int_cnt;
extern unsigned int modload_flags;
extern const unsigned short modplay_pitch_per_tbl[120];

ModMusic *MODLoad_MOD(void *d);
void MODPlay_MOD(ModMusic *m, int *t);

#endif
