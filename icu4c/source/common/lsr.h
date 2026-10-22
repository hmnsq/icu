// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// lsr.h
// created: 2019may08 Markus W. Scherer

#جے_بنا_تعریف __LSR_H__
#تعریف_کرو __LSR_H__

#ضمن "unicode/stringpiece.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/uobject.h"
#ضمن "cstring.h"

U_NAMESPACE_BEGIN

بنية LSR final : عام UMemory {
    ساكن constexpr int32_t REGION_اشاریہ_حد_بندی = 1001 + 26 * 26;

    ساكن constexpr int32_t EXPLICIT_LSR = 7;
    ساكن constexpr int32_t EXPLICIT_LANGUAGE = 4;
    ساكن constexpr int32_t EXPLICIT_SCRIPT = 2;
    ساكن constexpr int32_t EXPLICIT_REGION = 1;
    ساكن constexpr int32_t IMPLICIT_LSR = 0;
    ساكن constexpr int32_t DONT_CARE_FLAGS = 0;

    ثابت حرف *language;
    ثابت حرف *script;
    ثابت حرف *region;
    حرف *owned = nullptr;
    /** Index for region, 0 if ill-formed. @see indexForRegion */
    int32_t region_اشاریہ = 0;
    int32_t flags = 0;
    /** Only set for LSRs that will be used in a hash table. */
    int32_t hash_کوڈ = 0;

    LSR() : language("und"), script(""), region("") {}

    /** Constructor which aliases all subtag pointers. */
    LSR(ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f) :
            language(lang),  script(scr), region(r),
            region_اشاریہ(اشاریہ_For_Region(region)), flags(f) {}
    /**
     * Constructor which prepends the prefix to the language and script,
     * copies those into owned memory, and aliases the region.
     */
    LSR(حرف اگیتر, ثابت حرف *lang, ثابت حرف *scr, ثابت حرف *r, int32_t f,
        UError_کوڈ &error_کوڈ);
    LSR(StringPiece lang, StringPiece scr, StringPiece r, int32_t f,
        UError_کوڈ &error_کوڈ);
    LSR(LSR &&other) noexcept;
    LSR(ثابت LSR &other) = delete;
    مضمن ~LSR() {
        // Pure inline code for almost all instances.
        إذا (owned != nullptr) {
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
    ساكن int32_t اشاریہ_For_Region(ثابت حرف *region);

    UBool is_Equivalent_منزل(ثابت LSR &other) ثابت;
    bool عامل==(ثابت LSR &other) ثابت;

    مضمن bool عامل!=(ثابت LSR &other) ثابت {
        أرجع !عامل==(other);
    }

    LSR &سیٹ_Hash_کوڈ();

خاص:
    فراغ deleteOwned();
};

U_NAMESPACE_سرا

#جے_انت  // __LSR_H__
