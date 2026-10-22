// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lsr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(ਅੱਖਰ ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *lang, ਅਚਲ ਅੱਖਰ *scr, ਅਚਲ ਅੱਖਰ *r, int32_t f,
         UError_ਕੋਡ &error_ਕੋਡ) :
        language(ਖਾਲੀ_ਸੂਚਕ), script(ਖਾਲੀ_ਸੂਚਕ), region(r),
        region_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ_For_Region(region)), flags(f) {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        CharString langScript;
        langScript.append(ਅਗੇਤਰ, error_ਕੋਡ).append(lang, error_ਕੋਡ).append('\0', error_ਕੋਡ);
        int32_t script_ਵਿੱਥ = langScript.length();
        langScript.append(ਅਗੇਤਰ, error_ਕੋਡ).append(scr, error_ਕੋਡ);
        owned = langScript.clone_ਅੰਕੜੇ(error_ਕੋਡ);
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            language = owned;
            script = owned + script_ਵਿੱਥ;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_ਕੋਡ &error_ਕੋਡ) :
        language(ਖਾਲੀ_ਸੂਚਕ), script(ਖਾਲੀ_ਸੂਚਕ), region(ਖਾਲੀ_ਸੂਚਕ),
        region_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ_For_Region(r.ਅੰਕੜੇ())), flags(f) {
    ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
        CharString ਅੰਕੜੇ;
        ਅੰਕੜੇ.append(lang, error_ਕੋਡ).append('\0', error_ਕੋਡ);
        int32_t script_ਵਿੱਥ = ਅੰਕੜੇ.length();
        ਅੰਕੜੇ.append(scr, error_ਕੋਡ).append('\0', error_ਕੋਡ);
        int32_t region_ਵਿੱਥ = ਅੰਕੜੇ.length();
        ਅੰਕੜੇ.append(r, error_ਕੋਡ);
        owned = ਅੰਕੜੇ.clone_ਅੰਕੜੇ(error_ਕੋਡ);
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            language = owned;
            script = owned + script_ਵਿੱਥ;
            region = owned + region_ਵਿੱਥ;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_ਸੂਚਕਾਂਕ(other.region_ਸੂਚਕਾਂਕ), flags(other.flags),
        hash_ਕੋਡ(other.hash_ਕੋਡ) {
    ਜੇ (owned != ਖਾਲੀ_ਸੂਚਕ) {
        other.language = other.script = "";
        other.owned = ਖਾਲੀ_ਸੂਚਕ;
        other.hash_ਕੋਡ = 0;
    }
}

ਖਾਲੀ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::ਸੰਚਾਲਕ=(LSR &&other) noexcept {
    ਇਹ->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_ਸੂਚਕਾਂਕ = other.region_ਸੂਚਕਾਂਕ;
    flags = other.flags;
    owned = other.owned;
    hash_ਕੋਡ = other.hash_ਕੋਡ;
    ਜੇ (owned != ਖਾਲੀ_ਸੂਚਕ) {
        other.language = other.script = "";
        other.owned = ਖਾਲੀ_ਸੂਚਕ;
        other.hash_ਕੋਡ = 0;
    }
    ਵਾਪਸ *ਇਹ;
}

UBool LSR::is_Equivalent_ਮੰਜ਼ਿਲ(ਅਚਲ LSR &other) ਅਚਲ {
    ਵਾਪਸ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_ਸੂਚਕਾਂਕ == other.region_ਸੂਚਕਾਂਕ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_ਸੂਚਕਾਂਕ > 0 || uprv_strcmp(region, other.region) == 0);
}

ਤਾਰਕਿਕ LSR::ਸੰਚਾਲਕ==(ਅਚਲ LSR &other) ਅਚਲ {
    ਵਾਪਸ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_ਸੂਚਕਾਂਕ == other.region_ਸੂਚਕਾਂਕ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_ਸੂਚਕਾਂਕ > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::ਸੂਚਕਾਂਕ_For_Region(ਅਚਲ ਅੱਖਰ *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    ਜੇ (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        ਜੇ (b < 0 || 9 < b) { ਵਾਪਸ 0; }
        c = region[2] - '0';
        ਜੇ (c < 0 || 9 < c || region[3] != 0) { ਵਾਪਸ 0; }
        ਵਾਪਸ (10 * a + b) * 10 + c + 1;
    } ਨਹੀਂਤਾਂ {  // letters: "DE"
        a = uprv_ਉੱਤਲਾ_Ordinal(c);
        ਜੇ (a < 0 || 25 < a) { ਵਾਪਸ 0; }
        int32_t b = uprv_ਉੱਤਲਾ_Ordinal(region[1]);
        ਜੇ (b < 0 || 25 < b || region[2] != 0) { ਵਾਪਸ 0; }
        ਵਾਪਸ 26 * a + b + 1001;
    }
    ਵਾਪਸ 0;
}

LSR &LSR::ਸੈੱਟ_Hash_ਕੋਡ() {
    ਜੇ (hash_ਕੋਡ == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_ਸੂਚਕਾਂਕ;
        hash_ਕੋਡ = h * 37 + flags;
    }
    ਵਾਪਸ *ਇਹ;
}

U_NAMESPACE_ਸਿਰਾ
