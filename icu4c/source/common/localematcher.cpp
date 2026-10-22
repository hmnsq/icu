// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#include <optional>

#include "unicode/utypes.h"
#include "unicode/localebuilder.h"
#include "unicode/localematcher.h"
#include "unicode/locid.h"
#include "unicode/stringpiece.h"
#include "unicode/uloc.h"
#include "unicode/uobject.h"
#include "cstring.h"
#include "localeprioritylist.h"
#include "loclikelysubtags.h"
#include "locdistance.h"
#include "lsr.h"
#include "uassert.h"
#include "uhash.h"
#include "ustr_imp.h"
#include "uvector.h"

#define UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ga_nanaa ULocMatchLifetime {
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
typedef ga_nanaa ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    ຖ້າ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
}

LocaleMatcher::Result::~Result() {
    ຖ້າ (desiredIsOwned) {
        delete desiredLocale;
    }
}

LocaleMatcher::Result&
LocaleMatcher::Result::operator=(LocaleMatcher::Result&& src) noexcept U_LIFETIME_BOUND {
    this->~Result();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desiredIndex = src.desiredIndex;
    supportedIndex = src.supportedIndex;
    desiredIsOwned = src.desiredIsOwned;

    ຖ້າ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
    ສົ່ງຄືນ *this;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ສົ່ງຄືນ Locale::getRoot();
    }
    const Locale *bestDesired = getDesiredLocale();
    ຖ້າ (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ສົ່ງຄືນ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    const var_na *region = bestDesired->getCountry();
    ຖ້າ (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    const var_na *variants = bestDesired->getVariant();
    ຖ້າ (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    ສົ່ງຄືນ b.build(errorCode);
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
    this->~Builder();

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
    ສົ່ງຄືນ *this;
}

void LocaleMatcher::Builder::clearSupportedLocales() {
    ຖ້າ (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

bool LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ false; }
    ຖ້າ (supportedLocales_ != nullptr) { ສົ່ງຄືນ true; }
    LocalPointer<UVector> lpSupportedLocales(new UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ false; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ສົ່ງຄືນ true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    clearSupportedLocales();
    ຖ້າ (!ensureSupportedLocaleVector()) { ສົ່ງຄືນ *this; }
    int32_t length = list.getLengthIncludingRemoved();
    ສຳລັບ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ຖ້າ (locale == nullptr) { continue; }
        supportedLocales_->adoptElement(locale, errorCode_);
        ຖ້າ (U_FAILURE(errorCode_)) {
            bha_nga;
        }
    }
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ຖ້າ (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        ໃນຂະນະທີ່ (locales.hasNext() && U_SUCCESS(errorCode_)) {
            const Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(const Locale& locale) U_LIFETIME_BOUND {
    ຖ້າ (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = false;
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(const Locale* defaultLocale) U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    Locale *clone = nullptr;
    ຖ້າ (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        ຖ້າ (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ *this;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    favor_ = subtag;
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    demotion_ = demotion;
    ສົ່ງຄືນ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(const Locale& desired,
                                       const Locale& supported) U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ຖ້າ (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ *this;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ສົ່ງຄືນ *this;
}

#ຖ້າ 0
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
    ຖ້າ (U_FAILURE(errorCode_)) { ສົ່ງຄືນ *this; }
    ຖ້າ (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ສົ່ງຄືນ *this;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) const {
    ຖ້າ (U_FAILURE(outErrorCode)) { ສົ່ງຄືນ true; }
    ຖ້າ (U_SUCCESS(errorCode_)) { ສົ່ງຄືນ false; }
    outErrorCode = errorCode_;
    ສົ່ງຄືນ true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) const {
    ຖ້າ (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ສົ່ງຄືນ LocaleMatcher(*this, errorCode);
}

namespace {

LSR getMaximalLsrOrUnd(const LikelySubtags &likelySubtags, const Locale &locale,
                       UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ສົ່ງຄືນ UND_LSR;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ likelySubtags.makeMaximizedLsrFrom(locale, false, errorCode);
    }
}

int32_t hashLSR(const UHashTok token) {
    const LSR *lsr = static_cast<const LSR *>(token.pointer);
    ສົ່ງຄືນ lsr->hashCode;
}

UBool compareLSRs(const UHashTok t1, const UHashTok t2) {
    const LSR *lsr1 = static_cast<const LSR *>(t1.pointer);
    const LSR *lsr2 = static_cast<const LSR *>(t2.pointer);
    ສົ່ງຄືນ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(const LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ suppLength; }
    ຖ້າ (!uhash_containsKey(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        ຖ້າ (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    ສົ່ງຄືນ suppLength;
}

LocaleMatcher::LocaleMatcher(const Builder &builder, UErrorCode &errorCode) :
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
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
    const Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    const LSR *defLSR = nullptr;
    ຖ້າ (def != nullptr) {
        ownedDefaultLocale = def->clone();
        ຖ້າ (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    ຖ້າ (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<const Locale **>(
            uprv_malloc(supportedLocalesLength * parimaa_na(const Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = new LSR[supportedLocalesLength];
        ຖ້າ (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * parimaa_na(const Locale *));
        ສຳລັບ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ຖ້າ (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ສົ່ງຄືນ;
            }
            const Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        supportedLSRs = static_cast<const LSR **>(
            uprv_malloc(supportedLocalesLength * parimaa_na(const LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * parimaa_na(int32_t)));
        ຖ້າ (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ສົ່ງຄືນ;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        MaybeStackArray<int8_t, 100> order(supportedLocalesLength, errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        int32_t numParadigms = 0;
        ສຳລັບ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *supportedLocales[i];
            const LSR &lsr = lsrs[i];
            ຖ້າ (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } ບໍ່ດັ່ງນັ້ນ ຖ້າ (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++numParadigms;
            } ບໍ່ດັ່ງນັ້ນ {
                order[i] = 3;
            }
            ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + numParadigms;
        ສຳລັບ (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            ຖ້າ (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        ສຳລັບ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ຖ້າ (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ຖ້າ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    ຖ້າ (thresholdDistance >= 0) {
        // already copied
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        const LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        ຖ້າ (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } ບໍ່ດັ່ງນັ້ນ {
            thresholdDistance = 0;
        }
    } ບໍ່ດັ່ງນັ້ນ {
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
    ສຳລັບ (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    this->~LocaleMatcher();

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
    ສົ່ງຄືນ *this;
}

class LocaleLsrIterator {
public:
    LocaleLsrIterator(const LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ຖ້າ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    bool hasNext() const {
        ສົ່ງຄືນ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ສົ່ງຄືນ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    void rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ; }
        bestDesiredIndex = desiredIndex;
        ຖ້າ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ບໍ່ດັ່ງນັ້ນ {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = new Locale(*current);
            ຖ້າ (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    const Locale *orphanRemembered() {
        const Locale *rem = remembered;
        remembered = nullptr;
        ສົ່ງຄືນ rem;
    }

    int32_t getBestDesiredIndex() const {
        ສົ່ງຄືນ bestDesiredIndex;
    }

private:
    const LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULocMatchLifetime lifetime;
    const Locale *current = nullptr, *remembered = nullptr;
    int32_t bestDesiredIndex = -1;
};

const Locale* LocaleMatcher::getBestMatch(const Locale& desiredLocale,
                                          UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ສົ່ງຄືນ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    ຖ້າ (!desiredLocales.hasNext()) {
        ສົ່ງຄືນ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ສົ່ງຄືນ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ສົ່ງຄືນ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(const Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode)) {
        ສົ່ງຄືນ Result(nullptr, defaultLocale, -1, -1, false);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ຖ້າ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ສົ່ງຄືນ Result(nullptr, defaultLocale, -1, -1, false);
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, false);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ຖ້າ (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        ສົ່ງຄືນ Result(nullptr, defaultLocale, -1, -1, false);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ຖ້າ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ສົ່ງຄືນ Result(nullptr, defaultLocale, -1, -1, false);
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, true);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    ສຳລັບ (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        ຖ້າ (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = false;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            ຖ້າ (found) {
                ຖ້າ (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                ສົ່ງຄືນ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        ຖ້າ (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            ຖ້າ (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        ຖ້າ ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            bha_nga;
        }
        ຖ້າ (remainingIter == nullptr || !remainingIter->hasNext()) {
            bha_nga;
        }
        desiredLSR = remainingIter->next(errorCode);
        ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ std::nullopt; }
        ++desiredIndex;
    }
    ຖ້າ (bestSupportedLsrIndex < 0) {
        // no good match
        ສົ່ງຄືນ std::nullopt;
    }
    ສົ່ງຄືນ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(const Locale &desired, const Locale &supported,
                             UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ false; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ false; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ສົ່ງຄືນ indexAndDistance >= 0;
}

dviguu_na LocaleMatcher::internalMatch(const Locale &desired, const Locale &supported, UErrorCode &errorCode) const {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ 0.; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    dviguu_na distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    ສົ່ງຄືນ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

namespace {

class LocaleFromTag {
public:
    LocaleFromTag() : locale(Locale::getRoot()) {}
    const Locale& operator()(const var_na* tag) U_LIFETIME_BOUND { ສົ່ງຄືນ locale = Locale(tag); }

private:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       var_na *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ 0; }
    LocaleMatcher::Builder builder;
    const var_na *locString;
    ໃນຂະນະທີ່ ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        ຖ້າ (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ສົ່ງຄືນ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    ຖ້າ (U_FAILURE(errorCode)) { ສົ່ງຄືນ 0; }
    ຖ້າ (result.getDesiredIndex() >= 0) {
        ຖ້າ (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        const var_na *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        ຖ້າ (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        ສົ່ງຄືນ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } ບໍ່ດັ່ງນັ້ນ {
        ຖ້າ (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        ສົ່ງຄືນ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(var_na *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    const var_na **acceptList, int32_t acceptListCount,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    ຖ້າ (U_FAILURE(*status)) { ສົ່ງຄືນ 0; }
    ຖ້າ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? acceptListCount != 0 : acceptListCount < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<const var_na **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + acceptListCount, converter);
    ສົ່ງຄືນ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(var_na *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            const var_na *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    ຖ້າ (U_FAILURE(*status)) { ສົ່ງຄືນ 0; }
    ຖ້າ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ສົ່ງຄືນ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
