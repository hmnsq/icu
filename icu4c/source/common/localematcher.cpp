// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ <optional>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/localebuilder.h"
#ಅನ್ತರ್ಭಾವ "unicode/localematcher.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uloc.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "localeprioritylist.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "locdistance.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Lifetime {
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
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ U_ಒಳ_DOXYGEN
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ULoc_ಹೊಂದಿಕೆ_Lifetime ULoc_ಹೊಂದಿಕೆ_Lifetime;
#ಆದರೆ_ಅಂತ್ಯ

U_NAMESPACE_BEGIN

LocaleMatcher::ಫಲಿತಾಂಶ_2::ಫಲಿತಾಂಶ_2(LocaleMatcher::ಫಲಿತಾಂಶ_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_ಸೂಚ್ಯಂಕ(src.desired_ಸೂಚ್ಯಂಕ),
        supported_ಸೂಚ್ಯಂಕ(src.supported_ಸೂಚ್ಯಂಕ),
        desiredIsOwned(src.desiredIsOwned) {
    ಆದರೆ (desiredIsOwned) {
        src.desiredLocale = ಶೂನ್ಯಸೂಚಕ;
        src.desired_ಸೂಚ್ಯಂಕ = -1;
        src.desiredIsOwned = ಸುಳ್ಳು;
    }
}

LocaleMatcher::ಫಲಿತಾಂಶ_2::~ಫಲಿತಾಂಶ_2() {
    ಆದರೆ (desiredIsOwned) {
        ಅಳಿಸು desiredLocale;
    }
}

LocaleMatcher::ಫಲಿತಾಂಶ_2&
LocaleMatcher::ಫಲಿತಾಂಶ_2::ಕ್ರಿಯಾಕಾರಕ=(LocaleMatcher::ಫಲಿತಾಂಶ_2&& src) noexcept U_LIFETIME_BOUND {
    ಇದು->~ಫಲಿತಾಂಶ_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_ಸೂಚ್ಯಂಕ = src.desired_ಸೂಚ್ಯಂಕ;
    supported_ಸೂಚ್ಯಂಕ = src.supported_ಸೂಚ್ಯಂಕ;
    desiredIsOwned = src.desiredIsOwned;

    ಆದರೆ (desiredIsOwned) {
        src.desiredLocale = ಶೂನ್ಯಸೂಚಕ;
        src.desired_ಸೂಚ್ಯಂಕ = -1;
        src.desiredIsOwned = ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

Locale LocaleMatcher::ಫಲಿತಾಂಶ_2::makeResolvedLocale(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || supportedLocale == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ Locale::get_ಬೇರು();
    }
    ಸ್ಥಿರಾಂಕ Locale *ಉತ್ತಮ_Desired = getDesiredLocale();
    ಆದರೆ (ಉತ್ತಮ_Desired == ಶೂನ್ಯಸೂಚಕ || *supportedLocale == *ಉತ್ತಮ_Desired) {
        ಹಿಂತಿರುಗಿ *supportedLocale;
    }
    LocaleBuilder b;
    b.ಗಣ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region = ಉತ್ತಮ_Desired->getCountry();
    ಆದರೆ (*region != 0) {
        b.ಗಣ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variants = ಉತ್ತಮ_Desired->getVariant();
    ಆದರೆ (*variants != 0) {
        b.ಗಣ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*ಉತ್ತಮ_Desired, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ b.build(error_ಸಂಕೇತ);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_ಸಂಕೇತ_2(src.error_ಸಂಕೇತ_2),
        supportedLocales_(src.supportedLocales_),
        ಮಿತಿ_ದೂರ(src.ಮಿತಿ_ದೂರ),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = ಶೂನ್ಯಸೂಚಕ;
    src.defaultLocale_ = ಶೂನ್ಯಸೂಚಕ;
}

LocaleMatcher::Builder::~Builder() {
    ಅಳಿಸು supportedLocales_;
    ಅಳಿಸು defaultLocale_;
    ಅಳಿಸು max_ದೂರ_Desired;
    ಅಳಿಸು max_ದೂರ_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಕ್ರಿಯಾಕಾರಕ=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    ಇದು->~Builder();

    error_ಸಂಕೇತ_2 = src.error_ಸಂಕೇತ_2;
    supportedLocales_ = src.supportedLocales_;
    ಮಿತಿ_ದೂರ = src.ಮಿತಿ_ದೂರ;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = ಶೂನ್ಯಸೂಚಕ;
    src.defaultLocale_ = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ LocaleMatcher::Builder::clearSupportedLocales() {
    ಆದರೆ (supportedLocales_ != ಶೂನ್ಯಸೂಚಕ) {
        supportedLocales_->removeAllElements();
    }
}

ತಾರ್ಕಿಕ LocaleMatcher::Builder::ensure_Supported_Locale_ಸದಿಶ() {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಆದರೆ (supportedLocales_ != ಶೂನ್ಯಸೂಚಕ) { ಹಿಂತಿರುಗಿ ನಿಜ; }
    Local_ಸೂಚಕ<UVector> lpSupportedLocales(ಹೊಸ UVector(uprv_deleteUObject, ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ_2), error_ಸಂಕೇತ_2);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ಆದ್ಯತೆ_List list(locales, error_ಸಂಕೇತ_2);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    clearSupportedLocales();
    ಆದರೆ (!ensure_Supported_Locale_ಸದಿಶ()) { ಹಿಂತಿರುಗಿ *ಇದು; }
    int32_t length = list.get_ಉದ್ದಳತೆ_Including_Removed();
    ಗಾಗಿ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ಆದರೆ (locale == ಶೂನ್ಯಸೂಚಕ) { ಮುಂದುವರಿ; }
        supportedLocales_->adoptElement(locale, error_ಸಂಕೇತ_2);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ಆದರೆ (ensure_Supported_Locale_ಸದಿಶ()) {
        clearSupportedLocales();
        ಆಗ (locales.has_ಮುಂದಿನ() && U_SUCCESS(error_ಸಂಕೇತ_2)) {
            ಸ್ಥಿರಾಂಕ Locale &locale = locales.next();
            Local_ಸೂಚಕ<Locale> clone (locale.clone(), error_ಸಂಕೇತ_2);
            supportedLocales_->adoptElement(clone.orphan(), error_ಸಂಕೇತ_2);
        }
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಕೂಡಿಸು_Supported_Locale(ಸ್ಥಿರಾಂಕ Locale& locale) U_LIFETIME_BOUND {
    ಆದರೆ (ensure_Supported_Locale_ಸದಿಶ()) {
        Local_ಸೂಚಕ<Locale> clone(locale.clone(), error_ಸಂಕೇತ_2);
        supportedLocales_->adoptElement(clone.orphan(), error_ಸಂಕೇತ_2);
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::ಗಣ_No_Default_Locale() U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಅಳಿಸು defaultLocale_;
    defaultLocale_ = ಶೂನ್ಯಸೂಚಕ;
    withDefault_ = ಸುಳ್ಳು;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Default_Locale(ಸ್ಥಿರಾಂಕ Locale* defaultLocale) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    Locale *clone = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (defaultLocale != ಶೂನ್ಯಸೂಚಕ) {
        clone = defaultLocale->clone();
        ಆದರೆ (clone == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ *ಇದು;
        }
    }
    ಅಳಿಸು defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = ನಿಜ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Favor_Subtag(ULoc_ಹೊಂದಿಕೆ_Favor_Subtag subtag) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    favor_ = subtag;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Demotion_ಪ್ರತಿ_Desired_Locale(ULoc_ಹೊಂದಿಕೆ_Demotion demotion) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    demotion_ = demotion;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ಗಣ_Max_ದೂರ(ಸ್ಥಿರಾಂಕ Locale& desired,
                                       ಸ್ಥಿರಾಂಕ Locale& supported) U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ಆದರೆ (desiredClone == ಶೂನ್ಯಸೂಚಕ || supportedClone == ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು desiredClone;  // in case only one could not be allocated
        ಅಳಿಸು supportedClone;
        error_ಸಂಕೇತ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಅಳಿಸು max_ದೂರ_Desired;
    ಅಳಿಸು max_ದೂರ_Supported;
    max_ದೂರ_Desired = desiredClone;
    max_ದೂರ_Supported = supportedClone;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

#ಆದರೆ 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::ಆಂತರಿಕ_ಗಣ_ಮಿತಿ_ದೂರ(int32_t ಮಿತಿ_ದೂರ_2) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ *ಇದು; }
    ಆದರೆ (ಮಿತಿ_ದೂರ_2 > 100) {
        ಮಿತಿ_ದೂರ_2 = 100;
    }
    ಮಿತಿ_ದೂರ = ಮಿತಿ_ದೂರ_2;
    ಹಿಂತಿರುಗಿ *ಇದು;
}
#ಆದರೆ_ಅಂತ್ಯ

UBool LocaleMatcher::Builder::copy_Error_ಗಮ್ಯ(UError_ಸಂಕೇತ &ಹೊರಹೋಗು_Error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(ಹೊರಹೋಗು_Error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ನಿಜ; }
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ_2)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಹೊರಹೋಗು_Error_ಸಂಕೇತ = error_ಸಂಕೇತ_2;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ) && U_FAILURE(error_ಸಂಕೇತ_2)) {
        error_ಸಂಕೇತ = error_ಸಂಕೇತ_2;
    }
    ಹಿಂತಿರುಗಿ LocaleMatcher(*ಇದು, error_ಸಂಕೇತ);
}

ನಾಮಾಕಾಶ {

LSR getMaximalLsrOrUnd(ಸ್ಥಿರಾಂಕ LikelySubtags &likelySubtags, ಸ್ಥಿರಾಂಕ Locale &locale,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ಹಿಂತಿರುಗಿ UND_LSR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ likelySubtags.makeMaximizedLsrFrom(locale, ಸುಳ್ಳು, error_ಸಂಕೇತ);
    }
}

int32_t hashLSR(ಸ್ಥಿರಾಂಕ UHashTok token) {
    ಸ್ಥಿರಾಂಕ LSR *lsr = static_cast<ಸ್ಥಿರಾಂಕ LSR *>(token.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ lsr->hash_ಸಂಕೇತ;
}

UBool compareLSRs(ಸ್ಥಿರಾಂಕ UHashTok t1, ಸ್ಥಿರಾಂಕ UHashTok t2) {
    ಸ್ಥಿರಾಂಕ LSR *lsr1 = static_cast<ಸ್ಥಿರಾಂಕ LSR *>(t1.ಸೂಚಕ);
    ಸ್ಥಿರಾಂಕ LSR *lsr2 = static_cast<ಸ್ಥಿರಾಂಕ LSR *>(t2.ಸೂಚಕ);
    ಹಿಂತಿರುಗಿ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ಸ್ಥಿರಾಂಕ LSR &lsr, int32_t i, int32_t supp_ಉದ್ದಳತೆ,
                                   UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ supp_ಉದ್ದಳತೆ; }
    ಆದರೆ (!uhash_ಒಳಗೊಂಡಿದೆ_ಕೀಲಿ(supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ, const_cast<LSR *>(&lsr), i, &error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            supportedLSRs[supp_ಉದ್ದಳತೆ] = &lsr;
            supportedIndexes[supp_ಉದ್ದಳತೆ++] = i;
        }
    }
    ಹಿಂತಿರುಗಿ supp_ಉದ್ದಳತೆ;
}

LocaleMatcher::LocaleMatcher(ಸ್ಥಿರಾಂಕ Builder &builder, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) :
        likelySubtags(*LikelySubtags::getSingleton(error_ಸಂಕೇತ)),
        locale_ದೂರ(*Locale_ದೂರ::getSingleton(error_ಸಂಕೇತ)),
        ಮಿತಿ_ದೂರ_2(builder.ಮಿತಿ_ದೂರ),
        demotion_ಪ್ರತಿ_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(ಶೂನ್ಯಸೂಚಕ), lsrs(ಶೂನ್ಯಸೂಚಕ), supported_Locales_ಉದ್ದಳತೆ(0),
        supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(ಶೂನ್ಯಸೂಚಕ),
        supportedLSRs(ಶೂನ್ಯಸೂಚಕ), supportedIndexes(ಶೂನ್ಯಸೂಚಕ), supported_LSRs_ಉದ್ದಳತೆ(0),
        ownedDefaultLocale(ಶೂನ್ಯಸೂಚಕ), defaultLocale(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    ಸ್ಥಿರಾಂಕ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ಸ್ಥಿರಾಂಕ LSR *defLSR = ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ (def != ಶೂನ್ಯಸೂಚಕ) {
        ownedDefaultLocale = def->clone();
        ಆದರೆ (ownedDefaultLocale == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_ಉದ್ದಳತೆ = builder.supportedLocales_ != ಶೂನ್ಯಸೂಚಕ ?
        builder.supportedLocales_->size() : 0;
    ಆದರೆ (supported_Locales_ಉದ್ದಳತೆ > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ಸ್ಥಿರಾಂಕ Locale **>(
            uprv_malloc(supported_Locales_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = ಹೊಸ LSR[supported_Locales_ಉದ್ದಳತೆ];
        ಆದರೆ (supportedLocales == ಶೂನ್ಯಸೂಚಕ || lsrs == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ Locale *));
        ಗಾಗಿ (int32_t i = 0; i < supported_Locales_ಉದ್ದಳತೆ; ++i) {
            ಸ್ಥಿರಾಂಕ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ಆದರೆ (supportedLocales[i] == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಸ್ಥಿರಾಂಕ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_ಸಂಕೇತ);
            lsr.ಗಣ_Hash_ಸಂಕೇತ();
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ = uhash_open_ಗಾತ್ರ(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        supportedLSRs = static_cast<ಸ್ಥಿರಾಂಕ LSR **>(
            uprv_malloc(supported_Locales_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_ಉದ್ದಳತೆ * ಗಾತ್ರ(int32_t)));
        ಆದರೆ (supportedLSRs == ಶೂನ್ಯಸೂಚಕ || supportedIndexes == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        int32_t supp_ಉದ್ದಳತೆ = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ರಾಶಿ_Array<int8_t, 100> ಕ್ರಮ(supported_Locales_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        int32_t ಅಂಕೆ_Paradigms = 0;
        ಗಾಗಿ (int32_t i = 0; i < supported_Locales_ಉದ್ದಳತೆ; ++i) {
            ಸ್ಥಿರಾಂಕ Locale &locale = *supportedLocales[i];
            ಸ್ಥಿರಾಂಕ LSR &lsr = lsrs[i];
            ಆದರೆ (defLSR == ಶೂನ್ಯಸೂಚಕ && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ಕ್ರಮ[i] = 1;
                supp_ಉದ್ದಳತೆ = putIfAbsent(lsr, 0, supp_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (defLSR != ಶೂನ್ಯಸೂಚಕ && lsr.is_Equivalent_ಗಮ್ಯ(*defLSR)) {
                ಕ್ರಮ[i] = 1;
                supp_ಉದ್ದಳತೆ = putIfAbsent(lsr, i, supp_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (locale_ದೂರ.isParadigmLSR(lsr)) {
                ಕ್ರಮ[i] = 2;
                ++ಅಂಕೆ_Paradigms;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಕ್ರಮ[i] = 3;
            }
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_ಎಲ್ಲೆ = supp_ಉದ್ದಳತೆ + ಅಂಕೆ_Paradigms;
        ಗಾಗಿ (int32_t i = 0; i < supported_Locales_ಉದ್ದಳತೆ && supp_ಉದ್ದಳತೆ < paradigm_ಎಲ್ಲೆ; ++i) {
            ಆದರೆ (ಕ್ರಮ[i] == 2) {
                supp_ಉದ್ದಳತೆ = putIfAbsent(lsrs[i], i, supp_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
            }
        }
        // Add remaining supported locales.
        ಗಾಗಿ (int32_t i = 0; i < supported_Locales_ಉದ್ದಳತೆ; ++i) {
            ಆದರೆ (ಕ್ರಮ[i] == 3) {
                supp_ಉದ್ದಳತೆ = putIfAbsent(lsrs[i], i, supp_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
            }
        }
        supported_LSRs_ಉದ್ದಳತೆ = supp_ಉದ್ದಳತೆ;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ಆದರೆ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_ಪ್ರತಿ_Desired_Locale = locale_ದೂರ.get_Default_Demotion_ಪ್ರತಿ_Desired_Locale();
    }

    ಆದರೆ (ಮಿತಿ_ದೂರ_2 >= 0) {
        // already copied
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (builder.max_ದೂರ_Desired != ಶೂನ್ಯಸೂಚಕ) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_ದೂರ_Supported, error_ಸಂಕೇತ);
        ಸ್ಥಿರಾಂಕ LSR *pSuppLSR = &suppLSR;
        int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ = locale_ದೂರ.get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_ದೂರ_Desired, error_ಸಂಕೇತ),
                &pSuppLSR, 1,
                Locale_ದೂರ::ಸರಿಸು_ದೂರ(100), favorSubtag, direction);
        ಆದರೆ (U_SUCCESS(error_ಸಂಕೇತ)) {
            // +1 for an exclusive threshold from an inclusive max.
            ಮಿತಿ_ದೂರ_2 = Locale_ದೂರ::get_ದೂರ_Floor(ಸೂಚ್ಯಂಕ_And_ದೂರ) + 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಮಿತಿ_ದೂರ_2 = 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಮಿತಿ_ದೂರ_2 = locale_ದೂರ.get_Default_Script_ದೂರ();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_ದೂರ(src.locale_ದೂರ),
        ಮಿತಿ_ದೂರ_2(src.ಮಿತಿ_ದೂರ_2),
        demotion_ಪ್ರತಿ_Desired_Locale(src.demotion_ಪ್ರತಿ_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_ಉದ್ದಳತೆ(src.supported_Locales_ಉದ್ದಳತೆ),
        supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(src.supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_ಉದ್ದಳತೆ(src.supported_LSRs_ಉದ್ದಳತೆ),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = ಶೂನ್ಯಸೂಚಕ;
    src.lsrs = ಶೂನ್ಯಸೂಚಕ;
    src.supported_Locales_ಉದ್ದಳತೆ = 0;
    src.supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ = ಶೂನ್ಯಸೂಚಕ;
    src.supportedLSRs = ಶೂನ್ಯಸೂಚಕ;
    src.supportedIndexes = ಶೂನ್ಯಸೂಚಕ;
    src.supported_LSRs_ಉದ್ದಳತೆ = 0;
    src.ownedDefaultLocale = ಶೂನ್ಯಸೂಚಕ;
    src.defaultLocale = ಶೂನ್ಯಸೂಚಕ;
}

LocaleMatcher::~LocaleMatcher() {
    ಗಾಗಿ (int32_t i = 0; i < supported_Locales_ಉದ್ದಳತೆ; ++i) {
        ಅಳಿಸು supportedLocales[i];
    }
    uprv_free(supportedLocales);
    ಅಳಿಸು[] lsrs;
    uhash_close(supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    ಅಳಿಸು ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::ಕ್ರಿಯಾಕಾರಕ=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    ಇದು->~LocaleMatcher();

    ಮಿತಿ_ದೂರ_2 = src.ಮಿತಿ_ದೂರ_2;
    demotion_ಪ್ರತಿ_Desired_Locale = src.demotion_ಪ್ರತಿ_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_ಉದ್ದಳತೆ = src.supported_Locales_ಉದ್ದಳತೆ;
    supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ = src.supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_ಉದ್ದಳತೆ = src.supported_LSRs_ಉದ್ದಳತೆ;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = ಶೂನ್ಯಸೂಚಕ;
    src.lsrs = ಶೂನ್ಯಸೂಚಕ;
    src.supported_Locales_ಉದ್ದಳತೆ = 0;
    src.supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ = ಶೂನ್ಯಸೂಚಕ;
    src.supportedLSRs = ಶೂನ್ಯಸೂಚಕ;
    src.supportedIndexes = ಶೂನ್ಯಸೂಚಕ;
    src.supported_LSRs_ಉದ್ದಳತೆ = 0;
    src.ownedDefaultLocale = ಶೂನ್ಯಸೂಚಕ;
    src.defaultLocale = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ವರ್ಗ LocaleLsrIterator {
ಸಾರ್ವಜನಿಕ:
    LocaleLsrIterator(ಸ್ಥಿರಾಂಕ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_ಹೊಂದಿಕೆ_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ಆದರೆ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            ಅಳಿಸು remembered;
        }
    }

    ತಾರ್ಕಿಕ has_ಮುಂದಿನ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ locales.has_ಮುಂದಿನ();
    }

    LSR next(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        current = &locales.next();
        ಹಿಂತಿರುಗಿ getMaximalLsrOrUnd(likelySubtags, *current, error_ಸಂಕೇತ);
    }

    ಶೂನ್ಯ rememberCurrent(int32_t desired_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ಉತ್ತಮ_Desired_ಸೂಚ್ಯಂಕ = desired_ಸೂಚ್ಯಂಕ;
        ಆದರೆ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // ULOCMATCH_TEMPORARY_LOCALES
            ಅಳಿಸು remembered;
            remembered = ಹೊಸ Locale(*current);
            ಆದರೆ (remembered == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            }
        }
    }

    ಸ್ಥಿರಾಂಕ Locale *orphanRemembered() {
        ಸ್ಥಿರಾಂಕ Locale *rem = remembered;
        remembered = ಶೂನ್ಯಸೂಚಕ;
        ಹಿಂತಿರುಗಿ rem;
    }

    int32_t get_ಉತ್ತಮ_Desired_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ ಉತ್ತಮ_Desired_ಸೂಚ್ಯಂಕ;
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರಾಂಕ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_ಹೊಂದಿಕೆ_Lifetime lifetime;
    ಸ್ಥಿರಾಂಕ Locale *current = ಶೂನ್ಯಸೂಚಕ, *remembered = ಶೂನ್ಯಸೂಚಕ;
    int32_t ಉತ್ತಮ_Desired_ಸೂಚ್ಯಂಕ = -1;
};

ಸ್ಥಿರಾಂಕ Locale* LocaleMatcher::get_ಉತ್ತಮ_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale& desiredLocale,
                                          UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    std::optional<int32_t> supp_ಸೂಚ್ಯಂಕ = get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ಸಂಕೇತ),
        ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && supp_ಸೂಚ್ಯಂಕ.has_value() ? supportedLocales[*supp_ಸೂಚ್ಯಂಕ]
                                                         : defaultLocale;
}

ಸ್ಥಿರಾಂಕ Locale* LocaleMatcher::get_ಉತ್ತಮ_ಹೊಂದಿಕೆ(Locale::Iterator& desiredLocales,
                                          UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    ಆದರೆ (!desiredLocales.has_ಮುಂದಿನ()) {
        ಹಿಂತಿರುಗಿ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_ಸೂಚ್ಯಂಕ = get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(lsrIter.next(error_ಸಂಕೇತ), &lsrIter, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(error_ಸಂಕೇತ) && supp_ಸೂಚ್ಯಂಕ.has_value() ? supportedLocales[*supp_ಸೂಚ್ಯಂಕ]
                                                         : defaultLocale;
}

ಸ್ಥಿರಾಂಕ Locale*
LocaleMatcher::get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_For_List_String(StringPiece desiredLocaleList,
                                         UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    Locale_ಆದ್ಯತೆ_List list(desiredLocaleList, error_ಸಂಕೇತ);
    Locale_ಆದ್ಯತೆ_List::Iterator iter = list.iterator();
    ಹಿಂತಿರುಗಿ get_ಉತ್ತಮ_ಹೊಂದಿಕೆ(iter, error_ಸಂಕೇತ);
}

LocaleMatcher::ಫಲಿತಾಂಶ_2
LocaleMatcher::get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(ಸ್ಥಿರಾಂಕ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ, defaultLocale, -1, -1, ಸುಳ್ಳು);
    }
    std::optional<int32_t> supp_ಸೂಚ್ಯಂಕ = get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ಸಂಕೇತ),
        ಶೂನ್ಯಸೂಚಕ, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || !supp_ಸೂಚ್ಯಂಕ.has_value()) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ, defaultLocale, -1, -1, ಸುಳ್ಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(&desiredLocale, supportedLocales[*supp_ಸೂಚ್ಯಂಕ], 0, *supp_ಸೂಚ್ಯಂಕ, ಸುಳ್ಳು);
    }
}

LocaleMatcher::ಫಲಿತಾಂಶ_2
LocaleMatcher::get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(Locale::Iterator& desiredLocales,
                                  UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || !desiredLocales.has_ಮುಂದಿನ()) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ, defaultLocale, -1, -1, ಸುಳ್ಳು);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_ಸೂಚ್ಯಂಕ = get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(lsrIter.next(error_ಸಂಕೇತ), &lsrIter, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || !supp_ಸೂಚ್ಯಂಕ.has_value()) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(ಶೂನ್ಯಸೂಚಕ, defaultLocale, -1, -1, ಸುಳ್ಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2(lsrIter.orphanRemembered(), supportedLocales[*supp_ಸೂಚ್ಯಂಕ],
                      lsrIter.get_ಉತ್ತಮ_Desired_ಸೂಚ್ಯಂಕ(), *supp_ಸೂಚ್ಯಂಕ, ನಿಜ);
    }
}

std::optional<int32_t> LocaleMatcher::get_ಉತ್ತಮ_Supp_ಸೂಚ್ಯಂಕ(LSR desiredLSR,
                                                       LocaleLsrIterator *ಉಳಿದ_Iter,
                                                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ std::nullopt; }
    int32_t desired_ಸೂಚ್ಯಂಕ = 0;
    int32_t ಉತ್ತಮ_Supported_Lsr_ಸೂಚ್ಯಂಕ = -1;
    ಗಾಗಿ (int32_t ಉತ್ತಮ_Shifted_ದೂರ = Locale_ದೂರ::ಸರಿಸು_ದೂರ(ಮಿತಿ_ದೂರ_2);;) {
        // Quick check for exact maximized LSR.
        ಆದರೆ (supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ != ಶೂನ್ಯಸೂಚಕ) {
            desiredLSR.ಗಣ_Hash_ಸಂಕೇತ();
            UBool ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
            int32_t supp_ಸೂಚ್ಯಂಕ = uhash_geti_And_ಸಿಕ್ಕಿತು(supported_Lsr_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ, &desiredLSR, &ಸಿಕ್ಕಿತು);
            ಆದರೆ (ಸಿಕ್ಕಿತು) {
                ಆದರೆ (ಉಳಿದ_Iter != ಶೂನ್ಯಸೂಚಕ) {
                    ಉಳಿದ_Iter->rememberCurrent(desired_ಸೂಚ್ಯಂಕ, error_ಸಂಕೇತ);
                }
                ಹಿಂತಿರುಗಿ supp_ಸೂಚ್ಯಂಕ;
            }
        }
        int32_t ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ = locale_ದೂರ.get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
                desiredLSR, supportedLSRs, supported_LSRs_ಉದ್ದಳತೆ,
                ಉತ್ತಮ_Shifted_ದೂರ, favorSubtag, direction);
        ಆದರೆ (ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ >= 0) {
            ಉತ್ತಮ_Shifted_ದೂರ = Locale_ದೂರ::get_Shifted_ದೂರ(ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ);
            ಆದರೆ (ಉಳಿದ_Iter != ಶೂನ್ಯಸೂಚಕ) {
                ಉಳಿದ_Iter->rememberCurrent(desired_ಸೂಚ್ಯಂಕ, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ std::nullopt; }
            }
            ಉತ್ತಮ_Supported_Lsr_ಸೂಚ್ಯಂಕ = Locale_ದೂರ::get_ಸೂಚ್ಯಂಕ(ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ);
        }
        ಆದರೆ ((ಉತ್ತಮ_Shifted_ದೂರ -= Locale_ದೂರ::ಸರಿಸು_ದೂರ(demotion_ಪ್ರತಿ_Desired_Locale)) <= 0) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ಉಳಿದ_Iter == ಶೂನ್ಯಸೂಚಕ || !ಉಳಿದ_Iter->has_ಮುಂದಿನ()) {
            ನಿಲ್ಲಿಸು;
        }
        desiredLSR = ಉಳಿದ_Iter->next(error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ std::nullopt; }
        ++desired_ಸೂಚ್ಯಂಕ;
    }
    ಆದರೆ (ಉತ್ತಮ_Supported_Lsr_ಸೂಚ್ಯಂಕ < 0) {
        // no good match
        ಹಿಂತಿರುಗಿ std::nullopt;
    }
    ಹಿಂತಿರುಗಿ supportedIndexes[ಉತ್ತಮ_Supported_Lsr_ಸೂಚ್ಯಂಕ];
}

UBool LocaleMatcher::is_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale &desired, ಸ್ಥಿರಾಂಕ Locale &supported,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    ಸ್ಥಿರಾಂಕ LSR *pSuppLSR = &suppLSR;
    int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ = locale_ದೂರ.get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ಸಂಕೇತ),
            &pSuppLSR, 1,
            Locale_ದೂರ::ಸರಿಸು_ದೂರ(ಮಿತಿ_ದೂರ_2), favorSubtag, direction);
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_And_ದೂರ >= 0;
}

ದ್ವಿಗುಣ LocaleMatcher::ಆಂತರಿಕ_ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ Locale &desired, ಸ್ಥಿರಾಂಕ Locale &supported, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0.; }
    ಸ್ಥಿರಾಂಕ LSR *pSuppLSR = &suppLSR;
    int32_t ಸೂಚ್ಯಂಕ_And_ದೂರ = locale_ದೂರ.get_ಉತ್ತಮ_ಸೂಚ್ಯಂಕ_And_ದೂರ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ಸಂಕೇತ),
            &pSuppLSR, 1,
            Locale_ದೂರ::ಸರಿಸು_ದೂರ(ಮಿತಿ_ದೂರ_2), favorSubtag, direction);
    ದ್ವಿಗುಣ ದೂರ = Locale_ದೂರ::get_ದೂರ_Double(ಸೂಚ್ಯಂಕ_And_ದೂರ);
    ಹಿಂತಿರುಗಿ (100.0 - ದೂರ) / 100.0;
}

U_NAMESPACE_ಕೊನೆ

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

ನಾಮಾಕಾಶ {

ವರ್ಗ Locale_From_ಟ್ಯಾಗ್ {
ಸಾರ್ವಜನಿಕ:
    Locale_From_ಟ್ಯಾಗ್() : locale(Locale::get_ಬೇರು()) {}
    ಸ್ಥಿರಾಂಕ Locale& ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಟ್ಯಾಗ್) U_LIFETIME_BOUND { ಹಿಂತಿರುಗಿ locale = Locale(ಟ್ಯಾಗ್); }

ಖಾಸಗಿ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       ಅಕ್ಷರ *dest, int32_t capacity, UAccept_ಫಲಿತಾಂಶ *accept_ಫಲಿತಾಂಶ,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    LocaleMatcher::Builder builder;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locString;
    ಆಗ ((locString = uenum_ಮುಂದಿನ(&supportedLocales, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ)) != ಶೂನ್ಯಸೂಚಕ) {
        Locale loc(locString);
        ಆದರೆ (loc.isBogus()) {
            error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        builder.ಕೂಡಿಸು_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_ಸಂಕೇತ);
    LocaleMatcher::ಫಲಿತಾಂಶ_2 ಫಲಿತಾಂಶ = matcher.get_ಉತ್ತಮ_ಹೊಂದಿಕೆ_ಫಲಿತಾಂಶ(desiredLocales, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ (ಫಲಿತಾಂಶ.get_Desired_ಸೂಚ್ಯಂಕ() >= 0) {
        ಆದರೆ (accept_ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            *accept_ಫಲಿತಾಂಶ = *ಫಲಿತಾಂಶ.getDesiredLocale() == *ಫಲಿತಾಂಶ.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಉತ್ತಮ_Str = ಫಲಿತಾಂಶ.getSupportedLocale()->getName();
        int32_t ಉತ್ತಮ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(ಉತ್ತಮ_Str));
        ಆದರೆ (ಉತ್ತಮ_ಉದ್ದಳತೆ <= capacity) {
            uprv_memcpy(dest, ಉತ್ತಮ_Str, ಉತ್ತಮ_ಉದ್ದಳತೆ);
        }
        ಹಿಂತಿರುಗಿ u_terminateChars(dest, capacity, ಉತ್ತಮ_ಉದ್ದಳತೆ, &error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (accept_ಫಲಿತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
            *accept_ಫಲಿತಾಂಶ = ULOC_ACCEPT_FAILED;
        }
        ಹಿಂತಿರುಗಿ u_terminateChars(dest, capacity, 0, &error_ಸಂಕೇತ);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(ಅಕ್ಷರ *ಫಲಿತಾಂಶ, int32_t ಫಲಿತಾಂಶ_ಲಭ್ಯ,
                    UAccept_ಫಲಿತಾಂಶ *ಹೊರಹೋಗು_ಫಲಿತಾಂಶ,
                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **acceptList, int32_t accept_List_ಎಣಿಕೆ,
                    UEnumeration *ಲಭ್ಯ_Locales,
                    UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ ((ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ ? ಫಲಿತಾಂಶ_ಲಭ್ಯ != 0 : ಫಲಿತಾಂಶ_ಲಭ್ಯ < 0) ||
            (acceptList == ಶೂನ್ಯಸೂಚಕ ? accept_List_ಎಣಿಕೆ != 0 : accept_List_ಎಣಿಕೆ < 0) ||
            ಲಭ್ಯ_Locales == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    Locale_From_ಟ್ಯಾಗ್ converter;
    Locale::ConvertingIterator<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **, Locale_From_ಟ್ಯಾಗ್> desiredLocales(
        acceptList, acceptList + accept_List_ಎಣಿಕೆ, converter);
    ಹಿಂತಿರುಗಿ acceptLanguage(*ಲಭ್ಯ_Locales, desiredLocales,
                          ಫಲಿತಾಂಶ, ಫಲಿತಾಂಶ_ಲಭ್ಯ, ಹೊರಹೋಗು_ಫಲಿತಾಂಶ, *ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(ಅಕ್ಷರ *ಫಲಿತಾಂಶ, int32_t ಫಲಿತಾಂಶ_ಲಭ್ಯ,
                            UAccept_ಫಲಿತಾಂಶ *ಹೊರಹೋಗು_ಫಲಿತಾಂಶ,
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *httpAcceptLanguage,
                            UEnumeration *ಲಭ್ಯ_Locales,
                            UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ 0; }
    ಆದರೆ ((ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ ? ಫಲಿತಾಂಶ_ಲಭ್ಯ != 0 : ಫಲಿತಾಂಶ_ಲಭ್ಯ < 0) ||
            httpAcceptLanguage == ಶೂನ್ಯಸೂಚಕ || ಲಭ್ಯ_Locales == ಶೂನ್ಯಸೂಚಕ) {
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    Locale_ಆದ್ಯತೆ_List list(httpAcceptLanguage, *ಸ್ಥಿತಿ_2);
    Locale_ಆದ್ಯತೆ_List::Iterator desiredLocales = list.iterator();
    ಹಿಂತಿರುಗಿ acceptLanguage(*ಲಭ್ಯ_Locales, desiredLocales,
                          ಫಲಿತಾಂಶ, ಫಲಿತಾಂಶ_ಲಭ್ಯ, ಹೊರಹೋಗು_ಫಲಿತಾಂಶ, *ಸ್ಥಿತಿ_2);
}
