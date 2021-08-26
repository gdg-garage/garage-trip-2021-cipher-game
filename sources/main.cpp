#include <cage-core/logger.h>
#include <cage-core/config.h>

using namespace cage;

void cipherSudocube();
void cipherDna();

int main(int argc, const char *args[])
{
	Holder<Logger> log = newLogger();
	log->format.bind<logFormatConsole>();
	log->output.bind<logOutputStdOut>();

	try
	{
		configSetUint64("cage/random/seed1", 9608723347976139271);
		configSetUint64("cage/random/seed2", 4522334992954367309);

		cipherSudocube();
		cipherDna();

		return 0;
	}
	catch (...)
	{
		detail::logCurrentCaughtException();
	}
	return 1;
}
