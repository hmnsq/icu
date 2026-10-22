// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.h
// created: 2019may08 Markus W. Scherer

#నిర్వచితం_కాకపోతే __LOCALEMATCHER_H__
#నిర్వచించు __LOCALEMATCHER_H__

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ <optional>

#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uobject.h"

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
గణన ULoc_సరిపోలిక_Favor_Subtag {
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
#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
రకనిర్వచనం గణన ULoc_సరిపోలిక_Favor_Subtag ULoc_సరిపోలిక_Favor_Subtag;
#అయితే_ముగింపు

/**
 * Builder option for whether all desired locales are treated equally or
 * earlier ones are preferred.
 *
 * @see LocaleMatcher::Builder#setDemotionPerDesiredLocale(ULocMatchDemotion)
 * @stable ICU 65
 */
గణన ULoc_సరిపోలిక_Demotion {
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
#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
రకనిర్వచనం గణన ULoc_సరిపోలిక_Demotion ULoc_సరిపోలిక_Demotion;
#అయితే_ముగింపు

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
గణన ULoc_సరిపోలిక_Direction {
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
#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
రకనిర్వచనం గణన ULoc_సరిపోలిక_Direction ULoc_సరిపోలిక_Direction;
#అయితే_ముగింపు

నిర్మాణం UHashtable;

U_NAMESPACE_BEGIN

నిర్మాణం LSR;

class LikelySubtags;
class Locale_దూరం;
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
class U_COMMON_API LocaleMatcher : బహిరంగ UMemory {
బహిరంగ:
    /**
     * Data for the best-matching pair of a desired and a supported locale.
     * Movable but not copyable.
     *
     * @stable ICU 65
     */
    class U_COMMON_API ఫలితం_2 : బహిరంగ UMemory {
    బహిరంగ:
        /**
         * Move constructor; might modify the source.
         * This object will have the same contents that the source object had.
         *
         * @param src Result to move contents from.
         * @stable ICU 65
         */
        ఫలితం_2(ఫలితం_2 &&src) noexcept;

        /**
         * Destructor.
         *
         * @stable ICU 65
         */
        ~ఫలితం_2();

        /**
         * Move assignment; might modify the source.
         * This object will have the same contents that the source object had.
         *
         * @param src Result to move contents from.
         * @stable ICU 65
         */
        ఫలితం_2& క్రియాకారకం=(ఫలితం_2&& src) noexcept U_LIFETIME_BOUND;

        /**
         * Returns the best-matching desired locale.
         * nullptr if the list of desired locales is empty or if none matched well enough.
         *
         * @return the best-matching desired locale, or nullptr.
         * @stable ICU 65
         */
        అంతఃవరుస స్థిరరాశి Locale* getDesiredLocale() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి desiredLocale; }

        /**
         * Returns the best-matching supported locale.
         * If none matched well enough, this is the default locale.
         * The default locale is nullptr if Builder::setNoDefaultLocale() was called,
         * or if the list of supported locales is empty and no explicit default locale is set.
         *
         * @return the best-matching supported locale, or nullptr.
         * @stable ICU 65
         */
        అంతఃవరుస స్థిరరాశి Locale* getSupportedLocale() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి supportedLocale; }

        /**
         * Returns the index of the best-matching desired locale in the input Iterable order.
         * -1 if the list of desired locales is empty or if none matched well enough.
         *
         * @return the index of the best-matching desired locale, or -1.
         * @stable ICU 65
         */
        అంతఃవరుస int32_t get_Desired_సూచీ() స్థిరరాశి { తిరిగి desired_సూచీ; }

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
        అంతఃవరుస int32_t get_Supported_సూచీ() స్థిరరాశి { తిరిగి supported_సూచీ; }

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
        Locale makeResolvedLocale(UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    అంతరంగ:
        ఫలితం_2(స్థిరరాశి Locale *desired, స్థిరరాశి Locale *supported,
               int32_t des_సూచీ, int32_t supp_సూచీ, UBool owned) :
                desiredLocale(desired), supportedLocale(supported),
                desired_సూచీ(des_సూచీ), supported_సూచీ(supp_సూచీ),
                desiredIsOwned(owned) {}

        ఫలితం_2(స్థిరరాశి ఫలితం_2 &other) = delete;
        ఫలితం_2 &క్రియాకారకం=(స్థిరరాశి ఫలితం_2 &other) = delete;

        స్థిరరాశి Locale *desiredLocale;
        స్థిరరాశి Locale *supportedLocale;
        int32_t desired_సూచీ;
        int32_t supported_సూచీ;
        UBool desiredIsOwned;

        మిత్ర class LocaleMatcher;
    };

    /**
     * LocaleMatcher builder.
     * Movable but not copyable.
     *
     * @stable ICU 65
     */
    class U_COMMON_API Builder : బహిరంగ UMemory {
    బహిరంగ:
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
        Builder& క్రియాకారకం=(Builder&& src) noexcept U_LIFETIME_BOUND;

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
        Builder& సమితి_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND;

        /**
         * Copies the supported locales, preserving iteration order.
         * Clears any previously set/added supported locales first.
         * Duplicates are allowed, and are not removed.
         *
         * @param locales the list of locale
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& సమితి_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND;

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
        మూస <typename Iter>
        Builder& సమితి_Supported_Locales(Iter begin, Iter చివర) U_LIFETIME_BOUND {
            ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *this; }
            clearSupportedLocales();
            అయితే (begin != చివర) {
                కూడు_Supported_Locale(*begin++);
            }
            తిరిగి *this;
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
        మూస <typename Iter, typename Conv>
        Builder& సమితి_Supported_Locales_ద్వారా_Converter(Iter begin, Iter చివర,
                                                 Conv converter) U_LIFETIME_BOUND {
            ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *this; }
            clearSupportedLocales();
            అయితే (begin != చివర) {
                కూడు_Supported_Locale(converter(*begin++));
            }
            తిరిగి *this;
        }

        /**
         * Adds another supported locale.
         * Duplicates are allowed, and are not removed.
         *
         * @param locale another locale
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& కూడు_Supported_Locale(స్థిరరాశి Locale& locale) U_LIFETIME_BOUND;

        /**
         * Sets no default locale.
         * There will be no explicit or implicit default locale.
         * If there is no good match, then the matcher will return nullptr for the
         * best supported locale.
         *
         * @stable ICU 68
         */
        Builder& సమితి_No_Default_Locale() U_LIFETIME_BOUND;

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
        Builder& సమితి_Default_Locale(స్థిరరాశి Locale* defaultLocale) U_LIFETIME_BOUND;

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
        Builder& సమితి_Favor_Subtag(ULoc_సరిపోలిక_Favor_Subtag subtag) U_LIFETIME_BOUND;

        /**
         * Option for whether all desired locales are treated equally or
         * earlier ones are preferred (this is the default).
         *
         * @param demotion the demotion per desired locale to set.
         * @return this Builder object
         * @stable ICU 65
         */
        Builder& సమితి_Demotion_చొప్పున_Desired_Locale(ULoc_సరిపోలిక_Demotion demotion) U_LIFETIME_BOUND;

        /**
         * Option for whether to include or ignore one-way (fallback) match data.
         * By default, they are included.
         *
         * @param matchDirection the match direction to set.
         * @return this Builder object
         * @stable ICU 67
         */
        Builder& సమితి_Direction(ULoc_సరిపోలిక_Direction సరిపోలిక_Direction) U_LIFETIME_BOUND {
            ఒకవేళ (U_SUCCESS(error_సంకేతం_2)) {
                direction_ = సరిపోలిక_Direction;
            }
            తిరిగి *this;
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
        Builder& సమితి_Max_దూరం(స్థిరరాశి Locale& desired, స్థిరరాశి Locale& supported) U_LIFETIME_BOUND;

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
        UBool copy_Error_గమ్యం(UError_సంకేతం &బహిర్గతం_Error_సంకేతం) స్థిరరాశి;

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
        LocaleMatcher build(UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    అంతరంగ:
        మిత్ర class LocaleMatcher;

        Builder(స్థిరరాశి Builder &other) = delete;
        Builder &క్రియాకారకం=(స్థిరరాశి Builder &other) = delete;

        శూన్య clearSupportedLocales();
        bool ensure_Supported_Locale_సదిశ();

        UError_సంకేతం error_సంకేతం_2 = U_ZERO_ERROR;
        UVector *supportedLocales_ = nullptr;
        int32_t పరిమితి_దూరం = -1;
        ULoc_సరిపోలిక_Demotion demotion_ = ULOCMATCH_DEMOTION_REGION;
        Locale *defaultLocale_ = nullptr;
        bool withDefault_ = true;
        ULoc_సరిపోలిక_Favor_Subtag favor_ = ULOCMATCH_FAVOR_LANGUAGE;
        ULoc_సరిపోలిక_Direction direction_ = ULOCMATCH_DIRECTION_WITH_ONE_WAY;
        Locale *max_దూరం_Desired = nullptr;
        Locale *max_దూరం_Supported = nullptr;
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
    LocaleMatcher& క్రియాకారకం=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND;

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
    స్థిరరాశి Locale* get_ఉత్తమ_సరిపోలిక(స్థిరరాశి Locale& desiredLocale,
                               UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND;

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
    స్థిరరాశి Locale* get_ఉత్తమ_సరిపోలిక(Locale::Iterator& desiredLocales,
                               UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND;

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
    స్థిరరాశి Locale* get_ఉత్తమ_సరిపోలిక_For_List_String(StringPiece desiredLocaleList,
                                            UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND;

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
    ఫలితం_2 get_ఉత్తమ_సరిపోలిక_ఫలితం(స్థిరరాశి Locale& desiredLocale U_LIFETIME_BOUND,
                              UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND;

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
    ఫలితం_2 get_ఉత్తమ_సరిపోలిక_ఫలితం(Locale::Iterator& desiredLocales,
                              UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND;

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
    UBool is_సరిపోలిక(స్థిరరాశి Locale &desired, స్థిరరాశి Locale &supported, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

#నిర్వచితం_కాకపోతే U_HIDE_అంతర్గత_API
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
    ద్విగుణ అంతర్గత_సరిపోలిక(స్థిరరాశి Locale &desired, స్థిరరాశి Locale &supported, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;
#అయితే_ముగింపు  // U_HIDE_INTERNAL_API

అంతరంగ:
    LocaleMatcher(స్థిరరాశి Builder &builder, UError_సంకేతం &error_సంకేతం);
    LocaleMatcher(స్థిరరాశి LocaleMatcher &other) = delete;
    LocaleMatcher &క్రియాకారకం=(స్థిరరాశి LocaleMatcher &other) = delete;

    int32_t putIfAbsent(స్థిరరాశి LSR &lsr, int32_t i, int32_t supp_నిడివి, UError_సంకేతం &error_సంకేతం);

    std::optional<int32_t> get_ఉత్తమ_Supp_సూచీ(LSR desiredLSR, LocaleLsrIterator *మిగిలిన_Iter, UError_సంకేతం &error_సంకేతం) స్థిరరాశి;

    స్థిరరాశి LikelySubtags &likelySubtags;
    స్థిరరాశి Locale_దూరం &locale_దూరం;
    int32_t పరిమితి_దూరం_2;
    int32_t demotion_చొప్పున_Desired_Locale;
    ULoc_సరిపోలిక_Favor_Subtag favorSubtag;
    ULoc_సరిపోలిక_Direction direction;

    // These are in input order.
    స్థిరరాశి Locale ** supportedLocales;
    LSR *lsrs;
    int32_t supported_Locales_నిడివి;
    // These are in preference order: 1. Default locale 2. paradigm locales 3. others.
    UHashtable *supported_Lsr_గమ్యం_సూచీ;  // Map<LSR, Integer>
    // Array versions of the supportedLsrToIndex keys and values.
    // The distance lookup loops over the supportedLSRs and returns the index of the best match.
    స్థిరరాశి LSR **supportedLSRs;
    int32_t *supportedIndexes;
    int32_t supported_LSRs_నిడివి;
    Locale *ownedDefaultLocale;
    స్థిరరాశి Locale *defaultLocale;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు  // U_SHOW_CPLUSPLUS_API
#అయితే_ముగింపు  // __LOCALEMATCHER_H__
