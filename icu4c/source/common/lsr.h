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

    ध्रुव वर्ण *language;
    ध्रुव वर्ण *script;
    ध्रुव वर्ण *region;
    वर्ण *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ध्रुव वर्ण *lang, ध्रुव वर्ण *scr, ध्रुव वर्ण *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(वर्ण prefix, ध्रुव वर्ण *lang, ध्रुव वर्ण *scr, ध्रुव वर्ण *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(ध्रुव LSR &other) = delete;
    अन्तःस्थ ~LSR() {
        // Pure inline code for almost all instances.
        यदि (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &चालक=(LSR &&other) noexcept;
    LSR &चालक=(ध्रुव LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    स्थिर int32_t indexForRegion(ध्रुव वर्ण *region);

    UBool isEquivalentTo(ध्रुव LSR &other) ध्रुव;
    bool चालक==(ध्रुव LSR &other) ध्रुव;

    अन्तःस्थ bool चालक!=(ध्रुव LSR &other) ध्रुव {
        प्रत्यागम !चालक==(other);
    }

    LSR &setHashCode();

गुप्त:
    शून्य deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
