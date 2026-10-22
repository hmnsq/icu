// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#که_نه_تعریف_شوی DICTBE_H
#تعریف_کړه DICTBE_H

#ضمن "unicode/utypes.h"
#ضمن "unicode/uniset.h"
#ضمن "unicode/utext.h"

#ضمن "brkeng.h"
#ضمن "hash.h"
#ضمن "mlbe.h"
#ضمن "uvectr32.h"

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
class DictionaryBreakEngine : عام LanguageBreakEngine {
 خاص:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سېټ    f_سېټ;

 عام:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  مجازي ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  مجازي UBool handles(UChar32 c, ثابت حرف* locale) ثابت override;

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
  مجازي int32_t پیدا_کړه_Breaks( UText *متن,
                              int32_t پیل_Pos,
                              int32_t پای_ټکی_Pos,
                              UVector32 &وموندل_Breaks,
                              UBool isPhraseBreaking,
                              UError_کوډ& حالت ) ثابت override;

 محمي:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  مجازي فراغ سېټ_Characters( ثابت Unicode_سېټ &سېټ );

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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
                                           int32_t range_پیل,
                                           int32_t range_پای_ټکی,
                                           UVector32 &وموندل_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کوډ& حالت) ثابت = 0;

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
class ThaiBreakEngine : عام DictionaryBreakEngine {
 خاص:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سېټ                f_پای_ټکی_کلمه_سېټ;
  Unicode_سېټ                f_Begin_کلمه_سېټ;
  Unicode_سېټ                f_Suffix_سېټ;
  Unicode_سېټ                f_Mark_سېټ;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازي ~ThaiBreakEngine();

 محمي:
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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
                                           int32_t range_پیل,
                                           int32_t range_پای_ټکی,
                                           UVector32 &وموندل_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کوډ& حالت) ثابت override;

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
class LaoBreakEngine : عام DictionaryBreakEngine {
 خاص:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سېټ                f_پای_ټکی_کلمه_سېټ;
  Unicode_سېټ                f_Begin_کلمه_سېټ;
  Unicode_سېټ                f_Mark_سېټ;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازي ~LaoBreakEngine();

 محمي:
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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
                                           int32_t range_پیل,
                                           int32_t range_پای_ټکی,
                                           UVector32 &وموندل_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کوډ& حالت) ثابت override;

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
class BurmeseBreakEngine : عام DictionaryBreakEngine {
 خاص:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سېټ                f_پای_ټکی_کلمه_سېټ;
  Unicode_سېټ                f_Begin_کلمه_سېټ;
  Unicode_سېټ                f_Mark_سېټ;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازي ~BurmeseBreakEngine();

 محمي:
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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
                                           int32_t range_پیل,
                                           int32_t range_پای_ټکی,
                                           UVector32 &وموندل_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کوډ& حالت) ثابت override;

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
class KhmerBreakEngine : عام DictionaryBreakEngine {
 خاص:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سېټ                f_پای_ټکی_کلمه_سېټ;
  Unicode_سېټ                f_Begin_کلمه_سېټ;
  Unicode_سېټ                f_Mark_سېټ;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_کوډ &حالت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازي ~KhmerBreakEngine();

 محمي:
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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
                                           int32_t range_پیل,
                                           int32_t range_پای_ټکی,
                                           UVector32 &وموندل_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کوډ& حالت) ثابت override;

};

#إذا !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
تعداد LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : عام DictionaryBreakEngine {
 محمي:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_سېټ                f_Hangul_کلمه_سېټ;
  Unicode_سېټ                f_Digit_Or_Open_Punctuation_Or_Alphabet_سېټ;
  Unicode_سېټ                f_Close_Punctuation_سېټ;

  DictionaryMatcher        *fDictionary;
  ثابت Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 خاص:
  // Load Japanese extensions.
  فراغ بارول_Japanese_Extensions(UError_کوډ& error);
  // Load Japanese Hiragana.
  فراغ بارول_Hiragana(UError_کوډ& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  فراغ initJapanesePhraseParameter(UError_کوډ& error);

  Hashtable f_Skip_سېټ;

 عام:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_کوډ &حالت);

    /**
     * <p>Virtual destructor.</p>
     */
  مجازي ~CjkBreakEngine();

 محمي:
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
  مجازي int32_t divide_پورته_Dictionary_Range( UText *متن,
          int32_t range_پیل,
          int32_t range_پای_ټکی,
          UVector32 &وموندل_Breaks,
          UBool isPhraseBreaking,
          UError_کوډ& حالت) ثابت override;

};

#د_که_پای

U_NAMESPACE_پای_ټکی

    /* DICTBE_H */
#د_که_پای
