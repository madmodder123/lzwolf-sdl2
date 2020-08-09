// Misc stuff to transition from ZDoom to Wolf

#ifndef __ZDOOM_SUPPORT__
#define __ZDOOM_SUPPORT__

#include "m_crc32.h"
#include "templates.h"
#include "basictypes.h"

#include <cstring>

#ifdef __ANDROID__
#include <android/log.h>
#define printf(...) __android_log_print(ANDROID_LOG_INFO,"LZWolf",__VA_ARGS__)
#endif

int ParseHex(const char* hex);

// Ensures that a double is a whole number or a half
static inline bool CheckTicsValid(double tics)
{
	double ipart;
	double fpart = modf(tics, &ipart);
	if(MIN(fabs(fpart), fabs(0.5 - fpart)) > 0.0001)
		return false;
	return true;
}

static inline char* copystring(const char* s)
{
	char *b;
	if (s)
	{
		size_t len = strlen (s) + 1;
		b = new char[len];
		memcpy (b, s, len);
	}
	else
	{
		b = new char[1];
		b[0] = '\0';
	}
	return b;
}

static inline char *ncopystring (const char *string)
{
	if (string == NULL || string[0] == 0)
	{
		return NULL;
	}
	return copystring (string);
}

static inline void ReplaceString(char* &ptr, const char* str)
{
	if(ptr)
	{
		if(ptr == str)
			return;
		delete[] ptr;
	}
	ptr = copystring(str);
}

static inline unsigned int MakeKey(const char *s, size_t len)
{
	BYTE* hashString = new BYTE[len];
	memcpy(hashString, s, len);
	for(size_t i = 0;i < len;++i)
		hashString[i] = tolower(*s++);
	const DWORD ret = CalcCRC32(hashString, static_cast<unsigned int>(len));
	delete[] hashString;
	return ret;
}
static inline unsigned int MakeKey(const char *s) { return MakeKey(s, strlen(s)); }

// Technically T should always be FString, but we use a template to avoid a
// circular dependency issue.
template<class T> void FixPathSeperator (T &path) { path.ReplaceChars('\\', '/'); }

#define countof(x) (sizeof(x)/sizeof(x[0]))
#ifndef __BIG_ENDIAN__
#define MAKE_ID(a,b,c,d)	((DWORD)((a)|((b)<<8)|((c)<<16)|((d)<<24)))
#else
#define MAKE_ID(a,b,c,d)	((DWORD)((d)|((c)<<8)|((b)<<16)|((a)<<24)))
#endif

#define MAXWIDTH 5120
//#define Printf printf
#define I_FatalError Quit
void I_Error(const char* format, ...);

#define MulScale16(x,y) (SDWORD((SQWORD(x)*SQWORD(y))>>16))

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define STACK_ARGS __cdecl
#else
#define STACK_ARGS
#endif

// [RH] This gets used all over; define it here:
int STACK_ARGS Printf (int printlevel, const char *, ...) GCCPRINTF(2,3);
int STACK_ARGS Printf (const char *, ...) GCCPRINTF(1,2);

// [RH] Same here:
int STACK_ARGS DPrintf (const char *, ...) GCCPRINTF(1,2);

#endif /* __ZDOOM_SUPPORT__ */
