// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cstring.h"

U_NAMESPACE_BEGIN

கட்டமைப்பு LSR final : பொது UMemory {
    நிலையான constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    நிலையான constexpr int32_t EXPLICIT_LSR = 7;
    நிலையான constexpr int32_t EXPLICIT_LANGUAGE = 4;
    நிலையான constexpr int32_t EXPLICIT_SCRIPT = 2;
    நிலையான constexpr int32_t EXPLICIT_REGION = 1;
    நிலையான constexpr int32_t IMPLICIT_LSR = 0;
    நிலையான constexpr int32_t DONT_CARE_FLAGS = 0;

    மாறிலி எழுத்து *language;
    மாறிலி எழுத்து *script;
    மாறிலி எழுத்து *region;
    எழுத்து *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(மாறிலி எழுத்து *lang, மாறிலி எழுத்து *scr, மாறிலி எழுத்து *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(எழுத்து prefix, மாறிலி எழுத்து *lang, மாறிலி எழுத்து *scr, மாறிலி எழுத்து *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(மாறிலி LSR &other) = delete;
    உள்வரி ~LSR() {
        // Pure inline code for almost all instances.
        எனில் (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(மாறிலி LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    நிலையான int32_t indexForRegion(மாறிலி எழுத்து *region);

    UBool isEquivalentTo(மாறிலி LSR &other) மாறிலி;
    bool operator==(மாறிலி LSR &other) மாறிலி;

    உள்வரி bool operator!=(மாறிலி LSR &other) மாறிலி {
        திரும்பு !operator==(other);
    }

    LSR &setHashCode();

தனிப்பட்ட:
    வெறுமை deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
