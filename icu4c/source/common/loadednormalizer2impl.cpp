// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* loadednormalizer2impl.cpp
*
* created on: 2014sep03
* created by: Markus W. Scherer
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/udata.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/unistr.h"
#شامل "unicode/unorm.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "norm2allmodes.h"
#شامل "normalizer2impl.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

جماعت LoadedNormalizer2Impl : عوامی Normalizer2Impl {
عوامی:
    LoadedNormalizer2Impl() : حافظہ(خالی_اشاریہ), ownedTrie(خالی_اشاریہ) {}
    مجازی ~LoadedNormalizer2Impl();

    خالی load(ثابت حرف *packageName, ثابت حرف *name, UError_رمز &error_رمز);

نجی:
    ساکن UBool U_CALLCONV
    isAcceptable(خالی *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);

    UData_حافظہ *حافظہ;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(حافظہ);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(خالی * /*context*/,
                                    ثابت حرف * /* type */, ثابت حرف * /*name*/,
                                    ثابت UDataInfo *pInfo) {
    اگر(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ڈیٹا_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->ڈیٹا_Format[1]==0x72 &&
        pInfo->ڈیٹا_Format[2]==0x6d &&
        pInfo->ڈیٹا_Format[3]==0x32 &&
        pInfo->format_نسخہ[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        واپس سچ;
    } ورنہ {
        واپس جھوٹ;
    }
}

خالی
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    حافظہ=udata_openChoice(packageName, "nrm", name, isAcceptable, یہ, &error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }
    ثابت uint8_t* اندر_بائٹیں = static_cast<ثابت uint8_t*>(udata_get_حافظہ(حافظہ));
    ثابت int32_t* اندر_Indexes = reinterpret_cast<ثابت int32_t*>(اندر_بائٹیں);
    int32_t indexes_لمبائی=اندر_Indexes[IX_NORM_TRIE_انحراف]/4;
    اگر(indexes_لمبائی<=IX_MIN_LCCC_CP) {
        error_رمز=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        واپس;
    }

    int32_t انحراف=اندر_Indexes[IX_NORM_TRIE_انحراف];
    int32_t اگلا_انحراف=اندر_Indexes[IX_EXTRA_ڈیٹا_انحراف];
    ownedTrie=ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٹ_16,
                                     اندر_بائٹیں+انحراف, اگلا_انحراف-انحراف, خالی_اشاریہ,
                                     &error_رمز);
    اگر(U_FAILURE(error_رمز)) {
        واپس;
    }

    انحراف=اگلا_انحراف;
    اگلا_انحراف=اندر_Indexes[IX_SMALL_FCD_انحراف];
    ثابت uint16_t* اندر_Extra_ڈیٹا = reinterpret_cast<ثابت uint16_t*>(اندر_بائٹیں + انحراف);

    // smallFCD: new in formatVersion 2
    انحراف=اگلا_انحراف;
    ثابت uint8_t *اندر_Small_FCD=اندر_بائٹیں+انحراف;

    init(اندر_Indexes, ownedTrie, اندر_Extra_ڈیٹا, اندر_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    LoadedNormalizer2Impl *impl=نیا LoadedNormalizer2Impl;
    اگر(impl==خالی_اشاریہ) {
        error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }
    impl->load(packageName, name, error_رمز);
    واپس createInstance(impl, error_رمز);
}

U_CDECL_BEGIN
ساکن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_انتہا

#اگر !NORM2_HARDCODE_NFC_ڈیٹا
ساکن Norm2AllModes *nfcSingleton;
ساکن icu::UInitOnce nfcInitOnce {};
#اگر_ختم

ساکن Norm2AllModes *nfkcSingleton;
ساکن icu::UInitOnce nfkcInitOnce {};

ساکن Norm2AllModes *nfkc_cfSingleton;
ساکن icu::UInitOnce nfkc_cfInitOnce {};

ساکن Norm2AllModes *nfkc_scfSingleton;
ساکن icu::UInitOnce nfkc_scfInitOnce {};

ساکن UHashtable    *cache=خالی_اشاریہ;

// UInitOnce singleton initialization function
ساکن خالی U_CALLCONV initSingletons(ثابت حرف *what, UError_رمز &error_رمز) {
#اگر !NORM2_HARDCODE_NFC_ڈیٹا
    اگر (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(خالی_اشاریہ, "nfc", error_رمز);
    } ورنہ
#اگر_ختم
    اگر (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(خالی_اشاریہ, "nfkc", error_رمز);
    } ورنہ اگر (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(خالی_اشاریہ, "nfkc_cf", error_رمز);
    } ورنہ اگر (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(خالی_اشاریہ, "nfkc_scf", error_رمز);
    } ورنہ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساکن خالی U_CALLCONV deleteNorm2AllModes(خالی *allModes) {
    حذف (Norm2AllModes *)allModes;
}

ساکن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#اگر !NORM2_HARDCODE_NFC_ڈیٹا
    حذف nfcSingleton;
    nfcSingleton = خالی_اشاریہ;
    nfcInitOnce.reset();
#اگر_ختم

    حذف nfkcSingleton;
    nfkcSingleton = خالی_اشاریہ;
    nfkcInitOnce.reset();

    حذف nfkc_cfSingleton;
    nfkc_cfSingleton = خالی_اشاریہ;
    nfkc_cfInitOnce.reset();

    حذف nfkc_scfSingleton;
    nfkc_scfSingleton = خالی_اشاریہ;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=خالی_اشاریہ;
    واپس سچ;
}

U_CDECL_انتہا

#اگر !NORM2_HARDCODE_NFC_ڈیٹا
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_رمز);
    واپس nfcSingleton;
}
#اگر_ختم

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_رمز);
    واپس nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_رمز);
    واپس nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_رمز);
    واپس nfkc_scfSingleton;
}

#اگر !NORM2_HARDCODE_NFC_ڈیٹا
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->comp : خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->decomp : خالی_اشاریہ;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->fcd : خالی_اشاریہ;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->fcc : خالی_اشاریہ;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? allModes->impl : خالی_اشاریہ;
}
#اگر_ختم

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->comp : خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->decomp : خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->comp : خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? &allModes->comp : خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2_منوال منوال,
                         UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    اگر(name==خالی_اشاریہ || *name==0) {
        error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    ثابت Norm2AllModes *allModes=خالی_اشاریہ;
    اگر(packageName==خالی_اشاریہ) {
        اگر(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_رمز);
        } ورنہ اگر(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_رمز);
        } ورنہ اگر(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_رمز);
        } ورنہ اگر(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_رمز);
        }
    }
    اگر(allModes==خالی_اشاریہ && U_SUCCESS(error_رمز)) {
        {
            Mutex تالا;
            اگر(cache!=خالی_اشاریہ) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        اگر(allModes==خالی_اشاریہ) {
            ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_اشارہ_گر<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_رمز));
            اگر(U_SUCCESS(error_رمز)) {
                Mutex تالا;
                اگر(cache==خالی_اشاریہ) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, خالی_اشاریہ, &error_رمز);
                    اگر(U_FAILURE(error_رمز)) {
                        واپس خالی_اشاریہ;
                    }
                    uhash_سیٹ_کلید_Deleter(cache, uprv_free);
                    uhash_سیٹ_قدر_Deleter(cache, deleteNorm2AllModes);
                }
                خالی *عارضی=uhash_get(cache, name);
                اگر(عارضی==خالی_اشاریہ) {
                    int32_t کلید_لمبائی= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(کلید_لمبائی));
                    اگر(nameCopy==خالی_اشاریہ) {
                        error_رمز=U_حافظہ_تخصیص_ERROR;
                        واپس خالی_اشاریہ;
                    }
                    uprv_memcpy(nameCopy, name, کلید_لمبائی);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_رمز);
                } ورنہ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(عارضی);
                }
            }
        }
    }
    اگر(allModes!=خالی_اشاریہ && U_SUCCESS(error_رمز)) {
        انتخاب(منوال) {
        حالت UNORM2_COMPOSE:
            واپس &allModes->comp;
        حالت UNORM2_DECOMPOSE:
            واپس &allModes->decomp;
        حالت UNORM2_FCD:
            واپس &allModes->fcd;
        حالت UNORM2_COMPOSE_CONTIGUOUS:
            واپس &allModes->fcc;
        طےشدہ:
            توڑو;  // do nothing
        }
    }
    واپس خالی_اشاریہ;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_منوال منوال, UError_رمز &error_رمز) {
    اگر(U_FAILURE(error_رمز)) {
        واپس خالی_اشاریہ;
    }
    انتخاب(منوال) {
    حالت UNORM_NFD:
        واپس Normalizer2::getNFDInstance(error_رمز);
    حالت UNORM_NFKD:
        واپس Normalizer2::getNFKDInstance(error_رمز);
    حالت UNORM_NFC:
        واپس Normalizer2::getNFCInstance(error_رمز);
    حالت UNORM_NFKC:
        واپس Normalizer2::getNFKCInstance(error_رمز);
    حالت UNORM_FCD:
        واپس getFCDInstance(error_رمز);
    طےشدہ:  // UNORM_NONE
        واپس getNoopInstance(error_رمز);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? allModes->impl : خالی_اشاریہ;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_رمز &error_رمز) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_رمز);
    واپس allModes!=خالی_اشاریہ ? allModes->impl : خالی_اشاریہ;
}

U_NAMESPACE_انتہا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_رمز *p_Error_رمز) {
    واپس (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_رمز);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_رمز *p_Error_رمز) {
    واپس (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_رمز);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_رمز *p_Error_رمز) {
    واپس (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_رمز);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_رمز *p_Error_رمز) {
    واپس (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_رمز);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2_منوال منوال,
                   UError_رمز *p_Error_رمز) {
    واپس (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, منوال, *p_Error_رمز);
}

U_CFUNC UNormalization_Check_نتیجہ
unorm_get_تیز_Check(UChar32 c, UNormalization_منوال منوال) {
    اگر(منوال<=UNORM_NONE || UNORM_FCD<=منوال) {
        واپس UNORM_YES;
    }
    UError_رمز error_رمز=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(منوال, error_رمز);
    اگر(U_SUCCESS(error_رمز)) {
        واپس ((ثابت Normalizer2WithImpl *)norm2)->get_تیز_Check(c);
    } ورنہ {
        واپس UNORM_MAYBE;
    }
}

#اگر_ختم  // !UCONFIG_NO_NORMALIZATION
