// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#അന്തര്ഭാവ <optional>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/localebuilder.h"
#അന്തര്ഭാവ "unicode/localematcher.h"
#അന്തര്ഭാവ "unicode/locid.h"
#അന്തര്ഭാവ "unicode/stringpiece.h"
#അന്തര്ഭാവ "unicode/uloc.h"
#അന്തര്ഭാവ "unicode/uobject.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "localeprioritylist.h"
#അന്തര്ഭാവ "loclikelysubtags.h"
#അന്തര്ഭാവ "locdistance.h"
#അന്തര്ഭാവ "lsr.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
എണ്ണൽ ULocMatchLifetime {
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
തരനിർവചനം എണ്ണൽ ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    എങ്കിൽ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = അസത്യം;
    }
}

LocaleMatcher::Result::~Result() {
    എങ്കിൽ (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    ഇത്->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    എങ്കിൽ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = അസത്യം;
    }
    തിരികെ *ഇത്;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        തിരികെ Locale::get_വേര്();
    }
    സ്ഥിരം Locale *bestDesired = getDesiredLocale();
    എങ്കിൽ (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        തിരികെ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    സ്ഥിരം അക്ഷരം *region = bestDesired->getCountry();
    എങ്കിൽ (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    സ്ഥിരം അക്ഷരം *variants = bestDesired->getVariant();
    എങ്കിൽ (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    തിരികെ b.build(errorCode);
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
    ഇത്->~Builder();

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
    തിരികെ *ഇത്;
}

ശൂന്യം LocaleMatcher::Builder::clearSupportedLocales() {
    എങ്കിൽ (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

തർക്കം LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ അസത്യം; }
    എങ്കിൽ (supportedLocales_ != nullptr) { തിരികെ സത്യം; }
    LocalPointer<UVector> lpSupportedLocales(പുതിയ UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ അസത്യം; }
    supportedLocales_ = lpSupportedLocales.orphan();
    തിരികെ സത്യം;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    clearSupportedLocales();
    എങ്കിൽ (!ensureSupportedLocaleVector()) { തിരികെ *ഇത്; }
    int32_t length = list.getLengthIncludingRemoved();
    വേണ്ടി (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        എങ്കിൽ (locale == nullptr) { തുടരുക; }
        supportedLocales_->adoptElement(locale, errorCode_);
        എങ്കിൽ (U_FAILURE(errorCode_)) {
            നിർത്തുക;
        }
    }
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    എങ്കിൽ (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        ആകുമ്പോൾ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            സ്ഥിരം Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(സ്ഥിരം Locale& locale) U_LIFETIME_BOUND {
    എങ്കിൽ (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = അസത്യം;
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(സ്ഥിരം Locale* defaultLocale) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    Locale *clone = nullptr;
    എങ്കിൽ (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        എങ്കിൽ (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ *ഇത്;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = സത്യം;
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    favor_ = subtag;
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    demotion_ = demotion;
    തിരികെ *ഇത്;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(സ്ഥിരം Locale& desired,
                                       സ്ഥിരം Locale& supported) U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    എങ്കിൽ (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ *ഇത്;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    തിരികെ *ഇത്;
}

#എങ്കിൽ 0
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
    എങ്കിൽ (U_FAILURE(errorCode_)) { തിരികെ *ഇത്; }
    എങ്കിൽ (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    തിരികെ *ഇത്;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(outErrorCode)) { തിരികെ സത്യം; }
    എങ്കിൽ (U_SUCCESS(errorCode_)) { തിരികെ അസത്യം; }
    outErrorCode = errorCode_;
    തിരികെ സത്യം;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    തിരികെ LocaleMatcher(*ഇത്, errorCode);
}

നാമാകാശ {

LSR getMaximalLsrOrUnd(സ്ഥിരം LikelySubtags &likelySubtags, സ്ഥിരം Locale &locale,
                       UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        തിരികെ UND_LSR;
    } അല്ലെങ്കിൽ {
        തിരികെ likelySubtags.makeMaximizedLsrFrom(locale, അസത്യം, errorCode);
    }
}

int32_t hashLSR(സ്ഥിരം UHashTok token) {
    സ്ഥിരം LSR *lsr = static_cast<സ്ഥിരം LSR *>(token.pointer);
    തിരികെ lsr->hashCode;
}

UBool compareLSRs(സ്ഥിരം UHashTok t1, സ്ഥിരം UHashTok t2) {
    സ്ഥിരം LSR *lsr1 = static_cast<സ്ഥിരം LSR *>(t1.pointer);
    സ്ഥിരം LSR *lsr2 = static_cast<സ്ഥിരം LSR *>(t2.pointer);
    തിരികെ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(സ്ഥിരം LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ suppLength; }
    എങ്കിൽ (!uhash_contains_താക്കോൽ(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    തിരികെ suppLength;
}

LocaleMatcher::LocaleMatcher(സ്ഥിരം Builder &builder, UErrorCode &errorCode) :
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
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
    സ്ഥിരം Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    സ്ഥിരം LSR *defLSR = nullptr;
    എങ്കിൽ (def != nullptr) {
        ownedDefaultLocale = def->clone();
        എങ്കിൽ (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    എങ്കിൽ (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<സ്ഥിരം Locale **>(
            uprv_malloc(supportedLocalesLength * വലുപ്പം(സ്ഥിരം Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = പുതിയ LSR[supportedLocalesLength];
        എങ്കിൽ (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * വലുപ്പം(സ്ഥിരം Locale *));
        വേണ്ടി (int32_t i = 0; i < supportedLocalesLength; ++i) {
            സ്ഥിരം Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            എങ്കിൽ (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            സ്ഥിരം Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        supportedLSRs = static_cast<സ്ഥിരം LSR **>(
            uprv_malloc(supportedLocalesLength * വലുപ്പം(സ്ഥിരം LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * വലുപ്പം(int32_t)));
        എങ്കിൽ (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_അട്ടി_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        int32_t അക്കം_Paradigms = 0;
        വേണ്ടി (int32_t i = 0; i < supportedLocalesLength; ++i) {
            സ്ഥിരം Locale &locale = *supportedLocales[i];
            സ്ഥിരം LSR &lsr = lsrs[i];
            എങ്കിൽ (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } അല്ലെങ്കിൽ എങ്കിൽ (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } അല്ലെങ്കിൽ എങ്കിൽ (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++അക്കം_Paradigms;
            } അല്ലെങ്കിൽ {
                order[i] = 3;
            }
            എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + അക്കം_Paradigms;
        വേണ്ടി (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            എങ്കിൽ (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        വേണ്ടി (int32_t i = 0; i < supportedLocalesLength; ++i) {
            എങ്കിൽ (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    എങ്കിൽ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    എങ്കിൽ (thresholdDistance >= 0) {
        // already copied
    } അല്ലെങ്കിൽ എങ്കിൽ (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        സ്ഥിരം LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        എങ്കിൽ (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } അല്ലെങ്കിൽ {
            thresholdDistance = 0;
        }
    } അല്ലെങ്കിൽ {
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
    വേണ്ടി (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    ഇത്->~LocaleMatcher();

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
    തിരികെ *ഇത്;
}

വർഗ്ഗം LocaleLsrIterator {
പൊതു:
    LocaleLsrIterator(സ്ഥിരം LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        എങ്കിൽ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    തർക്കം hasNext() സ്ഥിരം {
        തിരികെ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        തിരികെ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    ശൂന്യം rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ; }
        bestDesiredIndex = desiredIndex;
        എങ്കിൽ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } അല്ലെങ്കിൽ {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = പുതിയ Locale(*current);
            എങ്കിൽ (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    സ്ഥിരം Locale *orphanRemembered() {
        സ്ഥിരം Locale *rem = remembered;
        remembered = nullptr;
        തിരികെ rem;
    }

    int32_t getBestDesiredIndex() സ്ഥിരം {
        തിരികെ bestDesiredIndex;
    }

സ്വകാര്യ:
    സ്ഥിരം LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    സ്ഥിരം Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

സ്ഥിരം Locale* LocaleMatcher::getBestMatch(സ്ഥിരം Locale& desiredLocale,
                                          UErrorCode& errorCode) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    തിരികെ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

സ്ഥിരം Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    എങ്കിൽ (!desiredLocales.hasNext()) {
        തിരികെ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    തിരികെ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

സ്ഥിരം Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    തിരികെ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(സ്ഥിരം Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode)) {
        തിരികെ Result(nullptr, defaultLocale, -1, -1, അസത്യം);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        തിരികെ Result(nullptr, defaultLocale, -1, -1, അസത്യം);
    } അല്ലെങ്കിൽ {
        തിരികെ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, അസത്യം);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) സ്ഥിരം U_LIFETIME_BOUND {
    എങ്കിൽ (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        തിരികെ Result(nullptr, defaultLocale, -1, -1, അസത്യം);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        തിരികെ Result(nullptr, defaultLocale, -1, -1, അസത്യം);
    } അല്ലെങ്കിൽ {
        തിരികെ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, സത്യം);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    വേണ്ടി (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        എങ്കിൽ (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = അസത്യം;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            എങ്കിൽ (found) {
                എങ്കിൽ (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                തിരികെ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        എങ്കിൽ (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            എങ്കിൽ (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        എങ്കിൽ ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            നിർത്തുക;
        }
        എങ്കിൽ (remainingIter == nullptr || !remainingIter->hasNext()) {
            നിർത്തുക;
        }
        desiredLSR = remainingIter->next(errorCode);
        എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ std::nullopt; }
        ++desiredIndex;
    }
    എങ്കിൽ (bestSupportedLsrIndex < 0) {
        // no good match
        തിരികെ std::nullopt;
    }
    തിരികെ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(സ്ഥിരം Locale &desired, സ്ഥിരം Locale &supported,
                             UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ അസത്യം; }
    സ്ഥിരം LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    തിരികെ indexAndDistance >= 0;
}

ഇരട്ട LocaleMatcher::internalMatch(സ്ഥിരം Locale &desired, സ്ഥിരം Locale &supported, UErrorCode &errorCode) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0.; }
    സ്ഥിരം LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ഇരട്ട distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    തിരികെ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

നാമാകാശ {

വർഗ്ഗം LocaleFromTag {
പൊതു:
    LocaleFromTag() : locale(Locale::get_വേര്()) {}
    സ്ഥിരം Locale& operator()(സ്ഥിരം അക്ഷരം* tag) U_LIFETIME_BOUND { തിരികെ locale = Locale(tag); }

സ്വകാര്യ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       അക്ഷരം *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0; }
    LocaleMatcher::Builder builder;
    സ്ഥിരം അക്ഷരം *locString;
    ആകുമ്പോൾ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        എങ്കിൽ (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            തിരികെ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    എങ്കിൽ (U_FAILURE(errorCode)) { തിരികെ 0; }
    എങ്കിൽ (result.getDesiredIndex() >= 0) {
        എങ്കിൽ (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        സ്ഥിരം അക്ഷരം *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        എങ്കിൽ (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        തിരികെ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        തിരികെ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(അക്ഷരം *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    സ്ഥിരം അക്ഷരം **acceptList, int32_t accept_List_എണ്ണം,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) { തിരികെ 0; }
    എങ്കിൽ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_എണ്ണം != 0 : accept_List_എണ്ണം < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<സ്ഥിരം അക്ഷരം **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_എണ്ണം, converter);
    തിരികെ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(അക്ഷരം *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            സ്ഥിരം അക്ഷരം *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    എങ്കിൽ (U_FAILURE(*status)) { തിരികെ 0; }
    എങ്കിൽ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    തിരികെ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
