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
    LoadedNormalizer2Impl() : حافظہ(خالی_سوچک), ownedTrie(خالی_سوچک) {}
    مجازی ~LoadedNormalizer2Impl();

    فراغ load(ثابت حرف *packageName, ثابت حرف *name, UError_کوڈ &error_کوڈ);

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);

    UData_حافظہ *حافظہ;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(حافظہ);
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
        pInfo->انکڑے_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->انکڑے_Format[1]==0x72 &&
        pInfo->انکڑے_Format[2]==0x6d &&
        pInfo->انکڑے_Format[3]==0x32 &&
        pInfo->format_نسخہ[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        أرجع سچ;
    } وإلا {
        أرجع خطأ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    حافظہ=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }
    ثابت uint8_t* وچ_بائٹاں = static_cast<ثابت uint8_t*>(udata_get_حافظہ(حافظہ));
    ثابت int32_t* وچ_Indexes = reinterpret_cast<ثابت int32_t*>(وچ_بائٹاں);
    int32_t indexes_لمائی=وچ_Indexes[IX_NORM_TRIE_وتھ]/4;
    إذا(indexes_لمائی<=IX_MIN_LCCC_CP) {
        error_کوڈ=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t وتھ=وچ_Indexes[IX_NORM_TRIE_وتھ];
    int32_t اگلا_وتھ=وچ_Indexes[IX_EXTRA_انکڑے_وتھ];
    ownedTrie=ucptrie_open_From_ثنائی(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٹ_16,
                                     وچ_بائٹاں+وتھ, اگلا_وتھ-وتھ, خالی_سوچک,
                                     &error_کوڈ);
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع;
    }

    وتھ=اگلا_وتھ;
    اگلا_وتھ=وچ_Indexes[IX_SMALL_FCD_وتھ];
    ثابت uint16_t* وچ_Extra_انکڑے = reinterpret_cast<ثابت uint16_t*>(وچ_بائٹاں + وتھ);

    // smallFCD: new in formatVersion 2
    وتھ=اگلا_وتھ;
    ثابت uint8_t *وچ_Small_FCD=وچ_بائٹاں+وتھ;

    init(وچ_Indexes, ownedTrie, وچ_Extra_انکڑے, وچ_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    إذا(impl==خالی_سوچک) {
        error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }
    impl->load(packageName, name, error_کوڈ);
    أرجع createInstance(impl, error_کوڈ);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_سرا

#إذا !NORM2_HARDCODE_NFC_انکڑے
ساكن Norm2AllModes *nfcSingleton;
ساكن icu::UInitOnce nfcInitOnce {};
#جے_انت

ساكن Norm2AllModes *nfkcSingleton;
ساكن icu::UInitOnce nfkcInitOnce {};

ساكن Norm2AllModes *nfkc_cfSingleton;
ساكن icu::UInitOnce nfkc_cfInitOnce {};

ساكن Norm2AllModes *nfkc_scfSingleton;
ساكن icu::UInitOnce nfkc_scfInitOnce {};

ساكن UHashtable    *cache=خالی_سوچک;

// UInitOnce singleton initialization function
ساكن فراغ U_CALLCONV initSingletons(ثابت حرف *what, UError_کوڈ &error_کوڈ) {
#إذا !NORM2_HARDCODE_NFC_انکڑے
    إذا (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(خالی_سوچک, "nfc", error_کوڈ);
    } وإلا
#جے_انت
    إذا (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(خالی_سوچک, "nfkc", error_کوڈ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(خالی_سوچک, "nfkc_cf", error_کوڈ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(خالی_سوچک, "nfkc_scf", error_کوڈ);
    } وإلا {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    مٹاؤ (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#إذا !NORM2_HARDCODE_NFC_انکڑے
    مٹاؤ nfcSingleton;
    nfcSingleton = خالی_سوچک;
    nfcInitOnce.reset();
#جے_انت

    مٹاؤ nfkcSingleton;
    nfkcSingleton = خالی_سوچک;
    nfkcInitOnce.reset();

    مٹاؤ nfkc_cfSingleton;
    nfkc_cfSingleton = خالی_سوچک;
    nfkc_cfInitOnce.reset();

    مٹاؤ nfkc_scfSingleton;
    nfkc_scfSingleton = خالی_سوچک;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=خالی_سوچک;
    أرجع سچ;
}

U_CDECL_سرا

#إذا !NORM2_HARDCODE_NFC_انکڑے
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_کوڈ);
    أرجع nfcSingleton;
}
#جے_انت

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_کوڈ);
    أرجع nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_کوڈ);
    أرجع nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_کوڈ);
    أرجع nfkc_scfSingleton;
}

#إذا !NORM2_HARDCODE_NFC_انکڑے
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->comp : خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->decomp : خالی_سوچک;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->fcd : خالی_سوچک;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->fcc : خالی_سوچک;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? allModes->impl : خالی_سوچک;
}
#جے_انت

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->comp : خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->decomp : خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->comp : خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? &allModes->comp : خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2_منوال منوال,
                         UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    إذا(name==خالی_سوچک || *name==0) {
        error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    ثابت Norm2AllModes *allModes=خالی_سوچک;
    إذا(packageName==خالی_سوچک) {
        إذا(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_کوڈ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_کوڈ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_کوڈ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_کوڈ);
        }
    }
    إذا(allModes==خالی_سوچک && U_SUCCESS(error_کوڈ)) {
        {
            Mutex جندرا;
            إذا(cache!=خالی_سوچک) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        إذا(allModes==خالی_سوچک) {
            ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_اشارہ_گر<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_کوڈ));
            إذا(U_SUCCESS(error_کوڈ)) {
                Mutex جندرا;
                إذا(cache==خالی_سوچک) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, خالی_سوچک, &error_کوڈ);
                    إذا(U_FAILURE(error_کوڈ)) {
                        أرجع خالی_سوچک;
                    }
                    uhash_سیٹ_کلید_Deleter(cache, uprv_free);
                    uhash_سیٹ_قدر_Deleter(cache, deleteNorm2AllModes);
                }
                فراغ *عارضی=uhash_get(cache, name);
                إذا(عارضی==خالی_سوچک) {
                    int32_t کلید_لمائی= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(کلید_لمائی));
                    إذا(nameCopy==خالی_سوچک) {
                        error_کوڈ=U_حافظہ_ونڈ_ERROR;
                        أرجع خالی_سوچک;
                    }
                    uprv_memcpy(nameCopy, name, کلید_لمائی);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_کوڈ);
                } وإلا {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(عارضی);
                }
            }
        }
    }
    إذا(allModes!=خالی_سوچک && U_SUCCESS(error_کوڈ)) {
        بدل(منوال) {
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
    أرجع خالی_سوچک;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_منوال منوال, UError_کوڈ &error_کوڈ) {
    إذا(U_FAILURE(error_کوڈ)) {
        أرجع خالی_سوچک;
    }
    بدل(منوال) {
    حالة UNORM_NFD:
        أرجع Normalizer2::getNFDInstance(error_کوڈ);
    حالة UNORM_NFKD:
        أرجع Normalizer2::getNFKDInstance(error_کوڈ);
    حالة UNORM_NFC:
        أرجع Normalizer2::getNFCInstance(error_کوڈ);
    حالة UNORM_NFKC:
        أرجع Normalizer2::getNFKCInstance(error_کوڈ);
    حالة UNORM_FCD:
        أرجع getFCDInstance(error_کوڈ);
    افتراضي:  // UNORM_NONE
        أرجع getNoopInstance(error_کوڈ);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? allModes->impl : خالی_سوچک;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_کوڈ &error_کوڈ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_کوڈ);
    أرجع allModes!=خالی_سوچک ? allModes->impl : خالی_سوچک;
}

U_NAMESPACE_سرا

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_کوڈ *p_Error_کوڈ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_کوڈ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_کوڈ *p_Error_کوڈ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_کوڈ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_کوڈ *p_Error_کوڈ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_کوڈ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_کوڈ *p_Error_کوڈ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_کوڈ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2_منوال منوال,
                   UError_کوڈ *p_Error_کوڈ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, منوال, *p_Error_کوڈ);
}

U_CFUNC UNormalization_Check_نتیجہ
unorm_get_تیز_Check(UChar32 c, UNormalization_منوال منوال) {
    إذا(منوال<=UNORM_NONE || UNORM_FCD<=منوال) {
        أرجع UNORM_YES;
    }
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(منوال, error_کوڈ);
    إذا(U_SUCCESS(error_کوڈ)) {
        أرجع ((ثابت Normalizer2WithImpl *)norm2)->get_تیز_Check(c);
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

#جے_انت  // !UCONFIG_NO_NORMALIZATION
