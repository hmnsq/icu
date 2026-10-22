// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#اگر_تعریف_نشده DICTBE_H
#تعریف_کن DICTBE_H

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

  Unicode_مجموعه    f_مجموعه;

 عام:

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
  مجازی int32_t پیدا_کن_Breaks( UText *متن,
                              int32_t شروع_Pos,
                              int32_t انتها_Pos,
                              UVector32 &یافت_Breaks,
                              UBool isPhraseBreaking,
                              UError_کد& وضعیت ) ثابت override;

 محمي:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  مجازی فراغ مجموعه_Characters( ثابت Unicode_مجموعه &مجموعه );

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
                                           int32_t range_شروع,
                                           int32_t range_انتها,
                                           UVector32 &یافت_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کد& وضعیت) ثابت = 0;

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

  Unicode_مجموعه                f_انتها_کلمه_مجموعه;
  Unicode_مجموعه                f_Begin_کلمه_مجموعه;
  Unicode_مجموعه                f_Suffix_مجموعه;
  Unicode_مجموعه                f_Mark_مجموعه;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~ThaiBreakEngine();

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
                                           int32_t range_شروع,
                                           int32_t range_انتها,
                                           UVector32 &یافت_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کد& وضعیت) ثابت override;

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

  Unicode_مجموعه                f_انتها_کلمه_مجموعه;
  Unicode_مجموعه                f_Begin_کلمه_مجموعه;
  Unicode_مجموعه                f_Mark_مجموعه;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~LaoBreakEngine();

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
                                           int32_t range_شروع,
                                           int32_t range_انتها,
                                           UVector32 &یافت_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کد& وضعیت) ثابت override;

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

  Unicode_مجموعه                f_انتها_کلمه_مجموعه;
  Unicode_مجموعه                f_Begin_کلمه_مجموعه;
  Unicode_مجموعه                f_Mark_مجموعه;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~BurmeseBreakEngine();

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
                                           int32_t range_شروع,
                                           int32_t range_انتها,
                                           UVector32 &یافت_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کد& وضعیت) ثابت override;

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

  Unicode_مجموعه                f_انتها_کلمه_مجموعه;
  Unicode_مجموعه                f_Begin_کلمه_مجموعه;
  Unicode_مجموعه                f_Mark_مجموعه;
  DictionaryMatcher  *fDictionary;

 عام:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_کد &وضعیت);

  /**
   * <p>Virtual destructor.</p>
   */
  مجازی ~KhmerBreakEngine();

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
                                           int32_t range_شروع,
                                           int32_t range_انتها,
                                           UVector32 &یافت_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_کد& وضعیت) ثابت override;

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
  Unicode_مجموعه                f_Hangul_کلمه_مجموعه;
  Unicode_مجموعه                f_Digit_Or_Open_Punctuation_Or_Alphabet_مجموعه;
  Unicode_مجموعه                f_Close_Punctuation_مجموعه;

  DictionaryMatcher        *fDictionary;
  ثابت Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 خاص:
  // Load Japanese extensions.
  فراغ بارگیری_Japanese_Extensions(UError_کد& error);
  // Load Japanese Hiragana.
  فراغ بارگیری_Hiragana(UError_کد& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  فراغ initJapanesePhraseParameter(UError_کد& error);

  Hashtable f_Skip_مجموعه;

 عام:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_کد &وضعیت);

    /**
     * <p>Virtual destructor.</p>
     */
  مجازی ~CjkBreakEngine();

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
  مجازی int32_t divide_به_بالا_Dictionary_Range( UText *متن,
          int32_t range_شروع,
          int32_t range_انتها,
          UVector32 &یافت_Breaks,
          UBool isPhraseBreaking,
          UError_کد& وضعیت) ثابت override;

};

#پایان_اگر

U_NAMESPACE_انتها

    /* DICTBE_H */
#پایان_اگر
