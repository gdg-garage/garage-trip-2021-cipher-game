#include "common.h"

#include <cage-core/math.h>

namespace
{
	constexpr const char *dnaPerChar[] = {
		"AT", // A
		"GCA",
		"GTA",
		"CC",
		"AA",
		"GCC",
		"GCT",
		"GTC",
		"AG",
		"GTG",
		"GAT",
		"TT",
		"CG",
		"TA",
		"AC",
		"GAG",
		"GGT",
		"CA",
		"TC",
		"TG",
		"GAA",
		"CT",
		"GCG",
		"GGA",
		"GTT",
		"GAC", // Z
	};

	char complementary(char c)
	{
		switch (c)
		{
		case 'A': return 'T';
		case 'T': return 'A';
		case 'C': return 'G';
		case 'G': return 'C';
		case 'a': return 't';
		case 't': return 'a';
		case 'c': return 'g';
		case 'g': return 'c';
		default: CAGE_THROW_CRITICAL(Exception, "invalid dna base");
		}
	}

	char different(char c)
	{
		char p = c;
		while (p == c)
		{
			switch (randomRange(0, 4))
			{
			case 0: p = 'A'; break;
			case 1: p = 'T'; break;
			case 2: p = 'C'; break;
			case 3: p = 'G'; break;
			}
		}
		return p;
	}

	std::string dnaEncode(const std::string &s)
	{
		std::string o;
		for (const char i : s)
		{
			if (i >= 'A' && i <= 'Z')
			{
				for (const char c : string(dnaPerChar[i - 'A']))
				{
					o += "<div style=\"display:inline-block\">";
					char a = c;
					char b = complementary(c);
					if (randomChance() < 0.1)
					{
						if (randomChance() < 0.5)
							a = different(a);
						else
							b = different(b);
						a = tolower(a);
						b = tolower(b);
					}
					o += std::string() + a + "<br>" + b;
					o += "</div>";
				}
			}
			else
				o += i;
		}
		return o;
	}
}

void cipherDna()
{
	constexpr uint32 cypherIndex = 3;
	rngReseed();
	std::string i = readInput(cypherIndex);
	i = replaceLfToSpaces(i);
	i = replaceCzech(i);
	i = toUpper(i);
	std::string s = dnaEncode(i);
	const std::string o = generateHeader(cypherIndex, "DNA") + s + generateFooter(cypherIndex);
	writeOutput(cypherIndex, o);
}
