// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ <optional>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localematcher.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringpiece.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uloc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "localeprioritylist.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "loclikelysubtags.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locdistance.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lsr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
ਗਿਣਤੀ ULoc_ਮੇਲ_Lifetime {
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
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ U_ਵਿੱਚ_DOXYGEN
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ULoc_ਮੇਲ_Lifetime ULoc_ਮੇਲ_Lifetime;
#ਜੇ_ਅੰਤ

U_NAMESPACE_BEGIN

LocaleMatcher::ਨਤੀਜਾ_2::ਨਤੀਜਾ_2(LocaleMatcher::ਨਤੀਜਾ_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_ਸੂਚਕਾਂਕ(src.desired_ਸੂਚਕਾਂਕ),
        supported_ਸੂਚਕਾਂਕ(src.supported_ਸੂਚਕਾਂਕ),
        desiredIsOwned(src.desiredIsOwned) {
    ਜੇ (desiredIsOwned) {
        src.desiredLocale = ਖਾਲੀ_ਸੂਚਕ;
        src.desired_ਸੂਚਕਾਂਕ = -1;
        src.desiredIsOwned = ਝੂਠ;
    }
}

LocaleMatcher::ਨਤੀਜਾ_2::~ਨਤੀਜਾ_2() {
    ਜੇ (desiredIsOwned) {
        ਮਿਟਾਓ desiredLocale;
    }
}

LocaleMatcher::ਨਤੀਜਾ_2&
LocaleMatcher::ਨਤੀਜਾ_2::ਸੰਚਾਲਕ=(LocaleMatcher::ਨਤੀਜਾ_2&& src) noexcept U_LIFETIME_BOUND {
    ਇਹ->~ਨਤੀਜਾ_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_ਸੂਚਕਾਂਕ = src.desired_ਸੂਚਕਾਂਕ;
    supported_ਸੂਚਕਾਂਕ = src.supported_ਸੂਚਕਾਂਕ;
    desiredIsOwned = src.desiredIsOwned;

    ਜੇ (desiredIsOwned) {
        src.desiredLocale = ਖਾਲੀ_ਸੂਚਕ;
        src.desired_ਸੂਚਕਾਂਕ = -1;
        src.desiredIsOwned = ਝੂਠ;
    }
    ਵਾਪਸ *ਇਹ;
}

Locale LocaleMatcher::ਨਤੀਜਾ_2::makeResolvedLocale(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ) || supportedLocale == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ Locale::get_ਜੜ੍ਹ();
    }
    ਅਚਲ Locale *ਉੱਤਮ_Desired = getDesiredLocale();
    ਜੇ (ਉੱਤਮ_Desired == ਖਾਲੀ_ਸੂਚਕ || *supportedLocale == *ਉੱਤਮ_Desired) {
        ਵਾਪਸ *supportedLocale;
    }
    LocaleBuilder b;
    b.ਸੈੱਟ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ਅਚਲ ਅੱਖਰ *region = ਉੱਤਮ_Desired->getCountry();
    ਜੇ (*region != 0) {
        b.ਸੈੱਟ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ਅਚਲ ਅੱਖਰ *variants = ਉੱਤਮ_Desired->getVariant();
    ਜੇ (*variants != 0) {
        b.ਸੈੱਟ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*ਉੱਤਮ_Desired, error_ਕੋਡ);
    ਵਾਪਸ b.build(error_ਕੋਡ);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_ਕੋਡ_2(src.error_ਕੋਡ_2),
        supportedLocales_(src.supportedLocales_),
        ਹੱਦ_ਦੂਰੀ(src.ਹੱਦ_ਦੂਰੀ),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = ਖਾਲੀ_ਸੂਚਕ;
    src.defaultLocale_ = ਖਾਲੀ_ਸੂਚਕ;
}

LocaleMatcher::Builder::~Builder() {
    ਮਿਟਾਓ supportedLocales_;
    ਮਿਟਾਓ defaultLocale_;
    ਮਿਟਾਓ max_ਦੂਰੀ_Desired;
    ਮਿਟਾਓ max_ਦੂਰੀ_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੰਚਾਲਕ=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    ਇਹ->~Builder();

    error_ਕੋਡ_2 = src.error_ਕੋਡ_2;
    supportedLocales_ = src.supportedLocales_;
    ਹੱਦ_ਦੂਰੀ = src.ਹੱਦ_ਦੂਰੀ;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = ਖਾਲੀ_ਸੂਚਕ;
    src.defaultLocale_ = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ *ਇਹ;
}

ਖਾਲੀ LocaleMatcher::Builder::clearSupportedLocales() {
    ਜੇ (supportedLocales_ != ਖਾਲੀ_ਸੂਚਕ) {
        supportedLocales_->removeAllElements();
    }
}

ਤਾਰਕਿਕ LocaleMatcher::Builder::ensure_Supported_Locale_ਸਦਿਸ਼() {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ ਝੂਠ; }
    ਜੇ (supportedLocales_ != ਖਾਲੀ_ਸੂਚਕ) { ਵਾਪਸ ਸੱਚ; }
    Local_ਸੂਚਕ<UVector> lpSupportedLocales(ਨਵਾਂ UVector(uprv_deleteUObject, ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ_2), error_ਕੋਡ_2);
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ ਝੂਠ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    ਵਾਪਸ ਸੱਚ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ਤਰਜੀਹ_List list(locales, error_ਕੋਡ_2);
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    clearSupportedLocales();
    ਜੇ (!ensure_Supported_Locale_ਸਦਿਸ਼()) { ਵਾਪਸ *ਇਹ; }
    int32_t length = list.get_ਲੰਮਾਈ_Including_Removed();
    ਲਈ (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ਜੇ (locale == ਖਾਲੀ_ਸੂਚਕ) { ਜਾਰੀ; }
        supportedLocales_->adoptElement(locale, error_ਕੋਡ_2);
        ਜੇ (U_FAILURE(error_ਕੋਡ_2)) {
            ਰੋਕੋ;
        }
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ਜੇ (ensure_Supported_Locale_ਸਦਿਸ਼()) {
        clearSupportedLocales();
        ਜਦੋਂ (locales.has_ਅਗਲਾ() && U_SUCCESS(error_ਕੋਡ_2)) {
            ਅਚਲ Locale &locale = locales.next();
            Local_ਸੂਚਕ<Locale> clone (locale.clone(), error_ਕੋਡ_2);
            supportedLocales_->adoptElement(clone.orphan(), error_ਕੋਡ_2);
        }
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਜੋੜੋ_Supported_Locale(ਅਚਲ Locale& locale) U_LIFETIME_BOUND {
    ਜੇ (ensure_Supported_Locale_ਸਦਿਸ਼()) {
        Local_ਸੂਚਕ<Locale> clone(locale.clone(), error_ਕੋਡ_2);
        supportedLocales_->adoptElement(clone.orphan(), error_ਕੋਡ_2);
    }
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::ਸੈੱਟ_No_Default_Locale() U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    ਮਿਟਾਓ defaultLocale_;
    defaultLocale_ = ਖਾਲੀ_ਸੂਚਕ;
    withDefault_ = ਝੂਠ;
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Default_Locale(ਅਚਲ Locale* defaultLocale) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    Locale *clone = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (defaultLocale != ਖਾਲੀ_ਸੂਚਕ) {
        clone = defaultLocale->clone();
        ਜੇ (clone == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ *ਇਹ;
        }
    }
    ਮਿਟਾਓ defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = ਸੱਚ;
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Favor_Subtag(ULoc_ਮੇਲ_Favor_Subtag subtag) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    favor_ = subtag;
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Demotion_ਪ੍ਰਤੀ_Desired_Locale(ULoc_ਮੇਲ_Demotion demotion) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    demotion_ = demotion;
    ਵਾਪਸ *ਇਹ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::ਸੈੱਟ_Max_ਦੂਰੀ(ਅਚਲ Locale& desired,
                                       ਅਚਲ Locale& supported) U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ਜੇ (desiredClone == ਖਾਲੀ_ਸੂਚਕ || supportedClone == ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ desiredClone;  // in case only one could not be allocated
        ਮਿਟਾਓ supportedClone;
        error_ਕੋਡ_2 = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ *ਇਹ;
    }
    ਮਿਟਾਓ max_ਦੂਰੀ_Desired;
    ਮਿਟਾਓ max_ਦੂਰੀ_Supported;
    max_ਦੂਰੀ_Desired = desiredClone;
    max_ਦੂਰੀ_Supported = supportedClone;
    ਵਾਪਸ *ਇਹ;
}

#ਜੇ 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::ਅੰਦਰੂਨੀ_ਸੈੱਟ_ਹੱਦ_ਦੂਰੀ(int32_t ਹੱਦ_ਦੂਰੀ_2) {
    ਜੇ (U_FAILURE(error_ਕੋਡ_2)) { ਵਾਪਸ *ਇਹ; }
    ਜੇ (ਹੱਦ_ਦੂਰੀ_2 > 100) {
        ਹੱਦ_ਦੂਰੀ_2 = 100;
    }
    ਹੱਦ_ਦੂਰੀ = ਹੱਦ_ਦੂਰੀ_2;
    ਵਾਪਸ *ਇਹ;
}
#ਜੇ_ਅੰਤ

UBool LocaleMatcher::Builder::copy_Error_ਮੰਜ਼ਿਲ(UError_ਕੋਡ &ਬਾਹਰਲਾ_Error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(ਬਾਹਰਲਾ_Error_ਕੋਡ)) { ਵਾਪਸ ਸੱਚ; }
    ਜੇ (U_SUCCESS(error_ਕੋਡ_2)) { ਵਾਪਸ ਝੂਠ; }
    ਬਾਹਰਲਾ_Error_ਕੋਡ = error_ਕੋਡ_2;
    ਵਾਪਸ ਸੱਚ;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_SUCCESS(error_ਕੋਡ) && U_FAILURE(error_ਕੋਡ_2)) {
        error_ਕੋਡ = error_ਕੋਡ_2;
    }
    ਵਾਪਸ LocaleMatcher(*ਇਹ, error_ਕੋਡ);
}

ਨਾਮਾਕਾਸ਼ {

LSR getMaximalLsrOrUnd(ਅਚਲ LikelySubtags &likelySubtags, ਅਚਲ Locale &locale,
                       UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        ਵਾਪਸ UND_LSR;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ likelySubtags.makeMaximizedLsrFrom(locale, ਝੂਠ, error_ਕੋਡ);
    }
}

int32_t hashLSR(ਅਚਲ UHashTok token) {
    ਅਚਲ LSR *lsr = static_cast<ਅਚਲ LSR *>(token.ਸੂਚਕ);
    ਵਾਪਸ lsr->hash_ਕੋਡ;
}

UBool compareLSRs(ਅਚਲ UHashTok t1, ਅਚਲ UHashTok t2) {
    ਅਚਲ LSR *lsr1 = static_cast<ਅਚਲ LSR *>(t1.ਸੂਚਕ);
    ਅਚਲ LSR *lsr2 = static_cast<ਅਚਲ LSR *>(t2.ਸੂਚਕ);
    ਵਾਪਸ *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ਅਚਲ LSR &lsr, int32_t i, int32_t supp_ਲੰਮਾਈ,
                                   UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ supp_ਲੰਮਾਈ; }
    ਜੇ (!uhash_ਸ਼ਾਮਲ_ਹੈ_ਕੁੰਜੀ(supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ, const_cast<LSR *>(&lsr), i, &error_ਕੋਡ);
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            supportedLSRs[supp_ਲੰਮਾਈ] = &lsr;
            supportedIndexes[supp_ਲੰਮਾਈ++] = i;
        }
    }
    ਵਾਪਸ supp_ਲੰਮਾਈ;
}

LocaleMatcher::LocaleMatcher(ਅਚਲ Builder &builder, UError_ਕੋਡ &error_ਕੋਡ) :
        likelySubtags(*LikelySubtags::getSingleton(error_ਕੋਡ)),
        locale_ਦੂਰੀ(*Locale_ਦੂਰੀ::getSingleton(error_ਕੋਡ)),
        ਹੱਦ_ਦੂਰੀ_2(builder.ਹੱਦ_ਦੂਰੀ),
        demotion_ਪ੍ਰਤੀ_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(ਖਾਲੀ_ਸੂਚਕ), lsrs(ਖਾਲੀ_ਸੂਚਕ), supported_Locales_ਲੰਮਾਈ(0),
        supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(ਖਾਲੀ_ਸੂਚਕ),
        supportedLSRs(ਖਾਲੀ_ਸੂਚਕ), supportedIndexes(ਖਾਲੀ_ਸੂਚਕ), supported_LSRs_ਲੰਮਾਈ(0),
        ownedDefaultLocale(ਖਾਲੀ_ਸੂਚਕ), defaultLocale(ਖਾਲੀ_ਸੂਚਕ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    ਅਚਲ Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ਅਚਲ LSR *defLSR = ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ (def != ਖਾਲੀ_ਸੂਚਕ) {
        ownedDefaultLocale = def->clone();
        ਜੇ (ownedDefaultLocale == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_ਲੰਮਾਈ = builder.supportedLocales_ != ਖਾਲੀ_ਸੂਚਕ ?
        builder.supportedLocales_->size() : 0;
    ਜੇ (supported_Locales_ਲੰਮਾਈ > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ਅਚਲ Locale **>(
            uprv_malloc(supported_Locales_ਲੰਮਾਈ * ਆਕਾਰ(ਅਚਲ Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = ਨਵਾਂ LSR[supported_Locales_ਲੰਮਾਈ];
        ਜੇ (supportedLocales == ਖਾਲੀ_ਸੂਚਕ || lsrs == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_ਲੰਮਾਈ * ਆਕਾਰ(ਅਚਲ Locale *));
        ਲਈ (int32_t i = 0; i < supported_Locales_ਲੰਮਾਈ; ++i) {
            ਅਚਲ Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ਜੇ (supportedLocales[i] == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            ਅਚਲ Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_ਕੋਡ);
            lsr.ਸੈੱਟ_Hash_ਕੋਡ();
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ = uhash_open_ਮਾਪ(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_ਲੰਮਾਈ, &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        supportedLSRs = static_cast<ਅਚਲ LSR **>(
            uprv_malloc(supported_Locales_ਲੰਮਾਈ * ਆਕਾਰ(ਅਚਲ LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_ਲੰਮਾਈ * ਆਕਾਰ(int32_t)));
        ਜੇ (supportedLSRs == ਖਾਲੀ_ਸੂਚਕ || supportedIndexes == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        int32_t supp_ਲੰਮਾਈ = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ਢੇਰ_Array<int8_t, 100> ਕ੍ਰਮ(supported_Locales_ਲੰਮਾਈ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        int32_t ਅੰਕ_Paradigms = 0;
        ਲਈ (int32_t i = 0; i < supported_Locales_ਲੰਮਾਈ; ++i) {
            ਅਚਲ Locale &locale = *supportedLocales[i];
            ਅਚਲ LSR &lsr = lsrs[i];
            ਜੇ (defLSR == ਖਾਲੀ_ਸੂਚਕ && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ਕ੍ਰਮ[i] = 1;
                supp_ਲੰਮਾਈ = putIfAbsent(lsr, 0, supp_ਲੰਮਾਈ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ ਜੇ (defLSR != ਖਾਲੀ_ਸੂਚਕ && lsr.is_Equivalent_ਮੰਜ਼ਿਲ(*defLSR)) {
                ਕ੍ਰਮ[i] = 1;
                supp_ਲੰਮਾਈ = putIfAbsent(lsr, i, supp_ਲੰਮਾਈ, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ ਜੇ (locale_ਦੂਰੀ.isParadigmLSR(lsr)) {
                ਕ੍ਰਮ[i] = 2;
                ++ਅੰਕ_Paradigms;
            } ਨਹੀਂਤਾਂ {
                ਕ੍ਰਮ[i] = 3;
            }
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_ਹੱਦਬੰਦੀ = supp_ਲੰਮਾਈ + ਅੰਕ_Paradigms;
        ਲਈ (int32_t i = 0; i < supported_Locales_ਲੰਮਾਈ && supp_ਲੰਮਾਈ < paradigm_ਹੱਦਬੰਦੀ; ++i) {
            ਜੇ (ਕ੍ਰਮ[i] == 2) {
                supp_ਲੰਮਾਈ = putIfAbsent(lsrs[i], i, supp_ਲੰਮਾਈ, error_ਕੋਡ);
            }
        }
        // Add remaining supported locales.
        ਲਈ (int32_t i = 0; i < supported_Locales_ਲੰਮਾਈ; ++i) {
            ਜੇ (ਕ੍ਰਮ[i] == 3) {
                supp_ਲੰਮਾਈ = putIfAbsent(lsrs[i], i, supp_ਲੰਮਾਈ, error_ਕੋਡ);
            }
        }
        supported_LSRs_ਲੰਮਾਈ = supp_ਲੰਮਾਈ;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ਜੇ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_ਪ੍ਰਤੀ_Desired_Locale = locale_ਦੂਰੀ.get_Default_Demotion_ਪ੍ਰਤੀ_Desired_Locale();
    }

    ਜੇ (ਹੱਦ_ਦੂਰੀ_2 >= 0) {
        // already copied
    } ਨਹੀਂਤਾਂ ਜੇ (builder.max_ਦੂਰੀ_Desired != ਖਾਲੀ_ਸੂਚਕ) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_ਦੂਰੀ_Supported, error_ਕੋਡ);
        ਅਚਲ LSR *pSuppLSR = &suppLSR;
        int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ = locale_ਦੂਰੀ.get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_ਦੂਰੀ_Desired, error_ਕੋਡ),
                &pSuppLSR, 1,
                Locale_ਦੂਰੀ::ਖਿਸਕਾਓ_ਦੂਰੀ(100), favorSubtag, direction);
        ਜੇ (U_SUCCESS(error_ਕੋਡ)) {
            // +1 for an exclusive threshold from an inclusive max.
            ਹੱਦ_ਦੂਰੀ_2 = Locale_ਦੂਰੀ::get_ਦੂਰੀ_Floor(ਸੂਚਕਾਂਕ_And_ਦੂਰੀ) + 1;
        } ਨਹੀਂਤਾਂ {
            ਹੱਦ_ਦੂਰੀ_2 = 0;
        }
    } ਨਹੀਂਤਾਂ {
        ਹੱਦ_ਦੂਰੀ_2 = locale_ਦੂਰੀ.get_Default_Script_ਦੂਰੀ();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_ਦੂਰੀ(src.locale_ਦੂਰੀ),
        ਹੱਦ_ਦੂਰੀ_2(src.ਹੱਦ_ਦੂਰੀ_2),
        demotion_ਪ੍ਰਤੀ_Desired_Locale(src.demotion_ਪ੍ਰਤੀ_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_ਲੰਮਾਈ(src.supported_Locales_ਲੰਮਾਈ),
        supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(src.supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_ਲੰਮਾਈ(src.supported_LSRs_ਲੰਮਾਈ),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = ਖਾਲੀ_ਸੂਚਕ;
    src.lsrs = ਖਾਲੀ_ਸੂਚਕ;
    src.supported_Locales_ਲੰਮਾਈ = 0;
    src.supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ = ਖਾਲੀ_ਸੂਚਕ;
    src.supportedLSRs = ਖਾਲੀ_ਸੂਚਕ;
    src.supportedIndexes = ਖਾਲੀ_ਸੂਚਕ;
    src.supported_LSRs_ਲੰਮਾਈ = 0;
    src.ownedDefaultLocale = ਖਾਲੀ_ਸੂਚਕ;
    src.defaultLocale = ਖਾਲੀ_ਸੂਚਕ;
}

LocaleMatcher::~LocaleMatcher() {
    ਲਈ (int32_t i = 0; i < supported_Locales_ਲੰਮਾਈ; ++i) {
        ਮਿਟਾਓ supportedLocales[i];
    }
    uprv_free(supportedLocales);
    ਮਿਟਾਓ[] lsrs;
    uhash_close(supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    ਮਿਟਾਓ ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::ਸੰਚਾਲਕ=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    ਇਹ->~LocaleMatcher();

    ਹੱਦ_ਦੂਰੀ_2 = src.ਹੱਦ_ਦੂਰੀ_2;
    demotion_ਪ੍ਰਤੀ_Desired_Locale = src.demotion_ਪ੍ਰਤੀ_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_ਲੰਮਾਈ = src.supported_Locales_ਲੰਮਾਈ;
    supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ = src.supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_ਲੰਮਾਈ = src.supported_LSRs_ਲੰਮਾਈ;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = ਖਾਲੀ_ਸੂਚਕ;
    src.lsrs = ਖਾਲੀ_ਸੂਚਕ;
    src.supported_Locales_ਲੰਮਾਈ = 0;
    src.supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ = ਖਾਲੀ_ਸੂਚਕ;
    src.supportedLSRs = ਖਾਲੀ_ਸੂਚਕ;
    src.supportedIndexes = ਖਾਲੀ_ਸੂਚਕ;
    src.supported_LSRs_ਲੰਮਾਈ = 0;
    src.ownedDefaultLocale = ਖਾਲੀ_ਸੂਚਕ;
    src.defaultLocale = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ *ਇਹ;
}

ਵਰਗ LocaleLsrIterator {
ਜਨਤਕ:
    LocaleLsrIterator(ਅਚਲ LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_ਮੇਲ_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ਜੇ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            ਮਿਟਾਓ remembered;
        }
    }

    ਤਾਰਕਿਕ has_ਅਗਲਾ() ਅਚਲ {
        ਵਾਪਸ locales.has_ਅਗਲਾ();
    }

    LSR next(UError_ਕੋਡ &error_ਕੋਡ) {
        current = &locales.next();
        ਵਾਪਸ getMaximalLsrOrUnd(likelySubtags, *current, error_ਕੋਡ);
    }

    ਖਾਲੀ rememberCurrent(int32_t desired_ਸੂਚਕਾਂਕ, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਉੱਤਮ_Desired_ਸੂਚਕਾਂਕ = desired_ਸੂਚਕਾਂਕ;
        ਜੇ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ਨਹੀਂਤਾਂ {
            // ULOCMATCH_TEMPORARY_LOCALES
            ਮਿਟਾਓ remembered;
            remembered = ਨਵਾਂ Locale(*current);
            ਜੇ (remembered == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            }
        }
    }

    ਅਚਲ Locale *orphanRemembered() {
        ਅਚਲ Locale *rem = remembered;
        remembered = ਖਾਲੀ_ਸੂਚਕ;
        ਵਾਪਸ rem;
    }

    int32_t get_ਉੱਤਮ_Desired_ਸੂਚਕਾਂਕ() ਅਚਲ {
        ਵਾਪਸ ਉੱਤਮ_Desired_ਸੂਚਕਾਂਕ;
    }

ਨਿਜੀ:
    ਅਚਲ LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_ਮੇਲ_Lifetime lifetime;
    ਅਚਲ Locale *current = ਖਾਲੀ_ਸੂਚਕ, *remembered = ਖਾਲੀ_ਸੂਚਕ;
    int32_t ਉੱਤਮ_Desired_ਸੂਚਕਾਂਕ = -1;
};

ਅਚਲ Locale* LocaleMatcher::get_ਉੱਤਮ_ਮੇਲ(ਅਚਲ Locale& desiredLocale,
                                          UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    std::optional<int32_t> supp_ਸੂਚਕਾਂਕ = get_ਉੱਤਮ_Supp_ਸੂਚਕਾਂਕ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ਕੋਡ),
        ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && supp_ਸੂਚਕਾਂਕ.has_value() ? supportedLocales[*supp_ਸੂਚਕਾਂਕ]
                                                         : defaultLocale;
}

ਅਚਲ Locale* LocaleMatcher::get_ਉੱਤਮ_ਮੇਲ(Locale::Iterator& desiredLocales,
                                          UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    ਜੇ (!desiredLocales.has_ਅਗਲਾ()) {
        ਵਾਪਸ defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_ਸੂਚਕਾਂਕ = get_ਉੱਤਮ_Supp_ਸੂਚਕਾਂਕ(lsrIter.next(error_ਕੋਡ), &lsrIter, error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(error_ਕੋਡ) && supp_ਸੂਚਕਾਂਕ.has_value() ? supportedLocales[*supp_ਸੂਚਕਾਂਕ]
                                                         : defaultLocale;
}

ਅਚਲ Locale*
LocaleMatcher::get_ਉੱਤਮ_ਮੇਲ_For_List_String(StringPiece desiredLocaleList,
                                         UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    Locale_ਤਰਜੀਹ_List list(desiredLocaleList, error_ਕੋਡ);
    Locale_ਤਰਜੀਹ_List::Iterator iter = list.iterator();
    ਵਾਪਸ get_ਉੱਤਮ_ਮੇਲ(iter, error_ਕੋਡ);
}

LocaleMatcher::ਨਤੀਜਾ_2
LocaleMatcher::get_ਉੱਤਮ_ਮੇਲ_ਨਤੀਜਾ(ਅਚਲ Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਨਤੀਜਾ_2(ਖਾਲੀ_ਸੂਚਕ, defaultLocale, -1, -1, ਝੂਠ);
    }
    std::optional<int32_t> supp_ਸੂਚਕਾਂਕ = get_ਉੱਤਮ_Supp_ਸੂਚਕਾਂਕ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ਕੋਡ),
        ਖਾਲੀ_ਸੂਚਕ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ) || !supp_ਸੂਚਕਾਂਕ.has_value()) {
        ਵਾਪਸ ਨਤੀਜਾ_2(ਖਾਲੀ_ਸੂਚਕ, defaultLocale, -1, -1, ਝੂਠ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਤੀਜਾ_2(&desiredLocale, supportedLocales[*supp_ਸੂਚਕਾਂਕ], 0, *supp_ਸੂਚਕਾਂਕ, ਝੂਠ);
    }
}

LocaleMatcher::ਨਤੀਜਾ_2
LocaleMatcher::get_ਉੱਤਮ_ਮੇਲ_ਨਤੀਜਾ(Locale::Iterator& desiredLocales,
                                  UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ U_LIFETIME_BOUND {
    ਜੇ (U_FAILURE(error_ਕੋਡ) || !desiredLocales.has_ਅਗਲਾ()) {
        ਵਾਪਸ ਨਤੀਜਾ_2(ਖਾਲੀ_ਸੂਚਕ, defaultLocale, -1, -1, ਝੂਠ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_ਸੂਚਕਾਂਕ = get_ਉੱਤਮ_Supp_ਸੂਚਕਾਂਕ(lsrIter.next(error_ਕੋਡ), &lsrIter, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ) || !supp_ਸੂਚਕਾਂਕ.has_value()) {
        ਵਾਪਸ ਨਤੀਜਾ_2(ਖਾਲੀ_ਸੂਚਕ, defaultLocale, -1, -1, ਝੂਠ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਤੀਜਾ_2(lsrIter.orphanRemembered(), supportedLocales[*supp_ਸੂਚਕਾਂਕ],
                      lsrIter.get_ਉੱਤਮ_Desired_ਸੂਚਕਾਂਕ(), *supp_ਸੂਚਕਾਂਕ, ਸੱਚ);
    }
}

std::optional<int32_t> LocaleMatcher::get_ਉੱਤਮ_Supp_ਸੂਚਕਾਂਕ(LSR desiredLSR,
                                                       LocaleLsrIterator *ਬਚਿਆ_Iter,
                                                       UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ std::nullopt; }
    int32_t desired_ਸੂਚਕਾਂਕ = 0;
    int32_t ਉੱਤਮ_Supported_Lsr_ਸੂਚਕਾਂਕ = -1;
    ਲਈ (int32_t ਉੱਤਮ_Shifted_ਦੂਰੀ = Locale_ਦੂਰੀ::ਖਿਸਕਾਓ_ਦੂਰੀ(ਹੱਦ_ਦੂਰੀ_2);;) {
        // Quick check for exact maximized LSR.
        ਜੇ (supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ != ਖਾਲੀ_ਸੂਚਕ) {
            desiredLSR.ਸੈੱਟ_Hash_ਕੋਡ();
            UBool ਮਿਲਿਆ = ਝੂਠ;
            int32_t supp_ਸੂਚਕਾਂਕ = uhash_geti_And_ਮਿਲਿਆ(supported_Lsr_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ, &desiredLSR, &ਮਿਲਿਆ);
            ਜੇ (ਮਿਲਿਆ) {
                ਜੇ (ਬਚਿਆ_Iter != ਖਾਲੀ_ਸੂਚਕ) {
                    ਬਚਿਆ_Iter->rememberCurrent(desired_ਸੂਚਕਾਂਕ, error_ਕੋਡ);
                }
                ਵਾਪਸ supp_ਸੂਚਕਾਂਕ;
            }
        }
        int32_t ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ = locale_ਦੂਰੀ.get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
                desiredLSR, supportedLSRs, supported_LSRs_ਲੰਮਾਈ,
                ਉੱਤਮ_Shifted_ਦੂਰੀ, favorSubtag, direction);
        ਜੇ (ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ >= 0) {
            ਉੱਤਮ_Shifted_ਦੂਰੀ = Locale_ਦੂਰੀ::get_Shifted_ਦੂਰੀ(ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ);
            ਜੇ (ਬਚਿਆ_Iter != ਖਾਲੀ_ਸੂਚਕ) {
                ਬਚਿਆ_Iter->rememberCurrent(desired_ਸੂਚਕਾਂਕ, error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ std::nullopt; }
            }
            ਉੱਤਮ_Supported_Lsr_ਸੂਚਕਾਂਕ = Locale_ਦੂਰੀ::get_ਸੂਚਕਾਂਕ(ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ);
        }
        ਜੇ ((ਉੱਤਮ_Shifted_ਦੂਰੀ -= Locale_ਦੂਰੀ::ਖਿਸਕਾਓ_ਦੂਰੀ(demotion_ਪ੍ਰਤੀ_Desired_Locale)) <= 0) {
            ਰੋਕੋ;
        }
        ਜੇ (ਬਚਿਆ_Iter == ਖਾਲੀ_ਸੂਚਕ || !ਬਚਿਆ_Iter->has_ਅਗਲਾ()) {
            ਰੋਕੋ;
        }
        desiredLSR = ਬਚਿਆ_Iter->next(error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ std::nullopt; }
        ++desired_ਸੂਚਕਾਂਕ;
    }
    ਜੇ (ਉੱਤਮ_Supported_Lsr_ਸੂਚਕਾਂਕ < 0) {
        // no good match
        ਵਾਪਸ std::nullopt;
    }
    ਵਾਪਸ supportedIndexes[ਉੱਤਮ_Supported_Lsr_ਸੂਚਕਾਂਕ];
}

UBool LocaleMatcher::is_ਮੇਲ(ਅਚਲ Locale &desired, ਅਚਲ Locale &supported,
                             UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    ਅਚਲ LSR *pSuppLSR = &suppLSR;
    int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ = locale_ਦੂਰੀ.get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ਕੋਡ),
            &pSuppLSR, 1,
            Locale_ਦੂਰੀ::ਖਿਸਕਾਓ_ਦੂਰੀ(ਹੱਦ_ਦੂਰੀ_2), favorSubtag, direction);
    ਵਾਪਸ ਸੂਚਕਾਂਕ_And_ਦੂਰੀ >= 0;
}

ਦੁੱਗਣਾ LocaleMatcher::ਅੰਦਰੂਨੀ_ਮੇਲ(ਅਚਲ Locale &desired, ਅਚਲ Locale &supported, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0.; }
    ਅਚਲ LSR *pSuppLSR = &suppLSR;
    int32_t ਸੂਚਕਾਂਕ_And_ਦੂਰੀ = locale_ਦੂਰੀ.get_ਉੱਤਮ_ਸੂਚਕਾਂਕ_And_ਦੂਰੀ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ਕੋਡ),
            &pSuppLSR, 1,
            Locale_ਦੂਰੀ::ਖਿਸਕਾਓ_ਦੂਰੀ(ਹੱਦ_ਦੂਰੀ_2), favorSubtag, direction);
    ਦੁੱਗਣਾ ਦੂਰੀ = Locale_ਦੂਰੀ::get_ਦੂਰੀ_Double(ਸੂਚਕਾਂਕ_And_ਦੂਰੀ);
    ਵਾਪਸ (100.0 - ਦੂਰੀ) / 100.0;
}

U_NAMESPACE_ਸਿਰਾ

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

ਨਾਮਾਕਾਸ਼ {

ਵਰਗ Locale_From_ਟੈਗ {
ਜਨਤਕ:
    Locale_From_ਟੈਗ() : locale(Locale::get_ਜੜ੍ਹ()) {}
    ਅਚਲ Locale& ਸੰਚਾਲਕ()(ਅਚਲ ਅੱਖਰ* ਟੈਗ) U_LIFETIME_BOUND { ਵਾਪਸ locale = Locale(ਟੈਗ); }

ਨਿਜੀ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       ਅੱਖਰ *dest, int32_t capacity, UAccept_ਨਤੀਜਾ *accept_ਨਤੀਜਾ,
                       UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }
    LocaleMatcher::Builder builder;
    ਅਚਲ ਅੱਖਰ *locString;
    ਜਦੋਂ ((locString = uenum_ਅਗਲਾ(&supportedLocales, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ)) != ਖਾਲੀ_ਸੂਚਕ) {
        Locale loc(locString);
        ਜੇ (loc.isBogus()) {
            error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
            ਵਾਪਸ 0;
        }
        builder.ਜੋੜੋ_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_ਕੋਡ);
    LocaleMatcher::ਨਤੀਜਾ_2 ਨਤੀਜਾ = matcher.get_ਉੱਤਮ_ਮੇਲ_ਨਤੀਜਾ(desiredLocales, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ 0; }
    ਜੇ (ਨਤੀਜਾ.get_Desired_ਸੂਚਕਾਂਕ() >= 0) {
        ਜੇ (accept_ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
            *accept_ਨਤੀਜਾ = *ਨਤੀਜਾ.getDesiredLocale() == *ਨਤੀਜਾ.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ਅਚਲ ਅੱਖਰ *ਉੱਤਮ_Str = ਨਤੀਜਾ.getSupportedLocale()->getName();
        int32_t ਉੱਤਮ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(ਉੱਤਮ_Str));
        ਜੇ (ਉੱਤਮ_ਲੰਮਾਈ <= capacity) {
            uprv_memcpy(dest, ਉੱਤਮ_Str, ਉੱਤਮ_ਲੰਮਾਈ);
        }
        ਵਾਪਸ u_terminateChars(dest, capacity, ਉੱਤਮ_ਲੰਮਾਈ, &error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਜੇ (accept_ਨਤੀਜਾ != ਖਾਲੀ_ਸੂਚਕ) {
            *accept_ਨਤੀਜਾ = ULOC_ACCEPT_FAILED;
        }
        ਵਾਪਸ u_terminateChars(dest, capacity, 0, &error_ਕੋਡ);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(ਅੱਖਰ *ਨਤੀਜਾ, int32_t ਨਤੀਜਾ_ਉਪਲਬਧ,
                    UAccept_ਨਤੀਜਾ *ਬਾਹਰਲਾ_ਨਤੀਜਾ,
                    ਅਚਲ ਅੱਖਰ **acceptList, int32_t accept_List_ਗਿਣਤੀ,
                    UEnumeration *ਉਪਲਬਧ_Locales,
                    UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
    ਜੇ ((ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ ? ਨਤੀਜਾ_ਉਪਲਬਧ != 0 : ਨਤੀਜਾ_ਉਪਲਬਧ < 0) ||
            (acceptList == ਖਾਲੀ_ਸੂਚਕ ? accept_List_ਗਿਣਤੀ != 0 : accept_List_ਗਿਣਤੀ < 0) ||
            ਉਪਲਬਧ_Locales == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    Locale_From_ਟੈਗ converter;
    Locale::ConvertingIterator<ਅਚਲ ਅੱਖਰ **, Locale_From_ਟੈਗ> desiredLocales(
        acceptList, acceptList + accept_List_ਗਿਣਤੀ, converter);
    ਵਾਪਸ acceptLanguage(*ਉਪਲਬਧ_Locales, desiredLocales,
                          ਨਤੀਜਾ, ਨਤੀਜਾ_ਉਪਲਬਧ, ਬਾਹਰਲਾ_ਨਤੀਜਾ, *ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(ਅੱਖਰ *ਨਤੀਜਾ, int32_t ਨਤੀਜਾ_ਉਪਲਬਧ,
                            UAccept_ਨਤੀਜਾ *ਬਾਹਰਲਾ_ਨਤੀਜਾ,
                            ਅਚਲ ਅੱਖਰ *httpAcceptLanguage,
                            UEnumeration *ਉਪਲਬਧ_Locales,
                            UError_ਕੋਡ *ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)) { ਵਾਪਸ 0; }
    ਜੇ ((ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ ? ਨਤੀਜਾ_ਉਪਲਬਧ != 0 : ਨਤੀਜਾ_ਉਪਲਬਧ < 0) ||
            httpAcceptLanguage == ਖਾਲੀ_ਸੂਚਕ || ਉਪਲਬਧ_Locales == ਖਾਲੀ_ਸੂਚਕ) {
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }
    Locale_ਤਰਜੀਹ_List list(httpAcceptLanguage, *ਸਥਿਤੀ);
    Locale_ਤਰਜੀਹ_List::Iterator desiredLocales = list.iterator();
    ਵਾਪਸ acceptLanguage(*ਉਪਲਬਧ_Locales, desiredLocales,
                          ਨਤੀਜਾ, ਨਤੀਜਾ_ਉਪਲਬਧ, ਬਾਹਰਲਾ_ਨਤੀਜਾ, *ਸਥਿਤੀ);
}
