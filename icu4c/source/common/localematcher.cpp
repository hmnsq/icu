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

#وصف_ڪريو UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
تعداد ULoc_ميلاپ_Lifetime {
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
#جيڪڏهن_وصف_نه_ٿيل U_اندريون_DOXYGEN
تعريفنوع تعداد ULoc_ميلاپ_Lifetime ULoc_ميلاپ_Lifetime;
#جيڪڏهن_پورو

U_NAMESPACE_BEGIN

LocaleMatcher::نتيجو_2::نتيجو_2(LocaleMatcher::نتيجو_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_اشاريو(src.desired_اشاريو),
        supported_اشاريو(src.supported_اشاريو),
        desiredIsOwned(src.desiredIsOwned) {
    إذا (desiredIsOwned) {
        src.desiredLocale = خالي_اشارو;
        src.desired_اشاريو = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::نتيجو_2::~نتيجو_2() {
    إذا (desiredIsOwned) {
        ميٽيو desiredLocale;
    }
}

LocaleMatcher::نتيجو_2&
LocaleMatcher::نتيجو_2::عامل=(LocaleMatcher::نتيجو_2&& src) noexcept U_LIFETIME_BOUND {
    هذا->~نتيجو_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_اشاريو = src.desired_اشاريو;
    supported_اشاريو = src.supported_اشاريو;
    desiredIsOwned = src.desiredIsOwned;

    إذا (desiredIsOwned) {
        src.desiredLocale = خالي_اشارو;
        src.desired_اشاريو = -1;
        src.desiredIsOwned = خطأ;
    }
    أرجع *هذا;
}

Locale LocaleMatcher::نتيجو_2::makeResolvedLocale(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ) || supportedLocale == خالي_اشارو) {
        أرجع Locale::get_پاڙ();
    }
    ثابت Locale *بهترين_Desired = getDesiredLocale();
    إذا (بهترين_Desired == خالي_اشارو || *supportedLocale == *بهترين_Desired) {
        أرجع *supportedLocale;
    }
    LocaleBuilder b;
    b.سيٽ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = بهترين_Desired->getCountry();
    إذا (*region != 0) {
        b.سيٽ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = بهترين_Desired->getVariant();
    إذا (*variants != 0) {
        b.سيٽ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*بهترين_Desired, error_ڪوڊ);
    أرجع b.build(error_ڪوڊ);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_ڪوڊ_2(src.error_ڪوڊ_2),
        supportedLocales_(src.supportedLocales_),
        حد_مفاصلو(src.حد_مفاصلو),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = خالي_اشارو;
    src.defaultLocale_ = خالي_اشارو;
}

LocaleMatcher::Builder::~Builder() {
    ميٽيو supportedLocales_;
    ميٽيو defaultLocale_;
    ميٽيو max_مفاصلو_Desired;
    ميٽيو max_مفاصلو_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::عامل=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    هذا->~Builder();

    error_ڪوڊ_2 = src.error_ڪوڊ_2;
    supportedLocales_ = src.supportedLocales_;
    حد_مفاصلو = src.حد_مفاصلو;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = خالي_اشارو;
    src.defaultLocale_ = خالي_اشارو;
    أرجع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    إذا (supportedLocales_ != خالي_اشارو) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensure_Supported_Locale_ويڪٽر() {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع خطأ; }
    إذا (supportedLocales_ != خالي_اشارو) { أرجع سچ; }
    Local_اشارو<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, خالي_اشارو, error_ڪوڊ_2), error_ڪوڊ_2);
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    أرجع سچ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ترجيح_List list(locales, error_ڪوڊ_2);
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    clearSupportedLocales();
    إذا (!ensure_Supported_Locale_ويڪٽر()) { أرجع *هذا; }
    int32_t length = list.get_ڊيگهائي_Including_Removed();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        إذا (locale == خالي_اشارو) { استمر; }
        supportedLocales_->adoptElement(locale, error_ڪوڊ_2);
        إذا (U_FAILURE(error_ڪوڊ_2)) {
            اكسر;
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_ويڪٽر()) {
        clearSupportedLocales();
        طالما (locales.has_ايندڙ() && U_SUCCESS(error_ڪوڊ_2)) {
            ثابت Locale &locale = locales.next();
            Local_اشارو<Locale> clone (locale.clone(), error_ڪوڊ_2);
            supportedLocales_->adoptElement(clone.orphan(), error_ڪوڊ_2);
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::جوڙيو_Supported_Locale(ثابت Locale& locale) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_ويڪٽر()) {
        Local_اشارو<Locale> clone(locale.clone(), error_ڪوڊ_2);
        supportedLocales_->adoptElement(clone.orphan(), error_ڪوڊ_2);
    }
    أرجع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::سيٽ_No_Default_Locale() U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    ميٽيو defaultLocale_;
    defaultLocale_ = خالي_اشارو;
    withDefault_ = خطأ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Default_Locale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    Locale *clone = خالي_اشارو;
    إذا (defaultLocale != خالي_اشارو) {
        clone = defaultLocale->clone();
        إذا (clone == خالي_اشارو) {
            error_ڪوڊ_2 = U_يادگيري_ورهاست_ERROR;
            أرجع *هذا;
        }
    }
    ميٽيو defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = سچ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Favor_Subtag(ULoc_ميلاپ_Favor_Subtag subtag) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    favor_ = subtag;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Demotion_في_Desired_Locale(ULoc_ميلاپ_Demotion demotion) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    demotion_ = demotion;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سيٽ_Max_مفاصلو(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    إذا (desiredClone == خالي_اشارو || supportedClone == خالي_اشارو) {
        ميٽيو desiredClone;  // in case only one could not be allocated
        ميٽيو supportedClone;
        error_ڪوڊ_2 = U_يادگيري_ورهاست_ERROR;
        أرجع *هذا;
    }
    ميٽيو max_مفاصلو_Desired;
    ميٽيو max_مفاصلو_Supported;
    max_مفاصلو_Desired = desiredClone;
    max_مفاصلو_Supported = supportedClone;
    أرجع *هذا;
}

#إذا 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::اندروني_سيٽ_حد_مفاصلو(int32_t حد_مفاصلو_2) {
    إذا (U_FAILURE(error_ڪوڊ_2)) { أرجع *هذا; }
    إذا (حد_مفاصلو_2 > 100) {
        حد_مفاصلو_2 = 100;
    }
    حد_مفاصلو = حد_مفاصلو_2;
    أرجع *هذا;
}
#جيڪڏهن_پورو

UBool LocaleMatcher::Builder::copy_Error_منزل(UError_ڪوڊ &ٻاهريون_Error_ڪوڊ) ثابت {
    إذا (U_FAILURE(ٻاهريون_Error_ڪوڊ)) { أرجع سچ; }
    إذا (U_SUCCESS(error_ڪوڊ_2)) { أرجع خطأ; }
    ٻاهريون_Error_ڪوڊ = error_ڪوڊ_2;
    أرجع سچ;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_SUCCESS(error_ڪوڊ) && U_FAILURE(error_ڪوڊ_2)) {
        error_ڪوڊ = error_ڪوڊ_2;
    }
    أرجع LocaleMatcher(*هذا, error_ڪوڊ);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        أرجع UND_LSR;
    } وإلا {
        أرجع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, error_ڪوڊ);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.اشارو);
    أرجع lsr->hash_ڪوڊ;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.اشارو);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.اشارو);
    أرجع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t supp_ڊيگهائي,
                                   UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع supp_ڊيگهائي; }
    إذا (!uhash_شامل_آهي_ڪنجي(supported_Lsr_منزل_اشاريو, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_منزل_اشاريو, const_cast<LSR *>(&lsr), i, &error_ڪوڊ);
        إذا (U_SUCCESS(error_ڪوڊ)) {
            supportedLSRs[supp_ڊيگهائي] = &lsr;
            supportedIndexes[supp_ڊيگهائي++] = i;
        }
    }
    أرجع supp_ڊيگهائي;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UError_ڪوڊ &error_ڪوڊ) :
        likelySubtags(*LikelySubtags::getSingleton(error_ڪوڊ)),
        locale_مفاصلو(*Locale_مفاصلو::getSingleton(error_ڪوڊ)),
        حد_مفاصلو_2(builder.حد_مفاصلو),
        demotion_في_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(خالي_اشارو), lsrs(خالي_اشارو), supported_Locales_ڊيگهائي(0),
        supported_Lsr_منزل_اشاريو(خالي_اشارو),
        supportedLSRs(خالي_اشارو), supportedIndexes(خالي_اشارو), supported_LSRs_ڊيگهائي(0),
        ownedDefaultLocale(خالي_اشارو), defaultLocale(خالي_اشارو) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = خالي_اشارو;
    إذا (def != خالي_اشارو) {
        ownedDefaultLocale = def->clone();
        إذا (ownedDefaultLocale == خالي_اشارو) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_ڊيگهائي = builder.supportedLocales_ != خالي_اشارو ?
        builder.supportedLocales_->size() : 0;
    إذا (supported_Locales_ڊيگهائي > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supported_Locales_ڊيگهائي * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supported_Locales_ڊيگهائي];
        إذا (supportedLocales == خالي_اشارو || lsrs == خالي_اشارو) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_ڊيگهائي * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supported_Locales_ڊيگهائي; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            إذا (supportedLocales[i] == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_ڪوڊ);
            lsr.سيٽ_Hash_ڪوڊ();
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_منزل_اشاريو = uhash_open_سائيز(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_ڊيگهائي, &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supported_Locales_ڊيگهائي * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_ڊيگهائي * حجم(int32_t)));
        إذا (supportedLSRs == خالي_اشارو || supportedIndexes == خالي_اشارو) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        int32_t supp_ڊيگهائي = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ڍير_Array<int8_t, 100> ترتيب(supported_Locales_ڊيگهائي, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        int32_t انگ_Paradigms = 0;
        لكل (int32_t i = 0; i < supported_Locales_ڊيگهائي; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            إذا (defLSR == خالي_اشارو && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ترتيب[i] = 1;
                supp_ڊيگهائي = putIfAbsent(lsr, 0, supp_ڊيگهائي, error_ڪوڊ);
            } وإلا إذا (defLSR != خالي_اشارو && lsr.is_Equivalent_منزل(*defLSR)) {
                ترتيب[i] = 1;
                supp_ڊيگهائي = putIfAbsent(lsr, i, supp_ڊيگهائي, error_ڪوڊ);
            } وإلا إذا (locale_مفاصلو.isParadigmLSR(lsr)) {
                ترتيب[i] = 2;
                ++انگ_Paradigms;
            } وإلا {
                ترتيب[i] = 3;
            }
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_حد_بندي = supp_ڊيگهائي + انگ_Paradigms;
        لكل (int32_t i = 0; i < supported_Locales_ڊيگهائي && supp_ڊيگهائي < paradigm_حد_بندي; ++i) {
            إذا (ترتيب[i] == 2) {
                supp_ڊيگهائي = putIfAbsent(lsrs[i], i, supp_ڊيگهائي, error_ڪوڊ);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supported_Locales_ڊيگهائي; ++i) {
            إذا (ترتيب[i] == 3) {
                supp_ڊيگهائي = putIfAbsent(lsrs[i], i, supp_ڊيگهائي, error_ڪوڊ);
            }
        }
        supported_LSRs_ڊيگهائي = supp_ڊيگهائي;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    إذا (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_في_Desired_Locale = locale_مفاصلو.get_Default_Demotion_في_Desired_Locale();
    }

    إذا (حد_مفاصلو_2 >= 0) {
        // already copied
    } وإلا إذا (builder.max_مفاصلو_Desired != خالي_اشارو) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_مفاصلو_Supported, error_ڪوڊ);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t اشاريو_And_مفاصلو = locale_مفاصلو.get_بهترين_اشاريو_And_مفاصلو(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_مفاصلو_Desired, error_ڪوڊ),
                &pSuppLSR, 1,
                Locale_مفاصلو::سرڪايو_مفاصلو(100), favorSubtag, direction);
        إذا (U_SUCCESS(error_ڪوڊ)) {
            // +1 for an exclusive threshold from an inclusive max.
            حد_مفاصلو_2 = Locale_مفاصلو::get_مفاصلو_Floor(اشاريو_And_مفاصلو) + 1;
        } وإلا {
            حد_مفاصلو_2 = 0;
        }
    } وإلا {
        حد_مفاصلو_2 = locale_مفاصلو.get_Default_Script_مفاصلو();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_مفاصلو(src.locale_مفاصلو),
        حد_مفاصلو_2(src.حد_مفاصلو_2),
        demotion_في_Desired_Locale(src.demotion_في_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_ڊيگهائي(src.supported_Locales_ڊيگهائي),
        supported_Lsr_منزل_اشاريو(src.supported_Lsr_منزل_اشاريو),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_ڊيگهائي(src.supported_LSRs_ڊيگهائي),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = خالي_اشارو;
    src.lsrs = خالي_اشارو;
    src.supported_Locales_ڊيگهائي = 0;
    src.supported_Lsr_منزل_اشاريو = خالي_اشارو;
    src.supportedLSRs = خالي_اشارو;
    src.supportedIndexes = خالي_اشارو;
    src.supported_LSRs_ڊيگهائي = 0;
    src.ownedDefaultLocale = خالي_اشارو;
    src.defaultLocale = خالي_اشارو;
}

LocaleMatcher::~LocaleMatcher() {
    لكل (int32_t i = 0; i < supported_Locales_ڊيگهائي; ++i) {
        ميٽيو supportedLocales[i];
    }
    uprv_free(supportedLocales);
    ميٽيو[] lsrs;
    uhash_close(supported_Lsr_منزل_اشاريو);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    ميٽيو ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::عامل=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    هذا->~LocaleMatcher();

    حد_مفاصلو_2 = src.حد_مفاصلو_2;
    demotion_في_Desired_Locale = src.demotion_في_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_ڊيگهائي = src.supported_Locales_ڊيگهائي;
    supported_Lsr_منزل_اشاريو = src.supported_Lsr_منزل_اشاريو;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_ڊيگهائي = src.supported_LSRs_ڊيگهائي;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = خالي_اشارو;
    src.lsrs = خالي_اشارو;
    src.supported_Locales_ڊيگهائي = 0;
    src.supported_Lsr_منزل_اشاريو = خالي_اشارو;
    src.supportedLSRs = خالي_اشارو;
    src.supportedIndexes = خالي_اشارو;
    src.supported_LSRs_ڊيگهائي = 0;
    src.ownedDefaultLocale = خالي_اشارو;
    src.defaultLocale = خالي_اشارو;
    أرجع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_ميلاپ_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        إذا (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            ميٽيو remembered;
        }
    }

    منطقي has_ايندڙ() ثابت {
        أرجع locales.has_ايندڙ();
    }

    LSR next(UError_ڪوڊ &error_ڪوڊ) {
        current = &locales.next();
        أرجع getMaximalLsrOrUnd(likelySubtags, *current, error_ڪوڊ);
    }

    فراغ rememberCurrent(int32_t desired_اشاريو, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        بهترين_Desired_اشاريو = desired_اشاريو;
        إذا (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            ميٽيو remembered;
            remembered = جديد Locale(*current);
            إذا (remembered == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = خالي_اشارو;
        أرجع rem;
    }

    int32_t get_بهترين_Desired_اشاريو() ثابت {
        أرجع بهترين_Desired_اشاريو;
    }

خاص:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_ميلاپ_Lifetime lifetime;
    ثابت Locale *current = خالي_اشارو, *remembered = خالي_اشارو;
    int32_t بهترين_Desired_اشاريو = -1;
};

ثابت Locale* LocaleMatcher::get_بهترين_ميلاپ(ثابت Locale& desiredLocale,
                                          UError_ڪوڊ& error_ڪوڊ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    std::optional<int32_t> supp_اشاريو = get_بهترين_Supp_اشاريو(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ڪوڊ),
        خالي_اشارو, error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && supp_اشاريو.has_value() ? supportedLocales[*supp_اشاريو]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::get_بهترين_ميلاپ(Locale::Iterator& desiredLocales,
                                          UError_ڪوڊ& error_ڪوڊ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    إذا (!desiredLocales.has_ايندڙ()) {
        أرجع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاريو = get_بهترين_Supp_اشاريو(lsrIter.next(error_ڪوڊ), &lsrIter, error_ڪوڊ);
    أرجع U_SUCCESS(error_ڪوڊ) && supp_اشاريو.has_value() ? supportedLocales[*supp_اشاريو]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::get_بهترين_ميلاپ_For_List_String(StringPiece desiredLocaleList,
                                         UError_ڪوڊ& error_ڪوڊ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    Locale_ترجيح_List list(desiredLocaleList, error_ڪوڊ);
    Locale_ترجيح_List::Iterator iter = list.iterator();
    أرجع get_بهترين_ميلاپ(iter, error_ڪوڊ);
}

LocaleMatcher::نتيجو_2
LocaleMatcher::get_بهترين_ميلاپ_نتيجو(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_ڪوڊ& error_ڪوڊ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ)) {
        أرجع نتيجو_2(خالي_اشارو, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> supp_اشاريو = get_بهترين_Supp_اشاريو(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_ڪوڊ),
        خالي_اشارو, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ) || !supp_اشاريو.has_value()) {
        أرجع نتيجو_2(خالي_اشارو, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتيجو_2(&desiredLocale, supportedLocales[*supp_اشاريو], 0, *supp_اشاريو, خطأ);
    }
}

LocaleMatcher::نتيجو_2
LocaleMatcher::get_بهترين_ميلاپ_نتيجو(Locale::Iterator& desiredLocales,
                                  UError_ڪوڊ& error_ڪوڊ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_ڪوڊ) || !desiredLocales.has_ايندڙ()) {
        أرجع نتيجو_2(خالي_اشارو, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاريو = get_بهترين_Supp_اشاريو(lsrIter.next(error_ڪوڊ), &lsrIter, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ) || !supp_اشاريو.has_value()) {
        أرجع نتيجو_2(خالي_اشارو, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتيجو_2(lsrIter.orphanRemembered(), supportedLocales[*supp_اشاريو],
                      lsrIter.get_بهترين_Desired_اشاريو(), *supp_اشاريو, سچ);
    }
}

std::optional<int32_t> LocaleMatcher::get_بهترين_Supp_اشاريو(LSR desiredLSR,
                                                       LocaleLsrIterator *بچيل_Iter,
                                                       UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع std::nullopt; }
    int32_t desired_اشاريو = 0;
    int32_t بهترين_Supported_Lsr_اشاريو = -1;
    لكل (int32_t بهترين_Shifted_مفاصلو = Locale_مفاصلو::سرڪايو_مفاصلو(حد_مفاصلو_2);;) {
        // Quick check for exact maximized LSR.
        إذا (supported_Lsr_منزل_اشاريو != خالي_اشارو) {
            desiredLSR.سيٽ_Hash_ڪوڊ();
            UBool مليو = خطأ;
            int32_t supp_اشاريو = uhash_geti_And_مليو(supported_Lsr_منزل_اشاريو, &desiredLSR, &مليو);
            إذا (مليو) {
                إذا (بچيل_Iter != خالي_اشارو) {
                    بچيل_Iter->rememberCurrent(desired_اشاريو, error_ڪوڊ);
                }
                أرجع supp_اشاريو;
            }
        }
        int32_t بهترين_اشاريو_And_مفاصلو = locale_مفاصلو.get_بهترين_اشاريو_And_مفاصلو(
                desiredLSR, supportedLSRs, supported_LSRs_ڊيگهائي,
                بهترين_Shifted_مفاصلو, favorSubtag, direction);
        إذا (بهترين_اشاريو_And_مفاصلو >= 0) {
            بهترين_Shifted_مفاصلو = Locale_مفاصلو::get_Shifted_مفاصلو(بهترين_اشاريو_And_مفاصلو);
            إذا (بچيل_Iter != خالي_اشارو) {
                بچيل_Iter->rememberCurrent(desired_اشاريو, error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) { أرجع std::nullopt; }
            }
            بهترين_Supported_Lsr_اشاريو = Locale_مفاصلو::get_اشاريو(بهترين_اشاريو_And_مفاصلو);
        }
        إذا ((بهترين_Shifted_مفاصلو -= Locale_مفاصلو::سرڪايو_مفاصلو(demotion_في_Desired_Locale)) <= 0) {
            اكسر;
        }
        إذا (بچيل_Iter == خالي_اشارو || !بچيل_Iter->has_ايندڙ()) {
            اكسر;
        }
        desiredLSR = بچيل_Iter->next(error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع std::nullopt; }
        ++desired_اشاريو;
    }
    إذا (بهترين_Supported_Lsr_اشاريو < 0) {
        // no good match
        أرجع std::nullopt;
    }
    أرجع supportedIndexes[بهترين_Supported_Lsr_اشاريو];
}

UBool LocaleMatcher::is_ميلاپ(ثابت Locale &desired, ثابت Locale &supported,
                             UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاريو_And_مفاصلو = locale_مفاصلو.get_بهترين_اشاريو_And_مفاصلو(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ڪوڊ),
            &pSuppLSR, 1,
            Locale_مفاصلو::سرڪايو_مفاصلو(حد_مفاصلو_2), favorSubtag, direction);
    أرجع اشاريو_And_مفاصلو >= 0;
}

مزدوج LocaleMatcher::اندروني_ميلاپ(ثابت Locale &desired, ثابت Locale &supported, UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاريو_And_مفاصلو = locale_مفاصلو.get_بهترين_اشاريو_And_مفاصلو(
            getMaximalLsrOrUnd(likelySubtags, desired, error_ڪوڊ),
            &pSuppLSR, 1,
            Locale_مفاصلو::سرڪايو_مفاصلو(حد_مفاصلو_2), favorSubtag, direction);
    مزدوج مفاصلو = Locale_مفاصلو::get_مفاصلو_Double(اشاريو_And_مفاصلو);
    أرجع (100.0 - مفاصلو) / 100.0;
}

U_NAMESPACE_پڇاڙي

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف Locale_From_ٽيگ {
عام:
    Locale_From_ٽيگ() : locale(Locale::get_پاڙ()) {}
    ثابت Locale& عامل()(ثابت حرف* ٽيگ) U_LIFETIME_BOUND { أرجع locale = Locale(ٽيگ); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAccept_نتيجو *accept_نتيجو,
                       UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    طالما ((locString = uenum_ايندڙ(&supportedLocales, خالي_اشارو, &error_ڪوڊ)) != خالي_اشارو) {
        Locale loc(locString);
        إذا (loc.isBogus()) {
            error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
            أرجع 0;
        }
        builder.جوڙيو_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_ڪوڊ);
    LocaleMatcher::نتيجو_2 نتيجو = matcher.get_بهترين_ميلاپ_نتيجو(desiredLocales, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع 0; }
    إذا (نتيجو.get_Desired_اشاريو() >= 0) {
        إذا (accept_نتيجو != خالي_اشارو) {
            *accept_نتيجو = *نتيجو.getDesiredLocale() == *نتيجو.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *بهترين_Str = نتيجو.getSupportedLocale()->getName();
        int32_t بهترين_ڊيگهائي = static_cast<int32_t>(uprv_strlen(بهترين_Str));
        إذا (بهترين_ڊيگهائي <= capacity) {
            uprv_memcpy(dest, بهترين_Str, بهترين_ڊيگهائي);
        }
        أرجع u_terminateChars(dest, capacity, بهترين_ڊيگهائي, &error_ڪوڊ);
    } وإلا {
        إذا (accept_نتيجو != خالي_اشارو) {
            *accept_نتيجو = ULOC_ACCEPT_FAILED;
        }
        أرجع u_terminateChars(dest, capacity, 0, &error_ڪوڊ);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *نتيجو, int32_t نتيجو_موجود,
                    UAccept_نتيجو *ٻاهريون_نتيجو,
                    ثابت حرف **acceptList, int32_t accept_List_ڳڻپ,
                    UEnumeration *موجود_Locales,
                    UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((نتيجو == خالي_اشارو ? نتيجو_موجود != 0 : نتيجو_موجود < 0) ||
            (acceptList == خالي_اشارو ? accept_List_ڳڻپ != 0 : accept_List_ڳڻپ < 0) ||
            موجود_Locales == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    Locale_From_ٽيگ converter;
    Locale::ConvertingIterator<ثابت حرف **, Locale_From_ٽيگ> desiredLocales(
        acceptList, acceptList + accept_List_ڳڻپ, converter);
    أرجع acceptLanguage(*موجود_Locales, desiredLocales,
                          نتيجو, نتيجو_موجود, ٻاهريون_نتيجو, *حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *نتيجو, int32_t نتيجو_موجود,
                            UAccept_نتيجو *ٻاهريون_نتيجو,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *موجود_Locales,
                            UError_ڪوڊ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((نتيجو == خالي_اشارو ? نتيجو_موجود != 0 : نتيجو_موجود < 0) ||
            httpAcceptLanguage == خالي_اشارو || موجود_Locales == خالي_اشارو) {
        *حالت = U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }
    Locale_ترجيح_List list(httpAcceptLanguage, *حالت);
    Locale_ترجيح_List::Iterator desiredLocales = list.iterator();
    أرجع acceptLanguage(*موجود_Locales, desiredLocales,
                          نتيجو, نتيجو_موجود, ٻاهريون_نتيجو, *حالت);
}
