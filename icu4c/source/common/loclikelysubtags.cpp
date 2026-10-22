// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ضمن <utility>
#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/uscript.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "loclikelysubtags.h"
#ضمن "lsr.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"
#ضمن "uinvchar.h"
#ضمن "umutex.h"
#ضمن "uniquecharstr.h"
#ضمن "uresdata.h"
#ضمن "uresimp.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

نطاق {

constexpr حرف PSEUDO_ACCENTS_پیشوند = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_پیشوند = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_پیشوند = ',';  // -XC, -PSCRACK

}  // namespace

Locale_فاصله_داده::Locale_فاصله_داده(Locale_فاصله_داده &&داده) :
        فاصله_Trie_بایتها(داده.فاصله_Trie_بایتها),
        region_مقصد_Partitions(داده.region_مقصد_Partitions),
        partitions(داده.partitions),
        paradigms(داده.paradigms), paradigms_درازی(داده.paradigms_درازی),
        distances(داده.distances) {
    داده.partitions = اشاره_تهی;
    داده.paradigms = اشاره_تهی;
}

Locale_فاصله_داده::~Locale_فاصله_داده() {
    uprv_free(partitions);
    حذف[] paradigms;
}

بنية Likely_Subtags_داده {
    UResourceBundle *langInfoBundle = اشاره_تهی;
    Unique_Char_رشتهها رشتهها;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trie_بایتها = اشاره_تهی;
    LSR *lsrs = اشاره_تهی;
    int32_t lsrs_درازی = 0;

    Locale_فاصله_داده فاصله_داده;

    Likely_Subtags_داده(UError_کد &error_کد) : رشتهها(error_کد) {}

    ~Likely_Subtags_داده() {
        ures_close(langInfoBundle);
        حذف[] lsrs;
    }

    فراغ load(UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع; }
        langInfoBundle = ures_open_مستقیم(اشاره_تهی, "langInfo", &error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }
        پشته_UResource_Bundle پشته_موقت_Bundle;
        Resource_داده_مقدار value;
        ures_get_مقدار_With_Fallback(langInfoBundle, "likely", پشته_موقت_Bundle.getAlias(),
                                  value, error_کد);
        Resource_جدول likely_جدول = value.get_جدول(error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_حافظه<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_درازی = 0, regions_درازی = 0, lsr_Subtags_درازی = 0;
        ResourceArray m49Array;
        إذا (likely_جدول.پیدا_کن_مقدار("m49", value)) {
            m49Array = value.getArray(error_کد);
        } وإلا {
            error_کد = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        إذا (!read_رشتهها(likely_جدول, "languageAliases", value,
                         languageIndexes, languages_درازی, error_کد) ||
                !read_رشتهها(likely_جدول, "regionAliases", value,
                             regionIndexes, regions_درازی, error_کد) ||
                !read_LSREncoded_رشتهها(likely_جدول, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_درازی, error_کد)) {
            أرجع;
        }
        إذا ((languages_درازی & 1) != 0 ||
                (regions_درازی & 1) != 0 ||
                (lsr_Subtags_درازی % 3) != 0) {
            error_کد = U_INVALID_FORMAT_ERROR;
            أرجع;
        }
        إذا (lsr_Subtags_درازی == 0) {
            error_کد = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }

        إذا (!likely_جدول.پیدا_کن_مقدار("trie", value)) {
            error_کد = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        int32_t length;
        trie_بایتها = value.get_دودویی(length, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_کد تطابق_Error_کد = U_ZERO_ERROR;
        ures_get_مقدار_With_Fallback(langInfoBundle, "match", پشته_موقت_Bundle.getAlias(),
                                  value, تطابق_Error_کد);
        Local_حافظه<int32_t> افراز_Indexes, paradigmSubtagIndexes;
        int32_t partitions_درازی = 0, paradigm_Subtags_درازی = 0;
        إذا (U_SUCCESS(تطابق_Error_کد)) {
            Resource_جدول تطابق_جدول = value.get_جدول(error_کد);
            إذا (U_FAILURE(error_کد)) { أرجع; }

            إذا (تطابق_جدول.پیدا_کن_مقدار("trie", value)) {
                فاصله_داده.فاصله_Trie_بایتها = value.get_دودویی(length, error_کد);
                إذا (U_FAILURE(error_کد)) { أرجع; }
            }

            إذا (تطابق_جدول.پیدا_کن_مقدار("regionToPartitions", value)) {
                فاصله_داده.region_مقصد_Partitions = value.get_دودویی(length, error_کد);
                إذا (U_FAILURE(error_کد)) { أرجع; }
                إذا (length < LSR::REGION_شاخص_حد) {
                    error_کد = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }

            إذا (!read_رشتهها(تطابق_جدول, "partitions", value,
                             افراز_Indexes, partitions_درازی, error_کد) ||
                    !read_LSREncoded_رشتهها(تطابق_جدول, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_درازی, error_کد)) {
                أرجع;
            }
            إذا ((paradigm_Subtags_درازی % 3) != 0) {
                error_کد = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            إذا (تطابق_جدول.پیدا_کن_مقدار("distances", value)) {
                فاصله_داده.distances = value.get_Int_وکتور(length, error_کد);
                إذا (U_FAILURE(error_کد)) { أرجع; }
                إذا (length < 4) {  // LocaleDistance IX_LIMIT
                    error_کد = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }
        } وإلا إذا (تطابق_Error_کد == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وإلا {  // error other than missing resource
            error_کد = تطابق_Error_کد;
            أرجع;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        رشتهها.freeze();

        languageAliases = CharStringMap(languages_درازی / 2, error_کد);
        لكل (int32_t i = 0; i < languages_درازی; i += 2) {
            languageAliases.put(رشتهها.get(languageIndexes[i]),
                                رشتهها.get(languageIndexes[i + 1]), error_کد);
        }

        regionAliases = CharStringMap(regions_درازی / 2, error_کد);
        لكل (int32_t i = 0; i < regions_درازی; i += 2) {
            regionAliases.put(رشتهها.get(regionIndexes[i]),
                              رشتهها.get(regionIndexes[i + 1]), error_کد);
        }
        إذا (U_FAILURE(error_کد)) { أرجع; }

        lsrs_درازی = lsr_Subtags_درازی / 3;
        lsrs = جديد LSR[lsrs_درازی];
        إذا (lsrs == اشاره_تهی) {
            error_کد = U_حافظه_تخصیص_ERROR;
            أرجع;
        }
        لكل (int32_t i = 0, j = 0; i < lsr_Subtags_درازی; i += 3, ++j) {
            lsrs[j] = LSR(رشتهها.get(lsrSubtagIndexes[i]),
                          رشتهها.get(lsrSubtagIndexes[i + 1]),
                          رشتهها.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        إذا (partitions_درازی > 0) {
            فاصله_داده.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitions_درازی * حجم(ثابت حرف *)));
            إذا (فاصله_داده.partitions == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0; i < partitions_درازی; ++i) {
                فاصله_داده.partitions[i] = رشتهها.get(افراز_Indexes[i]);
            }
        }

        إذا (paradigm_Subtags_درازی > 0) {
            فاصله_داده.paradigms_درازی = paradigm_Subtags_درازی / 3;
            LSR *paradigms = جديد LSR[فاصله_داده.paradigms_درازی];
            إذا (paradigms == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0, j = 0; i < paradigm_Subtags_درازی; i += 3, ++j) {
                paradigms[j] = LSR(رشتهها.get(paradigmSubtagIndexes[i]),
                                   رشتهها.get(paradigmSubtagIndexes[i + 1]),
                                   رشتهها.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            فاصله_داده.paradigms = paradigms;
        }
    }

خاص:
    منطقي read_رشتهها(ثابت Resource_جدول &جدول, ثابت حرف *کلید, Resource_مقدار &value,
                     Local_حافظه<int32_t> &indexes, int32_t &length, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
        إذا (جدول.پیدا_کن_مقدار(کلید, value)) {
            ResourceArray stringArray = value.getArray(error_کد);
            إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
            length = stringArray.get_اندازه();
            إذا (length == 0) { أرجع درست; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            إذا (rawIndexes == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                إذا (stringArray.get_مقدار(i, value)) {  // returns true because i < length
                    int32_t str_درازی = 0;
                    rawIndexes[i] = رشتهها.جمع_کن(value.getString(str_درازی, error_کد), error_کد);
                    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
                }
            }
        }
        أرجع درست;
    }
    UnicodeString مقصد_Language(صحيح encoded) {
        إذا (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        حرف lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        إذا (encoded / (27 * 27) == 0) {
            أرجع UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        أرجع UnicodeString(lang, 3, US_INV);
    }
    UnicodeString مقصد_Script(صحيح encoded) {
        إذا (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ثابت حرف* script = uscript_getShortName(static_cast<UScript_کد>(encoded));
        إذا (script == اشاره_تهی) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        أرجع UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_شاخص_مقصد_کد(ثابت ResourceArray &m49Array, Resource_مقدار &value, صحيح شاخص, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (m49Array.get_مقدار(شاخص, value)) {
            أرجع value.getUnicodeString(error_کد);
        }
        // "m49" does not include the index.
        error_کد = U_MISSING_RESOURCE_ERROR;
        أرجع UNICODE_STRING_SIMPLE("");
    }

    UnicodeString مقصد_Region(ثابت ResourceArray& m49Array, Resource_مقدار &value, صحيح encoded, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد) || encoded == 0 || encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        إذا (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            أرجع  m49_شاخص_مقصد_کد(m49Array, value, encoded, error_کد);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        أرجع UnicodeString(region, 2, US_INV);
    }

    منطقي read_LSREncoded_رشتهها(ثابت Resource_جدول &جدول, ثابت حرف* کلید, Resource_مقدار &value, ثابت ResourceArray& m49Array,
                     Local_حافظه<int32_t> &indexes, int32_t &length, UError_کد &error_کد) {
        إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
        إذا (جدول.پیدا_کن_مقدار(کلید, value)) {
            ثابت int32_t* vectors = value.get_Int_وکتور(length, error_کد);
            إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
            إذا (length == 0) { أرجع درست; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            إذا (rawIndexes == اشاره_تهی) {
                error_کد = U_حافظه_تخصیص_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                rawIndexes[i*3] = رشتهها.جمع_کن_By_مقدار(مقصد_Language(vectors[i]), error_کد);
                rawIndexes[i*3+1] = رشتهها.جمع_کن_By_مقدار(مقصد_Script(vectors[i]), error_کد);
                rawIndexes[i*3+2] = رشتهها.جمع_کن_By_مقدار(
                    مقصد_Region(m49Array, value, vectors[i], error_کد), error_کد);
                إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
            }
            length *= 3;
        }
        أرجع درست;
    }
};

نطاق {

LikelySubtags *gLikelySubtags = اشاره_تهی;
UVector *gMacroregions = اشاره_تهی;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    حذف gLikelySubtags;
    gLikelySubtags = اشاره_تهی;
    حذف gMacroregions;
    gMacroregions = اشاره_تهی;
    gInitOnce.reset();
    أرجع درست;
}

constexpr ثابت char16_t* MACROREGION_HARDCODE[] = {
    u"001~3",
    u"005",
    u"009",
    u"011",
    u"013~5",
    u"017~9",
    u"021",
    u"029",
    u"030",
    u"034~5",
    u"039",
    u"053~4",
    u"057",
    u"061",
    u"142~3",
    u"145",
    u"150~1",
    u"154~5",
    u"202",
    u"419",
    u"EU",
    u"EZ",
    u"QO",
    u"UN",
};

constexpr char16_t RANGE_MARKER = 0x7E; /* '~' */
فراغ processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UError_کد& وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع; }
    int32_t rangeMarkerLocation = regionName.شاخص_از(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,وضعیت);
    إذا ( rangeMarkerLocation > 0 ) {
        char16_t انتها_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        طالما ( buf[rangeMarkerLocation-1] <= انتها_Range && U_SUCCESS(وضعیت)) {
            Local_اشاره_گر<UnicodeString> newRegion(جديد UnicodeString(buf), وضعیت);
            newMacroRegions->adoptElement(newRegion.orphan(),وضعیت);
            buf[rangeMarkerLocation-1]++;
        }
    } وإلا {
        Local_اشاره_گر<UnicodeString> newRegion(جديد UnicodeString(regionName), وضعیت);
        newMacroRegions->adoptElement(newRegion.orphan(),وضعیت);
    }
}

#إذا U_DEBUG
UVector* بارگیری_Macroregions(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع اشاره_تهی; }
    Local_اشاره_گر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, وضعیت), وضعیت);

    Local_UResource_Bundle_اشاره_گر supplemental_داده(ures_open_مستقیم(اشاره_تهی,"supplementalData",&وضعیت));
    Local_UResource_Bundle_اشاره_گر idValidity(ures_get_By_کلید(supplemental_داده.getAlias(),"idValidity",اشاره_تهی,&وضعیت));
    Local_UResource_Bundle_اشاره_گر regionList(ures_get_By_کلید(idValidity.getAlias(),"region",اشاره_تهی,&وضعیت));
    Local_UResource_Bundle_اشاره_گر regionMacro(ures_get_By_کلید(regionList.getAlias(),"macroregion",اشاره_تهی,&وضعیت));

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    طالما (ures_has_بعدی(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_بعدی_Unicode_String(regionMacro.getAlias(),اشاره_تهی,&وضعیت);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع اشاره_تهی;
        }
    }

    أرجع newMacroRegions.orphan();
}
#پایان_اگر // U_DEBUG

UVector* getStaticMacroregions(UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) { أرجع اشاره_تهی; }
    Local_اشاره_گر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, وضعیت), وضعیت);

    إذا (U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }

    لكل (ثابت خودکار *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), وضعیت);
        إذا (U_FAILURE(وضعیت)) {
            أرجع اشاره_تهی;
        }
    }

    أرجع newMacroRegions.orphan();
}

}  // namespace

فراغ U_CALLCONV LikelySubtags::initLikelySubtags(UError_کد &error_کد) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == اشاره_تهی);
    Likely_Subtags_داده داده(error_کد);
    داده.load(error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع; }
    gLikelySubtags = جديد LikelySubtags(داده);
    gMacroregions = getStaticMacroregions(error_کد);
#إذا U_DEBUG
    خودکار macroregions_From_داده = بارگیری_Macroregions(error_کد);
    U_ASSERT((*gMacroregions) == (*macroregions_From_داده));
    حذف macroregions_From_داده;
#پایان_اگر
    إذا (U_FAILURE(error_کد) || gLikelySubtags == اشاره_تهی || gMacroregions == اشاره_تهی) {
        حذف gLikelySubtags;
        حذف gMacroregions;
        error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    ucln_common_ثبات_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UError_کد &error_کد) {
    إذا (U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کد);
    أرجع gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_داده &داده) :
        langInfoBundle(داده.langInfoBundle),
        رشتهها(داده.رشتهها.orphan_Char_رشتهها()),
        languageAliases(std::move(داده.languageAliases)),
        regionAliases(std::move(داده.regionAliases)),
        trie(داده.trie_بایتها),
        lsrs(داده.lsrs),
#إذا U_DEBUG
        lsrs_درازی(داده.lsrs_درازی),
#پایان_اگر // U_DEBUG
        فاصله_داده(std::move(داده.فاصله_داده)) {
    داده.langInfoBundle = اشاره_تهی;
    داده.lsrs = اشاره_تهی;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_نتیجه نتیجه = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_بعدی(نتیجه));
    trieUndState = trie.getState64();
    نتیجه = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_بعدی(نتیجه));
    trieUndZzzzState = trie.getState64();
    نتیجه = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_مقدار(نتیجه));
    default_Lsr_شاخص = trie.get_مقدار();
    trie.reset();

    لكل (char16_t c = u'a'; c <= u'z'; ++c) {
        نتیجه = trie.next(c);
        إذا (نتیجه == USTRINGTRIE_NO_مقدار) {
            trie_اولی_Letter_حالتها[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    حذف رشتهها;
    حذف[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقي return_درون_داد_If_Unmatch,
                                         UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    إذا (locale.isBogus()) {
        error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع {};
    }
    ثابت حرف *name = locale.getName();
    إذا (!return_درون_داد_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        أرجع LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_درون_داد_If_Unmatch, error_کد);

    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        أرجع LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_کد);
    }
    أرجع max;
}

نطاق {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    أرجع canonical == اشاره_تهی ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقي return_درون_داد_If_Unmatch,
                                     UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    إذا (!return_درون_داد_If_Unmatch) {
        حرف c1;
        إذا (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            بدل (c1) {
            حالة 'A':
                أرجع LSR(PSEUDO_ACCENTS_پیشوند, language, script, region,
                           LSR::EXPLICIT_LSR, error_کد);
            حالة 'B':
                أرجع LSR(PSEUDO_BIDI_پیشوند, language, script, region,
                           LSR::EXPLICIT_LSR, error_کد);
            حالة 'C':
                أرجع LSR(PSEUDO_CRACKED_پیشوند, language, script, region,
                           LSR::EXPLICIT_LSR, error_کد);
            افتراضي:  // normal locale
                اكسر;
            }
        }

        إذا (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            إذا (uprv_strcmp(variant, "PSACCENT") == 0) {
                أرجع LSR(PSEUDO_ACCENTS_پیشوند, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_کد);
            } وإلا إذا (uprv_strcmp(variant, "PSBIDI") == 0) {
                أرجع LSR(PSEUDO_BIDI_پیشوند, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_کد);
            } وإلا إذا (uprv_strcmp(variant, "PSCRACK") == 0) {
                أرجع LSR(PSEUDO_CRACKED_پیشوند, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_کد);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    أرجع maximize(language, script, region, return_درون_داد_If_Unmatch, error_کد);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقي return_درون_داد_If_Unmatch,
                             UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    أرجع maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_درون_داد_If_Unmatch,
                    error_کد);
}

منطقي LikelySubtags::isMacroregion(StringPiece& region, UError_کد& error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع خطأ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    أرجع gMacroregions->شامل_است((فراغ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقي return_درون_داد_If_Unmatch,
                             UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    إذا (language.compare("und") == 0) {
        language = "";
    }
    إذا (script.compare("Zzzz") == 0) {
        script = "";
    }
    إذا (region.compare("ZZ") == 0) {
        region = "";
    }
    إذا (!script.empty() && !region.empty() && !language.empty()) {
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کد);  // already maximized
    }
    منطقي retainLanguage = خطأ;
    منطقي retainScript = خطأ;
    منطقي retainRegion = خطأ;

    بایتها_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_پایینی_Ordinal(language.داده()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_اولی_Letter_حالتها[c0]) != 0) {
        value = trie_بعدی(iter.reset_مقصد_State64(state), language, 1);
    } وإلا {
        value = trie_بعدی(iter, language, 0);
    }
    منطقي تطابق_Language = (value >= 0);
    منطقي تطابق_Script = خطأ;
    إذا (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وإلا {
        retainLanguage = درست;
        iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value >= 0 && !script.empty()) {
        تطابق_Script = درست;
    }
    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وإلا {
        value = trie_بعدی(iter, script, 0);
        إذا (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وإلا {
            retainScript = درست;
            إذا (state == 0) {
                iter.reset_مقصد_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_مقصد_State64(state);
                value = trie_بعدی(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقي تطابق_Region = خطأ;
    إذا (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وإلا {
        value = trie_بعدی(iter, region, 0);
        إذا (value >= 0) {
            إذا (!region.empty() && !isMacroregion(region, error_کد)) {
                retainRegion = درست;
                تطابق_Region = درست;
            }
        } وإلا {
            retainRegion = درست;
            إذا (state == 0) {
                value = default_Lsr_شاخص;
            } وإلا {
                iter.reset_مقصد_State64(state);
                value = trie_بعدی(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                إذا (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
                    value = trie_بعدی(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_خالی_State = iter.getState64();
                    value = trie_بعدی(iter, region, 0);
                    // Fallback to und =>
                    إذا (value < 0) {
                        iter.reset_مقصد_State64(trie_Und_خالی_State);
                        value = trie_بعدی(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_درازی);
    إذا (return_درون_داد_If_Unmatch &&
        (!(تطابق_Language || تطابق_Script || (تطابق_Region && language.empty())))) {
      أرجع LSR("", "", "", LSR::EXPLICIT_LSR, error_کد);  // no matching.
    }
    إذا (language.empty()) {
        language = StringPiece("und");
    }

    إذا (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ثابت LSR &matched = lsrs[value];
        أرجع LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    إذا (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    إذا (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    إذا (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_ماسک = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    أرجع LSR(language, script, region, retain_ماسک, error_کد);
}

int32_t LikelySubtags::compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    إذا (uprv_strcmp(lsr.language, other.language) != 0) {
        أرجع 0xfffffffc;  // negative, lsr not better than other
    }
    إذا (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t شاخص;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            شاخص = likelyInfo >> 2;
        } وإلا {
            شاخص = get_Likely_شاخص(lsr.language, "");
            likelyInfo = شاخص << 2;
        }
        ثابت LSR &likely = lsrs[شاخص];
        إذا (uprv_strcmp(lsr.script, likely.script) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    إذا (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t شاخص;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            شاخص = likelyInfo >> 2;
        } وإلا {
            شاخص = get_Likely_شاخص(lsr.language, lsr.region);
            likelyInfo = (شاخص << 2) | 2;
        }
        ثابت LSR &likely = lsrs[شاخص];
        إذا (uprv_strcmp(lsr.region, likely.region) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    أرجع likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_شاخص(ثابت حرف *language, ثابت حرف *script) ثابت {
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    إذا (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    بایتها_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_پایینی_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_اولی_Letter_حالتها[c0]) != 0) {
        value = trie_بعدی(iter.reset_مقصد_State64(state), language, 1);
    } وإلا {
        value = trie_بعدی(iter, language, 0);
    }
    إذا (value >= 0) {
        state = iter.getState64();
    } وإلا {
        iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
    } وإلا {
        value = trie_بعدی(iter, script, 0);
        إذا (value >= 0) {
            state = iter.getState64();
        } وإلا {
            إذا (state == 0) {
                iter.reset_مقصد_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_مقصد_State64(state);
                value = trie_بعدی(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    إذا (value > 0) {
        // Final value from just language or language+script.
    } وإلا {
        value = trie_بعدی(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_درازی);
    أرجع value;
}

int32_t LikelySubtags::trie_بعدی(بایتها_Trie &iter, ثابت حرف *s, int32_t i) {
    UString_Trie_نتیجه نتیجه;
    uint8_t c;
    إذا ((c = s[i]) == 0) {
        نتیجه = iter.next(u'*');
    } وإلا {
        لكل (;;) {
            c = uprv_inv_Char_مقصد_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            إذا (next != 0) {
                إذا (!USTRINGTRIE_HAS_بعدی(iter.next(c))) {
                    أرجع -1;
                }
            } وإلا {
                // last character of this subtag
                نتیجه = iter.next(c | 0x80);
                اكسر;
            }
            c = next;
        }
    }
    بدل (نتیجه) {
    حالة USTRINGTRIE_NO_تطابق: أرجع -1;
    حالة USTRINGTRIE_NO_مقدار: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_مقدار:
        U_ASSERT(iter.get_مقدار() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_مقدار: أرجع iter.get_مقدار();
    افتراضي: أرجع -1;
    }
}
int32_t LikelySubtags::trie_بعدی(بایتها_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_نتیجه نتیجه;
    uint8_t c;
    إذا (s.length() == i) {
        نتیجه = iter.next(u'*');
    } وإلا {
        c = s.داده()[i];
        لكل (;;) {
            c = uprv_inv_Char_مقصد_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            إذا (i+1 != s.length()) {
                إذا (!USTRINGTRIE_HAS_بعدی(iter.next(c))) {
                    أرجع -1;
                }
                c = s.داده()[++i];
            } وإلا {
                // last character of this subtag
                نتیجه = iter.next(c | 0x80);
                اكسر;
            }
        }
    }
    بدل (نتیجه) {
    حالة USTRINGTRIE_NO_تطابق: أرجع -1;
    حالة USTRINGTRIE_NO_مقدار: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_مقدار:
        U_ASSERT(iter.get_مقدار() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_مقدار: أرجع iter.get_مقدار();
    افتراضي: أرجع -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقي favorScript,
                                    UError_کد &error_کد) ثابت {
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    LSR max = maximize(language, script, region, درست, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    // If no match, return it.
    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کد);
    }
    // try language
    LSR test = maximize(max.language, "", "", درست, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    إذا (test.is_Equivalent_مقصد(max)) {
        أرجع LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_کد);
    }

    إذا (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, درست, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع {}; }
        إذا (test.is_Equivalent_مقصد(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کد);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", درست, error_کد);
    إذا (U_FAILURE(error_کد)) { أرجع {}; }
    إذا (test.is_Equivalent_مقصد(max)) {
        أرجع LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_کد);
    }
    إذا (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, درست, error_کد);
        إذا (U_FAILURE(error_کد)) { أرجع {}; }
        إذا (test.is_Equivalent_مقصد(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کد);
        }
    }
    أرجع LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_کد);
}

U_NAMESPACE_انتها
