// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"

U_NAMESPACE_BEGIN

ಸಂರಚನಾ LSR final : ಸಾರ್ವಜನಿಕ UMemory {
    ಸ್ಥಿರ constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    ಸ್ಥಿರ constexpr int32_t EXPLICIT_LSR = 7;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_SCRIPT = 2;
    ಸ್ಥಿರ constexpr int32_t EXPLICIT_REGION = 1;
    ಸ್ಥಿರ constexpr int32_t IMPLICIT_LSR = 0;
    ಸ್ಥಿರ constexpr int32_t DONT_CARE_FLAGS = 0;

    ಧ್ರುವ ವರ್ಣ *language;
    ಧ್ರುವ ವರ್ಣ *script;
    ಧ್ರುವ ವರ್ಣ *region;
    ವರ್ಣ *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ಧ್ರುವ ವರ್ಣ *lang, ಧ್ರುವ ವರ್ಣ *scr, ಧ್ರುವ ವರ್ಣ *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(ವರ್ಣ prefix, ಧ್ರುವ ವರ್ಣ *lang, ಧ್ರುವ ವರ್ಣ *scr, ಧ್ರುವ ವರ್ಣ *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(ಧ್ರುವ LSR &other) = delete;
    ಅನ್ತಃಸ್ಥ ~LSR() {
        // Pure inline code for almost all instances.
        ಯದಿ (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(ಧ್ರುವ LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ಸ್ಥಿರ int32_t indexForRegion(ಧ್ರುವ ವರ್ಣ *region);

    UBool isEquivalentTo(ಧ್ರುವ LSR &other) ಧ್ರುವ;
    bool operator==(ಧ್ರುವ LSR &other) ಧ್ರುವ;

    ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ LSR &other) ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ !operator==(other);
    }

    LSR &setHashCode();

ಗುಪ್ತ:
    ಶೂನ್ಯ deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
