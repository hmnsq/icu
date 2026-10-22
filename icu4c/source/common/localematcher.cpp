// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাৱ <optional>

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/localebuilder.h"
#অন্তর্ভাৱ "unicode/localematcher.h"
#অন্তর্ভাৱ "unicode/locid.h"
#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/uloc.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "localeprioritylist.h"
#অন্তর্ভাৱ "loclikelysubtags.h"
#অন্তর্ভাৱ "locdistance.h"
#অন্তর্ভাৱ "lsr.h"
#অন্তর্ভাৱ "uassert.h"
#অন্তর্ভাৱ "uhash.h"
#অন্তর্ভাৱ "ustr_imp.h"
#অন্তর্ভাৱ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
গণনা ULocMatchLifetime {
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
প্রকারনির্দেশ গণনা ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = মিছা;
    }
}

LocaleMatcher::Result::~Result() {
    যদি (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    এতত্->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = মিছা;
    }
    ঘূৰাই *এতত্;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ঘূৰাই Locale::get_মূল();
    }
    ধ্রুৱ Locale *bestDesired = getDesiredLocale();
    যদি (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ঘূৰাই *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ধ্রুৱ ৱর্ণ *region = bestDesired->getCountry();
    যদি (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ধ্রুৱ ৱর্ণ *variants = bestDesired->getVariant();
    যদি (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    ঘূৰাই b.build(errorCode);
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
    এতত্->~Builder();

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
    ঘূৰাই *এতত্;
}

শূন্য LocaleMatcher::Builder::clearSupportedLocales() {
    যদি (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

তার্কিক LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই মিছা; }
    যদি (supportedLocales_ != nullptr) { ঘূৰাই সঁচা; }
    LocalPointer<UVector> lpSupportedLocales(নূতন UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই মিছা; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ঘূৰাই সঁচা;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    clearSupportedLocales();
    যদি (!ensureSupportedLocaleVector()) { ঘূৰাই *এতত্; }
    int32_t length = list.getLengthIncludingRemoved();
    বাবে (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        যদি (locale == nullptr) { অনুৱর্ত; }
        supportedLocales_->adoptElement(locale, errorCode_);
        যদি (U_FAILURE(errorCode_)) {
            ভঙ্গ;
        }
    }
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        যেতিয়ালৈ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ধ্রুৱ Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ধ্রুৱ Locale& locale) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = মিছা;
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ধ্রুৱ Locale* defaultLocale) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    Locale *clone = nullptr;
    যদি (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        যদি (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই *এতত্;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = সঁচা;
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    favor_ = subtag;
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    demotion_ = demotion;
    ঘূৰাই *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ধ্রুৱ Locale& desired,
                                       ধ্রুৱ Locale& supported) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    যদি (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই *এতত্;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ঘূৰাই *এতত্;
}

#যদি 0
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
    যদি (U_FAILURE(errorCode_)) { ঘূৰাই *এতত্; }
    যদি (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ঘূৰাই *এতত্;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ধ্রুৱ {
    যদি (U_FAILURE(outErrorCode)) { ঘূৰাই সঁচা; }
    যদি (U_SUCCESS(errorCode_)) { ঘূৰাই মিছা; }
    outErrorCode = errorCode_;
    ঘূৰাই সঁচা;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ঘূৰাই LocaleMatcher(*এতত্, errorCode);
}

নামাকাশ {

LSR getMaximalLsrOrUnd(ধ্রুৱ LikelySubtags &likelySubtags, ধ্রুৱ Locale &locale,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ঘূৰাই UND_LSR;
    } অন্যথা {
        ঘূৰাই likelySubtags.makeMaximizedLsrFrom(locale, মিছা, errorCode);
    }
}

int32_t hashLSR(ধ্রুৱ UHashTok token) {
    ধ্রুৱ LSR *lsr = static_cast<ধ্রুৱ LSR *>(token.pointer);
    ঘূৰাই lsr->hashCode;
}

UBool compareLSRs(ধ্রুৱ UHashTok t1, ধ্রুৱ UHashTok t2) {
    ধ্রুৱ LSR *lsr1 = static_cast<ধ্রুৱ LSR *>(t1.pointer);
    ধ্রুৱ LSR *lsr2 = static_cast<ধ্রুৱ LSR *>(t2.pointer);
    ঘূৰাই *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ধ্রুৱ LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই suppLength; }
    যদি (!uhash_contains_চাবি(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        যদি (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    ঘূৰাই suppLength;
}

LocaleMatcher::LocaleMatcher(ধ্রুৱ Builder &builder, UErrorCode &errorCode) :
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
    যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
    ধ্রুৱ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ধ্রুৱ LSR *defLSR = nullptr;
    যদি (def != nullptr) {
        ownedDefaultLocale = def->clone();
        যদি (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    যদি (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ধ্রুৱ Locale **>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(ধ্রুৱ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = নূতন LSR[supportedLocalesLength];
        যদি (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * পরিমাণ(ধ্রুৱ Locale *));
        বাবে (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ধ্রুৱ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            যদি (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ঘূৰাই;
            }
            ধ্রুৱ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        supportedLSRs = static_cast<ধ্রুৱ LSR **>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(ধ্রুৱ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(int32_t)));
        যদি (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ঘূৰাই;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_স্তূপ_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        int32_t অঙ্ক_Paradigms = 0;
        বাবে (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ধ্রুৱ Locale &locale = *supportedLocales[i];
            ধ্রুৱ LSR &lsr = lsrs[i];
            যদি (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } অন্যথা যদি (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } অন্যথা যদি (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++অঙ্ক_Paradigms;
            } অন্যথা {
                order[i] = 3;
            }
            যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + অঙ্ক_Paradigms;
        বাবে (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            যদি (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        বাবে (int32_t i = 0; i < supportedLocalesLength; ++i) {
            যদি (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    যদি (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    যদি (thresholdDistance >= 0) {
        // already copied
    } অন্যথা যদি (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ধ্রুৱ LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        যদি (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } অন্যথা {
            thresholdDistance = 0;
        }
    } অন্যথা {
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
    বাবে (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    এতত্->~LocaleMatcher();

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
    ঘূৰাই *এতত্;
}

শ্ৰেণী LocaleLsrIterator {
সার্ৱজনিক:
    LocaleLsrIterator(ধ্রুৱ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        যদি (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    তার্কিক hasNext() ধ্রুৱ {
        ঘূৰাই locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ঘূৰাই getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    শূন্য rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ঘূৰাই; }
        bestDesiredIndex = desiredIndex;
        যদি (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } অন্যথা {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = নূতন Locale(*current);
            যদি (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ধ্রুৱ Locale *orphanRemembered() {
        ধ্রুৱ Locale *rem = remembered;
        remembered = nullptr;
        ঘূৰাই rem;
    }

    int32_t getBestDesiredIndex() ধ্রুৱ {
        ঘূৰাই bestDesiredIndex;
    }

গুপ্ত:
    ধ্রুৱ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ধ্রুৱ Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ধ্রুৱ Locale* LocaleMatcher::getBestMatch(ধ্রুৱ Locale& desiredLocale,
                                          UErrorCode& errorCode) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ঘূৰাই U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ধ্রুৱ Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    যদি (!desiredLocales.hasNext()) {
        ঘূৰাই defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ঘূৰাই U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ধ্রুৱ Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ঘূৰাই getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ধ্রুৱ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) {
        ঘূৰাই Result(nullptr, defaultLocale, -1, -1, মিছা);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    যদি (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ঘূৰাই Result(nullptr, defaultLocale, -1, -1, মিছা);
    } অন্যথা {
        ঘূৰাই Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, মিছা);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ধ্রুৱ U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        ঘূৰাই Result(nullptr, defaultLocale, -1, -1, মিছা);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    যদি (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ঘূৰাই Result(nullptr, defaultLocale, -1, -1, মিছা);
    } অন্যথা {
        ঘূৰাই Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, সঁচা);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    বাবে (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        যদি (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = মিছা;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            যদি (found) {
                যদি (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                ঘূৰাই suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        যদি (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            যদি (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                যদি (U_FAILURE(errorCode)) { ঘূৰাই std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        যদি ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            ভঙ্গ;
        }
        যদি (remainingIter == nullptr || !remainingIter->hasNext()) {
            ভঙ্গ;
        }
        desiredLSR = remainingIter->next(errorCode);
        যদি (U_FAILURE(errorCode)) { ঘূৰাই std::nullopt; }
        ++desiredIndex;
    }
    যদি (bestSupportedLsrIndex < 0) {
        // no good match
        ঘূৰাই std::nullopt;
    }
    ঘূৰাই supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ধ্রুৱ Locale &desired, ধ্রুৱ Locale &supported,
                             UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই মিছা; }
    ধ্রুৱ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ঘূৰাই indexAndDistance >= 0;
}

দ্ৱিগুণ LocaleMatcher::internalMatch(ধ্রুৱ Locale &desired, ধ্রুৱ Locale &supported, UErrorCode &errorCode) ধ্রুৱ {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই 0.; }
    ধ্রুৱ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    দ্ৱিগুণ distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    ঘূৰাই (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

নামাকাশ {

শ্ৰেণী LocaleFromTag {
সার্ৱজনিক:
    LocaleFromTag() : locale(Locale::get_মূল()) {}
    ধ্রুৱ Locale& operator()(ধ্রুৱ ৱর্ণ* tag) U_LIFETIME_BOUND { ঘূৰাই locale = Locale(tag); }

গুপ্ত:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       ৱর্ণ *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ঘূৰাই 0; }
    LocaleMatcher::Builder builder;
    ধ্রুৱ ৱর্ণ *locString;
    যেতিয়ালৈ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        যদি (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ঘূৰাই 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    যদি (U_FAILURE(errorCode)) { ঘূৰাই 0; }
    যদি (result.getDesiredIndex() >= 0) {
        যদি (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ধ্রুৱ ৱর্ণ *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        যদি (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        ঘূৰাই u_terminateChars(dest, capacity, bestLength, &errorCode);
    } অন্যথা {
        যদি (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        ঘূৰাই u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(ৱর্ণ *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ধ্রুৱ ৱর্ণ **acceptList, int32_t accept_List_গুনতি,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ঘূৰাই 0; }
    যদি ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_গুনতি != 0 : accept_List_গুনতি < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ধ্রুৱ ৱর্ণ **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_গুনতি, converter);
    ঘূৰাই acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(ৱর্ণ *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ধ্রুৱ ৱর্ণ *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ঘূৰাই 0; }
    যদি ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ঘূৰাই acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
