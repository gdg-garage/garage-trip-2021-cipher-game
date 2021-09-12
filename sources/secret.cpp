#include "common.h"

#include <cage-core/files.h>

namespace
{
	constexpr const uint32 nums[] = { 79, 103, 557 };

	void findPrimesThatSumAPrime()
	{
		for (uint32 i = 0; i < 10000; i++)
		{
			if (!isPrime(i))
				continue;
			const uint32 sum = nums[0] + nums[1] + i;
			if (isPrime(sum))
				CAGE_LOG(SeverityEnum::Info, "primes", Stringizer() + "79 + 103 + " + i + " = " + sum);
		}
	}
}

void cipherSecret()
{
	//findPrimesThatSumAPrime();
	rngReseed();
	uint32 sum = 0;
	for (uint32 i : nums)
	{
		if (!isPrime(i))
			CAGE_THROW_ERROR(Exception, "number not prime");
		sum += i;
	}
	if (!isPrime(sum))
		CAGE_THROW_ERROR(Exception, "sum not prime");

	{
		const std::string s = (Stringizer() + nums[0] + " + " + nums[1] + " + " + nums[2] + " = " + sum).value.c_str();
		Holder<File> f = writeFile(pathJoin(solutionPath, "secret.html"));
		const std::string o = std::string() + "<html><head><style>*{font-size:14pt; font-family:monospace;}</style></head><body>" + s + "</body></html>";
		f->write(o);
		f->close();
	}

	const std::string o = generateHeader(4, "Tajemství") + generateFooter();
	writeOutput("5", o);
}
