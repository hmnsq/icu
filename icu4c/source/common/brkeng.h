// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/**
 ************************************************************************************
 * Copyright (C) 2006-2012, International Business Machines Corporation and others. *
 * All Rights Reserved.                                                             *
 ************************************************************************************
 */

#ifndef BRKENG_H
#define BRKENG_H

#അന്തര്ഭാവ "unicode/umisc.h"
#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "unicode/utext.h"
#അന്തര്ഭാവ "unicode/uscript.h"

U_NAMESPACE_BEGIN

class UnicodeSet;
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
class LanguageBreakEngine : പൊതു UObject {
 പൊതു:

  /**
   * <p>Default constructor.</p>
   *
   */
  LanguageBreakEngine();

  /**
   * <p>Virtual destructor.</p>
   */
  മായാ ~LanguageBreakEngine();

 /**
  * <p>Indicate whether this engine handles a particular character for
  * a particular kind of break.</p>
  *
  * @param c A character which begins a run that the engine might handle
  * @param locale The locale.
  * @return true if this engine handles the particular character and break
  * type.
  */
  മായാ UBool handles(UChar32 c, സ്ഥിരം അക്ഷരം* locale) സ്ഥിരം = 0;

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
  മായാ int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t endPos,
                              UVector32 &foundBreaks,
                              UBool isPhraseBreaking,
                              UErrorCode &status) സ്ഥിരം = 0;

};

/*******************************************************************
 * BreakEngineWrapper
 */

/**
 * <p>BreakEngineWrapper implement LanguageBreakEngine by
 * a thin wrapper that delegate the task to ExternalBreakEngine
 * </p>
 */
class BreakEngineWrapper : പൊതു  LanguageBreakEngine {
 പൊതു:

  BreakEngineWrapper(ExternalBreakEngine* engine, UErrorCode &status);

  മായാ ~BreakEngineWrapper();

  മായാ UBool handles(UChar32 c, സ്ഥിരം അക്ഷരം* locale) സ്ഥിരം override;

  മായാ int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t endPos,
                              UVector32 &foundBreaks,
                              UBool isPhraseBreaking,
                              UErrorCode &status) സ്ഥിരം override;

 സ്വകാര്യ:
  LocalPointer<ExternalBreakEngine> delegate;
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
class LanguageBreakFactory : പൊതു UMemory {
 പൊതു:

  /**
   * <p>Default constructor.</p>
   *
   */
  LanguageBreakFactory();

  /**
   * <p>Virtual destructor.</p>
   */
  മായാ ~LanguageBreakFactory();

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
  മായാ സ്ഥിരം LanguageBreakEngine *getEngineFor(UChar32 c, സ്ഥിരം അക്ഷരം* locale) = 0;

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

class UnhandledEngine : പൊതു LanguageBreakEngine {
 സ്വകാര്യ:

    /**
     * The sets of characters handled.
     * @internal
     */

  UnicodeSet    *fHandled;

 പൊതു:

  /**
   * <p>Default constructor.</p>
   *
   */
  UnhandledEngine(UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  മായാ ~UnhandledEngine();

 /**
  * <p>Indicate whether this engine handles a particular character for
  * a particular kind of break.</p>
  *
  * @param c A character which begins a run that the engine might handle
  * @param locale The locale.
  * @return true if this engine handles the particular character and break
  * type.
  */
  മായാ UBool handles(UChar32 c, സ്ഥിരം അക്ഷരം* locale) സ്ഥിരം override;

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
  മായാ int32_t findBreaks( UText *text,
                              int32_t startPos,
                              int32_t endPos,
                              UVector32 &foundBreaks,
                              UBool isPhraseBreaking,
                              UErrorCode &status) സ്ഥിരം override;

 /**
  * <p>Tell the engine to handle a particular character and break type.</p>
  *
  * @param c A character which the engine should handle
  */
  മായാ ശൂന്യം handleCharacter(UChar32 c);

};

/*******************************************************************
 * ICULanguageBreakFactory
 */

/**
 * <p>ICULanguageBreakFactory is the default LanguageBreakFactory for
 * ICU. It creates dictionary-based LanguageBreakEngines from dictionary
 * data in the ICU data file.</p>
 */
class ICULanguageBreakFactory : പൊതു LanguageBreakFactory {
 സ്വകാര്യ:

    /**
     * The stack of break engines created by this factory
     * @internal
     */

  UStack    *fEngines;

 പൊതു:

  /**
   * <p>Standard constructor.</p>
   *
   */
  ICULanguageBreakFactory(UErrorCode &status);

  /**
   * <p>Virtual destructor.</p>
   */
  മായാ ~ICULanguageBreakFactory();

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
  മായാ സ്ഥിരം LanguageBreakEngine *getEngineFor(UChar32 c, സ്ഥിരം അക്ഷരം* locale) override;

  /**
   * Add and adopt the engine and return an URegistryKey.
   * @param engine The ExternalBreakEngine to be added and adopt. The caller
   *     pass the ownership and should not release the memory after this.
   * @param status the error code.
   */
  മായാ ശൂന്യം addExternalEngine(ExternalBreakEngine* engine, UErrorCode& status);

സംരക്ഷിത:
 /**
  * <p>Create a LanguageBreakEngine for the set of characters to which
  * the supplied character belongs, for the specified break type.</p>
  *
  * @param c A character that begins a run for which a LanguageBreakEngine is
  * sought.
  * @param locale The locale.
  * @return A LanguageBreakEngine with the desired characteristics, or 0.
  */
  മായാ സ്ഥിരം LanguageBreakEngine *loadEngineFor(UChar32 c, സ്ഥിരം അക്ഷരം* locale);

  /**
   * <p>Create a DictionaryMatcher for the specified script and break type.</p>
   * @param script An ISO 15924 script code that identifies the dictionary to be
   * created.
   * @return A DictionaryMatcher with the desired characteristics, or nullptr.
   */
  മായാ DictionaryMatcher *loadDictionaryMatcherFor(UScriptCode script);

 സ്വകാര്യ:
  ശൂന്യം ensureEngines(UErrorCode& status);
};

U_NAMESPACE_END

    /* BRKENG_H */
#endif
