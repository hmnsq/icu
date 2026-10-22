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

LSR::LSR(حرف مختاړی, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UError_کوډ &error_کوډ) :
        language(تش_اشاره), script(تش_اشاره), region(r),
        region_شاخص(شاخص_For_Region(region)), flags(f) {
    إذا (U_SUCCESS(error_کوډ)) {
        CharString langScript;
        langScript.append(مختاړی, error_کوډ).append(lang, error_کوډ).append('\0', error_کوډ);
        int32_t script_دننه_واټن = langScript.length();
        langScript.append(مختاړی, error_کوډ).append(scr, error_کوډ);
        owned = langScript.clone_معلومات(error_کوډ);
        إذا (U_SUCCESS(error_کوډ)) {
            language = owned;
            script = owned + script_دننه_واټن;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_کوډ &error_کوډ) :
        language(تش_اشاره), script(تش_اشاره), region(تش_اشاره),
        region_شاخص(شاخص_For_Region(r.معلومات())), flags(f) {
    إذا (U_SUCCESS(error_کوډ)) {
        CharString معلومات;
        معلومات.append(lang, error_کوډ).append('\0', error_کوډ);
        int32_t script_دننه_واټن = معلومات.length();
        معلومات.append(scr, error_کوډ).append('\0', error_کوډ);
        int32_t region_دننه_واټن = معلومات.length();
        معلومات.append(r, error_کوډ);
        owned = معلومات.clone_معلومات(error_کوډ);
        إذا (U_SUCCESS(error_کوډ)) {
            language = owned;
            script = owned + script_دننه_واټن;
            region = owned + region_دننه_واټن;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_شاخص(other.region_شاخص), flags(other.flags),
        hash_کوډ(other.hash_کوډ) {
    إذا (owned != تش_اشاره) {
        other.language = other.script = "";
        other.owned = تش_اشاره;
        other.hash_کوډ = 0;
    }
}

فراغ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::چلوونکی=(LSR &&other) noexcept {
    هذا->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_شاخص = other.region_شاخص;
    flags = other.flags;
    owned = other.owned;
    hash_کوډ = other.hash_کوډ;
    إذا (owned != تش_اشاره) {
        other.language = other.script = "";
        other.owned = تش_اشاره;
        other.hash_کوډ = 0;
    }
    أرجع *هذا;
}

UBool LSR::is_Equivalent_منزل(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_شاخص == other.region_شاخص &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_شاخص > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقي LSR::چلوونکی==(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_شاخص == other.region_شاخص &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_شاخص > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::شاخص_For_Region(ثابت حرف *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    إذا (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        إذا (b < 0 || 9 < b) { أرجع 0; }
        c = region[2] - '0';
        إذا (c < 0 || 9 < c || region[3] != 0) { أرجع 0; }
        أرجع (10 * a + b) * 10 + c + 1;
    } وإلا {  // letters: "DE"
        a = uprv_پاسنی_Ordinal(c);
        إذا (a < 0 || 25 < a) { أرجع 0; }
        int32_t b = uprv_پاسنی_Ordinal(region[1]);
        إذا (b < 0 || 25 < b || region[2] != 0) { أرجع 0; }
        أرجع 26 * a + b + 1001;
    }
    أرجع 0;
}

LSR &LSR::سېټ_Hash_کوډ() {
    إذا (hash_کوډ == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_شاخص;
        hash_کوډ = h * 37 + flags;
    }
    أرجع *هذا;
}

U_NAMESPACE_پای_ټکی
