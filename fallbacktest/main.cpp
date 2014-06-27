//
//  main.cpp
//  fallbacktest
//
//  Created by Dominik Rottsches on 27/06/14.
//  Copyright (c) 2014 Dominik Rottsches. All rights reserved.
//

#include <iostream>
#include <fontconfig/fontconfig.h>
#include <list>
#include <string.h>


struct HasCharEntry {
    HasCharEntry() : numSupportedFonts(0), filename("") {};
    FcChar32 theChar;
    unsigned numSupportedFonts;
    std::string filename;
};

typedef std::list<HasCharEntry> HasCharList;

static const unsigned long MAX_UNICODE = 0x10FFFF;

HasCharList charSupportList(MAX_UNICODE, HasCharEntry());

void fillUcsChars() {
    unsigned int charCount = 0;
    for(HasCharList::iterator i = charSupportList.begin(); i != charSupportList.end(); ++i) {
        (*i).theChar = charCount++;
    }
}

int main(int argc, const char * argv[])
{
    FcInit();
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
    FcFontSet* fs = FcFontList(config, pat, os);
    printf("Total matching fonts: %d\n", fs->nfont);
    
    fillUcsChars();

    for (int i=0; fs && i < fs->nfont && i < 10; ++i) {
        FcPattern* font = fs->fonts[i];
        FcChar8* file, *style, *family;
        if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch &&
            FcPatternGetString(font, FC_FAMILY, 0, &family) == FcResultMatch &&
            FcPatternGetString(font, FC_STYLE, 0, &style) == FcResultMatch)
        {
            printf("Filename: %s (family %s, style %s)\n", file, family, style);
        }

        FcCharSet* charSet;
        FcPatternGetCharSet(font, FC_CHARSET, 0,  &charSet);

        HasCharList::iterator j = charSupportList.begin();
        while (j != charSupportList.end())
        {
            if (FcCharSetHasChar(charSet, (*j).theChar)) {
                (*j).numSupportedFonts++;
                if ((*j).numSupportedFonts > 1) {
                    printf("Char %x removed, already supported by %s\n", (*j).theChar, (*j).filename.c_str());
                    charSupportList.erase(j++); // remove and increment
                    continue;
                }
                printf("First font to support %x, adding filename %s.\n", (*j).theChar, file);
                (*j).filename = std::string((char*)file);
            }
            ++j;
        }
        
    }
    if (fs)
        FcFontSetDestroy(fs);
    return 0;
}

