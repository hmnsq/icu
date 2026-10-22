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

#تعریف_کرو UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
شمار ULoc_مطابقت_Lifetime {
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
#اگر_غیر_تعریف_شدہ U_اندر_DOXYGEN
قسمتعریف شمار ULoc_مطابقت_Lifetime ULoc_مطابقت_Lifetime;
#اگر_ختم

U_NAMESPACE_BEGIN

LocaleMatcher::نتیجہ_2::نتیجہ_2(LocaleMatcher::نتیجہ_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_اشاریہ(src.desired_اشاریہ),
        supported_اشاریہ(src.supported_اشاریہ),
        desiredIsOwned(src.desiredIsOwned) {
    اگر (desiredIsOwned) {
        src.desiredLocale = خالی_اشاریہ;
        src.desired_اشاریہ = -1;
        src.desiredIsOwned = جھوٹ;
    }
}

LocaleMatcher::نتیجہ_2::~نتیجہ_2() {
    اگر (desiredIsOwned) {
        حذف desiredLocale;
    }
}

LocaleMatcher::نتیجہ_2&
LocaleMatcher::نتیجہ_2::عامل=(LocaleMatcher::نتیجہ_2&& src) noexcept U_LIFETIME_BOUND {
    یہ->~نتیجہ_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_اشاریہ = src.desired_اشاریہ;
    supported_اشاریہ = src.supported_اشاریہ;
    desiredIsOwned = src.desiredIsOwned;

    اگر (desiredIsOwned) {
        src.desiredLocale = خالی_اشاریہ;
        src.desired_اشاریہ = -1;
        src.desiredIsOwned = جھوٹ;
    }
    واپس *یہ;
}

Locale LocaleMatcher::نتیجہ_2::makeResolvedLocale(UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز) || supportedLocale == خالی_اشاریہ) {
        واپس Locale::get_جڑ();
    }
    ثابت Locale *بہترین_Desired = getDesiredLocale();
    اگر (بہترین_Desired == خالی_اشاریہ || *supportedLocale == *بہترین_Desired) {
        واپس *supportedLocale;
    }
    LocaleBuilder b;
    b.سیٹ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = بہترین_Desired->getCountry();
    اگر (*region != 0) {
        b.سیٹ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = بہترین_Desired->getVariant();
    اگر (*variants != 0) {
        b.سیٹ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*بہترین_Desired, error_رمز);
    واپس b.build(error_رمز);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_رمز_2(src.error_رمز_2),
        supportedLocales_(src.supportedLocales_),
        حد_فاصلہ(src.حد_فاصلہ),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = خالی_اشاریہ;
    src.defaultLocale_ = خالی_اشاریہ;
}

LocaleMatcher::Builder::~Builder() {
    حذف supportedLocales_;
    حذف defaultLocale_;
    حذف max_فاصلہ_Desired;
    حذف max_فاصلہ_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::عامل=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    یہ->~Builder();

    error_رمز_2 = src.error_رمز_2;
    supportedLocales_ = src.supportedLocales_;
    حد_فاصلہ = src.حد_فاصلہ;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = خالی_اشاریہ;
    src.defaultLocale_ = خالی_اشاریہ;
    واپس *یہ;
}

خالی LocaleMatcher::Builder::clearSupportedLocales() {
    اگر (supportedLocales_ != خالی_اشاریہ) {
        supportedLocales_->removeAllElements();
    }
}

منطقی LocaleMatcher::Builder::ensure_Supported_Locale_سمتیہ() {
    اگر (U_FAILURE(error_رمز_2)) { واپس جھوٹ; }
    اگر (supportedLocales_ != خالی_اشاریہ) { واپس سچ; }
    Local_اشارہ_گر<UVector> lpSupportedLocales(نیا UVector(uprv_deleteUObject, خالی_اشاریہ, error_رمز_2), error_رمز_2);
    اگر (U_FAILURE(error_رمز_2)) { واپس جھوٹ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    واپس سچ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ترجیح_List list(locales, error_رمز_2);
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    clearSupportedLocales();
    اگر (!ensure_Supported_Locale_سمتیہ()) { واپس *یہ; }
    int32_t length = list.get_لمبائی_Including_Removed();
    برائے (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        اگر (locale == خالی_اشاریہ) { جاری; }
        supportedLocales_->adoptElement(locale, error_رمز_2);
        اگر (U_FAILURE(error_رمز_2)) {
            توڑو;
        }
    }
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    اگر (ensure_Supported_Locale_سمتیہ()) {
        clearSupportedLocales();
        جب (locales.has_اگلا() && U_SUCCESS(error_رمز_2)) {
            ثابت Locale &locale = locales.next();
            Local_اشارہ_گر<Locale> clone (locale.clone(), error_رمز_2);
            supportedLocales_->adoptElement(clone.orphan(), error_رمز_2);
        }
    }
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::جوڑو_Supported_Locale(ثابت Locale& locale) U_LIFETIME_BOUND {
    اگر (ensure_Supported_Locale_سمتیہ()) {
        Local_اشارہ_گر<Locale> clone(locale.clone(), error_رمز_2);
        supportedLocales_->adoptElement(clone.orphan(), error_رمز_2);
    }
    واپس *یہ;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::سیٹ_No_Default_Locale() U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    حذف defaultLocale_;
    defaultLocale_ = خالی_اشاریہ;
    withDefault_ = جھوٹ;
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Default_Locale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    Locale *clone = خالی_اشاریہ;
    اگر (defaultLocale != خالی_اشاریہ) {
        clone = defaultLocale->clone();
        اگر (clone == خالی_اشاریہ) {
            error_رمز_2 = U_حافظہ_تخصیص_ERROR;
            واپس *یہ;
        }
    }
    حذف defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = سچ;
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Favor_Subtag(ULoc_مطابقت_Favor_Subtag subtag) U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    favor_ = subtag;
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Demotion_فی_Desired_Locale(ULoc_مطابقت_Demotion demotion) U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    demotion_ = demotion;
    واپس *یہ;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سیٹ_Max_فاصلہ(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    اگر (desiredClone == خالی_اشاریہ || supportedClone == خالی_اشاریہ) {
        حذف desiredClone;  // in case only one could not be allocated
        حذف supportedClone;
        error_رمز_2 = U_حافظہ_تخصیص_ERROR;
        واپس *یہ;
    }
    حذف max_فاصلہ_Desired;
    حذف max_فاصلہ_Supported;
    max_فاصلہ_Desired = desiredClone;
    max_فاصلہ_Supported = supportedClone;
    واپس *یہ;
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
LocaleMatcher::Builder &LocaleMatcher::Builder::اندرونی_سیٹ_حد_فاصلہ(int32_t حد_فاصلہ_2) {
    اگر (U_FAILURE(error_رمز_2)) { واپس *یہ; }
    اگر (حد_فاصلہ_2 > 100) {
        حد_فاصلہ_2 = 100;
    }
    حد_فاصلہ = حد_فاصلہ_2;
    واپس *یہ;
}
#اگر_ختم

UBool LocaleMatcher::Builder::copy_Error_مقصد(UError_رمز &برآمد_Error_رمز) ثابت {
    اگر (U_FAILURE(برآمد_Error_رمز)) { واپس سچ; }
    اگر (U_SUCCESS(error_رمز_2)) { واپس جھوٹ; }
    برآمد_Error_رمز = error_رمز_2;
    واپس سچ;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_رمز &error_رمز) ثابت {
    اگر (U_SUCCESS(error_رمز) && U_FAILURE(error_رمز_2)) {
        error_رمز = error_رمز_2;
    }
    واپس LocaleMatcher(*یہ, error_رمز);
}

نامخلا {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        واپس UND_LSR;
    } ورنہ {
        واپس likelySubtags.makeMaximizedLsrFrom(locale, جھوٹ, error_رمز);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.اشارہ_گر);
    واپس lsr->hash_رمز;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.اشارہ_گر);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.اشارہ_گر);
    واپس *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t supp_لمبائی,
                                   UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس supp_لمبائی; }
    اگر (!uhash_شامل_ہے_کلید(supported_Lsr_مقصد_اشاریہ, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_مقصد_اشاریہ, const_cast<LSR *>(&lsr), i, &error_رمز);
        اگر (U_SUCCESS(error_رمز)) {
            supportedLSRs[supp_لمبائی] = &lsr;
            supportedIndexes[supp_لمبائی++] = i;
        }
    }
    واپس supp_لمبائی;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UError_رمز &error_رمز) :
        likelySubtags(*LikelySubtags::getSingleton(error_رمز)),
        locale_فاصلہ(*Locale_فاصلہ::getSingleton(error_رمز)),
        حد_فاصلہ_2(builder.حد_فاصلہ),
        demotion_فی_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(خالی_اشاریہ), lsrs(خالی_اشاریہ), supported_Locales_لمبائی(0),
        supported_Lsr_مقصد_اشاریہ(خالی_اشاریہ),
        supportedLSRs(خالی_اشاریہ), supportedIndexes(خالی_اشاریہ), supported_LSRs_لمبائی(0),
        ownedDefaultLocale(خالی_اشاریہ), defaultLocale(خالی_اشاریہ) {
    اگر (U_FAILURE(error_رمز)) { واپس; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = خالی_اشاریہ;
    اگر (def != خالی_اشاریہ) {
        ownedDefaultLocale = def->clone();
        اگر (ownedDefaultLocale == خالی_اشاریہ) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_لمبائی = builder.supportedLocales_ != خالی_اشاریہ ?
        builder.supportedLocales_->size() : 0;
    اگر (supported_Locales_لمبائی > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supported_Locales_لمبائی * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = نیا LSR[supported_Locales_لمبائی];
        اگر (supportedLocales == خالی_اشاریہ || lsrs == خالی_اشاریہ) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_لمبائی * حجم(ثابت Locale *));
        برائے (int32_t i = 0; i < supported_Locales_لمبائی; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            اگر (supportedLocales[i] == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_رمز);
            lsr.سیٹ_Hash_رمز();
            اگر (U_FAILURE(error_رمز)) { واپس; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_مقصد_اشاریہ = uhash_open_سائز(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_لمبائی, &error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supported_Locales_لمبائی * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_لمبائی * حجم(int32_t)));
        اگر (supportedLSRs == خالی_اشاریہ || supportedIndexes == خالی_اشاریہ) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        int32_t supp_لمبائی = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ڈھیر_Array<int8_t, 100> ترتیب(supported_Locales_لمبائی, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        int32_t عدد_Paradigms = 0;
        برائے (int32_t i = 0; i < supported_Locales_لمبائی; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            اگر (defLSR == خالی_اشاریہ && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ترتیب[i] = 1;
                supp_لمبائی = putIfAbsent(lsr, 0, supp_لمبائی, error_رمز);
            } ورنہ اگر (defLSR != خالی_اشاریہ && lsr.is_Equivalent_مقصد(*defLSR)) {
                ترتیب[i] = 1;
                supp_لمبائی = putIfAbsent(lsr, i, supp_لمبائی, error_رمز);
            } ورنہ اگر (locale_فاصلہ.isParadigmLSR(lsr)) {
                ترتیب[i] = 2;
                ++عدد_Paradigms;
            } ورنہ {
                ترتیب[i] = 3;
            }
            اگر (U_FAILURE(error_رمز)) { واپس; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_حد_بندی = supp_لمبائی + عدد_Paradigms;
        برائے (int32_t i = 0; i < supported_Locales_لمبائی && supp_لمبائی < paradigm_حد_بندی; ++i) {
            اگر (ترتیب[i] == 2) {
                supp_لمبائی = putIfAbsent(lsrs[i], i, supp_لمبائی, error_رمز);
            }
        }
        // Add remaining supported locales.
        برائے (int32_t i = 0; i < supported_Locales_لمبائی; ++i) {
            اگر (ترتیب[i] == 3) {
                supp_لمبائی = putIfAbsent(lsrs[i], i, supp_لمبائی, error_رمز);
            }
        }
        supported_LSRs_لمبائی = supp_لمبائی;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    اگر (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_فی_Desired_Locale = locale_فاصلہ.get_Default_Demotion_فی_Desired_Locale();
    }

    اگر (حد_فاصلہ_2 >= 0) {
        // already copied
    } ورنہ اگر (builder.max_فاصلہ_Desired != خالی_اشاریہ) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصلہ_Supported, error_رمز);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_فاصلہ_Desired, error_رمز),
                &pSuppLSR, 1,
                Locale_فاصلہ::سرکاؤ_فاصلہ(100), favorSubtag, direction);
        اگر (U_SUCCESS(error_رمز)) {
            // +1 for an exclusive threshold from an inclusive max.
            حد_فاصلہ_2 = Locale_فاصلہ::get_فاصلہ_Floor(اشاریہ_And_فاصلہ) + 1;
        } ورنہ {
            حد_فاصلہ_2 = 0;
        }
    } ورنہ {
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
        supported_Locales_لمبائی(src.supported_Locales_لمبائی),
        supported_Lsr_مقصد_اشاریہ(src.supported_Lsr_مقصد_اشاریہ),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_لمبائی(src.supported_LSRs_لمبائی),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = خالی_اشاریہ;
    src.lsrs = خالی_اشاریہ;
    src.supported_Locales_لمبائی = 0;
    src.supported_Lsr_مقصد_اشاریہ = خالی_اشاریہ;
    src.supportedLSRs = خالی_اشاریہ;
    src.supportedIndexes = خالی_اشاریہ;
    src.supported_LSRs_لمبائی = 0;
    src.ownedDefaultLocale = خالی_اشاریہ;
    src.defaultLocale = خالی_اشاریہ;
}

LocaleMatcher::~LocaleMatcher() {
    برائے (int32_t i = 0; i < supported_Locales_لمبائی; ++i) {
        حذف supportedLocales[i];
    }
    uprv_free(supportedLocales);
    حذف[] lsrs;
    uhash_close(supported_Lsr_مقصد_اشاریہ);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    حذف ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::عامل=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    یہ->~LocaleMatcher();

    حد_فاصلہ_2 = src.حد_فاصلہ_2;
    demotion_فی_Desired_Locale = src.demotion_فی_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_لمبائی = src.supported_Locales_لمبائی;
    supported_Lsr_مقصد_اشاریہ = src.supported_Lsr_مقصد_اشاریہ;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_لمبائی = src.supported_LSRs_لمبائی;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = خالی_اشاریہ;
    src.lsrs = خالی_اشاریہ;
    src.supported_Locales_لمبائی = 0;
    src.supported_Lsr_مقصد_اشاریہ = خالی_اشاریہ;
    src.supportedLSRs = خالی_اشاریہ;
    src.supportedIndexes = خالی_اشاریہ;
    src.supported_LSRs_لمبائی = 0;
    src.ownedDefaultLocale = خالی_اشاریہ;
    src.defaultLocale = خالی_اشاریہ;
    واپس *یہ;
}

جماعت LocaleLsrIterator {
عوامی:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_مطابقت_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        اگر (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            حذف remembered;
        }
    }

    منطقی has_اگلا() ثابت {
        واپس locales.has_اگلا();
    }

    LSR next(UError_رمز &error_رمز) {
        current = &locales.next();
        واپس getMaximalLsrOrUnd(likelySubtags, *current, error_رمز);
    }

    خالی rememberCurrent(int32_t desired_اشاریہ, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس; }
        بہترین_Desired_اشاریہ = desired_اشاریہ;
        اگر (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } ورنہ {
            // ULOCMATCH_TEMPORARY_LOCALES
            حذف remembered;
            remembered = نیا Locale(*current);
            اگر (remembered == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = خالی_اشاریہ;
        واپس rem;
    }

    int32_t get_بہترین_Desired_اشاریہ() ثابت {
        واپس بہترین_Desired_اشاریہ;
    }

نجی:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_مطابقت_Lifetime lifetime;
    ثابت Locale *current = خالی_اشاریہ, *remembered = خالی_اشاریہ;
    int32_t بہترین_Desired_اشاریہ = -1;
};

ثابت Locale* LocaleMatcher::get_بہترین_مطابقت(ثابت Locale& desiredLocale,
                                          UError_رمز& error_رمز) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_رمز),
        خالی_اشاریہ, error_رمز);
    واپس U_SUCCESS(error_رمز) && supp_اشاریہ.has_value() ? supportedLocales[*supp_اشاریہ]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::get_بہترین_مطابقت(Locale::Iterator& desiredLocales,
                                          UError_رمز& error_رمز) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    اگر (!desiredLocales.has_اگلا()) {
        واپس defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(lsrIter.next(error_رمز), &lsrIter, error_رمز);
    واپس U_SUCCESS(error_رمز) && supp_اشاریہ.has_value() ? supportedLocales[*supp_اشاریہ]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::get_بہترین_مطابقت_For_List_String(StringPiece desiredLocaleList,
                                         UError_رمز& error_رمز) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    Locale_ترجیح_List list(desiredLocaleList, error_رمز);
    Locale_ترجیح_List::Iterator iter = list.iterator();
    واپس get_بہترین_مطابقت(iter, error_رمز);
}

LocaleMatcher::نتیجہ_2
LocaleMatcher::get_بہترین_مطابقت_نتیجہ(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_رمز& error_رمز) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز)) {
        واپس نتیجہ_2(خالی_اشاریہ, defaultLocale, -1, -1, جھوٹ);
    }
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_رمز),
        خالی_اشاریہ, error_رمز);
    اگر (U_FAILURE(error_رمز) || !supp_اشاریہ.has_value()) {
        واپس نتیجہ_2(خالی_اشاریہ, defaultLocale, -1, -1, جھوٹ);
    } ورنہ {
        واپس نتیجہ_2(&desiredLocale, supportedLocales[*supp_اشاریہ], 0, *supp_اشاریہ, جھوٹ);
    }
}

LocaleMatcher::نتیجہ_2
LocaleMatcher::get_بہترین_مطابقت_نتیجہ(Locale::Iterator& desiredLocales,
                                  UError_رمز& error_رمز) ثابت U_LIFETIME_BOUND {
    اگر (U_FAILURE(error_رمز) || !desiredLocales.has_اگلا()) {
        واپس نتیجہ_2(خالی_اشاریہ, defaultLocale, -1, -1, جھوٹ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_اشاریہ = get_بہترین_Supp_اشاریہ(lsrIter.next(error_رمز), &lsrIter, error_رمز);
    اگر (U_FAILURE(error_رمز) || !supp_اشاریہ.has_value()) {
        واپس نتیجہ_2(خالی_اشاریہ, defaultLocale, -1, -1, جھوٹ);
    } ورنہ {
        واپس نتیجہ_2(lsrIter.orphanRemembered(), supportedLocales[*supp_اشاریہ],
                      lsrIter.get_بہترین_Desired_اشاریہ(), *supp_اشاریہ, سچ);
    }
}

std::optional<int32_t> LocaleMatcher::get_بہترین_Supp_اشاریہ(LSR desiredLSR,
                                                       LocaleLsrIterator *بقایا_Iter,
                                                       UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس std::nullopt; }
    int32_t desired_اشاریہ = 0;
    int32_t بہترین_Supported_Lsr_اشاریہ = -1;
    برائے (int32_t بہترین_Shifted_فاصلہ = Locale_فاصلہ::سرکاؤ_فاصلہ(حد_فاصلہ_2);;) {
        // Quick check for exact maximized LSR.
        اگر (supported_Lsr_مقصد_اشاریہ != خالی_اشاریہ) {
            desiredLSR.سیٹ_Hash_رمز();
            UBool ملا = جھوٹ;
            int32_t supp_اشاریہ = uhash_geti_And_ملا(supported_Lsr_مقصد_اشاریہ, &desiredLSR, &ملا);
            اگر (ملا) {
                اگر (بقایا_Iter != خالی_اشاریہ) {
                    بقایا_Iter->rememberCurrent(desired_اشاریہ, error_رمز);
                }
                واپس supp_اشاریہ;
            }
        }
        int32_t بہترین_اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
                desiredLSR, supportedLSRs, supported_LSRs_لمبائی,
                بہترین_Shifted_فاصلہ, favorSubtag, direction);
        اگر (بہترین_اشاریہ_And_فاصلہ >= 0) {
            بہترین_Shifted_فاصلہ = Locale_فاصلہ::get_Shifted_فاصلہ(بہترین_اشاریہ_And_فاصلہ);
            اگر (بقایا_Iter != خالی_اشاریہ) {
                بقایا_Iter->rememberCurrent(desired_اشاریہ, error_رمز);
                اگر (U_FAILURE(error_رمز)) { واپس std::nullopt; }
            }
            بہترین_Supported_Lsr_اشاریہ = Locale_فاصلہ::get_اشاریہ(بہترین_اشاریہ_And_فاصلہ);
        }
        اگر ((بہترین_Shifted_فاصلہ -= Locale_فاصلہ::سرکاؤ_فاصلہ(demotion_فی_Desired_Locale)) <= 0) {
            توڑو;
        }
        اگر (بقایا_Iter == خالی_اشاریہ || !بقایا_Iter->has_اگلا()) {
            توڑو;
        }
        desiredLSR = بقایا_Iter->next(error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس std::nullopt; }
        ++desired_اشاریہ;
    }
    اگر (بہترین_Supported_Lsr_اشاریہ < 0) {
        // no good match
        واپس std::nullopt;
    }
    واپس supportedIndexes[بہترین_Supported_Lsr_اشاریہ];
}

UBool LocaleMatcher::is_مطابقت(ثابت Locale &desired, ثابت Locale &supported,
                             UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_رمز),
            &pSuppLSR, 1,
            Locale_فاصلہ::سرکاؤ_فاصلہ(حد_فاصلہ_2), favorSubtag, direction);
    واپس اشاریہ_And_فاصلہ >= 0;
}

دوہرا LocaleMatcher::اندرونی_مطابقت(ثابت Locale &desired, ثابت Locale &supported, UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t اشاریہ_And_فاصلہ = locale_فاصلہ.get_بہترین_اشاریہ_And_فاصلہ(
            getMaximalLsrOrUnd(likelySubtags, desired, error_رمز),
            &pSuppLSR, 1,
            Locale_فاصلہ::سرکاؤ_فاصلہ(حد_فاصلہ_2), favorSubtag, direction);
    دوہرا فاصلہ = Locale_فاصلہ::get_فاصلہ_Double(اشاریہ_And_فاصلہ);
    واپس (100.0 - فاصلہ) / 100.0;
}

U_NAMESPACE_انتہا

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نامخلا {

جماعت Locale_From_لیبل {
عوامی:
    Locale_From_لیبل() : locale(Locale::get_جڑ()) {}
    ثابت Locale& عامل()(ثابت حرف* لیبل) U_LIFETIME_BOUND { واپس locale = Locale(لیبل); }

نجی:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAccept_نتیجہ *accept_نتیجہ,
                       UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    جب ((locString = uenum_اگلا(&supportedLocales, خالی_اشاریہ, &error_رمز)) != خالی_اشاریہ) {
        Locale loc(locString);
        اگر (loc.isBogus()) {
            error_رمز = U_ILLEGAL_دلیل_ERROR;
            واپس 0;
        }
        builder.جوڑو_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_رمز);
    LocaleMatcher::نتیجہ_2 نتیجہ = matcher.get_بہترین_مطابقت_نتیجہ(desiredLocales, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس 0; }
    اگر (نتیجہ.get_Desired_اشاریہ() >= 0) {
        اگر (accept_نتیجہ != خالی_اشاریہ) {
            *accept_نتیجہ = *نتیجہ.getDesiredLocale() == *نتیجہ.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *بہترین_Str = نتیجہ.getSupportedLocale()->getName();
        int32_t بہترین_لمبائی = static_cast<int32_t>(uprv_strlen(بہترین_Str));
        اگر (بہترین_لمبائی <= capacity) {
            uprv_memcpy(dest, بہترین_Str, بہترین_لمبائی);
        }
        واپس u_terminateChars(dest, capacity, بہترین_لمبائی, &error_رمز);
    } ورنہ {
        اگر (accept_نتیجہ != خالی_اشاریہ) {
            *accept_نتیجہ = ULOC_ACCEPT_FAILED;
        }
        واپس u_terminateChars(dest, capacity, 0, &error_رمز);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *نتیجہ, int32_t نتیجہ_دستیاب,
                    UAccept_نتیجہ *برآمد_نتیجہ,
                    ثابت حرف **acceptList, int32_t accept_List_گنتی,
                    UEnumeration *دستیاب_Locales,
                    UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) { واپس 0; }
    اگر ((نتیجہ == خالی_اشاریہ ? نتیجہ_دستیاب != 0 : نتیجہ_دستیاب < 0) ||
            (acceptList == خالی_اشاریہ ? accept_List_گنتی != 0 : accept_List_گنتی < 0) ||
            دستیاب_Locales == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    Locale_From_لیبل converter;
    Locale::ConvertingIterator<ثابت حرف **, Locale_From_لیبل> desiredLocales(
        acceptList, acceptList + accept_List_گنتی, converter);
    واپس acceptLanguage(*دستیاب_Locales, desiredLocales,
                          نتیجہ, نتیجہ_دستیاب, برآمد_نتیجہ, *حالت_2);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *نتیجہ, int32_t نتیجہ_دستیاب,
                            UAccept_نتیجہ *برآمد_نتیجہ,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *دستیاب_Locales,
                            UError_رمز *حالت_2) {
    اگر (U_FAILURE(*حالت_2)) { واپس 0; }
    اگر ((نتیجہ == خالی_اشاریہ ? نتیجہ_دستیاب != 0 : نتیجہ_دستیاب < 0) ||
            httpAcceptLanguage == خالی_اشاریہ || دستیاب_Locales == خالی_اشاریہ) {
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }
    Locale_ترجیح_List list(httpAcceptLanguage, *حالت_2);
    Locale_ترجیح_List::Iterator desiredLocales = list.iterator();
    واپس acceptLanguage(*دستیاب_Locales, desiredLocales,
                          نتیجہ, نتیجہ_دستیاب, برآمد_نتیجہ, *حالت_2);
}
