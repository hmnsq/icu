// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#அந்தர்பாவ <optional>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/localebuilder.h"
#அந்தர்பாவ "unicode/localematcher.h"
#அந்தர்பாவ "unicode/locid.h"
#அந்தர்பாவ "unicode/stringpiece.h"
#அந்தர்பாவ "unicode/uloc.h"
#அந்தர்பாவ "unicode/uobject.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "localeprioritylist.h"
#அந்தர்பாவ "loclikelysubtags.h"
#அந்தர்பாவ "locdistance.h"
#அந்தர்பாவ "lsr.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
எண்ணிக்கை ULocMatchLifetime {
    /**
     * Locale objects are temporary.
     * The matcher will make a copy of a locale that will be used beyond one function call.
     *
     * @draft ICU 65
     */
    ULOCMATCH_TEMPORARY_LOCALES,
    /**
     * Locale objects are stored at least as long as the matcher is used.
     * The matcher will keep only a pointer to a locale that will be used beyond one function call,
     * avoiding a copy.
     *
     * @draft ICU 65
     */
    ULOCMATCH_STORED_LOCALES  // TODO: permanent? cached? clone?
};
#ifndef U_IN_DOXYGEN
வகைவரை எண்ணிக்கை ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    எனில் (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = பொய்;
    }
}

LocaleMatcher::Result::~Result() {
    எனில் (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    இது->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    எனில் (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = பொய்;
    }
    திரும்பு *இது;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        திரும்பு Locale::get_வேர்();
    }
    மாறிலி Locale *bestDesired = getDesiredLocale();
    எனில் (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        திரும்பு *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    மாறிலி எழுத்து *region = bestDesired->getCountry();
    எனில் (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    மாறிலி எழுத்து *variants = bestDesired->getVariant();
    எனில் (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    திரும்பு b.build(errorCode);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        errorCode_(src.errorCode_),
        supportedLocales_(src.supportedLocales_),
        thresholdDistance_(src.thresholdDistance_),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = nullptr;
    src.defaultLocale_ = nullptr;
}

LocaleMatcher::Builder::~Builder() {
    delete supportedLocales_;
    delete defaultLocale_;
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::operator=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    இது->~Builder();

    errorCode_ = src.errorCode_;
    supportedLocales_ = src.supportedLocales_;
    thresholdDistance_ = src.thresholdDistance_;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = nullptr;
    src.defaultLocale_ = nullptr;
    திரும்பு *இது;
}

வெறுமை LocaleMatcher::Builder::clearSupportedLocales() {
    எனில் (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

தருக்க LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு பொய்; }
    எனில் (supportedLocales_ != nullptr) { திரும்பு உண்மை; }
    LocalPointer<UVector> lpSupportedLocales(புதிய UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    எனில் (U_FAILURE(errorCode_)) { திரும்பு பொய்; }
    supportedLocales_ = lpSupportedLocales.orphan();
    திரும்பு உண்மை;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    clearSupportedLocales();
    எனில் (!ensureSupportedLocaleVector()) { திரும்பு *இது; }
    int32_t length = list.getLengthIncludingRemoved();
    க்கு (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        எனில் (locale == nullptr) { தொடர்; }
        supportedLocales_->adoptElement(locale, errorCode_);
        எனில் (U_FAILURE(errorCode_)) {
            நிறுத்து;
        }
    }
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    எனில் (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        போது (locales.hasNext() && U_SUCCESS(errorCode_)) {
            மாறிலி Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(மாறிலி Locale& locale) U_LIFETIME_BOUND {
    எனில் (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    திரும்பு *இது;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = பொய்;
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(மாறிலி Locale* defaultLocale) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    Locale *clone = nullptr;
    எனில் (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        எனில் (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு *இது;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = உண்மை;
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    favor_ = subtag;
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    demotion_ = demotion;
    திரும்பு *இது;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(மாறிலி Locale& desired,
                                       மாறிலி Locale& supported) U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    எனில் (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு *இது;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    திரும்பு *இது;
}

#எனில் 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::internalSetThresholdDistance(int32_t thresholdDistance) {
    எனில் (U_FAILURE(errorCode_)) { திரும்பு *இது; }
    எனில் (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    திரும்பு *இது;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) மாறிலி {
    எனில் (U_FAILURE(outErrorCode)) { திரும்பு உண்மை; }
    எனில் (U_SUCCESS(errorCode_)) { திரும்பு பொய்; }
    outErrorCode = errorCode_;
    திரும்பு உண்மை;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) மாறிலி {
    எனில் (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    திரும்பு LocaleMatcher(*இது, errorCode);
}

நாமாகாஶ {

LSR getMaximalLsrOrUnd(மாறிலி LikelySubtags &likelySubtags, மாறிலி Locale &locale,
                       UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        திரும்பு UND_LSR;
    } இல்லையெனில் {
        திரும்பு likelySubtags.makeMaximizedLsrFrom(locale, பொய், errorCode);
    }
}

int32_t hashLSR(மாறிலி UHashTok token) {
    மாறிலி LSR *lsr = static_cast<மாறிலி LSR *>(token.pointer);
    திரும்பு lsr->hashCode;
}

UBool compareLSRs(மாறிலி UHashTok t1, மாறிலி UHashTok t2) {
    மாறிலி LSR *lsr1 = static_cast<மாறிலி LSR *>(t1.pointer);
    மாறிலி LSR *lsr2 = static_cast<மாறிலி LSR *>(t2.pointer);
    திரும்பு *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(மாறிலி LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு suppLength; }
    எனில் (!uhash_contains_சாவி(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        எனில் (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    திரும்பு suppLength;
}

LocaleMatcher::LocaleMatcher(மாறிலி Builder &builder, UErrorCode &errorCode) :
        likelySubtags(*LikelySubtags::getSingleton(errorCode)),
        localeDistance(*LocaleDistance::getSingleton(errorCode)),
        thresholdDistance(builder.thresholdDistance_),
        demotionPerDesiredLocale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(nullptr), lsrs(nullptr), supportedLocalesLength(0),
        supportedLsrToIndex(nullptr),
        supportedLSRs(nullptr), supportedIndexes(nullptr), supportedLSRsLength(0),
        ownedDefaultLocale(nullptr), defaultLocale(nullptr) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு; }
    மாறிலி Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    மாறிலி LSR *defLSR = nullptr;
    எனில் (def != nullptr) {
        ownedDefaultLocale = def->clone();
        எனில் (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    எனில் (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<மாறிலி Locale **>(
            uprv_malloc(supportedLocalesLength * அளவு(மாறிலி Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = புதிய LSR[supportedLocalesLength];
        எனில் (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * அளவு(மாறிலி Locale *));
        க்கு (int32_t i = 0; i < supportedLocalesLength; ++i) {
            மாறிலி Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            எனில் (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                திரும்பு;
            }
            மாறிலி Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        supportedLSRs = static_cast<மாறிலி LSR **>(
            uprv_malloc(supportedLocalesLength * அளவு(மாறிலி LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * அளவு(int32_t)));
        எனில் (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_அடுக்கு_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        int32_t இலக்கம்_Paradigms = 0;
        க்கு (int32_t i = 0; i < supportedLocalesLength; ++i) {
            மாறிலி Locale &locale = *supportedLocales[i];
            மாறிலி LSR &lsr = lsrs[i];
            எனில் (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } இல்லையெனில் எனில் (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } இல்லையெனில் எனில் (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++இலக்கம்_Paradigms;
            } இல்லையெனில் {
                order[i] = 3;
            }
            எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + இலக்கம்_Paradigms;
        க்கு (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            எனில் (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        க்கு (int32_t i = 0; i < supportedLocalesLength; ++i) {
            எனில் (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    எனில் (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    எனில் (thresholdDistance >= 0) {
        // already copied
    } இல்லையெனில் எனில் (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        மாறிலி LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        எனில் (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } இல்லையெனில் {
            thresholdDistance = 0;
        }
    } இல்லையெனில் {
        thresholdDistance = localeDistance.getDefaultScriptDistance();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        localeDistance(src.localeDistance),
        thresholdDistance(src.thresholdDistance),
        demotionPerDesiredLocale(src.demotionPerDesiredLocale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supportedLocalesLength(src.supportedLocalesLength),
        supportedLsrToIndex(src.supportedLsrToIndex),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supportedLSRsLength(src.supportedLSRsLength),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supportedLocalesLength = 0;
    src.supportedLsrToIndex = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supportedLSRsLength = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
}

LocaleMatcher::~LocaleMatcher() {
    க்கு (int32_t i = 0; i < supportedLocalesLength; ++i) {
        delete supportedLocales[i];
    }
    uprv_free(supportedLocales);
    delete[] lsrs;
    uhash_close(supportedLsrToIndex);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    delete ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::operator=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    இது->~LocaleMatcher();

    thresholdDistance = src.thresholdDistance;
    demotionPerDesiredLocale = src.demotionPerDesiredLocale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supportedLocalesLength = src.supportedLocalesLength;
    supportedLsrToIndex = src.supportedLsrToIndex;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supportedLSRsLength = src.supportedLSRsLength;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supportedLocalesLength = 0;
    src.supportedLsrToIndex = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supportedLSRsLength = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
    திரும்பு *இது;
}

வகுப்பு LocaleLsrIterator {
பொது:
    LocaleLsrIterator(மாறிலி LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        எனில் (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    தருக்க hasNext() மாறிலி {
        திரும்பு locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        திரும்பு getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    வெறுமை rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        எனில் (U_FAILURE(errorCode)) { திரும்பு; }
        bestDesiredIndex = desiredIndex;
        எனில் (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } இல்லையெனில் {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = புதிய Locale(*current);
            எனில் (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    மாறிலி Locale *orphanRemembered() {
        மாறிலி Locale *rem = remembered;
        remembered = nullptr;
        திரும்பு rem;
    }

    int32_t getBestDesiredIndex() மாறிலி {
        திரும்பு bestDesiredIndex;
    }

தனிப்பட்ட:
    மாறிலி LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    மாறிலி Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

மாறிலி Locale* LocaleMatcher::getBestMatch(மாறிலி Locale& desiredLocale,
                                          UErrorCode& errorCode) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    திரும்பு U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

மாறிலி Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    எனில் (!desiredLocales.hasNext()) {
        திரும்பு defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    திரும்பு U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

மாறிலி Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode)) { திரும்பு nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    திரும்பு getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(மாறிலி Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode)) {
        திரும்பு Result(nullptr, defaultLocale, -1, -1, பொய்);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    எனில் (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        திரும்பு Result(nullptr, defaultLocale, -1, -1, பொய்);
    } இல்லையெனில் {
        திரும்பு Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, பொய்);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) மாறிலி U_LIFETIME_BOUND {
    எனில் (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        திரும்பு Result(nullptr, defaultLocale, -1, -1, பொய்);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    எனில் (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        திரும்பு Result(nullptr, defaultLocale, -1, -1, பொய்);
    } இல்லையெனில் {
        திரும்பு Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, உண்மை);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    க்கு (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        எனில் (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = பொய்;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            எனில் (found) {
                எனில் (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                திரும்பு suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        எனில் (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            எனில் (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                எனில் (U_FAILURE(errorCode)) { திரும்பு std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        எனில் ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            நிறுத்து;
        }
        எனில் (remainingIter == nullptr || !remainingIter->hasNext()) {
            நிறுத்து;
        }
        desiredLSR = remainingIter->next(errorCode);
        எனில் (U_FAILURE(errorCode)) { திரும்பு std::nullopt; }
        ++desiredIndex;
    }
    எனில் (bestSupportedLsrIndex < 0) {
        // no good match
        திரும்பு std::nullopt;
    }
    திரும்பு supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(மாறிலி Locale &desired, மாறிலி Locale &supported,
                             UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு பொய்; }
    மாறிலி LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    திரும்பு indexAndDistance >= 0;
}

இரட்டை LocaleMatcher::internalMatch(மாறிலி Locale &desired, மாறிலி Locale &supported, UErrorCode &errorCode) மாறிலி {
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0.; }
    மாறிலி LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    இரட்டை distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    திரும்பு (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

நாமாகாஶ {

வகுப்பு LocaleFromTag {
பொது:
    LocaleFromTag() : locale(Locale::get_வேர்()) {}
    மாறிலி Locale& operator()(மாறிலி எழுத்து* tag) U_LIFETIME_BOUND { திரும்பு locale = Locale(tag); }

தனிப்பட்ட:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       எழுத்து *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0; }
    LocaleMatcher::Builder builder;
    மாறிலி எழுத்து *locString;
    போது ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        எனில் (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            திரும்பு 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    எனில் (U_FAILURE(errorCode)) { திரும்பு 0; }
    எனில் (result.getDesiredIndex() >= 0) {
        எனில் (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        மாறிலி எழுத்து *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        எனில் (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        திரும்பு u_terminateChars(dest, capacity, bestLength, &errorCode);
    } இல்லையெனில் {
        எனில் (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        திரும்பு u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(எழுத்து *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    மாறிலி எழுத்து **acceptList, int32_t accept_List_மொத்தம்,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    எனில் (U_FAILURE(*status)) { திரும்பு 0; }
    எனில் ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_மொத்தம் != 0 : accept_List_மொத்தம் < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<மாறிலி எழுத்து **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_மொத்தம், converter);
    திரும்பு acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(எழுத்து *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            மாறிலி எழுத்து *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    எனில் (U_FAILURE(*status)) { திரும்பு 0; }
    எனில் ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    திரும்பு acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
