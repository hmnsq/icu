// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#यदि_अपरिभाषितम् DICTBE_H
#परिभाषय DICTBE_H

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/utext.h"

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "hash.h"
#अन्तर्भाव "mlbe.h"
#अन्तर्भाव "uvectr32.h"

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
class DictionaryBreakEngine : सार्वजनिक LanguageBreakEngine {
 गुप्त:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_समुच्चयः    f_समुच्चयः;

 सार्वजनिक:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  आभास UBool handles(UChar32 c, ध्रुव वर्ण* locale) ध्रुव override;

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
  आभास int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t अन्तः_Pos,
                              UVector32 &प्राप्तम्_Breaks,
                              UBool isPhraseBreaking,
                              UError_संहिता& स्थितिः ) ध्रुव override;

 रक्षित:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  आभास शून्य समुच्चयः_Characters( ध्रुव Unicode_समुच्चयः &समुच्चयः );

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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                           int32_t rangeStart,
                                           int32_t range_अन्तः,
                                           UVector32 &प्राप्तम्_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_संहिता& स्थितिः) ध्रुव = 0;

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
class ThaiBreakEngine : सार्वजनिक DictionaryBreakEngine {
 गुप्त:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_समुच्चयः                f_अन्तः_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Begin_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Suffix_समुच्चयः;
  Unicode_समुच्चयः                f_Mark_समुच्चयः;
  DictionaryMatcher  *fDictionary;

 सार्वजनिक:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः);

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~ThaiBreakEngine();

 रक्षित:
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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                           int32_t rangeStart,
                                           int32_t range_अन्तः,
                                           UVector32 &प्राप्तम्_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_संहिता& स्थितिः) ध्रुव override;

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
class LaoBreakEngine : सार्वजनिक DictionaryBreakEngine {
 गुप्त:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_समुच्चयः                f_अन्तः_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Begin_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Mark_समुच्चयः;
  DictionaryMatcher  *fDictionary;

 सार्वजनिक:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः);

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~LaoBreakEngine();

 रक्षित:
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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                           int32_t rangeStart,
                                           int32_t range_अन्तः,
                                           UVector32 &प्राप्तम्_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_संहिता& स्थितिः) ध्रुव override;

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
class BurmeseBreakEngine : सार्वजनिक DictionaryBreakEngine {
 गुप्त:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_समुच्चयः                f_अन्तः_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Begin_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Mark_समुच्चयः;
  DictionaryMatcher  *fDictionary;

 सार्वजनिक:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः);

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~BurmeseBreakEngine();

 रक्षित:
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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                           int32_t rangeStart,
                                           int32_t range_अन्तः,
                                           UVector32 &प्राप्तम्_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_संहिता& स्थितिः) ध्रुव override;

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
class KhmerBreakEngine : सार्वजनिक DictionaryBreakEngine {
 गुप्त:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_समुच्चयः                f_अन्तः_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Begin_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Mark_समुच्चयः;
  DictionaryMatcher  *fDictionary;

 सार्वजनिक:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_संहिता &स्थितिः);

  /**
   * <p>Virtual destructor.</p>
   */
  आभास ~KhmerBreakEngine();

 रक्षित:
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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
                                           int32_t rangeStart,
                                           int32_t range_अन्तः,
                                           UVector32 &प्राप्तम्_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_संहिता& स्थितिः) ध्रुव override;

};

#यदि !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
गणना LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : सार्वजनिक DictionaryBreakEngine {
 रक्षित:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_समुच्चयः                f_Hangul_Word_समुच्चयः;
  Unicode_समुच्चयः                f_Digit_Or_Open_Punctuation_Or_Alphabet_समुच्चयः;
  Unicode_समुच्चयः                f_Close_Punctuation_समुच्चयः;

  DictionaryMatcher        *fDictionary;
  ध्रुव Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 गुप्त:
  // Load Japanese extensions.
  शून्य आरोपय_Japanese_Extensions(UError_संहिता& error);
  // Load Japanese Hiragana.
  शून्य आरोपय_Hiragana(UError_संहिता& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  शून्य initJapanesePhraseParameter(UError_संहिता& error);

  Hashtable f_Skip_समुच्चयः;

 सार्वजनिक:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_संहिता &स्थितिः);

    /**
     * <p>Virtual destructor.</p>
     */
  आभास ~CjkBreakEngine();

 रक्षित:
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
  आभास int32_t divide_ऊर्ध्वम्_Dictionary_Range( UText *text,
          int32_t rangeStart,
          int32_t range_अन्तः,
          UVector32 &प्राप्तम्_Breaks,
          UBool isPhraseBreaking,
          UError_संहिता& स्थितिः) ध्रुव override;

};

#यदि_अन्तः

U_NAMESPACE_अन्तः

    /* DICTBE_H */
#यदि_अन्तः
