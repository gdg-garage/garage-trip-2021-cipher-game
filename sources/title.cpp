#include "common.h"

namespace
{
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
}
