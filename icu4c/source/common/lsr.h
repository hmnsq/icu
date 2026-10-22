// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cstring.h"

U_NAMESPACE_BEGIN

संरचना LSR final : सार्वजनिक UMemory {
    स्थिर constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    स्थिर constexpr int32_t EXPLICIT_LSR = 7;
    स्थिर constexpr int32_t EXPLICIT_LANGUAGE = 4;
    स्थिर constexpr int32_t EXPLICIT_SCRIPT = 2;
    स्थिर constexpr int32_t EXPLICIT_REGION = 1;
    स्थिर constexpr int32_t IMPLICIT_LSR = 0;
    स्थिर constexpr int32_t DONT_CARE_FLAGS = 0;

    स्थिरांक अक्षर *language;
    स्थिरांक अक्षर *script;
    स्थिरांक अक्षर *region;
    अक्षर *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(स्थिरांक अक्षर *lang, स्थिरांक अक्षर *scr, स्थिरांक अक्षर *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(अक्षर prefix, स्थिरांक अक्षर *lang, स्थिरांक अक्षर *scr, स्थिरांक अक्षर *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(स्थिरांक LSR &other) = delete;
    ओळीत ~LSR() {
        // Pure inline code for almost all instances.
        जर (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &चालक=(LSR &&other) noexcept;
    LSR &चालक=(स्थिरांक LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    स्थिर int32_t indexForRegion(स्थिरांक अक्षर *region);

    UBool isEquivalentTo(स्थिरांक LSR &other) स्थिरांक;
    bool चालक==(स्थिरांक LSR &other) स्थिरांक;

    ओळीत bool चालक!=(स्थिरांक LSR &other) स्थिरांक {
        परत !चालक==(other);
    }

    LSR &setHashCode();

खाजगी:
    शून्य deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
