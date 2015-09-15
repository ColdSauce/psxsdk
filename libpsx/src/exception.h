#ifndef _PSXSDK_EXCEPTION_H
#define _PSXSDK_EXCEPTION_H

extern void (*_EXC_vblank_handler)();
extern void (*_EXC_sio_handler)(unsigned char *data);
extern void (*_EXC_dma_handler)();
extern unsigned int _EXC_vblank_handler_set;
extern unsigned int _EXC_sio_handler_set;
extern unsigned int _EXC_dma_handler_set;


void __PSX_Init_NoBios();

#endif
