// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#अगर_अघोषित __LSR_H__
#घोषणा __LSR_H__

#समावेश "unicode/stringpiece.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/uobject.h"
#समावेश "cstring.h"

U_NAMESPACE_BEGIN

काष्ठा LSR final : खुला UMemory {
    अटल constexpr int32_t REGION_सूचकांक_LIMIT = 1001 + 26 * 26;

    अटल constexpr int32_t EXPLICIT_LSR = 7;
    अटल constexpr int32_t EXPLICIT_LANGUAGE = 4;
    अटल constexpr int32_t EXPLICIT_SCRIPT = 2;
    अटल constexpr int32_t EXPLICIT_REGION = 1;
    अटल constexpr int32_t IMPLICIT_LSR = 0;
    अटल constexpr int32_t DONT_CARE_FLAGS = 0;

    स्थिर अक्षर *language;
    स्थिर अक्षर *script;
    स्थिर अक्षर *region;
    अक्षर *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_सूचकांक = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(स्थिर अक्षर *lang, स्थिर अक्षर *scr, स्थिर अक्षर *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_सूचकांक(सूचकांक_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(अक्षर prefix, स्थिर अक्षर *lang, स्थिर अक्षर *scr, स्थिर अक्षर *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(स्थिर LSR &other) = delete;
    अंतरभूत ~LSR() {
        // Pure inline code for almost all instances.
        अगर (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &चालक=(LSR &&other) noexcept;
    LSR &चालक=(स्थिर LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    अटल int32_t सूचकांक_For_Region(स्थिर अक्षर *region);

    UBool isEquivalentTo(स्थिर LSR &other) स्थिर;
    bool चालक==(स्थिर LSR &other) स्थिर;

    अंतरभूत bool चालक!=(स्थिर LSR &other) स्थिर {
        वापस !चालक==(other);
    }

    LSR &setHashCode();

निजी:
    व्योम deleteOwned();
};

U_NAMESPACE_END

#पूर्ण_अगर  // __LSR_H__
