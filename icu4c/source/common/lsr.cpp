// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(वर्ण prefix, ध्रुव वर्ण *lang, ध्रुव वर्ण *scr, ध्रुव वर्ण *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    जइ (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        जइ (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    जइ (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        जइ (U_SUCCESS(errorCode)) {
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
    जइ (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

शून्य LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    एतत्->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    जइ (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    प्रत्यागम *एतत्;
}

UBool LSR::isEquivalentTo(ध्रुव LSR &other) ध्रुव {
    प्रत्यागम
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

तार्किक LSR::operator==(ध्रुव LSR &other) ध्रुव {
    प्रत्यागम
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(ध्रुव वर्ण *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    जइ (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        जइ (b < 0 || 9 < b) { प्रत्यागम 0; }
        c = region[2] - '0';
        जइ (c < 0 || 9 < c || region[3] != 0) { प्रत्यागम 0; }
        प्रत्यागम (10 * a + b) * 10 + c + 1;
    } अण्णहा {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        जइ (a < 0 || 25 < a) { प्रत्यागम 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        जइ (b < 0 || 25 < b || region[2] != 0) { प्रत्यागम 0; }
        प्रत्यागम 26 * a + b + 1001;
    }
    प्रत्यागम 0;
}

LSR &LSR::setHashCode() {
    जइ (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    प्रत्यागम *एतत्;
}

U_NAMESPACE_END
