/*****************************************************************************\
** mandelbrot sample program for cc65.                                       **
**                                                                           **
** (w) 2002 by groepaz/hitmen, TGI support by Stefan Haubenthal              **
\*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#if 0
#include <conio.h>
#include <tgi.h>
#include <cc65.h>
#endif

#define fabs(x) ((x)>=0?(x):-(x))

/* Graphics definitions */
#ifndef SCREEN_X
#define SCREEN_X        40
#endif
#ifndef SCREEN_Y
#define SCREEN_Y        25
#endif
#define MAXCOL          16

#define maxiterations   32

/* Workaround missing clock stuff */
#ifdef __APPLE2__
#  define clock()       0
#  define CLK_TCK       1
#endif

/* Use dynamically loaded driver by default */
#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

char color;

#define tgi_setcolor(x) color=(x)

void tgi_setpixel(x,y){
#ifdef C64
 static char *p=(char*)0x400,*c=(char*)0xd800;
 if(color==0) *p=' '; else *p=224;
 *c=color;
 p++;c++;
#else
 if(x==SCREEN_X-1&&y==SCREEN_Y-1) return;
 putchar(color?'A'+color:' ');
#ifdef DOUBLE
 putchar(color?'A'+color:' ');
#endif
#endif
}


/* Use static local variables for speed */
#if 0
#pragma static-locals (1);
#endif


void mandelbrot (float x1, float y1, float x2,
                 float y2)
{
    register unsigned char count;
    register float r, r1, i;
    register float xs, ys, xx, yy;
    register float x, y;

    /* Calc stepwidth */
    xs = ((x2 - x1) / (SCREEN_X));
    ys = ((y2 - y1) / (SCREEN_Y));

    yy = y1;
    for (y = 0; y < (SCREEN_Y); y++) {
        yy += ys;
        xx = x1;
        for (x = 0; x < (SCREEN_X); x++) {
            xx += xs;
            /* Do iterations */
            r = 0;
            i = 0;
            for (count = 0; (count < maxiterations) &&
                 (fabs (r) < 2.) && (fabs (i) < 2.);
                 ++count) {
                r1 = (r*r - i*i) + xx;
                /* i = (mulfp(mulfp(r,i),tofp(2)))+yy; */
                i = r*i*2. + yy;
                r = r1;
            }
            if (count == maxiterations) {
                tgi_setcolor (0);
            } else {
                if (MAXCOL == 2) {
                    tgi_setcolor (1);
                } else {
                    tgi_setcolor (count % MAXCOL);
                }
            }
            /* Set pixel */
            tgi_setpixel (x, y);
        }
    }
}

int main (void)
{
    clock_t t;
    unsigned long sec;
    unsigned sec10;
    unsigned char err;

#if 0
    clrscr ();

#if DYN_DRV
    /* Load the graphics driver */
    cprintf ("initializing... mompls\r\n");
    tgi_load_driver (tgi_stddrv);
#else
    /* Install the graphics driver */
    tgi_install (tgi_static_stddrv);
#endif
    err = tgi_geterror ();
    if (err  != TGI_ERR_OK) {
        cprintf ("Error #%d initializing graphics.\r\n%s\r\n",
                 err, tgi_geterrormsg (err));
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    };
    cprintf ("ok.\n\r");

    /* Initialize graphics */
    tgi_init ();
    tgi_clear ();
#endif

    t = clock ();

    /* Calc mandelbrot set */
    mandelbrot (-2., -2.,2.,2.);
    fflush(stdout);

    t = clock () - t;
#if 0
    /* Fetch the character from the keyboard buffer and discard it */
    cgetc ();

    /* Shut down gfx mode and return to textmode */
    tgi_done ();
    /* Calculate stats */
    sec = (t * 10) / CLK_TCK;
    sec10 = sec % 10;
    sec /= 10;

    /* Output stats */
    printf ("time  : %lu.%us\n\r", sec, sec10);
    if (doesclrscrafterexit ()) {
        /* Wait for a key, then end */
        cputs ("Press any key when done...\n\r");
        cgetc ();
    }
#endif
    /* Done */
    return EXIT_SUCCESS;
}
