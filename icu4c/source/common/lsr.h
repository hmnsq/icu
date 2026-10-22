// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cstring.h"

U_NAMESPACE_BEGIN

સંરચના LSR final : જાહેર UMemory {
    સ્થિર constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    સ્થિર constexpr int32_t EXPLICIT_LSR = 7;
    સ્થિર constexpr int32_t EXPLICIT_LANGUAGE = 4;
    સ્થિર constexpr int32_t EXPLICIT_SCRIPT = 2;
    સ્થિર constexpr int32_t EXPLICIT_REGION = 1;
    સ્થિર constexpr int32_t IMPLICIT_LSR = 0;
    સ્થિર constexpr int32_t DONT_CARE_FLAGS = 0;

    અચલ અક્ષર *language;
    અચલ અક્ષર *script;
    અચલ અક્ષર *region;
    અક્ષર *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(અચલ અક્ષર *lang, અચલ અક્ષર *scr, અચલ અક્ષર *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(અક્ષર prefix, અચલ અક્ષર *lang, અચલ અક્ષર *scr, અચલ અક્ષર *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(અચલ LSR &other) = delete;
    અંતઃરેખ ~LSR() {
        // Pure inline code for almost all instances.
        જો (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(અચલ LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    સ્થિર int32_t indexForRegion(અચલ અક્ષર *region);

    UBool isEquivalentTo(અચલ LSR &other) અચલ;
    bool operator==(અચલ LSR &other) અચલ;

    અંતઃરેખ bool operator!=(અચલ LSR &other) અચલ {
        પાછા !operator==(other);
    }

    LSR &setHashCode();

ખાનગી:
    શૂન્ય deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
