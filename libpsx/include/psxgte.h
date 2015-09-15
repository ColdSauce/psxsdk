#ifndef _PSXGTE_H
#define _PSXGTE_H

/** GTE operations */

enum
{
	/** Perspective transformation */
	GTE_OP_RTPS = 	0x0180001,
	/** Perspective Transformation on 3 points */
	GTE_OP_RTPT = 	0x0280030,
	/** Multiply vector by matrix and vector addition */
	GTE_OP_MVMVA =	0x0400012,
	/** Depth Cue Color light */
	GTE_OP_DCPL =	0x0680029,
	/** Depth Cueing */
	GTE_OP_DPCS =	0x0780010,
	/** Interpolation of a vector and far color vector */
	GTE_OP_INTPL =	0x0980011,
	/** Square vector */
	GTE_OP_SQR =	0x0A00428,
	/** Normal color single vector */
	GTE_OP_NCS =	0x0C8041E,
	/** Normal color three vectors */
	GTE_OP_NCT =	0x0D80420,
	/** Normal color depth cue single vector */
	GTE_OP_NCDS =	0x0E80413,
	/** Normal color depth cue three vectors */
	GTE_OP_NCDT =	0x0F80416,
	/** Depth Cueing */
	GTE_OP_DPCT =	0x0F8002A,
	/** Normal Color Color single vector */
	GTE_OP_NCCS =	0x108041B,
	/** Normal Color Color three vectors */
	GTE_OP_NCCT =	0x118043F,
	/** Color Depth Cue */
	GTE_OP_CDP =	0x1280414,
	/** Color Color */
	GTE_OP_CC =	0x138041C,
	/** Normal clipping */
	GTE_OP_NCLIP =	0x1400006,
	/** Average of three Z values */
	GTE_OP_AVSZ3 =	0x158002D,
	/** Average of four Z values */
	GTE_OP_AVSZ4 =	0x168002E,
	/** Outer product of 2 vectors */
	GTE_OP_OP =	0x170000C,
	/** General purpose interpolation */
	GTE_OP_GPF =	0x190003D,
	/** General purpose interpolation */
	GTE_OP_GPL =	0x1A0003E
}gte_operations;

/** GTE error flags, grab from cop2 register 63 */

enum
{
	GTE_FLAG_MAC1_OVF_POS =		0x40000000,
	GTE_FLAG_MAC2_OVF_POS =		0x20000000,
	GTE_FLAG_MAC3_OVF_POS =		0x10000000,

	GTE_FLAG_MAC1_OVF_NEG =		0x08000000,
	GTE_FLAG_MAC2_OVF_NEG =		0x04000000,
	GTE_FLAG_MAC3_OVF_NEG =		0x02000000,

	GTE_FLAG_IR1_SATURATED =	0x01000000,
	GTE_FLAG_IR2_SATURATED =	0x00800000,
	GTE_FLAG_IR3_SATURATED =	0x00400000,

	GTE_FLAG_COL_FIFO_R_SATURATED =	0x00200000,
	GTE_FLAG_COL_FIFO_G_SATURATED =	0x00100000,
	GTE_FLAG_COL_FIFO_B_SATURATED =	0x00080000,

	GTE_FLAG_SZ3_OTZ_SATURATED =	0x00040000,

	GTE_FLAG_DIV_OVF_SATURATED =	0x00020000,

	GTE_FLAG_MAC0_OVF_POS =		0x00010000,
	GTE_FLAG_MAC0_OVF_NEG =		0x00008000,

	GTE_FLAG_SX2_SATURATED =	0x00004000,
	GTE_FLAG_SY2_SATURATED =	0x00002000,

	GTE_FLAG_IR0_SATURATED =	0x00001000
}gte_error_flags;

/** GTE data registers */

enum
{
	/** Vector 0 X and Y */
	GTE_R_VXY0 =	0,
	/** Vector 0 Z */
	GTE_R_VZ0 =	1,	
	/** Vector 1 X and Y */
	GTE_R_VXY1 =	2,
	/** Vector 1 Z */
	GTE_R_VZ1 =	3,
	/** Vector 2 X and Y */
	GTE_R_VXY2 =	4,
	/** Vector 2 Z */
	GTE_R_VZ2 =	5,
	/** RGB value */
	GTE_R_RGB =	6,
	/** Z Average value */
	GTE_R_OTZ =	7,
	/** Intermediate value 0 */
	GTE_R_IR0 =	8,
	/** Intermediate value 1 */
	GTE_R_IR1 =	9,
	/** Intermediate value 2 */
	GTE_R_IR2 =	10,
	/** Intermediate value 3 */
	GTE_R_IR3 =	11,
	/** Screen XY coordinates 0 FIFO */
	GTE_R_SXY0 =	12,
	/** Screen XY coordinates 1 FIFO */
	GTE_R_SXY1 =	13,
	/** Screen XY coordinates 2 FIFO */
	GTE_R_SXY2 =	14,
	/** Screen XY coordinates P FIFO */
	GTE_R_SXYP =	15,
	/** Screen Z 0 FIFO */
	GTE_R_SZ0 =	16,
	/** Screen Z 1 FIFO */
	GTE_R_SZ1 =	17,
	/** Screen Z 2 FIFO */
	GTE_R_SZ2 =	18,
	/** Screen Z 3 FIFO */
	GTE_R_SZ3 =	19,
	/** Characteristic color 0 FIFO */
	GTE_R_RGB0 =	20,
	/** Characteristic color 1 FIFO */
	GTE_R_RGB1 =	21,
	/** Characteristic color 2 FIFO */
	GTE_R_RGB2 =	22,
	/** Sum of products value 0 */
	GTE_R_MAC0 =	24,
	/** Sum of products value 1 */
	GTE_R_MAC1 =	25,
	/** Sum of products value 2 */
	GTE_R_MAC2 =	26,
	/** Sum of products value 3 */
	GTE_R_MAC3 =	27,
	/** IRGB ?? */
	GTE_R_IRGB =	28,
	/** ORGB ?? */
	GTE_R_ORGB =	29,
	/** Leading zero count source data. */
	GTE_R_LZCS =	30,
	/** Leading zero count result */
	GTE_R_LZCR =	31
}gte_data_registers;
	

#endif
