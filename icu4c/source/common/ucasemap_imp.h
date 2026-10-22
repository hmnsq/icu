// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucasemap_imp.h
// created: 2017feb08 Markus W. Scherer

#جيڪڏهن_وصف_نه_ٿيل __UCASEMAP_IMP_H__
#وصف_ڪريو __UCASEMAP_IMP_H__

#ضمن "unicode/utypes.h"
#ضمن "unicode/ucasemap.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/unistr.h"  // for UStringCaseMapper
#ضمن "ucase.h"

/**
 * Bit mask for the titlecasing iterator options bit field.
 * Currently only 3 out of 8 values are used:
 * 0 (words), U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
 * See stringoptions.h.
 * @internal
 */
#وصف_ڪريو U_TITLECASE_ITERATOR_نقاب 0xe0

/**
 * Bit mask for the titlecasing index adjustment options bit set.
 * Currently two bits are defined:
 * U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED.
 * See stringoptions.h.
 * @internal
 */
#وصف_ڪريو U_TITLECASE_ADJUSTMENT_نقاب 0x600

/**
 * Internal API, used by u_strcasecmp() etc.
 * Compare strings case-insensitively,
 * in code point order or code unit order.
 */
U_CFUNC int32_t
u_strcmpFold(ثابت UChar *s1, int32_t length1,
             ثابت UChar *s2, int32_t length2,
             uint32_t options,
             UError_ڪوڊ *p_Error_ڪوڊ);

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
U_CAPI فراغ
u_case_Insensitive_اڳياڙي_ميلاپ(ثابت UChar *s1, int32_t length1,
                             ثابت UChar *s2, int32_t length2,
                             uint32_t options,
                             int32_t *ميلاپ_Len1, int32_t *ميلاپ_Len2,
                             UError_ڪوڊ *p_Error_ڪوڊ);

#جيڪڏهن_وصف_ٿيل __cplusplus

U_NAMESPACE_BEGIN

class BreakIterator;        // unicode/brkiter.h
class ByteSink;
class Locale;               // unicode/locid.h

/** Returns true if the options are valid. Otherwise false, and sets an error. */
مضمن UBool ustrcase_checkTitleAdjustmentOptions(uint32_t options, UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع false; }
    إذا ((options & U_TITLECASE_ADJUSTMENT_نقاب) == U_TITLECASE_ADJUSTMENT_نقاب) {
        // Both options together.
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع false;
    }
    أرجع true;
}

مضمن UBool ustrcase_isLNS(UChar32 c) {
    // Letter, number, symbol,
    // or a private use code point because those are typically used as letters or numbers.
    // Consider modifier letters only if they are cased.
    ثابت uint32_t LNS = (U_GC_L_نقاب|U_GC_N_نقاب|U_GC_S_نقاب|U_GC_CO_نقاب) & ~U_GC_LM_نقاب;
    صحيح gc = u_charType(c);
    أرجع (U_نقاب(gc) & LNS) != 0 || (gc == U_MODIFIER_LETTER && ucase_getType(c) != UCASE_NONE);
}

#إذا !UCONFIG_NO_BREAK_ITERATION

/** Returns nullptr if error. Pass in either locale or locID, not both. */
U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(ثابت Locale* locale,
                                              ثابت حرف* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_اشارو<BreakIterator>& ownedIter,
                                              UError_ڪوڊ& error_ڪوڊ);

#جيڪڏهن_پورو

/*
 * Internal string casing functions implementing
 * ustring.h/ustrcase.cpp and UnicodeString case mapping functions.
 */

بنية UCaseMapImpl : عام icu::UMemory {
    /** Implements most of ucasemap_open(). */
    UCaseMapImpl(ثابت حرف *localeID, uint32_t opts, UError_ڪوڊ *p_Error_ڪوڊ);
    ~UCaseMapImpl();

#إذا !UCONFIG_NO_BREAK_ITERATION
    icu::BreakIterator *iter;  /* We adopt the iterator, so we own it. */
#جيڪڏهن_پورو
    حرف locale[32];
    int32_t caseLocale;
    uint32_t options;
};

U_NAMESPACE_پڇاڙي

#إذا UCONFIG_NO_BREAK_ITERATION
#   define UCASEMAP_BREAK_ITERATOR_PARAM
#   define UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل
#   define UCASEMAP_BREAK_ITERATOR
#   define UCASEMAP_BREAK_ITERATOR_NULL
#وإلا
#   define UCASEMAP_BREAK_ITERATOR_PARAM icu::BreakIterator *iter,
#   define UCASEMAP_BREAK_ITERATOR_اڻ_استعمال_ٿيل icu::BreakIterator *,
#   define UCASEMAP_BREAK_ITERATOR iter,
#   define UCASEMAP_BREAK_ITERATOR_NULL عدم,
#جيڪڏهن_پورو

U_CFUNC int32_t
ustrcase_getCaseLocale(ثابت حرف *locale);

// TODO: swap src / dest if approved for new public api
/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_هيٺاهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ);

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_مٿانهون(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ);

#إذا !UCONFIG_NO_BREAK_ITERATION

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_منزل_Title(int32_t caseLocale, uint32_t options,
                         icu::BreakIterator *iter,
                         char16_t *dest, int32_t dest_صلاحيت,
                         ثابت char16_t *src, int32_t src_ڊيگهائي,
                         icu::Edits *edits,
                         UError_ڪوڊ &error_ڪوڊ);

#جيڪڏهن_پورو

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_اندروني_Fold(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                      char16_t *dest, int32_t dest_صلاحيت,
                      ثابت char16_t *src, int32_t src_ڊيگهائي,
                      icu::Edits *edits,
                      UError_ڪوڊ &error_ڪوڊ);

/**
 * Common string case mapping implementation for ucasemap_toXyz() and UnicodeString::toXyz().
 * Implements argument checking.
 */
U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_صلاحيت,
             ثابت char16_t *src, int32_t src_ڊيگهائي,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_ڪوڊ &error_ڪوڊ);

/**
 * Common string case mapping implementation for old-fashioned u_strToXyz() functions
 * that allow the source string to overlap the destination buffer.
 * Implements argument checking and internally works with an intermediate buffer if necessary.
 */
U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_صلاحيت,
                        ثابت char16_t *src, int32_t src_ڊيگهائي,
                        UStringCaseMapper *stringCaseMapper,
                        UError_ڪوڊ &error_ڪوڊ);

/**
 * UTF-8 string case mapping function type, used by ucasemap_mapUTF8().
 * UTF-8 version of UStringCaseMapper.
 * All error checking must be done.
 */
تعريفنوع فراغ U_CALLCONV
UTF8CaseMapper(int32_t caseLocale, uint32_t options,
#إذا !UCONFIG_NO_BREAK_ITERATION
               icu::BreakIterator *iter,
#جيڪڏهن_پورو
               ثابت uint8_t *src, int32_t src_ڊيگهائي,
               icu::ByteSink &sink, icu::Edits *edits,
               UError_ڪوڊ &error_ڪوڊ);

#إذا !UCONFIG_NO_BREAK_ITERATION

/** Implements UTF8CaseMapper. */
U_CFUNC فراغ U_CALLCONV
ucasemap_اندروني_UTF8_منزل_Title(int32_t caseLocale, uint32_t options,
        icu::BreakIterator *iter,
        ثابت uint8_t *src, int32_t src_ڊيگهائي,
        icu::ByteSink &sink, icu::Edits *edits,
        UError_ڪوڊ &error_ڪوڊ);

#جيڪڏهن_پورو

فراغ
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 ثابت حرف *src, int32_t src_ڊيگهائي,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_ڪوڊ &error_ڪوڊ);

/**
 * Implements argument checking and buffer handling
 * for UTF-8 string case mapping as a common function.
 */
int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 حرف *dest, int32_t dest_صلاحيت,
                 ثابت حرف *src, int32_t src_ڊيگهائي,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_ڪوڊ &error_ڪوڊ);

U_NAMESPACE_BEGIN
namespace Greek_مٿانهون {

// Data bits.
ساكن ثابت uint32_t مٿانهون_نقاب = 0x3ff;
ساكن ثابت uint32_t HAS_VOWEL = 0x1000;
ساكن ثابت uint32_t HAS_YPOGEGRAMMENI = 0x2000;
ساكن ثابت uint32_t HAS_ACCENT = 0x4000;
ساكن ثابت uint32_t HAS_DIALYTIKA = 0x8000;
// Further bits during data building and processing, not stored in the data map.
ساكن ثابت uint32_t HAS_COMBINING_DIALYTIKA = 0x10000;
ساكن ثابت uint32_t HAS_OTHER_GREEK_DIACRITIC = 0x20000;

ساكن ثابت uint32_t HAS_VOWEL_AND_ACCENT = HAS_VOWEL | HAS_ACCENT;
ساكن ثابت uint32_t HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA =
        HAS_VOWEL_AND_ACCENT | HAS_DIALYTIKA;
ساكن ثابت uint32_t HAS_EITHER_DIALYTIKA = HAS_DIALYTIKA | HAS_COMBINING_DIALYTIKA;

// State bits.
ساكن ثابت uint32_t AFTER_CASED = 1;
ساكن ثابت uint32_t AFTER_VOWEL_WITH_COMBINING_ACCENT = 2;
ساكن ثابت uint32_t AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT = 4;

uint32_t get_Letter_معلومات(UChar32 c);

/**
 * Returns a non-zero value for each of the Greek combining diacritics
 * listed in The Unicode Standard, version 8, chapter 7.2 Greek,
 * plus some perispomeni look-alikes.
 */
uint32_t get_Diacritic_معلومات(UChar32 c);

}  // namespace GreekUpper
U_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو  // __cplusplus

#جيڪڏهن_پورو  // __UCASEMAP_IMP_H__
