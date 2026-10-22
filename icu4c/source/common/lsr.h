// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __LSR_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __LSR_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"

U_NAMESPACE_BEGIN

ਬਣਤਰ LSR final : ਜਨਤਕ UMemory {
    ਸਥਿਰ constexpr int32_t REGION_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ = 1001 + 26 * 26;

    ਸਥਿਰ constexpr int32_t EXPLICIT_LSR = 7;
    ਸਥਿਰ constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ਸਥਿਰ constexpr int32_t EXPLICIT_SCRIPT = 2;
    ਸਥਿਰ constexpr int32_t EXPLICIT_REGION = 1;
    ਸਥਿਰ constexpr int32_t IMPLICIT_LSR = 0;
    ਸਥਿਰ constexpr int32_t DONT_CARE_FLAGS = 0;

    ਅਚਲ ਅੱਖਰ *language;
    ਅਚਲ ਅੱਖਰ *script;
    ਅਚਲ ਅੱਖਰ *region;
    ਅੱਖਰ *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_ਸੂਚਕਾਂਕ = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hash_ਕੋਡ = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ਅਚਲ ਅੱਖਰ *lang, ਅਚਲ ਅੱਖਰ *scr, ਅਚਲ ਅੱਖਰ *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_ਸੂਚਕਾਂਕ(ਸੂਚਕਾਂਕ_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(ਅੱਖਰ ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *lang, ਅਚਲ ਅੱਖਰ *scr, ਅਚਲ ਅੱਖਰ *r, int32_t f,
        UError_ਕੋਡ &error_ਕੋਡ);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UError_ਕੋਡ &error_ਕੋਡ);
    LSR(LSR &&other) noexcept;
    LSR(ਅਚਲ LSR &other) = delete;
    ਸਤਰਅੰਦਰ ~LSR() {
        // Pure inline code for almost all instances.
        ਜੇ (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &ਸੰਚਾਲਕ=(LSR &&other) noexcept;
    LSR &ਸੰਚਾਲਕ=(ਅਚਲ LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ਸਥਿਰ int32_t ਸੂਚਕਾਂਕ_For_Region(ਅਚਲ ਅੱਖਰ *region);

    UBool is_Equivalent_ਮੰਜ਼ਿਲ(ਅਚਲ LSR &other) ਅਚਲ;
    bool ਸੰਚਾਲਕ==(ਅਚਲ LSR &other) ਅਚਲ;

    ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ LSR &other) ਅਚਲ {
        ਵਾਪਸ !ਸੰਚਾਲਕ==(other);
    }

    LSR &ਸੈੱਟ_Hash_ਕੋਡ();

ਨਿਜੀ:
    ਖਾਲੀ deleteOwned();
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __LSR_H__
