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
    फर्काऊ *एतत्;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        फर्काऊ Locale::get_जरा();
    }
    अचर Locale *bestDesired = getDesiredLocale();
    यदि (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        फर्काऊ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    अचर वर्ण *region = bestDesired->getCountry();
    यदि (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    अचर वर्ण *variants = bestDesired->getVariant();
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
    फर्काऊ b.build(errorCode);
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
    फर्काऊ *एतत्;
}

शून्य LocaleMatcher::Builder::clearSupportedLocales() {
    यदि (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

तार्किक LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ असत्य; }
    यदि (supportedLocales_ != nullptr) { फर्काऊ सत्य; }
    LocalPointer<UVector> lpSupportedLocales(नूतन UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    यदि (U_FAILURE(errorCode_)) { फर्काऊ असत्य; }
    supportedLocales_ = lpSupportedLocales.orphan();
    फर्काऊ सत्य;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    clearSupportedLocales();
    यदि (!ensureSupportedLocaleVector()) { फर्काऊ *एतत्; }
    int32_t length = list.getLengthIncludingRemoved();
    लागि (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        यदि (locale == nullptr) { जारी; }
        supportedLocales_->adoptElement(locale, errorCode_);
        यदि (U_FAILURE(errorCode_)) {
            रोक;
        }
    }
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    यदि (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        जबसम्म (locales.hasNext() && U_SUCCESS(errorCode_)) {
            अचर Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(अचर Locale& locale) U_LIFETIME_BOUND {
    यदि (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = असत्य;
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(अचर Locale* defaultLocale) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    Locale *clone = nullptr;
    यदि (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        यदि (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ *एतत्;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = सत्य;
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    favor_ = subtag;
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    demotion_ = demotion;
    फर्काऊ *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(अचर Locale& desired,
                                       अचर Locale& supported) U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    यदि (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ *एतत्;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    फर्काऊ *एतत्;
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
    यदि (U_FAILURE(errorCode_)) { फर्काऊ *एतत्; }
    यदि (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    फर्काऊ *एतत्;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) अचर {
    यदि (U_FAILURE(outErrorCode)) { फर्काऊ सत्य; }
    यदि (U_SUCCESS(errorCode_)) { फर्काऊ असत्य; }
    outErrorCode = errorCode_;
    फर्काऊ सत्य;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) अचर {
    यदि (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    फर्काऊ LocaleMatcher(*एतत्, errorCode);
}

नामाकाश {

LSR getMaximalLsrOrUnd(अचर LikelySubtags &likelySubtags, अचर Locale &locale,
                       UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        फर्काऊ UND_LSR;
    } अन्यथा {
        फर्काऊ likelySubtags.makeMaximizedLsrFrom(locale, असत्य, errorCode);
    }
}

int32_t hashLSR(अचर UHashTok token) {
    अचर LSR *lsr = static_cast<अचर LSR *>(token.pointer);
    फर्काऊ lsr->hashCode;
}

UBool compareLSRs(अचर UHashTok t1, अचर UHashTok t2) {
    अचर LSR *lsr1 = static_cast<अचर LSR *>(t1.pointer);
    अचर LSR *lsr2 = static_cast<अचर LSR *>(t2.pointer);
    फर्काऊ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(अचर LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ suppLength; }
    यदि (!uhash_contains_साँचो(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        यदि (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    फर्काऊ suppLength;
}

LocaleMatcher::LocaleMatcher(अचर Builder &builder, UErrorCode &errorCode) :
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
    यदि (U_FAILURE(errorCode)) { फर्काऊ; }
    अचर Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    अचर LSR *defLSR = nullptr;
    यदि (def != nullptr) {
        ownedDefaultLocale = def->clone();
        यदि (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    यदि (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<अचर Locale **>(
            uprv_malloc(supportedLocalesLength * परिमाण(अचर Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नूतन LSR[supportedLocalesLength];
        यदि (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * परिमाण(अचर Locale *));
        लागि (int32_t i = 0; i < supportedLocalesLength; ++i) {
            अचर Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            यदि (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
            अचर Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        supportedLSRs = static_cast<अचर LSR **>(
            uprv_malloc(supportedLocalesLength * परिमाण(अचर LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * परिमाण(int32_t)));
        यदि (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_थाक_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        int32_t अङ्क_Paradigms = 0;
        लागि (int32_t i = 0; i < supportedLocalesLength; ++i) {
            अचर Locale &locale = *supportedLocales[i];
            अचर LSR &lsr = lsrs[i];
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
                ++अङ्क_Paradigms;
            } अन्यथा {
                order[i] = 3;
            }
            यदि (U_FAILURE(errorCode)) { फर्काऊ; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + अङ्क_Paradigms;
        लागि (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            यदि (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        लागि (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
        अचर LSR *pSuppLSR = &suppLSR;
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
    लागि (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    फर्काऊ *एतत्;
}

वर्ग LocaleLsrIterator {
सार्वजनिक:
    LocaleLsrIterator(अचर LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        यदि (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    तार्किक hasNext() अचर {
        फर्काऊ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        फर्काऊ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    शून्य rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        यदि (U_FAILURE(errorCode)) { फर्काऊ; }
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

    अचर Locale *orphanRemembered() {
        अचर Locale *rem = remembered;
        remembered = nullptr;
        फर्काऊ rem;
    }

    int32_t getBestDesiredIndex() अचर {
        फर्काऊ bestDesiredIndex;
    }

गुप्त:
    अचर LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    अचर Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

अचर Locale* LocaleMatcher::getBestMatch(अचर Locale& desiredLocale,
                                          UErrorCode& errorCode) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    फर्काऊ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

अचर Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    यदि (!desiredLocales.hasNext()) {
        फर्काऊ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    फर्काऊ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

अचर Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    फर्काऊ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(अचर Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode)) {
        फर्काऊ Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    यदि (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        फर्काऊ Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        फर्काऊ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, असत्य);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) अचर U_LIFETIME_BOUND {
    यदि (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        फर्काऊ Result(nullptr, defaultLocale, -1, -1, असत्य);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    यदि (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        फर्काऊ Result(nullptr, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        फर्काऊ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, सत्य);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode)) { फर्काऊ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    लागि (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        यदि (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = असत्य;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            यदि (found) {
                यदि (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                फर्काऊ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        यदि (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            यदि (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                यदि (U_FAILURE(errorCode)) { फर्काऊ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        यदि ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            रोक;
        }
        यदि (remainingIter == nullptr || !remainingIter->hasNext()) {
            रोक;
        }
        desiredLSR = remainingIter->next(errorCode);
        यदि (U_FAILURE(errorCode)) { फर्काऊ std::nullopt; }
        ++desiredIndex;
    }
    यदि (bestSupportedLsrIndex < 0) {
        // no good match
        फर्काऊ std::nullopt;
    }
    फर्काऊ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(अचर Locale &desired, अचर Locale &supported,
                             UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ असत्य; }
    अचर LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    फर्काऊ indexAndDistance >= 0;
}

द्विगुण LocaleMatcher::internalMatch(अचर Locale &desired, अचर Locale &supported, UErrorCode &errorCode) अचर {
    यदि (U_FAILURE(errorCode)) { फर्काऊ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ 0.; }
    अचर LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    द्विगुण distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    फर्काऊ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

वर्ग LocaleFromTag {
सार्वजनिक:
    LocaleFromTag() : locale(Locale::get_जरा()) {}
    अचर Locale& चालक()(अचर वर्ण* tag) U_LIFETIME_BOUND { फर्काऊ locale = Locale(tag); }

गुप्त:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       वर्ण *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    यदि (U_FAILURE(errorCode)) { फर्काऊ 0; }
    LocaleMatcher::Builder builder;
    अचर वर्ण *locString;
    जबसम्म ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        यदि (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            फर्काऊ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    यदि (U_FAILURE(errorCode)) { फर्काऊ 0; }
    यदि (result.getDesiredIndex() >= 0) {
        यदि (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        अचर वर्ण *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        यदि (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        फर्काऊ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } अन्यथा {
        यदि (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        फर्काऊ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(वर्ण *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    अचर वर्ण **acceptList, int32_t accept_List_गन्ती,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    यदि (U_FAILURE(*status)) { फर्काऊ 0; }
    यदि ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_गन्ती != 0 : accept_List_गन्ती < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<अचर वर्ण **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_गन्ती, converter);
    फर्काऊ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(वर्ण *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            अचर वर्ण *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    यदि (U_FAILURE(*status)) { फर्काऊ 0; }
    यदि ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    फर्काऊ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
