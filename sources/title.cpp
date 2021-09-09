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
		std::string res = "<table>\n";
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
Tabulka relativní frekvence písmen v obecném textu:
<br>
)foo" + letterFreq + R"foo(

<hr>
Prvočíslo je přirozené číslo větší než 1, které je dělitelné jen dvěma děliteli: jedničkou a samo sebou.

<hr>
)foo" + dnaSvg + R"foo(
<br>
V DNA se vyskytují čtyři báze, typicky značené písmeny A,&nbsp;T,&nbsp;C&nbsp;a&nbsp;G.
<br>
A se pojí s T dvěma a C s G třemi vodíkovými vazbami.
<br>
Jiné kombinace nejsou možné.
<br>
Sekvence AA kóduje písmeno E.
<br>
Sekvence AT kóduje písmeno A.
<br>
Sekvence GAC kóduje písmeno Z.
</body>
</html>
)foo";

	writeOutput("1", preface);
}
