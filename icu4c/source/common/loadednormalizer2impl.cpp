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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "norm2allmodes.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

ವರ್ಗ LoadedNormalizer2Impl : ಸಾರ್ವಜನಿಕ Normalizer2Impl {
ಸಾರ್ವಜನಿಕ:
    LoadedNormalizer2Impl() : ಸ್ಮರಣೆ(ಶೂನ್ಯಸೂಚಕ), ownedTrie(ಶೂನ್ಯಸೂಚಕ) {}
    ವಾಸ್ತವಿಕ ~LoadedNormalizer2Impl();

    ಶೂನ್ಯ load(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

ಖಾಸಗಿ:
    ಸ್ಥಿರ UBool U_CALLCONV
    isAcceptable(ಶೂನ್ಯ *context, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *type, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ UDataInfo *pInfo);

    UData_ಸ್ಮರಣೆ *ಸ್ಮರಣೆ;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(ಸ್ಮರಣೆ);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(ಶೂನ್ಯ * /*context*/,
                                    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /* type */, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
                                    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಆದರೆ(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x72 &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x6d &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x32 &&
        pInfo->format_ಆವೃತ್ತಿ[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        ಹಿಂತಿರುಗಿ ನಿಜ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
}

ಶೂನ್ಯ
LoadedNormalizer2Impl::load(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಮರಣೆ=udata_openChoice(packageName, "nrm", name, isAcceptable, ಇದು, &error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಸ್ಥಿರಾಂಕ uint8_t* ಒಳ_ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t*>(udata_get_ಸ್ಮರಣೆ(ಸ್ಮರಣೆ));
    ಸ್ಥಿರಾಂಕ int32_t* ಒಳ_Indexes = reinterpret_cast<ಸ್ಥಿರಾಂಕ int32_t*>(ಒಳ_ಬೈಟುಗಳು);
    int32_t indexes_ಉದ್ದಳತೆ=ಒಳ_Indexes[IX_NORM_TRIE_ಸ್ಥಾನಾಂತರ]/4;
    ಆದರೆ(indexes_ಉದ್ದಳತೆ<=IX_MIN_LCCC_CP) {
        error_ಸಂಕೇತ=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಹಿಂತಿರುಗಿ;
    }

    int32_t ಸ್ಥಾನಾಂತರ=ಒಳ_Indexes[IX_NORM_TRIE_ಸ್ಥಾನಾಂತರ];
    int32_t ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=ಒಳ_Indexes[IX_EXTRA_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ];
    ownedTrie=ucptrie_open_From_ದ್ವಿಮಾನ(UCPTRIE_TYPE_FAST, UCPTRIE_ಮೌಲ್ಯ_ಬಿಟ್ಟುಗಳು_16,
                                     ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ-ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯಸೂಚಕ,
                                     &error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
    ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ=ಒಳ_Indexes[IX_SMALL_FCD_ಸ್ಥಾನಾಂತರ];
    ಸ್ಥಿರಾಂಕ uint16_t* ಒಳ_Extra_ದತ್ತಾಂಶ = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(ಒಳ_ಬೈಟುಗಳು + ಸ್ಥಾನಾಂತರ);

    // smallFCD: new in formatVersion 2
    ಸ್ಥಾನಾಂತರ=ಮುಂದಿನ_ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_Small_FCD=ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ;

    init(ಒಳ_Indexes, ownedTrie, ಒಳ_Extra_ದತ್ತಾಂಶ, ಒಳ_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                              UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    LoadedNormalizer2Impl *impl=ಹೊಸ LoadedNormalizer2Impl;
    ಆದರೆ(impl==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    impl->load(packageName, name, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ createInstance(impl, error_ಸಂಕೇತ);
}

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_ಕೊನೆ

#ಆದರೆ !NORM2_HARDCODE_NFC_ದತ್ತಾಂಶ
ಸ್ಥಿರ Norm2AllModes *nfcSingleton;
ಸ್ಥಿರ icu::UInitOnce nfcInitOnce {};
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ Norm2AllModes *nfkcSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkcInitOnce {};

ಸ್ಥಿರ Norm2AllModes *nfkc_cfSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkc_cfInitOnce {};

ಸ್ಥಿರ Norm2AllModes *nfkc_scfSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkc_scfInitOnce {};

ಸ್ಥಿರ UHashtable    *cache=ಶೂನ್ಯಸೂಚಕ;

// UInitOnce singleton initialization function
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initSingletons(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *what, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
#ಆದರೆ !NORM2_HARDCODE_NFC_ದತ್ತಾಂಶ
    ಆದರೆ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(ಶೂನ್ಯಸೂಚಕ, "nfc", error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(ಶೂನ್ಯಸೂಚಕ, "nfkc", error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(ಶೂನ್ಯಸೂಚಕ, "nfkc_cf", error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(ಶೂನ್ಯಸೂಚಕ, "nfkc_scf", error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV deleteNorm2AllModes(ಶೂನ್ಯ *allModes) {
    ಅಳಿಸು (Norm2AllModes *)allModes;
}

ಸ್ಥಿರ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ಆದರೆ !NORM2_HARDCODE_NFC_ದತ್ತಾಂಶ
    ಅಳಿಸು nfcSingleton;
    nfcSingleton = ಶೂನ್ಯಸೂಚಕ;
    nfcInitOnce.reset();
#ಆದರೆ_ಅಂತ್ಯ

    ಅಳಿಸು nfkcSingleton;
    nfkcSingleton = ಶೂನ್ಯಸೂಚಕ;
    nfkcInitOnce.reset();

    ಅಳಿಸು nfkc_cfSingleton;
    nfkc_cfSingleton = ಶೂನ್ಯಸೂಚಕ;
    nfkc_cfInitOnce.reset();

    ಅಳಿಸು nfkc_scfSingleton;
    nfkc_scfSingleton = ಶೂನ್ಯಸೂಚಕ;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CDECL_ಕೊನೆ

#ಆದರೆ !NORM2_HARDCODE_NFC_ದತ್ತಾಂಶ
ಸ್ಥಿರಾಂಕ Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ nfcSingleton;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ nfkcSingleton;
}

ಸ್ಥಿರಾಂಕ Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ nfkc_cfSingleton;
}

ಸ್ಥಿರಾಂಕ Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ nfkc_scfSingleton;
}

#ಆದರೆ !NORM2_HARDCODE_NFC_ದತ್ತಾಂಶ
ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->comp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFDInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->decomp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *Normalizer2Factory::getFCDInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->fcd : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *Normalizer2Factory::getFCCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->fcc : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? allModes->impl : ಶೂನ್ಯಸೂಚಕ;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFKCInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->comp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFKDInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->decomp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->comp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? &allModes->comp : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2::getInstance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                         UNormalization2_ಬಹುಲಕ ಬಹುಲಕ,
                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(name==ಶೂನ್ಯಸೂಚಕ || *name==0) {
        error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=ಶೂನ್ಯಸೂಚಕ;
    ಆದರೆ(packageName==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_ಸಂಕೇತ);
        }
    }
    ಆದರೆ(allModes==ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(error_ಸಂಕೇತ)) {
        {
            Mutex ಬೀಗ;
            ಆದರೆ(cache!=ಶೂನ್ಯಸೂಚಕ) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ಆದರೆ(allModes==ಶೂನ್ಯಸೂಚಕ) {
            ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            Local_ಸೂಚಕ<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_ಸಂಕೇತ));
            ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
                Mutex ಬೀಗ;
                ಆದರೆ(cache==ಶೂನ್ಯಸೂಚಕ) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, ಶೂನ್ಯಸೂಚಕ, &error_ಸಂಕೇತ);
                    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    }
                    uhash_ಗಣ_ಕೀಲಿ_Deleter(cache, uprv_free);
                    uhash_ಗಣ_ಮೌಲ್ಯ_Deleter(cache, deleteNorm2AllModes);
                }
                ಶೂನ್ಯ *ತಾತ್ಕಾಲಿಕ=uhash_get(cache, name);
                ಆದರೆ(ತಾತ್ಕಾಲಿಕ==ಶೂನ್ಯಸೂಚಕ) {
                    int32_t ಕೀಲಿ_ಉದ್ದಳತೆ= static_cast<int32_t>(uprv_strlen(name)+1);
                    ಅಕ್ಷರ* nameCopy = static_cast<ಅಕ್ಷರ*>(uprv_malloc(ಕೀಲಿ_ಉದ್ದಳತೆ));
                    ಆದರೆ(nameCopy==ಶೂನ್ಯಸೂಚಕ) {
                        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
                    }
                    uprv_memcpy(nameCopy, name, ಕೀಲಿ_ಉದ್ದಳತೆ);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(ತಾತ್ಕಾಲಿಕ);
                }
            }
        }
    }
    ಆದರೆ(allModes!=ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(error_ಸಂಕೇತ)) {
        ಆಯ್ಕೆ(ಬಹುಲಕ) {
        ಸ್ಥಿತಿ UNORM2_COMPOSE:
            ಹಿಂತಿರುಗಿ &allModes->comp;
        ಸ್ಥಿತಿ UNORM2_DECOMPOSE:
            ಹಿಂತಿರುಗಿ &allModes->decomp;
        ಸ್ಥಿತಿ UNORM2_FCD:
            ಹಿಂತಿರುಗಿ &allModes->fcd;
        ಸ್ಥಿತಿ UNORM2_COMPOSE_CONTIGUOUS:
            ಹಿಂತಿರುಗಿ &allModes->fcc;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;  // do nothing
        }
    }
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_ಬಹುಲಕ ಬಹುಲಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಆಯ್ಕೆ(ಬಹುಲಕ) {
    ಸ್ಥಿತಿ UNORM_NFD:
        ಹಿಂತಿರುಗಿ Normalizer2::getNFDInstance(error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ UNORM_NFKD:
        ಹಿಂತಿರುಗಿ Normalizer2::getNFKDInstance(error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ UNORM_NFC:
        ಹಿಂತಿರುಗಿ Normalizer2::getNFCInstance(error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ UNORM_NFKC:
        ಹಿಂತಿರುಗಿ Normalizer2::getNFKCInstance(error_ಸಂಕೇತ);
    ಸ್ಥಿತಿ UNORM_FCD:
        ಹಿಂತಿರುಗಿ getFCDInstance(error_ಸಂಕೇತ);
    ಪೂರ್ವನಿಯೋಜಿತ:  // UNORM_NONE
        ಹಿಂತಿರುಗಿ getNoopInstance(error_ಸಂಕೇತ);
    }
}

ಸ್ಥಿರಾಂಕ Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? allModes->impl : ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರಾಂಕ Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ allModes!=ಶೂನ್ಯಸೂಚಕ ? allModes->impl : ಶೂನ್ಯಸೂಚಕ;
}

U_NAMESPACE_ಕೊನೆ

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ಸ್ಥಿರಾಂಕ UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_ಸಂಕೇತ);
}

U_CAPI ಸ್ಥಿರಾಂಕ UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_ಸಂಕೇತ);
}

U_CAPI ಸ್ಥಿರಾಂಕ UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_ಸಂಕೇತ);
}

U_CAPI ಸ್ಥಿರಾಂಕ UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_ಸಂಕೇತ);
}

U_CAPI ಸ್ಥಿರಾಂಕ UNormalizer2 * U_EXPORT2
unorm2_getInstance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *packageName,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                   UNormalization2_ಬಹುಲಕ ಬಹುಲಕ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ UNormalizer2 *)Normalizer2::getInstance(packageName, name, ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
}

U_CFUNC UNormalization_Check_ಫಲಿತಾಂಶ
unorm_get_ಶೀಘ್ರ_Check(UChar32 c, UNormalization_ಬಹುಲಕ ಬಹುಲಕ) {
    ಆದರೆ(ಬಹುಲಕ<=UNORM_NONE || UNORM_FCD<=ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ UNORM_YES;
    }
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಸ್ಥಿರಾಂಕ Normalizer2 *norm2=Normalizer2Factory::getInstance(ಬಹುಲಕ, error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ((ಸ್ಥಿರಾಂಕ Normalizer2WithImpl *)norm2)->get_ಶೀಘ್ರ_Check(c);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UNORM_MAYBE;
    }
}

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_NORMALIZATION
