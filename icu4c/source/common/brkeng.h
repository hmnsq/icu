// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 ************************************************************************************
 * Copyright (C) 2006-2012, International Business Machines Corporation and others. *
 * All Rights Reserved.                                                             *
 ************************************************************************************
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BRKENG_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BRKENG_H

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/umisc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utext.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"

U_NAMESPACE_BEGIN

class Unicode_ਸੈੱਟ;
class UStack;
class UVector32;
class DictionaryMatcher;
class ExternalBreakEngine;

/*******************************************************************
 * LanguageBreakEngine
 */

/**
 * <p>LanguageBreakEngines implement language-specific knowledge for
 * finding text boundaries within a run of characters belonging to a
 * specific set. The boundaries will be of a specific kind, e.g. word,
 * line, etc.</p>
 *
 * <p>LanguageBreakEngines should normally be implemented so as to
 * be shared between threads without locking.</p>
 */
class LanguageBreakEngine : ਜਨਤਕ UObject {
 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   */
  LanguageBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~LanguageBreakEngine();

 /**
  * <p>Indicate whether this engine handles a particular character for
  * a particular kind of break.</p>
  *
  * @param c A character which begins a run that the engine might handle
  * @param locale The locale.
  * @return true if this engine handles the particular character and break
  * type.
  */
  ਆਭਾਸੀ UBool handles(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) ਅਚਲ = 0;

 /**
  * <p>Find any breaks within a run in the supplied text.</p>
  *
  * @param text A UText representing the text. The
  * iterator is left at the end of the run of characters which the engine
  * is capable of handling.
  * @param startPos The start of the run within the supplied text.
  * @param endPos The end of the run within the supplied text.
  * @param foundBreaks A Vector of int32_t to receive the breaks.
  * @param status Information on any errors encountered.
  * @return The number of breaks found.
  */
  ਆਭਾਸੀ int32_t ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                              int32_t ਸ਼ੁਰੂ_Pos,
                              int32_t ਸਿਰਾ_Pos,
                              UVector32 &ਮਿਲਿਆ_Breaks,
                              UBool isPhraseBreaking,
                              UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ = 0;

};

/*******************************************************************
 * BreakEngineWrapper
 */

/**
 * <p>BreakEngineWrapper implement LanguageBreakEngine by
 * a thin wrapper that delegate the task to ExternalBreakEngine
 * </p>
 */
class BreakEngineWrapper : ਜਨਤਕ  LanguageBreakEngine {
 ਜਨਤਕ:

  BreakEngineWrapper(ExternalBreakEngine* engine, UError_ਕੋਡ &ਸਥਿਤੀ);

  ਆਭਾਸੀ ~BreakEngineWrapper();

  ਆਭਾਸੀ UBool handles(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) ਅਚਲ override;

  ਆਭਾਸੀ int32_t ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                              int32_t ਸ਼ੁਰੂ_Pos,
                              int32_t ਸਿਰਾ_Pos,
                              UVector32 &ਮਿਲਿਆ_Breaks,
                              UBool isPhraseBreaking,
                              UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override;

 ਨਿਜੀ:
  Local_ਸੂਚਕ<ExternalBreakEngine> delegate;
};

/*******************************************************************
 * LanguageBreakFactory
 */

/**
 * <p>LanguageBreakFactorys find and return a LanguageBreakEngine
 * that can determine breaks for characters in a specific set, if
 * such an object can be found.</p>
 *
 * <p>If a LanguageBreakFactory is to be shared between threads,
 * appropriate synchronization must be used; there is none internal
 * to the factory.</p>
 *
 * <p>A LanguageBreakEngine returned by a LanguageBreakFactory can
 * normally be shared between threads without synchronization, unless
 * the specific subclass of LanguageBreakFactory indicates otherwise.</p>
 *
 * <p>A LanguageBreakFactory is responsible for deleting any LanguageBreakEngine
 * it returns when it itself is deleted, unless the specific subclass of
 * LanguageBreakFactory indicates otherwise. Naturally, the factory should
 * not be deleted until the LanguageBreakEngines it has returned are no
 * longer needed.</p>
 */
class LanguageBreakFactory : ਜਨਤਕ UMemory {
 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   */
  LanguageBreakFactory();

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~LanguageBreakFactory();

 /**
  * <p>Find and return a LanguageBreakEngine that can find the desired
  * kind of break for the set of characters to which the supplied
  * character belongs. It is up to the set of available engines to
  * determine what the sets of characters are.</p>
  *
  * @param c A character that begins a run for which a LanguageBreakEngine is
  * sought.
  * @param locale The locale.
  * @return A LanguageBreakEngine with the desired characteristics, or 0.
  */
  ਆਭਾਸੀ ਅਚਲ LanguageBreakEngine *getEngineFor(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) = 0;

};

/*******************************************************************
 * UnhandledEngine
 */

/**
 * <p>UnhandledEngine is a special subclass of LanguageBreakEngine that
 * handles characters that no other LanguageBreakEngine is available to
 * handle. It is told the character and the type of break; at its
 * discretion it may handle more than the specified character (e.g.,
 * the entire script to which that character belongs.</p>
 *
 * <p>UnhandledEngines may not be shared between threads without
 * external synchronization.</p>
 */

class UnhandledEngine : ਜਨਤਕ LanguageBreakEngine {
 ਨਿਜੀ:

    /**
     * The sets of characters handled.
     * @internal
     */

  Unicode_ਸੈੱਟ    *fHandled;

 ਜਨਤਕ:

  /**
   * <p>Default constructor.</p>
   *
   */
  UnhandledEngine(UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~UnhandledEngine();

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
  * @param text A UText representing the text (TODO: UText). The
  * iterator is left at the end of the run of characters which the engine
  * is capable of handling.
  * @param startPos The start of the run within the supplied text.
  * @param endPos The end of the run within the supplied text.
  * @param foundBreaks An allocated C array of the breaks found, if any
  * @param status Information on any errors encountered.
  * @return The number of breaks found.
  */
  ਆਭਾਸੀ int32_t ਲੱਭੋ_Breaks( UText *ਪਾਠ,
                              int32_t ਸ਼ੁਰੂ_Pos,
                              int32_t ਸਿਰਾ_Pos,
                              UVector32 &ਮਿਲਿਆ_Breaks,
                              UBool isPhraseBreaking,
                              UError_ਕੋਡ &ਸਥਿਤੀ) ਅਚਲ override;

 /**
  * <p>Tell the engine to handle a particular character and break type.</p>
  *
  * @param c A character which the engine should handle
  */
  ਆਭਾਸੀ ਖਾਲੀ handleCharacter(UChar32 c);

};

/*******************************************************************
 * ICULanguageBreakFactory
 */

/**
 * <p>ICULanguageBreakFactory is the default LanguageBreakFactory for
 * ICU. It creates dictionary-based LanguageBreakEngines from dictionary
 * data in the ICU data file.</p>
 */
class ICULanguageBreakFactory : ਜਨਤਕ LanguageBreakFactory {
 ਨਿਜੀ:

    /**
     * The stack of break engines created by this factory
     * @internal
     */

  UStack    *fEngines;

 ਜਨਤਕ:

  /**
   * <p>Standard constructor.</p>
   *
   */
  ICULanguageBreakFactory(UError_ਕੋਡ &ਸਥਿਤੀ);

  /**
   * <p>Virtual destructor.</p>
   */
  ਆਭਾਸੀ ~ICULanguageBreakFactory();

 /**
  * <p>Find and return a LanguageBreakEngine that can find the desired
  * kind of break for the set of characters to which the supplied
  * character belongs. It is up to the set of available engines to
  * determine what the sets of characters are.</p>
  *
  * @param c A character that begins a run for which a LanguageBreakEngine is
  * sought.
  * @param locale The locale.
  * @return A LanguageBreakEngine with the desired characteristics, or 0.
  */
  ਆਭਾਸੀ ਅਚਲ LanguageBreakEngine *getEngineFor(UChar32 c, ਅਚਲ ਅੱਖਰ* locale) override;

  /**
   * Add and adopt the engine and return an URegistryKey.
   * @param engine The ExternalBreakEngine to be added and adopt. The caller
   *     pass the ownership and should not release the memory after this.
   * @param status the error code.
   */
  ਆਭਾਸੀ ਖਾਲੀ ਜੋੜੋ_External_Engine(ExternalBreakEngine* engine, UError_ਕੋਡ& ਸਥਿਤੀ);

ਸੁਰੱਖਿਅਤ:
 /**
  * <p>Create a LanguageBreakEngine for the set of characters to which
  * the supplied character belongs, for the specified break type.</p>
  *
  * @param c A character that begins a run for which a LanguageBreakEngine is
  * sought.
  * @param locale The locale.
  * @return A LanguageBreakEngine with the desired characteristics, or 0.
  */
  ਆਭਾਸੀ ਅਚਲ LanguageBreakEngine *ਲੱਦੋ_Engine_For(UChar32 c, ਅਚਲ ਅੱਖਰ* locale);

  /**
   * <p>Create a DictionaryMatcher for the specified script and break type.</p>
   * @param script An ISO 15924 script code that identifies the dictionary to be
   * created.
   * @return A DictionaryMatcher with the desired characteristics, or nullptr.
   */
  ਆਭਾਸੀ DictionaryMatcher *ਲੱਦੋ_Dictionary_Matcher_For(UScript_ਕੋਡ script);

 ਨਿਜੀ:
  ਖਾਲੀ ensureEngines(UError_ਕੋਡ& ਸਥਿਤੀ);
};

U_NAMESPACE_ਸਿਰਾ

    /* BRKENG_H */
#ਜੇ_ਅੰਤ
