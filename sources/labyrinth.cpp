#include "common.h"

#include <cage-core/pointerRangeHolder.h>
#include <cage-core/enumerate.h>
#include <cage-core/string.h>
#include <cage-core/files.h>
#include <cage-core/math.h>
#include <cage-core/utf.h>

#include <initializer_list>
#include <vector>

namespace
{
	enum class Cell : char
	{
		None = 0, // outside of playable map
		Wall = 1,
		Empty = 2, // walkable
		Start = 3,
		Goal = 4,
		Path = 5,
		Tmp = (char)m,
	};

	struct Labyrinth
	{
		Holder<PointerRange<Cell>> cells;
		uint32 width = 0, height = 0;
		uint32 path = m;

		Cell &cell(ivec2 p)
		{
			CAGE_ASSERT(p[0] >= 0 && (uint32)p[0] < width && p[1] >= 0 && (uint32)p[1] < height);
			return cells[p[1] * width + p[0]];
		}
	};

	struct Generator
	{
		std::vector<Cell> cells;
		uint32 width = 0, height = 0;

		void generate()
		{
			rooms();
			corridors();
		}

		Cell &cell(ivec2 p)
		{
			CAGE_ASSERT(p[0] >= 0 && (uint32)p[0] < width && p[1] >= 0 && (uint32)p[1] < height);
			return cells[p[1] * width + p[0]];
		}

	private:
		uint32 countCells(Cell cell) const
		{
			uint32 cnt = 0;
			for (Cell c : cells)
				if (c == cell)
					cnt++;
			return cnt;
		}

		void rectReplace(ivec2 a, ivec2 b, Cell what, Cell with)
		{
			for (sint32 y = a[1]; y < b[1]; y++)
			{
				for (sint32 x = a[0]; x < b[0]; x++)
				{
					Cell &c = cell(ivec2(x, y));
					if (c == what)
						c = with;
				}
			}
		}

		ivec2 findAny(Cell c)
		{
			for (uint32 i = 0; i < cells.size(); i++)
				if (cells[i] == c)
					return ivec2(i % width, i / width);
			return ivec2(-1);
		}

		void seedReplace(ivec2 seed, Cell what, Cell with)
		{
			CAGE_ASSERT(what != with);
			std::vector<ivec2> q;
			q.push_back(seed);
			while (!q.empty())
			{
				const ivec2 p = q.back();
				q.pop_back();
				Cell &c = cell(p);
				if (c != what)
					continue;
				c = with;
				for (ivec2 it : { p + ivec2(-1, 0), p + ivec2(1, 0), p + ivec2(0, -1), p + ivec2(0, 1) })
					if (cell(it) == what)
						q.push_back(it);
			}
		}

		bool isConnected()
		{
			seedReplace(findAny(Cell::Empty), Cell::Empty, Cell::Tmp);
			bool res = countCells(Cell::Empty) == 0;
			rectReplace(ivec2(), ivec2(width, height), Cell::Tmp, Cell::Empty); // restore back
			return res;
		}

		void rooms()
		{
			while (true)
			{
				width = randomRange(50, 60);
				height = randomRange(35, 40);
				cells.clear();
				cells.resize(width * height, Cell::None);

				// generate random rectangular rooms
				while (countCells(Cell::Empty) < width * height * 3 / 5)
				{
					constexpr uint32 S = 6;
					ivec2 a = ivec2(randomRange(1u, width - S), randomRange(1u, height - S));
					ivec2 b = a + randomRange2i(3, S + 1);
					rectReplace(a, b, Cell::None, Cell::Empty);
				}

				// find outline
				for (uint32 y = 1; y < height - 1; y++)
				{
					for (uint32 x = 1; x < width; x++)
					{
						const ivec2 p = ivec2(x, y);
						Cell &c = cell(p);
						if (c != Cell::Empty)
							continue;
						uint32 neighs = 0;
						for (sint32 j = -1; j < 2; j++)
							for (sint32 i = -1; i < 2; i++)
								neighs += cell(p + ivec2(i, j)) == Cell::None;
						if (neighs > 0)
							c = Cell::Wall;
					}
				}

				// detect disconnected areas
				if (isConnected())
					break;
			}
		}

		void corridors()
		{
			for (uint32 round = 0; round < 1000000; round++)
			{
				const ivec2 p = ivec2(randomRange(2u, width - 2), randomRange(2u, height - 2));
				Cell &c = cell(p);
				if (c != Cell::Empty)
					continue;
				bool neighs[8];
				{
					bool *n = neighs;
					for (ivec2 it : { ivec2(-1, 0), ivec2(-1, -1), ivec2(0, -1), ivec2(1, -1), ivec2(1, 0), ivec2(1, 1), ivec2(0, 1), ivec2(-1, 1) })
						*n++ = cell(p + it) == Cell::Wall;
				}
				if (!neighs[0] && !neighs[2] && !neighs[4] && !neighs[6])
					continue; // no 4-connected neighbor
				uint32 sum = 0;
				for (bool b : neighs)
					sum += b;
				if (sum > 2)
					continue;
				uint32 changes = 0;
				for (uint32 i = 0; i < 8; i++)
					changes += neighs[i] != neighs[(i + 1) % 8];
				if (changes != 2)
					continue;
				c = Cell::Wall;
			}

			CAGE_ASSERT(isConnected());
		}
	};

	Labyrinth generate()
	{
		Generator gen;
		gen.generate();
		Labyrinth lab;
		lab.width = gen.width;
		lab.height = gen.height;
		lab.cells = PointerRangeHolder<Cell>(std::move(gen.cells));
		return lab;
	}

	struct Paths
	{
		Labyrinth &lab;

		std::vector<uint32> dists;
		std::vector<ivec2> prevs;

		Paths(Labyrinth &lab) : lab(lab)
		{}

		Cell &cell(ivec2 p)
		{
			CAGE_ASSERT(p[0] >= 0 && (uint32)p[0] < lab.width && p[1] >= 0 && (uint32)p[1] < lab.height);
			return lab.cells[p[1] * lab.width + p[0]];
		}

		bool walkable(ivec2 p)
		{
			switch (cell(p))
			{
			case Cell::Empty:
			case Cell::Start:
			case Cell::Goal:
			case Cell::Path:
				return true;
			default:
				return false;
			}
		}

		uint32 &dist(ivec2 p)
		{
			return dists[p[1] * lab.width + p[0]];
		}

		ivec2 &prev(ivec2 p)
		{
			return prevs[p[1] * lab.width + p[0]];
		}

		void distances(ivec2 start)
		{
			CAGE_ASSERT(walkable(start));

			dists.clear();
			prevs.clear();
			dists.resize(lab.cells.size(), m);
			prevs.resize(lab.cells.size(), ivec2(-1));

			std::vector<ivec2> q;
			q.push_back(start);
			dist(start) = 0;
			while (!q.empty())
			{
				const ivec2 p = q.front();
				q.erase(q.begin());
				const uint32 pd = dist(p);
				CAGE_ASSERT(pd != m);
				CAGE_ASSERT(walkable(p));
				for (ivec2 it : { p + ivec2(-1, 0), p + ivec2(1, 0), p + ivec2(0, -1), p + ivec2(0, 1) })
				{
					if (!walkable(it))
						continue;
					if (dist(it) <= pd + 1)
						continue;
					dist(it) = pd + 1;
					prev(it) = p;
					q.push_back(it);
				}
			}
		}

		ivec2 pickRandom()
		{
			while (true)
			{
				const ivec2 p = ivec2(randomRange(1u, lab.width - 1), randomRange(1u, lab.height - 1));
				if (walkable(p))
					return p;
			}
		}

		ivec2 pickFurthest()
		{
			uint32 d = 0;
			ivec2 p = ivec2(-1);
			for (uint32 i = 0; i < dists.size(); i++)
			{
				uint32 k = dists[i];
				if (k == m)
					continue;
				if (k > d)
				{
					d = k;
					p = ivec2(i % lab.width, i / lab.width);
				}
			}
			return p;
		}

		void markPath(ivec2 start, ivec2 goal)
		{
			uint32 cnt = 1;
			start = prev(start);
			while (start != goal)
			{
				cell(start) = Cell::Path;
				start = prev(start);
				cnt++;
			}
			lab.path = cnt;
		}

		void paths()
		{
			distances(pickRandom());
			ivec2 start = pickFurthest();
			distances(start);
			ivec2 goal = pickFurthest();
			std::swap(start, goal);
			cell(start) = Cell::Start;
			cell(goal) = Cell::Goal;
			markPath(start, goal);
		}
	};

	std::string iconSize(const std::string &icon)
	{
		return std::string() + "<span style=\"margin:-2.5pt; font-size:70%\">" + icon + "</span>";
	}

	const std::string Start = iconSize(u8"\U0001F415"); // dog
	const std::string Goal = iconSize(u8"\U0001F9B4"); // bone
	const std::string Path = u8"\U0001F43E"; // paw prints
	const std::string Empty = u8"\u00B7"; // middle dot
	const std::string Space = u8"&nbsp;"; // indivisible space

	string connectedWall(uint32 neighbors)
	{
		switch (neighbors)
		{
		case 0: return "O";
		case 1: return u8"\u2500";
		case 2: return u8"\u2502";
		case 3: return u8"\u2518";
		case 4: return u8"\u2500";
		case 5: return u8"\u2500";
		case 6: return u8"\u2514";
		case 7: return u8"\u2534";
		case 8: return u8"\u2502";
		case 9: return u8"\u2510";
		case 10: return u8"\u2502";
		case 11: return u8"\u2524";
		case 12: return u8"\u250C";
		case 13: return u8"\u252C";
		case 14: return u8"\u251C";
		case 15: return u8"\u253C";
		default:
			CAGE_THROW_CRITICAL(Exception, "invalid connected walls code");
		}
	}

	string convertToAscii(Labyrinth &lab, uint32 y)
	{
		string res;
		for (uint32 x = 1; x < lab.width - 1; x++)
		{
			switch (lab.cell(ivec2(x, y)))
			{
			case Cell::Wall:
			{
				uint32 neighbors = 0;
				if (lab.cell(ivec2(x - 1, y)) == Cell::Wall)
					neighbors += 1; // left
				if (lab.cell(ivec2(x, y - 1)) == Cell::Wall)
					neighbors += 2; // top
				if (lab.cell(ivec2(x + 1, y)) == Cell::Wall)
					neighbors += 4; // right
				if (lab.cell(ivec2(x, y + 1)) == Cell::Wall)
					neighbors += 8; // bottom
				res += connectedWall(neighbors);
			} break;
			case Cell::Start:
				res += "S";
				break;
			case Cell::Goal:
				res += "G";
				break;
			case Cell::Path:
				res += "+";
				break;
			case Cell::Empty:
				res += Empty.c_str();
				break;
			default:
				res += "?";
				break;
			}
		}
		return res;
	}

	std::string addLineBreaks(const std::string &s)
	{
		const auto br = utf8to32("<br>");
		auto a = utf8to32(s);
		std::vector<uint32> r;
		for (const auto &it : enumerate(a))
		{
			if (it.index % 55 == 0)
				for (auto i : br)
					r.push_back(i);
			r.push_back(*it);
		}
		const auto b = utf32to8(r);
		return std::string(b.data(), b.size());
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
}

void cipherLabyrinth()
{
	constexpr uint32 cypherIndex = 1;

	rngReseed();
	//randomChance(); // offset rng
	Labyrinth lab;
	while (true)
	{
		lab = generate();
		Paths paths(lab);
		paths.paths();
		if (isPrime(lab.path))
			break;
	}

	std::string plain, cipher;
	for (uint32 y = 1; y < lab.height - 1; y++)
	{
		const string s = convertToAscii(lab, y);
		plain += (replace(s, "?", "&nbsp;") + "<br>").c_str();
		string r = trim(s, true, true, "?");
		r = replace(r, "?", string(Space.c_str()));
		r = replace(r, "+", Empty);
		r = replace(r, "S", Start);
		r = replace(r, "G", Goal);
		cipher += (r + "<br>").c_str();
	}
	//cipher = addLineBreaks(cipher);
	//replace(cipher, " ", "&nbsp;");
	//replace(plain, " ", "&nbsp;");
	plain += string(stringizer() + "<hr>" + lab.path).c_str();

	{
		const std::string o = generateHeader(cypherIndex, "Prvočíslo") + cipher + generateFooter(cypherIndex);
		writeOutput(cypherIndex, o);
	}

	{
		Holder<File> f = writeFile(pathJoin(solutionPath, "labyrinth.html"));
		const std::string o = std::string() + "<html><head><style>*{font-size:14pt; font-family:monospace;}</style></head><body>" + plain + "</body></html>";
		f->write(o);
		f->close();
	}
}
