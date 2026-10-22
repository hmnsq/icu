// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#include "unicode/utypes.h"
#include "charstr.h"
#include "cmemory.h"
#include "cstring.h"
#include "lsr.h"
#include "uinvchar.h"
#include "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(char prefix, const char *lang, const char *scr, const char *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    nếu (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        nếu (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    nếu (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        nếu (U_SUCCESS(errorCode)) {
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
    nếu (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

void LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    this->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    nếu (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    trả_về *this;
}

UBool LSR::isEquivalentTo(const LSR &other) const {
    trả_về
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

bool LSR::operator==(const LSR &other) const {
    trả_về
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(const char *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    nếu (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        nếu (b < 0 || 9 < b) { trả_về 0; }
        c = region[2] - '0';
        nếu (c < 0 || 9 < c || region[3] != 0) { trả_về 0; }
        trả_về (10 * a + b) * 10 + c + 1;
    } không_thì {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        nếu (a < 0 || 25 < a) { trả_về 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        nếu (b < 0 || 25 < b || region[2] != 0) { trả_về 0; }
        trả_về 26 * a + b + 1001;
    }
    trả_về 0;
}

LSR &LSR::setHashCode() {
    nếu (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    trả_về *this;
}

U_NAMESPACE_END
