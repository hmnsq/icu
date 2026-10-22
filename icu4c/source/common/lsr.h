// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cstring.h"

U_NAMESPACE_BEGIN

সংরচনা LSR final : সার্বজনিক UMemory {
    স্থির constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    স্থির constexpr int32_t EXPLICIT_LSR = 7;
    স্থির constexpr int32_t EXPLICIT_LANGUAGE = 4;
    স্থির constexpr int32_t EXPLICIT_SCRIPT = 2;
    স্থির constexpr int32_t EXPLICIT_REGION = 1;
    স্থির constexpr int32_t IMPLICIT_LSR = 0;
    স্থির constexpr int32_t DONT_CARE_FLAGS = 0;

    ধ্রুব বর্ণ *language;
    ধ্রুব বর্ণ *script;
    ধ্রুব বর্ণ *region;
    বর্ণ *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ধ্রুব বর্ণ *lang, ধ্রুব বর্ণ *scr, ধ্রুব বর্ণ *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(বর্ণ prefix, ধ্রুব বর্ণ *lang, ধ্রুব বর্ণ *scr, ধ্রুব বর্ণ *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(ধ্রুব LSR &other) = delete;
    অন্তঃস্থ ~LSR() {
        // Pure inline code for almost all instances.
        যদি (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(ধ্রুব LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    স্থির int32_t indexForRegion(ধ্রুব বর্ণ *region);

    UBool isEquivalentTo(ধ্রুব LSR &other) ধ্রুব;
    bool operator==(ধ্রুব LSR &other) ধ্রুব;

    অন্তঃস্থ bool operator!=(ধ্রুব LSR &other) ধ্রুব {
        প্রত্যাগম !operator==(other);
    }

    LSR &setHashCode();

গুপ্ত:
    শূন্য deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
