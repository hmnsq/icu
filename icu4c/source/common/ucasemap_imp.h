// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucasemap_imp.h
// created: 2017feb08 Markus W. Scherer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __UCASEMAP_IMP_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __UCASEMAP_IMP_H__

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucasemap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"  // for UStringCaseMapper
#ਅਨ੍ਤਰ੍ਭਾਵ "ucase.h"

/**
 * Bit mask for the titlecasing iterator options bit field.
 * Currently only 3 out of 8 values are used:
 * 0 (words), U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
 * See stringoptions.h.
 * @internal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_TITLECASE_ITERATOR_ਨਕਾਬ 0xe0

/**
 * Bit mask for the titlecasing index adjustment options bit set.
 * Currently two bits are defined:
 * U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED.
 * See stringoptions.h.
 * @internal
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_TITLECASE_ADJUSTMENT_ਨਕਾਬ 0x600

/**
 * Internal API, used by u_strcasecmp() etc.
 * Compare strings case-insensitively,
 * in code point order or code unit order.
 */
U_CFUNC int32_t
u_strcmpFold(ਅਚਲ UChar *s1, int32_t length1,
             ਅਚਲ UChar *s2, int32_t length2,
             uint32_t options,
             UError_ਕੋਡ *p_Error_ਕੋਡ);

/**
 * Internal API, used for detecting length of
 * shared prefix case-insensitively.
 * @param s1            input string 1
 * @param length1       length of string 1, or -1 (NULL terminated)
 * @param s2            input string 2
 * @param length2       length of string 2, or -1 (NULL terminated)
 * @param options       compare options
 * @param matchLen1     (output) length of partial prefix match in s1
 * @param matchLen2     (output) length of partial prefix match in s2
 * @param pErrorCode    receives error status
 */
U_CAPI ਖਾਲੀ
u_case_Insensitive_ਅਗੇਤਰ_ਮੇਲ(ਅਚਲ UChar *s1, int32_t length1,
                             ਅਚਲ UChar *s2, int32_t length2,
                             uint32_t options,
                             int32_t *ਮੇਲ_Len1, int32_t *ਮੇਲ_Len2,
                             UError_ਕੋਡ *p_Error_ਕੋਡ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus

U_NAMESPACE_BEGIN

class BreakIterator;        // unicode/brkiter.h
class ByteSink;
class Locale;               // unicode/locid.h

/** Returns true if the options are valid. Otherwise false, and sets an error. */
ਸਤਰਅੰਦਰ UBool ustrcase_checkTitleAdjustmentOptions(uint32_t options, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ false; }
    ਜੇ ((options & U_TITLECASE_ADJUSTMENT_ਨਕਾਬ) == U_TITLECASE_ADJUSTMENT_ਨਕਾਬ) {
        // Both options together.
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

ਸਤਰਅੰਦਰ UBool ustrcase_isLNS(UChar32 c) {
    // Letter, number, symbol,
    // or a private use code point because those are typically used as letters or numbers.
    // Consider modifier letters only if they are cased.
    ਅਚਲ uint32_t LNS = (U_GC_L_ਨਕਾਬ|U_GC_N_ਨਕਾਬ|U_GC_S_ਨਕਾਬ|U_GC_CO_ਨਕਾਬ) & ~U_GC_LM_ਨਕਾਬ;
    ਪੂਰਨ gc = u_charType(c);
    ਵਾਪਸ (U_ਨਕਾਬ(gc) & LNS) != 0 || (gc == U_MODIFIER_LETTER && ucase_getType(c) != UCASE_NONE);
}

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

/** Returns nullptr if error. Pass in either locale or locID, not both. */
U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ਅਚਲ Locale* locale,
                                              ਅਚਲ ਅੱਖਰ* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_ਸੂਚਕ<BreakIterator>& ownedIter,
                                              UError_ਕੋਡ& error_ਕੋਡ);

#ਜੇ_ਅੰਤ

/*
 * Internal string casing functions implementing
 * ustring.h/ustrcase.cpp and UnicodeString case mapping functions.
 */

ਬਣਤਰ UCaseMapImpl : ਜਨਤਕ icu::UMemory {
    /** Implements most of ucasemap_open(). */
    UCaseMapImpl(ਅਚਲ ਅੱਖਰ *localeID, uint32_t opts, UError_ਕੋਡ *p_Error_ਕੋਡ);
    ~UCaseMapImpl();

#ਜੇ !UCONFIG_NO_BREAK_ITERATION
    icu::BreakIterator *iter;  /* We adopt the iterator, so we own it. */
#ਜੇ_ਅੰਤ
    ਅੱਖਰ locale[32];
    int32_t caseLocale;
    uint32_t options;
};

U_NAMESPACE_ਸਿਰਾ

#ਜੇ UCONFIG_NO_BREAK_ITERATION
#   define UCASEMAP_BREAK_ITERATOR_PARAM
#   define UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ
#   define UCASEMAP_BREAK_ITERATOR
#   define UCASEMAP_BREAK_ITERATOR_NULL
#ਨਹੀਂਤਾਂ
#   define UCASEMAP_BREAK_ITERATOR_PARAM icu::BreakIterator *iter,
#   define UCASEMAP_BREAK_ITERATOR_ਅਣਵਰਤਿਆ icu::BreakIterator *,
#   define UCASEMAP_BREAK_ITERATOR iter,
#   define UCASEMAP_BREAK_ITERATOR_NULL ਨਿਰਾ,
#ਜੇ_ਅੰਤ

U_CFUNC int32_t
ustrcase_getCaseLocale(ਅਚਲ ਅੱਖਰ *locale);

// TODO: swap src / dest if approved for new public api
/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ);

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Title(int32_t caseLocale, uint32_t options,
                         icu::BreakIterator *iter,
                         char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                         ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                         icu::Edits *edits,
                         UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ_ਅੰਤ

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_ਅੰਦਰੂਨੀ_Fold(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                      char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                      ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                      icu::Edits *edits,
                      UError_ਕੋਡ &error_ਕੋਡ);

/**
 * Common string case mapping implementation for ucasemap_toXyz() and UnicodeString::toXyz().
 * Implements argument checking.
 */
U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
             ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_ਕੋਡ &error_ਕੋਡ);

/**
 * Common string case mapping implementation for old-fashioned u_strToXyz() functions
 * that allow the source string to overlap the destination buffer.
 * Implements argument checking and internally works with an intermediate buffer if necessary.
 */
U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                        ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                        UStringCaseMapper *stringCaseMapper,
                        UError_ਕੋਡ &error_ਕੋਡ);

/**
 * UTF-8 string case mapping function type, used by ucasemap_mapUTF8().
 * UTF-8 version of UStringCaseMapper.
 * All error checking must be done.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ U_CALLCONV
UTF8CaseMapper(int32_t caseLocale, uint32_t options,
#ਜੇ !UCONFIG_NO_BREAK_ITERATION
               icu::BreakIterator *iter,
#ਜੇ_ਅੰਤ
               ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
               icu::ByteSink &sink, icu::Edits *edits,
               UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ !UCONFIG_NO_BREAK_ITERATION

/** Implements UTF8CaseMapper. */
U_CFUNC ਖਾਲੀ U_CALLCONV
ucasemap_ਅੰਦਰੂਨੀ_UTF8_ਮੰਜ਼ਿਲ_Title(int32_t caseLocale, uint32_t options,
        icu::BreakIterator *iter,
        ਅਚਲ uint8_t *src, int32_t src_ਲੰਮਾਈ,
        icu::ByteSink &sink, icu::Edits *edits,
        UError_ਕੋਡ &error_ਕੋਡ);

#ਜੇ_ਅੰਤ

ਖਾਲੀ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_ਕੋਡ &error_ਕੋਡ);

/**
 * Implements argument checking and buffer handling
 * for UTF-8 string case mapping as a common function.
 */
int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                 ਅਚਲ ਅੱਖਰ *src, int32_t src_ਲੰਮਾਈ,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_ਕੋਡ &error_ਕੋਡ);

U_NAMESPACE_BEGIN
namespace Greek_ਉੱਤਲਾ {

// Data bits.
ਸਥਿਰ ਅਚਲ uint32_t ਉੱਤਲਾ_ਨਕਾਬ = 0x3ff;
ਸਥਿਰ ਅਚਲ uint32_t HAS_VOWEL = 0x1000;
ਸਥਿਰ ਅਚਲ uint32_t HAS_YPOGEGRAMMENI = 0x2000;
ਸਥਿਰ ਅਚਲ uint32_t HAS_ACCENT = 0x4000;
ਸਥਿਰ ਅਚਲ uint32_t HAS_DIALYTIKA = 0x8000;
// Further bits during data building and processing, not stored in the data map.
ਸਥਿਰ ਅਚਲ uint32_t HAS_COMBINING_DIALYTIKA = 0x10000;
ਸਥਿਰ ਅਚਲ uint32_t HAS_OTHER_GREEK_DIACRITIC = 0x20000;

ਸਥਿਰ ਅਚਲ uint32_t HAS_VOWEL_AND_ACCENT = HAS_VOWEL | HAS_ACCENT;
ਸਥਿਰ ਅਚਲ uint32_t HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA =
        HAS_VOWEL_AND_ACCENT | HAS_DIALYTIKA;
ਸਥਿਰ ਅਚਲ uint32_t HAS_EITHER_DIALYTIKA = HAS_DIALYTIKA | HAS_COMBINING_DIALYTIKA;

// State bits.
ਸਥਿਰ ਅਚਲ uint32_t AFTER_CASED = 1;
ਸਥਿਰ ਅਚਲ uint32_t AFTER_VOWEL_WITH_COMBINING_ACCENT = 2;
ਸਥਿਰ ਅਚਲ uint32_t AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT = 4;

uint32_t get_Letter_ਅੰਕੜੇ(UChar32 c);

/**
 * Returns a non-zero value for each of the Greek combining diacritics
 * listed in The Unicode Standard, version 8, chapter 7.2 Greek,
 * plus some perispomeni look-alikes.
 */
uint32_t get_Diacritic_ਅੰਕੜੇ(UChar32 c);

}  // namespace GreekUpper
U_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ  // __cplusplus

#ਜੇ_ਅੰਤ  // __UCASEMAP_IMP_H__
