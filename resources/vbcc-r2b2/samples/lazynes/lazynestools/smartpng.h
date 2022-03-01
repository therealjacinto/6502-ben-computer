// smartpng.h - ultra compact PNG file format decoder, 'Lazycow 1998

// remarks:
// `- needs InitInflate() and inflate() from zlib
// `- handles 1,2,4,8bit palette based and 24bit RGB true color, 1,2,4,8bit
//    greyscale, filters, compression, multiple IDAT-chunks, tRNS-chunks
// `- not supported, yet: 16bit Grey, 48bit RGB, alpha, interlacing, CRC-check

#include <stdint.h>

#define pngMaxPalEntries 256
#define maxPNGWidth 4096

// smartPNG opaque object
typedef void* PNG;

 // will be filled with data by pngGetIHDR(), pngGetPLTE, pngGettRNS()
//
typedef struct tagPNGInfo {
	int32_t
		size,      // size of PNGInfo, has to be set to sizeof(PNGInfo)
		width,    // will be filled by pngGetIHDR()
		height,  // will be filled by pngGetIHDR()
		colors, // number of colors in PLTE-chunk
		* transRGB;  // pointer to transparent color (default: #000000)
	unsigned char // (will be filled by pngInit())
		* color,   // points to an array of RGB-colors, sized colors*3
		* alpha;  // points to an array of alpha-values, sized colors*1
} PNGInfo;

// utility function
int32_t pngGet31(unsigned char* buf);

   // inits the PNG file parser
  // i: PNGInfo structure
 // result: smartPNG handle or NULL on failure
//
PNG pngInit(PNGInfo* i);

  // frees all allocates ressources
 // p: smartPNG handle
//
void pngFree(PNG p);

void* pngGetHead(PNG p, void* b);
void* pngGetIHDR(PNG p, void* b);
void* pngGetPLTE(PNG p, void* b);
void* pngGettRNS(PNG p, void* b);
void* pngGetXXXX(PNG p, void* b);

            // p: smartPNG handle
           // b: pointer to buffer position of IDAT-chunk
          // dPtr: pointer to destination data
         // mPtr: pointer to destination mask (can be NULL)
        // wPtr: pointer to work buffer (has to be sized maxPNGWidth*2+2)
       // ps: pixelSize in pixels
      // bpr: bytesPerRow of destination data/mask / bpr<0 means: halfscan-mode
     // bpp: bytesPerPixel (1..4) <0 means byteswap
    // rm, gm, bm, am: RGBA mask - e.g. 0xff000, 0x00ff00, 0x0000ff, 0x000000
   // sbRGB/mbRGB: shape/mask background RGB color
  // mapGreyRGB: color value that will be multiplied to grey values or -1
 // result: pointer to buffer position after that IDAT-chunk
//
void* pngGetIDAT(PNG p, void* b, void* dPtr, void* mPtr, void* wPtr, int32_t ps,
	int32_t bpr, int32_t bpp, uint32_t rm, uint32_t gm, uint32_t bm,
	uint32_t am, int32_t sbRGB, int32_t mbRGB,
	int32_t useMaskRGB, int32_t mapGreyRGB);
