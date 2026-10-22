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

LSR::LSR(वर्ण prefix, अचर वर्ण *lang, अचर वर्ण *scr, अचर वर्ण *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    यदि (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        यदि (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    यदि (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        यदि (U_SUCCESS(errorCode)) {
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
    यदि (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

शून्य LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::चालक=(LSR &&other) noexcept {
    एतत्->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    यदि (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    फर्काऊ *एतत्;
}

UBool LSR::isEquivalentTo(अचर LSR &other) अचर {
    फर्काऊ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

तार्किक LSR::चालक==(अचर LSR &other) अचर {
    फर्काऊ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(अचर वर्ण *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    यदि (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        यदि (b < 0 || 9 < b) { फर्काऊ 0; }
        c = region[2] - '0';
        यदि (c < 0 || 9 < c || region[3] != 0) { फर्काऊ 0; }
        फर्काऊ (10 * a + b) * 10 + c + 1;
    } अन्यथा {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        यदि (a < 0 || 25 < a) { फर्काऊ 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        यदि (b < 0 || 25 < b || region[2] != 0) { फर्काऊ 0; }
        फर्काऊ 26 * a + b + 1001;
    }
    फर्काऊ 0;
}

LSR &LSR::setHashCode() {
    यदि (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    फर्काऊ *एतत्;
}

U_NAMESPACE_END
