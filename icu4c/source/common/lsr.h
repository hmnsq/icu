// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#شامل "unicode/stringpiece.h"
#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "cstring.h"

U_NAMESPACE_BEGIN

ساختار LSR final : عمومی UMemory {
    ایستا constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    ایستا constexpr int32_t EXPLICIT_LSR = 7;
    ایستا constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ایستا constexpr int32_t EXPLICIT_SCRIPT = 2;
    ایستا constexpr int32_t EXPLICIT_REGION = 1;
    ایستا constexpr int32_t IMPLICIT_LSR = 0;
    ایستا constexpr int32_t DONT_CARE_FLAGS = 0;

    ثابت نویسه *language;
    ثابت نویسه *script;
    ثابت نویسه *region;
    نویسه *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ثابت نویسه *lang, ثابت نویسه *scr, ثابت نویسه *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(نویسه prefix, ثابت نویسه *lang, ثابت نویسه *scr, ثابت نویسه *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(ثابت LSR &other) = delete;
    درونخطی ~LSR() {
        // Pure inline code for almost all instances.
        اگر (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(ثابت LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ایستا int32_t indexForRegion(ثابت نویسه *region);

    UBool isEquivalentTo(ثابت LSR &other) ثابت;
    bool operator==(ثابت LSR &other) ثابت;

    درونخطی bool operator!=(ثابت LSR &other) ثابت {
        بازگشت !operator==(other);
    }

    LSR &setHashCode();

خصوصی:
    تهی deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
