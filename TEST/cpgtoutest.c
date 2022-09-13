#include <stdio.h>
#include <assert.h>
#include <string.h>
// including the source file is necessary
// to conduct unit tests on static functions
#include "../rtfsed.c"
#include "../STATIC/cpgtou.h"

static unsigned char input[] = { 0x94, 0x45, 0x8e, 0xd2, 0x90, 0xed, 0x8e, 
                                 0x6d, 0x82, 0xaa, 0x8e, 0x98, 0x82, 0xf0, 
                                 0x93, 0x7c, 0x82, 0xb7, 0x81, 0x42, 0x00 };
static unsigned char output[256] = { 0 };

#define ENCOD(x,y) (encode_utf8(y,x))
#define CHECK(x,y) (assert(!strcmp(x,y)))
#define STRINGADD(s1, s2, max) (strncat((char *)s1, (char *)s2, (max - strlen((char *)s1) - 1)))

int main(void) {
    int32_t uccp;
    const int32_t *mult;
    uint8_t xtra = 0U;

    size_t i;

    char u[5];

    for (i = 0; i < strlen((char *)input); i++) {
        uccp = cpgtou(cpgfromcharsetnum(128), input[i], &xtra, &mult);
        if (uccp >= 0) { ENCOD(u, uccp); STRINGADD(output, u, 256); }
        if (uccp == cpMULT) while(*mult) { ENCOD(u, *mult); STRINGADD(output, u, 256); }
    }

    CHECK((char *)output, u8"忍者戦士が侍を倒す。");

    return 0;
}