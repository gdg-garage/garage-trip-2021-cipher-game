#include "common.h"

#include <cage-core/random.h>
#include <cage-core/files.h>
#include <cage-core/debug.h>

namespace
{
	string findDataPath()
	{
		return pathToAbs(pathSearchTowardsRoot("data", PathTypeFlags::Directory));
	}

	string findInputPath()
	{
		return pathToAbs(pathSearchTowardsRoot("inputs", PathTypeFlags::Directory));
	}

	string findOutputPath()
	{
		try
		{
			detail::OverrideBreakpoint ob;
			return pathToAbs(pathSearchTowardsRoot("outputs", PathTypeFlags::Directory));
		}
		catch (...)
		{
			return pathJoin(findInputPath(), "../outputs");
		}
	}

	string findSolutionPath()
	{
		try
		{
			detail::OverrideBreakpoint ob;
			return pathToAbs(pathSearchTowardsRoot("solutions", PathTypeFlags::Directory));
		}
		catch (...)
		{
			return pathJoin(findInputPath(), "../solutions");
		}
	}
}

const string dataPath = findDataPath();
const string inputPath = findInputPath();
const string outputPath = findOutputPath();
const string solutionPath = findSolutionPath();

std::string readInputFile(const string &filename)
{
	auto b = readFile(pathJoin(inputPath, filename))->readAll();
	std::string s = std::string(b.data(), b.size());
	return s;
}

std::string readInput(uint32 index)
{
	return readInputFile(stringizer() + index + ".txt");
}

std::string generateHeader(uint32 index, const string &name)
{
	std::string s = R"foo(<html>
<head>
<title>DOCID - TITLE</title>
<style>
*
{
	font-size: 14pt;
	font-family: monospace;
}
#docid
{
	float: right;
}
</style>
</head>
<body>
<div id="docid">
TITLE (DOCID)
</div>
)foo";

	replace(s, "TITLE", name.c_str());
	replace(s, "DOCID", string(stringizer() + index).c_str());

	{
		const string n = stringizer() + index + "-preface.txt";
		if (pathIsFile(pathJoin(inputPath, n)))
		{
			std::string preface = readInputFile(n);
			s += preface + "<hr>\n";
		}
		else
			s += "<div style=\"clear: both\" />\n";
	}

	return s;
}

std::string generateFooter(uint32 index)
{
	return R"foo(
</body>
</html>
)foo";
}

void writeOutput(uint32 index, const std::string &output)
{
	auto f = writeFile(pathJoin(outputPath, stringizer() + index + ".html"));
	f->write(PointerRange<const char>(output.data(), output.data() + output.size()));
	f->close();
}

void replace(std::string &str, const std::string &what, const std::string &with)
{
	while (true)
	{
		auto it = str.find(what);
		if (it == str.npos)
			break;
		str.replace(it, what.length(), with);
	}
}

std::string replaceCzech(const std::string &s)
{
	std::string o = s;

	replace(o, "á", "a");
	replace(o, "é", "e");
	replace(o, "ě", "e");
	replace(o, "í", "i");
	replace(o, "ý", "y");
	replace(o, "ó", "o");
	replace(o, "ú", "u");
	replace(o, "ů", "u");

	replace(o, "Á", "A");
	replace(o, "É", "E");
	replace(o, "Ě", "E");
	replace(o, "Í", "I");
	replace(o, "Ý", "Y");
	replace(o, "Ó", "O");
	replace(o, "Ú", "U");
	replace(o, "Ů", "U");

	replace(o, "č", "c");
	replace(o, "ď", "d");
	replace(o, "ň", "n");
	replace(o, "ř", "r");
	replace(o, "š", "s");
	replace(o, "ť", "t");
	replace(o, "ž", "z");

	replace(o, "Č", "C");
	replace(o, "Ď", "D");
	replace(o, "Ň", "N");
	replace(o, "Ř", "R");
	replace(o, "Š", "S");
	replace(o, "Ť", "T");
	replace(o, "Ž", "Z");

	return o;
}

std::string replaceLfToSpaces(const std::string &s)
{
	std::string o = s;
	replace(o, "\n", " ");
	replace(o, "\r", " ");
	replace(o, "  ", " ");
	return o;
}

std::string toUpper(const std::string &s)
{
	std::string o;
	o.reserve(s.size());
	for (const char c : s)
	{
		if (c >= 'a' && c <= 'z')
			o += c - 'a' + 'A';
		else
			o += c;
	}
	return o;
}

std::string toLower(const std::string &s)
{
	std::string o;
	o.reserve(s.size());
	for (const char c : s)
	{
		if (c >= 'A' && c <= 'Z')
			o += c - 'A' + 'a';
		else
			o += c;
	}
	return o;
}

void rngReseed()
{
	detail::globalRandomGenerator() = RandomGenerator(9608723347976139271, 4522334992954367309);
}

bool isPrime(uint32 n)
{
	switch (n)
	{
	case 0:
	case 1:
		return false;
	case 2:
	case 3:
		return true;
	}
	if (n % 2 == 0 || n % 3 == 0)
		return false;
	for (uint32 i = 5; i * i <= n; i += 6)
		if ((n % i == 0) || (n % (i + 2) == 0))
			return false;
	return true;
}
