// lazyNES lazyhello demo

#include "lazynes.h"

int main() {

	static const ubyte bgColors[]={2,33};
	static const char text[]="HELLO LAZYNES!";
	
	lnSync(lfBlank);                 // blank screen to enable lnPush()
	lnPush(lnBackCol,2,bgColors);   // set colors, always directly after lnSync()
	lnPush(lnNameTab0+32,14,text); // draw text in 2nd line
	lnSync(0);                    // sync with vblank, unblank screen

	return 0;
}
