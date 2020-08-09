// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.
//
// This file is based on pragmas.h from Ken Silverman's original Build
// source code release but is meant for use with any compiler and does not
// rely on any inline assembly.
//

#if _MSC_VER
#pragma once
#endif

#if defined(__GNUC__) && !defined(__forceinline)
#define __forceinline __inline__ __attribute__((always_inline))
#endif

static __forceinline SDWORD Scale (SDWORD a, SDWORD b, SDWORD c)
{
	return (SDWORD)(((SQWORD)a*b)/c);
}

static __forceinline SDWORD MulScale (SDWORD a, SDWORD b, SDWORD c)
{
	return (SDWORD)(((SQWORD)a*b)>>c);
}
