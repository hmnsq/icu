// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#اگر_غیر_تعریف_شدہ __LSR_H__
#تعریف_کرو __LSR_H__

#شامل "unicode/stringpiece.h"
#شامل "unicode/utypes.h"
#شامل "unicode/uobject.h"
#شامل "cstring.h"

U_NAMESPACE_BEGIN

ساخت LSR final : عوامی UMemory {
    ساکن constexpr int32_t REGION_اشاریہ_حد_بندی = 1001 + 26 * 26;

    ساکن constexpr int32_t EXPLICIT_LSR = 7;
    ساکن constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ساکن constexpr int32_t EXPLICIT_SCRIPT = 2;
    ساکن constexpr int32_t EXPLICIT_REGION = 1;
    ساکن constexpr int32_t IMPLICIT_LSR = 0;
    ساکن constexpr int32_t DONT_CARE_FLAGS = 0;

    ثابت حرف *language;
    ثابت حرف *script;
    ثابت حرف *region;
    حرف *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_اشاریہ = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hash_رمز = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_اشاریہ(اشاریہ_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(حرف سابقہ, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
        UError_رمز &error_رمز);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UError_رمز &error_رمز);
    LSR(LSR &&other) noexcept;
    LSR(ثابت LSR &other) = delete;
    اندرونی ~LSR() {
        // Pure inline code for almost all instances.
        اگر (owned != nullptr) {
            deleteOwned();
        }
    }

    LSR &عامل=(LSR &&other) noexcept;
    LSR &عامل=(ثابت LSR &other) = delete;

    /**
     * Returns a positive index (>0) for a well-formed region code.
     * Do not rely on a particular region->index mapping; it may change.
     * Returns 0 for ill-formed strings.
     */
    ساکن int32_t اشاریہ_For_Region(ثابت حرف *region);

    UBool is_Equivalent_مقصد(ثابت LSR &other) ثابت;
    bool عامل==(ثابت LSR &other) ثابت;

    اندرونی bool عامل!=(ثابت LSR &other) ثابت {
        واپس !عامل==(other);
    }

    LSR &سیٹ_Hash_رمز();

نجی:
    خالی deleteOwned();
};

U_NAMESPACE_انتہا

#اگر_ختم  // __LSR_H__
