#ifndef _PSXGPU_H
#define _PSXGPU_H

/**
 * Initializes the GPU. Same as GsInitEx(0)
 *
 * The display is left disabled.
 * You can enable it with GsEnableDisplay() or more preferably with GsSetVideoMode()
 */
 
void GsInit(void);

/**
 * Initializes the GPU.
 *
 * The display is left disabled.
 * You can enable it with GsEnableDisplay() or more preferably with GsSetVideoMode()
 * @param flags Flag bitmask 
 */
 
void GsInitEx(unsigned int flags);

/**
 * Resets the GPU
 */
 
void GsReset(void);

/**
 * Enables the display (i.e. generation of video output).
 *
 * Unless you are doing low-level GPU programming, you don't need to call this function;
 * the display is enabled automatically by GsSetVideoMode().
 * @param enable If TRUE (>=1) the display will be enabled, if FALSE (== 0) it will be disabled
 */
 
void GsEnableDisplay(int enable);

/**
 * Sets a video mode and enables the display. It does so in a quicker way
 * than GsSetVideoModeEx(), which wants more arguments but offers greater control
 * @param width Width
 + 640, 384, 320 and 256 are supported by the PlayStation hardware.
 * @param height Height
 + 480 and 240 are supported by the PlayStation hardware. 
 *  You probably want to use GsSetVideoModeEx() if you set 
 *   480 here, because this function doesn't enable interlacing.
 *  @param video_mode Video mode
 + VMODE_NTSC for NTSC, VMODE_PAL for PAL
 * @return 1 on success, 0 on failure (such as unavailable video mode)
 */

int GsSetVideoMode(int width, int height, int video_mode);

/**
 * Works like GsSetVideoMode() but offers more control.
 * Interlaced, 24-bit RGB, and "reverse" flags can be specified.
  * @param width Width
 + 640, 384, 320 and 256 are supported by the PlayStation hardware.
 * @param height Height
 + 480 and 240 are supported by the PlayStation hardware. 
  *  @param video_mode Video mode
 + VMODE_NTSC for NTSC, VMODE_PAL for PAL
 * @param rgb24 24-bit RGB mode
 + If TRUE (!=0) enables the very rarely used 24-bit color mode. Unless you're very creative, don't enable this.
 * @param inter Interlacing
 + If TRUE (!=0) enables interlacing. If you set the height paramater to 480, and you want to display your program on
    a real television screen, you must enable this.
 * @param reverse Reverse??
 + The function of this is not really known. Stay safe and set this to FALSE (0).
 * @return 1 on success, 0 on failure (such as unavailable video mode)
 */

int GsSetVideoModeEx(int width, int height, int video_mode, int rgb24, int inter, int reverse);

/**
 * Assigns the internal pointer to the primitive list to the desired one,
 * and resets the linked list position counter.
 * The memory address specified by your pointer has to have enough space free to contain all
 * the packets which you want to send.
 * @param listptr Pointer to primitive list
 */

void GsSetList(unsigned int *listptr);

/**
 * Assigns the internal pointer to the primitive list to the desired one,
 * an sets the linked list position counter to the specified value.
 * The memory address specified by your pointer has to have enough space free to contain all
 * the packets which you want to send, and you must ensure that the specified position
 * is not out of bounds.
 * @param listptr Pointer to primitive list
 * @param listpos List position
 */
 
void GsSetListEx(unsigned int *listptr, unsigned int listpos);

/**
 * Draws the primitive list.
 *
 * This also has the effect of resetting the current drawing list position.
 */
 
void GsDrawList(void);

/**
 * Draws the primitive list using port I/O access.
 * 
 * GsDrawList() uses DMA to transfer the primitive data in the linked list to the GPU.
 *
 * This function which is of main interest to low-level programmers, uses GPU port I/O access instead, 
 * and it is slower (as reads and writes must be done by the main CPU), but works in all situations,
 * even when you can't get DMA working.
 *
 * This function due to its nature is blocking, and always waits for completion.
 */
 
void GsDrawListPIO(void); 

/**
 * Makes non-blocking gpu functions like GsDrawList()
 * wait for completion. Removes the need to use GsIsDrawing()/GsIsWorking()
 */

void GsSetAutoWait(void);

/** Monochrome 3 point polygon */

typedef struct
{
	/** Red color component (0-255) */
	unsigned char r; 
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b; 
	/** X Coordinates for vertexes */
	short x[3]; 
	/** Y Coordinates for vertexes */
	short y[3];
	/** Attribute bitmask */
	unsigned int attribute;
}GsPoly3;

/** Monochrome 4 point polygon */

typedef struct
{
	/** Red color component (0-255) */
	unsigned char r; 
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b; 
	/** X Coordinates for vertexes */	
	short x[4];
	/** Y Coordinates for vertexes */
	short y[4];
	/** Attribute */
	unsigned int attribute;
}GsPoly4;

/** Textured 3 point polygon */

typedef struct
{
	/** Red color component (0-255) */
	unsigned char r; 
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b;
	/** X Coordinates for vertexes */
	short x[3];
	/** Y Coordinates for vertexes */
	short y[3];
	/** X Texture offsets for vertexes */
	unsigned char u[3];
	/** Y Texture offset for vertexes */
	unsigned char v[3];
	/** CLUT X coordinate */
	short cx;
	/** CLUT Y coordinate */
	short cy;
	/** Attribute */
	unsigned int attribute;
	/** Texture page */
	unsigned char tpage;
}GsTPoly3;

/** Textured 4 point polygon */

typedef struct
{
	/** X Coordinates for vertexes */
	short x[4];
	/** Y Coordinates for vertexes */
	short y[4];
	/** Red color component (0-255) */
	unsigned char r;
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b;
	/** CLUT X coordinate */
	short cx;
	/** CLUT Y coordinate */
	short cy;
	/** Texture page */
	unsigned char tpage;
	/** Horizontal texture offset */
	unsigned char u[4];
	/** Vertical texture offset */
	unsigned char v[4];
	/** Attribute */
	unsigned int attribute;
}GsTPoly4;

/** Graduated 3 point polygon */

typedef struct
{
	/** Red color components (0-255) */
	unsigned char r[3];
	/** Green color components (0-255) */
	unsigned char g[3];
	/** Blue color components (0-255) */
	unsigned char b[3];
	/** X Coordinates for vertexes */
	short x[3];
	/** Y Coordinates for vertexes */
	short y[3];
	/** Attribute */
	unsigned int attribute;
}GsGPoly3;

/** Graduated 4 point polygon */

typedef struct
{
	/** Red color components (0-255) */
	unsigned char r[4];
	/** Green color components (0-255) */
	unsigned char g[4];
	/** Blue color components (0-255) */
	unsigned char b[4];
	/** X Coordinates for vertexes */
	short x[4];
	/** Y Coordinates for vertexes */
	short y[4];
	/** Attribute */
	unsigned int attribute;
}GsGPoly4;

/** Graduated textured 3 point polygon */

typedef struct
{
	/** Red color components (0-255) */
	unsigned char r[3];
	/** Green color components (0-255) */
	unsigned char g[3];
	/** Blue color components (0-255) */
	unsigned char b[3];
	/** X Coordinates for vertexes */
	short x[3];
	/** Y Coordinates for vertexes */
	short y[3];
	/** CLUT X coordinate */
	short cx;
	/** CLUT Y coordinate */
	short cy;
	/** Texture page */
	unsigned char tpage;
	/** Horizontal texture offset */
	unsigned char u[3];
	/** Vertical texture offset */
	unsigned char v[3];
	/** Attribute */
	unsigned int attribute;
}GsGTPoly3;

/** Graduated textured 4 point polygon */

typedef struct
{
	/** Red color components (0-255) */
	unsigned char r[4];
	/** Green color components (0-255) */
	unsigned char g[4];
	/** Blue color components (0-255) */
	unsigned char b[4];
	/** X Coordinates for vertexes */
	short x[4];
	/** Y Coordinates for vertexes */
	short y[4];
	/** CLUT X coordinate */
	short cx;
	/** CLUT Y coordinate */
	short cy;
	/** Texture page */
	unsigned char tpage;
	/** Horizontal texture offset */
	unsigned char u[4];
	/** Vertical texture offset */
	unsigned char v[4];
	/** Attribute */
	unsigned int attribute;
}GsGTPoly4;

/** Monochrome line */

typedef struct
{
	/** Red color component (0-255) */
	unsigned char r;
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b;
	/** X Coordinates for points */
	short x[2];
	/** Y Coordinates for points */
	short y[2];
	/** Attribute */
	unsigned int attribute;
}GsLine;

/** Dot (pixel) */

typedef struct
{
	/** Red color component (0-255) */
	unsigned char r;
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b;
	/** X coordinate */
	short x;
	/** Y coordinate */
	short y;
	/** Attribute */
	unsigned int attribute;
}GsDot;

/** Graduated line */

typedef struct
{
	/** Red color components (0-255) */
	unsigned char r[2];
	/** Green color components (0-255) */
	unsigned char g[2];
	/** Blue color components (0-255) */
	unsigned char b[2];
	/** X coordinates for points */
	short x[2];
	/** Y coordinates for points */
	short y[2];
	/** Attribute */
	unsigned int attribute;
}GsGLine;

/** Sprite */

typedef struct
{
	short x, y; /* X, Y positions */
	unsigned char u, v; /* Offset into texture page of sprite image data */
	short w, h; /* Width and height of sprite */
	short cx, cy; /* Color look up table (palette) X, Y positions */
	unsigned char r, g, b; /* Luminosity of color components - 128 is normal luminosity */
	unsigned char tpage; /* Texture page */
	unsigned int attribute; /* Attribute */
	
	/* Scaling? These are only candidates...
	
	   scalex:
	    Denotes horizontal scaling
	    
	    0 = true size (unmodified)
	    1 = true size (*1)
	    2 = double size (*2)
	    3 = triple size (*3)
	    ...
	    
	    -1 = true size (/1)
	    -2 = half size (/2)
	    -3 = one-third size (/3)
	    ...
	    
	   scaley:
	    Denotes vertical scaling
	    
	    *** The behaviour below was introduced in PSXSDK 0.5
	    
	    If scalex > 8,
		resulting width will be (original_width * scalex) / 4096
		scalex = 4096 (SCALE_ONE) (original width), scalex = 2048 (half width), etc.
	    If scaley > 8,
	        resulting height will be (original_height * scaley) / 4096
		works like scalex but affects height
	 */
	 int scalex, scaley;

	int rotate; // Rotation angle - Fixed point notation, 4096 = 1 degree
	int mx, my; // Coordinates of rotation center - relative to coordinates of sprite
}GsSprite;

/** Rectangle */

typedef struct
{
	
	short x, y;
	short w, h;
	unsigned char r, g, b;
	unsigned int attribute; /* Attribute */
}GsRectangle;

typedef struct
{
	/** Number of points */
	unsigned int npoints;
	/** Red color component (0-255) */
	unsigned char r;
	/** Green color component (0-255) */
	unsigned char g;
	/** Blue color component (0-255) */
	unsigned char b;
	/** X Coordinates for points */
	short *x;
	/** Y Coordinates for points */
	short *y;
	/** Attribute */
	unsigned int attribute;
}GsPolyLine;

typedef struct
{
	/** Number of points */
	unsigned int npoints;
	/** Red color components (0-255) */
	unsigned char *r;
	/** Green color components (0-255) */
	unsigned char *g;
	/** Blue color components (0-255) */
	unsigned char *b;
	/** X Coordinates for points */
	short *x;
	/** Y Coordinates for points */
	short *y;
	/** Attribute */
	unsigned int attribute;
}GsGPolyLine;

/** Map */

//typedef struct
//{
//	short x, y; /* X, Y positions */
//	unsigned char u, v; /* Offset into texture page of sprite image data */
//	short w, h; /* Width and height of tilemap */
//	short l; /* Length of tilemap line */
//	short cx, cy; /* Color look up table (palette) X, Y positions */
//	unsigned char r, g, b; /* Luminosity of color components - 128 is normal luminosity */
//	unsigned char tpage; /* Texture page */
//	unsigned int attribute; /* Attribute */
//	
//	unsigned short tmw, tmh; /* Map texture width and height */
//	unsigned char tw, th; /* Map tile width and height */
//	
//	unsigned char tsize; /* Size of tile in map (1 = 8-bit, 2 = 16-bit, 4 = 32-bit) */
//
//	unsigned int tmask; /* Inverted mask for tile number */
//	
//	void *data; /* Pointer to beginning of map data */
//}GsMap;

/** Texture color modes. */

enum psx_gpu_texmodes
{
 /** 4-bit color mode */
 COLORMODE_4BPP, 
 /** 8-bit color mode */
 COLORMODE_8BPP, 
 /** 16-bit color mode */
 COLORMODE_16BPP, 
 /** 24-bit color mode */
 COLORMODE_24BPP
};

/** 
 * This is the luminance factor with which images
 * are drawn as they are stored. (i.e. without applying lighting)
 *
 * NORMAL_LUMINOSITY (sic) is kept for backward compatibility,
 * but "luminosity" is an incorrect term here.
 */

#define NORMAL_LUMINANCE	128
#define NORMAL_LUMINOSITY	NORMAL_LUMINANCE

/**
 * Macro to specify texture color mode, takes a value from psx_gpu_texmodes
 */
#define COLORMODE(x)		x&3
/**
 * Macro to specify translucency/semitransparency mode, where x can be a value from 0 to 3.
 *
 * If a pixel in image data to be drawn has the STP bit set, semitransparency
 * processing is enabled for that pixel.
 *
 * When the color is black (RGB=0,0,0) STP is processed differently from when it is not
 * black.
 *
 * The table below shows the differences:
 *
 * Color  |  STP bit  |  Processing off   |  Processing on
 * -----    | ----          |   -----                   | ----
 * Black |  0 | Transparent  |   Transparent
 * Black |   1 | Non-transparent |Non-Transparent
 * Not black  |  0 | Non-Transparent |Non-Transparent
 * Not black  | 1 | Non-Transparent   |Transparent
 *
 * If the image pixel is semi-transparent (STP bit set) and not black, the formulas for the
 * final pixel color are the following:
 *
 * Mode    | From framebuffer pixel| From image pixel
 * ----        | ---                                   | -----
 * 0 | +50% | +50%
 * 1 | +100% | +100%
 * 2 | +100% | -100%
 * 3 | +100% | +25%
 *
 * The final color component values do not underflow or overflow, they simply saturate at 0 or 255.
 */
#define TRANS_MODE(x)		((x&3)<<2)
/**
 * Enable semi-transparency processing for the primitive.
 */  
#define ENABLE_TRANS		(1<<4)
/**
 * Enable horizontal flipping for the primitive.
 */
#define H_FLIP			(1<<5)
/**
 * Enable vertical flipping for the primitive.
 */
#define V_FLIP			(1<<6)

/**
 * Drawing environment.
 */

typedef struct
{
	/**
	 * Dithering enabled flag.
	 */
	unsigned char dither;
	/**
	 * Enable drawing on display area flag.
	 *
	 * If this flag is enabled, drawing on display area is allowed.
	 *
	 * Usually you enable this flag when you do not want to use double buffering.
	 */
	unsigned char draw_on_display;
	
	/**
	 * Drawing area X start coordinate in framebuffer
	 */
	short x;
	/**
	 * Drawing area Y start coordinate in framebuffer
	 */
	short y;
	/**
	 * Drawing area width.
	 */
	short w;
	/**
	 * Drawing area height.
	 */
	short h;

	/*
	 * Drawing offset
	 */
	//short off_x, off_y;
	
	/**
	 * Masking settings (can also be set with GsSetMasking())
	 */
	
	 /**
	  * Do not draw over pixels which have their mask bit set 
	  */
	unsigned char ignore_mask;
	 /**
	  * If this is set, every pixel drawn will have the mask bit set 
	  */
	unsigned char set_mask;
}GsDrawEnv;

/**
 * Display environment.
 *
 * The width and height of the display area are those of the currently set video mode.
 */

typedef struct
{
	/** Display area X start coordinate in framebuffer. */
	short x;
	/** Display area Y start coordinate in framebuffer. */
	short y;
}GsDispEnv;

/**
 * Image 
 * @brief This structure describes a TIM image
 */

typedef struct
{

	
	/** Pixel (color) mode. 0 = 4bpp, 1 = 8bpp, 2 = 16bpp, 3 = 24bpp */
	int pmode;
	/** Reports whether this image has a Color Look Up Table. 1 if there's a CLUT, 0 otherwise. */
	int has_clut;
	/** X coordinate of CLUT in framebuffer */
	int clut_x;
	/** Y coordinate of CLUT in framebuffer */
	int clut_y;
	/** Width of CLUT in framebuffer */
	int clut_w;
	/** Height of CLUT in framebuffer */
	int clut_h;
	/** X coordinate of image in framebuffer */
	int x;
	/** Y coordinate of image in framebuffer */
	int y; 
	/** Width of image in framebuffer */
	int w;
	/** Height of image in framebuffer */
	int h;
	/** Pointer to CLUT data */
	void *clut_data;
	/** Pointer to image data */
	void *data;
}GsImage;

/**
 * Adds a monochrome 3 point polygon to the packet list
 * @param poly3 Pointer to structure for monochrome 3 point polygon
 */

void GsSortPoly3(GsPoly3 *poly3);

/**
 * Adds a monochrome 4 point polygon to the packet list
 * @param poly4 Pointer to structure for monochrome 4 point polygon
 */

void GsSortPoly4(GsPoly4 *poly4);

/**
 * Adds a textured 3 point polygon to the packet list
 * @param tpoly3 Pointer to structure for textured 3 point polygon
 */
 
void GsSortTPoly3(GsTPoly3 *tpoly3);

/**
 * Adds a textured 4 point polygon to the packet list
 * @param tpoly4 Pointer to structure for textured 4 point polygon
 */

void GsSortTPoly4(GsTPoly4 *tpoly4);

/**
 * Adds a gradated 3 point polygon to the packet list
 * @param poly3 Pointer to structure for gradated 3 point polygon
 */

void GsSortGPoly3(GsGPoly3 *poly3);

/**
 * Adds a gradated 4 point polygon to the packet list
 * @param poly4 Pointer to structure for gradated 4 point polygon
 */

void GsSortGPoly4(GsGPoly4 *poly4);

/**
 * Adds a gradated textured 3 point polygon to the packet list
 * @param tpoly3 Pointer to structure for textured 3 point polygon
 */
 
void GsSortGTPoly3(GsGTPoly3 *tpoly3);

/**
 * Adds a gradated 4 point polygon to the packet list
 * @param tpoly4 Pointer to structure for textured 4 point polygon
 */

void GsSortGTPoly4(GsGTPoly4 *tpoly4);

/**
 * Adds a monochrome line to the packet list
 * @param line Pointer to structure for monochrome line
 */
 
void GsSortLine(GsLine *line);

/**
 * Adds a gradated line to the packet list
 * @param line Pointer to structure for gradated line
 */
 
void GsSortGLine(GsGLine *line);

/**
 * Adds a dot (pixel) to the packet list
 * @param dot Pointer to structure for dot
 */
 
void GsSortDot(GsDot *dot);

/**
 * Adds a sprite to the packet list
 * @param sprite Pointer to structure for sprite
 */

void GsSortSprite(GsSprite *sprite);

/**
 * Always adds a REAL sprite to the packet list
 *
 * + GsSortSprite() on the other hand checks for scaling and flipping, which
 * are not supported by the "sprite" primitive on the PlayStation, but instead
 * are done by using a textured 4-point polygon accordingly.
 * GsSortSprite() only uses the sprite primitive when all the attributes
 * can be done with a "sprite" primitive as well.
 *
 * @param sprite Pointer to structure for sprite
 */
 
void GsSortSimpleSprite(GsSprite *sprite);

/**
 * Adds a rectangle to the packet list
 * @param rectangle Pointer to structure for rectangle
 */
 
void GsSortRectangle(GsRectangle *rectangle);

/**
 * Moves image data from a part of the framebuffer to another.
 * Actually it does a copy.
 * @param src_x Top-left X coordinate of source area
 * @param src_y Top-left Y coordinate of source area
 * @param dst_x Top-left X coordinate of destination area
 * @param dst_y Top-left Y coordinate of destination area
 * @param w Width of area
 * @param h Height of area
 */

void MoveImage(int src_x, int src_y, int dst_x, int dst_y, int w, int h);

/**
 * Loads image data into framebuffer memory.
 * @param img Pointer to raw image data
 * @param x Top-left X coordinate of destination area
 * @param y Top-left Y coordinate of destination area
 * @param w Width of image data
 * @param h Height of image data
 */

void LoadImage(void *img, int x, int y, int w, int h);

/**
 * Draws a rectangle in the framebuffer, without considering drawing
 * and display environments (i.e. it does so in an absolute way)
 * @param x Top-left X coordinate of area
 * @param y Top-left Y coordinate of area
 * @param w Area width
 * @param h Area height
 * @param r Red (0 - 255)
 * @param g Green (0 - 255)
 * @param b Blue (0 - 255)
 */
 
void DrawFBRect(int x, int y, int w, int h, int r, int g, int b);

/**
 * Set drawing environment
 * @param drawenv Pointer to drawing environment structure
 */

void GsSetDrawEnv(GsDrawEnv *drawenv);

/**
 * Set display environment
 * @param dispenv Pointer to display environment structure
 */

void GsSetDispEnv(GsDispEnv *dispenv);



/* If this flag is set, pixels drawn have MSB set */
#define MASK_SET		1	
/* If this flag is set, pixels aren't drawn over pixels with MSB set */
#define MASK_IGNORE		2
 
 /**
 * Sets masking settings
 * @param flag Bitwise flags
 */
 
void GsSetMasking(unsigned char flag);

/**
 * Return pointer position in linked list
 * @return Pointer position in linked list
 */
 
unsigned int GsListPos(void);

/**
 * Three functions which send data to the control port and to the data port
 */
 
void gpu_ctrl(unsigned int command, unsigned int param);
void gpu_data(unsigned int data);
void gpu_data_ctrl(unsigned int command, unsigned int param);

/**
 * Puts information about a TIM image passed in a buffer in a GsImage structure.
 */

int GsImageFromTim(GsImage *image, void *timdata);

/**
 * Uploads an image described by a GsImage structure to video memory.
 */

void GsUploadImage(GsImage *image);

/**
 * Fills a GsSprite structure with information from an image described
 * by a GsImage structure, then optionally uploads data to video memory.
 * Sprite coordinates are set to 0.
 */
 
int GsSpriteFromImage(GsSprite *sprite, GsImage *image, int do_upload);

/**
 * Checks if the GPU is drawing
 * @return 1 if GPU is drawing, 0 otherwise
 */

int GsIsDrawing(void);

/**
 * Checks if the GPU is working. Alias of GsIsDrawing()
 * @return 1 if GPU is working, 0 otherwise.
 */

int GsIsWorking(void); // Alias of GsIsDrawing()

/**
 * Clear Video RAM
 */
 
void GsClearMem(void);

/**
 * Loads the built-in 8x8 font in Video RAM at the specified coordinates
 *
 *
 * Specifying cx and cy as -1 will not load the black&white CLUT to 
 * video memory. Use GsSetFont() to specify clut x and clut y in that case.
 *
 * The font occupies a space of 16x128 pixels in 16-bit mode,
 * and 64x128 in 4-bit mode.
*  @param fb_x  X coordinate in framebuffer
 * @param fb_y Y coordinate in framebuffer
 * @param cx X coordinate of black/white CLUT in framebuffer
 * @param cy Y coordinate of black/white CLUT in framebuffer
 */
 
void GsLoadFont(int fb_x, int fb_y, int cx, int cy);

/**
 * Prints string using 8x8 font at screen coordinates x, y
 * @param x X coordinate
 * @param y Y coordinate
 * @param fmt format (like *printf())
 * @return Position identifier.
 */

unsigned int GsPrintFont(int x, int y, const char *fmt, ...);

/**
 * Prints string using 8x8 font at screen coordinates x, y
 * Apart from using a variable argument list, this function
 * is identical to GsPrintFont()
 * @param x X coordinate
 * @param y Y coordinate
 * @param fmt format (like *printf())
 * @param ap Variable argument list
 * @return Position identifier.
 */

unsigned int GsVPrintFont(int x, int y, const char *fmt, va_list ap);

/**
 * Change font coordinates without reloading it
 */
 
void GsSetFont(int fb_x, int fb_y, int cx, int cy);

/**
 * This is a function that sets an internal variable
 * flags should be specified as an OR mask
 *
 * to set the wrap and scale attributes, with X scaling factor 2 and Y scaling
 * factor 3, for example:
 *	GsSetFontAttrib(PRFONT_WRAP | PRFONT_SCALE
 *				| PRFONT_SCALEX(2) | PRFONT_SCALEY(3));
 *
 * to remove all attributes (normal printing):
 *
 *	GsSetFontAttrib(0);
 *
 * PRFONT_WRAP can't coexist with PRFONT_CENTER or PRFONT_RIGHT
 * PRFONT_CENTER and PRFONT_RIGHT are justifying attributes and as such
 * they are mutually exclusive - they cannot coexist with each other.
 * Specifying both will give priority to PRFONT_CENTER.	
 *
 * Attribute list:
 *
 * PRFONT_WRAP - Wrap to next row when the text would fall off the screen
 *				Ideal for debug
 * PRFONT_SCALE - Enable font scaling
 * PRFONT_SCALEX(i) - Specifies the factor "i" as the X scaling factor
 *                                   This has the same meaning as a sprite's scaling factor
 * PRFONT_SCALEY(i) - Specify the factor "i" as the Y scaling factor
 *                                   This has the same meaning as a sprite's scaling factor
 * PRFONT_RIGHT - Justifies text to the right
 * PRFONT_CENTER - Justifies text at the center
 * PRFONT_RL(f) - Luminance factor for the red component
 * PRFONT_GL(f) - Luminance factor for the green component
 * PRFONT_BL(f) - Luminance factor for the blue component
*/

void GsSetFontAttrib(unsigned int flags);

/**
 * Sets drawing environment
 * Enables drawing on the display area, disables dithering and disables all masking flags by default
 * @param x Top-left X coordinate of framebuffer area to use for drawing
 * @param y Top-left Y coordinate of framebuffer area to use for drawing
 * @param w  Width of area
 * @param h Height of area
 */

void GsSetDrawEnvSimple(int x, int y, int w, int h);

/**
 * Sets display environment
 * @param x Top-left X coordinate of framebuffer area to display
 * @param y Top-left Y coordinate of framebuffer area to display
 */

void GsSetDispEnvSimple(int x, int y);

/**
   Television video modes.
*/
enum psx_gpu_vmodes
{
	/** NTSC video mode, 60Hz */
	VMODE_NTSC, 
	/** PAL video mode, 50Hz */
	VMODE_PAL
};

/** One scaling unit. */

#define SCALE_ONE		4096

/** One rotation unit. */

#define ROTATE_ONE	4096

// GsSetFontAttrib() Attribute Flags

#define PRFONT_WRAP				1
#define PRFONT_CENTER			2
#define PRFONT_RIGHT				4
#define PRFONT_SCALE				8
#define PRFONT_UNIXLF				16
#define PRFONT_COLOR				32

// These below are not really attributes... but use them as if they were.

unsigned int PRFONT_SCALEX(int i);
unsigned int PRFONT_SCALEY(int i);
unsigned int PRFONT_RL(unsigned char f);
unsigned int PRFONT_GL(unsigned char f);
unsigned int PRFONT_BL(unsigned char f);

// Use this to get the final X and Y positions from the return value
// of GsPrintFont(). Especially useful after wrapping.

#define prfont_get_fx(i)				((short)(i & 0xffff))
#define prfont_get_fy(i)				((short)((i >> 16) & 0xffff))

// Width and height of the screen in the current video mode

/** This global variable reports the current screen width, as set by GsSetVideoMode(). Do not modify. */
extern unsigned short GsScreenW;
/** This global variable reports the current screen height, as set by GsSetVideoMode(). Do not modify. */
extern unsigned short GsScreenH;
/** This global variable reports the current screen color mode (PAL or NTSC), as set by GsSetVideoMode().
     You can use the values in the psx_gpu_vmodes enum to evaluate this. Do not modify. */
extern unsigned char GsScreenM; // Current video mode

/** This global variable reports the width of the current drawing environment. */
extern unsigned short GsCurDrawEnvW;
/** This global variable reports the height of the current drawing environment. */
extern unsigned short GsCurDrawEnvH;

/**
 * Clear the entire drawing area with specified color
 * @param r Red (0 - 255)
 * @param g Green (0 - 255)
 * @param b Blue (0 - 255)
 */

void GsSortCls(int r, int g, int b);

/**
  @attention Macros by their definition do not like being passed invalid values.
*/  

/**
 * Macro to get a texture page number from a coordinate in VRAM
 * @param x X coordinate (0-1023)
 * @param y Y coordinate (0-511)
 */ 

#define gs_get_tpage_num(x,y)		((x/64)+((y/256)*16))

/**
 * Macro to get an horizontal texture offset from a X coordinate in VRAM
 * @param x X coordinate (0-1023)
 */

#define gs_get_tpage_u(x)			(x&0x3f)

/**
 * Macro to get a vertical texture offset from a Y coordinate in VRAM
 * @param y Y coordinate (0-511)
 */

#define gs_get_tpage_v(y)			(y&0xff)

/**
 * This macro converts a RGB 888 color to PlayStation color format.
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 */

#define gs_rgb_to_psx(r, g, b)			((r>>3)|((g>>3)<<5)|((b>>3)<<10))

/**
 * This macro converts a RGB 888 color (a color whose color components have an intensity between 0 and 255)
 * to PlayStation color format, considering an alpha bit. If the alpha bit is non-zero, the STP bit in the color
 * is set and will be considered for PlayStation translucency effects.
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @param a Alpha bit (STP/semitransparency bit)
 */

#define gs_rgba_to_psx(r, g, b, a)		((r>>3)|((g>>3)<<5)|((b>>3)<<10)|(a==0?0:1))

/**
 * This function can rotate a vector about the X, Y and Z axes
 * If you don't want to rotate a vector about an axis, pass 0 as angle to that axis
 *
 * It is correct to pass the same argument to v and n, as calculations are first done in an internal buffer
 * and then stored in the output array
 *
 * @param x_a Number of degrees (0-359) to which the vector should be rotated about the X axis
 * @param y_a Number of degrees (0-359) to which the vector should be rotated about the Y axis
 * @param z_a Number of degrees (0-359) to which the vector should be rotated about the Z axis
 * @param v Pointer to an array of coordinates for the source vector
 * @param n Pointer to destination array for the coordinates of the rotated vector
 */

void GsRotateVector(int x_a, int y_a, int z_a, double *v, double *n);

/**
 * Adds a monochrome polyline to the packet list
 * @param line Pointer to structure for monochrome polyline
 */

void GsSortPolyLine(GsPolyLine *line);

/**
 * Adds a gradated polyline to the packet list
 * @param line Pointer to structure for monochrome line
 */

void GsSortGPolyLine(GsGPolyLine *line);

//void GsSortSimpleMap(GsMap *map);

#endif
