// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#ضمن <optional>

#ضمن "unicode/utypes.h"
#ضمن "unicode/localebuilder.h"
#ضمن "unicode/localematcher.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/stringpiece.h"
#ضمن "unicode/uloc.h"
#ضمن "unicode/uobject.h"
#ضمن "cstring.h"
#ضمن "localeprioritylist.h"
#ضمن "loclikelysubtags.h"
#ضمن "locdistance.h"
#ضمن "lsr.h"
#ضمن "uassert.h"
#ضمن "uhash.h"
#ضمن "ustr_imp.h"
#ضمن "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
تعداد ULocMatchLifetime {
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
تعريفنوع تعداد ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    إذا (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::Result::~Result() {
    إذا (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    هذا->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    إذا (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = خطأ;
    }
    إرجاع *هذا;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        إرجاع Locale::get_جذر();
    }
    ثابت Locale *bestDesired = getDesiredLocale();
    إذا (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        إرجاع *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = bestDesired->getCountry();
    إذا (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = bestDesired->getVariant();
    إذا (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    إرجاع b.build(errorCode);
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
    هذا->~Builder();

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
    إرجاع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    إذا (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    إذا (U_FAILURE(errorCode_)) { إرجاع خطأ; }
    إذا (supportedLocales_ != nullptr) { إرجاع true; }
    LocalPointer<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    إذا (U_FAILURE(errorCode_)) { إرجاع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    إرجاع true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    clearSupportedLocales();
    إذا (!ensureSupportedLocaleVector()) { إرجاع *هذا; }
    int32_t length = list.getLengthIncludingRemoved();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        إذا (locale == nullptr) { استمر; }
        supportedLocales_->adoptElement(locale, errorCode_);
        إذا (U_FAILURE(errorCode_)) {
            اكسر;
        }
    }
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    إذا (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        بينما (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ثابت Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ثابت Locale& locale) U_LIFETIME_BOUND {
    إذا (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    إرجاع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = خطأ;
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    Locale *clone = nullptr;
    إذا (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        إذا (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            إرجاع *هذا;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    favor_ = subtag;
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    demotion_ = demotion;
    إرجاع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    إذا (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        إرجاع *هذا;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    إرجاع *هذا;
}

#إذا 0
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
    إذا (U_FAILURE(errorCode_)) { إرجاع *هذا; }
    إذا (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    إرجاع *هذا;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    إذا (U_FAILURE(outErrorCode)) { إرجاع true; }
    إذا (U_SUCCESS(errorCode_)) { إرجاع خطأ; }
    outErrorCode = errorCode_;
    إرجاع true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ثابت {
    إذا (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    إرجاع LocaleMatcher(*هذا, errorCode);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        إرجاع UND_LSR;
    } وإلا {
        إرجاع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, errorCode);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.pointer);
    إرجاع lsr->hashCode;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.pointer);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.pointer);
    إرجاع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { إرجاع suppLength; }
    إذا (!uhash_contains_مفتاح(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        إذا (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    إرجاع suppLength;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UErrorCode &errorCode) :
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
    إذا (U_FAILURE(errorCode)) { إرجاع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = nullptr;
    إذا (def != nullptr) {
        ownedDefaultLocale = def->clone();
        إذا (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            إرجاع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        إذا (U_FAILURE(errorCode)) { إرجاع; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    إذا (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supportedLocalesLength];
        إذا (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            إرجاع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            إذا (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                إرجاع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            إذا (U_FAILURE(errorCode)) { إرجاع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        إذا (U_FAILURE(errorCode)) { إرجاع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * حجم(int32_t)));
        إذا (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            إرجاع;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_مكدس_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        إذا (U_FAILURE(errorCode)) { إرجاع; }
        int32_t رقم_Paradigms = 0;
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            إذا (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } وإلا إذا (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } وإلا إذا (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++رقم_Paradigms;
            } وإلا {
                order[i] = 3;
            }
            إذا (U_FAILURE(errorCode)) { إرجاع; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + رقم_Paradigms;
        لكل (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            إذا (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            إذا (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    إذا (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    إذا (thresholdDistance >= 0) {
        // already copied
    } وإلا إذا (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        إذا (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } وإلا {
            thresholdDistance = 0;
        }
    } وإلا {
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
    لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    هذا->~LocaleMatcher();

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
    إرجاع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        إذا (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    منطقي hasNext() ثابت {
        إرجاع locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        إرجاع getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    فراغ rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        إذا (U_FAILURE(errorCode)) { إرجاع; }
        bestDesiredIndex = desiredIndex;
        إذا (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = جديد Locale(*current);
            إذا (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = nullptr;
        إرجاع rem;
    }

    int32_t getBestDesiredIndex() ثابت {
        إرجاع bestDesiredIndex;
    }

خاص:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ثابت Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ثابت Locale* LocaleMatcher::getBestMatch(ثابت Locale& desiredLocale,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode)) { إرجاع nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    إرجاع U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode)) { إرجاع nullptr; }
    إذا (!desiredLocales.hasNext()) {
        إرجاع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    إرجاع U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode)) { إرجاع nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    إرجاع getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode)) {
        إرجاع Result(nullptr, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    إذا (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        إرجاع Result(nullptr, defaultLocale, -1, -1, خطأ);
    } وإلا {
        إرجاع Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, خطأ);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        إرجاع Result(nullptr, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    إذا (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        إرجاع Result(nullptr, defaultLocale, -1, -1, خطأ);
    } وإلا {
        إرجاع Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, true);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { إرجاع std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    لكل (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        إذا (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = خطأ;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            إذا (found) {
                إذا (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                إرجاع suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        إذا (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            إذا (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                إذا (U_FAILURE(errorCode)) { إرجاع std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        إذا ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            اكسر;
        }
        إذا (remainingIter == nullptr || !remainingIter->hasNext()) {
            اكسر;
        }
        desiredLSR = remainingIter->next(errorCode);
        إذا (U_FAILURE(errorCode)) { إرجاع std::nullopt; }
        ++desiredIndex;
    }
    إذا (bestSupportedLsrIndex < 0) {
        // no good match
        إرجاع std::nullopt;
    }
    إرجاع supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ثابت Locale &desired, ثابت Locale &supported,
                             UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { إرجاع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    إذا (U_FAILURE(errorCode)) { إرجاع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    إرجاع indexAndDistance >= 0;
}

مزدوج LocaleMatcher::internalMatch(ثابت Locale &desired, ثابت Locale &supported, UErrorCode &errorCode) ثابت {
    إذا (U_FAILURE(errorCode)) { إرجاع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    إذا (U_FAILURE(errorCode)) { إرجاع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    مزدوج distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    إرجاع (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف LocaleFromTag {
عام:
    LocaleFromTag() : locale(Locale::get_جذر()) {}
    ثابت Locale& operator()(ثابت حرف* tag) U_LIFETIME_BOUND { إرجاع locale = Locale(tag); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    إذا (U_FAILURE(errorCode)) { إرجاع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    بينما ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        إذا (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            إرجاع 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    إذا (U_FAILURE(errorCode)) { إرجاع 0; }
    إذا (result.getDesiredIndex() >= 0) {
        إذا (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        إذا (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        إرجاع u_terminateChars(dest, capacity, bestLength, &errorCode);
    } وإلا {
        إذا (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        إرجاع u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ثابت حرف **acceptList, int32_t accept_List_تعداد,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    إذا (U_FAILURE(*status)) { إرجاع 0; }
    إذا ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_تعداد != 0 : accept_List_تعداد < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ثابت حرف **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_تعداد, converter);
    إرجاع acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    إذا (U_FAILURE(*status)) { إرجاع 0; }
    إذا ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    إرجاع acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
