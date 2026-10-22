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

#समावेश <limits>

#समावेश "cmemory.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/ubidi.h"
#समावेश "unicode/utf16.h"
#समावेश "ubidi_props.h"
#समावेश "ubidiimp.h"
#समावेश "uassert.h"

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
अटल स्थिर Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
अटल स्थिर Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
अटल स्थिर Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#घोषणा DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#घोषणा DIRPROP_FLAG_E(level)  flagE[(level)&1]
#घोषणा DIRPROP_FLAG_O(level)  flagO[(level)&1]

#घोषणा DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#घोषणा NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UErrorCode errorCode=U_ZERO_ERROR;
    वापस ubidi_openSized(0, 0, &errorCode);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_लंबाई_2, int32_t max_Run_गिनती, UErrorCode *pErrorCode) {
    UBiDi *pBiDi;

    /* check the argument values */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    } अन्यथा अगर(max_लंबाई_2<0 || max_Run_गिनती<0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(माप(UBiDi));
    अगर(pBiDi==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, माप(UBiDi));

    /* allocate memory for arrays as requested */
    अगर(max_लंबाई_2>0) {
        अगर( !getInitialDirPropsMemory(pBiDi, max_लंबाई_2) ||
            !getInitialLevelsMemory(pBiDi, max_लंबाई_2)
        ) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        }
    } अन्यथा {
        pBiDi->mayAllocateText=true;
    }

    अगर(max_Run_गिनती>0) {
        अगर(max_Run_गिनती==1) {
            /* use simpleRuns[] */
            pBiDi->runs_आकार=माप(Run);
        } अन्यथा अगर(!getInitialRunsMemory(pBiDi, max_Run_गिनती)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        }
    } अन्यथा {
        pBiDi->mayAllocateRuns=true;
    }

    अगर(U_SUCCESS(*pErrorCode)) {
        वापस pBiDi;
    } अन्यथा {
        ubidi_close(pBiDi);
        वापस nullptr;
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
ubidi_getMemory(BidiMemoryForAllocation *bidiMem, int32_t *p_आकार, UBool mayAllocate, माप_प्रकार आकार_Needed) {
    अगर (आकार_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      वापस false;
    }
    व्योम **pMemory = (व्योम **)bidiMem;
    /* check for existing memory */
    अगर(*pMemory==nullptr) {
        /* we need to allocate memory */
        अगर(mayAllocate && (*pMemory=uprv_malloc(आकार_Needed))!=nullptr) {
            *p_आकार=static_cast<int32_t>(आकार_Needed);
            वापस true;
        } अन्यथा {
            वापस false;
        }
    } अन्यथा {
        अगर(आकार_Needed<=static_cast<माप_प्रकार>(*p_आकार)) {
            /* there is already enough memory */
            वापस true;
        }
        अन्यथा अगर(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            वापस false;
        } अन्यथा {
            /* we try to grow */
            व्योम *memory;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            अगर((memory=uprv_realloc(*pMemory, आकार_Needed))!=nullptr) {
                *pMemory=memory;
                *p_आकार=static_cast<int32_t>(आकार_Needed);
                वापस true;
            } अन्यथा {
                /* we failed to grow */
                वापस false;
            }
        }
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    अगर(pBiDi!=nullptr) {
        pBiDi->pParaBiDi=nullptr;          /* in case one tries to reuse this block */
        अगर(pBiDi->dirPropsMemory!=nullptr) {
            uprv_free(pBiDi->dirPropsMemory);
        }
        अगर(pBiDi->levelsMemory!=nullptr) {
            uprv_free(pBiDi->levelsMemory);
        }
        अगर(pBiDi->openingsMemory!=nullptr) {
            uprv_free(pBiDi->openingsMemory);
        }
        अगर(pBiDi->parasMemory!=nullptr) {
            uprv_free(pBiDi->parasMemory);
        }
        अगर(pBiDi->runsMemory!=nullptr) {
            uprv_free(pBiDi->runsMemory);
        }
        अगर(pBiDi->isolatesMemory!=nullptr) {
            uprv_free(pBiDi->isolatesMemory);
        }
        अगर(pBiDi->जोड़ो_Points.points!=nullptr) {
            uprv_free(pBiDi->जोड़ो_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI व्योम U_EXPORT2
ubidi_setInverse(UBiDi *pBiDi, UBool isInverse) {
    अगर(pBiDi!=nullptr) {
        pBiDi->isInverse=isInverse;
        pBiDi->reorderingMode = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    अगर(pBiDi!=nullptr) {
        वापस pBiDi->isInverse;
    } अन्यथा {
        वापस false;
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
U_CAPI व्योम U_EXPORT2
ubidi_setReorderingMode(UBiDi *pBiDi, UBiDiReorderingMode reorderingMode) UPRV_NO_SANITIZE_UNDEFINED {
    अगर ((pBiDi!=nullptr) && (reorderingMode >= UBIDI_REORDER_DEFAULT)
                        && (reorderingMode < UBIDI_REORDER_गिनती)) {
        pBiDi->reorderingMode = reorderingMode;
        pBiDi->isInverse = reorderingMode == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBiDiReorderingMode U_EXPORT2
ubidi_getReorderingMode(UBiDi *pBiDi) {
    अगर (pBiDi!=nullptr) {
        वापस pBiDi->reorderingMode;
    } अन्यथा {
        वापस UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_setReorderingOptions(UBiDi *pBiDi, uint32_t reorderingOptions) {
    अगर (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_जोड़ो_MARKS;
    }
    अगर (pBiDi!=nullptr) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    अगर (pBiDi!=nullptr) {
        वापस pBiDi->reorderingOptions;
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getBaseDirection(स्थिर char16_t *text,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    अगर( text==nullptr || length<-1 ){
        वापस UBIDI_NEUTRAL;
    }

    अगर(length==-1) {
        length=u_strlen(text);
    }

    क्रम( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dir = u_charDirection(uchar);
        अगर( dir == U_बायाँ_TO_दायाँ )
                वापस UBIDI_LTR;
        अगर( dir == U_दायाँ_TO_बायाँ || dir ==U_दायाँ_TO_बायाँ_ARABIC )
                वापस UBIDI_RTL;
    }
    वापस UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
अटल DirProp
firstL_R_AL(UBiDi *pBiDi) {
    स्थिर char16_t *text=pBiDi->prologue;
    int32_t length=pBiDi->pro_लंबाई;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, परिणाम=ON;
    क्रम(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        अगर(परिणाम==ON) {
            अगर(dirProp==L || dirProp==R || dirProp==AL) {
                परिणाम=dirProp;
            }
        } अन्यथा {
            अगर(dirProp==B) {
                परिणाम=ON;
            }
        }
    }
    वापस परिणाम;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
अटल UBool
check_Para_गिनती(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_गिनती;
    अगर(pBiDi->paras==pBiDi->simpleParas) {
        अगर(count<=SIMPLE_PARAS_गिनती)
            वापस true;
        अगर(!getInitialParasMemory(pBiDi, SIMPLE_PARAS_गिनती * 2))
            वापस false;
        pBiDi->paras=pBiDi->parasMemory;
        uprv_memcpy(pBiDi->parasMemory, pBiDi->simpleParas, SIMPLE_PARAS_गिनती * माप(Para));
        वापस true;
    }
    अगर(!getInitialParasMemory(pBiDi, count * 2))
        वापस false;
    pBiDi->paras=pBiDi->parasMemory;
    वापस true;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
अटल UBool
getDirProps(UBiDi *pBiDi) {
    स्थिर char16_t *text=pBiDi->text;
    DirProp *dirProps=pBiDi->dirPropsMemory;    /* pBiDi->dirProps is const */

    int32_t i=0, original_लंबाई=pBiDi->original_लंबाई;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reorderingMode == UBIDI_REORDER_INVERSE_LIKE_DIRECT ||
            pBiDi->reorderingMode == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t lastArabicPos=-1;
    int32_t control_गिनती=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    क्रमागत State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp lastStrong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_Start_ढेर[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  previous_State_ढेर[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t ढेर_Last=-1;

    अगर(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    अगर(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        lastStrong=defaultParaLevel;
        अगर(pBiDi->pro_लंबाई>0 &&                    /* there is a prologue */
           (dirProp=firstL_R_AL(pBiDi))!=ON) {  /* with a strong character */
            अगर(dirProp==L)
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
    क्रम( /* i=0 above */ ; i<original_लंबाई; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, original_लंबाई, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        अगर(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        अगर(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_गिनती++;
        अगर(dirProp==L) {
            अगर(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_गिनती-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            अन्यथा अगर(state==SEEKING_STRONG_FOR_FSI) {
                अगर(ढेर_Last<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            lastStrong=L;
            जारी;
        }
        अगर(dirProp==R || dirProp==AL) {
            अगर(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_गिनती-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            अन्यथा अगर(state==SEEKING_STRONG_FOR_FSI) {
                अगर(ढेर_Last<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_Start_ढेर[ढेर_Last]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            lastStrong=R;
            अगर(dirProp==AL)
                lastArabicPos=i-1;
            जारी;
        }
        अगर(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            ढेर_Last++;
            अगर(ढेर_Last<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_Start_ढेर[ढेर_Last]=i-1;
                previous_State_ढेर[ढेर_Last]=state;
            }
            अगर(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            अन्यथा
                state=LOOKING_FOR_PDI;
            जारी;
        }
        अगर(dirProp==PDI) {
            अगर(state==SEEKING_STRONG_FOR_FSI) {
                अगर(ढेर_Last<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            अगर(ढेर_Last>=0) {
                अगर(ढेर_Last<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=previous_State_ढेर[ढेर_Last];
                ढेर_Last--;
            }
            जारी;
        }
        अगर(dirProp==B) {
            अगर(i<original_लंबाई && uchar==CR && text[i]==LF) /* do nothing on the CR */
                जारी;
            pBiDi->paras[pBiDi->para_गिनती-1].limit=i;
            अगर(isDefaultLevelInverse && lastStrong==R)
                pBiDi->paras[pBiDi->para_गिनती-1].level=1;
            अगर(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_गिनती=control_गिनती;
            }
            अगर(i<original_लंबाई) {              /* B not last char in text */
                pBiDi->para_गिनती++;
                अगर(check_Para_गिनती(pBiDi)==false)    /* not enough memory for a new para entry */
                    वापस false;
                अगर(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_गिनती-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    lastStrong=defaultParaLevel;
                } अन्यथा {
                    pBiDi->paras[pBiDi->para_गिनती-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                ढेर_Last=-1;
            }
            जारी;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    अगर(ढेर_Last>UBIDI_MAX_EXPLICIT_LEVEL) {
        ढेर_Last=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    जबतक(ढेर_Last>=0) {
        अगर(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            अवरोध;
        }
        state=previous_State_ढेर[ढेर_Last];
        ढेर_Last--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    अगर(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        अगर(pBiDi->length<original_लंबाई)
            pBiDi->para_गिनती--;
    } अन्यथा {
        pBiDi->paras[pBiDi->para_गिनती-1].limit=original_लंबाई;
        pBiDi->control_गिनती=control_गिनती;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    अगर(isDefaultLevelInverse && lastStrong==R) {
        pBiDi->paras[pBiDi->para_गिनती-1].level=1;
    }
    अगर(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    क्रम(i=0; i<pBiDi->para_गिनती; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    अगर(pBiDi->orderParagraphsLTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->lastArabicPos=lastArabicPos;
    वापस true;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_सूचकांक(स्थिर UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    क्रम(i=0; i<pBiDi->para_गिनती; i++)
        अगर(pindex<pBiDi->paras[i].limit)
            अवरोध;
    अगर(i>=pBiDi->para_गिनती)
        i=pBiDi->para_गिनती-1;
    वापस (UBiDiLevel)(pBiDi->paras[i].level);
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

अटल व्योम
bracketInit(UBiDi *pBiDi, Bracket_तथ्य *bd) {
    bd->pBiDi=pBiDi;
    bd->isoRunLast=0;
    bd->isoRuns[0].start=0;
    bd->isoRuns[0].limit=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].lastStrong = bd->isoRuns[0].lastBase = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    अगर(pBiDi->openingsMemory) {
        bd->openings=pBiDi->openingsMemory;
        bd->openings_गिनती=pBiDi->openings_आकार / माप(Opening);
    } अन्यथा {
        bd->openings=bd->simpleOpenings;
        bd->openings_गिनती=SIMPLE_OPENINGS_गिनती;
    }
    bd->isNumbersSpecial=bd->pBiDi->reorderingMode==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reorderingMode==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
अटल व्योम
bracketProcessB(Bracket_तथ्य *bd, UBiDiLevel level) {
    bd->isoRunLast=0;
    bd->isoRuns[0].limit=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].lastStrong=bd->isoRuns[0].lastBase=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
अटल व्योम
bracketProcessBoundary(Bracket_तथ्य *bd, int32_t lastCcPos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    DirProp *dirProps=bd->pBiDi->dirProps;
    अगर(DIRPROP_FLAG(dirProps[lastCcPos])&MASK_ISO)  /* after an isolate */
        वापस;
    अगर(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    pLastIsoRun->limit=pLastIsoRun->start;
    pLastIsoRun->level=embeddingLevel;
    pLastIsoRun->lastStrong=pLastIsoRun->lastBase=contextLevel&1;
    pLastIsoRun->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    pLastIsoRun->contextPos = static_cast<UBiDiDirection>(lastCcPos);
}

/* LRI or RLI */
अटल व्योम
bracketProcessLRI_RLI(Bracket_तथ्य *bd, UBiDiLevel level) {
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    int16_t lastLimit;
    pLastIsoRun->lastBase=ON;
    lastLimit=pLastIsoRun->limit;
    bd->isoRunLast++;
    pLastIsoRun++;
    pLastIsoRun->start=pLastIsoRun->limit=lastLimit;
    pLastIsoRun->level=level;
    pLastIsoRun->lastStrong=pLastIsoRun->lastBase=level&1;
    pLastIsoRun->contextDir = static_cast<UBiDiDirection>(level & 1);
    pLastIsoRun->contextPos=0;
}

/* PDI */
अटल व्योम
bracketProcessPDI(Bracket_तथ्य *bd) {
    IsoRun *pLastIsoRun;
    bd->isoRunLast--;
    pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    pLastIsoRun->lastBase=ON;
}

/* newly found opening bracket: create an openings entry */
अटल UBool                            /* return true if success */
bracketAddOpening(Bracket_तथ्य *bd, char16_t match, int32_t position) {
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    Opening *pOpening;
    अगर(pLastIsoRun->limit>=bd->openings_गिनती) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        अगर(!getInitialOpeningsMemory(pBiDi, pLastIsoRun->limit * 2))
            वापस false;
        अगर(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openingsMemory, bd->simpleOpenings,
                        SIMPLE_OPENINGS_गिनती * माप(Opening));
        bd->openings=pBiDi->openingsMemory;     /* may have changed */
        bd->openings_गिनती=pBiDi->openings_आकार / माप(Opening);
    }
    pOpening=&bd->openings[pLastIsoRun->limit];
    pOpening->position=position;
    pOpening->match=match;
    pOpening->contextDir=pLastIsoRun->contextDir;
    pOpening->contextPos=pLastIsoRun->contextPos;
    pOpening->flags=0;
    pLastIsoRun->limit++;
    वापस true;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
अटल व्योम
fixN0c(Bracket_तथ्य *bd, int32_t opening_सूचकांक, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    क्रम(k=opening_सूचकांक+1, qOpening=&bd->openings[k]; k<pLastIsoRun->limit; k++, qOpening++) {
        अगर(qOpening->match>=0)      /* not an N0c match */
            जारी;
        अगर(newPropPosition<qOpening->contextPos)
            अवरोध;
        अगर(newPropPosition>=qOpening->position)
            जारी;
        अगर(newProp==qOpening->contextDir)
            अवरोध;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->match);
        dirProps[closingPosition]=newProp;
        qOpening->match=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
अटल DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_तथ्य *bd, int32_t openIdx, int32_t position) {
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(pLastIsoRun->level & 1);
    stable=true;            /* assume stable until proved otherwise */

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

    अगर((direction==0 && pOpening->flags&FOUND_L) ||
       (direction==1 && pOpening->flags&FOUND_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    अन्यथा अगर(pOpening->flags&(FOUND_L|FOUND_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==pLastIsoRun->start);
        अगर(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        अन्यथा
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } अन्यथा {
        /* forget this and any brackets nested within this pair */
        pLastIsoRun->limit= static_cast<uint16_t>(openIdx);
        वापस ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    अगर(stable) {
        pLastIsoRun->limit= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        जबतक(pLastIsoRun->limit>pLastIsoRun->start &&
              bd->openings[pLastIsoRun->limit-1].position==pOpening->position)
            pLastIsoRun->limit--;
    } अन्यथा {
        int32_t k;
        pOpening->match=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        जबतक(k>=pLastIsoRun->start &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].match=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        क्रम(k=openIdx+1; k<pLastIsoRun->limit; k++) {
            qOpening=&bd->openings[k];
            अगर(qOpening->position>=position)
                अवरोध;
            अगर(qOpening->match>0)
                qOpening->match=0;
        }
    }
    वापस newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
अटल UBool                            /* return true if success */
bracketProcessChar(Bracket_तथ्य *bd, int32_t position) {
    IsoRun *pLastIsoRun=&bd->isoRuns[bd->isoRunLast];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    अगर(dirProp==ON) {
        char16_t c, match;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->text[position];
        क्रम(idx=pLastIsoRun->limit-1; idx>=pLastIsoRun->start; idx--) {
            अगर(bd->openings[idx].match!=c)
                जारी;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            अगर(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                अवरोध;
            }
            pLastIsoRun->lastBase=ON;
            pLastIsoRun->contextDir = static_cast<UBiDiDirection>(newProp);
            pLastIsoRun->contextPos=position;
            level=bd->pBiDi->levels[position];
            अगर(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                pLastIsoRun->lastStrong=newProp;
                flag=DIRPROP_FLAG(newProp);
                क्रम(i=pLastIsoRun->start; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->levels[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->levels[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            वापस true;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        अगर(c)
            match= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        अन्यथा
            match=0;
        अगर(match!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            अगर(match==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                अगर(!bracketAddOpening(bd, 0x3009, position))
                    वापस false;
            }
            अन्यथा अगर(match==0x3009) {         /* RIGHT ANGLE BRACKET */
                अगर(!bracketAddOpening(bd, 0x232A, position))
                    वापस false;
            }
            अगर(!bracketAddOpening(bd, match, position))
                वापस false;
        }
    }
    level=bd->pBiDi->levels[position];
    अगर(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        अगर(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        pLastIsoRun->lastBase=newProp;
        pLastIsoRun->lastStrong=newProp;
        pLastIsoRun->contextDir = static_cast<UBiDiDirection>(newProp);
        pLastIsoRun->contextPos=position;
    }
    अन्यथा अगर(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        pLastIsoRun->lastBase=dirProp;
        pLastIsoRun->lastStrong=dirProp;
        pLastIsoRun->contextDir = static_cast<UBiDiDirection>(newProp);
        pLastIsoRun->contextPos=position;
    }
    अन्यथा अगर(dirProp==EN) {
        pLastIsoRun->lastBase=EN;
        अगर(pLastIsoRun->lastStrong==L) {
            newProp=L;                  /* W7 */
            अगर(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            pLastIsoRun->contextDir = static_cast<UBiDiDirection>(L);
            pLastIsoRun->contextPos=position;
        }
        अन्यथा {
            newProp=R;                  /* N0 */
            अगर(pLastIsoRun->lastStrong==AL)
                dirProps[position]=AN;  /* W2 */
            अन्यथा
                dirProps[position]=ENR;
            pLastIsoRun->contextDir = static_cast<UBiDiDirection>(R);
            pLastIsoRun->contextPos=position;
        }
    }
    अन्यथा अगर(dirProp==AN) {
        newProp=R;                      /* N0 */
        pLastIsoRun->lastBase=AN;
        pLastIsoRun->contextDir = static_cast<UBiDiDirection>(R);
        pLastIsoRun->contextPos=position;
    }
    अन्यथा अगर(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=pLastIsoRun->lastBase;
        अगर(newProp==ON)
            dirProps[position]=newProp;
    }
    अन्यथा {
        newProp=dirProp;
        pLastIsoRun->lastBase=dirProp;
    }
    अगर(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        क्रम(i=pLastIsoRun->start; i<pLastIsoRun->limit; i++)
            अगर(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    वापस true;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
अटल UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    अगर(!(flags&MASK_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&MASK_POSSIBLE_N)))) {
        वापस UBIDI_LTR;
    } अन्यथा अगर(!(flags&MASK_LTR)) {
        वापस UBIDI_RTL;
    } अन्यथा {
        वापस UBIDI_MIXED;
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
अटल UBiDiDirection
resolveExplicitLevels(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    स्थिर char16_t *text=pBiDi->text;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_गिनती=0;

    अगर(U_FAILURE(*pErrorCode)) { वापस UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    अगर((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        वापस direction;
    }
    अगर(pBiDi->reorderingMode > UBIDI_REORDER_LAST_LOGICAL_TO_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_सूचकांक, start, limit;
        क्रम(para_सूचकांक=0; para_सूचकांक<pBiDi->para_गिनती; para_सूचकांक++) {
            अगर(para_सूचकांक==0)
                start=0;
            अन्यथा
                start=pBiDi->paras[para_सूचकांक-1].limit;
            limit=pBiDi->paras[para_सूचकांक].limit;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_सूचकांक].level);
            क्रम(i=start; i<limit; i++)
                levels[i]=level;
        }
        वापस direction;   /* no bracket matching for inverse BiDi */
    }
    अगर(!(flags&(MASK_EXPLICIT|MASK_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_सूचकांक, start, limit;
        Bracket_तथ्य bracket_तथ्य;
        bracketInit(pBiDi, &bracket_तथ्य);
        क्रम(para_सूचकांक=0; para_सूचकांक<pBiDi->para_गिनती; para_सूचकांक++) {
            अगर(para_सूचकांक==0)
                start=0;
            अन्यथा
                start=pBiDi->paras[para_सूचकांक-1].limit;
            limit=pBiDi->paras[para_सूचकांक].limit;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_सूचकांक].level);
            क्रम(i=start; i<limit; i++) {
                levels[i]=level;
                dirProp=dirProps[i];
                अगर(dirProp==BN)
                    जारी;
                अगर(dirProp==B) {
                    अगर((i+1)<length) {
                        अगर(text[i]==CR && text[i+1]==LF)
                            जारी;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_तथ्य, level);
                    }
                    जारी;
                }
                अगर(!bracketProcessChar(&bracket_तथ्य, i)) {
                    *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                    वापस UBIDI_LTR;
                }
            }
        }
        वापस direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel previousLevel=level;     /* previous level for regular (not CC) characters */
        int32_t lastCcPos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t ढेर_2[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t ढेर_Last=0;
        int32_t overflow_Isolate_गिनती=0;
        int32_t overflow_Embedding_गिनती=0;
        int32_t valid_Isolate_गिनती=0;
        Bracket_तथ्य bracket_तथ्य;
        bracketInit(pBiDi, &bracket_तथ्य);
        ढेर_2[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        क्रम(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            चयन(dirProp) {
            हाल LRE:
            हाल RLE:
            हाल LRO:
            हाल RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                levels[i]=previousLevel;
                अगर (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                अन्यथा
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                अगर(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && overflow_Isolate_गिनती==0 &&
                                                         overflow_Embedding_गिनती==0) {
                    lastCcPos=i;
                    embeddingLevel=newLevel;
                    अगर(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    ढेर_Last++;
                    ढेर_2[ढेर_Last]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } अन्यथा {
                    अगर(overflow_Isolate_गिनती==0)
                        overflow_Embedding_गिनती++;
                }
                अवरोध;
            हाल PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                levels[i]=previousLevel;
                /* handle all the overflow cases first */
                अगर(overflow_Isolate_गिनती) {
                    अवरोध;
                }
                अगर(overflow_Embedding_गिनती) {
                    overflow_Embedding_गिनती--;
                    अवरोध;
                }
                अगर(ढेर_Last>0 && ढेर_2[ढेर_Last]<ISOLATE) {   /* not an isolate entry */
                    lastCcPos=i;
                    ढेर_Last--;
                    embeddingLevel = static_cast<UBiDiLevel>(ढेर_2[ढेर_Last]);
                }
                अवरोध;
            हाल LRI:
            हाल RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                levels[i]=NO_OVERRIDE(embeddingLevel);
                अगर(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_तथ्य, lastCcPos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                previousLevel=embeddingLevel;
                /* (X5a, X5b) */
                अगर(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                अन्यथा
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                अगर(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && overflow_Isolate_गिनती==0 &&
                                                         overflow_Embedding_गिनती==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    lastCcPos=i;
                    valid_Isolate_गिनती++;
                    अगर(valid_Isolate_गिनती>pBiDi->isolate_गिनती)
                        pBiDi->isolate_गिनती=valid_Isolate_गिनती;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    ढेर_Last++;
                    ढेर_2[ढेर_Last]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_तथ्य, embeddingLevel);
                } अन्यथा {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    overflow_Isolate_गिनती++;
                }
                अवरोध;
            हाल PDI:
                अगर(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_तथ्य, lastCcPos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                अगर(overflow_Isolate_गिनती) {
                    overflow_Isolate_गिनती--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                अन्यथा अगर(valid_Isolate_गिनती) {
                    flags|=DIRPROP_FLAG(PDI);
                    lastCcPos=i;
                    overflow_Embedding_गिनती=0;
                    जबतक(ढेर_2[ढेर_Last]<ISOLATE) /* pop embedding entries */
                        ढेर_Last--;                /* until the last isolate entry */
                    ढेर_Last--;                    /* pop also the last isolate entry */
                    valid_Isolate_गिनती--;
                    bracketProcessPDI(&bracket_तथ्य);
                } अन्यथा
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(ढेर_2[ढेर_Last]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                previousLevel=embeddingLevel;
                levels[i]=NO_OVERRIDE(embeddingLevel);
                अवरोध;
            हाल B:
                flags|=DIRPROP_FLAG(B);
                levels[i]=GET_PARALEVEL(pBiDi, i);
                अगर((i+1)<length) {
                    अगर(text[i]==CR && text[i+1]==LF)
                        अवरोध;          /* skip CR when followed by LF */
                    overflow_Embedding_गिनती=overflow_Isolate_गिनती=0;
                    valid_Isolate_गिनती=0;
                    ढेर_Last=0;
                    previousLevel=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    ढेर_2[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_तथ्य, embeddingLevel);
                }
                अवरोध;
            हाल BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                levels[i]=previousLevel;
                flags|=DIRPROP_FLAG(BN);
                अवरोध;
            शेष:
                /* all other types are normal characters and get the "real" level */
                अगर(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(previousLevel)) {
                    bracketProcessBoundary(&bracket_तथ्य, lastCcPos,
                                           previousLevel, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    अगर(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    अन्यथा
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                previousLevel=embeddingLevel;
                levels[i]=embeddingLevel;
                अगर(!bracketProcessChar(&bracket_तथ्य, i))
                    वापस static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                अवरोध;
            }
        }
        अगर(flags&MASK_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        अगर(pBiDi->orderParagraphsLTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    वापस direction;
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
अटल UBiDiDirection
checkExplicitLevels(UBiDi *pBiDi, UErrorCode *pErrorCode) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t isolate_गिनती=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_गिनती=0;

    int32_t current_Para_सूचकांक = 0;
    int32_t currentParaLimit = pBiDi->paras[0].limit;
    int32_t currentParaLevel = pBiDi->paraLevel;

    क्रम(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=levels[i];
        DirProp dirProp=dirProps[i];
        अगर(dirProp==LRI || dirProp==RLI) {
            isolate_गिनती++;
            अगर(isolate_गिनती>pBiDi->isolate_गिनती)
                pBiDi->isolate_गिनती=isolate_गिनती;
        }
        अन्यथा अगर(dirProp==PDI)
            isolate_गिनती--;
        अन्यथा अगर(dirProp==B)
            isolate_गिनती=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        अगर (pBiDi->defaultParaLevel != 0 &&
                i == currentParaLimit && (current_Para_सूचकांक + 1) < pBiDi->para_गिनती) {
            currentParaLevel = pBiDi->paras[++current_Para_सूचकांक].level;
            currentParaLimit = pBiDi->paras[current_Para_सूचकांक].limit;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        अगर (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            अगर (level == 0) {
                अगर (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } अन्यथा {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    levels[i] = level | overrideFlag;
                }
            } अन्यथा {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
                वापस UBIDI_LTR;
            }
        }
        अगर (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } अन्यथा {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    अगर(flags&MASK_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    वापस directionFromFlags(pBiDi);
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
#घोषणा IMPTABPROPS_COLUMNS 16
#घोषणा IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#घोषणा GET_STATEPROPS(cell) ((cell)&0x1f)
#घोषणा GET_ACTIONPROPS(cell) ((cell)>>5)
#घोषणा s(action, newState) ((uint8_t)(newState+(action<<5)))

अटल स्थिर uint8_t groupProp[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
क्रमागत { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

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
अटल स्थिर uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
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
#अघोषित s

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
#घोषणा IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#घोषणा IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#घोषणा GET_STATE(cell) ((cell)&0x0f)
#घोषणा GET_ACTION(cell) ((cell)>>4)
#घोषणा s(action, newState) ((uint8_t)(newState+(action<<4)))

प्रकार uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
प्रकार uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
प्रकार काष्ठा ImpTabPair {
    स्थिर व्योम * pImpTab[2];
    स्थिर व्योम * pImpAct[2];
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

अटल स्थिर ImpTab impTabL_DEFAULT =   /* Even paragraph level */
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
अटल स्थिर ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
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
अटल स्थिर ImpAct impAct0 = {0,1,2,3,4};
अटल स्थिर ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

अटल स्थिर ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
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
अटल स्थिर ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

अटल स्थिर ImpTab impTabL_GROUP_NUMBERS_WITH_R =
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
अटल स्थिर ImpTab impTabR_GROUP_NUMBERS_WITH_R =
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
अटल स्थिर ImpTabPair impTab_GROUP_NUMBERS_WITH_R = {
                        {&impTabL_GROUP_NUMBERS_WITH_R,
                         &impTabR_GROUP_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


अटल स्थिर ImpTab impTabL_INVERSE_NUMBERS_AS_L =
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
अटल स्थिर ImpTab impTabR_INVERSE_NUMBERS_AS_L =
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
अटल स्थिर ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

अटल स्थिर ImpTab impTabR_INVERSE_LIKE_DIRECT =   /* Odd  paragraph level */
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
अटल स्थिर ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
अटल स्थिर ImpTabPair impTab_INVERSE_LIKE_DIRECT = {
                        {&impTabL_DEFAULT,
                         &impTabR_INVERSE_LIKE_DIRECT},
                        {&impAct0, &impAct1}};

अटल स्थिर ImpTab impTabL_INVERSE_LIKE_DIRECT_WITH_MARKS =
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
अटल स्थिर ImpTab impTabR_INVERSE_LIKE_DIRECT_WITH_MARKS =
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
अटल स्थिर ImpAct impAct2 = {0,1,2,5,6,7,8};
अटल स्थिर ImpAct impAct3 = {0,1,9,10,11,12};
अटल स्थिर ImpTabPair impTab_INVERSE_LIKE_DIRECT_WITH_MARKS = {
                        {&impTabL_INVERSE_LIKE_DIRECT_WITH_MARKS,
                         &impTabR_INVERSE_LIKE_DIRECT_WITH_MARKS},
                        {&impAct2, &impAct3}};

अटल स्थिर ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &impTabR_INVERSE_LIKE_DIRECT},
                        {&impAct0, &impAct1}};

अटल स्थिर ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
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
अटल स्थिर ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &impTabR_INVERSE_LIKE_DIRECT_WITH_MARKS},
                        {&impAct2, &impAct3}};

#अघोषित s

प्रकार काष्ठा {
    स्थिर ImpTab * pImpTab;             /* level table pointer          */
    स्थिर ImpAct * pImpAct;             /* action map array             */
    int32_t startON;                    /* start of ON sequence         */
    int32_t startL2EN;                  /* start of level 2 sequence    */
    int32_t lastStrongRTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t runStart;                   /* start position of the run    */
    UBiDiLevel runLevel;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

अटल व्योम
addPoint(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#घोषणा FIRSTALLOC  10
    Point point;
    जोड़ो_Points_2 * p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);

    अगर (p_जोड़ो_Points->capacity == 0)
    {
        p_जोड़ो_Points->points=static_cast<Point *>(uprv_malloc(माप(Point)*FIRSTALLOC));
        अगर (p_जोड़ो_Points->points == nullptr)
        {
            p_जोड़ो_Points->errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        p_जोड़ो_Points->capacity=FIRSTALLOC;
    }
    अगर (p_जोड़ो_Points->size >= p_जोड़ो_Points->capacity) /* no room for new point */
    {
        Point * savePoints=p_जोड़ो_Points->points;
        p_जोड़ो_Points->points=static_cast<Point *>(uprv_realloc(p_जोड़ो_Points->points,
                                           p_जोड़ो_Points->capacity*2*माप(Point)));
        अगर (p_जोड़ो_Points->points == nullptr)
        {
            p_जोड़ो_Points->points=savePoints;
            p_जोड़ो_Points->errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        अन्यथा  p_जोड़ो_Points->capacity*=2;
    }
    point.pos=pos;
    point.flag=flag;
    p_जोड़ो_Points->points[p_जोड़ो_Points->size]=point;
    p_जोड़ो_Points->size++;
#अघोषित FIRSTALLOC
}

अटल व्योम
setLevelsOutsideIsolates(UBiDi *pBiDi, int32_t start, int32_t limit, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t isolate_गिनती=0, k;
    क्रम(k=start; k<limit; k++) {
        dirProp=dirProps[k];
        अगर(dirProp==PDI)
            isolate_गिनती--;
        अगर(isolate_गिनती==0)
            levels[k]=level;
        अगर(dirProp==LRI || dirProp==RLI)
            isolate_गिनती++;
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

अटल व्योम
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t start, int32_t limit) {
    uint8_t cell, oldStateSeq, actionSeq;
    स्थिर ImpTab * pImpTab=pLevState->pImpTab;
    स्थिर ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * levels=pBiDi->levels;
    UBiDiLevel level, addLevel;
    जोड़ो_Points_2 * p_जोड़ो_Points;
    int32_t start0, k;

    start0=start;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    addLevel=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    अगर(actionSeq) {
        चयन(actionSeq) {
        हाल 1:                         /* init ON seq */
            pLevState->startON=start0;
            अवरोध;

        हाल 2:                         /* prepend ON seq to current seq */
            start=pLevState->startON;
            अवरोध;

        हाल 3:                         /* EN/AN after R+ON */
            level=pLevState->runLevel+1;
            setLevelsOutsideIsolates(pBiDi, pLevState->startON, start0, level);
            अवरोध;

        हाल 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->runLevel+2;
            setLevelsOutsideIsolates(pBiDi, pLevState->startON, start0, level);
            अवरोध;

        हाल 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            अगर (pLevState->startL2EN >= 0) {
                addPoint(pBiDi, pLevState->startL2EN, LRM_BEFORE);
            }
            pLevState->startL2EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);
            अगर ((p_जोड़ो_Points->capacity == 0) ||
                (p_जोड़ो_Points->size <= p_जोड़ो_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->lastStrongRTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                अगर ((level & 1) && (pLevState->startON > 0)) {  /* after ON */
                    start=pLevState->startON;   /* reset to basic run level */
                }
                अगर (_prop == DirProp_S)                /* add LRM before S */
                {
                    addPoint(pBiDi, start0, LRM_BEFORE);
                    p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;
                }
                अवरोध;
            }
            /* reset previous RTL cont to level for LTR text */
            क्रम (k=pLevState->lastStrongRTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                levels[k]=(levels[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;
            pLevState->lastStrongRTL=-1;
            अगर (_prop == DirProp_S)            /* add LRM before S */
            {
                addPoint(pBiDi, start0, LRM_BEFORE);
                p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;
            }
            अवरोध;

        हाल 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);
            अगर (p_जोड़ो_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_जोड़ो_Points->size=p_जोड़ो_Points->confirmed;
            pLevState->startON=-1;
            pLevState->startL2EN=-1;
            pLevState->lastStrongRTL=limit - 1;
            अवरोध;

        हाल 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            अगर ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reorderingMode!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                अगर (pLevState->startL2EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->lastStrongRTL=limit - 1;
                    अवरोध;
                }
                अगर (pLevState->startL2EN >= 0)  /* after EN, no AN */
                {
                    addPoint(pBiDi, pLevState->startL2EN, LRM_BEFORE);
                    pLevState->startL2EN=-2;
                }
                /* note AN */
                addPoint(pBiDi, start0, LRM_BEFORE);
                अवरोध;
            }
            /* if first EN/AN after R/AL */
            अगर (pLevState->startL2EN == -1) {
                pLevState->startL2EN=start0;
            }
            अवरोध;

        हाल 8:                         /* note location of latest R/AL */
            pLevState->lastStrongRTL=limit - 1;
            pLevState->startON=-1;
            अवरोध;

        हाल 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            क्रम (k=start0-1; k>=0 && !(levels[k]&1); k--);
            अगर(k>=0) {
                addPoint(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);
                p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;   /* confirm it */
            }
            pLevState->startON=start0;
            अवरोध;

        हाल 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            addPoint(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            addPoint(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            अवरोध;

        हाल 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);
            p_जोड़ो_Points->size=p_जोड़ो_Points->confirmed;
            अगर (_prop == DirProp_S)            /* add RLM before S */
            {
                addPoint(pBiDi, start0, RLM_BEFORE);
                p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;
            }
            अवरोध;

        हाल 12:                        /* L after L+ON/AN */
            level=pLevState->runLevel + addLevel;
            क्रम(k=pLevState->startON; k<start0; k++) {
                अगर (levels[k]<level)
                    levels[k]=level;
            }
            p_जोड़ो_Points=&(pBiDi->जोड़ो_Points);
            p_जोड़ो_Points->confirmed=p_जोड़ो_Points->size;   /* confirm inserts */
            pLevState->startON=start0;
            अवरोध;

        हाल 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->runLevel;
            क्रम(k=start0-1; k>=pLevState->startON; k--) {
                अगर(levels[k]==level+3) {
                    जबतक(levels[k]==level+3) {
                        levels[k--]-=2;
                    }
                    जबतक(levels[k]==level) {
                        k--;
                    }
                }
                अगर(levels[k]==level+2) {
                    levels[k]=level;
                    जारी;
                }
                levels[k]=level+1;
            }
            अवरोध;

        हाल 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->runLevel+1;
            क्रम(k=start0-1; k>=pLevState->startON; k--) {
                अगर(levels[k]>level) {
                    levels[k]-=2;
                }
            }
            अवरोध;

        शेष:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    अगर((addLevel) || (start < start0)) {
        level=pLevState->runLevel + addLevel;
        अगर(start>=pLevState->runStart) {
            क्रम(k=start; k<limit; k++) {
                levels[k]=level;
            }
        } अन्यथा {
            setLevelsOutsideIsolates(pBiDi, start, limit, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
अटल DirProp
lastL_R_AL(UBiDi *pBiDi) {
    स्थिर char16_t *text=pBiDi->prologue;
    int32_t length=pBiDi->pro_लंबाई;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    क्रम(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_पूर्व(text, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        अगर(dirProp==L) {
            वापस DirProp_L;
        }
        अगर(dirProp==R || dirProp==AL) {
            वापस DirProp_R;
        }
        अगर(dirProp==B) {
            वापस DirProp_ON;
        }
    }
    वापस DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
अटल DirProp
firstL_R_AL_EN_AN(UBiDi *pBiDi) {
    स्थिर char16_t *text=pBiDi->epilogue;
    int32_t length=pBiDi->epi_लंबाई;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    क्रम(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_अग्र(text, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        अगर(dirProp==L) {
            वापस DirProp_L;
        }
        अगर(dirProp==R || dirProp==AL) {
            वापस DirProp_R;
        }
        अगर(dirProp==EN) {
            वापस DirProp_EN;
        }
        अगर(dirProp==AN) {
            वापस DirProp_AN;
        }
    }
    वापस DirProp_ON;
}

अटल व्योम
resolveImplicitLevels(UBiDi *pBiDi,
                      int32_t start, int32_t limit,
                      DirProp sor, DirProp eor) {
    स्थिर DirProp *dirProps=pBiDi->dirProps;
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
        static_cast<UBool>((start < pBiDi->lastArabicPos) && (GET_PARALEVEL(pBiDi, start) & 1) &&
                           (pBiDi->reorderingMode == UBIDI_REORDER_INVERSE_LIKE_DIRECT ||
                            pBiDi->reorderingMode == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.startL2EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.lastStrongRTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.runStart=start;
    levState.runLevel=pBiDi->levels[start];
    levState.pImpTab = static_cast<स्थिर ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.runLevel & 1]);
    levState.pImpAct = static_cast<स्थिर ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.runLevel & 1]);
    अगर(start==0 && pBiDi->pro_लंबाई>0) {
        DirProp lastStrong=lastL_R_AL(pBiDi);
        अगर(lastStrong!=DirProp_ON) {
            sor=lastStrong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    अगर(dirProps[start]==PDI  && pBiDi->isolate_गिनती >= 0) {
        levState.startON=pBiDi->isolates[pBiDi->isolate_गिनती].startON;
        start1=pBiDi->isolates[pBiDi->isolate_गिनती].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_गिनती].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_गिनती].state;
        pBiDi->isolate_गिनती--;
    } अन्यथा {
        levState.startON=-1;
        start1=start;
        अगर(dirProps[start]==NSM)
            stateImp = 1 + sor;
        अन्यथा
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, start, start);
    }
    start2=start;                       /* to make Java compiler happy */

    क्रम(i=start; i<=limit; i++) {
        अगर(i>=limit) {
            int32_t k;
            क्रम(k=limit-1; k>start&&(DIRPROP_FLAG(dirProps[k])&MASK_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            अगर(dirProp==LRI || dirProp==RLI)
                अवरोध;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } अन्यथा {
            DirProp prop, prop1;
            prop=dirProps[i];
            अगर(prop==B) {
                pBiDi->isolate_गिनती=-1; /* current isolates stack entry == none */
            }
            अगर(inverseRTL) {
                अगर(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } अन्यथा अगर(prop==EN) {
                    अगर(अग्र_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        अग्र_Strong_Prop=R;   /* set default */
                        अग्र_Strong_Pos=limit;
                        क्रम(j=i+1; j<limit; j++) {
                            prop1=dirProps[j];
                            अगर(prop1==L || prop1==R || prop1==AL) {
                                अग्र_Strong_Prop=prop1;
                                अग्र_Strong_Pos=j;
                                अवरोध;
                            }
                        }
                    }
                    अगर(अग्र_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=groupProp[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        अगर((i==limit) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        अगर(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            चयन(actionImp) {
            हाल 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                अवरोध;
            हाल 2:             /* init new seq2 */
                start2=i;
                अवरोध;
            हाल 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                अवरोध;
            हाल 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                अवरोध;
            शेष:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    अगर(limit==pBiDi->length && pBiDi->epi_लंबाई>0) {
        DirProp firstStrong=firstL_R_AL_EN_AN(pBiDi);
        अगर(firstStrong!=DirProp_ON) {
            eor=firstStrong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    क्रम(i=limit-1; i>start&&(DIRPROP_FLAG(dirProps[i])&MASK_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    अगर((dirProp==LRI || dirProp==RLI) && limit<pBiDi->length) {
        pBiDi->isolate_गिनती++;
        pBiDi->isolates[pBiDi->isolate_गिनती].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_गिनती].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_गिनती].start1=start1;
        pBiDi->isolates[pBiDi->isolate_गिनती].startON=levState.startON;
    }
    अन्यथा
        processPropertySeq(pBiDi, &levState, eor, limit, limit);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
अटल व्योम
adjustWSLevels(UBiDi *pBiDi) {
    स्थिर DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *levels=pBiDi->levels;
    int32_t i;

    अगर(pBiDi->flags&MASK_WS) {
        UBool orderParagraphsLTR=pBiDi->orderParagraphsLTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        जबतक(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            जबतक(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&MASK_WS) {
                अगर(orderParagraphsLTR&&(flag&DIRPROP_FLAG(B))) {
                    levels[i]=0;
                } अन्यथा {
                    levels[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            जबतक(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                अगर(flag&MASK_BN_EXPLICIT) {
                    levels[i]=levels[i+1];
                } अन्यथा अगर(orderParagraphsLTR&&(flag&DIRPROP_FLAG(B))) {
                    levels[i]=0;
                    अवरोध;
                } अन्यथा अगर(flag&MASK_B_S) {
                    levels[i]=GET_PARALEVEL(pBiDi, i);
                    अवरोध;
                }
            }
        }
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_setContext(UBiDi *pBiDi,
                 स्थिर char16_t *prologue, int32_t pro_लंबाई,
                 स्थिर char16_t *epilogue, int32_t epi_लंबाई,
                 UErrorCode *pErrorCode) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    अगर(pBiDi==nullptr || pro_लंबाई<-1 || epi_लंबाई<-1 ||
       (prologue==nullptr && pro_लंबाई!=0) || (epilogue==nullptr && epi_लंबाई!=0)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर(pro_लंबाई==-1) {
        pBiDi->pro_लंबाई=u_strlen(prologue);
    } अन्यथा {
        pBiDi->pro_लंबाई=pro_लंबाई;
    }
    अगर(epi_लंबाई==-1) {
        pBiDi->epi_लंबाई=u_strlen(epilogue);
    } अन्यथा {
        pBiDi->epi_लंबाई=epi_लंबाई;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

अटल व्योम
setParaSuccess(UBiDi *pBiDi) {
    pBiDi->pro_लंबाई=0;                 /* forget the last context */
    pBiDi->epi_लंबाई=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#घोषणा BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#घोषणा BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

अटल व्योम
setParaRunsOnly(UBiDi *pBiDi, स्थिर char16_t *text, int32_t length,
                UBiDiLevel paraLevel, UErrorCode *pErrorCode) {
    int32_t *runsOnlyMemory = nullptr;
    int32_t *visualMap;
    char16_t *visualText;
    int32_t save_लंबाई, saveTrailingWSStart;
    स्थिर UBiDiLevel *levels;
    UBiDiLevel *saveLevels;
    UBiDiDirection saveDirection;
    UBool saveMayAllocateText;
    Run *runs;
    int32_t visual_लंबाई, i, j, visualStart, logicalStart,
            run_गिनती, run_लंबाई, addedRuns, जोड़ो_Remove,
            start, limit, step, सूचकांक_Odd_Bit, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reorderingMode=UBIDI_REORDER_DEFAULT;
    अगर(length==0) {
        ubidi_setPara(pBiDi, text, length, paraLevel, nullptr, pErrorCode);
        जाओ cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runsOnlyMemory=static_cast<int32_t *>(uprv_malloc(length*(माप(int32_t)+माप(char16_t)+माप(UBiDiLevel))));
    अगर(runsOnlyMemory==nullptr) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        जाओ cleanup3;
    }
    visualMap=runsOnlyMemory;
    visualText = reinterpret_cast<char16_t*>(&visualMap[length]);
    saveLevels = reinterpret_cast<UBiDiLevel*>(&visualText[length]);
    saveOptions=pBiDi->reorderingOptions;
    अगर(saveOptions & UBIDI_OPTION_जोड़ो_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_जोड़ो_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_setPara(pBiDi, text, length, paraLevel, nullptr, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        जाओ cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    levels=ubidi_getLevels(pBiDi, pErrorCode);
    uprv_memcpy(saveLevels, levels, (माप_प्रकार)pBiDi->length*माप(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_लंबाई=pBiDi->length;
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
    visual_लंबाई=ubidi_writeReordered(pBiDi, visualText, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_INTERNAL_RUNS_ONLY, pErrorCode);
    ubidi_getVisualMap(pBiDi, visualMap, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        जाओ cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reorderingMode=UBIDI_REORDER_INVERSE_LIKE_DIRECT;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    saveMayAllocateText=pBiDi->mayAllocateText;
    pBiDi->mayAllocateText=false;
    ubidi_setPara(pBiDi, visualText, visual_लंबाई, paraLevel, nullptr, pErrorCode);
    pBiDi->mayAllocateText=saveMayAllocateText;
    ubidi_getRuns(pBiDi, pErrorCode);
    अगर(U_FAILURE(*pErrorCode)) {
        जाओ cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    run_गिनती=pBiDi->run_गिनती;
    runs=pBiDi->runs;
    visualStart=0;
    क्रम(i=0; i<run_गिनती; i++, visualStart+=run_लंबाई) {
        run_लंबाई=runs[i].visualLimit-visualStart;
        अगर(run_लंबाई<2) {
            जारी;
        }
        logicalStart=GET_सूचकांक(runs[i].logicalStart);
        क्रम(j=logicalStart+1; j<logicalStart+run_लंबाई; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            अगर((BIDI_ABS(index0-index1)!=1) || (saveLevels[index0]!=saveLevels[index1])) {
                addedRuns++;
            }
        }
    }
    अगर(addedRuns) {
        अगर(getRunsMemory(pBiDi, run_गिनती+addedRuns)) {
            अगर(run_गिनती==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runsMemory[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runsMemory;
            pBiDi->run_गिनती+=addedRuns;
        } अन्यथा {
            *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
            जाओ cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    क्रम(i=run_गिनती-1; i>=0; i--) {
        run_लंबाई= i==0 ? runs[0].visualLimit :
                          runs[i].visualLimit-runs[i-1].visualLimit;
        logicalStart=runs[i].logicalStart;
        सूचकांक_Odd_Bit=GET_ODD_BIT(logicalStart);
        logicalStart=GET_सूचकांक(logicalStart);
        अगर(run_लंबाई<2) {
            अगर(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logicalStart];
            runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            saveLevels[logicalPos]^सूचकांक_Odd_Bit);
            जारी;
        }
        अगर(सूचकांक_Odd_Bit) {
            start=logicalStart;
            limit=logicalStart+run_लंबाई-1;
            step=1;
        } अन्यथा {
            start=logicalStart+run_लंबाई-1;
            limit=logicalStart;
            step=-1;
        }
        क्रम(j=start; j!=limit; j+=step) {
            index0=visualMap[j];
            index1=visualMap[j+step];
            अगर((BIDI_ABS(index0-index1)!=1) || (saveLevels[index0]!=saveLevels[index1])) {
                logicalPos=BIDI_MIN(visualMap[start], index0);
                runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            saveLevels[logicalPos]^सूचकांक_Odd_Bit);
                runs[i+addedRuns].visualLimit=runs[i].visualLimit;
                runs[i].visualLimit-=BIDI_ABS(j-start)+1;
                जोड़ो_Remove=runs[i].जोड़ो_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].जोड़ो_Remove=जोड़ो_Remove;
                runs[i].जोड़ो_Remove&=~जोड़ो_Remove;
                start=j+step;
                addedRuns--;
            }
        }
        अगर(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[start], visualMap[limit]);
        runs[i+addedRuns].logicalStart=MAKE_सूचकांक_ODD_PAIR(logicalPos,
                                            saveLevels[logicalPos]^सूचकांक_Odd_Bit);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->text=text;
    pBiDi->length=save_लंबाई;
    pBiDi->original_लंबाई=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    अगर(save_लंबाई>pBiDi->levels_आकार) {
        save_लंबाई=pBiDi->levels_आकार;
    }
    uprv_memcpy(pBiDi->levels, saveLevels, (माप_प्रकार)save_लंबाई*माप(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    अगर(pBiDi->run_गिनती>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runsOnlyMemory);

    pBiDi->reorderingMode=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI व्योम U_EXPORT2
ubidi_setPara(UBiDi *pBiDi, स्थिर char16_t *text, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embeddingLevels,
              UErrorCode *pErrorCode) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    अगर(pBiDi==nullptr || text==nullptr || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर(length==-1) {
        length=u_strlen(text);
    }

    /* special treatment for RUNS_ONLY mode */
    अगर(pBiDi->reorderingMode==UBIDI_REORDER_RUNS_ONLY) {
        setParaRunsOnly(pBiDi, text, length, paraLevel, pErrorCode);
        वापस;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=nullptr;          /* mark unfinished setPara */
    pBiDi->text=text;
    pBiDi->length=pBiDi->original_लंबाई=pBiDi->परिणाम_लंबाई=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_गिनती=1;

    pBiDi->dirProps=nullptr;
    pBiDi->levels=nullptr;
    pBiDi->runs=nullptr;
    pBiDi->जोड़ो_Points.size=0;         /* clean up from last call */
    pBiDi->जोड़ो_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    अगर(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        अगर(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->run_गिनती=0;
        pBiDi->para_गिनती=0;
        setParaSuccess(pBiDi);          /* mark successful setPara */
        वापस;
    }

    pBiDi->run_गिनती=-1;

    /* allocate paras memory */
    अगर(pBiDi->parasMemory)
        pBiDi->paras=pBiDi->parasMemory;
    अन्यथा
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    अगर(getDirPropsMemory(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dirPropsMemory;
        अगर(!getDirProps(pBiDi)) {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
    } अन्यथा {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    अगर(embeddingLevels==nullptr) {
        /* no: determine explicit levels according to the (Xn) rules */\
        अगर(getLevelsMemory(pBiDi, length)) {
            pBiDi->levels=pBiDi->levelsMemory;
            direction=resolveExplicitLevels(pBiDi, pErrorCode);
            अगर(U_FAILURE(*pErrorCode)) {
                वापस;
            }
        } अन्यथा {
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
    } अन्यथा {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->levels=embeddingLevels;
        direction=checkExplicitLevels(pBiDi, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस;
        }
    }

    /* allocate isolate memory */
    अगर(pBiDi->isolate_गिनती<=SIMPLE_ISOLATES_गिनती)
        pBiDi->isolates=pBiDi->simpleIsolates;
    अन्यथा
        अगर((int32_t)(pBiDi->isolate_गिनती*माप(Isolate))<=pBiDi->isolates_आकार)
            pBiDi->isolates=pBiDi->isolatesMemory;
        अन्यथा {
            अगर(getInitialIsolatesMemory(pBiDi, pBiDi->isolate_गिनती)) {
                pBiDi->isolates=pBiDi->isolatesMemory;
            } अन्यथा {
                *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस;
            }
        }
    pBiDi->isolate_गिनती=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    चयन(direction) {
    हाल UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        अवरोध;
    हाल UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        अवरोध;
    शेष:
        /*
         *  Choose the right implicit state table
         */
        चयन(pBiDi->reorderingMode) {
        हाल UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            अवरोध;
        हाल UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            अवरोध;
        हाल UBIDI_REORDER_GROUP_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&impTab_GROUP_NUMBERS_WITH_R;
            अवरोध;
        हाल UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            अवरोध;
        हाल UBIDI_REORDER_INVERSE_LIKE_DIRECT:
            अगर (pBiDi->reorderingOptions & UBIDI_OPTION_जोड़ो_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_LIKE_DIRECT_WITH_MARKS;
            } अन्यथा {
                pBiDi->pImpTabPair=&impTab_INVERSE_LIKE_DIRECT;
            }
            अवरोध;
        हाल UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            अगर (pBiDi->reorderingOptions & UBIDI_OPTION_जोड़ो_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } अन्यथा {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            अवरोध;
        शेष:
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
        अगर(embeddingLevels==nullptr && pBiDi->para_गिनती<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolveImplicitLevels(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } अन्यथा {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *levels=pBiDi->levels;
            int32_t start, limit=0;
            UBiDiLevel level, अग्र_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            अग्र_Level=levels[0];
            अगर(level<अग्र_Level) {
                eor=GET_LR_FROM_LEVEL(अग्र_Level);
            } अन्यथा {
                eor=GET_LR_FROM_LEVEL(level);
            }

            करो {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                start=limit;
                level=अग्र_Level;
                अगर((start>0) && (dirProps[start-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, start));
                } अन्यथा {
                    sor=eor;
                }

                /* search for the limit of this run */
                जबतक((++limit<length) &&
                      ((levels[limit]==level) ||
                       (DIRPROP_FLAG(dirProps[limit])&MASK_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                अगर(limit<length) {
                    अग्र_Level=levels[limit];
                } अन्यथा {
                    अग्र_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                अगर(NO_OVERRIDE(level)<NO_OVERRIDE(अग्र_Level)) {
                    eor=GET_LR_FROM_LEVEL(अग्र_Level);
                } अन्यथा {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                अगर(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolveImplicitLevels(pBiDi, start, limit, sor, eor);
                } अन्यथा {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    करो {
                        levels[start++]&=~UBIDI_LEVEL_OVERRIDE;
                    } जबतक(start<limit);
                }
            } जबतक(limit<length);
        }
        /* check if we got any memory shortage while adding insert points */
        अगर (U_FAILURE(pBiDi->जोड़ो_Points.errorCode))
        {
            *pErrorCode=pBiDi->जोड़ो_Points.errorCode;
            वापस;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        अवरोध;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    अगर((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_जोड़ो_MARKS) &&
       ((pBiDi->reorderingMode==UBIDI_REORDER_INVERSE_LIKE_DIRECT) ||
        (pBiDi->reorderingMode==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, start, last;
        UBiDiLevel level;
        DirProp dirProp;
        क्रम(i=0; i<pBiDi->para_गिनती; i++) {
            last=(pBiDi->paras[i].limit)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            अगर(level==0)
                जारी;           /* LTR paragraph */
            start= i==0 ? 0 : pBiDi->paras[i-1].limit;
            क्रम(j=last; j>=start; j--) {
                dirProp=dirProps[j];
                अगर(dirProp==L) {
                    अगर(j<last) {
                        जबतक(dirProps[last]==B) {
                            last--;
                        }
                    }
                    addPoint(pBiDi, last, RLM_BEFORE);
                    अवरोध;
                }
                अगर(DIRPROP_FLAG(dirProp) & MASK_R_AL) {
                    अवरोध;
                }
            }
        }
    }

    अगर(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->परिणाम_लंबाई -= pBiDi->control_गिनती;
    } अन्यथा {
        pBiDi->परिणाम_लंबाई += pBiDi->जोड़ो_Points.size;
    }
    setParaSuccess(pBiDi);              /* mark successful setPara */
}

U_CAPI व्योम U_EXPORT2
ubidi_orderParagraphsLTR(UBiDi *pBiDi, UBool orderParagraphsLTR) {
    अगर(pBiDi!=nullptr) {
        pBiDi->orderParagraphsLTR=orderParagraphsLTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isOrderParagraphsLTR(UBiDi *pBiDi) {
    अगर(pBiDi!=nullptr) {
        वापस pBiDi->orderParagraphsLTR;
    } अन्यथा {
        वापस false;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(स्थिर UBiDi *pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->direction;
    } अन्यथा {
        वापस UBIDI_LTR;
    }
}

U_CAPI स्थिर char16_t* U_EXPORT2
ubidi_getText(स्थिर UBiDi* pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->text;
    } अन्यथा {
        वापस nullptr;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_लंबाई(स्थिर UBiDi *pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->original_लंबाई;
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_लंबाई(स्थिर UBiDi *pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->length;
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_परिणाम_लंबाई(स्थिर UBiDi *pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->परिणाम_लंबाई;
    } अन्यथा {
        वापस 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(स्थिर UBiDi *pBiDi) {
    अगर(IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस pBiDi->paraLevel;
    } अन्यथा {
        वापस 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_गिनती_Paragraphs(UBiDi *pBiDi) {
    अगर(!IS_VALID_PARA_OR_पंक्ति(pBiDi)) {
        वापस 0;
    } अन्यथा {
        वापस pBiDi->para_गिनती;
    }
}

U_CAPI व्योम U_EXPORT2
ubidi_get_Paragraph_By_सूचकांक(स्थिर UBiDi *pBiDi, int32_t para_सूचकांक,
                          int32_t *pParaStart, int32_t *pParaLimit,
                          UBiDiLevel *pParaLevel, UErrorCode *pErrorCode) {
    int32_t paraStart;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode);
    RETURN_VOID_IF_BAD_RANGE(para_सूचकांक, 0, pBiDi->para_गिनती, *pErrorCode);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    अगर(para_सूचकांक) {
        paraStart=pBiDi->paras[para_सूचकांक-1].limit;
    } अन्यथा {
        paraStart=0;
    }
    अगर(pParaStart!=nullptr) {
        *pParaStart=paraStart;
    }
    अगर(pParaLimit!=nullptr) {
        *pParaLimit=pBiDi->paras[para_सूचकांक].limit;
    }
    अगर(pParaLevel!=nullptr) {
        *pParaLevel=GET_PARALEVEL(pBiDi, paraStart);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(स्थिर UBiDi *pBiDi, int32_t char_सूचकांक,
                          int32_t *pParaStart, int32_t *pParaLimit,
                          UBiDiLevel *pParaLevel, UErrorCode *pErrorCode) {
    int32_t para_सूचकांक;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(pErrorCode, -1);
    RETURN_IF_NOT_VALID_PARA_OR_पंक्ति(pBiDi, *pErrorCode, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_सूचकांक, 0, pBiDi->length, *pErrorCode, -1);

    क्रम(para_सूचकांक=0; char_सूचकांक>=pBiDi->paras[para_सूचकांक].limit; para_सूचकांक++);
    ubidi_get_Paragraph_By_सूचकांक(pBiDi, para_सूचकांक, pParaStart, pParaLimit, pParaLevel, pErrorCode);
    वापस para_सूचकांक;
}

U_CAPI व्योम U_EXPORT2
ubidi_setClassCallback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       स्थिर व्योम *newContext, UBiDiClassCallback **oldFn,
                       स्थिर व्योम **oldContext, UErrorCode *pErrorCode)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(pErrorCode);
    अगर(pBiDi==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }
    अगर( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    अगर( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI व्योम U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, स्थिर व्योम **context)
{
    अगर(pBiDi==nullptr) {
        वापस;
    }
    अगर( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    अगर( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    अगर( pBiDi->fnClassCallback == nullptr ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    अगर(dir >= U_CHAR_DIRECTION_गिनती) {
        dir = (UCharDirection)ON;
    }
    वापस dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#अगर U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(स्थिर UnicodeString& s) {
    अगर (s == u"ltr") {
        वापस U_MF_BIDI_OPTION_LTR;
    }
    अगर (s == u"rtl") {
        वापस U_MF_BIDI_OPTION_RTL;
    }
    अगर (s == u"auto") {
        वापस U_MF_BIDI_OPTION_AUTO;
    }
    वापस U_MF_BIDI_OPTION_INHERIT; // inherit is default
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
    अगर (bidiIsolationStrategy == U_MF_BIDI_OFF)
        वापस &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    bool isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    चयन (dir) {
        हाल U_MF_DIRECTIONALITY_LTR:
            अगर (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                वापस &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            अगर (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            अवरोध; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        हाल U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            अगर (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            अवरोध; // End of 2iv.
        // 2v. Else:
        शेष:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            अगर (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } अन्यथा {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            अवरोध; // End of 2v
    } // `fmt` now contains the isolated string
    वापस &fmt;
}

U_NAMESPACE_END

#पूर्ण_अगर // #if SHOW_CPLUSPLUS_API

