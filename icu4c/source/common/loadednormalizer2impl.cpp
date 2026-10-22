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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/udata.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/unorm.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "norm2allmodes.h"
#ضمن "normalizer2impl.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

صنف LoadedNormalizer2Impl : عام Normalizer2Impl {
عام:
    LoadedNormalizer2Impl() : حافظه(اشاره_تهی), ownedTrie(اشاره_تهی) {}
    مجازی ~LoadedNormalizer2Impl();

    فراغ load(ثابت حرف *packageName, ثابت حرف *name, UError_کد &error_کد);

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);

    UData_حافظه *حافظه;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(حافظه);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(فراغ * /*context*/,
                                    ثابت حرف * /* type */, ثابت حرف * /*name*/,
                                    ثابت UDataInfo *pInfo) {
    إذا(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->داده_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->داده_Format[1]==0x72 &&
        pInfo->داده_Format[2]==0x6d &&
        pInfo->داده_Format[3]==0x32 &&
        pInfo->format_نسخه[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        أرجع درست;
    } وإلا {
        أرجع خطأ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    حافظه=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    ثابت uint8_t* درون_بایتها = static_cast<ثابت uint8_t*>(udata_get_حافظه(حافظه));
    ثابت int32_t* درون_Indexes = reinterpret_cast<ثابت int32_t*>(درون_بایتها);
    int32_t indexes_درازی=درون_Indexes[IX_NORM_TRIE_جابه_جایی]/4;
    إذا(indexes_درازی<=IX_MIN_LCCC_CP) {
        error_کد=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t جابه_جایی=درون_Indexes[IX_NORM_TRIE_جابه_جایی];
    int32_t بعدی_جابه_جایی=درون_Indexes[IX_EXTRA_داده_جابه_جایی];
    ownedTrie=ucptrie_open_From_دودویی(UCPTRIE_TYPE_FAST, UCPTRIE_مقدار_بیتها_16,
                                     درون_بایتها+جابه_جایی, بعدی_جابه_جایی-جابه_جایی, اشاره_تهی,
                                     &error_کد);
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }

    جابه_جایی=بعدی_جابه_جایی;
    بعدی_جابه_جایی=درون_Indexes[IX_SMALL_FCD_جابه_جایی];
    ثابت uint16_t* درون_Extra_داده = reinterpret_cast<ثابت uint16_t*>(درون_بایتها + جابه_جایی);

    // smallFCD: new in formatVersion 2
    جابه_جایی=بعدی_جابه_جایی;
    ثابت uint8_t *درون_Small_FCD=درون_بایتها+جابه_جایی;

    init(درون_Indexes, ownedTrie, درون_Extra_داده, درون_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    إذا(impl==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    impl->load(packageName, name, error_کد);
    أرجع createInstance(impl, error_کد);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_انتها

#إذا !NORM2_HARDCODE_NFC_داده
ساكن Norm2AllModes *nfcSingleton;
ساكن icu::UInitOnce nfcInitOnce {};
#پایان_اگر

ساكن Norm2AllModes *nfkcSingleton;
ساكن icu::UInitOnce nfkcInitOnce {};

ساكن Norm2AllModes *nfkc_cfSingleton;
ساكن icu::UInitOnce nfkc_cfInitOnce {};

ساكن Norm2AllModes *nfkc_scfSingleton;
ساكن icu::UInitOnce nfkc_scfInitOnce {};

ساكن UHashtable    *cache=اشاره_تهی;

// UInitOnce singleton initialization function
ساكن فراغ U_CALLCONV initSingletons(ثابت حرف *what, UError_کد &error_کد) {
#إذا !NORM2_HARDCODE_NFC_داده
    إذا (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(اشاره_تهی, "nfc", error_کد);
    } وإلا
#پایان_اگر
    إذا (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(اشاره_تهی, "nfkc", error_کد);
    } وإلا إذا (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(اشاره_تهی, "nfkc_cf", error_کد);
    } وإلا إذا (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(اشاره_تهی, "nfkc_scf", error_کد);
    } وإلا {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    حذف (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#إذا !NORM2_HARDCODE_NFC_داده
    حذف nfcSingleton;
    nfcSingleton = اشاره_تهی;
    nfcInitOnce.reset();
#پایان_اگر

    حذف nfkcSingleton;
    nfkcSingleton = اشاره_تهی;
    nfkcInitOnce.reset();

    حذف nfkc_cfSingleton;
    nfkc_cfSingleton = اشاره_تهی;
    nfkc_cfInitOnce.reset();

    حذف nfkc_scfSingleton;
    nfkc_scfSingleton = اشاره_تهی;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=اشاره_تهی;
    أرجع درست;
}

U_CDECL_انتها

#إذا !NORM2_HARDCODE_NFC_داده
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_کد);
    أرجع nfcSingleton;
}
#پایان_اگر

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_کد);
    أرجع nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_کد);
    أرجع nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) { أرجع اشاره_تهی; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_کد);
    أرجع nfkc_scfSingleton;
}

#إذا !NORM2_HARDCODE_NFC_داده
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->comp : اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->decomp : اشاره_تهی;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->fcd : اشاره_تهی;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->fcc : اشاره_تهی;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? allModes->impl : اشاره_تهی;
}
#پایان_اگر

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->comp : اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->decomp : اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->comp : اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? &allModes->comp : اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2_نما نما,
                         UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    إذا(name==اشاره_تهی || *name==0) {
        error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    ثابت Norm2AllModes *allModes=اشاره_تهی;
    إذا(packageName==اشاره_تهی) {
        إذا(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_کد);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_کد);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_کد);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_کد);
        }
    }
    إذا(allModes==اشاره_تهی && U_SUCCESS(error_کد)) {
        {
            Mutex قفل;
            إذا(cache!=اشاره_تهی) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        إذا(allModes==اشاره_تهی) {
            ucln_common_ثبات_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_اشاره_گر<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_کد));
            إذا(U_SUCCESS(error_کد)) {
                Mutex قفل;
                إذا(cache==اشاره_تهی) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, اشاره_تهی, &error_کد);
                    إذا(U_FAILURE(error_کد)) {
                        أرجع اشاره_تهی;
                    }
                    uhash_مجموعه_کلید_Deleter(cache, uprv_free);
                    uhash_مجموعه_مقدار_Deleter(cache, deleteNorm2AllModes);
                }
                فراغ *موقت=uhash_get(cache, name);
                إذا(موقت==اشاره_تهی) {
                    int32_t کلید_درازی= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(کلید_درازی));
                    إذا(nameCopy==اشاره_تهی) {
                        error_کد=U_حافظه_تخصیص_ERROR;
                        أرجع اشاره_تهی;
                    }
                    uprv_memcpy(nameCopy, name, کلید_درازی);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_کد);
                } وإلا {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(موقت);
                }
            }
        }
    }
    إذا(allModes!=اشاره_تهی && U_SUCCESS(error_کد)) {
        بدل(نما) {
        حالة UNORM2_COMPOSE:
            أرجع &allModes->comp;
        حالة UNORM2_DECOMPOSE:
            أرجع &allModes->decomp;
        حالة UNORM2_FCD:
            أرجع &allModes->fcd;
        حالة UNORM2_COMPOSE_CONTIGUOUS:
            أرجع &allModes->fcc;
        افتراضي:
            اكسر;  // do nothing
        }
    }
    أرجع اشاره_تهی;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_نما نما, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    بدل(نما) {
    حالة UNORM_NFD:
        أرجع Normalizer2::getNFDInstance(error_کد);
    حالة UNORM_NFKD:
        أرجع Normalizer2::getNFKDInstance(error_کد);
    حالة UNORM_NFC:
        أرجع Normalizer2::getNFCInstance(error_کد);
    حالة UNORM_NFKC:
        أرجع Normalizer2::getNFKCInstance(error_کد);
    حالة UNORM_FCD:
        أرجع getFCDInstance(error_کد);
    افتراضي:  // UNORM_NONE
        أرجع getNoopInstance(error_کد);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? allModes->impl : اشاره_تهی;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_کد &error_کد) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کد);
    أرجع allModes!=اشاره_تهی ? allModes->impl : اشاره_تهی;
}

U_NAMESPACE_انتها

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_کد *p_Error_کد) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_کد);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_کد *p_Error_کد) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_کد);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_کد *p_Error_کد) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_کد);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_کد *p_Error_کد) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_کد);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2_نما نما,
                   UError_کد *p_Error_کد) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, نما, *p_Error_کد);
}

U_CFUNC UNormalization_Check_نتیجه
unorm_get_سریع_Check(UChar32 c, UNormalization_نما نما) {
    إذا(نما<=UNORM_NONE || UNORM_FCD<=نما) {
        أرجع UNORM_YES;
    }
    UError_کد error_کد=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(نما, error_کد);
    إذا(U_SUCCESS(error_کد)) {
        أرجع ((ثابت Normalizer2WithImpl *)norm2)->get_سریع_Check(c);
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

#پایان_اگر  // !UCONFIG_NO_NORMALIZATION
