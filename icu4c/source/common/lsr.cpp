// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "lsr.h"
#شامل "uinvchar.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(نویسه prefix, ثابت نویسه *lang, ثابت نویسه *scr, ثابت نویسه *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    اگر (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        اگر (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    اگر (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        اگر (U_SUCCESS(errorCode)) {
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
    اگر (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

تهی LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    این->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    اگر (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    بازگشت *این;
}

UBool LSR::isEquivalentTo(ثابت LSR &other) ثابت {
    بازگشت
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقی LSR::operator==(ثابت LSR &other) ثابت {
    بازگشت
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(ثابت نویسه *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    اگر (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        اگر (b < 0 || 9 < b) { بازگشت 0; }
        c = region[2] - '0';
        اگر (c < 0 || 9 < c || region[3] != 0) { بازگشت 0; }
        بازگشت (10 * a + b) * 10 + c + 1;
    } وگرنه {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        اگر (a < 0 || 25 < a) { بازگشت 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        اگر (b < 0 || 25 < b || region[2] != 0) { بازگشت 0; }
        بازگشت 26 * a + b + 1001;
    }
    بازگشت 0;
}

LSR &LSR::setHashCode() {
    اگر (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    بازگشت *این;
}

U_NAMESPACE_END
