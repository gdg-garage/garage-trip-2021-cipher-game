#include "common.h"

#include <cage-core/math.h>

#include <vector>

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
		std::vector<string> parts;
		for (const char i : s)
		{
			if (i >= 'A' && i <= 'Z')
			{
				for (const char c : string(dnaPerChar[i - 'A']))
				{
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
					parts.push_back(stringizer() + string(a) + "&nbsp;" + string(b) + "<br>");
				}
			}
			else
				parts.push_back(string(i) + "<br>");
		}
		std::string o = "<style>div { vertical-align: top; margin-right: 7em; }</style><div>";
		uint32 index = 0;
		while (!parts.empty())
		{
			if (index++ % 42 == 0)
				o += "</div><div style=\"display: inline-block\">";
			o += parts.front().c_str();
			parts.erase(parts.begin());
		}
		return o + "</div>";
	}
}

void cipherDna()
{
	rngReseed();
	std::string i = readInput("dna.txt");
	i = replaceLfToSpaces(i);
	i = replaceCzech(i);
	i = toUpper(i);
	std::string s = dnaEncode(i);
	const std::string o = generateHeader(3, "DNA") + s + generateFooter();
	writeOutput("4", o);
}
