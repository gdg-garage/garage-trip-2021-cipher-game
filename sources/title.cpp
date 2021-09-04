#include "common.h"

#include <cage-core/lineReader.h>
#include <cage-core/string.h>

#include <vector>

namespace
{
	std::string frequenciesTable(const std::string &csv)
	{
		std::vector<string> freqs;
		{
			auto lr = newLineReader(csv);
			string line;
			while (lr->readLine(line))
			{
				split(line, ",");
				freqs.push_back(subString(line, 0, 6));
			}
		}
		CAGE_ASSERT(freqs.size() == 26);
		std::string res = "<table>\n";
		for (uint32 i = 0; i < 13; i++)
		{
			res += "<tr>\n";
			res += (stringizer() + "<td class=\"letter\">" + string('A' + i) + "<td class=\"frequency\">" + freqs[i]).value.c_str();
			res += (stringizer() + "<td class=\"letter\">" + string('N' + i) + "<td class=\"frequency\">" + freqs[i + 13]).value.c_str();
			res += "</tr>\n";
		}
		return res + "</table>\n";
	}
}

void makeTitle()
{
	const std::string titleSvg = readInput("title.svg");
	const std::string title = R"foo(<html>
<head>
<title>Title</title>
</head>
<body>
<style>
html, body, svg {
  width: 100%;
  height: 100%;
  margin: 0;
}
</style>
)foo" + titleSvg + R"foo(
</body>
</html>
)foo";

	writeOutput("0", title);

	const std::string dnaSvg = readInput("dna.svg");
	const std::string letterFreq = frequenciesTable(readInput("frequencies.csv"));
	const std::string preface = R"foo(<html>
<head>
<title>Preface</title>
</head>
<body>
<style>
body, p, td {
	font-size: 14pt;
	font-family: monospace;
}
svg { height: 15em; }
#docid { float: right; }
body { margin: 2em; }
</style>
<div id="docid">
Předmluva
</div>
<div style="clear: both" />

<hr>
V celé hře se používá výhradně anglická abeceda (26 znaků).
<br>
Všechny texty jsou psány spisovnou češtinou (s odstraněnou diakritikou).
<br>
Tabulka relativní frekvence písmen:
<br>
)foo" + letterFreq + R"foo(

<hr>
Prvočíslo je přirozené číslo větší než 1, které je dělitelné jen dvěma děliteli: jedničkou a samo sebou.

<hr>
)foo" + dnaSvg + R"foo(
<br>
A se páruje s T dvěma vodíkovými vazbami.
<br>
C se páruje s G třemi vodíkovými vazbami.
<br>
Sekvence AT kóduje písmeno a.
<br>
Sekvence GAC kóduje písmeno z.
</body>
</html>
)foo";

	writeOutput("1", preface);
}
