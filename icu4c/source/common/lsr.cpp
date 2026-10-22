// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#شامل "unicode/utypes.h"
#شامل "charstr.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "lsr.h"
#شامل "uinvchar.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(حرف سابقہ, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
         UError_رمز &error_رمز) :
        language(خالی_اشاریہ), script(خالی_اشاریہ), region(r),
        region_اشاریہ(اشاریہ_For_Region(region)), flags(f) {
    اگر (U_SUCCESS(error_رمز)) {
        CharString langScript;
        langScript.append(سابقہ, error_رمز).append(lang, error_رمز).append('\0', error_رمز);
        int32_t script_انحراف = langScript.length();
        langScript.append(سابقہ, error_رمز).append(scr, error_رمز);
        owned = langScript.clone_ڈیٹا(error_رمز);
        اگر (U_SUCCESS(error_رمز)) {
            language = owned;
            script = owned + script_انحراف;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_رمز &error_رمز) :
        language(خالی_اشاریہ), script(خالی_اشاریہ), region(خالی_اشاریہ),
        region_اشاریہ(اشاریہ_For_Region(r.ڈیٹا())), flags(f) {
    اگر (U_SUCCESS(error_رمز)) {
        CharString ڈیٹا;
        ڈیٹا.append(lang, error_رمز).append('\0', error_رمز);
        int32_t script_انحراف = ڈیٹا.length();
        ڈیٹا.append(scr, error_رمز).append('\0', error_رمز);
        int32_t region_انحراف = ڈیٹا.length();
        ڈیٹا.append(r, error_رمز);
        owned = ڈیٹا.clone_ڈیٹا(error_رمز);
        اگر (U_SUCCESS(error_رمز)) {
            language = owned;
            script = owned + script_انحراف;
            region = owned + region_انحراف;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_اشاریہ(other.region_اشاریہ), flags(other.flags),
        hash_رمز(other.hash_رمز) {
    اگر (owned != خالی_اشاریہ) {
        other.language = other.script = "";
        other.owned = خالی_اشاریہ;
        other.hash_رمز = 0;
    }
}

خالی LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::عامل=(LSR &&other) noexcept {
    یہ->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_اشاریہ = other.region_اشاریہ;
    flags = other.flags;
    owned = other.owned;
    hash_رمز = other.hash_رمز;
    اگر (owned != خالی_اشاریہ) {
        other.language = other.script = "";
        other.owned = خالی_اشاریہ;
        other.hash_رمز = 0;
    }
    واپس *یہ;
}

UBool LSR::is_Equivalent_مقصد(ثابت LSR &other) ثابت {
    واپس
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_اشاریہ == other.region_اشاریہ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_اشاریہ > 0 || uprv_strcmp(region, other.region) == 0);
}

منطقی LSR::عامل==(ثابت LSR &other) ثابت {
    واپس
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
    اگر (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        اگر (b < 0 || 9 < b) { واپس 0; }
        c = region[2] - '0';
        اگر (c < 0 || 9 < c || region[3] != 0) { واپس 0; }
        واپس (10 * a + b) * 10 + c + 1;
    } ورنہ {  // letters: "DE"
        a = uprv_بالائی_Ordinal(c);
        اگر (a < 0 || 25 < a) { واپس 0; }
        int32_t b = uprv_بالائی_Ordinal(region[1]);
        اگر (b < 0 || 25 < b || region[2] != 0) { واپس 0; }
        واپس 26 * a + b + 1001;
    }
    واپس 0;
}

LSR &LSR::سیٹ_Hash_رمز() {
    اگر (hash_رمز == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_اشاریہ;
        hash_رمز = h * 37 + flags;
    }
    واپس *یہ;
}

U_NAMESPACE_انتہا
