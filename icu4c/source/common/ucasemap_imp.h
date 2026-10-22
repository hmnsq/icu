// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucasemap_imp.h
// created: 2017feb08 Markus W. Scherer

#ifndef __UCASEMAP_IMP_H__
#define __UCASEMAP_IMP_H__

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ucasemap.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/unistr.h"  // for UStringCaseMapper
#অন্তর্ভাব "ucase.h"

/**
 * Bit mask for the titlecasing iterator options bit field.
 * Currently only 3 out of 8 values are used:
 * 0 (words), U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
 * See stringoptions.h.
 * @internal
 */
#define U_TITLECASE_ITERATOR_MASK 0xe0

/**
 * Bit mask for the titlecasing index adjustment options bit set.
 * Currently two bits are defined:
 * U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED.
 * See stringoptions.h.
 * @internal
 */
#define U_TITLECASE_ADJUSTMENT_MASK 0x600

/**
 * Internal API, used by u_strcasecmp() etc.
 * Compare strings case-insensitively,
 * in code point order or code unit order.
 */
U_CFUNC int32_t
u_strcmpFold(ধ্রুবক UChar *s1, int32_t length1,
             ধ্রুবক UChar *s2, int32_t length2,
             uint32_t options,
             UErrorCode *pErrorCode);

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
U_CAPI শূন্য
u_caseInsensitivePrefixMatch(ধ্রুবক UChar *s1, int32_t length1,
                             ধ্রুবক UChar *s2, int32_t length2,
                             uint32_t options,
                             int32_t *matchLen1, int32_t *matchLen2,
                             UErrorCode *pErrorCode);

#ifdef __cplusplus

U_NAMESPACE_BEGIN

class BreakIterator;        // unicode/brkiter.h
class ByteSink;
class Locale;               // unicode/locid.h

/** Returns true if the options are valid. Otherwise false, and sets an error. */
অন্তঃরেখ UBool ustrcase_checkTitleAdjustmentOptions(uint32_t options, UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত false; }
    যদি ((options & U_TITLECASE_ADJUSTMENT_MASK) == U_TITLECASE_ADJUSTMENT_MASK) {
        // Both options together.
        errorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত false;
    }
    ফেরত true;
}

অন্তঃরেখ UBool ustrcase_isLNS(UChar32 c) {
    // Letter, number, symbol,
    // or a private use code point because those are typically used as letters or numbers.
    // Consider modifier letters only if they are cased.
    ধ্রুবক uint32_t LNS = (U_GC_L_MASK|U_GC_N_MASK|U_GC_S_MASK|U_GC_CO_MASK) & ~U_GC_LM_MASK;
    পূর্ণসংখ্যা gc = u_charType(c);
    ফেরত (U_MASK(gc) & LNS) != 0 || (gc == U_MODIFIER_LETTER && ucase_getType(c) != UCASE_NONE);
}

#যদি !UCONFIG_NO_BREAK_ITERATION

/** Returns nullptr if error. Pass in either locale or locID, not both. */
U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ধ্রুবক Locale* locale,
                                              ধ্রুবক অক্ষর* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              LocalPointer<BreakIterator>& ownedIter,
                                              UErrorCode& errorCode);

#endif

/*
 * Internal string casing functions implementing
 * ustring.h/ustrcase.cpp and UnicodeString case mapping functions.
 */

কাঠামো UCaseMapImpl : সর্বজনীন icu::UMemory {
    /** Implements most of ucasemap_open(). */
    UCaseMapImpl(ধ্রুবক অক্ষর *localeID, uint32_t opts, UErrorCode *pErrorCode);
    ~UCaseMapImpl();

#যদি !UCONFIG_NO_BREAK_ITERATION
    icu::BreakIterator *iter;  /* We adopt the iterator, so we own it. */
#endif
    অক্ষর locale[32];
    int32_t caseLocale;
    uint32_t options;
};

U_NAMESPACE_END

#যদি UCONFIG_NO_BREAK_ITERATION
#   define UCASEMAP_BREAK_ITERATOR_PARAM
#   define UCASEMAP_BREAK_ITERATOR_UNUSED
#   define UCASEMAP_BREAK_ITERATOR
#   define UCASEMAP_BREAK_ITERATOR_NULL
#অন্যথা
#   define UCASEMAP_BREAK_ITERATOR_PARAM icu::BreakIterator *iter,
#   define UCASEMAP_BREAK_ITERATOR_UNUSED icu::BreakIterator *,
#   define UCASEMAP_BREAK_ITERATOR iter,
#   define UCASEMAP_BREAK_ITERATOR_NULL ফাঁকা,
#endif

U_CFUNC int32_t
ustrcase_getCaseLocale(ধ্রুবক অক্ষর *locale);

// TODO: swap src / dest if approved for new public api
/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_internalToLower(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode);

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_internalToUpper(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode);

#যদি !UCONFIG_NO_BREAK_ITERATION

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_internalToTitle(int32_t caseLocale, uint32_t options,
                         icu::BreakIterator *iter,
                         char16_t *dest, int32_t destCapacity,
                         ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                         icu::Edits *edits,
                         UErrorCode &errorCode);

#endif

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_internalFold(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                      char16_t *dest, int32_t destCapacity,
                      ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                      icu::Edits *edits,
                      UErrorCode &errorCode);

/**
 * Common string case mapping implementation for ucasemap_toXyz() and UnicodeString::toXyz().
 * Implements argument checking.
 */
U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t destCapacity,
             ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UErrorCode &errorCode);

/**
 * Common string case mapping implementation for old-fashioned u_strToXyz() functions
 * that allow the source string to overlap the destination buffer.
 * Implements argument checking and internally works with an intermediate buffer if necessary.
 */
U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t destCapacity,
                        ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                        UStringCaseMapper *stringCaseMapper,
                        UErrorCode &errorCode);

/**
 * UTF-8 string case mapping function type, used by ucasemap_mapUTF8().
 * UTF-8 version of UStringCaseMapper.
 * All error checking must be done.
 */
প্রকারলেখ শূন্য U_CALLCONV
UTF8CaseMapper(int32_t caseLocale, uint32_t options,
#যদি !UCONFIG_NO_BREAK_ITERATION
               icu::BreakIterator *iter,
#endif
               ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
               icu::ByteSink &sink, icu::Edits *edits,
               UErrorCode &errorCode);

#যদি !UCONFIG_NO_BREAK_ITERATION

/** Implements UTF8CaseMapper. */
U_CFUNC শূন্য U_CALLCONV
ucasemap_internalUTF8ToTitle(int32_t caseLocale, uint32_t options,
        icu::BreakIterator *iter,
        ধ্রুবক uint8_t *src, int32_t src_দৈর্ঘ্য,
        icu::ByteSink &sink, icu::Edits *edits,
        UErrorCode &errorCode);

#endif

শূন্য
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UErrorCode &errorCode);

/**
 * Implements argument checking and buffer handling
 * for UTF-8 string case mapping as a common function.
 */
int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 অক্ষর *dest, int32_t destCapacity,
                 ধ্রুবক অক্ষর *src, int32_t src_দৈর্ঘ্য,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UErrorCode &errorCode);

U_NAMESPACE_BEGIN
namespace GreekUpper {

// Data bits.
স্থির ধ্রুবক uint32_t UPPER_MASK = 0x3ff;
স্থির ধ্রুবক uint32_t HAS_VOWEL = 0x1000;
স্থির ধ্রুবক uint32_t HAS_YPOGEGRAMMENI = 0x2000;
স্থির ধ্রুবক uint32_t HAS_ACCENT = 0x4000;
স্থির ধ্রুবক uint32_t HAS_DIALYTIKA = 0x8000;
// Further bits during data building and processing, not stored in the data map.
স্থির ধ্রুবক uint32_t HAS_COMBINING_DIALYTIKA = 0x10000;
স্থির ধ্রুবক uint32_t HAS_OTHER_GREEK_DIACRITIC = 0x20000;

স্থির ধ্রুবক uint32_t HAS_VOWEL_AND_ACCENT = HAS_VOWEL | HAS_ACCENT;
স্থির ধ্রুবক uint32_t HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA =
        HAS_VOWEL_AND_ACCENT | HAS_DIALYTIKA;
স্থির ধ্রুবক uint32_t HAS_EITHER_DIALYTIKA = HAS_DIALYTIKA | HAS_COMBINING_DIALYTIKA;

// State bits.
স্থির ধ্রুবক uint32_t AFTER_CASED = 1;
স্থির ধ্রুবক uint32_t AFTER_VOWEL_WITH_COMBINING_ACCENT = 2;
স্থির ধ্রুবক uint32_t AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT = 4;

uint32_t get_Letter_তথ্য(UChar32 c);

/**
 * Returns a non-zero value for each of the Greek combining diacritics
 * listed in The Unicode Standard, version 8, chapter 7.2 Greek,
 * plus some perispomeni look-alikes.
 */
uint32_t get_Diacritic_তথ্য(UChar32 c);

}  // namespace GreekUpper
U_NAMESPACE_END

#endif  // __cplusplus

#endif  // __UCASEMAP_IMP_H__
