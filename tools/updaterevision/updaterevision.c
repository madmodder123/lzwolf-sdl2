/* updaterevision.c
 *
 * Public domain. This program uses the svnversion command to get the
 * repository revision for a particular directory and writes it into
 * a header file so that it can be used as a project's build number.
 */

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
// [BB] New #includes.
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Used to strip newline characters from lines read by fgets.
void stripnl(char *str)
{
	if (*str != '\0')
	{
		size_t len = strlen(str);
		if (str[len - 1] == '\n')
		{
			str[len - 1] = '\0';
		}
	}
}

int main(int argc, char **argv)
{
	char vertag[128] = { 0 }, lastlog[128] = { 0 }, lasthash[128] = { 0 }, *hash = NULL;
	FILE *stream = NULL;
	int gotrev = 0, needupdate = 1;
	char hgdateString[128] = { 0 };
	time_t hgdate = 0;
	char hgHash[13];
	hgHash[0] = '\0';

	vertag[0] = '\0';
	lastlog[0] = '\0';

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <path to hginfo.h>\n", argv[0]);
		return 1;
	}

	// Use commit date and short hash
	stream = popen("hg log --config trusted.users=* -r. --template \"{latesttag}-{latesttagdistance}-{node|short}\\n{node|short}\\n{date|hgdate}\"", "r");

	if (NULL != stream)
	{
		if (fgets(vertag, sizeof vertag, stream) == vertag &&
			fgets(lastlog, sizeof lastlog, stream) == lastlog &&
			fgets(hgdateString, sizeof hgdateString, stream) == hgdateString)
		{
			stripnl(vertag);
			stripnl(lastlog);
			stripnl(hgdateString);
			gotrev = 1;
		}

		pclose(stream);
	}

	if (gotrev)
	{
		struct tm *lt;

		hash = lastlog;

		hgdate = atoi ( hgdateString );
		lt = gmtime( &hgdate );
		sprintf ( hgdateString, "%d%02d%02d-%02d%02d", lt->tm_year - 100, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min);
	}
	if (hash == NULL)
	{
		fprintf(stderr, "Failed to get commit info: %s\n", strerror(errno));
		strcpy(vertag, "<unknown version>");
		lastlog[0] = '\0';
		lastlog[1] = '0';
		lastlog[2] = '\0';
		hash = lastlog + 1;
	}

	stream = fopen (argv[1], "r");
	if (stream != NULL)
	{
		if (!gotrev)
		{ // If we didn't get a revision but the file does exist, leave it alone.
			fclose (stream);
			return 0;
		}
		// Read the revision that's in this file already. If it's the same as
		// what we've got, then we don't need to modify it and can avoid rebuilding
		// dependant files.
		if (fgets(lasthash, sizeof lasthash, stream) == lasthash)
		{
			stripnl(lasthash);
			if (strcmp(hash, lasthash + 3) == 0)
			{
				needupdate = 0;
			}
		}
		fclose (stream);
	}

	if (needupdate)
	{
		stream = fopen (argv[1], "w");
		if (stream == NULL)
		{
			return 1;
		}
		fprintf(stream,
"// %s\n"
"//\n"
"// This file was automatically generated by the\n"
"// updaterevision tool. Do not edit by hand.\n"
"\n"
"#define HG_DESCRIPTION \"%s\"\n"
"#define HG_HASH \"%s\"\n"
"#define HG_TIME \"%s\"\n"
"#define HG_TIME_INT %lluull\n",
			hash, vertag, hash, hgdateString, (unsigned long long)hgdate);
		fclose(stream);
		fprintf(stderr, "%s updated to commit %s (%s).\n", argv[1], hgdateString, vertag);
	}
	else
	{
		fprintf (stderr, "%s is up to date at commit %s (%s).\n", argv[1], hgdateString, vertag);
	}

	return 0;
}
