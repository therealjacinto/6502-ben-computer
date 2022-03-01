// __write() / clock()

#include "nes.h"

typedef  unsigned char ubyte;
typedef unsigned short uword;

ubyte
	vruBuf[3+32+3+32+1]; // write up to 32 bytes, clear up to 32 bytes, endMarker

#define OAMBUF ((volatile unsigned char*)0x0200)

// volatile, because vram updates are processed in the NMI
extern __zpage volatile ubyte __vruCON, __nmiSCROLLY, __nmiPPUCTRL;
extern __zpage volatile void* __vruSRC;
extern __zpage volatile uword __nmiCOUNTER;
enum { vruHor=64, vruVer=128, vruEnd=0xff };

typedef unsigned short time_t;
time_t clock() {
	__nmiPPUCTRL|=128;  *PPUCTRL|=128; // enable NMI
	return __nmiCOUNTER;
}

ubyte __cx=0, __cy=1, sy=0; // cursor x/y, screen y scroll pos

        // Writes text to the nametable with the vram update mechanism of the
       // NMI handler. Scrolls up and clears line if necessary. Sets the cursor.
      //
     // The function has to wait for the next NMI if it is called more than
    // once in a frame, if the NMI output buffer is full or if there're
   // control characters in the middle of the text.
  //
 // remark: Only chars 32..126, '\n' and '\r' are supported so far.
//
unsigned int __write(int file,const char *p,unsigned int len) {
	unsigned int n=len;  ubyte bPos;

	__nmiPPUCTRL|=128;  *PPUCTRL|=128; // enable NMI
	while (__vruCON); // wait until NMI has unlocked the vru semaphore
	__vruSRC=vruBuf;  bPos=3;
	while (n) {
		char c=*p++;  --n;  if (32<=c && c<127) vruBuf[bPos++]=c-32; // printable?
		if (!n || '\n'==c || '\r'==c || bPos>=3+32) { // EOT? CR? LF? buffer filled?
			// *** trigger vru request ***
			if (bPos>3) { // buffer filled already?
				uword w;  ubyte r=__cy+(sy>>3);  while (r>=30) r-=30; // calc row
				w=0x2000+__cx+(r<<5);  vruBuf[0]=(w>>8)|vruHor;  vruBuf[1]=w&255; // hi/lo
				vruBuf[2]=bPos-3;  vruBuf[bPos]=vruEnd; // len, end marker
			}
			if ('\n'==c) __cx=32; else if ('\r'==c) __cx=0; else __cx+=bPos-3;
			while (__cx>=32) { // advance cursor?
				__cx-=32;  if (__cy<28) ++__cy; else { // scroll?
					uword w;  ubyte r;  sy+=8;  if (sy>=240) sy-=240;
					__nmiSCROLLY=sy;  r=sy>>3;  r+=29;  while (r>=30) r-=30;
					w=0x2000+(r<<5);  if (3==bPos) bPos=0; // empty buffer? then reset
					vruBuf[bPos++]=(w>>8)|vruHor;  vruBuf[bPos++]=w&255; // clear line
					vruBuf[bPos++]=32;  for (r=0;r<32;++r) vruBuf[bPos++]=0;
					vruBuf[bPos]=vruEnd;
				}
			}
			OAMBUF[252]=(__cy<<3)-1;  OAMBUF[255]=__cx<<3; // place cursor
			if (bPos>3) __vruCON|=128; // lock vru semaphore and trigger NMI vram update
			if (n) { while (__vruCON);  bPos=3; } // more text? NMI-wait, reset bPos
		}
	}

  return len;
}
