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

LSR::LSR(var_na prefix, const var_na *lang, const var_na *scr, const var_na *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    ပေႃး (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        ပေႃး (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    ပေႃး (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        ပေႃး (U_SUCCESS(errorCode)) {
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
    ပေႃး (owned != nullptr) {
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
    ပေႃး (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    ပွၵ်ႈ *this;
}

UBool LSR::isEquivalentTo(const LSR &other) const {
    ပွၵ်ႈ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

bool LSR::operator==(const LSR &other) const {
    ပွၵ်ႈ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(const var_na *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    ပေႃး (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        ပေႃး (b < 0 || 9 < b) { ပွၵ်ႈ 0; }
        c = region[2] - '0';
        ပေႃး (c < 0 || 9 < c || region[3] != 0) { ပွၵ်ႈ 0; }
        ပွၵ်ႈ (10 * a + b) * 10 + c + 1;
    } ပေႃးဢမ်ႇ {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        ပေႃး (a < 0 || 25 < a) { ပွၵ်ႈ 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        ပေႃး (b < 0 || 25 < b || region[2] != 0) { ပွၵ်ႈ 0; }
        ပွၵ်ႈ 26 * a + b + 1001;
    }
    ပွၵ်ႈ 0;
}

LSR &LSR::setHashCode() {
    ပေႃး (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    ပွၵ်ႈ *this;
}

U_NAMESPACE_END
