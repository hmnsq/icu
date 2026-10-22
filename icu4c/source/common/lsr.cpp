// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "lsr.h"
#അന്തര്ഭാവ "uinvchar.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(അക്ഷരം prefix, സ്ഥിരം അക്ഷരം *lang, സ്ഥിരം അക്ഷരം *scr, സ്ഥിരം അക്ഷരം *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    എങ്കിൽ (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
            region = owned + regionOffset;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        regionIndex(other.regionIndex), flags(other.flags),
        hashCode(other.hashCode) {
    എങ്കിൽ (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

ശൂന്യം LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    ഇത്->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    എങ്കിൽ (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    തിരികെ *ഇത്;
}

UBool LSR::isEquivalentTo(സ്ഥിരം LSR &other) സ്ഥിരം {
    തിരികെ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

തർക്കം LSR::operator==(സ്ഥിരം LSR &other) സ്ഥിരം {
    തിരികെ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(സ്ഥിരം അക്ഷരം *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    എങ്കിൽ (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        എങ്കിൽ (b < 0 || 9 < b) { തിരികെ 0; }
        c = region[2] - '0';
        എങ്കിൽ (c < 0 || 9 < c || region[3] != 0) { തിരികെ 0; }
        തിരികെ (10 * a + b) * 10 + c + 1;
    } അല്ലെങ്കിൽ {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        എങ്കിൽ (a < 0 || 25 < a) { തിരികെ 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        എങ്കിൽ (b < 0 || 25 < b || region[2] != 0) { തിരികെ 0; }
        തിരികെ 26 * a + b + 1001;
    }
    തിരികെ 0;
}

LSR &LSR::setHashCode() {
    എങ്കിൽ (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    തിരികെ *ഇത്;
}

U_NAMESPACE_END
