// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "charstr.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "lsr.h"
#அந்தர்பாவ "uinvchar.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(எழுத்து prefix, மாறிலி எழுத்து *lang, மாறிலி எழுத்து *scr, மாறிலி எழுத்து *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    எனில் (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        எனில் (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    எனில் (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        எனில் (U_SUCCESS(errorCode)) {
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
    எனில் (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

வெறுமை LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    இது->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    எனில் (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    திரும்பு *இது;
}

UBool LSR::isEquivalentTo(மாறிலி LSR &other) மாறிலி {
    திரும்பு
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

தருக்க LSR::operator==(மாறிலி LSR &other) மாறிலி {
    திரும்பு
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(மாறிலி எழுத்து *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    எனில் (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        எனில் (b < 0 || 9 < b) { திரும்பு 0; }
        c = region[2] - '0';
        எனில் (c < 0 || 9 < c || region[3] != 0) { திரும்பு 0; }
        திரும்பு (10 * a + b) * 10 + c + 1;
    } இல்லையெனில் {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        எனில் (a < 0 || 25 < a) { திரும்பு 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        எனில் (b < 0 || 25 < b || region[2] != 0) { திரும்பு 0; }
        திரும்பு 26 * a + b + 1001;
    }
    திரும்பு 0;
}

LSR &LSR::setHashCode() {
    எனில் (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    திரும்பு *இது;
}

U_NAMESPACE_END
