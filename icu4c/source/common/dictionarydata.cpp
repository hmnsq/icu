// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014-2016, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* dictionarydata.h
*
* created on: 2012may31
* created by: Markus W. Scherer & Maxime Serrano
*/

#अन्तर्भाव "dictionarydata.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "cmemory.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

ध्रुव int32_t  Dictionary_दत्त::TRIE_TYPE_अष्टकानि = 0;
ध्रुव int32_t  Dictionary_दत्त::TRIE_TYPE_UCHARS = 1;
ध्रुव int32_t  Dictionary_दत्त::TRIE_TYPE_आवरणम् = 7;
ध्रुव int32_t  Dictionary_दत्त::TRIE_HAS_मानानि = 8;

ध्रुव int32_t  Dictionary_दत्त::TRANSFORM_NONE = 0;
ध्रुव int32_t  Dictionary_दत्त::TRANSFORM_TYPE_विस्थापनम् = 0x1000000;
ध्रुव int32_t  Dictionary_दत्त::TRANSFORM_TYPE_आवरणम् = 0x7f000000;
ध्रुव int32_t  Dictionary_दत्त::TRANSFORM_विस्थापनम्_आवरणम् = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() ध्रुव {
    प्रत्यागम Dictionary_दत्त::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *text, int32_t max_दीर्घता_2, int32_t सीमा,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानानि,
                            int32_t *उपसर्गः) ध्रुव {

    UCharsTrie uct(characters);
    int32_t starting_Text_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t word_गणनम् = 0;
    int32_t संहिता_Points_Matched = 0;

    प्रति (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_परिणाम परिणाम = (संहिता_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t दीर्घता_Matched = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - starting_Text_सूचकांक;
        संहिता_Points_Matched += 1;
        यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
            यदि (word_गणनम् < सीमा) {
                यदि (मानानि != शून्यसूचकः) {
                    मानानि[word_गणनम्] = uct.get_मानम्();
                }
                यदि (lengths != शून्यसूचकः) {
                    lengths[word_गणनम्] = दीर्घता_Matched;
                }
                यदि (cpLengths != शून्यसूचकः) {
                    cpLengths[word_गणनम्] = संहिता_Points_Matched;
                }
                ++word_गणनम्;
            }
            यदि (परिणाम == USTRINGTRIE_FINAL_मानम्) {
                भङ्ग;
            }
        }
        अन्यथा यदि (परिणाम == USTRINGTRIE_NO_मेलनम्) {
            भङ्ग;
        }
        यदि (दीर्घता_Matched >= max_दीर्घता_2) {
            भङ्ग;
        }
    }

    यदि (उपसर्गः != शून्यसूचकः) {
        *उपसर्गः = संहिता_Points_Matched;
    }
    प्रत्यागम word_गणनम्;
}

अष्टकानि_Dictionary_Matcher::~अष्टकानि_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 अष्टकानि_Dictionary_Matcher::transform(UChar32 c) ध्रुव {
    यदि ((transformConstant & Dictionary_दत्त::TRANSFORM_TYPE_आवरणम्) == Dictionary_दत्त::TRANSFORM_TYPE_विस्थापनम्) {
        यदि (c == 0x200D) {
            प्रत्यागम 0xFF;
        } अन्यथा यदि (c == 0x200C) {
            प्रत्यागम 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_दत्त::TRANSFORM_विस्थापनम्_आवरणम्);
        यदि (delta < 0 || 0xFD < delta) {
            प्रत्यागम U_SENTINEL;
        }
        प्रत्यागम static_cast<UChar32>(delta);
    }
    प्रत्यागम c;
}

int32_t अष्टकानि_Dictionary_Matcher::getType() ध्रुव {
    प्रत्यागम Dictionary_दत्त::TRIE_TYPE_अष्टकानि;
}

int32_t अष्टकानि_Dictionary_Matcher::matches(UText *text, int32_t max_दीर्घता_2, int32_t सीमा,
                            int32_t *lengths, int32_t *cpLengths, int32_t *मानानि,
                            int32_t *उपसर्गः) ध्रुव {
    अष्टकानि_Trie bt(characters);
    int32_t starting_Text_सूचकांक = static_cast<int32_t>(utext_get_Native_सूचकांक(text));
    int32_t word_गणनम् = 0;
    int32_t संहिता_Points_Matched = 0;

    प्रति (UChar32 c = utext_next32(text); c >= 0; c=utext_next32(text)) {
        UString_Trie_परिणाम परिणाम = (संहिता_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t दीर्घता_Matched = static_cast<int32_t>(utext_get_Native_सूचकांक(text)) - starting_Text_सूचकांक;
        संहिता_Points_Matched += 1;
        यदि (USTRINGTRIE_HAS_मानम्(परिणाम)) {
            यदि (word_गणनम् < सीमा) {
                यदि (मानानि != शून्यसूचकः) {
                    मानानि[word_गणनम्] = bt.get_मानम्();
                }
                यदि (lengths != शून्यसूचकः) {
                    lengths[word_गणनम्] = दीर्घता_Matched;
                }
                यदि (cpLengths != शून्यसूचकः) {
                    cpLengths[word_गणनम्] = संहिता_Points_Matched;
                }
                ++word_गणनम्;
            }
            यदि (परिणाम == USTRINGTRIE_FINAL_मानम्) {
                भङ्ग;
            }
        }
        अन्यथा यदि (परिणाम == USTRINGTRIE_NO_मेलनम्) {
            भङ्ग;
        }
        यदि (दीर्घता_Matched >= max_दीर्घता_2) {
            भङ्ग;
        }
    }

    यदि (उपसर्गः != शून्यसूचकः) {
        *उपसर्गः = संहिता_Points_Matched;
    }
    प्रत्यागम word_गणनम्;
}


U_NAMESPACE_अन्तः

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_swap(ध्रुव UDataSwapper *ds, ध्रुव शून्य *अन्तर्_दत्त, int32_t length,
           शून्य *बहिर्गमनम्_दत्त, UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;
    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;
    ध्रुव int32_t *अन्तर्_Indexes;
    int32_t indexes[Dictionary_दत्त::IX_गणनम्];
    int32_t i, विस्थापनम्, size;

    शीर्षकम्_आकार = udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि (p_Error_संहिता == शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) प्रत्यागम 0;
    pInfo = (ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त + 4);
    यदि (!(pInfo->दत्त_Format[0] == 0x44 && 
          pInfo->दत्त_Format[1] == 0x69 && 
          pInfo->दत्त_Format[2] == 0x63 && 
          pInfo->दत्त_Format[3] == 0x74 && 
          pInfo->format_संस्करणम्[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1], pInfo->दत्त_Format[2], pInfo->दत्त_Format[3], pInfo->format_संस्करणम्[0]);
        *p_Error_संहिता = U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि = (ध्रुव uint8_t *)अन्तर्_दत्त + शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि = (बहिर्गमनम्_दत्त == शून्यसूचकः) ? शून्यसूचकः : (uint8_t *)बहिर्गमनम्_दत्त + शीर्षकम्_आकार;

    अन्तर्_Indexes = (ध्रुव int32_t *)अन्तर्_अष्टकानि;
    यदि (length >= 0) {
        length -= शीर्षकम्_आकार;
        यदि (length < (int32_t)(परिमाण(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    प्रति (i = 0; i < Dictionary_दत्त::IX_गणनम्; i++) {
        indexes[i] = udata_readInt32(ds, अन्तर्_Indexes[i]);
    }

    size = indexes[Dictionary_दत्त::IX_योग_आकार];

    यदि (length >= 0) {
        यदि (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_संहिता = U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }

        यदि (अन्तर्_अष्टकानि != बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, size);
        }

        विस्थापनम् = 0;
        ds->swapArray32(ds, अन्तर्_अष्टकानि, परिमाण(indexes), बहिर्गमनम्_अष्टकानि, p_Error_संहिता);
        विस्थापनम् = (int32_t)परिमाण(indexes);
        int32_t trieType = indexes[Dictionary_दत्त::IX_TRIE_TYPE] & Dictionary_दत्त::TRIE_TYPE_आवरणम्;
        int32_t अग्र_विस्थापनम् = indexes[Dictionary_दत्त::IX_RESERVED1_विस्थापनम्];

        यदि (trieType == Dictionary_दत्त::TRIE_TYPE_UCHARS) {
            ds->swapArray16(ds, अन्तर्_अष्टकानि + विस्थापनम्, अग्र_विस्थापनम् - विस्थापनम्, बहिर्गमनम्_अष्टकानि + विस्थापनम्, p_Error_संहिता);
        } अन्यथा यदि (trieType == Dictionary_दत्त::TRIE_TYPE_अष्टकानि) {
            // nothing to do
        } अन्यथा {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_संहिता = U_UNSUPPORTED_ERROR;
            प्रत्यागम 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        विस्थापनम् = अग्र_विस्थापनम्;
        अग्र_विस्थापनम् = indexes[Dictionary_दत्त::IX_RESERVED2_विस्थापनम्];
        विस्थापनम् = अग्र_विस्थापनम्;
        अग्र_विस्थापनम् = indexes[Dictionary_दत्त::IX_योग_आकार];
        विस्थापनम् = अग्र_विस्थापनम्;
    }
    प्रत्यागम शीर्षकम्_आकार + size;
}
#यदि_अन्तः
