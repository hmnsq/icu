// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "lsr.h"
#অন্তর্ভাব "uinvchar.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(বর্ণ prefix, ধ্রুব বর্ণ *lang, ধ্রুব বর্ণ *scr, ধ্রুব বর্ণ *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        regionIndex(indexForRegion(region)), flags(f) {
    যদি (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.cloneData(errorCode);
        যদি (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        regionIndex(indexForRegion(r.data())), flags(f) {
    যদি (U_SUCCESS(errorCode)) {
        CharString data;
        data.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = data.length();
        data.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = data.length();
        data.append(r, errorCode);
        owned = data.cloneData(errorCode);
        যদি (U_SUCCESS(errorCode)) {
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
    যদি (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

শূন্য LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::operator=(LSR &&other) noexcept {
    এতত্->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    regionIndex = other.regionIndex;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    যদি (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    প্রত্যাগম *এতত্;
}

UBool LSR::isEquivalentTo(ধ্রুব LSR &other) ধ্রুব {
    প্রত্যাগম
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0);
}

তার্কিক LSR::operator==(ধ্রুব LSR &other) ধ্রুব {
    প্রত্যাগম
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        regionIndex == other.regionIndex &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (regionIndex > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::indexForRegion(ধ্রুব বর্ণ *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    যদি (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        যদি (b < 0 || 9 < b) { প্রত্যাগম 0; }
        c = region[2] - '0';
        যদি (c < 0 || 9 < c || region[3] != 0) { প্রত্যাগম 0; }
        প্রত্যাগম (10 * a + b) * 10 + c + 1;
    } অন্যথা {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        যদি (a < 0 || 25 < a) { প্রত্যাগম 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        যদি (b < 0 || 25 < b || region[2] != 0) { প্রত্যাগম 0; }
        প্রত্যাগম 26 * a + b + 1001;
    }
    প্রত্যাগম 0;
}

LSR &LSR::setHashCode() {
    যদি (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + regionIndex;
        hashCode = h * 37 + flags;
    }
    প্রত্যাগম *এতত্;
}

U_NAMESPACE_END
