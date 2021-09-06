#include "cage-core/core.h"

#include <string>

using namespace cage;

extern const String inputPath;
extern const String outputPath;
extern const String solutionPath;

std::string generateHeader(uint32 index, const String &name);
std::string generateFooter();
std::string readInput(const String &name);
void writeOutput(const String &name, const std::string &output);

void replace(std::string &str, const std::string &what, const std::string &with);
std::string replaceCzech(const std::string &s);
std::string replaceLfToSpaces(const std::string &s);
std::string toUpper(const std::string &s);
std::string toLower(const std::string &s);

void rngReseed();
bool isPrime(uint32 n);

