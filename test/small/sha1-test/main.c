#include "sha1.h"
#include <stdio.h>
#include <string.h>

#define LENGTH_OF_ARRAY(x) (sizeof(x)/sizeof((x)[0]))

int main (int argc, char* argv[])
{
    typedef struct _TestVec
    {
        const char* src;
        const char* dst;
    } TestVec;
    
    const TestVec tests[] =
    {
        // empty string
        {
            "",
            "da39a3ee5e6b4b0d3255bfef95601890afd80709"
        },
        // simple strings
        {
            "abc",
            "a9993e364706816aba3e25717850c26c9cd0d89d"
        },
        {
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
            "84983e441c3bd26ebaae4aa1f95129e5e54670f1"
        },
        {   // From wikipedia
            "The quick brown fox jumps over the lazy dog",
            "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"
        },
        {
            "The quick brown fox jumps over the lazy cog",
            "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3"
        },
        
        // exactly 512 bits
        {
            "0123456701234567012345670123456701234567012345670123456701234567",
            "e0c094e867ef46c350ef54a7f59dd60bed92ae83"
        },
        // exceeding 512 bits
        {
            "Jaska ajaa allaskaljaa. Jaskalla jalalla kaljaa. Kassalla jalka, jalalla kassa. Lakas kalja.",
            "9cd84ad78816c6c39fbed822ae8188fd8e6afd11"
        },
        // multiple of 512
        {
            "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
            "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
            "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX"
            "hoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAXhoaxHOAX",
            "5350efaf647d6b227d235e1263007e957f3151f4"
        }
    };

    int i, failCount;
    for (i = 0, failCount = 0; i < LENGTH_OF_ARRAY(tests); i++)
    {
        const TestVec* test = &tests[i];
        Sha1Digest computed = Sha1_get(test->src, strlen(test->src));
        Sha1Digest expected = Sha1Digest_fromStr(test->dst);

        printf("Testing %d/%zu...", i+1, LENGTH_OF_ARRAY(tests));

        if (memcmp(&computed, &expected, sizeof(Sha1Digest)))
        {
            char cStr[41];  // two bytes per digit plus terminator
            printf("failed!\n");
            Sha1Digest_toStr(&computed, cStr);
            printf("Expected %s, got %s\n", test->dst, cStr);
            failCount++;
        }
        else
            printf("success.\n");
    }

    return failCount;
}
