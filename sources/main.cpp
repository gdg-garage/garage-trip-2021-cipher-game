#include <cage-core/logger.h>

using namespace cage;

void cipherLabyrinth();
void cipherSudocube();
void cipherDna();

int main(int argc, const char *args[])
{
	Holder<Logger> log = newLogger();
	log->format.bind<logFormatConsole>();
	log->output.bind<logOutputStdOut>();

	try
	{
		cipherSudocube();
		cipherDna();
		cipherLabyrinth();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
	}
	return 1;
}
