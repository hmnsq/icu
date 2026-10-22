// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#समावेश "unicode/utypes.h"
#समावेश "charstr.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "lsr.h"
#समावेश "uinvchar.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(अक्षर prefix, स्थिर अक्षर *lang, स्थिर अक्षर *scr, स्थिर अक्षर *r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(r),
        region_सूचकांक(सूचकांक_For_Region(region)), flags(f) {
    अगर (U_SUCCESS(errorCode)) {
        CharString langScript;
        langScript.append(prefix, errorCode).append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = langScript.length();
        langScript.append(prefix, errorCode).append(scr, errorCode);
        owned = langScript.clone_तथ्य(errorCode);
        अगर (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UErrorCode &errorCode) :
        language(nullptr), script(nullptr), region(nullptr),
        region_सूचकांक(सूचकांक_For_Region(r.तथ्य())), flags(f) {
    अगर (U_SUCCESS(errorCode)) {
        CharString तथ्य;
        तथ्य.append(lang, errorCode).append('\0', errorCode);
        int32_t scriptOffset = तथ्य.length();
        तथ्य.append(scr, errorCode).append('\0', errorCode);
        int32_t regionOffset = तथ्य.length();
        तथ्य.append(r, errorCode);
        owned = तथ्य.clone_तथ्य(errorCode);
        अगर (U_SUCCESS(errorCode)) {
            language = owned;
            script = owned + scriptOffset;
            region = owned + regionOffset;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_सूचकांक(other.region_सूचकांक), flags(other.flags),
        hashCode(other.hashCode) {
    अगर (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
}

व्योम LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::चालक=(LSR &&other) noexcept {
    यह->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_सूचकांक = other.region_सूचकांक;
    flags = other.flags;
    owned = other.owned;
    hashCode = other.hashCode;
    अगर (owned != nullptr) {
        other.language = other.script = "";
        other.owned = nullptr;
        other.hashCode = 0;
    }
    वापस *यह;
}

UBool LSR::isEquivalentTo(स्थिर LSR &other) स्थिर {
    वापस
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_सूचकांक == other.region_सूचकांक &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_सूचकांक > 0 || uprv_strcmp(region, other.region) == 0);
}

bool LSR::चालक==(स्थिर LSR &other) स्थिर {
    वापस
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_सूचकांक == other.region_सूचकांक &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_सूचकांक > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::सूचकांक_For_Region(स्थिर अक्षर *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    अगर (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        अगर (b < 0 || 9 < b) { वापस 0; }
        c = region[2] - '0';
        अगर (c < 0 || 9 < c || region[3] != 0) { वापस 0; }
        वापस (10 * a + b) * 10 + c + 1;
    } अन्यथा {  // letters: "DE"
        a = uprv_upperOrdinal(c);
        अगर (a < 0 || 25 < a) { वापस 0; }
        int32_t b = uprv_upperOrdinal(region[1]);
        अगर (b < 0 || 25 < b || region[2] != 0) { वापस 0; }
        वापस 26 * a + b + 1001;
    }
    वापस 0;
}

LSR &LSR::setHashCode() {
    अगर (hashCode == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_सूचकांक;
        hashCode = h * 37 + flags;
    }
    वापस *यह;
}

U_NAMESPACE_END
