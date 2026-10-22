// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

LSR::LSR(ಅಕ್ಷರ ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lang, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *r, int32_t f,
         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        language(ಶೂನ್ಯಸೂಚಕ), script(ಶೂನ್ಯಸೂಚಕ), region(r),
        region_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ_For_Region(region)), flags(f) {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        CharString langScript;
        langScript.append(ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ).append(lang, error_ಸಂಕೇತ).append('\0', error_ಸಂಕೇತ);
        int32_t script_ಸ್ಥಾನಾಂತರ = langScript.length();
        langScript.append(ಪೂರ್ವಪ್ರತ್ಯಯ, error_ಸಂಕೇತ).append(scr, error_ಸಂಕೇತ);
        owned = langScript.clone_ದತ್ತಾಂಶ(error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            language = owned;
            script = owned + script_ಸ್ಥಾನಾಂತರ;
        }
    }
}

LSR::LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        language(ಶೂನ್ಯಸೂಚಕ), script(ಶೂನ್ಯಸೂಚಕ), region(ಶೂನ್ಯಸೂಚಕ),
        region_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ_For_Region(r.ದತ್ತಾಂಶ())), flags(f) {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
        CharString ದತ್ತಾಂಶ;
        ದತ್ತಾಂಶ.append(lang, error_ಸಂಕೇತ).append('\0', error_ಸಂಕೇತ);
        int32_t script_ಸ್ಥಾನಾಂತರ = ದತ್ತಾಂಶ.length();
        ದತ್ತಾಂಶ.append(scr, error_ಸಂಕೇತ).append('\0', error_ಸಂಕೇತ);
        int32_t region_ಸ್ಥಾನಾಂತರ = ದತ್ತಾಂಶ.length();
        ದತ್ತಾಂಶ.append(r, error_ಸಂಕೇತ);
        owned = ದತ್ತಾಂಶ.clone_ದತ್ತಾಂಶ(error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            language = owned;
            script = owned + script_ಸ್ಥಾನಾಂತರ;
            region = owned + region_ಸ್ಥಾನಾಂತರ;
        }
    }
}

LSR::LSR(LSR &&other) noexcept :
        language(other.language), script(other.script), region(other.region), owned(other.owned),
        region_ಸೂಚ್ಯಂಕ(other.region_ಸೂಚ್ಯಂಕ), flags(other.flags),
        hash_ಸಂಕೇತ(other.hash_ಸಂಕೇತ) {
    ಆದರೆ (owned != ಶೂನ್ಯಸೂಚಕ) {
        other.language = other.script = "";
        other.owned = ಶೂನ್ಯಸೂಚಕ;
        other.hash_ಸಂಕೇತ = 0;
    }
}

ಶೂನ್ಯ LSR::deleteOwned() {
    uprv_free(owned);
}

LSR &LSR::ಕ್ರಿಯಾಕಾರಕ=(LSR &&other) noexcept {
    ಇದು->~LSR();
    language = other.language;
    script = other.script;
    region = other.region;
    region_ಸೂಚ್ಯಂಕ = other.region_ಸೂಚ್ಯಂಕ;
    flags = other.flags;
    owned = other.owned;
    hash_ಸಂಕೇತ = other.hash_ಸಂಕೇತ;
    ಆದರೆ (owned != ಶೂನ್ಯಸೂಚಕ) {
        other.language = other.script = "";
        other.owned = ಶೂನ್ಯಸೂಚಕ;
        other.hash_ಸಂಕೇತ = 0;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool LSR::is_Equivalent_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ LSR &other) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_ಸೂಚ್ಯಂಕ == other.region_ಸೂಚ್ಯಂಕ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_ಸೂಚ್ಯಂಕ > 0 || uprv_strcmp(region, other.region) == 0);
}

ತಾರ್ಕಿಕ LSR::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ LSR &other) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ
        uprv_strcmp(language, other.language) == 0 &&
        uprv_strcmp(script, other.script) == 0 &&
        region_ಸೂಚ್ಯಂಕ == other.region_ಸೂಚ್ಯಂಕ &&
        // Compare regions if both are ill-formed (and their indexes are 0).
        (region_ಸೂಚ್ಯಂಕ > 0 || uprv_strcmp(region, other.region) == 0) &&
        flags == other.flags;
}

int32_t LSR::ಸೂಚ್ಯಂಕ_For_Region(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region) {
    int32_t c = region[0];
    int32_t a = c - '0';
    ಆದರೆ (0 <= a && a <= 9) {  // digits: "419"
        int32_t b = region[1] - '0';
        ಆದರೆ (b < 0 || 9 < b) { ಹಿಂತಿರುಗಿ 0; }
        c = region[2] - '0';
        ಆದರೆ (c < 0 || 9 < c || region[3] != 0) { ಹಿಂತಿರುಗಿ 0; }
        ಹಿಂತಿರುಗಿ (10 * a + b) * 10 + c + 1;
    } ಇಲ್ಲದಿದ್ದರೆ {  // letters: "DE"
        a = uprv_ಊರ್ಧ್ವ_Ordinal(c);
        ಆದರೆ (a < 0 || 25 < a) { ಹಿಂತಿರುಗಿ 0; }
        int32_t b = uprv_ಊರ್ಧ್ವ_Ordinal(region[1]);
        ಆದರೆ (b < 0 || 25 < b || region[2] != 0) { ಹಿಂತಿರುಗಿ 0; }
        ಹಿಂತಿರುಗಿ 26 * a + b + 1001;
    }
    ಹಿಂತಿರುಗಿ 0;
}

LSR &LSR::ಗಣ_Hash_ಸಂಕೇತ() {
    ಆದರೆ (hash_ಸಂಕೇತ == 0) {
        uint32_t h = ustr_hashCharsN(language, static_cast<int32_t>(uprv_strlen(language)));
        h = h * 37 + ustr_hashCharsN(script, static_cast<int32_t>(uprv_strlen(script)));
        h = h * 37 + region_ಸೂಚ್ಯಂಕ;
        hash_ಸಂಕೇತ = h * 37 + flags;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

U_NAMESPACE_ಕೊನೆ
