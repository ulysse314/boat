#ifndef Debug_h
#define Debug_h

#define DebugInfo() DebugInfo1("")
#define DebugInfo1(a) printDebugInfo(__FILE__, __LINE__, a)

void printDebugInfo(const char *fileName, unsigned int lineNumber, const char *info);

#endif // Debug_h