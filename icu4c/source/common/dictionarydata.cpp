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

#అన్తర్భావ "dictionarydata.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "cmemory.h"

#ఒకవేళ !UCONFIG_NO_BREAK_ITERATION

U_NAMESPACE_BEGIN

స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRIE_TYPE_బైట్లు = 0;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRIE_TYPE_UCHARS = 1;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRIE_TYPE_ముసుగు = 7;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRIE_HAS_విలువలు = 8;

స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRANSFORM_NONE = 0;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRANSFORM_TYPE_స్థానభ్రంశం = 0x1000000;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRANSFORM_TYPE_ముసుగు = 0x7f000000;
స్థిరరాశి int32_t  Dictionary_దత్తాంశం::TRANSFORM_స్థానభ్రంశం_ముసుగు = 0x1fffff;
    
DictionaryMatcher::~DictionaryMatcher() {
}

UCharsDictionaryMatcher::~UCharsDictionaryMatcher() {
    udata_close(file);
}

int32_t UCharsDictionaryMatcher::getType() స్థిరరాశి {
    తిరిగి Dictionary_దత్తాంశం::TRIE_TYPE_UCHARS;
}

int32_t UCharsDictionaryMatcher::matches(UText *పాఠ్యం, int32_t max_నిడివి_2, int32_t హద్దు,
                            int32_t *lengths, int32_t *cpLengths, int32_t *విలువలు,
                            int32_t *ఉపసర్గ) స్థిరరాశి {

    UCharsTrie uct(characters);
    int32_t starting_పాఠ్యం_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
    int32_t మాట_లెక్క = 0;
    int32_t సంకేతం_Points_Matched = 0;

    కొరకు (UChar32 c = utext_next32(పాఠ్యం); c >= 0; c=utext_next32(పాఠ్యం)) {
        UString_Trie_ఫలితం ఫలితం = (సంకేతం_Points_Matched == 0) ? uct.first(c) : uct.next(c);
        int32_t నిడివి_Matched = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - starting_పాఠ్యం_సూచీ;
        సంకేతం_Points_Matched += 1;
        ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
            ఒకవేళ (మాట_లెక్క < హద్దు) {
                ఒకవేళ (విలువలు != శూన్యసూచిక) {
                    విలువలు[మాట_లెక్క] = uct.get_విలువ();
                }
                ఒకవేళ (lengths != శూన్యసూచిక) {
                    lengths[మాట_లెక్క] = నిడివి_Matched;
                }
                ఒకవేళ (cpLengths != శూన్యసూచిక) {
                    cpLengths[మాట_లెక్క] = సంకేతం_Points_Matched;
                }
                ++మాట_లెక్క;
            }
            ఒకవేళ (ఫలితం == USTRINGTRIE_FINAL_విలువ) {
                ఆపు;
            }
        }
        లేకపోతే ఒకవేళ (ఫలితం == USTRINGTRIE_NO_సరిపోలిక) {
            ఆపు;
        }
        ఒకవేళ (నిడివి_Matched >= max_నిడివి_2) {
            ఆపు;
        }
    }

    ఒకవేళ (ఉపసర్గ != శూన్యసూచిక) {
        *ఉపసర్గ = సంకేతం_Points_Matched;
    }
    తిరిగి మాట_లెక్క;
}

బైట్లు_Dictionary_Matcher::~బైట్లు_Dictionary_Matcher() {
    udata_close(file);
}

UChar32 బైట్లు_Dictionary_Matcher::transform(UChar32 c) స్థిరరాశి {
    ఒకవేళ ((transformConstant & Dictionary_దత్తాంశం::TRANSFORM_TYPE_ముసుగు) == Dictionary_దత్తాంశం::TRANSFORM_TYPE_స్థానభ్రంశం) {
        ఒకవేళ (c == 0x200D) {
            తిరిగి 0xFF;
        } లేకపోతే ఒకవేళ (c == 0x200C) {
            తిరిగి 0xFE;
        }
        int32_t delta = c - (transformConstant & Dictionary_దత్తాంశం::TRANSFORM_స్థానభ్రంశం_ముసుగు);
        ఒకవేళ (delta < 0 || 0xFD < delta) {
            తిరిగి U_SENTINEL;
        }
        తిరిగి static_cast<UChar32>(delta);
    }
    తిరిగి c;
}

int32_t బైట్లు_Dictionary_Matcher::getType() స్థిరరాశి {
    తిరిగి Dictionary_దత్తాంశం::TRIE_TYPE_బైట్లు;
}

int32_t బైట్లు_Dictionary_Matcher::matches(UText *పాఠ్యం, int32_t max_నిడివి_2, int32_t హద్దు,
                            int32_t *lengths, int32_t *cpLengths, int32_t *విలువలు,
                            int32_t *ఉపసర్గ) స్థిరరాశి {
    బైట్లు_Trie bt(characters);
    int32_t starting_పాఠ్యం_సూచీ = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం));
    int32_t మాట_లెక్క = 0;
    int32_t సంకేతం_Points_Matched = 0;

    కొరకు (UChar32 c = utext_next32(పాఠ్యం); c >= 0; c=utext_next32(పాఠ్యం)) {
        UString_Trie_ఫలితం ఫలితం = (సంకేతం_Points_Matched == 0) ? bt.first(transform(c)) : bt.next(transform(c));
        int32_t నిడివి_Matched = static_cast<int32_t>(utext_get_Native_సూచీ(పాఠ్యం)) - starting_పాఠ్యం_సూచీ;
        సంకేతం_Points_Matched += 1;
        ఒకవేళ (USTRINGTRIE_HAS_విలువ(ఫలితం)) {
            ఒకవేళ (మాట_లెక్క < హద్దు) {
                ఒకవేళ (విలువలు != శూన్యసూచిక) {
                    విలువలు[మాట_లెక్క] = bt.get_విలువ();
                }
                ఒకవేళ (lengths != శూన్యసూచిక) {
                    lengths[మాట_లెక్క] = నిడివి_Matched;
                }
                ఒకవేళ (cpLengths != శూన్యసూచిక) {
                    cpLengths[మాట_లెక్క] = సంకేతం_Points_Matched;
                }
                ++మాట_లెక్క;
            }
            ఒకవేళ (ఫలితం == USTRINGTRIE_FINAL_విలువ) {
                ఆపు;
            }
        }
        లేకపోతే ఒకవేళ (ఫలితం == USTRINGTRIE_NO_సరిపోలిక) {
            ఆపు;
        }
        ఒకవేళ (నిడివి_Matched >= max_నిడివి_2) {
            ఆపు;
        }
    }

    ఒకవేళ (ఉపసర్గ != శూన్యసూచిక) {
        *ఉపసర్గ = సంకేతం_Points_Matched;
    }
    తిరిగి మాట_లెక్క;
}


U_NAMESPACE_చివర

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
udict_మార్పిడి(స్థిరరాశి UDataSwapper *ds, స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length,
           శూన్య *బహిర్గతం_దత్తాంశం, UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;
    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;
    స్థిరరాశి int32_t *లోపల_Indexes;
    int32_t indexes[Dictionary_దత్తాంశం::IX_లెక్క];
    int32_t i, స్థానభ్రంశం, size;

    శీర్షిక_కొలత = udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ (p_Error_సంకేతం == శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) తిరిగి 0;
    pInfo = (స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం + 4);
    ఒకవేళ (!(pInfo->దత్తాంశం_Format[0] == 0x44 && 
          pInfo->దత్తాంశం_Format[1] == 0x69 && 
          pInfo->దత్తాంశం_Format[2] == 0x63 && 
          pInfo->దత్తాంశం_Format[3] == 0x74 && 
          pInfo->format_సంస్కరణ[0] == 1)) {
        udata_printError(ds, "udict_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as dictionary data\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1], pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3], pInfo->format_సంస్కరణ[0]);
        *p_Error_సంకేతం = U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు = (స్థిరరాశి uint8_t *)లోపల_దత్తాంశం + శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు = (బహిర్గతం_దత్తాంశం == శూన్యసూచిక) ? శూన్యసూచిక : (uint8_t *)బహిర్గతం_దత్తాంశం + శీర్షిక_కొలత;

    లోపల_Indexes = (స్థిరరాశి int32_t *)లోపల_బైట్లు;
    ఒకవేళ (length >= 0) {
        length -= శీర్షిక_కొలత;
        ఒకవేళ (length < (int32_t)(పరిమాణం(indexes))) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for dictionary data\n", length);
            *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    కొరకు (i = 0; i < Dictionary_దత్తాంశం::IX_లెక్క; i++) {
        indexes[i] = udata_readInt32(ds, లోపల_Indexes[i]);
    }

    size = indexes[Dictionary_దత్తాంశం::IX_మొత్తం_కొలత];

    ఒకవేళ (length >= 0) {
        ఒకవేళ (length < size) {
            udata_printError(ds, "udict_swap(): too few bytes (%d after header) for all of dictionary data\n", length);
            *p_Error_సంకేతం = U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }

        ఒకవేళ (లోపల_బైట్లు != బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, size);
        }

        స్థానభ్రంశం = 0;
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, పరిమాణం(indexes), బహిర్గతం_బైట్లు, p_Error_సంకేతం);
        స్థానభ్రంశం = (int32_t)పరిమాణం(indexes);
        int32_t trieType = indexes[Dictionary_దత్తాంశం::IX_TRIE_TYPE] & Dictionary_దత్తాంశం::TRIE_TYPE_ముసుగు;
        int32_t తదుపరి_స్థానభ్రంశం = indexes[Dictionary_దత్తాంశం::IX_RESERVED1_స్థానభ్రంశం];

        ఒకవేళ (trieType == Dictionary_దత్తాంశం::TRIE_TYPE_UCHARS) {
            ds->మార్పిడి_Array16(ds, లోపల_బైట్లు + స్థానభ్రంశం, తదుపరి_స్థానభ్రంశం - స్థానభ్రంశం, బహిర్గతం_బైట్లు + స్థానభ్రంశం, p_Error_సంకేతం);
        } లేకపోతే ఒకవేళ (trieType == Dictionary_దత్తాంశం::TRIE_TYPE_బైట్లు) {
            // nothing to do
        } లేకపోతే {
            udata_printError(ds, "udict_swap(): unknown trie type!\n");
            *p_Error_సంకేతం = U_UNSUPPORTED_ERROR;
            తిరిగి 0;
        }

        // these next two sections are empty in the current format,
        // but may be used later.
        స్థానభ్రంశం = తదుపరి_స్థానభ్రంశం;
        తదుపరి_స్థానభ్రంశం = indexes[Dictionary_దత్తాంశం::IX_RESERVED2_స్థానభ్రంశం];
        స్థానభ్రంశం = తదుపరి_స్థానభ్రంశం;
        తదుపరి_స్థానభ్రంశం = indexes[Dictionary_దత్తాంశం::IX_మొత్తం_కొలత];
        స్థానభ్రంశం = తదుపరి_స్థానభ్రంశం;
    }
    తిరిగి శీర్షిక_కొలత + size;
}
#అయితే_ముగింపు
