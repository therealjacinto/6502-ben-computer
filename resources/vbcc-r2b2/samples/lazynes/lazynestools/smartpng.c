// smartpng - png support functions for smartfx

#include "zlib.h"
#include "smartpng.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// filters
#define PNG_FILTER_VALUE_NONE  0
#define PNG_FILTER_VALUE_SUB   1
#define PNG_FILTER_VALUE_UP    2
#define PNG_FILTER_VALUE_AVG   3
#define PNG_FILTER_VALUE_PAETH 4
#define PNG_FILTER_VALUE_LAST  5

/* These determine if an ancillary chunk's data has been successfully read
 * from the PNG header, or if the application has filled in the corresponding
 * data in the info_struct to be written into the output file.  The values
 * of the PNG_INFO_<chunk> defines should NOT be changed.
 */
#define PNG_INFO_gAMA 0x0001
#define PNG_INFO_sBIT 0x0002
#define PNG_INFO_cHRM 0x0004
#define PNG_INFO_PLTE 0x0008
#define PNG_INFO_tRNS 0x0010
#define PNG_INFO_bKGD 0x0020
#define PNG_INFO_hIST 0x0040
#define PNG_INFO_pHYs 0x0080
#define PNG_INFO_oFFs 0x0100
#define PNG_INFO_tIME 0x0200
#define PNG_INFO_pCAL 0x0400
#define PNG_INFO_sRGB 0x0800   /* GR-P, 0.96a */
#define PNG_INFO_iCCP 0x1000   /* ESR, 1.0.6 */
#define PNG_INFO_sPLT 0x2000   /* ESR, 1.0.6 */
#define PNG_INFO_sCAL 0x4000   /* ESR, 1.0.6 */
#define PNG_INFO_IDAT 0x8000L  /* ESR, 1.0.6 */

/* These describe the color_type field in png_info. */
/* color type masks */
#define PNG_COLOR_MASK_PALETTE    1
#define PNG_COLOR_MASK_COLOR      2
#define PNG_COLOR_MASK_ALPHA      4

/* This is for filter type. PNG 1.0-1.2 only define the single type. */
#define PNG_FILTER_TYPE_BASE      0 /* Single row per-byte filtering */
#define PNG_FILTER_TYPE_DEFAULT   PNG_FILTER_TYPE_BASE

/* color types.  Note that not all combinations are legal */
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB        (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA  (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
/* aliases */
#define PNG_COLOR_TYPE_RGBA  PNG_COLOR_TYPE_RGB_ALPHA
#define PNG_COLOR_TYPE_GA  PNG_COLOR_TYPE_GRAY_ALPHA


 // private
//
int32_t pngGet31(unsigned char* buf) {
	return ((int32_t)(*buf)<<24)+((int32_t)(*(buf+1))<<16)+
		((int32_t)(*(buf+2))<<8)+(int32_t)(*(buf+3));
}

 // private
//
int32_t pngGet16(unsigned char* buf) {
	return ((int32_t)(*buf)<<8)+(int32_t)(*(buf+1));
}

// private
//
int32_t pngGet16X(unsigned char* buf) {
	return ((int32_t)(*(buf+1))<<8)+(int32_t)(*(buf+0));
}

      // private
     // f: filter number
    //  r: pointer to actual row
   //   p: pointer to previous row
  // bpr: bytesPerRow
 //  bpp: bytesPerPixel
//
void pngUnfilterRow(int32_t f, unsigned char* r, unsigned char* p,
	int32_t bpr, int32_t bpp) {
	if (f==PNG_FILTER_VALUE_NONE) {
		; // no filter

	} else if (f==PNG_FILTER_VALUE_SUB) {
		int32_t i, e=bpr;  unsigned char* rp=r+bpp,* lp=r;
		for (i=bpp;i<e;i+=1)
			{ *rp=(unsigned char)(((int32_t)(*rp)+(int32_t)(*lp++))&0xff);  rp+=1; }

	} else if (f==PNG_FILTER_VALUE_UP) {
		int32_t i, e=bpr;  unsigned char* rp=r,* pp=p;
		for (i=0;i<e;i+=1)
			{ *rp=(unsigned char)(((int32_t)(*rp)+(int32_t)(*pp++))&0xff);  rp+=1; }

	} else if (f==PNG_FILTER_VALUE_AVG) {
		int32_t i, e=bpr-bpp;  unsigned char* rp=r,* pp=p,* lp=r;
		for (i=0;i<bpp;i+=1)
			{ *rp=(unsigned char)(((int32_t)(*rp)+((int32_t)(*pp++)/2))&0xff);  rp+=1; }
		for (i=0;i<e;i+=1)
			{ *rp=(unsigned char)(((int32_t)(*rp)+(int32_t)(*pp++ +*lp++)/2)&0xff); rp+=1;}

	} else if (f==PNG_FILTER_VALUE_PAETH) {
		int32_t i, e=bpr-bpp;  unsigned char* rp=r,* pp=p,* lp=r,* cp=p;
		for (i=0;i<bpp;i+=1)
			{ *rp=(unsigned char)(((int32_t)(*rp)+(int32_t)(*pp++))&0xff);  rp+=1; }
		for (i=0;i<e;i+=1) {
			int32_t a=*lp++, b=*pp++, c=*cp++, pa, pb, pc, p;  p=b-c;  pc=a-c;
			pa=abs(p);  pb=abs(pc);  pc=abs(p+pc);
			p=(pa<=pb&&pa<=pc)?a:(pb<=pc)?b:c;
			*rp=(unsigned char)(((int32_t)(*rp)+p)&0xff);  rp+=1;
		}

	}
}

 // private
//
int32_t pngChunkSize(void* b) {
	unsigned char* pc=b;  return (pc[0]<<24)+(pc[1]<<16)+(pc[2]<<8)+pc[3];
}

 // private
//
typedef struct tagPNGBase {
	PNGInfo* i;
	z_stream s;
	unsigned char
		paletteStorage[pngMaxPalEntries*3],
		alphaStorage[pngMaxPalEntries];
	int32_t
		lPos, // position in l1-line-buffer
		yPos, // position in image-buffer
		skip, // first call status
		bitDepth,
		colorType,
		compressionType,
		filterType,
		interlaceType,
		pixelDepth;
} PNGBase;

PNGBase pngBase; // private

// *** public functions ***

 // allocate private info
//
PNG pngInit(PNGInfo* i) {
	pngBase.skip=3+4;
	if (pngBase.i=i) {
		pngBase.i->alpha=pngBase.i->color=NULL;
		//pngBase.transRGB=0;  i->transRGB=&pngBase.transRGB;
		return (PNG)&pngBase;
	} else return NULL;
}

 // free private info
//
void pngFree(PNG p) {
}

  // read 8byte header of png file and prepare png_info for IDAT chunk reading
 // result: NULL if no PNG-header
//
void* pngGetHead(PNG p, void* b) {
	unsigned char* buf=b;
	if (buf[0]!=0x89 || buf[1]!='P' || buf[2]!='N' || buf[3]!='G'
		|| buf[4]!=0x0d || buf[5]!=0x0a || buf[6]!=0x1a || buf[7]!=0x0a)
		return NULL;
	return buf+8;
}

 // process unknown chunk
//
void* pngGetXXXX(PNG p, void* b) {
	unsigned char* buf=b;  uint32_t length=pngChunkSize(b);
	return 8+buf+length+4;
}

  // read and check the IDHR chunk
 // result: increased buffer pointer or NULL on error
//
void* pngGetIHDR(PNG p, void* b) {
	PNGBase* png=p;  unsigned char* buf=b;  uint32_t length=pngChunkSize(b);
  buf+=8;  if (length!=13) return NULL;
  int32_t channels=0;

	png->i->width=pngGet31(buf);
	png->i->height=pngGet31(buf+4);
	png->bitDepth=buf[8];
	png->colorType=buf[9];
	png->compressionType=buf[10];
	png->filterType=buf[11];
	png->interlaceType=buf[12];

	// find number of channels
	//printf("debug-x:%d\n",png->colorType);
	switch (png->colorType) {
		case PNG_COLOR_TYPE_GRAY: {
			int32_t n, c, v;
			channels=1;
			//printf("grey:%d\n",1<<png->bitDepth);
			png->i->color=png->paletteStorage;  c=png->i->colors=1<<png->bitDepth;
			for (n=0,v=255/(c-1);n<c;n+=1) { png->i->color[n*3+0]
				=png->i->color[n*3+1]=png->i->color[n*3+2]=n*v;
			}
		} break;
		case PNG_COLOR_TYPE_PALETTE: channels=1; break;
		case PNG_COLOR_TYPE_RGB: channels=3; break;
		case PNG_COLOR_TYPE_GRAY_ALPHA: channels=2; break;
		case PNG_COLOR_TYPE_RGB_ALPHA: channels=4; break;
	}
	// set up other useful info
	png->pixelDepth=png->bitDepth*channels;
	//png->i->rowBytes=PNG_ROWBYTES(pi->pixel_depth,pi->width);
//printf("debug:FILTER:%d\n",pi->filter_type);
//printf("debug:COMP:%d\n",pi->compression_type);
	/*
printf("debug:COL:%d\n",pi->color_type);
printf("debug:CHAN:%d\n",pi->channels);
printf("debug:P-DEPTH:%d\n",pi->pixel_depth);
printf("debug:ROWBYTES:%d\n",pi->rowbytes);
	*/
	//printf("wh:%d %d\n",pi->width,pi->height);
	buf+=length+4; // add chunk size and CRC
	return buf;
}

 // process palette chunk
//
void* pngGetPLTE(PNG p, void* b) {
	PNGBase* png=p;  unsigned char* buf=b;  int32_t length=pngChunkSize(b);  int32_t n;
	buf+=8;  if (length>256*3 || length%3) return NULL; // assert
	//printf("DEBUG:PLTE----2\n");
	//pi->valid|=PNG_INFO_PLTE;
	png->i->colors=length/3;
	png->i->color=(void*)&png->paletteStorage;
	if (png->i->alpha==NULL) {
		png->i->alpha=png->alphaStorage;
		for (n=0;n<png->i->colors;n+=1) png->i->alpha[n]=0xff;
	}
	for (n=0;n<png->i->colors*3;n+=1) png->i->color[n]=*buf++;
	//for (n=0;n<png->i->paletteEntries;n+=1) {
	//	int32_t i;  for (i=0;i<3;i+=1) printf("%02x",png->i->palette[n*3+i]);
	//	printf("\n");
	//}
	return buf+4;
}

 //
//
void* pngGettRNS(PNG p, void* b) {
	PNGBase* png=p;
	unsigned char* buf=b;  uint32_t length=pngChunkSize(b);  int32_t n;
	//printf("debug-tRNS:%d %d\n",length,png->i->colors);
	buf+=8;  if (length>256 || length>png->i->colors) return NULL; // assert
	if (png->i->alpha==NULL) {
		png->i->alpha=png->alphaStorage;
		for (n=0;n<png->i->colors;n+=1) png->i->alpha[n]=0xff;
	}
	png->i->alpha=(void*)&png->alphaStorage;
	for (n=0;n<length;n+=1) png->i->alpha[n]=*buf++;
	return buf+4;
}

voidpf MyZalloc(voidpf opaque, uInt items, uInt size) {
	return NULL;//malloc(num_bytes);
}

void MyZFree(voidpf png_ptr, voidpf ptr) {
	free (ptr);
}

/*
tRNS: 0==fully transparent, 255==fully opaque
-> just check for entries with 0x00 and mask them!
*/

   // ps: pixel size
  // bpr<0: bv*rgb/256 or -1 to disable (means a 2nd pixel is placed at y+bpr/2)
 // bpp<0: swapped byte order (needed in SHORT data type for 16bit textures)
//
void* pngGetIDAT(PNG p, void* b, void* dPtr, void* mPtr, void* wPtr, int32_t ps,
	int32_t bpr, int32_t bpp, uint32_t rm, uint32_t gm, uint32_t bm,
	uint32_t am, int32_t sbRGB, int32_t mbRGB, int32_t useMaskRGB, int32_t mapGreyRGB) {
	PNGBase* png=p;  unsigned char* buf=b,* l0,* l1;  uint32_t r8, g8, b8;
	int32_t
		bc=0, bv=-1, // (b)yte(c)hange, (b)lending(v)alue (or -1)
		cPos,       // actual working position in chunk
		fullChunk=pngChunkSize(b), // data bytes in IDAT chunk
		maskPixel=0, rgbPixelDepth, rgbBytes, lineBytes, r, n, x, y, c,
		rs=0, gs=0, bs=0, rd=0, gd=0, bd=0; // rgba-(s)rc/(d)st-shift
	short pbw, pbl; // pixelByteWith, pixelByteLine
	char bTab[8][8]={{1},{1,0},{1,1,0},{0,1,1,0},{0,1,1,1,0},{0,1,1,1,0,0},
		{0,1,1,1,1,0,0},{0,0,1,1,1,1,0,0}};


	/*for (n=0;n<8;n++) {
		char* pb=bTab[n];
		printf("n:%d: ",n);
		for (c=0;c<8;++c) printf("%d ",pb[c]);//bTab[n][c]);
		printf("\n");
	}
	exit(0);*/

	if (ps<1) ps=1;  // assert  (p)ixel(s)ize in pixels...
	if (bpp<0) { bpp=-bpp;  bc=1; }    // enable byteswap?
	if (bpr<0) { bpr=-bpr;  bv=192; } // enable scanline halfscan mode?
	pbw=ps*bpp;  pbl=bpr/ps;

	// calculate RGB destination shift and source shift values based on RGB mask
	for (c=0;c<32;c+=1) if (rm&(1<<c)) { rs=c;  break; }
	for (c=rs;c<33;c+=1) if (c==32 || (rm&(1<<c))==0) { rd=8-(c-rs);  break; }
	for (c=0;c<32;c+=1) if (gm&(1<<c)) { gs=c;  break; }
	for (c=gs;c<33;c+=1) if (c==32 || (gm&(1<<c))==0) { gd=8-(c-gs);  break; }
	for (c=0;c<32;c+=1) if (bm&(1<<c)) { bs=c;  break; }
	for (c=bs;c<33;c+=1) if (c==32 || (bm&(1<<c))==0) { bd=8-(c-bs);  break; }
	//for (c=0;c<32;c+=1) if (am&(1<<c)) { as=c;  break; }
	//for (c=as;c<33;c+=1) if (c==32 || (am&(1<<c))==0) { ad=8-(c-as);  break; }

	//printf("rm:%08x gm:%08x bm:%08x am:%08x\n",rm,gm,bm,am);
	//printf("rs:%8ld gs:%8ld bs:%8ld as:%8ld\n",rs,gs,bs,0);
	//printf("rd:%8ld gd:%8ld bd:%8ld ad:%8ld\n",rd,gd,bd,0);
	//printf("pbw:%ld pbl:%ld\n",pbw,pbl);

	// calculate masking pixel value
	if (useMaskRGB!=-1) {
		r8=(((useMaskRGB&0x00ff0000)>>16));
		g8=(((useMaskRGB&0x0000ff00)>>8));
		b8=(((useMaskRGB&0x000000ff)));
		maskPixel=((r8>>rd)<<rs)|((g8>>gd)<<gs)|((b8>>bd)<<bs);
	}

	// configure parser
	if (png->colorType==PNG_COLOR_TYPE_PALETTE
		|| png->colorType==PNG_COLOR_TYPE_GRAY)
		{ rgbPixelDepth=png->pixelDepth;  rgbBytes=1; }
		else { rgbPixelDepth=8;  rgbBytes=3; }
	// reset filter line buffers
	if (png->i->width>maxPNGWidth) png->i->width=maxPNGWidth; // overflow check
	lineBytes=png->i->width*rgbBytes*rgbPixelDepth/8+1;

	buf+=8; // skip IDAT header

	// that should skip comptype/compflags and stuff?
	if (png->skip>4) { // >4 means initional call
		png->yPos=0;  png->skip-=3;  png->lPos=0;
		// allocate inflate state
		png->s.zalloc=Z_NULL;  png->s.zfree=Z_NULL;  png->s.opaque=Z_NULL;
		png->s.next_in=Z_NULL;  png->s.avail_in=0;
		r=inflateInit(&png->s);
		l0=wPtr;  for (n=0;n<lineBytes;n+=1) l0[n]=0; // clear for filter-action
	}
	// concat IDAT-content-loop to fill line buffer l1
	cPos=0;  while (cPos<fullChunk) {
		l0=wPtr;  l1=wPtr+maxPNGWidth*4+1;
		n=fullChunk-cPos; // amount to inflate
		png->s.next_in=buf;  png->s.avail_in=n;
		png->s.next_out=l1+png->lPos;  png->s.avail_out=lineBytes-png->lPos;
		r=inflate(&png->s,Z_NO_FLUSH);
		buf+=n-png->s.avail_in;  cPos+=n-png->s.avail_in;
		png->lPos+=lineBytes-png->lPos-png->s.avail_out;

		if (png->lPos==lineBytes) { // line filled?
			unsigned char* dat,* mas, f;  uint32_t dv, // dv:shifted RGB-value
				mv, sv=0; // mv: mask-RGB-value sv:blended RGB-value
			y=png->yPos;  png->yPos=0;  x=-1;  f=l1[0];  l1[0]=0;
			//printf("w:%d lb:%d\n",png->i->width,lineBytes);
			if (f) pngUnfilterRow(f,&l1[1],&l0[1],lineBytes-1,rgbBytes);
			for (x=0;x<lineBytes;x+=1) l0[x]=l1[x]; // remember row
			//for (x=0;x<lineBytes;x+=1) printf("%02x",l1[x]); printf("\n");
			l1+=1;  x=0; // skip filter and reset for processing
			dat=dPtr+y*bpr+bpp*x;  if (mPtr) mas=mPtr+y*bpr+bpp*x; else mas=NULL;
			while (x<png->i->width) {
				for (n=0;n<8;n+=rgbPixelDepth) {
					char* pb=bTab[(ps-1)&7];  short i=0, c=-1;  while (i<rgbBytes) {
						if (png->colorType==PNG_COLOR_TYPE_RGB
							|| png->colorType==PNG_COLOR_TYPE_RGB_ALPHA) { // RGB8 based
							if (i==0) r8=l1[0];  else if (i==1) g8=l1[0];
								else if (i==2) b8=l1[0];
						} else if (png->colorType==PNG_COLOR_TYPE_PALETTE
							|| png->colorType==PNG_COLOR_TYPE_GRAY) { // PLTE based
							unsigned char* pc, v=l1[0];  v>>=8-png->pixelDepth-n;
							c=v&((1<<png->pixelDepth)-1);
							pc=(void*)&png->i->color[3*c];  r8=*pc++;  g8=*pc++;  b8=*pc;
						} else r8=g8=b8=0xff;
						i+=1;  if (n+rgbPixelDepth>=8) l1+=1;
					}
					if (mapGreyRGB>0 && r8==g8 && r8==b8) {
						r8=(r8*((mapGreyRGB&0xff0000)>>16))>>8;
						g8=(g8*((mapGreyRGB&0x00ff00)>>8))>>8;
						b8=(b8*((mapGreyRGB&0x0000ff)))>>8;
					}
					dv=((r8>>rd)<<rs)|((g8>>gd)<<gs)|((b8>>bd)<<bs);
					if (bv!=-1) sv=(((r8*bv/256)>>rd)<<rs)|(((g8*bv/256)>>gd)<<gs)
						|(((b8*bv/256)>>bd)<<bs);

					if (useMaskRGB!=-1 && (dv==maskPixel ||
						(png->colorType==PNG_COLOR_TYPE_PALETTE
						|| png->colorType==PNG_COLOR_TYPE_GRAY) && png->i->alpha
						&& c!=-1 && png->i->alpha[c]==0)) { sv=dv=sbRGB;  mv=mbRGB; }
						else { mv=0;  dv|=am;  sv|=am; }
					if (bc) dv=(dv>>24)|((dv<<8)&0xff0000)|((dv>>8)&0xff00)|(dv<<24);
					if (bc && bv!=-1)
						sv=(sv>>24)|((sv<<8)&0xff0000)|((sv>>8)&0xff00)|(sv<<24);
					for (c=0;c<bpp;c+=1) { // set all bytes of one pixel here
						int32_t u, v, i;  for (i=v=0;v<bpr;v+=pbl,++i) for (u=0;u<pbw;u+=bpp)
							if (bv==-1 || pb[i&7]) // standard or blending effect?
							// if (v!=bpr-pbl || bv==-1) // standard or blending halfscan effect?
							{ dat[u+v]=dv>>24;  if (mas) mas[u+v]=mv>>24; }
							else { dat[u+v]=sv>>24;  if (mas) mas[u+v]=mv>>24; }
						dat+=1;  sv<<=8;  dv<<=8;  if (mas) { mas+=1;  mv<<=8; }
					}
					x+=1;  dat+=pbw-bpp;  if (mas) mas+=pbw-bpp;
				}
			}
			//for (x=0;x<lineBytes*3;x+=3) printf("%02x%02x%02x",debug[x],debug[x+1],debug[x+2]); printf("\n");
			y+=1;  png->yPos=y;  png->lPos=0;
		}
		/*
		printf("debug-bytes: %x %x %x %x\n",buf[-4],buf[-3],buf[-2],buf[-1]);
		printf("debug-bytes: %x %x %x %x\n",buf[0],buf[1],buf[2],buf[3]);
		printf("debug-bytes: %x %x %x %x\n",buf[4],buf[5],buf[6],buf[7]);
		printf("debug-bytes: %x %x %x %x\n",buf[8],buf[9],buf[10],buf[11]);
		*/
		if (r!=Z_OK) break;
	}
	buf+=4; // that should be chunk CRC, needs to be checked
	if (r!=Z_OK) { inflateEnd(&png->s); }
	if (r==Z_OK || r==Z_STREAM_END) return buf; else return NULL;
}

	/*
	pd=1: maxshifts=8/1=8-1 - shifts:=(maxshifts-n)*pd

	pd=2: maxshifts=8/2=4-1 - shifts:=(maxshifts-n)*pd

	0:3 -> 6 n=0 maxshifts=8-2=6 shifts:=ms-n
	1:2 -> 4 n=2
	2:1 -> 2 n=4
	3:0 -> 0 n=6

	4: 8/4=2 shifts:=(maxshifts-n)*pd

	0:1 -> 4 n=0 maxshifts=8-4=4 shifts:=ms-n
	1:0 -> 0 n=4

	8: 8/8=1

	0:0 -> 0
	*/
