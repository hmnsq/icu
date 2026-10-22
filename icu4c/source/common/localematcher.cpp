// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#અન્તર્ભાવ <optional>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/localebuilder.h"
#અન્તર્ભાવ "unicode/localematcher.h"
#અન્તર્ભાવ "unicode/locid.h"
#અન્તર્ભાવ "unicode/stringpiece.h"
#અન્તર્ભાવ "unicode/uloc.h"
#અન્તર્ભાવ "unicode/uobject.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "localeprioritylist.h"
#અન્તર્ભાવ "loclikelysubtags.h"
#અન્તર્ભાવ "locdistance.h"
#અન્તર્ભાવ "lsr.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ગણના ULocMatchLifetime {
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
પ્રકારવ્યાખ્યા ગણના ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    જો (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ખોટું;
    }
}

LocaleMatcher::Result::~Result() {
    જો (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    આ->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    જો (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = ખોટું;
    }
    પાછા *આ;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        પાછા Locale::get_મૂળ();
    }
    અચલ Locale *bestDesired = getDesiredLocale();
    જો (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        પાછા *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    અચલ અક્ષર *region = bestDesired->getCountry();
    જો (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    અચલ અક્ષર *variants = bestDesired->getVariant();
    જો (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    પાછા b.build(errorCode);
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
    આ->~Builder();

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
    પાછા *આ;
}

શૂન્ય LocaleMatcher::Builder::clearSupportedLocales() {
    જો (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

તાર્કિક LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    જો (U_FAILURE(errorCode_)) { પાછા ખોટું; }
    જો (supportedLocales_ != nullptr) { પાછા સાચું; }
    LocalPointer<UVector> lpSupportedLocales(નવું UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    જો (U_FAILURE(errorCode_)) { પાછા ખોટું; }
    supportedLocales_ = lpSupportedLocales.orphan();
    પાછા સાચું;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    clearSupportedLocales();
    જો (!ensureSupportedLocaleVector()) { પાછા *આ; }
    int32_t length = list.getLengthIncludingRemoved();
    માટે (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        જો (locale == nullptr) { ચાલુ; }
        supportedLocales_->adoptElement(locale, errorCode_);
        જો (U_FAILURE(errorCode_)) {
            તોડો;
        }
    }
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    જો (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        જ્યારે (locales.hasNext() && U_SUCCESS(errorCode_)) {
            અચલ Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(અચલ Locale& locale) U_LIFETIME_BOUND {
    જો (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    પાછા *આ;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = ખોટું;
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(અચલ Locale* defaultLocale) U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    Locale *clone = nullptr;
    જો (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        જો (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            પાછા *આ;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = સાચું;
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    favor_ = subtag;
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    demotion_ = demotion;
    પાછા *આ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(અચલ Locale& desired,
                                       અચલ Locale& supported) U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    જો (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        પાછા *આ;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    પાછા *આ;
}

#જો 0
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
    જો (U_FAILURE(errorCode_)) { પાછા *આ; }
    જો (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    પાછા *આ;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) અચલ {
    જો (U_FAILURE(outErrorCode)) { પાછા સાચું; }
    જો (U_SUCCESS(errorCode_)) { પાછા ખોટું; }
    outErrorCode = errorCode_;
    પાછા સાચું;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) અચલ {
    જો (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    પાછા LocaleMatcher(*આ, errorCode);
}

નામાકાશ {

LSR getMaximalLsrOrUnd(અચલ LikelySubtags &likelySubtags, અચલ Locale &locale,
                       UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        પાછા UND_LSR;
    } નહીંતર {
        પાછા likelySubtags.makeMaximizedLsrFrom(locale, ખોટું, errorCode);
    }
}

int32_t hashLSR(અચલ UHashTok token) {
    અચલ LSR *lsr = static_cast<અચલ LSR *>(token.pointer);
    પાછા lsr->hashCode;
}

UBool compareLSRs(અચલ UHashTok t1, અચલ UHashTok t2) {
    અચલ LSR *lsr1 = static_cast<અચલ LSR *>(t1.pointer);
    અચલ LSR *lsr2 = static_cast<અચલ LSR *>(t2.pointer);
    પાછા *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(અચલ LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા suppLength; }
    જો (!uhash_contains_ચાવી(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        જો (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    પાછા suppLength;
}

LocaleMatcher::LocaleMatcher(અચલ Builder &builder, UErrorCode &errorCode) :
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
    જો (U_FAILURE(errorCode)) { પાછા; }
    અચલ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    અચલ LSR *defLSR = nullptr;
    જો (def != nullptr) {
        ownedDefaultLocale = def->clone();
        જો (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    જો (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<અચલ Locale **>(
            uprv_malloc(supportedLocalesLength * કદ(અચલ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = નવું LSR[supportedLocalesLength];
        જો (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * કદ(અચલ Locale *));
        માટે (int32_t i = 0; i < supportedLocalesLength; ++i) {
            અચલ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            જો (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                પાછા;
            }
            અચલ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            જો (U_FAILURE(errorCode)) { પાછા; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        supportedLSRs = static_cast<અચલ LSR **>(
            uprv_malloc(supportedLocalesLength * કદ(અચલ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * કદ(int32_t)));
        જો (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ઢગલો_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        જો (U_FAILURE(errorCode)) { પાછા; }
        int32_t અંક_Paradigms = 0;
        માટે (int32_t i = 0; i < supportedLocalesLength; ++i) {
            અચલ Locale &locale = *supportedLocales[i];
            અચલ LSR &lsr = lsrs[i];
            જો (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } નહીંતર જો (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } નહીંતર જો (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++અંક_Paradigms;
            } નહીંતર {
                order[i] = 3;
            }
            જો (U_FAILURE(errorCode)) { પાછા; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + અંક_Paradigms;
        માટે (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            જો (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        માટે (int32_t i = 0; i < supportedLocalesLength; ++i) {
            જો (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    જો (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    જો (thresholdDistance >= 0) {
        // already copied
    } નહીંતર જો (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        અચલ LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        જો (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } નહીંતર {
            thresholdDistance = 0;
        }
    } નહીંતર {
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
    માટે (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    આ->~LocaleMatcher();

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
    પાછા *આ;
}

વર્ગ LocaleLsrIterator {
જાહેર:
    LocaleLsrIterator(અચલ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        જો (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    તાર્કિક hasNext() અચલ {
        પાછા locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        પાછા getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    શૂન્ય rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        જો (U_FAILURE(errorCode)) { પાછા; }
        bestDesiredIndex = desiredIndex;
        જો (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } નહીંતર {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = નવું Locale(*current);
            જો (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    અચલ Locale *orphanRemembered() {
        અચલ Locale *rem = remembered;
        remembered = nullptr;
        પાછા rem;
    }

    int32_t getBestDesiredIndex() અચલ {
        પાછા bestDesiredIndex;
    }

ખાનગી:
    અચલ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    અચલ Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

અચલ Locale* LocaleMatcher::getBestMatch(અચલ Locale& desiredLocale,
                                          UErrorCode& errorCode) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    પાછા U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

અચલ Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    જો (!desiredLocales.hasNext()) {
        પાછા defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    પાછા U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

અચલ Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode)) { પાછા nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    પાછા getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(અચલ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode)) {
        પાછા Result(nullptr, defaultLocale, -1, -1, ખોટું);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    જો (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        પાછા Result(nullptr, defaultLocale, -1, -1, ખોટું);
    } નહીંતર {
        પાછા Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, ખોટું);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) અચલ U_LIFETIME_BOUND {
    જો (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        પાછા Result(nullptr, defaultLocale, -1, -1, ખોટું);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    જો (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        પાછા Result(nullptr, defaultLocale, -1, -1, ખોટું);
    } નહીંતર {
        પાછા Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, સાચું);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    માટે (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        જો (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = ખોટું;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            જો (found) {
                જો (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                પાછા suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        જો (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            જો (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                જો (U_FAILURE(errorCode)) { પાછા std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        જો ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            તોડો;
        }
        જો (remainingIter == nullptr || !remainingIter->hasNext()) {
            તોડો;
        }
        desiredLSR = remainingIter->next(errorCode);
        જો (U_FAILURE(errorCode)) { પાછા std::nullopt; }
        ++desiredIndex;
    }
    જો (bestSupportedLsrIndex < 0) {
        // no good match
        પાછા std::nullopt;
    }
    પાછા supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(અચલ Locale &desired, અચલ Locale &supported,
                             UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા ખોટું; }
    અચલ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    પાછા indexAndDistance >= 0;
}

દ્વિગુણ LocaleMatcher::internalMatch(અચલ Locale &desired, અચલ Locale &supported, UErrorCode &errorCode) અચલ {
    જો (U_FAILURE(errorCode)) { પાછા 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા 0.; }
    અચલ LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    દ્વિગુણ distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    પાછા (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

નામાકાશ {

વર્ગ LocaleFromTag {
જાહેર:
    LocaleFromTag() : locale(Locale::get_મૂળ()) {}
    અચલ Locale& operator()(અચલ અક્ષર* tag) U_LIFETIME_BOUND { પાછા locale = Locale(tag); }

ખાનગી:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       અક્ષર *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    જો (U_FAILURE(errorCode)) { પાછા 0; }
    LocaleMatcher::Builder builder;
    અચલ અક્ષર *locString;
    જ્યારે ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        જો (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            પાછા 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    જો (U_FAILURE(errorCode)) { પાછા 0; }
    જો (result.getDesiredIndex() >= 0) {
        જો (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        અચલ અક્ષર *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        જો (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        પાછા u_terminateChars(dest, capacity, bestLength, &errorCode);
    } નહીંતર {
        જો (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        પાછા u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(અક્ષર *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    અચલ અક્ષર **acceptList, int32_t accept_List_ગણતરી,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    જો (U_FAILURE(*status)) { પાછા 0; }
    જો ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_ગણતરી != 0 : accept_List_ગણતરી < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<અચલ અક્ષર **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_ગણતરી, converter);
    પાછા acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(અક્ષર *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            અચલ અક્ષર *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    જો (U_FAILURE(*status)) { પાછા 0; }
    જો ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    પાછા acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
