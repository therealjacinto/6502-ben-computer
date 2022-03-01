   //
  // png2spr - Q&D PNG to sprite converter, C64/NES/Amiga, 'Lazycow 2013
 // disclaimer: this is C code, not a C tutorial
//

// gcc -I../smartfx -Wno-parentheses -o png2spr png2spr.c ../smartfx/smartpng.c -lz -L../libz-1.2.7

// V1.0: [01.07.2013] c64 mode for powerglove
// V1.1: [17.02.2015] mig export support
// V1.2: [17.03.2016] bug fixed in modulo calculation
// V1.3: [14.06.2016] nes mode
// V1.4: [08.06.2019] -u for amiga mode writes col file
// V1.5: [20.06.2020] -t -b for adjusting start offsets, first public version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smartpng.h"

enum {
	maxPaletteEntries=1+4*3,
	maxNESPalEntries=1+4*3,
	maxC64PalEntries=3,
	maxMetaSprites=256,
	maxMetaSpriteTabLen=maxMetaSprites*12*4 // max size 12 segments -> 24x32 for now
};

short
	metaSprOff[maxMetaSprites]; // offsets into metaSprTab
unsigned char
	metaSprTab[maxMetaSpriteTabLen],
//	metaSprLo[maxMetaSprites],  // low-byte of offsets into metaSprTab...
//	metaSprHi[maxMetaSprites], // high-byte of offsets into metaSprTab...
	remapTab[maxMetaSprites],
	colTab[maxPaletteEntries];
signed char
	metaYFac[maxMetaSprites],
	metaXOff[maxMetaSprites],
	metaYOff[maxMetaSprites];


enum { //pixelPerRow=24*8, spritesPerRow=8,
	overlayOff=1,
	};

 //
//
char* pngChunkName(void* b) { // not thread-safe, yet!
	static char n[5];  char* pc=b;
	n[0]=pc[4];  n[1]=pc[5];  n[2]=pc[6];  n[3]=pc[7];  n[4]='\0';
	return n;
}

 //
//
unsigned char GetPNGIndex(PNGInfo* p,
	unsigned char r, unsigned char g, unsigned char b) {
	long n;
	for (n=0;n<p->colors;++n) {
		if (p->color[n*3+0]==r
			&& p->color[n*3+1]==g
			&& p->color[n*3+2]==b
			) return n;
	}
	return 0;
}


 // get C64 index of RGB tripple
//
unsigned char GetIndex(PNGInfo* p,
	unsigned char r, unsigned char g, unsigned char b) {
	return GetPNGIndex(p,r,g,b);
}


unsigned char ConvIndex(unsigned char i) {
	switch (i) {
		// black?
		case 13: case 14: case 15: case 46: case 47: case 62: case 63: return 29;
		case 32: return 48; // white?
		default: return i; // others
	}
}

 //
//
int GetIV(char** p) {
	int v=0, n=0;  while ('0'<=**p && **p<='9' || '-'==**p) {
		if ('-'==**p) n=1; else { v*=10;  v+=**p-'0'; }
		++*p;
	}
	if (n) v=-v; // negative flag set?
	if (**p) ++*p; // skip ","
	return v;
}

 //
//
int GetI(char** p) {
	int v=0;
	while (' '==**p) ++*p;
	while ('0'<=**p && **p<='9') { v*=10;  v+=**p-'0';  ++*p; }
	return v;
}

void* nxPutS31(void* p, long v)
	{ unsigned char* b=p;  b[3]=v&255;  b[2]=(v>>8)&255;  b[1]=(v>>16)&255;
	b[0]=(v>>24)&255;  b+=4;  return b; }
void* nxPutU16(void* p, unsigned short v)
	{ unsigned char* b=p;  b[1]=v&255;  b[0]=(v>>8)&255;  b+=2;  return b; }

enum { cmSprites=256, maxSprBW=4, maxSprH=32, maxSprD=8, maxBagSize=16384 };

 //
//
int main(int argc, char** argv) {
	FILE* file;  char* name0=NULL,* error=NULL,* command=NULL,
		sprName[256], segName[256], tabName[256], lofName[256], remName[256],
		offName[256], palName[256], colName[256], numName[256], baseName[256];
	long bgc=0, mc1=0, mc2=0, olc=0;
	int l, cSprites=0, mode=0, a1, a3, mig=0, planes=0, cbf=8, header=0,
		overlaySupport=0,
		autoAdjust=0, detectDoubles=0, remapTable=0, nes=0, cPos=0, skip=0,
		utility=0, baseOffset=0, tileOffset=0,
		rw=24, rh=21,  // raster size
		iw=16, ih=16; // inner size

	memset(metaXOff,0,sizeof(metaXOff));  memset(metaYOff,0,sizeof(metaYOff));
	memset(metaYFac,1,sizeof(metaYFac));
	memset(remapTab,0,sizeof(remapTab));

	if (argc==1) {
		printf("usage: png2spr -l <sprites> <options> sprites.png\n");
		printf("V1.5\n");
		printf("options:\n");
		printf("-a auto shrink sprite (Amiga) or auto insert OL sprites (c64)\n");
		printf("-c <x> color list (c64:bgc,mc1,mc2,olc\n");
		printf(" - nes:bgc,a,a,a,b,b,b,c,c,c,d,d,d\n");
		printf("-d detect doubles and write remap table (C64)\n");
		printf("-h <x> add shape headers AND MASK, x=screenbytesPerLine (Amiga)\n");
		printf("-l <x> limit to <x> sprites in output files\n");
		printf("-k <x> skip <x> sprites\n");
		printf("-n enables NES mode\n");
		printf("-o <outfile> - changes base name of output files\n");
		printf("-p <planes> amount of planes to write (enables Amiga mode)\n");
		printf("-r <x>,<y> specify raster (default: 24x21)\n");
		printf("-b <x> base offset for offset file (default: 0)\n");
		printf("-t <x> tile offset for offset file (default: 0)\n");
		printf("-u write utility files (num/pal/col)\n");
		printf("-i <x>,<y> specify internal raster (default: 16x16)\n");
		//printf("-s generate c sourcefile n.i.\n");
		printf("-x <x>,... adjust x offset by x pixels (NES)\n");
		printf("-v <x>,... vertical height multiplyer (NES)\n");
		exit(0);
	}
	a1=1;  a3=argc-1;  baseName[0]='\0';
	for (;;) {
		if (argc>a1 && argv[a1][0]=='-') {
			int advance=1;
			     if (argv[a1][1]=='a') { autoAdjust=overlaySupport=1; }
			else if (argv[a1][1]=='d') { detectDoubles=remapTable=1; }
			else if (argv[a1][1]=='v') {
				char* p=argv[a1+1];  int x;
				//printf("<%s>\n",p);
				x=0;  while (*p) {
					int i=GetIV(&p);
					if (x>=maxMetaSprites) break; else {
						metaYFac[x]=i;
						//printf("store:%d:%d:%d\n",skip,x,i);
					}
					++x;
				}
				advance=2;
			} else if (argv[a1][1]=='h') { header=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='k') { skip=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='l') { cSprites=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='n') { nes=1; }
			else if (argv[a1][1]=='o') { strcpy(baseName,argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='p')
				{ planes=atoi(argv[a1+1]);  cbf=planes*8;  advance=2;  mig=1; }
			else if (argv[a1][1]=='u') { utility=1; }
			else if (argv[a1][1]=='i') {
				char* p=argv[a1+1];  iw=GetIV(&p);  ih=GetIV(&p);  advance=2;
			} else if (argv[a1][1]=='r') {
				char* p=argv[a1+1];
				rw=GetIV(&p);
				rh=GetIV(&p);
				advance=2;
			} else if (argv[a1][1]=='t')  { tileOffset=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='b') { baseOffset=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='c') {
				char* p=argv[a1+1];
				while (*p) if (cPos<maxPaletteEntries) {
					int i=GetIV(&p);
					colTab[cPos++]=i;
				}
				if (cPos>=1) bgc=colTab[0];
				if (cPos>=2) mc1=colTab[1];
				if (cPos>=3) mc2=colTab[2];
				if (cPos>=4) olc=colTab[3];
				//printf("cols:%d:%d:%d:%d\n",bgc,mc1,mc2,olc);
				advance=2;
			} else if (argv[a1][1]=='x') {
				char* p=argv[a1+1];  int x;
				//printf("<%s>\n",p);
				x=0;  while (*p) {
					int i=GetIV(&p);
					if (x>=maxMetaSprites) break; else {
						metaXOff[x]=i;
						//printf("store:%d:%d:%d\n",skip,x,i);
					}
					++x;
				}
				advance=2;
			}
			a1+=advance;
		} else break;
	}

	// assert colors
	if (nes) for (a1=0;a1<maxNESPalEntries;++a1) colTab[a1]=ConvIndex(colTab[a1]);

	// define output name
	if (strlen(baseName)) { // overwrite?
		if (strlen(baseName)>=5 && baseName[strlen(baseName)-3]>='.')
			strcat(baseName,".xxx");
		name0=baseName;
	} else name0=argv[a3];
	strcpy(sprName,name0);  l=strlen(sprName);
	if (sprName[l-4]!='.') { printf("wrong sprites filename\n");  exit(0); }
	sprName[l-3]='s';  sprName[l-2]='p';  sprName[l-1]='r';
	strcpy(segName,name0);  l=strlen(segName);
	segName[l-3]='s';  segName[l-2]='e';  segName[l-1]='g';
	strcpy(tabName,name0);  l=strlen(tabName);
	tabName[l-3]='t';  tabName[l-2]='a';  tabName[l-1]='b';
	strcpy(lofName,name0);  l=strlen(lofName);
	lofName[l-3]='l';  lofName[l-2]='o';  lofName[l-1]='f';
	strcpy(remName,name0);  l=strlen(remName);
	remName[l-3]='r';  remName[l-2]='e';  remName[l-1]='m';
	strcpy(offName,name0);  l=strlen(offName);
	offName[l-3]='o';  offName[l-2]='f';  offName[l-1]='f';
	strcpy(palName,name0);  l=strlen(palName);
	palName[l-3]='p';  palName[l-2]='a';  palName[l-1]='l';
	strcpy(colName,name0);  l=strlen(colName);
	colName[l-3]='c';  colName[l-2]='o';  colName[l-1]='l';
	strcpy(numName,name0);  l=strlen(numName);
	numName[l-3]='n';  numName[l-2]='u';  numName[l-1]='m';
	name0=argv[a3]; // point to original name for loading

	cSprites+=skip;

	for (;;) {
		void* bufPtr=NULL,* convBuf=NULL,* workBuf=NULL;  long h, bufSize=0;
		enum { bufferSize=cmSprites*maxSprBW*maxSprH*maxSprD };
		unsigned char //sprTab[cmSprites],
			buffer[bufferSize];
		//for (l=0;l<cmSprites;++l) sprTab[l]=0;
		for (l=0;l<cmSprites*64;++l) buffer[l]=0;

		// check size for buffer
		if ((file=fopen(name0,"rb"))==NULL) { error="file error (0)"; break; }
		fseek(file,0,SEEK_END);  h=ftell(file);  fclose(file);
		if (h>bufSize) bufSize=h;

		if ((bufPtr=(void*)malloc(bufSize))==NULL) { error="out of mem"; break; }
		if ((workBuf=(void*)malloc(2*4*(maxPNGWidth+1)))==NULL)
			{ error="out of mem";  break; }

		if (bufPtr && (file=fopen(name0,"rb"))) {
			h=fread(bufPtr,1,bufSize,file);  fclose(file);  file=NULL;
			PNGInfo pngInfo;  PNG png=NULL;  long bpp, bpl=0;
			if (bufPtr) {
				if ((png=pngInit(&pngInfo))==NULL) { error="png ini failed";  break; }
				if ((bufPtr=pngGetHead(png,bufPtr))==NULL)
					{ pngFree(png);  png=NULL;  error="png ini 2 failed";  break; }
			}
			for (;bufPtr;) {
				//printf("debug: name:%s\n",pngChunkName(bufPtr));
				if (strcmp(pngChunkName(bufPtr),"IHDR")==0) {
					bufPtr=pngGetIHDR(png,bufPtr);
					bpp=3;  bpl=bpp*pngInfo.width;
					if ((convBuf=(void*)malloc(pngInfo.width*pngInfo.height*bpp))==NULL)
						{ error="out of memory";  break; }
					//if (bwPtr) *bwPtr=pngInfo.width;
					//if (bhPtr) *bhPtr=pngInfo.height;
					//if (pngInfo.width>fb->maxShapeW)
					//	{ pngFree(png);  png=NULL;  return NULL; }
					//if (pngInfo.height>fb->maxShapeH)
					//	{ pngFree(png);  png=NULL;  return NULL; }
				} else if (strcmp(pngChunkName(bufPtr),"PLTE")==0) {
					bufPtr=pngGetPLTE(png,bufPtr);
				} else if (strcmp(pngChunkName(bufPtr),"tRNS")==0) {
					bufPtr=pngGettRNS(png,bufPtr);
				} else if (strcmp(pngChunkName(bufPtr),"IDAT")==0) {
					bufPtr=pngGetIDAT(png,bufPtr,convBuf,NULL,workBuf,1,
						//bpl,bpp,0xff000,0x00ff00,0x0000ff,0,0,0,0,0);
						bpl,bpp,0xff000000,0x00ff0000,0x0000ff00,0,0,0,0,-1);
				} else {
					int last=(strcmp(pngChunkName(bufPtr),"IEND")==0);
					bufPtr=pngGetXXXX(png,bufPtr);
					if (last) break;
				}
			}
			//if (file=fopen(sprName,"wb")) {
			if (1) {
				unsigned char* p=convBuf, v, v2;  long cx, cy, x, y, i,
					spriteCount=0, //c0=cSprites*64,
					bp=0,
					spritesPerRow=pngInfo.width/rw, // 8 :)
					pixelsPerRow=pngInfo.width;
				enum { ucMaxCols=5 };
				unsigned char ucTab[ucMaxCols]={255,255,255,255,255}; // used colors - up to 4 (really?) 5?
				long
					sn=0,    // number of current meta sprite [0..175] (NES/C64)
					cmSI=0; // image number of sprite segment (NES)
				short
					//olSkip=0, // skip next sprite because of overlay-sprite?
					yNext=1, // increased if detecting double-height metasprites
					ti=0;  // index into meta sprite attribute table (NES)

				for (cy=0;spriteCount<cSprites && cy<99;cy+=yNext,yNext=1) for (cx=0;
					spriteCount<cSprites && cx<spritesPerRow;cx+=1) {
					//printf("c:%d(%d) skip:%d\n",spriteCount,cSprites,skip);
					if (//0==olSkip &&
						spriteCount>=skip) { // cx/cy == sprite no.
						int
							col=255,
							hi=0, // hires or multicolor? (C64)
							cc=0;

						  //
						 // *** NES support ***
						//
						if (nes) {
							long
								trh=rh,
								sx,  // horizontal offset of sprite segment in pixels
								sy, // vertical offset of sprite segment in pixels
								n;
							metaSprOff[sn]=ti;
							//metaSprLo[sn]=ti&255; // store new meta sprite offset (:=tab-index)
							//metaSprHi[sn]=ti>>8;
							trh=metaYFac[sn]*rh; // <- double height metasprite? //printf("sn:%d:%d\n",sn,metaYFac[sn]);
							if (metaYFac[sn]>yNext) yNext=metaYFac[sn];
							for (sy=0;sy<trh;sy+=8) for (sx=0;sx<rw;sx+=8) { // browse spriteSegs
								int
									cEntries=0,
									pal=0,
									zf=1; // zero flag should be cleared when a bit is set
								signed char
									cTab[4]={-1,-1,-1,-1}, // found color indexes
									fTab[4]={0,0,0,0};    // amount of matching colors in palettes

								 // remark: all colors of a sprite seg are needed to
								// identify the palette number -> so we need 2 passes
								// pass 1: collect colors
								for (y=0;y<8;++y) for (x=0;x<8;++x) {
									unsigned char r, g, b, i, c, found;
									long o=3*(
										cy*pixelsPerRow*rh +pixelsPerRow*sy +pixelsPerRow*y
										+cx*rw +sx +x);
									if (sy+y>=trh) r=g=b=0; else { r=p[o+0];  g=p[o+1];  b=p[o+2]; }
									i=ConvIndex(GetPNGIndex(&pngInfo,r,g,b)); // i := cleaned index
									// index already stored?
									found=0;  for (c=0;c<cEntries;++c) if
										(cTab[c]==i) { found=1;  break; }
									if (0==found) {
										cTab[cEntries]=i;  if (cEntries<4) ++cEntries;
									}
								}
								//printf("c:%d:%d:%d:%d\n",cTab[0],cTab[1],cTab[2],cTab[3]);

								// pass 2: count matching colors in all palettes
								for (n=0;n<4;++n) { // check 4 palettes
									for (x=0;x<cEntries;++x) { // check all found colors
										int found=0;  for (y=0;y<4;++y) { // compare 4 entries
											int i;
											if (0==y) i=0; else i=n*3+y; // BGCOL? PALCOL?
											if (cTab[x]==colTab[i]) { found=1;  break; }
										}
										if (found) ++fTab[n]; // increase palette quality indicator
									}
								}
								// select palette with highest match count
								x=pal=0;  for (n=0;n<4;++n) if (fTab[n]>x) { x=fTab[n];  pal=n; }
								//printf("f:%d:%d:%d:%d p:%d\n",fTab[0],fTab[1],fTab[2],fTab[3],pal);
								// add color to palette? (todo)

								// pass 3: set bits
								for (n=0;n<2;++n) {    // plane for the current sprite-seg
									for (y=0;y<8;++y) { // for each byte in seg 0..7
										int v=0;
										for (x=0;x<8;++x) { // for each bit 0..7
											unsigned char s=0, i, r, g, b;
											long o=3*(
												cy*pixelsPerRow*rh +pixelsPerRow*sy +pixelsPerRow*y
												+cx*rw +sx +x);
											r=p[o+0];  g=p[o+1];  b=p[o+2];
											if (sy+y>=trh) s=0; else { // overflow? -> transparent
												int j;
												i=ConvIndex(GetPNGIndex(&pngInfo,r,g,b));
												if (i==colTab[0]) s=0; else for // BG color? -> transparent
													(j=0;j<3;++j) if (i==colTab[1+pal*3+j])
													{ s=j+1;  break; } // deep bit value "s" is 1..3
											}
											if (s&(1<<n)) v|=(1<<(7-x));
										}
										if (v) zf=0;
										if (bp<bufferSize) buffer[bp++]=v;
									}
								}

								if (zf==0) { // non-empty sprite seg?
									long found=-1;  unsigned char f, mirrored=0;
									// check previous sprite segs for duplicate entry
									for (n=0;n<cmSI;++n) {
										unsigned char m[16], equal=1;

										// equal?
										for (y=0;y<16;++y) if (buffer[n*16+y]!=buffer[cmSI*16+y])
											{ equal=0;  break; }
										if (equal) { found=n;  break; } // unmirrored duplicate found!

										// mirrored?
										for (y=0;y<16;++y) { // generate mirrored dummy for comparison
											m[y]=0;
											for (x=0;x<8;++x) if (buffer[cmSI*16+y]&(1<<x)) m[y]|=1<<(7-x);
										}
										mirrored=1;
										for (y=0;y<16;++y) if (buffer[n*16+y]!=m[y])
											{ mirrored=0;  break; }
										if (mirrored) { found=n;  break; } // mirrored duplicate found!
									}
									if (found!=-1) { n=found;  bp-=16; } else n=cmSI++;
									f=pal;  if (mirrored) f|=64; // set OAM_FLIP_H?

									// construct new metasprite: Add entry to metaSprTab...
									metaSprTab[ti++]=sx+metaXOff[sn];
									metaSprTab[ti++]=sy+metaYOff[sn];
									metaSprTab[ti++]=baseOffset+n;  // used image number
									metaSprTab[ti++]=f; // palette + attributes
									//printf("sn:%d:%d\n",sn,metaXOff[sn]);
								} else bp-=16; // wind back buffer index, because seg is unused
							}
							metaSprTab[ti++]=128; // place end marker
							++sn;

						/*if (nes) { // <-- this version only converts 8x8 sprites
							long n, v=0;
							for (n=0;n<2;++n) {    // plane
								for (y=0;y<8;++y) { // for each byte in seg 0..7
									v=0;
									for (x=0;x<8;++x) { // bit
										unsigned char i, r, g, b;//, f, j;
										long o=3*(cy*pixelsPerRow+cx*8+pixelsPerRow*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										//i=GetNESIndex(&pngInfo,r,g,b);
										if (0==r && 0==g && 0==b) i=0; else i=1; // b/w only so far
										//if (i==13 || i==2) i=0; else i=1; // replace index 13
										if (i&(1<<n)) v|=(1<<(7-x));
									}
									buffer[bp++]=v;
								}
							}*/

						  //
						 // ***** Amiga support *****
						//

						// todo:
						// *OK* rw < ew? -> output empty bits
						// - used pixels < iw? -> dec ew

						} else if (mig) {
							long
								ew=iw, eh=ih; // edited w/h (if autoAdjust is set)

							if (autoAdjust) { // can we decrease width/height?
								// can we shrink the amount of lines?
								long lw, tw=iw;  if (rw<tw) tw=rw; // test-width
								ew=eh=1;
								for (y=0;y<ih;y+=1) {
									lw=0;  for (x=0;x<tw;++x) {
										unsigned char r, g, b;
										long o=3*(cy*rh*pixelsPerRow+cx*rw+pixelsPerRow*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										if (GetPNGIndex(&pngInfo,r,g,b)) { lw=x+1;  eh=y+1; }
									}
									if (lw>ew) ew=lw;
									//if (f) { eh=y+1;  break; } // effective height found
								}
								ew+=15;  ew>>=4;  ew<<=4; // round-up to next 16bit border

								//if (eh<1) eh=1; // just in case
								/*
								eh=1;
								for (y=ih-1;y>0;--y) { // find bottommost line with data here
									int f=0;
									for (x=0;x<tw;++x) {
										unsigned char r, g, b;
										long o=3*(cy*rh*pixelsPerRow+cx*rw+pixelsPerRow*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										if (GetPNGIndex(&pngInfo,r,g,b)) { f=1;  break; }
									}
									if (f) { eh=y+1;  break; } // effective height found
								}
								if (eh<1) eh=1; // just in case
								*/
							}

							long
								shapeSize=(ew/16)*2*planes*eh,
								lineBytes=(ew/16)*2*planes;
							//printf("mig1:%d:%d (%d:%d)\n",ew,ih,ew,eh);
							if (header) {
								nxPutU16(&buffer[bp],(header/8)-2*(ew/16)-2);  bp+=2; // modulo A (screen)
								nxPutU16(&buffer[bp],-2);  bp+=2; // modulo B (shape)
								nxPutU16(&buffer[bp],-2);  bp+=2; // modulo C (mask)
								nxPutU16(&buffer[bp],(header/8)-2*(ew/16)-2);  bp+=2; // modulo D (screen)
								nxPutU16(&buffer[bp],(ew/16)+1+64*planes*eh);  bp+=2; // blitSize
								nxPutU16(&buffer[bp],shapeSize);  bp+=2; // shapeSize (to jump to mask)
								nxPutU16(&buffer[bp],eh);  bp+=2; // height
								nxPutU16(&buffer[bp],lineBytes);  bp+=2; // lineBytes
							}
							for (y=0;y<eh;y+=1) {
								long i, n, s, t;
								for (s=1,n=0;n<planes;++n,s<<=1) for (t=0;t<ew/8;++t) {
									unsigned char v=0;  for (x=0;x<8;++x) {
										unsigned char r=0, g=0, b=0;  if (t*8+x<rw) { // inside?
											long o=3*(cy*rh*pixelsPerRow+cx*rw+pixelsPerRow*y+t*8+x);
											r=p[o+0];  g=p[o+1];  b=p[o+2];
										}
										i=GetPNGIndex(&pngInfo,r,g,b);  if (i&s) v|=(1<<(7-x));
									}
									//printf("ynt:%d:%d:%d\n",y,n,t);
									buffer[bp++]=v;
								}
							}

							// build mask?
							if (header) for (y=0;y<eh;y+=1) {
								long n, s, t,
									startOff=bp-shapeSize; // index to start of image data row
								for (s=1,n=0;n<planes;++n,s<<=1) for (t=0;t<ew/8;++t) {
									long b;  unsigned char v;
									// loop to calculate mask bytes
									v=0;  for (b=0;b<planes;++b) v|=buffer[startOff+t+b*(ew/16)*2];
									buffer[bp++]=v;
								}
							}

						  //
						 // *** C64 support ***
						//
						} else {
							unsigned char myFlags=0;  int rawSpr=bp/64;

							//printf("c64-sn:%d rawSpr:%d\n",sn,rawSpr);

							// analyse sprite: only hi-pixels? only one color?
							for (y=0;y<21;y+=1) { // detect hires pixels
								for (i=0;i<3;i+=1) {
									for (x=0;x<8;x+=2) {
										long o=3*((y+cy*21)*pixelsPerRow+cx*24+i*8+x);
										unsigned char r1, g1, b1, r2, g2, b2;
										r1=p[o+0];  g1=p[o+1];  b1=p[o+2];
										r2=p[o+3];  g2=p[o+4];  b2=p[o+5];
										if (r1!=r2 || g1!=g2 || b1!=b2) hi=1;
									}
								}
							}
							if (hi==0) myFlags|=16; // mc sprite? -> |=16

							cc=0;  for (y=0;y<21;y+=1) { // count colors
								for (i=0;i<3;i+=1) {
									unsigned char r, g, b;  int f, j;
									v=0;  for (x=0;x<8;x+=1) {
										long o=3*((y+cy*21)*pixelsPerRow+cx*24+i*8+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										f=0;  if (cc) for (j=0;j<cc;j+=1) if (GetIndex(&pngInfo,r,g,b)==ucTab[j]) f=1;
										if (f==0 && cc<ucMaxCols) {
											int j=GetIndex(&pngInfo,r,g,b);  ucTab[cc++]=j;
											if (hi) { if (j!=bgc) col=j; }
												else { if (j!=bgc && j!=mc1 && j!=mc2) col=j; }
										}
										if (f==0 && cc>ucMaxCols) { printf("warning: color overflow\n"); }
									}
								}
							}
							if (0<=col && col<=15) myFlags|=col; //sprTab[cx+spritesPerRow*cy]|=col; // color found?

							//if (104<=spriteCount && spriteCount<108) {
								//if (hi) printf("%d:hi(%d)\n",spriteCount,col);
								//	else printf("%d:mc(%d)\n",spriteCount,col);
								//printf("cc:%d col:%d (xy:%d:%d)\n",cc,col,cx,cy);
							//}

							// write sprites
							int overlayError=0;
							for (y=0;y<63;y+=1) buffer[bp+y]=0; // might be filled with data
							                                    // after a backstep!
							for (y=0;y<21;y+=1) {
								for (i=0;i<3;i+=1) {
									v=v2=0;  if (hi && cc<=2) { // hires 1 color
										for (x=0;x<8;x+=1) {
											long o=3*((y+cy*21)*pixelsPerRow+cx*24+i*8+x), j;
											unsigned char r, g, b;
											r=p[o+0];  g=p[o+1];  b=p[o+2];
											j=GetIndex(&pngInfo,r,g,b);
											if (j!=bgc) v|=(1<<(7-x));
										}
										//printf("hi1:%d\n",cc);
									} else if (hi==0) { // multicolor
										for (x=0;x<8;x+=2) {
											long o=3*((y+cy*21)*pixelsPerRow+cx*24+i*8+x), j;
											unsigned char r, g, b;
											r=p[o+0];  g=p[o+1];  b=p[o+2];
											j=GetIndex(&pngInfo,r,g,b);
											//printf("i-%d:%d %d %d (%d)\n",j,r,g,b,bgc);
											if (j==bgc) ; // bgc
											else if (j==mc1) v|=(1<<(7-(x+1)));
											else if (j==mc2) { v|=(1<<(7-(x+0)));  v|=(1<<(7-(x+1))); }
											else v|=(1<<(7-(x+0)));  // sprite color
										}
									} else { // overlay
										//printf("ol1\n");
										for (x=0;x<8;x+=1) {
											long o=3*((y+cy*21)*pixelsPerRow+cx*24+i*8+x), j;
											unsigned char r, g, b;
											r=p[o+0];  g=p[o+1];  b=p[o+2];
											j=GetIndex(&pngInfo,r,g,b);
											//printf("i-%d:%d %d %d (%d)\n",j,r,g,b,olc);
											if (j==bgc) ; // bgc
											else if (j==olc) v|=(1<<(7-x));
											else if (j==mc1 && (x&1)==0) v2|=(1<<(7-(x+1)));
											else if (j==mc1 && (x&1)) v2|=(1<<(7-(x)));
											else if (j==mc2 && (x&1)==0) { v2|=(1<<(7-(x+0)));  v2|=(1<<(7-(x+1))); }
											else if (j==mc2 && (x&1)) { v2|=(1<<(7-(x-1)));  v2|=(1<<(7-(x))); }
											else if ((x&1)==0) v2|=(1<<(7-(x+0)));  // sprite color
											else if ((x&1)) v2|=(1<<(7-(x-1)));  // sprite color
											else printf("undefined condition\n");
										}
										//printf("e\n");  //exit(0);
									}
									if (!overlaySupport && v2) overlayError=1;
									if (bp+64*overlayOff<cmSprites*64) buffer[bp+64*overlayOff]|=v2;
										else printf("overflow\n"); // <- warning: suspicous errorCheck
										//                               (might be broken)
									buffer[bp++]|=v;
								}
							}
							if (1==overlayError) printf(
								"warning: overlay sprites not activated, but found at #%ld\n",
								sn);

							if (hi && cc>2 && sn+overlayOff<cSprites+1) { // OL-sprite!
								buffer[bp++]=olc|32;
								if (autoAdjust) { bp+=64;  buffer[bp-1]=col|16; }
							} else { // standard sprite
								buffer[bp++]=myFlags;
							}
							//printf("\n");
							//if (1 || bp>=60*64) printf("i:%d %d\n",bp/64,sprTab[bp/64]);
							//printf("n:%d:(%d):%d\n",cx+spritesPerRow*cy,bp,sprTab[cx+spritesPerRow*cy]);
							//v=sprTab[cx+spritesPerRow*cy];  buffer[bp++]=v;
							//printf("v:%d\n",bp-1);
							//fwrite(&v,1,1,file);
							//c0-=1;  if (c0<0) c0=0;  // fitter

							// look for doubles?
							if (detectDoubles && sn) {
								int x, f, filledSprites=bp/64-1, z;  long myOff=bp-64;
								//for (x=0;x<21;x+=1) {
									//for (f=0;f<3;f+=1) printf("%02x",buffer[myOff+x*3+f]);
									//printf("\n");
								//}
								f=-1;
								z=1;  for (y=0;y<63;y+=1) if (buffer[myOff+y]) { z=0;  break; }
								if (z) { f=255; } // empty sprite!
									else for (x=0;x<filledSprites;x+=1) { // sprite not empty...
									int y;  long browseOff=x*64;  f=x;
									for (y=0;y<64;y+=1) { // compare sprite data and color byte!
										if (buffer[myOff+y]!=buffer[browseOff+y]) { f=-1;  break; }
									}
									if (f!=-1) break; // identical copy has been found
								}
								//printf(">then:%ld filled:%d myOff:%ld cxy:%ld:%ld",
								//	spriteCount,filledSprites,myOff,cx,cy);
								//printf("check:%ld filled:%d (%d) myOff:%ld\n",
								//	spriteCount,filledSprites,f,myOff);
								//printf("sn:%ld:%d / %d:%d\n",sn,rawSpr,filledSprites,-1);
								if (f!=-1) // duplicate found?
								          // store index of origial in remap table and step back
									   { remapTab[sn]=f;  bp-=64; }
									else remapTab[sn]=rawSpr; // no duplicate found
									//printf("f:%d\n",f);
							} else {
								//printf("el:%ld:%d\n",sn,rawSpr);
								remapTab[sn]=rawSpr;//bp/64-1;
							}
							//printf("sc:%d sn:%d [%d]\n",sn,sn,remapTab[sn]);
							++sn;

						}

						if (0) { // verbose output
							if (hi && cc<=2) printf("h");
								else if (hi==0) printf("m"); else printf("o");
							if (cx==spritesPerRow-1) printf("\n");
						}
					} //else if (olSkip) {
						//printf("else\n");
						//olSkip=0;  //bp+=64;
						//v=sprTab[cx+spritesPerRow*cy];  buffer[bp-1]=v;
					//}
					++spriteCount;
				}

				// write buffer in standard mode
				if (mode!=1 && (file=fopen(sprName,"wb"))) {
					for (l=0;l<bp;++l) fwrite(&buffer[l],1,1,file);
					fclose(file);  file=NULL;
				}

				if (utility && !mig && (file=fopen(numName,"wb"))) {
					unsigned char n;
					if (nes) n=bp/16; else n=bp/64;
					fwrite(&n,1,1,file);
					fclose(file);  file=NULL;
				}
				if (utility && mig && (file=fopen(colName,"wb"))) {
					PNGInfo* p=&pngInfo;  long n, cPos=0;
					unsigned char myColTab[256+3];
					for (n=0;n<p->colors;++n) {
						myColTab[cPos++]=p->color[n*3+0];
						myColTab[cPos++]=p->color[n*3+1];
						myColTab[cPos++]=p->color[n*3+2];
					}
					fwrite(myColTab,1,cPos,file);
					fclose(file);  file=NULL;
				}

				// write always 2 colors in c64 mode!
				if (utility && !nes && !mig && (file=fopen(palName,"wb"))) {
					static unsigned char wPal[2];  wPal[0]=mc1;  wPal[1]=mc2;
					fwrite(wPal,1,2,file);  fclose(file);  file=NULL;
				}

				// write meta sprite offsets?
				if (nes) {
					if (file=fopen(offName,"wb")) { // write offset table
						int i;  for (i=0;i<sn;i+=1) {
							short off=metaSprOff[i]+tileOffset;
							unsigned char lo=off&255, hi=off/256;
							fwrite(&lo,1,1,file);  fwrite(&hi,1,1,file);
						}
						//fwrite(metaSprOff,1,sn*2,file);
						fclose(file);  file=NULL;
					}
					if (file=fopen(tabName,"wb")) // write meta sprite table
						{ fwrite(metaSprTab,1,ti,file);  fclose(file);  file=NULL; }
					if (file=fopen(palName,"wb")) { // write sprite palette
						static unsigned char wPal[16]; // build neslib compatible palette
						//printf("<%s>\n",palName);
						//printf("c:%d:%d:%d:%d\n",colTab[0],colTab[1],colTab[2],colTab[3]);
						wPal[0]=wPal[4]=wPal[8]=wPal[12]=colTab[0];
						wPal[1]=colTab[1];  wPal[2]=colTab[2];  wPal[3]=colTab[3];
						wPal[5]=colTab[4];  wPal[6]=colTab[5];  wPal[7]=colTab[6];
						wPal[9]=colTab[7];  wPal[10]=colTab[8];  wPal[11]=colTab[9];
						wPal[13]=colTab[10];  wPal[14]=colTab[11];  wPal[15]=colTab[12];
						fwrite(wPal,1,16,file);  fclose(file);  file=NULL;
					}
				}
				if (remapTable) {
					if (file=fopen(remName,"wb")) // write remap table
						{ fwrite(remapTab,1,sn,file);  fclose(file);  file=NULL; }
				}

				 // mode-1 support (obsolete) only grab segments: s:80,g:10:12:4
				//
				if (mode==1 && (file=fopen(segName,"wb"))) {
					char* p;  unsigned char* m=buffer;  //int bc0=0, bc1=0;
					for (p=command;*p;++p) if (*p=='s') {
						int a;  p+=2;  a=GetI(&p);
						m+=64*a;
					} else if (*p=='o') { // old grab
						int a, h1, h2, s, y;
						p+=2;  a=GetI(&p);  ++p;  h1=GetI(&p);  ++p;  h2=GetI(&p);
						for (s=0;s<a;++s) { // browse sprites
							for (y=0;y<h2;++y) {
								fwrite(&m[3*(h1+y)],1,1,file);
								fwrite(&m[3*(h1+y)+1],1,1,file);
							}
							m+=64;
						}
					} else if (*p=='g') { // grab
						int h, y;
						p+=2;  for (;;++p) {
							h=GetI(&p);
							for (y=0;y<h;++y) {
								fwrite(&m[3*(21-h+y)],1,1,file);
								fwrite(&m[3*(21-h+y)+1],1,1,file);
							}
							if (*p=='\0') break;
							m+=64;
						}
					}
					fclose(file);  file=NULL;
				}

			}
			if (png) { pngFree(png);  png=NULL; }
			if (convBuf) { free(convBuf);  convBuf=NULL; }

		} else printf("file not found\n");
		// cleanup
		if (workBuf) { free(workBuf);  workBuf=NULL; }
		break;
	}
	return 0;
}
