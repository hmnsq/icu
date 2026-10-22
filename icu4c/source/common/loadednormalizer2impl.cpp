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
    LoadedNormalizer2Impl() : يادگيري(خالي_اشارو), ownedTrie(خالي_اشارو) {}
    مجازي ~LoadedNormalizer2Impl();

    فراغ load(ثابت حرف *packageName, ثابت حرف *name, UError_ڪوڊ &error_ڪوڊ);

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);

    UData_يادگيري *يادگيري;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(يادگيري);
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
        pInfo->format_نسخو[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        أرجع سچ;
    } وإلا {
        أرجع خطأ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    يادگيري=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    ثابت uint8_t* اندريون_بائيٽون = static_cast<ثابت uint8_t*>(udata_get_يادگيري(يادگيري));
    ثابت int32_t* اندريون_Indexes = reinterpret_cast<ثابت int32_t*>(اندريون_بائيٽون);
    int32_t indexes_ڊيگهائي=اندريون_Indexes[IX_NORM_TRIE_وٿي]/4;
    إذا(indexes_ڊيگهائي<=IX_MIN_LCCC_CP) {
        error_ڪوڊ=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        أرجع;
    }

    int32_t وٿي=اندريون_Indexes[IX_NORM_TRIE_وٿي];
    int32_t ايندڙ_وٿي=اندريون_Indexes[IX_EXTRA_معلومات_وٿي];
    ownedTrie=ucptrie_open_From_ٻٽو(UCPTRIE_TYPE_FAST, UCPTRIE_قدر_بٽ_16,
                                     اندريون_بائيٽون+وٿي, ايندڙ_وٿي-وٿي, خالي_اشارو,
                                     &error_ڪوڊ);
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }

    وٿي=ايندڙ_وٿي;
    ايندڙ_وٿي=اندريون_Indexes[IX_SMALL_FCD_وٿي];
    ثابت uint16_t* اندريون_Extra_معلومات = reinterpret_cast<ثابت uint16_t*>(اندريون_بائيٽون + وٿي);

    // smallFCD: new in formatVersion 2
    وٿي=ايندڙ_وٿي;
    ثابت uint8_t *اندريون_Small_FCD=اندريون_بائيٽون+وٿي;

    init(اندريون_Indexes, ownedTrie, اندريون_Extra_معلومات, اندريون_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    إذا(impl==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    impl->load(packageName, name, error_ڪوڊ);
    أرجع createInstance(impl, error_ڪوڊ);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_پڇاڙي

#إذا !NORM2_HARDCODE_NFC_معلومات
ساكن Norm2AllModes *nfcSingleton;
ساكن icu::UInitOnce nfcInitOnce {};
#جيڪڏهن_پورو

ساكن Norm2AllModes *nfkcSingleton;
ساكن icu::UInitOnce nfkcInitOnce {};

ساكن Norm2AllModes *nfkc_cfSingleton;
ساكن icu::UInitOnce nfkc_cfInitOnce {};

ساكن Norm2AllModes *nfkc_scfSingleton;
ساكن icu::UInitOnce nfkc_scfInitOnce {};

ساكن UHashtable    *cache=خالي_اشارو;

// UInitOnce singleton initialization function
ساكن فراغ U_CALLCONV initSingletons(ثابت حرف *what, UError_ڪوڊ &error_ڪوڊ) {
#إذا !NORM2_HARDCODE_NFC_معلومات
    إذا (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(خالي_اشارو, "nfc", error_ڪوڊ);
    } وإلا
#جيڪڏهن_پورو
    إذا (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(خالي_اشارو, "nfkc", error_ڪوڊ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(خالي_اشارو, "nfkc_cf", error_ڪوڊ);
    } وإلا إذا (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(خالي_اشارو, "nfkc_scf", error_ڪوڊ);
    } وإلا {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    ميٽيو (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#إذا !NORM2_HARDCODE_NFC_معلومات
    ميٽيو nfcSingleton;
    nfcSingleton = خالي_اشارو;
    nfcInitOnce.reset();
#جيڪڏهن_پورو

    ميٽيو nfkcSingleton;
    nfkcSingleton = خالي_اشارو;
    nfkcInitOnce.reset();

    ميٽيو nfkc_cfSingleton;
    nfkc_cfSingleton = خالي_اشارو;
    nfkc_cfInitOnce.reset();

    ميٽيو nfkc_scfSingleton;
    nfkc_scfSingleton = خالي_اشارو;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=خالي_اشارو;
    أرجع سچ;
}

U_CDECL_پڇاڙي

#إذا !NORM2_HARDCODE_NFC_معلومات
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_ڪوڊ);
    أرجع nfcSingleton;
}
#جيڪڏهن_پورو

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_ڪوڊ);
    أرجع nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_ڪوڊ);
    أرجع nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_ڪوڊ);
    أرجع nfkc_scfSingleton;
}

#إذا !NORM2_HARDCODE_NFC_معلومات
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->comp : خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->decomp : خالي_اشارو;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->fcd : خالي_اشارو;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->fcc : خالي_اشارو;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? allModes->impl : خالي_اشارو;
}
#جيڪڏهن_پورو

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->comp : خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->decomp : خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->comp : خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? &allModes->comp : خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2_گهڻائي گهڻائي,
                         UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    إذا(name==خالي_اشارو || *name==0) {
        error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    ثابت Norm2AllModes *allModes=خالي_اشارو;
    إذا(packageName==خالي_اشارو) {
        إذا(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_ڪوڊ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_ڪوڊ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_ڪوڊ);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_ڪوڊ);
        }
    }
    إذا(allModes==خالي_اشارو && U_SUCCESS(error_ڪوڊ)) {
        {
            Mutex تالو;
            إذا(cache!=خالي_اشارو) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        إذا(allModes==خالي_اشارو) {
            ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_اشارو<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_ڪوڊ));
            إذا(U_SUCCESS(error_ڪوڊ)) {
                Mutex تالو;
                إذا(cache==خالي_اشارو) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, خالي_اشارو, &error_ڪوڊ);
                    إذا(U_FAILURE(error_ڪوڊ)) {
                        أرجع خالي_اشارو;
                    }
                    uhash_سيٽ_ڪنجي_Deleter(cache, uprv_free);
                    uhash_سيٽ_قدر_Deleter(cache, deleteNorm2AllModes);
                }
                فراغ *عارضي=uhash_get(cache, name);
                إذا(عارضي==خالي_اشارو) {
                    int32_t ڪنجي_ڊيگهائي= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(ڪنجي_ڊيگهائي));
                    إذا(nameCopy==خالي_اشارو) {
                        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
                        أرجع خالي_اشارو;
                    }
                    uprv_memcpy(nameCopy, name, ڪنجي_ڊيگهائي);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_ڪوڊ);
                } وإلا {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(عارضي);
                }
            }
        }
    }
    إذا(allModes!=خالي_اشارو && U_SUCCESS(error_ڪوڊ)) {
        بدل(گهڻائي) {
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
    أرجع خالي_اشارو;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_گهڻائي گهڻائي, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    بدل(گهڻائي) {
    حالة UNORM_NFD:
        أرجع Normalizer2::getNFDInstance(error_ڪوڊ);
    حالة UNORM_NFKD:
        أرجع Normalizer2::getNFKDInstance(error_ڪوڊ);
    حالة UNORM_NFC:
        أرجع Normalizer2::getNFCInstance(error_ڪوڊ);
    حالة UNORM_NFKC:
        أرجع Normalizer2::getNFKCInstance(error_ڪوڊ);
    حالة UNORM_FCD:
        أرجع getFCDInstance(error_ڪوڊ);
    افتراضي:  // UNORM_NONE
        أرجع getNoopInstance(error_ڪوڊ);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? allModes->impl : خالي_اشارو;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_ڪوڊ &error_ڪوڊ) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ڪوڊ);
    أرجع allModes!=خالي_اشارو ? allModes->impl : خالي_اشارو;
}

U_NAMESPACE_پڇاڙي

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_ڪوڊ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_ڪوڊ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_ڪوڊ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_ڪوڊ);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2_گهڻائي گهڻائي,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, گهڻائي, *p_Error_ڪوڊ);
}

U_CFUNC UNormalization_Check_نتيجو
unorm_get_تيز_Check(UChar32 c, UNormalization_گهڻائي گهڻائي) {
    إذا(گهڻائي<=UNORM_NONE || UNORM_FCD<=گهڻائي) {
        أرجع UNORM_YES;
    }
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(گهڻائي, error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ)) {
        أرجع ((ثابت Normalizer2WithImpl *)norm2)->get_تيز_Check(c);
    } وإلا {
        أرجع UNORM_MAYBE;
    }
}

#جيڪڏهن_پورو  // !UCONFIG_NO_NORMALIZATION
