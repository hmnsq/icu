// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "charstr.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "lsr.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(ଅକ୍ଷର prefix, ଅଚଳ ଅକ୍ଷର *lang, ଅଚଳ ଅକ୍ଷର *scr, ଅଚଳ ଅକ୍ଷର *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    ଯଦି (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        ଯଦି (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    ଯଦି (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        ଯଦି (U_SUCCESS(errorCode)) {
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
    ଯଦି (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

ଶୂନ୍ୟ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    ଏହି->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    ଯଦି (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UBool LSR::isEquivalentTo(ଅଚଳ LSR &other) ଅଚଳ {
    ଫେରନ୍ତୁ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

ତାର୍କିକ LSR::operator==(ଅଚଳ LSR &other) ଅଚଳ {
    ଫେରନ୍ତୁ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(ଅଚଳ ଅକ୍ଷର *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    ଯଦି (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        ଯଦି (b < 0 || 9 < b) { ଫେରନ୍ତୁ 0; }
        c = region[2] - '0';
        ଯଦି (c < 0 || 9 < c || region[3] != 0) { ଫେରନ୍ତୁ 0; }
        ଫେରନ୍ତୁ (10 * a + b) * 10 + c + 1;
    } ଅନ୍ୟଥା {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        ଯଦି (a < 0 || 25 < a) { ଫେରନ୍ତୁ 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        ଯଦି (b < 0 || 25 < b || region[2] != 0) { ଫେରନ୍ତୁ 0; }
        ଫେରନ୍ତୁ 26 * a + b + 1001;
    }
    ଫେରନ୍ତୁ 0;
}

LSR &LSR::setHashCode() {
    ଯଦି (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

U_NAMESPACE_END
