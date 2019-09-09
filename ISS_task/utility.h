#pragma once

#ifndef _UTILITY_H_
#define _UTILITY_H_

inline unsigned int convertBytesToUintLittleEndian(const char *bytes)
{
	return ((bytes[3] & 0xff) << 24) | ((bytes[2] & 0xff) << 16) | ((bytes[1] & 0xff) << 8) | (bytes[0] & 0xff);
}

inline const char *memstr(const char* haystack, const char* needle, size_t size)
{
	const char *p;
	const char *hs = haystack;
	size_t needlesize = strlen(needle);

	for (p = haystack; p + needlesize <= hs + size; p++) 
	{
		if (memcmp(p, needle, needlesize) == 0) 
			return p;
	}
	return NULL;
}

#endif
