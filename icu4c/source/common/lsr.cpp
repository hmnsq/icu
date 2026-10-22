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

LSR::LSR(حرف اڳياڙي, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UError_ڪوڊ &error_ڪوڊ) :
        language(خالي_اشارو), script(خالي_اشارو), region(r),
        region_اشاريو(اشاريو_For_Region(region)), flags(f) {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        CharString langScript;
        langScript.append(اڳياڙي, error_ڪوڊ).append(lang, error_ڪوڊ).append('\0', error_ڪوڊ);
        int32_t script_وٿي = langScript.length();
        langScript.append(اڳياڙي, error_ڪوڊ).append(scr, error_ڪوڊ);
        owned = langScript.clone_معلومات(error_ڪوڊ);
        إذا (U_SUCCESS(error_ڪوڊ)) {
            language = owned;
            script = owned + script_وٿي;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_ڪوڊ &error_ڪوڊ) :
        language(خالي_اشارو), script(خالي_اشارو), region(خالي_اشارو),
        region_اشاريو(اشاريو_For_Region(r.معلومات())), flags(f) {
    إذا (U_SUCCESS(error_ڪوڊ)) {
        CharString معلومات;
        معلومات.append(lang, error_ڪوڊ).append('\0', error_ڪوڊ);
        int32_t script_وٿي = معلومات.length();
        معلومات.append(scr, error_ڪوڊ).append('\0', error_ڪوڊ);
        int32_t region_وٿي = معلومات.length();
        معلومات.append(r, error_ڪوڊ);
        owned = معلومات.clone_معلومات(error_ڪوڊ);
        إذا (U_SUCCESS(error_ڪوڊ)) {
            language = owned;
            script = owned + script_وٿي;
            region = owned + region_وٿي;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_اشاريو(other.region_اشاريو), flags(other.flags),
        hash_ڪوڊ(other.hash_ڪوڊ) {
    إذا (owned != خالي_اشارو) {
        other.language = other.script = "";
        other.owned = خالي_اشارو;
        other.hash_ڪوڊ = 0;
    }
}

فراغ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::عامل=(LSR &&other) noexcept {
    هذا->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_اشاريو = other.region_اشاريو;
    flags = other.flags;
    owned = other.owned;
    hash_ڪوڊ = other.hash_ڪوڊ;
    إذا (owned != خالي_اشارو) {
        other.language = other.script = "";
        other.owned = خالي_اشارو;
        other.hash_ڪوڊ = 0;
    }
    أرجع *هذا;
}

UBool LSR::is_Equivalent_منزل(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_اشاريو == other.region_اشاريو &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_اشاريو > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقي LSR::عامل==(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_اشاريو == other.region_اشاريو &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_اشاريو > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::اشاريو_For_Region(ثابت حرف *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    إذا (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        إذا (b < 0 || 9 < b) { أرجع 0; }
        c = region[2] - '0';
        إذا (c < 0 || 9 < c || region[3] != 0) { أرجع 0; }
        أرجع (10 * a + b) * 10 + c + 1;
    } وإلا {  // letters: "DE"
        a = uprv_مٿانهون_Ordinal(c);
        إذا (a < 0 || 25 < a) { أرجع 0; }
        int32_t b = uprv_مٿانهون_Ordinal(region[1]);
        إذا (b < 0 || 25 < b || region[2] != 0) { أرجع 0; }
        أرجع 26 * a + b + 1001;
    }
    أرجع 0;
}

LSR &LSR::سيٽ_Hash_ڪوڊ() {
    إذا (hash_ڪوڊ == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_اشاريو;
        hash_ڪوڊ = h * 37 + flags;
    }
    أرجع *هذا;
}

U_NAMESPACE_پڇاڙي
