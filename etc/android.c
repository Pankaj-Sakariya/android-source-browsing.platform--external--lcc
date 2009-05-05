/* x86s running Linux */

#include <string.h>
#include <stdlib.h>
#include "get_my_path.h"

static char cppPath[PATH_MAX];
static char rccPath[PATH_MAX];
static char asPath[PATH_MAX];
static char ldPath[PATH_MAX];

static char armElfXPath[PATH_MAX];
static char crtBeginPath[PATH_MAX];
static char crtEndPath[PATH_MAX];
static char targetOutLibDir[PATH_MAX];



#define LCCDIR "/usr/bin"

char *suffixes[] = { ".c", ".i", ".s", ".o", ".out", 0 };
char inputs[256] = "";
char *cpp[] = { cppPath,
    "-U__GNUC__", "-D_POSIX_SOURCE", "-D__STDC__=1", "-D__STRICT_ANSI__",
    "-Dunix", "-Dlinux",
    "-D__unix__", "-D__i386__", "-D__linux__", "-D__signed__=signed",
    "$1", "$2", "$3", 0 };
char *include[] = {"-I" LCCDIR "include", "-I" LCCDIR "gcc/include", "-I/usr/include", 0 };
char *com[] = {rccPath, "-target=arm/linux", "$1", "$2", "$3", 0 };
char *as[] = { asPath, "-o", "$3", "$1", "$2", 0 };
char *ld[] = {
    ldPath, "-T", armElfXPath, "-dynamic-linker=/system/bin/linker",
    "-o", "$3",
    crtBeginPath,
    "$1", "$2",
    crtEndPath,
    "-L", targetOutLibDir,
    "-lc", "-lm", "-ldl",
    0 };

extern char *concat(char *, char *);

int option(char *arg) {
      if (strncmp(arg, "-lccdir=", 8) == 0) {
        cpp[0] = concat(&arg[8], "/gcc/cpp");
        include[0] = concat("-I", concat(&arg[8], "/include"));
        include[1] = concat("-I", concat(&arg[8], "/gcc/include"));
        ld[9]  = concat(&arg[8], "/gcc/crtbegin.o");
        ld[12] = concat("-L", &arg[8]);
        ld[14] = concat("-L", concat(&arg[8], "/gcc"));
        ld[19] = concat(&arg[8], "/gcc/crtend.o");
        com[0] = concat(&arg[8], "/rcc");
    } else if (strcmp(arg, "-p") == 0 || strcmp(arg, "-pg") == 0) {
        ld[7] = "/usr/lib/gcrt1.o";
        ld[18] = "-lgmon";
    } else if (strcmp(arg, "-b") == 0)
        ;
    else if (strcmp(arg, "-g") == 0)
        ;
    else if (strncmp(arg, "-ld=", 4) == 0)
        ld[0] = &arg[4];
    else if (strcmp(arg, "-static") == 0) {
        ld[3] = "-static";
        ld[4] = "";
    } else
        return 0;
    return 1;
}

static void combine(char* dest, size_t destSize, const char* a, const char* b) {
    if (destSize < 1) {
        return;
    }
    size_t aLen = strlen(a);
    size_t bLen = strlen(b);
    if (aLen + bLen > destSize-1) {
        dest[0] = 0;
        return;
    }
    memcpy(dest, a, aLen);
    memcpy(dest + aLen, b, bLen + 1);
}

static void append(char* dest, size_t destSize, const char* a) {
    size_t destLen = strlen(dest);
    size_t aLen = strlen(a);
    if (destLen + aLen > destSize-1) {
        dest[0] = 0;
        return;
    }
    memcpy(dest + destLen, a, aLen + 1);
}

static void getEnv(char* buf, size_t bufLen, const char* name) {
    buf[0] = '\0';
    const char* value = getenv(name);
    if (value != NULL) {
        strncpy(buf, value, bufLen);
        buf[bufLen-1] = '\0';
    }
}

void platform_init() {
    char myDir[PATH_MAX];
    char androidProductOut[PATH_MAX];
    char androidToolChain[PATH_MAX];
    char androidBuildTop[PATH_MAX];
    get_my_path(myDir);
    getEnv(androidProductOut, PATH_MAX, "ANDROID_PRODUCT_OUT");
    combine(targetOutLibDir, PATH_MAX, androidProductOut, "/obj/lib");
    getEnv(androidToolChain, PATH_MAX, "ANDROID_TOOLCHAIN");
    getEnv(androidBuildTop, PATH_MAX, "ANDROID_BUILD_TOP");
    char* lastSlash = strrchr(myDir, '/');
    if (lastSlash != NULL) {
        lastSlash[1] = '\0';
        combine(cppPath, PATH_MAX, myDir, "lcc-cpp");
        combine(rccPath, PATH_MAX, myDir, "lcc-rcc");
        combine(asPath, PATH_MAX, androidToolChain, "/arm-eabi-as");
        combine(ldPath, PATH_MAX, androidToolChain, "/arm-eabi-ld");

        combine(armElfXPath, PATH_MAX, androidBuildTop, "/build/core/armelf.x");
        combine(crtBeginPath, PATH_MAX, targetOutLibDir, "/crtbegin_dynamic.o");
        combine(crtEndPath, PATH_MAX, targetOutLibDir, "/crtend_android.o");
    }
}
