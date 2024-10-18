// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef ___MAC_TString_H_AFX____
#define ___MAC_TString_H_AFX____

#pragma GCC diagnostic ignored "-Wtautological-compare"
#pragma GCC diagnostic ignored "-Wunused-value"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "platform.h"
// PORTABLE

char * AvcGetTextBufferBig(void);

int toupp(int);

class TString
{
	char * m_pData;
	UInt32 m_nSize;
	UInt32 m_nLength;
	
public:
	TString()
	{
		m_pData = new char[64];
		if (m_pData)
		{
			m_nSize = 64;
			m_nLength = 0;
			m_pData[0] = 0;
		}
		else
		{
			m_nSize = 0;
		}
	}
	~TString()
	{
		if (m_pData)
		{
			delete [] m_pData;
			m_nSize = 0;
			m_pData = NULL;
		}
	}
	TString &operator=(const char *c)
	{
		if (c == NULL)
		{
			Empty();
			return * this;
		}
		UInt32 nl = 0;
		UInt32 len = strlen(c);
		if (len + 1 <= m_nSize)
		{
			//ykr strcpy_s(m_pData, m_nSize, c);
			strncpy(m_pData, c, m_nSize);
			m_nLength = len;
			return * this;
		}
		nl = (len/64 + 1)*64;
		char * p = new char[nl];
		if (p == NULL)
		{
			return * this;
		}
		strcpy(p, c);
		if (m_pData)
			delete [] m_pData;
		m_pData = p;
		m_nSize = nl;
		m_nLength = len;
		return * this;
	}
	TString &operator=(TString &t)
	{
		(*this) = t.c_str();
		return * this;
	}
	TString &operator=(const char c)
	{
		char s[2];
		s[0] = c;
		s[1] = 0;
		(*this) = s;
		return * this;
	}
	int operator==(const char *p)
	{
		if (p == NULL)
			return IsEmpty();
		return (strcmp(m_pData, p) == 0) ? 1 : 0;
	}
	int operator==(TString &t)
	{
		return (strcmp(m_pData, t.c_str()) == 0) ? 1 : 0;
	}
	TString &operator+=(const char c)
	{
		char s[2];
		s[0] = c;
		s[1] = 0;
		(*this) += s;
		return * this;
	}
	TString &operator+=(const int i)
	{
		char s[20];
		sprintf(s, "%d", i);
		(*this) += s;
		return * this;
	}
	TString &operator+=(TString &t)
	{
		(*this) += t.c_str();
		return * this;
	}
	TString &operator+=(const char *c)
	{
		if (c == NULL)
		{
			return * this;
		}
		int nl = 0;
		int len = strlen(c);
		if (len + m_nLength + 1 <= m_nSize)
		{
			strcat(m_pData, c);
			m_nLength += len;
			return * this;
		}
		nl = ((len + m_nLength + 1)/64 + 1)*64;
		char * p = new char[nl];
		if (p == NULL)
		{
			return * this;
		}
		if (m_pData)
			strcpy(p, m_pData);
		strcat(p, c);
		if (m_pData)
			delete [] m_pData;
		m_pData = p;
		m_nSize = nl;
		m_nLength += len;
		return * this;
	}
	TString &Insert(UInt32 nPos, const char *c)
	{
		if (c == NULL)
		{
			return * this;
		}
		int nl = 0;
		int len = strlen(c);
		if ((len + m_nLength + 1) <= m_nSize)
		{
			if (nPos > m_nLength)
				nPos = m_nLength;
			int i = m_nLength;
			for(i; i >= (SInt32)nPos; i--)
				m_pData[i+len] = m_pData[i];
			strncpy(m_pData + nPos, c, len);
			m_nLength += len;
			return * this;
		}
		nl = ((len + m_nLength + 1)/64 + 1)*64;
		char * p = new char[nl];
		if (p == NULL)
		{
			return * this;
		}
		if (nPos > m_nLength) nPos = m_nLength;
		if (m_pData) strncpy(p, m_pData, nPos);
		if (m_pData == NULL) nPos = 0;
		strcpy(p + nPos, c);
		if (m_pData)
		{
			strcpy(p + nPos + len, m_pData+nPos);
		}
		if (m_pData)
			delete [] m_pData;
		m_pData = p;
		m_nSize = nl;
		m_nLength += len;
		return * this;
	}
	TString &SetStr(const char * c, UInt32 len)
	{
		if (c == NULL)
		{
			Empty();
			return * this;
		}
		UInt32 nl = 0;
		if (len + 1 <= m_nSize)
		{
			strncpy(m_pData, c, len);
			m_pData[len] = 0;
			m_nLength = strlen(m_pData);
			return * this;
		}
		nl = (len/64 + 1)*64;
		char * p = new char[nl];
		if (p == NULL)
		{
			return * this;
		}
		strncpy(p, c, len);
		p[len] = 0;
		if (m_pData)
			delete [] m_pData;
		m_pData = p;
		m_nSize = nl;
		m_nLength = strlen(m_pData);
		return * this;
	}

	void Mid(UInt32 a, UInt32 b, TString &dest)
	{
/*		if (a > b)
		{
			b = a + 1;
		}*/
		if (a > m_nLength)
		{
			a = m_nLength;
		}
		if (a + b > m_nLength)
		{
			b = m_nLength - a;
		}
		if (m_pData)
		{
			dest.SetStr(m_pData + a, b);
		}
	}
	void Left(UInt32 n, TString &dest)
	{
		if (m_pData)
		{
			dest.SetStr(m_pData, n);
		}
	}
	void Right(UInt32 n, TString &dest)
	{
		if (m_pData)
		{
			if (n > m_nLength)
				dest = m_pData;
			else
				dest = m_pData + (m_nLength - n);
		}
	}
	const char * str()
	{
		if (m_pData)
			return (const char *)m_pData;
		else
			return "";
	}
	void Empty(void)
	{
		if (m_pData)
		{
			m_nLength = 0;
			m_pData[0] = 0;
		}
	}
	int GetLength()
	{
		return m_nLength;
	}
	Boolean IsEmpty(void)
	{
		return (m_pData == NULL) || (m_nLength == 0) ? true : false;
	}
	char operator[](UInt32 i)
	{
		if (i >= 0 && i < m_nSize && m_pData)
			return m_pData[i];
		return 0;
	}
	char GetAt(UInt32 i)
	{
		if (i >= 0 && i < m_nSize && m_pData)
			return m_pData[i];
		return 0;
	}
	operator const char *()
	{
		return c_str();
	}
	const char * c_str()
	{
		if (m_pData)
			return (const char *)m_pData;
		else
			return "";
	}
	int Find(const char *substr, UInt32 nFrom = 0)
	{
		if (m_pData == NULL)
			return -1;
		if (nFrom >= m_nLength)
			return -1;
		char * p = strstr(m_pData + nFrom, substr);
		if (p == NULL)
			return -1;
		return (m_nLength - strlen(p));
	}
	int FindRev(char c)
	{
		if (m_pData == NULL)
			return -1;
		if (m_nLength < 1)
			return -1;
		for(int i = m_nLength - 1; i >= 0; i--)
		{
			if (m_pData[i] == c)
				return i;
		}
		return -1;
	}
	void Delete(UInt32 nFrom, UInt32 nCount = 1)
	{
		if (0 == nCount || m_pData==NULL || nFrom<0 || nFrom>=m_nLength) return;
		
		int ac = nCount;
		if (nFrom + ac > m_nLength)
        {
			ac = m_nLength - nFrom;
		}

        //GCAL_DLOG("TString.Delete: nFrom=%d, nCount=%d, ac=%d, strlen=%d", nFrom, nCount, ac, m_nLength);
        memmove(m_pData + nFrom, m_pData + nFrom + ac, 1 + m_nLength - (nFrom + ac));
        //strcpy(m_pData + nFrom, m_pData + nFrom + ac);
		//m_pData[nFrom + ac] = 0;
		m_nLength = strlen(m_pData);
	}
	void TrimRight()
	{
		if (m_pData == NULL || m_nLength < 1)
			return;
		char * p = m_pData + m_nLength - 1;
		while(p >= m_pData && (*p == 9 || *p==10 || *p==13 || *p==32))
		{
			*p = 0;
			--p;
		}
		m_nLength = strlen(m_pData);
	}
	void TrimLeft()
	{
		if (m_pData == NULL)
			return;
		char * p = m_pData;
		char * ps = m_pData + m_nLength;
		while(p < ps && (*p==9 || *p==10 || *p==13 || *p==32))
		{
			p++;
		}
		if (p != m_pData)
		{
			strcpy(m_pData, p);
			m_nLength = strlen(m_pData);
		}
	}
	/*void Format(const char *format, const char *arg1)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, const char * arg2)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, double arg2)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, int arg2, int arg3, const char * arg4)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2, arg3, arg4);
		(*this) = pp;
	}
	void Format(const char *format, int arg2, int arg3)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg2, arg3);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, double arg2, const char * arg4)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2, arg4);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, const char * arg2, int arg3)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2, arg3);
		(*this) = pp;
	}
	void Format(const char *format, const char *arg1, const char * arg2, const char * arg3)
	{
		char * pp = AvcGetTextBufferBig();
		sprintf(pp, format, arg1, arg2, arg3);
		(*this) = pp;
	}*/
	void Format(const char *cMessageFormat, ...)
	{
		char * pp = AvcGetTextBufferBig();
		va_list argp;
		// obtain formatted message with arguments and push them into buffer
		va_start(argp, cMessageFormat);
		vsprintf(pp, cMessageFormat, argp);
		va_end(argp);
		(*this) = pp;
		return;
	}
	void Replace(char * src, char *dest)
	{
		TString s, st;
		int curr = 0;
		int prevcurr = 0;

		while(prevcurr = Find(src, curr), prevcurr >= 0)
		{
			Mid(curr, prevcurr, st);
			s += st;
			s += dest;
			prevcurr += strlen(src);
			curr = prevcurr;
		}
		Mid(curr, m_nLength, st);
		s += st;
		(*this) = s.c_str();
	}
	int CompareNoCase(const char * src)
	{
		if (m_pData == NULL)
			return -1;
		if (src == NULL)
			return 1;
		return stricmp(m_pData, src);
	}
};

void LogStart(char *pLogFile);
void LogClose();
void Log(char *fmt, ...);

#endif 
