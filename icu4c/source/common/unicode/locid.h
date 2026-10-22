// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1996-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*
* File locid.h
*
* Created by: Helena Shih
*
* Modification History:
*
*   Date        Name        Description
*   02/11/97    aliu        Changed gLocPath to fgLocPath and added methods to
*                           get and set it.
*   04/02/97    aliu        Made operator!= inline; fixed return value of getName().
*   04/15/97    aliu        Cleanup for AIX/Win32.
*   04/24/97    aliu        Numerous changes per code review.
*   08/18/98    stephen     Added tokenizeString(),changed getDisplayName()
*   09/08/98    stephen     Moved definition of kEmptyString for Mac Port
*   11/09/99    weiv        Added const char * getName() const;
*   04/12/00    srl         removing unicodestring api's and cached hash code
*   08/10/01    grhoten     Change the static Locales to accessor functions
******************************************************************************
*/

#اگر_غیر_تعریف_شدہ LOCID_H
#تعریف_کرو LOCID_H

#شامل "unicode/utypes.h"

#اگر U_SHOW_CPLUSPLUS_API

#شامل <cstdint>
#شامل <string_view>

#شامل "unicode/bytestream.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/strenum.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uobject.h"
#شامل "unicode/putil.h"
#شامل "unicode/uloc.h"

/**
 * \file
 * \brief C++ API: Locale ID object.
 */

U_NAMESPACE_BEGIN

// Forward Declarations
خالی U_CALLCONV locale_دستیاب_init(); /**< @internal */

class StringEnumeration;
class UnicodeString;

/**
 * A <code>Locale</code> object represents a specific geographical, political,
 * or cultural region. An operation that requires a <code>Locale</code> to perform
 * its task is called <em>locale-sensitive</em> and uses the <code>Locale</code>
 * to tailor information for the user. For example, displaying a number
 * is a locale-sensitive operation--the number should be formatted
 * according to the customs/conventions of the user's native country,
 * region, or culture.
 *
 * The Locale class is not suitable for subclassing.
 *
 * <P>
 * You can create a <code>Locale</code> object using the constructor in
 * this class:
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *       Locale( const   char*  language,
 *               const   char*  country,
 *               const   char*  variant);
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 * The first argument to the constructors is a valid <STRONG>ISO
 * Language Code.</STRONG> These codes are the lower-case two-letter
 * codes as defined by ISO-639.
 * You can find a full list of these codes at:
 * <BR><a href ="http://www.loc.gov/standards/iso639-2/">
 * http://www.loc.gov/standards/iso639-2/</a>
 *
 * <P>
 * The second argument to the constructors is a valid <STRONG>ISO Country
 * Code.</STRONG> These codes are the upper-case two-letter codes
 * as defined by ISO-3166.
 * You can find a full list of these codes at a number of sites, such as:
 * <BR><a href="http://www.iso.org/iso/en/prods-services/iso3166ma/index.html">
 * http://www.iso.org/iso/en/prods-services/iso3166ma/index.html</a>
 *
 * <P>
 * The third constructor requires a third argument--the <STRONG>Variant.</STRONG>
 * The Variant codes are vendor and browser-specific.
 * For example, use REVISED for a language's revised script orthography, and POSIX for POSIX.
 * Where there are two variants, separate them with an underscore, and
 * put the most important one first. For
 * example, a Traditional Spanish collation might be referenced, with
 * "ES", "ES", "Traditional_POSIX".
 *
 * <P>
 * Because a <code>Locale</code> object is just an identifier for a region,
 * no validity check is performed when you construct a <code>Locale</code>.
 * If you want to see whether particular resources are available for the
 * <code>Locale</code> you construct, you must query those resources. For
 * example, ask the <code>NumberFormat</code> for the locales it supports
 * using its <code>getAvailableLocales</code> method.
 * <BR><STRONG>Note:</STRONG> When you ask for a resource for a particular
 * locale, you get back the best available match, not necessarily
 * precisely what you asked for. For more information, look at
 * <code>ResourceBundle</code>.
 *
 * <P>
 * The <code>Locale</code> class provides a number of convenient constants
 * that you can use to create <code>Locale</code> objects for commonly used
 * locales. For example, the following refers to a <code>Locale</code> object
 * for the United States:
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *       Locale::getUS()
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 *
 * <P>
 * Once you've created a <code>Locale</code> you can query it for information about
 * itself. Use <code>getCountry</code> to get the ISO Country Code and
 * <code>getLanguage</code> to get the ISO Language Code. You can
 * use <code>getDisplayCountry</code> to get the
 * name of the country suitable for displaying to the user. Similarly,
 * you can use <code>getDisplayLanguage</code> to get the name of
 * the language suitable for displaying to the user. Interestingly,
 * the <code>getDisplayXXX</code> methods are themselves locale-sensitive
 * and have two versions: one that uses the default locale and one
 * that takes a locale as an argument and displays the name or country in
 * a language appropriate to that locale.
 *
 * <P>
 * ICU provides a number of classes that perform locale-sensitive
 * operations. For example, the <code>NumberFormat</code> class formats
 * numbers, currency, or percentages in a locale-sensitive manner. Classes
 * such as <code>NumberFormat</code> have a number of convenience methods
 * for creating a default object of that type. For example, the
 * <code>NumberFormat</code> class provides these three convenience methods
 * for creating a default <code>NumberFormat</code> object:
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *     UErrorCode success = U_ZERO_ERROR;
 *     Locale myLocale;
 *     NumberFormat *nf;
 *
 *     nf = NumberFormat::createInstance( success );          delete nf;
 *     nf = NumberFormat::createCurrencyInstance( success );  delete nf;
 *     nf = NumberFormat::createPercentInstance( success );   delete nf;
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 * Each of these methods has two variants; one with an explicit locale
 * and one without; the latter using the default locale.
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *     nf = NumberFormat::createInstance( myLocale, success );          delete nf;
 *     nf = NumberFormat::createCurrencyInstance( myLocale, success );  delete nf;
 *     nf = NumberFormat::createPercentInstance( myLocale, success );   delete nf;
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 * A <code>Locale</code> is the mechanism for identifying the kind of object
 * (<code>NumberFormat</code>) that you would like to get. The locale is
 * <STRONG>just</STRONG> a mechanism for identifying objects,
 * <STRONG>not</STRONG> a container for the objects themselves.
 *
 * <P>
 * Each class that performs locale-sensitive operations allows you
 * to get all the available objects of that type. You can sift
 * through these objects by language, country, or variant,
 * and use the display names to present a menu to the user.
 * For example, you can create a menu of all the collation objects
 * suitable for a given language. Such classes implement these
 * three class methods:
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *       static Locale* getAvailableLocales(int32_t& numLocales)
 *       static UnicodeString& getDisplayName(const Locale&  objectLocale,
 *                                            const Locale&  displayLocale,
 *                                            UnicodeString& displayName)
 *       static UnicodeString& getDisplayName(const Locale&  objectLocale,
 *                                            UnicodeString& displayName)
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 *
 * @stable ICU 2.0
 * @see ResourceBundle
 */
class U_COMMON_API_CLASS Locale : عوامی UObject {
عوامی:
    /** Useful constant for the Root locale. @stable ICU 4.4 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 get_جڑ();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getEnglish();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getFrench();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getGerman();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getItalian();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getJapanese();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getKorean();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getChinese();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getSimplifiedChinese();
    /** Useful constant for this language. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getTraditionalChinese();

    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getFrance();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getGermany();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getItaly();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getJapan();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getKorea();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getChina();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getPRC();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getTaiwan();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getUK();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getUS();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getCanada();
    /** Useful constant for this country/region. @stable ICU 2.0 */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getCanadaFrench();

    /**
     * Construct a default locale object, a Locale for the default locale ID.
     *
     * @see getDefault
     * @see uloc_getDefault
     * @stable ICU 2.0
     */
    U_COMMON_API Locale();

    /**
     * Construct a locale from language, country, variant.
     * If an error occurs, then the constructed object will be "bogus"
     * (isBogus() will return true).
     *
     * @param language Lowercase two-letter or three-letter ISO-639 code.
     *  This parameter can instead be an ICU style C locale (e.g. "en_US"),
     *  but the other parameters must not be used.
     *  This parameter can be nullptr; if so,
     *  the locale is initialized to match the current default locale.
     *  (This is the same as using the default constructor.)
     *  Please note: The Java Locale class does NOT accept the form
     *  'new Locale("en_US")' but only 'new Locale("en","US")'
     *
     * @param country  Uppercase two-letter ISO-3166 code. (optional)
     * @param variant  Uppercase vendor and browser specific code. See class
     *                 description. (optional)
     * @param keywordsAndValues A string consisting of keyword/values pairs, such as
     *                 "collation=phonebook;currency=euro"
     *
     * @see getDefault
     * @see uloc_getDefault
     * @stable ICU 2.0
     */
    U_COMMON_API Locale(ثابت حرف* language,
                        ثابت حرف* country = nullptr,
                        ثابت حرف* variant = nullptr,
                        ثابت حرف* keywords_And_اقدار = nullptr);

    /**
     * Initializes a Locale object from another Locale object.
     *
     * @param other The Locale object being copied in.
     * @stable ICU 2.0
     */
    U_COMMON_API Locale(ثابت Locale& other);

    /**
     * Move constructor; might leave source in bogus state.
     * This locale will have the same contents that the source locale had.
     *
     * @param other The Locale object being moved in.
     * @stable ICU 63
     */
    U_COMMON_API Locale(Locale&& other) noexcept;

    /**
     * Destructor
     * @stable ICU 2.0
     */
    U_COMMON_API مجازی ~Locale();

    /**
     * Replaces the entire contents of *this with the specified value.
     *
     * @param other The Locale object being copied in.
     * @return      *this
     * @stable ICU 2.0
     */
    U_COMMON_API Locale& عامل=(ثابت Locale& other) U_LIFETIME_BOUND;

    /**
     * Move assignment operator; might leave source in bogus state.
     * This locale will have the same contents that the source locale had.
     * The behavior is undefined if *this and the source are the same object.
     *
     * @param other The Locale object being moved in.
     * @return      *this
     * @stable ICU 63
     */
    U_COMMON_API Locale& عامل=(Locale&& other) noexcept U_LIFETIME_BOUND;

    /**
     * Checks if two locale keys are the same.
     *
     * @param other The locale key object to be compared with this.
     * @return      true if the two locale keys are the same, false otherwise.
     * @stable ICU 2.0
     */
    U_COMMON_API bool عامل==(ثابت Locale& other) ثابت;

    /**
     * Checks if two locale keys are not the same.
     *
     * @param other The locale key object to be compared with this.
     * @return      true if the two locale keys are not the same, false
     *              otherwise.
     * @stable ICU 2.0
     */
    U_COMMON_API اندرونی bool عامل!=(ثابت Locale& other) ثابت;

    /**
     * Clone this object.
     * Clones can be used concurrently in multiple threads.
     * If an error occurs, then nullptr is returned.
     * The caller must delete the clone.
     *
     * @return a clone of this object
     *
     * @see getDynamicClassID
     * @stable ICU 2.8
     */
    U_COMMON_API Locale* clone() ثابت;

#اگر_غیر_تعریف_شدہ U_HIDE_SYSTEM_API
    /**
     * Common methods of getting the current default Locale. Used for the
     * presentation: menus, dialogs, etc. Generally set once when your applet or
     * application is initialized, then never reset. (If you do reset the
     * default locale, you probably want to reload your GUI, so that the change
     * is reflected in your interface.)
     *
     * More advanced programs will allow users to use different locales for
     * different fields, e.g. in a spreadsheet.
     *
     * Note that the initial setting will match the host system.
     * @return a reference to the Locale object for the default locale ID
     * @system
     * @stable ICU 2.0
     */
    U_COMMON_API ساکن ثابت Locale& U_EXPORT2 getDefault();

    /**
     * Sets the default. Normally set once at the beginning of a process,
     * then never reset.
     * setDefault() only changes ICU's default locale ID, <strong>not</strong>
     * the default locale ID of the runtime environment.
     *
     * @param newLocale Locale to set to.  If nullptr, set to the value obtained
     *                  from the runtime environment.
     * @param success The error code.
     * @system
     * @stable ICU 2.0
     */
    U_COMMON_API ساکن خالی U_EXPORT2 سیٹ_Default(ثابت Locale& newLocale, UError_رمز& success);
#اگر_ختم  /* U_HIDE_SYSTEM_API */

    /**
     * Returns a Locale for the specified BCP47 language tag string.
     * If the specified language tag contains any ill-formed subtags,
     * the first such subtag and all following subtags are ignored.
     * <p>
     * This implements the 'Language-Tag' production of BCP 47, and so
     * supports legacy language tags (marked as “Type: grandfathered” in BCP 47)
     * (regular and irregular) as well as private use language tags.
     *
     * Private use tags are represented as 'x-whatever',
     * and legacy tags are converted to their canonical replacements where they exist.
     *
     * Note that a few legacy tags have no modern replacement;
     * these will be converted using the fallback described in
     * the first paragraph, so some information might be lost.
     *
     * @param tag     the input BCP47 language tag.
     * @param status  error information if creating the Locale failed.
     * @return        the Locale for the specified BCP47 language tag.
     * @stable ICU 63
     */
    U_COMMON_API ساکن Locale U_EXPORT2 for_Language_لیبل(StringPiece لیبل, UError_رمز& حالت_2);

    /**
     * Returns a well-formed language tag for this Locale.
     * <p>
     * <b>Note</b>: Any locale fields which do not satisfy the BCP47 syntax
     * requirement will be silently omitted from the result.
     *
     * If this function fails, partial output may have been written to the sink.
     *
     * @param sink    the output sink receiving the BCP47 language
     *                tag for this Locale.
     * @param status  error information if creating the language tag failed.
     * @stable ICU 63
     */
    U_COMMON_API خالی مقصد_Language_لیبل(ByteSink& sink, UError_رمز& حالت_2) ثابت;

    /**
     * Returns a well-formed language tag for this Locale.
     * <p>
     * <b>Note</b>: Any locale fields which do not satisfy the BCP47 syntax
     * requirement will be silently omitted from the result.
     *
     * @param status  error information if creating the language tag failed.
     * @return        the BCP47 language tag for this Locale.
     * @stable ICU 63
     */
    سانچہ<typename StringClass>
    اندرونی StringClass مقصد_Language_لیبل(UError_رمز& حالت_2) ثابت;

    /**
     * Creates a locale which has had minimal canonicalization
     * as per uloc_getName().
     * @param name The name to create from.  If name is null,
     *  the default Locale is used.
     * @return new locale object
     * @stable ICU 2.0
     * @see uloc_getName
     */
    U_COMMON_API ساکن Locale U_EXPORT2 createFromName(ثابت حرف* name);

#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    /** @internal */
    U_COMMON_API ساکن Locale U_EXPORT2 createFromName(StringPiece name);
#اگر_ختم  /* U_HIDE_INTERNAL_API */

    /**
     * Creates a locale from the given string after canonicalizing
     * the string according to CLDR by calling uloc_canonicalize().
     * @param name the locale ID to create from.  Must not be nullptr.
     * @return a new locale object corresponding to the given name
     * @stable ICU 3.0
     * @see uloc_canonicalize
     */
    U_COMMON_API ساکن Locale U_EXPORT2 createCanonical(ثابت حرف* name);

    /**
     * Returns the locale's ISO-639 language code.
     * @return      An alias to the code
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getLanguage() ثابت U_LIFETIME_BOUND;

    /**
     * Returns the locale's ISO-15924 abbreviation script code.
     * @return      An alias to the code
     * @see uscript_getShortName
     * @see uscript_getCode
     * @stable ICU 2.8
     */
    U_COMMON_API ثابت حرف* getScript() ثابت U_LIFETIME_BOUND;

    /**
     * Returns the locale's ISO-3166 country code.
     * @return      An alias to the code
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getCountry() ثابت U_LIFETIME_BOUND;

    /**
     * Returns the locale's variant code.
     * @return      An alias to the code
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND;

    /**
     * Returns the programmatic name of the entire locale, with the language,
     * country and variant separated by underbars. If a field is missing, up
     * to two leading underbars will occur. Example: "en", "de_DE", "en_US_WIN",
     * "de__POSIX", "fr__MAC", "__MAC", "_MT", "_FR_EURO"
     * @return      A pointer to "name".
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getName() ثابت U_LIFETIME_BOUND;

    /**
     * Returns the programmatic name of the entire locale as getName() would return,
     * but without keywords.
     * @return      A pointer to "name".
     * @see getName
     * @stable ICU 2.8
     */
    U_COMMON_API ثابت حرف* get_بنیاد_Name() ثابت U_LIFETIME_BOUND;

    /**
     * Add the likely subtags for this Locale, per the algorithm described
     * in the following CLDR technical report:
     *
     *   http://www.unicode.org/reports/tr35/#Likely_Subtags
     *
     * If this Locale is already in the maximal form, or not valid, or there is
     * no data available for maximization, the Locale will be unchanged.
     *
     * For example, "sh" cannot be maximized, since there is no
     * reasonable maximization.
     *
     * Examples:
     *
     * "und_Zzzz" maximizes to "en_Latn_US"
     *
     * "en" maximizes to "en_Latn_US"
     *
     * "de" maximizes to "de_Latn_DE"
     *
     * "sr" maximizes to "sr_Cyrl_RS"
     *
     * "zh_Hani" maximizes to "zh_Hani_CN"
     *
     * @param status  error information if maximizing this Locale failed.
     *                If this Locale is not well-formed, the error code is
     *                U_ILLEGAL_ARGUMENT_ERROR.
     * @stable ICU 63
     */
    U_COMMON_API خالی جوڑو_Likely_Subtags(UError_رمز& حالت_2);

    /**
     * Minimize the subtags for this Locale, per the algorithm described
     * in the following CLDR technical report:
     *
     *   http://www.unicode.org/reports/tr35/#Likely_Subtags
     *
     * If this Locale is already in the minimal form, or not valid, or there is
     * no data available for minimization, the Locale will be unchanged.
     *
     * Since the minimization algorithm relies on proper maximization, see the
     * comments for addLikelySubtags for reasons why there might not be any
     * data.
     *
     * Examples:
     *
     * "en_Latn_US" minimizes to "en"
     *
     * "de_Latn_US" minimizes to "de"
     *
     * "sr_Cyrl_RS" minimizes to "sr"
     *
     * "zh_Hant_TW" minimizes to "zh_TW" (The region is preferred to the
     * script, and minimizing to "zh" would imply "zh_Hans_CN".)
     *
     * @param status  error information if maximizing this Locale failed.
     *                If this Locale is not well-formed, the error code is
     *                U_ILLEGAL_ARGUMENT_ERROR.
     * @stable ICU 63
     */
    U_COMMON_API خالی minimizeSubtags(UError_رمز& حالت_2);

    /**
     * Canonicalize the locale ID of this object according to CLDR.
     * @param status the status code
     * @stable ICU 67
     * @see createCanonical
     */
    U_COMMON_API خالی canonicalize(UError_رمز& حالت_2);

    /**
     * Gets the list of keywords for the specified locale.
     *
     * @param status the status code
     * @return pointer to StringEnumeration class, or nullptr if there are no keywords.
     * Client must dispose of it by calling delete.
     * @see getKeywords
     * @stable ICU 2.8
     */
    U_COMMON_API StringEnumeration* createKeywords(UError_رمز& حالت_2) ثابت;

    /**
     * Gets the list of Unicode keywords for the specified locale.
     *
     * @param status the status code
     * @return pointer to StringEnumeration class, or nullptr if there are no keywords.
     * Client must dispose of it by calling delete.
     * @see getUnicodeKeywords
     * @stable ICU 63
     */
    U_COMMON_API StringEnumeration* createUnicodeKeywords(UError_رمز& حالت_2) ثابت;

    /**
     * Gets the set of keywords for this Locale.
     *
     * A wrapper to call createKeywords() and write the resulting
     * keywords as standard strings (or compatible objects) into any kind of
     * container that can be written to by an STL style output iterator.
     *
     * @param iterator  an STL style output iterator to write the keywords to.
     * @param status    error information if creating set of keywords failed.
     * @stable ICU 63
     */
    سانچہ<typename StringClass, typename OutputIterator>
    اندرونی خالی getKeywords(OutputIterator iterator, UError_رمز& حالت_2) ثابت;

    /**
     * Gets the set of Unicode keywords for this Locale.
     *
     * A wrapper to call createUnicodeKeywords() and write the resulting
     * keywords as standard strings (or compatible objects) into any kind of
     * container that can be written to by an STL style output iterator.
     *
     * @param iterator  an STL style output iterator to write the keywords to.
     * @param status    error information if creating set of keywords failed.
     * @stable ICU 63
     */
    سانچہ<typename StringClass, typename OutputIterator>
    اندرونی خالی getUnicodeKeywords(OutputIterator iterator, UError_رمز& حالت_2) ثابت;

    /**
     * Gets the value for a keyword.
     *
     * This uses legacy keyword=value pairs, like "collation=phonebook".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName name of the keyword for which we want the value. Case insensitive.
     * @param buffer The buffer to receive the keyword value.
     * @param bufferCapacity The capacity of receiving buffer
     * @param status Returns any error information while performing this operation.
     * @return the length of the keyword value
     *
     * @stable ICU 2.8
     */
    U_COMMON_API int32_t get_Keyword_قدر(ثابت حرف* keywordName,
                                         حرف* بفر,
                                         int32_t بفر_استعداد,
                                         UError_رمز& حالت_2) ثابت;

    /**
     * Gets the value for a keyword.
     *
     * This uses legacy keyword=value pairs, like "collation=phonebook".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName  name of the keyword for which we want the value.
     * @param sink         the sink to receive the keyword value.
     * @param status       error information if getting the value failed.
     * @stable ICU 63
     */
    U_COMMON_API خالی get_Keyword_قدر(StringPiece keywordName, ByteSink& sink, UError_رمز& حالت_2) ثابت;

    /**
     * Gets the value for a keyword.
     *
     * This uses legacy keyword=value pairs, like "collation=phonebook".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName  name of the keyword for which we want the value.
     * @param status       error information if getting the value failed.
     * @return             the keyword value.
     * @stable ICU 63
     */
    سانچہ<typename StringClass>
    اندرونی StringClass get_Keyword_قدر(StringPiece keywordName, UError_رمز& حالت_2) ثابت;

    /**
     * Gets the Unicode value for a Unicode keyword.
     *
     * This uses Unicode key-value pairs, like "co-phonebk".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName  name of the keyword for which we want the value.
     * @param sink         the sink to receive the keyword value.
     * @param status       error information if getting the value failed.
     * @stable ICU 63
     */
    U_COMMON_API خالی get_Unicode_Keyword_قدر(StringPiece keywordName,
                                             ByteSink& sink,
                                             UError_رمز& حالت_2) ثابت;

    /**
     * Gets the Unicode value for a Unicode keyword.
     *
     * This uses Unicode key-value pairs, like "co-phonebk".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName  name of the keyword for which we want the value.
     * @param status       error information if getting the value failed.
     * @return             the keyword value.
     * @stable ICU 63
     */
    سانچہ<typename StringClass>
    اندرونی StringClass get_Unicode_Keyword_قدر(StringPiece keywordName, UError_رمز& حالت_2) ثابت;

    /**
     * Sets or removes the value for a keyword.
     *
     * For removing all keywords, use getBaseName(),
     * and construct a new Locale if it differs from getName().
     *
     * This uses legacy keyword=value pairs, like "collation=phonebook".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName name of the keyword to be set. Case insensitive.
     * @param keywordValue value of the keyword to be set. If 0-length or
     *  nullptr, will result in the keyword being removed. No error is given if
     *  that keyword does not exist.
     * @param status Returns any error information while performing this operation.
     *
     * @stable ICU 49
     */
    U_COMMON_API خالی سیٹ_Keyword_قدر(ثابت حرف* keywordName,
                                      ثابت حرف* keyword_قدر,
                                      UError_رمز& حالت_2) {
        سیٹ_Keyword_قدر(StringPiece{keywordName}, StringPiece{keyword_قدر}, حالت_2);
    }

    /**
     * Sets or removes the value for a keyword.
     *
     * For removing all keywords, use getBaseName(),
     * and construct a new Locale if it differs from getName().
     *
     * This uses legacy keyword=value pairs, like "collation=phonebook".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName name of the keyword to be set.
     * @param keywordValue value of the keyword to be set. If 0-length or
     *  nullptr, will result in the keyword being removed. No error is given if
     *  that keyword does not exist.
     * @param status Returns any error information while performing this operation.
     * @stable ICU 63
     */
    U_COMMON_API خالی سیٹ_Keyword_قدر(StringPiece keywordName,
                                      StringPiece keyword_قدر,
                                      UError_رمز& حالت_2);

    /**
     * Sets or removes the Unicode value for a Unicode keyword.
     *
     * For removing all keywords, use getBaseName(),
     * and construct a new Locale if it differs from getName().
     *
     * This uses Unicode key-value pairs, like "co-phonebk".
     *
     * ICU4C doesn't do automatic conversion between legacy and Unicode
     * keywords and values in getters and setters (as opposed to ICU4J).
     *
     * @param keywordName name of the keyword to be set.
     * @param keywordValue value of the keyword to be set. If 0-length or
     *  nullptr, will result in the keyword being removed. No error is given if
     *  that keyword does not exist.
     * @param status Returns any error information while performing this operation.
     * @stable ICU 63
     */
    U_COMMON_API خالی سیٹ_Unicode_Keyword_قدر(StringPiece keywordName,
                                             StringPiece keyword_قدر,
                                             UError_رمز& حالت_2);

    /**
     * returns the locale's three-letter language code, as specified
     * in ISO draft standard ISO-639-2.
     * @return      An alias to the code, or an empty string
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getISO3Language() ثابت;

    /**
     * Fills in "name" with the locale's three-letter ISO-3166 country code.
     * @return      An alias to the code, or an empty string
     * @stable ICU 2.0
     */
    U_COMMON_API ثابت حرف* getISO3Country() ثابت;

    /**
     * Returns the Windows LCID value corresponding to this locale.
     * This value is stored in the resource data for the locale as a one-to-four-digit
     * hexadecimal number.  If the resource is missing, in the wrong format, or
     * there is no Windows LCID value that corresponds to this locale, returns 0.
     * @stable ICU 2.0
     */
    U_COMMON_API uint32_t getLCID() ثابت;

    /**
     * Returns whether this locale's script is written right-to-left.
     * If there is no script subtag, then the likely script is used, see uloc_addLikelySubtags().
     * If no likely script is known, then false is returned.
     *
     * A script is right-to-left according to the CLDR script metadata
     * which corresponds to whether the script's letters have Bidi_Class=R or AL.
     *
     * Returns true for "ar" and "en-Hebr", false for "zh" and "fa-Cyrl".
     *
     * @return true if the locale's script is written right-to-left
     * @stable ICU 54
     */
    U_COMMON_API UBool is_دایاں_مقصد_بایاں() ثابت;

    /**
     * Fills in "dispLang" with the name of this locale's language in a format suitable for
     * user display in the default locale.  For example, if the locale's language code is
     * "fr" and the default locale's language code is "en", this function would set
     * dispLang to "French".
     * @param dispLang  Receives the language's display name.
     * @return          A reference to "dispLang".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayLanguage(UnicodeString& dispLang U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispLang" with the name of this locale's language in a format suitable for
     * user display in the locale specified by "displayLocale".  For example, if the locale's
     * language code is "en" and displayLocale's language code is "fr", this function would set
     * dispLang to "Anglais".
     * @param displayLocale  Specifies the locale to be used to display the name.  In other words,
     *                  if the locale's language code is "en", passing Locale::getFrench() for
     *                  displayLocale would result in "Anglais", while passing Locale::getGerman()
     *                  for displayLocale would result in "Englisch".
     * @param dispLang  Receives the language's display name.
     * @return          A reference to "dispLang".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayLanguage(ثابت Locale& displayLocale,
                                                   UnicodeString& dispLang U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispScript" with the name of this locale's script in a format suitable
     * for user display in the default locale.  For example, if the locale's script code
     * is "LATN" and the default locale's language code is "en", this function would set
     * dispScript to "Latin".
     * @param dispScript    Receives the scripts's display name.
     * @return              A reference to "dispScript".
     * @stable ICU 2.8
     */
    U_COMMON_API UnicodeString& getDisplayScript(UnicodeString& dispScript U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispScript" with the name of this locale's country in a format suitable
     * for user display in the locale specified by "displayLocale".  For example, if the locale's
     * script code is "LATN" and displayLocale's language code is "en", this function would set
     * dispScript to "Latin".
     * @param displayLocale      Specifies the locale to be used to display the name.  In other
     *                      words, if the locale's script code is "LATN", passing
     *                      Locale::getFrench() for displayLocale would result in "", while
     *                      passing Locale::getGerman() for displayLocale would result in
     *                      "".
     * @param dispScript    Receives the scripts's display name.
     * @return              A reference to "dispScript".
     * @stable ICU 2.8
     */
    U_COMMON_API UnicodeString& getDisplayScript(ثابت Locale& displayLocale,
                                                 UnicodeString& dispScript U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispCountry" with the name of this locale's country in a format suitable
     * for user display in the default locale.  For example, if the locale's country code
     * is "FR" and the default locale's language code is "en", this function would set
     * dispCountry to "France".
     * @param dispCountry   Receives the country's display name.
     * @return              A reference to "dispCountry".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString&
    getDisplayCountry(UnicodeString& dispCountry U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispCountry" with the name of this locale's country in a format suitable
     * for user display in the locale specified by "displayLocale".  For example, if the locale's
     * country code is "US" and displayLocale's language code is "fr", this function would set
     * dispCountry to "&Eacute;tats-Unis".
     * @param displayLocale      Specifies the locale to be used to display the name.  In other
     *                      words, if the locale's country code is "US", passing
     *                      Locale::getFrench() for displayLocale would result in "&Eacute;tats-Unis", while
     *                      passing Locale::getGerman() for displayLocale would result in
     *                      "Vereinigte Staaten".
     * @param dispCountry   Receives the country's display name.
     * @return              A reference to "dispCountry".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString&
    getDisplayCountry(ثابت Locale& displayLocale,
                      UnicodeString& dispCountry U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispVar" with the name of this locale's variant code in a format suitable
     * for user display in the default locale.
     * @param dispVar   Receives the variant's name.
     * @return          A reference to "dispVar".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayVariant(UnicodeString& dispVar U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "dispVar" with the name of this locale's variant code in a format
     * suitable for user display in the locale specified by "displayLocale".
     * @param displayLocale  Specifies the locale to be used to display the name.
     * @param dispVar   Receives the variant's display name.
     * @return          A reference to "dispVar".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayVariant(ثابت Locale& displayLocale,
                                                  UnicodeString& dispVar U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "name" with the name of this locale in a format suitable for user display
     * in the default locale.  This function uses getDisplayLanguage(), getDisplayCountry(),
     * and getDisplayVariant() to do its work, and outputs the display name in the format
     * "language (country[,variant])".  For example, if the default locale is en_US, then
     * fr_FR's display name would be "French (France)", and es_MX_Traditional's display name
     * would be "Spanish (Mexico,Traditional)".
     * @param name  Receives the locale's display name.
     * @return      A reference to "name".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayName(UnicodeString& name U_LIFETIME_BOUND) ثابت;

    /**
     * Fills in "name" with the name of this locale in a format suitable for user display
     * in the locale specified by "displayLocale".  This function uses getDisplayLanguage(),
     * getDisplayCountry(), and getDisplayVariant() to do its work, and outputs the display
     * name in the format "language (country[,variant])".  For example, if displayLocale is
     * fr_FR, then en_US's display name would be "Anglais (&Eacute;tats-Unis)", and no_NO_NY's
     * display name would be "norv&eacute;gien (Norv&egrave;ge,NY)".
     * @param displayLocale  Specifies the locale to be used to display the name.
     * @param name      Receives the locale's display name.
     * @return          A reference to "name".
     * @stable ICU 2.0
     */
    U_COMMON_API UnicodeString& getDisplayName(ثابت Locale& displayLocale,
                                               UnicodeString& name U_LIFETIME_BOUND) ثابت;

    /**
     * Generates a hash code for the locale.
     * @stable ICU 2.0
     */
    U_COMMON_API int32_t hash_رمز() ثابت;

    /**
     * Sets the locale to bogus
     * A bogus locale represents a non-existing locale associated
     * with services that can be instantiated from non-locale data
     * in addition to locale (for example, collation can be
     * instantiated from a locale and from a rule set).
     * @stable ICU 2.1
     */
    U_COMMON_API خالی سیٹ_مقصد_Bogus();

    /**
     * Gets the bogus state. Locale object can be bogus if it doesn't exist
     * @return false if it is a real locale, true if it is a bogus locale
     * @stable ICU 2.1
     */
    U_COMMON_API اندرونی UBool isBogus() ثابت;

    /**
     * Returns a list of all installed locales.
     * @param count Receives the number of locales in the list.
     * @return      A pointer to an array of Locale objects.  This array is the list
     *              of all locales with installed resource files.  The called does NOT
     *              get ownership of this list, and must NOT delete it.
     * @stable ICU 2.0
     */
    U_COMMON_API ساکن ثابت Locale* U_EXPORT2 get_دستیاب_Locales(int32_t& گنتی);

    /**
     * Gets a list of all available 2-letter country codes defined in ISO 3166.  This is a
     * pointer to an array of pointers to arrays of char.  All of these pointers are
     * owned by ICU-- do not delete them, and do not write through them.  The array is
     * terminated with a null pointer.
     * @return a list of all available country codes
     * @stable ICU 2.0
     */
    U_COMMON_API ساکن ثابت حرف* ثابت* U_EXPORT2 getISOCountries();

    /**
     * Returns a list of all unique language codes defined in ISO 639.
     * They can be 2 or 3 letter codes, as defined by
     * <a href="https://www.ietf.org/rfc/bcp/bcp47.html#section-2.2.1">
     * BCP 47, section 2.2.1</a>. This is a pointer
     * to an array of pointers to arrays of char.  All of these pointers are owned
     * by ICU-- do not delete them, and do not write through them.  The array is
     * terminated with a null pointer.
     * @return a list of all available language codes
     * @stable ICU 2.0
     */
    U_COMMON_API ساکن ثابت حرف* ثابت* U_EXPORT2 getISOLanguages();

    /**
     * ICU "poor man's RTTI", returns a UClassID for this class.
     *
     * @stable ICU 2.2
     */
    U_COMMON_API ساکن UClassID U_EXPORT2 getStaticClassID();

    /**
     * ICU "poor man's RTTI", returns a UClassID for the actual class.
     *
     * @stable ICU 2.2
     */
    U_COMMON_API مجازی UClassID getDynamicClassID() ثابت override;

    /**
     * A Locale iterator interface similar to a Java Iterator<Locale>.
     * @stable ICU 65
     */
    class U_COMMON_API Iterator /* not : public UObject because this is an interface/mixin class */ {
    عوامی:
        /** @stable ICU 65 */
        مجازی ~Iterator();

        /**
         * @return true if next() can be called again.
         * @stable ICU 65
         */
        مجازی UBool has_اگلا() ثابت = 0;

        /**
         * @return the next locale.
         * @stable ICU 65
         */
        مجازی ثابت Locale &اگلا() = 0;
    };

    /**
     * A generic Locale iterator implementation over Locale input iterators.
     * @stable ICU 65
     */
    سانچہ<typename Iter>
    class RangeIterator : عوامی Iterator, عوامی UMemory {
    عوامی:
        /**
         * Constructs an iterator from a begin/end range.
         * Each of the iterator parameter values must be an
         * input iterator whose value is convertible to const Locale &.
         *
         * @param begin Start of range.
         * @param end Exclusive end of range.
         * @stable ICU 65
         */
        RangeIterator(Iter begin, Iter انتہا) : it_(begin), انتہا_3(انتہا) {}

        /**
         * @return true if next() can be called again.
         * @stable ICU 65
         */
        UBool has_اگلا() ثابت override { واپس it_ != انتہا_3; }

        /**
         * @return the next locale.
         * @stable ICU 65
         */
        ثابت Locale &اگلا() override { واپس *it_++; }

    نجی:
        Iter it_;
        ثابت Iter انتہا_3;
    };

    /**
     * A generic Locale iterator implementation over Locale input iterators.
     * Calls the converter to convert each *begin to a const Locale &.
     * @stable ICU 65
     */
    سانچہ<typename Iter, typename Conv>
    class ConvertingIterator : عوامی Iterator, عوامی UMemory {
    عوامی:
        /**
         * Constructs an iterator from a begin/end range.
         * Each of the iterator parameter values must be an
         * input iterator whose value the converter converts to const Locale &.
         *
         * @param begin Start of range.
         * @param end Exclusive end of range.
         * @param converter Converter from *begin to const Locale & or compatible.
         * @stable ICU 65
         */
        ConvertingIterator(Iter begin, Iter انتہا, Conv converter) :
                it_(begin), انتہا_3(انتہا), converter_(converter) {}

        /**
         * @return true if next() can be called again.
         * @stable ICU 65
         */
        UBool has_اگلا() ثابت override { واپس it_ != انتہا_3; }

        /**
         * @return the next locale.
         * @stable ICU 65
         */
        ثابت Locale &اگلا() override { واپس converter_(*it_++); }

    نجی:
        Iter it_;
        ثابت Iter انتہا_3;
        Conv converter_;
    };

محفوظ: /* only protected for testing purposes. DO NOT USE. */
#اگر_غیر_تعریف_شدہ U_HIDE_اندرونی_API
    /**
     * Set this from a single POSIX style locale string.
     * @internal
     */
    U_COMMON_API خالی سیٹ_From_POSIXID(ثابت حرف* posixID);
    /**
     * Minimize the subtags for this Locale, per the algorithm described
     * @param favorScript favor to keep script if true, to keep region if false.
     * @param status  error information if maximizing this Locale failed.
     *                If this Locale is not well-formed, the error code is
     *                U_ILLEGAL_ARGUMENT_ERROR.
     * @internal
     */
    U_COMMON_API خالی minimizeSubtags(bool favorScript, UError_رمز& حالت_2);
#اگر_ختم  /* U_HIDE_INTERNAL_API */

نجی:
    /**
     * Initialize the locale object with a new name.
     * Was deprecated - used in implementation - moved internal
     *
     * @param cLocaleID The new locale name.
     * @param canonicalize whether to call uloc_canonicalize on cLocaleID
     */
    Locale& init(ثابت حرف* localeID, UBool canonicalize) U_LIFETIME_BOUND;
    Locale& init(StringPiece localeID, UBool canonicalize) U_LIFETIME_BOUND;

    /*
     * Internal constructor to allow construction of a locale object with
     *   NO side effects.   (Default constructor tries to get
     *   the default locale.)
     */
    شمار ELocaleType : uint8_t {
        eBOGUS,
        eNEST,
        e_ڈھیر,
    };
    Locale(ELocaleType);

    /**
     * Initialize the locale cache for commonly used locales
     */
    ساکن Locale* getLocaleCache();

    اتحاد Payload;
    ساخت Nest;
    ساخت ڈھیر_2;

    /**
     * Locale data that can be nested directly within the union Payload object.
     */
    ساخت Nest {
        ساکن constexpr size_t سائز_3 = 32;

        ELocaleType type = eNEST;
        حرف language[4];
        حرف script[5];
        حرف region[4];
        uint8_t variantBegin;
        حرف بنیاد_Name[سائز_3 -
                      حجم type -
                      حجم language -
                      حجم script -
                      حجم region -
                      حجم variantBegin];

        ثابت حرف* getLanguage() ثابت U_LIFETIME_BOUND { واپس language; }
        ثابت حرف* getScript() ثابت U_LIFETIME_BOUND { واپس script; }
        ثابت حرف* getRegion() ثابت U_LIFETIME_BOUND { واپس region; }
        ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND {
            واپس variantBegin == 0 ? "" : get_بنیاد_Name() + variantBegin;
        }
        ثابت حرف* get_بنیاد_Name() ثابت U_LIFETIME_BOUND { واپس بنیاد_Name; }

        // Doesn't inherit from UMemory, shouldn't be heap allocated.
        ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
        ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;

        Nest() : language{'\0'}, script{'\0'}, region{'\0'}, variantBegin{0}, بنیاد_Name{'\0'} {}

        خالی init(std::string_view language,
                  std::string_view script,
                  std::string_view region,
                  uint8_t variantBegin);

        ساکن bool سماتا(int32_t لمبائی,
                         std::string_view language,
                         std::string_view script,
                         std::string_view region) {
            واپس لمبائی < static_cast<int32_t>(حجم Nest::بنیاد_Name) &&
                   language.سائز_2() < حجم Nest::language &&
                   script.سائز_2() < حجم Nest::script &&
                   region.سائز_2() < حجم Nest::region;
        }

      نجی:
        دوست اتحاد Payload;
        Nest(ڈھیر_2&& ڈھیر_3, uint8_t variantBegin);
    };
    static_assert(حجم(Nest) == Nest::سائز_3);

    /**
     * Locale data that needs to be heap allocated in the union Payload object.
     */
    ساخت ڈھیر_2 {
        ساخت Alloc;

        ELocaleType type;
        حرف language[ULOC_LANG_استعداد];
        حرف script[ULOC_SCRIPT_استعداد];
        حرف region[ULOC_COUNTRY_استعداد];
        Alloc* ptr;

        ثابت حرف* getLanguage() ثابت U_LIFETIME_BOUND { واپس language; }
        ثابت حرف* getScript() ثابت U_LIFETIME_BOUND { واپس script; }
        ثابت حرف* getRegion() ثابت U_LIFETIME_BOUND { واپس region; }
        ثابت حرف* getVariant() ثابت U_LIFETIME_BOUND;
        ثابت حرف* get_بھرا_Name() ثابت U_LIFETIME_BOUND;
        ثابت حرف* get_بنیاد_Name() ثابت U_LIFETIME_BOUND;

        // Doesn't inherit from UMemory, shouldn't be heap allocated.
        ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
        ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;

        ڈھیر_2(std::string_view language,
             std::string_view script,
             std::string_view region,
             int32_t variantBegin);
        ~ڈھیر_2();

        ڈھیر_2& عامل=(ثابت ڈھیر_2& other) U_LIFETIME_BOUND;
        ڈھیر_2& عامل=(ڈھیر_2&& other) noexcept U_LIFETIME_BOUND;
    };
    static_assert(حجم(ڈھیر_2) <= حجم(Nest));

    /**
     * This is kind of std::variant but customized to not waste any space on the
     * discriminator or on any padding, and to copy any heap allocated object.
     */
    اتحاد Payload {
      نجی:
        Nest nest;
        ڈھیر_2 ڈھیر_3;
        ELocaleType type;

        خالی copy(ثابت Payload& other);
        خالی move(Payload&& other) noexcept;

      عوامی:
        // Doesn't inherit from UMemory, shouldn't be heap allocated.
        ساکن خالی* U_EXPORT2 عامل new(size_t) noexcept = delete;
        ساکن خالی* U_EXPORT2 عامل new[](size_t) noexcept = delete;

        Payload() : type{eBOGUS} {}
        ~Payload();

        Payload(ثابت Payload& other);
        Payload(Payload&& other) noexcept;

        Payload& عامل=(ثابت Payload& other) U_LIFETIME_BOUND;
        Payload& عامل=(Payload&& other) noexcept U_LIFETIME_BOUND;

        خالی سیٹ_مقصد_Bogus();
        bool isBogus() ثابت { واپس type == eBOGUS; }

        سانچہ <typename T, typename... دلائل> T& emplace(دلائل&&... دلائل_2) U_LIFETIME_BOUND;

        سانچہ <typename T> T* get() U_LIFETIME_BOUND;

        سانچہ <typename BogusFn, typename NestFn, typename ڈھیر_Fn, typename... دلائل>
        خودکار دورہ(BogusFn bogusFn, NestFn nestFn, ڈھیر_Fn ڈھیر_Fn_2, دلائل... دلائل_2) ثابت;
    } payload;

    /**
     * Call a field getter function on either Nest or Heap in payload.
     * (This is kind of std::visit but simpler and without exceptions.)
     *
     * @tparam NEST Pointer to the Nest getter function.
     * @tparam HEAP Pointer to the Heap getter function.
     * @return the result from the getter, or the empty string if isBogus().
     */
    سانچہ <ثابت حرف* (Nest::*ثابت NEST)() ثابت,
              ثابت حرف* (ڈھیر_2::*ثابت ڈھیر_4)() ثابت>
    ثابت حرف* get_میدان() ثابت U_LIFETIME_BOUND;

    ساکن ثابت Locale &getLocale(صحیح locid);

    /**
     * A friend to allow the default locale to be set by either the C or C++ API.
     */
    دوست Locale *locale_سیٹ_default_اندرونی(ثابت حرف *, UError_رمز& حالت_2);

    /**
     */
    دوست خالی U_CALLCONV locale_دستیاب_init();
};

U_COMMON_API اندرونی bool
Locale::عامل!=(ثابت    Locale&     other) ثابت
{
    واپس !عامل==(other);
}

سانچہ<typename StringClass> اندرونی StringClass
Locale::مقصد_Language_لیبل(UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) { واپس {}; }
    StringClass نتیجہ;
    StringByteSink<StringClass> sink(&نتیجہ);
    مقصد_Language_لیبل(sink, حالت_2);
    واپس نتیجہ;
}

سانچہ<typename StringClass, typename OutputIterator> اندرونی خالی
Locale::getKeywords(OutputIterator iterator, UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) { واپس; }
    Local_اشارہ_گر<StringEnumeration> کلیدیں(createKeywords(حالت_2));
    اگر (U_FAILURE(حالت_2) || کلیدیں.isNull()) {
        واپس;
    }
    برائے (;;) {
        int32_t نتیجہ_لمبائی;
        ثابت حرف* بفر = کلیدیں->اگلا(&نتیجہ_لمبائی, حالت_2);
        اگر (U_FAILURE(حالت_2) || بفر == nullptr) {
            واپس;
        }
        *iterator++ = StringClass(بفر, نتیجہ_لمبائی);
    }
}

سانچہ<typename StringClass, typename OutputIterator> اندرونی خالی
Locale::getUnicodeKeywords(OutputIterator iterator, UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) { واپس; }
    Local_اشارہ_گر<StringEnumeration> کلیدیں(createUnicodeKeywords(حالت_2));
    اگر (U_FAILURE(حالت_2) || کلیدیں.isNull()) {
        واپس;
    }
    برائے (;;) {
        int32_t نتیجہ_لمبائی;
        ثابت حرف* بفر = کلیدیں->اگلا(&نتیجہ_لمبائی, حالت_2);
        اگر (U_FAILURE(حالت_2) || بفر == nullptr) {
            واپس;
        }
        *iterator++ = StringClass(بفر, نتیجہ_لمبائی);
    }
}

سانچہ<typename StringClass> اندرونی StringClass
Locale::get_Keyword_قدر(StringPiece keywordName, UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) { واپس {}; }
    StringClass نتیجہ;
    StringByteSink<StringClass> sink(&نتیجہ);
    get_Keyword_قدر(keywordName, sink, حالت_2);
    واپس نتیجہ;
}

سانچہ<typename StringClass> اندرونی StringClass
Locale::get_Unicode_Keyword_قدر(StringPiece keywordName, UError_رمز& حالت_2) ثابت
{
    اگر (U_FAILURE(حالت_2)) { واپس {}; }
    StringClass نتیجہ;
    StringByteSink<StringClass> sink(&نتیجہ);
    get_Unicode_Keyword_قدر(keywordName, sink, حالت_2);
    واپس نتیجہ;
}

U_COMMON_API اندرونی UBool
Locale::isBogus() ثابت {
    واپس payload.isBogus();
}

U_NAMESPACE_انتہا

#اگر_ختم /* U_SHOW_CPLUSPLUS_API */

#اگر_ختم
