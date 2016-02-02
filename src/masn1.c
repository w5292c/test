#include "masn1.h"

#include <assert.h>
#include <openssl/asn1t.h>

typedef struct _ProtocolData
{
  long version;
  ASN1_OCTET_STRING *command_name;
  ASN1_OCTET_STRING *args;
} PROTOCOL_DATA_ASN1;

ASN1_SEQUENCE(PROTOCOL_DATA_ASN1) = {
  ASN1_SIMPLE(PROTOCOL_DATA_ASN1, version, LONG),
  ASN1_SIMPLE(PROTOCOL_DATA_ASN1, command_name, ASN1_OCTET_STRING),
  ASN1_SIMPLE(PROTOCOL_DATA_ASN1, args, ASN1_OCTET_STRING)
} ASN1_SEQUENCE_END(PROTOCOL_DATA_ASN1);

IMPLEMENT_ASN1_FUNCTIONS(PROTOCOL_DATA_ASN1);

static void hexDump(const unsigned char *pData, int length);

void test_asn1(void)
{
  unsigned char *buffer = NULL;
  PROTOCOL_DATA_ASN1 data;

  data.version = 12345008;

  ASN1_OCTET_STRING command_name = {0};
  ASN1_OCTET_STRING_set(&command_name, (const unsigned char *)"cmd-0001", 8);
  data.command_name = &command_name;

  ASN1_OCTET_STRING args = {0};
  ASN1_OCTET_STRING_set(&args, (const unsigned char *)"arg-0001", 8);
  data.args = &args;

  const int res = i2d_PROTOCOL_DATA_ASN1(&data, &buffer);

  printf("Result: %d\n", res);
  hexDump(buffer, res);

  /* */
  const unsigned char *p = buffer;
  PROTOCOL_DATA_ASN1 *result = d2i_PROTOCOL_DATA_ASN1(NULL, &p, res);
  assert(result);

  printf("Parsed\n- Version: %ld\n", result->version);

  /* IMPLEMENT_ASN1_ENCODE_FUNCTIONS_fname(stname, itname, fname) \
        stname *d2i_##fname(stname **a, const unsigned char **in, long len) \
        { \
                return (stname *)ASN1_item_d2i((ASN1_VALUE **)a, in, len, ASN1_ITEM_rptr(itname));\
        }*/
  PROTOCOL_DATA_ASN1_free(result);
  free(buffer);
}

void hexDump(const unsigned char *pData, int length)
{
  int i;
  for (i = 0; i < length; ++i) {
    if (!(i % 16)) {
      printf("%4.4x: ", i);
    }
    char byte = pData[i];
    char lowByte = (0xFU&byte) + '0';
    char highByte = (0xFU&(byte>>4)) + '0';
    if (lowByte > '9') {
      // 0x0A => 'A', etc...
      lowByte += 'A' - ('9' + 1);
    }
    if (highByte > '9') {
      // 0x0A => 'A', etc...
      highByte += 'A' - ('9' + 1);
    }
    if (byte < 32) {
      byte = '.';
    }
    printf("%c%c(%c)", highByte, lowByte, byte);
    if (i%16 == 15) {
      printf("\n");
    }
  }
  printf("\n");
}
