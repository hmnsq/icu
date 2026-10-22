// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#اگر_غیر_تعریف_شدہ DICTBE_H
#تعریف_کرو DICTBE_H

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
class DictionaryBreakEngine : عوامی LanguageBreakEngine {
 نجی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سیٹ    f_سیٹ;

 عوامی:

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
  مجازی UBool handles(UChar32 c, ثابت حرف* locale) ثابت override;

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
  مجازی int32_t ڈھونڈو_Breaks( UText *متن,
                              int32_t آغاز_Pos,
                              int32_t انتہا_Pos,
                              UVector32 &ملا_Breaks,
                              UBool isPhraseBreaking,
                              UError_رمز& حالت_2 ) ثابت override;

 محفوظ:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  مجازی خالی سیٹ_Characters( ثابت Unicode_سیٹ &سیٹ );

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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
                                           int32_t range_آغاز,
                                           int32_t range_انتہا,
                                           UVector32 &ملا_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_رمز& حالت_2) ثابت = 0;

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
class ThaiBreakEngine : عوامی DictionaryBreakEngine {
 نجی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سیٹ                f_انتہا_لفظ_سیٹ;
  Unicode_سیٹ                f_Begin_لفظ_سیٹ;
  Unicode_سیٹ                f_Suffix_سیٹ;
  Unicode_سیٹ                f_Mark_سیٹ;
  DictionaryMatcher  *fDictionary;

 عوامی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~ThaiBreakEngine();

 محفوظ:
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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
                                           int32_t range_آغاز,
                                           int32_t range_انتہا,
                                           UVector32 &ملا_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_رمز& حالت_2) ثابت override;

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
class LaoBreakEngine : عوامی DictionaryBreakEngine {
 نجی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سیٹ                f_انتہا_لفظ_سیٹ;
  Unicode_سیٹ                f_Begin_لفظ_سیٹ;
  Unicode_سیٹ                f_Mark_سیٹ;
  DictionaryMatcher  *fDictionary;

 عوامی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~LaoBreakEngine();

 محفوظ:
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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
                                           int32_t range_آغاز,
                                           int32_t range_انتہا,
                                           UVector32 &ملا_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_رمز& حالت_2) ثابت override;

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
class BurmeseBreakEngine : عوامی DictionaryBreakEngine {
 نجی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سیٹ                f_انتہا_لفظ_سیٹ;
  Unicode_سیٹ                f_Begin_لفظ_سیٹ;
  Unicode_سیٹ                f_Mark_سیٹ;
  DictionaryMatcher  *fDictionary;

 عوامی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~BurmeseBreakEngine();

 محفوظ:
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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
                                           int32_t range_آغاز,
                                           int32_t range_انتہا,
                                           UVector32 &ملا_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_رمز& حالت_2) ثابت override;

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
class KhmerBreakEngine : عوامی DictionaryBreakEngine {
 نجی:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_سیٹ                f_انتہا_لفظ_سیٹ;
  Unicode_سیٹ                f_Begin_لفظ_سیٹ;
  Unicode_سیٹ                f_Mark_سیٹ;
  DictionaryMatcher  *fDictionary;

 عوامی:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_رمز &حالت_2);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~KhmerBreakEngine();

 محفوظ:
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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
                                           int32_t range_آغاز,
                                           int32_t range_انتہا,
                                           UVector32 &ملا_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_رمز& حالت_2) ثابت override;

};

#اگر !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
شمار LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : عوامی DictionaryBreakEngine {
 محفوظ:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_سیٹ                f_Hangul_لفظ_سیٹ;
  Unicode_سیٹ                f_Digit_Or_Open_Punctuation_Or_Alphabet_سیٹ;
  Unicode_سیٹ                f_Close_Punctuation_سیٹ;

  DictionaryMatcher        *fDictionary;
  ثابت Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 نجی:
  // Load Japanese extensions.
  خالی بھرو_Japanese_Extensions(UError_رمز& error);
  // Load Japanese Hiragana.
  خالی بھرو_Hiragana(UError_رمز& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  خالی initJapanesePhraseParameter(UError_رمز& error);

  Hashtable f_Skip_سیٹ;

 عوامی:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_رمز &حالت_2);

    /**
     * <p>Virtual destructor.</p>
     */
  مجازی ~CjkBreakEngine();

 محفوظ:
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
  مجازی int32_t divide_اوپر_Dictionary_Range( UText *متن,
          int32_t range_آغاز,
          int32_t range_انتہا,
          UVector32 &ملا_Breaks,
          UBool isPhraseBreaking,
          UError_رمز& حالت_2) ثابت override;

};

#اگر_ختم

U_NAMESPACE_انتہا

    /* DICTBE_H */
#اگر_ختم
