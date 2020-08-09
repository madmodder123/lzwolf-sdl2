/*
** zstring.cpp
** A dynamically-allocated string class.
**
**---------------------------------------------------------------------------
** Copyright 2005-2008 Randy Heit
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**---------------------------------------------------------------------------
**
*/

#include <algorithm>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <new>		// for bad_alloc

#include "zstring.h"

FString::FString (const FString &other) : LockCount(0)
{
	AttachToOther(other);
}

FString::FString (const char *copyStr) : LockCount(0)
{
	if (copyStr == NULL || *copyStr == '\0')
	{
		CharsPtr = std::make_shared<std::string>();
	}
	else
	{
		CharsPtr = std::make_shared<std::string>(copyStr);
	}
}

FString::FString (const char *copyStr, size_t len) : LockCount(0)
{
	CharsPtr = std::make_shared<std::string>(copyStr, len);
}

FString::FString (char oneChar) : LockCount(0)
{
	if (oneChar == '\0')
	{
		CharsPtr = std::make_shared<std::string>();
	}
	else
	{
		CharsPtr = std::make_shared<std::string>(1, oneChar);
	}
}

FString::FString (const FString &head, const FString &tail) : LockCount(0)
{
	auto cat = *(head.CharsPtr) + *(tail.CharsPtr);
	CharsPtr = std::make_shared<std::string>(cat);
}

FString::FString (const FString &head, const char *tail) : LockCount(0)
{
	auto cat = *(head.CharsPtr) + tail;
	CharsPtr = std::make_shared<std::string>(cat);
}

FString::FString (const FString &head, char tail) : LockCount(0)
{
	auto cat = *(head.CharsPtr) + tail;
	CharsPtr = std::make_shared<std::string>(cat);
}

FString::FString (const char *head, const FString &tail) : LockCount(0)
{
	auto cat = std::string(head) + *(tail.CharsPtr);
	CharsPtr = std::make_shared<std::string>(cat);
}

FString::FString (const char *head, const char *tail) : LockCount(0)
{
	auto cat = std::string(head) + std::string(tail);
	CharsPtr = std::make_shared<std::string>(cat);
}

FString::FString (char head, const FString &tail) : LockCount(0)
{
	auto cat = std::string(1, head) + *(tail.CharsPtr);
	CharsPtr = std::make_shared<std::string>(cat);
}

char *FString::LockNewBuffer(size_t len)
{
	CharsPtr = std::make_shared<std::string>('\0', len);
	LockCount = 1;
	return &(*CharsPtr)[0];
}

void FString::CopyOnWrite()
{
	if (CharsPtr.use_count() > 1)
	{
		CharsPtr = std::make_shared<std::string>(*CharsPtr);
	}
}

char *FString::LockBuffer()
{
	CopyOnWrite();
	LockCount++;
	return &(*CharsPtr)[0];
}

void FString::UnlockBuffer()
{
	assert(LockCount > 0);
	LockCount--;
}

void FString::AttachToOther(const FString &other)
{
	if (&other != this)
	{
		if (other.LockCount > 0)
		{
			CharsPtr = std::make_shared<std::string>(*(other.CharsPtr));
		}
		else
		{
			CharsPtr = other.CharsPtr;
		}
	}
}

FString &FString::operator = (const FString &other)
{
	AttachToOther(other);
	return *this;
}

FString &FString::operator = (const char *copyStr)
{
	if (copyStr == NULL || *copyStr == '\0')
	{
		CharsPtr = std::make_shared<std::string>();
	}
	else
	{
		CharsPtr = std::make_shared<std::string>(copyStr);
	}
	return *this;
}

void FString::Format (const char *fmt, ...)
{
	va_list arglist;
	va_start (arglist, fmt);
	VFormat (fmt, arglist);
	va_end (arglist);
}

void FString::AppendFormat (const char *fmt, ...)
{
	va_list arglist;
	va_start (arglist, fmt);
	StringFormat::VWorker (FormatHelper, this, fmt, arglist);
	va_end (arglist);
}

void FString::VFormat (const char *fmt, va_list arglist)
{
	CharsPtr = std::make_shared<std::string>();
	StringFormat::VWorker (FormatHelper, this, fmt, arglist);
}

void FString::VAppendFormat (const char *fmt, va_list arglist)
{
	StringFormat::VWorker (FormatHelper, this, fmt, arglist);
}

int FString::FormatHelper (void *data, const char *cstr, int len)
{
	FString *str = (FString *)data;
	str->CopyOnWrite();
	str->AppendCStrPart(cstr, len);
	return len;
}

FString FString::operator + (const FString &tail) const
{
	return FString (*this, tail);
}

FString FString::operator + (const char *tail) const
{
	return FString (*this, tail);
}

FString operator + (const char *head, const FString &tail)
{
	return FString (head, tail);
}

FString FString::operator + (char tail) const
{
	return FString (*this, tail);
}

FString operator + (char head, const FString &tail)
{
	return FString (head, tail);
}

FString &FString::operator += (const FString &tail)
{
	CopyOnWrite();
	(*CharsPtr) += *(tail.CharsPtr);
	return *this;
}

FString &FString::operator += (const char *tail)
{
	CopyOnWrite();
	(*CharsPtr) += tail;
	return *this;
}

FString &FString::operator += (char tail)
{
	CopyOnWrite();
	(*CharsPtr) += tail;
	return *this;
}

FString &FString::AppendCStrPart (const char *tail, size_t tailLen)
{
	CopyOnWrite();
	(*CharsPtr) += std::string(tail, tailLen);
	return *this;
}

void FString::Truncate (long newlen)
{
	if (newlen >= 0 && newlen < (long)Len())
	{
		CopyOnWrite();
		(*CharsPtr).erase(newlen);
	}
}

FString FString::Left (size_t numChars) const
{
	size_t len = Len();
	if (len < numChars)
	{
		numChars = len;
	}
	return FString (CharsPtr->c_str(), numChars);
}

FString FString::Right (size_t numChars) const
{
	size_t len = Len();
	if (len < numChars)
	{
		numChars = len;
	}
	return FString (CharsPtr->c_str() + len - numChars, numChars);
}

FString FString::Mid (size_t pos, size_t numChars) const
{
	size_t len = Len();
	if (pos >= len)
	{
		return FString();
	}
	if (pos + numChars > len || pos + numChars < pos)
	{
		numChars = len - pos;
	}
	return FString (CharsPtr->c_str() + pos, numChars);
}

long FString::IndexOf (const FString &substr, long startIndex) const
{
	return IndexOf (substr.CharsPtr->c_str(), startIndex);
}

long FString::IndexOf (const char *substr, long startIndex) const
{
	if (startIndex > 0 && Len() <= (size_t)startIndex)
	{
		return -1;
	}
	const char *str = strstr (CharsPtr->c_str() + startIndex, substr);
	if (str == NULL)
	{
		return -1;
	}
	return long(str - CharsPtr->c_str());
}

long FString::IndexOf (char subchar, long startIndex) const
{
	if (startIndex > 0 && Len() <= (size_t)startIndex)
	{
		return -1;
	}
	const char *str = strchr (CharsPtr->c_str() + startIndex, subchar);
	if (str == NULL)
	{
		return -1;
	}
	return long(str - CharsPtr->c_str());
}

long FString::IndexOfAny (const FString &charset, long startIndex) const
{
	return IndexOfAny (charset.CharsPtr->c_str(), startIndex);
}

long FString::IndexOfAny (const char *charset, long startIndex) const
{
	if (startIndex > 0 && Len() <= (size_t)startIndex)
	{
		return -1;
	}
	const char *brk = strpbrk (CharsPtr->c_str() + startIndex, charset);
	if (brk == NULL)
	{
		return -1;
	}
	return long(brk - CharsPtr->c_str());
}

long FString::LastIndexOf (const FString &substr) const
{
	return LastIndexOf (substr.CharsPtr->c_str(), long(Len()), substr.Len());
}

long FString::LastIndexOf (const char *substr) const
{
	return LastIndexOf (substr, long(Len()), strlen(substr));
}

long FString::LastIndexOf (char subchar) const
{
	return LastIndexOf (subchar, long(Len()));
}

long FString::LastIndexOf (const FString &substr, long endIndex) const
{
	return LastIndexOf (substr.CharsPtr->c_str(), endIndex, substr.Len());
}

long FString::LastIndexOf (const char *substr, long endIndex) const
{
	return LastIndexOf (substr, endIndex, strlen(substr));
}

long FString::LastIndexOf (char subchar, long endIndex) const
{
	if ((size_t)endIndex > Len())
	{
		endIndex = long(Len());
	}
	while (--endIndex >= 0)
	{
		if ((*CharsPtr)[endIndex] == subchar)
		{
			return endIndex;
		}
	}
	return -1;
}

long FString::LastIndexOf (const char *substr, long endIndex, size_t substrlen) const
{
	if ((size_t)endIndex > Len())
	{
		endIndex = long(Len());
	}
	substrlen--;
	while (--endIndex >= long(substrlen))
	{
		if (strncmp (substr, CharsPtr->c_str() + endIndex - substrlen, substrlen + 1) == 0)
		{
			return endIndex;
		}
	}
	return -1;
}

long FString::LastIndexOfAny (const FString &charset) const
{
	return LastIndexOfAny (charset.CharsPtr->c_str(), long(Len()));
}

long FString::LastIndexOfAny (const char *charset) const
{
	return LastIndexOfAny (charset, long(Len()));
}

long FString::LastIndexOfAny (const FString &charset, long endIndex) const
{
	return LastIndexOfAny (charset.CharsPtr->c_str(), endIndex);
}

long FString::LastIndexOfAny (const char *charset, long endIndex) const
{
	if ((size_t)endIndex > Len())
	{
		endIndex = long(Len());
	}
	while (--endIndex >= 0)
	{
		if (strchr (charset, (*CharsPtr)[endIndex]) != NULL)
		{
			return endIndex;
		}
	}
	return -1;
}

void FString::ToUpper ()
{
	LockBuffer();
	auto& s = *CharsPtr;
	std::transform(std::begin(s), std::end(s), std::begin(s), toupper);
	UnlockBuffer();
}

void FString::ToLower ()
{
	LockBuffer();
	auto& s = *CharsPtr;
	std::transform(std::begin(s), std::end(s), std::begin(s), tolower);
	UnlockBuffer();
}

void FString::SwapCase ()
{
	LockBuffer();
	auto xf = [](char ch) {
		return isupper(ch) ? tolower(ch) : toupper(ch);
	};
	auto& s = *CharsPtr;
	std::transform(std::begin(s), std::end(s), std::begin(s), xf);
	UnlockBuffer();
}

void FString::Insert (size_t index, const FString &instr)
{
	Insert (index, instr.CharsPtr->c_str(), instr.Len());
}

void FString::Insert (size_t index, const char *instr)
{
	Insert (index, instr, strlen(instr));
}

void FString::Insert (size_t index, const char *instr, size_t instrlen)
{
	size_t mylen = Len();
	if (index > mylen)
	{
		index = mylen;
	}
	CopyOnWrite();
	(*CharsPtr).insert(index, instr, instrlen);
}

void FString::ReplaceChars (char oldchar, char newchar)
{
	LockBuffer();
	std::replace(std::begin(*CharsPtr), std::end(*CharsPtr), oldchar, newchar);
	UnlockBuffer();
}

void FString::ReplaceChars (const char *oldcharset, char newchar)
{
	LockBuffer();
	auto xf = [oldcharset,newchar](char ch) {
		return (strchr (oldcharset, ch) != NULL) ? newchar : ch;
	};
	auto& s = *CharsPtr;
	std::transform(std::begin(s), std::end(s), std::begin(s), xf);
	UnlockBuffer();
}

void FString::StripChars (char killchar)
{
	LockBuffer();
	auto& s = *CharsPtr;
	s.erase(std::remove_if(std::begin(s), std::end(s), [killchar](char ch) {
		return ch == killchar; }), std::end(s));
	UnlockBuffer();
}

void FString::StripChars (const char *killchars)
{
	size_t read, write, mylen;

	LockBuffer();
	auto& s = *CharsPtr;
	s.erase(std::remove_if(std::begin(s), std::end(s), [killchars](char ch) {
		return strchr (killchars, ch); }), std::end(s));
	UnlockBuffer();
}

bool FString::IsInt () const
{
	// String must match: [whitespace] [{+ | �}] [0 [{ x | X }]] [digits] [whitespace]

/* This state machine is based on a simplification of re2c's output for this input:
digits		= [0-9];
hexdigits	= [0-9a-fA-F];
octdigits	= [0-7];

("0" octdigits+ | "0" [xX] hexdigits+ | (digits \ '0') digits*) { return true; }
[\000-\377] { return false; }*/
	const char *YYCURSOR = (*CharsPtr).c_str();
	char yych;

	yych = *YYCURSOR;

	// Skip preceding whitespace
	while (yych != '\0' && isspace(yych)) { yych = *++YYCURSOR; }

	// Check for sign
	if (yych == '+' || yych == '-') { yych = *++YYCURSOR; }

	if (yych == '0')
	{
		yych = *++YYCURSOR;
		if (yych >= '0' && yych <= '7')
		{
			do { yych = *++YYCURSOR; } while (yych >= '0' && yych <= '7');
		}
		else if (yych == 'X' || yych == 'x')
		{
			bool gothex = false;
			yych = *++YYCURSOR;
			while ((yych >= '0' && yych <= '9') || (yych >= 'A' && yych <= 'F') || (yych >= 'a' && yych <= 'f'))
			{
				gothex = true;
				yych = *++YYCURSOR;
			}
			if (!gothex) return false;
		}
		else
		{
			return false;
		}
	}
	else if (yych >= '1' && yych <= '9')
	{
		do { yych = *++YYCURSOR; } while (yych >= '0' && yych <= '9');
	}
	else
	{
		return false;
	}

	// The rest should all be whitespace
	while (yych != '\0' && isspace(yych)) { yych = *++YYCURSOR; }
	return yych == '\0';
}

bool FString::IsFloat () const
{
	// String must match: [whitespace] [sign] [digits] [.digits] [ {d | D | e | E}[sign]digits] [whitespace]
/* This state machine is based on a simplification of re2c's output for this input:
digits		= [0-9];

(digits+ | digits* "." digits+) ([dDeE] [+-]? digits+)? { return true; }
[\000-\377] { return false; }
*/
	const char *YYCURSOR = (*CharsPtr).c_str();
	char yych;
	bool gotdig = false;

	yych = *YYCURSOR;

	// Skip preceding whitespace
	while (yych != '\0' && isspace(yych)) { yych = *++YYCURSOR; }

	// Check for sign
	if (yych == '+' || yych == '-') { yych = *++YYCURSOR; }

	while (yych >= '0' && yych <= '9')
	{
		gotdig = true;
		yych = *++YYCURSOR;
	}
	if (yych == '.')
	{
		yych = *++YYCURSOR;
		if (yych >= '0' && yych <= '9')
		{
			gotdig = true;
			do { yych = *++YYCURSOR; } while (yych >= '0' && yych <= '9');
		}
		else return false;
	}
	if (gotdig)
	{
		if (yych == 'D' || yych == 'd' || yych == 'E' || yych == 'e')
		{
			yych = *++YYCURSOR;
			if (yych == '+' || yych == '-') yych = *++YYCURSOR;
			while (yych >= '0' && yych <= '9') { yych = *++YYCURSOR; }
		}
	}

	// The rest should all be whitespace
	while (yych != '\0' && isspace(yych)) { yych = *++YYCURSOR; }
	return yych == '\0';
}

long FString::ToLong (int base) const
{
	return strtol ((*CharsPtr).c_str(), NULL, base);
}

unsigned long FString::ToULong (int base) const
{
	return strtoul ((*CharsPtr).c_str(), NULL, base);
}

double FString::ToDouble () const
{
	return strtod ((*CharsPtr).c_str(), NULL);
}
