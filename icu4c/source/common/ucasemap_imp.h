// © 2017 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// ucasemap_imp.h
// created: 2017feb08 Markus W. Scherer

#నిర్వచితం_కాకపోతే __UCASEMAP_IMP_H__
#నిర్వచించు __UCASEMAP_IMP_H__

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucasemap.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/unistr.h"  // for UStringCaseMapper
#అన్తర్భావ "ucase.h"

/**
 * Bit mask for the titlecasing iterator options bit field.
 * Currently only 3 out of 8 values are used:
 * 0 (words), U_TITLECASE_WHOLE_STRING, U_TITLECASE_SENTENCES.
 * See stringoptions.h.
 * @internal
 */
#నిర్వచించు U_TITLECASE_ITERATOR_ముసుగు 0xe0

/**
 * Bit mask for the titlecasing index adjustment options bit set.
 * Currently two bits are defined:
 * U_TITLECASE_NO_BREAK_ADJUSTMENT, U_TITLECASE_ADJUST_TO_CASED.
 * See stringoptions.h.
 * @internal
 */
#నిర్వచించు U_TITLECASE_ADJUSTMENT_ముసుగు 0x600

/**
 * Internal API, used by u_strcasecmp() etc.
 * Compare strings case-insensitively,
 * in code point order or code unit order.
 */
U_CFUNC int32_t
u_strcmpFold(స్థిరరాశి UChar *s1, int32_t length1,
             స్థిరరాశి UChar *s2, int32_t length2,
             uint32_t options,
             UError_సంకేతం *p_Error_సంకేతం);

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
U_CAPI శూన్య
u_case_Insensitive_ఉపసర్గ_సరిపోలిక(స్థిరరాశి UChar *s1, int32_t length1,
                             స్థిరరాశి UChar *s2, int32_t length2,
                             uint32_t options,
                             int32_t *సరిపోలిక_Len1, int32_t *సరిపోలిక_Len2,
                             UError_సంకేతం *p_Error_సంకేతం);

#నిర్వచితమైతే __cplusplus

U_NAMESPACE_BEGIN

class BreakIterator;        // unicode/brkiter.h
class ByteSink;
class Locale;               // unicode/locid.h

/** Returns true if the options are valid. Otherwise false, and sets an error. */
అంతఃవరుస UBool ustrcase_checkTitleAdjustmentOptions(uint32_t options, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి false; }
    ఒకవేళ ((options & U_TITLECASE_ADJUSTMENT_ముసుగు) == U_TITLECASE_ADJUSTMENT_ముసుగు) {
        // Both options together.
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి false;
    }
    తిరిగి true;
}

అంతఃవరుస UBool ustrcase_isLNS(UChar32 c) {
    // Letter, number, symbol,
    // or a private use code point because those are typically used as letters or numbers.
    // Consider modifier letters only if they are cased.
    స్థిరరాశి uint32_t LNS = (U_GC_L_ముసుగు|U_GC_N_ముసుగు|U_GC_S_ముసుగు|U_GC_CO_ముసుగు) & ~U_GC_LM_ముసుగు;
    పూర్ణాంకం gc = u_charType(c);
    తిరిగి (U_ముసుగు(gc) & LNS) != 0 || (gc == U_MODIFIER_LETTER && ucase_getType(c) != UCASE_NONE);
}

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

/** Returns nullptr if error. Pass in either locale or locID, not both. */
U_CFUNC
BreakIterator* ustrcase_getTitleBreakIterator(స్థిరరాశి Locale* locale,
                                              స్థిరరాశి అక్షరం* locID,
                                              uint32_t options,
                                              BreakIterator* iter U_LIFETIME_BOUND,
                                              Local_సూచిక<BreakIterator>& ownedIter,
                                              UError_సంకేతం& error_సంకేతం);

#అయితే_ముగింపు

/*
 * Internal string casing functions implementing
 * ustring.h/ustrcase.cpp and UnicodeString case mapping functions.
 */

నిర్మాణం UCaseMapImpl : బహిరంగ icu::UMemory {
    /** Implements most of ucasemap_open(). */
    UCaseMapImpl(స్థిరరాశి అక్షరం *localeID, uint32_t opts, UError_సంకేతం *p_Error_సంకేతం);
    ~UCaseMapImpl();

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
    icu::BreakIterator *iter;  /* We adopt the iterator, so we own it. */
#అయితే_ముగింపు
    అక్షరం locale[32];
    int32_t caseLocale;
    uint32_t options;
};

U_NAMESPACE_చివర

#ఒకవేళ UCONFIG_NO_BREAK_ITERATION
#   define UCASEMAP_BREAK_ITERATOR_PARAM
#   define UCASEMAP_BREAK_ITERATOR_వాడని
#   define UCASEMAP_BREAK_ITERATOR
#   define UCASEMAP_BREAK_ITERATOR_NULL
#లేకపోతే
#   define UCASEMAP_BREAK_ITERATOR_PARAM icu::BreakIterator *iter,
#   define UCASEMAP_BREAK_ITERATOR_వాడని icu::BreakIterator *,
#   define UCASEMAP_BREAK_ITERATOR iter,
#   define UCASEMAP_BREAK_ITERATOR_NULL ఖాళీ,
#అయితే_ముగింపు

U_CFUNC int32_t
ustrcase_getCaseLocale(స్థిరరాశి అక్షరం *locale);

// TODO: swap src / dest if approved for new public api
/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_క్రింది(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం);

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_పై(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం);

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_గమ్యం_Title(int32_t caseLocale, uint32_t options,
                         icu::BreakIterator *iter,
                         char16_t *dest, int32_t dest_సామర్థ్యం,
                         స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                         icu::Edits *edits,
                         UError_సంకేతం &error_సంకేతం);

#అయితే_ముగింపు

/** Implements UStringCaseMapper. */
U_CFUNC int32_t U_CALLCONV
ustrcase_అంతర్గత_Fold(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                      char16_t *dest, int32_t dest_సామర్థ్యం,
                      స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                      icu::Edits *edits,
                      UError_సంకేతం &error_సంకేతం);

/**
 * Common string case mapping implementation for ucasemap_toXyz() and UnicodeString::toXyz().
 * Implements argument checking.
 */
U_CFUNC int32_t
ustrcase_map(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
             char16_t *dest, int32_t dest_సామర్థ్యం,
             స్థిరరాశి char16_t *src, int32_t src_నిడివి,
             UStringCaseMapper *stringCaseMapper,
             icu::Edits *edits,
             UError_సంకేతం &error_సంకేతం);

/**
 * Common string case mapping implementation for old-fashioned u_strToXyz() functions
 * that allow the source string to overlap the destination buffer.
 * Implements argument checking and internally works with an intermediate buffer if necessary.
 */
U_CFUNC int32_t
ustrcase_mapWithOverlap(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                        char16_t *dest, int32_t dest_సామర్థ్యం,
                        స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                        UStringCaseMapper *stringCaseMapper,
                        UError_సంకేతం &error_సంకేతం);

/**
 * UTF-8 string case mapping function type, used by ucasemap_mapUTF8().
 * UTF-8 version of UStringCaseMapper.
 * All error checking must be done.
 */
రకనిర్వచనం శూన్య U_CALLCONV
UTF8CaseMapper(int32_t caseLocale, uint32_t options,
#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION
               icu::BreakIterator *iter,
#అయితే_ముగింపు
               స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
               icu::ByteSink &sink, icu::Edits *edits,
               UError_సంకేతం &error_సంకేతం);

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

/** Implements UTF8CaseMapper. */
U_CFUNC శూన్య U_CALLCONV
ucasemap_అంతర్గత_UTF8_గమ్యం_Title(int32_t caseLocale, uint32_t options,
        icu::BreakIterator *iter,
        స్థిరరాశి uint8_t *src, int32_t src_నిడివి,
        icu::ByteSink &sink, icu::Edits *edits,
        UError_సంకేతం &error_సంకేతం);

#అయితే_ముగింపు

శూన్య
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::ByteSink &sink, icu::Edits *edits,
                 UError_సంకేతం &error_సంకేతం);

/**
 * Implements argument checking and buffer handling
 * for UTF-8 string case mapping as a common function.
 */
int32_t
ucasemap_mapUTF8(int32_t caseLocale, uint32_t options, UCASEMAP_BREAK_ITERATOR_PARAM
                 అక్షరం *dest, int32_t dest_సామర్థ్యం,
                 స్థిరరాశి అక్షరం *src, int32_t src_నిడివి,
                 UTF8CaseMapper *stringCaseMapper,
                 icu::Edits *edits,
                 UError_సంకేతం &error_సంకేతం);

U_NAMESPACE_BEGIN
namespace Greek_పై {

// Data bits.
స్థిర స్థిరరాశి uint32_t పై_ముసుగు = 0x3ff;
స్థిర స్థిరరాశి uint32_t HAS_VOWEL = 0x1000;
స్థిర స్థిరరాశి uint32_t HAS_YPOGEGRAMMENI = 0x2000;
స్థిర స్థిరరాశి uint32_t HAS_ACCENT = 0x4000;
స్థిర స్థిరరాశి uint32_t HAS_DIALYTIKA = 0x8000;
// Further bits during data building and processing, not stored in the data map.
స్థిర స్థిరరాశి uint32_t HAS_COMBINING_DIALYTIKA = 0x10000;
స్థిర స్థిరరాశి uint32_t HAS_OTHER_GREEK_DIACRITIC = 0x20000;

స్థిర స్థిరరాశి uint32_t HAS_VOWEL_AND_ACCENT = HAS_VOWEL | HAS_ACCENT;
స్థిర స్థిరరాశి uint32_t HAS_VOWEL_AND_ACCENT_AND_DIALYTIKA =
        HAS_VOWEL_AND_ACCENT | HAS_DIALYTIKA;
స్థిర స్థిరరాశి uint32_t HAS_EITHER_DIALYTIKA = HAS_DIALYTIKA | HAS_COMBINING_DIALYTIKA;

// State bits.
స్థిర స్థిరరాశి uint32_t AFTER_CASED = 1;
స్థిర స్థిరరాశి uint32_t AFTER_VOWEL_WITH_COMBINING_ACCENT = 2;
స్థిర స్థిరరాశి uint32_t AFTER_VOWEL_WITH_PRECOMPOSED_ACCENT = 4;

uint32_t get_Letter_దత్తాంశం(UChar32 c);

/**
 * Returns a non-zero value for each of the Greek combining diacritics
 * listed in The Unicode Standard, version 8, chapter 7.2 Greek,
 * plus some perispomeni look-alikes.
 */
uint32_t get_Diacritic_దత్తాంశం(UChar32 c);

}  // namespace GreekUpper
U_NAMESPACE_చివర

#అయితే_ముగింపు  // __cplusplus

#అయితే_ముగింపు  // __UCASEMAP_IMP_H__
