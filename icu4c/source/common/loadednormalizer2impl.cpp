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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/normalizer2.h"
#అన్తర్భావ "unicode/ucptrie.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/unorm.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "mutex.h"
#అన్తర్భావ "norm2allmodes.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uhash.h"

U_NAMESPACE_BEGIN

తరగతి LoadedNormalizer2Impl : బహిరంగ Normalizer2Impl {
బహిరంగ:
    LoadedNormalizer2Impl() : జ్ఞాపకం(శూన్యసూచిక), ownedTrie(శూన్యసూచిక) {}
    మాయా ~LoadedNormalizer2Impl();

    శూన్య load(స్థిరరాశి అక్షరం *packageName, స్థిరరాశి అక్షరం *name, UError_సంకేతం &error_సంకేతం);

అంతరంగ:
    స్థిర UBool U_CALLCONV
    isAcceptable(శూన్య *context, స్థిరరాశి అక్షరం *type, స్థిరరాశి అక్షరం *name, స్థిరరాశి UDataInfo *pInfo);

    UData_జ్ఞాపకం *జ్ఞాపకం;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(జ్ఞాపకం);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(శూన్య * /*context*/,
                                    స్థిరరాశి అక్షరం * /* type */, స్థిరరాశి అక్షరం * /*name*/,
                                    స్థిరరాశి UDataInfo *pInfo) {
    ఒకవేళ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->దత్తాంశం_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->దత్తాంశం_Format[1]==0x72 &&
        pInfo->దత్తాంశం_Format[2]==0x6d &&
        pInfo->దత్తాంశం_Format[3]==0x32 &&
        pInfo->format_సంస్కరణ[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        తిరిగి నిజం;
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

శూన్య
LoadedNormalizer2Impl::load(స్థిరరాశి అక్షరం *packageName, స్థిరరాశి అక్షరం *name, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    జ్ఞాపకం=udata_openChoice(packageName, "nrm", name, isAcceptable, ఇది, &error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    స్థిరరాశి uint8_t* లోపల_బైట్లు = static_cast<స్థిరరాశి uint8_t*>(udata_get_జ్ఞాపకం(జ్ఞాపకం));
    స్థిరరాశి int32_t* లోపల_Indexes = reinterpret_cast<స్థిరరాశి int32_t*>(లోపల_బైట్లు);
    int32_t indexes_నిడివి=లోపల_Indexes[IX_NORM_TRIE_స్థానభ్రంశం]/4;
    ఒకవేళ(indexes_నిడివి<=IX_MIN_LCCC_CP) {
        error_సంకేతం=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        తిరిగి;
    }

    int32_t స్థానభ్రంశం=లోపల_Indexes[IX_NORM_TRIE_స్థానభ్రంశం];
    int32_t తదుపరి_స్థానభ్రంశం=లోపల_Indexes[IX_EXTRA_దత్తాంశం_స్థానభ్రంశం];
    ownedTrie=ucptrie_open_From_ద్విమాన(UCPTRIE_TYPE_FAST, UCPTRIE_విలువ_బిట్లు_16,
                                     లోపల_బైట్లు+స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం-స్థానభ్రంశం, శూన్యసూచిక,
                                     &error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }

    స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;
    తదుపరి_స్థానభ్రంశం=లోపల_Indexes[IX_SMALL_FCD_స్థానభ్రంశం];
    స్థిరరాశి uint16_t* లోపల_Extra_దత్తాంశం = reinterpret_cast<స్థిరరాశి uint16_t*>(లోపల_బైట్లు + స్థానభ్రంశం);

    // smallFCD: new in formatVersion 2
    స్థానభ్రంశం=తదుపరి_స్థానభ్రంశం;
    స్థిరరాశి uint8_t *లోపల_Small_FCD=లోపల_బైట్లు+స్థానభ్రంశం;

    init(లోపల_Indexes, ownedTrie, లోపల_Extra_దత్తాంశం, లోపల_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(స్థిరరాశి అక్షరం *packageName,
                              స్థిరరాశి అక్షరం *name,
                              UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    LoadedNormalizer2Impl *impl=కొత్త LoadedNormalizer2Impl;
    ఒకవేళ(impl==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    impl->load(packageName, name, error_సంకేతం);
    తిరిగి createInstance(impl, error_సంకేతం);
}

U_CDECL_BEGIN
స్థిర UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_చివర

#ఒకవేళ !NORM2_HARDCODE_NFC_దత్తాంశం
స్థిర Norm2AllModes *nfcSingleton;
స్థిర icu::UInitOnce nfcInitOnce {};
#అయితే_ముగింపు

స్థిర Norm2AllModes *nfkcSingleton;
స్థిర icu::UInitOnce nfkcInitOnce {};

స్థిర Norm2AllModes *nfkc_cfSingleton;
స్థిర icu::UInitOnce nfkc_cfInitOnce {};

స్థిర Norm2AllModes *nfkc_scfSingleton;
స్థిర icu::UInitOnce nfkc_scfInitOnce {};

స్థిర UHashtable    *cache=శూన్యసూచిక;

// UInitOnce singleton initialization function
స్థిర శూన్య U_CALLCONV initSingletons(స్థిరరాశి అక్షరం *what, UError_సంకేతం &error_సంకేతం) {
#ఒకవేళ !NORM2_HARDCODE_NFC_దత్తాంశం
    ఒకవేళ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(శూన్యసూచిక, "nfc", error_సంకేతం);
    } లేకపోతే
#అయితే_ముగింపు
    ఒకవేళ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(శూన్యసూచిక, "nfkc", error_సంకేతం);
    } లేకపోతే ఒకవేళ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(శూన్యసూచిక, "nfkc_cf", error_సంకేతం);
    } లేకపోతే ఒకవేళ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(శూన్యసూచిక, "nfkc_scf", error_సంకేతం);
    } లేకపోతే {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

స్థిర శూన్య U_CALLCONV deleteNorm2AllModes(శూన్య *allModes) {
    తొలగించు (Norm2AllModes *)allModes;
}

స్థిర UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ఒకవేళ !NORM2_HARDCODE_NFC_దత్తాంశం
    తొలగించు nfcSingleton;
    nfcSingleton = శూన్యసూచిక;
    nfcInitOnce.reset();
#అయితే_ముగింపు

    తొలగించు nfkcSingleton;
    nfkcSingleton = శూన్యసూచిక;
    nfkcInitOnce.reset();

    తొలగించు nfkc_cfSingleton;
    nfkc_cfSingleton = శూన్యసూచిక;
    nfkc_cfInitOnce.reset();

    తొలగించు nfkc_scfSingleton;
    nfkc_scfSingleton = శూన్యసూచిక;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=శూన్యసూచిక;
    తిరిగి నిజం;
}

U_CDECL_చివర

#ఒకవేళ !NORM2_HARDCODE_NFC_దత్తాంశం
స్థిరరాశి Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_సంకేతం);
    తిరిగి nfcSingleton;
}
#అయితే_ముగింపు

స్థిరరాశి Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_సంకేతం);
    తిరిగి nfkcSingleton;
}

స్థిరరాశి Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_సంకేతం);
    తిరిగి nfkc_cfSingleton;
}

స్థిరరాశి Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_సంకేతం);
    తిరిగి nfkc_scfSingleton;
}

#ఒకవేళ !NORM2_HARDCODE_NFC_దత్తాంశం
స్థిరరాశి Normalizer2 *
Normalizer2::getNFCInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->comp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2::getNFDInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->decomp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *Normalizer2Factory::getFCDInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->fcd : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *Normalizer2Factory::getFCCInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->fcc : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? allModes->impl : శూన్యసూచిక;
}
#అయితే_ముగింపు

స్థిరరాశి Normalizer2 *
Normalizer2::getNFKCInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->comp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2::getNFKDInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->decomp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->comp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? &allModes->comp : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2::getInstance(స్థిరరాశి అక్షరం *packageName,
                         స్థిరరాశి అక్షరం *name,
                         UNormalization2_బహుళకం బహుళకం,
                         UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ(name==శూన్యసూచిక || *name==0) {
        error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    స్థిరరాశి Norm2AllModes *allModes=శూన్యసూచిక;
    ఒకవేళ(packageName==శూన్యసూచిక) {
        ఒకవేళ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_సంకేతం);
        } లేకపోతే ఒకవేళ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_సంకేతం);
        } లేకపోతే ఒకవేళ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_సంకేతం);
        } లేకపోతే ఒకవేళ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_సంకేతం);
        }
    }
    ఒకవేళ(allModes==శూన్యసూచిక && U_SUCCESS(error_సంకేతం)) {
        {
            Mutex తాళం;
            ఒకవేళ(cache!=శూన్యసూచిక) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ఒకవేళ(allModes==శూన్యసూచిక) {
            ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_సూచిక<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_సంకేతం));
            ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
                Mutex తాళం;
                ఒకవేళ(cache==శూన్యసూచిక) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, శూన్యసూచిక, &error_సంకేతం);
                    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                        తిరిగి శూన్యసూచిక;
                    }
                    uhash_సమితి_కీలకం_Deleter(cache, uprv_free);
                    uhash_సమితి_విలువ_Deleter(cache, deleteNorm2AllModes);
                }
                శూన్య *తాత్కాలిక=uhash_get(cache, name);
                ఒకవేళ(తాత్కాలిక==శూన్యసూచిక) {
                    int32_t కీలకం_నిడివి= static_cast<int32_t>(uprv_strlen(name)+1);
                    అక్షరం* nameCopy = static_cast<అక్షరం*>(uprv_malloc(కీలకం_నిడివి));
                    ఒకవేళ(nameCopy==శూన్యసూచిక) {
                        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        తిరిగి శూన్యసూచిక;
                    }
                    uprv_memcpy(nameCopy, name, కీలకం_నిడివి);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_సంకేతం);
                } లేకపోతే {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(తాత్కాలిక);
                }
            }
        }
    }
    ఒకవేళ(allModes!=శూన్యసూచిక && U_SUCCESS(error_సంకేతం)) {
        ఎంపిక(బహుళకం) {
        స్థితి UNORM2_COMPOSE:
            తిరిగి &allModes->comp;
        స్థితి UNORM2_DECOMPOSE:
            తిరిగి &allModes->decomp;
        స్థితి UNORM2_FCD:
            తిరిగి &allModes->fcd;
        స్థితి UNORM2_COMPOSE_CONTIGUOUS:
            తిరిగి &allModes->fcc;
        అప్రమేయ:
            ఆపు;  // do nothing
        }
    }
    తిరిగి శూన్యసూచిక;
}

స్థిరరాశి Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_బహుళకం బహుళకం, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }
    ఎంపిక(బహుళకం) {
    స్థితి UNORM_NFD:
        తిరిగి Normalizer2::getNFDInstance(error_సంకేతం);
    స్థితి UNORM_NFKD:
        తిరిగి Normalizer2::getNFKDInstance(error_సంకేతం);
    స్థితి UNORM_NFC:
        తిరిగి Normalizer2::getNFCInstance(error_సంకేతం);
    స్థితి UNORM_NFKC:
        తిరిగి Normalizer2::getNFKCInstance(error_సంకేతం);
    స్థితి UNORM_FCD:
        తిరిగి getFCDInstance(error_సంకేతం);
    అప్రమేయ:  // UNORM_NONE
        తిరిగి getNoopInstance(error_సంకేతం);
    }
}

స్థిరరాశి Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? allModes->impl : శూన్యసూచిక;
}

స్థిరరాశి Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_సంకేతం &error_సంకేతం) {
    స్థిరరాశి Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_సంకేతం);
    తిరిగి allModes!=శూన్యసూచిక ? allModes->impl : శూన్యసూచిక;
}

U_NAMESPACE_చివర

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI స్థిరరాశి UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి (స్థిరరాశి UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_సంకేతం);
}

U_CAPI స్థిరరాశి UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి (స్థిరరాశి UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_సంకేతం);
}

U_CAPI స్థిరరాశి UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి (స్థిరరాశి UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_సంకేతం);
}

U_CAPI స్థిరరాశి UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి (స్థిరరాశి UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_సంకేతం);
}

U_CAPI స్థిరరాశి UNormalizer2 * U_EXPORT2
unorm2_getInstance(స్థిరరాశి అక్షరం *packageName,
                   స్థిరరాశి అక్షరం *name,
                   UNormalization2_బహుళకం బహుళకం,
                   UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి (స్థిరరాశి UNormalizer2 *)Normalizer2::getInstance(packageName, name, బహుళకం, *p_Error_సంకేతం);
}

U_CFUNC UNormalization_Check_ఫలితం
unorm_get_శీఘ్ర_Check(UChar32 c, UNormalization_బహుళకం బహుళకం) {
    ఒకవేళ(బహుళకం<=UNORM_NONE || UNORM_FCD<=బహుళకం) {
        తిరిగి UNORM_YES;
    }
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    స్థిరరాశి Normalizer2 *norm2=Normalizer2Factory::getInstance(బహుళకం, error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        తిరిగి ((స్థిరరాశి Normalizer2WithImpl *)norm2)->get_శీఘ్ర_Check(c);
    } లేకపోతే {
        తిరిగి UNORM_MAYBE;
    }
}

#అయితే_ముగింపు  // !UCONFIG_NO_NORMALIZATION
