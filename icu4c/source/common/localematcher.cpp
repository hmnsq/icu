// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव <optional>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localebuilder.h"
#अन्तर्भाव "unicode/localematcher.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "localeprioritylist.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "locdistance.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
गणना ULocMatchLifetime {
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
प्रकारव्याख्या गणना ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    जर (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = असत्य;
    }
}

LocaleMatcher::Result::~Result() {
    जर (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::चालक=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    हे->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    जर (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = असत्य;
    }
    परत *हे;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        परत Locale::get_मूळ();
    }
    स्थिरांक Locale *bestDesired = getDesiredLocale();
    जर (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        परत *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    स्थिरांक अक्षर *region = bestDesired->getCountry();
    जर (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    स्थिरांक अक्षर *variants = bestDesired->getVariant();
    जर (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    परत b.build(errorCode);
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
LocaleMatcher::Builder::चालक=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    हे->~Builder();

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
    परत *हे;
}

शून्य LocaleMatcher::Builder::clearSupportedLocales() {
    जर (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

तार्किक LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    जर (U_FAILURE(errorCode_)) { परत असत्य; }
    जर (supportedLocales_ != nullptr) { परत सत्य; }
    LocalPointer<UVector> lpSupportedLocales(नवीन UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    जर (U_FAILURE(errorCode_)) { परत असत्य; }
    supportedLocales_ = lpSupportedLocales.orphan();
    परत सत्य;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    clearSupportedLocales();
    जर (!ensureSupportedLocaleVector()) { परत *हे; }
    int32_t length = list.getLengthIncludingRemoved();
    साठी (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        जर (locale == nullptr) { सुरूठेवा; }
        supportedLocales_->adoptElement(locale, errorCode_);
        जर (U_FAILURE(errorCode_)) {
            थांबा;
        }
    }
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    जर (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        पर्यंत (locales.hasNext() && U_SUCCESS(errorCode_)) {
            स्थिरांक Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(स्थिरांक Locale& locale) U_LIFETIME_BOUND {
    जर (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    परत *हे;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = असत्य;
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(स्थिरांक Locale* defaultLocale) U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    Locale *clone = nullptr;
    जर (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        जर (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            परत *हे;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = सत्य;
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    favor_ = subtag;
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    demotion_ = demotion;
    परत *हे;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(स्थिरांक Locale& desired,
                                       स्थिरांक Locale& supported) U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    जर (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        परत *हे;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    परत *हे;
}

#जर 0
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
    जर (U_FAILURE(errorCode_)) { परत *हे; }
    जर (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    परत *हे;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) स्थिरांक {
    जर (U_FAILURE(outErrorCode)) { परत सत्य; }
    जर (U_SUCCESS(errorCode_)) { परत असत्य; }
    outErrorCode = errorCode_;
    परत सत्य;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) स्थिरांक {
    जर (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    परत LocaleMatcher(*हे, errorCode);
}

नामाकाश {

LSR getMaximalLsrOrUnd(स्थिरांक LikelySubtags &likelySubtags, स्थिरांक Locale &locale,
                       UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        परत UND_LSR;
    } अन्यथा {
        परत likelySubtags.makeMaximizedLsrFrom(locale, असत्य, errorCode);
    }
}

int32_t hashLSR(स्थिरांक UHashTok token) {
    स्थिरांक LSR *lsr = static_cast<स्थिरांक LSR *>(token.pointer);
    परत lsr->hashCode;
}

UBool compareLSRs(स्थिरांक UHashTok t1, स्थिरांक UHashTok t2) {
    स्थिरांक LSR *lsr1 = static_cast<स्थिरांक LSR *>(t1.pointer);
    स्थिरांक LSR *lsr2 = static_cast<स्थिरांक LSR *>(t2.pointer);
    परत *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(स्थिरांक LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत suppLength; }
    जर (!uhash_contains_किल्ली(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        जर (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    परत suppLength;
}

LocaleMatcher::LocaleMatcher(स्थिरांक Builder &builder, UErrorCode &errorCode) :
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
    जर (U_FAILURE(errorCode)) { परत; }
    स्थिरांक Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    स्थिरांक LSR *defLSR = nullptr;
    जर (def != nullptr) {
        ownedDefaultLocale = def->clone();
        जर (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    जर (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<स्थिरांक Locale **>(
            uprv_malloc(supportedLocalesLength * आकार(स्थिरांक Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नवीन LSR[supportedLocalesLength];
        जर (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * आकार(स्थिरांक Locale *));
        साठी (int32_t i = 0; i < supportedLocalesLength; ++i) {
            स्थिरांक Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            जर (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                परत;
            }
            स्थिरांक Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            जर (U_FAILURE(errorCode)) { परत; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        supportedLSRs = static_cast<स्थिरांक LSR **>(
            uprv_malloc(supportedLocalesLength * आकार(स्थिरांक LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * आकार(int32_t)));
        जर (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_चळत_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        जर (U_FAILURE(errorCode)) { परत; }
        int32_t अंक_Paradigms = 0;
        साठी (int32_t i = 0; i < supportedLocalesLength; ++i) {
            स्थिरांक Locale &locale = *supportedLocales[i];
            स्थिरांक LSR &lsr = lsrs[i];
            जर (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } अन्यथा जर (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } अन्यथा जर (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++अंक_Paradigms;
            } अन्यथा {
                order[i] = 3;
            }
            जर (U_FAILURE(errorCode)) { परत; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + अंक_Paradigms;
        साठी (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            जर (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        साठी (int32_t i = 0; i < supportedLocalesLength; ++i) {
            जर (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    जर (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    जर (thresholdDistance >= 0) {
        // already copied
    } अन्यथा जर (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        स्थिरांक LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        जर (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
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
    साठी (int32_t i = 0; i < supportedLocalesLength; ++i) {
        delete supportedLocales[i];
    }
    uprv_free(supportedLocales);
    delete[] lsrs;
    uhash_close(supportedLsrToIndex);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    delete ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::चालक=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    हे->~LocaleMatcher();

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
    परत *हे;
}

वर्ग LocaleLsrIterator {
सार्वजनिक:
    LocaleLsrIterator(स्थिरांक LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        जर (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    तार्किक hasNext() स्थिरांक {
        परत locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        परत getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    शून्य rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        जर (U_FAILURE(errorCode)) { परत; }
        bestDesiredIndex = desiredIndex;
        जर (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } अन्यथा {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = नवीन Locale(*current);
            जर (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    स्थिरांक Locale *orphanRemembered() {
        स्थिरांक Locale *rem = remembered;
        remembered = nullptr;
        परत rem;
    }

    int32_t getBestDesiredIndex() स्थिरांक {
        परत bestDesiredIndex;
    }

खाजगी:
    स्थिरांक LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    स्थिरांक Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

स्थिरांक Locale* LocaleMatcher::getBestMatch(स्थिरांक Locale& desiredLocale,
                                          UErrorCode& errorCode) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    परत U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

स्थिरांक Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    जर (!desiredLocales.hasNext()) {
        परत defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    परत U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

स्थिरांक Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode)) { परत nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    परत getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(स्थिरांक Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode)) {
        परत Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    जर (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        परत Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        परत Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, असत्य);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) स्थिरांक U_LIFETIME_BOUND {
    जर (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        परत Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    जर (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        परत Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        परत Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, सत्य);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    साठी (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        जर (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = असत्य;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            जर (found) {
                जर (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                परत suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        जर (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            जर (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                जर (U_FAILURE(errorCode)) { परत std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        जर ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            थांबा;
        }
        जर (remainingIter == nullptr || !remainingIter->hasNext()) {
            थांबा;
        }
        desiredLSR = remainingIter->next(errorCode);
        जर (U_FAILURE(errorCode)) { परत std::nullopt; }
        ++desiredIndex;
    }
    जर (bestSupportedLsrIndex < 0) {
        // no good match
        परत std::nullopt;
    }
    परत supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(स्थिरांक Locale &desired, स्थिरांक Locale &supported,
                             UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    जर (U_FAILURE(errorCode)) { परत असत्य; }
    स्थिरांक LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    परत indexAndDistance >= 0;
}

द्विगुण LocaleMatcher::internalMatch(स्थिरांक Locale &desired, स्थिरांक Locale &supported, UErrorCode &errorCode) स्थिरांक {
    जर (U_FAILURE(errorCode)) { परत 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    जर (U_FAILURE(errorCode)) { परत 0.; }
    स्थिरांक LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    द्विगुण distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    परत (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

वर्ग LocaleFromTag {
सार्वजनिक:
    LocaleFromTag() : locale(Locale::get_मूळ()) {}
    स्थिरांक Locale& चालक()(स्थिरांक अक्षर* tag) U_LIFETIME_BOUND { परत locale = Locale(tag); }

खाजगी:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       अक्षर *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    जर (U_FAILURE(errorCode)) { परत 0; }
    LocaleMatcher::Builder builder;
    स्थिरांक अक्षर *locString;
    पर्यंत ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        जर (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            परत 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    जर (U_FAILURE(errorCode)) { परत 0; }
    जर (result.getDesiredIndex() >= 0) {
        जर (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        स्थिरांक अक्षर *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        जर (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        परत u_terminateChars(dest, capacity, bestLength, &errorCode);
    } अन्यथा {
        जर (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        परत u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(अक्षर *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    स्थिरांक अक्षर **acceptList, int32_t accept_List_मोजणी,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    जर (U_FAILURE(*status)) { परत 0; }
    जर ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_मोजणी != 0 : accept_List_मोजणी < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<स्थिरांक अक्षर **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_मोजणी, converter);
    परत acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(अक्षर *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            स्थिरांक अक्षर *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    जर (U_FAILURE(*status)) { परत 0; }
    जर ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    परत acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
