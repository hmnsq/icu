// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#ifndef DICTBE_H
#define DICTBE_H

#شامل "unicode/utypes.h"
#شامل "unicode/uniset.h"
#شامل "unicode/utext.h"

#شامل "brkeng.h"
#شامل "hash.h"
#شامل "mlbe.h"
#شامل "uvectr32.h"

U_NAMESPACE_BEGIN

class DictionaryMatcher;
class MlBreakEngine;
class Normalizer2;

/*******************************************************************
 * DictionaryBreakEngine
 */

/**
 * <p>DictionaryBreakEngine is a kind of LanguageBreakEngine that uses a
 * dictionary to determine language-specific breaks.</p>
 *
 * <p>After it is constructed a DictionaryBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class DictionaryBreakEngine : عمومی LanguageBreakEngine {
 خصوصی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet    fSet;

 عمومی:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  مجازی UBool handles(UChar32 c, ثابت نویسه* locale) ثابت override;

  /**
   * <p>Find any breaks within a run in the supplied text.</p>
   *
   * @param text A UText representing the text. The iterator is left at
   * the end of the run of characters which the engine is capable of handling
   * that starts from the first character in the range.
   * @param startPos The start of the run within the supplied text.
   * @param endPos The end of the run within the supplied text.
   * @param foundBreaks vector of int32_t to receive the break positions
   * @param status Information on any errors encountered.
   * @return The number of breaks found.
   */
  مجازی int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t endPos,
                              UVector32 &foundBreaks,
                              UBool isPhraseBreaking,
                              UErrorCode& status ) ثابت override;

 محافظتشده:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  مجازی تهی setCharacters( ثابت UnicodeSet &set );

 /**
  * <p>Divide up a range of known dictionary characters handled by this break engine.</p>
  *
  * @param text A UText representing the text
  * @param rangeStart The start of the range of dictionary characters
  * @param rangeEnd The end of the range of dictionary characters
  * @param foundBreaks Output of C array of int32_t break positions, or 0
  * @param status Information on any errors encountered.
  * @return The number of breaks found
  */
  مجازی int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) ثابت = 0;

};

/*******************************************************************
 * ThaiBreakEngine
 */

/**
 * <p>ThaiBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary and heuristics to determine Thai-specific breaks.</p>
 *
 * <p>After it is constructed a ThaiBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class ThaiBreakEngine : عمومی DictionaryBreakEngine {
 خصوصی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fSuffixSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 عمومی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~ThaiBreakEngine();

 محافظتشده:
 /**
  * <p>Divide up a range of known dictionary characters handled by this break engine.</p>
  *
  * @param text A UText representing the text
  * @param rangeStart The start of the range of dictionary characters
  * @param rangeEnd The end of the range of dictionary characters
  * @param foundBreaks Output of C array of int32_t break positions, or 0
  * @param status Information on any errors encountered.
  * @return The number of breaks found
  */
  مجازی int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) ثابت override;

};

/*******************************************************************
 * LaoBreakEngine
 */

/**
 * <p>LaoBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary and heuristics to determine Lao-specific breaks.</p>
 *
 * <p>After it is constructed a LaoBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class LaoBreakEngine : عمومی DictionaryBreakEngine {
 خصوصی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 عمومی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~LaoBreakEngine();

 محافظتشده:
 /**
  * <p>Divide up a range of known dictionary characters handled by this break engine.</p>
  *
  * @param text A UText representing the text
  * @param rangeStart The start of the range of dictionary characters
  * @param rangeEnd The end of the range of dictionary characters
  * @param foundBreaks Output of C array of int32_t break positions, or 0
  * @param status Information on any errors encountered.
  * @return The number of breaks found
  */
  مجازی int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) ثابت override;

};

/*******************************************************************
 * BurmeseBreakEngine
 */

/**
 * <p>BurmeseBreakEngine is a kind of DictionaryBreakEngine that uses a
 * DictionaryMatcher and heuristics to determine Burmese-specific breaks.</p>
 *
 * <p>After it is constructed a BurmeseBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class BurmeseBreakEngine : عمومی DictionaryBreakEngine {
 خصوصی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 عمومی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~BurmeseBreakEngine();

 محافظتشده:
 /**
  * <p>Divide up a range of known dictionary characters.</p>
  *
  * @param text A UText representing the text
  * @param rangeStart The start of the range of dictionary characters
  * @param rangeEnd The end of the range of dictionary characters
  * @param foundBreaks Output of C array of int32_t break positions, or 0
  * @param status Information on any errors encountered.
  * @return The number of breaks found
  */
  مجازی int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) ثابت override;

};

/*******************************************************************
 * KhmerBreakEngine
 */

/**
 * <p>KhmerBreakEngine is a kind of DictionaryBreakEngine that uses a
 * DictionaryMatcher and heuristics to determine Khmer-specific breaks.</p>
 *
 * <p>After it is constructed a KhmerBreakEngine may be shared between
 * threads without synchronization.</p>
 */
class KhmerBreakEngine : عمومی DictionaryBreakEngine {
 خصوصی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 عمومی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~KhmerBreakEngine();

 محافظتشده:
 /**
  * <p>Divide up a range of known dictionary characters.</p>
  *
  * @param text A UText representing the text
  * @param rangeStart The start of the range of dictionary characters
  * @param rangeEnd The end of the range of dictionary characters
  * @param foundBreaks Output of C array of int32_t break positions, or 0
  * @param status Information on any errors encountered.
  * @return The number of breaks found
  */
  مجازی int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) ثابت override;

};

#اگر !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
شمارش LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : عمومی DictionaryBreakEngine {
 محافظتشده:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  UnicodeSet                fHangulWordSet;
  UnicodeSet                fDigitOrOpenPunctuationOrAlphabetSet;
  UnicodeSet                fClosePunctuationSet;

  DictionaryMatcher        *fDictionary;
  ثابت Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 خصوصی:
  // Load Japanese extensions.
  تهی loadJapaneseExtensions(UErrorCode& error);
  // Load Japanese Hiragana.
  تهی loadHiragana(UErrorCode& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  تهی initJapanesePhraseParameter(UErrorCode& error);

  Hashtable fSkipSet;

 عمومی:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UErrorCode &status);

    /**
     * <p>Virtual destructor.</p>
     */
  مجازی ~CjkBreakEngine();

 محافظتشده:
    /**
     * <p>Divide up a range of known dictionary characters handled by this break engine.</p>
     *
     * @param text A UText representing the text
     * @param rangeStart The start of the range of dictionary characters
     * @param rangeEnd The end of the range of dictionary characters
     * @param foundBreaks Output of C array of int32_t break positions, or 0
     * @param status Information on any errors encountered.
     * @return The number of breaks found
     */
  مجازی int32_t divideUpDictionaryRange( UText *text,
          int32_t rangeStart,
          int32_t rangeEnd,
          UVector32 &foundBreaks,
          UBool isPhraseBreaking,
          UErrorCode& status) ثابت override;

};

#endif

U_NAMESPACE_END

    /* DICTBE_H */
#endif
