#include <cage-core/logger.h>

using namespace cage;

void makeTitle();
void cipherLabyrinth();
void cipherSudocube();
void cipherDna();
void cipherSecret();

int main(int argc, const char *args[])
{
	Holder<Logger> log = newLogger();
	log->format.bind<logFormatConsole>();
	log->output.bind<logOutputStdOut>();

	try
	{
		makeTitle();
		cipherLabyrinth();
		cipherSudocube();
		cipherDna();
		cipherSecret();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
	}
	return 1;
}
