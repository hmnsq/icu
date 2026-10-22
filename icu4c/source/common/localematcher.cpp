// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ <optional>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/localematcher.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "localeprioritylist.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "locdistance.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ಗಣನಾ ULocMatchLifetime {
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    ಯದಿ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ಅಸತ್ಯ;
    }
}

LocaleMatcher::Result::~Result() {
    ಯದಿ (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    ಏತತ್->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    ಯದಿ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ಪ್ರತ್ಯಾಗಮ Locale::get_ಬೇರು();
    }
    ಧ್ರುವ Locale *bestDesired = getDesiredLocale();
    ಯದಿ (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ಪ್ರತ್ಯಾಗಮ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ಧ್ರುವ ವರ್ಣ *region = bestDesired->getCountry();
    ಯದಿ (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ಧ್ರುವ ವರ್ಣ *variants = bestDesired->getVariant();
    ಯದಿ (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    ಪ್ರತ್ಯಾಗಮ b.build(errorCode);
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
    ಏತತ್->~Builder();

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
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ LocaleMatcher::Builder::clearSupportedLocales() {
    ಯದಿ (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

ತಾರ್ಕಿಕ LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಯದಿ (supportedLocales_ != nullptr) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
    LocalPointer<UVector> lpSupportedLocales(ನೂತನ UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    clearSupportedLocales();
    ಯದಿ (!ensureSupportedLocaleVector()) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    int32_t length = list.getLengthIncludingRemoved();
    ಪ್ರತಿ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ಯದಿ (locale == nullptr) { ಅನುವರ್ತ; }
        supportedLocales_->adoptElement(locale, errorCode_);
        ಯದಿ (U_FAILURE(errorCode_)) {
            ಭಙ್ಗ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ಯದಿ (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        ಯಾವತ್ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ಧ್ರುವ Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ಧ್ರುವ Locale& locale) U_LIFETIME_BOUND {
    ಯದಿ (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = ಅಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ಧ್ರುವ Locale* defaultLocale) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    Locale *clone = nullptr;
    ಯದಿ (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        ಯದಿ (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = ಸತ್ಯ;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    favor_ = subtag;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    demotion_ = demotion;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ಧ್ರುವ Locale& desired,
                                       ಧ್ರುವ Locale& supported) U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ಯದಿ (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

#ಯದಿ 0
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
    ಯದಿ (U_FAILURE(errorCode_)) { ಪ್ರತ್ಯಾಗಮ *ಏತತ್; }
    ಯದಿ (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(outErrorCode)) { ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; }
    ಯದಿ (U_SUCCESS(errorCode_)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    outErrorCode = errorCode_;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ಪ್ರತ್ಯಾಗಮ LocaleMatcher(*ಏತತ್, errorCode);
}

ನಾಮಾಕಾಶ {

LSR getMaximalLsrOrUnd(ಧ್ರುವ LikelySubtags &likelySubtags, ಧ್ರುವ Locale &locale,
                       UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ಪ್ರತ್ಯಾಗಮ UND_LSR;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ likelySubtags.makeMaximizedLsrFrom(locale, ಅಸತ್ಯ, errorCode);
    }
}

int32_t hashLSR(ಧ್ರುವ UHashTok token) {
    ಧ್ರುವ LSR *lsr = static_cast<ಧ್ರುವ LSR *>(token.pointer);
    ಪ್ರತ್ಯಾಗಮ lsr->hashCode;
}

UBool compareLSRs(ಧ್ರುವ UHashTok t1, ಧ್ರುವ UHashTok t2) {
    ಧ್ರುವ LSR *lsr1 = static_cast<ಧ್ರುವ LSR *>(t1.pointer);
    ಧ್ರುವ LSR *lsr2 = static_cast<ಧ್ರುವ LSR *>(t2.pointer);
    ಪ್ರತ್ಯಾಗಮ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ಧ್ರುವ LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ suppLength; }
    ಯದಿ (!uhash_contains_ಕೀಲಿ(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        ಯದಿ (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    ಪ್ರತ್ಯಾಗಮ suppLength;
}

LocaleMatcher::LocaleMatcher(ಧ್ರುವ Builder &builder, UErrorCode &errorCode) :
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
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
    ಧ್ರುವ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ಧ್ರುವ LSR *defLSR = nullptr;
    ಯದಿ (def != nullptr) {
        ownedDefaultLocale = def->clone();
        ಯದಿ (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    ಯದಿ (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ಧ್ರುವ Locale **>(
            uprv_malloc(supportedLocalesLength * ಪರಿಮಾಣ(ಧ್ರುವ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = ನೂತನ LSR[supportedLocalesLength];
        ಯದಿ (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * ಪರಿಮಾಣ(ಧ್ರುವ Locale *));
        ಪ್ರತಿ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ಧ್ರುವ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ಯದಿ (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಧ್ರುವ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        supportedLSRs = static_cast<ಧ್ರುವ LSR **>(
            uprv_malloc(supportedLocalesLength * ಪರಿಮಾಣ(ಧ್ರುವ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * ಪರಿಮಾಣ(int32_t)));
        ಯದಿ (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ರಾಶಿ_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        int32_t ಅಂಕೆ_Paradigms = 0;
        ಪ್ರತಿ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ಧ್ರುವ Locale &locale = *supportedLocales[i];
            ಧ್ರುವ LSR &lsr = lsrs[i];
            ಯದಿ (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } ಅನ್ಯಥಾ ಯದಿ (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } ಅನ್ಯಥಾ ಯದಿ (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++ಅಂಕೆ_Paradigms;
            } ಅನ್ಯಥಾ {
                order[i] = 3;
            }
            ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + ಅಂಕೆ_Paradigms;
        ಪ್ರತಿ (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            ಯದಿ (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        ಪ್ರತಿ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ಯದಿ (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ಯದಿ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    ಯದಿ (thresholdDistance >= 0) {
        // already copied
    } ಅನ್ಯಥಾ ಯದಿ (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ಧ್ರುವ LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        ಯದಿ (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } ಅನ್ಯಥಾ {
            thresholdDistance = 0;
        }
    } ಅನ್ಯಥಾ {
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
    ಪ್ರತಿ (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    ಏತತ್->~LocaleMatcher();

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
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ವರ್ಗ LocaleLsrIterator {
ಸಾರ್ವಜನಿಕ:
    LocaleLsrIterator(ಧ್ರುವ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ಯದಿ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    ತಾರ್ಕಿಕ hasNext() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ಪ್ರತ್ಯಾಗಮ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    ಶೂನ್ಯ rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ; }
        bestDesiredIndex = desiredIndex;
        ಯದಿ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ಅನ್ಯಥಾ {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = ನೂತನ Locale(*current);
            ಯದಿ (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ಧ್ರುವ Locale *orphanRemembered() {
        ಧ್ರುವ Locale *rem = remembered;
        remembered = nullptr;
        ಪ್ರತ್ಯಾಗಮ rem;
    }

    int32_t getBestDesiredIndex() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ bestDesiredIndex;
    }

ಗುಪ್ತ:
    ಧ್ರುವ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ಧ್ರುವ Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ಧ್ರುವ Locale* LocaleMatcher::getBestMatch(ಧ್ರುವ Locale& desiredLocale,
                                          UErrorCode& errorCode) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ಧ್ರುವ Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    ಯದಿ (!desiredLocales.hasNext()) {
        ಪ್ರತ್ಯಾಗಮ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ಧ್ರುವ Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ಪ್ರತ್ಯಾಗಮ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ಧ್ರುವ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ Result(nullptr, defaultLocale, -1, -1, ಅಸತ್ಯ);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ಯದಿ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ಪ್ರತ್ಯಾಗಮ Result(nullptr, defaultLocale, -1, -1, ಅಸತ್ಯ);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, ಅಸತ್ಯ);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ಧ್ರುವ U_LIFETIME_BOUND {
    ಯದಿ (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        ಪ್ರತ್ಯಾಗಮ Result(nullptr, defaultLocale, -1, -1, ಅಸತ್ಯ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ಯದಿ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ಪ್ರತ್ಯಾಗಮ Result(nullptr, defaultLocale, -1, -1, ಅಸತ್ಯ);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, ಸತ್ಯ);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    ಪ್ರತಿ (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        ಯದಿ (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = ಅಸತ್ಯ;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            ಯದಿ (found) {
                ಯದಿ (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                ಪ್ರತ್ಯಾಗಮ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        ಯದಿ (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            ಯದಿ (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        ಯದಿ ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            ಭಙ್ಗ;
        }
        ಯದಿ (remainingIter == nullptr || !remainingIter->hasNext()) {
            ಭಙ್ಗ;
        }
        desiredLSR = remainingIter->next(errorCode);
        ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ std::nullopt; }
        ++desiredIndex;
    }
    ಯದಿ (bestSupportedLsrIndex < 0) {
        // no good match
        ಪ್ರತ್ಯಾಗಮ std::nullopt;
    }
    ಪ್ರತ್ಯಾಗಮ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ಧ್ರುವ Locale &desired, ಧ್ರುವ Locale &supported,
                             UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
    ಧ್ರುವ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ಪ್ರತ್ಯಾಗಮ indexAndDistance >= 0;
}

ದ್ವಿಗುಣ LocaleMatcher::internalMatch(ಧ್ರುವ Locale &desired, ಧ್ರುವ Locale &supported, UErrorCode &errorCode) ಧ್ರುವ {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0.; }
    ಧ್ರುವ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ದ್ವಿಗುಣ distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    ಪ್ರತ್ಯಾಗಮ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

ನಾಮಾಕಾಶ {

ವರ್ಗ LocaleFromTag {
ಸಾರ್ವಜನಿಕ:
    LocaleFromTag() : locale(Locale::get_ಬೇರು()) {}
    ಧ್ರುವ Locale& operator()(ಧ್ರುವ ವರ್ಣ* tag) U_LIFETIME_BOUND { ಪ್ರತ್ಯಾಗಮ locale = Locale(tag); }

ಗುಪ್ತ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       ವರ್ಣ *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    LocaleMatcher::Builder builder;
    ಧ್ರುವ ವರ್ಣ *locString;
    ಯಾವತ್ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        ಯದಿ (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    ಯದಿ (U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ (result.getDesiredIndex() >= 0) {
        ಯದಿ (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ಧ್ರುವ ವರ್ಣ *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        ಯದಿ (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } ಅನ್ಯಥಾ {
        ಯದಿ (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(ವರ್ಣ *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ಧ್ರುವ ವರ್ಣ **acceptList, int32_t accept_List_ಎಣಿಕೆ,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_ಎಣಿಕೆ != 0 : accept_List_ಎಣಿಕೆ < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ಧ್ರುವ ವರ್ಣ **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_ಎಣಿಕೆ, converter);
    ಪ್ರತ್ಯಾಗಮ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(ವರ್ಣ *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ಧ್ರುವ ವರ್ಣ *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    ಯದಿ (U_FAILURE(*status)) { ಪ್ರತ್ಯಾಗಮ 0; }
    ಯದಿ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ಪ್ರತ್ಯಾಗಮ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
