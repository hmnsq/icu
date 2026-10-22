// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.h
// created: 2019may08 Markus W. Scherer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __LOCALEMATCHER_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __LOCALEMATCHER_H__

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ <optional>

#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

/**
 * \file
 * \brief C++ API: Locale matcher: User's desired locales vs. application's supported locales.
 */

/**
 * Builder option for whether the language subtag or the script subtag is most important.
 *
 * @see LocaleMatcher::Builder#setFavorSubtag(ULocMatchFavorSubtag)
 * @stable ICU 65
 */
ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Favor_Subtag {
    /**
     * Language differences are most important, then script differences, then region differences.
     * (This is the default behavior.)
     *
     * @stable ICU 65
     */
    ULOCMATCH_FAVOR_LANGUAGE,
    /**
     * Makes script differences matter relatively more than language differences.
     *
     * @stable ICU 65
     */
    ULOCMATCH_FAVOR_SCRIPT
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Favor_Subtag ULoc_ಹೊಂದಿಕೆ_Favor_Subtag;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Builder option for whether all desired locales are treated equally or
 * earlier ones are preferred.
 *
 * @see LocaleMatcher::Builder#setDemotionPerDesiredLocale(ULocMatchDemotion)
 * @stable ICU 65
 */
ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Demotion {
    /**
     * All desired locales are treated equally.
     *
     * @stable ICU 65
     */
    ULOCMATCH_DEMOTION_NONE,
    /**
     * Earlier desired locales are preferred.
     *
     * <p>From each desired locale to the next,
     * the distance to any supported locale is increased by an additional amount
     * which is at least as large as most region mismatches.
     * A later desired locale has to have a better match with some supported locale
     * due to more than merely having the same region subtag.
     *
     * <p>For example: <code>Supported={en, sv}  desired=[en-GB, sv]</code>
     * yields <code>Result(en-GB, en)</code> because
     * with the demotion of sv its perfect match is no better than
     * the region distance between the earlier desired locale en-GB and en=en-US.
     *
     * <p>Notes:
     * <ul>
     *   <li>In some cases, language and/or script differences can be as small as
     *       the typical region difference. (Example: sr-Latn vs. sr-Cyrl)
     *   <li>It is possible for certain region differences to be larger than usual,
     *       and larger than the demotion.
     *       (As of CLDR 35 there is no such case, but
     *        this is possible in future versions of the data.)
     * </ul>
     *
     * @stable ICU 65
     */
    ULOCMATCH_DEMOTION_REGION
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Demotion ULoc_ಹೊಂದಿಕೆ_Demotion;
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Builder option for whether to include or ignore one-way (fallback) match data.
 * The LocaleMatcher uses CLDR languageMatch data which includes fallback (oneway=true) entries.
 * Sometimes it is desirable to ignore those.
 *
 * <p>For example, consider a web application with the UI in a given language,
 * with a link to another, related web app.
 * The link should include the UI language, and the target server may also use
 * the client’s Accept-Language header data.
 * The target server has its own list of supported languages.
 * One may want to favor UI language consistency, that is,
 * if there is a decent match for the original UI language, we want to use it,
 * but not if it is merely a fallback.
 *
 * @see LocaleMatcher::Builder#setDirection(ULocMatchDirection)
 * @stable ICU 67
 */
ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Direction {
    /**
     * Locale matching includes one-way matches such as Breton→French. (default)
     *
     * @stable ICU 67
     */
    ULOCMATCH_DIRECTION_WITH_ONE_WAY,
    /**
     * Locale matching limited to two-way matches including e.g. Danish↔Norwegian
     * but ignoring one-way matches.
     *
     * @stable ICU 67
     */
    ULOCMATCH_DIRECTION_ONLY_TWO_WAY
};
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Direction ULoc_ಹೊಂದಿಕೆ_Direction;
#ಆದರೆ_ಅಂತ್ಯ

ರಚನೆ UHashtable;

U_NAMESPACE_BEGIN

ರಚನೆ LSR;

class LikelySubtags;
class Locale_ದೂರ;
class LocaleLsrIterator;
class UVector;

/**
 * Immutable class that picks the best match between a user's desired locales and
 * an application's supported locales.
 * Movable but not copyable.
 *
 * <p>Example:
 * <pre>
 * UErrorCode errorCode = U_ZERO_ERROR;
 * LocaleMatcher matcher = LocaleMatcher::Builder().setSupportedLocales("fr, en-GB, en").build(errorCode);
 * Locale *bestSupported = matcher.getBestLocale(Locale.US, errorCode);  // "en"
 * </pre>
 *
 * <p>A matcher takes into account when languages are close to one another,
 * such as Danish and Norwegian,
 * and when regional variants are close, like en-GB and en-AU as opposed to en-US.
 *
 * <p>If there are multiple supported locales with the same (language, script, region)
 * likely subtags, then the current implementation returns the first of those locales.
 * It ignores variant subtags (except for pseudolocale variants) and extensions.
 * This may change in future versions.
 *
 * <p>For example, the current implementation does not distinguish between
 * de, de-DE, de-Latn, de-1901, de-u-co-phonebk.
 *
 * <p>If you prefer one equivalent locale over another, then provide only the preferred one,
 * or place it earlier in the list of supported locales.
 *
 * <p>Otherwise, the order of supported locales may have no effect on the best-match results.
 * The current implementation compares each desired locale with supported locales
 * in the following order:
 * 1. Default locale, if supported;
 * 2. CLDR "paradigm locales" like en-GB and es-419;
 * 3. other supported locales.
 * This may change in future versions.
 *
 * <p>Often a product will just need one matcher instance, built with the languages
 * that it supports. However, it may want multiple instances with different
 * default languages based on additional information, such as the domain.
 *
 * <p>This class is not intended for public subclassing.
 *
 * @stable ICU 65
 */
class U_COMMON_API LocaleMatcher : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    /**
     * Data for the best-matching pair of a desired and a supported locale.
     * Movable but not copyable.
     *
     * @stable ICU 65
     */
    class U_COMMON_API ಫಲಿತಾಂಶ_2 : ಸಾರ್ವಜನಿಕ UMemory {
    ಸಾರ್ವಜನಿಕ:
        /**
         * Move constructor; might modify the source.
         * This object will have the same contents that the source object had.
         *
         * @param src Result to move contents from.
         * @stable ICU 65
         */
        ಫಲಿತಾಂಶ_2(ಫಲಿತಾಂಶ_2 &&src) noexcept;

        /**
         * Destructor.
         *
         * @stable ICU 65
         */
        ~ಫಲಿತಾಂಶ_2();

        /**
         * Move assignment; might modify the source.
         * This object will have the same contents that the source object had.
         *
         * @param src Result to move contents from.
         * @stable ICU 65
         */
        ಫಲಿತಾಂಶ_2& ಕ್ರಿಯಾಕಾರಕ=(ಫಲಿತಾಂಶ_2&& src) noexcept U_LIFETIME_BOUND;

        /**
         * Returns the best-matching desired locale.
         * nullptr if the list of desired locales is empty or if none matched well enough.
         *
         * @return the best-matching desired locale, or nullptr.
         * @stable ICU 65
         */
        ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ Locale* getDesiredLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ desiredLocale; }

        /**
         * Returns the best-matching supported locale.
         * If none matched well enough, this is the default locale.
         * The default locale is nullptr if Builder::setNoDefaultLocale() was called,
         * or if the list of supported locales is empty and no explicit default locale is set.
         *
         * @return the best-matching supported locale, or nullptr.
         * @stable ICU 65
         */
        ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ Locale* getSupportedLocale() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ supportedLocale; }

        /**
         * Returns the index of the best-matching desired locale in the input Iterable order.
         * -1 if the list of desired locales is empty or if none matched well enough.
         *
         * @return the index of the best-matching desired locale, or -1.
         * @stable ICU 65
         */
        ಸಾಲೊಳಗೆ int32_t get_Desired_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ desired_ಸೂಚ್ಯಂಕ; }

        /**
         * Returns the index of the best-matching supported locale in the
         * constructor’s or builder’s input order (“set” Collection plus “added” locales).
         * If the matcher was built from a locale list string, then the iteration order is that
         * of a LocalePriorityList built from the same string.
         * -1 if the list of supported locales is empty or if none matched well enough.
         *
         * @return the index of the best-matching supported locale, or -1.
         * @stable ICU 65
         */
        ಸಾಲೊಳಗೆ int32_t get_Supported_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ supported_ಸೂಚ್ಯಂಕ; }

        /**
         * Takes the best-matching supported locale and adds relevant fields of the
         * best-matching desired locale, such as the -t- and -u- extensions.
         * May replace some fields of the supported locale.
         * The result is the locale that should be used for date and number formatting, collation, etc.
         * Returns the root locale if getSupportedLocale() returns nullptr.
         *
         * <p>Example: desired=ar-SA-u-nu-latn, supported=ar-EG, resolved locale=ar-SA-u-nu-latn
         *
         * @return a locale combining the best-matching desired and supported locales.
         * @stable ICU 65
         */
        Locale makeResolvedLocale(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
        ಫಲಿತಾಂಶ_2(ಸ್ಥಿರಾಂಕ Locale *desired, ಸ್ಥಿರಾಂಕ Locale *supported,
               int32_t des_ಸೂಚ್ಯಂಕ, int32_t supp_ಸೂಚ್ಯಂಕ, UBool owned) :
                desiredLocale(desired), supportedLocale(supported),
                desired_ಸೂಚ್ಯಂಕ(des_ಸೂಚ್ಯಂಕ), supported_ಸೂಚ್ಯಂಕ(supp_ಸೂಚ್ಯಂಕ),
                desiredIsOwned(owned) {}

        ಫಲಿತಾಂಶ_2(ಸ್ಥಿರಾಂಕ ಫಲಿತಾಂಶ_2 &other) = delete;
        ಫಲಿತಾಂಶ_2 &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಫಲಿತಾಂಶ_2 &other) = delete;

        ಸ್ಥಿರಾಂಕ Locale *desiredLocale;
        ಸ್ಥಿರಾಂಕ Locale *supportedLocale;
        int32_t desired_ಸೂಚ್ಯಂಕ;
        int32_t supported_ಸೂಚ್ಯಂಕ;
        UBool desiredIsOwned;

        ಮಿತ್ರ class LocaleMatcher;
    };

    /**
     * LocaleMatcher builder.
     * Movable but not copyable.
     *
     * @stable ICU 65
     */
    class U_COMMON_API Builder : ಸಾರ್ವಜನಿಕ UMemory {
    ಸಾರ್ವಜನಿಕ:
        /**
         * Constructs a builder used in chaining parameters for building a LocaleMatcher.
         *
         * @stable ICU 65
         */
        Builder() {}

        /**
         * Move constructor; might modify the source.
         * This builder will have the same contents that the source builder had.
         *
         * @param src Builder to move contents from.
         * @stable ICU 65
         */
        Builder(Builder &&src) noexcept;

        /**
         * Destructor.
         *
         * @stable ICU 65
         */
        ~Builder();

        /**
         * Move assignment; might modify the source.
         * This builder will have the same contents that the source builder had.
         *
         * @param src Builder to move contents from.
         * @stable ICU 65
         */
        Builder& ಕ್ರಿಯಾಕಾರಕ=(Builder&& src) noexcept U_LIFETIME_BOUND;

        /**
         * Parses an Accept-Language string
         * (<a href="https://tools.ietf.org/html/rfc2616#section-14.4">RFC 2616 Section 14.4</a>),
         * such as "af, en, fr;q=0.9", and sets the supported locales accordingly.
         * Allows whitespace in more places but does not allow "*".
         * Clears any previously set/added supported locales first.
         *
         * @param locales the Accept-Language string of locales to set
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಗಣ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND;

        /**
         * Copies the supported locales, preserving iteration order.
         * Clears any previously set/added supported locales first.
         * Duplicates are allowed, and are not removed.
         *
         * @param locales the list of locale
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಗಣ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND;

        /**
         * Copies the supported locales from the begin/end range, preserving iteration order.
         * Clears any previously set/added supported locales first.
         * Duplicates are allowed, and are not removed.
         *
         * Each of the iterator parameter values must be an
         * input iterator whose value is convertible to const Locale &.
         *
         * @param begin Start of range.
         * @param end Exclusive end of range.
         * @return this Builder object
         * @stable ICU 65
         */
        ಮಾದರಿ <typename Iter>
        Builder& ಗಣ_Supported_Locales(Iter begin, Iter ಕೊನೆ) U_LIFETIME_BOUND {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *this; }
            clearSupportedLocales();
            ಆಗ (begin != ಕೊನೆ) {
                ಕೂಡಿಸು_Supported_Locale(*begin++);
            }
            ಹಿಂತಿರುಗಿ *this;
        }

        /**
         * Copies the supported locales from the begin/end range, preserving iteration order.
         * Calls the converter to convert each *begin to a Locale or const Locale &.
         * Clears any previously set/added supported locales first.
         * Duplicates are allowed, and are not removed.
         *
         * Each of the iterator parameter values must be an
         * input iterator whose value is convertible to const Locale &.
         *
         * @param begin Start of range.
         * @param end Exclusive end of range.
         * @param converter Converter from *begin to const Locale & or compatible.
         * @return this Builder object
         * @stable ICU 65
         */
        ಮಾದರಿ <typename Iter, typename Conv>
        Builder& ಗಣ_Supported_Locales_ಮೂಲಕ_Converter(Iter begin, Iter ಕೊನೆ,
                                                 Conv converter) U_LIFETIME_BOUND {
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *this; }
            clearSupportedLocales();
            ಆಗ (begin != ಕೊನೆ) {
                ಕೂಡಿಸು_Supported_Locale(converter(*begin++));
            }
            ಹಿಂತಿರುಗಿ *this;
        }

        /**
         * Adds another supported locale.
         * Duplicates are allowed, and are not removed.
         *
         * @param locale another locale
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಕೂಡಿಸು_Supported_Locale(ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND;

        /**
         * Sets no default locale.
         * There will be no explicit or implicit default locale.
         * If there is no good match, then the matcher will return nullptr for the
         * best supported locale.
         *
         * @stable ICU 68
         */
        Builder& ಗಣ_No_Default_Locale() U_LIFETIME_BOUND;

        /**
         * Sets the default locale; if nullptr, or if it is not set explicitly,
         * then the first supported locale is used as the default locale.
         * There is no default locale at all (nullptr will be returned instead)
         * if setNoDefaultLocale() is called.
         *
         * @param defaultLocale the default locale (will be copied)
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಗಣ_Default_Locale(ಸ್ಥಿರಾಂಕ Locale* defaultLocale) U_LIFETIME_BOUND;

        /**
         * If ULOCMATCH_FAVOR_SCRIPT, then the language differences are smaller than script
         * differences.
         * This is used in situations (such as maps) where
         * it is better to fall back to the same script than a similar language.
         *
         * @param subtag the subtag to favor
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಗಣ_Favor_Subtag(ULoc_ಹೊಂದಿಕೆ_Favor_Subtag subtag) U_LIFETIME_BOUND;

        /**
         * Option for whether all desired locales are treated equally or
         * earlier ones are preferred (this is the default).
         *
         * @param demotion the demotion per desired locale to set.
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& ಗಣ_Demotion_ಪ್ರತಿ_Desired_Locale(ULoc_ಹೊಂದಿಕೆ_Demotion demotion) U_LIFETIME_BOUND;

        /**
         * Option for whether to include or ignore one-way (fallback) match data.
         * By default, they are included.
         *
         * @param matchDirection the match direction to set.
         * @return this Builder object
         * @stable ICU 67
         */
        Builder& ಗಣ_Direction(ULoc_ಹೊಂದಿಕೆ_Direction ಹೊಂದಿಕೆ_Direction) U_LIFETIME_BOUND {
            ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ_2)) {
                direction_ = ಹೊಂದಿಕೆ_Direction;
            }
            ಹಿಂತಿರುಗಿ *this;
        }

        /**
         * Sets the maximum distance for an acceptable match.
         * The matcher will return a match for a pair of locales only if
         * they match at least as well as the pair given here.
         *
         * For example, setMaxDistance(en-US, en-GB) limits matches to ones where the
         * (desired, support) locales have a distance no greater than a region subtag difference.
         * This is much stricter than the CLDR default.
         *
         * The details of locale matching are subject to changes in
         * CLDR data and in the algorithm.
         * Specifying a maximum distance in relative terms via a sample pair of locales
         * insulates from changes that affect all distance metrics similarly,
         * but some changes will necessarily affect relative distances between
         * different pairs of locales.
         *
         * @param desired the desired locale for distance comparison.
         * @param supported the supported locale for distance comparison.
         * @return this Builder object
         * @stable ICU 68
         */
        Builder& ಗಣ_Max_ದೂರ(ಸ್ಥಿರಾಂಕ Locale& desired, ಸ್ಥಿರಾಂಕ Locale& supported) U_LIFETIME_BOUND;

        /**
         * Sets the UErrorCode if an error occurred while setting parameters.
         * Preserves older error codes in the outErrorCode.
         *
         * @param outErrorCode Set to an error code if it does not contain one already
         *                  and an error occurred while setting parameters.
         *                  Otherwise unchanged.
         * @return true if U_FAILURE(outErrorCode)
         * @stable ICU 65
         */
        UBool copy_Error_ಗಮ್ಯ(UError_ಸಂಕೇತ &ಹೊರಹೋಗು_Error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

        /**
         * Builds and returns a new locale matcher.
         * This builder can continue to be used.
         *
         * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
         *                  or else the function returns immediately. Check for U_FAILURE()
         *                  on output or use with function chaining. (See User Guide for details.)
         * @return LocaleMatcher
         * @stable ICU 65
         */
        LocaleMatcher build(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
        ಮಿತ್ರ class LocaleMatcher;

        Builder(ಸ್ಥಿರಾಂಕ Builder &other) = delete;
        Builder &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Builder &other) = delete;

        ಶೂನ್ಯ clearSupportedLocales();
        bool ensure_Supported_Locale_ಸದಿಶ();

        UError_ಸಂಕೇತ error_ಸಂಕೇತ_2 = U_ZERO_ERROR;
        UVector *supportedLocales_ = nullptr;
        int32_t ಮಿತಿ_ದೂರ = -1;
        ULoc_ಹೊಂದಿಕೆ_Demotion demotion_ = ULOCMATCH_DEMOTION_REGION;
        Locale *defaultLocale_ = nullptr;
        bool withDefault_ = true;
        ULoc_ಹೊಂದಿಕೆ_Favor_Subtag favor_ = ULOCMATCH_FAVOR_LANGUAGE;
        ULoc_ಹೊಂದಿಕೆ_Direction direction_ = ULOCMATCH_DIRECTION_WITH_ONE_WAY;
        Locale *max_ದೂರ_Desired = nullptr;
        Locale *max_ದೂರ_Supported = nullptr;
    };

    // FYI No public LocaleMatcher constructors in C++; use the Builder.

    /**
     * Move copy constructor; might modify the source.
     * This matcher will have the same settings that the source matcher had.
     * @param src source matcher
     * @stable ICU 65
     */
    LocaleMatcher(LocaleMatcher &&src) noexcept;

    /**
     * Destructor.
     * @stable ICU 65
     */
    ~LocaleMatcher();

    /**
     * Move assignment operator; might modify the source.
     * This matcher will have the same settings that the source matcher had.
     * The behavior is undefined if *this and src are the same object.
     * @param src source matcher
     * @return *this
     * @stable ICU 65
     */
    LocaleMatcher& ಕ್ರಿಯಾಕಾರಕ=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND;

    /**
     * Returns the supported locale which best matches the desired locale.
     *
     * @param desiredLocale Typically a user's language.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return the best-matching supported locale.
     * @stable ICU 65
     */
    ಸ್ಥಿರಾಂಕ Locale* get_ಉತ್ತಮ_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale& desiredLocale,
                               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

    /**
     * Returns the supported locale which best matches one of the desired locales.
     *
     * @param desiredLocales Typically a user's languages, in order of preference (descending).
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return the best-matching supported locale.
     * @stable ICU 65
     */
    ಸ್ಥಿರಾಂಕ Locale* get_ಉತ್ತಮ_ಹೊಂದಿಕೆ(Locale::Iterator& desiredLocales,
                               UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

    /**
     * Parses an Accept-Language string
     * (<a href="https://tools.ietf.org/html/rfc2616#section-14.4">RFC 2616 Section 14.4</a>),
     * such as "af, en, fr;q=0.9",
     * and returns the supported locale which best matches one of the desired locales.
     * Allows whitespace in more places but does not allow "*".
     *
     * @param desiredLocaleList Typically a user's languages, as an Accept-Language string.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return the best-matching supported locale.
     * @stable ICU 65
     */
    ಸ್ಥಿರಾಂಕ Locale* get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_For_List_String(StringPiece desiredLocaleList,
                                            UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

    /**
     * Returns the best match between the desired locale and the supported locales.
     * If the result's desired locale is not nullptr, then it is the address of the input locale.
     * It has not been cloned.
     *
     * @param desiredLocale Typically a user's language.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return the best-matching pair of the desired and a supported locale.
     * @stable ICU 65
     */
    ಫಲಿತಾಂಶ_2 get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(ಸ್ಥಿರಾಂಕ Locale& desiredLocale U_LIFETIME_BOUND,
                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

    /**
     * Returns the best match between the desired and supported locales.
     * If the result's desired locale is not nullptr, then it is a clone of
     * the best-matching desired locale. The Result object owns the clone.
     *
     * @param desiredLocales Typically a user's languages, in order of preference (descending).
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return the best-matching pair of a desired and a supported locale.
     * @stable ICU 65
     */
    ಫಲಿತಾಂಶ_2 get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(Locale::Iterator& desiredLocales,
                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;

    /**
     * Returns true if the pair of locales matches acceptably.
     * This is influenced by Builder options such as setDirection(), setFavorSubtag(),
     * and setMaxDistance().
     *
     * @param desired The desired locale.
     * @param supported The supported locale.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return true if the pair of locales matches acceptably.
     * @stable ICU 68
     */
    UBool is_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale &desired, ಸ್ಥಿರಾಂಕ Locale &supported, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_HIDE_ಆಂತರಿಕ_API
    /**
     * Returns a fraction between 0 and 1, where 1 means that the languages are a
     * perfect match, and 0 means that they are completely different.
     *
     * <p>This is mostly an implementation detail, and the precise values may change over time.
     * The implementation may use either the maximized forms or the others ones, or both.
     * The implementation may or may not rely on the forms to be consistent with each other.
     *
     * <p>Callers should construct and use a matcher rather than match pairs of locales directly.
     *
     * @param desired Desired locale.
     * @param supported Supported locale.
     * @param errorCode ICU error code. Its input value must pass the U_SUCCESS() test,
     *                  or else the function returns immediately. Check for U_FAILURE()
     *                  on output or use with function chaining. (See User Guide for details.)
     * @return value between 0 and 1, inclusive.
     * @internal (has a known user)
     */
    ದ್ವಿಗುಣ ಆಂತರಿಕ_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale &desired, ಸ್ಥಿರಾಂಕ Locale &supported, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;
#ಆದರೆ_ಅಂತ್ಯ  // U_HIDE_INTERNAL_API

ಖಾಸಗಿ:
    LocaleMatcher(ಸ್ಥಿರಾಂಕ Builder &builder, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    LocaleMatcher(ಸ್ಥಿರಾಂಕ LocaleMatcher &other) = delete;
    LocaleMatcher &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ LocaleMatcher &other) = delete;

    int32_t putIfAbsent(ಸ್ಥಿರಾಂಕ LSR &lsr, int32_t i, int32_t supp_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

    std::optional<int32_t> get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(LSR desiredLSR, LocaleLsrIterator *ಉಳಿದ_Iter, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ;

    ಸ್ಥಿರಾಂಕ LikelySubtags &likelySubtags;
    ಸ್ಥಿರಾಂಕ Locale_ದೂರ &locale_ದೂರ;
    int32_t ಮಿತಿ_ದೂರ_2;
    int32_t demotion_ಪ್ರತಿ_Desired_Locale;
    ULoc_ಹೊಂದಿಕೆ_Favor_Subtag favorSubtag;
    ULoc_ಹೊಂದಿಕೆ_Direction direction;

    // These are in input order.
    ಸ್ಥಿರಾಂಕ Locale ** supportedLocales;
    LSR *lsrs;
    int32_t supported_Locales_ಉದ್ದಳತೆ;
    // These are in preference order: 1. Default locale 2. paradigm locales 3. others.
    UHashtable *supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ;  // Map<LSR, Integer>
    // Array versions of the supportedLsrToIndex keys and values.
    // The distance lookup loops over the supportedLSRs and returns the index of the best match.
    ಸ್ಥಿರಾಂಕ LSR **supportedLSRs;
    int32_t *supportedIndexes;
    int32_t supported_LSRs_ಉದ್ದಳತೆ;
    Locale *ownedDefaultLocale;
    ಸ್ಥಿರಾಂಕ Locale *defaultLocale;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // U_SHOW_CPLUSPLUS_API
#ಆದರೆ_ಅಂತ್ಯ  // __LOCALEMATCHER_H__
