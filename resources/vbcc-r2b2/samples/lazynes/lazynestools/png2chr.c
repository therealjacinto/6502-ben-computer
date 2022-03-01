   //
  // png2chr - generates CHR/COL/PAL files, C64/NES/Amiga, 'Lazycow 2013
 // disclaimer: this is C code, not a C tutorial
//

// gcc -I../smartfx -Wno-parentheses -o png2chr png2chr.c ../smartfx/smartpng.c -lz -L../libz-1.2.7

// V1.0: (01.06.2013) initional version for Powerglove
// V1.1: (02.06.2013) support Powerglove mode-2/3 charset color groups (obsolete)
// V1.2: (01,02,2014( Pharaohs Return mode-4 color stripe data output support (obsolete)
// V1.3: (06.02.2014) mode-3 bugs fixed in color calculations (obsolete)
// V1.4: (06.05.2014) NES support for modes 8..15 (obsolete)
// V1.5: (09.10.2014) Amiga plane support (-p)
// V1.6: [18.01.2015] 256x64 format support for NES-Powerglove
// V1.7: [20.06.2020] -u added, first public release for lazyNES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "smartpng.h"

enum { maxChars=2048, maxCBF=256, maxCBC=3,
	c64MC=8, // multi color block
	c64Blank=16, // no pixels in block
	c64Solid=32, // solid block
	c64MCUnset=64,   // hi/mc state undefined (both would work)
	c64FCUnset=128, // no foreground color found
	nesBackColOnly=16, // flag for NES colTab[] (was 128)
	maxNESPalEntries=1+4*3
};

unsigned char
	memTab[maxChars*maxCBF],   // bits
	palTab[maxNESPalEntries], // NES palette entries
	colTab[maxChars*maxCBC], // storage for all 256 colors or char-colInfos
	tmpTab[maxChars*maxCBC], // temporary storage for color-grouping
	pixTab[maxChars], // ?
	strTab[maxChars/32][3]; // global color stripes

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


unsigned char ConvIndex(unsigned char i) {
	switch (i) {
		// black?
		case 13: case 14: case 15: case 46: case 47: case 62: case 63: return 29;
		case 32: return 48; // white?
		default: return i; // others
	}
}

 // get C64 index of RGB tripple
//
unsigned char GetC64Index(PNGInfo* p,
	unsigned char r, unsigned char g, unsigned char b) {
	return GetPNGIndex(p,r,g,b);
}


unsigned char Mirror(unsigned char i) {
	unsigned char o=0;  int n;
	for (n=0;n<8;n+=2) {
		if (i&(1<<n)) o|=(1<<(6-n));
		if (i&(1<<(n+1))) o|=(1<<(7-n));
	}
	return o;
}

  // v: char number to check
 // result: see below
//
unsigned char CheckCHR(int mode, unsigned char v) {
	int i, f;

	// rom range? - actually disabled and set to zero
	if (mode!=4 && ((1<=v && v<=26) || (48<=v && v<58)))
		return 1; //return 6;

	// all zero?
	for (f=i=0;i<8;++i) if (memTab[v*8+i]) { f=1;  break; }
	if (f==0) return 1;

	// H-mirrored with char-1?
	if (v>=1) {
		for (f=i=0;i<8;++i) if
			(Mirror(memTab[(v-1)*8+i])!=memTab[v*8+i]) { f=1;  break; }
		if (f==0) return 2;
	}

	// H-mirrored with char-3?
	if (v>=3) {
		for (f=i=0;i<8;++i) if
			(Mirror(memTab[(v-3)*8+i])!=memTab[v*8+i]) { f=1;  break; }
		if (f==0) return 3;
	}

	// V-mirrored with char-32?
	if (v>=32) {
		for (f=i=0;i<8;++i) if
			(memTab[(v-32)*8+7-i]!=memTab[v*8+i]) { f=1;  break; }
		if (f==0) return 4;
	}

	// V-mirrored with char-96?
	if (v>=96) {
		for (f=i=0;i<8;++i) if
			(memTab[(v-96)*8+7-i]!=memTab[v*8+i]) { f=1;  break; }
		if (f==0) return 5;
	}

	return 0;
}

 //
//
int GetIV(char** p) {
	int v=0;  while ('0'<=**p && **p<='9') { v*=10;  v+=**p-'0';  ++*p; }
	if (**p) ++*p; // skip ","
	return v;
}

enum { maxMapW=256, maxMapH=256 };

 //
//
int main(int argc, char** argv) {
	FILE* file=NULL,* cFile=NULL,* sFile=NULL,* tFile=NULL,* pFile=NULL,
		* mFile=NULL;  char* name1=NULL,* error=NULL,
		* command=NULL, chrName[256], colName[256], strName[256], txtName[256],
		palName[256], mapName[256];
	unsigned char bMap[maxMapW*maxMapH], cMap[maxMapW*maxMapH], //short wMap[maxMapW*maxMapH];
		bgc=0, mc1=0, mc2=0;
	int l, debug=0, group=0, mode=0, charAmount=256, qualAmount=256,
		nes=0, mig=0,
		a1, a2, a3, avoid=0, genMap=0, source=0, tweak=0, blankUnused=0,
		cPos=0, pPos=0, utility=0,
		planes=1, cbf=8, // char byte factor (c64==8, NES==16, Amiga==8..256)
		tcl=8,   // total char lines
		cpr=32; // chars per row (16 for 128x128 pixel, 32 for 256x64 pixel)

	memset(memTab,0,maxChars*maxCBF);

	if (argc<3) {
		printf("png2chr V1.7\n");
		printf("usage: png2chr <options> charset.png\n");
		printf("options:\n");
		printf("-a avoid duplicates\n");
		printf("-b blank unused chars in charset file\n");
		printf("-c <x> color list (c64:bgc,mc1,mc2 - nes:bgc,a,a,a,b,b,b,c,c,c,d,d,d\n");
		printf("-d show debug output\n");
		printf("-g group color block detection to 2x2 area (NES/C64)\n");
		printf("-l <x> limit to <x> chars in output files (default: all)\n");
		printf("-m generate mapfile\n");
		printf("-n NES mode\n");
		printf("-p <planes> select number of planes to write (Amiga)\n");
		printf("-q <x> qualified limit of char color detection (default: all)\n");
		printf("-s generate c sourcefile\n");
		printf("-t tweak colorbits (to use mc-chars in c64 sprites)\n");
		printf("-u write utility files\n");

		exit(0);
	}
	for (l=0;l<maxChars;++l) { pixTab[l]=0;  colTab[l]=255; }

	// let(a,word);

	// parse options
	a1=1;  a2=2;  a3=argc-1;
	for (;;) {
		if (argc>a1 && argv[a1][0]=='-') {
			int advance=1;
			if (argv[a1][1]=='a') avoid=1;
			else if (argv[a1][1]=='b') blankUnused=1;
			else if (argv[a1][1]=='c') {
				char* p=argv[a1+1];
				while (*p) if (pPos<256) {
					int i=GetIV(&p);
					palTab[pPos++]=i;
				}
				//printf("pp:%d\n",pPos);
				//bgc=palTab[0];
				//if (pPos>=3) {
					//int n;
					//bgc=palTab[0];
					//nes=1;
					//for (n=0;n<pPos;++n) if (palTab[n]==13) colTab[n]=15;
					//	else if (colTab[n]==32) colTab[n]=48; // <-- remap nes colors
				//} else {
				if (pPos>0) bgc=palTab[0]; // used for NES/C64
				if (pPos>1) mc1=palTab[1]; // C64 only
				if (pPos>2) mc2=palTab[2]; // C64 only
				//}
				//printf("c:%d:%d:%d\n",bgc,mc1,mc2);
				advance=2;
			} else if (argv[a1][1]=='m') genMap=1;
			else if (argv[a1][1]=='n') nes=1;
			else if (argv[a1][1]=='l') { charAmount=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='p')
				{ planes=atoi(argv[a1+1]);  cbf=planes*8;  advance=2;  mig=1; }
			else if (argv[a1][1]=='q') { qualAmount=atoi(argv[a1+1]);  advance=2; }
			else if (argv[a1][1]=='d') debug=1;
			else if (argv[a1][1]=='g') group=1;
			else if (argv[a1][1]=='s') source=1;
			else if (argv[a1][1]=='t') tweak=1;
			else if (argv[a1][1]=='u') utility=1;
			a1+=advance;  a2+=advance;
		} else break;
	}


	if (charAmount<qualAmount) qualAmount=charAmount; // <- adjust?
	// assert colors
	if (nes) {
		for (a1=0;a1<maxNESPalEntries;++a1) palTab[a1]=ConvIndex(palTab[a1]);
		bgc=ConvIndex(bgc);
	}
	//if (mig==0 && nes==0) cPos=0; // obsolete? already 0?

	//printf("debug:%d:%d:%d\n",bgc,mc1,mc2);
	//mode=2;
	// construct filename "output.chr"
	//if ((mode=atoi(argv[a1]))&8) { mode=(mode&7);  nes=1; } // NES mode?
	//if ((mode=atoi(argv[a1]))&16) { mode=(mode&7);  mig=1; } // Amiga mode?
	//if ((chars=atoi(argv[a2]))>maxChars) chars=maxChars;
	//if (mode==4) command=argv[3];
	//if (mig) { planes=atoi(argv[3]);  cbf=8*planes; } // yo
	if (nes) cbf=16; // yo
	//bgc=atoi(argv[3]);  mc1=atoi(argv[4]);  mc2=atoi(argv[5]);
	name1=argv[a3];  strcpy(chrName,name1);  l=strlen(chrName);
	if (chrName[l-4]!='.') { printf("wrong charset filename\n");  exit(0); }
	chrName[l-3]='c';  chrName[l-2]='h';  chrName[l-1]='r';
	strcpy(colName,name1);
	colName[l-3]='c';  colName[l-2]='o';  colName[l-1]='l';
	strcpy(palName,name1);
	palName[l-3]='p';  palName[l-2]='a';  palName[l-1]='l';
	strcpy(strName,name1);
	strName[l-3]='s';  strName[l-2]='t';  strName[l-1]='r';
	strcpy(mapName,name1);
	mapName[l-3]='m';  mapName[l-2]='a';  mapName[l-1]='p';
	strcpy(txtName,name1);
	txtName[l-3]='c';  txtName[l-2]='\0';  txtName[l-1]='\0';
	//printf("converting... chr:<%s><%s>:%d\n",chrName,colName,chars);

	for (;;) {
		void* bufPtr=NULL,* convBuf=NULL,* workBuf=NULL;  long h, bufSize=0;
		// check size for buffer
		if ((file=fopen(name1,"rb"))==NULL) { error="file error (1)"; break; }
		fseek(file,0,SEEK_END);  h=ftell(file);  fclose(file);
		if (h>bufSize) bufSize=h;

		if ((bufPtr=(void*)malloc(bufSize))==NULL) { error="out of mem"; break; }
		if ((workBuf=(void*)malloc(2*4*(maxPNGWidth+1)))==NULL)
			{ error="out of mem";  break; }
		if (bufPtr && (file=fopen(name1,"rb"))) {
			h=fread(bufPtr,1,bufSize,file);  fclose(file);  file=NULL;
			PNGInfo pngInfo;  PNG png=NULL;  long bw, bh, bpp, bpl;
			if (bufPtr) {
				if ((png=pngInit(&pngInfo))==NULL) { error="png ini failed";  break; }
				if ((bufPtr=pngGetHead(png,bufPtr))==NULL)
					{ pngFree(png);  png=NULL;  error="png ini 2 failed";  break; }
			}
			bpl=0;
			for (;bufPtr;) {
				//printf("debug: name:%s\n",pngChunkName(bufPtr));
				if (strcmp(pngChunkName(bufPtr),"IHDR")==0) {
					bufPtr=pngGetIHDR(png,bufPtr);
					bw=pngInfo.width;  bh=pngInfo.height;  bpp=3;  bpl=bpp*bw;
					cpr=bw/8;  tcl=bh/8;
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
			/*{
				unsigned char* p=convBuf;
				int x, y;
				for (y=0;y<8;y++) printf("%d \n",*p++);
				exit(0);
			}*/

			if (file=fopen(chrName,"wb")) { //&& (pFile=fopen(palName,"wb"))
				//&& (cFile=fopen(colName,"wb"))
				//&& (sFile=fopen(strName,"wb"))
				//) {
				unsigned char* p=convBuf;
				char * csp=command; // color segment pointer
				long cx, cy, x, y, cCount=charAmount*8, // 8==lines
					pCol,
					csl=0, // color segment line
					cla=0, // color line amount for this segment
					mPos=0;
				if (genMap) {
					mFile=fopen(mapName,"wb");
					if (NULL==mFile) { printf("error creating mapfile\n");  exit(0); }
				}

				// setup global colors
				if (mode==4) {
					int i;
					//printf("command:<%s>:%d\n",command,strlen(command));
					//printf("<%s>:",csp);  printf("<%d>\n",GetIV(&csp));
					//printf("<%s>:",csp);  printf("<%d>\n",GetIV(&csp));
					for (;;) {
						cla=GetIV(&csp);
						bgc=GetIV(&csp);  mc1=GetIV(&csp);  mc2=GetIV(&csp);

						for (i=0;i<cla;++i) {
							strTab[csl][0]=bgc;
							strTab[csl][1]=mc1;
							strTab[csl][2]=mc2;
							++csl;
						}
						//printf("csp1:%d\n",csp);
						//printf("csp2:%d\n",*csp);
						if (*csp==0) break;
					}
				}

				 // bf=255, mc1=255, mc2=255
				// analyse charset: 32x8(256x64)/256
				for (cy=0;(cCount || charAmount==0) && cy<tcl/*(chars-1)/cpr+1*/;cy+=1) {
					if (mode==4) // set global colors from strip-table?
						{ bgc=strTab[cy][0];  mc1=strTab[cy][1];  mc2=strTab[cy][2]; }
					for (cx=0;(cCount || charAmount==0) && cx<cpr;cx+=1) {
						long cc=0;  unsigned char
							col=64|128, // the char color (64:mc/hi=unset, 128:blkCol=unset)
							mc=0,   // multi color flag
							hi=0,  // hi res pixel flag
							tbgc, // temporary background color index
							mapChar,
							ucTab[4]={255,255,255,255}; // used colors - up to 4

						// C64
						if (0==mig && 0==nes) for (y=0;y<8;y+=1) {  // pass 1: checking colors and mc/hi
							// check if only 2 colors? -> hires, store single color
							// 2..4 colors? multicolor, compare
							for (x=0;x<8;x+=1) {
								unsigned char r, g, b;  long o=3*(cx*8+x+(cpr*8)*(8*cy+y)), f, j;
								r=p[o+0];  g=p[o+1];  b=p[o+2];
								f=0;  for (j=0;j<cc;j+=1) if (GetC64Index(&pngInfo,r,g,b)==ucTab[j]) f=1;
								if (f==0 && cc<4) {
									int i=GetC64Index(&pngInfo,r,g,b);  ucTab[cc++]=i;
									if (i!=bgc && i!=mc1 && i!=mc2) // dont touch flag "|64"!
										{ col&=~(15|128);  col|=i; }
									if ((col&128)==0 && (col&15)>=8) {
										printf("warning: block color invalid (%ld:%ld)\n",cx,cy);
									}
									//printf("c found:%d\n",ucTab[cc]);
								}
								if (f==0 && cc>4) {
									printf("warning: color overflow\n");
								}
								//if (r|g|b) v|=(1<<(7-x));
							}
							// mc or hi? look for colors to decide?
							//if (cy==1) printf("cx:%d mc:%d x:%d cc:%d 0:%d bgc:%d col:%d\n",
							//	cx,mc,y,cc,ucTab[0],bgc,col);
							for (x=0;x<cc;++x) if ((ucTab[x]!=bgc && ucTab[x]>=8)
								|| ((ucTab[x]==mc1 || ucTab[x]==mc2) && col&64)
								// mc colors in there and undecided yet? then it MUST be mc
								) { col&=~64;  mc=1; }
						}

						if (0==mig && 0==nes) for (y=0;y<8;y+=1) {
							for (x=0;x<8;x+=2) { // pass 2: mc or hi? look for hi pixels!
								unsigned char r1, g1, b1, r2, g2, b2;  long
									o1=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x),
									o2=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x+1);
								r1=p[o1+0];  g1=p[o1+1];  b1=p[o1+2];
								r2=p[o2+0];  g2=p[o2+1];  b2=p[o2+2];
								//printf("rgb:%d %d %d - %d %d %d\n",r1,g1,b1,r2,g2,b2);
								if (r1!=r2 || g1!=g2 || b1!=b2) { col&=~64;  hi=1;  break; }
							}
						}

						// hires block, but wrong BG color? then throw a warning
						if (hi) for (x=0;x<cc;++x) if (ucTab[x]!=bgc && ucTab[x]>=8) {
							printf("warning: wrong color in hires block(%ld:%ld)\n",cx,cy);
							break;
						}

						// use bgc as background or zero?
						tbgc=0;  if (0==mig && 0==nes && hi) for (y=0;y<8;y+=1)
							for (x=0;x<8;x+=1) {
							unsigned char r, g, b;  long o=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x);
							r=p[o+0];  g=p[o+1];  b=p[o+2];
							if (GetC64Index(&pngInfo,r,g,b)==bgc) tbgc=bgc;
						}

						 // Amiga
						//
						if (mig) {
							//printf("mig-c:%ld p:%d\n",pngInfo.colors,planes);
							for (y=0;y<8;y+=1) {
								long i, r, s;
								s=1;  for (r=0;r<planes;r+=1,s<<=1) {
									unsigned char v=0;
									for (x=0;x<8;x+=1) {
										unsigned char r, g, b;  long o=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										i=GetPNGIndex(&pngInfo,r,g,b);
										if (i&s) v|=(1<<(7-x));
									}
									memTab[mPos++]=v;
									//printf("%02x",v);
								}
								//printf("\n");
							}
							//exit(0);

						 // NES
						//
						} else if (nes) {
							int
								n,
								cEntries=0,
								pal=0,
								bf=1, // nesBackColOnly flag should be cleared if other color is there
								zf=1; // zero flag should be cleared when a bit is set
							signed char
								cTab[4]={-1,-1,-1,-1}, // found color indexes
								fTab[4]={0,0,0,0};    // amount of matching colors in palettes

							 // remark: all colors of a sprite seg are needed to
							// identify the palette number -> so we need 2 passes

							// pass 1: collect colors
							for (y=0;y<8;++y) for (x=0;x<8;++x) {
								unsigned char r, g, b, i, c, found;
								long o=3*(cy*cpr*8*8+cx*8+(cpr*8)*y+x);
								//long o=3*(
									//cy*cpr*8*8 +cpr*8*sy +cpr*8*y
									//+cx*8 +sx +x);
								r=p[o+0];  g=p[o+1];  b=p[o+2];
								i=ConvIndex(GetPNGIndex(&pngInfo,r,g,b)); // i := cleaned index
								// index already stored?
								found=0;  for (c=0;c<cEntries;++c) if
									(cTab[c]==i) { found=1;  break; }
								if (0==found && cEntries<4) cTab[cEntries++]=i;
							}

							// pass 2: count matching colors in all palettes and select palette
							bf=1;  for (n=0;n<4;++n) { // check 4 palettes
								for (x=0;x<cEntries;++x) { // check all found colors
									int found=0;  for (y=0;y<4;++y) { // compare 4 entries
										int i;
										if (0==y) i=0; else { i=n*3+y; } // BGCOL? PALCOL?
										if (cTab[x]==palTab[i]) {
											if (i) bf=0; // found color is not BG-col!
											found=1;  break;
										}
									}
									if (found) ++fTab[n]; // increase palette quality indicator
								}
							}
							// select palette with highest match count
							x=pal=0;  for (n=0;n<4;++n) if (fTab[n]>x) { x=fTab[n];  pal=n; }
							colTab[cPos]=pal;
							if (bf) colTab[cPos]|=nesBackColOnly; // := empty block
							//printf("[%d]",colTab[cPos]);
							++cPos;


							/*long cz=cx+cpr*cy, cs, n, v=0; //
							for (cs=0;cs<2;++cs) for (n=0;n<2;++n) { // seg / plane
								for (y=0;y<8;++y) { // for each byte in char 0..15
									v=0;  for (x=0;x<8;++x) { // bit
										unsigned char i, r, g, b;  long o;
										// might have memTab overflow problem, but works
										o=3*(cx*8+cy*(cpr*8*16)+cs*(cpr*8*8)+(cpr*8)*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										i=GetPNGIndex(&pngInfo,r,g,b);
										if (n) i>>=1;
										if (i) v|=(1<<(7-x));
									}
									memTab[mPos++]=v;
								}
							}*/
							// pass 3: set bits
							for (n=0;n<2;++n) { // plane
								for (y=0;y<8;++y) { // for each byte in char 0..7
									int v=0, j;
									for (x=0;x<8;++x) { // bit
										unsigned char s=0, i, r, g, b;//, f, j;
										long o=3*(cy*cpr*8*8+cx*8+(cpr*8)*y+x);
										r=p[o+0];  g=p[o+1];  b=p[o+2];
										//i=GetNESIndex(&pngInfo,r,g,b); // buggy
										//if (i&(1<<n)) v|=(1<<(7-x));
										i=ConvIndex(GetPNGIndex(&pngInfo,r,g,b));
										if (i==palTab[0]) s=0; else for // BG color? -> transparent
											(j=0;j<3;++j) if (i==palTab[1+pal*3+j])
											{ s=(j+1);  break; } // deep bit value "s" is 1..3
										if (s&(1<<n)) v|=(1<<(7-x));
									}
									if (v) zf=0;
									if (mPos<maxChars*maxCBF) memTab[mPos++]=v;
								}
							}
							cCount-=8;  if (cCount<0) cCount=0; // ?

						 // C64
						//
						} else {
							unsigned char i, hofTab[16], bf=1, sf=1; // is it a blank/solid block?
							for (y=0;y<16;++y) hofTab[y]=0; // clear hofTab
							for (y=0;y<8;y+=1) { // pass 3: build bitmap for actual char
								unsigned char v=0;
								if (hi==0) for (x=0;x<8;x+=2) { // *** "mc" *** - check for 00/01/10/11
									unsigned char r, g, b;  long
										o=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x);
									r=p[o+0];  g=p[o+1];  b=p[o+2];  i=GetC64Index(&pngInfo,r,g,b);
									if (i<16) ++hofTab[i];
									// cc is the amount of total colors (incl. bgc)
									if (cc>2) { col&=~64;  mc=1; }
									if (i==bgc) { sf=0; } // bgc
										else if ((0==tweak && i==mc1)
											|| (tweak && i==mc2)) { v|=(1<<(7-(x+1)));  sf=bf=0; }
										else if ((0==tweak && i==mc2)
											|| (tweak && i!=mc1 && i!=mc2)) { v|=(1<<(7-(x+0)));  sf=bf=0; }
										else if (0==tweak
											|| (tweak && i==mc1)) { v|=(1<<(7-(x+0)));  v|=(1<<(7-(x+1)));  bf=0; } // block color
								} else for (x=0;x<8;x+=1) { // *** "hi" ***
									unsigned char r, g, b;  long
										o=3*(cx*8+cy*(cpr*8*8)+(cpr*8)*y+x);
									r=p[o+0];  g=p[o+1];  b=p[o+2];
									if ((i=GetC64Index(&pngInfo,r,g,b))!=tbgc)
										{ v|=(1<<(7-x));  bf=0; } else sf=0;
								}
								if (i<16) ++hofTab[i];
								memTab[mPos++]=v;
								cCount-=1;  if (cCount<0) cCount=0;
							}
							// look for most used pixel here
							i=x=0;  for (y=0;y<16;++y) if (hofTab[y]>x) { i=y;  x=hofTab[y]; }
							pixTab[cPos]=i;
							//printf("<%d>",bf);
							if (mc) col|=8;
							if (bf) { col|=c64Blank; }
							if (sf) { col|=c64Solid; }
							colTab[cPos++]=col;
						}

						//if (cy==2) printf("%02ld.",col&15);
						//if ((cy==2) && cx==39) printf("\n");
						//if (cx>=0) break;

						  //
						 // check most pixel
						//
						//printf("m:%d:%d\n",mPos,cbf);

						  //
						 // check for duplicates?
						//
						mapChar=mPos/cbf;
						if (avoid && mPos>cbf) {
							int
								//off=(cx+cpr*cy)*cbf, //
								x, y, found=-1;
							//printf("check:%ld:%ld\n",cx,cy);

							for (x=0;x<mPos-cbf;x+=cbf) {
								int mismatch=0;
								//printf("mx:%ld:%d\n",mPos-cbf,x);
								for (y=0;y<cbf;++y) {
									//printf("y:%d:%d:%d\n",y,memTab[mPos-cbf+y],memTab[x+y]);
									if (memTab[mPos-cbf+y]!=memTab[x+y]) { mismatch=1;  break; }
								}
								if (0==mismatch) { found=x;  break; } // found a duplicate?
							}
							if (found!=-1) { // found! backtrace counters
								mapChar=(found/8)+1;
								//printf("found:%d:%d %d\n",cx,cy,found);
								mPos-=cbf;
								cPos-=1;
								cCount+=8;
							}
						}
						if (genMap && cx<maxMapW && cy<maxMapH) {
							//printf("m:%d\n",mapChar&255);
							unsigned char c=colTab[(mapChar-1)&255];
							//if (c&128) c|=8;
							bMap[cx+cpr*cy]=(mapChar-1)&255;
							cMap[cx+cpr*cy]=c&15;
							//if (cy==2) printf("[%d]",c&15);
							//if (cy==2 && cx==39) printf("\n");
						}
							//wMap[cx+cpr*cy]=mapChar;
						//if (cx>=4) break;
					}
					//if (cy>=0) break; // >=3 means: 0..3 !!
				}
				//printf("debug:%ld\n",mPos);

				  //
				 // post-loop afterprocessing
				//

				   // analyse colors - group to 2x2 char blocks
				  //  64 means: hi and mc would be possible
				 //  128 means: preset, no block color found
				//
				memset(tmpTab,0,sizeof tmpTab);  memcpy(tmpTab,colTab,qualAmount);
				if (0==nes && group) for (y=0;y<qualAmount/cpr+1;++y) for
					(x=0;x<cpr;++x) if (y*cpr+x<qualAmount) {
					unsigned char ucTab[4]={255,255,255,255}; // used colors - up to 4
					int c=tmpTab[y*cpr+x];
					if (c&(64|128)) {
						int t, cc, fc, u, v, xs, ys, c128, mcc;

						//if ((x==21 || x==22) && (y==2 || y==3))
						//	printf("p1:%d:%d\n",x,y);

						// pass 1: look for color (collect all block colors)
						xs=x/2;  xs*=2;  ys=y/2;  ys*=2;  fc=255;  cc=0;  c128=0;  mcc=0;
						for (v=0;v<2;++v) for (u=0;u<2;++u) {
							t=tmpTab[(ys+v)*cpr+(xs+u)];
							ucTab[cc++]=t;  if ((t&128)==0) fc=t;
								else c128+=1; // count entries with 128-flag
							if (t&64 || t&8) mcc+=1; // count entries which might be mc
						}
						if (fc&64 && mcc) fc|=8; // prefer multicolor if mc is set somewhere
						//if ((x==21 || x==22) && (y==2 || y==3))
						//	printf("debug-foundcolor:%d:%d:%d\n",fc,c128,mcc);

						// pass 2: check for color doubles
						v=cc;  cc=0;
						//if ((x==21 || x==22) && (y==2 || y==3))
						//	for (u=0;u<v;++u) printf("p2:%d\n",ucTab[u]);

						for (u=0;u<v;++u) if (
							(ucTab[u]&(15|16|128))==(fc&15) // exactly the same color?
							|| (ucTab[u]&64 && (ucTab[u]&7)==(fc&7)) // exactly the same color?
							|| ucTab[u]&(16|128)
							) ++cc; // "c" fixed?
						// caution: cc is 4 or !=4 - it does NOT represent the color-amount!

						//if ((x==10 || x==11) && (y==2 || y==3))
						//	{ printf("xy:%ld:%ld:%d:%d cc:%d mcc:%d\n",x,y,fc,v,cc,mcc);
						//	for (u=0;u<v;++u) printf("u:%d:%d\n",u,ucTab[u]); }

						// pass 3: copy color if everything's fine
						if (c128==4) { if (mcc==4) fc=8; else fc=0; } // empty 2x2 block?
						if (cc==4 && fc!=255) for (v=0;v<2;++v) for (u=0;u<2;++u) {
							colTab[(ys+v)*cpr+(xs+u)]&=~15;
							colTab[(ys+v)*cpr+(xs+u)]|=fc&15;
							tmpTab[(ys+v)*cpr+(xs+u)]&=~15;
							tmpTab[(ys+v)*cpr+(xs+u)]|=fc&15;

						}
					}
				}
				if (nes && group) for (y=0;y<qualAmount/cpr+1;y+=1) for
					(x=0;x<cpr;x+=1) if (y*cpr+x<qualAmount) {
					unsigned char ucTab[4]={255,255,255,255}; // used palettes - up to 4
					int t, cc, u, v, xs, ys;

					// char is a blank? then look for palette
					if (tmpTab[y*cpr+x]&nesBackColOnly) {

						// pass 1: collect all set palettes (except blanks)
						xs=x/2;  xs*=2;  ys=y/2;  ys*=2;  cc=0;
						for (v=0;v<2;++v) for (u=0;u<2;++u) {
							t=tmpTab[(ys+v)*cpr+(xs+u)];
							if (0==(t&nesBackColOnly)) ucTab[cc++]=t&3;
						}

						// pass 2: dected errors with non-blank chars with wrong palette

						if (cc) {// ok, we have found a palette
							colTab[y*cpr+x]=(ucTab[0]&3)|nesBackColOnly;
							tmpTab[y*cpr+x]=(ucTab[0]&3)|nesBackColOnly;
							//printf("%d:%d %d:%d\n",cc,ucTab[0],ucTab[1],ucTab[2]);
						}

					}
				}

				// analyse colors - reuse previous char color if none detected
				pCol=0; // reset previous color memory
				if (mode==4) for (y=0;y<charAmount/cpr+1;++y) for (x=0;x<cpr;++x) if
					(y*cpr+x<charAmount) {
					int c=colTab[y*cpr+x];
					if (c&128) { c&=~(15|128);  c|=pCol&15; }
					colTab[y*cpr+x]=c;
					pCol=c;
				}

				 // control-text output
				//
				if (debug && 0==mig) {
					int h=charAmount/cpr+1;  if (genMap) h=bh/8;
					printf("info: '#'(64) mc/hi=unset '@'(128) blkCol=unset '-'(16) blank\n");
					for (y=0;y<h;++y) for (x=0;x<cpr;++x) if (genMap || y*cpr+x<charAmount) {
						int c=colTab[y*cpr+x];
						if (c&16) printf("-"); else
						if (c&32) printf("!"); else // debug
						if (c&64) printf("#"); else
						if (c&128) printf("@"); else

						//c=pixTab[y*cpr+x]&15; // debug

						if ((c&128)==0 && (c>>4)) printf("%c",'A'+(int)(c>>4));
							else printf("%1x",(int)c&15);
							//else printf("-");
						//if (c&8) printf("<"); else printf(" ");
						if ((x&1)==1) printf(" ");
						if (x==cpr-1) printf("\n");
					}
				}

				// transfer |16 to |128
				// mehh... garbage! better move NES-flag to 16!
				/*if (0==mig && 0==nes) {
					for (y=0;y<charAmount/cpr+1;++y) for (x=0;x<cpr;++x) {
						unsigned char v=colTab[y*cpr+x];
						colTab[y*cpr+x]=v&(15|c64Blank|c64Solid);
						if (v&c64Blank) colTab[y*cpr+x]|=128;
					}
				}*/

				// check for mirrored chars?
				if (mode==3) for (y=0;y<charAmount/cpr+1;++y) for (x=0;x<cpr;++x) if
					(y*cpr+x<charAmount) { // pass 1: set flags
					colTab[y*cpr+x]&=15;
					colTab[y*cpr+x]|=(CheckCHR(mode,y*cpr+x)<<4);
				}
				// prepare rgb888 color-tab?
				if (mig) {
					PNGInfo* p=&pngInfo;  long n;
					cPos=0;
					for (n=0;n<p->colors;++n) {
						colTab[cPos++]=p->color[n*3+0];
						colTab[cPos++]=p->color[n*3+1];
						colTab[cPos++]=p->color[n*3+2];
					}
				}

				// this removes the |16 blank flag - pixTab should be written seperately
				/*if (nes==0 && mig==0) for (x=0;x<maxChars;++x) {
					colTab[x]&=15;  colTab[x]|=pixTab[x]<<4;
				}*/
				// write NES palette: 1..16 entries
				if (utility && !mig && (pFile=fopen(palName,"wb"))) {
					if (genMap) fwrite(cMap,1,cpr*tcl,pFile); else if (nes) {
						x=pPos;  if ((y=x)>4) y=4;
						if (y>0) fwrite(&palTab[0],1,y,pFile); // p1
						x-=4;  if ((y=x)>3) y=3;
						if (y>0) fwrite(&palTab[0],1,1,pFile);
						if (y>0) fwrite(&palTab[4],1,y,pFile); // p2
						x-=3;  if ((y=x)>3) y=3;
						if (y>0) fwrite(&palTab[0],1,1,pFile);
						if (y>0) fwrite(&palTab[7],1,y,pFile); // p3
						x-=3;  if ((y=x)>3) y=3;
						if (y>0) fwrite(&palTab[0],1,1,pFile);
						if (y>0) fwrite(&palTab[10],1,y,pFile); // p4
					} else {
						fwrite(&bgc,1,1,pFile);
						fwrite(&mc1,1,1,pFile);
						fwrite(&mc2,1,1,pFile);
					}
					fclose(pFile);  pFile=NULL;
				}
				if (genMap) { fwrite(bMap,1,cpr*tcl,mFile); }

				//for (y=0;y<192;y+=1) printf("[%d]",colTab[y]);
				//printf("\n%s\n",colName);

				if (utility && (cFile=fopen(colName,"wb"))) {
					fwrite(colTab,1,cPos,cFile);
					fclose(cFile);  cFile=NULL;
				}

				if (mode==3) { // only write crucial chars?
					int c;
					for (y=0;y<charAmount/cpr+1;++y) for (x=0;x<cpr;++x) if ((y*cpr+x<charAmount)
						&& (c=(colTab[y*cpr+x]>>4))==0
						) fwrite(&memTab[(y*cpr+x)*cbf],cbf,1,file);
				} else {
					int s=mPos;
					if (blankUnused && s<charAmount*cbf) s=charAmount*cbf;
					fwrite(memTab,1,s,file);
					//printf("mp:%d:%d\n",mPos,chars*cbf);
				}

				/*if (mode==4) for (y=0;y<csl;++y) {
					fwrite(&strTab[y][0],1,1,sFile);
					fwrite(&strTab[y][1],1,1,sFile);
					fwrite(&strTab[y][2],1,1,sFile);
				}*/

				if (source && (tFile=fopen(txtName,"wb"))) { // output source file? huh?
					int lc, i, n;
					char b[256];
					for (n=i=0;i<strlen(txtName);++i) {
						if (('a'<=txtName[i] && txtName[i]<='z')
							|| ('0'<=txtName[i] && txtName[i]<='9')
						) b[n++]=txtName[i];
						else if (txtName[i]=='.') break;
					}

					b[n]='\0';
					fprintf(tFile,"const unsigned char %sCHR[]={\n",b);
					lc=0;  for (i=0;i<charAmount*cbf;++i) {
						fprintf(tFile,"0x%02x",memTab[i]);
						if (i!=charAmount*cbf-1) fprintf(tFile,",");
						if (++lc>=16) { lc=0;  fprintf(tFile,"\n"); }
					}
					fprintf(tFile,"};\n");

					fprintf(tFile,"const unsigned char %sCOL[]={\n",b);
					lc=0;  for (i=0;i<cPos;++i) {
						fprintf(tFile,"0x%02x",colTab[i]);
						if (i!=cPos-1) fprintf(tFile,",");
						if (++lc>=16) { lc=0;  fprintf(tFile,"\n"); }
					}
					fprintf(tFile,"};\n");

					fclose(tFile);  tFile=NULL;
				}

				if (file) { fclose(file);  file=NULL; }
				//if (pFile) { fclose(pFile);  pFile=NULL; }
				//if (cFile) { fclose(cFile);  pFile=NULL; }
				//fclose(sFile);  sFile=NULL;
			}
			if (mFile) { fclose(mFile);  mFile=NULL; }
			if (png) { pngFree(png);  png=NULL; }
			if (convBuf) { free(convBuf);  convBuf=NULL; }

		} else printf("file not found\n");
		// cleanup
		if (workBuf) { free(workBuf);  workBuf=NULL; }
		break;
	}
	if (error) printf("%s\n",error);
	return 0;
}
