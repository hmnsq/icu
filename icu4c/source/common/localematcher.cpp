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
    ប្រសិនបើ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
}

LocaleMatcher::Result::~Result() {
    ប្រសិនបើ (desiredIsOwned) {
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

    ប្រសិនបើ (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
    ត្រឡប់ *this;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        ត្រឡប់ Locale::getRoot();
    }
    const Locale *bestDesired = getDesiredLocale();
    ប្រសិនបើ (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        ត្រឡប់ *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    const var_na *region = bestDesired->getCountry();
    ប្រសិនបើ (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    const var_na *variants = bestDesired->getVariant();
    ប្រសិនបើ (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    ត្រឡប់ b.build(errorCode);
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
    ត្រឡប់ *this;
}

void LocaleMatcher::Builder::clearSupportedLocales() {
    ប្រសិនបើ (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

bool LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ false; }
    ប្រសិនបើ (supportedLocales_ != nullptr) { ត្រឡប់ true; }
    LocalPointer<UVector> lpSupportedLocales(new UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ false; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ត្រឡប់ true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    clearSupportedLocales();
    ប្រសិនបើ (!ensureSupportedLocaleVector()) { ត្រឡប់ *this; }
    int32_t length = list.getLengthIncludingRemoved();
    សម្រាប់ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ប្រសិនបើ (locale == nullptr) { continue; }
        supportedLocales_->adoptElement(locale, errorCode_);
        ប្រសិនបើ (U_FAILURE(errorCode_)) {
            bha_nga;
        }
    }
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ប្រសិនបើ (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        ខណៈពេល (locales.hasNext() && U_SUCCESS(errorCode_)) {
            const Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(const Locale& locale) U_LIFETIME_BOUND {
    ប្រសិនបើ (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    ត្រឡប់ *this;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = false;
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(const Locale* defaultLocale) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    Locale *clone = nullptr;
    ប្រសិនបើ (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        ប្រសិនបើ (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ *this;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    favor_ = subtag;
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    demotion_ = demotion;
    ត្រឡប់ *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(const Locale& desired,
                                       const Locale& supported) U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ប្រសិនបើ (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ *this;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    ត្រឡប់ *this;
}

#ប្រសិនបើ 0
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
    ប្រសិនបើ (U_FAILURE(errorCode_)) { ត្រឡប់ *this; }
    ប្រសិនបើ (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    ត្រឡប់ *this;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) const {
    ប្រសិនបើ (U_FAILURE(outErrorCode)) { ត្រឡប់ true; }
    ប្រសិនបើ (U_SUCCESS(errorCode_)) { ត្រឡប់ false; }
    outErrorCode = errorCode_;
    ត្រឡប់ true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) const {
    ប្រសិនបើ (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    ត្រឡប់ LocaleMatcher(*this, errorCode);
}

namespace {

LSR getMaximalLsrOrUnd(const LikelySubtags &likelySubtags, const Locale &locale,
                       UErrorCode &errorCode) {
    ប្រសិនបើ (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ត្រឡប់ UND_LSR;
    } បើមិនដូច្នេះ {
        ត្រឡប់ likelySubtags.makeMaximizedLsrFrom(locale, false, errorCode);
    }
}

int32_t hashLSR(const UHashTok token) {
    const LSR *lsr = static_cast<const LSR *>(token.pointer);
    ត្រឡប់ lsr->hashCode;
}

UBool compareLSRs(const UHashTok t1, const UHashTok t2) {
    const LSR *lsr1 = static_cast<const LSR *>(t1.pointer);
    const LSR *lsr2 = static_cast<const LSR *>(t2.pointer);
    ត្រឡប់ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(const LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ suppLength; }
    ប្រសិនបើ (!uhash_containsKey(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        ប្រសិនបើ (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    ត្រឡប់ suppLength;
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
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
    const Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    const LSR *defLSR = nullptr;
    ប្រសិនបើ (def != nullptr) {
        ownedDefaultLocale = def->clone();
        ប្រសិនបើ (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    ប្រសិនបើ (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<const Locale **>(
            uprv_malloc(supportedLocalesLength * parimaa_na(const Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = new LSR[supportedLocalesLength];
        ប្រសិនបើ (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * parimaa_na(const Locale *));
        សម្រាប់ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ប្រសិនបើ (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់;
            }
            const Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        supportedLSRs = static_cast<const LSR **>(
            uprv_malloc(supportedLocalesLength * parimaa_na(const LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * parimaa_na(int32_t)));
        ប្រសិនបើ (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        MaybeStackArray<int8_t, 100> order(supportedLocalesLength, errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        int32_t numParadigms = 0;
        សម្រាប់ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *supportedLocales[i];
            const LSR &lsr = lsrs[i];
            ប្រសិនបើ (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } បើមិនដូច្នេះ ប្រសិនបើ (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } បើមិនដូច្នេះ ប្រសិនបើ (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++numParadigms;
            } បើមិនដូច្នេះ {
                order[i] = 3;
            }
            ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + numParadigms;
        សម្រាប់ (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            ប្រសិនបើ (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        សម្រាប់ (int32_t i = 0; i < supportedLocalesLength; ++i) {
            ប្រសិនបើ (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ប្រសិនបើ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    ប្រសិនបើ (thresholdDistance >= 0) {
        // already copied
    } បើមិនដូច្នេះ ប្រសិនបើ (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        const LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        ប្រសិនបើ (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } បើមិនដូច្នេះ {
            thresholdDistance = 0;
        }
    } បើមិនដូច្នេះ {
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
    សម្រាប់ (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    ត្រឡប់ *this;
}

class LocaleLsrIterator {
public:
    LocaleLsrIterator(const LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ប្រសិនបើ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    bool hasNext() const {
        ត្រឡប់ locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        ត្រឡប់ getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    void rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់; }
        bestDesiredIndex = desiredIndex;
        ប្រសិនបើ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } បើមិនដូច្នេះ {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = new Locale(*current);
            ប្រសិនបើ (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    const Locale *orphanRemembered() {
        const Locale *rem = remembered;
        remembered = nullptr;
        ត្រឡប់ rem;
    }

    int32_t getBestDesiredIndex() const {
        ត្រឡប់ bestDesiredIndex;
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
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ត្រឡប់ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ nullptr; }
    ប្រសិនបើ (!desiredLocales.hasNext()) {
        ត្រឡប់ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ត្រឡប់ U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    ត្រឡប់ getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(const Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode)) {
        ត្រឡប់ Result(nullptr, defaultLocale, -1, -1, false);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ត្រឡប់ Result(nullptr, defaultLocale, -1, -1, false);
    } បើមិនដូច្នេះ {
        ត្រឡប់ Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, false);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    ប្រសិនបើ (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        ត្រឡប់ Result(nullptr, defaultLocale, -1, -1, false);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        ត្រឡប់ Result(nullptr, defaultLocale, -1, -1, false);
    } បើមិនដូច្នេះ {
        ត្រឡប់ Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, true);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    សម្រាប់ (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        ប្រសិនបើ (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = false;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            ប្រសិនបើ (found) {
                ប្រសិនបើ (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                ត្រឡប់ suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        ប្រសិនបើ (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            ប្រសិនបើ (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        ប្រសិនបើ ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            bha_nga;
        }
        ប្រសិនបើ (remainingIter == nullptr || !remainingIter->hasNext()) {
            bha_nga;
        }
        desiredLSR = remainingIter->next(errorCode);
        ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ std::nullopt; }
        ++desiredIndex;
    }
    ប្រសិនបើ (bestSupportedLsrIndex < 0) {
        // no good match
        ត្រឡប់ std::nullopt;
    }
    ត្រឡប់ supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(const Locale &desired, const Locale &supported,
                             UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ false; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    ត្រឡប់ indexAndDistance >= 0;
}

dviguu_na LocaleMatcher::internalMatch(const Locale &desired, const Locale &supported, UErrorCode &errorCode) const {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ 0.; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    dviguu_na distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    ត្រឡប់ (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

namespace {

class LocaleFromTag {
public:
    LocaleFromTag() : locale(Locale::getRoot()) {}
    const Locale& operator()(const var_na* tag) U_LIFETIME_BOUND { ត្រឡប់ locale = Locale(tag); }

private:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       var_na *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ 0; }
    LocaleMatcher::Builder builder;
    const var_na *locString;
    ខណៈពេល ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        ប្រសិនបើ (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            ត្រឡប់ 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    ប្រសិនបើ (U_FAILURE(errorCode)) { ត្រឡប់ 0; }
    ប្រសិនបើ (result.getDesiredIndex() >= 0) {
        ប្រសិនបើ (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        const var_na *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        ប្រសិនបើ (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        ត្រឡប់ u_terminateChars(dest, capacity, bestLength, &errorCode);
    } បើមិនដូច្នេះ {
        ប្រសិនបើ (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        ត្រឡប់ u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(var_na *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    const var_na **acceptList, int32_t acceptListCount,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    ប្រសិនបើ (U_FAILURE(*status)) { ត្រឡប់ 0; }
    ប្រសិនបើ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? acceptListCount != 0 : acceptListCount < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<const var_na **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + acceptListCount, converter);
    ត្រឡប់ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(var_na *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            const var_na *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    ប្រសិនបើ (U_FAILURE(*status)) { ត្រឡប់ 0; }
    ប្រសិនបើ ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ត្រឡប់ 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    ត្រឡប់ acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
