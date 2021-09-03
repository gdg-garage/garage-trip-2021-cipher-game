#include "common.h"

#include <cage-core/flatSet.h>
#include <cage-core/enumerate.h>
#include <cage-core/files.h>
#include <cage-core/math.h>

#include <vector>

namespace
{
#define MAKE_SIDE(X) X + 0, X + 1, X + 2, X + 3, X + 4, X + 5, X + 6, X + 7, X + 8, X + 9, X + 10, X + 11, X + 12, X + 13, X + 14, X + 15
#define MAKE_FOUR(X) X + 0, X + 1, X + 2, X + 3
#define MAKE_QUAD(X) X + 0, X + 4, X + 8, X + 12
	const std::vector<FlatSet<uint8>> constraints = {
		// sides
		FlatSet<uint8>{ MAKE_SIDE(0) },
		FlatSet<uint8>{ MAKE_SIDE(16) },
		FlatSet<uint8>{ MAKE_SIDE(32) },
		FlatSet<uint8>{ MAKE_SIDE(48) },
		FlatSet<uint8>{ MAKE_SIDE(64) },
		FlatSet<uint8>{ MAKE_SIDE(80) },
		// vertical rings
		FlatSet<uint8>{ MAKE_QUAD(0), MAKE_QUAD(32), MAKE_QUAD(64), MAKE_QUAD(80) },
		FlatSet<uint8>{ MAKE_QUAD(1), MAKE_QUAD(33), MAKE_QUAD(65), MAKE_QUAD(81) },
		FlatSet<uint8>{ MAKE_QUAD(2), MAKE_QUAD(34), MAKE_QUAD(66), MAKE_QUAD(82) },
		FlatSet<uint8>{ MAKE_QUAD(3), MAKE_QUAD(35), MAKE_QUAD(67), MAKE_QUAD(83) },
		// horizontal rings
		FlatSet<uint8>{ MAKE_FOUR(16), MAKE_FOUR(32), MAKE_FOUR(48), MAKE_FOUR(92) },
		FlatSet<uint8>{ MAKE_FOUR(20), MAKE_FOUR(36), MAKE_FOUR(52), MAKE_FOUR(88) },
		FlatSet<uint8>{ MAKE_FOUR(24), MAKE_FOUR(40), MAKE_FOUR(56), MAKE_FOUR(84) },
		FlatSet<uint8>{ MAKE_FOUR(28), MAKE_FOUR(44), MAKE_FOUR(60), MAKE_FOUR(80) },
		// circular rings
		FlatSet<uint8>{ MAKE_FOUR(0),  MAKE_FOUR(76), MAKE_QUAD(16), MAKE_QUAD(51) },
		FlatSet<uint8>{ MAKE_FOUR(4),  MAKE_FOUR(72), MAKE_QUAD(17), MAKE_QUAD(50) },
		FlatSet<uint8>{ MAKE_FOUR(8),  MAKE_FOUR(68), MAKE_QUAD(18), MAKE_QUAD(49) },
		FlatSet<uint8>{ MAKE_FOUR(12), MAKE_FOUR(64), MAKE_QUAD(19), MAKE_QUAD(48) },
	};
#undef MAKE_SIDE
#undef MAKE_FOUR
#undef MAKE_QUAD

	bool isValid(const std::vector<uint8> &data, uint32 index)
	{
		if (data[index] == 0)
			return true;
		for (const auto &ci : constraints)
		{
			if (ci.count(index) == 0)
				continue;
			for (auto i : ci)
				if (i != index && data[i] == data[index])
					return false;
		}
		return true;
	}

	bool isValid(const std::vector<uint8> &data)
	{
		const uint32 s = numeric_cast<uint32>(data.size());
		for (uint32 i = 0; i < s; i++)
			if (!isValid(data, i))
				return false;
		return true;
	}

	std::string printCrossSide(const std::vector<uint8> &data, uint32 index, const FlatSet<uint8> &responseFields)
	{
		std::string res = "<table class=\"side\">";
		for (uint32 r = 0; r < 4; r++)
		{
			res += "<tr>";
			for (uint32 c = 0; c < 4; c++)
			{
				const uint32 idx = index + r * 4 + c;
				const uint8 t = data[idx];
				const string s = t ? stringizer() + t : stringizer();
				if (responseFields.count(idx))
					res += std::string() + "<td class=\"mark\">" + s.c_str() + "</td>";
				else
					res += std::string() + "<td>" + s.c_str() + "</td>";
			}
			res += "</tr>";
		}
		return res + "</table>";
	}

	std::string printCross(const std::vector<uint8> &data, const FlatSet<uint8> &responseFields)
	{
		CAGE_ASSERT(data.size() == 96);
		std::string res = "<table>";
		{
			res += "<tr>";
			res += "<td class=\"empty\" />";
			res += "<td class=\"cross\">" + printCrossSide(data, 0, responseFields) + "</td>";
			res += "<td class=\"empty\"  />";
			res += "</tr>";
		}
		{
			res += "<tr>";
			res += "<td class=\"cross\">" + printCrossSide(data, 16, responseFields) + "</td>";
			res += "<td class=\"cross\">" + printCrossSide(data, 32, responseFields) + "</td>";
			res += "<td class=\"cross\">" + printCrossSide(data, 48, responseFields) + "</td>";
			res += "</tr>";
		}
		{
			res += "<tr>";
			res += "<td class=\"empty\"  />";
			res += "<td class=\"cross\">" + printCrossSide(data, 64, responseFields) + "</td>";
			res += "<td class=\"empty\"  />";
			res += "</tr>";
		}
		{
			res += "<tr>";
			res += "<td class=\"empty\"  />";
			res += "<td class=\"cross\">" + printCrossSide(data, 80, responseFields) + "</td>";
			res += "<td class=\"empty\"  />";
			res += "</tr>";
		}
		return res + "</table>";
	}

	uint32 findEmpty(const std::vector<uint8> &data)
	{
		for (const auto &it : enumerate(data))
			if (*it == 0)
				return it.index;
		return m;
	}

	bool solver(std::vector<uint8> &data)
	{
		const uint32 idx = findEmpty(data);
		if (idx == m)
			return true;
		CAGE_ASSERT(data[idx] == 0);
		for (uint8 i = 1; i <= 16; i++)
		{
			data[idx] = i;
			if (isValid(data, idx))
			{
				if (solver(data))
					return true;
			}
		}
		data[idx] = 0;
		return false;
	}

	std::vector<uint8> randomSeed()
	{
		std::vector<uint8> data;
		data.resize(96);
		for (uint32 round = 0; round < 3; round++)
		{
			for (auto &it : enumerate(data))
			{
				if (*it != 0 || randomChance() > 0.1)
					continue;
				*it = randomRange(1u, 17u);
				if (!isValid(data, it.index))
					*it = 0;
			}
		}
		CAGE_ASSERT(isValid(data));
		return data;
	}

	uint32 countSolutions(const std::vector<uint8> &data, uint32 index)
	{
		std::vector<uint8> d = data;
		uint32 res = 0;
		CAGE_ASSERT(data[index] == 0);
		for (uint8 i = 1; i <= 16; i++)
		{
			d[index] = i;
			if (isValid(d, index))
			{
				if (solver(d))
					res++;
			}
		}
		return res;
	}

	std::vector<uint8> makeBlanks(const std::vector<uint8> &data)
	{
		std::vector<uint8> q = data;
		for (uint32 round = 0; round < 35; round++) // make 35 tiles blank
		{
			while (true) // find a suitable tile for blanking
			{
				const uint32 idx = randomRange(std::size_t(0), data.size());
				if (q[idx] == 0)
					continue;
				CAGE_ASSERT(q[idx] == data[idx]);
				q[idx] = 0;
				if (countSolutions(q, idx) == 1) // make sure blanking the tile leaves only one solution
					break;
				q[idx] = data[idx];
			}
		}
		return q;
	}

	uint32 sumFields(const std::vector<uint8> &data, const FlatSet<uint8> &responseFields)
	{
		uint32 sum = 0;
		for (auto i : responseFields)
			sum += data[i];
		return sum;
	}

	FlatSet<uint8> findResponseFields(const std::vector<uint8> &data, const std::vector<uint8> &question)
	{
		FlatSet<uint8> posibilities;
		for (const auto &it : enumerate(question))
			if (*it == 0)
				posibilities.insert(it.index);

		while (true)
		{
			FlatSet<uint8> pos = posibilities;
			FlatSet<uint8> r;
			while (!pos.empty())
			{
				const uint32 i = randomRange(uint32(0), uint32(pos.size()));
				const uint32 v = *(pos.begin() + i);
				pos.erase(pos.begin() + i);
				r.insert(v);
				if (r.size() > 7 && isPrime(sumFields(data, r)))
					return r;
			}
		}
	}
}

void cipherSudocube()
{
	constexpr uint32 cypherIndex = 2;

	rngReseed();
	std::vector<uint8> data;
	while (true)
	{
		data = randomSeed();
		if (solver(data))
			break;
	}
	CAGE_ASSERT(findEmpty(data) == m);
	CAGE_ASSERT(isValid(data));

	std::vector<uint8> question = makeBlanks(data);
	CAGE_ASSERT(findEmpty(question) != m);
	CAGE_ASSERT(isValid(question));

	const FlatSet<uint8> responseFields = findResponseFields(data, question);

	const std::string style = R"foo(<style>
table { border-collapse: collapse; }
td { margin: 0px; padding: 0px; }
.side td { border: 1px solid gray; text-align: center; width: 2em; height: 2em; }
td.cross { border: 1px solid black; }
td.mark { background-color: lightgray; }
</style>
)foo";

	{
		const std::string o = generateHeader(cypherIndex, "Kostka") + style + printCross(question, responseFields) + generateFooter(cypherIndex);
		writeOutput(cypherIndex, o);
	}

	{
		Holder<File> f = writeFile(pathJoin(solutionPath, "sudocube.html"));
		const std::string response = (stringizer() + sumFields(data, responseFields)).value.c_str();
		const std::string o = std::string() + "<html><head><style>*{font-size:14pt; font-family:monospace;}</style></head><body>" + style + printCross(data, responseFields) + "<hr>" + response + "</body></html>";
		f->write(o);
		f->close();
	}
}
