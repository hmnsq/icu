// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"

U_NAMESPACE_BEGIN

ସଂରଚନା LSR final : ସର୍ବସାଧାରଣ UMemory {
    ସ୍ଥିର constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    ସ୍ଥିର constexpr int32_t EXPLICIT_LSR = 7;
    ସ୍ଥିର constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ସ୍ଥିର constexpr int32_t EXPLICIT_SCRIPT = 2;
    ସ୍ଥିର constexpr int32_t EXPLICIT_REGION = 1;
    ସ୍ଥିର constexpr int32_t IMPLICIT_LSR = 0;
    ସ୍ଥିର constexpr int32_t DONT_CARE_FLAGS = 0;

    ଅଚଳ ଅକ୍ଷର *language;
    ଅଚଳ ଅକ୍ଷର *script;
    ଅଚଳ ଅକ୍ଷର *region;
    ଅକ୍ଷର *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ଅଚଳ ଅକ୍ଷର *lang, ଅଚଳ ଅକ୍ଷର *scr, ଅଚଳ ଅକ୍ଷର *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(ଅକ୍ଷର prefix, ଅଚଳ ଅକ୍ଷର *lang, ଅଚଳ ଅକ୍ଷର *scr, ଅଚଳ ଅକ୍ଷର *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(ଅଚଳ LSR &other) = delete;
    ଧାଡ଼ିଭିତରେ ~LSR() {
        // Pure inline code for almost all instances.
        ଯଦି (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(ଅଚଳ LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ସ୍ଥିର int32_t indexForRegion(ଅଚଳ ଅକ୍ଷର *region);

    UBool isEquivalentTo(ଅଚଳ LSR &other) ଅଚଳ;
    bool operator==(ଅଚଳ LSR &other) ଅଚଳ;

    ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ LSR &other) ଅଚଳ {
        ଫେରନ୍ତୁ !operator==(other);
    }

    LSR &setHashCode();

ବ୍ୟକ୍ତିଗତ:
    ଶୂନ୍ୟ deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
