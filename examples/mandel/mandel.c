/*
 * Simple fractal generator for the PlayStation
 *
 * by Giuseppe Gatta
 * Placed in the Public Domain.
 *
 * Consider enabling compiler optimizations, for instance this code is much faster
 * if you add -O3 to the gcc command line.
 *
 * The fractals are rendered at a 160x120 resolution in an array in memory,
 * then this array is uploaded to video memory and handled as 8-bit image data.
 * We render at 160x120 to speed rendering up because the PlayStation is a slow machine
 * for doing this kind of thing.
 *
 * A sprite primitive is used to display the image data on the screen, and the primitive
 * is scaled twice horizontally and vertically to cover the 320x240 screen with a 160x120
 * image.
 *
 * The fractal drawing code by default also exploits the simmetry of the Mandrelbot
 * and Tricorn sets to further speed rendering up (2x). To have a real rendering of
 * the Burning Ship fractal, though, you must disable simmetry because that fractal
 * does not have this property.
 *
 * This is the description of the controls of this program:
 * 
 * SELECT - Toggle palette (Red->Green->Blue->Yellow->Violet->Red->...)
 * START - Render fractal
 * X - Toggle type of fractal to render (Mandelbrot->Tricorn->Burning Ship->Mandelbrot->...)
 * O - Toggle simmetry (on the screen as 'S')
 * LEFT - Decrease multibrot power (displayed on the screen as 'M')
 * RIGHT - Increase multibrot power
 * UP - Decrease number of maximum iterations (on the screen as 'I')
 * DOWN - Increase number of maximum iterations
 */

#include <psx.h>
#include <stdio.h>

unsigned char fractal_image_data[120][160];
unsigned int prim_list[0x4000];
unsigned short clut_buf[256];

volatile int frac_x=0, frac_y=0;

enum
{
	FRAC_MANDELBROT, // Normal mandelbrot
	FRAC_TRICORN,
	FRAC_BSHIP,
	FRAC_TF,

	FRAC_NOSIMM = 0x200,
};

/*
 * Code based on http://reocities.com/CapeCanaveral/5003/mandel.htm
 */

void mandrelbrot_int(int width, int height, int maxI, int type, int M)
{
	#define FIXSIZE 25
	#define mul(a,b) ((((long long)a)*(b))>>FIXSIZE)
	#define fixpt(a) ((int)(((a)*(1<<FIXSIZE))))
	#define integer(a) (((a)+(1<<(FIXSIZE-1)))>>FIXSIZE)

	int x0,y0,x1,y1,p,q,xn;
	double xmin=-2.5,ymin=-1.5,xmax=1.5,ymax=1.5,xs,ys;
	int i,x,y,t;
	int m;

	xs=(xmax-xmin)/width;
	ys=(ymax-ymin)/height;

	t = type & 0xff;

	for (y=0;y<((type&FRAC_NOSIMM)?height:(height/2));y++) 
	{
		for (x=0;x<width;x++) 
		{                        
			p=fixpt(xmin+x*xs); // c_re
			q=fixpt(ymin+y*ys);
			xn=0;
			x0=0;
			y0=0;
			i=0;

			while ((mul(xn,xn)+mul(y0,y0))<fixpt(4) && ++i<maxI)
			{
				switch(t)
				{
					case FRAC_BSHIP:
						if(x0<0)x0=-x0;
						if(y0<0)y0=-y0;
					break;
					case FRAC_TRICORN:
						y0=-y0;	
					break;
				}

				x1=x0;
				y1=y0;

				for(m=1;m<M;m++)
				{
					xn=mul(x0,x1) - mul(y0,y1);           
					y0=mul(y0,x1) + mul(x0,y1);
					x0=xn;
				}

				x0=xn;
				x0+=p;
				y0+=q;
			}

			if (i==maxI) i=1;
			fractal_image_data[y][x] = (i*256)/maxI;

			if(!(type & FRAC_NOSIMM))
				fractal_image_data[(height-1)-y][x] = (i*256)/maxI;
		}	
	}
}

volatile int display_is_old = 1;
volatile int time_counter = 0;

void prog_vblank_handler()
{
	display_is_old = 1;
	time_counter++;
}

int frac_type = FRAC_MANDELBROT;
unsigned int frac_maxI = 64;
int frac_M=2;

const char *frac_type_string[] =
	{ "Mandelbrot",
	   "Tricorn",
           "BurningShip"};

GsSprite frac_sprite;

int main()
{
	int x, dbuf=0;

	int waspal=0;
	int wastype=0;
	int wasrender=0;
	int wassimm=0;
	int wasMm=0,wasMp=0, wasIm=0, wasIp=0;
	int rendering_time = -1;

	unsigned short padbuf;

	PSX_InitEx(PSX_INIT_NOBIOS);

	GsInit();
	GsSetList(prim_list);
	GsClearMem();
	GsSetVideoMode(320, 240, EXAMPLES_VMODE);
	GsLoadFont(768, 0, 768, 256);
	SetVBlankHandler(prog_vblank_handler);

// Fill & upload CLUTs
	
// Red CLUT
	for(x = 0; x < 256; x++)
		clut_buf[x] = ((x*4)>255?255:x*4)>>3;

	LoadImage(clut_buf, 512, 256, 256, 1);
	while(GsIsDrawing());

// Green CLUT
	for(x = 0; x < 256; x++)
		clut_buf[x] = (((x*4)>255?255:x*4)>>3)<<5;

	LoadImage(clut_buf, 512, 257, 256, 1);
	while(GsIsDrawing());

// Blue CLUT
	for(x = 0; x < 256; x++)
		clut_buf[x] = (((x*4)>255?255:x*4)>>3)<<10;

	LoadImage(clut_buf, 512, 258, 256, 1);	
	while(GsIsDrawing());


// Yellow CLUT
	for(x = 0; x < 256; x++)
	{
		clut_buf[x] = ((x*4)>255?255:x*4)>>3;
		clut_buf[x]|= clut_buf[x]<<5;
	}

	LoadImage(clut_buf, 512, 259, 256, 1);
	while(GsIsDrawing());

// Violet CLUT
	for(x = 0; x < 256; x++)
	{
		clut_buf[x] = ((x*4)>255?255:x*4)>>3;
		clut_buf[x]|= clut_buf[x]<<10;
	}

	LoadImage(clut_buf, 512, 260, 256, 1);
	while(GsIsDrawing());

	frac_sprite.tpage = 5;
	frac_sprite.attribute = COLORMODE(COLORMODE_8BPP);
	frac_sprite.u = 0;
	frac_sprite.v = 0;
	frac_sprite.cx = 512;
	frac_sprite.cy = 256;
	frac_sprite.x = 0;
	frac_sprite.y = 0;
	frac_sprite.w = 160;
	frac_sprite.h = 120;
	frac_sprite.r = frac_sprite.g = frac_sprite.b = NORMAL_LUMINOSITY;
	frac_sprite.scalex = SCALE_ONE*2;
	frac_sprite.scaley = SCALE_ONE*2;

	time_counter = 0;
	mandrelbrot_int(160, 120, frac_maxI, frac_type, frac_M);
	rendering_time = time_counter * (GsScreenM==VMODE_NTSC?17:20);
	LoadImage(fractal_image_data, 320, 0, 80, 120);
	while(GsIsDrawing());

	while(1)
	{
		if(display_is_old)
		{
			dbuf=!dbuf;
			GsSetDispEnvSimple(0, dbuf?0:256);
			GsSetDrawEnvSimple(0, dbuf?256:0, 320, 240);

			GsSortCls(0,0,0);
			GsSortSprite(&frac_sprite);

			GsPrintFont(0, 0, "Type: %s", frac_type_string[(frac_type&0xff)]);
			GsPrintFont(0, 8, "I=%d, M=%d, S=%c", frac_maxI, frac_M, 
				frac_type&FRAC_NOSIMM?'N':'Y');
			GsPrintFont(0, 16, "Time: %d.%03d", rendering_time / 1000, rendering_time % 1000);

			GsDrawList();
			while(GsIsDrawing());

			PSX_ReadPad(&padbuf, NULL);

			if((padbuf & PAD_SELECT) && !waspal)
			{
				frac_sprite.cy++;
				if(frac_sprite.cy>260)
					frac_sprite.cy = 256;
				
				waspal=1;
			}

			if((padbuf & PAD_CROSS) && !wastype)
			{
				frac_type++;
				if((frac_type & 0xff) >= FRAC_TF)
					frac_type&=~0xff;

				wastype=1;
			}

			if((padbuf & PAD_START) && !wasrender)
			{
				GsPrintFont(0, 224, "Rendering...");
				GsDrawList();
				while(GsIsDrawing());

				dbuf=!dbuf;
				GsSetDispEnvSimple(0, dbuf?0:256);
				GsSetDrawEnvSimple(0, dbuf?256:0, 320, 240);
				
				time_counter = 0;
				mandrelbrot_int(160, 120, frac_maxI, frac_type, frac_M);
				rendering_time = time_counter * (GsScreenM==VMODE_NTSC?17:20);
				LoadImage(fractal_image_data, 320, 0, 80, 120);
				while(GsIsDrawing());

				wasrender=1;
			}

			if((padbuf & PAD_CIRCLE) && !wassimm)
			{
				frac_type ^= FRAC_NOSIMM;
				wassimm=1;
			}

			if((padbuf & PAD_LEFT) && !wasMm)
			{
				if(frac_M>2)frac_M--;
				wasMm=1;
			}

			if((padbuf & PAD_RIGHT) && !wasMp)
			{
				frac_M++;
				wasMp=1;
			}

			if((padbuf & PAD_UP) && !wasIm)
			{
				if(frac_maxI>1)frac_maxI--;
				wasIm=1;
			}

			if((padbuf & PAD_DOWN) && !wasIp)
			{
				frac_maxI++;
				wasIp=1;
			}

			if(!(padbuf & PAD_SELECT))
				waspal=0;

			if(!(padbuf & PAD_CROSS))
				wastype=0;

			if(!(padbuf & PAD_START))
				wasrender=0;

			if(!(padbuf & PAD_CIRCLE))
				wassimm=0;

			if(!(padbuf & PAD_LEFT))
				wasMm=0;
			
			if(!(padbuf & PAD_RIGHT))
				wasMp=0;

			if(!(padbuf & PAD_UP))
				wasIm=0;
			
			if(!(padbuf & PAD_DOWN))
				wasIp=0;

			display_is_old=0;
		}
	}

	return 0;
}
