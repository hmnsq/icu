// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// localematcher.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव <optional>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/localebuilder.h"
#अन्तर्भाव "unicode/localematcher.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uloc.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "localeprioritylist.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "locdistance.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "uvector.h"

#परिभाषय UND_LSR LSR("und", "", "", LSR::EXPLICIT_LSR)

/**
 * Indicator for the lifetime of desired-locale objects passed into the LocaleMatcher.
 *
 * @draft ICU 65
 */
गणना ULoc_मेलनम्_Lifetime {
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
#यदि_अपरिभाषितम् U_अन्तर्_DOXYGEN
प्रकारनिर्देश गणना ULoc_मेलनम्_Lifetime ULoc_मेलनम्_Lifetime;
#यदि_अन्तः

U_NAMESPACE_BEGIN

LocaleMatcher::परिणाम_2::परिणाम_2(LocaleMatcher::परिणाम_2 &&src) noexcept :
        desiredLocale(src.desiredLocale),
        supportedLocale(src.supportedLocale),
        desired_सूचकांक(src.desired_सूचकांक),
        supported_सूचकांक(src.supported_सूचकांक),
        desiredIsOwned(src.desiredIsOwned) {
    यदि (desiredIsOwned) {
        src.desiredLocale = शून्यसूचकः;
        src.desired_सूचकांक = -1;
        src.desiredIsOwned = असत्य;
    }
}

LocaleMatcher::परिणाम_2::~परिणाम_2() {
    यदि (desiredIsOwned) {
        विलोपय desiredLocale;
    }
}

LocaleMatcher::परिणाम_2&
LocaleMatcher::परिणाम_2::संक्रियकः=(LocaleMatcher::परिणाम_2&& src) noexcept U_LIFETIME_BOUND {
    एतत्->~परिणाम_2();

    desiredLocale = src.desiredLocale;
    supportedLocale = src.supportedLocale;
    desired_सूचकांक = src.desired_सूचकांक;
    supported_सूचकांक = src.supported_सूचकांक;
    desiredIsOwned = src.desiredIsOwned;

    यदि (desiredIsOwned) {
        src.desiredLocale = शून्यसूचकः;
        src.desired_सूचकांक = -1;
        src.desiredIsOwned = असत्य;
    }
    प्रत्यागम *एतत्;
}

Locale LocaleMatcher::परिणाम_2::makeResolvedLocale(UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता) || supportedLocale == शून्यसूचकः) {
        प्रत्यागम Locale::get_मूलम्();
    }
    ध्रुव Locale *bestDesired = getDesiredLocale();
    यदि (bestDesired == शून्यसूचकः || *supportedLocale == *bestDesired) {
        प्रत्यागम *supportedLocale;
    }
    LocaleBuilder b;
    b.समुच्चयः_Locale(*supportedLocale);

    // Copy the region from bestDesired, if there is one.
    ध्रुव वर्ण *region = bestDesired->getCountry();
    यदि (*region != 0) {
        b.समुच्चयः_Region(region);
    }

    // Copy the variants from bestDesired, if there are any.
    // Note that this will override any supportedLocale variants.
    // For example, "sco-ulster-fonipa" + "...-fonupa" => "sco-fonupa" (replacing ulster).
    ध्रुव वर्ण *variants = bestDesired->getVariant();
    यदि (*variants != 0) {
        b.समुच्चयः_Variant(variants);
    }

    // Copy the extensions from bestDesired, if there are any.
    // C++ note: The following note, copied from Java, may not be true,
    // as long as C++ copies by legacy ICU keyword, not by extension singleton.
    // Note that this will override any supportedLocale extensions.
    // For example, "th-u-nu-latn-ca-buddhist" + "...-u-nu-native" => "th-u-nu-native"
    // (replacing calendar).
    b.copyExtensionsFrom(*bestDesired, error_संहिता);
    प्रत्यागम b.build(error_संहिता);
}

LocaleMatcher::Builder::Builder(LocaleMatcher::Builder &&src) noexcept :
        error_संहिता_2(src.error_संहिता_2),
        supportedLocales_(src.supportedLocales_),
        देहली_Distance(src.देहली_Distance),
        demotion_(src.demotion_),
        defaultLocale_(src.defaultLocale_),
        withDefault_(src.withDefault_),
        favor_(src.favor_),
        direction_(src.direction_) {
    src.supportedLocales_ = शून्यसूचकः;
    src.defaultLocale_ = शून्यसूचकः;
}

LocaleMatcher::Builder::~Builder() {
    विलोपय supportedLocales_;
    विलोपय defaultLocale_;
    विलोपय maxDistanceDesired_;
    विलोपय maxDistanceSupported_;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::संक्रियकः=(LocaleMatcher::Builder&& src) noexcept U_LIFETIME_BOUND {
    एतत्->~Builder();

    error_संहिता_2 = src.error_संहिता_2;
    supportedLocales_ = src.supportedLocales_;
    देहली_Distance = src.देहली_Distance;
    demotion_ = src.demotion_;
    defaultLocale_ = src.defaultLocale_;
    withDefault_ = src.withDefault_,
    favor_ = src.favor_;
    direction_ = src.direction_;

    src.supportedLocales_ = शून्यसूचकः;
    src.defaultLocale_ = शून्यसूचकः;
    प्रत्यागम *एतत्;
}

शून्य LocaleMatcher::Builder::clearSupportedLocales() {
    यदि (supportedLocales_ != शून्यसूचकः) {
        supportedLocales_->removeAllElements();
    }
}

तार्किक LocaleMatcher::Builder::ensure_Supported_Locale_सदिशम्() {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम असत्य; }
    यदि (supportedLocales_ != शून्यसूचकः) { प्रत्यागम सत्य; }
    LocalPointer<UVector> lpSupportedLocales(नूतन UVector(uprv_deleteUObject, शून्यसूचकः, error_संहिता_2), error_संहिता_2);
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम असत्य; }
    supportedLocales_ = lpSupportedLocales.orphan();
    प्रत्यागम सत्य;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Supported_Locales_From_List_String(StringPiece locales) U_LIFETIME_BOUND {
    Locale_प्राथमिकता_List list(locales, error_संहिता_2);
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    clearSupportedLocales();
    यदि (!ensure_Supported_Locale_सदिशम्()) { प्रत्यागम *एतत्; }
    int32_t length = list.get_दीर्घता_Including_Removed();
    प्रति (int32_t i = 0; i < length; ++i) {
        Locale *locale = list.orphanLocaleAt(i);
        यदि (locale == शून्यसूचकः) { अनुवर्त; }
        supportedLocales_->adoptElement(locale, error_संहिता_2);
        यदि (U_FAILURE(error_संहिता_2)) {
            भङ्ग;
        }
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Supported_Locales(Locale::Iterator& locales) U_LIFETIME_BOUND {
    यदि (ensure_Supported_Locale_सदिशम्()) {
        clearSupportedLocales();
        यावत् (locales.has_अग्र() && U_SUCCESS(error_संहिता_2)) {
            ध्रुव Locale &locale = locales.next();
            LocalPointer<Locale> clone (locale.clone(), error_संहिता_2);
            supportedLocales_->adoptElement(clone.orphan(), error_संहिता_2);
        }
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::सङ्कलय_Supported_Locale(ध्रुव Locale& locale) U_LIFETIME_BOUND {
    यदि (ensure_Supported_Locale_सदिशम्()) {
        LocalPointer<Locale> clone(locale.clone(), error_संहिता_2);
        supportedLocales_->adoptElement(clone.orphan(), error_संहिता_2);
    }
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder& LocaleMatcher::Builder::समुच्चयः_No_Default_Locale() U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    विलोपय defaultLocale_;
    defaultLocale_ = शून्यसूचकः;
    withDefault_ = असत्य;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Default_Locale(ध्रुव Locale* defaultLocale) U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    Locale *clone = शून्यसूचकः;
    यदि (defaultLocale != शून्यसूचकः) {
        clone = defaultLocale->clone();
        यदि (clone == शून्यसूचकः) {
            error_संहिता_2 = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
    }
    विलोपय defaultLocale_;
    defaultLocale_ = clone;
    withDefault_ = सत्य;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Favor_Subtag(ULoc_मेलनम्_Favor_Subtag subtag) U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    favor_ = subtag;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Demotion_प्रत्येकम्_Desired_Locale(ULoc_मेलनम्_Demotion demotion) U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    demotion_ = demotion;
    प्रत्यागम *एतत्;
}

LocaleMatcher::Builder&
LocaleMatcher::Builder::समुच्चयः_Max_Distance(ध्रुव Locale& desired,
                                       ध्रुव Locale& supported) U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    Locale *desiredClone = desired.clone();
    Locale *supportedClone = supported.clone();
    यदि (desiredClone == शून्यसूचकः || supportedClone == शून्यसूचकः) {
        विलोपय desiredClone;  // in case only one could not be allocated
        विलोपय supportedClone;
        error_संहिता_2 = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम *एतत्;
    }
    विलोपय maxDistanceDesired_;
    विलोपय maxDistanceSupported_;
    maxDistanceDesired_ = desiredClone;
    maxDistanceSupported_ = supportedClone;
    प्रत्यागम *एतत्;
}

#यदि 0
/**
 * <i>Internal only!</i>
 *
 * @param thresholdDistance the thresholdDistance to set, with -1 = default
 * @return this Builder object
 * @internal
 * @deprecated This API is ICU internal only.
 */
@Deprecated
LocaleMatcher::Builder &LocaleMatcher::Builder::आन्तरिकम्_समुच्चयः_देहली_Distance(int32_t देहली_Distance_2) {
    यदि (U_FAILURE(error_संहिता_2)) { प्रत्यागम *एतत्; }
    यदि (देहली_Distance_2 > 100) {
        देहली_Distance_2 = 100;
    }
    देहली_Distance = देहली_Distance_2;
    प्रत्यागम *एतत्;
}
#यदि_अन्तः

UBool LocaleMatcher::Builder::copy_Error_गन्तव्यम्(UError_संहिता &बहिर्गमनम्_Error_संहिता) ध्रुव {
    यदि (U_FAILURE(बहिर्गमनम्_Error_संहिता)) { प्रत्यागम सत्य; }
    यदि (U_SUCCESS(error_संहिता_2)) { प्रत्यागम असत्य; }
    बहिर्गमनम्_Error_संहिता = error_संहिता_2;
    प्रत्यागम सत्य;
}

LocaleMatcher LocaleMatcher::Builder::build(UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_SUCCESS(error_संहिता) && U_FAILURE(error_संहिता_2)) {
        error_संहिता = error_संहिता_2;
    }
    प्रत्यागम LocaleMatcher(*एतत्, error_संहिता);
}

नामाकाश {

LSR getMaximalLsrOrUnd(ध्रुव LikelySubtags &likelySubtags, ध्रुव Locale &locale,
                       UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता) || locale.isBogus() || *locale.getName() == 0 /* "und" */) {
        प्रत्यागम UND_LSR;
    } अन्यथा {
        प्रत्यागम likelySubtags.makeMaximizedLsrFrom(locale, असत्य, error_संहिता);
    }
}

int32_t hashLSR(ध्रुव UHashTok token) {
    ध्रुव LSR *lsr = static_cast<ध्रुव LSR *>(token.pointer);
    प्रत्यागम lsr->hash_संहिता;
}

UBool compareLSRs(ध्रुव UHashTok t1, ध्रुव UHashTok t2) {
    ध्रुव LSR *lsr1 = static_cast<ध्रुव LSR *>(t1.pointer);
    ध्रुव LSR *lsr2 = static_cast<ध्रुव LSR *>(t2.pointer);
    प्रत्यागम *lsr1 == *lsr2;
}

}  // namespace

int32_t LocaleMatcher::putIfAbsent(ध्रुव LSR &lsr, int32_t i, int32_t supp_दीर्घता,
                                   UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम supp_दीर्घता; }
    यदि (!uhash_अन्तर्भवति_कुञ्चिका(supported_Lsr_गन्तव्यम्_सूचकांक, &lsr)) {
        uhash_putiAllowZero(supported_Lsr_गन्तव्यम्_सूचकांक, const_cast<LSR *>(&lsr), i, &error_संहिता);
        यदि (U_SUCCESS(error_संहिता)) {
            supportedLSRs[supp_दीर्घता] = &lsr;
            supportedIndexes[supp_दीर्घता++] = i;
        }
    }
    प्रत्यागम supp_दीर्घता;
}

LocaleMatcher::LocaleMatcher(ध्रुव Builder &builder, UError_संहिता &error_संहिता) :
        likelySubtags(*LikelySubtags::getSingleton(error_संहिता)),
        localeDistance(*LocaleDistance::getSingleton(error_संहिता)),
        देहली_Distance_2(builder.देहली_Distance),
        demotion_प्रत्येकम्_Desired_Locale(0),
        favorSubtag(builder.favor_),
        direction(builder.direction_),
        supportedLocales(शून्यसूचकः), lsrs(शून्यसूचकः), supported_Locales_दीर्घता(0),
        supported_Lsr_गन्तव्यम्_सूचकांक(शून्यसूचकः),
        supportedLSRs(शून्यसूचकः), supportedIndexes(शून्यसूचकः), supported_LSRs_दीर्घता(0),
        ownedDefaultLocale(शून्यसूचकः), defaultLocale(शून्यसूचकः) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    ध्रुव Locale *def = builder.defaultLocale_;
    LSR builderDefaultLSR;
    ध्रुव LSR *defLSR = शून्यसूचकः;
    यदि (def != शून्यसूचकः) {
        ownedDefaultLocale = def->clone();
        यदि (ownedDefaultLocale == शून्यसूचकः) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        def = ownedDefaultLocale;
        builderDefaultLSR = getMaximalLsrOrUnd(likelySubtags, *def, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        defLSR = &builderDefaultLSR;
    }
    supported_Locales_दीर्घता = builder.supportedLocales_ != शून्यसूचकः ?
        builder.supportedLocales_->size() : 0;
    यदि (supported_Locales_दीर्घता > 0) {
        // Store the supported locales in input order,
        // so that when different types are used (e.g., language tag strings)
        // we can return those by parallel index.
        supportedLocales = static_cast<ध्रुव Locale **>(
            uprv_malloc(supported_Locales_दीर्घता * परिमाण(ध्रुव Locale *)));
        // Supported LRSs in input order.
        // In C++, we store these permanently to simplify ownership management
        // in the hash tables. Duplicate LSRs (if any) are unused overhead.
        lsrs = नूतन LSR[supported_Locales_दीर्घता];
        यदि (supportedLocales == शून्यसूचकः || lsrs == शून्यसूचकः) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        // If the constructor fails partway, we need null pointers for destructibility.
        uprv_memset(supportedLocales, 0, supported_Locales_दीर्घता * परिमाण(ध्रुव Locale *));
        प्रति (int32_t i = 0; i < supported_Locales_दीर्घता; ++i) {
            ध्रुव Locale &locale = *static_cast<Locale *>(builder.supportedLocales_->elementAt(i));
            supportedLocales[i] = locale.clone();
            यदि (supportedLocales[i] == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            ध्रुव Locale &supportedLocale = *supportedLocales[i];
            LSR &lsr = lsrs[i] = getMaximalLsrOrUnd(likelySubtags, supportedLocale, error_संहिता);
            lsr.समुच्चयः_Hash_संहिता();
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        }

        // We need an unordered map from LSR to first supported locale with that LSR,
        // and an ordered list of (LSR, supported index) for
        // the supported locales in the following order:
        // 1. Default locale, if it is supported.
        // 2. Priority locales (aka "paradigm locales") in builder order.
        // 3. Remaining locales in builder order.
        supported_Lsr_गन्तव्यम्_सूचकांक = uhash_open_आकार(hashLSR, compareLSRs, uhash_compareLong,
                                             supported_Locales_दीर्घता, &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        supportedLSRs = static_cast<ध्रुव LSR **>(
            uprv_malloc(supported_Locales_दीर्घता * परिमाण(ध्रुव LSR *)));
        supportedIndexes = static_cast<int32_t *>(
            uprv_malloc(supported_Locales_दीर्घता * परिमाण(int32_t)));
        यदि (supportedLSRs == शून्यसूचकः || supportedIndexes == शून्यसूचकः) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        int32_t supp_दीर्घता = 0;
        // Determine insertion order.
        // Add locales immediately that are equivalent to the default.
        Maybe_राशिः_Array<int8_t, 100> क्रमः(supported_Locales_दीर्घता, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        int32_t अङ्कः_Paradigms = 0;
        प्रति (int32_t i = 0; i < supported_Locales_दीर्घता; ++i) {
            ध्रुव Locale &locale = *supportedLocales[i];
            ध्रुव LSR &lsr = lsrs[i];
            यदि (defLSR == शून्यसूचकः && builder.withDefault_) {
                // Implicit default locale = first supported locale, if not turned off.
                U_ASSERT(i == 0);
                def = &locale;
                defLSR = &lsr;
                क्रमः[i] = 1;
                supp_दीर्घता = putIfAbsent(lsr, 0, supp_दीर्घता, error_संहिता);
            } अन्यथा यदि (defLSR != शून्यसूचकः && lsr.is_Equivalent_गन्तव्यम्(*defLSR)) {
                क्रमः[i] = 1;
                supp_दीर्घता = putIfAbsent(lsr, i, supp_दीर्घता, error_संहिता);
            } अन्यथा यदि (localeDistance.isParadigmLSR(lsr)) {
                क्रमः[i] = 2;
                ++अङ्कः_Paradigms;
            } अन्यथा {
                क्रमः[i] = 3;
            }
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        }
        // Add supported paradigm locales.
        int32_t paradigm_सीमा = supp_दीर्घता + अङ्कः_Paradigms;
        प्रति (int32_t i = 0; i < supported_Locales_दीर्घता && supp_दीर्घता < paradigm_सीमा; ++i) {
            यदि (क्रमः[i] == 2) {
                supp_दीर्घता = putIfAbsent(lsrs[i], i, supp_दीर्घता, error_संहिता);
            }
        }
        // Add remaining supported locales.
        प्रति (int32_t i = 0; i < supported_Locales_दीर्घता; ++i) {
            यदि (क्रमः[i] == 3) {
                supp_दीर्घता = putIfAbsent(lsrs[i], i, supp_दीर्घता, error_संहिता);
            }
        }
        supported_LSRs_दीर्घता = supp_दीर्घता;
        // If supportedLSRsLength < supportedLocalesLength then
        // we waste as many array slots as there are duplicate supported LSRs,
        // but the amount of wasted space is small as long as there are few duplicates.
    }

    defaultLocale = def;

    यदि (builder.demotion_ == ULOCMATCH_DEMOTION_REGION) {
        demotion_प्रत्येकम्_Desired_Locale = localeDistance.get_Default_Demotion_प्रत्येकम्_Desired_Locale();
    }

    यदि (देहली_Distance_2 >= 0) {
        // already copied
    } अन्यथा यदि (builder.maxDistanceDesired_ != शून्यसूचकः) {
        LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceSupported_, error_संहिता);
        ध्रुव LSR *pSuppLSR = &suppLSR;
        int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
                getMaximalLsrOrUnd(likelySubtags, *builder.maxDistanceDesired_, error_संहिता),
                &pSuppLSR, 1,
                LocaleDistance::सरणम्_Distance(100), favorSubtag, direction);
        यदि (U_SUCCESS(error_संहिता)) {
            // +1 for an exclusive threshold from an inclusive max.
            देहली_Distance_2 = LocaleDistance::getDistanceFloor(सूचकांक_And_Distance) + 1;
        } अन्यथा {
            देहली_Distance_2 = 0;
        }
    } अन्यथा {
        देहली_Distance_2 = localeDistance.getDefaultScriptDistance();
    }
}

LocaleMatcher::LocaleMatcher(LocaleMatcher &&src) noexcept :
        likelySubtags(src.likelySubtags),
        localeDistance(src.localeDistance),
        देहली_Distance_2(src.देहली_Distance_2),
        demotion_प्रत्येकम्_Desired_Locale(src.demotion_प्रत्येकम्_Desired_Locale),
        favorSubtag(src.favorSubtag),
        direction(src.direction),
        supportedLocales(src.supportedLocales), lsrs(src.lsrs),
        supported_Locales_दीर्घता(src.supported_Locales_दीर्घता),
        supported_Lsr_गन्तव्यम्_सूचकांक(src.supported_Lsr_गन्तव्यम्_सूचकांक),
        supportedLSRs(src.supportedLSRs),
        supportedIndexes(src.supportedIndexes),
        supported_LSRs_दीर्घता(src.supported_LSRs_दीर्घता),
        ownedDefaultLocale(src.ownedDefaultLocale), defaultLocale(src.defaultLocale) {
    src.supportedLocales = शून्यसूचकः;
    src.lsrs = शून्यसूचकः;
    src.supported_Locales_दीर्घता = 0;
    src.supported_Lsr_गन्तव्यम्_सूचकांक = शून्यसूचकः;
    src.supportedLSRs = शून्यसूचकः;
    src.supportedIndexes = शून्यसूचकः;
    src.supported_LSRs_दीर्घता = 0;
    src.ownedDefaultLocale = शून्यसूचकः;
    src.defaultLocale = शून्यसूचकः;
}

LocaleMatcher::~LocaleMatcher() {
    प्रति (int32_t i = 0; i < supported_Locales_दीर्घता; ++i) {
        विलोपय supportedLocales[i];
    }
    uprv_free(supportedLocales);
    विलोपय[] lsrs;
    uhash_close(supported_Lsr_गन्तव्यम्_सूचकांक);
    uprv_free(supportedLSRs);
    uprv_free(supportedIndexes);
    विलोपय ownedDefaultLocale;
}

LocaleMatcher& LocaleMatcher::संक्रियकः=(LocaleMatcher&& src) noexcept U_LIFETIME_BOUND {
    एतत्->~LocaleMatcher();

    देहली_Distance_2 = src.देहली_Distance_2;
    demotion_प्रत्येकम्_Desired_Locale = src.demotion_प्रत्येकम्_Desired_Locale;
    favorSubtag = src.favorSubtag;
    direction = src.direction;
    supportedLocales = src.supportedLocales;
    lsrs = src.lsrs;
    supported_Locales_दीर्घता = src.supported_Locales_दीर्घता;
    supported_Lsr_गन्तव्यम्_सूचकांक = src.supported_Lsr_गन्तव्यम्_सूचकांक;
    supportedLSRs = src.supportedLSRs;
    supportedIndexes = src.supportedIndexes;
    supported_LSRs_दीर्घता = src.supported_LSRs_दीर्घता;
    ownedDefaultLocale = src.ownedDefaultLocale;
    defaultLocale = src.defaultLocale;

    src.supportedLocales = शून्यसूचकः;
    src.lsrs = शून्यसूचकः;
    src.supported_Locales_दीर्घता = 0;
    src.supported_Lsr_गन्तव्यम्_सूचकांक = शून्यसूचकः;
    src.supportedLSRs = शून्यसूचकः;
    src.supportedIndexes = शून्यसूचकः;
    src.supported_LSRs_दीर्घता = 0;
    src.ownedDefaultLocale = शून्यसूचकः;
    src.defaultLocale = शून्यसूचकः;
    प्रत्यागम *एतत्;
}

वर्ग LocaleLsrIterator {
सार्वजनिक:
    LocaleLsrIterator(ध्रुव LikelySubtags &likelySubtags, Locale::Iterator &locales,
                      ULoc_मेलनम्_Lifetime lifetime) :
            likelySubtags(likelySubtags), locales(locales), lifetime(lifetime) {}

    ~LocaleLsrIterator() {
        यदि (lifetime == ULOCMATCH_TEMPORARY_LOCALES) {
            विलोपय remembered;
        }
    }

    तार्किक has_अग्र() ध्रुव {
        प्रत्यागम locales.has_अग्र();
    }

    LSR next(UError_संहिता &error_संहिता) {
        current = &locales.next();
        प्रत्यागम getMaximalLsrOrUnd(likelySubtags, *current, error_संहिता);
    }

    शून्य rememberCurrent(int32_t desired_सूचकांक, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        best_Desired_सूचकांक = desired_सूचकांक;
        यदि (lifetime == ULOCMATCH_STORED_LOCALES) {
            remembered = current;
        } अन्यथा {
            // ULOCMATCH_TEMPORARY_LOCALES
            विलोपय remembered;
            remembered = नूतन Locale(*current);
            यदि (remembered == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            }
        }
    }

    ध्रुव Locale *orphanRemembered() {
        ध्रुव Locale *rem = remembered;
        remembered = शून्यसूचकः;
        प्रत्यागम rem;
    }

    int32_t get_Best_Desired_सूचकांक() ध्रुव {
        प्रत्यागम best_Desired_सूचकांक;
    }

गुप्त:
    ध्रुव LikelySubtags &likelySubtags;
    Locale::Iterator &locales;
    ULoc_मेलनम्_Lifetime lifetime;
    ध्रुव Locale *current = शून्यसूचकः, *remembered = शून्यसूचकः;
    int32_t best_Desired_सूचकांक = -1;
};

ध्रुव Locale* LocaleMatcher::get_Best_मेलनम्(ध्रुव Locale& desiredLocale,
                                          UError_संहिता& error_संहिता) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_संहिता),
        शून्यसूचकः, error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && supp_सूचकांक.has_value() ? supportedLocales[*supp_सूचकांक]
                                                         : defaultLocale;
}

ध्रुव Locale* LocaleMatcher::get_Best_मेलनम्(Locale::Iterator& desiredLocales,
                                          UError_संहिता& error_संहिता) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    यदि (!desiredLocales.has_अग्र()) {
        प्रत्यागम defaultLocale;
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(lsrIter.next(error_संहिता), &lsrIter, error_संहिता);
    प्रत्यागम U_SUCCESS(error_संहिता) && supp_सूचकांक.has_value() ? supportedLocales[*supp_सूचकांक]
                                                         : defaultLocale;
}

ध्रुव Locale*
LocaleMatcher::get_Best_मेलनम्_For_List_String(StringPiece desiredLocaleList,
                                         UError_संहिता& error_संहिता) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    Locale_प्राथमिकता_List list(desiredLocaleList, error_संहिता);
    Locale_प्राथमिकता_List::Iterator iter = list.iterator();
    प्रत्यागम get_Best_मेलनम्(iter, error_संहिता);
}

LocaleMatcher::परिणाम_2
LocaleMatcher::get_Best_मेलनम्_परिणाम(ध्रुव Locale& desiredLocale U_LIFETIME_BOUND,
                                  UError_संहिता& error_संहिता) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता)) {
        प्रत्यागम परिणाम_2(शून्यसूचकः, defaultLocale, -1, -1, असत्य);
    }
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(
        getMaximalLsrOrUnd(likelySubtags, desiredLocale, error_संहिता),
        शून्यसूचकः, error_संहिता);
    यदि (U_FAILURE(error_संहिता) || !supp_सूचकांक.has_value()) {
        प्रत्यागम परिणाम_2(शून्यसूचकः, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        प्रत्यागम परिणाम_2(&desiredLocale, supportedLocales[*supp_सूचकांक], 0, *supp_सूचकांक, असत्य);
    }
}

LocaleMatcher::परिणाम_2
LocaleMatcher::get_Best_मेलनम्_परिणाम(Locale::Iterator& desiredLocales,
                                  UError_संहिता& error_संहिता) ध्रुव U_LIFETIME_BOUND {
    यदि (U_FAILURE(error_संहिता) || !desiredLocales.has_अग्र()) {
        प्रत्यागम परिणाम_2(शून्यसूचकः, defaultLocale, -1, -1, असत्य);
    }
    LocaleLsrIterator lsrIter(likelySubtags, desiredLocales, ULOCMATCH_TEMPORARY_LOCALES);
    std::optional<int32_t> supp_सूचकांक = get_Best_Supp_सूचकांक(lsrIter.next(error_संहिता), &lsrIter, error_संहिता);
    यदि (U_FAILURE(error_संहिता) || !supp_सूचकांक.has_value()) {
        प्रत्यागम परिणाम_2(शून्यसूचकः, defaultLocale, -1, -1, असत्य);
    } अन्यथा {
        प्रत्यागम परिणाम_2(lsrIter.orphanRemembered(), supportedLocales[*supp_सूचकांक],
                      lsrIter.get_Best_Desired_सूचकांक(), *supp_सूचकांक, सत्य);
    }
}

std::optional<int32_t> LocaleMatcher::get_Best_Supp_सूचकांक(LSR desiredLSR,
                                                       LocaleLsrIterator *remainingIter,
                                                       UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम std::nullopt; }
    int32_t desired_सूचकांक = 0;
    int32_t best_Supported_Lsr_सूचकांक = -1;
    प्रति (int32_t bestShiftedDistance = LocaleDistance::सरणम्_Distance(देहली_Distance_2);;) {
        // Quick check for exact maximized LSR.
        यदि (supported_Lsr_गन्तव्यम्_सूचकांक != शून्यसूचकः) {
            desiredLSR.समुच्चयः_Hash_संहिता();
            UBool प्राप्तम् = असत्य;
            int32_t supp_सूचकांक = uhash_geti_And_प्राप्तम्(supported_Lsr_गन्तव्यम्_सूचकांक, &desiredLSR, &प्राप्तम्);
            यदि (प्राप्तम्) {
                यदि (remainingIter != शून्यसूचकः) {
                    remainingIter->rememberCurrent(desired_सूचकांक, error_संहिता);
                }
                प्रत्यागम supp_सूचकांक;
            }
        }
        int32_t best_सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
                desiredLSR, supportedLSRs, supported_LSRs_दीर्घता,
                bestShiftedDistance, favorSubtag, direction);
        यदि (best_सूचकांक_And_Distance >= 0) {
            bestShiftedDistance = LocaleDistance::getShiftedDistance(best_सूचकांक_And_Distance);
            यदि (remainingIter != शून्यसूचकः) {
                remainingIter->rememberCurrent(desired_सूचकांक, error_संहिता);
                यदि (U_FAILURE(error_संहिता)) { प्रत्यागम std::nullopt; }
            }
            best_Supported_Lsr_सूचकांक = LocaleDistance::get_सूचकांक(best_सूचकांक_And_Distance);
        }
        यदि ((bestShiftedDistance -= LocaleDistance::सरणम्_Distance(demotion_प्रत्येकम्_Desired_Locale)) <= 0) {
            भङ्ग;
        }
        यदि (remainingIter == शून्यसूचकः || !remainingIter->has_अग्र()) {
            भङ्ग;
        }
        desiredLSR = remainingIter->next(error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम std::nullopt; }
        ++desired_सूचकांक;
    }
    यदि (best_Supported_Lsr_सूचकांक < 0) {
        // no good match
        प्रत्यागम std::nullopt;
    }
    प्रत्यागम supportedIndexes[best_Supported_Lsr_सूचकांक];
}

UBool LocaleMatcher::is_मेलनम्(ध्रुव Locale &desired, ध्रुव Locale &supported,
                             UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, error_संहिता),
            &pSuppLSR, 1,
            LocaleDistance::सरणम्_Distance(देहली_Distance_2), favorSubtag, direction);
    प्रत्यागम सूचकांक_And_Distance >= 0;
}

द्विगुण LocaleMatcher::आन्तरिकम्_मेलनम्(ध्रुव Locale &desired, ध्रुव Locale &supported, UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम 0.; }
    // Returns the inverse of the distance: That is, 1-distance(desired, supported).
    LSR suppLSR = getMaximalLsrOrUnd(likelySubtags, supported, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम 0.; }
    ध्रुव LSR *pSuppLSR = &suppLSR;
    int32_t सूचकांक_And_Distance = localeDistance.get_Best_सूचकांक_And_Distance(
            getMaximalLsrOrUnd(likelySubtags, desired, error_संहिता),
            &pSuppLSR, 1,
            LocaleDistance::सरणम्_Distance(देहली_Distance_2), favorSubtag, direction);
    द्विगुण distance = LocaleDistance::getDistanceDouble(सूचकांक_And_Distance);
    प्रत्यागम (100.0 - distance) / 100.0;
}

U_NAMESPACE_अन्तः

// uloc_acceptLanguage() --------------------------------------------------- ***

U_NAMESPACE_USE

नामाकाश {

वर्ग Locale_From_चिह्नकम् {
सार्वजनिक:
    Locale_From_चिह्नकम्() : locale(Locale::get_मूलम्()) {}
    ध्रुव Locale& संक्रियकः()(ध्रुव वर्ण* चिह्नकम्) U_LIFETIME_BOUND { प्रत्यागम locale = Locale(चिह्नकम्); }

गुप्त:
    // Store the locale in the converter, rather than return a reference to a temporary,
    // or a value which could go out of scope with the caller's reference to it.
    Locale locale;
};

int32_t acceptLanguage(UEnumeration &supportedLocales, Locale::Iterator &desiredLocales,
                       वर्ण *dest, int32_t capacity, UAccept_परिणाम *accept_परिणाम,
                       UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम 0; }
    LocaleMatcher::Builder builder;
    ध्रुव वर्ण *locString;
    यावत् ((locString = uenum_अग्र(&supportedLocales, शून्यसूचकः, &error_संहिता)) != शून्यसूचकः) {
        Locale loc(locString);
        यदि (loc.isBogus()) {
            error_संहिता = U_ILLEGAL_तर्कः_ERROR;
            प्रत्यागम 0;
        }
        builder.सङ्कलय_Supported_Locale(loc);
    }
    LocaleMatcher matcher = builder.build(error_संहिता);
    LocaleMatcher::परिणाम_2 परिणाम = matcher.get_Best_मेलनम्_परिणाम(desiredLocales, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम 0; }
    यदि (परिणाम.get_Desired_सूचकांक() >= 0) {
        यदि (accept_परिणाम != शून्यसूचकः) {
            *accept_परिणाम = *परिणाम.getDesiredLocale() == *परिणाम.getSupportedLocale() ?
                ULOC_ACCEPT_VALID : ULOC_ACCEPT_FALLBACK;
        }
        ध्रुव वर्ण *bestStr = परिणाम.getSupportedLocale()->getName();
        int32_t best_दीर्घता = static_cast<int32_t>(uprv_strlen(bestStr));
        यदि (best_दीर्घता <= capacity) {
            uprv_memcpy(dest, bestStr, best_दीर्घता);
        }
        प्रत्यागम u_terminateChars(dest, capacity, best_दीर्घता, &error_संहिता);
    } अन्यथा {
        यदि (accept_परिणाम != शून्यसूचकः) {
            *accept_परिणाम = ULOC_ACCEPT_FAILED;
        }
        प्रत्यागम u_terminateChars(dest, capacity, 0, &error_संहिता);
    }
}

}  // namespace

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguage(वर्ण *परिणाम, int32_t परिणाम_उपलब्धम्,
                    UAccept_परिणाम *बहिर्गमनम्_परिणाम,
                    ध्रुव वर्ण **acceptList, int32_t accept_List_गणनम्,
                    UEnumeration *उपलब्धम्_Locales,
                    UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम 0; }
    यदि ((परिणाम == शून्यसूचकः ? परिणाम_उपलब्धम् != 0 : परिणाम_उपलब्धम् < 0) ||
            (acceptList == शून्यसूचकः ? accept_List_गणनम् != 0 : accept_List_गणनम् < 0) ||
            उपलब्धम्_Locales == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    Locale_From_चिह्नकम् converter;
    Locale::ConvertingIterator<ध्रुव वर्ण **, Locale_From_चिह्नकम्> desiredLocales(
        acceptList, acceptList + accept_List_गणनम्, converter);
    प्रत्यागम acceptLanguage(*उपलब्धम्_Locales, desiredLocales,
                          परिणाम, परिणाम_उपलब्धम्, बहिर्गमनम्_परिणाम, *स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uloc_acceptLanguageFromHTTP(वर्ण *परिणाम, int32_t परिणाम_उपलब्धम्,
                            UAccept_परिणाम *बहिर्गमनम्_परिणाम,
                            ध्रुव वर्ण *httpAcceptLanguage,
                            UEnumeration *उपलब्धम्_Locales,
                            UError_संहिता *स्थितिः) {
    यदि (U_FAILURE(*स्थितिः)) { प्रत्यागम 0; }
    यदि ((परिणाम == शून्यसूचकः ? परिणाम_उपलब्धम् != 0 : परिणाम_उपलब्धम् < 0) ||
            httpAcceptLanguage == शून्यसूचकः || उपलब्धम्_Locales == शून्यसूचकः) {
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }
    Locale_प्राथमिकता_List list(httpAcceptLanguage, *स्थितिः);
    Locale_प्राथमिकता_List::Iterator desiredLocales = list.iterator();
    प्रत्यागम acceptLanguage(*उपलब्धम्_Locales, desiredLocales,
                          परिणाम, परिणाम_उपलब्धम्, बहिर्गमनम्_परिणाम, *स्थितिः);
}
