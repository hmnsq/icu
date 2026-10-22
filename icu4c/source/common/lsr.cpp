// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#ضمن "unicode/utypes.h"
#ضمن "charstr.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "lsr.h"
#ضمن "uinvchar.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(حرف prefix, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    𐡄𐡍 (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        𐡄𐡍 (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    𐡄𐡍 (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        𐡄𐡍 (U_SUCCESS(errorCode)) {
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
    𐡄𐡍 (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

فراغ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    هذا->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    𐡄𐡍 (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    أرجع *هذا;
}

UBool LSR::isEquivalentTo(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقي LSR::operator==(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(ثابت حرف *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    𐡄𐡍 (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        𐡄𐡍 (b < 0 || 9 < b) { أرجع 0; }
        c = region[2] - '0';
        𐡄𐡍 (c < 0 || 9 < c || region[3] != 0) { أرجع 0; }
        أرجع (10 * a + b) * 10 + c + 1;
    } وإلا {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        𐡄𐡍 (a < 0 || 25 < a) { أرجع 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        𐡄𐡍 (b < 0 || 25 < b || region[2] != 0) { أرجع 0; }
        أرجع 26 * a + b + 1001;
    }
    أرجع 0;
}

LSR &LSR::setHashCode() {
    𐡄𐡍 (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    أرجع *هذا;
}

U_NAMESPACE_END
