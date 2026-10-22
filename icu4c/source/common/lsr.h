// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ifndef __LSR_H__
#define __LSR_H__

#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cstring.h"

U_NAMESPACE_BEGIN

ഘടന LSR final : പൊതു UMemory {
    സ്ഥിതം constexpr int32_t REGION_INDEX_LIMIT = 1001 + 26 * 26;

    സ്ഥിതം constexpr int32_t EXPLICIT_LSR = 7;
    സ്ഥിതം constexpr int32_t EXPLICIT_LANGUAGE = 4;
    സ്ഥിതം constexpr int32_t EXPLICIT_SCRIPT = 2;
    സ്ഥിതം constexpr int32_t EXPLICIT_REGION = 1;
    സ്ഥിതം constexpr int32_t IMPLICIT_LSR = 0;
    സ്ഥിതം constexpr int32_t DONT_CARE_FLAGS = 0;

    സ്ഥിരം അക്ഷരം *language;
    സ്ഥിരം അക്ഷരം *script;
    സ്ഥിരം അക്ഷരം *region;
    അക്ഷരം *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t regionIndex = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hashCode = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(സ്ഥിരം അക്ഷരം *lang, സ്ഥിരം അക്ഷരം *scr, സ്ഥിരം അക്ഷരം *r, int32_t f) :
            language(lang),  script(scr), region(r),
            regionIndex(indexForRegion(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(അക്ഷരം prefix, സ്ഥിരം അക്ഷരം *lang, സ്ഥിരം അക്ഷരം *scr, സ്ഥിരം അക്ഷരം *r, int32_t f,
        UErrorCode &errorCode);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UErrorCode &errorCode);
    LSR(LSR &&other) noexcept;
    LSR(സ്ഥിരം LSR &other) = delete;
    വരിയിൽ ~LSR() {
        // Pure inline code for almost all instances.
        എങ്കിൽ (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &operator=(LSR &&other) noexcept;
    LSR &operator=(സ്ഥിരം LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    സ്ഥിതം int32_t indexForRegion(സ്ഥിരം അക്ഷരം *region);

    UBool isEquivalentTo(സ്ഥിരം LSR &other) സ്ഥിരം;
    bool operator==(സ്ഥിരം LSR &other) സ്ഥിരം;

    വരിയിൽ bool operator!=(സ്ഥിരം LSR &other) സ്ഥിരം {
        തിരികെ !operator==(other);
    }

    LSR &setHashCode();

സ്വകാര്യ:
    ശൂന്യം deleteOwned();
};

U_NAMESPACE_END

#endif  // __LSR_H__
