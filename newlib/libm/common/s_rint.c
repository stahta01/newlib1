
/* @(#)s_rint.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */
/*
FUNCTION
<<rint>>, <<rintf>>--round to integer
INDEX
	rint
INDEX
	rintf

ANSI_SYNOPSIS
	#include <math.h>
	double rint(double <[x]>);
	float rintf(float <[x]>);

DESCRIPTION
	The <<rint>> functions round their argument to an integer value in
	floating-point format, using the current rounding direction.  They
	raise the "inexact" floating-point exception if the result differs
	in value from the argument.  See the <<nearbyint>> functions for the
	same function with the "inexact" floating-point exception never being
	raised.  Newlib does not directly support floating-point exceptions.
	The <<rint>> functions are written so that the "inexact" exception is
	raised in hardware implementations that support it, even though Newlib
	does not provide access.

RETURNS
<[x]> rounded to an integral value, using the current rounding direction.

PORTABILITY
ANSI C, POSIX

SEEALSO
<<nearbyint>>, <<round>>

*/

/*
 * rint(x)
 * Return x rounded to integral value according to the prevailing
 * rounding mode.
 * Method:
 *	Using floating addition.
 * Exception:
 *	Inexact flag raised if x not equal to rint(x).
 */

#include "fdlibm.h"

#ifndef _DOUBLE_IS_32BITS

#ifdef __STDC__
static const double
#else
static double 
#endif
TWO52[2]={
  4.50359962737049600000e+15, /* 0x43300000, 0x00000000 */
 -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

#ifdef __STDC__
	double rint(double x)
#else
	double rint(x)
	double x;
#endif
{
	__int32_t i0,j0,sx;
	__uint32_t i,i1;
	double t;
	volatile double w;
	EXTRACT_WORDS(i0,i1,x);
	sx = (i0>>31)&1;
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
	    if(j0<0) { 	
		if(((i0&0x7fffffff)|i1)==0) return x;
		i1 |= (i0&0x0fffff);
		i0 &= 0xfffe0000;
		i0 |= ((i1|-i1)>>12)&0x80000;
		SET_HIGH_WORD(x,i0);
	        w = TWO52[sx]+x;
	        t =  w-TWO52[sx];
		GET_HIGH_WORD(i0,t);
		SET_HIGH_WORD(t,(i0&0x7fffffff)|(sx<<31));
	        return t;
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) return x; /* x is integral */
		i>>=1;
		if(((i0&i)|i1)!=0) {
		    if(j0==19) i1 = 0x40000000; else
		    i0 = (i0&(~i))|((0x20000)>>j0);
		}
	    }
	} else if (j0>51) {
	    if(j0==0x400) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	} else {
	    i = ((__uint32_t)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) return x;	/* x is integral */
	    i>>=1;
	    if((i1&i)!=0) i1 = (i1&(~i))|((0x40000000)>>(j0-20));
	}
	INSERT_WORDS(x,i0,i1);
	w = TWO52[sx]+x;
	return w-TWO52[sx];
}

#endif /* _DOUBLE_IS_32BITS */



