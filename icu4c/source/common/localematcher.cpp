// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাব <optional>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/localematcher.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "localeprioritylist.h"
#অন্তর্ভাব "loclikelysubtags.h"
#অন্তর্ভাব "locdistance.h"
#অন্তর্ভাব "lsr.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uvector.h"

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
প্রকারলেখ গণনা ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::ফলাফল_2::ফলাফল_2(LocaleMatcher::ফলাফল_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_সূচক(src.desired_সূচক),
        supported_সূচক(src.supported_সূচক),
        desiredIsOwned(src.desiredIsOwned) {
    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desired_সূচক = -1;
        src.desiredIsOwned = মিথ্যা;
    }
}

LocaleMatcher::ফলাফল_2::~ফলাফল_2() {
    যদি (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::ফলাফল_2&
LocaleMatcher::ফলাফল_2::operator=(LocaleMatcher::ফলাফল_2&& src) noexcept U_LIFETIME_BOUND {
    এই->~ফলাফল_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_সূচক = src.desired_সূচক;
    supported_সূচক = src.supported_সূচক;
    desiredIsOwned = src.desiredIsOwned;

    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desired_সূচক = -1;
        src.desiredIsOwned = মিথ্যা;
    }
    ফেরত *এই;
}

Locale LocaleMatcher::ফলাফল_2::makeResolvedLocale(UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ফেরত Locale::get_মূল();
    }
    ধ্রুবক Locale *bestDesired = getDesiredLocale();
    যদি (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ফেরত *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ধ্রুবক অক্ষর *region = bestDesired->getCountry();
    যদি (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ধ্রুবক অক্ষর *variants = bestDesired->getVariant();
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
    ফেরত b.build(errorCode);
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
    এই->~Builder();

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
    ফেরত *এই;
}

শূন্য LocaleMatcher::Builder::clearSupportedLocales() {
    যদি (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

যৌক্তিক LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    যদি (U_FAILURE(errorCode_)) { ফেরত মিথ্যা; }
    যদি (supportedLocales_ != nullptr) { ফেরত সত্য; }
    LocalPointer<UVector> lpSupportedLocales(নতুন UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    যদি (U_FAILURE(errorCode_)) { ফেরত মিথ্যা; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ফেরত সত্য;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    clearSupportedLocales();
    যদি (!ensureSupportedLocaleVector()) { ফেরত *এই; }
    int32_t length = list.get_দৈর্ঘ্য_Including_Removed();
    জন্য (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        যদি (locale == nullptr) { চালু; }
        supportedLocales_->adoptElement(locale, errorCode_);
        যদি (U_FAILURE(errorCode_)) {
            বিরতি;
        }
    }
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        যখন (locales.has_পরবর্তী() && U_SUCCESS(errorCode_)) {
            ধ্রুবক Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ধ্রুবক Locale& locale) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ফেরত *এই;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = মিথ্যা;
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ধ্রুবক Locale* defaultLocale) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    Locale *clone = nullptr;
    যদি (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        যদি (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ফেরত *এই;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = সত্য;
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    favor_ = subtag;
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    demotion_ = demotion;
    ফেরত *এই;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ধ্রুবক Locale& desired,
                                       ধ্রুবক Locale& supported) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    যদি (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ফেরত *এই;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ফেরত *এই;
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
    যদি (U_FAILURE(errorCode_)) { ফেরত *এই; }
    যদি (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ফেরত *এই;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ধ্রুবক {
    যদি (U_FAILURE(outErrorCode)) { ফেরত সত্য; }
    যদি (U_SUCCESS(errorCode_)) { ফেরত মিথ্যা; }
    outErrorCode = errorCode_;
    ফেরত সত্য;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ধ্রুবক {
    যদি (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ফেরত LocaleMatcher(*এই, errorCode);
}

নামাকাশ {

LSR getMaximalLsrOrUnd(ধ্রুবক LikelySubtags &likelySubtags, ধ্রুবক Locale &locale,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ফেরত UND_LSR;
    } অন্যথা {
        ফেরত likelySubtags.makeMaximizedLsrFrom(locale, মিথ্যা, errorCode);
    }
}

int32_t hashLSR(ধ্রুবক UHashTok token) {
    ধ্রুবক LSR *lsr = static_cast<ধ্রুবক LSR *>(token.pointer);
    ফেরত lsr->hashCode;
}

UBool compareLSRs(ধ্রুবক UHashTok t1, ধ্রুবক UHashTok t2) {
    ধ্রুবক LSR *lsr1 = static_cast<ধ্রুবক LSR *>(t1.pointer);
    ধ্রুবক LSR *lsr2 = static_cast<ধ্রুবক LSR *>(t2.pointer);
    ফেরত *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ধ্রুবক LSR &lsr, int32_t i, int32_t supp_দৈর্ঘ্য,
                                   UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত supp_দৈর্ঘ্য; }
    যদি (!uhash_contains_চাবি(supported_Lsr_To_সূচক, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_To_সূচক, const_cast<LSR *>(&lsr), i, &errorCode);
        যদি (U_SUCCESS(errorCode)) {
            supportedLSRs[supp_দৈর্ঘ্য] = &lsr;
            supportedIndexes[supp_দৈর্ঘ্য++] = i;
        }
    }
    ফেরত supp_দৈর্ঘ্য;
}

LocaleMatcher::LocaleMatcher(ধ্রুবক Builder &builder, UErrorCode &errorCode) :
        likelySubtags(*LikelySubtags::getSingleton(errorCode)),
        localeDistance(*LocaleDistance::getSingleton(errorCode)),
        thresholdDistance(builder.thresholdDistance_),
        demotionPerDesiredLocale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(nullptr), lsrs(nullptr), supported_Locales_দৈর্ঘ্য(0),
        supported_Lsr_To_সূচক(nullptr),
        supportedLSRs(nullptr), supportedIndexes(nullptr), supported_LSRs_দৈর্ঘ্য(0),
        ownedDefaultLocale(nullptr), defaultLocale(nullptr) {
    যদি (U_FAILURE(errorCode)) { ফেরত; }
    ধ্রুবক Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ধ্রুবক LSR *defLSR = nullptr;
    যদি (def != nullptr) {
        ownedDefaultLocale = def->clone();
        যদি (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_দৈর্ঘ্য = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    যদি (supported_Locales_দৈর্ঘ্য > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ধ্রুবক Locale **>(
            uprv_malloc(supported_Locales_দৈর্ঘ্য * আকার(ধ্রুবক Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = নতুন LSR[supported_Locales_দৈর্ঘ্য];
        যদি (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_দৈর্ঘ্য * আকার(ধ্রুবক Locale *));
        জন্য (int32_t i = 0; i < supported_Locales_দৈর্ঘ্য; ++i) {
            ধ্রুবক Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            যদি (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ফেরত;
            }
            ধ্রুবক Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            যদি (U_FAILURE(errorCode)) { ফেরত; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_To_সূচক = uhash_open_আকার(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_দৈর্ঘ্য, &errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        supportedLSRs = static_cast<ধ্রুবক LSR **>(
            uprv_malloc(supported_Locales_দৈর্ঘ্য * আকার(ধ্রুবক LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_দৈর্ঘ্য * আকার(int32_t)));
        যদি (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        int32_t supp_দৈর্ঘ্য = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_স্তূপ_Array<int8_t, 100> order(supported_Locales_দৈর্ঘ্য, errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        int32_t অঙ্ক_Paradigms = 0;
        জন্য (int32_t i = 0; i < supported_Locales_দৈর্ঘ্য; ++i) {
            ধ্রুবক Locale &locale = *supportedLocales[i];
            ধ্রুবক LSR &lsr = lsrs[i];
            যদি (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                supp_দৈর্ঘ্য = putIfAbsent(lsr, 0, supp_দৈর্ঘ্য, errorCode);
            } অন্যথা যদি (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                supp_দৈর্ঘ্য = putIfAbsent(lsr, i, supp_দৈর্ঘ্য, errorCode);
            } অন্যথা যদি (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++অঙ্ক_Paradigms;
            } অন্যথা {
                order[i] = 3;
            }
            যদি (U_FAILURE(errorCode)) { ফেরত; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = supp_দৈর্ঘ্য + অঙ্ক_Paradigms;
        জন্য (int32_t i = 0; i < supported_Locales_দৈর্ঘ্য && supp_দৈর্ঘ্য < paradigmLimit; ++i) {
            যদি (order[i] == 2) {
                supp_দৈর্ঘ্য = putIfAbsent(lsrs[i], i, supp_দৈর্ঘ্য, errorCode);
            }
        }
        // Add remaining supported locales.
        জন্য (int32_t i = 0; i < supported_Locales_দৈর্ঘ্য; ++i) {
            যদি (order[i] == 3) {
                supp_দৈর্ঘ্য = putIfAbsent(lsrs[i], i, supp_দৈর্ঘ্য, errorCode);
            }
        }
        supported_LSRs_দৈর্ঘ্য = supp_দৈর্ঘ্য;
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
        ধ্রুবক LSR *pSuppLSR = &suppLSR;
        int32_t সূচক_And_Distance = localeDistance.get_Best_সূচক_And_Distance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        যদি (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(সূচক_And_Distance) + 1;
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
        supported_Locales_দৈর্ঘ্য(src.supported_Locales_দৈর্ঘ্য),
        supported_Lsr_To_সূচক(src.supported_Lsr_To_সূচক),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_দৈর্ঘ্য(src.supported_LSRs_দৈর্ঘ্য),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supported_Locales_দৈর্ঘ্য = 0;
    src.supported_Lsr_To_সূচক = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supported_LSRs_দৈর্ঘ্য = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
}

LocaleMatcher::~LocaleMatcher() {
    জন্য (int32_t i = 0; i < supported_Locales_দৈর্ঘ্য; ++i) {
        delete supportedLocales[i];
    }
    uprv_free(supportedLocales);
    delete[] lsrs;
    uhash_close(supported_Lsr_To_সূচক);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    delete ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::operator=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    এই->~LocaleMatcher();

    thresholdDistance = src.thresholdDistance;
    demotionPerDesiredLocale = src.demotionPerDesiredLocale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_দৈর্ঘ্য = src.supported_Locales_দৈর্ঘ্য;
    supported_Lsr_To_সূচক = src.supported_Lsr_To_সূচক;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_দৈর্ঘ্য = src.supported_LSRs_দৈর্ঘ্য;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supported_Locales_দৈর্ঘ্য = 0;
    src.supported_Lsr_To_সূচক = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supported_LSRs_দৈর্ঘ্য = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
    ফেরত *এই;
}

শ্রেণী LocaleLsrIterator {
সর্বজনীন:
    LocaleLsrIterator(ধ্রুবক LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        যদি (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    যৌক্তিক has_পরবর্তী() ধ্রুবক {
        ফেরত locales.has_পরবর্তী();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ফেরত getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    শূন্য rememberCurrent(int32_t desired_সূচক, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { ফেরত; }
        best_Desired_সূচক = desired_সূচক;
        যদি (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } অন্যথা {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = নতুন Locale(*current);
            যদি (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ধ্রুবক Locale *orphanRemembered() {
        ধ্রুবক Locale *rem = remembered;
        remembered = nullptr;
        ফেরত rem;
    }

    int32_t get_Best_Desired_সূচক() ধ্রুবক {
        ফেরত best_Desired_সূচক;
    }

ব্যক্তিগত:
    ধ্রুবক LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ধ্রুবক Locale *current = nullptr, *remembered = nullptr;
    int32_t best_Desired_সূচক = -1;
};

ধ্রুবক Locale* LocaleMatcher::getBestMatch(ধ্রুবক Locale& desiredLocale,
                                          UErrorCode& errorCode) ধ্রুবক U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    std::optional<int32_t> supp_সূচক = get_Best_Supp_সূচক(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ফেরত U_SUCCESS(errorCode) && supp_সূচক.has_value() ? supportedLocales[*supp_সূচক]
                                                         : defaultLocale;
}

ধ্রুবক Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ধ্রুবক U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    যদি (!desiredLocales.has_পরবর্তী()) {
        ফেরত defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_সূচক = get_Best_Supp_সূচক(lsrIter.next(errorCode), &lsrIter, errorCode);
    ফেরত U_SUCCESS(errorCode) && supp_সূচক.has_value() ? supportedLocales[*supp_সূচক]
                                                         : defaultLocale;
}

ধ্রুবক Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ধ্রুবক U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { ফেরত nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ফেরত getBestMatch(iter, errorCode);
}

LocaleMatcher::ফলাফল_2
LocaleMatcher::get_Best_Match_ফলাফল(ধ্রুবক Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ধ্রুবক U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) {
        ফেরত ফলাফল_2(nullptr, defaultLocale, -1, -1, মিথ্যা);
    }
    std::optional<int32_t> supp_সূচক = get_Best_Supp_সূচক(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    যদি (U_FAILURE(errorCode) || !supp_সূচক.has_value()) {
        ফেরত ফলাফল_2(nullptr, defaultLocale, -1, -1, মিথ্যা);
    } অন্যথা {
        ফেরত ফলাফল_2(&desiredLocale, supportedLocales[*supp_সূচক], 0, *supp_সূচক, মিথ্যা);
    }
}

LocaleMatcher::ফলাফল_2
LocaleMatcher::get_Best_Match_ফলাফল(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ধ্রুবক U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode) || !desiredLocales.has_পরবর্তী()) {
        ফেরত ফলাফল_2(nullptr, defaultLocale, -1, -1, মিথ্যা);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_সূচক = get_Best_Supp_সূচক(lsrIter.next(errorCode), &lsrIter, errorCode);
    যদি (U_FAILURE(errorCode) || !supp_সূচক.has_value()) {
        ফেরত ফলাফল_2(nullptr, defaultLocale, -1, -1, মিথ্যা);
    } অন্যথা {
        ফেরত ফলাফল_2(lsrIter.orphanRemembered(), supportedLocales[*supp_সূচক],
                      lsrIter.get_Best_Desired_সূচক(), *supp_সূচক, সত্য);
    }
}

std::optional<int32_t> LocaleMatcher::get_Best_Supp_সূচক(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত std::nullopt; }
    int32_t desired_সূচক = 0;
    int32_t best_Supported_Lsr_সূচক = -1;
    জন্য (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        যদি (supported_Lsr_To_সূচক != nullptr) {
            desiredLSR.setHashCode();
            UBool found = মিথ্যা;
            int32_t supp_সূচক = uhash_getiAndFound(supported_Lsr_To_সূচক, &desiredLSR, &found);
            যদি (found) {
                যদি (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desired_সূচক, errorCode);
                }
                ফেরত supp_সূচক;
            }
        }
        int32_t best_সূচক_And_Distance = localeDistance.get_Best_সূচক_And_Distance(
                desiredLSR, supportedLSRs, supported_LSRs_দৈর্ঘ্য,
                bestShiftedDistance, favorSubtag, direction);
        যদি (best_সূচক_And_Distance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(best_সূচক_And_Distance);
            যদি (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desired_সূচক, errorCode);
                যদি (U_FAILURE(errorCode)) { ফেরত std::nullopt; }
            }
            best_Supported_Lsr_সূচক = LocaleDistance::get_সূচক(best_সূচক_And_Distance);
        }
        যদি ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            বিরতি;
        }
        যদি (remainingIter == nullptr || !remainingIter->has_পরবর্তী()) {
            বিরতি;
        }
        desiredLSR = remainingIter->next(errorCode);
        যদি (U_FAILURE(errorCode)) { ফেরত std::nullopt; }
        ++desired_সূচক;
    }
    যদি (best_Supported_Lsr_সূচক < 0) {
        // no good match
        ফেরত std::nullopt;
    }
    ফেরত supportedIndexes[best_Supported_Lsr_সূচক];
}

UBool LocaleMatcher::isMatch(ধ্রুবক Locale &desired, ধ্রুবক Locale &supported,
                             UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত মিথ্যা; }
    ধ্রুবক LSR *pSuppLSR = &suppLSR;
    int32_t সূচক_And_Distance = localeDistance.get_Best_সূচক_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ফেরত সূচক_And_Distance >= 0;
}

দ্বিগুণ LocaleMatcher::internalMatch(ধ্রুবক Locale &desired, ধ্রুবক Locale &supported, UErrorCode &errorCode) ধ্রুবক {
    যদি (U_FAILURE(errorCode)) { ফেরত 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত 0.; }
    ধ্রুবক LSR *pSuppLSR = &suppLSR;
    int32_t সূচক_And_Distance = localeDistance.get_Best_সূচক_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    দ্বিগুণ distance = LocaleDistance::getDistanceDouble(সূচক_And_Distance);
    ফেরত (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

নামাকাশ {

শ্রেণী LocaleFromTag {
সর্বজনীন:
    LocaleFromTag() : locale(Locale::get_মূল()) {}
    ধ্রুবক Locale& operator()(ধ্রুবক অক্ষর* tag) U_LIFETIME_BOUND { ফেরত locale = Locale(tag); }

ব্যক্তিগত:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       অক্ষর *dest, int32_t capacity, UAccept_ফলাফল *accept_ফলাফল,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { ফেরত 0; }
    LocaleMatcher::Builder builder;
    ধ্রুবক অক্ষর *locString;
    যখন ((locString = uenum_পরবর্তী(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        যদি (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ফেরত 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::ফলাফল_2 ফলাফল = matcher.get_Best_Match_ফলাফল(desiredLocales, errorCode);
    যদি (U_FAILURE(errorCode)) { ফেরত 0; }
    যদি (ফলাফল.get_Desired_সূচক() >= 0) {
        যদি (accept_ফলাফল != nullptr) {
            *accept_ফলাফল = *ফলাফল.getDesiredLocale() == *ফলাফল.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ধ্রুবক অক্ষর *bestStr = ফলাফল.getSupportedLocale()->getName();
        int32_t best_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(bestStr));
        যদি (best_দৈর্ঘ্য <= capacity) {
            uprv_memcpy(dest, bestStr, best_দৈর্ঘ্য);
        }
        ফেরত u_terminateChars(dest, capacity, best_দৈর্ঘ্য, &errorCode);
    } অন্যথা {
        যদি (accept_ফলাফল != nullptr) {
            *accept_ফলাফল = ULOC_ACCEPT_FAILED;
        }
        ফেরত u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(অক্ষর *ফলাফল, int32_t ফলাফল_Available,
                    UAccept_ফলাফল *out_ফলাফল,
                    ধ্রুবক অক্ষর **acceptList, int32_t accept_List_গুনতি,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ফেরত 0; }
    যদি ((ফলাফল == nullptr ? ফলাফল_Available != 0 : ফলাফল_Available < 0) ||
            (acceptList == nullptr ? accept_List_গুনতি != 0 : accept_List_গুনতি < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ধ্রুবক অক্ষর **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_গুনতি, converter);
    ফেরত acceptLanguage(*availableLocales, desiredLocales,
                          ফলাফল, ফলাফল_Available, out_ফলাফল, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(অক্ষর *ফলাফল, int32_t ফলাফল_Available,
                            UAccept_ফলাফল *out_ফলাফল,
                            ধ্রুবক অক্ষর *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    যদি (U_FAILURE(*status)) { ফেরত 0; }
    যদি ((ফলাফল == nullptr ? ফলাফল_Available != 0 : ফলাফল_Available < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ফেরত acceptLanguage(*availableLocales, desiredLocales,
                          ফলাফল, ফলাফল_Available, out_ফলাফল, *status);
}
