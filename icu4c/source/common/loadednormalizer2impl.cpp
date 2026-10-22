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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "norm2allmodes.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

वर्ग LoadedNormalizer2Impl : सार्वजनिक Normalizer2Impl {
सार्वजनिक:
    LoadedNormalizer2Impl() : स्मृतिः(शून्यसूचकः), ownedTrie(शून्यसूचकः) {}
    आभास ~LoadedNormalizer2Impl();

    शून्य load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UError_संहिता &error_संहिता);

गुप्त:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, ध्रुव वर्ण *type, ध्रुव वर्ण *name, ध्रुव UDataInfo *pInfo);

    UData_स्मृतिः *स्मृतिः;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(स्मृतिः);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(शून्य * /*context*/,
                                    ध्रुव वर्ण * /* type */, ध्रुव वर्ण * /*name*/,
                                    ध्रुव UDataInfo *pInfo) {
    यदि(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->दत्त_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->दत्त_Format[1]==0x72 &&
        pInfo->दत्त_Format[2]==0x6d &&
        pInfo->दत्त_Format[3]==0x32 &&
        pInfo->format_संस्करणम्[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        प्रत्यागम सत्य;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

शून्य
LoadedNormalizer2Impl::load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    स्मृतिः=udata_openChoice(packageName, "nrm", name, isAcceptable, एतत्, &error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    ध्रुव uint8_t* अन्तर्_अष्टकानि = static_cast<ध्रुव uint8_t*>(udata_get_स्मृतिः(स्मृतिः));
    ध्रुव int32_t* अन्तर्_Indexes = reinterpret_cast<ध्रुव int32_t*>(अन्तर्_अष्टकानि);
    int32_t indexes_दीर्घता=अन्तर्_Indexes[IX_NORM_TRIE_विस्थापनम्]/4;
    यदि(indexes_दीर्घता<=IX_MIN_LCCC_CP) {
        error_संहिता=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        प्रत्यागम;
    }

    int32_t विस्थापनम्=अन्तर्_Indexes[IX_NORM_TRIE_विस्थापनम्];
    int32_t अग्र_विस्थापनम्=अन्तर्_Indexes[IX_EXTRA_दत्त_विस्थापनम्];
    ownedTrie=ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मानम्_द्वयङ्काः_16,
                                     अन्तर्_अष्टकानि+विस्थापनम्, अग्र_विस्थापनम्-विस्थापनम्, शून्यसूचकः,
                                     &error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }

    विस्थापनम्=अग्र_विस्थापनम्;
    अग्र_विस्थापनम्=अन्तर्_Indexes[IX_SMALL_FCD_विस्थापनम्];
    ध्रुव uint16_t* अन्तर्_Extra_दत्त = reinterpret_cast<ध्रुव uint16_t*>(अन्तर्_अष्टकानि + विस्थापनम्);

    // smallFCD: new in formatVersion 2
    विस्थापनम्=अग्र_विस्थापनम्;
    ध्रुव uint8_t *अन्तर्_Small_FCD=अन्तर्_अष्टकानि+विस्थापनम्;

    init(अन्तर्_Indexes, ownedTrie, अन्तर्_Extra_दत्त, अन्तर्_Small_FCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ध्रुव वर्ण *packageName,
                              ध्रुव वर्ण *name,
                              UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    LoadedNormalizer2Impl *impl=नूतन LoadedNormalizer2Impl;
    यदि(impl==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    impl->load(packageName, name, error_संहिता);
    प्रत्यागम createInstance(impl, error_संहिता);
}

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_अन्तः

#यदि !NORM2_HARDCODE_NFC_दत्त
स्थिर Norm2AllModes *nfcSingleton;
स्थिर icu::UInitOnce nfcInitOnce {};
#यदि_अन्तः

स्थिर Norm2AllModes *nfkcSingleton;
स्थिर icu::UInitOnce nfkcInitOnce {};

स्थिर Norm2AllModes *nfkc_cfSingleton;
स्थिर icu::UInitOnce nfkc_cfInitOnce {};

स्थिर Norm2AllModes *nfkc_scfSingleton;
स्थिर icu::UInitOnce nfkc_scfInitOnce {};

स्थिर UHashtable    *cache=शून्यसूचकः;

// UInitOnce singleton initialization function
स्थिर शून्य U_CALLCONV initSingletons(ध्रुव वर्ण *what, UError_संहिता &error_संहिता) {
#यदि !NORM2_HARDCODE_NFC_दत्त
    यदि (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(शून्यसूचकः, "nfc", error_संहिता);
    } अन्यथा
#यदि_अन्तः
    यदि (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(शून्यसूचकः, "nfkc", error_संहिता);
    } अन्यथा यदि (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(शून्यसूचकः, "nfkc_cf", error_संहिता);
    } अन्यथा यदि (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(शून्यसूचकः, "nfkc_scf", error_संहिता);
    } अन्यथा {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV deleteNorm2AllModes(शून्य *allModes) {
    विलोपय (Norm2AllModes *)allModes;
}

स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#यदि !NORM2_HARDCODE_NFC_दत्त
    विलोपय nfcSingleton;
    nfcSingleton = शून्यसूचकः;
    nfcInitOnce.reset();
#यदि_अन्तः

    विलोपय nfkcSingleton;
    nfkcSingleton = शून्यसूचकः;
    nfkcInitOnce.reset();

    विलोपय nfkc_cfSingleton;
    nfkc_cfSingleton = शून्यसूचकः;
    nfkc_cfInitOnce.reset();

    विलोपय nfkc_scfSingleton;
    nfkc_scfSingleton = शून्यसूचकः;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=शून्यसूचकः;
    प्रत्यागम सत्य;
}

U_CDECL_अन्तः

#यदि !NORM2_HARDCODE_NFC_दत्त
ध्रुव Norm2AllModes *
Norm2AllModes::getNFCInstance(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", error_संहिता);
    प्रत्यागम nfcSingleton;
}
#यदि_अन्तः

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKCInstance(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", error_संहिता);
    प्रत्यागम nfkcSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", error_संहिता);
    प्रत्यागम nfkc_cfSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", error_संहिता);
    प्रत्यागम nfkc_scfSingleton;
}

#यदि !NORM2_HARDCODE_NFC_दत्त
ध्रुव Normalizer2 *
Normalizer2::getNFCInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->comp : शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2::getNFDInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->decomp : शून्यसूचकः;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCDInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->fcd : शून्यसूचकः;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCCInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->fcc : शून्यसूचकः;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? allModes->impl : शून्यसूचकः;
}
#यदि_अन्तः

ध्रुव Normalizer2 *
Normalizer2::getNFKCInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->comp : शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKDInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->decomp : शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->comp : शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? &allModes->comp : शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2::getInstance(ध्रुव वर्ण *packageName,
                         ध्रुव वर्ण *name,
                         UNormalization2_बहुलकम् बहुलकम्,
                         UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि(name==शून्यसूचकः || *name==0) {
        error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव Norm2AllModes *allModes=शून्यसूचकः;
    यदि(packageName==शून्यसूचकः) {
        यदि(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(error_संहिता);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(error_संहिता);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(error_संहिता);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(error_संहिता);
        }
    }
    यदि(allModes==शून्यसूचकः && U_SUCCESS(error_संहिता)) {
        {
            Mutex तालकम्;
            यदि(cache!=शून्यसूचकः) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        यदि(allModes==शून्यसूचकः) {
            ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, error_संहिता));
            यदि(U_SUCCESS(error_संहिता)) {
                Mutex तालकम्;
                यदि(cache==शून्यसूचकः) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, शून्यसूचकः, &error_संहिता);
                    यदि(U_FAILURE(error_संहिता)) {
                        प्रत्यागम शून्यसूचकः;
                    }
                    uhash_समुच्चयः_कुञ्चिका_Deleter(cache, uprv_free);
                    uhash_समुच्चयः_मानम्_Deleter(cache, deleteNorm2AllModes);
                }
                शून्य *अस्थायी=uhash_get(cache, name);
                यदि(अस्थायी==शून्यसूचकः) {
                    int32_t कुञ्चिका_दीर्घता= static_cast<int32_t>(uprv_strlen(name)+1);
                    वर्ण* nameCopy = static_cast<वर्ण*>(uprv_malloc(कुञ्चिका_दीर्घता));
                    यदि(nameCopy==शून्यसूचकः) {
                        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                        प्रत्यागम शून्यसूचकः;
                    }
                    uprv_memcpy(nameCopy, name, कुञ्चिका_दीर्घता);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &error_संहिता);
                } अन्यथा {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(अस्थायी);
                }
            }
        }
    }
    यदि(allModes!=शून्यसूचकः && U_SUCCESS(error_संहिता)) {
        विकल्प(बहुलकम्) {
        पक्ष UNORM2_COMPOSE:
            प्रत्यागम &allModes->comp;
        पक्ष UNORM2_DECOMPOSE:
            प्रत्यागम &allModes->decomp;
        पक्ष UNORM2_FCD:
            प्रत्यागम &allModes->fcd;
        पक्ष UNORM2_COMPOSE_CONTIGUOUS:
            प्रत्यागम &allModes->fcc;
        स्वभाव:
            भङ्ग;  // do nothing
        }
    }
    प्रत्यागम शून्यसूचकः;
}

ध्रुव Normalizer2 *
Normalizer2Factory::getInstance(UNormalization_बहुलकम् बहुलकम्, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    विकल्प(बहुलकम्) {
    पक्ष UNORM_NFD:
        प्रत्यागम Normalizer2::getNFDInstance(error_संहिता);
    पक्ष UNORM_NFKD:
        प्रत्यागम Normalizer2::getNFKDInstance(error_संहिता);
    पक्ष UNORM_NFC:
        प्रत्यागम Normalizer2::getNFCInstance(error_संहिता);
    पक्ष UNORM_NFKC:
        प्रत्यागम Normalizer2::getNFKCInstance(error_संहिता);
    पक्ष UNORM_FCD:
        प्रत्यागम getFCDInstance(error_संहिता);
    स्वभाव:  // UNORM_NONE
        प्रत्यागम getNoopInstance(error_संहिता);
    }
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? allModes->impl : शून्यसूचकः;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UError_संहिता &error_संहिता) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(error_संहिता);
    प्रत्यागम allModes!=शून्यसूचकः ? allModes->impl : शून्यसूचकः;
}

U_NAMESPACE_अन्तः

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UError_संहिता *p_Error_संहिता) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCInstance(*p_Error_संहिता);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UError_संहिता *p_Error_संहिता) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKDInstance(*p_Error_संहिता);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UError_संहिता *p_Error_संहिता) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*p_Error_संहिता);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UError_संहिता *p_Error_संहिता) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*p_Error_संहिता);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getInstance(ध्रुव वर्ण *packageName,
                   ध्रुव वर्ण *name,
                   UNormalization2_बहुलकम् बहुलकम्,
                   UError_संहिता *p_Error_संहिता) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getInstance(packageName, name, बहुलकम्, *p_Error_संहिता);
}

U_CFUNC UNormalization_Check_परिणाम
unorm_get_शीघ्र_Check(UChar32 c, UNormalization_बहुलकम् बहुलकम्) {
    यदि(बहुलकम्<=UNORM_NONE || UNORM_FCD<=बहुलकम्) {
        प्रत्यागम UNORM_YES;
    }
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    ध्रुव Normalizer2 *norm2=Normalizer2Factory::getInstance(बहुलकम्, error_संहिता);
    यदि(U_SUCCESS(error_संहिता)) {
        प्रत्यागम ((ध्रुव Normalizer2WithImpl *)norm2)->get_शीघ्र_Check(c);
    } अन्यथा {
        प्रत्यागम UNORM_MAYBE;
    }
}

#यदि_अन्तः  // !UCONFIG_NO_NORMALIZATION
