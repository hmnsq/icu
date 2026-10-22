// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#समावेश <optional>

#समावेश "unicode/utypes.h"
#समावेश "unicode/localebuilder.h"
#समावेश "unicode/localematcher.h"
#समावेश "unicode/locid.h"
#समावेश "unicode/stringpiece.h"
#समावेश "unicode/uloc.h"
#समावेश "unicode/uobject.h"
#समावेश "cstring.h"
#समावेश "localeprioritylist.h"
#समावेश "loclikelysubtags.h"
#समावेश "locdistance.h"
#समावेश "lsr.h"
#समावेश "uassert.h"
#समावेश "uhash.h"
#समावेश "ustr_imp.h"
#समावेश "uvector.h"

#घोषणा UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
क्रमागत ULocMatchLifetime {
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
#अगर_अघोषित U_IN_DOXYGEN
प्रकार क्रमागत ULocMatchLifetime ULocMatchLifetime;
#पूर्ण_अगर

U_NAMESPACE_BEGIN

LocaleMatcher::परिणाम_2::परिणाम_2(LocaleMatcher::परिणाम_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_सूचकांक(src.desired_सूचकांक),
        supported_सूचकांक(src.supported_सूचकांक),
        desiredIsOwned(src.desiredIsOwned) {
    अगर (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desired_सूचकांक = -1;
        src.desiredIsOwned = false;
    }
}

LocaleMatcher::परिणाम_2::~परिणाम_2() {
    अगर (desiredIsOwned) {
        मिटाओ desiredLocale;
    }
}

LocaleMatcher::परिणाम_2&
LocaleMatcher::परिणाम_2::चालक=(LocaleMatcher::परिणाम_2&& src) noexcept U_LIFETIME_BOUND {
    यह->~परिणाम_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_सूचकांक = src.desired_सूचकांक;
    supported_सूचकांक = src.supported_सूचकांक;
    desiredIsOwned = src.desiredIsOwned;

    अगर (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desired_सूचकांक = -1;
        src.desiredIsOwned = false;
    }
    वापस *यह;
}

Locale LocaleMatcher::परिणाम_2::makeResolvedLocale(UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        वापस Locale::get_मूल();
    }
    स्थिर Locale *bestDesired = getDesiredLocale();
    अगर (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        वापस *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    स्थिर अक्षर *region = bestDesired->getCountry();
    अगर (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    स्थिर अक्षर *variants = bestDesired->getVariant();
    अगर (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    वापस b.build(errorCode);
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
    मिटाओ supportedLocales_;
    मिटाओ defaultLocale_;
    मिटाओ maxDistanceDesired_;
    मिटाओ maxDistanceSupported_;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::चालक=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    यह->~Builder();

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
    वापस *यह;
}

व्योम LocaleMatcher::Builder::clearSupportedLocales() {
    अगर (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

bool LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    अगर (U_FAILURE(errorCode_)) { वापस false; }
    अगर (supportedLocales_ != nullptr) { वापस true; }
    LocalPointer<UVector> lpSupportedLocales(नया UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    अगर (U_FAILURE(errorCode_)) { वापस false; }
    supportedLocales_ = lpSupportedLocales.orphan();
    वापस true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    clearSupportedLocales();
    अगर (!ensureSupportedLocaleVector()) { वापस *यह; }
    int32_t length = list.get_लंबाई_Including_Removed();
    क्रम (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        अगर (locale == nullptr) { जारी; }
        supportedLocales_->adoptElement(locale, errorCode_);
        अगर (U_FAILURE(errorCode_)) {
            अवरोध;
        }
    }
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    अगर (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        जबतक (locales.has_अग्र() && U_SUCCESS(errorCode_)) {
            स्थिर Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(स्थिर Locale& locale) U_LIFETIME_BOUND {
    अगर (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    वापस *यह;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    मिटाओ defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = false;
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(स्थिर Locale* defaultLocale) U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    Locale *clone = nullptr;
    अगर (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        अगर (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            वापस *यह;
        }
    }
    मिटाओ defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    favor_ = subtag;
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    demotion_ = demotion;
    वापस *यह;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(स्थिर Locale& desired,
                                       स्थिर Locale& supported) U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    अगर (desiredClone == nullptr || supportedClone == nullptr) {
        मिटाओ desiredClone;  // in case only one could not be allocated
        मिटाओ supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        वापस *यह;
    }
    मिटाओ maxDistanceDesired_;
    मिटाओ maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    वापस *यह;
}

#अगर 0
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
    अगर (U_FAILURE(errorCode_)) { वापस *यह; }
    अगर (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    वापस *यह;
}
#पूर्ण_अगर

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) स्थिर {
    अगर (U_FAILURE(outErrorCode)) { वापस true; }
    अगर (U_SUCCESS(errorCode_)) { वापस false; }
    outErrorCode = errorCode_;
    वापस true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) स्थिर {
    अगर (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    वापस LocaleMatcher(*यह, errorCode);
}

namespace {

LSR getMaximalLsrOrUnd(स्थिर LikelySubtags &likelySubtags, स्थिर Locale &locale,
                       UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        वापस UND_LSR;
    } अन्यथा {
        वापस likelySubtags.makeMaximizedLsrFrom(locale, false, errorCode);
    }
}

int32_t hashLSR(स्थिर UHashTok token) {
    स्थिर LSR *lsr = static_cast<स्थिर LSR *>(token.pointer);
    वापस lsr->hashCode;
}

UBool compareLSRs(स्थिर UHashTok t1, स्थिर UHashTok t2) {
    स्थिर LSR *lsr1 = static_cast<स्थिर LSR *>(t1.pointer);
    स्थिर LSR *lsr2 = static_cast<स्थिर LSR *>(t2.pointer);
    वापस *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(स्थिर LSR &lsr, int32_t i, int32_t supp_लंबाई,
                                   UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस supp_लंबाई; }
    अगर (!uhash_contains_कुंजी(supported_Lsr_To_सूचकांक, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_To_सूचकांक, const_cast<LSR *>(&lsr), i, &errorCode);
        अगर (U_SUCCESS(errorCode)) {
            supportedLSRs[supp_लंबाई] = &lsr;
            supportedIndexes[supp_लंबाई++] = i;
        }
    }
    वापस supp_लंबाई;
}

LocaleMatcher::LocaleMatcher(स्थिर Builder &builder, UErrorCode &errorCode) :
        likelySubtags(*LikelySubtags::getSingleton(errorCode)),
        localeDistance(*LocaleDistance::getSingleton(errorCode)),
        thresholdDistance(builder.thresholdDistance_),
        demotionPerDesiredLocale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(nullptr), lsrs(nullptr), supported_Locales_लंबाई(0),
        supported_Lsr_To_सूचकांक(nullptr),
        supportedLSRs(nullptr), supportedIndexes(nullptr), supported_LSRs_लंबाई(0),
        ownedDefaultLocale(nullptr), defaultLocale(nullptr) {
    अगर (U_FAILURE(errorCode)) { वापस; }
    स्थिर Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    स्थिर LSR *defLSR = nullptr;
    अगर (def != nullptr) {
        ownedDefaultLocale = def->clone();
        अगर (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_लंबाई = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    अगर (supported_Locales_लंबाई > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<स्थिर Locale **>(
            uprv_malloc(supported_Locales_लंबाई * माप(स्थिर Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नया LSR[supported_Locales_लंबाई];
        अगर (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_लंबाई * माप(स्थिर Locale *));
        क्रम (int32_t i = 0; i < supported_Locales_लंबाई; ++i) {
            स्थिर Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            अगर (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
            स्थिर Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            अगर (U_FAILURE(errorCode)) { वापस; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_To_सूचकांक = uhash_open_आकार(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_लंबाई, &errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        supportedLSRs = static_cast<स्थिर LSR **>(
            uprv_malloc(supported_Locales_लंबाई * माप(स्थिर LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_लंबाई * माप(int32_t)));
        अगर (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        int32_t supp_लंबाई = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ढेर_Array<int8_t, 100> order(supported_Locales_लंबाई, errorCode);
        अगर (U_FAILURE(errorCode)) { वापस; }
        int32_t अंक_Paradigms = 0;
        क्रम (int32_t i = 0; i < supported_Locales_लंबाई; ++i) {
            स्थिर Locale &locale = *supportedLocales[i];
            स्थिर LSR &lsr = lsrs[i];
            अगर (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                supp_लंबाई = putIfAbsent(lsr, 0, supp_लंबाई, errorCode);
            } अन्यथा अगर (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                supp_लंबाई = putIfAbsent(lsr, i, supp_लंबाई, errorCode);
            } अन्यथा अगर (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++अंक_Paradigms;
            } अन्यथा {
                order[i] = 3;
            }
            अगर (U_FAILURE(errorCode)) { वापस; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = supp_लंबाई + अंक_Paradigms;
        क्रम (int32_t i = 0; i < supported_Locales_लंबाई && supp_लंबाई < paradigmLimit; ++i) {
            अगर (order[i] == 2) {
                supp_लंबाई = putIfAbsent(lsrs[i], i, supp_लंबाई, errorCode);
            }
        }
        // Add remaining supported locales.
        क्रम (int32_t i = 0; i < supported_Locales_लंबाई; ++i) {
            अगर (order[i] == 3) {
                supp_लंबाई = putIfAbsent(lsrs[i], i, supp_लंबाई, errorCode);
            }
        }
        supported_LSRs_लंबाई = supp_लंबाई;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    अगर (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    अगर (thresholdDistance >= 0) {
        // already copied
    } अन्यथा अगर (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        स्थिर LSR *pSuppLSR = &suppLSR;
        int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        अगर (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(सूचकांक_And_Distance) + 1;
        } अन्यथा {
            thresholdDistance = 0;
        }
    } अन्यथा {
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
        supported_Locales_लंबाई(src.supported_Locales_लंबाई),
        supported_Lsr_To_सूचकांक(src.supported_Lsr_To_सूचकांक),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_लंबाई(src.supported_LSRs_लंबाई),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supported_Locales_लंबाई = 0;
    src.supported_Lsr_To_सूचकांक = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supported_LSRs_लंबाई = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
}

LocaleMatcher::~LocaleMatcher() {
    क्रम (int32_t i = 0; i < supported_Locales_लंबाई; ++i) {
        मिटाओ supportedLocales[i];
    }
    uprv_free(supportedLocales);
    मिटाओ[] lsrs;
    uhash_close(supported_Lsr_To_सूचकांक);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    मिटाओ ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::चालक=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    यह->~LocaleMatcher();

    thresholdDistance = src.thresholdDistance;
    demotionPerDesiredLocale = src.demotionPerDesiredLocale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_लंबाई = src.supported_Locales_लंबाई;
    supported_Lsr_To_सूचकांक = src.supported_Lsr_To_सूचकांक;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_लंबाई = src.supported_LSRs_लंबाई;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = nullptr;
    src.lsrs = nullptr;
    src.supported_Locales_लंबाई = 0;
    src.supported_Lsr_To_सूचकांक = nullptr;
    src.supportedLSRs = nullptr;
    src.supportedIndexes = nullptr;
    src.supported_LSRs_लंबाई = 0;
    src.ownedDefaultLocale = nullptr;
    src.defaultLocale = nullptr;
    वापस *यह;
}

श्रेणी LocaleLsrIterator {
खुला:
    LocaleLsrIterator(स्थिर LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        अगर (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            मिटाओ remembered;
        }
    }

    bool has_अग्र() स्थिर {
        वापस locales.has_अग्र();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        वापस getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    व्योम rememberCurrent(int32_t desired_सूचकांक, UErrorCode &errorCode) {
        अगर (U_FAILURE(errorCode)) { वापस; }
        best_Desired_सूचकांक = desired_सूचकांक;
        अगर (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } अन्यथा {
            // ULOCMATCH_TEMPORARY_LOCALES
            मिटाओ remembered;
            remembered = नया Locale(*current);
            अगर (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    स्थिर Locale *orphanRemembered() {
        स्थिर Locale *rem = remembered;
        remembered = nullptr;
        वापस rem;
    }

    int32_t get_Best_Desired_सूचकांक() स्थिर {
        वापस best_Desired_सूचकांक;
    }

निजी:
    स्थिर LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    स्थिर Locale *current = nullptr, *remembered = nullptr;
    int32_t best_Desired_सूचकांक = -1;
};

स्थिर Locale* LocaleMatcher::getBestMatch(स्थिर Locale& desiredLocale,
                                          UErrorCode& errorCode) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    वापस U_SUCCESS(errorCode) && supp_सूचकांक.has_value() ? supportedLocales[*supp_सूचकांक]
                                                         : defaultLocale;
}

स्थिर Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    अगर (!desiredLocales.has_अग्र()) {
        वापस defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(lsrIter.next(errorCode), &lsrIter, errorCode);
    वापस U_SUCCESS(errorCode) && supp_सूचकांक.has_value() ? supportedLocales[*supp_सूचकांक]
                                                         : defaultLocale;
}

स्थिर Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode)) { वापस nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    वापस getBestMatch(iter, errorCode);
}

LocaleMatcher::परिणाम_2
LocaleMatcher::get_Best_Match_परिणाम(स्थिर Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode)) {
        वापस परिणाम_2(nullptr, defaultLocale, -1, -1, false);
    }
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    अगर (U_FAILURE(errorCode) || !supp_सूचकांक.has_value()) {
        वापस परिणाम_2(nullptr, defaultLocale, -1, -1, false);
    } अन्यथा {
        वापस परिणाम_2(&desiredLocale, supportedLocales[*supp_सूचकांक], 0, *supp_सूचकांक, false);
    }
}

LocaleMatcher::परिणाम_2
LocaleMatcher::get_Best_Match_परिणाम(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) स्थिर U_LIFETIME_BOUND {
    अगर (U_FAILURE(errorCode) || !desiredLocales.has_अग्र()) {
        वापस परिणाम_2(nullptr, defaultLocale, -1, -1, false);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(lsrIter.next(errorCode), &lsrIter, errorCode);
    अगर (U_FAILURE(errorCode) || !supp_सूचकांक.has_value()) {
        वापस परिणाम_2(nullptr, defaultLocale, -1, -1, false);
    } अन्यथा {
        वापस परिणाम_2(lsrIter.orphanRemembered(), supportedLocales[*supp_सूचकांक],
                      lsrIter.get_Best_Desired_सूचकांक(), *supp_सूचकांक, true);
    }
}

std::optional<int32_t> LocaleMatcher::get_Best_Supp_सूचकांक(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस std::nullopt; }
    int32_t desired_सूचकांक = 0;
    int32_t best_Supported_Lsr_सूचकांक = -1;
    क्रम (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        अगर (supported_Lsr_To_सूचकांक != nullptr) {
            desiredLSR.setHashCode();
            UBool found = false;
            int32_t supp_सूचकांक = uhash_getiAndFound(supported_Lsr_To_सूचकांक, &desiredLSR, &found);
            अगर (found) {
                अगर (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desired_सूचकांक, errorCode);
                }
                वापस supp_सूचकांक;
            }
        }
        int32_t best_सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
                desiredLSR, supportedLSRs, supported_LSRs_लंबाई,
                bestShiftedDistance, favorSubtag, direction);
        अगर (best_सूचकांक_And_Distance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(best_सूचकांक_And_Distance);
            अगर (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desired_सूचकांक, errorCode);
                अगर (U_FAILURE(errorCode)) { वापस std::nullopt; }
            }
            best_Supported_Lsr_सूचकांक = LocaleDistance::get_सूचकांक(best_सूचकांक_And_Distance);
        }
        अगर ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            अवरोध;
        }
        अगर (remainingIter == nullptr || !remainingIter->has_अग्र()) {
            अवरोध;
        }
        desiredLSR = remainingIter->next(errorCode);
        अगर (U_FAILURE(errorCode)) { वापस std::nullopt; }
        ++desired_सूचकांक;
    }
    अगर (best_Supported_Lsr_सूचकांक < 0) {
        // no good match
        वापस std::nullopt;
    }
    वापस supportedIndexes[best_Supported_Lsr_सूचकांक];
}

UBool LocaleMatcher::isMatch(स्थिर Locale &desired, स्थिर Locale &supported,
                             UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस false; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस false; }
    स्थिर LSR *pSuppLSR = &suppLSR;
    int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    वापस सूचकांक_And_Distance >= 0;
}

द्विगुन LocaleMatcher::internalMatch(स्थिर Locale &desired, स्थिर Locale &supported, UErrorCode &errorCode) स्थिर {
    अगर (U_FAILURE(errorCode)) { वापस 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस 0.; }
    स्थिर LSR *pSuppLSR = &suppLSR;
    int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    द्विगुन distance = LocaleDistance::getDistanceDouble(सूचकांक_And_Distance);
    वापस (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

namespace {

श्रेणी LocaleFromTag {
खुला:
    LocaleFromTag() : locale(Locale::get_मूल()) {}
    स्थिर Locale& चालक()(स्थिर अक्षर* tag) U_LIFETIME_BOUND { वापस locale = Locale(tag); }

निजी:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       अक्षर *dest, int32_t capacity, UAccept_परिणाम *accept_परिणाम,
                       UErrorCode &errorCode) {
    अगर (U_FAILURE(errorCode)) { वापस 0; }
    LocaleMatcher::Builder builder;
    स्थिर अक्षर *locString;
    जबतक ((locString = uenum_अग्र(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        अगर (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            वापस 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::परिणाम_2 परिणाम = matcher.get_Best_Match_परिणाम(desiredLocales, errorCode);
    अगर (U_FAILURE(errorCode)) { वापस 0; }
    अगर (परिणाम.get_Desired_सूचकांक() >= 0) {
        अगर (accept_परिणाम != nullptr) {
            *accept_परिणाम = *परिणाम.getDesiredLocale() == *परिणाम.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        स्थिर अक्षर *bestStr = परिणाम.getSupportedLocale()->getName();
        int32_t best_लंबाई = static_cast<int32_t>(uprv_strlen(bestStr));
        अगर (best_लंबाई <= capacity) {
            uprv_memcpy(dest, bestStr, best_लंबाई);
        }
        वापस u_terminateChars(dest, capacity, best_लंबाई, &errorCode);
    } अन्यथा {
        अगर (accept_परिणाम != nullptr) {
            *accept_परिणाम = ULOC_ACCEPT_FAILED;
        }
        वापस u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(अक्षर *परिणाम, int32_t परिणाम_Available,
                    UAccept_परिणाम *out_परिणाम,
                    स्थिर अक्षर **acceptList, int32_t accept_List_गिनती,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    अगर (U_FAILURE(*status)) { वापस 0; }
    अगर ((परिणाम == nullptr ? परिणाम_Available != 0 : परिणाम_Available < 0) ||
            (acceptList == nullptr ? accept_List_गिनती != 0 : accept_List_गिनती < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<स्थिर अक्षर **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_गिनती, converter);
    वापस acceptLanguage(*availableLocales, desiredLocales,
                          परिणाम, परिणाम_Available, out_परिणाम, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(अक्षर *परिणाम, int32_t परिणाम_Available,
                            UAccept_परिणाम *out_परिणाम,
                            स्थिर अक्षर *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    अगर (U_FAILURE(*status)) { वापस 0; }
    अगर ((परिणाम == nullptr ? परिणाम_Available != 0 : परिणाम_Available < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    वापस acceptLanguage(*availableLocales, desiredLocales,
                          परिणाम, परिणाम_Available, out_परिणाम, *status);
}
