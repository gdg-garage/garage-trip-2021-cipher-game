#include "cage-core/core.h"

#include <string>

using namespace cage;

extern const string inputPath;
extern const string outputPath;
extern const string solutionPath;

std::string generateHeader(uint32 index, const string &name);
std::string generateFooter();
std::string readInput(const string &name);
void writeOutput(const string &name, const std::string &output);

void replace(std::string &str, const std::string &what, const std::string &with);
std::string replaceCzech(const std::string &s);
std::string replaceLfToSpaces(const std::string &s);
std::string toUpper(const std::string &s);
std::string toLower(const std::string &s);

void rngReseed();
bool isPrime(uint32 n);

