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

#تعریف_کن UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
تعداد ULoc_تطابق_Lifetime {
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
#اگر_تعریف_نشده U_درون_DOXYGEN
تعريفنوع تعداد ULoc_تطابق_Lifetime ULoc_تطابق_Lifetime;
#پایان_اگر

U_NAMESPACE_BEGIN

LocaleMatcher::نتیجه_2::نتیجه_2(LocaleMatcher::نتیجه_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_شاخص(src.desired_شاخص),
        supported_شاخص(src.supported_شاخص),
        desiredIsOwned(src.desiredIsOwned) {
    إذا (desiredIsOwned) {
        src.desiredLocale = اشاره_تهی;
        src.desired_شاخص = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::نتیجه_2::~نتیجه_2() {
    إذا (desiredIsOwned) {
        حذف desiredLocale;
    }
}

LocaleMatcher::نتیجه_2&
LocaleMatcher::نتیجه_2::عملگر=(LocaleMatcher::نتیجه_2&& src) noexcept U_LIFETIME_BOUND {
    هذا->~نتیجه_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_شاخص = src.desired_شاخص;
    supported_شاخص = src.supported_شاخص;
    desiredIsOwned = src.desiredIsOwned;

    إذا (desiredIsOwned) {
        src.desiredLocale = اشاره_تهی;
        src.desired_شاخص = -1;
        src.desiredIsOwned = خطأ;
    }
    أرجع *هذا;
}

Locale LocaleMatcher::نتیجه_2::makeResolvedLocale(UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد) || supportedLocale == اشاره_تهی) {
        أرجع Locale::get_ریشه();
    }
    ثابت Locale *بهترین_Desired = getDesiredLocale();
    إذا (بهترین_Desired == اشاره_تهی || *supportedLocale == *بهترین_Desired) {
        أرجع *supportedLocale;
    }
    LocaleBuilder b;
    b.مجموعه_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = بهترین_Desired->getCountry();
    إذا (*region != 0) {
        b.مجموعه_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = بهترین_Desired->getVariant();
    إذا (*variants != 0) {
        b.مجموعه_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*بهترین_Desired, error_کد);
    أرجع b.build(error_کد);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_کد_2(src.error_کد_2),
        supportedLocales_(src.supportedLocales_),
        آستانه_فاصله(src.آستانه_فاصله),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = اشاره_تهی;
    src.defaultLocale_ = اشاره_تهی;
}

LocaleMatcher::Builder::~Builder() {
    حذف supportedLocales_;
    حذف defaultLocale_;
    حذف max_فاصله_Desired;
    حذف max_فاصله_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::عملگر=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    هذا->~Builder();

    error_کد_2 = src.error_کد_2;
    supportedLocales_ = src.supportedLocales_;
    آستانه_فاصله = src.آستانه_فاصله;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = اشاره_تهی;
    src.defaultLocale_ = اشاره_تهی;
    أرجع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    إذا (supportedLocales_ != اشاره_تهی) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensure_Supported_Locale_وکتور() {
    إذا (U_FAILURE(error_کد_2)) { أرجع خطأ; }
    إذا (supportedLocales_ != اشاره_تهی) { أرجع درست; }
    Local_اشاره_گر<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, اشاره_تهی, error_کد_2), error_کد_2);
    إذا (U_FAILURE(error_کد_2)) { أرجع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    أرجع درست;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_اولویت_List list(locales, error_کد_2);
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    clearSupportedLocales();
    إذا (!ensure_Supported_Locale_وکتور()) { أرجع *هذا; }
    int32_t length = list.get_درازی_Including_Removed();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        إذا (locale == اشاره_تهی) { استمر; }
        supportedLocales_->adoptElement(locale, error_کد_2);
        إذا (U_FAILURE(error_کد_2)) {
            اكسر;
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_وکتور()) {
        clearSupportedLocales();
        طالما (locales.has_بعدی() && U_SUCCESS(error_کد_2)) {
            ثابت Locale &locale = locales.next();
            Local_اشاره_گر<Locale> clone (locale.clone(), error_کد_2);
            supportedLocales_->adoptElement(clone.orphan(), error_کد_2);
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::جمع_کن_Supported_Locale(ثابت Locale& locale) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_وکتور()) {
        Local_اشاره_گر<Locale> clone(locale.clone(), error_کد_2);
        supportedLocales_->adoptElement(clone.orphan(), error_کد_2);
    }
    أرجع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::مجموعه_No_Default_Locale() U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    حذف defaultLocale_;
    defaultLocale_ = اشاره_تهی;
    withDefault_ = خطأ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Default_Locale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    Locale *clone = اشاره_تهی;
    إذا (defaultLocale != اشاره_تهی) {
        clone = defaultLocale->clone();
        إذا (clone == اشاره_تهی) {
            error_کد_2 = U_حافظه_تخصیص_ERROR;
            أرجع *هذا;
        }
    }
    حذف defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = درست;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Favor_Subtag(ULoc_تطابق_Favor_Subtag subtag) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    favor_ = subtag;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Demotion_در_هر_Desired_Locale(ULoc_تطابق_Demotion demotion) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    demotion_ = demotion;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::مجموعه_Max_فاصله(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    إذا (desiredClone == اشاره_تهی || supportedClone == اشاره_تهی) {
        حذف desiredClone;  // in case only one could not be allocated
        حذف supportedClone;
        error_کد_2 = U_حافظه_تخصیص_ERROR;
        أرجع *هذا;
    }
    حذف max_فاصله_Desired;
    حذف max_فاصله_Supported;
    max_فاصله_Desired = desiredClone;
    max_فاصله_Supported = supportedClone;
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
LocaleMatcher::Builder &LocaleMatcher::Builder::داخلی_مجموعه_آستانه_فاصله(int32_t آستانه_فاصله_2) {
    إذا (U_FAILURE(error_کد_2)) { أرجع *هذا; }
    إذا (آستانه_فاصله_2 > 100) {
        آستانه_فاصله_2 = 100;
    }
    آستانه_فاصله = آستانه_فاصله_2;
    أرجع *هذا;
}
#پایان_اگر

UBool LocaleMatcher::Builder::copy_Error_مقصد(UError_کد &بیرون_Error_کد) ثابت {
    إذا (U_FAILURE(بیرون_Error_کد)) { أرجع درست; }
    إذا (U_SUCCESS(error_کد_2)) { أرجع خطأ; }
    بیرون_Error_کد = error_کد_2;
    أرجع درست;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_کد &error_کد) ثابت {
    إذا (U_SUCCESS(error_کد) && U_FAILURE(error_کد_2)) {
        error_کد = error_کد_2;
    }
    أرجع LocaleMatcher(*هذا, error_کد);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        أرجع UND_LSR;
    } وإلا {
        أرجع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, error_کد);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.اشاره_گر);
    أرجع lsr->hash_کد;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.اشاره_گر);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.اشاره_گر);
    أرجع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t supp_درازی,
                                   UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع supp_درازی; }
    إذا (!uhash_شامل_است_کلید(supported_Lsr_مقصد_شاخص, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_مقصد_شاخص, const_cast<LSR *>(&lsr), i, &error_کد);
        إذا (U_SUCCESS(error_کد)) {
            supportedLSRs[supp_درازی] = &lsr;
            supportedIndexes[supp_درازی++] = i;
        }
    }
    أرجع supp_درازی;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UError_کد &error_کد) :
        likelySubtags(*LikelySubtags::getSingleton(error_کد)),
        locale_فاصله(*Locale_فاصله::getSingleton(error_کد)),
        آستانه_فاصله_2(builder.آستانه_فاصله),
        demotion_در_هر_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(اشاره_تهی), lsrs(اشاره_تهی), supported_Locales_درازی(0),
        supported_Lsr_مقصد_شاخص(اشاره_تهی),
        supportedLSRs(اشاره_تهی), supportedIndexes(اشاره_تهی), supported_LSRs_درازی(0),
        ownedDefaultLocale(اشاره_تهی), defaultLocale(اشاره_تهی) {
    إذا (U_FAILURE(error_کد)) { أرجع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = اشاره_تهی;
    إذا (def != اشاره_تهی) {
        ownedDefaultLocale = def->clone();
        إذا (ownedDefaultLocale == اشاره_تهی) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_درازی = builder.supportedLocales_ != اشاره_تهی ?
        builder.supportedLocales_->size() : 0;
    إذا (supported_Locales_درازی > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supported_Locales_درازی * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supported_Locales_درازی];
        إذا (supportedLocales == اشاره_تهی || lsrs == اشاره_تهی) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_درازی * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supported_Locales_درازی; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            إذا (supportedLocales[i] == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_کد);
            lsr.مجموعه_Hash_کد();
            إذا (U_FAILURE(error_کد)) { أرجع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_مقصد_شاخص = uhash_open_اندازه(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_درازی, &error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supported_Locales_درازی * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_درازی * حجم(int32_t)));
        إذا (supportedLSRs == اشاره_تهی || supportedIndexes == اشاره_تهی) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        int32_t supp_درازی = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_پشته_Array<int8_t, 100> ترتیب(supported_Locales_درازی, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        int32_t رقم_Paradigms = 0;
        لكل (int32_t i = 0; i < supported_Locales_درازی; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            إذا (defLSR == اشاره_تهی && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ترتیب[i] = 1;
                supp_درازی = putIfAbsent(lsr, 0, supp_درازی, error_کد);
            } وإلا إذا (defLSR != اشاره_تهی && lsr.is_Equivalent_مقصد(*defLSR)) {
                ترتیب[i] = 1;
                supp_درازی = putIfAbsent(lsr, i, supp_درازی, error_کد);
            } وإلا إذا (locale_فاصله.isParadigmLSR(lsr)) {
                ترتیب[i] = 2;
                ++رقم_Paradigms;
            } وإلا {
                ترتیب[i] = 3;
            }
            إذا (U_FAILURE(error_کد)) { أرجع; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_حد = supp_درازی + رقم_Paradigms;
        لكل (int32_t i = 0; i < supported_Locales_درازی && supp_درازی < paradigm_حد; ++i) {
            إذا (ترتیب[i] == 2) {
                supp_درازی = putIfAbsent(lsrs[i], i, supp_درازی, error_کد);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supported_Locales_درازی; ++i) {
            إذا (ترتیب[i] == 3) {
                supp_درازی = putIfAbsent(lsrs[i], i, supp_درازی, error_کد);
            }
        }
        supported_LSRs_درازی = supp_درازی;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    إذا (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_در_هر_Desired_Locale = locale_فاصله.get_Default_Demotion_در_هر_Desired_Locale();
    }

    إذا (آستانه_فاصله_2 >= 0) {
        // already copied
    } وإلا إذا (builder.max_فاصله_Desired != اشاره_تهی) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصله_Supported, error_کد);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t شاخص_And_فاصله = locale_فاصله.get_بهترین_شاخص_And_فاصله(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصله_Desired, error_کد),
                &pSuppLSR, 1,
                Locale_فاصله::لغزش_فاصله(100), favorSubtag, direction);
        إذا (U_SUCCESS(error_کد)) {
            // +1 for an exclusive threshold from an inclusive max.
            آستانه_فاصله_2 = Locale_فاصله::get_فاصله_Floor(شاخص_And_فاصله) + 1;
        } وإلا {
            آستانه_فاصله_2 = 0;
        }
    } وإلا {
        آستانه_فاصله_2 = locale_فاصله.get_Default_Script_فاصله();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_فاصله(src.locale_فاصله),
        آستانه_فاصله_2(src.آستانه_فاصله_2),
        demotion_در_هر_Desired_Locale(src.demotion_در_هر_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_درازی(src.supported_Locales_درازی),
        supported_Lsr_مقصد_شاخص(src.supported_Lsr_مقصد_شاخص),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_درازی(src.supported_LSRs_درازی),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = اشاره_تهی;
    src.lsrs = اشاره_تهی;
    src.supported_Locales_درازی = 0;
    src.supported_Lsr_مقصد_شاخص = اشاره_تهی;
    src.supportedLSRs = اشاره_تهی;
    src.supportedIndexes = اشاره_تهی;
    src.supported_LSRs_درازی = 0;
    src.ownedDefaultLocale = اشاره_تهی;
    src.defaultLocale = اشاره_تهی;
}

LocaleMatcher::~LocaleMatcher() {
    لكل (int32_t i = 0; i < supported_Locales_درازی; ++i) {
        حذف supportedLocales[i];
    }
    uprv_free(supportedLocales);
    حذف[] lsrs;
    uhash_close(supported_Lsr_مقصد_شاخص);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    حذف ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::عملگر=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    هذا->~LocaleMatcher();

    آستانه_فاصله_2 = src.آستانه_فاصله_2;
    demotion_در_هر_Desired_Locale = src.demotion_در_هر_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_درازی = src.supported_Locales_درازی;
    supported_Lsr_مقصد_شاخص = src.supported_Lsr_مقصد_شاخص;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_درازی = src.supported_LSRs_درازی;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = اشاره_تهی;
    src.lsrs = اشاره_تهی;
    src.supported_Locales_درازی = 0;
    src.supported_Lsr_مقصد_شاخص = اشاره_تهی;
    src.supportedLSRs = اشاره_تهی;
    src.supportedIndexes = اشاره_تهی;
    src.supported_LSRs_درازی = 0;
    src.ownedDefaultLocale = اشاره_تهی;
    src.defaultLocale = اشاره_تهی;
    أرجع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_تطابق_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        إذا (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            حذف remembered;
        }
    }

    منطقي has_بعدی() ثابت {
        أرجع locales.has_بعدی();
    }

    LSR next(UError_کد &error_کد) {
        current = &locales.next();
        أرجع getMaximalLsrOrUnd(likelySubtags, *current, error_کد);
    }

    فراغ rememberCurrent(int32_t desired_شاخص, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع; }
        بهترین_Desired_شاخص = desired_شاخص;
        إذا (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            حذف remembered;
            remembered = جديد Locale(*current);
            إذا (remembered == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = اشاره_تهی;
        أرجع rem;
    }

    int32_t get_بهترین_Desired_شاخص() ثابت {
        أرجع بهترین_Desired_شاخص;
    }

خاص:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_تطابق_Lifetime lifetime;
    ثابت Locale *current = اشاره_تهی, *remembered = اشاره_تهی;
    int32_t بهترین_Desired_شاخص = -1;
};

ثابت Locale* LocaleMatcher::get_بهترین_تطابق(ثابت Locale& desiredLocale,
                                          UError_کد& error_کد) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    std::optional<int32_t> supp_شاخص = get_بهترین_Supp_شاخص(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کد),
        اشاره_تهی, error_کد);
    أرجع U_SUCCESS(error_کد) && supp_شاخص.has_value() ? supportedLocales[*supp_شاخص]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::get_بهترین_تطابق(Locale::Iterator& desiredLocales,
                                          UError_کد& error_کد) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    إذا (!desiredLocales.has_بعدی()) {
        أرجع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_شاخص = get_بهترین_Supp_شاخص(lsrIter.next(error_کد), &lsrIter, error_کد);
    أرجع U_SUCCESS(error_کد) && supp_شاخص.has_value() ? supportedLocales[*supp_شاخص]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::get_بهترین_تطابق_For_List_String(StringPiece desiredLocaleList,
                                         UError_کد& error_کد) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    Locale_اولویت_List list(desiredLocaleList, error_کد);
    Locale_اولویت_List::Iterator iter = list.iterator();
    أرجع get_بهترین_تطابق(iter, error_کد);
}

LocaleMatcher::نتیجه_2
LocaleMatcher::get_بهترین_تطابق_نتیجه(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_کد& error_کد) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد)) {
        أرجع نتیجه_2(اشاره_تهی, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> supp_شاخص = get_بهترین_Supp_شاخص(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کد),
        اشاره_تهی, error_کد);
    إذا (U_FAILURE(error_کد) || !supp_شاخص.has_value()) {
        أرجع نتیجه_2(اشاره_تهی, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتیجه_2(&desiredLocale, supportedLocales[*supp_شاخص], 0, *supp_شاخص, خطأ);
    }
}

LocaleMatcher::نتیجه_2
LocaleMatcher::get_بهترین_تطابق_نتیجه(Locale::Iterator& desiredLocales,
                                  UError_کد& error_کد) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کد) || !desiredLocales.has_بعدی()) {
        أرجع نتیجه_2(اشاره_تهی, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_شاخص = get_بهترین_Supp_شاخص(lsrIter.next(error_کد), &lsrIter, error_کد);
    إذا (U_FAILURE(error_کد) || !supp_شاخص.has_value()) {
        أرجع نتیجه_2(اشاره_تهی, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتیجه_2(lsrIter.orphanRemembered(), supportedLocales[*supp_شاخص],
                      lsrIter.get_بهترین_Desired_شاخص(), *supp_شاخص, درست);
    }
}

std::optional<int32_t> LocaleMatcher::get_بهترین_Supp_شاخص(LSR desiredLSR,
                                                       LocaleLsrIterator *مانده_Iter,
                                                       UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع std::nullopt; }
    int32_t desired_شاخص = 0;
    int32_t بهترین_Supported_Lsr_شاخص = -1;
    لكل (int32_t بهترین_Shifted_فاصله = Locale_فاصله::لغزش_فاصله(آستانه_فاصله_2);;) {
        // Quick check for exact maximized LSR.
        إذا (supported_Lsr_مقصد_شاخص != اشاره_تهی) {
            desiredLSR.مجموعه_Hash_کد();
            UBool یافت = خطأ;
            int32_t supp_شاخص = uhash_geti_And_یافت(supported_Lsr_مقصد_شاخص, &desiredLSR, &یافت);
            إذا (یافت) {
                إذا (مانده_Iter != اشاره_تهی) {
                    مانده_Iter->rememberCurrent(desired_شاخص, error_کد);
                }
                أرجع supp_شاخص;
            }
        }
        int32_t بهترین_شاخص_And_فاصله = locale_فاصله.get_بهترین_شاخص_And_فاصله(
                desiredLSR, supportedLSRs, supported_LSRs_درازی,
                بهترین_Shifted_فاصله, favorSubtag, direction);
        إذا (بهترین_شاخص_And_فاصله >= 0) {
            بهترین_Shifted_فاصله = Locale_فاصله::get_Shifted_فاصله(بهترین_شاخص_And_فاصله);
            إذا (مانده_Iter != اشاره_تهی) {
                مانده_Iter->rememberCurrent(desired_شاخص, error_کد);
                إذا (U_FAILURE(error_کد)) { أرجع std::nullopt; }
            }
            بهترین_Supported_Lsr_شاخص = Locale_فاصله::get_شاخص(بهترین_شاخص_And_فاصله);
        }
        إذا ((بهترین_Shifted_فاصله -= Locale_فاصله::لغزش_فاصله(demotion_در_هر_Desired_Locale)) <= 0) {
            اكسر;
        }
        إذا (مانده_Iter == اشاره_تهی || !مانده_Iter->has_بعدی()) {
            اكسر;
        }
        desiredLSR = مانده_Iter->next(error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع std::nullopt; }
        ++desired_شاخص;
    }
    إذا (بهترین_Supported_Lsr_شاخص < 0) {
        // no good match
        أرجع std::nullopt;
    }
    أرجع supportedIndexes[بهترین_Supported_Lsr_شاخص];
}

UBool LocaleMatcher::is_تطابق(ثابت Locale &desired, ثابت Locale &supported,
                             UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t شاخص_And_فاصله = locale_فاصله.get_بهترین_شاخص_And_فاصله(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کد),
            &pSuppLSR, 1,
            Locale_فاصله::لغزش_فاصله(آستانه_فاصله_2), favorSubtag, direction);
    أرجع شاخص_And_فاصله >= 0;
}

مزدوج LocaleMatcher::داخلی_تطابق(ثابت Locale &desired, ثابت Locale &supported, UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t شاخص_And_فاصله = locale_فاصله.get_بهترین_شاخص_And_فاصله(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کد),
            &pSuppLSR, 1,
            Locale_فاصله::لغزش_فاصله(آستانه_فاصله_2), favorSubtag, direction);
    مزدوج فاصله = Locale_فاصله::get_فاصله_Double(شاخص_And_فاصله);
    أرجع (100.0 - فاصله) / 100.0;
}

U_NAMESPACE_انتها

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف Locale_From_برچسب {
عام:
    Locale_From_برچسب() : locale(Locale::get_ریشه()) {}
    ثابت Locale& عملگر()(ثابت حرف* برچسب) U_LIFETIME_BOUND { أرجع locale = Locale(برچسب); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAccept_نتیجه *accept_نتیجه,
                       UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    طالما ((locString = uenum_بعدی(&supportedLocales, اشاره_تهی, &error_کد)) != اشاره_تهی) {
        Locale loc(locString);
        إذا (loc.isBogus()) {
            error_کد = U_ILLEGAL_آرگومان_ERROR;
            أرجع 0;
        }
        builder.جمع_کن_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_کد);
    LocaleMatcher::نتیجه_2 نتیجه = matcher.get_بهترین_تطابق_نتیجه(desiredLocales, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع 0; }
    إذا (نتیجه.get_Desired_شاخص() >= 0) {
        إذا (accept_نتیجه != اشاره_تهی) {
            *accept_نتیجه = *نتیجه.getDesiredLocale() == *نتیجه.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *بهترین_Str = نتیجه.getSupportedLocale()->getName();
        int32_t بهترین_درازی = static_cast<int32_t>(uprv_strlen(بهترین_Str));
        إذا (بهترین_درازی <= capacity) {
            uprv_memcpy(dest, بهترین_Str, بهترین_درازی);
        }
        أرجع u_terminateChars(dest, capacity, بهترین_درازی, &error_کد);
    } وإلا {
        إذا (accept_نتیجه != اشاره_تهی) {
            *accept_نتیجه = ULOC_ACCEPT_FAILED;
        }
        أرجع u_terminateChars(dest, capacity, 0, &error_کد);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *نتیجه, int32_t نتیجه_موجود,
                    UAccept_نتیجه *بیرون_نتیجه,
                    ثابت حرف **acceptList, int32_t accept_List_شمار,
                    UEnumeration *موجود_Locales,
                    UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) { أرجع 0; }
    إذا ((نتیجه == اشاره_تهی ? نتیجه_موجود != 0 : نتیجه_موجود < 0) ||
            (acceptList == اشاره_تهی ? accept_List_شمار != 0 : accept_List_شمار < 0) ||
            موجود_Locales == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    Locale_From_برچسب converter;
    Locale::ConvertingIterator<ثابت حرف **, Locale_From_برچسب> desiredLocales(
        acceptList, acceptList + accept_List_شمار, converter);
    أرجع acceptLanguage(*موجود_Locales, desiredLocales,
                          نتیجه, نتیجه_موجود, بیرون_نتیجه, *وضعیت);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *نتیجه, int32_t نتیجه_موجود,
                            UAccept_نتیجه *بیرون_نتیجه,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *موجود_Locales,
                            UError_کد *وضعیت) {
    إذا (U_FAILURE(*وضعیت)) { أرجع 0; }
    إذا ((نتیجه == اشاره_تهی ? نتیجه_موجود != 0 : نتیجه_موجود < 0) ||
            httpAcceptLanguage == اشاره_تهی || موجود_Locales == اشاره_تهی) {
        *وضعیت = U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }
    Locale_اولویت_List list(httpAcceptLanguage, *وضعیت);
    Locale_اولویت_List::Iterator desiredLocales = list.iterator();
    أرجع acceptLanguage(*موجود_Locales, desiredLocales,
                          نتیجه, نتیجه_موجود, بیرون_نتیجه, *وضعیت);
}
