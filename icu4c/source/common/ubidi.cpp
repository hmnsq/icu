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

#अन्तर्भाव <limits>

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/ubidi.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ubidi_props.h"
#अन्तर्भाव "ubidiimp.h"
#अन्तर्भाव "uassert.h"

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
स्थिर ध्रुव Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
स्थिर ध्रुव Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
स्थिर ध्रुव Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#परिभाषय DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#परिभाषय DIRPROP_FLAG_E(level)  flagE[(level)&1]
#परिभाषय DIRPROP_FLAG_O(level)  flagO[(level)&1]

#परिभाषय DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#परिभाषय NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    प्रत्यागम ubidi_openSized(0, 0, &error_संहिता);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_दीर्घता_2, int32_t max_सततम्_गणनम्, UError_संहिता *p_Error_संहिता) {
    UBiDi *pBiDi;

    /* check the argument values */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    } अन्यथा यदि(max_दीर्घता_2<0 || max_सततम्_गणनम्<0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(परिमाण(UBiDi));
    यदि(pBiDi==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, परिमाण(UBiDi));

    /* allocate memory for arrays as requested */
    यदि(max_दीर्घता_2>0) {
        यदि( !get_Initial_Dir_Props_स्मृतिः(pBiDi, max_दीर्घता_2) ||
            !get_Initial_स्तराः_स्मृतिः(pBiDi, max_दीर्घता_2)
        ) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        }
    } अन्यथा {
        pBiDi->mayAllocateText=सत्य;
    }

    यदि(max_सततम्_गणनम्>0) {
        यदि(max_सततम्_गणनम्==1) {
            /* use simpleRuns[] */
            pBiDi->runs_आकार=परिमाण(सततम्);
        } अन्यथा यदि(!get_Initial_Runs_स्मृतिः(pBiDi, max_सततम्_गणनम्)) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        }
    } अन्यथा {
        pBiDi->mayAllocateRuns=सत्य;
    }

    यदि(U_SUCCESS(*p_Error_संहिता)) {
        प्रत्यागम pBiDi;
    } अन्यथा {
        ubidi_close(pBiDi);
        प्रत्यागम शून्यसूचकः;
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
ubidi_get_स्मृतिः(Bidi_स्मृतिः_For_आवण्टनम् *bidiMem, int32_t *p_आकार, UBool mayAllocate, size_t आकार_Needed) {
    यदि (आकार_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      प्रत्यागम असत्य;
    }
    शून्य **p_स्मृतिः = (शून्य **)bidiMem;
    /* check for existing memory */
    यदि(*p_स्मृतिः==शून्यसूचकः) {
        /* we need to allocate memory */
        यदि(mayAllocate && (*p_स्मृतिः=uprv_malloc(आकार_Needed))!=शून्यसूचकः) {
            *p_आकार=static_cast<int32_t>(आकार_Needed);
            प्रत्यागम सत्य;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    } अन्यथा {
        यदि(आकार_Needed<=static_cast<size_t>(*p_आकार)) {
            /* there is already enough memory */
            प्रत्यागम सत्य;
        }
        अन्यथा यदि(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            प्रत्यागम असत्य;
        } अन्यथा {
            /* we try to grow */
            शून्य *स्मृतिः;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            यदि((स्मृतिः=uprv_realloc(*p_स्मृतिः, आकार_Needed))!=शून्यसूचकः) {
                *p_स्मृतिः=स्मृतिः;
                *p_आकार=static_cast<int32_t>(आकार_Needed);
                प्रत्यागम सत्य;
            } अन्यथा {
                /* we failed to grow */
                प्रत्यागम असत्य;
            }
        }
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    यदि(pBiDi!=शून्यसूचकः) {
        pBiDi->pParaBiDi=शून्यसूचकः;          /* in case one tries to reuse this block */
        यदि(pBiDi->dir_Props_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->dir_Props_स्मृतिः);
        }
        यदि(pBiDi->स्तराः_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->स्तराः_स्मृतिः);
        }
        यदि(pBiDi->openings_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->openings_स्मृतिः);
        }
        यदि(pBiDi->paras_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->paras_स्मृतिः);
        }
        यदि(pBiDi->runs_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->runs_स्मृतिः);
        }
        यदि(pBiDi->isolates_स्मृतिः!=शून्यसूचकः) {
            uprv_free(pBiDi->isolates_स्मृतिः);
        }
        यदि(pBiDi->निवेशय_Points.points!=शून्यसूचकः) {
            uprv_free(pBiDi->निवेशय_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Inverse(UBiDi *pBiDi, UBool isInverse) {
    यदि(pBiDi!=शून्यसूचकः) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_बहुलकम् = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    यदि(pBiDi!=शून्यसूचकः) {
        प्रत्यागम pBiDi->isInverse;
    } अन्यथा {
        प्रत्यागम असत्य;
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
U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Reordering_बहुलकम्(UBiDi *pBiDi, UBi_Di_Reordering_बहुलकम् reordering_बहुलकम्) UPRV_NO_SANITIZE_UNDEFINED {
    यदि ((pBiDi!=शून्यसूचकः) && (reordering_बहुलकम् >= UBIDI_REORDER_DEFAULT)
                        && (reordering_बहुलकम् < UBIDI_REORDER_गणनम्)) {
        pBiDi->reordering_बहुलकम् = reordering_बहुलकम्;
        pBiDi->isInverse = reordering_बहुलकम् == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_बहुलकम् U_EXPORT2
ubidi_get_Reordering_बहुलकम्(UBiDi *pBiDi) {
    यदि (pBiDi!=शून्यसूचकः) {
        प्रत्यागम pBiDi->reordering_बहुलकम्;
    } अन्यथा {
        प्रत्यागम UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    यदि (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_निवेशय_MARKS;
    }
    यदि (pBiDi!=शून्यसूचकः) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    यदि (pBiDi!=शून्यसूचकः) {
        प्रत्यागम pBiDi->reorderingOptions;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_आधारः_Direction(ध्रुव char16_t *text,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    यदि( text==शून्यसूचकः || length<-1 ){
        प्रत्यागम UBIDI_NEUTRAL;
    }

    यदि(length==-1) {
        length=u_strlen(text);
    }

    प्रति( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dir = u_charDirection(uchar);
        यदि( dir == U_वामः_गन्तव्यम्_दक्षिणः )
                प्रत्यागम UBIDI_LTR;
        यदि( dir == U_दक्षिणः_गन्तव्यम्_वामः || dir ==U_दक्षिणः_गन्तव्यम्_वामः_ARABIC )
                प्रत्यागम UBIDI_RTL;
    }
    प्रत्यागम UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
स्थिर DirProp
firstL_R_AL(UBiDi *pBiDi) {
    ध्रुव char16_t *text=pBiDi->prologue;
    int32_t length=pBiDi->pro_दीर्घता;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, परिणाम=ON;
    प्रति(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        यदि(परिणाम==ON) {
            यदि(dirProp==L || dirProp==R || dirProp==AL) {
                परिणाम=dirProp;
            }
        } अन्यथा {
            यदि(dirProp==B) {
                परिणाम=ON;
            }
        }
    }
    प्रत्यागम परिणाम;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
स्थिर UBool
check_Para_गणनम्(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_गणनम्;
    यदि(pBiDi->paras==pBiDi->simpleParas) {
        यदि(count<=SIMPLE_PARAS_गणनम्)
            प्रत्यागम सत्य;
        यदि(!get_Initial_Paras_स्मृतिः(pBiDi, SIMPLE_PARAS_गणनम् * 2))
            प्रत्यागम असत्य;
        pBiDi->paras=pBiDi->paras_स्मृतिः;
        uprv_memcpy(pBiDi->paras_स्मृतिः, pBiDi->simpleParas, SIMPLE_PARAS_गणनम् * परिमाण(Para));
        प्रत्यागम सत्य;
    }
    यदि(!get_Initial_Paras_स्मृतिः(pBiDi, count * 2))
        प्रत्यागम असत्य;
    pBiDi->paras=pBiDi->paras_स्मृतिः;
    प्रत्यागम सत्य;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
स्थिर UBool
getDirProps(UBiDi *pBiDi) {
    ध्रुव char16_t *text=pBiDi->text;
    DirProp *dirProps=pBiDi->dir_Props_स्मृतिः;    /* pBiDi->dirProps is const */

    int32_t i=0, original_दीर्घता=pBiDi->original_दीर्घता;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_बहुलकम् == UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम् ||
            pBiDi->reordering_बहुलकम् == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t चरमः_Arabic_Pos=-1;
    int32_t control_गणनम्=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    गणना State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp चरमः_Strong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_Start_राशिः[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  previous_State_राशिः[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t राशिः_चरमः=-1;

    यदि(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    यदि(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        चरमः_Strong=defaultParaLevel;
        यदि(pBiDi->pro_दीर्घता>0 &&                    /* there is a prologue */
           (dirProp=firstL_R_AL(pBiDi))!=ON) {  /* with a strong character */
            यदि(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            अन्यथा
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } अन्यथा {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } अन्यथा {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    प्रति( /* i=0 above */ ; i<original_दीर्घता; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, original_दीर्घता, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        यदि(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        यदि(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_गणनम्++;
        यदि(dirProp==L) {
            यदि(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_गणनम्-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            अन्यथा यदि(state==SEEKING_STRONG_FOR_FSI) {
                यदि(राशिः_चरमः<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            चरमः_Strong=L;
            अनुवर्त;
        }
        यदि(dirProp==R || dirProp==AL) {
            यदि(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_गणनम्-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            अन्यथा यदि(state==SEEKING_STRONG_FOR_FSI) {
                यदि(राशिः_चरमः<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_Start_राशिः[राशिः_चरमः]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            चरमः_Strong=R;
            यदि(dirProp==AL)
                चरमः_Arabic_Pos=i-1;
            अनुवर्त;
        }
        यदि(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            राशिः_चरमः++;
            यदि(राशिः_चरमः<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_Start_राशिः[राशिः_चरमः]=i-1;
                previous_State_राशिः[राशिः_चरमः]=state;
            }
            यदि(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            अन्यथा
                state=LOOKING_FOR_PDI;
            अनुवर्त;
        }
        यदि(dirProp==PDI) {
            यदि(state==SEEKING_STRONG_FOR_FSI) {
                यदि(राशिः_चरमः<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            यदि(राशिः_चरमः>=0) {
                यदि(राशिः_चरमः<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=previous_State_राशिः[राशिः_चरमः];
                राशिः_चरमः--;
            }
            अनुवर्त;
        }
        यदि(dirProp==B) {
            यदि(i<original_दीर्घता && uchar==CR && text[i]==LF) /* do nothing on the CR */
                अनुवर्त;
            pBiDi->paras[pBiDi->para_गणनम्-1].सीमा=i;
            यदि(isDefaultLevelInverse && चरमः_Strong==R)
                pBiDi->paras[pBiDi->para_गणनम्-1].level=1;
            यदि(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_गणनम्=control_गणनम्;
            }
            यदि(i<original_दीर्घता) {              /* B not last char in text */
                pBiDi->para_गणनम्++;
                यदि(check_Para_गणनम्(pBiDi)==असत्य)    /* not enough memory for a new para entry */
                    प्रत्यागम असत्य;
                यदि(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_गणनम्-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    चरमः_Strong=defaultParaLevel;
                } अन्यथा {
                    pBiDi->paras[pBiDi->para_गणनम्-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                राशिः_चरमः=-1;
            }
            अनुवर्त;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    यदि(राशिः_चरमः>UBIDI_MAX_EXPLICIT_LEVEL) {
        राशिः_चरमः=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    यावत्(राशिः_चरमः>=0) {
        यदि(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            भङ्ग;
        }
        state=previous_State_राशिः[राशिः_चरमः];
        राशिः_चरमः--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    यदि(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        यदि(pBiDi->length<original_दीर्घता)
            pBiDi->para_गणनम्--;
    } अन्यथा {
        pBiDi->paras[pBiDi->para_गणनम्-1].सीमा=original_दीर्घता;
        pBiDi->control_गणनम्=control_गणनम्;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    यदि(isDefaultLevelInverse && चरमः_Strong==R) {
        pBiDi->paras[pBiDi->para_गणनम्-1].level=1;
    }
    यदि(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    प्रति(i=0; i<pBiDi->para_गणनम्; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    यदि(pBiDi->क्रमः_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->चरमः_Arabic_Pos=चरमः_Arabic_Pos;
    प्रत्यागम सत्य;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_सूचकांक(ध्रुव UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    प्रति(i=0; i<pBiDi->para_गणनम्; i++)
        यदि(pindex<pBiDi->paras[i].सीमा)
            भङ्ग;
    यदि(i>=pBiDi->para_गणनम्)
        i=pBiDi->para_गणनम्-1;
    प्रत्यागम (UBiDiLevel)(pBiDi->paras[i].level);
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

स्थिर शून्य
bracketInit(UBiDi *pBiDi, Bracket_दत्त *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_सततम्_चरमः=0;
    bd->isoRuns[0].start=0;
    bd->isoRuns[0].सीमा=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].चरमः_Strong = bd->isoRuns[0].चरमः_आधारः = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    यदि(pBiDi->openings_स्मृतिः) {
        bd->openings=pBiDi->openings_स्मृतिः;
        bd->openings_गणनम्=pBiDi->openings_आकार / परिमाण(Opening);
    } अन्यथा {
        bd->openings=bd->simpleOpenings;
        bd->openings_गणनम्=SIMPLE_OPENINGS_गणनम्;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_बहुलकम्==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_बहुलकम्==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
स्थिर शून्य
bracketProcessB(Bracket_दत्त *bd, UBiDiLevel level) {
    bd->iso_सततम्_चरमः=0;
    bd->isoRuns[0].सीमा=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].चरमः_Strong=bd->isoRuns[0].चरमः_आधारः=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
स्थिर शून्य
bracketProcessBoundary(Bracket_दत्त *bd, int32_t चरमः_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    DirProp *dirProps=bd->pBiDi->dirProps;
    यदि(DIRPROP_FLAG(dirProps[चरमः_Cc_Pos])&आवरणम्_ISO)  /* after an isolate */
        प्रत्यागम;
    यदि(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_चरमः_Iso_सततम्->सीमा=p_चरमः_Iso_सततम्->start;
    p_चरमः_Iso_सततम्->level=embeddingLevel;
    p_चरमः_Iso_सततम्->चरमः_Strong=p_चरमः_Iso_सततम्->चरमः_आधारः=contextLevel&1;
    p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_चरमः_Iso_सततम्->contextPos = static_cast<UBiDiDirection>(चरमः_Cc_Pos);
}

/* LRI or RLI */
स्थिर शून्य
bracketProcessLRI_RLI(Bracket_दत्त *bd, UBiDiLevel level) {
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    int16_t चरमः_सीमा;
    p_चरमः_Iso_सततम्->चरमः_आधारः=ON;
    चरमः_सीमा=p_चरमः_Iso_सततम्->सीमा;
    bd->iso_सततम्_चरमः++;
    p_चरमः_Iso_सततम्++;
    p_चरमः_Iso_सततम्->start=p_चरमः_Iso_सततम्->सीमा=चरमः_सीमा;
    p_चरमः_Iso_सततम्->level=level;
    p_चरमः_Iso_सततम्->चरमः_Strong=p_चरमः_Iso_सततम्->चरमः_आधारः=level&1;
    p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_चरमः_Iso_सततम्->contextPos=0;
}

/* PDI */
स्थिर शून्य
bracketProcessPDI(Bracket_दत्त *bd) {
    Iso_सततम् *p_चरमः_Iso_सततम्;
    bd->iso_सततम्_चरमः--;
    p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    p_चरमः_Iso_सततम्->चरमः_आधारः=ON;
}

/* newly found opening bracket: create an openings entry */
स्थिर UBool                            /* return true if success */
bracket_सङ्कलय_Opening(Bracket_दत्त *bd, char16_t मेलनम्, int32_t position) {
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    Opening *pOpening;
    यदि(p_चरमः_Iso_सततम्->सीमा>=bd->openings_गणनम्) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        यदि(!get_Initial_Openings_स्मृतिः(pBiDi, p_चरमः_Iso_सततम्->सीमा * 2))
            प्रत्यागम असत्य;
        यदि(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_स्मृतिः, bd->simpleOpenings,
                        SIMPLE_OPENINGS_गणनम् * परिमाण(Opening));
        bd->openings=pBiDi->openings_स्मृतिः;     /* may have changed */
        bd->openings_गणनम्=pBiDi->openings_आकार / परिमाण(Opening);
    }
    pOpening=&bd->openings[p_चरमः_Iso_सततम्->सीमा];
    pOpening->position=position;
    pOpening->मेलनम्=मेलनम्;
    pOpening->contextDir=p_चरमः_Iso_सततम्->contextDir;
    pOpening->contextPos=p_चरमः_Iso_सततम्->contextPos;
    pOpening->flags=0;
    p_चरमः_Iso_सततम्->सीमा++;
    प्रत्यागम सत्य;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
स्थिर शून्य
fixN0c(Bracket_दत्त *bd, int32_t opening_सूचकांक, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    प्रति(k=opening_सूचकांक+1, qOpening=&bd->openings[k]; k<p_चरमः_Iso_सततम्->सीमा; k++, qOpening++) {
        यदि(qOpening->मेलनम्>=0)      /* not an N0c match */
            अनुवर्त;
        यदि(newPropPosition<qOpening->contextPos)
            भङ्ग;
        यदि(newPropPosition>=qOpening->position)
            अनुवर्त;
        यदि(newProp==qOpening->contextDir)
            भङ्ग;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->मेलनम्);
        dirProps[closingPosition]=newProp;
        qOpening->मेलनम्=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
स्थिर DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_दत्त *bd, int32_t openIdx, int32_t position) {
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_चरमः_Iso_सततम्->level & 1);
    stable=सत्य;            /* assume stable until proved otherwise */

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

    यदि((direction==0 && pOpening->flags&प्राप्तम्_L) ||
       (direction==1 && pOpening->flags&प्राप्तम्_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    अन्यथा यदि(pOpening->flags&(प्राप्तम्_L|प्राप्तम्_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_चरमः_Iso_सततम्->start);
        यदि(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        अन्यथा
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } अन्यथा {
        /* forget this and any brackets nested within this pair */
        p_चरमः_Iso_सततम्->सीमा= static_cast<uint16_t>(openIdx);
        प्रत्यागम ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    यदि(stable) {
        p_चरमः_Iso_सततम्->सीमा= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        यावत्(p_चरमः_Iso_सततम्->सीमा>p_चरमः_Iso_सततम्->start &&
              bd->openings[p_चरमः_Iso_सततम्->सीमा-1].position==pOpening->position)
            p_चरमः_Iso_सततम्->सीमा--;
    } अन्यथा {
        int32_t k;
        pOpening->मेलनम्=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        यावत्(k>=p_चरमः_Iso_सततम्->start &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].मेलनम्=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        प्रति(k=openIdx+1; k<p_चरमः_Iso_सततम्->सीमा; k++) {
            qOpening=&bd->openings[k];
            यदि(qOpening->position>=position)
                भङ्ग;
            यदि(qOpening->मेलनम्>0)
                qOpening->मेलनम्=0;
        }
    }
    प्रत्यागम newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
स्थिर UBool                            /* return true if success */
bracketProcessChar(Bracket_दत्त *bd, int32_t position) {
    Iso_सततम् *p_चरमः_Iso_सततम्=&bd->isoRuns[bd->iso_सततम्_चरमः];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    यदि(dirProp==ON) {
        char16_t c, मेलनम्;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->text[position];
        प्रति(idx=p_चरमः_Iso_सततम्->सीमा-1; idx>=p_चरमः_Iso_सततम्->start; idx--) {
            यदि(bd->openings[idx].मेलनम्!=c)
                अनुवर्त;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            यदि(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                भङ्ग;
            }
            p_चरमः_Iso_सततम्->चरमः_आधारः=ON;
            p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(newProp);
            p_चरमः_Iso_सततम्->contextPos=position;
            level=bd->pBiDi->स्तराः[position];
            यदि(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_चरमः_Iso_सततम्->चरमः_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                प्रति(i=p_चरमः_Iso_सततम्->start; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->स्तराः[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->स्तराः[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            प्रत्यागम सत्य;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        यदि(c)
            मेलनम्= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        अन्यथा
            मेलनम्=0;
        यदि(मेलनम्!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            यदि(मेलनम्==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                यदि(!bracket_सङ्कलय_Opening(bd, 0x3009, position))
                    प्रत्यागम असत्य;
            }
            अन्यथा यदि(मेलनम्==0x3009) {         /* RIGHT ANGLE BRACKET */
                यदि(!bracket_सङ्कलय_Opening(bd, 0x232A, position))
                    प्रत्यागम असत्य;
            }
            यदि(!bracket_सङ्कलय_Opening(bd, मेलनम्, position))
                प्रत्यागम असत्य;
        }
    }
    level=bd->pBiDi->स्तराः[position];
    यदि(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        यदि(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_चरमः_Iso_सततम्->चरमः_आधारः=newProp;
        p_चरमः_Iso_सततम्->चरमः_Strong=newProp;
        p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(newProp);
        p_चरमः_Iso_सततम्->contextPos=position;
    }
    अन्यथा यदि(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_चरमः_Iso_सततम्->चरमः_आधारः=dirProp;
        p_चरमः_Iso_सततम्->चरमः_Strong=dirProp;
        p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(newProp);
        p_चरमः_Iso_सततम्->contextPos=position;
    }
    अन्यथा यदि(dirProp==EN) {
        p_चरमः_Iso_सततम्->चरमः_आधारः=EN;
        यदि(p_चरमः_Iso_सततम्->चरमः_Strong==L) {
            newProp=L;                  /* W7 */
            यदि(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(L);
            p_चरमः_Iso_सततम्->contextPos=position;
        }
        अन्यथा {
            newProp=R;                  /* N0 */
            यदि(p_चरमः_Iso_सततम्->चरमः_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            अन्यथा
                dirProps[position]=ENR;
            p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(R);
            p_चरमः_Iso_सततम्->contextPos=position;
        }
    }
    अन्यथा यदि(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_चरमः_Iso_सततम्->चरमः_आधारः=AN;
        p_चरमः_Iso_सततम्->contextDir = static_cast<UBiDiDirection>(R);
        p_चरमः_Iso_सततम्->contextPos=position;
    }
    अन्यथा यदि(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_चरमः_Iso_सततम्->चरमः_आधारः;
        यदि(newProp==ON)
            dirProps[position]=newProp;
    }
    अन्यथा {
        newProp=dirProp;
        p_चरमः_Iso_सततम्->चरमः_आधारः=dirProp;
    }
    यदि(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        प्रति(i=p_चरमः_Iso_सततम्->start; i<p_चरमः_Iso_सततम्->सीमा; i++)
            यदि(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    प्रत्यागम सत्य;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
स्थिर UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    यदि(!(flags&आवरणम्_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&आवरणम्_POSSIBLE_N)))) {
        प्रत्यागम UBIDI_LTR;
    } अन्यथा यदि(!(flags&आवरणम्_LTR)) {
        प्रत्यागम UBIDI_RTL;
    } अन्यथा {
        प्रत्यागम UBIDI_MIXED;
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
स्थिर UBiDiDirection
resolve_Explicit_स्तराः(UBiDi *pBiDi, UError_संहिता *p_Error_संहिता) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *स्तराः=pBiDi->स्तराः;
    ध्रुव char16_t *text=pBiDi->text;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_गणनम्=0;

    यदि(U_FAILURE(*p_Error_संहिता)) { प्रत्यागम UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    यदि((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        प्रत्यागम direction;
    }
    यदि(pBiDi->reordering_बहुलकम् > UBIDI_REORDER_चरमः_LOGICAL_गन्तव्यम्_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_सूचकांक, start, सीमा;
        प्रति(para_सूचकांक=0; para_सूचकांक<pBiDi->para_गणनम्; para_सूचकांक++) {
            यदि(para_सूचकांक==0)
                start=0;
            अन्यथा
                start=pBiDi->paras[para_सूचकांक-1].सीमा;
            सीमा=pBiDi->paras[para_सूचकांक].सीमा;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_सूचकांक].level);
            प्रति(i=start; i<सीमा; i++)
                स्तराः[i]=level;
        }
        प्रत्यागम direction;   /* no bracket matching for inverse BiDi */
    }
    यदि(!(flags&(आवरणम्_EXPLICIT|आवरणम्_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_सूचकांक, start, सीमा;
        Bracket_दत्त bracket_दत्त;
        bracketInit(pBiDi, &bracket_दत्त);
        प्रति(para_सूचकांक=0; para_सूचकांक<pBiDi->para_गणनम्; para_सूचकांक++) {
            यदि(para_सूचकांक==0)
                start=0;
            अन्यथा
                start=pBiDi->paras[para_सूचकांक-1].सीमा;
            सीमा=pBiDi->paras[para_सूचकांक].सीमा;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_सूचकांक].level);
            प्रति(i=start; i<सीमा; i++) {
                स्तराः[i]=level;
                dirProp=dirProps[i];
                यदि(dirProp==BN)
                    अनुवर्त;
                यदि(dirProp==B) {
                    यदि((i+1)<length) {
                        यदि(text[i]==CR && text[i+1]==LF)
                            अनुवर्त;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_दत्त, level);
                    }
                    अनुवर्त;
                }
                यदि(!bracketProcessChar(&bracket_दत्त, i)) {
                    *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                    प्रत्यागम UBIDI_LTR;
                }
            }
        }
        प्रत्यागम direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel previousLevel=level;     /* previous level for regular (not CC) characters */
        int32_t चरमः_Cc_Pos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t राशिः_2[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t राशिः_चरमः=0;
        int32_t अतिप्रवाहः_Isolate_गणनम्=0;
        int32_t अतिप्रवाहः_Embedding_गणनम्=0;
        int32_t valid_Isolate_गणनम्=0;
        Bracket_दत्त bracket_दत्त;
        bracketInit(pBiDi, &bracket_दत्त);
        राशिः_2[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        प्रति(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            विकल्प(dirProp) {
            पक्ष LRE:
            पक्ष RLE:
            पक्ष LRO:
            पक्ष RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                स्तराः[i]=previousLevel;
                यदि (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                अन्यथा
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                यदि(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && अतिप्रवाहः_Isolate_गणनम्==0 &&
                                                         अतिप्रवाहः_Embedding_गणनम्==0) {
                    चरमः_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    यदि(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    राशिः_चरमः++;
                    राशिः_2[राशिः_चरमः]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } अन्यथा {
                    यदि(अतिप्रवाहः_Isolate_गणनम्==0)
                        अतिप्रवाहः_Embedding_गणनम्++;
                }
                भङ्ग;
            पक्ष PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                स्तराः[i]=previousLevel;
                /* handle all the overflow cases first */
                यदि(अतिप्रवाहः_Isolate_गणनम्) {
                    भङ्ग;
                }
                यदि(अतिप्रवाहः_Embedding_गणनम्) {
                    अतिप्रवाहः_Embedding_गणनम्--;
                    भङ्ग;
                }
                यदि(राशिः_चरमः>0 && राशिः_2[राशिः_चरमः]<ISOLATE) {   /* not an isolate entry */
                    चरमः_Cc_Pos=i;
                    राशिः_चरमः--;
                    embeddingLevel = static_cast<UBiDiLevel>(राशिः_2[राशिः_चरमः]);
                }
                भङ्ग;
            पक्ष LRI:
            पक्ष RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                स्तराः[i]=NO_OVERRIDE(embeddingLevel);
                यदि(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_दत्त, चरमः_Cc_Pos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                previousLevel=embeddingLevel;
                /* (X5a, X5b) */
                यदि(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                अन्यथा
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                यदि(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && अतिप्रवाहः_Isolate_गणनम्==0 &&
                                                         अतिप्रवाहः_Embedding_गणनम्==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    चरमः_Cc_Pos=i;
                    valid_Isolate_गणनम्++;
                    यदि(valid_Isolate_गणनम्>pBiDi->isolate_गणनम्)
                        pBiDi->isolate_गणनम्=valid_Isolate_गणनम्;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    राशिः_चरमः++;
                    राशिः_2[राशिः_चरमः]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_दत्त, embeddingLevel);
                } अन्यथा {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    अतिप्रवाहः_Isolate_गणनम्++;
                }
                भङ्ग;
            पक्ष PDI:
                यदि(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_दत्त, चरमः_Cc_Pos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                यदि(अतिप्रवाहः_Isolate_गणनम्) {
                    अतिप्रवाहः_Isolate_गणनम्--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                अन्यथा यदि(valid_Isolate_गणनम्) {
                    flags|=DIRPROP_FLAG(PDI);
                    चरमः_Cc_Pos=i;
                    अतिप्रवाहः_Embedding_गणनम्=0;
                    यावत्(राशिः_2[राशिः_चरमः]<ISOLATE) /* pop embedding entries */
                        राशिः_चरमः--;                /* until the last isolate entry */
                    राशिः_चरमः--;                    /* pop also the last isolate entry */
                    valid_Isolate_गणनम्--;
                    bracketProcessPDI(&bracket_दत्त);
                } अन्यथा
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(राशिः_2[राशिः_चरमः]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                previousLevel=embeddingLevel;
                स्तराः[i]=NO_OVERRIDE(embeddingLevel);
                भङ्ग;
            पक्ष B:
                flags|=DIRPROP_FLAG(B);
                स्तराः[i]=GET_PARALEVEL(pBiDi, i);
                यदि((i+1)<length) {
                    यदि(text[i]==CR && text[i+1]==LF)
                        भङ्ग;          /* skip CR when followed by LF */
                    अतिप्रवाहः_Embedding_गणनम्=अतिप्रवाहः_Isolate_गणनम्=0;
                    valid_Isolate_गणनम्=0;
                    राशिः_चरमः=0;
                    previousLevel=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    राशिः_2[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_दत्त, embeddingLevel);
                }
                भङ्ग;
            पक्ष BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                स्तराः[i]=previousLevel;
                flags|=DIRPROP_FLAG(BN);
                भङ्ग;
            स्वभाव:
                /* all other types are normal characters and get the "real" level */
                यदि(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_दत्त, चरमः_Cc_Pos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    यदि(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    अन्यथा
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                previousLevel=embeddingLevel;
                स्तराः[i]=embeddingLevel;
                यदि(!bracketProcessChar(&bracket_दत्त, i))
                    प्रत्यागम static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                भङ्ग;
            }
        }
        यदि(flags&आवरणम्_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        यदि(pBiDi->क्रमः_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    प्रत्यागम direction;
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
स्थिर UBiDiDirection
check_Explicit_स्तराः(UBiDi *pBiDi, UError_संहिता *p_Error_संहिता) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *स्तराः=pBiDi->स्तराः;
    int32_t isolate_गणनम्=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_गणनम्=0;

    int32_t current_Para_सूचकांक = 0;
    int32_t current_Para_सीमा = pBiDi->paras[0].सीमा;
    int32_t currentParaLevel = pBiDi->paraLevel;

    प्रति(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=स्तराः[i];
        DirProp dirProp=dirProps[i];
        यदि(dirProp==LRI || dirProp==RLI) {
            isolate_गणनम्++;
            यदि(isolate_गणनम्>pBiDi->isolate_गणनम्)
                pBiDi->isolate_गणनम्=isolate_गणनम्;
        }
        अन्यथा यदि(dirProp==PDI)
            isolate_गणनम्--;
        अन्यथा यदि(dirProp==B)
            isolate_गणनम्=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        यदि (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_सीमा && (current_Para_सूचकांक + 1) < pBiDi->para_गणनम्) {
            currentParaLevel = pBiDi->paras[++current_Para_सूचकांक].level;
            current_Para_सीमा = pBiDi->paras[current_Para_सूचकांक].सीमा;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        यदि (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            यदि (level == 0) {
                यदि (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } अन्यथा {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    स्तराः[i] = level | overrideFlag;
                }
            } अन्यथा {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम UBIDI_LTR;
            }
        }
        यदि (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } अन्यथा {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    यदि(flags&आवरणम्_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    प्रत्यागम directionFromFlags(pBiDi);
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
#परिभाषय IMPTABPROPS_COLUMNS 16
#परिभाषय IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#परिभाषय GET_STATEPROPS(cell) ((cell)&0x1f)
#परिभाषय GET_ACTIONPROPS(cell) ((cell)>>5)
#परिभाषय s(action, newState) ((uint8_t)(newState+(action<<5)))

स्थिर ध्रुव uint8_t गणः_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
गणना { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

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
स्थिर ध्रुव uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
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
#अपरिभाषय s

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
#परिभाषय IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#परिभाषय IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#परिभाषय GET_STATE(cell) ((cell)&0x0f)
#परिभाषय GET_ACTION(cell) ((cell)>>4)
#परिभाषय s(action, newState) ((uint8_t)(newState+(action<<4)))

प्रकारनिर्देश uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
प्रकारनिर्देश uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
प्रकारनिर्देश संरचना ImpTabPair {
    ध्रुव शून्य * pImpTab[2];
    ध्रुव शून्य * pImpAct[2];
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

स्थिर ध्रुव ImpTab impTabL_DEFAULT =   /* Even paragraph level */
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
स्थिर ध्रुव ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
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
स्थिर ध्रुव ImpAct impAct0 = {0,1,2,3,4};
स्थिर ध्रुव ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

स्थिर ध्रुव ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
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
स्थिर ध्रुव ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

स्थिर ध्रुव ImpTab imp_Tab_L_गणः_NUMBERS_WITH_R =
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
स्थिर ध्रुव ImpTab imp_Tab_R_गणः_NUMBERS_WITH_R =
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
स्थिर ध्रुव ImpTabPair imp_Tab_गणः_NUMBERS_WITH_R = {
                        {&imp_Tab_L_गणः_NUMBERS_WITH_R,
                         &imp_Tab_R_गणः_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


स्थिर ध्रुव ImpTab impTabL_INVERSE_NUMBERS_AS_L =
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
स्थिर ध्रुव ImpTab impTabR_INVERSE_NUMBERS_AS_L =
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
स्थिर ध्रुव ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

स्थिर ध्रुव ImpTab imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम् =   /* Odd  paragraph level */
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
स्थिर ध्रुव ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
स्थिर ध्रुव ImpTabPair imp_Tab_INVERSE_LIKE_प्रत्यक्षम् = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम्},
                        {&impAct0, &impAct1}};

स्थिर ध्रुव ImpTab imp_Tab_L_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS =
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
स्थिर ध्रुव ImpTab imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS =
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
स्थिर ध्रुव ImpAct impAct2 = {0,1,2,5,6,7,8};
स्थिर ध्रुव ImpAct impAct3 = {0,1,9,10,11,12};
स्थिर ध्रुव ImpTabPair imp_Tab_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS},
                        {&impAct2, &impAct3}};

स्थिर ध्रुव ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम्},
                        {&impAct0, &impAct1}};

स्थिर ध्रुव ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
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
स्थिर ध्रुव ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS},
                        {&impAct2, &impAct3}};

#अपरिभाषय s

प्रकारनिर्देश संरचना {
    ध्रुव ImpTab * pImpTab;             /* level table pointer          */
    ध्रुव ImpAct * pImpAct;             /* action map array             */
    int32_t startON;                    /* start of ON sequence         */
    int32_t startL2EN;                  /* start of level 2 sequence    */
    int32_t चरमः_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t सततम्_Start;                   /* start position of the run    */
    UBiDiLevel सततम्_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

स्थिर शून्य
सङ्कलय_Point(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#परिभाषय FIRSTALLOC  10
    Point point;
    निवेशय_Points_2 * p_निवेशय_Points=&(pBiDi->निवेशय_Points);

    यदि (p_निवेशय_Points->capacity == 0)
    {
        p_निवेशय_Points->points=static_cast<Point *>(uprv_malloc(परिमाण(Point)*FIRSTALLOC));
        यदि (p_निवेशय_Points->points == शून्यसूचकः)
        {
            p_निवेशय_Points->error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        p_निवेशय_Points->capacity=FIRSTALLOC;
    }
    यदि (p_निवेशय_Points->size >= p_निवेशय_Points->capacity) /* no room for new point */
    {
        Point * savePoints=p_निवेशय_Points->points;
        p_निवेशय_Points->points=static_cast<Point *>(uprv_realloc(p_निवेशय_Points->points,
                                           p_निवेशय_Points->capacity*2*परिमाण(Point)));
        यदि (p_निवेशय_Points->points == शून्यसूचकः)
        {
            p_निवेशय_Points->points=savePoints;
            p_निवेशय_Points->error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        अन्यथा  p_निवेशय_Points->capacity*=2;
    }
    point.pos=pos;
    point.flag=flag;
    p_निवेशय_Points->points[p_निवेशय_Points->size]=point;
    p_निवेशय_Points->size++;
#अपरिभाषय FIRSTALLOC
}

स्थिर शून्य
समुच्चयः_स्तराः_Outside_Isolates(UBiDi *pBiDi, int32_t start, int32_t सीमा, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *स्तराः=pBiDi->स्तराः;
    int32_t isolate_गणनम्=0, k;
    प्रति(k=start; k<सीमा; k++) {
        dirProp=dirProps[k];
        यदि(dirProp==PDI)
            isolate_गणनम्--;
        यदि(isolate_गणनम्==0)
            स्तराः[k]=level;
        यदि(dirProp==LRI || dirProp==RLI)
            isolate_गणनम्++;
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

स्थिर शून्य
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t start, int32_t सीमा) {
    uint8_t cell, oldStateSeq, actionSeq;
    ध्रुव ImpTab * pImpTab=pLevState->pImpTab;
    ध्रुव ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * स्तराः=pBiDi->स्तराः;
    UBiDiLevel level, सङ्कलय_Level;
    निवेशय_Points_2 * p_निवेशय_Points;
    int32_t start0, k;

    start0=start;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    सङ्कलय_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    यदि(actionSeq) {
        विकल्प(actionSeq) {
        पक्ष 1:                         /* init ON seq */
            pLevState->startON=start0;
            भङ्ग;

        पक्ष 2:                         /* prepend ON seq to current seq */
            start=pLevState->startON;
            भङ्ग;

        पक्ष 3:                         /* EN/AN after R+ON */
            level=pLevState->सततम्_Level+1;
            समुच्चयः_स्तराः_Outside_Isolates(pBiDi, pLevState->startON, start0, level);
            भङ्ग;

        पक्ष 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->सततम्_Level+2;
            समुच्चयः_स्तराः_Outside_Isolates(pBiDi, pLevState->startON, start0, level);
            भङ्ग;

        पक्ष 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            यदि (pLevState->startL2EN >= 0) {
                सङ्कलय_Point(pBiDi, pLevState->startL2EN, LRM_BEFORE);
            }
            pLevState->startL2EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_निवेशय_Points=&(pBiDi->निवेशय_Points);
            यदि ((p_निवेशय_Points->capacity == 0) ||
                (p_निवेशय_Points->size <= p_निवेशय_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->चरमः_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                यदि ((level & 1) && (pLevState->startON > 0)) {  /* after ON */
                    start=pLevState->startON;   /* reset to basic run level */
                }
                यदि (_prop == DirProp_S)                /* add LRM before S */
                {
                    सङ्कलय_Point(pBiDi, start0, LRM_BEFORE);
                    p_निवेशय_Points->confirmed=p_निवेशय_Points->size;
                }
                भङ्ग;
            }
            /* reset previous RTL cont to level for LTR text */
            प्रति (k=pLevState->चरमः_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                स्तराः[k]=(स्तराः[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_निवेशय_Points->confirmed=p_निवेशय_Points->size;
            pLevState->चरमः_Strong_RTL=-1;
            यदि (_prop == DirProp_S)            /* add LRM before S */
            {
                सङ्कलय_Point(pBiDi, start0, LRM_BEFORE);
                p_निवेशय_Points->confirmed=p_निवेशय_Points->size;
            }
            भङ्ग;

        पक्ष 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_निवेशय_Points=&(pBiDi->निवेशय_Points);
            यदि (p_निवेशय_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_निवेशय_Points->size=p_निवेशय_Points->confirmed;
            pLevState->startON=-1;
            pLevState->startL2EN=-1;
            pLevState->चरमः_Strong_RTL=सीमा - 1;
            भङ्ग;

        पक्ष 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            यदि ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_बहुलकम्!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                यदि (pLevState->startL2EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->चरमः_Strong_RTL=सीमा - 1;
                    भङ्ग;
                }
                यदि (pLevState->startL2EN >= 0)  /* after EN, no AN */
                {
                    सङ्कलय_Point(pBiDi, pLevState->startL2EN, LRM_BEFORE);
                    pLevState->startL2EN=-2;
                }
                /* note AN */
                सङ्कलय_Point(pBiDi, start0, LRM_BEFORE);
                भङ्ग;
            }
            /* if first EN/AN after R/AL */
            यदि (pLevState->startL2EN == -1) {
                pLevState->startL2EN=start0;
            }
            भङ्ग;

        पक्ष 8:                         /* note location of latest R/AL */
            pLevState->चरमः_Strong_RTL=सीमा - 1;
            pLevState->startON=-1;
            भङ्ग;

        पक्ष 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            प्रति (k=start0-1; k>=0 && !(स्तराः[k]&1); k--);
            यदि(k>=0) {
                सङ्कलय_Point(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_निवेशय_Points=&(pBiDi->निवेशय_Points);
                p_निवेशय_Points->confirmed=p_निवेशय_Points->size;   /* confirm it */
            }
            pLevState->startON=start0;
            भङ्ग;

        पक्ष 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            सङ्कलय_Point(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            सङ्कलय_Point(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            भङ्ग;

        पक्ष 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_निवेशय_Points=&(pBiDi->निवेशय_Points);
            p_निवेशय_Points->size=p_निवेशय_Points->confirmed;
            यदि (_prop == DirProp_S)            /* add RLM before S */
            {
                सङ्कलय_Point(pBiDi, start0, RLM_BEFORE);
                p_निवेशय_Points->confirmed=p_निवेशय_Points->size;
            }
            भङ्ग;

        पक्ष 12:                        /* L after L+ON/AN */
            level=pLevState->सततम्_Level + सङ्कलय_Level;
            प्रति(k=pLevState->startON; k<start0; k++) {
                यदि (स्तराः[k]<level)
                    स्तराः[k]=level;
            }
            p_निवेशय_Points=&(pBiDi->निवेशय_Points);
            p_निवेशय_Points->confirmed=p_निवेशय_Points->size;   /* confirm inserts */
            pLevState->startON=start0;
            भङ्ग;

        पक्ष 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->सततम्_Level;
            प्रति(k=start0-1; k>=pLevState->startON; k--) {
                यदि(स्तराः[k]==level+3) {
                    यावत्(स्तराः[k]==level+3) {
                        स्तराः[k--]-=2;
                    }
                    यावत्(स्तराः[k]==level) {
                        k--;
                    }
                }
                यदि(स्तराः[k]==level+2) {
                    स्तराः[k]=level;
                    अनुवर्त;
                }
                स्तराः[k]=level+1;
            }
            भङ्ग;

        पक्ष 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->सततम्_Level+1;
            प्रति(k=start0-1; k>=pLevState->startON; k--) {
                यदि(स्तराः[k]>level) {
                    स्तराः[k]-=2;
                }
            }
            भङ्ग;

        स्वभाव:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    यदि((सङ्कलय_Level) || (start < start0)) {
        level=pLevState->सततम्_Level + सङ्कलय_Level;
        यदि(start>=pLevState->सततम्_Start) {
            प्रति(k=start; k<सीमा; k++) {
                स्तराः[k]=level;
            }
        } अन्यथा {
            समुच्चयः_स्तराः_Outside_Isolates(pBiDi, start, सीमा, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
स्थिर DirProp
चरमः_L_R_AL(UBiDi *pBiDi) {
    ध्रुव char16_t *text=pBiDi->prologue;
    int32_t length=pBiDi->pro_दीर्घता;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    प्रति(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_पूर्व(text, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        यदि(dirProp==L) {
            प्रत्यागम DirProp_L;
        }
        यदि(dirProp==R || dirProp==AL) {
            प्रत्यागम DirProp_R;
        }
        यदि(dirProp==B) {
            प्रत्यागम DirProp_ON;
        }
    }
    प्रत्यागम DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
स्थिर DirProp
firstL_R_AL_EN_AN(UBiDi *pBiDi) {
    ध्रुव char16_t *text=pBiDi->epilogue;
    int32_t length=pBiDi->epi_दीर्घता;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    प्रति(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        यदि(dirProp==L) {
            प्रत्यागम DirProp_L;
        }
        यदि(dirProp==R || dirProp==AL) {
            प्रत्यागम DirProp_R;
        }
        यदि(dirProp==EN) {
            प्रत्यागम DirProp_EN;
        }
        यदि(dirProp==AN) {
            प्रत्यागम DirProp_AN;
        }
    }
    प्रत्यागम DirProp_ON;
}

स्थिर शून्य
resolve_Implicit_स्तराः(UBiDi *pBiDi,
                      int32_t start, int32_t सीमा,
                      DirProp sor, DirProp eor) {
    ध्रुव DirProp *dirProps=pBiDi->dirProps;
    DirProp dirProp;
    LevState levState;
    int32_t i, start1, start2;
    uint16_t oldStateImp, stateImp, actionImp;
    uint8_t gprop, resProp, cell;
    UBool inverseRTL;
    DirProp अग्र_Strong_Prop=R;
    int32_t अग्र_Strong_Pos=-1;

    /* check for RTL inverse BiDi mode */
    /* FOOD FOR THOUGHT: in case of RTL inverse BiDi, it would make sense to
     * loop on the text characters from end to start.
     * This would need a different properties state table (at least different
     * actions) and different levels state tables (maybe very similar to the
     * LTR corresponding ones.
     */
    inverseRTL =
        static_cast<UBool>((start < pBiDi->चरमः_Arabic_Pos) && (GET_PARALEVEL(pBiDi, start) & 1) &&
                           (pBiDi->reordering_बहुलकम् == UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम् ||
                            pBiDi->reordering_बहुलकम् == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.startL2EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.चरमः_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.सततम्_Start=start;
    levState.सततम्_Level=pBiDi->स्तराः[start];
    levState.pImpTab = static_cast<ध्रुव ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.सततम्_Level & 1]);
    levState.pImpAct = static_cast<ध्रुव ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.सततम्_Level & 1]);
    यदि(start==0 && pBiDi->pro_दीर्घता>0) {
        DirProp चरमः_Strong=चरमः_L_R_AL(pBiDi);
        यदि(चरमः_Strong!=DirProp_ON) {
            sor=चरमः_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    यदि(dirProps[start]==PDI  && pBiDi->isolate_गणनम् >= 0) {
        levState.startON=pBiDi->isolates[pBiDi->isolate_गणनम्].startON;
        start1=pBiDi->isolates[pBiDi->isolate_गणनम्].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_गणनम्].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_गणनम्].state;
        pBiDi->isolate_गणनम्--;
    } अन्यथा {
        levState.startON=-1;
        start1=start;
        यदि(dirProps[start]==NSM)
            stateImp = 1 + sor;
        अन्यथा
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, start, start);
    }
    start2=start;                       /* to make Java compiler happy */

    प्रति(i=start; i<=सीमा; i++) {
        यदि(i>=सीमा) {
            int32_t k;
            प्रति(k=सीमा-1; k>start&&(DIRPROP_FLAG(dirProps[k])&आवरणम्_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            यदि(dirProp==LRI || dirProp==RLI)
                भङ्ग;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } अन्यथा {
            DirProp prop, prop1;
            prop=dirProps[i];
            यदि(prop==B) {
                pBiDi->isolate_गणनम्=-1; /* current isolates stack entry == none */
            }
            यदि(inverseRTL) {
                यदि(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } अन्यथा यदि(prop==EN) {
                    यदि(अग्र_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        अग्र_Strong_Prop=R;   /* set default */
                        अग्र_Strong_Pos=सीमा;
                        प्रति(j=i+1; j<सीमा; j++) {
                            prop1=dirProps[j];
                            यदि(prop1==L || prop1==R || prop1==AL) {
                                अग्र_Strong_Prop=prop1;
                                अग्र_Strong_Pos=j;
                                भङ्ग;
                            }
                        }
                    }
                    यदि(अग्र_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=गणः_Prop[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        यदि((i==सीमा) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        यदि(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            विकल्प(actionImp) {
            पक्ष 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                भङ्ग;
            पक्ष 2:             /* init new seq2 */
                start2=i;
                भङ्ग;
            पक्ष 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                भङ्ग;
            पक्ष 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                भङ्ग;
            स्वभाव:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    यदि(सीमा==pBiDi->length && pBiDi->epi_दीर्घता>0) {
        DirProp firstStrong=firstL_R_AL_EN_AN(pBiDi);
        यदि(firstStrong!=DirProp_ON) {
            eor=firstStrong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    प्रति(i=सीमा-1; i>start&&(DIRPROP_FLAG(dirProps[i])&आवरणम्_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    यदि((dirProp==LRI || dirProp==RLI) && सीमा<pBiDi->length) {
        pBiDi->isolate_गणनम्++;
        pBiDi->isolates[pBiDi->isolate_गणनम्].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_गणनम्].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_गणनम्].start1=start1;
        pBiDi->isolates[pBiDi->isolate_गणनम्].startON=levState.startON;
    }
    अन्यथा
        processPropertySeq(pBiDi, &levState, eor, सीमा, सीमा);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
स्थिर शून्य
adjustWSLevels(UBiDi *pBiDi) {
    ध्रुव DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *स्तराः=pBiDi->स्तराः;
    int32_t i;

    यदि(pBiDi->flags&आवरणम्_WS) {
        UBool क्रमः_Paragraphs_LTR=pBiDi->क्रमः_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        यावत्(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            यावत्(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&आवरणम्_WS) {
                यदि(क्रमः_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    स्तराः[i]=0;
                } अन्यथा {
                    स्तराः[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            यावत्(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                यदि(flag&आवरणम्_BN_EXPLICIT) {
                    स्तराः[i]=स्तराः[i+1];
                } अन्यथा यदि(क्रमः_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    स्तराः[i]=0;
                    भङ्ग;
                } अन्यथा यदि(flag&आवरणम्_B_S) {
                    स्तराः[i]=GET_PARALEVEL(pBiDi, i);
                    भङ्ग;
                }
            }
        }
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Context(UBiDi *pBiDi,
                 ध्रुव char16_t *prologue, int32_t pro_दीर्घता,
                 ध्रुव char16_t *epilogue, int32_t epi_दीर्घता,
                 UError_संहिता *p_Error_संहिता) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    यदि(pBiDi==शून्यसूचकः || pro_दीर्घता<-1 || epi_दीर्घता<-1 ||
       (prologue==शून्यसूचकः && pro_दीर्घता!=0) || (epilogue==शून्यसूचकः && epi_दीर्घता!=0)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि(pro_दीर्घता==-1) {
        pBiDi->pro_दीर्घता=u_strlen(prologue);
    } अन्यथा {
        pBiDi->pro_दीर्घता=pro_दीर्घता;
    }
    यदि(epi_दीर्घता==-1) {
        pBiDi->epi_दीर्घता=u_strlen(epilogue);
    } अन्यथा {
        pBiDi->epi_दीर्घता=epi_दीर्घता;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

स्थिर शून्य
समुच्चयः_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_दीर्घता=0;                 /* forget the last context */
    pBiDi->epi_दीर्घता=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#परिभाषय BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#परिभाषय BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

स्थिर शून्य
समुच्चयः_Para_Runs_Only(UBiDi *pBiDi, ध्रुव char16_t *text, int32_t length,
                UBiDiLevel paraLevel, UError_संहिता *p_Error_संहिता) {
    int32_t *runs_Only_स्मृतिः = शून्यसूचकः;
    int32_t *visualMap;
    char16_t *visualText;
    int32_t save_दीर्घता, saveTrailingWSStart;
    ध्रुव UBiDiLevel *स्तराः;
    UBiDiLevel *save_स्तराः;
    UBiDiDirection saveDirection;
    UBool saveMayAllocateText;
    सततम् *runs;
    int32_t visual_दीर्घता, i, j, visualStart, logicalStart,
            सततम्_गणनम्, सततम्_दीर्घता, addedRuns, निवेशय_Remove,
            start, सीमा, चरणः, सूचकांक_Odd_Bit, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_बहुलकम्=UBIDI_REORDER_DEFAULT;
    यदि(length==0) {
        ubidi_समुच्चयः_Para(pBiDi, text, length, paraLevel, शून्यसूचकः, p_Error_संहिता);
        गच्छ cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_स्मृतिः=static_cast<int32_t *>(uprv_malloc(length*(परिमाण(int32_t)+परिमाण(char16_t)+परिमाण(UBiDiLevel))));
    यदि(runs_Only_स्मृतिः==शून्यसूचकः) {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        गच्छ cleanup3;
    }
    visualMap=runs_Only_स्मृतिः;
    visualText = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_स्तराः = reinterpret_cast<UBiDiLevel*>(&visualText[length]);
    saveOptions=pBiDi->reorderingOptions;
    यदि(saveOptions & UBIDI_OPTION_निवेशय_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_निवेशय_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_समुच्चयः_Para(pBiDi, text, length, paraLevel, शून्यसूचकः, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        गच्छ cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    स्तराः=ubidi_get_स्तराः(pBiDi, p_Error_संहिता);
    uprv_memcpy(save_स्तराः, स्तराः, (size_t)pBiDi->length*परिमाण(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_दीर्घता=pBiDi->length;
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
    visual_दीर्घता=ubidi_writeReordered(pBiDi, visualText, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_आन्तरिकम्_RUNS_ONLY, p_Error_संहिता);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        गच्छ cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_बहुलकम्=UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम्;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    saveMayAllocateText=pBiDi->mayAllocateText;
    pBiDi->mayAllocateText=असत्य;
    ubidi_समुच्चयः_Para(pBiDi, visualText, visual_दीर्घता, paraLevel, शून्यसूचकः, p_Error_संहिता);
    pBiDi->mayAllocateText=saveMayAllocateText;
    ubidi_getRuns(pBiDi, p_Error_संहिता);
    यदि(U_FAILURE(*p_Error_संहिता)) {
        गच्छ cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    सततम्_गणनम्=pBiDi->सततम्_गणनम्;
    runs=pBiDi->runs;
    visualStart=0;
    प्रति(i=0; i<सततम्_गणनम्; i++, visualStart+=सततम्_दीर्घता) {
        सततम्_दीर्घता=runs[i].visual_सीमा-visualStart;
        यदि(सततम्_दीर्घता<2) {
            अनुवर्त;
        }
        logicalStart=GET_सूचकांक(runs[i].logicalStart);
        प्रति(j=logicalStart+1; j<logicalStart+सततम्_दीर्घता; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            यदि((BIDI_ABS(index0-index1)!=1) || (save_स्तराः[index0]!=save_स्तराः[index1])) {
                addedRuns++;
            }
        }
    }
    यदि(addedRuns) {
        यदि(get_Runs_स्मृतिः(pBiDi, सततम्_गणनम्+addedRuns)) {
            यदि(सततम्_गणनम्==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_स्मृतिः[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_स्मृतिः;
            pBiDi->सततम्_गणनम्+=addedRuns;
        } अन्यथा {
            *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    प्रति(i=सततम्_गणनम्-1; i>=0; i--) {
        सततम्_दीर्घता= i==0 ? runs[0].visual_सीमा :
                          runs[i].visual_सीमा-runs[i-1].visual_सीमा;
        logicalStart=runs[i].logicalStart;
        सूचकांक_Odd_Bit=GET_ODD_BIT(logicalStart);
        logicalStart=GET_सूचकांक(logicalStart);
        यदि(सततम्_दीर्घता<2) {
            यदि(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logicalStart];
            runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            save_स्तराः[logicalPos]^सूचकांक_Odd_Bit);
            अनुवर्त;
        }
        यदि(सूचकांक_Odd_Bit) {
            start=logicalStart;
            सीमा=logicalStart+सततम्_दीर्घता-1;
            चरणः=1;
        } अन्यथा {
            start=logicalStart+सततम्_दीर्घता-1;
            सीमा=logicalStart;
            चरणः=-1;
        }
        प्रति(j=start; j!=सीमा; j+=चरणः) {
            index0=visualMap[j];
            index1=visualMap[j+चरणः];
            यदि((BIDI_ABS(index0-index1)!=1) || (save_स्तराः[index0]!=save_स्तराः[index1])) {
                logicalPos=BIDI_MIN(visualMap[start], index0);
                runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            save_स्तराः[logicalPos]^सूचकांक_Odd_Bit);
                runs[i+addedRuns].visual_सीमा=runs[i].visual_सीमा;
                runs[i].visual_सीमा-=BIDI_ABS(j-start)+1;
                निवेशय_Remove=runs[i].निवेशय_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].निवेशय_Remove=निवेशय_Remove;
                runs[i].निवेशय_Remove&=~निवेशय_Remove;
                start=j+चरणः;
                addedRuns--;
            }
        }
        यदि(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[start], visualMap[सीमा]);
        runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            save_स्तराः[logicalPos]^सूचकांक_Odd_Bit);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->text=text;
    pBiDi->length=save_दीर्घता;
    pBiDi->original_दीर्घता=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    यदि(save_दीर्घता>pBiDi->स्तराः_आकार) {
        save_दीर्घता=pBiDi->स्तराः_आकार;
    }
    uprv_memcpy(pBiDi->स्तराः, save_स्तराः, (size_t)save_दीर्घता*परिमाण(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    यदि(pBiDi->सततम्_गणनम्>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_स्मृतिः);

    pBiDi->reordering_बहुलकम्=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Para(UBiDi *pBiDi, ध्रुव char16_t *text, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_स्तराः,
              UError_संहिता *p_Error_संहिता) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    यदि(pBiDi==शून्यसूचकः || text==शून्यसूचकः || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि(length==-1) {
        length=u_strlen(text);
    }

    /* special treatment for RUNS_ONLY mode */
    यदि(pBiDi->reordering_बहुलकम्==UBIDI_REORDER_RUNS_ONLY) {
        समुच्चयः_Para_Runs_Only(pBiDi, text, length, paraLevel, p_Error_संहिता);
        प्रत्यागम;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=शून्यसूचकः;          /* mark unfinished setPara */
    pBiDi->text=text;
    pBiDi->length=pBiDi->original_दीर्घता=pBiDi->परिणाम_दीर्घता=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_गणनम्=1;

    pBiDi->dirProps=शून्यसूचकः;
    pBiDi->स्तराः=शून्यसूचकः;
    pBiDi->runs=शून्यसूचकः;
    pBiDi->निवेशय_Points.size=0;         /* clean up from last call */
    pBiDi->निवेशय_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    यदि(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        यदि(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->सततम्_गणनम्=0;
        pBiDi->para_गणनम्=0;
        समुच्चयः_Para_Success(pBiDi);          /* mark successful setPara */
        प्रत्यागम;
    }

    pBiDi->सततम्_गणनम्=-1;

    /* allocate paras memory */
    यदि(pBiDi->paras_स्मृतिः)
        pBiDi->paras=pBiDi->paras_स्मृतिः;
    अन्यथा
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    यदि(get_Dir_Props_स्मृतिः(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_स्मृतिः;
        यदि(!getDirProps(pBiDi)) {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    } अन्यथा {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    यदि(embedding_स्तराः==शून्यसूचकः) {
        /* no: determine explicit levels according to the (Xn) rules */\
        यदि(get_स्तराः_स्मृतिः(pBiDi, length)) {
            pBiDi->स्तराः=pBiDi->स्तराः_स्मृतिः;
            direction=resolve_Explicit_स्तराः(pBiDi, p_Error_संहिता);
            यदि(U_FAILURE(*p_Error_संहिता)) {
                प्रत्यागम;
            }
        } अन्यथा {
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    } अन्यथा {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->स्तराः=embedding_स्तराः;
        direction=check_Explicit_स्तराः(pBiDi, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम;
        }
    }

    /* allocate isolate memory */
    यदि(pBiDi->isolate_गणनम्<=SIMPLE_ISOLATES_गणनम्)
        pBiDi->isolates=pBiDi->simpleIsolates;
    अन्यथा
        यदि((int32_t)(pBiDi->isolate_गणनम्*परिमाण(Isolate))<=pBiDi->isolates_आकार)
            pBiDi->isolates=pBiDi->isolates_स्मृतिः;
        अन्यथा {
            यदि(get_Initial_Isolates_स्मृतिः(pBiDi, pBiDi->isolate_गणनम्)) {
                pBiDi->isolates=pBiDi->isolates_स्मृतिः;
            } अन्यथा {
                *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
        }
    pBiDi->isolate_गणनम्=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    विकल्प(direction) {
    पक्ष UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        भङ्ग;
    पक्ष UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        भङ्ग;
    स्वभाव:
        /*
         *  Choose the right implicit state table
         */
        विकल्प(pBiDi->reordering_बहुलकम्) {
        पक्ष UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            भङ्ग;
        पक्ष UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            भङ्ग;
        पक्ष UBIDI_REORDER_गणः_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_गणः_NUMBERS_WITH_R;
            भङ्ग;
        पक्ष UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            भङ्ग;
        पक्ष UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम्:
            यदि (pBiDi->reorderingOptions & UBIDI_OPTION_निवेशय_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_प्रत्यक्षम्_WITH_MARKS;
            } अन्यथा {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_प्रत्यक्षम्;
            }
            भङ्ग;
        पक्ष UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            यदि (pBiDi->reorderingOptions & UBIDI_OPTION_निवेशय_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } अन्यथा {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            भङ्ग;
        स्वभाव:
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
        यदि(embedding_स्तराः==शून्यसूचकः && pBiDi->para_गणनम्<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_स्तराः(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } अन्यथा {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *स्तराः=pBiDi->स्तराः;
            int32_t start, सीमा=0;
            UBiDiLevel level, अग्र_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            अग्र_Level=स्तराः[0];
            यदि(level<अग्र_Level) {
                eor=GET_LR_FROM_LEVEL(अग्र_Level);
            } अन्यथा {
                eor=GET_LR_FROM_LEVEL(level);
            }

            कुरु {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                start=सीमा;
                level=अग्र_Level;
                यदि((start>0) && (dirProps[start-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, start));
                } अन्यथा {
                    sor=eor;
                }

                /* search for the limit of this run */
                यावत्((++सीमा<length) &&
                      ((स्तराः[सीमा]==level) ||
                       (DIRPROP_FLAG(dirProps[सीमा])&आवरणम्_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                यदि(सीमा<length) {
                    अग्र_Level=स्तराः[सीमा];
                } अन्यथा {
                    अग्र_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                यदि(NO_OVERRIDE(level)<NO_OVERRIDE(अग्र_Level)) {
                    eor=GET_LR_FROM_LEVEL(अग्र_Level);
                } अन्यथा {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                यदि(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_स्तराः(pBiDi, start, सीमा, sor, eor);
                } अन्यथा {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    कुरु {
                        स्तराः[start++]&=~UBIDI_LEVEL_OVERRIDE;
                    } यावत्(start<सीमा);
                }
            } यावत्(सीमा<length);
        }
        /* check if we got any memory shortage while adding insert points */
        यदि (U_FAILURE(pBiDi->निवेशय_Points.error_संहिता))
        {
            *p_Error_संहिता=pBiDi->निवेशय_Points.error_संहिता;
            प्रत्यागम;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        भङ्ग;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    यदि((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_निवेशय_MARKS) &&
       ((pBiDi->reordering_बहुलकम्==UBIDI_REORDER_INVERSE_LIKE_प्रत्यक्षम्) ||
        (pBiDi->reordering_बहुलकम्==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, start, चरमः;
        UBiDiLevel level;
        DirProp dirProp;
        प्रति(i=0; i<pBiDi->para_गणनम्; i++) {
            चरमः=(pBiDi->paras[i].सीमा)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            यदि(level==0)
                अनुवर्त;           /* LTR paragraph */
            start= i==0 ? 0 : pBiDi->paras[i-1].सीमा;
            प्रति(j=चरमः; j>=start; j--) {
                dirProp=dirProps[j];
                यदि(dirProp==L) {
                    यदि(j<चरमः) {
                        यावत्(dirProps[चरमः]==B) {
                            चरमः--;
                        }
                    }
                    सङ्कलय_Point(pBiDi, चरमः, RLM_BEFORE);
                    भङ्ग;
                }
                यदि(DIRPROP_FLAG(dirProp) & आवरणम्_R_AL) {
                    भङ्ग;
                }
            }
        }
    }

    यदि(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->परिणाम_दीर्घता -= pBiDi->control_गणनम्;
    } अन्यथा {
        pBiDi->परिणाम_दीर्घता += pBiDi->निवेशय_Points.size;
    }
    समुच्चयः_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI शून्य U_EXPORT2
ubidi_क्रमः_Paragraphs_LTR(UBiDi *pBiDi, UBool क्रमः_Paragraphs_LTR) {
    यदि(pBiDi!=शून्यसूचकः) {
        pBiDi->क्रमः_Paragraphs_LTR=क्रमः_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_क्रमः_Paragraphs_LTR(UBiDi *pBiDi) {
    यदि(pBiDi!=शून्यसूचकः) {
        प्रत्यागम pBiDi->क्रमः_Paragraphs_LTR;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(ध्रुव UBiDi *pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->direction;
    } अन्यथा {
        प्रत्यागम UBIDI_LTR;
    }
}

U_CAPI ध्रुव char16_t* U_EXPORT2
ubidi_getText(ध्रुव UBiDi* pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->text;
    } अन्यथा {
        प्रत्यागम शून्यसूचकः;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_दीर्घता(ध्रुव UBiDi *pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->original_दीर्घता;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_दीर्घता(ध्रुव UBiDi *pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->length;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_परिणाम_दीर्घता(ध्रुव UBiDi *pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->परिणाम_दीर्घता;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(ध्रुव UBiDi *pBiDi) {
    यदि(IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम pBiDi->paraLevel;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_गणनम्_Paragraphs(UBiDi *pBiDi) {
    यदि(!IS_VALID_PARA_OR_पङ्क्तिः(pBiDi)) {
        प्रत्यागम 0;
    } अन्यथा {
        प्रत्यागम pBiDi->para_गणनम्;
    }
}

U_CAPI शून्य U_EXPORT2
ubidi_get_Paragraph_By_सूचकांक(ध्रुव UBiDi *pBiDi, int32_t para_सूचकांक,
                          int32_t *pParaStart, int32_t *p_Para_सीमा,
                          UBiDiLevel *pParaLevel, UError_संहिता *p_Error_संहिता) {
    int32_t paraStart;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता);
    RETURN_VOID_IF_BAD_RANGE(para_सूचकांक, 0, pBiDi->para_गणनम्, *p_Error_संहिता);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    यदि(para_सूचकांक) {
        paraStart=pBiDi->paras[para_सूचकांक-1].सीमा;
    } अन्यथा {
        paraStart=0;
    }
    यदि(pParaStart!=शून्यसूचकः) {
        *pParaStart=paraStart;
    }
    यदि(p_Para_सीमा!=शून्यसूचकः) {
        *p_Para_सीमा=pBiDi->paras[para_सूचकांक].सीमा;
    }
    यदि(pParaLevel!=शून्यसूचकः) {
        *pParaLevel=GET_PARALEVEL(pBiDi, paraStart);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(ध्रुव UBiDi *pBiDi, int32_t char_सूचकांक,
                          int32_t *pParaStart, int32_t *p_Para_सीमा,
                          UBiDiLevel *pParaLevel, UError_संहिता *p_Error_संहिता) {
    int32_t para_सूचकांक;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पङ्क्तिः(pBiDi, *p_Error_संहिता, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_सूचकांक, 0, pBiDi->length, *p_Error_संहिता, -1);

    प्रति(para_सूचकांक=0; char_सूचकांक>=pBiDi->paras[para_सूचकांक].सीमा; para_सूचकांक++);
    ubidi_get_Paragraph_By_सूचकांक(pBiDi, para_सूचकांक, pParaStart, p_Para_सीमा, pParaLevel, p_Error_संहिता);
    प्रत्यागम para_सूचकांक;
}

U_CAPI शून्य U_EXPORT2
ubidi_समुच्चयः_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       ध्रुव शून्य *newContext, UBiDiClassCallback **oldFn,
                       ध्रुव शून्य **oldContext, UError_संहिता *p_Error_संहिता)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_संहिता);
    यदि(pBiDi==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }
    यदि( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    यदि( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI शून्य U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, ध्रुव शून्य **context)
{
    यदि(pBiDi==शून्यसूचकः) {
        प्रत्यागम;
    }
    यदि( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    यदि( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    यदि( pBiDi->fnClassCallback == शून्यसूचकः ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    यदि(dir >= U_CHAR_DIRECTION_गणनम्) {
        dir = (UCharDirection)ON;
    }
    प्रत्यागम dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#यदि U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(ध्रुव UnicodeString& s) {
    यदि (s == u"ltr") {
        प्रत्यागम U_MF_BIDI_OPTION_LTR;
    }
    यदि (s == u"rtl") {
        प्रत्यागम U_MF_BIDI_OPTION_RTL;
    }
    यदि (s == u"auto") {
        प्रत्यागम U_MF_BIDI_OPTION_AUTO;
    }
    प्रत्यागम U_MF_BIDI_OPTION_INHERIT; // inherit is default
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
    यदि (bidiIsolationStrategy == U_MF_BIDI_OFF)
        प्रत्यागम &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    तार्किक isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    विकल्प (dir) {
        पक्ष U_MF_DIRECTIONALITY_LTR:
            यदि (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                प्रत्यागम &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            यदि (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            भङ्ग; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        पक्ष U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            यदि (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            भङ्ग; // End of 2iv.
        // 2v. Else:
        स्वभाव:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            यदि (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            भङ्ग; // End of 2v
    } // `fmt` now contains the isolated string
    प्रत्यागम &fmt;
}

U_NAMESPACE_अन्तः

#यदि_अन्तः // #if SHOW_CPLUSPLUS_API

