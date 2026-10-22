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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/uniset.h"
#અન્તર્ભાવ "unicode/utext.h"

#અન્તર્ભાવ "brkeng.h"
#અન્તર્ભાવ "hash.h"
#અન્તર્ભાવ "mlbe.h"
#અન્તર્ભાવ "uvectr32.h"

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
class DictionaryBreakEngine : જાહેર LanguageBreakEngine {
 ખાનગી:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet    fSet;

 જાહેર:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  આભાસી ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  આભાસી UBool handles(UChar32 c, અચલ અક્ષર* locale) અચલ override;

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
  આભાસી int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t endPos,
                              UVector32 &foundBreaks,
                              UBool isPhraseBreaking,
                              UErrorCode& status ) અચલ override;

 સુરક્ષિત:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  આભાસી શૂન્ય setCharacters( અચલ UnicodeSet &set );

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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) અચલ = 0;

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
class ThaiBreakEngine : જાહેર DictionaryBreakEngine {
 ખાનગી:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fSuffixSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 જાહેર:

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
  આભાસી ~ThaiBreakEngine();

 સુરક્ષિત:
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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) અચલ override;

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
class LaoBreakEngine : જાહેર DictionaryBreakEngine {
 ખાનગી:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 જાહેર:

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
  આભાસી ~LaoBreakEngine();

 સુરક્ષિત:
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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) અચલ override;

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
class BurmeseBreakEngine : જાહેર DictionaryBreakEngine {
 ખાનગી:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 જાહેર:

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
  આભાસી ~BurmeseBreakEngine();

 સુરક્ષિત:
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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) અચલ override;

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
class KhmerBreakEngine : જાહેર DictionaryBreakEngine {
 ખાનગી:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  UnicodeSet                fEndWordSet;
  UnicodeSet                fBeginWordSet;
  UnicodeSet                fMarkSet;
  DictionaryMatcher  *fDictionary;

 જાહેર:

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
  આભાસી ~KhmerBreakEngine();

 સુરક્ષિત:
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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
                                           int32_t rangeStart,
                                           int32_t rangeEnd,
                                           UVector32 &foundBreaks,
                                           UBool isPhraseBreaking,
                                           UErrorCode& status) અચલ override;

};

#જો !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
ગણના LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : જાહેર DictionaryBreakEngine {
 સુરક્ષિત:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  UnicodeSet                fHangulWordSet;
  UnicodeSet                fDigitOrOpenPunctuationOrAlphabetSet;
  UnicodeSet                fClosePunctuationSet;

  DictionaryMatcher        *fDictionary;
  અચલ Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 ખાનગી:
  // Load Japanese extensions.
  શૂન્ય loadJapaneseExtensions(UErrorCode& error);
  // Load Japanese Hiragana.
  શૂન્ય loadHiragana(UErrorCode& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  શૂન્ય initJapanesePhraseParameter(UErrorCode& error);

  Hashtable fSkipSet;

 જાહેર:

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
  આભાસી ~CjkBreakEngine();

 સુરક્ષિત:
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
  આભાસી int32_t divideUpDictionaryRange( UText *text,
          int32_t rangeStart,
          int32_t rangeEnd,
          UVector32 &foundBreaks,
          UBool isPhraseBreaking,
          UErrorCode& status) અચલ override;

};

#endif

U_NAMESPACE_END

    /* DICTBE_H */
#endif
