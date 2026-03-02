// test_getos.c
// Unit tests for getos.c pure functions.
// Build: gcc -DTESTING test_getos.c -o test_getos
// Run:   ./test_getos

#define TESTING
#include "getos.c"

#include <stdio.h>

static int passed = 0;
static int failed = 0;

#define CHECK(desc, expr)                                          \
    do {                                                           \
        if (expr) {                                                \
            printf("  PASS  %s\n", desc);                         \
            passed++;                                             \
        } else {                                                   \
            printf("  FAIL  %s\n", desc);                         \
            failed++;                                             \
        }                                                          \
    } while (0)

// ---------------------------------------------------------------------------

static void test_is_valid_host(void)
{
    printf("is_valid_host\n");
    CHECK("plain hostname",            is_valid_host("google.com")           == 1);
    CHECK("subdomain with dash",       is_valid_host("sub-domain.example.com") == 1);
    CHECK("IPv4 address",              is_valid_host("192.168.1.1")          == 1);
    CHECK("IPv6 address",              is_valid_host("::1")                  == 1);
    CHECK("underscore allowed",        is_valid_host("host_name")            == 1);
    CHECK("semicolon rejected",        is_valid_host("host;evil")            == 0);
    CHECK("pipe rejected",             is_valid_host("host|evil")            == 0);
    CHECK("ampersand rejected",        is_valid_host("host&evil")            == 0);
    CHECK("subshell rejected",         is_valid_host("host$(cmd)")           == 0);
    CHECK("space rejected",            is_valid_host("host name")            == 0);
    CHECK("backslash rejected",        is_valid_host("host\\evil")           == 0);
    CHECK("empty string accepted",     is_valid_host("")                     == 1);
}

// ---------------------------------------------------------------------------

static void test_find_executable(void)
{
    printf("find_executable\n");
    CHECK("sh is found",               find_executable("sh")                 == 1);
    CHECK("nonexistent binary",        find_executable("__no_such_binary__") == 0);
}

// ---------------------------------------------------------------------------

static int ttl_arr[] = {32, 64, 128, 255};
static int ttl_n     = 4;

static void test_findClosest(void)
{
    printf("findClosest\n");
    CHECK("exact match 32",            findClosest(ttl_arr, ttl_n, 32)  == 32);
    CHECK("exact match 64",            findClosest(ttl_arr, ttl_n, 64)  == 64);
    CHECK("exact match 128",           findClosest(ttl_arr, ttl_n, 128) == 128);
    CHECK("exact match 255",           findClosest(ttl_arr, ttl_n, 255) == 255);
    CHECK("below min clamps to 32",    findClosest(ttl_arr, ttl_n, 0)   == 32);
    CHECK("above max clamps to 255",   findClosest(ttl_arr, ttl_n, 300) == 255);
    CHECK("63 rounds to 64",           findClosest(ttl_arr, ttl_n, 63)  == 64);
    CHECK("90 rounds to 64",           findClosest(ttl_arr, ttl_n, 90)  == 64);
    CHECK("96 rounds to 128",          findClosest(ttl_arr, ttl_n, 96)  == 128);
    CHECK("200 rounds to 255",         findClosest(ttl_arr, ttl_n, 200) == 255);
    CHECK("midpoint 96 breaks to 128", findClosest(ttl_arr, ttl_n, 96)  == 128);
}

// ---------------------------------------------------------------------------

static void test_getClosest(void)
{
    printf("getClosest\n");
    CHECK("closer to lower",      getClosest(64, 128, 90)  == 64);
    CHECK("closer to upper",      getClosest(64, 128, 100) == 128);
    CHECK("equidistant → upper",  getClosest(64, 128, 96)  == 128);
    CHECK("closer to lower (32)", getClosest(32, 64,  40)  == 32);
    CHECK("closer to upper (64)", getClosest(32, 64,  50)  == 64);
    CHECK("closer to upper (255)",getClosest(128, 255, 200) == 255);
}

// ---------------------------------------------------------------------------

int main(void)
{
    test_is_valid_host();
    printf("\n");
    test_find_executable();
    printf("\n");
    test_getClosest();
    printf("\n");
    test_findClosest();

    printf("\n%d passed, %d failed\n", passed, failed);
    return failed == 0 ? 0 : 1;
}
