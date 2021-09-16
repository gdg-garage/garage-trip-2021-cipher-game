#include "common.h"

#include <cage-core/lineReader.h>
#include <cage-core/string.h>

#include <vector>

namespace
{
	std::string frequenciesTable(const std::string &csv)
	{
		std::vector<String> freqs;
		{
			auto lr = newLineReader(csv);
			String line;
			while (lr->readLine(line))
			{
				split(line, ",");
				freqs.push_back(subString(line, 0, 6));
			}
		}
		CAGE_ASSERT(freqs.size() == 26);
		std::string res = "<table style=\"float:right; margin-left: 1em\">\n";
		for (uint32 i = 0; i < 13; i++)
		{
			res += "<tr>\n";
			res += (Stringizer() + "<td class=\"letter\">" + String('A' + i) + "<td class=\"frequency\">" + freqs[i]).value.c_str();
			res += (Stringizer() + "<td class=\"letter\">" + String('N' + i) + "<td class=\"frequency\">" + freqs[i + 13]).value.c_str();
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
svg { height: 15em; float: right; margin-left: 1em; }
#docid { float: right; }
body { margin: 2em; }
hr { clear: both; }
</style>
<div id="docid">
Předmluva
</div>
<div style="clear: both" />

<hr>
)foo" + letterFreq + R"foo(
V celé hře se používá výhradně anglická abeceda (26 znaků).
<br>
Všechny texty jsou psány spisovnou češtinou (s&nbsp;odstraněnou diakritikou).
<br>
V tabulce vpravo jsou relativní frekvence písmen v&nbsp;obecném textu.

<hr>
Prvočíslo je přirozené číslo větší než 1, které je dělitelné jen dvěma děliteli: jedničkou a&nbsp;samo sebou.

<hr>
)foo" + dnaSvg + R"foo(
Evoluce byla velice šikovná v nalezení úsporného kódování proteinů.
<br>
V DNA se vyskytují čtyři báze, typicky značené písmeny A,&nbsp;T,&nbsp;C&nbsp;a&nbsp;G.
<br>
A se pojí s T dvěma a C s G třemi vodíkovými vazbami.
<br>
Jiné kombinace nejsou možné.
<br>
Známé sekvence: AA&nbsp;=&nbsp;E, AT&nbsp;=&nbsp;A, GGT&nbsp;=&nbsp;Q a&nbsp;GAC&nbsp;=&nbsp;Z.

<hr>
Výsledkem každé úlohy je jedno číslo v rozsahu 10 až 1000.
</body>
</html>
)foo";

	writeOutput("1", preface);
}
