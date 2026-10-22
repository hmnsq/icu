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
    LoadedNormalizer2Impl() : حافظه(تش_اشاره), ownedTrie(تش_اشاره) {}
    مجازي ~LoadedNormalizer2Impl();

    فراغ load(ثابت حرف *packageName, ثابت حرف *name, UError_کوډ &error_کوډ);

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
        pInfo->معلومات_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->معلومات_Format[1]==0x72 &&
        pInfo->معلومات_Format[2]==0x6d &&
        pInfo->معلومات_Format[3]==0x32 &&
        pInfo->format_نسخه[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        أرجع ریښتیا;
    } وإلا {
        أرجع خطأ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    حافظه=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }
    ثابت uint8_t* دننه_بایټونه = static_cast<ثابت uint8_t*>(udata_get_حافظه(حافظه));
    ثابت int32_t* دننه_Indexes = reinterpret_cast<ثابت int32_t*>(دننه_بایټونه);
    int32_t indexes_اوږدتیا=دننه_Indexes[IX_NORM_TRIE_دننه_واټن]/4;
    إذا(indexes_اوږدتیا<=IX_MIN_LCCC_CP) {
        error_کوډ=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t دننه_واټن=دننه_Indexes[IX_NORM_TRIE_دننه_واټن];
    int32_t بل_دننه_واټن=دننه_Indexes[IX_EXTRA_معلومات_دننه_واټن];
    ownedTrie=ucptrie_open_From_دوګونی(UCPTRIE_TYPE_FAST, UCPTRIE_ارزښت_بټونه_16,
                                     دننه_بایټونه+دننه_واټن, بل_دننه_واټن-دننه_واټن, تش_اشاره,
                                     &error_کوډ);
    إذا(U_FAILURE(error_کوډ)) {
        أرجع;
    }

    دننه_واټن=بل_دننه_واټن;
    بل_دننه_واټن=دننه_Indexes[IX_SMALL_FCD_دننه_واټن];
    ثابت uint16_t* دننه_Extra_معلومات = reinterpret_cast<ثابت uint16_t*>(دننه_بایټونه + دننه_واټن);

    // smallFCD: new in formatVersion 2
    دننه_واټن=بل_دننه_واټن;
    ثابت uint8_t *دننه_Small_FCD=دننه_بایټونه+دننه_واټن;

    init(دننه_Indexes, ownedTrie, دننه_Extra_معلومات, دننه_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    إذا(impl==تش_اشاره) {
        error_کوډ=U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }
    impl->load(packageName, name, error_کوډ);
    أرجع createInstance(impl, error_کوډ);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_پای_ټکی

#إذا !NORM2_HARDCODE_NFC_معلومات
ساكن Norm2AllModes *nfcSingleton;
ساكن icu::UInitOnce nfcInitOnce {};
#د_که_پای

ساكن Norm2AllModes *nfkcSingleton;
ساكن icu::UInitOnce nfkcInitOnce {};

ساكن Norm2AllModes *nfkc_cfSingleton;
ساكن icu::UInitOnce nfkc_cfInitOnce {};

ساكن Norm2AllModes *nfkc_scfSingleton;
ساكن icu::UInitOnce nfkc_scfInitOnce {};

ساكن UHashtable    *cache=تش_اشاره;

// UInitOnce singleton initialization function
ساكن فراغ U_CALLCONV initSingletons(ثابت حرف *what, UError_کوډ &error_کوډ) {
#إذا !NORM2_HARDCODE_NFC_معلومات
    إذا (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(تش_اشاره, "nfc", error_کوډ);
    } وإلا
#د_که_پای
    إذا (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(تش_اشاره, "nfkc", error_کوډ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(تش_اشاره, "nfkc_cf", error_کوډ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(تش_اشاره, "nfkc_scf", error_کوډ);
    } وإلا {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    ړنګول (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#إذا !NORM2_HARDCODE_NFC_معلومات
    ړنګول nfcSingleton;
    nfcSingleton = تش_اشاره;
    nfcInitOnce.reset();
#د_که_پای

    ړنګول nfkcSingleton;
    nfkcSingleton = تش_اشاره;
    nfkcInitOnce.reset();

    ړنګول nfkc_cfSingleton;
    nfkc_cfSingleton = تش_اشاره;
    nfkc_cfInitOnce.reset();

    ړنګول nfkc_scfSingleton;
    nfkc_scfSingleton = تش_اشاره;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=تش_اشاره;
    أرجع ریښتیا;
}

U_CDECL_پای_ټکی

#إذا !NORM2_HARDCODE_NFC_معلومات
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_کوډ);
    أرجع nfcSingleton;
}
#د_که_پای

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_کوډ);
    أرجع nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_کوډ);
    أرجع nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_کوډ);
    أرجع nfkc_scfSingleton;
}

#إذا !NORM2_HARDCODE_NFC_معلومات
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->comp : تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->decomp : تش_اشاره;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->fcd : تش_اشاره;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->fcc : تش_اشاره;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? allModes->impl : تش_اشاره;
}
#د_که_پای

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->comp : تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->decomp : تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->comp : تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? &allModes->comp : تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2_ډېرپېښ ډېرپېښ,
                         UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    إذا(name==تش_اشاره || *name==0) {
        error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    ثابت Norm2AllModes *allModes=تش_اشاره;
    إذا(packageName==تش_اشاره) {
        إذا(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_کوډ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_کوډ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_کوډ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_کوډ);
        }
    }
    إذا(allModes==تش_اشاره && U_SUCCESS(error_کوډ)) {
        {
            Mutex قلف;
            إذا(cache!=تش_اشاره) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        إذا(allModes==تش_اشاره) {
            ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_اشاره_ګر<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_کوډ));
            إذا(U_SUCCESS(error_کوډ)) {
                Mutex قلف;
                إذا(cache==تش_اشاره) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, تش_اشاره, &error_کوډ);
                    إذا(U_FAILURE(error_کوډ)) {
                        أرجع تش_اشاره;
                    }
                    uhash_سېټ_کلی_Deleter(cache, uprv_free);
                    uhash_سېټ_ارزښت_Deleter(cache, deleteNorm2AllModes);
                }
                فراغ *لنډمهاله=uhash_get(cache, name);
                إذا(لنډمهاله==تش_اشاره) {
                    int32_t کلی_اوږدتیا= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(کلی_اوږدتیا));
                    إذا(nameCopy==تش_اشاره) {
                        error_کوډ=U_حافظه_ویش_ERROR;
                        أرجع تش_اشاره;
                    }
                    uprv_memcpy(nameCopy, name, کلی_اوږدتیا);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_کوډ);
                } وإلا {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(لنډمهاله);
                }
            }
        }
    }
    إذا(allModes!=تش_اشاره && U_SUCCESS(error_کوډ)) {
        بدل(ډېرپېښ) {
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
    أرجع تش_اشاره;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_ډېرپېښ ډېرپېښ, UError_کوډ &error_کوډ) {
    إذا(U_FAILURE(error_کوډ)) {
        أرجع تش_اشاره;
    }
    بدل(ډېرپېښ) {
    حالة UNORM_NFD:
        أرجع Normalizer2::getNFDInstance(error_کوډ);
    حالة UNORM_NFKD:
        أرجع Normalizer2::getNFKDInstance(error_کوډ);
    حالة UNORM_NFC:
        أرجع Normalizer2::getNFCInstance(error_کوډ);
    حالة UNORM_NFKC:
        أرجع Normalizer2::getNFKCInstance(error_کوډ);
    حالة UNORM_FCD:
        أرجع getFCDInstance(error_کوډ);
    افتراضي:  // UNORM_NONE
        أرجع getNoopInstance(error_کوډ);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? allModes->impl : تش_اشاره;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_کوډ &error_کوډ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کوډ);
    أرجع allModes!=تش_اشاره ? allModes->impl : تش_اشاره;
}

U_NAMESPACE_پای_ټکی

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_کوډ *p_Error_کوډ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_کوډ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_کوډ *p_Error_کوډ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_کوډ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_کوډ *p_Error_کوډ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_کوډ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_کوډ *p_Error_کوډ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_کوډ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2_ډېرپېښ ډېرپېښ,
                   UError_کوډ *p_Error_کوډ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, ډېرپېښ, *p_Error_کوډ);
}

U_CFUNC UNormalization_Check_پایله
unorm_get_چټک_Check(UChar32 c, UNormalization_ډېرپېښ ډېرپېښ) {
    إذا(ډېرپېښ<=UNORM_NONE || UNORM_FCD<=ډېرپېښ) {
        أرجع UNORM_YES;
    }
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(ډېرپېښ, error_کوډ);
    إذا(U_SUCCESS(error_کوډ)) {
        أرجع ((ثابت Normalizer2WithImpl *)norm2)->get_چټک_Check(c);
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

#د_که_پای  // !UCONFIG_NO_NORMALIZATION
