// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "charstr.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "lsr.h"
#અન્તર્ભાવ "uinvchar.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(અક્ષર prefix, અચલ અક્ષર *lang, અચલ અક્ષર *scr, અચલ અક્ષર *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    જો (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        જો (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    જો (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        જો (U_SUCCESS(errorCode)) {
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
    જો (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

શૂન્ય LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    આ->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    જો (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    પાછા *આ;
}

UBool LSR::isEquivalentTo(અચલ LSR &other) અચલ {
    પાછા
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

તાર્કિક LSR::operator==(અચલ LSR &other) અચલ {
    પાછા
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(અચલ અક્ષર *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    જો (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        જો (b < 0 || 9 < b) { પાછા 0; }
        c = region[2] - '0';
        જો (c < 0 || 9 < c || region[3] != 0) { પાછા 0; }
        પાછા (10 * a + b) * 10 + c + 1;
    } નહીંતર {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        જો (a < 0 || 25 < a) { પાછા 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        જો (b < 0 || 25 < b || region[2] != 0) { પાછા 0; }
        પાછા 26 * a + b + 1001;
    }
    પાછા 0;
}

LSR &LSR::setHashCode() {
    જો (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    પાછા *આ;
}

U_NAMESPACE_END
