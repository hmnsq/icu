// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LSR_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LSR_H__

#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

U_NAMESPACE_BEGIN

ರಚನೆ LSR final : ಸಾರ್ವಜನಿಕ UMemory {
    ಸ್ಥಿರ constexpr int32_t REGION_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ = 1001 + 26 * 26;

    ಸ್ಥಿರ constexpr int32_t EXPLICIT_LSR = 7;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_SCRIPT = 2;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_REGION = 1;
    ಸ್ಥಿರ constexpr int32_t IMPLICIT_LSR = 0;
    ಸ್ಥಿರ constexpr int32_t DONT_CARE_FLAGS = 0;

    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region;
    ಅಕ್ಷರ *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_ಸೂಚ್ಯಂಕ = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hash_ಸಂಕೇತ = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lang, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_ಸೂಚ್ಯಂಕ(ಸೂಚ್ಯಂಕ_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(ಅಕ್ಷರ ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lang, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scr, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *r, int32_t f,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    LSR(LSR &&other) noexcept;
    LSR(ಸ್ಥಿರಾಂಕ LSR &other) = delete;
    ಸಾಲೊಳಗೆ ~LSR() {
        // Pure inline code for almost all instances.
        ಆದರೆ (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &ಕ್ರಿಯಾಕಾರಕ=(LSR &&other) noexcept;
    LSR &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ಸ್ಥಿರ int32_t ಸೂಚ್ಯಂಕ_For_Region(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region);

    UBool is_Equivalent_ಗಮ್ಯ(ಸ್ಥಿರಾಂಕ LSR &other) ಸ್ಥಿರಾಂಕ;
    bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ LSR &other) ಸ್ಥಿರಾಂಕ;

    ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ LSR &other) ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(other);
    }

    LSR &ಗಣ_Hash_ಸಂಕೇತ();

ಖಾಸಗಿ:
    ಶೂನ್ಯ deleteOwned();
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // __LSR_H__
