// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#شامل <optional>

#شامل "unicode/utypes.h"
#شامل "unicode/localebuilder.h"
#شامل "unicode/localematcher.h"
#شامل "unicode/locid.h"
#شامل "unicode/stringpiece.h"
#شامل "unicode/uloc.h"
#شامل "unicode/uobject.h"
#شامل "cstring.h"
#شامل "localeprioritylist.h"
#شامل "loclikelysubtags.h"
#شامل "locdistance.h"
#شامل "lsr.h"
#شامل "uassert.h"
#شامل "uhash.h"
#شامل "ustr_imp.h"
#شامل "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
شمارش ULocMatchLifetime {
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
تعریفنوع شمارش ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    اگر (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = نادرست;
    }
}

LocaleMatcher::Result::~Result() {
    اگر (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    این->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    اگر (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = نادرست;
    }
    بازگشت *این;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        بازگشت Locale::get_ریشه();
    }
    ثابت Locale *bestDesired = getDesiredLocale();
    اگر (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        بازگشت *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت نویسه *region = bestDesired->getCountry();
    اگر (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت نویسه *variants = bestDesired->getVariant();
    اگر (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    بازگشت b.build(errorCode);
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
    این->~Builder();

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
    بازگشت *این;
}

تهی LocaleMatcher::Builder::clearSupportedLocales() {
    اگر (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

منطقی LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    اگر (U_FAILURE(errorCode_)) { بازگشت نادرست; }
    اگر (supportedLocales_ != nullptr) { بازگشت درست; }
    LocalPointer<UVector> lpSupportedLocales(جدید UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    اگر (U_FAILURE(errorCode_)) { بازگشت نادرست; }
    supportedLocales_ = lpSupportedLocales.orphan();
    بازگشت درست;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    clearSupportedLocales();
    اگر (!ensureSupportedLocaleVector()) { بازگشت *این; }
    int32_t length = list.getLengthIncludingRemoved();
    برای (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        اگر (locale == nullptr) { ادامه; }
        supportedLocales_->adoptElement(locale, errorCode_);
        اگر (U_FAILURE(errorCode_)) {
            بشکن;
        }
    }
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    اگر (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        وقتی (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ثابت Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ثابت Locale& locale) U_LIFETIME_BOUND {
    اگر (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    بازگشت *این;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = نادرست;
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    Locale *clone = nullptr;
    اگر (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        اگر (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            بازگشت *این;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = درست;
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    favor_ = subtag;
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    demotion_ = demotion;
    بازگشت *این;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    اگر (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        بازگشت *این;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    بازگشت *این;
}

#اگر 0
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
    اگر (U_FAILURE(errorCode_)) { بازگشت *این; }
    اگر (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    بازگشت *این;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ثابت {
    اگر (U_FAILURE(outErrorCode)) { بازگشت درست; }
    اگر (U_SUCCESS(errorCode_)) { بازگشت نادرست; }
    outErrorCode = errorCode_;
    بازگشت درست;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ثابت {
    اگر (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    بازگشت LocaleMatcher(*این, errorCode);
}

فضاینام {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        بازگشت UND_LSR;
    } وگرنه {
        بازگشت likelySubtags.makeMaximizedLsrFrom(locale, نادرست, errorCode);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.pointer);
    بازگشت lsr->hashCode;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.pointer);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.pointer);
    بازگشت *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت suppLength; }
    اگر (!uhash_contains_کلید(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        اگر (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    بازگشت suppLength;
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
    اگر (U_FAILURE(errorCode)) { بازگشت; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = nullptr;
    اگر (def != nullptr) {
        ownedDefaultLocale = def->clone();
        اگر (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    اگر (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supportedLocalesLength * اندازه(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جدید LSR[supportedLocalesLength];
        اگر (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * اندازه(ثابت Locale *));
        برای (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            اگر (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                بازگشت;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            اگر (U_FAILURE(errorCode)) { بازگشت; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supportedLocalesLength * اندازه(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * اندازه(int32_t)));
        اگر (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_پشته_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        int32_t رقم_Paradigms = 0;
        برای (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            اگر (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } وگرنه اگر (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } وگرنه اگر (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++رقم_Paradigms;
            } وگرنه {
                order[i] = 3;
            }
            اگر (U_FAILURE(errorCode)) { بازگشت; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + رقم_Paradigms;
        برای (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            اگر (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        برای (int32_t i = 0; i < supportedLocalesLength; ++i) {
            اگر (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    اگر (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    اگر (thresholdDistance >= 0) {
        // already copied
    } وگرنه اگر (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        اگر (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } وگرنه {
            thresholdDistance = 0;
        }
    } وگرنه {
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
    برای (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    این->~LocaleMatcher();

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
    بازگشت *این;
}

رده LocaleLsrIterator {
عمومی:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        اگر (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    منطقی hasNext() ثابت {
        بازگشت locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        بازگشت getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    تهی rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        اگر (U_FAILURE(errorCode)) { بازگشت; }
        bestDesiredIndex = desiredIndex;
        اگر (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وگرنه {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = جدید Locale(*current);
            اگر (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = nullptr;
        بازگشت rem;
    }

    int32_t getBestDesiredIndex() ثابت {
        بازگشت bestDesiredIndex;
    }

خصوصی:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ثابت Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ثابت Locale* LocaleMatcher::getBestMatch(ثابت Locale& desiredLocale,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    بازگشت U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    اگر (!desiredLocales.hasNext()) {
        بازگشت defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    بازگشت U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode)) { بازگشت nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    بازگشت getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode)) {
        بازگشت Result(nullptr, defaultLocale, -1, -1, نادرست);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    اگر (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        بازگشت Result(nullptr, defaultLocale, -1, -1, نادرست);
    } وگرنه {
        بازگشت Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, نادرست);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        بازگشت Result(nullptr, defaultLocale, -1, -1, نادرست);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    اگر (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        بازگشت Result(nullptr, defaultLocale, -1, -1, نادرست);
    } وگرنه {
        بازگشت Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, درست);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    برای (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        اگر (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = نادرست;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            اگر (found) {
                اگر (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                بازگشت suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        اگر (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            اگر (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                اگر (U_FAILURE(errorCode)) { بازگشت std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        اگر ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            بشکن;
        }
        اگر (remainingIter == nullptr || !remainingIter->hasNext()) {
            بشکن;
        }
        desiredLSR = remainingIter->next(errorCode);
        اگر (U_FAILURE(errorCode)) { بازگشت std::nullopt; }
        ++desiredIndex;
    }
    اگر (bestSupportedLsrIndex < 0) {
        // no good match
        بازگشت std::nullopt;
    }
    بازگشت supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ثابت Locale &desired, ثابت Locale &supported,
                             UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت نادرست; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    بازگشت indexAndDistance >= 0;
}

دوگانه LocaleMatcher::internalMatch(ثابت Locale &desired, ثابت Locale &supported, UErrorCode &errorCode) ثابت {
    اگر (U_FAILURE(errorCode)) { بازگشت 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    دوگانه distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    بازگشت (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

فضاینام {

رده LocaleFromTag {
عمومی:
    LocaleFromTag() : locale(Locale::get_ریشه()) {}
    ثابت Locale& operator()(ثابت نویسه* tag) U_LIFETIME_BOUND { بازگشت locale = Locale(tag); }

خصوصی:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       نویسه *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    اگر (U_FAILURE(errorCode)) { بازگشت 0; }
    LocaleMatcher::Builder builder;
    ثابت نویسه *locString;
    وقتی ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        اگر (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            بازگشت 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    اگر (U_FAILURE(errorCode)) { بازگشت 0; }
    اگر (result.getDesiredIndex() >= 0) {
        اگر (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت نویسه *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        اگر (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        بازگشت u_terminateChars(dest, capacity, bestLength, &errorCode);
    } وگرنه {
        اگر (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        بازگشت u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(نویسه *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ثابت نویسه **acceptList, int32_t accept_List_شمار,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    اگر (U_FAILURE(*status)) { بازگشت 0; }
    اگر ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_شمار != 0 : accept_List_شمار < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ثابت نویسه **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_شمار, converter);
    بازگشت acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(نویسه *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ثابت نویسه *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    اگر (U_FAILURE(*status)) { بازگشت 0; }
    اگر ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    بازگشت acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
