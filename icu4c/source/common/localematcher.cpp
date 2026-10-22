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
प्रकारनिर्देश गणना ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    यदि (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = असत्य;
    }
}

LocaleMatcher::Result::~Result() {
    यदि (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::चालक=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    एतत्->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    यदि (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = असत्य;
    }
    प्रत्यागम *एतत्;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        प्रत्यागम Locale::get_मूल();
    }
    ध्रुव Locale *bestDesired = getDesiredLocale();
    यदि (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        प्रत्यागम *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ध्रुव वर्ण *region = bestDesired->getCountry();
    यदि (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ध्रुव वर्ण *variants = bestDesired->getVariant();
    यदि (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    प्रत्यागम b.build(errorCode);
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
    एतत्->~Builder();

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
    प्रत्यागम *एतत्;
}

शून्य LocaleMatcher::Builder::clearSupportedLocales() {
    यदि (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

तार्किक LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम असत्य; }
    यदि (supportedLocales_ != nullptr) { प्रत्यागम सत्य; }
    LocalPointer<UVector> lpSupportedLocales(नूतन UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम असत्य; }
    supportedLocales_ = lpSupportedLocales.orphan();
    प्रत्यागम सत्य;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    clearSupportedLocales();
    यदि (!ensureSupportedLocaleVector()) { प्रत्यागम *एतत्; }
    int32_t length = list.getLengthIncludingRemoved();
    प्रति (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        यदि (locale == nullptr) { अनुवर्त; }
        supportedLocales_->adoptElement(locale, errorCode_);
        यदि (U_FAILURE(errorCode_)) {
            भङ्ग;
        }
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    यदि (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        यावत् (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ध्रुव Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ध्रुव Locale& locale) U_LIFETIME_BOUND {
    यदि (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = असत्य;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ध्रुव Locale* defaultLocale) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    Locale *clone = nullptr;
    यदि (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        यदि (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = सत्य;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    favor_ = subtag;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    demotion_ = demotion;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ध्रुव Locale& desired,
                                       ध्रुव Locale& supported) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    यदि (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम *एतत्;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    प्रत्यागम *एतत्;
}

#यदि 0
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
    यदि (U_FAILURE(errorCode_)) { प्रत्यागम *एतत्; }
    यदि (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    प्रत्यागम *एतत्;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    यदि (U_FAILURE(outErrorCode)) { प्रत्यागम सत्य; }
    यदि (U_SUCCESS(errorCode_)) { प्रत्यागम असत्य; }
    outErrorCode = errorCode_;
    प्रत्यागम सत्य;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ध्रुव {
    यदि (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    प्रत्यागम LocaleMatcher(*एतत्, errorCode);
}

नामाकाश {

LSR getMaximalLsrOrUnd(ध्रुव LikelySubtags &likelySubtags, ध्रुव Locale &locale,
                       UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        प्रत्यागम UND_LSR;
    } अन्यथा {
        प्रत्यागम likelySubtags.makeMaximizedLsrFrom(locale, असत्य, errorCode);
    }
}

int32_t hashLSR(ध्रुव UHashTok token) {
    ध्रुव LSR *lsr = static_cast<ध्रुव LSR *>(token.pointer);
    प्रत्यागम lsr->hashCode;
}

UBool compareLSRs(ध्रुव UHashTok t1, ध्रुव UHashTok t2) {
    ध्रुव LSR *lsr1 = static_cast<ध्रुव LSR *>(t1.pointer);
    ध्रुव LSR *lsr2 = static_cast<ध्रुव LSR *>(t2.pointer);
    प्रत्यागम *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ध्रुव LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम suppLength; }
    यदि (!uhash_contains_कुंजी(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        यदि (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    प्रत्यागम suppLength;
}

LocaleMatcher::LocaleMatcher(ध्रुव Builder &builder, UErrorCode &errorCode) :
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
    यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
    ध्रुव Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ध्रुव LSR *defLSR = nullptr;
    यदि (def != nullptr) {
        ownedDefaultLocale = def->clone();
        यदि (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    यदि (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ध्रुव Locale **>(
            uprv_malloc(supportedLocalesLength * परिमाण(ध्रुव Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नूतन LSR[supportedLocalesLength];
        यदि (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * परिमाण(ध्रुव Locale *));
        प्रति (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ध्रुव Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            यदि (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                प्रत्यागम;
            }
            ध्रुव Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        supportedLSRs = static_cast<ध्रुव LSR **>(
            uprv_malloc(supportedLocalesLength * परिमाण(ध्रुव LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * परिमाण(int32_t)));
        यदि (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ढेर_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        int32_t अंक_Paradigms = 0;
        प्रति (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ध्रुव Locale &locale = *supportedLocales[i];
            ध्रुव LSR &lsr = lsrs[i];
            यदि (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } अन्यथा यदि (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } अन्यथा यदि (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++अंक_Paradigms;
            } अन्यथा {
                order[i] = 3;
            }
            यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + अंक_Paradigms;
        प्रति (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            यदि (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        प्रति (int32_t i = 0; i < supportedLocalesLength; ++i) {
            यदि (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    यदि (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    यदि (thresholdDistance >= 0) {
        // already copied
    } अन्यथा यदि (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ध्रुव LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        यदि (U_SUCCESS(errorCode)) {
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
    प्रति (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    एतत्->~LocaleMatcher();

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
    प्रत्यागम *एतत्;
}

वर्ग LocaleLsrIterator {
सार्वजनिक:
    LocaleLsrIterator(ध्रुव LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        यदि (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    तार्किक hasNext() ध्रुव {
        प्रत्यागम locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        प्रत्यागम getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    शून्य rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode)) { प्रत्यागम; }
        bestDesiredIndex = desiredIndex;
        यदि (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } अन्यथा {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = नूतन Locale(*current);
            यदि (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ध्रुव Locale *orphanRemembered() {
        ध्रुव Locale *rem = remembered;
        remembered = nullptr;
        प्रत्यागम rem;
    }

    int32_t getBestDesiredIndex() ध्रुव {
        प्रत्यागम bestDesiredIndex;
    }

गुप्त:
    ध्रुव LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ध्रुव Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ध्रुव Locale* LocaleMatcher::getBestMatch(ध्रुव Locale& desiredLocale,
                                          UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    प्रत्यागम U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ध्रुव Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    यदि (!desiredLocales.hasNext()) {
        प्रत्यागम defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    प्रत्यागम U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ध्रुव Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    प्रत्यागम getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ध्रुव Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) {
        प्रत्यागम Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    यदि (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        प्रत्यागम Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        प्रत्यागम Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, असत्य);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        प्रत्यागम Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    यदि (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        प्रत्यागम Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        प्रत्यागम Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, सत्य);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    प्रति (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        यदि (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = असत्य;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            यदि (found) {
                यदि (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                प्रत्यागम suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        यदि (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            यदि (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                यदि (U_FAILURE(errorCode)) { प्रत्यागम std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        यदि ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            भङ्ग;
        }
        यदि (remainingIter == nullptr || !remainingIter->hasNext()) {
            भङ्ग;
        }
        desiredLSR = remainingIter->next(errorCode);
        यदि (U_FAILURE(errorCode)) { प्रत्यागम std::nullopt; }
        ++desiredIndex;
    }
    यदि (bestSupportedLsrIndex < 0) {
        // no good match
        प्रत्यागम std::nullopt;
    }
    प्रत्यागम supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ध्रुव Locale &desired, ध्रुव Locale &supported,
                             UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम असत्य; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    प्रत्यागम indexAndDistance >= 0;
}

द्विगुण LocaleMatcher::internalMatch(ध्रुव Locale &desired, ध्रुव Locale &supported, UErrorCode &errorCode) ध्रुव {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0.; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    द्विगुण distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    प्रत्यागम (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

वर्ग LocaleFromTag {
सार्वजनिक:
    LocaleFromTag() : locale(Locale::get_मूल()) {}
    ध्रुव Locale& चालक()(ध्रुव वर्ण* tag) U_LIFETIME_BOUND { प्रत्यागम locale = Locale(tag); }

गुप्त:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       वर्ण *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0; }
    LocaleMatcher::Builder builder;
    ध्रुव वर्ण *locString;
    यावत् ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        यदि (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            प्रत्यागम 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    यदि (U_FAILURE(errorCode)) { प्रत्यागम 0; }
    यदि (result.getDesiredIndex() >= 0) {
        यदि (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ध्रुव वर्ण *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        यदि (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        प्रत्यागम u_terminateChars(dest, capacity, bestLength, &errorCode);
    } अन्यथा {
        यदि (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        प्रत्यागम u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(वर्ण *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ध्रुव वर्ण **acceptList, int32_t accept_List_गिनती,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    यदि (U_FAILURE(*status)) { प्रत्यागम 0; }
    यदि ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_गिनती != 0 : accept_List_गिनती < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ध्रुव वर्ण **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_गिनती, converter);
    प्रत्यागम acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(वर्ण *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ध्रुव वर्ण *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    यदि (U_FAILURE(*status)) { प्रत्यागम 0; }
    यदि ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    प्रत्यागम acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
