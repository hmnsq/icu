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
enum ULocMatchLifetime {
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
typedef enum ULocMatchLifetime ULocMatchLifetime;
#endif

U_NAMESPACE_BEGIN

LocaleMatcher::Result::Result(LocaleMatcher::Result &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desiredIndex(src.desiredIndex),
        supportedIndex(src.supportedIndex),
        desiredIsOwned(src.desiredIsOwned) {
    만약 (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
}

LocaleMatcher::Result::~Result() {
    만약 (desiredIsOwned) {
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

    만약 (desiredIsOwned) {
        src.desiredLocale = nullptr;
        src.desiredIndex = -1;
        src.desiredIsOwned = false;
    }
    반환 *this;
}

Locale LocaleMatcher::Result::makeResolvedLocale(UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode) || supportedLocale == nullptr) {
        반환 Locale::getRoot();
    }
    const Locale *bestDesired = getDesiredLocale();
    만약 (bestDesired == nullptr || *supportedLocale == *bestDesired) {
        반환 *supportedLocale;
    }
    LocaleBuilder b;
    b.setLocale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    const char *region = bestDesired->getCountry();
    만약 (*region != 0) {
        b.setRegion(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    const char *variants = bestDesired->getVariant();
    만약 (*variants != 0) {
        b.setVariant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, errorCode);
    반환 b.build(errorCode);
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
    반환 *this;
}

void LocaleMatcher::Builder::clearSupportedLocales() {
    만약 (supportedLocales_ != nullptr) {
        supportedLocales_->removeAllElements();
    }
}

bool LocaleMatcher::Builder::ensureSupportedLocaleVector() {
    만약 (U_FAILURE(errorCode_)) { 반환 false; }
    만약 (supportedLocales_ != nullptr) { 반환 true; }
    LocalPointer<UVector> lpSupportedLocales(new UVector(uprv_deleteUObject, nullptr, errorCode_), errorCode_);
    만약 (U_FAILURE(errorCode_)) { 반환 false; }
    supportedLocales_ = lpSupportedLocales.orphan();
    반환 true;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocalesFromListString(StringPiece locales) U_LIFETIME_BOUND {
    LocalePriorityList list(locales, errorCode_);
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    clearSupportedLocales();
    만약 (!ensureSupportedLocaleVector()) { 반환 *this; }
    int32_t length = list.getLengthIncludingRemoved();
    위해 (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        만약 (locale == nullptr) { continue; }
        supportedLocales_->adoptElement(locale, errorCode_);
        만약 (U_FAILURE(errorCode_)) {
            break;
        }
    }
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setSupportedLocales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    만약 (ensureSupportedLocaleVector()) {
        clearSupportedLocales();
        동안 (locales.hasNext() && U_SUCCESS(errorCode_)) {
            const Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), errorCode_);
            supportedLocales_->adoptElement(clone.orphan(), errorCode_);
        }
    }
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::addSupportedLocale(const Locale& locale) U_LIFETIME_BOUND {
    만약 (ensureSupportedLocaleVector()) {
        LocalPointer<Locale> clone(locale.clone(), errorCode_);
        supportedLocales_->adoptElement(clone.orphan(), errorCode_);
    }
    반환 *this;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::setNoDefaultLocale() U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    delete defaultLocale_;
    defaultLocale_ = nullptr;
    withDefault_ = false;
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDefaultLocale(const Locale* defaultLocale) U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    Locale *clone = nullptr;
    만약 (defaultLocale != nullptr) {
        clone = defaultLocale->clone();
        만약 (clone == nullptr) {
            errorCode_ = U_MEMORY_ALLOCATION_ERROR;
            반환 *this;
        }
    }
    delete defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = true;
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setFavorSubtag(ULocMatchFavorSubtag subtag) U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    favor_ = subtag;
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setDemotionPerDesiredLocale(ULocMatchDemotion demotion) U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    demotion_ = demotion;
    반환 *this;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::setMaxDistance(const Locale& desired,
                                       const Locale& supported) U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    만약 (desiredClone == nullptr || supportedClone == nullptr) {
        delete desiredClone;  // in case only one could not be allocated
        delete supportedClone;
        errorCode_ = U_MEMORY_ALLOCATION_ERROR;
        반환 *this;
    }
    delete maxDistanceDesired_;
    delete maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    반환 *this;
}

#만약 0
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
    만약 (U_FAILURE(errorCode_)) { 반환 *this; }
    만약 (thresholdDistance > 100) {
        thresholdDistance = 100;
    }
    thresholdDistance_ = thresholdDistance;
    반환 *this;
}
#endif

UBool LocaleMatcher::Builder::copyErrorTo(UErrorCode &outErrorCode) const {
    만약 (U_FAILURE(outErrorCode)) { 반환 true; }
    만약 (U_SUCCESS(errorCode_)) { 반환 false; }
    outErrorCode = errorCode_;
    반환 true;
}

LocaleMatcher LocaleMatcher::Builder::build(UErrorCode &errorCode) const {
    만약 (U_SUCCESS(errorCode) && U_FAILURE(errorCode_)) {
        errorCode = errorCode_;
    }
    반환 LocaleMatcher(*this, errorCode);
}

namespace {

LSR getMaximalLsrOrUnd(const LikelySubtags &likelySubtags, const Locale &locale,
                       UErrorCode &errorCode) {
    만약 (U_FAILURE(errorCode) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        반환 UND_LSR;
    } 아니면 {
        반환 likelySubtags.makeMaximizedLsrFrom(locale, false, errorCode);
    }
}

int32_t hashLSR(const UHashTok token) {
    const LSR *lsr = static_cast<const LSR *>(token.pointer);
    반환 lsr->hashCode;
}

UBool compareLSRs(const UHashTok t1, const UHashTok t2) {
    const LSR *lsr1 = static_cast<const LSR *>(t1.pointer);
    const LSR *lsr2 = static_cast<const LSR *>(t2.pointer);
    반환 *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(const LSR &lsr, int32_t i, int32_t suppLength,
                                   UErrorCode &errorCode) {
    만약 (U_FAILURE(errorCode)) { 반환 suppLength; }
    만약 (!uhash_containsKey(supportedLsrToIndex, &lsr)) {
        uhash_putiAllowZero(supportedLsrToIndex, const_cast<LSR *>(&lsr), i, &errorCode);
        만약 (U_SUCCESS(errorCode)) {
            supportedLSRs[suppLength] = &lsr;
            supportedIndexes[suppLength++] = i;
        }
    }
    반환 suppLength;
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
    만약 (U_FAILURE(errorCode)) { 반환; }
    const Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    const LSR *defLSR = nullptr;
    만약 (def != nullptr) {
        ownedDefaultLocale = def->clone();
        만약 (ownedDefaultLocale == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            반환;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
        defLSR = &builderDefaultLSR;
    }
    supportedLocalesLength = builder.supportedLocales_ != nullptr ?
        builder.supportedLocales_->size() : 0;
    만약 (supportedLocalesLength > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<const Locale **>(
            uprv_malloc(supportedLocalesLength * sizeof(const Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = new LSR[supportedLocalesLength];
        만약 (supportedLocales == nullptr || lsrs == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            반환;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supportedLocalesLength * sizeof(const Locale *));
        위해 (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            만약 (supportedLocales[i] == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
                반환;
            }
            const Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, errorCode);
            lsr.setHashCode();
            만약 (U_FAILURE(errorCode)) { 반환; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supportedLsrToIndex = uhash_openSize(hashLSR, compareLSRs, uhash_compareLong,
                                             supportedLocalesLength, &errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
        supportedLSRs = static_cast<const LSR **>(
            uprv_malloc(supportedLocalesLength * sizeof(const LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supportedLocalesLength * sizeof(int32_t)));
        만약 (supportedLSRs == nullptr || supportedIndexes == nullptr) {
            errorCode = U_MEMORY_ALLOCATION_ERROR;
            반환;
        }
        int32_t suppLength = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        MaybeStackArray<int8_t, 100> order(supportedLocalesLength, errorCode);
        만약 (U_FAILURE(errorCode)) { 반환; }
        int32_t numParadigms = 0;
        위해 (int32_t i = 0; i < supportedLocalesLength; ++i) {
            const Locale &locale = *supportedLocales[i];
            const LSR &lsr = lsrs[i];
            만약 (defLSR == nullptr && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                order[i] = 1;
                suppLength = putIfAbsent(lsr, 0, suppLength, errorCode);
            } 아니면 만약 (defLSR != nullptr && lsr.isEquivalentTo(*defLSR)) {
                order[i] = 1;
                suppLength = putIfAbsent(lsr, i, suppLength, errorCode);
            } 아니면 만약 (localeDistance.isParadigmLSR(lsr)) {
                order[i] = 2;
                ++numParadigms;
            } 아니면 {
                order[i] = 3;
            }
            만약 (U_FAILURE(errorCode)) { 반환; }
        }
        // Add supported paradigm locales.
        int32_t paradigmLimit = suppLength + numParadigms;
        위해 (int32_t i = 0; i < supportedLocalesLength && suppLength < paradigmLimit; ++i) {
            만약 (order[i] == 2) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        // Add remaining supported locales.
        위해 (int32_t i = 0; i < supportedLocalesLength; ++i) {
            만약 (order[i] == 3) {
                suppLength = putIfAbsent(lsrs[i], i, suppLength, errorCode);
            }
        }
        supportedLSRsLength = suppLength;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    만약 (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotionPerDesiredLocale = localeDistance.getDefaultDemotionPerDesiredLocale();
    }

    만약 (thresholdDistance >= 0) {
        // already copied
    } 아니면 만약 (builder.maxDistanceDesired_ != nullptr) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, errorCode);
        const LSR *pSuppLSR = &suppLSR;
        int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, errorCode),
                &pSuppLSR, 1,
                LocaleDistance::shiftDistance(100), favorSubtag, direction);
        만약 (U_SUCCESS(errorCode)) {
            // +1 for an exclusive threshold from an inclusive max.
            thresholdDistance = LocaleDistance::getDistanceFloor(indexAndDistance) + 1;
        } 아니면 {
            thresholdDistance = 0;
        }
    } 아니면 {
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
    위해 (int32_t i = 0; i < supportedLocalesLength; ++i) {
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
    반환 *this;
}

class LocaleLsrIterator {
public:
    LocaleLsrIterator(const LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULocMatchLifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        만약 (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            delete remembered;
        }
    }

    bool hasNext() const {
        반환 locales.hasNext();
    }

    LSR next(UErrorCode &errorCode) {
        current = &locales.next();
        반환 getMaximalLsrOrUnd(likelySubtags, *current, errorCode);
    }

    void rememberCurrent(int32_t desiredIndex, UErrorCode &errorCode) {
        만약 (U_FAILURE(errorCode)) { 반환; }
        bestDesiredIndex = desiredIndex;
        만약 (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } 아니면 {
            // ULOCMATCH_TEMPORARY_LOCALES
            delete remembered;
            remembered = new Locale(*current);
            만약 (remembered == nullptr) {
                errorCode = U_MEMORY_ALLOCATION_ERROR;
            }
        }
    }

    const Locale *orphanRemembered() {
        const Locale *rem = remembered;
        remembered = nullptr;
        반환 rem;
    }

    int32_t getBestDesiredIndex() const {
        반환 bestDesiredIndex;
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
    만약 (U_FAILURE(errorCode)) { 반환 nullptr; }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    반환 U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale* LocaleMatcher::getBestMatch(Locale::Iterator& desiredLocales,
                                          UErrorCode& errorCode) const U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode)) { 반환 nullptr; }
    만약 (!desiredLocales.hasNext()) {
        반환 defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    반환 U_SUCCESS(errorCode) && suppIndex.has_value() ? supportedLocales[*suppIndex]
                                                         : defaultLocale;
}

const Locale*
LocaleMatcher::getBestMatchForListString(StringPiece desiredLocaleList,
                                         UErrorCode& errorCode) const U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode)) { 반환 nullptr; }
    LocalePriorityList list(desiredLocaleList, errorCode);
    LocalePriorityList::Iterator iter = list.iterator();
    반환 getBestMatch(iter, errorCode);
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(const Locale& desiredLocale U_LIFETIME_BOUND,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode)) {
        반환 Result(nullptr, defaultLocale, -1, -1, false);
    }
    std::optional<int32_t> suppIndex = getBestSuppIndex(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, errorCode),
        nullptr, errorCode);
    만약 (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        반환 Result(nullptr, defaultLocale, -1, -1, false);
    } 아니면 {
        반환 Result(&desiredLocale, supportedLocales[*suppIndex], 0, *suppIndex, false);
    }
}

LocaleMatcher::Result
LocaleMatcher::getBestMatchResult(Locale::Iterator& desiredLocales,
                                  UErrorCode& errorCode) const U_LIFETIME_BOUND {
    만약 (U_FAILURE(errorCode) || !desiredLocales.hasNext()) {
        반환 Result(nullptr, defaultLocale, -1, -1, false);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> suppIndex = getBestSuppIndex(lsrIter.next(errorCode), &lsrIter, errorCode);
    만약 (U_FAILURE(errorCode) || !suppIndex.has_value()) {
        반환 Result(nullptr, defaultLocale, -1, -1, false);
    } 아니면 {
        반환 Result(lsrIter.orphanRemembered(), supportedLocales[*suppIndex],
                      lsrIter.getBestDesiredIndex(), *suppIndex, true);
    }
}

std::optional<int32_t> LocaleMatcher::getBestSuppIndex(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 std::nullopt; }
    int32_t desiredIndex = 0;
    int32_t bestSupportedLsrIndex = -1;
    위해 (int32_t bestShiftedDistance = LocaleDistance::shiftDistance(thresholdDistance);;) {
        // Quick check for exact maximized LSR.
        만약 (supportedLsrToIndex != nullptr) {
            desiredLSR.setHashCode();
            UBool found = false;
            int32_t suppIndex = uhash_getiAndFound(supportedLsrToIndex, &desiredLSR, &found);
            만약 (found) {
                만약 (remainingIter != nullptr) {
                    remainingIter->rememberCurrent(desiredIndex, errorCode);
                }
                반환 suppIndex;
            }
        }
        int32_t bestIndexAndDistance = localeDistance.getBestIndexAndDistance(
                desiredLSR, supportedLSRs, supportedLSRsLength,
                bestShiftedDistance, favorSubtag, direction);
        만약 (bestIndexAndDistance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(bestIndexAndDistance);
            만약 (remainingIter != nullptr) {
                remainingIter->rememberCurrent(desiredIndex, errorCode);
                만약 (U_FAILURE(errorCode)) { 반환 std::nullopt; }
            }
            bestSupportedLsrIndex = LocaleDistance::getIndex(bestIndexAndDistance);
        }
        만약 ((bestShiftedDistance -= LocaleDistance::shiftDistance(demotionPerDesiredLocale)) <= 0) {
            break;
        }
        만약 (remainingIter == nullptr || !remainingIter->hasNext()) {
            break;
        }
        desiredLSR = remainingIter->next(errorCode);
        만약 (U_FAILURE(errorCode)) { 반환 std::nullopt; }
        ++desiredIndex;
    }
    만약 (bestSupportedLsrIndex < 0) {
        // no good match
        반환 std::nullopt;
    }
    반환 supportedIndexes[bestSupportedLsrIndex];
}

UBool LocaleMatcher::isMatch(const Locale &desired, const Locale &supported,
                             UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 false; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 false; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    반환 indexAndDistance >= 0;
}

double LocaleMatcher::internalMatch(const Locale &desired, const Locale &supported, UErrorCode &errorCode) const {
    만약 (U_FAILURE(errorCode)) { 반환 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 0.; }
    const LSR *pSuppLSR = &suppLSR;
    int32_t indexAndDistance = localeDistance.getBestIndexAndDistance(
            getMaximalLsrOrUnd(likelySubtags, desired, errorCode),
            &pSuppLSR, 1,
            LocaleDistance::shiftDistance(thresholdDistance), favorSubtag, direction);
    double distance = LocaleDistance::getDistanceDouble(indexAndDistance);
    반환 (100.0 - distance) / 100.0;
}

U_NAMESPACE_END

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

namespace {

class LocaleFromTag {
public:
    LocaleFromTag() : locale(Locale::getRoot()) {}
    const Locale& operator()(const char* tag) U_LIFETIME_BOUND { 반환 locale = Locale(tag); }

private:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       char *dest, int32_t capacity, UAcceptResult *acceptResult,
                       UErrorCode &errorCode) {
    만약 (U_FAILURE(errorCode)) { 반환 0; }
    LocaleMatcher::Builder builder;
    const char *locString;
    동안 ((locString = uenum_next(&supportedLocales, nullptr, &errorCode)) != nullptr) {
        Locale loc(locString);
        만약 (loc.isBogus()) {
            errorCode = U_ILLEGAL_ARGUMENT_ERROR;
            반환 0;
        }
        builder.addSupportedLocale(loc);
    }
    LocaleMatcher matcher = builder.build(errorCode);
    LocaleMatcher::Result result = matcher.getBestMatchResult(desiredLocales, errorCode);
    만약 (U_FAILURE(errorCode)) { 반환 0; }
    만약 (result.getDesiredIndex() >= 0) {
        만약 (acceptResult != nullptr) {
            *acceptResult = *result.getDesiredLocale() == *result.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        const char *bestStr = result.getSupportedLocale()->getName();
        int32_t bestLength = static_cast<int32_t>(uprv_strlen(bestStr));
        만약 (bestLength <= capacity) {
            uprv_memcpy(dest, bestStr, bestLength);
        }
        반환 u_terminateChars(dest, capacity, bestLength, &errorCode);
    } 아니면 {
        만약 (acceptResult != nullptr) {
            *acceptResult = ULOC_ACCEPT_FAILED;
        }
        반환 u_terminateChars(dest, capacity, 0, &errorCode);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(char *result, int32_t resultAvailable,
                    UAcceptResult *outResult,
                    const char **acceptList, int32_t acceptListCount,
                    UEnumeration *availableLocales,
                    UErrorCode *status) {
    만약 (U_FAILURE(*status)) { 반환 0; }
    만약 ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            (acceptList == nullptr ? acceptListCount != 0 : acceptListCount < 0) ||
            availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        반환 0;
    }
    LocaleFromTag converter;
    Locale::ConvertingIterator<const char **, LocaleFromTag> desiredLocales(
        acceptList, acceptList + acceptListCount, converter);
    반환 acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(char *result, int32_t resultAvailable,
                            UAcceptResult *outResult,
                            const char *httpAcceptLanguage,
                            UEnumeration *availableLocales,
                            UErrorCode *status) {
    만약 (U_FAILURE(*status)) { 반환 0; }
    만약 ((result == nullptr ? resultAvailable != 0 : resultAvailable < 0) ||
            httpAcceptLanguage == nullptr || availableLocales == nullptr) {
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        반환 0;
    }
    LocalePriorityList list(httpAcceptLanguage, *status);
    LocalePriorityList::Iterator desiredLocales = list.iterator();
    반환 acceptLanguage(*availableLocales, desiredLocales,
                          result, resultAvailable, outResult, *status);
}
