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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/normalizer2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucptrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unorm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "norm2allmodes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"

U_NAMESPACE_BEGIN

ਵਰਗ LoadedNormalizer2Impl : ਜਨਤਕ Normalizer2Impl {
ਜਨਤਕ:
    LoadedNormalizer2Impl() : ਸਮ੍ਰਿਤੀ(ਖਾਲੀ_ਸੂਚਕ), ownedTrie(ਖਾਲੀ_ਸੂਚਕ) {}
    ਆਭਾਸੀ ~LoadedNormalizer2Impl();

    ਖਾਲੀ load(ਅਚਲ ਅੱਖਰ *packageName, ਅਚਲ ਅੱਖਰ *name, UError_ਕੋਡ &error_ਕੋਡ);

ਨਿਜੀ:
    ਸਥਿਰ UBool U_CALLCONV
    isAcceptable(ਖਾਲੀ *context, ਅਚਲ ਅੱਖਰ *type, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ UDataInfo *pInfo);

    UData_ਸਮ੍ਰਿਤੀ *ਸਮ੍ਰਿਤੀ;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(ਸਮ੍ਰਿਤੀ);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(ਖਾਲੀ * /*context*/,
                                    ਅਚਲ ਅੱਖਰ * /* type */, ਅਚਲ ਅੱਖਰ * /*name*/,
                                    ਅਚਲ UDataInfo *pInfo) {
    ਜੇ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x72 &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x6d &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x32 &&
        pInfo->format_ਸੰਸਕਰਣ[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        ਵਾਪਸ ਸੱਚ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਝੂਠ;
    }
}

ਖਾਲੀ
LoadedNormalizer2Impl::load(ਅਚਲ ਅੱਖਰ *packageName, ਅਚਲ ਅੱਖਰ *name, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਸਮ੍ਰਿਤੀ=udata_openChoice(packageName, "nrm", name, isAcceptable, ਇਹ, &error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    ਅਚਲ uint8_t* ਵਿੱਚ_ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t*>(udata_get_ਸਮ੍ਰਿਤੀ(ਸਮ੍ਰਿਤੀ));
    ਅਚਲ int32_t* ਵਿੱਚ_Indexes = reinterpret_cast<ਅਚਲ int32_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ);
    int32_t indexes_ਲੰਮਾਈ=ਵਿੱਚ_Indexes[IX_NORM_TRIE_ਵਿੱਥ]/4;
    ਜੇ(indexes_ਲੰਮਾਈ<=IX_MIN_LCCC_CP) {
        error_ਕੋਡ=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ਵਾਪਸ;
    }

    int32_t ਵਿੱਥ=ਵਿੱਚ_Indexes[IX_NORM_TRIE_ਵਿੱਥ];
    int32_t ਅਗਲਾ_ਵਿੱਥ=ਵਿੱਚ_Indexes[IX_EXTRA_ਅੰਕੜੇ_ਵਿੱਥ];
    ownedTrie=ucptrie_open_From_ਦੁਆਧਾਰੀ(UCPTRIE_TYPE_FAST, UCPTRIE_ਮੁੱਲ_ਬਿੱਟ_16,
                                     ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, ਅਗਲਾ_ਵਿੱਥ-ਵਿੱਥ, ਖਾਲੀ_ਸੂਚਕ,
                                     &error_ਕੋਡ);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;
    ਅਗਲਾ_ਵਿੱਥ=ਵਿੱਚ_Indexes[IX_SMALL_FCD_ਵਿੱਥ];
    ਅਚਲ uint16_t* ਵਿੱਚ_Extra_ਅੰਕੜੇ = reinterpret_cast<ਅਚਲ uint16_t*>(ਵਿੱਚ_ਬਾਈਟਾਂ + ਵਿੱਥ);

    // smallFCD: new in formatVersion 2
    ਵਿੱਥ=ਅਗਲਾ_ਵਿੱਥ;
    ਅਚਲ uint8_t *ਵਿੱਚ_Small_FCD=ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ;

    init(ਵਿੱਚ_Indexes, ownedTrie, ਵਿੱਚ_Extra_ਅੰਕੜੇ, ਵਿੱਚ_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ਅਚਲ ਅੱਖਰ *packageName,
                              ਅਚਲ ਅੱਖਰ *name,
                              UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    LoadedNormalizer2Impl *impl=ਨਵਾਂ LoadedNormalizer2Impl;
    ਜੇ(impl==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    impl->load(packageName, name, error_ਕੋਡ);
    ਵਾਪਸ createInstance(impl, error_ਕੋਡ);
}

U_CDECL_BEGIN
ਸਥਿਰ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_ਸਿਰਾ

#ਜੇ !NORM2_HARDCODE_NFC_ਅੰਕੜੇ
ਸਥਿਰ Norm2AllModes *nfcSingleton;
ਸਥਿਰ icu::UInitOnce nfcInitOnce {};
#ਜੇ_ਅੰਤ

ਸਥਿਰ Norm2AllModes *nfkcSingleton;
ਸਥਿਰ icu::UInitOnce nfkcInitOnce {};

ਸਥਿਰ Norm2AllModes *nfkc_cfSingleton;
ਸਥਿਰ icu::UInitOnce nfkc_cfInitOnce {};

ਸਥਿਰ Norm2AllModes *nfkc_scfSingleton;
ਸਥਿਰ icu::UInitOnce nfkc_scfInitOnce {};

ਸਥਿਰ UHashtable    *cache=ਖਾਲੀ_ਸੂਚਕ;

// UInitOnce singleton initialization function
ਸਥਿਰ ਖਾਲੀ U_CALLCONV initSingletons(ਅਚਲ ਅੱਖਰ *what, UError_ਕੋਡ &error_ਕੋਡ) {
#ਜੇ !NORM2_HARDCODE_NFC_ਅੰਕੜੇ
    ਜੇ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(ਖਾਲੀ_ਸੂਚਕ, "nfc", error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    ਜੇ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(ਖਾਲੀ_ਸੂਚਕ, "nfkc", error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(ਖਾਲੀ_ਸੂਚਕ, "nfkc_cf", error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(ਖਾਲੀ_ਸੂਚਕ, "nfkc_scf", error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ਸਥਿਰ ਖਾਲੀ U_CALLCONV deleteNorm2AllModes(ਖਾਲੀ *allModes) {
    ਮਿਟਾਓ (Norm2AllModes *)allModes;
}

ਸਥਿਰ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ਜੇ !NORM2_HARDCODE_NFC_ਅੰਕੜੇ
    ਮਿਟਾਓ nfcSingleton;
    nfcSingleton = ਖਾਲੀ_ਸੂਚਕ;
    nfcInitOnce.reset();
#ਜੇ_ਅੰਤ

    ਮਿਟਾਓ nfkcSingleton;
    nfkcSingleton = ਖਾਲੀ_ਸੂਚਕ;
    nfkcInitOnce.reset();

    ਮਿਟਾਓ nfkc_cfSingleton;
    nfkc_cfSingleton = ਖਾਲੀ_ਸੂਚਕ;
    nfkc_cfInitOnce.reset();

    ਮਿਟਾਓ nfkc_scfSingleton;
    nfkc_scfSingleton = ਖਾਲੀ_ਸੂਚਕ;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ ਸੱਚ;
}

U_CDECL_ਸਿਰਾ

#ਜੇ !NORM2_HARDCODE_NFC_ਅੰਕੜੇ
ਅਚਲ Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_ਕੋਡ);
    ਵਾਪਸ nfcSingleton;
}
#ਜੇ_ਅੰਤ

ਅਚਲ Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_ਕੋਡ);
    ਵਾਪਸ nfkcSingleton;
}

ਅਚਲ Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_ਕੋਡ);
    ਵਾਪਸ nfkc_cfSingleton;
}

ਅਚਲ Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_ਕੋਡ);
    ਵਾਪਸ nfkc_scfSingleton;
}

#ਜੇ !NORM2_HARDCODE_NFC_ਅੰਕੜੇ
ਅਚਲ Normalizer2 *
Normalizer2::getNFCInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->comp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2::getNFDInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->decomp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *Normalizer2Factory::getFCDInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->fcd : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *Normalizer2Factory::getFCCInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->fcc : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? allModes->impl : ਖਾਲੀ_ਸੂਚਕ;
}
#ਜੇ_ਅੰਤ

ਅਚਲ Normalizer2 *
Normalizer2::getNFKCInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->comp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2::getNFKDInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->decomp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->comp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? &allModes->comp : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2::getInstance(ਅਚਲ ਅੱਖਰ *packageName,
                         ਅਚਲ ਅੱਖਰ *name,
                         UNormalization2_ਬਹੁਲਕ ਬਹੁਲਕ,
                         UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(name==ਖਾਲੀ_ਸੂਚਕ || *name==0) {
        error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ Norm2AllModes *allModes=ਖਾਲੀ_ਸੂਚਕ;
    ਜੇ(packageName==ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ ਜੇ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_ਕੋਡ);
        }
    }
    ਜੇ(allModes==ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(error_ਕੋਡ)) {
        {
            Mutex ਤਾਲਾ;
            ਜੇ(cache!=ਖਾਲੀ_ਸੂਚਕ) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ਜੇ(allModes==ਖਾਲੀ_ਸੂਚਕ) {
            ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_ਸੂਚਕ<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_ਕੋਡ));
            ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
                Mutex ਤਾਲਾ;
                ਜੇ(cache==ਖਾਲੀ_ਸੂਚਕ) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, ਖਾਲੀ_ਸੂਚਕ, &error_ਕੋਡ);
                    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    }
                    uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(cache, uprv_free);
                    uhash_ਸੈੱਟ_ਮੁੱਲ_Deleter(cache, deleteNorm2AllModes);
                }
                ਖਾਲੀ *ਆਰਜ਼ੀ=uhash_get(cache, name);
                ਜੇ(ਆਰਜ਼ੀ==ਖਾਲੀ_ਸੂਚਕ) {
                    int32_t ਕੁੰਜੀ_ਲੰਮਾਈ= static_cast<int32_t>(uprv_strlen(name)+1);
                    ਅੱਖਰ* nameCopy = static_cast<ਅੱਖਰ*>(uprv_malloc(ਕੁੰਜੀ_ਲੰਮਾਈ));
                    ਜੇ(nameCopy==ਖਾਲੀ_ਸੂਚਕ) {
                        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
                    }
                    uprv_memcpy(nameCopy, name, ਕੁੰਜੀ_ਲੰਮਾਈ);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_ਕੋਡ);
                } ਨਹੀਂਤਾਂ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(ਆਰਜ਼ੀ);
                }
            }
        }
    }
    ਜੇ(allModes!=ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(error_ਕੋਡ)) {
        ਚੋਣ(ਬਹੁਲਕ) {
        ਹਾਲਤ UNORM2_COMPOSE:
            ਵਾਪਸ &allModes->comp;
        ਹਾਲਤ UNORM2_DECOMPOSE:
            ਵਾਪਸ &allModes->decomp;
        ਹਾਲਤ UNORM2_FCD:
            ਵਾਪਸ &allModes->fcd;
        ਹਾਲਤ UNORM2_COMPOSE_CONTIGUOUS:
            ਵਾਪਸ &allModes->fcc;
        ਮੂਲ:
            ਰੋਕੋ;  // do nothing
        }
    }
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਚੋਣ(ਬਹੁਲਕ) {
    ਹਾਲਤ UNORM_NFD:
        ਵਾਪਸ Normalizer2::getNFDInstance(error_ਕੋਡ);
    ਹਾਲਤ UNORM_NFKD:
        ਵਾਪਸ Normalizer2::getNFKDInstance(error_ਕੋਡ);
    ਹਾਲਤ UNORM_NFC:
        ਵਾਪਸ Normalizer2::getNFCInstance(error_ਕੋਡ);
    ਹਾਲਤ UNORM_NFKC:
        ਵਾਪਸ Normalizer2::getNFKCInstance(error_ਕੋਡ);
    ਹਾਲਤ UNORM_FCD:
        ਵਾਪਸ getFCDInstance(error_ਕੋਡ);
    ਮੂਲ:  // UNORM_NONE
        ਵਾਪਸ getNoopInstance(error_ਕੋਡ);
    }
}

ਅਚਲ Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? allModes->impl : ਖਾਲੀ_ਸੂਚਕ;
}

ਅਚਲ Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_ਕੋਡ &error_ਕੋਡ) {
    ਅਚਲ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ਕੋਡ);
    ਵਾਪਸ allModes!=ਖਾਲੀ_ਸੂਚਕ ? allModes->impl : ਖਾਲੀ_ਸੂਚਕ;
}

U_NAMESPACE_ਸਿਰਾ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ਅਚਲ UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ (ਅਚਲ UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_ਕੋਡ);
}

U_CAPI ਅਚਲ UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ (ਅਚਲ UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_ਕੋਡ);
}

U_CAPI ਅਚਲ UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ (ਅਚਲ UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_ਕੋਡ);
}

U_CAPI ਅਚਲ UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ (ਅਚਲ UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_ਕੋਡ);
}

U_CAPI ਅਚਲ UNormalizer2 * U_EXPORT2
unorm2_getInstance(ਅਚਲ ਅੱਖਰ *packageName,
                   ਅਚਲ ਅੱਖਰ *name,
                   UNormalization2_ਬਹੁਲਕ ਬਹੁਲਕ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ (ਅਚਲ UNormalizer2 *)Normalizer2::getInstance(packageName, name, ਬਹੁਲਕ, *p_Error_ਕੋਡ);
}

U_CFUNC UNormalization_Check_ਨਤੀਜਾ
unorm_get_ਤੇਜ਼_Check(UChar32 c, UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ) {
    ਜੇ(ਬਹੁਲਕ<=UNORM_NONE || UNORM_FCD<=ਬਹੁਲਕ) {
        ਵਾਪਸ UNORM_YES;
    }
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਅਚਲ Normalizer2 *norm2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਵਾਪਸ ((ਅਚਲ Normalizer2WithImpl *)norm2)->get_ਤੇਜ਼_Check(c);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UNORM_MAYBE;
    }
}

#ਜੇ_ਅੰਤ  // !UCONFIG_NO_NORMALIZATION
