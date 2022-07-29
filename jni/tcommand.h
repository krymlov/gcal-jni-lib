#ifndef tcommand_rtf2t__
#define tcommand_rtf2t__

class TCommand
{
public:
	bool is_arg(int i, char * str);
	int IsArg(int i);
	int Read();
	char line[1024];
	char * args[32];
	int argn;
	TCommand()
	{
		argn = 0;
	}
	int Divide();
	int operator==(char * pc);
};


#endif