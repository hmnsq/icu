// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DICTBE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICTBE_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uniset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "brkeng.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mlbe.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvectr32.h"

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
class DictionaryBreakEngine : ਜਨਤਕ LanguageBreakEngine {
 ਨਿਜੀ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ਸੈੱਟ    f_ਸੈੱਟ;

 ਜਨਤਕ:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  ਆਭਾਸੀ UBool handles(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) ਅਚਲ override;

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
  ਆਭਾਸੀ int32_t ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                              int32_t ਸ਼ੁਰੂ_Pos,
                              int32_t ਸਿਰਾ_Pos,
                              UVector32 &ਮਿਲਿਆ_Breaks,
                              UBool isPhraseBreaking,
                              UError_ਕੋਡ& ਸਥਿਤੀ ) ਅਚਲ override;

 ਸੁਰੱਖਿਅਤ:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  ਆਭਾਸੀ ਖਾਲੀ ਸੈੱਟ_Characters( ਅਚਲ Unicode_ਸੈੱਟ &ਸੈੱਟ );

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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                           int32_t range_ਸ਼ੁਰੂ,
                                           int32_t range_ਸਿਰਾ,
                                           UVector32 &ਮਿਲਿਆ_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ = 0;

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
class ThaiBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
 ਨਿਜੀ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ਸੈੱਟ                f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Begin_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Suffix_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Mark_ਸੈੱਟ;
  DictionaryMatcher  *fDictionary;

 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~ThaiBreakEngine();

 ਸੁਰੱਖਿਅਤ:
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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                           int32_t range_ਸ਼ੁਰੂ,
                                           int32_t range_ਸਿਰਾ,
                                           UVector32 &ਮਿਲਿਆ_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

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
class LaoBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
 ਨਿਜੀ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ਸੈੱਟ                f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Begin_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Mark_ਸੈੱਟ;
  DictionaryMatcher  *fDictionary;

 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~LaoBreakEngine();

 ਸੁਰੱਖਿਅਤ:
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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                           int32_t range_ਸ਼ੁਰੂ,
                                           int32_t range_ਸਿਰਾ,
                                           UVector32 &ਮਿਲਿਆ_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

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
class BurmeseBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
 ਨਿਜੀ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ਸੈੱਟ                f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Begin_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Mark_ਸੈੱਟ;
  DictionaryMatcher  *fDictionary;

 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~BurmeseBreakEngine();

 ਸੁਰੱਖਿਅਤ:
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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                           int32_t range_ਸ਼ੁਰੂ,
                                           int32_t range_ਸਿਰਾ,
                                           UVector32 &ਮਿਲਿਆ_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

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
class KhmerBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
 ਨਿਜੀ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ਸੈੱਟ                f_ਸਿਰਾ_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Begin_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Mark_ਸੈੱਟ;
  DictionaryMatcher  *fDictionary;

 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~KhmerBreakEngine();

 ਸੁਰੱਖਿਅਤ:
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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
                                           int32_t range_ਸ਼ੁਰੂ,
                                           int32_t range_ਸਿਰਾ,
                                           UVector32 &ਮਿਲਿਆ_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

};

#ਜੇ !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
ਗਿਣਤੀ LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : ਜਨਤਕ DictionaryBreakEngine {
 ਸੁਰੱਖਿਅਤ:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_ਸੈੱਟ                f_Hangul_ਸ਼ਬਦ_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Digit_Or_Open_Punctuation_Or_Alphabet_ਸੈੱਟ;
  Unicode_ਸੈੱਟ                f_Close_Punctuation_ਸੈੱਟ;

  DictionaryMatcher        *fDictionary;
  ਅਚਲ Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 ਨਿਜੀ:
  // Load Japanese extensions.
  ਖਾਲੀ ਲੱਦੋ_Japanese_Extensions(UError_ਕੋਡ& error);
  // Load Japanese Hiragana.
  ਖਾਲੀ ਲੱਦੋ_Hiragana(UError_ਕੋਡ& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  ਖਾਲੀ initJapanesePhraseParameter(UError_ਕੋਡ& error);

  Hashtable f_Skip_ਸੈੱਟ;

 ਜਨਤਕ:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_ਕੋਡ &ਸਥਿਤੀ);

    /**
     * <p>Virtual destructor.</p>
     */
  ਆਭਾਸੀ ~CjkBreakEngine();

 ਸੁਰੱਖਿਅਤ:
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
  ਆਭਾਸੀ int32_t divide_ਉੱਪਰ_Dictionary_Range( UText *ਪਾਠ,
          int32_t range_ਸ਼ੁਰੂ,
          int32_t range_ਸਿਰਾ,
          UVector32 &ਮਿਲਿਆ_Breaks,
          UBool isPhraseBreaking,
          UError_ਕੋਡ& ਸਥਿਤੀ) ਅਚਲ override;

};

#ਜੇ_ਅੰਤ

U_NAMESPACE_ਸਿਰਾ

    /* DICTBE_H */
#ਜੇ_ਅੰਤ
