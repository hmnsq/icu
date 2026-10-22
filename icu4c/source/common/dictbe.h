// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 *******************************************************************************
 * Copyright (C) 2006-2014, International Business Machines Corporation   *
 * and others. All Rights Reserved.                                            *
 *******************************************************************************
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DICTBE_H
#ವ್ಯಾಖ್ಯಾನಿಸು DICTBE_H

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/uniset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utext.h"

#ಅನ್ತರ್ಭಾವ "brkeng.h"
#ಅನ್ತರ್ಭಾವ "hash.h"
#ಅನ್ತರ್ಭಾವ "mlbe.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

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
class DictionaryBreakEngine : ಸಾರ್ವಜನಿಕ LanguageBreakEngine {
 ಖಾಸಗಿ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ಗಣ    f_ಗಣ;

 ಸಾರ್ವಜನಿಕ:

  /**
   * <p>Constructor </p>
   */
  DictionaryBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  ವಾಸ್ತವಿಕ ~DictionaryBreakEngine();

  /**
   * <p>Indicate whether this engine handles a particular character for
   * a particular kind of break.</p>
   *
   * @param c A character which begins a run that the engine might handle
   * @param locale The locale.
   * @return true if this engine handles the particular character and break
   * type.
   */
  ವಾಸ್ತವಿಕ UBool handles(UChar32 c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* locale) ಸ್ಥಿರಾಂಕ override;

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
  ವಾಸ್ತವಿಕ int32_t ಕಂಡುಹಿಡಿ_Breaks( UText *ಪಠ್ಯ,
                              int32_t ಆರಂಭ_Pos,
                              int32_t ಕೊನೆ_Pos,
                              UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                              UBool isPhraseBreaking,
                              UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2 ) ಸ್ಥಿರಾಂಕ override;

 ಸಂರಕ್ಷಿತ:

 /**
  * <p>Set the character set handled by this engine.</p>
  *
  * @param set A UnicodeSet of the set of characters handled by the engine
  */
  ವಾಸ್ತವಿಕ ಶೂನ್ಯ ಗಣ_Characters( ಸ್ಥಿರಾಂಕ Unicode_ಗಣ &ಗಣ );

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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                           int32_t range_ಆರಂಭ,
                                           int32_t range_ಕೊನೆ,
                                           UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ = 0;

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
class ThaiBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
 ಖಾಸಗಿ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ಗಣ                f_ಕೊನೆ_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Begin_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Suffix_ಗಣ;
  Unicode_ಗಣ                f_Mark_ಗಣ;
  DictionaryMatcher  *fDictionary;

 ಸಾರ್ವಜನಿಕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  ThaiBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

  /**
   * <p>Virtual destructor.</p>
   */
  ವಾಸ್ತವಿಕ ~ThaiBreakEngine();

 ಸಂರಕ್ಷಿತ:
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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                           int32_t range_ಆರಂಭ,
                                           int32_t range_ಕೊನೆ,
                                           UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

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
class LaoBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
 ಖಾಸಗಿ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ಗಣ                f_ಕೊನೆ_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Begin_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Mark_ಗಣ;
  DictionaryMatcher  *fDictionary;

 ಸಾರ್ವಜನಿಕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  LaoBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

  /**
   * <p>Virtual destructor.</p>
   */
  ವಾಸ್ತವಿಕ ~LaoBreakEngine();

 ಸಂರಕ್ಷಿತ:
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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                           int32_t range_ಆರಂಭ,
                                           int32_t range_ಕೊನೆ,
                                           UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

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
class BurmeseBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
 ಖಾಸಗಿ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ಗಣ                f_ಕೊನೆ_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Begin_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Mark_ಗಣ;
  DictionaryMatcher  *fDictionary;

 ಸಾರ್ವಜನಿಕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  BurmeseBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

  /**
   * <p>Virtual destructor.</p>
   */
  ವಾಸ್ತವಿಕ ~BurmeseBreakEngine();

 ಸಂರಕ್ಷಿತ:
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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                           int32_t range_ಆರಂಭ,
                                           int32_t range_ಕೊನೆ,
                                           UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

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
class KhmerBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
 ಖಾಸಗಿ:
    /**
     * The set of characters handled by this engine
     * @internal
     */

  Unicode_ಗಣ                f_ಕೊನೆ_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Begin_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Mark_ಗಣ;
  DictionaryMatcher  *fDictionary;

 ಸಾರ್ವಜನಿಕ:

  /**
   * <p>Default constructor.</p>
   *
   * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
   * engine is deleted.
   */
  KhmerBreakEngine(DictionaryMatcher *adoptDictionary, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

  /**
   * <p>Virtual destructor.</p>
   */
  ವಾಸ್ತವಿಕ ~KhmerBreakEngine();

 ಸಂರಕ್ಷಿತ:
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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
                                           int32_t range_ಆರಂಭ,
                                           int32_t range_ಕೊನೆ,
                                           UVector32 &ಸಿಕ್ಕಿತು_Breaks,
                                           UBool isPhraseBreaking,
                                           UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

};

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

/*******************************************************************
 * CjkBreakEngine
 */

//indicates language/script that the CjkBreakEngine will handle
ಎಣಿಕೆ LanguageType {
    kKorean,
    kChineseJapanese
};

/**
 * <p>CjkBreakEngine is a kind of DictionaryBreakEngine that uses a
 * dictionary with costs associated with each word and
 * Viterbi decoding to determine CJK-specific breaks.</p>
 */
class CjkBreakEngine : ಸಾರ್ವಜನಿಕ DictionaryBreakEngine {
 ಸಂರಕ್ಷಿತ:
    /**
     * The set of characters handled by this engine
     * @internal
     */
  Unicode_ಗಣ                f_Hangul_ಶಬ್ದ_ಗಣ;
  Unicode_ಗಣ                f_Digit_Or_Open_Punctuation_Or_Alphabet_ಗಣ;
  Unicode_ಗಣ                f_Close_Punctuation_ಗಣ;

  DictionaryMatcher        *fDictionary;
  ಸ್ಥಿರಾಂಕ Normalizer2        *nfkcNorm2;
  MlBreakEngine            *fMlBreakEngine;
  bool                      isCj;

 ಖಾಸಗಿ:
  // Load Japanese extensions.
  ಶೂನ್ಯ ತುಂಬು_Japanese_Extensions(UError_ಸಂಕೇತ& error);
  // Load Japanese Hiragana.
  ಶೂನ್ಯ ತುಂಬು_Hiragana(UError_ಸಂಕೇತ& error);
  // Initialize fSkipSet by loading Japanese Hiragana and extensions.
  ಶೂನ್ಯ initJapanesePhraseParameter(UError_ಸಂಕೇತ& error);

  Hashtable f_Skip_ಗಣ;

 ಸಾರ್ವಜನಿಕ:

    /**
     * <p>Default constructor.</p>
     *
     * @param adoptDictionary A DictionaryMatcher to adopt. Deleted when the
     * engine is deleted. The DictionaryMatcher must contain costs for each word
     * in order for the dictionary to work properly.
     */
  CjkBreakEngine(DictionaryMatcher *adoptDictionary, LanguageType type, UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2);

    /**
     * <p>Virtual destructor.</p>
     */
  ವಾಸ್ತವಿಕ ~CjkBreakEngine();

 ಸಂರಕ್ಷಿತ:
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
  ವಾಸ್ತವಿಕ int32_t divide_ಮೇಲೆ_Dictionary_Range( UText *ಪಠ್ಯ,
          int32_t range_ಆರಂಭ,
          int32_t range_ಕೊನೆ,
          UVector32 &ಸಿಕ್ಕಿತು_Breaks,
          UBool isPhraseBreaking,
          UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) ಸ್ಥಿರಾಂಕ override;

};

#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_ಕೊನೆ

    /* DICTBE_H */
#ಆದರೆ_ಅಂತ್ಯ
