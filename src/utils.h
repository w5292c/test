#ifndef TEST_UTILS_H

class Utils
{
public:
  static void hexDump(const char *pData);
  static void hexDump(const unsigned char *pData, int length);
};

#endif /* TEST_UTILS_H */
