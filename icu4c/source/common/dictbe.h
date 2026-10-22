// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#నిర్వచితం_కాకపోతే DICTBE_H
#నిర్వచించు DICTBE_H

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/uniset.h"
#అన్తర్భావ "unicode/utext.h"

#అన్తర్భావ "brkeng.h"
#అన్తర్భావ "hash.h"
#అన్తర్భావ "mlbe.h"
#అన్తర్భావ "uvectr32.h"

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
class DictionaryBreakEngine : బహిరంగ LanguageBreakEngine {
 అంతరంగ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_సమితి    f_సమితి;

 బహిరంగ:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  మాయా ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  మాయా UBool handles(UChar32 c, స్థిరరాశి అక్షరం* locale) స్థిరరాశి override;

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
  మాయా int32_t కనుగొను_Breaks( UText *పాఠ్యం,
                              int32_t ప్రారంభం_Pos,
                              int32_t చివర_Pos,
                              UVector32 &దొరికింది_Breaks,
                              UBool isPhraseBreaking,
                              UError_సంకేతం& స్థితి_2 ) స్థిరరాశి override;

 రక్షిత:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  మాయా శూన్య సమితి_Characters( స్థిరరాశి Unicode_సమితి &సమితి );

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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                           int32_t range_ప్రారంభం,
                                           int32_t range_చివర,
                                           UVector32 &దొరికింది_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_సంకేతం& స్థితి_2) స్థిరరాశి = 0;

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
class ThaiBreakEngine : బహిరంగ DictionaryBreakEngine {
 అంతరంగ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_సమితి                f_చివర_మాట_సమితి;
  Unicode_సమితి                f_Begin_మాట_సమితి;
  Unicode_సమితి                f_Suffix_సమితి;
  Unicode_సమితి                f_Mark_సమితి;
  DictionaryMatcher  *fDictionary;

 బహిరంగ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2);

  /**
   * <p>Virtual destructor.</p>
   */
  మాయా ~ThaiBreakEngine();

 రక్షిత:
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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                           int32_t range_ప్రారంభం,
                                           int32_t range_చివర,
                                           UVector32 &దొరికింది_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

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
class LaoBreakEngine : బహిరంగ DictionaryBreakEngine {
 అంతరంగ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_సమితి                f_చివర_మాట_సమితి;
  Unicode_సమితి                f_Begin_మాట_సమితి;
  Unicode_సమితి                f_Mark_సమితి;
  DictionaryMatcher  *fDictionary;

 బహిరంగ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2);

  /**
   * <p>Virtual destructor.</p>
   */
  మాయా ~LaoBreakEngine();

 రక్షిత:
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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                           int32_t range_ప్రారంభం,
                                           int32_t range_చివర,
                                           UVector32 &దొరికింది_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

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
class BurmeseBreakEngine : బహిరంగ DictionaryBreakEngine {
 అంతరంగ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_సమితి                f_చివర_మాట_సమితి;
  Unicode_సమితి                f_Begin_మాట_సమితి;
  Unicode_సమితి                f_Mark_సమితి;
  DictionaryMatcher  *fDictionary;

 బహిరంగ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2);

  /**
   * <p>Virtual destructor.</p>
   */
  మాయా ~BurmeseBreakEngine();

 రక్షిత:
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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                           int32_t range_ప్రారంభం,
                                           int32_t range_చివర,
                                           UVector32 &దొరికింది_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

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
class KhmerBreakEngine : బహిరంగ DictionaryBreakEngine {
 అంతరంగ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_సమితి                f_చివర_మాట_సమితి;
  Unicode_సమితి                f_Begin_మాట_సమితి;
  Unicode_సమితి                f_Mark_సమితి;
  DictionaryMatcher  *fDictionary;

 బహిరంగ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_సంకేతం &స్థితి_2);

  /**
   * <p>Virtual destructor.</p>
   */
  మాయా ~KhmerBreakEngine();

 రక్షిత:
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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
                                           int32_t range_ప్రారంభం,
                                           int32_t range_చివర,
                                           UVector32 &దొరికింది_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

};

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
గణన LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : బహిరంగ DictionaryBreakEngine {
 రక్షిత:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_సమితి                f_Hangul_మాట_సమితి;
  Unicode_సమితి                f_Digit_Or_Open_Punctuation_Or_Alphabet_సమితి;
  Unicode_సమితి                f_Close_Punctuation_సమితి;

  DictionaryMatcher        *fDictionary;
  స్థిరరాశి Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 అంతరంగ:
  // Load Japanese extensions.
  శూన్య ఎక్కించు_Japanese_Extensions(UError_సంకేతం& error);
  // Load Japanese Hiragana.
  శూన్య ఎక్కించు_Hiragana(UError_సంకేతం& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  శూన్య initJapanesePhraseParameter(UError_సంకేతం& error);

  Hashtable f_Skip_సమితి;

 బహిరంగ:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_సంకేతం &స్థితి_2);

    /**
     * <p>Virtual destructor.</p>
     */
  మాయా ~CjkBreakEngine();

 రక్షిత:
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
  మాయా int32_t divide_పైకి_Dictionary_Range( UText *పాఠ్యం,
          int32_t range_ప్రారంభం,
          int32_t range_చివర,
          UVector32 &దొరికింది_Breaks,
          UBool isPhraseBreaking,
          UError_సంకేతం& స్థితి_2) స్థిరరాశి override;

};

#అయితే_ముగింపు

U_NAMESPACE_చివర

    /* DICTBE_H */
#అయితే_ముగింపు
