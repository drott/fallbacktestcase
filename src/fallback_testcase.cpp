// g++-4.8 fallback_testcase.cpp `pkg-config --libs --cflags fontconfig` -o fallbacktestcase

#include <fontconfig/fontconfig.h>
#include <stdio.h>

int main() {
    FcInit();
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
    FcFontSet* fs = FcFontList(config, pat, os);
    printf("Total matching fonts: %d\n", fs->nfont);
    for (int i=0; fs && i < fs->nfont; ++i) {
       FcPattern* font = fs->fonts[i];
       FcChar8* file, *style, *family;
       if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
           FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
           FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch)
       {
          printf("Filename: %s (family %s, style %s)\n", file, family, style);
       }
    }
    if (fs) FcFontSetDestroy(fs);
}

