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

LSR::LSR(حرف پیشوند, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UError_کد &error_کد) :
        language(اشاره_تهی), script(اشاره_تهی), region(r),
        region_شاخص(شاخص_For_Region(region)), flags(f) {
    إذا (U_SUCCESS(error_کد)) {
        CharString langScript;
        langScript.append(پیشوند, error_کد).append(lang, error_کد).append('\0', error_کد);
        int32_t script_جابه_جایی = langScript.length();
        langScript.append(پیشوند, error_کد).append(scr, error_کد);
        owned = langScript.clone_داده(error_کد);
        إذا (U_SUCCESS(error_کد)) {
            language = owned;
            script = owned + script_جابه_جایی;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_کد &error_کد) :
        language(اشاره_تهی), script(اشاره_تهی), region(اشاره_تهی),
        region_شاخص(شاخص_For_Region(r.داده())), flags(f) {
    إذا (U_SUCCESS(error_کد)) {
        CharString داده;
        داده.append(lang, error_کد).append('\0', error_کد);
        int32_t script_جابه_جایی = داده.length();
        داده.append(scr, error_کد).append('\0', error_کد);
        int32_t region_جابه_جایی = داده.length();
        داده.append(r, error_کد);
        owned = داده.clone_داده(error_کد);
        إذا (U_SUCCESS(error_کد)) {
            language = owned;
            script = owned + script_جابه_جایی;
            region = owned + region_جابه_جایی;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_شاخص(other.region_شاخص), flags(other.flags),
        hash_کد(other.hash_کد) {
    إذا (owned != اشاره_تهی) {
        other.language = other.script = "";
        other.owned = اشاره_تهی;
        other.hash_کد = 0;
    }
}

فراغ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::عملگر=(LSR &&other) noexcept {
    هذا->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_شاخص = other.region_شاخص;
    flags = other.flags;
    owned = other.owned;
    hash_کد = other.hash_کد;
    إذا (owned != اشاره_تهی) {
        other.language = other.script = "";
        other.owned = اشاره_تهی;
        other.hash_کد = 0;
    }
    أرجع *هذا;
}

UBool LSR::is_Equivalent_مقصد(ثابت LSR &other) ثابت {
    أرجع
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_شاخص == other.region_شاخص &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_شاخص > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقي LSR::عملگر==(ثابت LSR &other) ثابت {
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
        a = uprv_فوقانی_Ordinal(c);
        إذا (a < 0 || 25 < a) { أرجع 0; }
        int32_t b = uprv_فوقانی_Ordinal(region[1]);
        إذا (b < 0 || 25 < b || region[2] != 0) { أرجع 0; }
        أرجع 26 * a + b + 1001;
    }
    أرجع 0;
}

LSR &LSR::مجموعه_Hash_کد() {
    إذا (hash_کد == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_شاخص;
        hash_کد = h * 37 + flags;
    }
    أرجع *هذا;
}

U_NAMESPACE_انتها
