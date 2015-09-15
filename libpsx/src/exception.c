/**
 * exception.c
 *
 * Exception handling code (part 2)
 */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <psx.h>
 
#define DPCR			*((unsigned int*)0x1f8010f0)
#define DICR			*((unsigned int*)0x1f8010f4)
#define IPENDING		*((volatile unsigned int*)0x1f801070)
#define IMASK			*((volatile unsigned int*)0x1f801074)

#define GPU_DATA_PORT_ADDR			0x1f801810
#define GPU_CONTROL_PORT_ADDR			0x1f801814
#define GPU_DATA_PORT				*((unsigned int*)GPU_DATA_PORT_ADDR)
#define GPU_CONTROL_PORT			*((unsigned int*)GPU_CONTROL_PORT_ADDR) 

void __psxsdk_exception_manager();

void (*_EXC_vblank_handler)();
void (*_EXC_cdrom_handler)();
void (*_EXC_sio_handler)(unsigned char *data);
void (*_EXC_dma_handler)();
unsigned int _EXC_vblank_handler_set;
unsigned int _EXC_cdrom_handler_set;
unsigned int _EXC_sio_handler_set;
unsigned int _EXC_dma_handler_set;

volatile int __psxsdk_gpu_dma_finished;

void __psxsdk_real_exception_handler()
{
	unsigned int Cause = get_cop0_register(COP0_CAUSE);
	unsigned int Cause_excCode = (Cause >> 2) & 31;
	unsigned int Cause_IP = (Cause >> 8) & 255; 
	unsigned int SR = get_cop0_register(COP0_SR);
	unsigned int SR_IM = (SR >> 8) & 255;
	int i;
/*	unsigned int oldSR = SR;
	unsigned int psxIP = IPENDING;
	unsigned int psxIM = IMASK;
	unsigned int sio_data;*/
	
	if(Cause_excCode == 0) // interrupt generated the exception
	{
		/*for(i = 0; i < 8; i++)
		{
			if((Cause_IP & (1<<i)) && (SR_IM & (1<<i)))
			{
				SR ^= 1<<(i+8);
				set_cop0_register(COP0_SR, SR);
				
				switch(i)
				{
					case 2: // PSX Interrupt controller
					//	program_vblank_handler();
					break;
				}
			}
		}*/
		
		if((Cause_IP & (1<<2)) && (SR_IM & (1<<2)))
		{
			Cause ^= 1<<10;
			set_cop0_register(COP0_CAUSE, Cause);
			//SR ^= 1<<10;
			//set_cop0_register(COP0_SR, SR);

			//while(IPENDING != 0)
			{			
				for(i = 0; i < 11; i++)
				{
					if(IPENDING & (1<<i))
					{
				//		printf("IP = %x\n", IPENDING);
// Very interesting, when reading joypad status the PCSXR emulator sets in IPENDING that IRQ7 is pending even
// if it is not enabled in IMASK! That is insane, but it can be easily worked around.
// So we are going to disable pending bits for IRQs in IPENDING in any case, even if the in IRQ at hand was
// not enabled in IMASK.
						
						IPENDING ^= 1 << i;
						
						if(IMASK & (1<<i))
						{
					//		printf("IM = %x\n", IMASK);
							
							switch(i)
							{
								case 0: // VBLANK					
							//		if(!(GPU_CONTROL_PORT & (1<<0x1c)))
							//			GPU_CONTROL_PORT = 0x02000000;
								
								// Execute the user-supplied VBlank handler.	
									if(_EXC_vblank_handler_set)
										_EXC_vblank_handler();
								break;
								case 2:
							/*		if(_EXC_cdrom_handler_set)
										_EXC_cdrom_handler();*/
								break;
								case 3: // DMA
								// Execute the user-supplied DMA handler.	
									if(_EXC_dma_handler_set)
										_EXC_dma_handler();
								break;
								case 8:
								//while(!(SIO_STAT & 2))
								//sio_data = SIO_RX_DATA;
							
								//_EXC_sio_handler((unsigned char*)&sio_data);
						
							//	SIO_CTRL |= SIOCTRL_ACK;
								break;
							}
						}
					}
				}
			}
		}		
	}
}
 
static void __psxsdk_dma_handler()
{
	unsigned int s_dicr = DICR;
	unsigned char irq = (s_dicr >> 24) & 127;
	
	if(irq & (1<<2)) // GPU
		__psxsdk_gpu_dma_finished = 1;
		
	// Acknowledge
	DICR = s_dicr;
	
	// Waste some cycles, so that the acknowledgement is reported
//	int x;
	
//	for(x = 0; x < 1000; x++);
}

extern void _internal_cdromlib_callback();

 void __PSX_Init_NoBios()
 {
	_EXC_vblank_handler = NULL;
	_EXC_cdrom_handler = _internal_cdromlib_callback;
	_EXC_dma_handler = __psxsdk_dma_handler;
	_EXC_sio_handler = NULL;
	 
	_EXC_vblank_handler_set = 0;
	_EXC_cdrom_handler_set = 0;
	_EXC_dma_handler_set = 1;
	_EXC_sio_handler_set = 0;
	 
 	IMASK = 0; // Clear Mask
	IPENDING = 0; // Clear pending interrupts

// Disable interrupts	 
	 
	set_cop0_register(COP0_SR, 0);
	
// Change exception vector to point to our exception manager	 
	 
	*((unsigned int*)0x80000080) = 0x08000000 | ((((unsigned int)__psxsdk_exception_manager)>>2) & 0x3FFFFFF);
	*((unsigned int*)0x80000084) = 0;
	
	
// Enable interrupt generation, and interrupt 2 (PlayStation Interrupt Controller)
	set_cop0_register(COP0_SR, (1<<10) | 1);

// Enable VBlank, CDROM and DMA IRQs (on PlayStation Interrupt Controller)
	IMASK = 1 | /* CDROM */ /*4 |*/ 8;

// Set DMA channel priority
	DPCR = 0x07654321;
	
// Enable DMA IRQ master, and IRQ generation for DMA channel 2 (GPU)
	DICR = (1<<23) | (1<<(16+2));

// Setup variables
	 __psxsdk_gpu_dma_finished = 1;
}
