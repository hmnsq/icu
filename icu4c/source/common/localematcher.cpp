// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#অন্তর্ভাব <optional>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/localebuilder.h"
#অন্তর্ভাব "unicode/localematcher.h"
#অন্তর্ভাব "unicode/locid.h"
#অন্তর্ভাব "unicode/stringpiece.h"
#অন্তর্ভাব "unicode/uloc.h"
#অন্তর্ভাব "unicode/uobject.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "localeprioritylist.h"
#অন্তর্ভাব "loclikelysubtags.h"
#অন্তর্ভাব "locdistance.h"
#অন্তর্ভাব "lsr.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
গণনা ULocMatchLifetime {
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
প্রকারনির্দেশ গণনা ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = অসত্য;
    }
}

LocaleMatcher::Result::~Result() {
    যদি (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    এতত্->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    যদি (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = অসত্য;
    }
    প্রত্যাগম *এতত্;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        প্রত্যাগম Locale::get_মূল();
    }
    ধ্রুব Locale *bestDesired = getDesiredLocale();
    যদি (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        প্রত্যাগম *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ধ্রুব বর্ণ *region = bestDesired->getCountry();
    যদি (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ধ্রুব বর্ণ *variants = bestDesired->getVariant();
    যদি (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    প্রত্যাগম b.build(errorCode);
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
    এতত্->~Builder();

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
    প্রত্যাগম *এতত্;
}

শূন্য LocaleMatcher::Builder::clearSupportedLocales() {
    যদি (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

তার্কিক LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম অসত্য; }
    যদি (supportedLocales_ != nullptr) { প্রত্যাগম সত্য; }
    LocalPointer<UVector> lpSupportedLocales(নূতন UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম অসত্য; }
    supportedLocales_ = lpSupportedLocales.orphan();
    প্রত্যাগম সত্য;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    clearSupportedLocales();
    যদি (!ensureSupportedLocaleVector()) { প্রত্যাগম *এতত্; }
    int32_t length = list.getLengthIncludingRemoved();
    প্রতি (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        যদি (locale == nullptr) { অনুবর্ত; }
        supportedLocales_->adoptElement(locale, errorCode_);
        যদি (U_FAILURE(errorCode_)) {
            ভঙ্গ;
        }
    }
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        যাবত্ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            ধ্রুব Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(ধ্রুব Locale& locale) U_LIFETIME_BOUND {
    যদি (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = অসত্য;
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(ধ্রুব Locale* defaultLocale) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    Locale *clone = nullptr;
    যদি (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        যদি (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম *এতত্;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = সত্য;
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    favor_ = subtag;
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    demotion_ = demotion;
    প্রত্যাগম *এতত্;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(ধ্রুব Locale& desired,
                                       ধ্রুব Locale& supported) U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    যদি (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম *এতত্;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    প্রত্যাগম *এতত্;
}

#যদি 0
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
    যদি (U_FAILURE(errorCode_)) { প্রত্যাগম *এতত্; }
    যদি (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    প্রত্যাগম *এতত্;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) ধ্রুব {
    যদি (U_FAILURE(outErrorCode)) { প্রত্যাগম সত্য; }
    যদি (U_SUCCESS(errorCode_)) { প্রত্যাগম অসত্য; }
    outErrorCode = errorCode_;
    প্রত্যাগম সত্য;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) ধ্রুব {
    যদি (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    প্রত্যাগম LocaleMatcher(*এতত্, errorCode);
}

নামাকাশ {

LSR getMaximalLsrOrUnd(ধ্রুব LikelySubtags &likelySubtags, ধ্রুব Locale &locale,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        প্রত্যাগম UND_LSR;
    } অন্যথা {
        প্রত্যাগম likelySubtags.makeMaximizedLsrFrom(locale, অসত্য, errorCode);
    }
}

int32_t hashLSR(ধ্রুব UHashTok token) {
    ধ্রুব LSR *lsr = static_cast<ধ্রুব LSR *>(token.pointer);
    প্রত্যাগম lsr->hashCode;
}

UBool compareLSRs(ধ্রুব UHashTok t1, ধ্রুব UHashTok t2) {
    ধ্রুব LSR *lsr1 = static_cast<ধ্রুব LSR *>(t1.pointer);
    ধ্রুব LSR *lsr2 = static_cast<ধ্রুব LSR *>(t2.pointer);
    প্রত্যাগম *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ধ্রুব LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম suppLength; }
    যদি (!uhash_contains_চাবি(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        যদি (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    প্রত্যাগম suppLength;
}

LocaleMatcher::LocaleMatcher(ধ্রুব Builder &builder, UErrorCode &errorCode) :
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
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
    ধ্রুব Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ধ্রুব LSR *defLSR = nullptr;
    যদি (def != nullptr) {
        ownedDefaultLocale = def->clone();
        যদি (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    যদি (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ধ্রুব Locale **>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(ধ্রুব Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = নূতন LSR[supportedLocalesLength];
        যদি (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * পরিমাণ(ধ্রুব Locale *));
        প্রতি (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ধ্রুব Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            যদি (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            ধ্রুব Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        supportedLSRs = static_cast<ধ্রুব LSR **>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(ধ্রুব LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * পরিমাণ(int32_t)));
        যদি (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_স্তূপ_Array<int8_t, 100> order(supportedLocalesLength, errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        int32_t অঙ্ক_Paradigms = 0;
        প্রতি (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ধ্রুব Locale &locale = *supportedLocales[i];
            ধ্রুব LSR &lsr = lsrs[i];
            যদি (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } অন্যথা যদি (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } অন্যথা যদি (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++অঙ্ক_Paradigms;
            } অন্যথা {
                order[i] = 3;
            }
            যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + অঙ্ক_Paradigms;
        প্রতি (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            যদি (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        প্রতি (int32_t i = 0; i < supportedLocalesLength; ++i) {
            যদি (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    যদি (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    যদি (thresholdDistance >= 0) {
        // already copied
    } অন্যথা যদি (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        ধ্রুব LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        যদি (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } অন্যথা {
            thresholdDistance = 0;
        }
    } অন্যথা {
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
    প্রতি (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    এতত্->~LocaleMatcher();

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
    প্রত্যাগম *এতত্;
}

বর্গ LocaleLsrIterator {
সার্বজনিক:
    LocaleLsrIterator(ধ্রুব LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        যদি (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    তার্কিক hasNext() ধ্রুব {
        প্রত্যাগম locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        প্রত্যাগম getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    শূন্য rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম; }
        bestDesiredIndex = desiredIndex;
        যদি (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } অন্যথা {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = নূতন Locale(*current);
            যদি (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    ধ্রুব Locale *orphanRemembered() {
        ধ্রুব Locale *rem = remembered;
        remembered = nullptr;
        প্রত্যাগম rem;
    }

    int32_t getBestDesiredIndex() ধ্রুব {
        প্রত্যাগম bestDesiredIndex;
    }

গুপ্ত:
    ধ্রুব LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    ধ্রুব Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

ধ্রুব Locale* LocaleMatcher::getBestMatch(ধ্রুব Locale& desiredLocale,
                                          UErrorCode& errorCode) ধ্রুব U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    প্রত্যাগম U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ধ্রুব Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) ধ্রুব U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    যদি (!desiredLocales.hasNext()) {
        প্রত্যাগম defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    প্রত্যাগম U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

ধ্রুব Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) ধ্রুব U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    প্রত্যাগম getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(ধ্রুব Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) ধ্রুব U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode)) {
        প্রত্যাগম Result(nullptr, defaultLocale, -1, -1, অসত্য);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    যদি (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        প্রত্যাগম Result(nullptr, defaultLocale, -1, -1, অসত্য);
    } অন্যথা {
        প্রত্যাগম Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, অসত্য);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) ধ্রুব U_LIFETIME_BOUND {
    যদি (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        প্রত্যাগম Result(nullptr, defaultLocale, -1, -1, অসত্য);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    যদি (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        প্রত্যাগম Result(nullptr, defaultLocale, -1, -1, অসত্য);
    } অন্যথা {
        প্রত্যাগম Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, সত্য);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    প্রতি (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        যদি (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = অসত্য;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            যদি (found) {
                যদি (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                প্রত্যাগম suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        যদি (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            যদি (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                যদি (U_FAILURE(errorCode)) { প্রত্যাগম std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        যদি ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            ভঙ্গ;
        }
        যদি (remainingIter == nullptr || !remainingIter->hasNext()) {
            ভঙ্গ;
        }
        desiredLSR = remainingIter->next(errorCode);
        যদি (U_FAILURE(errorCode)) { প্রত্যাগম std::nullopt; }
        ++desiredIndex;
    }
    যদি (bestSupportedLsrIndex < 0) {
        // no good match
        প্রত্যাগম std::nullopt;
    }
    প্রত্যাগম supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(ধ্রুব Locale &desired, ধ্রুব Locale &supported,
                             UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম অসত্য; }
    ধ্রুব LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    প্রত্যাগম indexAndDistance >= 0;
}

দ্বিগুণ LocaleMatcher::internalMatch(ধ্রুব Locale &desired, ধ্রুব Locale &supported, UErrorCode &errorCode) ধ্রুব {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0.; }
    ধ্রুব LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    দ্বিগুণ distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    প্রত্যাগম (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

নামাকাশ {

বর্গ LocaleFromTag {
সার্বজনিক:
    LocaleFromTag() : locale(Locale::get_মূল()) {}
    ধ্রুব Locale& operator()(ধ্রুব বর্ণ* tag) U_LIFETIME_BOUND { প্রত্যাগম locale = Locale(tag); }

গুপ্ত:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       বর্ণ *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0; }
    LocaleMatcher::Builder builder;
    ধ্রুব বর্ণ *locString;
    যাবত্ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        যদি (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            প্রত্যাগম 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    যদি (U_FAILURE(errorCode)) { প্রত্যাগম 0; }
    যদি (result.getDesiredIndex() >= 0) {
        যদি (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ধ্রুব বর্ণ *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        যদি (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        প্রত্যাগম u_terminateChars(dest, capacity, bestLength, &errorCode);
    } অন্যথা {
        যদি (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        প্রত্যাগম u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(বর্ণ *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    ধ্রুব বর্ণ **acceptList, int32_t accept_List_গুনতি,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    যদি (U_FAILURE(*status)) { প্রত্যাগম 0; }
    যদি ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? accept_List_গুনতি != 0 : accept_List_গুনতি < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<ধ্রুব বর্ণ **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + accept_List_গুনতি, converter);
    প্রত্যাগম acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(বর্ণ *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            ধ্রুব বর্ণ *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    যদি (U_FAILURE(*status)) { প্রত্যাগম 0; }
    যদি ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    প্রত্যাগম acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
