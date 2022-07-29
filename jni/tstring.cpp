// stdafx.cpp : source file that includes just the standard includes
//	VCal5beta.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "tstring.h"

// PORTABLE

int toupp(int i)
{
	if (i >= 'a' && i <= 'z')
		return i - 32;
	return i;
}

char * AvcGetTextBufferBig(void)
{
	static char szBuffer[2048];
	
	return szBuffer;
}

FILE * g_fLogFile = NULL;

void LogClose()
{
	if (g_fLogFile != NULL)
	{
		fclose(g_fLogFile);
		g_fLogFile = NULL;
	}
}

void LogStart(char *pLogFile)
{
	LogClose();
	g_fLogFile = fopen(pLogFile, "w");
	if (g_fLogFile == NULL)
	{
		fprintf(stderr, "LogFile failed to open.\n");
	}
}

void Log(char *fmt, ...)
{
	static char szBuff[1024];
	if (g_fLogFile != NULL)
	{
		va_list argp;
		va_start(argp, fmt);
		vsprintf(szBuff, fmt, argp);
		va_end(argp);
		fprintf(g_fLogFile, "%s", szBuff);
		putc('\n', g_fLogFile);
		fprintf(stderr, "%s", szBuff);
		putc('\n', stderr);
	}
}

