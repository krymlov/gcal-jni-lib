#include "tcommand.h"
#include "string.h"
#include "stdio.h"

#pragma GCC diagnostic ignored "-Wpointer-arith"

int TCommand::operator==(char * pc)
{
	return (strcmp(args[0], pc) == 0) ? 1 : 0;
}

int TCommand::Divide()
{
	char * p = line;
	argn = 0;
	while(*p)
	{
		while(*p && *p==' ')
			p++;
		if (*p == '\"')
		{
			p++;
			args[argn] = p;
			argn++;
			while(*p && *p != '\"')
			{
				if (*p == '\\')
					p++;
				p++;
			}
			if (*p == NULL)
				return argn;
			*p = 0;
		}
		else
		{
			args[argn] = p;
			argn++;
			while(*p && *p!=' ')
				p++;
			if (*p == NULL)
				return argn;
			*p = 0;
		}
		p++;
	}

	return argn;
}

int TCommand::Read()
{
	int len;
	printf("gcal>");
	char * p = fgets(line, 1024, stdin);
	len = strlen(line);
	if (len > 0)
	{
		if (line[len - 1] == '\n')
			line[len - 1] = 0;
	}
	if (p == NULL)
		return 0;
	if (strncmp(line, "exit", 4) == 0)
		return 0;
	Divide();
	return 1;
}

int TCommand::IsArg(int i)
{
	return (i < argn);
}

bool TCommand::is_arg(int i, char *str)
{
	if (i >= argn)
		return false;

	if (strcmp(str, args[i]) == 0)
		return true;

	return false;
}
