#ifndef _PSXPAD_H
#define _PSXPAD_H

/* Pad bits defines. */

#define PAD_LEFT	(1<<15)
#define PAD_RIGHT	(1<<13)
#define PAD_UP		(1<<12)
#define PAD_DOWN	(1<<14)
#define PAD_L2		1
#define PAD_R2		(1<<1)
#define PAD_L1		(1<<2)
#define PAD_R1		(1<<3)
#define PAD_TRIANGLE	(1<<4)
#define PAD_CIRCLE	(1<<5)
#define PAD_CROSS	(1<<6)
#define PAD_SQUARE	(1<<7)
#define PAD_SELECT	(1<<8)
#define PAD_LANALOGB	(1<<9)
#define PAD_RANALOGB	(1<<10)
#define PAD_START	(1<<11)

#define PAD_READ_RAW_SIZE		21

/**
 * These values below are to be used for evalauting the type field of the
 * psx_pad_state structure
 */

enum psx_pad_types
{
	/** No pad connected. */
	PADTYPE_NONE,
	/** Normal pad. */
	PADTYPE_NORMALPAD,
	/** Analog joystick, the early ones with analog. */
	PADTYPE_ANALOGJOY,
	/** Analog pad, the Dual Shock controller. */
	PADTYPE_ANALOGPAD,
	/** Namco NeGcon. Many steering wheels implement this protocol as well. */
	PADTYPE_NEGCON, // Namco NeGcon
	/** Konami Justifier gun controller. Many third party gun controllers implement this protocol. */
	PADTYPE_KONAMIGUN,
	/** Unknown pad type. */
	PADTYPE_UNKNOWN
};

/**
 * This structure contains the state of the pad (game controller) after
 * polling it with PSX_PollPad()
 */

typedef struct
{
	/** Status. 0 on success, 255 on failure. */
	unsigned char status;
	/** Id. Bits 7-4 indicate the type. Bits 3-0 indicate the number of words
	     in the raw packet returned by the controller. */
	unsigned char id;
	/** Type of pad. To be evaluated with the types in the psx_pad_types enum */
	unsigned char type;
	/** Button bitmask. To be checked by AND'ing with the defines in
	     psxpad.h for buttons. If a bit is set for a button, it is pressed.
	     Checking the pad type to use this bitmask is not necessary at all, 
	     and if button emulations are set up, this may not represent the
	     buttons actually pressed. Also reliable when type is PADTYPE_UNKNOWN */
	unsigned short buttons;
	
	/** Extra data for non-normal controllers. 
	     You should check the value of the type field before accessing any of this data
	*/
	
	union extra
	{
		/** Data for analog joysticks. 
		 *   @attention Due to the poor calibration of the analog sticks, it is recommended
		 *   that you do not assume "left" for X values lower than zero and
		 *   "right" for X values higher than zero.
		 *
		 *   @attention It is recommended to pick a value, for instance 64, and assume
		 *   "left" for values lower than -64, "right" for values higher than 64,
		 *   and no movement for values between -64 and 64.
		 *
		 *   @attention The same is valid for Y values, "up" and "down".
		 **/
		
		struct analogJoy
		{
			/** X coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally left: -128, totally right: 127
			*/
			signed char x[2];
			/** Y coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally up: -128, totally down: 127
			*/			
			signed char y[2];
		}analogJoy;
		
		/** Data for analog joypads (controller). 
		 *   @attention Due to the poor calibration of the analog sticks, it is recommended
		 *   that you do not assume "left" for X values lower than zero and
		 *   "right" for X values higher than zero.
		 *
		 *   @attention It is recommended to pick a value, for instance 64, and assume
		 *   "left" for values lower than -64, "right" for values higher than 64,
		 *   and no movement for values between -64 and 64.
		 *
		 *   @attention The same is valid for Y values, "up" and "down".
		 **/
		
		struct analogPad
		{
			/** X coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally left: -128, totally right: 127
			*/
			signed char x[2];
			/** Y coordinates for the left analog stick and the right analog stick 
			 * @par Value 
			 * Totally up: -128, totally down: 127
			*/			
			signed char y[2];
		}analogPad;
		
		/** Data for Namco NeGcon and steering wheels using its protocol. 
		  * Many steering wheels use this protocol or can switch to it if desired.
		 */
		
		struct negCon
		{
			/**
			 * Steering wheel position. 
			 *
			 * Unlike analog sticks, the steering is accurate and this value is reliable.
			 *
			 *  @par Value
			 * When steering left it is lower than zero, when steering right it is higher than zero
			 * and when not steering at all it is zero.
			 */
			
			signed char steering;
			
			/**
			 * Pressure for button I (1).
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char one;
			
			/**
			 * Pressure for button II (2).
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char two;
			
			/**
			 * Pressure for "L" shoulder button.
			 * @par Value 
			 * 0 = not pressed, 255 = maximum pressure
			 */
			
			unsigned char shoulder;
		}negCon;
	}extra;
}psx_pad_state;

void QueryPAD(int pad_n, unsigned char *in, unsigned char *out, int len);
void pad_read_raw(int pad_n, unsigned char *arr);
void pad_escape_mode(int pad_n, int enable);
void pad_enable_vibration(int pad_n);
void pad_set_vibration(int pad_n, unsigned char small, unsigned char big);

#endif
