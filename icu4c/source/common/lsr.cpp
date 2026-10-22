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

LSR::LSR(অক্ষর prefix, ধ্রুবক অক্ষর *lang, ধ্রুবক অক্ষর *scr, ধ্রুবক অক্ষর *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        region_সূচক(সূচক_For_Region(region)), flags(f) {
    যদি (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.clone_তথ্য(errorCode);
        যদি (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        region_সূচক(সূচক_For_Region(r.তথ্য())), flags(f) {
    যদি (U_SUCCESS(errorCode)) {
        CharString তথ্য;
        তথ্য.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = তথ্য.length();
        তথ্য.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = তথ্য.length();
        তথ্য.append(r, errorCode);
        owned = তথ্য.clone_তথ্য(errorCode);
        যদি (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
            region = owned + regionOffset;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_সূচক(other.region_সূচক), flags(other.flags),
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
    এই->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_সূচক = other.region_সূচক;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    যদি (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    ফেরত *এই;
}

UBool LSR::isEquivalentTo(ধ্রুবক LSR &other) ধ্রুবক {
    ফেরত
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_সূচক == other.region_সূচক &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_সূচক > 0 || uprv_strcmp(region, other.region) == 0);
}

যৌক্তিক LSR::operator==(ধ্রুবক LSR &other) ধ্রুবক {
    ফেরত
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_সূচক == other.region_সূচক &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_সূচক > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::সূচক_For_Region(ধ্রুবক অক্ষর *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    যদি (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        যদি (b < 0 || 9 < b) { ফেরত 0; }
        c = region[2] - '0';
        যদি (c < 0 || 9 < c || region[3] != 0) { ফেরত 0; }
        ফেরত (10 * a + b) * 10 + c + 1;
    } অন্যথা {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        যদি (a < 0 || 25 < a) { ফেরত 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        যদি (b < 0 || 25 < b || region[2] != 0) { ফেরত 0; }
        ফেরত 26 * a + b + 1001;
    }
    ফেরত 0;
}

LSR &LSR::setHashCode() {
    যদি (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_সূচক;
        hashCode = h * 37 + flags;
    }
    ফেরত *এই;
}

U_NAMESPACE_END
