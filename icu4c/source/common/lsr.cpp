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

LSR::LSR(حرف اگیتر, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UError_کوڈ &error_کوڈ) :
        language(خالی_سوچک), script(خالی_سوچک), region(r),
        region_اشاریہ(اشاریہ_For_Region(region)), flags(f) {
    إذا (U_SUCCESS(error_کوڈ)) {
        CharString langScript;
        langScript.append(اگیتر, error_کوڈ).append(lang, error_کوڈ).append('\0', error_کوڈ);
        int32_t script_وتھ = langScript.length();
        langScript.append(اگیتر, error_کوڈ).append(scr, error_کوڈ);
        owned = langScript.clone_انکڑے(error_کوڈ);
        إذا (U_SUCCESS(error_کوڈ)) {
            language = owned;
            script = owned + script_وتھ;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_کوڈ &error_کوڈ) :
        language(خالی_سوچک), script(خالی_سوچک), region(خالی_سوچک),
        region_اشاریہ(اشاریہ_For_Region(r.انکڑے())), flags(f) {
    إذا (U_SUCCESS(error_کوڈ)) {
        CharString انکڑے;
        انکڑے.append(lang, error_کوڈ).append('\0', error_کوڈ);
        int32_t script_وتھ = انکڑے.length();
        انکڑے.append(scr, error_کوڈ).append('\0', error_کوڈ);
        int32_t region_وتھ = انکڑے.length();
        انکڑے.append(r, error_کوڈ);
        owned = انکڑے.clone_انکڑے(error_کوڈ);
        إذا (U_SUCCESS(error_کوڈ)) {
            language = owned;
            script = owned + script_وتھ;
            region = owned + region_وتھ;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_اشاریہ(other.region_اشاریہ), flags(other.flags),
        hash_کوڈ(other.hash_کوڈ) {
    إذا (owned != خالی_سوچک) {
        other.language = other.script = "";
        other.owned = خالی_سوچک;
        other.hash_کوڈ = 0;
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
    region_اشاریہ = other.region_اشاریہ;
    flags = other.flags;
    owned = other.owned;
    hash_کوڈ = other.hash_کوڈ;
    إذا (owned != خالی_سوچک) {
        other.language = other.script = "";
        other.owned = خالی_سوچک;
        other.hash_کوڈ = 0;
    }
    أرجع *هذا;
}

UBool LSR::is_Equivalent_منزل(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_اشاریہ == other.region_اشاریہ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_اشاریہ > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقي LSR::عامل==(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_اشاریہ == other.region_اشاریہ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_اشاریہ > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::اشاریہ_For_Region(ثابت حرف *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    إذا (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        إذا (b < 0 || 9 < b) { أرجع 0; }
        c = region[2] - '0';
        إذا (c < 0 || 9 < c || region[3] != 0) { أرجع 0; }
        أرجع (10 * a + b) * 10 + c + 1;
    } وإلا {  // letters: "DE"
        a = uprv_اتلا_Ordinal(c);
        إذا (a < 0 || 25 < a) { أرجع 0; }
        int32_t b = uprv_اتلا_Ordinal(region[1]);
        إذا (b < 0 || 25 < b || region[2] != 0) { أرجع 0; }
        أرجع 26 * a + b + 1001;
    }
    أرجع 0;
}

LSR &LSR::سیٹ_Hash_کوڈ() {
    إذا (hash_کوڈ == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_اشاریہ;
        hash_کوڈ = h * 37 + flags;
    }
    أرجع *هذا;
}

U_NAMESPACE_سرا
