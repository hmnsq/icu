// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#ଅନ୍ତର୍ଭାଵ <optional>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localebuilder.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localematcher.h"
#ଅନ୍ତର୍ଭାଵ "unicode/locid.h"
#ଅନ୍ତର୍ଭାଵ "unicode/stringpiece.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uloc.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "localeprioritylist.h"
#ଅନ୍ତର୍ଭାଵ "loclikelysubtags.h"
#ଅନ୍ତର୍ଭାଵ "locdistance.h"
#ଅନ୍ତର୍ଭାଵ "lsr.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ଗଣନା ULocMatchLifetime {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    ଯଦି (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ମିଥ୍ୟା;
    }
}

LocaleMatcher::Result::~Result() {
    ଯଦି (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    ଏହି->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    ଯଦି (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ *ଏହି;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ଫେରନ୍ତୁ Locale::get_ମୂଳ();
    }
    ଅଚଳ Locale *bestDesired = getDesiredLocale();
    ଯଦି (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ଫେରନ୍ତୁ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ଅଚଳ ଅକ୍ଷର *region = bestDesired->getCountry();
    ଯଦି (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ଅଚଳ ଅକ୍ଷର *variants = bestDesired->getVariant();
    ଯଦି (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    ଫେରନ୍ତୁ b.build(errorCode);
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
    ଏହି->~Builder();

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
    ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ LocaleMatcher::Builder::clearSupportedLocales() {
    ଯଦି (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

ତାର୍କିକ LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଯଦି (supportedLocales_ != nullptr) { ଫେରନ୍ତୁ ସତ୍ୟ; }
    LocalPointer<UVector> lpSupportedLocales(ନୂତନ UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    clearSupportedLocales();
    ଯଦି (!ensureSupportedLocaleVector()) { ଫେରନ୍ତୁ *ଏହି; }
    int32_t length = list.getLengthIncludingRemoved();
    ପାଇଁ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ଯଦି (locale == nullptr) { ଜାରି; }
        supportedLocales_->adoptElement(locale, errorCode_);
        ଯଦି (U_FAILURE(errorCode_)) {
            ଭାଙ୍ଗ;
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ଯଦି (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        ଯେତେବେଳେ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ଅଚଳ Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ଅଚଳ Locale& locale) U_LIFETIME_BOUND {
    ଯଦି (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = ମିଥ୍ୟା;
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ଅଚଳ Locale* defaultLocale) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    Locale *clone = nullptr;
    ଯଦି (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        ଯଦି (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ *ଏହି;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = ସତ୍ୟ;
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    favor_ = subtag;
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    demotion_ = demotion;
    ଫେରନ୍ତୁ *ଏହି;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ଅଚଳ Locale& desired,
                                       ଅଚଳ Locale& supported) U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ଯଦି (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ *ଏହି;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ଫେରନ୍ତୁ *ଏହି;
}

#ଯଦି 0
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
    ଯଦି (U_FAILURE(errorCode_)) { ଫେରନ୍ତୁ *ଏହି; }
    ଯଦି (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ଫେରନ୍ତୁ *ଏହି;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(outErrorCode)) { ଫେରନ୍ତୁ ସତ୍ୟ; }
    ଯଦି (U_SUCCESS(errorCode_)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    outErrorCode = errorCode_;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ଫେରନ୍ତୁ LocaleMatcher(*ଏହି, errorCode);
}

ନାମାକାଶ {

LSR getMaximalLsrOrUnd(ଅଚଳ LikelySubtags &likelySubtags, ଅଚଳ Locale &locale,
                       UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ଫେରନ୍ତୁ UND_LSR;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ likelySubtags.makeMaximizedLsrFrom(locale, ମିଥ୍ୟା, errorCode);
    }
}

int32_t hashLSR(ଅଚଳ UHashTok token) {
    ଅଚଳ LSR *lsr = static_cast<ଅଚଳ LSR *>(token.pointer);
    ଫେରନ୍ତୁ lsr->hashCode;
}

UBool compareLSRs(ଅଚଳ UHashTok t1, ଅଚଳ UHashTok t2) {
    ଅଚଳ LSR *lsr1 = static_cast<ଅଚଳ LSR *>(t1.pointer);
    ଅଚଳ LSR *lsr2 = static_cast<ଅଚଳ LSR *>(t2.pointer);
    ଫେରନ୍ତୁ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ଅଚଳ LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ suppLength; }
    ଯଦି (!uhash_contains_ଚାବି(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        ଯଦି (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    ଫେରନ୍ତୁ suppLength;
}

LocaleMatcher::LocaleMatcher(ଅଚଳ Builder &builder, UErrorCode &errorCode) :
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
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
    ଅଚଳ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ଅଚଳ LSR *defLSR = nullptr;
    ଯଦି (def != nullptr) {
        ownedDefaultLocale = def->clone();
        ଯଦି (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    ଯଦି (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ଅଚଳ Locale **>(
            uprv_malloc(supportedLocalesLength * ଆକାର(ଅଚଳ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = ନୂତନ LSR[supportedLocalesLength];
        ଯଦି (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * ଆକାର(ଅଚଳ Locale *));
        ପାଇଁ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ଅଚଳ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ଯଦି (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ;
            }
            ଅଚଳ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        supportedLSRs = static_cast<ଅଚଳ LSR **>(
            uprv_malloc(supportedLocalesLength * ଆକାର(ଅଚଳ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * ଆକାର(int32_t)));
        ଯଦି (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ଗଦା_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        int32_t ଅଙ୍କ_Paradigms = 0;
        ପାଇଁ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ଅଚଳ Locale &locale = *supportedLocales[i];
            ଅଚଳ LSR &lsr = lsrs[i];
            ଯଦି (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } ଅନ୍ୟଥା ଯଦି (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } ଅନ୍ୟଥା ଯଦି (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++ଅଙ୍କ_Paradigms;
            } ଅନ୍ୟଥା {
                order[i] = 3;
            }
            ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + ଅଙ୍କ_Paradigms;
        ପାଇଁ (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            ଯଦି (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        ପାଇଁ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ଯଦି (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ଯଦି (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    ଯଦି (thresholdDistance >= 0) {
        // already copied
    } ଅନ୍ୟଥା ଯଦି (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ଅଚଳ LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        ଯଦି (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } ଅନ୍ୟଥା {
            thresholdDistance = 0;
        }
    } ଅନ୍ୟଥା {
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
    ପାଇଁ (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    ଏହି->~LocaleMatcher();

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
    ଫେରନ୍ତୁ *ଏହି;
}

ଶ୍ରେଣୀ LocaleLsrIterator {
ସର୍ବସାଧାରଣ:
    LocaleLsrIterator(ଅଚଳ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ଯଦି (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    ତାର୍କିକ hasNext() ଅଚଳ {
        ଫେରନ୍ତୁ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ଫେରନ୍ତୁ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    ଶୂନ୍ୟ rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ; }
        bestDesiredIndex = desiredIndex;
        ଯଦି (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ଅନ୍ୟଥା {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = ନୂତନ Locale(*current);
            ଯଦି (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ଅଚଳ Locale *orphanRemembered() {
        ଅଚଳ Locale *rem = remembered;
        remembered = nullptr;
        ଫେରନ୍ତୁ rem;
    }

    int32_t getBestDesiredIndex() ଅଚଳ {
        ଫେରନ୍ତୁ bestDesiredIndex;
    }

ବ୍ୟକ୍ତିଗତ:
    ଅଚଳ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ଅଚଳ Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ଅଚଳ Locale* LocaleMatcher::getBestMatch(ଅଚଳ Locale& desiredLocale,
                                          UErrorCode& errorCode) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ଅଚଳ Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    ଯଦି (!desiredLocales.hasNext()) {
        ଫେରନ୍ତୁ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ଫେରନ୍ତୁ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ଅଚଳ Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ଫେରନ୍ତୁ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ଅଚଳ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ Result(nullptr, defaultLocale, -1, -1, ମିଥ୍ୟା);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ଯଦି (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ଫେରନ୍ତୁ Result(nullptr, defaultLocale, -1, -1, ମିଥ୍ୟା);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, ମିଥ୍ୟା);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ଅଚଳ U_LIFETIME_BOUND {
    ଯଦି (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        ଫେରନ୍ତୁ Result(nullptr, defaultLocale, -1, -1, ମିଥ୍ୟା);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ଯଦି (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ଫେରନ୍ତୁ Result(nullptr, defaultLocale, -1, -1, ମିଥ୍ୟା);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, ସତ୍ୟ);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    ପାଇଁ (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        ଯଦି (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = ମିଥ୍ୟା;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            ଯଦି (found) {
                ଯଦି (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                ଫେରନ୍ତୁ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        ଯଦି (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            ଯଦି (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        ଯଦି ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            ଭାଙ୍ଗ;
        }
        ଯଦି (remainingIter == nullptr || !remainingIter->hasNext()) {
            ଭାଙ୍ଗ;
        }
        desiredLSR = remainingIter->next(errorCode);
        ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ std::nullopt; }
        ++desiredIndex;
    }
    ଯଦି (bestSupportedLsrIndex < 0) {
        // no good match
        ଫେରନ୍ତୁ std::nullopt;
    }
    ଫେରନ୍ତୁ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ଅଚଳ Locale &desired, ଅଚଳ Locale &supported,
                             UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
    ଅଚଳ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ଫେରନ୍ତୁ indexAndDistance >= 0;
}

ଦ୍ୱିଗୁଣ LocaleMatcher::internalMatch(ଅଚଳ Locale &desired, ଅଚଳ Locale &supported, UErrorCode &errorCode) ଅଚଳ {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0.; }
    ଅଚଳ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ଦ୍ୱିଗୁଣ distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    ଫେରନ୍ତୁ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

ନାମାକାଶ {

ଶ୍ରେଣୀ LocaleFromTag {
ସର୍ବସାଧାରଣ:
    LocaleFromTag() : locale(Locale::get_ମୂଳ()) {}
    ଅଚଳ Locale& operator()(ଅଚଳ ଅକ୍ଷର* tag) U_LIFETIME_BOUND { ଫେରନ୍ତୁ locale = Locale(tag); }

ବ୍ୟକ୍ତିଗତ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       ଅକ୍ଷର *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0; }
    LocaleMatcher::Builder builder;
    ଅଚଳ ଅକ୍ଷର *locString;
    ଯେତେବେଳେ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        ଯଦି (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    ଯଦି (U_FAILURE(errorCode)) { ଫେରନ୍ତୁ 0; }
    ଯଦି (result.getDesiredIndex() >= 0) {
        ଯଦି (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ଅଚଳ ଅକ୍ଷର *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        ଯଦି (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        ଫେରନ୍ତୁ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } ଅନ୍ୟଥା {
        ଯଦି (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        ଫେରନ୍ତୁ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(ଅକ୍ଷର *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ଅଚଳ ଅକ୍ଷର **acceptList, int32_t accept_List_ଗଣନା,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ 0; }
    ଯଦି ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_ଗଣନା != 0 : accept_List_ଗଣନା < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ଅଚଳ ଅକ୍ଷର **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_ଗଣନା, converter);
    ଫେରନ୍ତୁ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(ଅକ୍ଷର *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ଅଚଳ ଅକ୍ଷର *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    ଯଦି (U_FAILURE(*status)) { ଫେରନ୍ତୁ 0; }
    ଯଦି ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ଫେରନ୍ତୁ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
