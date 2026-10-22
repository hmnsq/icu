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
    אם (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = שקר;
    }
}

LocaleMatcher::Result::~Result() {
    אם (desiredIsOwned) {
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

    אם (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = שקר;
    }
    החזר *هذا;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        החזר Locale::getRoot();
    }
    ثابت Locale *bestDesired = getDesiredLocale();
    אם (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        החזר *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = bestDesired->getCountry();
    אם (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = bestDesired->getVariant();
    אם (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    החזר b.build(errorCode);
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
    החזר *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    אם (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    אם (U_FAILURE(errorCode_)) { החזר שקר; }
    אם (supportedLocales_ != nullptr) { החזר אמת; }
    LocalPointer<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    אם (U_FAILURE(errorCode_)) { החזר שקר; }
    supportedLocales_ = lpSupportedLocales.orphan();
    החזר אמת;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    clearSupportedLocales();
    אם (!ensureSupportedLocaleVector()) { החזר *هذا; }
    int32_t length = list.getLengthIncludingRemoved();
    עבור (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        אם (locale == nullptr) { استمر; }
        supportedLocales_->adoptElement(locale, errorCode_);
        אם (U_FAILURE(errorCode_)) {
            اكسر;
        }
    }
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    אם (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        כלעוד (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ثابت Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ثابت Locale& locale) U_LIFETIME_BOUND {
    אם (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    החזר *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = שקר;
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    Locale *clone = nullptr;
    אם (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        אם (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            החזר *هذا;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = אמת;
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    favor_ = subtag;
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    demotion_ = demotion;
    החזר *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    אם (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        החזר *هذا;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    החזר *هذا;
}

#אם 0
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
    אם (U_FAILURE(errorCode_)) { החזר *هذا; }
    אם (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    החזר *هذا;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    אם (U_FAILURE(outErrorCode)) { החזר אמת; }
    אם (U_SUCCESS(errorCode_)) { החזר שקר; }
    outErrorCode = errorCode_;
    החזר אמת;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ثابت {
    אם (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    החזר LocaleMatcher(*هذا, errorCode);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UErrorCode &errorCode) {
    אם (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        החזר UND_LSR;
    } אחרת {
        החזר likelySubtags.makeMaximizedLsrFrom(locale, שקר, errorCode);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.pointer);
    החזר lsr->hashCode;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.pointer);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.pointer);
    החזר *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    אם (U_FAILURE(errorCode)) { החזר suppLength; }
    אם (!uhash_containsKey(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        אם (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    החזר suppLength;
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
    אם (U_FAILURE(errorCode)) { החזר; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = nullptr;
    אם (def != nullptr) {
        ownedDefaultLocale = def->clone();
        אם (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            החזר;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    אם (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supportedLocalesLength];
        אם (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            החזר;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * حجم(ثابت Locale *));
        עבור (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            אם (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                החזר;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            אם (U_FAILURE(errorCode)) { החזר; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * حجم(int32_t)));
        אם (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            החזר;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        MaybeStackArray<int8_t, 100> order(supportedLocalesLength, errorCode);
        אם (U_FAILURE(errorCode)) { החזר; }
        int32_t numParadigms = 0;
        עבור (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            אם (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } אחרת אם (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } אחרת אם (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++numParadigms;
            } אחרת {
                order[i] = 3;
            }
            אם (U_FAILURE(errorCode)) { החזר; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + numParadigms;
        עבור (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            אם (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        עבור (int32_t i = 0; i < supportedLocalesLength; ++i) {
            אם (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    אם (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    אם (thresholdDistance >= 0) {
        // already copied
    } אחרת אם (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        אם (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } אחרת {
            thresholdDistance = 0;
        }
    } אחרת {
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
    עבור (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    החזר *هذا;
}

מחלקה LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        אם (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    منطقي hasNext() ثابت {
        החזר locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        החזר getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    فراغ rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        אם (U_FAILURE(errorCode)) { החזר; }
        bestDesiredIndex = desiredIndex;
        אם (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } אחרת {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = جديد Locale(*current);
            אם (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = nullptr;
        החזר rem;
    }

    int32_t getBestDesiredIndex() ثابت {
        החזר bestDesiredIndex;
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
    אם (U_FAILURE(errorCode)) { החזר nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    החזר U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode)) { החזר nullptr; }
    אם (!desiredLocales.hasNext()) {
        החזר defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    החזר U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode)) { החזר nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    החזר getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode)) {
        החזר Result(nullptr, defaultLocale, -1, -1, שקר);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    אם (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        החזר Result(nullptr, defaultLocale, -1, -1, שקר);
    } אחרת {
        החזר Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, שקר);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    אם (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        החזר Result(nullptr, defaultLocale, -1, -1, שקר);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    אם (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        החזר Result(nullptr, defaultLocale, -1, -1, שקר);
    } אחרת {
        החזר Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, אמת);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode)) { החזר std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    עבור (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        אם (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = שקר;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            אם (found) {
                אם (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                החזר suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        אם (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            אם (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                אם (U_FAILURE(errorCode)) { החזר std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        אם ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            اكسر;
        }
        אם (remainingIter == nullptr || !remainingIter->hasNext()) {
            اكسر;
        }
        desiredLSR = remainingIter->next(errorCode);
        אם (U_FAILURE(errorCode)) { החזר std::nullopt; }
        ++desiredIndex;
    }
    אם (bestSupportedLsrIndex < 0) {
        // no good match
        החזר std::nullopt;
    }
    החזר supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ثابت Locale &desired, ثابت Locale &supported,
                             UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode)) { החזר שקר; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    אם (U_FAILURE(errorCode)) { החזר שקר; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    החזר indexAndDistance >= 0;
}

مزدوج LocaleMatcher::internalMatch(ثابت Locale &desired, ثابت Locale &supported, UErrorCode &errorCode) ثابت {
    אם (U_FAILURE(errorCode)) { החזר 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    אם (U_FAILURE(errorCode)) { החזר 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    مزدوج distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    החזר (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

מחלקה LocaleFromTag {
عام:
    LocaleFromTag() : locale(Locale::getRoot()) {}
    ثابت Locale& operator()(ثابت حرف* tag) U_LIFETIME_BOUND { החזר locale = Locale(tag); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    אם (U_FAILURE(errorCode)) { החזר 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    כלעוד ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        אם (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            החזר 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    אם (U_FAILURE(errorCode)) { החזר 0; }
    אם (result.getDesiredIndex() >= 0) {
        אם (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        אם (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        החזר u_terminateChars(dest, capacity, bestLength, &errorCode);
    } אחרת {
        אם (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        החזר u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ثابت حرف **acceptList, int32_t acceptListCount,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    אם (U_FAILURE(*status)) { החזר 0; }
    אם ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? acceptListCount != 0 : acceptListCount < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ثابت حرف **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + acceptListCount, converter);
    החזר acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    אם (U_FAILURE(*status)) { החזר 0; }
    אם ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        החזר 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    החזר acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
