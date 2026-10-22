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
    सचे (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = मिच्छा;
    }
}

LocaleMatcher::Result::~Result() {
    सचे (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    एतत्->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    सचे (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = मिच्छा;
    }
    पटिनिवत्त *एतत्;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        पटिनिवत्त Locale::get_मूलम्();
    }
    ध्रुव Locale *bestDesired = getDesiredLocale();
    सचे (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        पटिनिवत्त *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ध्रुव वर्ण *region = bestDesired->getCountry();
    सचे (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ध्रुव वर्ण *variants = bestDesired->getVariant();
    सचे (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    पटिनिवत्त b.build(errorCode);
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
    पटिनिवत्त *एतत्;
}

शून्य LocaleMatcher::Builder::clearSupportedLocales() {
    सचे (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

तार्किक LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त मिच्छा; }
    सचे (supportedLocales_ != nullptr) { पटिनिवत्त सच्च; }
    LocalPointer<UVector> lpSupportedLocales(नूतन UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त मिच्छा; }
    supportedLocales_ = lpSupportedLocales.orphan();
    पटिनिवत्त सच्च;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    clearSupportedLocales();
    सचे (!ensureSupportedLocaleVector()) { पटिनिवत्त *एतत्; }
    int32_t length = list.getLengthIncludingRemoved();
    अत्थाय (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        सचे (locale == nullptr) { अनुवर्त; }
        supportedLocales_->adoptElement(locale, errorCode_);
        सचे (U_FAILURE(errorCode_)) {
            भङ्ग;
        }
    }
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    सचे (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        याव (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ध्रुव Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ध्रुव Locale& locale) U_LIFETIME_BOUND {
    सचे (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = मिच्छा;
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ध्रुव Locale* defaultLocale) U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    Locale *clone = nullptr;
    सचे (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        सचे (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त *एतत्;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = सच्च;
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    favor_ = subtag;
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    demotion_ = demotion;
    पटिनिवत्त *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ध्रुव Locale& desired,
                                       ध्रुव Locale& supported) U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    सचे (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त *एतत्;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    पटिनिवत्त *एतत्;
}

#सचे 0
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
    सचे (U_FAILURE(errorCode_)) { पटिनिवत्त *एतत्; }
    सचे (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    पटिनिवत्त *एतत्;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ध्रुव {
    सचे (U_FAILURE(outErrorCode)) { पटिनिवत्त सच्च; }
    सचे (U_SUCCESS(errorCode_)) { पटिनिवत्त मिच्छा; }
    outErrorCode = errorCode_;
    पटिनिवत्त सच्च;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ध्रुव {
    सचे (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    पटिनिवत्त LocaleMatcher(*एतत्, errorCode);
}

नामाकाश {

LSR getMaximalLsrOrUnd(ध्रुव LikelySubtags &likelySubtags, ध्रुव Locale &locale,
                       UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        पटिनिवत्त UND_LSR;
    } अञ्ञथा {
        पटिनिवत्त likelySubtags.makeMaximizedLsrFrom(locale, मिच्छा, errorCode);
    }
}

int32_t hashLSR(ध्रुव UHashTok token) {
    ध्रुव LSR *lsr = static_cast<ध्रुव LSR *>(token.pointer);
    पटिनिवत्त lsr->hashCode;
}

UBool compareLSRs(ध्रुव UHashTok t1, ध्रुव UHashTok t2) {
    ध्रुव LSR *lsr1 = static_cast<ध्रुव LSR *>(t1.pointer);
    ध्रुव LSR *lsr2 = static_cast<ध्रुव LSR *>(t2.pointer);
    पटिनिवत्त *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ध्रुव LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त suppLength; }
    सचे (!uhash_contains_कुञ्चिका(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        सचे (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    पटिनिवत्त suppLength;
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
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
    ध्रुव Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ध्रुव LSR *defLSR = nullptr;
    सचे (def != nullptr) {
        ownedDefaultLocale = def->clone();
        सचे (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    सचे (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ध्रुव Locale **>(
            uprv_malloc(supportedLocalesLength * परिमाण(ध्रुव Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नूतन LSR[supportedLocalesLength];
        सचे (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * परिमाण(ध्रुव Locale *));
        अत्थाय (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ध्रुव Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            सचे (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            ध्रुव Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        supportedLSRs = static_cast<ध्रुव LSR **>(
            uprv_malloc(supportedLocalesLength * परिमाण(ध्रुव LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * परिमाण(int32_t)));
        सचे (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_राशिः_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        int32_t अङ्कः_Paradigms = 0;
        अत्थाय (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ध्रुव Locale &locale = *supportedLocales[i];
            ध्रुव LSR &lsr = lsrs[i];
            सचे (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } अञ्ञथा सचे (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } अञ्ञथा सचे (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++अङ्कः_Paradigms;
            } अञ्ञथा {
                order[i] = 3;
            }
            सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + अङ्कः_Paradigms;
        अत्थाय (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            सचे (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        अत्थाय (int32_t i = 0; i < supportedLocalesLength; ++i) {
            सचे (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    सचे (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    सचे (thresholdDistance >= 0) {
        // already copied
    } अञ्ञथा सचे (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ध्रुव LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        सचे (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } अञ्ञथा {
            thresholdDistance = 0;
        }
    } अञ्ञथा {
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
    अत्थाय (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    पटिनिवत्त *एतत्;
}

वग्ग LocaleLsrIterator {
सार्वजनिक:
    LocaleLsrIterator(ध्रुव LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        सचे (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    तार्किक hasNext() ध्रुव {
        पटिनिवत्त locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        पटिनिवत्त getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    शून्य rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त; }
        bestDesiredIndex = desiredIndex;
        सचे (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } अञ्ञथा {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = नूतन Locale(*current);
            सचे (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ध्रुव Locale *orphanRemembered() {
        ध्रुव Locale *rem = remembered;
        remembered = nullptr;
        पटिनिवत्त rem;
    }

    int32_t getBestDesiredIndex() ध्रुव {
        पटिनिवत्त bestDesiredIndex;
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
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    पटिनिवत्त U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ध्रुव Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    सचे (!desiredLocales.hasNext()) {
        पटिनिवत्त defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    पटिनिवत्त U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ध्रुव Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    पटिनिवत्त getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ध्रुव Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode)) {
        पटिनिवत्त Result(nullptr, defaultLocale, -1, -1, मिच्छा);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    सचे (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        पटिनिवत्त Result(nullptr, defaultLocale, -1, -1, मिच्छा);
    } अञ्ञथा {
        पटिनिवत्त Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, मिच्छा);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ध्रुव U_LIFETIME_BOUND {
    सचे (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        पटिनिवत्त Result(nullptr, defaultLocale, -1, -1, मिच्छा);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    सचे (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        पटिनिवत्त Result(nullptr, defaultLocale, -1, -1, मिच्छा);
    } अञ्ञथा {
        पटिनिवत्त Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, सच्च);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    अत्थाय (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        सचे (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = मिच्छा;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            सचे (found) {
                सचे (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                पटिनिवत्त suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        सचे (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            सचे (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                सचे (U_FAILURE(errorCode)) { पटिनिवत्त std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        सचे ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            भङ्ग;
        }
        सचे (remainingIter == nullptr || !remainingIter->hasNext()) {
            भङ्ग;
        }
        desiredLSR = remainingIter->next(errorCode);
        सचे (U_FAILURE(errorCode)) { पटिनिवत्त std::nullopt; }
        ++desiredIndex;
    }
    सचे (bestSupportedLsrIndex < 0) {
        // no good match
        पटिनिवत्त std::nullopt;
    }
    पटिनिवत्त supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ध्रुव Locale &desired, ध्रुव Locale &supported,
                             UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त मिच्छा; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    पटिनिवत्त indexAndDistance >= 0;
}

द्विगुण LocaleMatcher::internalMatch(ध्रुव Locale &desired, ध्रुव Locale &supported, UErrorCode &errorCode) ध्रुव {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त 0.; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    द्विगुण distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    पटिनिवत्त (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

वग्ग LocaleFromTag {
सार्वजनिक:
    LocaleFromTag() : locale(Locale::get_मूलम्()) {}
    ध्रुव Locale& operator()(ध्रुव वर्ण* tag) U_LIFETIME_BOUND { पटिनिवत्त locale = Locale(tag); }

गुप्त:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       वर्ण *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त 0; }
    LocaleMatcher::Builder builder;
    ध्रुव वर्ण *locString;
    याव ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        सचे (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            पटिनिवत्त 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    सचे (U_FAILURE(errorCode)) { पटिनिवत्त 0; }
    सचे (result.getDesiredIndex() >= 0) {
        सचे (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ध्रुव वर्ण *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        सचे (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        पटिनिवत्त u_terminateChars(dest, capacity, bestLength, &errorCode);
    } अञ्ञथा {
        सचे (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        पटिनिवत्त u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(वर्ण *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ध्रुव वर्ण **acceptList, int32_t accept_List_गणनम्,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    सचे (U_FAILURE(*status)) { पटिनिवत्त 0; }
    सचे ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_गणनम् != 0 : accept_List_गणनम् < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ध्रुव वर्ण **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_गणनम्, converter);
    पटिनिवत्त acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(वर्ण *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ध्रुव वर्ण *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    सचे (U_FAILURE(*status)) { पटिनिवत्त 0; }
    सचे ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    पटिनिवत्त acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
