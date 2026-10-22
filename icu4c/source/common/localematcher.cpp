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
    𐤀𐤌 (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::Result::~Result() {
    𐤀𐤌 (desiredIsOwned) {
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

    𐤀𐤌 (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = خطأ;
    }
    أرجع *هذا;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        أرجع Locale::getRoot();
    }
    ثابت Locale *bestDesired = getDesiredLocale();
    𐤀𐤌 (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        أرجع *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = bestDesired->getCountry();
    𐤀𐤌 (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = bestDesired->getVariant();
    𐤀𐤌 (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    أرجع b.build(errorCode);
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
    أرجع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    𐤀𐤌 (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع خطأ; }
    𐤀𐤌 (supportedLocales_ != nullptr) { أرجع true; }
    LocalPointer<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    أرجع true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    clearSupportedLocales();
    𐤀𐤌 (!ensureSupportedLocaleVector()) { أرجع *هذا; }
    int32_t length = list.getLengthIncludingRemoved();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        𐤀𐤌 (locale == nullptr) { استمر; }
        supportedLocales_->adoptElement(locale, errorCode_);
        𐤀𐤌 (U_FAILURE(errorCode_)) {
            اكسر;
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    𐤀𐤌 (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        طالما (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ثابت Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ثابت Locale& locale) U_LIFETIME_BOUND {
    𐤀𐤌 (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    أرجع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = خطأ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    Locale *clone = nullptr;
    𐤀𐤌 (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        𐤀𐤌 (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            أرجع *هذا;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    favor_ = subtag;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    demotion_ = demotion;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    𐤀𐤌 (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        أرجع *هذا;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    أرجع *هذا;
}

#𐤀𐤌 0
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
    𐤀𐤌 (U_FAILURE(errorCode_)) { أرجع *هذا; }
    𐤀𐤌 (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    أرجع *هذا;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    𐤀𐤌 (U_FAILURE(outErrorCode)) { أرجع true; }
    𐤀𐤌 (U_SUCCESS(errorCode_)) { أرجع خطأ; }
    outErrorCode = errorCode_;
    أرجع true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    أرجع LocaleMatcher(*هذا, errorCode);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        أرجع UND_LSR;
    } وإلا {
        أرجع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, errorCode);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.pointer);
    أرجع lsr->hashCode;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.pointer);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.pointer);
    أرجع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع suppLength; }
    𐤀𐤌 (!uhash_containsKey(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        𐤀𐤌 (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    أرجع suppLength;
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
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = nullptr;
    𐤀𐤌 (def != nullptr) {
        ownedDefaultLocale = def->clone();
        𐤀𐤌 (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    𐤀𐤌 (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supportedLocalesLength];
        𐤀𐤌 (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            𐤀𐤌 (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                أرجع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supportedLocalesLength * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * حجم(int32_t)));
        𐤀𐤌 (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        MaybeStackArray<int8_t, 100> order(supportedLocalesLength, errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        int32_t numParadigms = 0;
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            𐤀𐤌 (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } وإلا 𐤀𐤌 (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } وإلا 𐤀𐤌 (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++numParadigms;
            } وإلا {
                order[i] = 3;
            }
            𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + numParadigms;
        لكل (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            𐤀𐤌 (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supportedLocalesLength; ++i) {
            𐤀𐤌 (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    𐤀𐤌 (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    𐤀𐤌 (thresholdDistance >= 0) {
        // already copied
    } وإلا 𐤀𐤌 (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        𐤀𐤌 (U_SUCCESS(errorCode)) {
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
    أرجع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        𐤀𐤌 (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    منطقي hasNext() ثابت {
        أرجع locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        أرجع getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    فراغ rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع; }
        bestDesiredIndex = desiredIndex;
        𐤀𐤌 (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = جديد Locale(*current);
            𐤀𐤌 (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = nullptr;
        أرجع rem;
    }

    int32_t getBestDesiredIndex() ثابت {
        أرجع bestDesiredIndex;
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
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    أرجع U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع nullptr; }
    𐤀𐤌 (!desiredLocales.hasNext()) {
        أرجع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    أرجع U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    أرجع getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode)) {
        أرجع Result(nullptr, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        أرجع Result(nullptr, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, خطأ);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    𐤀𐤌 (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        أرجع Result(nullptr, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        أرجع Result(nullptr, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, true);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    لكل (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        𐤀𐤌 (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = خطأ;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            𐤀𐤌 (found) {
                𐤀𐤌 (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                أرجع suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        𐤀𐤌 (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            𐤀𐤌 (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                𐤀𐤌 (U_FAILURE(errorCode)) { أرجع std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        𐤀𐤌 ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            اكسر;
        }
        𐤀𐤌 (remainingIter == nullptr || !remainingIter->hasNext()) {
            اكسر;
        }
        desiredLSR = remainingIter->next(errorCode);
        𐤀𐤌 (U_FAILURE(errorCode)) { أرجع std::nullopt; }
        ++desiredIndex;
    }
    𐤀𐤌 (bestSupportedLsrIndex < 0) {
        // no good match
        أرجع std::nullopt;
    }
    أرجع supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ثابت Locale &desired, ثابت Locale &supported,
                             UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    أرجع indexAndDistance >= 0;
}

مزدوج LocaleMatcher::internalMatch(ثابت Locale &desired, ثابت Locale &supported, UErrorCode &errorCode) ثابت {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    مزدوج distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    أرجع (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف LocaleFromTag {
عام:
    LocaleFromTag() : locale(Locale::getRoot()) {}
    ثابت Locale& operator()(ثابت حرف* tag) U_LIFETIME_BOUND { أرجع locale = Locale(tag); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    طالما ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        𐤀𐤌 (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            أرجع 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    𐤀𐤌 (U_FAILURE(errorCode)) { أرجع 0; }
    𐤀𐤌 (result.getDesiredIndex() >= 0) {
        𐤀𐤌 (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        𐤀𐤌 (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        أرجع u_terminateChars(dest, capacity, bestLength, &errorCode);
    } وإلا {
        𐤀𐤌 (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        أرجع u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ثابت حرف **acceptList, int32_t acceptListCount,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    𐤀𐤌 (U_FAILURE(*status)) { أرجع 0; }
    𐤀𐤌 ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? acceptListCount != 0 : acceptListCount < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ثابت حرف **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + acceptListCount, converter);
    أرجع acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    𐤀𐤌 (U_FAILURE(*status)) { أرجع 0; }
    𐤀𐤌 ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    أرجع acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
