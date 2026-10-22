// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ubidi.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999jul27
*   created by: Markus W. Scherer, updated by Matitiahu Allouche
*
*/

#అన్తర్భావ <limits>

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/ubidi.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ubidi_props.h"
#అన్తర్భావ "ubidiimp.h"
#అన్తర్భావ "uassert.h"

/*
 * General implementation notes:
 *
 * Throughout the implementation, there are comments like (W2) that refer to
 * rules of the BiDi algorithm, in this example to the second rule of the
 * resolution of weak types.
 *
 * For handling surrogate pairs, where two char16_t's form one "abstract" (or UTF-32)
 * character according to UTF-16, the second char16_t gets the directional property of
 * the entire character assigned, while the first one gets a BN, a boundary
 * neutral, type, which is ignored by most of the algorithm according to
 * rule (X9) and the implementation suggestions of the BiDi algorithm.
 *
 * Later, adjustWSLevels() will set the level for each BN to that of the
 * following character (char16_t), which results in surrogate pairs getting the
 * same level on each of their surrogates.
 *
 * In a UTF-8 implementation, the same thing could be done: the last byte of
 * a multi-byte sequence would get the "real" property, while all previous
 * bytes of that sequence would get BN.
 *
 * It is not possible to assign all those parts of a character the same real
 * property because this would fail in the resolution of weak types with rules
 * that look at immediately surrounding types.
 *
 * As a related topic, this implementation does not remove Boundary Neutral
 * types from the input, but ignores them wherever this is relevant.
 * For example, the loop for the resolution of the weak types reads
 * types until it finds a non-BN.
 * Also, explicit embedding codes are neither changed into BN nor removed.
 * They are only treated the same way real BNs are.
 * As stated before, adjustWSLevels() takes care of them at the end.
 * For the purpose of conformance, the levels of all these codes
 * do not matter.
 *
 * Note that this implementation modifies the dirProps
 * after the initial setup, when applying X5c (replace FSI by LRI or RLI),
 * X6, N0 (replace paired brackets by L or R).
 *
 * In this implementation, the resolution of weak types (W1 to W6),
 * neutrals (N1 and N2), and the assignment of the resolved level (In)
 * are all done in one single loop, in resolveImplicitLevels().
 * Changes of dirProp values are done on the fly, without writing
 * them back to the dirProps array.
 *
 *
 * This implementation contains code that allows to bypass steps of the
 * algorithm that are not needed on the specific paragraph
 * in order to speed up the most common cases considerably,
 * like text that is entirely LTR, or RTL text without numbers.
 *
 * Most of this is done by setting a bit for each directional property
 * in a flags variable and later checking for whether there are
 * any LTR characters or any RTL characters, or both, whether
 * there are any explicit embedding codes, etc.
 *
 * If the (Xn) steps are performed, then the flags are re-evaluated,
 * because they will then not contain the embedding codes any more
 * and will be adjusted for override codes, so that subsequently
 * more bypassing may be possible than what the initial flags suggested.
 *
 * If the text is not mixed-directional, then the
 * algorithm steps for the weak type resolution are not performed,
 * and all levels are set to the paragraph level.
 *
 * If there are no explicit embedding codes, then the (Xn) steps
 * are not performed.
 *
 * If embedding levels are supplied as a parameter, then all
 * explicit embedding codes are ignored, and the (Xn) steps
 * are not performed.
 *
 * White Space types could get the level of the run they belong to,
 * and are checked with a test of (flags&MASK_EMBEDDING) to
 * consider if the paragraph direction should be considered in
 * the flags variable.
 *
 * If there are no White Space types in the paragraph, then
 * (L1) is not necessary in adjustWSLevels().
 */

/* to avoid some conditional statements, use tiny constant arrays */
స్థిర స్థిరరాశి Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
స్థిర స్థిరరాశి Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
స్థిర స్థిరరాశి Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#నిర్వచించు DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#నిర్వచించు DIRPROP_FLAG_E(level)  flagE[(level)&1]
#నిర్వచించు DIRPROP_FLAG_O(level)  flagO[(level)&1]

#నిర్వచించు DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#నిర్వచించు NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    తిరిగి ubidi_openSized(0, 0, &error_సంకేతం);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_నిడివి_2, int32_t max_నిరంతరం_లెక్క, UError_సంకేతం *p_Error_సంకేతం) {
    UBiDi *pBiDi;

    /* check the argument values */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    } లేకపోతే ఒకవేళ(max_నిడివి_2<0 || max_నిరంతరం_లెక్క<0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(పరిమాణం(UBiDi));
    ఒకవేళ(pBiDi==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, పరిమాణం(UBiDi));

    /* allocate memory for arrays as requested */
    ఒకవేళ(max_నిడివి_2>0) {
        ఒకవేళ( !get_Initial_Dir_Props_జ్ఞాపకం(pBiDi, max_నిడివి_2) ||
            !get_Initial_స్థాయిలు_జ్ఞాపకం(pBiDi, max_నిడివి_2)
        ) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    } లేకపోతే {
        pBiDi->may_Allocate_పాఠ్యం=నిజం;
    }

    ఒకవేళ(max_నిరంతరం_లెక్క>0) {
        ఒకవేళ(max_నిరంతరం_లెక్క==1) {
            /* use simpleRuns[] */
            pBiDi->runs_కొలత=పరిమాణం(నిరంతరం);
        } లేకపోతే ఒకవేళ(!get_Initial_Runs_జ్ఞాపకం(pBiDi, max_నిరంతరం_లెక్క)) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    } లేకపోతే {
        pBiDi->mayAllocateRuns=నిజం;
    }

    ఒకవేళ(U_SUCCESS(*p_Error_సంకేతం)) {
        తిరిగి pBiDi;
    } లేకపోతే {
        ubidi_close(pBiDi);
        తిరిగి శూన్యసూచిక;
    }
}

/*
 * We are allowed to allocate memory if memory==nullptr or
 * mayAllocate==true for each array that we need.
 * We also try to grow memory as needed if we
 * allocate it.
 *
 * Assume sizeNeeded>0.
 * If *pMemory!=nullptr, then assume *pSize>0.
 *
 * ### this realloc() may unnecessarily copy the old data,
 * which we know we don't need any more;
 * is this the best way to do this??
 */
U_CFUNC UBool
ubidi_get_జ్ఞాపకం(Bidi_జ్ఞాపకం_For_కేటాయింపు *bidiMem, int32_t *p_కొలత, UBool mayAllocate, size_t కొలత_Needed) {
    ఒకవేళ (కొలత_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      తిరిగి అబద్ధం;
    }
    శూన్య **p_జ్ఞాపకం = (శూన్య **)bidiMem;
    /* check for existing memory */
    ఒకవేళ(*p_జ్ఞాపకం==శూన్యసూచిక) {
        /* we need to allocate memory */
        ఒకవేళ(mayAllocate && (*p_జ్ఞాపకం=uprv_malloc(కొలత_Needed))!=శూన్యసూచిక) {
            *p_కొలత=static_cast<int32_t>(కొలత_Needed);
            తిరిగి నిజం;
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    } లేకపోతే {
        ఒకవేళ(కొలత_Needed<=static_cast<size_t>(*p_కొలత)) {
            /* there is already enough memory */
            తిరిగి నిజం;
        }
        లేకపోతే ఒకవేళ(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            తిరిగి అబద్ధం;
        } లేకపోతే {
            /* we try to grow */
            శూన్య *జ్ఞాపకం;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            ఒకవేళ((జ్ఞాపకం=uprv_realloc(*p_జ్ఞాపకం, కొలత_Needed))!=శూన్యసూచిక) {
                *p_జ్ఞాపకం=జ్ఞాపకం;
                *p_కొలత=static_cast<int32_t>(కొలత_Needed);
                తిరిగి నిజం;
            } లేకపోతే {
                /* we failed to grow */
                తిరిగి అబద్ధం;
            }
        }
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    ఒకవేళ(pBiDi!=శూన్యసూచిక) {
        pBiDi->pParaBiDi=శూన్యసూచిక;          /* in case one tries to reuse this block */
        ఒకవేళ(pBiDi->dir_Props_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->dir_Props_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->స్థాయిలు_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->స్థాయిలు_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->openings_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->openings_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->paras_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->paras_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->runs_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->runs_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->isolates_జ్ఞాపకం!=శూన్యసూచిక) {
            uprv_free(pBiDi->isolates_జ్ఞాపకం);
        }
        ఒకవేళ(pBiDi->చొప్పించు_Points.points!=శూన్యసూచిక) {
            uprv_free(pBiDi->చొప్పించు_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Inverse(UBiDi *pBiDi, UBool isInverse) {
    ఒకవేళ(pBiDi!=శూన్యసూచిక) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_బహుళకం = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    ఒకవేళ(pBiDi!=శూన్యసూచిక) {
        తిరిగి pBiDi->isInverse;
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

/* FOOD FOR THOUGHT: currently the reordering modes are a mixture of
 * algorithm for direct BiDi, algorithm for inverse BiDi and the bizarre
 * concept of RUNS_ONLY which is a double operation.
 * It could be advantageous to divide this into 3 concepts:
 * a) Operation: direct / inverse / RUNS_ONLY
 * b) Direct algorithm: default / NUMBERS_SPECIAL / GROUP_NUMBERS_WITH_R
 * c) Inverse algorithm: default / INVERSE_LIKE_DIRECT / NUMBERS_SPECIAL
 * This would allow combinations not possible today like RUNS_ONLY with
 * NUMBERS_SPECIAL.
 * Also allow to set INSERT_MARKS for the direct step of RUNS_ONLY and
 * REMOVE_CONTROLS for the inverse step.
 * Not all combinations would be supported, and probably not all do make sense.
 * This would need to document which ones are supported and what are the
 * fallbacks for unsupported combinations.
 */
U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Reordering_బహుళకం(UBiDi *pBiDi, UBi_Di_Reordering_బహుళకం reordering_బహుళకం) UPRV_NO_SANITIZE_UNDEFINED {
    ఒకవేళ ((pBiDi!=శూన్యసూచిక) && (reordering_బహుళకం >= UBIDI_REORDER_DEFAULT)
                        && (reordering_బహుళకం < UBIDI_REORDER_లెక్క)) {
        pBiDi->reordering_బహుళకం = reordering_బహుళకం;
        pBiDi->isInverse = reordering_బహుళకం == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_బహుళకం U_EXPORT2
ubidi_get_Reordering_బహుళకం(UBiDi *pBiDi) {
    ఒకవేళ (pBiDi!=శూన్యసూచిక) {
        తిరిగి pBiDi->reordering_బహుళకం;
    } లేకపోతే {
        తిరిగి UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    ఒకవేళ (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_చొప్పించు_MARKS;
    }
    ఒకవేళ (pBiDi!=శూన్యసూచిక) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    ఒకవేళ (pBiDi!=శూన్యసూచిక) {
        తిరిగి pBiDi->reorderingOptions;
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_పునాది_Direction(స్థిరరాశి char16_t *పాఠ్యం,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    ఒకవేళ( పాఠ్యం==శూన్యసూచిక || length<-1 ){
        తిరిగి UBIDI_NEUTRAL;
    }

    ఒకవేళ(length==-1) {
        length=u_strlen(పాఠ్యం);
    }

    కొరకు( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_తదుపరి(పాఠ్యం, i, length, uchar);
        dir = u_charDirection(uchar);
        ఒకవేళ( dir == U_ఎడమ_గమ్యం_కుడి )
                తిరిగి UBIDI_LTR;
        ఒకవేళ( dir == U_కుడి_గమ్యం_ఎడమ || dir ==U_కుడి_గమ్యం_ఎడమ_ARABIC )
                తిరిగి UBIDI_RTL;
    }
    తిరిగి UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
స్థిర DirProp
మొదటి_L_R_AL(UBiDi *pBiDi) {
    స్థిరరాశి char16_t *పాఠ్యం=pBiDi->prologue;
    int32_t length=pBiDi->pro_నిడివి;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, ఫలితం=ON;
    కొరకు(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_తదుపరి(పాఠ్యం, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ఒకవేళ(ఫలితం==ON) {
            ఒకవేళ(dirProp==L || dirProp==R || dirProp==AL) {
                ఫలితం=dirProp;
            }
        } లేకపోతే {
            ఒకవేళ(dirProp==B) {
                ఫలితం=ON;
            }
        }
    }
    తిరిగి ఫలితం;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
స్థిర UBool
check_Para_లెక్క(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_లెక్క;
    ఒకవేళ(pBiDi->paras==pBiDi->simpleParas) {
        ఒకవేళ(count<=SIMPLE_PARAS_లెక్క)
            తిరిగి నిజం;
        ఒకవేళ(!get_Initial_Paras_జ్ఞాపకం(pBiDi, SIMPLE_PARAS_లెక్క * 2))
            తిరిగి అబద్ధం;
        pBiDi->paras=pBiDi->paras_జ్ఞాపకం;
        uprv_memcpy(pBiDi->paras_జ్ఞాపకం, pBiDi->simpleParas, SIMPLE_PARAS_లెక్క * పరిమాణం(Para));
        తిరిగి నిజం;
    }
    ఒకవేళ(!get_Initial_Paras_జ్ఞాపకం(pBiDi, count * 2))
        తిరిగి అబద్ధం;
    pBiDi->paras=pBiDi->paras_జ్ఞాపకం;
    తిరిగి నిజం;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
స్థిర UBool
getDirProps(UBiDi *pBiDi) {
    స్థిరరాశి char16_t *పాఠ్యం=pBiDi->పాఠ్యం;
    DirProp *dirProps=pBiDi->dir_Props_జ్ఞాపకం;    /* pBiDi->dirProps is const */

    int32_t i=0, original_నిడివి=pBiDi->original_నిడివి;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_బహుళకం == UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష ||
            pBiDi->reordering_బహుళకం == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t చివరి_Arabic_Pos=-1;
    int32_t control_లెక్క=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    గణన State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp చివరి_Strong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_ప్రారంభం_దొంతర[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  మునుపటి_State_దొంతర[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t దొంతర_చివరి=-1;

    ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    ఒకవేళ(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        చివరి_Strong=defaultParaLevel;
        ఒకవేళ(pBiDi->pro_నిడివి>0 &&                    /* there is a prologue */
           (dirProp=మొదటి_L_R_AL(pBiDi))!=ON) {  /* with a strong character */
            ఒకవేళ(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            లేకపోతే
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } లేకపోతే {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } లేకపోతే {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    కొరకు( /* i=0 above */ ; i<original_నిడివి; ) {
        /* i is incremented by U16_NEXT */
        U16_తదుపరి(పాఠ్యం, i, original_నిడివి, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        ఒకవేళ(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        ఒకవేళ(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_లెక్క++;
        ఒకవేళ(dirProp==L) {
            ఒకవేళ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_లెక్క-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            లేకపోతే ఒకవేళ(state==SEEKING_STRONG_FOR_FSI) {
                ఒకవేళ(దొంతర_చివరి<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            చివరి_Strong=L;
            కొనసాగు;
        }
        ఒకవేళ(dirProp==R || dirProp==AL) {
            ఒకవేళ(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_లెక్క-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            లేకపోతే ఒకవేళ(state==SEEKING_STRONG_FOR_FSI) {
                ఒకవేళ(దొంతర_చివరి<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_ప్రారంభం_దొంతర[దొంతర_చివరి]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            చివరి_Strong=R;
            ఒకవేళ(dirProp==AL)
                చివరి_Arabic_Pos=i-1;
            కొనసాగు;
        }
        ఒకవేళ(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            దొంతర_చివరి++;
            ఒకవేళ(దొంతర_చివరి<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_ప్రారంభం_దొంతర[దొంతర_చివరి]=i-1;
                మునుపటి_State_దొంతర[దొంతర_చివరి]=state;
            }
            ఒకవేళ(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            లేకపోతే
                state=LOOKING_FOR_PDI;
            కొనసాగు;
        }
        ఒకవేళ(dirProp==PDI) {
            ఒకవేళ(state==SEEKING_STRONG_FOR_FSI) {
                ఒకవేళ(దొంతర_చివరి<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            ఒకవేళ(దొంతర_చివరి>=0) {
                ఒకవేళ(దొంతర_చివరి<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=మునుపటి_State_దొంతర[దొంతర_చివరి];
                దొంతర_చివరి--;
            }
            కొనసాగు;
        }
        ఒకవేళ(dirProp==B) {
            ఒకవేళ(i<original_నిడివి && uchar==CR && పాఠ్యం[i]==LF) /* do nothing on the CR */
                కొనసాగు;
            pBiDi->paras[pBiDi->para_లెక్క-1].హద్దు=i;
            ఒకవేళ(isDefaultLevelInverse && చివరి_Strong==R)
                pBiDi->paras[pBiDi->para_లెక్క-1].level=1;
            ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_లెక్క=control_లెక్క;
            }
            ఒకవేళ(i<original_నిడివి) {              /* B not last char in text */
                pBiDi->para_లెక్క++;
                ఒకవేళ(check_Para_లెక్క(pBiDi)==అబద్ధం)    /* not enough memory for a new para entry */
                    తిరిగి అబద్ధం;
                ఒకవేళ(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_లెక్క-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    చివరి_Strong=defaultParaLevel;
                } లేకపోతే {
                    pBiDi->paras[pBiDi->para_లెక్క-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                దొంతర_చివరి=-1;
            }
            కొనసాగు;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    ఒకవేళ(దొంతర_చివరి>UBIDI_MAX_EXPLICIT_LEVEL) {
        దొంతర_చివరి=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    అయితే(దొంతర_చివరి>=0) {
        ఒకవేళ(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            ఆపు;
        }
        state=మునుపటి_State_దొంతర[దొంతర_చివరి];
        దొంతర_చివరి--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        ఒకవేళ(pBiDi->length<original_నిడివి)
            pBiDi->para_లెక్క--;
    } లేకపోతే {
        pBiDi->paras[pBiDi->para_లెక్క-1].హద్దు=original_నిడివి;
        pBiDi->control_లెక్క=control_లెక్క;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    ఒకవేళ(isDefaultLevelInverse && చివరి_Strong==R) {
        pBiDi->paras[pBiDi->para_లెక్క-1].level=1;
    }
    ఒకవేళ(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    కొరకు(i=0; i<pBiDi->para_లెక్క; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    ఒకవేళ(pBiDi->క్రమం_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->చివరి_Arabic_Pos=చివరి_Arabic_Pos;
    తిరిగి నిజం;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_సూచీ(స్థిరరాశి UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    కొరకు(i=0; i<pBiDi->para_లెక్క; i++)
        ఒకవేళ(pindex<pBiDi->paras[i].హద్దు)
            ఆపు;
    ఒకవేళ(i>=pBiDi->para_లెక్క)
        i=pBiDi->para_లెక్క-1;
    తిరిగి (UBiDiLevel)(pBiDi->paras[i].level);
}

/* Functions for handling paired brackets ----------------------------------- */

/* In the isoRuns array, the first entry is used for text outside of any
   isolate sequence.  Higher entries are used for each more deeply nested
   isolate sequence. isoRunLast is the index of the last used entry.  The
   openings array is used to note the data of opening brackets not yet
   matched by a closing bracket, or matched but still susceptible to change
   level.
   Each isoRun entry contains the index of the first and
   one-after-last openings entries for pending opening brackets it
   contains.  The next openings entry to use is the one-after-last of the
   most deeply nested isoRun entry.
   isoRun entries also contain their current embedding level and the last
   encountered strong character, since these will be needed to resolve
   the level of paired brackets.  */

స్థిర శూన్య
bracketInit(UBiDi *pBiDi, Bracket_దత్తాంశం *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_నిరంతరం_చివరి=0;
    bd->isoRuns[0].ప్రారంభం=0;
    bd->isoRuns[0].హద్దు=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].చివరి_Strong = bd->isoRuns[0].చివరి_పునాది = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    ఒకవేళ(pBiDi->openings_జ్ఞాపకం) {
        bd->openings=pBiDi->openings_జ్ఞాపకం;
        bd->openings_లెక్క=pBiDi->openings_కొలత / పరిమాణం(Opening);
    } లేకపోతే {
        bd->openings=bd->simpleOpenings;
        bd->openings_లెక్క=SIMPLE_OPENINGS_లెక్క;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_బహుళకం==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_బహుళకం==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
స్థిర శూన్య
bracketProcessB(Bracket_దత్తాంశం *bd, UBiDiLevel level) {
    bd->iso_నిరంతరం_చివరి=0;
    bd->isoRuns[0].హద్దు=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].చివరి_Strong=bd->isoRuns[0].చివరి_పునాది=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
స్థిర శూన్య
bracketProcessBoundary(Bracket_దత్తాంశం *bd, int32_t చివరి_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    DirProp *dirProps=bd->pBiDi->dirProps;
    ఒకవేళ(DIRPROP_FLAG(dirProps[చివరి_Cc_Pos])&ముసుగు_ISO)  /* after an isolate */
        తిరిగి;
    ఒకవేళ(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_చివరి_Iso_నిరంతరం->హద్దు=p_చివరి_Iso_నిరంతరం->ప్రారంభం;
    p_చివరి_Iso_నిరంతరం->level=embeddingLevel;
    p_చివరి_Iso_నిరంతరం->చివరి_Strong=p_చివరి_Iso_నిరంతరం->చివరి_పునాది=contextLevel&1;
    p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_చివరి_Iso_నిరంతరం->contextPos = static_cast<UBiDiDirection>(చివరి_Cc_Pos);
}

/* LRI or RLI */
స్థిర శూన్య
bracketProcessLRI_RLI(Bracket_దత్తాంశం *bd, UBiDiLevel level) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    int16_t చివరి_హద్దు;
    p_చివరి_Iso_నిరంతరం->చివరి_పునాది=ON;
    చివరి_హద్దు=p_చివరి_Iso_నిరంతరం->హద్దు;
    bd->iso_నిరంతరం_చివరి++;
    p_చివరి_Iso_నిరంతరం++;
    p_చివరి_Iso_నిరంతరం->ప్రారంభం=p_చివరి_Iso_నిరంతరం->హద్దు=చివరి_హద్దు;
    p_చివరి_Iso_నిరంతరం->level=level;
    p_చివరి_Iso_నిరంతరం->చివరి_Strong=p_చివరి_Iso_నిరంతరం->చివరి_పునాది=level&1;
    p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_చివరి_Iso_నిరంతరం->contextPos=0;
}

/* PDI */
స్థిర శూన్య
bracketProcessPDI(Bracket_దత్తాంశం *bd) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం;
    bd->iso_నిరంతరం_చివరి--;
    p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    p_చివరి_Iso_నిరంతరం->చివరి_పునాది=ON;
}

/* newly found opening bracket: create an openings entry */
స్థిర UBool                            /* return true if success */
bracket_కూడు_Opening(Bracket_దత్తాంశం *bd, char16_t సరిపోలిక, int32_t position) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    Opening *pOpening;
    ఒకవేళ(p_చివరి_Iso_నిరంతరం->హద్దు>=bd->openings_లెక్క) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        ఒకవేళ(!get_Initial_Openings_జ్ఞాపకం(pBiDi, p_చివరి_Iso_నిరంతరం->హద్దు * 2))
            తిరిగి అబద్ధం;
        ఒకవేళ(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_జ్ఞాపకం, bd->simpleOpenings,
                        SIMPLE_OPENINGS_లెక్క * పరిమాణం(Opening));
        bd->openings=pBiDi->openings_జ్ఞాపకం;     /* may have changed */
        bd->openings_లెక్క=pBiDi->openings_కొలత / పరిమాణం(Opening);
    }
    pOpening=&bd->openings[p_చివరి_Iso_నిరంతరం->హద్దు];
    pOpening->position=position;
    pOpening->సరిపోలిక=సరిపోలిక;
    pOpening->contextDir=p_చివరి_Iso_నిరంతరం->contextDir;
    pOpening->contextPos=p_చివరి_Iso_నిరంతరం->contextPos;
    pOpening->flags=0;
    p_చివరి_Iso_నిరంతరం->హద్దు++;
    తిరిగి నిజం;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
స్థిర శూన్య
fixN0c(Bracket_దత్తాంశం *bd, int32_t opening_సూచీ, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    కొరకు(k=opening_సూచీ+1, qOpening=&bd->openings[k]; k<p_చివరి_Iso_నిరంతరం->హద్దు; k++, qOpening++) {
        ఒకవేళ(qOpening->సరిపోలిక>=0)      /* not an N0c match */
            కొనసాగు;
        ఒకవేళ(newPropPosition<qOpening->contextPos)
            ఆపు;
        ఒకవేళ(newPropPosition>=qOpening->position)
            కొనసాగు;
        ఒకవేళ(newProp==qOpening->contextDir)
            ఆపు;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->సరిపోలిక);
        dirProps[closingPosition]=newProp;
        qOpening->సరిపోలిక=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
స్థిర DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_దత్తాంశం *bd, int32_t openIdx, int32_t position) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_చివరి_Iso_నిరంతరం->level & 1);
    stable=నిజం;            /* assume stable until proved otherwise */

    /* The stable flag is set when brackets are paired and their
       level is resolved and cannot be changed by what will be
       found later in the source string.
       An unstable match can occur only when applying N0c, where
       the resolved level depends on the preceding context, and
       this context may be affected by text occurring later.
       Example: RTL paragraph containing:  abc[(latin) HEBREW]
       When the closing parenthesis is encountered, it appears
       that N0c1 must be applied since 'abc' sets an opposite
       direction context and both parentheses receive level 2.
       However, when the closing square bracket is processed,
       N0b applies because of 'HEBREW' being included within the
       brackets, thus the square brackets are treated like R and
       receive level 1. However, this changes the preceding
       context of the opening parenthesis, and it now appears
       that N0c2 must be applied to the parentheses rather than
       N0c1. */

    ఒకవేళ((direction==0 && pOpening->flags&దొరికింది_L) ||
       (direction==1 && pOpening->flags&దొరికింది_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    లేకపోతే ఒకవేళ(pOpening->flags&(దొరికింది_L|దొరికింది_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_చివరి_Iso_నిరంతరం->ప్రారంభం);
        ఒకవేళ(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        లేకపోతే
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } లేకపోతే {
        /* forget this and any brackets nested within this pair */
        p_చివరి_Iso_నిరంతరం->హద్దు= static_cast<uint16_t>(openIdx);
        తిరిగి ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    ఒకవేళ(stable) {
        p_చివరి_Iso_నిరంతరం->హద్దు= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        అయితే(p_చివరి_Iso_నిరంతరం->హద్దు>p_చివరి_Iso_నిరంతరం->ప్రారంభం &&
              bd->openings[p_చివరి_Iso_నిరంతరం->హద్దు-1].position==pOpening->position)
            p_చివరి_Iso_నిరంతరం->హద్దు--;
    } లేకపోతే {
        int32_t k;
        pOpening->సరిపోలిక=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        అయితే(k>=p_చివరి_Iso_నిరంతరం->ప్రారంభం &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].సరిపోలిక=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        కొరకు(k=openIdx+1; k<p_చివరి_Iso_నిరంతరం->హద్దు; k++) {
            qOpening=&bd->openings[k];
            ఒకవేళ(qOpening->position>=position)
                ఆపు;
            ఒకవేళ(qOpening->సరిపోలిక>0)
                qOpening->సరిపోలిక=0;
        }
    }
    తిరిగి newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
స్థిర UBool                            /* return true if success */
bracketProcessChar(Bracket_దత్తాంశం *bd, int32_t position) {
    Iso_నిరంతరం *p_చివరి_Iso_నిరంతరం=&bd->isoRuns[bd->iso_నిరంతరం_చివరి];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    ఒకవేళ(dirProp==ON) {
        char16_t c, సరిపోలిక;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->పాఠ్యం[position];
        కొరకు(idx=p_చివరి_Iso_నిరంతరం->హద్దు-1; idx>=p_చివరి_Iso_నిరంతరం->ప్రారంభం; idx--) {
            ఒకవేళ(bd->openings[idx].సరిపోలిక!=c)
                కొనసాగు;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            ఒకవేళ(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                ఆపు;
            }
            p_చివరి_Iso_నిరంతరం->చివరి_పునాది=ON;
            p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(newProp);
            p_చివరి_Iso_నిరంతరం->contextPos=position;
            level=bd->pBiDi->స్థాయిలు[position];
            ఒకవేళ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_చివరి_Iso_నిరంతరం->చివరి_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                కొరకు(i=p_చివరి_Iso_నిరంతరం->ప్రారంభం; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->స్థాయిలు[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->స్థాయిలు[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            తిరిగి నిజం;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        ఒకవేళ(c)
            సరిపోలిక= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        లేకపోతే
            సరిపోలిక=0;
        ఒకవేళ(సరిపోలిక!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            ఒకవేళ(సరిపోలిక==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                ఒకవేళ(!bracket_కూడు_Opening(bd, 0x3009, position))
                    తిరిగి అబద్ధం;
            }
            లేకపోతే ఒకవేళ(సరిపోలిక==0x3009) {         /* RIGHT ANGLE BRACKET */
                ఒకవేళ(!bracket_కూడు_Opening(bd, 0x232A, position))
                    తిరిగి అబద్ధం;
            }
            ఒకవేళ(!bracket_కూడు_Opening(bd, సరిపోలిక, position))
                తిరిగి అబద్ధం;
        }
    }
    level=bd->pBiDi->స్థాయిలు[position];
    ఒకవేళ(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        ఒకవేళ(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_చివరి_Iso_నిరంతరం->చివరి_పునాది=newProp;
        p_చివరి_Iso_నిరంతరం->చివరి_Strong=newProp;
        p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(newProp);
        p_చివరి_Iso_నిరంతరం->contextPos=position;
    }
    లేకపోతే ఒకవేళ(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_చివరి_Iso_నిరంతరం->చివరి_పునాది=dirProp;
        p_చివరి_Iso_నిరంతరం->చివరి_Strong=dirProp;
        p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(newProp);
        p_చివరి_Iso_నిరంతరం->contextPos=position;
    }
    లేకపోతే ఒకవేళ(dirProp==EN) {
        p_చివరి_Iso_నిరంతరం->చివరి_పునాది=EN;
        ఒకవేళ(p_చివరి_Iso_నిరంతరం->చివరి_Strong==L) {
            newProp=L;                  /* W7 */
            ఒకవేళ(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(L);
            p_చివరి_Iso_నిరంతరం->contextPos=position;
        }
        లేకపోతే {
            newProp=R;                  /* N0 */
            ఒకవేళ(p_చివరి_Iso_నిరంతరం->చివరి_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            లేకపోతే
                dirProps[position]=ENR;
            p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(R);
            p_చివరి_Iso_నిరంతరం->contextPos=position;
        }
    }
    లేకపోతే ఒకవేళ(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_చివరి_Iso_నిరంతరం->చివరి_పునాది=AN;
        p_చివరి_Iso_నిరంతరం->contextDir = static_cast<UBiDiDirection>(R);
        p_చివరి_Iso_నిరంతరం->contextPos=position;
    }
    లేకపోతే ఒకవేళ(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_చివరి_Iso_నిరంతరం->చివరి_పునాది;
        ఒకవేళ(newProp==ON)
            dirProps[position]=newProp;
    }
    లేకపోతే {
        newProp=dirProp;
        p_చివరి_Iso_నిరంతరం->చివరి_పునాది=dirProp;
    }
    ఒకవేళ(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        కొరకు(i=p_చివరి_Iso_నిరంతరం->ప్రారంభం; i<p_చివరి_Iso_నిరంతరం->హద్దు; i++)
            ఒకవేళ(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    తిరిగి నిజం;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
స్థిర UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    ఒకవేళ(!(flags&ముసుగు_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&ముసుగు_POSSIBLE_N)))) {
        తిరిగి UBIDI_LTR;
    } లేకపోతే ఒకవేళ(!(flags&ముసుగు_LTR)) {
        తిరిగి UBIDI_RTL;
    } లేకపోతే {
        తిరిగి UBIDI_MIXED;
    }
}

/*
 * Resolve the explicit levels as specified by explicit embedding codes.
 * Recalculate the flags to have them reflect the real properties
 * after taking the explicit embeddings into account.
 *
 * The BiDi algorithm is designed to result in the same behavior whether embedding
 * levels are externally specified (from "styled text", supposedly the preferred
 * method) or set by explicit embedding codes (LRx, RLx, PDF, FSI, PDI) in the plain text.
 * That is why (X9) instructs to remove all not-isolate explicit codes (and BN).
 * However, in a real implementation, the removal of these codes and their index
 * positions in the plain text is undesirable since it would result in
 * reallocated, reindexed text.
 * Instead, this implementation leaves the codes in there and just ignores them
 * in the subsequent processing.
 * In order to get the same reordering behavior, positions with a BN or a not-isolate
 * explicit embedding code just get the same level assigned as the last "real"
 * character.
 *
 * Some implementations, not this one, then overwrite some of these
 * directionality properties at "real" same-level-run boundaries by
 * L or R codes so that the resolution of weak types can be performed on the
 * entire paragraph at once instead of having to parse it once more and
 * perform that resolution on same-level-runs.
 * This limits the scope of the implicit rules in effectively
 * the same way as the run limits.
 *
 * Instead, this implementation does not modify these codes, except for
 * paired brackets whose properties (ON) may be replaced by L or R.
 * On one hand, the paragraph has to be scanned for same-level-runs, but
 * on the other hand, this saves another loop to reset these codes,
 * or saves making and modifying a copy of dirProps[].
 *
 *
 * Note that (Pn) and (Xn) changed significantly from version 4 of the BiDi algorithm.
 *
 *
 * Handling the stack of explicit levels (Xn):
 *
 * With the BiDi stack of explicit levels, as pushed with each
 * LRE, RLE, LRO, RLO, LRI, RLI and FSI and popped with each PDF and PDI,
 * the explicit level must never exceed UBIDI_MAX_EXPLICIT_LEVEL.
 *
 * In order to have a correct push-pop semantics even in the case of overflows,
 * overflow counters and a valid isolate counter are used as described in UAX#9
 * section 3.3.2 "Explicit Levels and Directions".
 *
 * This implementation assumes that UBIDI_MAX_EXPLICIT_LEVEL is odd.
 *
 * Returns normally the direction; -1 if there was a memory shortage
 *
 */
స్థిర UBiDiDirection
resolve_Explicit_స్థాయిలు(UBiDi *pBiDi, UError_సంకేతం *p_Error_సంకేతం) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
    స్థిరరాశి char16_t *పాఠ్యం=pBiDi->పాఠ్యం;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_లెక్క=0;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) { తిరిగి UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    ఒకవేళ((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        తిరిగి direction;
    }
    ఒకవేళ(pBiDi->reordering_బహుళకం > UBIDI_REORDER_చివరి_LOGICAL_గమ్యం_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_సూచీ, ప్రారంభం, హద్దు;
        కొరకు(para_సూచీ=0; para_సూచీ<pBiDi->para_లెక్క; para_సూచీ++) {
            ఒకవేళ(para_సూచీ==0)
                ప్రారంభం=0;
            లేకపోతే
                ప్రారంభం=pBiDi->paras[para_సూచీ-1].హద్దు;
            హద్దు=pBiDi->paras[para_సూచీ].హద్దు;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_సూచీ].level);
            కొరకు(i=ప్రారంభం; i<హద్దు; i++)
                స్థాయిలు[i]=level;
        }
        తిరిగి direction;   /* no bracket matching for inverse BiDi */
    }
    ఒకవేళ(!(flags&(ముసుగు_EXPLICIT|ముసుగు_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_సూచీ, ప్రారంభం, హద్దు;
        Bracket_దత్తాంశం bracket_దత్తాంశం;
        bracketInit(pBiDi, &bracket_దత్తాంశం);
        కొరకు(para_సూచీ=0; para_సూచీ<pBiDi->para_లెక్క; para_సూచీ++) {
            ఒకవేళ(para_సూచీ==0)
                ప్రారంభం=0;
            లేకపోతే
                ప్రారంభం=pBiDi->paras[para_సూచీ-1].హద్దు;
            హద్దు=pBiDi->paras[para_సూచీ].హద్దు;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_సూచీ].level);
            కొరకు(i=ప్రారంభం; i<హద్దు; i++) {
                స్థాయిలు[i]=level;
                dirProp=dirProps[i];
                ఒకవేళ(dirProp==BN)
                    కొనసాగు;
                ఒకవేళ(dirProp==B) {
                    ఒకవేళ((i+1)<length) {
                        ఒకవేళ(పాఠ్యం[i]==CR && పాఠ్యం[i+1]==LF)
                            కొనసాగు;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_దత్తాంశం, level);
                    }
                    కొనసాగు;
                }
                ఒకవేళ(!bracketProcessChar(&bracket_దత్తాంశం, i)) {
                    *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    తిరిగి UBIDI_LTR;
                }
            }
        }
        తిరిగి direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel మునుపటి_Level=level;     /* previous level for regular (not CC) characters */
        int32_t చివరి_Cc_Pos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t దొంతర_2[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t దొంతర_చివరి=0;
        int32_t అధికప్రవాహం_Isolate_లెక్క=0;
        int32_t అధికప్రవాహం_Embedding_లెక్క=0;
        int32_t valid_Isolate_లెక్క=0;
        Bracket_దత్తాంశం bracket_దత్తాంశం;
        bracketInit(pBiDi, &bracket_దత్తాంశం);
        దొంతర_2[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        కొరకు(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            ఎంపిక(dirProp) {
            స్థితి LRE:
            స్థితి RLE:
            స్థితి LRO:
            స్థితి RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                స్థాయిలు[i]=మునుపటి_Level;
                ఒకవేళ (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                లేకపోతే
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ఒకవేళ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && అధికప్రవాహం_Isolate_లెక్క==0 &&
                                                         అధికప్రవాహం_Embedding_లెక్క==0) {
                    చివరి_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    ఒకవేళ(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    దొంతర_చివరి++;
                    దొంతర_2[దొంతర_చివరి]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } లేకపోతే {
                    ఒకవేళ(అధికప్రవాహం_Isolate_లెక్క==0)
                        అధికప్రవాహం_Embedding_లెక్క++;
                }
                ఆపు;
            స్థితి PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                స్థాయిలు[i]=మునుపటి_Level;
                /* handle all the overflow cases first */
                ఒకవేళ(అధికప్రవాహం_Isolate_లెక్క) {
                    ఆపు;
                }
                ఒకవేళ(అధికప్రవాహం_Embedding_లెక్క) {
                    అధికప్రవాహం_Embedding_లెక్క--;
                    ఆపు;
                }
                ఒకవేళ(దొంతర_చివరి>0 && దొంతర_2[దొంతర_చివరి]<ISOLATE) {   /* not an isolate entry */
                    చివరి_Cc_Pos=i;
                    దొంతర_చివరి--;
                    embeddingLevel = static_cast<UBiDiLevel>(దొంతర_2[దొంతర_చివరి]);
                }
                ఆపు;
            స్థితి LRI:
            స్థితి RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                స్థాయిలు[i]=NO_OVERRIDE(embeddingLevel);
                ఒకవేళ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(మునుపటి_Level)) {
                    bracketProcessBoundary(&bracket_దత్తాంశం, చివరి_Cc_Pos,
                                           మునుపటి_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                మునుపటి_Level=embeddingLevel;
                /* (X5a, X5b) */
                ఒకవేళ(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                లేకపోతే
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                ఒకవేళ(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && అధికప్రవాహం_Isolate_లెక్క==0 &&
                                                         అధికప్రవాహం_Embedding_లెక్క==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    చివరి_Cc_Pos=i;
                    valid_Isolate_లెక్క++;
                    ఒకవేళ(valid_Isolate_లెక్క>pBiDi->isolate_లెక్క)
                        pBiDi->isolate_లెక్క=valid_Isolate_లెక్క;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    దొంతర_చివరి++;
                    దొంతర_2[దొంతర_చివరి]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_దత్తాంశం, embeddingLevel);
                } లేకపోతే {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    అధికప్రవాహం_Isolate_లెక్క++;
                }
                ఆపు;
            స్థితి PDI:
                ఒకవేళ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(మునుపటి_Level)) {
                    bracketProcessBoundary(&bracket_దత్తాంశం, చివరి_Cc_Pos,
                                           మునుపటి_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                ఒకవేళ(అధికప్రవాహం_Isolate_లెక్క) {
                    అధికప్రవాహం_Isolate_లెక్క--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                లేకపోతే ఒకవేళ(valid_Isolate_లెక్క) {
                    flags|=DIRPROP_FLAG(PDI);
                    చివరి_Cc_Pos=i;
                    అధికప్రవాహం_Embedding_లెక్క=0;
                    అయితే(దొంతర_2[దొంతర_చివరి]<ISOLATE) /* pop embedding entries */
                        దొంతర_చివరి--;                /* until the last isolate entry */
                    దొంతర_చివరి--;                    /* pop also the last isolate entry */
                    valid_Isolate_లెక్క--;
                    bracketProcessPDI(&bracket_దత్తాంశం);
                } లేకపోతే
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(దొంతర_2[దొంతర_చివరి]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                మునుపటి_Level=embeddingLevel;
                స్థాయిలు[i]=NO_OVERRIDE(embeddingLevel);
                ఆపు;
            స్థితి B:
                flags|=DIRPROP_FLAG(B);
                స్థాయిలు[i]=GET_PARALEVEL(pBiDi, i);
                ఒకవేళ((i+1)<length) {
                    ఒకవేళ(పాఠ్యం[i]==CR && పాఠ్యం[i+1]==LF)
                        ఆపు;          /* skip CR when followed by LF */
                    అధికప్రవాహం_Embedding_లెక్క=అధికప్రవాహం_Isolate_లెక్క=0;
                    valid_Isolate_లెక్క=0;
                    దొంతర_చివరి=0;
                    మునుపటి_Level=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    దొంతర_2[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_దత్తాంశం, embeddingLevel);
                }
                ఆపు;
            స్థితి BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                స్థాయిలు[i]=మునుపటి_Level;
                flags|=DIRPROP_FLAG(BN);
                ఆపు;
            అప్రమేయ:
                /* all other types are normal characters and get the "real" level */
                ఒకవేళ(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(మునుపటి_Level)) {
                    bracketProcessBoundary(&bracket_దత్తాంశం, చివరి_Cc_Pos,
                                           మునుపటి_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    ఒకవేళ(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    లేకపోతే
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                మునుపటి_Level=embeddingLevel;
                స్థాయిలు[i]=embeddingLevel;
                ఒకవేళ(!bracketProcessChar(&bracket_దత్తాంశం, i))
                    తిరిగి static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                ఆపు;
            }
        }
        ఒకవేళ(flags&ముసుగు_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        ఒకవేళ(pBiDi->క్రమం_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    తిరిగి direction;
}

/*
 * Use a pre-specified embedding levels array:
 *
 * Adjust the directional properties for overrides (->LEVEL_OVERRIDE),
 * ignore all explicit codes (X9),
 * and check all the preset levels.
 *
 * Recalculate the flags to have them reflect the real properties
 * after taking the explicit embeddings into account.
 */
స్థిర UBiDiDirection
check_Explicit_స్థాయిలు(UBiDi *pBiDi, UError_సంకేతం *p_Error_సంకేతం) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
    int32_t isolate_లెక్క=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_లెక్క=0;

    int32_t current_Para_సూచీ = 0;
    int32_t current_Para_హద్దు = pBiDi->paras[0].హద్దు;
    int32_t currentParaLevel = pBiDi->paraLevel;

    కొరకు(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=స్థాయిలు[i];
        DirProp dirProp=dirProps[i];
        ఒకవేళ(dirProp==LRI || dirProp==RLI) {
            isolate_లెక్క++;
            ఒకవేళ(isolate_లెక్క>pBiDi->isolate_లెక్క)
                pBiDi->isolate_లెక్క=isolate_లెక్క;
        }
        లేకపోతే ఒకవేళ(dirProp==PDI)
            isolate_లెక్క--;
        లేకపోతే ఒకవేళ(dirProp==B)
            isolate_లెక్క=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        ఒకవేళ (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_హద్దు && (current_Para_సూచీ + 1) < pBiDi->para_లెక్క) {
            currentParaLevel = pBiDi->paras[++current_Para_సూచీ].level;
            current_Para_హద్దు = pBiDi->paras[current_Para_సూచీ].హద్దు;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        ఒకవేళ (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            ఒకవేళ (level == 0) {
                ఒకవేళ (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } లేకపోతే {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    స్థాయిలు[i] = level | overrideFlag;
                }
            } లేకపోతే {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
                తిరిగి UBIDI_LTR;
            }
        }
        ఒకవేళ (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } లేకపోతే {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    ఒకవేళ(flags&ముసుగు_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    తిరిగి directionFromFlags(pBiDi);
}

/******************************************************************
 The Properties state machine table
*******************************************************************

 All table cells are 8 bits:
      bits 0..4:  next state
      bits 5..7:  action to perform (if > 0)

 Cells may be of format "n" where n represents the next state
 (except for the rightmost column).
 Cells may also be of format "s(x,y)" where x represents an action
 to perform and y represents the next state.

*******************************************************************
 Definitions and type for properties state table
*******************************************************************
*/
#నిర్వచించు IMPTABPROPS_COLUMNS 16
#నిర్వచించు IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#నిర్వచించు GET_STATEPROPS(cell) ((cell)&0x1f)
#నిర్వచించు GET_ACTIONPROPS(cell) ((cell)>>5)
#నిర్వచించు s(action, newState) ((uint8_t)(newState+(action<<5)))

స్థిర స్థిరరాశి uint8_t సమూహం_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
గణన { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

/******************************************************************

      PROPERTIES  STATE  TABLE

 In table impTabProps,
      - the ON column regroups ON and WS, FSI, RLI, LRI and PDI
      - the BN column regroups BN, LRE, RLE, LRO, RLO, PDF
      - the Res column is the reduced property assigned to a run

 Action 1: process current run1, init new run1
        2: init new run2
        3: process run1, process run2, init new run1
        4: process run1, set run1=run2, init new run2

 Notes:
  1) This table is used in resolveImplicitLevels().
  2) This table triggers actions when there is a change in the Bidi
     property of incoming characters (action 1).
  3) Most such property sequences are processed immediately (in
     fact, passed to processPropertySeq().
  4) However, numbers are assembled as one sequence. This means
     that undefined situations (like CS following digits, until
     it is known if the next char will be a digit) are held until
     following chars define them.
     Example: digits followed by CS, then comes another CS or ON;
              the digits will be processed, then the CS assigned
              as the start of an ON sequence (action 3).
  5) There are cases where more than one sequence must be
     processed, for instance digits followed by CS followed by L:
     the digits must be processed as one sequence, and the CS
     must be processed as an ON sequence, all this before starting
     assembling chars for the opening L sequence.


*/
స్థిర స్థిరరాశి uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
{
/*                        L ,     R ,    EN ,    AN ,    ON ,     S ,     B ,    ES ,    ET ,    CS ,    BN ,   NSM ,    AL ,   ENL ,   ENR , Res */
/* 0 Init        */ {     1 ,     2 ,     4 ,     5 ,     7 ,    15 ,    17 ,     7 ,     9 ,     7 ,     0 ,     7 ,     3 ,    18 ,    21 , DirProp_ON },
/* 1 L           */ {     1 , s(1,2), s(1,4), s(1,5), s(1,7),s(1,15),s(1,17), s(1,7), s(1,9), s(1,7),     1 ,     1 , s(1,3),s(1,18),s(1,21),  DirProp_L },
/* 2 R           */ { s(1,1),     2 , s(1,4), s(1,5), s(1,7),s(1,15),s(1,17), s(1,7), s(1,9), s(1,7),     2 ,     2 , s(1,3),s(1,18),s(1,21),  DirProp_R },
/* 3 AL          */ { s(1,1), s(1,2), s(1,6), s(1,6), s(1,8),s(1,16),s(1,17), s(1,8), s(1,8), s(1,8),     3 ,     3 ,     3 ,s(1,18),s(1,21),  DirProp_R },
/* 4 EN          */ { s(1,1), s(1,2),     4 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,10),    11 ,s(2,10),     4 ,     4 , s(1,3),    18 ,    21 , DirProp_EN },
/* 5 AN          */ { s(1,1), s(1,2), s(1,4),     5 , s(1,7),s(1,15),s(1,17), s(1,7), s(1,9),s(2,12),     5 ,     5 , s(1,3),s(1,18),s(1,21), DirProp_AN },
/* 6 AL:EN/AN    */ { s(1,1), s(1,2),     6 ,     6 , s(1,8),s(1,16),s(1,17), s(1,8), s(1,8),s(2,13),     6 ,     6 , s(1,3),    18 ,    21 , DirProp_AN },
/* 7 ON          */ { s(1,1), s(1,2), s(1,4), s(1,5),     7 ,s(1,15),s(1,17),     7 ,s(2,14),     7 ,     7 ,     7 , s(1,3),s(1,18),s(1,21), DirProp_ON },
/* 8 AL:ON       */ { s(1,1), s(1,2), s(1,6), s(1,6),     8 ,s(1,16),s(1,17),     8 ,     8 ,     8 ,     8 ,     8 , s(1,3),s(1,18),s(1,21), DirProp_ON },
/* 9 ET          */ { s(1,1), s(1,2),     4 , s(1,5),     7 ,s(1,15),s(1,17),     7 ,     9 ,     7 ,     9 ,     9 , s(1,3),    18 ,    21 , DirProp_ON },
/*10 EN+ES/CS    */ { s(3,1), s(3,2),     4 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    10 , s(4,7), s(3,3),    18 ,    21 , DirProp_EN },
/*11 EN+ET       */ { s(1,1), s(1,2),     4 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    11 , s(1,7),    11 ,    11 , s(1,3),    18 ,    21 , DirProp_EN },
/*12 AN+CS       */ { s(3,1), s(3,2), s(3,4),     5 , s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    12 , s(4,7), s(3,3),s(3,18),s(3,21), DirProp_AN },
/*13 AL:EN/AN+CS */ { s(3,1), s(3,2),     6 ,     6 , s(4,8),s(3,16),s(3,17), s(4,8), s(4,8), s(4,8),    13 , s(4,8), s(3,3),    18 ,    21 , DirProp_AN },
/*14 ON+ET       */ { s(1,1), s(1,2), s(4,4), s(1,5),     7 ,s(1,15),s(1,17),     7 ,    14 ,     7 ,    14 ,    14 , s(1,3),s(4,18),s(4,21), DirProp_ON },
/*15 S           */ { s(1,1), s(1,2), s(1,4), s(1,5), s(1,7),    15 ,s(1,17), s(1,7), s(1,9), s(1,7),    15 , s(1,7), s(1,3),s(1,18),s(1,21),  DirProp_S },
/*16 AL:S        */ { s(1,1), s(1,2), s(1,6), s(1,6), s(1,8),    16 ,s(1,17), s(1,8), s(1,8), s(1,8),    16 , s(1,8), s(1,3),s(1,18),s(1,21),  DirProp_S },
/*17 B           */ { s(1,1), s(1,2), s(1,4), s(1,5), s(1,7),s(1,15),    17 , s(1,7), s(1,9), s(1,7),    17 , s(1,7), s(1,3),s(1,18),s(1,21),  DirProp_B },
/*18 ENL         */ { s(1,1), s(1,2),    18 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,19),    20 ,s(2,19),    18 ,    18 , s(1,3),    18 ,    21 ,  DirProp_L },
/*19 ENL+ES/CS   */ { s(3,1), s(3,2),    18 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    19 , s(4,7), s(3,3),    18 ,    21 ,  DirProp_L },
/*20 ENL+ET      */ { s(1,1), s(1,2),    18 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    20 , s(1,7),    20 ,    20 , s(1,3),    18 ,    21 ,  DirProp_L },
/*21 ENR         */ { s(1,1), s(1,2),    21 , s(1,5), s(1,7),s(1,15),s(1,17),s(2,22),    23 ,s(2,22),    21 ,    21 , s(1,3),    18 ,    21 , DirProp_AN },
/*22 ENR+ES/CS   */ { s(3,1), s(3,2),    21 , s(3,5), s(4,7),s(3,15),s(3,17), s(4,7),s(4,14), s(4,7),    22 , s(4,7), s(3,3),    18 ,    21 , DirProp_AN },
/*23 ENR+ET      */ { s(1,1), s(1,2),    21 , s(1,5), s(1,7),s(1,15),s(1,17), s(1,7),    23 , s(1,7),    23 ,    23 , s(1,3),    18 ,    21 , DirProp_AN }
};

/*  we must undef macro s because the levels tables have a different
 *  structure (4 bits for action and 4 bits for next state.
 */
#నిర్వచనం_రద్దు s

/******************************************************************
 The levels state machine tables
*******************************************************************

 All table cells are 8 bits:
      bits 0..3:  next state
      bits 4..7:  action to perform (if > 0)

 Cells may be of format "n" where n represents the next state
 (except for the rightmost column).
 Cells may also be of format "s(x,y)" where x represents an action
 to perform and y represents the next state.

 This format limits each table to 16 states each and to 15 actions.

*******************************************************************
 Definitions and type for levels state tables
*******************************************************************
*/
#నిర్వచించు IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#నిర్వచించు IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#నిర్వచించు GET_STATE(cell) ((cell)&0x0f)
#నిర్వచించు GET_ACTION(cell) ((cell)>>4)
#నిర్వచించు s(action, newState) ((uint8_t)(newState+(action<<4)))

రకనిర్వచనం uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
రకనిర్వచనం uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
రకనిర్వచనం నిర్మాణం ImpTabPair {
    స్థిరరాశి శూన్య * pImpTab[2];
    స్థిరరాశి శూన్య * pImpAct[2];
} ImpTabPair;

/******************************************************************

      LEVELS  STATE  TABLES

 In all levels state tables,
      - state 0 is the initial state
      - the Res column is the increment to add to the text level
        for this property sequence.

 The impAct arrays for each table of a pair map the local action
 numbers of the table to the total list of actions. For instance,
 action 2 in a given table corresponds to the action number which
 appears in entry [2] of the impAct array for that table.
 The first entry of all impAct arrays must be 0.

 Action 1: init conditional sequence
        2: prepend conditional sequence to current sequence
        3: set ON sequence to new level - 1
        4: init EN/AN/ON sequence
        5: fix EN/AN/ON sequence followed by R
        6: set previous level sequence to level 2

 Notes:
  1) These tables are used in processPropertySeq(). The input
     is property sequences as determined by resolveImplicitLevels.
  2) Most such property sequences are processed immediately
     (levels are assigned).
  3) However, some sequences cannot be assigned a final level till
     one or more following sequences are received. For instance,
     ON following an R sequence within an even-level paragraph.
     If the following sequence is R, the ON sequence will be
     assigned basic run level+1, and so will the R sequence.
  4) S is generally handled like ON, since its level will be fixed
     to paragraph level in adjustWSLevels().

*/

స్థిర స్థిరరాశి ImpTab impTabL_DEFAULT =   /* Even paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     1 ,     0 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R          */ {     0 ,     1 ,     3 ,     3 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : AN         */ {     0 ,     1 ,     0 ,     2 , s(1,5), s(1,5),     0 ,  2 },
/* 3 : R+EN/AN    */ {     0 ,     1 ,     3 ,     3 , s(1,4), s(1,4),     0 ,  2 },
/* 4 : R+ON       */ {     0 , s(2,1), s(3,3), s(3,3),     4 ,     4 ,     0 ,  0 },
/* 5 : AN+ON      */ {     0 , s(2,1),     0 , s(3,2),     5 ,     5 ,     0 ,  0 }
};
స్థిర స్థిరరాశి ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     3 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+AN       */ {     1 ,     0 ,     1 ,     3 ,     5 ,     5 ,     0 ,  1 },
/* 4 : L+ON       */ { s(2,1),     0 , s(2,1),     3 ,     4 ,     4 ,     0 ,  0 },
/* 5 : L+AN+ON    */ {     1 ,     0 ,     1 ,     3 ,     5 ,     5 ,     0 ,  0 }
};
స్థిర స్థిరరాశి ImpAct impAct0 = {0,1,2,3,4};
స్థిర స్థిరరాశి ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

స్థిర స్థిరరాశి ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     2 , s(1,1), s(1,1),     0 ,     0 ,     0 ,  0 },
/* 1 : L+EN/AN    */ {     0 , s(4,2),     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 2 : R          */ {     0 ,     2 ,     4 ,     4 , s(1,3), s(1,3),     0 ,  1 },
/* 3 : R+ON       */ {     0 , s(2,2), s(3,4), s(3,4),     3 ,     3 ,     0 ,  0 },
/* 4 : R+EN/AN    */ {     0 ,     2 ,     4 ,     4 , s(1,3), s(1,3),     0 ,  2 }
};
స్థిర స్థిరరాశి ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

స్థిర స్థిరరాశి ImpTab imp_Tab_L_సమూహం_NUMBERS_WITH_R =
/*  In this table, EN/AN+ON sequences receive levels as if associated with R
    until proven that there is L or sor/eor on both sides. AN is handled like EN.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 init         */ {     0 ,     3 , s(1,1), s(1,1),     0 ,     0 ,     0 ,  0 },
/* 1 EN/AN        */ { s(2,0),     3 ,     1 ,     1 ,     2 , s(2,0), s(2,0),  2 },
/* 2 EN/AN+ON     */ { s(2,0),     3 ,     1 ,     1 ,     2 , s(2,0), s(2,0),  1 },
/* 3 R            */ {     0 ,     3 ,     5 ,     5 , s(1,4),     0 ,     0 ,  1 },
/* 4 R+ON         */ { s(2,0),     3 ,     5 ,     5 ,     4 , s(2,0), s(2,0),  1 },
/* 5 R+EN/AN      */ {     0 ,     3 ,     5 ,     5 , s(1,4),     0 ,     0 ,  2 }
};
స్థిర స్థిరరాశి ImpTab imp_Tab_R_సమూహం_NUMBERS_WITH_R =
/*  In this table, EN/AN+ON sequences receive levels as if associated with R
    until proven that there is L on both sides. AN is handled like EN.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 init         */ {     2 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 EN/AN        */ {     2 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  1 },
/* 2 L            */ {     2 ,     0 , s(1,4), s(1,4), s(1,3),     0 ,     0 ,  1 },
/* 3 L+ON         */ { s(2,2),     0 ,     4 ,     4 ,     3 ,     0 ,     0 ,  0 },
/* 4 L+EN/AN      */ { s(2,2),     0 ,     4 ,     4 ,     3 ,     0 ,     0 ,  1 }
};
స్థిర స్థిరరాశి ImpTabPair imp_Tab_సమూహం_NUMBERS_WITH_R = {
                        {&imp_Tab_L_సమూహం_NUMBERS_WITH_R,
                         &imp_Tab_R_సమూహం_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


స్థిర స్థిరరాశి ImpTab impTabL_INVERSE_NUMBERS_AS_L =
/*  This table is identical to the Default LTR table except that EN and AN are
    handled like L.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 ,     1 ,     0 ,     0 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R          */ {     0 ,     1 ,     0 ,     0 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : AN         */ {     0 ,     1 ,     0 ,     0 , s(1,5), s(1,5),     0 ,  2 },
/* 3 : R+EN/AN    */ {     0 ,     1 ,     0 ,     0 , s(1,4), s(1,4),     0 ,  2 },
/* 4 : R+ON       */ { s(2,0),     1 , s(2,0), s(2,0),     4 ,     4 , s(2,0),  1 },
/* 5 : AN+ON      */ { s(2,0),     1 , s(2,0), s(2,0),     5 ,     5 , s(2,0),  1 }
};
స్థిర స్థిరరాశి ImpTab impTabR_INVERSE_NUMBERS_AS_L =
/*  This table is identical to the Default RTL table except that EN and AN are
    handled like L.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     1 , s(1,4), s(1,4),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+AN       */ {     1 ,     0 ,     1 ,     1 ,     5 ,     5 ,     0 ,  1 },
/* 4 : L+ON       */ { s(2,1),     0 , s(2,1), s(2,1),     4 ,     4 ,     0 ,  0 },
/* 5 : L+AN+ON    */ {     1 ,     0 ,     1 ,     1 ,     5 ,     5 ,     0 ,  0 }
};
స్థిర స్థిరరాశి ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

స్థిర స్థిరరాశి ImpTab imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష =   /* Odd  paragraph level */
/*  In this table, conditional sequences receive the lower possible level
    until proven otherwise.
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L          */ {     1 ,     0 ,     1 ,     2 , s(1,3), s(1,3),     0 ,  1 },
/* 2 : EN/AN      */ {     1 ,     0 ,     2 ,     2 ,     0 ,     0 ,     0 ,  1 },
/* 3 : L+ON       */ { s(2,1), s(3,0),     6 ,     4 ,     3 ,     3 , s(3,0),  0 },
/* 4 : L+ON+AN    */ { s(2,1), s(3,0),     6 ,     4 ,     5 ,     5 , s(3,0),  3 },
/* 5 : L+AN+ON    */ { s(2,1), s(3,0),     6 ,     4 ,     5 ,     5 , s(3,0),  2 },
/* 6 : L+ON+EN    */ { s(2,1), s(3,0),     6 ,     4 ,     3 ,     3 , s(3,0),  1 }
};
స్థిర స్థిరరాశి ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
స్థిర స్థిరరాశి ImpTabPair imp_Tab_INVERSE_LIKE_ప్రత్యక్ష = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష},
                        {&impAct0, &impAct1}};

స్థిర స్థిరరాశి ImpTab imp_Tab_L_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS =
/*  The case handled in this table is (visually):  R EN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 , s(6,3),     0 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L+AN       */ {     0 , s(6,3),     0 ,     1 , s(1,2), s(3,0),     0 ,  4 },
/* 2 : L+AN+ON    */ { s(2,0), s(6,3), s(2,0),     1 ,     2 , s(3,0), s(2,0),  3 },
/* 3 : R          */ {     0 , s(6,3), s(5,5), s(5,6), s(1,4), s(3,0),     0 ,  3 },
/* 4 : R+ON       */ { s(3,0), s(4,3), s(5,5), s(5,6),     4 , s(3,0), s(3,0),  3 },
/* 5 : R+EN       */ { s(3,0), s(4,3),     5 , s(5,6), s(1,4), s(3,0), s(3,0),  4 },
/* 6 : R+AN       */ { s(3,0), s(4,3), s(5,5),     6 , s(1,4), s(3,0), s(3,0),  4 }
};
స్థిర స్థిరరాశి ImpTab imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS =
/*  The cases handled in this table are (visually):  R EN L
                                                     R L AN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ { s(1,3),     0 ,     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : R+EN/AN    */ { s(2,3),     0 ,     1 ,     1 ,     2 , s(4,0),     0 ,  1 },
/* 2 : R+EN/AN+ON */ { s(2,3),     0 ,     1 ,     1 ,     2 , s(4,0),     0 ,  0 },
/* 3 : L          */ {     3 ,     0 ,     3 , s(3,6), s(1,4), s(4,0),     0 ,  1 },
/* 4 : L+ON       */ { s(5,3), s(4,0),     5 , s(3,6),     4 , s(4,0), s(4,0),  0 },
/* 5 : L+ON+EN    */ { s(5,3), s(4,0),     5 , s(3,6),     4 , s(4,0), s(4,0),  1 },
/* 6 : L+AN       */ { s(5,3), s(4,0),     6 ,     6 ,     4 , s(4,0), s(4,0),  3 }
};
స్థిర స్థిరరాశి ImpAct impAct2 = {0,1,2,5,6,7,8};
స్థిర స్థిరరాశి ImpAct impAct3 = {0,1,9,10,11,12};
స్థిర స్థిరరాశి ImpTabPair imp_Tab_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS},
                        {&impAct2, &impAct3}};

స్థిర స్థిరరాశి ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష},
                        {&impAct0, &impAct1}};

స్థిర స్థిరరాశి ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
/*  The case handled in this table is (visually):  R EN L
*/
{
/*                         L ,     R ,    EN ,    AN ,    ON ,     S ,     B , Res */
/* 0 : init       */ {     0 , s(6,2),     1 ,     1 ,     0 ,     0 ,     0 ,  0 },
/* 1 : L+EN/AN    */ {     0 , s(6,2),     1 ,     1 ,     0 , s(3,0),     0 ,  4 },
/* 2 : R          */ {     0 , s(6,2), s(5,4), s(5,4), s(1,3), s(3,0),     0 ,  3 },
/* 3 : R+ON       */ { s(3,0), s(4,2), s(5,4), s(5,4),     3 , s(3,0), s(3,0),  3 },
/* 4 : R+EN/AN    */ { s(3,0), s(4,2),     4 ,     4 , s(1,3), s(3,0), s(3,0),  4 }
};
స్థిర స్థిరరాశి ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS},
                        {&impAct2, &impAct3}};

#నిర్వచనం_రద్దు s

రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి ImpTab * pImpTab;             /* level table pointer          */
    స్థిరరాశి ImpAct * pImpAct;             /* action map array             */
    int32_t ప్రారంభం_ON;                    /* start of ON sequence         */
    int32_t ప్రారంభం_L2_EN;                  /* start of level 2 sequence    */
    int32_t చివరి_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t నిరంతరం_ప్రారంభం;                   /* start position of the run    */
    UBiDiLevel నిరంతరం_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

స్థిర శూన్య
కూడు_బిందువు(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#నిర్వచించు FIRSTALLOC  10
    బిందువు బిందువు_2;
    చొప్పించు_Points_2 * p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);

    ఒకవేళ (p_చొప్పించు_Points->capacity == 0)
    {
        p_చొప్పించు_Points->points=static_cast<బిందువు *>(uprv_malloc(పరిమాణం(బిందువు)*FIRSTALLOC));
        ఒకవేళ (p_చొప్పించు_Points->points == శూన్యసూచిక)
        {
            p_చొప్పించు_Points->error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        p_చొప్పించు_Points->capacity=FIRSTALLOC;
    }
    ఒకవేళ (p_చొప్పించు_Points->size >= p_చొప్పించు_Points->capacity) /* no room for new point */
    {
        బిందువు * savePoints=p_చొప్పించు_Points->points;
        p_చొప్పించు_Points->points=static_cast<బిందువు *>(uprv_realloc(p_చొప్పించు_Points->points,
                                           p_చొప్పించు_Points->capacity*2*పరిమాణం(బిందువు)));
        ఒకవేళ (p_చొప్పించు_Points->points == శూన్యసూచిక)
        {
            p_చొప్పించు_Points->points=savePoints;
            p_చొప్పించు_Points->error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        లేకపోతే  p_చొప్పించు_Points->capacity*=2;
    }
    బిందువు_2.pos=pos;
    బిందువు_2.flag=flag;
    p_చొప్పించు_Points->points[p_చొప్పించు_Points->size]=బిందువు_2;
    p_చొప్పించు_Points->size++;
#నిర్వచనం_రద్దు FIRSTALLOC
}

స్థిర శూన్య
సమితి_స్థాయిలు_Outside_Isolates(UBiDi *pBiDi, int32_t ప్రారంభం, int32_t హద్దు, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
    int32_t isolate_లెక్క=0, k;
    కొరకు(k=ప్రారంభం; k<హద్దు; k++) {
        dirProp=dirProps[k];
        ఒకవేళ(dirProp==PDI)
            isolate_లెక్క--;
        ఒకవేళ(isolate_లెక్క==0)
            స్థాయిలు[k]=level;
        ఒకవేళ(dirProp==LRI || dirProp==RLI)
            isolate_లెక్క++;
    }
}

/* perform rules (Wn), (Nn), and (In) on a run of the text ------------------ */

/*
 * This implementation of the (Wn) rules applies all rules in one pass.
 * In order to do so, it needs a look-ahead of typically 1 character
 * (except for W5: sequences of ET) and keeps track of changes
 * in a rule Wp that affect a later Wq (p<q).
 *
 * The (Nn) and (In) rules are also performed in that same single loop,
 * but effectively one iteration behind for white space.
 *
 * Since all implicit rules are performed in one step, it is not necessary
 * to actually store the intermediate directional properties in dirProps[].
 */

స్థిర శూన్య
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t ప్రారంభం, int32_t హద్దు) {
    uint8_t cell, oldStateSeq, actionSeq;
    స్థిరరాశి ImpTab * pImpTab=pLevState->pImpTab;
    స్థిరరాశి ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * స్థాయిలు=pBiDi->స్థాయిలు;
    UBiDiLevel level, కూడు_Level;
    చొప్పించు_Points_2 * p_చొప్పించు_Points;
    int32_t start0, k;

    start0=ప్రారంభం;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    కూడు_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    ఒకవేళ(actionSeq) {
        ఎంపిక(actionSeq) {
        స్థితి 1:                         /* init ON seq */
            pLevState->ప్రారంభం_ON=start0;
            ఆపు;

        స్థితి 2:                         /* prepend ON seq to current seq */
            ప్రారంభం=pLevState->ప్రారంభం_ON;
            ఆపు;

        స్థితి 3:                         /* EN/AN after R+ON */
            level=pLevState->నిరంతరం_Level+1;
            సమితి_స్థాయిలు_Outside_Isolates(pBiDi, pLevState->ప్రారంభం_ON, start0, level);
            ఆపు;

        స్థితి 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->నిరంతరం_Level+2;
            సమితి_స్థాయిలు_Outside_Isolates(pBiDi, pLevState->ప్రారంభం_ON, start0, level);
            ఆపు;

        స్థితి 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            ఒకవేళ (pLevState->ప్రారంభం_L2_EN >= 0) {
                కూడు_బిందువు(pBiDi, pLevState->ప్రారంభం_L2_EN, LRM_BEFORE);
            }
            pLevState->ప్రారంభం_L2_EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);
            ఒకవేళ ((p_చొప్పించు_Points->capacity == 0) ||
                (p_చొప్పించు_Points->size <= p_చొప్పించు_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->చివరి_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                ఒకవేళ ((level & 1) && (pLevState->ప్రారంభం_ON > 0)) {  /* after ON */
                    ప్రారంభం=pLevState->ప్రారంభం_ON;   /* reset to basic run level */
                }
                ఒకవేళ (_prop == DirProp_S)                /* add LRM before S */
                {
                    కూడు_బిందువు(pBiDi, start0, LRM_BEFORE);
                    p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;
                }
                ఆపు;
            }
            /* reset previous RTL cont to level for LTR text */
            కొరకు (k=pLevState->చివరి_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                స్థాయిలు[k]=(స్థాయిలు[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;
            pLevState->చివరి_Strong_RTL=-1;
            ఒకవేళ (_prop == DirProp_S)            /* add LRM before S */
            {
                కూడు_బిందువు(pBiDi, start0, LRM_BEFORE);
                p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;
            }
            ఆపు;

        స్థితి 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);
            ఒకవేళ (p_చొప్పించు_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_చొప్పించు_Points->size=p_చొప్పించు_Points->confirmed;
            pLevState->ప్రారంభం_ON=-1;
            pLevState->ప్రారంభం_L2_EN=-1;
            pLevState->చివరి_Strong_RTL=హద్దు - 1;
            ఆపు;

        స్థితి 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            ఒకవేళ ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_బహుళకం!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                ఒకవేళ (pLevState->ప్రారంభం_L2_EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->చివరి_Strong_RTL=హద్దు - 1;
                    ఆపు;
                }
                ఒకవేళ (pLevState->ప్రారంభం_L2_EN >= 0)  /* after EN, no AN */
                {
                    కూడు_బిందువు(pBiDi, pLevState->ప్రారంభం_L2_EN, LRM_BEFORE);
                    pLevState->ప్రారంభం_L2_EN=-2;
                }
                /* note AN */
                కూడు_బిందువు(pBiDi, start0, LRM_BEFORE);
                ఆపు;
            }
            /* if first EN/AN after R/AL */
            ఒకవేళ (pLevState->ప్రారంభం_L2_EN == -1) {
                pLevState->ప్రారంభం_L2_EN=start0;
            }
            ఆపు;

        స్థితి 8:                         /* note location of latest R/AL */
            pLevState->చివరి_Strong_RTL=హద్దు - 1;
            pLevState->ప్రారంభం_ON=-1;
            ఆపు;

        స్థితి 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            కొరకు (k=start0-1; k>=0 && !(స్థాయిలు[k]&1); k--);
            ఒకవేళ(k>=0) {
                కూడు_బిందువు(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);
                p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;   /* confirm it */
            }
            pLevState->ప్రారంభం_ON=start0;
            ఆపు;

        స్థితి 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            కూడు_బిందువు(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            కూడు_బిందువు(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            ఆపు;

        స్థితి 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);
            p_చొప్పించు_Points->size=p_చొప్పించు_Points->confirmed;
            ఒకవేళ (_prop == DirProp_S)            /* add RLM before S */
            {
                కూడు_బిందువు(pBiDi, start0, RLM_BEFORE);
                p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;
            }
            ఆపు;

        స్థితి 12:                        /* L after L+ON/AN */
            level=pLevState->నిరంతరం_Level + కూడు_Level;
            కొరకు(k=pLevState->ప్రారంభం_ON; k<start0; k++) {
                ఒకవేళ (స్థాయిలు[k]<level)
                    స్థాయిలు[k]=level;
            }
            p_చొప్పించు_Points=&(pBiDi->చొప్పించు_Points);
            p_చొప్పించు_Points->confirmed=p_చొప్పించు_Points->size;   /* confirm inserts */
            pLevState->ప్రారంభం_ON=start0;
            ఆపు;

        స్థితి 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->నిరంతరం_Level;
            కొరకు(k=start0-1; k>=pLevState->ప్రారంభం_ON; k--) {
                ఒకవేళ(స్థాయిలు[k]==level+3) {
                    అయితే(స్థాయిలు[k]==level+3) {
                        స్థాయిలు[k--]-=2;
                    }
                    అయితే(స్థాయిలు[k]==level) {
                        k--;
                    }
                }
                ఒకవేళ(స్థాయిలు[k]==level+2) {
                    స్థాయిలు[k]=level;
                    కొనసాగు;
                }
                స్థాయిలు[k]=level+1;
            }
            ఆపు;

        స్థితి 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->నిరంతరం_Level+1;
            కొరకు(k=start0-1; k>=pLevState->ప్రారంభం_ON; k--) {
                ఒకవేళ(స్థాయిలు[k]>level) {
                    స్థాయిలు[k]-=2;
                }
            }
            ఆపు;

        అప్రమేయ:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    ఒకవేళ((కూడు_Level) || (ప్రారంభం < start0)) {
        level=pLevState->నిరంతరం_Level + కూడు_Level;
        ఒకవేళ(ప్రారంభం>=pLevState->నిరంతరం_ప్రారంభం) {
            కొరకు(k=ప్రారంభం; k<హద్దు; k++) {
                స్థాయిలు[k]=level;
            }
        } లేకపోతే {
            సమితి_స్థాయిలు_Outside_Isolates(pBiDi, ప్రారంభం, హద్దు, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
స్థిర DirProp
చివరి_L_R_AL(UBiDi *pBiDi) {
    స్థిరరాశి char16_t *పాఠ్యం=pBiDi->prologue;
    int32_t length=pBiDi->pro_నిడివి;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    కొరకు(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_PREV(పాఠ్యం, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ఒకవేళ(dirProp==L) {
            తిరిగి DirProp_L;
        }
        ఒకవేళ(dirProp==R || dirProp==AL) {
            తిరిగి DirProp_R;
        }
        ఒకవేళ(dirProp==B) {
            తిరిగి DirProp_ON;
        }
    }
    తిరిగి DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
స్థిర DirProp
మొదటి_L_R_AL_EN_AN(UBiDi *pBiDi) {
    స్థిరరాశి char16_t *పాఠ్యం=pBiDi->epilogue;
    int32_t length=pBiDi->epi_నిడివి;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    కొరకు(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_తదుపరి(పాఠ్యం, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        ఒకవేళ(dirProp==L) {
            తిరిగి DirProp_L;
        }
        ఒకవేళ(dirProp==R || dirProp==AL) {
            తిరిగి DirProp_R;
        }
        ఒకవేళ(dirProp==EN) {
            తిరిగి DirProp_EN;
        }
        ఒకవేళ(dirProp==AN) {
            తిరిగి DirProp_AN;
        }
    }
    తిరిగి DirProp_ON;
}

స్థిర శూన్య
resolve_Implicit_స్థాయిలు(UBiDi *pBiDi,
                      int32_t ప్రారంభం, int32_t హద్దు,
                      DirProp sor, DirProp eor) {
    స్థిరరాశి DirProp *dirProps=pBiDi->dirProps;
    DirProp dirProp;
    LevState levState;
    int32_t i, start1, start2;
    uint16_t oldStateImp, stateImp, actionImp;
    uint8_t gprop, resProp, cell;
    UBool inverseRTL;
    DirProp తదుపరి_Strong_Prop=R;
    int32_t తదుపరి_Strong_Pos=-1;

    /* check for RTL inverse BiDi mode */
    /* FOOD FOR THOUGHT: in case of RTL inverse BiDi, it would make sense to
     * loop on the text characters from end to start.
     * This would need a different properties state table (at least different
     * actions) and different levels state tables (maybe very similar to the
     * LTR corresponding ones.
     */
    inverseRTL =
        static_cast<UBool>((ప్రారంభం < pBiDi->చివరి_Arabic_Pos) && (GET_PARALEVEL(pBiDi, ప్రారంభం) & 1) &&
                           (pBiDi->reordering_బహుళకం == UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష ||
                            pBiDi->reordering_బహుళకం == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.ప్రారంభం_L2_EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.చివరి_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.నిరంతరం_ప్రారంభం=ప్రారంభం;
    levState.నిరంతరం_Level=pBiDi->స్థాయిలు[ప్రారంభం];
    levState.pImpTab = static_cast<స్థిరరాశి ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.నిరంతరం_Level & 1]);
    levState.pImpAct = static_cast<స్థిరరాశి ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.నిరంతరం_Level & 1]);
    ఒకవేళ(ప్రారంభం==0 && pBiDi->pro_నిడివి>0) {
        DirProp చివరి_Strong=చివరి_L_R_AL(pBiDi);
        ఒకవేళ(చివరి_Strong!=DirProp_ON) {
            sor=చివరి_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    ఒకవేళ(dirProps[ప్రారంభం]==PDI  && pBiDi->isolate_లెక్క >= 0) {
        levState.ప్రారంభం_ON=pBiDi->isolates[pBiDi->isolate_లెక్క].ప్రారంభం_ON;
        start1=pBiDi->isolates[pBiDi->isolate_లెక్క].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_లెక్క].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_లెక్క].state;
        pBiDi->isolate_లెక్క--;
    } లేకపోతే {
        levState.ప్రారంభం_ON=-1;
        start1=ప్రారంభం;
        ఒకవేళ(dirProps[ప్రారంభం]==NSM)
            stateImp = 1 + sor;
        లేకపోతే
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, ప్రారంభం, ప్రారంభం);
    }
    start2=ప్రారంభం;                       /* to make Java compiler happy */

    కొరకు(i=ప్రారంభం; i<=హద్దు; i++) {
        ఒకవేళ(i>=హద్దు) {
            int32_t k;
            కొరకు(k=హద్దు-1; k>ప్రారంభం&&(DIRPROP_FLAG(dirProps[k])&ముసుగు_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            ఒకవేళ(dirProp==LRI || dirProp==RLI)
                ఆపు;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } లేకపోతే {
            DirProp prop, prop1;
            prop=dirProps[i];
            ఒకవేళ(prop==B) {
                pBiDi->isolate_లెక్క=-1; /* current isolates stack entry == none */
            }
            ఒకవేళ(inverseRTL) {
                ఒకవేళ(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } లేకపోతే ఒకవేళ(prop==EN) {
                    ఒకవేళ(తదుపరి_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        తదుపరి_Strong_Prop=R;   /* set default */
                        తదుపరి_Strong_Pos=హద్దు;
                        కొరకు(j=i+1; j<హద్దు; j++) {
                            prop1=dirProps[j];
                            ఒకవేళ(prop1==L || prop1==R || prop1==AL) {
                                తదుపరి_Strong_Prop=prop1;
                                తదుపరి_Strong_Pos=j;
                                ఆపు;
                            }
                        }
                    }
                    ఒకవేళ(తదుపరి_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=సమూహం_Prop[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        ఒకవేళ((i==హద్దు) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        ఒకవేళ(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            ఎంపిక(actionImp) {
            స్థితి 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                ఆపు;
            స్థితి 2:             /* init new seq2 */
                start2=i;
                ఆపు;
            స్థితి 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                ఆపు;
            స్థితి 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                ఆపు;
            అప్రమేయ:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    ఒకవేళ(హద్దు==pBiDi->length && pBiDi->epi_నిడివి>0) {
        DirProp మొదటి_Strong=మొదటి_L_R_AL_EN_AN(pBiDi);
        ఒకవేళ(మొదటి_Strong!=DirProp_ON) {
            eor=మొదటి_Strong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    కొరకు(i=హద్దు-1; i>ప్రారంభం&&(DIRPROP_FLAG(dirProps[i])&ముసుగు_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    ఒకవేళ((dirProp==LRI || dirProp==RLI) && హద్దు<pBiDi->length) {
        pBiDi->isolate_లెక్క++;
        pBiDi->isolates[pBiDi->isolate_లెక్క].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_లెక్క].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_లెక్క].start1=start1;
        pBiDi->isolates[pBiDi->isolate_లెక్క].ప్రారంభం_ON=levState.ప్రారంభం_ON;
    }
    లేకపోతే
        processPropertySeq(pBiDi, &levState, eor, హద్దు, హద్దు);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
స్థిర శూన్య
adjustWSLevels(UBiDi *pBiDi) {
    స్థిరరాశి DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
    int32_t i;

    ఒకవేళ(pBiDi->flags&ముసుగు_WS) {
        UBool క్రమం_Paragraphs_LTR=pBiDi->క్రమం_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        అయితే(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            అయితే(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&ముసుగు_WS) {
                ఒకవేళ(క్రమం_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    స్థాయిలు[i]=0;
                } లేకపోతే {
                    స్థాయిలు[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            అయితే(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                ఒకవేళ(flag&ముసుగు_BN_EXPLICIT) {
                    స్థాయిలు[i]=స్థాయిలు[i+1];
                } లేకపోతే ఒకవేళ(క్రమం_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    స్థాయిలు[i]=0;
                    ఆపు;
                } లేకపోతే ఒకవేళ(flag&ముసుగు_B_S) {
                    స్థాయిలు[i]=GET_PARALEVEL(pBiDi, i);
                    ఆపు;
                }
            }
        }
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Context(UBiDi *pBiDi,
                 స్థిరరాశి char16_t *prologue, int32_t pro_నిడివి,
                 స్థిరరాశి char16_t *epilogue, int32_t epi_నిడివి,
                 UError_సంకేతం *p_Error_సంకేతం) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    ఒకవేళ(pBiDi==శూన్యసూచిక || pro_నిడివి<-1 || epi_నిడివి<-1 ||
       (prologue==శూన్యసూచిక && pro_నిడివి!=0) || (epilogue==శూన్యసూచిక && epi_నిడివి!=0)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ(pro_నిడివి==-1) {
        pBiDi->pro_నిడివి=u_strlen(prologue);
    } లేకపోతే {
        pBiDi->pro_నిడివి=pro_నిడివి;
    }
    ఒకవేళ(epi_నిడివి==-1) {
        pBiDi->epi_నిడివి=u_strlen(epilogue);
    } లేకపోతే {
        pBiDi->epi_నిడివి=epi_నిడివి;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

స్థిర శూన్య
సమితి_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_నిడివి=0;                 /* forget the last context */
    pBiDi->epi_నిడివి=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#నిర్వచించు BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#నిర్వచించు BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

స్థిర శూన్య
సమితి_Para_Runs_Only(UBiDi *pBiDi, స్థిరరాశి char16_t *పాఠ్యం, int32_t length,
                UBiDiLevel paraLevel, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t *runs_Only_జ్ఞాపకం = శూన్యసూచిక;
    int32_t *visualMap;
    char16_t *visual_పాఠ్యం;
    int32_t save_నిడివి, saveTrailingWSStart;
    స్థిరరాశి UBiDiLevel *స్థాయిలు;
    UBiDiLevel *save_స్థాయిలు;
    UBiDiDirection saveDirection;
    UBool save_May_Allocate_పాఠ్యం;
    నిరంతరం *runs;
    int32_t visual_నిడివి, i, j, visual_ప్రారంభం, logical_ప్రారంభం,
            నిరంతరం_లెక్క, నిరంతరం_నిడివి, addedRuns, చొప్పించు_Remove,
            ప్రారంభం, హద్దు, అడుగు, సూచీ_Odd_బిట్టు, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_బహుళకం=UBIDI_REORDER_DEFAULT;
    ఒకవేళ(length==0) {
        ubidi_సమితి_Para(pBiDi, పాఠ్యం, length, paraLevel, శూన్యసూచిక, p_Error_సంకేతం);
        వెళ్ళు cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_జ్ఞాపకం=static_cast<int32_t *>(uprv_malloc(length*(పరిమాణం(int32_t)+పరిమాణం(char16_t)+పరిమాణం(UBiDiLevel))));
    ఒకవేళ(runs_Only_జ్ఞాపకం==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        వెళ్ళు cleanup3;
    }
    visualMap=runs_Only_జ్ఞాపకం;
    visual_పాఠ్యం = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_స్థాయిలు = reinterpret_cast<UBiDiLevel*>(&visual_పాఠ్యం[length]);
    saveOptions=pBiDi->reorderingOptions;
    ఒకవేళ(saveOptions & UBIDI_OPTION_చొప్పించు_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_చొప్పించు_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_సమితి_Para(pBiDi, పాఠ్యం, length, paraLevel, శూన్యసూచిక, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        వెళ్ళు cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    స్థాయిలు=ubidi_get_స్థాయిలు(pBiDi, p_Error_సంకేతం);
    uprv_memcpy(save_స్థాయిలు, స్థాయిలు, (size_t)pBiDi->length*పరిమాణం(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_నిడివి=pBiDi->length;
    saveDirection=pBiDi->direction;

    /* FOOD FOR THOUGHT: instead of writing the visual text, we could use
     * the visual map and the dirProps array to drive the second call
     * to ubidi_setPara (but must make provision for possible removal of
     * BiDi controls.  Alternatively, only use the dirProps array via
     * customized classifier callback.
     *
     * Note: We pass UBIDI_INTERNAL_RUNS_ONLY so that if mirroring changes code point
     * length (e.g., U+1DB10 contracting back to U+221D), doWriteReverse() uses legacy
     * 1-to-1 code unit stepping (j += k) rather than character stepping (j += origBaseLen).
     * This ensures visualLength remains synchronized with pBiDi->length and allows the
     * trailing surrogate half (Bidi class LTR) to act as a run boundary splitter in the
     * subsequent ubidi_setPara call.
     */
    visual_నిడివి=ubidi_writeReordered(pBiDi, visual_పాఠ్యం, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_అంతర్గత_RUNS_ONLY, p_Error_సంకేతం);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        వెళ్ళు cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_బహుళకం=UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    save_May_Allocate_పాఠ్యం=pBiDi->may_Allocate_పాఠ్యం;
    pBiDi->may_Allocate_పాఠ్యం=అబద్ధం;
    ubidi_సమితి_Para(pBiDi, visual_పాఠ్యం, visual_నిడివి, paraLevel, శూన్యసూచిక, p_Error_సంకేతం);
    pBiDi->may_Allocate_పాఠ్యం=save_May_Allocate_పాఠ్యం;
    ubidi_getRuns(pBiDi, p_Error_సంకేతం);
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        వెళ్ళు cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    నిరంతరం_లెక్క=pBiDi->నిరంతరం_లెక్క;
    runs=pBiDi->runs;
    visual_ప్రారంభం=0;
    కొరకు(i=0; i<నిరంతరం_లెక్క; i++, visual_ప్రారంభం+=నిరంతరం_నిడివి) {
        నిరంతరం_నిడివి=runs[i].visual_హద్దు-visual_ప్రారంభం;
        ఒకవేళ(నిరంతరం_నిడివి<2) {
            కొనసాగు;
        }
        logical_ప్రారంభం=GET_సూచీ(runs[i].logical_ప్రారంభం);
        కొరకు(j=logical_ప్రారంభం+1; j<logical_ప్రారంభం+నిరంతరం_నిడివి; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            ఒకవేళ((BIDI_ABS(index0-index1)!=1) || (save_స్థాయిలు[index0]!=save_స్థాయిలు[index1])) {
                addedRuns++;
            }
        }
    }
    ఒకవేళ(addedRuns) {
        ఒకవేళ(get_Runs_జ్ఞాపకం(pBiDi, నిరంతరం_లెక్క+addedRuns)) {
            ఒకవేళ(నిరంతరం_లెక్క==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_జ్ఞాపకం[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_జ్ఞాపకం;
            pBiDi->నిరంతరం_లెక్క+=addedRuns;
        } లేకపోతే {
            *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    కొరకు(i=నిరంతరం_లెక్క-1; i>=0; i--) {
        నిరంతరం_నిడివి= i==0 ? runs[0].visual_హద్దు :
                          runs[i].visual_హద్దు-runs[i-1].visual_హద్దు;
        logical_ప్రారంభం=runs[i].logical_ప్రారంభం;
        సూచీ_Odd_బిట్టు=GET_ODD_బిట్టు(logical_ప్రారంభం);
        logical_ప్రారంభం=GET_సూచీ(logical_ప్రారంభం);
        ఒకవేళ(నిరంతరం_నిడివి<2) {
            ఒకవేళ(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logical_ప్రారంభం];
            runs[i+addedRuns].logical_ప్రారంభం=MAKE_సూచీ_ODD_PAIR(logicalPos,
                                            save_స్థాయిలు[logicalPos]^సూచీ_Odd_బిట్టు);
            కొనసాగు;
        }
        ఒకవేళ(సూచీ_Odd_బిట్టు) {
            ప్రారంభం=logical_ప్రారంభం;
            హద్దు=logical_ప్రారంభం+నిరంతరం_నిడివి-1;
            అడుగు=1;
        } లేకపోతే {
            ప్రారంభం=logical_ప్రారంభం+నిరంతరం_నిడివి-1;
            హద్దు=logical_ప్రారంభం;
            అడుగు=-1;
        }
        కొరకు(j=ప్రారంభం; j!=హద్దు; j+=అడుగు) {
            index0=visualMap[j];
            index1=visualMap[j+అడుగు];
            ఒకవేళ((BIDI_ABS(index0-index1)!=1) || (save_స్థాయిలు[index0]!=save_స్థాయిలు[index1])) {
                logicalPos=BIDI_MIN(visualMap[ప్రారంభం], index0);
                runs[i+addedRuns].logical_ప్రారంభం=MAKE_సూచీ_ODD_PAIR(logicalPos,
                                            save_స్థాయిలు[logicalPos]^సూచీ_Odd_బిట్టు);
                runs[i+addedRuns].visual_హద్దు=runs[i].visual_హద్దు;
                runs[i].visual_హద్దు-=BIDI_ABS(j-ప్రారంభం)+1;
                చొప్పించు_Remove=runs[i].చొప్పించు_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].చొప్పించు_Remove=చొప్పించు_Remove;
                runs[i].చొప్పించు_Remove&=~చొప్పించు_Remove;
                ప్రారంభం=j+అడుగు;
                addedRuns--;
            }
        }
        ఒకవేళ(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[ప్రారంభం], visualMap[హద్దు]);
        runs[i+addedRuns].logical_ప్రారంభం=MAKE_సూచీ_ODD_PAIR(logicalPos,
                                            save_స్థాయిలు[logicalPos]^సూచీ_Odd_బిట్టు);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->పాఠ్యం=పాఠ్యం;
    pBiDi->length=save_నిడివి;
    pBiDi->original_నిడివి=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    ఒకవేళ(save_నిడివి>pBiDi->స్థాయిలు_కొలత) {
        save_నిడివి=pBiDi->స్థాయిలు_కొలత;
    }
    uprv_memcpy(pBiDi->స్థాయిలు, save_స్థాయిలు, (size_t)save_నిడివి*పరిమాణం(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    ఒకవేళ(pBiDi->నిరంతరం_లెక్క>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_జ్ఞాపకం);

    pBiDi->reordering_బహుళకం=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Para(UBiDi *pBiDi, స్థిరరాశి char16_t *పాఠ్యం, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_స్థాయిలు,
              UError_సంకేతం *p_Error_సంకేతం) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    ఒకవేళ(pBiDi==శూన్యసూచిక || పాఠ్యం==శూన్యసూచిక || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ(length==-1) {
        length=u_strlen(పాఠ్యం);
    }

    /* special treatment for RUNS_ONLY mode */
    ఒకవేళ(pBiDi->reordering_బహుళకం==UBIDI_REORDER_RUNS_ONLY) {
        సమితి_Para_Runs_Only(pBiDi, పాఠ్యం, length, paraLevel, p_Error_సంకేతం);
        తిరిగి;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=శూన్యసూచిక;          /* mark unfinished setPara */
    pBiDi->పాఠ్యం=పాఠ్యం;
    pBiDi->length=pBiDi->original_నిడివి=pBiDi->ఫలితం_నిడివి=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_లెక్క=1;

    pBiDi->dirProps=శూన్యసూచిక;
    pBiDi->స్థాయిలు=శూన్యసూచిక;
    pBiDi->runs=శూన్యసూచిక;
    pBiDi->చొప్పించు_Points.size=0;         /* clean up from last call */
    pBiDi->చొప్పించు_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    ఒకవేళ(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        ఒకవేళ(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->నిరంతరం_లెక్క=0;
        pBiDi->para_లెక్క=0;
        సమితి_Para_Success(pBiDi);          /* mark successful setPara */
        తిరిగి;
    }

    pBiDi->నిరంతరం_లెక్క=-1;

    /* allocate paras memory */
    ఒకవేళ(pBiDi->paras_జ్ఞాపకం)
        pBiDi->paras=pBiDi->paras_జ్ఞాపకం;
    లేకపోతే
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    ఒకవేళ(get_Dir_Props_జ్ఞాపకం(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_జ్ఞాపకం;
        ఒకవేళ(!getDirProps(pBiDi)) {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    } లేకపోతే {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    ఒకవేళ(embedding_స్థాయిలు==శూన్యసూచిక) {
        /* no: determine explicit levels according to the (Xn) rules */\
        ఒకవేళ(get_స్థాయిలు_జ్ఞాపకం(pBiDi, length)) {
            pBiDi->స్థాయిలు=pBiDi->స్థాయిలు_జ్ఞాపకం;
            direction=resolve_Explicit_స్థాయిలు(pBiDi, p_Error_సంకేతం);
            ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                తిరిగి;
            }
        } లేకపోతే {
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    } లేకపోతే {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->స్థాయిలు=embedding_స్థాయిలు;
        direction=check_Explicit_స్థాయిలు(pBiDi, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి;
        }
    }

    /* allocate isolate memory */
    ఒకవేళ(pBiDi->isolate_లెక్క<=SIMPLE_ISOLATES_లెక్క)
        pBiDi->isolates=pBiDi->simpleIsolates;
    లేకపోతే
        ఒకవేళ((int32_t)(pBiDi->isolate_లెక్క*పరిమాణం(Isolate))<=pBiDi->isolates_కొలత)
            pBiDi->isolates=pBiDi->isolates_జ్ఞాపకం;
        లేకపోతే {
            ఒకవేళ(get_Initial_Isolates_జ్ఞాపకం(pBiDi, pBiDi->isolate_లెక్క)) {
                pBiDi->isolates=pBiDi->isolates_జ్ఞాపకం;
            } లేకపోతే {
                *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
        }
    pBiDi->isolate_లెక్క=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    ఎంపిక(direction) {
    స్థితి UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ఆపు;
    స్థితి UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        ఆపు;
    అప్రమేయ:
        /*
         *  Choose the right implicit state table
         */
        ఎంపిక(pBiDi->reordering_బహుళకం) {
        స్థితి UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            ఆపు;
        స్థితి UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            ఆపు;
        స్థితి UBIDI_REORDER_సమూహం_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_సమూహం_NUMBERS_WITH_R;
            ఆపు;
        స్థితి UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            ఆపు;
        స్థితి UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష:
            ఒకవేళ (pBiDi->reorderingOptions & UBIDI_OPTION_చొప్పించు_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ప్రత్యక్ష_WITH_MARKS;
            } లేకపోతే {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_ప్రత్యక్ష;
            }
            ఆపు;
        స్థితి UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            ఒకవేళ (pBiDi->reorderingOptions & UBIDI_OPTION_చొప్పించు_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } లేకపోతే {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            ఆపు;
        అప్రమేయ:
            /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
        /*
         * If there are no external levels specified and there
         * are no significant explicit level codes in the text,
         * then we can treat the entire paragraph as one run.
         * Otherwise, we need to perform the following rules on runs of
         * the text with the same embedding levels. (X10)
         * "Significant" explicit level codes are ones that actually
         * affect non-BN characters.
         * Examples for "insignificant" ones are empty embeddings
         * LRE-PDF, LRE-RLE-PDF-PDF, etc.
         */
        ఒకవేళ(embedding_స్థాయిలు==శూన్యసూచిక && pBiDi->para_లెక్క<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_స్థాయిలు(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } లేకపోతే {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *స్థాయిలు=pBiDi->స్థాయిలు;
            int32_t ప్రారంభం, హద్దు=0;
            UBiDiLevel level, తదుపరి_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            తదుపరి_Level=స్థాయిలు[0];
            ఒకవేళ(level<తదుపరి_Level) {
                eor=GET_LR_FROM_LEVEL(తదుపరి_Level);
            } లేకపోతే {
                eor=GET_LR_FROM_LEVEL(level);
            }

            చేయి {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                ప్రారంభం=హద్దు;
                level=తదుపరి_Level;
                ఒకవేళ((ప్రారంభం>0) && (dirProps[ప్రారంభం-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, ప్రారంభం));
                } లేకపోతే {
                    sor=eor;
                }

                /* search for the limit of this run */
                అయితే((++హద్దు<length) &&
                      ((స్థాయిలు[హద్దు]==level) ||
                       (DIRPROP_FLAG(dirProps[హద్దు])&ముసుగు_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                ఒకవేళ(హద్దు<length) {
                    తదుపరి_Level=స్థాయిలు[హద్దు];
                } లేకపోతే {
                    తదుపరి_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                ఒకవేళ(NO_OVERRIDE(level)<NO_OVERRIDE(తదుపరి_Level)) {
                    eor=GET_LR_FROM_LEVEL(తదుపరి_Level);
                } లేకపోతే {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                ఒకవేళ(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_స్థాయిలు(pBiDi, ప్రారంభం, హద్దు, sor, eor);
                } లేకపోతే {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    చేయి {
                        స్థాయిలు[ప్రారంభం++]&=~UBIDI_LEVEL_OVERRIDE;
                    } అయితే(ప్రారంభం<హద్దు);
                }
            } అయితే(హద్దు<length);
        }
        /* check if we got any memory shortage while adding insert points */
        ఒకవేళ (U_FAILURE(pBiDi->చొప్పించు_Points.error_సంకేతం))
        {
            *p_Error_సంకేతం=pBiDi->చొప్పించు_Points.error_సంకేతం;
            తిరిగి;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        ఆపు;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    ఒకవేళ((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_చొప్పించు_MARKS) &&
       ((pBiDi->reordering_బహుళకం==UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష) ||
        (pBiDi->reordering_బహుళకం==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, ప్రారంభం, చివరి;
        UBiDiLevel level;
        DirProp dirProp;
        కొరకు(i=0; i<pBiDi->para_లెక్క; i++) {
            చివరి=(pBiDi->paras[i].హద్దు)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            ఒకవేళ(level==0)
                కొనసాగు;           /* LTR paragraph */
            ప్రారంభం= i==0 ? 0 : pBiDi->paras[i-1].హద్దు;
            కొరకు(j=చివరి; j>=ప్రారంభం; j--) {
                dirProp=dirProps[j];
                ఒకవేళ(dirProp==L) {
                    ఒకవేళ(j<చివరి) {
                        అయితే(dirProps[చివరి]==B) {
                            చివరి--;
                        }
                    }
                    కూడు_బిందువు(pBiDi, చివరి, RLM_BEFORE);
                    ఆపు;
                }
                ఒకవేళ(DIRPROP_FLAG(dirProp) & ముసుగు_R_AL) {
                    ఆపు;
                }
            }
        }
    }

    ఒకవేళ(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->ఫలితం_నిడివి -= pBiDi->control_లెక్క;
    } లేకపోతే {
        pBiDi->ఫలితం_నిడివి += pBiDi->చొప్పించు_Points.size;
    }
    సమితి_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI శూన్య U_EXPORT2
ubidi_క్రమం_Paragraphs_LTR(UBiDi *pBiDi, UBool క్రమం_Paragraphs_LTR) {
    ఒకవేళ(pBiDi!=శూన్యసూచిక) {
        pBiDi->క్రమం_Paragraphs_LTR=క్రమం_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_క్రమం_Paragraphs_LTR(UBiDi *pBiDi) {
    ఒకవేళ(pBiDi!=శూన్యసూచిక) {
        తిరిగి pBiDi->క్రమం_Paragraphs_LTR;
    } లేకపోతే {
        తిరిగి అబద్ధం;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(స్థిరరాశి UBiDi *pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->direction;
    } లేకపోతే {
        తిరిగి UBIDI_LTR;
    }
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
ubidi_get_పాఠ్యం(స్థిరరాశి UBiDi* pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->పాఠ్యం;
    } లేకపోతే {
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_నిడివి(స్థిరరాశి UBiDi *pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->original_నిడివి;
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_నిడివి(స్థిరరాశి UBiDi *pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->length;
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_ఫలితం_నిడివి(స్థిరరాశి UBiDi *pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->ఫలితం_నిడివి;
    } లేకపోతే {
        తిరిగి 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(స్థిరరాశి UBiDi *pBiDi) {
    ఒకవేళ(IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి pBiDi->paraLevel;
    } లేకపోతే {
        తిరిగి 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_లెక్క_Paragraphs(UBiDi *pBiDi) {
    ఒకవేళ(!IS_VALID_PARA_OR_పంక్తి(pBiDi)) {
        తిరిగి 0;
    } లేకపోతే {
        తిరిగి pBiDi->para_లెక్క;
    }
}

U_CAPI శూన్య U_EXPORT2
ubidi_get_Paragraph_By_సూచీ(స్థిరరాశి UBiDi *pBiDi, int32_t para_సూచీ,
                          int32_t *p_Para_ప్రారంభం, int32_t *p_Para_హద్దు,
                          UBiDiLevel *pParaLevel, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t para_ప్రారంభం;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం);
    RETURN_VOID_IF_BAD_RANGE(para_సూచీ, 0, pBiDi->para_లెక్క, *p_Error_సంకేతం);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    ఒకవేళ(para_సూచీ) {
        para_ప్రారంభం=pBiDi->paras[para_సూచీ-1].హద్దు;
    } లేకపోతే {
        para_ప్రారంభం=0;
    }
    ఒకవేళ(p_Para_ప్రారంభం!=శూన్యసూచిక) {
        *p_Para_ప్రారంభం=para_ప్రారంభం;
    }
    ఒకవేళ(p_Para_హద్దు!=శూన్యసూచిక) {
        *p_Para_హద్దు=pBiDi->paras[para_సూచీ].హద్దు;
    }
    ఒకవేళ(pParaLevel!=శూన్యసూచిక) {
        *pParaLevel=GET_PARALEVEL(pBiDi, para_ప్రారంభం);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(స్థిరరాశి UBiDi *pBiDi, int32_t char_సూచీ,
                          int32_t *p_Para_ప్రారంభం, int32_t *p_Para_హద్దు,
                          UBiDiLevel *pParaLevel, UError_సంకేతం *p_Error_సంకేతం) {
    int32_t para_సూచీ;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం, -1);
    RETURN_IF_NOT_VALID_PARA_OR_పంక్తి(pBiDi, *p_Error_సంకేతం, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_సూచీ, 0, pBiDi->length, *p_Error_సంకేతం, -1);

    కొరకు(para_సూచీ=0; char_సూచీ>=pBiDi->paras[para_సూచీ].హద్దు; para_సూచీ++);
    ubidi_get_Paragraph_By_సూచీ(pBiDi, para_సూచీ, p_Para_ప్రారంభం, p_Para_హద్దు, pParaLevel, p_Error_సంకేతం);
    తిరిగి para_సూచీ;
}

U_CAPI శూన్య U_EXPORT2
ubidi_సమితి_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       స్థిరరాశి శూన్య *newContext, UBiDiClassCallback **oldFn,
                       స్థిరరాశి శూన్య **oldContext, UError_సంకేతం *p_Error_సంకేతం)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_సంకేతం);
    ఒకవేళ(pBiDi==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }
    ఒకవేళ( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    ఒకవేళ( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI శూన్య U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, స్థిరరాశి శూన్య **context)
{
    ఒకవేళ(pBiDi==శూన్యసూచిక) {
        తిరిగి;
    }
    ఒకవేళ( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    ఒకవేళ( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    ఒకవేళ( pBiDi->fnClassCallback == శూన్యసూచిక ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    ఒకవేళ(dir >= U_CHAR_DIRECTION_లెక్క) {
        dir = (UCharDirection)ON;
    }
    తిరిగి dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#ఒకవేళ U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(స్థిరరాశి UnicodeString& s) {
    ఒకవేళ (s == u"ltr") {
        తిరిగి U_MF_BIDI_OPTION_LTR;
    }
    ఒకవేళ (s == u"rtl") {
        తిరిగి U_MF_BIDI_OPTION_RTL;
    }
    ఒకవేళ (s == u"auto") {
        తిరిగి U_MF_BIDI_OPTION_AUTO;
    }
    తిరిగి U_MF_BIDI_OPTION_INHERIT; // inherit is default
}

// BiDi isolation
// --------------
// `uDirOption` is the directionality from a u:dir annotation on the expression
// that produced this formatted value, if present.
// `dir` is the directionality of `fmt`. This is determined from the resolved
// value that `fmt` is part of; that is, each function can set the directionality
// of the resolved value of its result.
U_CAPI UnicodeString* U_EXPORT2
ubidi_isolate(UMFBidiIsolationStrategy bidiIsolationStrategy,
                            UMFDirectionality msgdir,
                            UMFBidiIsolationStyle bidiIsolationStyle,
                            UMFBidiOption uDirOption,
                            UMFDirectionality dir,
                            UnicodeString& fmt) {
    // See "The Default Bidi Strategy" at:
    // https://github.com/unicode-org/message-format-wg/blob/main/spec/formatting.md#handling-bidirectional-text

    // If strategy is 'none', just return the string
    ఒకవేళ (bidiIsolationStrategy == U_MF_BIDI_OFF)
        తిరిగి &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    తార్కిక isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    ఎంపిక (dir) {
        స్థితి U_MF_DIRECTIONALITY_LTR:
            ఒకవేళ (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                తిరిగి &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ఒకవేళ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } లేకపోతే {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ఆపు; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        స్థితి U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ఒకవేళ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } లేకపోతే {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ఆపు; // End of 2iv.
        // 2v. Else:
        అప్రమేయ:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            ఒకవేళ (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } లేకపోతే {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            ఆపు; // End of 2v
    } // `fmt` now contains the isolated string
    తిరిగి &fmt;
}

U_NAMESPACE_చివర

#అయితే_ముగింపు // #if SHOW_CPLUSPLUS_API

