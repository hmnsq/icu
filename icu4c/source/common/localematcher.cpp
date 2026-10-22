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

#تعریف_کړه UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
تعداد ULoc_سمون_Lifetime {
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
#که_نه_تعریف_شوی U_دننه_DOXYGEN
تعريفنوع تعداد ULoc_سمون_Lifetime ULoc_سمون_Lifetime;
#د_که_پای

U_NAMESPACE_BEGIN

LocaleMatcher::پایله_2::پایله_2(LocaleMatcher::پایله_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_شاخص(src.desired_شاخص),
        supported_شاخص(src.supported_شاخص),
        desiredIsOwned(src.desiredIsOwned) {
    إذا (desiredIsOwned) {
        src.desiredLocale = تش_اشاره;
        src.desired_شاخص = -1;
        src.desiredIsOwned = خطأ;
    }
}

LocaleMatcher::پایله_2::~پایله_2() {
    إذا (desiredIsOwned) {
        ړنګول desiredLocale;
    }
}

LocaleMatcher::پایله_2&
LocaleMatcher::پایله_2::چلوونکی=(LocaleMatcher::پایله_2&& src) noexcept U_LIFETIME_BOUND {
    هذا->~پایله_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_شاخص = src.desired_شاخص;
    supported_شاخص = src.supported_شاخص;
    desiredIsOwned = src.desiredIsOwned;

    إذا (desiredIsOwned) {
        src.desiredLocale = تش_اشاره;
        src.desired_شاخص = -1;
        src.desiredIsOwned = خطأ;
    }
    أرجع *هذا;
}

Locale LocaleMatcher::پایله_2::makeResolvedLocale(UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ) || supportedLocale == تش_اشاره) {
        أرجع Locale::get_ریښه();
    }
    ثابت Locale *غوره_Desired = getDesiredLocale();
    إذا (غوره_Desired == تش_اشاره || *supportedLocale == *غوره_Desired) {
        أرجع *supportedLocale;
    }
    LocaleBuilder b;
    b.سېټ_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ثابت حرف *region = غوره_Desired->getCountry();
    إذا (*region != 0) {
        b.سېټ_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ثابت حرف *variants = غوره_Desired->getVariant();
    إذا (*variants != 0) {
        b.سېټ_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*غوره_Desired, error_کوډ);
    أرجع b.build(error_کوډ);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_کوډ_2(src.error_کوډ_2),
        supportedLocales_(src.supportedLocales_),
        پوله_واټن(src.پوله_واټن),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = تش_اشاره;
    src.defaultLocale_ = تش_اشاره;
}

LocaleMatcher::Builder::~Builder() {
    ړنګول supportedLocales_;
    ړنګول defaultLocale_;
    ړنګول max_واټن_Desired;
    ړنګول max_واټن_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::چلوونکی=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    هذا->~Builder();

    error_کوډ_2 = src.error_کوډ_2;
    supportedLocales_ = src.supportedLocales_;
    پوله_واټن = src.پوله_واټن;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = تش_اشاره;
    src.defaultLocale_ = تش_اشاره;
    أرجع *هذا;
}

فراغ LocaleMatcher::Builder::clearSupportedLocales() {
    إذا (supportedLocales_ != تش_اشاره) {
        supportedLocales_->removeAllElements();
    }
}

منطقي LocaleMatcher::Builder::ensure_Supported_Locale_ویکټور() {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع خطأ; }
    إذا (supportedLocales_ != تش_اشاره) { أرجع ریښتیا; }
    Local_اشاره_ګر<UVector> lpSupportedLocales(جديد UVector(uprv_deleteUObject, تش_اشاره, error_کوډ_2), error_کوډ_2);
    إذا (U_FAILURE(error_کوډ_2)) { أرجع خطأ; }
    supportedLocales_ = lpSupportedLocales.orphan();
    أرجع ریښتیا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_لومړیتوب_List list(locales, error_کوډ_2);
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    clearSupportedLocales();
    إذا (!ensure_Supported_Locale_ویکټور()) { أرجع *هذا; }
    int32_t length = list.get_اوږدتیا_Including_Removed();
    لكل (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        إذا (locale == تش_اشاره) { استمر; }
        supportedLocales_->adoptElement(locale, error_کوډ_2);
        إذا (U_FAILURE(error_کوډ_2)) {
            اكسر;
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_ویکټور()) {
        clearSupportedLocales();
        طالما (locales.has_بل() && U_SUCCESS(error_کوډ_2)) {
            ثابت Locale &locale = locales.next();
            Local_اشاره_ګر<Locale> clone (locale.clone(), error_کوډ_2);
            supportedLocales_->adoptElement(clone.orphan(), error_کوډ_2);
        }
    }
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::جمع_کړه_Supported_Locale(ثابت Locale& locale) U_LIFETIME_BOUND {
    إذا (ensure_Supported_Locale_ویکټور()) {
        Local_اشاره_ګر<Locale> clone(locale.clone(), error_کوډ_2);
        supportedLocales_->adoptElement(clone.orphan(), error_کوډ_2);
    }
    أرجع *هذا;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::سېټ_No_Default_Locale() U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    ړنګول defaultLocale_;
    defaultLocale_ = تش_اشاره;
    withDefault_ = خطأ;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Default_Locale(ثابت Locale* defaultLocale) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    Locale *clone = تش_اشاره;
    إذا (defaultLocale != تش_اشاره) {
        clone = defaultLocale->clone();
        إذا (clone == تش_اشاره) {
            error_کوډ_2 = U_حافظه_ویش_ERROR;
            أرجع *هذا;
        }
    }
    ړنګول defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = ریښتیا;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Favor_Subtag(ULoc_سمون_Favor_Subtag subtag) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    favor_ = subtag;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Demotion_فی_Desired_Locale(ULoc_سمون_Demotion demotion) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    demotion_ = demotion;
    أرجع *هذا;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::سېټ_Max_واټن(ثابت Locale& desired,
                                       ثابت Locale& supported) U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    إذا (desiredClone == تش_اشاره || supportedClone == تش_اشاره) {
        ړنګول desiredClone;  // in case only one could not be allocated
        ړنګول supportedClone;
        error_کوډ_2 = U_حافظه_ویش_ERROR;
        أرجع *هذا;
    }
    ړنګول max_واټن_Desired;
    ړنګول max_واټن_Supported;
    max_واټن_Desired = desiredClone;
    max_واټن_Supported = supportedClone;
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
LocaleMatcher::Builder &LocaleMatcher::Builder::دننی_سېټ_پوله_واټن(int32_t پوله_واټن_2) {
    إذا (U_FAILURE(error_کوډ_2)) { أرجع *هذا; }
    إذا (پوله_واټن_2 > 100) {
        پوله_واټن_2 = 100;
    }
    پوله_واټن = پوله_واټن_2;
    أرجع *هذا;
}
#د_که_پای

UBool LocaleMatcher::Builder::copy_Error_منزل(UError_کوډ &بهر_Error_کوډ) ثابت {
    إذا (U_FAILURE(بهر_Error_کوډ)) { أرجع ریښتیا; }
    إذا (U_SUCCESS(error_کوډ_2)) { أرجع خطأ; }
    بهر_Error_کوډ = error_کوډ_2;
    أرجع ریښتیا;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_کوډ &error_کوډ) ثابت {
    إذا (U_SUCCESS(error_کوډ) && U_FAILURE(error_کوډ_2)) {
        error_کوډ = error_کوډ_2;
    }
    أرجع LocaleMatcher(*هذا, error_کوډ);
}

نطاق {

LSR getMaximalLsrOrUnd(ثابت LikelySubtags &likelySubtags, ثابت Locale &locale,
                       UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        أرجع UND_LSR;
    } وإلا {
        أرجع likelySubtags.makeMaximizedLsrFrom(locale, خطأ, error_کوډ);
    }
}

int32_t hashLSR(ثابت UHashTok token) {
    ثابت LSR *lsr = static_cast<ثابت LSR *>(token.اشاره_ګر);
    أرجع lsr->hash_کوډ;
}

UBool compareLSRs(ثابت UHashTok t1, ثابت UHashTok t2) {
    ثابت LSR *lsr1 = static_cast<ثابت LSR *>(t1.اشاره_ګر);
    ثابت LSR *lsr2 = static_cast<ثابت LSR *>(t2.اشاره_ګر);
    أرجع *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ثابت LSR &lsr, int32_t i, int32_t supp_اوږدتیا,
                                   UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع supp_اوږدتیا; }
    إذا (!uhash_لري_کلی(supported_Lsr_منزل_شاخص, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_منزل_شاخص, const_cast<LSR *>(&lsr), i, &error_کوډ);
        إذا (U_SUCCESS(error_کوډ)) {
            supportedLSRs[supp_اوږدتیا] = &lsr;
            supportedIndexes[supp_اوږدتیا++] = i;
        }
    }
    أرجع supp_اوږدتیا;
}

LocaleMatcher::LocaleMatcher(ثابت Builder &builder, UError_کوډ &error_کوډ) :
        likelySubtags(*LikelySubtags::getSingleton(error_کوډ)),
        locale_واټن(*Locale_واټن::getSingleton(error_کوډ)),
        پوله_واټن_2(builder.پوله_واټن),
        demotion_فی_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(تش_اشاره), lsrs(تش_اشاره), supported_Locales_اوږدتیا(0),
        supported_Lsr_منزل_شاخص(تش_اشاره),
        supportedLSRs(تش_اشاره), supportedIndexes(تش_اشاره), supported_LSRs_اوږدتیا(0),
        ownedDefaultLocale(تش_اشاره), defaultLocale(تش_اشاره) {
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    ثابت Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ثابت LSR *defLSR = تش_اشاره;
    إذا (def != تش_اشاره) {
        ownedDefaultLocale = def->clone();
        إذا (ownedDefaultLocale == تش_اشاره) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_اوږدتیا = builder.supportedLocales_ != تش_اشاره ?
        builder.supportedLocales_->size() : 0;
    إذا (supported_Locales_اوږدتیا > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ثابت Locale **>(
            uprv_malloc(supported_Locales_اوږدتیا * حجم(ثابت Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = جديد LSR[supported_Locales_اوږدتیا];
        إذا (supportedLocales == تش_اشاره || lsrs == تش_اشاره) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_اوږدتیا * حجم(ثابت Locale *));
        لكل (int32_t i = 0; i < supported_Locales_اوږدتیا; ++i) {
            ثابت Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            إذا (supportedLocales[i] == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع;
            }
            ثابت Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_کوډ);
            lsr.سېټ_Hash_کوډ();
            إذا (U_FAILURE(error_کوډ)) { أرجع; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_منزل_شاخص = uhash_open_اندازه(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_اوږدتیا, &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        supportedLSRs = static_cast<ثابت LSR **>(
            uprv_malloc(supported_Locales_اوږدتیا * حجم(ثابت LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_اوږدتیا * حجم(int32_t)));
        إذا (supportedLSRs == تش_اشاره || supportedIndexes == تش_اشاره) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }
        int32_t supp_اوږدتیا = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_ډېری_Array<int8_t, 100> ترتیب(supported_Locales_اوږدتیا, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        int32_t عدد_Paradigms = 0;
        لكل (int32_t i = 0; i < supported_Locales_اوږدتیا; ++i) {
            ثابت Locale &locale = *supportedLocales[i];
            ثابت LSR &lsr = lsrs[i];
            إذا (defLSR == تش_اشاره && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                ترتیب[i] = 1;
                supp_اوږدتیا = putIfAbsent(lsr, 0, supp_اوږدتیا, error_کوډ);
            } وإلا إذا (defLSR != تش_اشاره && lsr.is_Equivalent_منزل(*defLSR)) {
                ترتیب[i] = 1;
                supp_اوږدتیا = putIfAbsent(lsr, i, supp_اوږدتیا, error_کوډ);
            } وإلا إذا (locale_واټن.isParadigmLSR(lsr)) {
                ترتیب[i] = 2;
                ++عدد_Paradigms;
            } وإلا {
                ترتیب[i] = 3;
            }
            إذا (U_FAILURE(error_کوډ)) { أرجع; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_حد = supp_اوږدتیا + عدد_Paradigms;
        لكل (int32_t i = 0; i < supported_Locales_اوږدتیا && supp_اوږدتیا < paradigm_حد; ++i) {
            إذا (ترتیب[i] == 2) {
                supp_اوږدتیا = putIfAbsent(lsrs[i], i, supp_اوږدتیا, error_کوډ);
            }
        }
        // Add remaining supported locales.
        لكل (int32_t i = 0; i < supported_Locales_اوږدتیا; ++i) {
            إذا (ترتیب[i] == 3) {
                supp_اوږدتیا = putIfAbsent(lsrs[i], i, supp_اوږدتیا, error_کوډ);
            }
        }
        supported_LSRs_اوږدتیا = supp_اوږدتیا;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    إذا (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_فی_Desired_Locale = locale_واټن.get_Default_Demotion_فی_Desired_Locale();
    }

    إذا (پوله_واټن_2 >= 0) {
        // already copied
    } وإلا إذا (builder.max_واټن_Desired != تش_اشاره) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_واټن_Supported, error_کوډ);
        ثابت LSR *pSuppLSR = &suppLSR;
        int32_t شاخص_And_واټن = locale_واټن.get_غوره_شاخص_And_واټن(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_واټن_Desired, error_کوډ),
                &pSuppLSR, 1,
                Locale_واټن::ښويدنه_واټن(100), favorSubtag, direction);
        إذا (U_SUCCESS(error_کوډ)) {
            // +1 for an exclusive threshold from an inclusive max.
            پوله_واټن_2 = Locale_واټن::get_واټن_Floor(شاخص_And_واټن) + 1;
        } وإلا {
            پوله_واټن_2 = 0;
        }
    } وإلا {
        پوله_واټن_2 = locale_واټن.get_Default_Script_واټن();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_واټن(src.locale_واټن),
        پوله_واټن_2(src.پوله_واټن_2),
        demotion_فی_Desired_Locale(src.demotion_فی_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_اوږدتیا(src.supported_Locales_اوږدتیا),
        supported_Lsr_منزل_شاخص(src.supported_Lsr_منزل_شاخص),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_اوږدتیا(src.supported_LSRs_اوږدتیا),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = تش_اشاره;
    src.lsrs = تش_اشاره;
    src.supported_Locales_اوږدتیا = 0;
    src.supported_Lsr_منزل_شاخص = تش_اشاره;
    src.supportedLSRs = تش_اشاره;
    src.supportedIndexes = تش_اشاره;
    src.supported_LSRs_اوږدتیا = 0;
    src.ownedDefaultLocale = تش_اشاره;
    src.defaultLocale = تش_اشاره;
}

LocaleMatcher::~LocaleMatcher() {
    لكل (int32_t i = 0; i < supported_Locales_اوږدتیا; ++i) {
        ړنګول supportedLocales[i];
    }
    uprv_free(supportedLocales);
    ړنګول[] lsrs;
    uhash_close(supported_Lsr_منزل_شاخص);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    ړنګول ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::چلوونکی=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    هذا->~LocaleMatcher();

    پوله_واټن_2 = src.پوله_واټن_2;
    demotion_فی_Desired_Locale = src.demotion_فی_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_اوږدتیا = src.supported_Locales_اوږدتیا;
    supported_Lsr_منزل_شاخص = src.supported_Lsr_منزل_شاخص;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_اوږدتیا = src.supported_LSRs_اوږدتیا;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = تش_اشاره;
    src.lsrs = تش_اشاره;
    src.supported_Locales_اوږدتیا = 0;
    src.supported_Lsr_منزل_شاخص = تش_اشاره;
    src.supportedLSRs = تش_اشاره;
    src.supportedIndexes = تش_اشاره;
    src.supported_LSRs_اوږدتیا = 0;
    src.ownedDefaultLocale = تش_اشاره;
    src.defaultLocale = تش_اشاره;
    أرجع *هذا;
}

صنف LocaleLsrIterator {
عام:
    LocaleLsrIterator(ثابت LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_سمون_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        إذا (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            ړنګول remembered;
        }
    }

    منطقي has_بل() ثابت {
        أرجع locales.has_بل();
    }

    LSR next(UError_کوډ &error_کوډ) {
        current = &locales.next();
        أرجع getMaximalLsrOrUnd(likelySubtags, *current, error_کوډ);
    }

    فراغ rememberCurrent(int32_t desired_شاخص, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        غوره_Desired_شاخص = desired_شاخص;
        إذا (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } وإلا {
            // ULOCMATCH_TEMPORARY_LOCALES
            ړنګول remembered;
            remembered = جديد Locale(*current);
            إذا (remembered == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
            }
        }
    }

    ثابت Locale *orphanRemembered() {
        ثابت Locale *rem = remembered;
        remembered = تش_اشاره;
        أرجع rem;
    }

    int32_t get_غوره_Desired_شاخص() ثابت {
        أرجع غوره_Desired_شاخص;
    }

خاص:
    ثابت LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_سمون_Lifetime lifetime;
    ثابت Locale *current = تش_اشاره, *remembered = تش_اشاره;
    int32_t غوره_Desired_شاخص = -1;
};

ثابت Locale* LocaleMatcher::get_غوره_سمون(ثابت Locale& desiredLocale,
                                          UError_کوډ& error_کوډ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    std::optional<int32_t> supp_شاخص = get_غوره_Supp_شاخص(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کوډ),
        تش_اشاره, error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && supp_شاخص.has_value() ? supportedLocales[*supp_شاخص]
                                                         : defaultLocale;
}

ثابت Locale* LocaleMatcher::get_غوره_سمون(Locale::Iterator& desiredLocales,
                                          UError_کوډ& error_کوډ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    إذا (!desiredLocales.has_بل()) {
        أرجع defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_شاخص = get_غوره_Supp_شاخص(lsrIter.next(error_کوډ), &lsrIter, error_کوډ);
    أرجع U_SUCCESS(error_کوډ) && supp_شاخص.has_value() ? supportedLocales[*supp_شاخص]
                                                         : defaultLocale;
}

ثابت Locale*
LocaleMatcher::get_غوره_سمون_For_List_String(StringPiece desiredLocaleList,
                                         UError_کوډ& error_کوډ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    Locale_لومړیتوب_List list(desiredLocaleList, error_کوډ);
    Locale_لومړیتوب_List::Iterator iter = list.iterator();
    أرجع get_غوره_سمون(iter, error_کوډ);
}

LocaleMatcher::پایله_2
LocaleMatcher::get_غوره_سمون_پایله(ثابت Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_کوډ& error_کوډ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ)) {
        أرجع پایله_2(تش_اشاره, defaultLocale, -1, -1, خطأ);
    }
    std::optional<int32_t> supp_شاخص = get_غوره_Supp_شاخص(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_کوډ),
        تش_اشاره, error_کوډ);
    إذا (U_FAILURE(error_کوډ) || !supp_شاخص.has_value()) {
        أرجع پایله_2(تش_اشاره, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع پایله_2(&desiredLocale, supportedLocales[*supp_شاخص], 0, *supp_شاخص, خطأ);
    }
}

LocaleMatcher::پایله_2
LocaleMatcher::get_غوره_سمون_پایله(Locale::Iterator& desiredLocales,
                                  UError_کوډ& error_کوډ) ثابت U_LIFETIME_BOUND {
    إذا (U_FAILURE(error_کوډ) || !desiredLocales.has_بل()) {
        أرجع پایله_2(تش_اشاره, defaultLocale, -1, -1, خطأ);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_شاخص = get_غوره_Supp_شاخص(lsrIter.next(error_کوډ), &lsrIter, error_کوډ);
    إذا (U_FAILURE(error_کوډ) || !supp_شاخص.has_value()) {
        أرجع پایله_2(تش_اشاره, defaultLocale, -1, -1, خطأ);
    } وإلا {
        أرجع پایله_2(lsrIter.orphanRemembered(), supportedLocales[*supp_شاخص],
                      lsrIter.get_غوره_Desired_شاخص(), *supp_شاخص, ریښتیا);
    }
}

std::optional<int32_t> LocaleMatcher::get_غوره_Supp_شاخص(LSR desiredLSR,
                                                       LocaleLsrIterator *پاتې_شوی_Iter,
                                                       UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع std::nullopt; }
    int32_t desired_شاخص = 0;
    int32_t غوره_Supported_Lsr_شاخص = -1;
    لكل (int32_t غوره_Shifted_واټن = Locale_واټن::ښويدنه_واټن(پوله_واټن_2);;) {
        // Quick check for exact maximized LSR.
        إذا (supported_Lsr_منزل_شاخص != تش_اشاره) {
            desiredLSR.سېټ_Hash_کوډ();
            UBool وموندل = خطأ;
            int32_t supp_شاخص = uhash_geti_And_وموندل(supported_Lsr_منزل_شاخص, &desiredLSR, &وموندل);
            إذا (وموندل) {
                إذا (پاتې_شوی_Iter != تش_اشاره) {
                    پاتې_شوی_Iter->rememberCurrent(desired_شاخص, error_کوډ);
                }
                أرجع supp_شاخص;
            }
        }
        int32_t غوره_شاخص_And_واټن = locale_واټن.get_غوره_شاخص_And_واټن(
                desiredLSR, supportedLSRs, supported_LSRs_اوږدتیا,
                غوره_Shifted_واټن, favorSubtag, direction);
        إذا (غوره_شاخص_And_واټن >= 0) {
            غوره_Shifted_واټن = Locale_واټن::get_Shifted_واټن(غوره_شاخص_And_واټن);
            إذا (پاتې_شوی_Iter != تش_اشاره) {
                پاتې_شوی_Iter->rememberCurrent(desired_شاخص, error_کوډ);
                إذا (U_FAILURE(error_کوډ)) { أرجع std::nullopt; }
            }
            غوره_Supported_Lsr_شاخص = Locale_واټن::get_شاخص(غوره_شاخص_And_واټن);
        }
        إذا ((غوره_Shifted_واټن -= Locale_واټن::ښويدنه_واټن(demotion_فی_Desired_Locale)) <= 0) {
            اكسر;
        }
        إذا (پاتې_شوی_Iter == تش_اشاره || !پاتې_شوی_Iter->has_بل()) {
            اكسر;
        }
        desiredLSR = پاتې_شوی_Iter->next(error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع std::nullopt; }
        ++desired_شاخص;
    }
    إذا (غوره_Supported_Lsr_شاخص < 0) {
        // no good match
        أرجع std::nullopt;
    }
    أرجع supportedIndexes[غوره_Supported_Lsr_شاخص];
}

UBool LocaleMatcher::is_سمون(ثابت Locale &desired, ثابت Locale &supported,
                             UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t شاخص_And_واټن = locale_واټن.get_غوره_شاخص_And_واټن(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کوډ),
            &pSuppLSR, 1,
            Locale_واټن::ښويدنه_واټن(پوله_واټن_2), favorSubtag, direction);
    أرجع شاخص_And_واټن >= 0;
}

مزدوج LocaleMatcher::دننی_سمون(ثابت Locale &desired, ثابت Locale &supported, UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع 0.; }
    ثابت LSR *pSuppLSR = &suppLSR;
    int32_t شاخص_And_واټن = locale_واټن.get_غوره_شاخص_And_واټن(
            getMaximalLsrOrUnd(likelySubtags, desired, error_کوډ),
            &pSuppLSR, 1,
            Locale_واټن::ښويدنه_واټن(پوله_واټن_2), favorSubtag, direction);
    مزدوج واټن = Locale_واټن::get_واټن_Double(شاخص_And_واټن);
    أرجع (100.0 - واټن) / 100.0;
}

U_NAMESPACE_پای_ټکی

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

نطاق {

صنف Locale_From_نښان {
عام:
    Locale_From_نښان() : locale(Locale::get_ریښه()) {}
    ثابت Locale& چلوونکی()(ثابت حرف* نښان) U_LIFETIME_BOUND { أرجع locale = Locale(نښان); }

خاص:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       حرف *dest, int32_t capacity, UAccept_پایله *accept_پایله,
                       UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع 0; }
    LocaleMatcher::Builder builder;
    ثابت حرف *locString;
    طالما ((locString = uenum_بل(&supportedLocales, تش_اشاره, &error_کوډ)) != تش_اشاره) {
        Locale loc(locString);
        إذا (loc.isBogus()) {
            error_کوډ = U_ILLEGAL_دلیل_ERROR;
            أرجع 0;
        }
        builder.جمع_کړه_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_کوډ);
    LocaleMatcher::پایله_2 پایله = matcher.get_غوره_سمون_پایله(desiredLocales, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع 0; }
    إذا (پایله.get_Desired_شاخص() >= 0) {
        إذا (accept_پایله != تش_اشاره) {
            *accept_پایله = *پایله.getDesiredLocale() == *پایله.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ثابت حرف *غوره_Str = پایله.getSupportedLocale()->getName();
        int32_t غوره_اوږدتیا = static_cast<int32_t>(uprv_strlen(غوره_Str));
        إذا (غوره_اوږدتیا <= capacity) {
            uprv_memcpy(dest, غوره_Str, غوره_اوږدتیا);
        }
        أرجع u_terminateChars(dest, capacity, غوره_اوږدتیا, &error_کوډ);
    } وإلا {
        إذا (accept_پایله != تش_اشاره) {
            *accept_پایله = ULOC_ACCEPT_FAILED;
        }
        أرجع u_terminateChars(dest, capacity, 0, &error_کوډ);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(حرف *پایله, int32_t پایله_شته,
                    UAccept_پایله *بهر_پایله,
                    ثابت حرف **acceptList, int32_t accept_List_شمېر,
                    UEnumeration *شته_Locales,
                    UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((پایله == تش_اشاره ? پایله_شته != 0 : پایله_شته < 0) ||
            (acceptList == تش_اشاره ? accept_List_شمېر != 0 : accept_List_شمېر < 0) ||
            شته_Locales == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    Locale_From_نښان converter;
    Locale::ConvertingIterator<ثابت حرف **, Locale_From_نښان> desiredLocales(
        acceptList, acceptList + accept_List_شمېر, converter);
    أرجع acceptLanguage(*شته_Locales, desiredLocales,
                          پایله, پایله_شته, بهر_پایله, *حالت);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(حرف *پایله, int32_t پایله_شته,
                            UAccept_پایله *بهر_پایله,
                            ثابت حرف *httpAcceptLanguage,
                            UEnumeration *شته_Locales,
                            UError_کوډ *حالت) {
    إذا (U_FAILURE(*حالت)) { أرجع 0; }
    إذا ((پایله == تش_اشاره ? پایله_شته != 0 : پایله_شته < 0) ||
            httpAcceptLanguage == تش_اشاره || شته_Locales == تش_اشاره) {
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }
    Locale_لومړیتوب_List list(httpAcceptLanguage, *حالت);
    Locale_لومړیتوب_List::Iterator desiredLocales = list.iterator();
    أرجع acceptLanguage(*شته_Locales, desiredLocales,
                          پایله, پایله_شته, بهر_پایله, *حالت);
}
