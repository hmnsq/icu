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

#تعریف_کرو UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
تعداد ULoc_میل_Lifetime {
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
#جے_بنا_تعریف U_وچ_DOXYGEN
تعريفنوع تعداد ULoc_میل_Lifetime ULoc_میل_Lifetime;
#جے_انت

U_NAMESPACE_BEGIN

LocaleMatcher::نتیجہ_2::نتیجہ_2(LocaleMatcher::نتیجہ_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_اشاریہ(src.desired_اشاریہ),
        supported_اشاریہ(src.supported_اشاریہ),
        desiredIsOwned(src.desiredIsOwned) {
    إذا (desiredIsOwned) {
        src.desiredLocale = خالی_سوچک;
        src.desired_اشاریہ = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::نتیجہ_2::~نتیجہ_2() {
    إذا (desiredIsOwned) {
        مٹاؤ desiredLocale;
    }
}

LocaleMatcher::نتیجہ_2&
LocaleMatcher::نتیجہ_2::عامل=(LocaleMatcher::نتیجہ_2&& src) noexcept U_LIFETIME_BOUND {
    هذا->~نتیجہ_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_اشاریہ = src.desired_اشاریہ;
    supported_اشاریہ = src.supported_اشاریہ;
    desiredIsOwned = src.desiredIsOwned;

    إذا (desiredIsOwned) {
        src.desiredLocale = خالی_سوچک;
        src.desired_اشاریہ = -1;
        src.desiredIsOwned = خطأ;
    }
    أرجع *هذا;
}

Locale LocaleMatcher::نتیجہ_2::makeResolvedLocale(UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ) || supportedLocale == خالی_سوچک) {
        أرجع Locale::get_جڑ();
    }
    ثابت Locale *بہترین_Desired = getDesiredLocale();
    إذا (بہترین_Desired == خالی_سوچک || *supportedLocale == *بہترین_Desired) {
        أرجع *supportedLocale;
    }
    LocaleBuilder b;
    b.سیٹ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = بہترین_Desired->getCountry();
    إذا (*region != 0) {
        b.سیٹ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = بہترین_Desired->getVariant();
    إذا (*variants != 0) {
        b.سیٹ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*بہترین_Desired, error_کوڈ);
    أرجع b.build(error_کوڈ);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_کوڈ_2(src.error_کوڈ_2),
        supportedLocales_(src.supportedLocales_),
        حد_فاصلہ(src.حد_فاصلہ),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = خالی_سوچک;
    src.defaultLocale_ = خالی_سوچک;
}

LocaleMatcher::Builder::~Builder() {
    مٹاؤ supportedLocales_;
    مٹاؤ defaultLocale_;
    مٹاؤ max_فاصلہ_Desired;
    مٹاؤ max_فاصلہ_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::عامل=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    هذا->~Builder();

    error_کوڈ_2 = src.error_کوڈ_2;
    supportedLocales_ = src.supportedLocales_;
    حد_فاصلہ = src.حد_فاصلہ;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = خالی_سوچک;
    src.defaultLocale_ = خالی_سوچک;
    أرجع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    إذا (supportedLocales_ != خالی_سوچک) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensure_Supported_Locale_سمتیہ() {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع خطأ; }
    إذا (supportedLocales_ != خالی_سوچک) { أرجع سچ; }
    Local_اشارہ_گر<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, خالی_سوچک, error_کوڈ_2), error_کوڈ_2);
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    أرجع سچ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ترجیح_List list(locales, error_کوڈ_2);
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    clearSupportedLocales();
    إذا (!ensure_Supported_Locale_سمتیہ()) { أرجع *هذا; }
    int32_t length = list.get_لمائی_Including_Removed();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        إذا (locale == خالی_سوچک) { استمر; }
        supportedLocales_->adoptElement(locale, error_کوڈ_2);
        إذا (U_FAILURE(error_کوڈ_2)) {
            اكسر;
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_سمتیہ()) {
        clearSupportedLocales();
        طالما (locales.has_اگلا() && U_SUCCESS(error_کوڈ_2)) {
            ثابت Locale &locale = locales.next();
            Local_اشارہ_گر<Locale> clone (locale.clone(), error_کوڈ_2);
            supportedLocales_->adoptElement(clone.orphan(), error_کوڈ_2);
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::جوڑو_Supported_Locale(ثابت Locale& locale) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_سمتیہ()) {
        Local_اشارہ_گر<Locale> clone(locale.clone(), error_کوڈ_2);
        supportedLocales_->adoptElement(clone.orphan(), error_کوڈ_2);
    }
    أرجع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::سیٹ_No_Default_Locale() U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    مٹاؤ defaultLocale_;
    defaultLocale_ = خالی_سوچک;
    withDefault_ = خطأ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Default_Locale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    Locale *clone = خالی_سوچک;
    إذا (defaultLocale != خالی_سوچک) {
        clone = defaultLocale->clone();
        إذا (clone == خالی_سوچک) {
            error_کوڈ_2 = U_حافظہ_ونڈ_ERROR;
            أرجع *هذا;
        }
    }
    مٹاؤ defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = سچ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Favor_Subtag(ULoc_میل_Favor_Subtag subtag) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    favor_ = subtag;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Demotion_فی_Desired_Locale(ULoc_میل_Demotion demotion) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    demotion_ = demotion;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Max_فاصلہ(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    إذا (desiredClone == خالی_سوچک || supportedClone == خالی_سوچک) {
        مٹاؤ desiredClone;  // in case only one could not be allocated
        مٹاؤ supportedClone;
        error_کوڈ_2 = U_حافظہ_ونڈ_ERROR;
        أرجع *هذا;
    }
    مٹاؤ max_فاصلہ_Desired;
    مٹاؤ max_فاصلہ_Supported;
    max_فاصلہ_Desired = desiredClone;
    max_فاصلہ_Supported = supportedClone;
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
LocaleMatcher::Builder &LocaleMatcher::Builder::اندرونی_سیٹ_حد_فاصلہ(int32_t حد_فاصلہ_2) {
    إذا (U_FAILURE(error_کوڈ_2)) { أرجع *هذا; }
    إذا (حد_فاصلہ_2 > 100) {
        حد_فاصلہ_2 = 100;
    }
    حد_فاصلہ = حد_فاصلہ_2;
    أرجع *هذا;
}
#جے_انت

UBool LocaleMatcher::Builder::copy_Error_منزل(UError_کوڈ &باہرلا_Error_کوڈ) ثابت {
    إذا (U_FAILURE(باہرلا_Error_کوڈ)) { أرجع سچ; }
    إذا (U_SUCCESS(error_کوڈ_2)) { أرجع خطأ; }
    باہرلا_Error_کوڈ = error_کوڈ_2;
    أرجع سچ;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_SUCCESS(error_کوڈ) && U_FAILURE(error_کوڈ_2)) {
        error_کوڈ = error_کوڈ_2;
    }
    أرجع LocaleMatcher(*هذا, error_کوڈ);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        أرجع UND_LSR;
    } وإلا {
        أرجع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, error_کوڈ);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.اشارہ_گر);
    أرجع lsr->hash_کوڈ;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.اشارہ_گر);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.اشارہ_گر);
    أرجع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t supp_لمائی,
                                   UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع supp_لمائی; }
    إذا (!uhash_شامل_اے_کلید(supported_Lsr_منزل_اشاریہ, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_منزل_اشاریہ, const_cast<LSR *>(&lsr), i, &error_کوڈ);
        إذا (U_SUCCESS(error_کوڈ)) {
            supportedLSRs[supp_لمائی] = &lsr;
            supportedIndexes[supp_لمائی++] = i;
        }
    }
    أرجع supp_لمائی;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UError_کوڈ &error_کوڈ) :
        likelySubtags(*LikelySubtags::getSingleton(error_کوڈ)),
        locale_فاصلہ(*Locale_فاصلہ::getSingleton(error_کوڈ)),
        حد_فاصلہ_2(builder.حد_فاصلہ),
        demotion_فی_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(خالی_سوچک), lsrs(خالی_سوچک), supported_Locales_لمائی(0),
        supported_Lsr_منزل_اشاریہ(خالی_سوچک),
        supportedLSRs(خالی_سوچک), supportedIndexes(خالی_سوچک), supported_LSRs_لمائی(0),
        ownedDefaultLocale(خالی_سوچک), defaultLocale(خالی_سوچک) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = خالی_سوچک;
    إذا (def != خالی_سوچک) {
        ownedDefaultLocale = def->clone();
        إذا (ownedDefaultLocale == خالی_سوچک) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_لمائی = builder.supportedLocales_ != خالی_سوچک ?
        builder.supportedLocales_->size() : 0;
    إذا (supported_Locales_لمائی > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supported_Locales_لمائی * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supported_Locales_لمائی];
        إذا (supportedLocales == خالی_سوچک || lsrs == خالی_سوچک) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_لمائی * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supported_Locales_لمائی; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            إذا (supportedLocales[i] == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_کوڈ);
            lsr.سیٹ_Hash_کوڈ();
            إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_منزل_اشاریہ = uhash_open_ناپ(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_لمائی, &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supported_Locales_لمائی * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_لمائی * حجم(int32_t)));
        إذا (supportedLSRs == خالی_سوچک || supportedIndexes == خالی_سوچک) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        int32_t supp_لمائی = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ڈھیر_Array<int8_t, 100> ترتیب(supported_Locales_لمائی, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        int32_t عدد_Paradigms = 0;
        لكل (int32_t i = 0; i < supported_Locales_لمائی; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            إذا (defLSR == خالی_سوچک && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ترتیب[i] = 1;
                supp_لمائی = putIfAbsent(lsr, 0, supp_لمائی, error_کوڈ);
            } وإلا إذا (defLSR != خالی_سوچک && lsr.is_Equivalent_منزل(*defLSR)) {
                ترتیب[i] = 1;
                supp_لمائی = putIfAbsent(lsr, i, supp_لمائی, error_کوڈ);
            } وإلا إذا (locale_فاصلہ.isParadigmLSR(lsr)) {
                ترتیب[i] = 2;
                ++عدد_Paradigms;
            } وإلا {
                ترتیب[i] = 3;
            }
            إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_حد_بندی = supp_لمائی + عدد_Paradigms;
        لكل (int32_t i = 0; i < supported_Locales_لمائی && supp_لمائی < paradigm_حد_بندی; ++i) {
            إذا (ترتیب[i] == 2) {
                supp_لمائی = putIfAbsent(lsrs[i], i, supp_لمائی, error_کوڈ);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supported_Locales_لمائی; ++i) {
            إذا (ترتیب[i] == 3) {
                supp_لمائی = putIfAbsent(lsrs[i], i, supp_لمائی, error_کوڈ);
            }
        }
        supported_LSRs_لمائی = supp_لمائی;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    إذا (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_فی_Desired_Locale = locale_فاصلہ.get_Default_Demotion_فی_Desired_Locale();
    }

    إذا (حد_فاصلہ_2 >= 0) {
        // already copied
    } وإلا إذا (builder.max_فاصلہ_Desired != خالی_سوچک) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصلہ_Supported, error_کوڈ);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصلہ_Desired, error_کوڈ),
                &pSuppLSR, 1,
                Locale_فاصلہ::کھسکاؤ_فاصلہ(100), favorSubtag, direction);
        إذا (U_SUCCESS(error_کوڈ)) {
            // +1 for an exclusive threshold from an inclusive max.
            حد_فاصلہ_2 = Locale_فاصلہ::get_فاصلہ_Floor(اشاریہ_And_فاصلہ) + 1;
        } وإلا {
            حد_فاصلہ_2 = 0;
        }
    } وإلا {
        حد_فاصلہ_2 = locale_فاصلہ.get_Default_Script_فاصلہ();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_فاصلہ(src.locale_فاصلہ),
        حد_فاصلہ_2(src.حد_فاصلہ_2),
        demotion_فی_Desired_Locale(src.demotion_فی_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_لمائی(src.supported_Locales_لمائی),
        supported_Lsr_منزل_اشاریہ(src.supported_Lsr_منزل_اشاریہ),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_لمائی(src.supported_LSRs_لمائی),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = خالی_سوچک;
    src.lsrs = خالی_سوچک;
    src.supported_Locales_لمائی = 0;
    src.supported_Lsr_منزل_اشاریہ = خالی_سوچک;
    src.supportedLSRs = خالی_سوچک;
    src.supportedIndexes = خالی_سوچک;
    src.supported_LSRs_لمائی = 0;
    src.ownedDefaultLocale = خالی_سوچک;
    src.defaultLocale = خالی_سوچک;
}

LocaleMatcher::~LocaleMatcher() {
    لكل (int32_t i = 0; i < supported_Locales_لمائی; ++i) {
        مٹاؤ supportedLocales[i];
    }
    uprv_free(supportedLocales);
    مٹاؤ[] lsrs;
    uhash_close(supported_Lsr_منزل_اشاریہ);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    مٹاؤ ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::عامل=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    هذا->~LocaleMatcher();

    حد_فاصلہ_2 = src.حد_فاصلہ_2;
    demotion_فی_Desired_Locale = src.demotion_فی_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_لمائی = src.supported_Locales_لمائی;
    supported_Lsr_منزل_اشاریہ = src.supported_Lsr_منزل_اشاریہ;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_لمائی = src.supported_LSRs_لمائی;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = خالی_سوچک;
    src.lsrs = خالی_سوچک;
    src.supported_Locales_لمائی = 0;
    src.supported_Lsr_منزل_اشاریہ = خالی_سوچک;
    src.supportedLSRs = خالی_سوچک;
    src.supportedIndexes = خالی_سوچک;
    src.supported_LSRs_لمائی = 0;
    src.ownedDefaultLocale = خالی_سوچک;
    src.defaultLocale = خالی_سوچک;
    أرجع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_میل_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        إذا (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            مٹاؤ remembered;
        }
    }

    منطقي has_اگلا() ثابت {
        أرجع locales.has_اگلا();
    }

    LSR next(UError_کوڈ &error_کوڈ) {
        current = &locales.next();
        أرجع getMaximalLsrOrUnd(likelySubtags, *current, error_کوڈ);
    }

    فراغ rememberCurrent(int32_t desired_اشاریہ, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        بہترین_Desired_اشاریہ = desired_اشاریہ;
        إذا (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            مٹاؤ remembered;
            remembered = جديد Locale(*current);
            إذا (remembered == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = خالی_سوچک;
        أرجع rem;
    }

    int32_t get_بہترین_Desired_اشاریہ() ثابت {
        أرجع بہترین_Desired_اشاریہ;
    }

خاص:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_میل_Lifetime lifetime;
    ثابت Locale *current = خالی_سوچک, *remembered = خالی_سوچک;
    int32_t بہترین_Desired_اشاریہ = -1;
};

ثابت Locale* LocaleMatcher::get_بہترین_میل(ثابت Locale& desiredLocale,
                                          UError_کوڈ& error_کوڈ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کوڈ),
        خالی_سوچک, error_کوڈ);
    أرجع U_SUCCESS(error_کوڈ) && supp_اشاریہ.has_value() ? supportedLocales[*supp_اشاریہ]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::get_بہترین_میل(Locale::Iterator& desiredLocales,
                                          UError_کوڈ& error_کوڈ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    إذا (!desiredLocales.has_اگلا()) {
        أرجع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(lsrIter.next(error_کوڈ), &lsrIter, error_کوڈ);
    أرجع U_SUCCESS(error_کوڈ) && supp_اشاریہ.has_value() ? supportedLocales[*supp_اشاریہ]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::get_بہترین_میل_For_List_String(StringPiece desiredLocaleList,
                                         UError_کوڈ& error_کوڈ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    Locale_ترجیح_List list(desiredLocaleList, error_کوڈ);
    Locale_ترجیح_List::Iterator iter = list.iterator();
    أرجع get_بہترین_میل(iter, error_کوڈ);
}

LocaleMatcher::نتیجہ_2
LocaleMatcher::get_بہترین_میل_نتیجہ(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_کوڈ& error_کوڈ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ)) {
        أرجع نتیجہ_2(خالی_سوچک, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کوڈ),
        خالی_سوچک, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ) || !supp_اشاریہ.has_value()) {
        أرجع نتیجہ_2(خالی_سوچک, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتیجہ_2(&desiredLocale, supportedLocales[*supp_اشاریہ], 0, *supp_اشاریہ, خطأ);
    }
}

LocaleMatcher::نتیجہ_2
LocaleMatcher::get_بہترین_میل_نتیجہ(Locale::Iterator& desiredLocales,
                                  UError_کوڈ& error_کوڈ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوڈ) || !desiredLocales.has_اگلا()) {
        أرجع نتیجہ_2(خالی_سوچک, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(lsrIter.next(error_کوڈ), &lsrIter, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ) || !supp_اشاریہ.has_value()) {
        أرجع نتیجہ_2(خالی_سوچک, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع نتیجہ_2(lsrIter.orphanRemembered(), supportedLocales[*supp_اشاریہ],
                      lsrIter.get_بہترین_Desired_اشاریہ(), *supp_اشاریہ, سچ);
    }
}

std::optional<int32_t> LocaleMatcher::get_بہترین_Supp_اشاریہ(LSR desiredLSR,
                                                       LocaleLsrIterator *بچیا_Iter,
                                                       UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع std::nullopt; }
    int32_t desired_اشاریہ = 0;
    int32_t بہترین_Supported_Lsr_اشاریہ = -1;
    لكل (int32_t بہترین_Shifted_فاصلہ = Locale_فاصلہ::کھسکاؤ_فاصلہ(حد_فاصلہ_2);;) {
        // Quick check for exact maximized LSR.
        إذا (supported_Lsr_منزل_اشاریہ != خالی_سوچک) {
            desiredLSR.سیٹ_Hash_کوڈ();
            UBool لبھیا = خطأ;
            int32_t supp_اشاریہ = uhash_geti_And_لبھیا(supported_Lsr_منزل_اشاریہ, &desiredLSR, &لبھیا);
            إذا (لبھیا) {
                إذا (بچیا_Iter != خالی_سوچک) {
                    بچیا_Iter->rememberCurrent(desired_اشاریہ, error_کوڈ);
                }
                أرجع supp_اشاریہ;
            }
        }
        int32_t بہترین_اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
                desiredLSR, supportedLSRs, supported_LSRs_لمائی,
                بہترین_Shifted_فاصلہ, favorSubtag, direction);
        إذا (بہترین_اشاریہ_And_فاصلہ >= 0) {
            بہترین_Shifted_فاصلہ = Locale_فاصلہ::get_Shifted_فاصلہ(بہترین_اشاریہ_And_فاصلہ);
            إذا (بچیا_Iter != خالی_سوچک) {
                بچیا_Iter->rememberCurrent(desired_اشاریہ, error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) { أرجع std::nullopt; }
            }
            بہترین_Supported_Lsr_اشاریہ = Locale_فاصلہ::get_اشاریہ(بہترین_اشاریہ_And_فاصلہ);
        }
        إذا ((بہترین_Shifted_فاصلہ -= Locale_فاصلہ::کھسکاؤ_فاصلہ(demotion_فی_Desired_Locale)) <= 0) {
            اكسر;
        }
        إذا (بچیا_Iter == خالی_سوچک || !بچیا_Iter->has_اگلا()) {
            اكسر;
        }
        desiredLSR = بچیا_Iter->next(error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع std::nullopt; }
        ++desired_اشاریہ;
    }
    إذا (بہترین_Supported_Lsr_اشاریہ < 0) {
        // no good match
        أرجع std::nullopt;
    }
    أرجع supportedIndexes[بہترین_Supported_Lsr_اشاریہ];
}

UBool LocaleMatcher::is_میل(ثابت Locale &desired, ثابت Locale &supported,
                             UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کوڈ),
            &pSuppLSR, 1,
            Locale_فاصلہ::کھسکاؤ_فاصلہ(حد_فاصلہ_2), favorSubtag, direction);
    أرجع اشاریہ_And_فاصلہ >= 0;
}

مزدوج LocaleMatcher::اندرونی_میل(ثابت Locale &desired, ثابت Locale &supported, UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کوڈ),
            &pSuppLSR, 1,
            Locale_فاصلہ::کھسکاؤ_فاصلہ(حد_فاصلہ_2), favorSubtag, direction);
    مزدوج فاصلہ = Locale_فاصلہ::get_فاصلہ_Double(اشاریہ_And_فاصلہ);
    أرجع (100.0 - فاصلہ) / 100.0;
}

U_NAMESPACE_سرا

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف Locale_From_ٹیگ {
عام:
    Locale_From_ٹیگ() : locale(Locale::get_جڑ()) {}
    ثابت Locale& عامل()(ثابت حرف* ٹیگ) U_LIFETIME_BOUND { أرجع locale = Locale(ٹیگ); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAccept_نتیجہ *accept_نتیجہ,
                       UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    طالما ((locString = uenum_اگلا(&supportedLocales, خالی_سوچک, &error_کوڈ)) != خالی_سوچک) {
        Locale loc(locString);
        إذا (loc.isBogus()) {
            error_کوڈ = U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        builder.جوڑو_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_کوڈ);
    LocaleMatcher::نتیجہ_2 نتیجہ = matcher.get_بہترین_میل_نتیجہ(desiredLocales, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع 0; }
    إذا (نتیجہ.get_Desired_اشاریہ() >= 0) {
        إذا (accept_نتیجہ != خالی_سوچک) {
            *accept_نتیجہ = *نتیجہ.getDesiredLocale() == *نتیجہ.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *بہترین_Str = نتیجہ.getSupportedLocale()->getName();
        int32_t بہترین_لمائی = static_cast<int32_t>(uprv_strlen(بہترین_Str));
        إذا (بہترین_لمائی <= capacity) {
            uprv_memcpy(dest, بہترین_Str, بہترین_لمائی);
        }
        أرجع u_terminateChars(dest, capacity, بہترین_لمائی, &error_کوڈ);
    } وإلا {
        إذا (accept_نتیجہ != خالی_سوچک) {
            *accept_نتیجہ = ULOC_ACCEPT_FAILED;
        }
        أرجع u_terminateChars(dest, capacity, 0, &error_کوڈ);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *نتیجہ, int32_t نتیجہ_دستیاب,
                    UAccept_نتیجہ *باہرلا_نتیجہ,
                    ثابت حرف **acceptList, int32_t accept_List_گنتی,
                    UEnumeration *دستیاب_Locales,
                    UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((نتیجہ == خالی_سوچک ? نتیجہ_دستیاب != 0 : نتیجہ_دستیاب < 0) ||
            (acceptList == خالی_سوچک ? accept_List_گنتی != 0 : accept_List_گنتی < 0) ||
            دستیاب_Locales == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    Locale_From_ٹیگ converter;
    Locale::ConvertingIterator<ثابت حرف **, Locale_From_ٹیگ> desiredLocales(
        acceptList, acceptList + accept_List_گنتی, converter);
    أرجع acceptLanguage(*دستیاب_Locales, desiredLocales,
                          نتیجہ, نتیجہ_دستیاب, باہرلا_نتیجہ, *حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *نتیجہ, int32_t نتیجہ_دستیاب,
                            UAccept_نتیجہ *باہرلا_نتیجہ,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *دستیاب_Locales,
                            UError_کوڈ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((نتیجہ == خالی_سوچک ? نتیجہ_دستیاب != 0 : نتیجہ_دستیاب < 0) ||
            httpAcceptLanguage == خالی_سوچک || دستیاب_Locales == خالی_سوچک) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    Locale_ترجیح_List list(httpAcceptLanguage, *حالت);
    Locale_ترجیح_List::Iterator desiredLocales = list.iterator();
    أرجع acceptLanguage(*دستیاب_Locales, desiredLocales,
                          نتیجہ, نتیجہ_دستیاب, باہرلا_نتیجہ, *حالت);
}
