// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#అన్తర్భావ <optional>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/localebuilder.h"
#అన్తర్భావ "unicode/localematcher.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uloc.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "localeprioritylist.h"
#అన్తర్భావ "loclikelysubtags.h"
#అన్తర్భావ "locdistance.h"
#అన్తర్భావ "lsr.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "uvector.h"

#నిర్వచించు UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
గణన ULoc_సరిపోలిక_Lifetime {
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
#నిర్వచితం_కాకపోతే U_లోపల_DOXYGEN
రకనిర్వచనం గణన ULoc_సరిపోలిక_Lifetime ULoc_సరిపోలిక_Lifetime;
#అయితే_ముగింపు

U_NAMESPACE_BEGIN

LocaleMatcher::ఫలితం_2::ఫలితం_2(LocaleMatcher::ఫలితం_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_సూచీ(src.desired_సూచీ),
        supported_సూచీ(src.supported_సూచీ),
        desiredIsOwned(src.desiredIsOwned) {
    ఒకవేళ (desiredIsOwned) {
        src.desiredLocale = శూన్యసూచిక;
        src.desired_సూచీ = -1;
        src.desiredIsOwned = అబద్ధం;
    }
}

LocaleMatcher::ఫలితం_2::~ఫలితం_2() {
    ఒకవేళ (desiredIsOwned) {
        తొలగించు desiredLocale;
    }
}

LocaleMatcher::ఫలితం_2&
LocaleMatcher::ఫలితం_2::క్రియాకారకం=(LocaleMatcher::ఫలితం_2&& src) noexcept U_LIFETIME_BOUND {
    ఇది->~ఫలితం_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_సూచీ = src.desired_సూచీ;
    supported_సూచీ = src.supported_సూచీ;
    desiredIsOwned = src.desiredIsOwned;

    ఒకవేళ (desiredIsOwned) {
        src.desiredLocale = శూన్యసూచిక;
        src.desired_సూచీ = -1;
        src.desiredIsOwned = అబద్ధం;
    }
    తిరిగి *ఇది;
}

Locale LocaleMatcher::ఫలితం_2::makeResolvedLocale(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం) || supportedLocale == శూన్యసూచిక) {
        తిరిగి Locale::get_మూలం();
    }
    స్థిరరాశి Locale *ఉత్తమ_Desired = getDesiredLocale();
    ఒకవేళ (ఉత్తమ_Desired == శూన్యసూచిక || *supportedLocale == *ఉత్తమ_Desired) {
        తిరిగి *supportedLocale;
    }
    LocaleBuilder b;
    b.సమితి_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    స్థిరరాశి అక్షరం *region = ఉత్తమ_Desired->getCountry();
    ఒకవేళ (*region != 0) {
        b.సమితి_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    స్థిరరాశి అక్షరం *variants = ఉత్తమ_Desired->getVariant();
    ఒకవేళ (*variants != 0) {
        b.సమితి_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*ఉత్తమ_Desired, error_సంకేతం);
    తిరిగి b.build(error_సంకేతం);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_సంకేతం_2(src.error_సంకేతం_2),
        supportedLocales_(src.supportedLocales_),
        పరిమితి_దూరం(src.పరిమితి_దూరం),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = శూన్యసూచిక;
    src.defaultLocale_ = శూన్యసూచిక;
}

LocaleMatcher::Builder::~Builder() {
    తొలగించు supportedLocales_;
    తొలగించు defaultLocale_;
    తొలగించు max_దూరం_Desired;
    తొలగించు max_దూరం_Supported;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::క్రియాకారకం=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    ఇది->~Builder();

    error_సంకేతం_2 = src.error_సంకేతం_2;
    supportedLocales_ = src.supportedLocales_;
    పరిమితి_దూరం = src.పరిమితి_దూరం;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = శూన్యసూచిక;
    src.defaultLocale_ = శూన్యసూచిక;
    తిరిగి *ఇది;
}

శూన్య LocaleMatcher::Builder::clearSupportedLocales() {
    ఒకవేళ (supportedLocales_ != శూన్యసూచిక) {
        supportedLocales_->removeAllElements();
    }
}

తార్కిక LocaleMatcher::Builder::ensure_Supported_Locale_సదిశ() {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి అబద్ధం; }
    ఒకవేళ (supportedLocales_ != శూన్యసూచిక) { తిరిగి నిజం; }
    Local_సూచిక<UVector> lpSupportedLocales(కొత్త UVector(uprv_deleteUObject, శూన్యసూచిక, error_సంకేతం_2), error_సంకేతం_2);
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి అబద్ధం; }
    supportedLocales_ = lpSupportedLocales.orphan();
    తిరిగి నిజం;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_ప్రాధాన్యత_List list(locales, error_సంకేతం_2);
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    clearSupportedLocales();
    ఒకవేళ (!ensure_Supported_Locale_సదిశ()) { తిరిగి *ఇది; }
    int32_t length = list.get_నిడివి_Including_Removed();
    కొరకు (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        ఒకవేళ (locale == శూన్యసూచిక) { కొనసాగు; }
        supportedLocales_->adoptElement(locale, error_సంకేతం_2);
        ఒకవేళ (U_FAILURE(error_సంకేతం_2)) {
            ఆపు;
        }
    }
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    ఒకవేళ (ensure_Supported_Locale_సదిశ()) {
        clearSupportedLocales();
        అయితే (locales.has_తదుపరి() && U_SUCCESS(error_సంకేతం_2)) {
            స్థిరరాశి Locale &locale = locales.next();
            Local_సూచిక<Locale> clone (locale.clone(), error_సంకేతం_2);
            supportedLocales_->adoptElement(clone.orphan(), error_సంకేతం_2);
        }
    }
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::కూడు_Supported_Locale(స్థిరరాశి Locale& locale) U_LIFETIME_BOUND {
    ఒకవేళ (ensure_Supported_Locale_సదిశ()) {
        Local_సూచిక<Locale> clone(locale.clone(), error_సంకేతం_2);
        supportedLocales_->adoptElement(clone.orphan(), error_సంకేతం_2);
    }
    తిరిగి *ఇది;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::సమితి_No_Default_Locale() U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    తొలగించు defaultLocale_;
    defaultLocale_ = శూన్యసూచిక;
    withDefault_ = అబద్ధం;
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Default_Locale(స్థిరరాశి Locale* defaultLocale) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    Locale *clone = శూన్యసూచిక;
    ఒకవేళ (defaultLocale != శూన్యసూచిక) {
        clone = defaultLocale->clone();
        ఒకవేళ (clone == శూన్యసూచిక) {
            error_సంకేతం_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
    }
    తొలగించు defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = నిజం;
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Favor_Subtag(ULoc_సరిపోలిక_Favor_Subtag subtag) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    favor_ = subtag;
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Demotion_చొప్పున_Desired_Locale(ULoc_సరిపోలిక_Demotion demotion) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    demotion_ = demotion;
    తిరిగి *ఇది;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::సమితి_Max_దూరం(స్థిరరాశి Locale& desired,
                                       స్థిరరాశి Locale& supported) U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    ఒకవేళ (desiredClone == శూన్యసూచిక || supportedClone == శూన్యసూచిక) {
        తొలగించు desiredClone;  // in case only one could not be allocated
        తొలగించు supportedClone;
        error_సంకేతం_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి *ఇది;
    }
    తొలగించు max_దూరం_Desired;
    తొలగించు max_దూరం_Supported;
    max_దూరం_Desired = desiredClone;
    max_దూరం_Supported = supportedClone;
    తిరిగి *ఇది;
}

#ఒకవేళ 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::అంతర్గత_సమితి_పరిమితి_దూరం(int32_t పరిమితి_దూరం_2) {
    ఒకవేళ (U_FAILURE(error_సంకేతం_2)) { తిరిగి *ఇది; }
    ఒకవేళ (పరిమితి_దూరం_2 > 100) {
        పరిమితి_దూరం_2 = 100;
    }
    పరిమితి_దూరం = పరిమితి_దూరం_2;
    తిరిగి *ఇది;
}
#అయితే_ముగింపు

UBool LocaleMatcher::Builder::copy_Error_గమ్యం(UError_సంకేతం &బహిర్గతం_Error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(బహిర్గతం_Error_సంకేతం)) { తిరిగి నిజం; }
    ఒకవేళ (U_SUCCESS(error_సంకేతం_2)) { తిరిగి అబద్ధం; }
    బహిర్గతం_Error_సంకేతం = error_సంకేతం_2;
    తిరిగి నిజం;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_SUCCESS(error_సంకేతం) && U_FAILURE(error_సంకేతం_2)) {
        error_సంకేతం = error_సంకేతం_2;
    }
    తిరిగి LocaleMatcher(*ఇది, error_సంకేతం);
}

నామాకాశ {

LSR getMaximalLsrOrUnd(స్థిరరాశి LikelySubtags &likelySubtags, స్థిరరాశి Locale &locale,
                       UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        తిరిగి UND_LSR;
    } లేకపోతే {
        తిరిగి likelySubtags.makeMaximizedLsrFrom(locale, అబద్ధం, error_సంకేతం);
    }
}

int32_t hashLSR(స్థిరరాశి UHashTok token) {
    స్థిరరాశి LSR *lsr = static_cast<స్థిరరాశి LSR *>(token.సూచిక);
    తిరిగి lsr->hash_సంకేతం;
}

UBool compareLSRs(స్థిరరాశి UHashTok t1, స్థిరరాశి UHashTok t2) {
    స్థిరరాశి LSR *lsr1 = static_cast<స్థిరరాశి LSR *>(t1.సూచిక);
    స్థిరరాశి LSR *lsr2 = static_cast<స్థిరరాశి LSR *>(t2.సూచిక);
    తిరిగి *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(స్థిరరాశి LSR &lsr, int32_t i, int32_t supp_నిడివి,
                                   UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి supp_నిడివి; }
    ఒకవేళ (!uhash_కలిగియుంది_కీలకం(supported_Lsr_గమ్యం_సూచీ, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_గమ్యం_సూచీ, const_cast<LSR *>(&lsr), i, &error_సంకేతం);
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            supportedLSRs[supp_నిడివి] = &lsr;
            supportedIndexes[supp_నిడివి++] = i;
        }
    }
    తిరిగి supp_నిడివి;
}

LocaleMatcher::LocaleMatcher(స్థిరరాశి Builder &builder, UError_సంకేతం &error_సంకేతం) :
        likelySubtags(*LikelySubtags::getSingleton(error_సంకేతం)),
        locale_దూరం(*Locale_దూరం::getSingleton(error_సంకేతం)),
        పరిమితి_దూరం_2(builder.పరిమితి_దూరం),
        demotion_చొప్పున_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(శూన్యసూచిక), lsrs(శూన్యసూచిక), supported_Locales_నిడివి(0),
        supported_Lsr_గమ్యం_సూచీ(శూన్యసూచిక),
        supportedLSRs(శూన్యసూచిక), supportedIndexes(శూన్యసూచిక), supported_LSRs_నిడివి(0),
        ownedDefaultLocale(శూన్యసూచిక), defaultLocale(శూన్యసూచిక) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    స్థిరరాశి Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    స్థిరరాశి LSR *defLSR = శూన్యసూచిక;
    ఒకవేళ (def != శూన్యసూచిక) {
        ownedDefaultLocale = def->clone();
        ఒకవేళ (ownedDefaultLocale == శూన్యసూచిక) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_నిడివి = builder.supportedLocales_ != శూన్యసూచిక ?
        builder.supportedLocales_->size() : 0;
    ఒకవేళ (supported_Locales_నిడివి > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<స్థిరరాశి Locale **>(
            uprv_malloc(supported_Locales_నిడివి * పరిమాణం(స్థిరరాశి Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = కొత్త LSR[supported_Locales_నిడివి];
        ఒకవేళ (supportedLocales == శూన్యసూచిక || lsrs == శూన్యసూచిక) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_నిడివి * పరిమాణం(స్థిరరాశి Locale *));
        కొరకు (int32_t i = 0; i < supported_Locales_నిడివి; ++i) {
            స్థిరరాశి Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            ఒకవేళ (supportedLocales[i] == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            స్థిరరాశి Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_సంకేతం);
            lsr.సమితి_Hash_సంకేతం();
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_గమ్యం_సూచీ = uhash_open_కొలత(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_నిడివి, &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        supportedLSRs = static_cast<స్థిరరాశి LSR **>(
            uprv_malloc(supported_Locales_నిడివి * పరిమాణం(స్థిరరాశి LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_నిడివి * పరిమాణం(int32_t)));
        ఒకవేళ (supportedLSRs == శూన్యసూచిక || supportedIndexes == శూన్యసూచిక) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        int32_t supp_నిడివి = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_దొంతర_Array<int8_t, 100> క్రమం(supported_Locales_నిడివి, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        int32_t అంకె_Paradigms = 0;
        కొరకు (int32_t i = 0; i < supported_Locales_నిడివి; ++i) {
            స్థిరరాశి Locale &locale = *supportedLocales[i];
            స్థిరరాశి LSR &lsr = lsrs[i];
            ఒకవేళ (defLSR == శూన్యసూచిక && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                క్రమం[i] = 1;
                supp_నిడివి = putIfAbsent(lsr, 0, supp_నిడివి, error_సంకేతం);
            } లేకపోతే ఒకవేళ (defLSR != శూన్యసూచిక && lsr.is_Equivalent_గమ్యం(*defLSR)) {
                క్రమం[i] = 1;
                supp_నిడివి = putIfAbsent(lsr, i, supp_నిడివి, error_సంకేతం);
            } లేకపోతే ఒకవేళ (locale_దూరం.isParadigmLSR(lsr)) {
                క్రమం[i] = 2;
                ++అంకె_Paradigms;
            } లేకపోతే {
                క్రమం[i] = 3;
            }
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_హద్దు = supp_నిడివి + అంకె_Paradigms;
        కొరకు (int32_t i = 0; i < supported_Locales_నిడివి && supp_నిడివి < paradigm_హద్దు; ++i) {
            ఒకవేళ (క్రమం[i] == 2) {
                supp_నిడివి = putIfAbsent(lsrs[i], i, supp_నిడివి, error_సంకేతం);
            }
        }
        // Add remaining supported locales.
        కొరకు (int32_t i = 0; i < supported_Locales_నిడివి; ++i) {
            ఒకవేళ (క్రమం[i] == 3) {
                supp_నిడివి = putIfAbsent(lsrs[i], i, supp_నిడివి, error_సంకేతం);
            }
        }
        supported_LSRs_నిడివి = supp_నిడివి;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    ఒకవేళ (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_చొప్పున_Desired_Locale = locale_దూరం.get_Default_Demotion_చొప్పున_Desired_Locale();
    }

    ఒకవేళ (పరిమితి_దూరం_2 >= 0) {
        // already copied
    } లేకపోతే ఒకవేళ (builder.max_దూరం_Desired != శూన్యసూచిక) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.max_దూరం_Supported, error_సంకేతం);
        స్థిరరాశి LSR *pSuppLSR = &suppLSR;
        int32_t సూచీ_And_దూరం = locale_దూరం.get_ఉత్తమ_సూచీ_And_దూరం(
                getMaximalLsrOrUnd(likelySubtags, *builder.max_దూరం_Desired, error_సంకేతం),
                &pSuppLSR, 1,
                Locale_దూరం::జరుపు_దూరం(100), favorSubtag, direction);
        ఒకవేళ (U_SUCCESS(error_సంకేతం)) {
            // +1 for an exclusive threshold from an inclusive max.
            పరిమితి_దూరం_2 = Locale_దూరం::get_దూరం_Floor(సూచీ_And_దూరం) + 1;
        } లేకపోతే {
            పరిమితి_దూరం_2 = 0;
        }
    } లేకపోతే {
        పరిమితి_దూరం_2 = locale_దూరం.get_Default_Script_దూరం();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        locale_దూరం(src.locale_దూరం),
        పరిమితి_దూరం_2(src.పరిమితి_దూరం_2),
        demotion_చొప్పున_Desired_Locale(src.demotion_చొప్పున_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_నిడివి(src.supported_Locales_నిడివి),
        supported_Lsr_గమ్యం_సూచీ(src.supported_Lsr_గమ్యం_సూచీ),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_నిడివి(src.supported_LSRs_నిడివి),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = శూన్యసూచిక;
    src.lsrs = శూన్యసూచిక;
    src.supported_Locales_నిడివి = 0;
    src.supported_Lsr_గమ్యం_సూచీ = శూన్యసూచిక;
    src.supportedLSRs = శూన్యసూచిక;
    src.supportedIndexes = శూన్యసూచిక;
    src.supported_LSRs_నిడివి = 0;
    src.ownedDefaultLocale = శూన్యసూచిక;
    src.defaultLocale = శూన్యసూచిక;
}

LocaleMatcher::~LocaleMatcher() {
    కొరకు (int32_t i = 0; i < supported_Locales_నిడివి; ++i) {
        తొలగించు supportedLocales[i];
    }
    uprv_free(supportedLocales);
    తొలగించు[] lsrs;
    uhash_close(supported_Lsr_గమ్యం_సూచీ);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    తొలగించు ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::క్రియాకారకం=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    ఇది->~LocaleMatcher();

    పరిమితి_దూరం_2 = src.పరిమితి_దూరం_2;
    demotion_చొప్పున_Desired_Locale = src.demotion_చొప్పున_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_నిడివి = src.supported_Locales_నిడివి;
    supported_Lsr_గమ్యం_సూచీ = src.supported_Lsr_గమ్యం_సూచీ;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_నిడివి = src.supported_LSRs_నిడివి;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = శూన్యసూచిక;
    src.lsrs = శూన్యసూచిక;
    src.supported_Locales_నిడివి = 0;
    src.supported_Lsr_గమ్యం_సూచీ = శూన్యసూచిక;
    src.supportedLSRs = శూన్యసూచిక;
    src.supportedIndexes = శూన్యసూచిక;
    src.supported_LSRs_నిడివి = 0;
    src.ownedDefaultLocale = శూన్యసూచిక;
    src.defaultLocale = శూన్యసూచిక;
    తిరిగి *ఇది;
}

తరగతి LocaleLsrIterator {
బహిరంగ:
    LocaleLsrIterator(స్థిరరాశి LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_సరిపోలిక_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        ఒకవేళ (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            తొలగించు remembered;
        }
    }

    తార్కిక has_తదుపరి() స్థిరరాశి {
        తిరిగి locales.has_తదుపరి();
    }

    LSR next(UError_సంకేతం &error_సంకేతం) {
        current = &locales.next();
        తిరిగి getMaximalLsrOrUnd(likelySubtags, *current, error_సంకేతం);
    }

    శూన్య rememberCurrent(int32_t desired_సూచీ, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        ఉత్తమ_Desired_సూచీ = desired_సూచీ;
        ఒకవేళ (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } లేకపోతే {
            // ULOCMATCH_TEMPORARY_LOCALES
            తొలగించు remembered;
            remembered = కొత్త Locale(*current);
            ఒకవేళ (remembered == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            }
        }
    }

    స్థిరరాశి Locale *orphanRemembered() {
        స్థిరరాశి Locale *rem = remembered;
        remembered = శూన్యసూచిక;
        తిరిగి rem;
    }

    int32_t get_ఉత్తమ_Desired_సూచీ() స్థిరరాశి {
        తిరిగి ఉత్తమ_Desired_సూచీ;
    }

అంతరంగ:
    స్థిరరాశి LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_సరిపోలిక_Lifetime lifetime;
    స్థిరరాశి Locale *current = శూన్యసూచిక, *remembered = శూన్యసూచిక;
    int32_t ఉత్తమ_Desired_సూచీ = -1;
};

స్థిరరాశి Locale* LocaleMatcher::get_ఉత్తమ_సరిపోలిక(స్థిరరాశి Locale& desiredLocale,
                                          UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    std::optional<int32_t> supp_సూచీ = get_ఉత్తమ_Supp_సూచీ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_సంకేతం),
        శూన్యసూచిక, error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && supp_సూచీ.has_value() ? supportedLocales[*supp_సూచీ]
                                                         : defaultLocale;
}

స్థిరరాశి Locale* LocaleMatcher::get_ఉత్తమ_సరిపోలిక(Locale::Iterator& desiredLocales,
                                          UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    ఒకవేళ (!desiredLocales.has_తదుపరి()) {
        తిరిగి defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_సూచీ = get_ఉత్తమ_Supp_సూచీ(lsrIter.next(error_సంకేతం), &lsrIter, error_సంకేతం);
    తిరిగి U_SUCCESS(error_సంకేతం) && supp_సూచీ.has_value() ? supportedLocales[*supp_సూచీ]
                                                         : defaultLocale;
}

స్థిరరాశి Locale*
LocaleMatcher::get_ఉత్తమ_సరిపోలిక_For_List_String(StringPiece desiredLocaleList,
                                         UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    Locale_ప్రాధాన్యత_List list(desiredLocaleList, error_సంకేతం);
    Locale_ప్రాధాన్యత_List::Iterator iter = list.iterator();
    తిరిగి get_ఉత్తమ_సరిపోలిక(iter, error_సంకేతం);
}

LocaleMatcher::ఫలితం_2
LocaleMatcher::get_ఉత్తమ_సరిపోలిక_ఫలితం(స్థిరరాశి Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) {
        తిరిగి ఫలితం_2(శూన్యసూచిక, defaultLocale, -1, -1, అబద్ధం);
    }
    std::optional<int32_t> supp_సూచీ = get_ఉత్తమ_Supp_సూచీ(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_సంకేతం),
        శూన్యసూచిక, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం) || !supp_సూచీ.has_value()) {
        తిరిగి ఫలితం_2(శూన్యసూచిక, defaultLocale, -1, -1, అబద్ధం);
    } లేకపోతే {
        తిరిగి ఫలితం_2(&desiredLocale, supportedLocales[*supp_సూచీ], 0, *supp_సూచీ, అబద్ధం);
    }
}

LocaleMatcher::ఫలితం_2
LocaleMatcher::get_ఉత్తమ_సరిపోలిక_ఫలితం(Locale::Iterator& desiredLocales,
                                  UError_సంకేతం& error_సంకేతం) స్థిరరాశి U_LIFETIME_BOUND {
    ఒకవేళ (U_FAILURE(error_సంకేతం) || !desiredLocales.has_తదుపరి()) {
        తిరిగి ఫలితం_2(శూన్యసూచిక, defaultLocale, -1, -1, అబద్ధం);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_సూచీ = get_ఉత్తమ_Supp_సూచీ(lsrIter.next(error_సంకేతం), &lsrIter, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం) || !supp_సూచీ.has_value()) {
        తిరిగి ఫలితం_2(శూన్యసూచిక, defaultLocale, -1, -1, అబద్ధం);
    } లేకపోతే {
        తిరిగి ఫలితం_2(lsrIter.orphanRemembered(), supportedLocales[*supp_సూచీ],
                      lsrIter.get_ఉత్తమ_Desired_సూచీ(), *supp_సూచీ, నిజం);
    }
}

std::optional<int32_t> LocaleMatcher::get_ఉత్తమ_Supp_సూచీ(LSR desiredLSR,
                                                       LocaleLsrIterator *మిగిలిన_Iter,
                                                       UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి std::nullopt; }
    int32_t desired_సూచీ = 0;
    int32_t ఉత్తమ_Supported_Lsr_సూచీ = -1;
    కొరకు (int32_t ఉత్తమ_Shifted_దూరం = Locale_దూరం::జరుపు_దూరం(పరిమితి_దూరం_2);;) {
        // Quick check for exact maximized LSR.
        ఒకవేళ (supported_Lsr_గమ్యం_సూచీ != శూన్యసూచిక) {
            desiredLSR.సమితి_Hash_సంకేతం();
            UBool దొరికింది = అబద్ధం;
            int32_t supp_సూచీ = uhash_geti_And_దొరికింది(supported_Lsr_గమ్యం_సూచీ, &desiredLSR, &దొరికింది);
            ఒకవేళ (దొరికింది) {
                ఒకవేళ (మిగిలిన_Iter != శూన్యసూచిక) {
                    మిగిలిన_Iter->rememberCurrent(desired_సూచీ, error_సంకేతం);
                }
                తిరిగి supp_సూచీ;
            }
        }
        int32_t ఉత్తమ_సూచీ_And_దూరం = locale_దూరం.get_ఉత్తమ_సూచీ_And_దూరం(
                desiredLSR, supportedLSRs, supported_LSRs_నిడివి,
                ఉత్తమ_Shifted_దూరం, favorSubtag, direction);
        ఒకవేళ (ఉత్తమ_సూచీ_And_దూరం >= 0) {
            ఉత్తమ_Shifted_దూరం = Locale_దూరం::get_Shifted_దూరం(ఉత్తమ_సూచీ_And_దూరం);
            ఒకవేళ (మిగిలిన_Iter != శూన్యసూచిక) {
                మిగిలిన_Iter->rememberCurrent(desired_సూచీ, error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి std::nullopt; }
            }
            ఉత్తమ_Supported_Lsr_సూచీ = Locale_దూరం::get_సూచీ(ఉత్తమ_సూచీ_And_దూరం);
        }
        ఒకవేళ ((ఉత్తమ_Shifted_దూరం -= Locale_దూరం::జరుపు_దూరం(demotion_చొప్పున_Desired_Locale)) <= 0) {
            ఆపు;
        }
        ఒకవేళ (మిగిలిన_Iter == శూన్యసూచిక || !మిగిలిన_Iter->has_తదుపరి()) {
            ఆపు;
        }
        desiredLSR = మిగిలిన_Iter->next(error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి std::nullopt; }
        ++desired_సూచీ;
    }
    ఒకవేళ (ఉత్తమ_Supported_Lsr_సూచీ < 0) {
        // no good match
        తిరిగి std::nullopt;
    }
    తిరిగి supportedIndexes[ఉత్తమ_Supported_Lsr_సూచీ];
}

UBool LocaleMatcher::is_సరిపోలిక(స్థిరరాశి Locale &desired, స్థిరరాశి Locale &supported,
                             UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    స్థిరరాశి LSR *pSuppLSR = &suppLSR;
    int32_t సూచీ_And_దూరం = locale_దూరం.get_ఉత్తమ_సూచీ_And_దూరం(
            getMaximalLsrOrUnd(likelySubtags, desired, error_సంకేతం),
            &pSuppLSR, 1,
            Locale_దూరం::జరుపు_దూరం(పరిమితి_దూరం_2), favorSubtag, direction);
    తిరిగి సూచీ_And_దూరం >= 0;
}

ద్విగుణ LocaleMatcher::అంతర్గత_సరిపోలిక(స్థిరరాశి Locale &desired, స్థిరరాశి Locale &supported, UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0.; }
    స్థిరరాశి LSR *pSuppLSR = &suppLSR;
    int32_t సూచీ_And_దూరం = locale_దూరం.get_ఉత్తమ_సూచీ_And_దూరం(
            getMaximalLsrOrUnd(likelySubtags, desired, error_సంకేతం),
            &pSuppLSR, 1,
            Locale_దూరం::జరుపు_దూరం(పరిమితి_దూరం_2), favorSubtag, direction);
    ద్విగుణ దూరం = Locale_దూరం::get_దూరం_Double(సూచీ_And_దూరం);
    తిరిగి (100.0 - దూరం) / 100.0;
}

U_NAMESPACE_చివర

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

నామాకాశ {

తరగతి Locale_From_ట్యాగ్ {
బహిరంగ:
    Locale_From_ట్యాగ్() : locale(Locale::get_మూలం()) {}
    స్థిరరాశి Locale& క్రియాకారకం()(స్థిరరాశి అక్షరం* ట్యాగ్) U_LIFETIME_BOUND { తిరిగి locale = Locale(ట్యాగ్); }

అంతరంగ:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       అక్షరం *dest, int32_t capacity, UAccept_ఫలితం *accept_ఫలితం,
                       UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0; }
    LocaleMatcher::Builder builder;
    స్థిరరాశి అక్షరం *locString;
    అయితే ((locString = uenum_తదుపరి(&supportedLocales, శూన్యసూచిక, &error_సంకేతం)) != శూన్యసూచిక) {
        Locale loc(locString);
        ఒకవేళ (loc.isBogus()) {
            error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
            తిరిగి 0;
        }
        builder.కూడు_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_సంకేతం);
    LocaleMatcher::ఫలితం_2 ఫలితం = matcher.get_ఉత్తమ_సరిపోలిక_ఫలితం(desiredLocales, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి 0; }
    ఒకవేళ (ఫలితం.get_Desired_సూచీ() >= 0) {
        ఒకవేళ (accept_ఫలితం != శూన్యసూచిక) {
            *accept_ఫలితం = *ఫలితం.getDesiredLocale() == *ఫలితం.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        స్థిరరాశి అక్షరం *ఉత్తమ_Str = ఫలితం.getSupportedLocale()->getName();
        int32_t ఉత్తమ_నిడివి = static_cast<int32_t>(uprv_strlen(ఉత్తమ_Str));
        ఒకవేళ (ఉత్తమ_నిడివి <= capacity) {
            uprv_memcpy(dest, ఉత్తమ_Str, ఉత్తమ_నిడివి);
        }
        తిరిగి u_terminateChars(dest, capacity, ఉత్తమ_నిడివి, &error_సంకేతం);
    } లేకపోతే {
        ఒకవేళ (accept_ఫలితం != శూన్యసూచిక) {
            *accept_ఫలితం = ULOC_ACCEPT_FAILED;
        }
        తిరిగి u_terminateChars(dest, capacity, 0, &error_సంకేతం);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(అక్షరం *ఫలితం, int32_t ఫలితం_అందుబాటు,
                    UAccept_ఫలితం *బహిర్గతం_ఫలితం,
                    స్థిరరాశి అక్షరం **acceptList, int32_t accept_List_లెక్క,
                    UEnumeration *అందుబాటు_Locales,
                    UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి 0; }
    ఒకవేళ ((ఫలితం == శూన్యసూచిక ? ఫలితం_అందుబాటు != 0 : ఫలితం_అందుబాటు < 0) ||
            (acceptList == శూన్యసూచిక ? accept_List_లెక్క != 0 : accept_List_లెక్క < 0) ||
            అందుబాటు_Locales == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    Locale_From_ట్యాగ్ converter;
    Locale::ConvertingIterator<స్థిరరాశి అక్షరం **, Locale_From_ట్యాగ్> desiredLocales(
        acceptList, acceptList + accept_List_లెక్క, converter);
    తిరిగి acceptLanguage(*అందుబాటు_Locales, desiredLocales,
                          ఫలితం, ఫలితం_అందుబాటు, బహిర్గతం_ఫలితం, *స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(అక్షరం *ఫలితం, int32_t ఫలితం_అందుబాటు,
                            UAccept_ఫలితం *బహిర్గతం_ఫలితం,
                            స్థిరరాశి అక్షరం *httpAcceptLanguage,
                            UEnumeration *అందుబాటు_Locales,
                            UError_సంకేతం *స్థితి_2) {
    ఒకవేళ (U_FAILURE(*స్థితి_2)) { తిరిగి 0; }
    ఒకవేళ ((ఫలితం == శూన్యసూచిక ? ఫలితం_అందుబాటు != 0 : ఫలితం_అందుబాటు < 0) ||
            httpAcceptLanguage == శూన్యసూచిక || అందుబాటు_Locales == శూన్యసూచిక) {
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    Locale_ప్రాధాన్యత_List list(httpAcceptLanguage, *స్థితి_2);
    Locale_ప్రాధాన్యత_List::Iterator desiredLocales = list.iterator();
    తిరిగి acceptLanguage(*అందుబాటు_Locales, desiredLocales,
                          ఫలితం, ఫలితం_అందుబాటు, బహిర్గతం_ఫలితం, *స్థితి_2);
}
