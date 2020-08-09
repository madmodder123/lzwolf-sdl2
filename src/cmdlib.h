// cmdlib.h

#ifndef __CMDLIB__
#define __CMDLIB__


//#include "doomtype.h"
//#include "doomdef.h"
//#include "m_fixed.h"
#include "wl_def.h"
#include "zstring.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

// the dec offsetof macro doesnt work very well...
#define myoffsetof(type,identifier) ((size_t)&((type *)1)->identifier - 1)

int		Q_filelength (FILE *f);
bool FileExists (const char *filename);
bool DirEntryExists (const char *pathname);

extern	FString progdir;

void 	DefaultExtension (char *path, const char *extension);
void 	DefaultExtension (FString &path, const char *extension);

FString	ExtractFilePath (const char *path);
FString	ExtractFileBase (const char *path, bool keep_extension=false);

int 	ParseNum (const char *str);
bool	IsNum (const char *str);		// [RH] added

bool CheckWildcards (const char *pattern, const char *text);

//void FormatGUID (char *buffer, size_t buffsize, const GUID &guid);

const char *myasctime ();

int strbin (char *str);
FString strbin1 (const char *start);
char *CleanseString (char *str);

void CreatePath(const char * fn);

FString ExpandEnvVars(const char *searchpathstring);
FString NicePath(const char *path);

struct FFileList
{
	FString Filename;
	bool isDirectory;
};

void ScanDirectory(TArray<FFileList> &list, const char *dirpath);

#endif
