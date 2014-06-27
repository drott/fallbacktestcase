#include <iostream>
#include <fontconfig/fontconfig.h>
#include <list>
#include <string.h>
#include <vector>


struct HasCharEntry {
    HasCharEntry() : numSupportedFonts(0), supportedBy(std::vector<std::string>()) {};
    FcChar32 theChar;
    unsigned numSupportedFonts;
    std::vector<std::string> supportedBy;
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

bool compareNumSupported(const HasCharEntry& first, const HasCharEntry& second) {
    return first.numSupportedFonts < second.numSupportedFonts;
}

int main(int argc, const char * argv[])
{
    FcInit();
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcPatternCreate();
    FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, FC_CHARSET, (char *) 0);
    FcFontSet* fs = FcFontList(config, pat, os);
    printf("Total matching fonts: %d\n", fs->nfont);
    
    fillUcsChars();

    for (int i=0; fs && i < fs->nfont; ++i) {
        FcPattern* font = fs->fonts[i];
        FcChar8* file, *style, *family;
        if (FcPatternGetString(font, FC_FILE, 0, &file) != FcResultMatch ||
            FcPatternGetString(font, FC_FAMILY, 0, &family) != FcResultMatch ||
            FcPatternGetString(font, FC_STYLE, 0, &style) != FcResultMatch)
            continue;

        FcCharSet* charSet;
        if (FcPatternGetCharSet(font, FC_CHARSET, 0,  &charSet) != FcResultMatch) {
            printf("Font %s cannot report charset.\n", family);
            continue;
        }

        HasCharList::iterator j = charSupportList.begin();
        while (j != charSupportList.end())
        {
            if (FcCharSetHasChar(charSet, (*j).theChar)) {
                (*j).numSupportedFonts++;
                (*j).supportedBy.push_back(std::string((char*)file));
                
            }
            ++j;
        }
        
    }
    
    charSupportList.sort(compareNumSupported);
    int lastSupportedNumFonts = 0;
    for (HasCharList::iterator i = charSupportList.begin(); i != charSupportList.end(); i++) {
        if (!(*i).numSupportedFonts)
            continue;
        
        if ((*i).numSupportedFonts > lastSupportedNumFonts) {
            printf("\n<br>Supported by %d fonts:<br>\n", (*i).numSupportedFonts);
            lastSupportedNumFonts = (*i).numSupportedFonts;
        }
        
        printf("&#x%x; ",(*i).theChar);
    }
    
    if (fs)
        FcFontSetDestroy(fs);
    return 0;
}

