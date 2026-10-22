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

#ضمن <limits>

#ضمن "cmemory.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/ubidi.h"
#ضمن "unicode/utf16.h"
#ضمن "ubidi_props.h"
#ضمن "ubidiimp.h"
#ضمن "uassert.h"

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
ساكن ثابت Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
ساكن ثابت Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
ساكن ثابت Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#تعریف_کرو DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#تعریف_کرو DIRPROP_FLAG_E(level)  flagE[(level)&1]
#تعریف_کرو DIRPROP_FLAG_O(level)  flagO[(level)&1]

#تعریف_کرو DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#تعریف_کرو NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    أرجع ubidi_openSized(0, 0, &error_کوڈ);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_لمائی_2, int32_t max_لگاتار_گنتی, UError_کوڈ *p_Error_کوڈ) {
    UBiDi *pBiDi;

    /* check the argument values */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    } وإلا إذا(max_لمائی_2<0 || max_لگاتار_گنتی<0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(حجم(UBiDi));
    إذا(pBiDi==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, حجم(UBiDi));

    /* allocate memory for arrays as requested */
    إذا(max_لمائی_2>0) {
        إذا( !get_Initial_Dir_Props_حافظہ(pBiDi, max_لمائی_2) ||
            !get_Initial_پدھراں_حافظہ(pBiDi, max_لمائی_2)
        ) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        }
    } وإلا {
        pBiDi->may_Allocate_متن=سچ;
    }

    إذا(max_لگاتار_گنتی>0) {
        إذا(max_لگاتار_گنتی==1) {
            /* use simpleRuns[] */
            pBiDi->runs_ناپ=حجم(لگاتار);
        } وإلا إذا(!get_Initial_Runs_حافظہ(pBiDi, max_لگاتار_گنتی)) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        }
    } وإلا {
        pBiDi->mayAllocateRuns=سچ;
    }

    إذا(U_SUCCESS(*p_Error_کوڈ)) {
        أرجع pBiDi;
    } وإلا {
        ubidi_close(pBiDi);
        أرجع خالی_سوچک;
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
ubidi_get_حافظہ(Bidi_حافظہ_For_ونڈ *bidiMem, int32_t *p_ناپ, UBool mayAllocate, size_t ناپ_Needed) {
    إذا (ناپ_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      أرجع خطأ;
    }
    فراغ **p_حافظہ = (فراغ **)bidiMem;
    /* check for existing memory */
    إذا(*p_حافظہ==خالی_سوچک) {
        /* we need to allocate memory */
        إذا(mayAllocate && (*p_حافظہ=uprv_malloc(ناپ_Needed))!=خالی_سوچک) {
            *p_ناپ=static_cast<int32_t>(ناپ_Needed);
            أرجع سچ;
        } وإلا {
            أرجع خطأ;
        }
    } وإلا {
        إذا(ناپ_Needed<=static_cast<size_t>(*p_ناپ)) {
            /* there is already enough memory */
            أرجع سچ;
        }
        وإلا إذا(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            أرجع خطأ;
        } وإلا {
            /* we try to grow */
            فراغ *حافظہ;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            إذا((حافظہ=uprv_realloc(*p_حافظہ, ناپ_Needed))!=خالی_سوچک) {
                *p_حافظہ=حافظہ;
                *p_ناپ=static_cast<int32_t>(ناپ_Needed);
                أرجع سچ;
            } وإلا {
                /* we failed to grow */
                أرجع خطأ;
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    إذا(pBiDi!=خالی_سوچک) {
        pBiDi->pParaBiDi=خالی_سوچک;          /* in case one tries to reuse this block */
        إذا(pBiDi->dir_Props_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->dir_Props_حافظہ);
        }
        إذا(pBiDi->پدھراں_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->پدھراں_حافظہ);
        }
        إذا(pBiDi->openings_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->openings_حافظہ);
        }
        إذا(pBiDi->paras_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->paras_حافظہ);
        }
        إذا(pBiDi->runs_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->runs_حافظہ);
        }
        إذا(pBiDi->isolates_حافظہ!=خالی_سوچک) {
            uprv_free(pBiDi->isolates_حافظہ);
        }
        إذا(pBiDi->داخل_کرو_Points.points!=خالی_سوچک) {
            uprv_free(pBiDi->داخل_کرو_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Inverse(UBiDi *pBiDi, UBool isInverse) {
    إذا(pBiDi!=خالی_سوچک) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_منوال = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    إذا(pBiDi!=خالی_سوچک) {
        أرجع pBiDi->isInverse;
    } وإلا {
        أرجع خطأ;
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
U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Reordering_منوال(UBiDi *pBiDi, UBi_Di_Reordering_منوال reordering_منوال) UPRV_NO_SANITIZE_UNDEFINED {
    إذا ((pBiDi!=خالی_سوچک) && (reordering_منوال >= UBIDI_REORDER_DEFAULT)
                        && (reordering_منوال < UBIDI_REORDER_گنتی)) {
        pBiDi->reordering_منوال = reordering_منوال;
        pBiDi->isInverse = reordering_منوال == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_منوال U_EXPORT2
ubidi_get_Reordering_منوال(UBiDi *pBiDi) {
    إذا (pBiDi!=خالی_سوچک) {
        أرجع pBiDi->reordering_منوال;
    } وإلا {
        أرجع UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    إذا (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_داخل_کرو_MARKS;
    }
    إذا (pBiDi!=خالی_سوچک) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    إذا (pBiDi!=خالی_سوچک) {
        أرجع pBiDi->reorderingOptions;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_ادھار_Direction(ثابت char16_t *متن,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    إذا( متن==خالی_سوچک || length<-1 ){
        أرجع UBIDI_NEUTRAL;
    }

    إذا(length==-1) {
        length=u_strlen(متن);
    }

    لكل( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dir = u_charDirection(uchar);
        إذا( dir == U_بایاں_منزل_دایاں )
                أرجع UBIDI_LTR;
        إذا( dir == U_دایاں_منزل_بایاں || dir ==U_دایاں_منزل_بایاں_ARABIC )
                أرجع UBIDI_RTL;
    }
    أرجع UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
ساكن DirProp
پہلا_L_R_AL(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->prologue;
    int32_t length=pBiDi->pro_لمائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, نتیجہ=ON;
    لكل(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        إذا(نتیجہ==ON) {
            إذا(dirProp==L || dirProp==R || dirProp==AL) {
                نتیجہ=dirProp;
            }
        } وإلا {
            إذا(dirProp==B) {
                نتیجہ=ON;
            }
        }
    }
    أرجع نتیجہ;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
ساكن UBool
check_Para_گنتی(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_گنتی;
    إذا(pBiDi->paras==pBiDi->simpleParas) {
        إذا(count<=SIMPLE_PARAS_گنتی)
            أرجع سچ;
        إذا(!get_Initial_Paras_حافظہ(pBiDi, SIMPLE_PARAS_گنتی * 2))
            أرجع خطأ;
        pBiDi->paras=pBiDi->paras_حافظہ;
        uprv_memcpy(pBiDi->paras_حافظہ, pBiDi->simpleParas, SIMPLE_PARAS_گنتی * حجم(Para));
        أرجع سچ;
    }
    إذا(!get_Initial_Paras_حافظہ(pBiDi, count * 2))
        أرجع خطأ;
    pBiDi->paras=pBiDi->paras_حافظہ;
    أرجع سچ;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
ساكن UBool
getDirProps(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->متن;
    DirProp *dirProps=pBiDi->dir_Props_حافظہ;    /* pBiDi->dirProps is const */

    int32_t i=0, original_لمائی=pBiDi->original_لمائی;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_LIKE_سدھا ||
            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t آخری_Arabic_Pos=-1;
    int32_t control_گنتی=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    تعداد State {
         NOT_SEEKING_STRONG,            /* 0: not contextual paraLevel, not after FSI */
         SEEKING_STRONG_FOR_PARA,       /* 1: looking for first strong char in para */
         SEEKING_STRONG_FOR_FSI,        /* 2: looking for first strong after FSI */
         LOOKING_FOR_PDI                /* 3: found strong after FSI, looking for PDI */
    };
    State state;
    DirProp آخری_Strong=ON;              /* for default level & inverse BiDi */
    /* The following stacks are used to manage isolate sequences. Those
       sequences may be nested, but obviously never more deeply than the
       maximum explicit embedding level.
       lastStack is the index of the last used entry in the stack. A value of -1
       means that there is no open isolate sequence.
       lastStack is reset to -1 on paragraph boundaries. */
    /* The following stack contains the position of the initiator of
       each open isolate sequence */
    int32_t isolate_شروع_ڈھیر[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  پچھلا_State_ڈھیر[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t ڈھیر_آخری=-1;

    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    إذا(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        آخری_Strong=defaultParaLevel;
        إذا(pBiDi->pro_لمائی>0 &&                    /* there is a prologue */
           (dirProp=پہلا_L_R_AL(pBiDi))!=ON) {  /* with a strong character */
            إذا(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            وإلا
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } وإلا {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } وإلا {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    لكل( /* i=0 above */ ; i<original_لمائی; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, original_لمائی, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        إذا(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        إذا(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_گنتی++;
        إذا(dirProp==L) {
            إذا(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_گنتی-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            وإلا إذا(state==SEEKING_STRONG_FOR_FSI) {
                إذا(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            آخری_Strong=L;
            استمر;
        }
        إذا(dirProp==R || dirProp==AL) {
            إذا(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_گنتی-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            وإلا إذا(state==SEEKING_STRONG_FOR_FSI) {
                إذا(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_شروع_ڈھیر[ڈھیر_آخری]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            آخری_Strong=R;
            إذا(dirProp==AL)
                آخری_Arabic_Pos=i-1;
            استمر;
        }
        إذا(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            ڈھیر_آخری++;
            إذا(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_شروع_ڈھیر[ڈھیر_آخری]=i-1;
                پچھلا_State_ڈھیر[ڈھیر_آخری]=state;
            }
            إذا(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            وإلا
                state=LOOKING_FOR_PDI;
            استمر;
        }
        إذا(dirProp==PDI) {
            إذا(state==SEEKING_STRONG_FOR_FSI) {
                إذا(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            إذا(ڈھیر_آخری>=0) {
                إذا(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=پچھلا_State_ڈھیر[ڈھیر_آخری];
                ڈھیر_آخری--;
            }
            استمر;
        }
        إذا(dirProp==B) {
            إذا(i<original_لمائی && uchar==CR && متن[i]==LF) /* do nothing on the CR */
                استمر;
            pBiDi->paras[pBiDi->para_گنتی-1].حد_بندی=i;
            إذا(isDefaultLevelInverse && آخری_Strong==R)
                pBiDi->paras[pBiDi->para_گنتی-1].level=1;
            إذا(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_گنتی=control_گنتی;
            }
            إذا(i<original_لمائی) {              /* B not last char in text */
                pBiDi->para_گنتی++;
                إذا(check_Para_گنتی(pBiDi)==خطأ)    /* not enough memory for a new para entry */
                    أرجع خطأ;
                إذا(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_گنتی-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    آخری_Strong=defaultParaLevel;
                } وإلا {
                    pBiDi->paras[pBiDi->para_گنتی-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                ڈھیر_آخری=-1;
            }
            استمر;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    إذا(ڈھیر_آخری>UBIDI_MAX_EXPLICIT_LEVEL) {
        ڈھیر_آخری=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    طالما(ڈھیر_آخری>=0) {
        إذا(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            اكسر;
        }
        state=پچھلا_State_ڈھیر[ڈھیر_آخری];
        ڈھیر_آخری--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        إذا(pBiDi->length<original_لمائی)
            pBiDi->para_گنتی--;
    } وإلا {
        pBiDi->paras[pBiDi->para_گنتی-1].حد_بندی=original_لمائی;
        pBiDi->control_گنتی=control_گنتی;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    إذا(isDefaultLevelInverse && آخری_Strong==R) {
        pBiDi->paras[pBiDi->para_گنتی-1].level=1;
    }
    إذا(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    لكل(i=0; i<pBiDi->para_گنتی; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    إذا(pBiDi->ترتیب_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->آخری_Arabic_Pos=آخری_Arabic_Pos;
    أرجع سچ;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_اشاریہ(ثابت UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    لكل(i=0; i<pBiDi->para_گنتی; i++)
        إذا(pindex<pBiDi->paras[i].حد_بندی)
            اكسر;
    إذا(i>=pBiDi->para_گنتی)
        i=pBiDi->para_گنتی-1;
    أرجع (UBiDiLevel)(pBiDi->paras[i].level);
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

ساكن فراغ
bracketInit(UBiDi *pBiDi, Bracket_انکڑے *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_لگاتار_آخری=0;
    bd->isoRuns[0].شروع=0;
    bd->isoRuns[0].حد_بندی=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].آخری_Strong = bd->isoRuns[0].آخری_ادھار = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    إذا(pBiDi->openings_حافظہ) {
        bd->openings=pBiDi->openings_حافظہ;
        bd->openings_گنتی=pBiDi->openings_ناپ / حجم(Opening);
    } وإلا {
        bd->openings=bd->simpleOpenings;
        bd->openings_گنتی=SIMPLE_OPENINGS_گنتی;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_منوال==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
ساكن فراغ
bracketProcessB(Bracket_انکڑے *bd, UBiDiLevel level) {
    bd->iso_لگاتار_آخری=0;
    bd->isoRuns[0].حد_بندی=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].آخری_Strong=bd->isoRuns[0].آخری_ادھار=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
ساكن فراغ
bracketProcessBoundary(Bracket_انکڑے *bd, int32_t آخری_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    DirProp *dirProps=bd->pBiDi->dirProps;
    إذا(DIRPROP_FLAG(dirProps[آخری_Cc_Pos])&نقاب_ISO)  /* after an isolate */
        أرجع;
    إذا(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_آخری_Iso_لگاتار->حد_بندی=p_آخری_Iso_لگاتار->شروع;
    p_آخری_Iso_لگاتار->level=embeddingLevel;
    p_آخری_Iso_لگاتار->آخری_Strong=p_آخری_Iso_لگاتار->آخری_ادھار=contextLevel&1;
    p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_آخری_Iso_لگاتار->contextPos = static_cast<UBiDiDirection>(آخری_Cc_Pos);
}

/* LRI or RLI */
ساكن فراغ
bracketProcessLRI_RLI(Bracket_انکڑے *bd, UBiDiLevel level) {
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    int16_t آخری_حد_بندی;
    p_آخری_Iso_لگاتار->آخری_ادھار=ON;
    آخری_حد_بندی=p_آخری_Iso_لگاتار->حد_بندی;
    bd->iso_لگاتار_آخری++;
    p_آخری_Iso_لگاتار++;
    p_آخری_Iso_لگاتار->شروع=p_آخری_Iso_لگاتار->حد_بندی=آخری_حد_بندی;
    p_آخری_Iso_لگاتار->level=level;
    p_آخری_Iso_لگاتار->آخری_Strong=p_آخری_Iso_لگاتار->آخری_ادھار=level&1;
    p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_آخری_Iso_لگاتار->contextPos=0;
}

/* PDI */
ساكن فراغ
bracketProcessPDI(Bracket_انکڑے *bd) {
    Iso_لگاتار *p_آخری_Iso_لگاتار;
    bd->iso_لگاتار_آخری--;
    p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    p_آخری_Iso_لگاتار->آخری_ادھار=ON;
}

/* newly found opening bracket: create an openings entry */
ساكن UBool                            /* return true if success */
bracket_جوڑو_Opening(Bracket_انکڑے *bd, char16_t میل, int32_t position) {
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    Opening *pOpening;
    إذا(p_آخری_Iso_لگاتار->حد_بندی>=bd->openings_گنتی) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        إذا(!get_Initial_Openings_حافظہ(pBiDi, p_آخری_Iso_لگاتار->حد_بندی * 2))
            أرجع خطأ;
        إذا(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_حافظہ, bd->simpleOpenings,
                        SIMPLE_OPENINGS_گنتی * حجم(Opening));
        bd->openings=pBiDi->openings_حافظہ;     /* may have changed */
        bd->openings_گنتی=pBiDi->openings_ناپ / حجم(Opening);
    }
    pOpening=&bd->openings[p_آخری_Iso_لگاتار->حد_بندی];
    pOpening->position=position;
    pOpening->میل=میل;
    pOpening->contextDir=p_آخری_Iso_لگاتار->contextDir;
    pOpening->contextPos=p_آخری_Iso_لگاتار->contextPos;
    pOpening->flags=0;
    p_آخری_Iso_لگاتار->حد_بندی++;
    أرجع سچ;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
ساكن فراغ
fixN0c(Bracket_انکڑے *bd, int32_t opening_اشاریہ, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    لكل(k=opening_اشاریہ+1, qOpening=&bd->openings[k]; k<p_آخری_Iso_لگاتار->حد_بندی; k++, qOpening++) {
        إذا(qOpening->میل>=0)      /* not an N0c match */
            استمر;
        إذا(newPropPosition<qOpening->contextPos)
            اكسر;
        إذا(newPropPosition>=qOpening->position)
            استمر;
        إذا(newProp==qOpening->contextDir)
            اكسر;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->میل);
        dirProps[closingPosition]=newProp;
        qOpening->میل=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
ساكن DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_انکڑے *bd, int32_t openIdx, int32_t position) {
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_آخری_Iso_لگاتار->level & 1);
    stable=سچ;            /* assume stable until proved otherwise */

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

    إذا((direction==0 && pOpening->flags&لبھیا_L) ||
       (direction==1 && pOpening->flags&لبھیا_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    وإلا إذا(pOpening->flags&(لبھیا_L|لبھیا_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_آخری_Iso_لگاتار->شروع);
        إذا(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        وإلا
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } وإلا {
        /* forget this and any brackets nested within this pair */
        p_آخری_Iso_لگاتار->حد_بندی= static_cast<uint16_t>(openIdx);
        أرجع ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    إذا(stable) {
        p_آخری_Iso_لگاتار->حد_بندی= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        طالما(p_آخری_Iso_لگاتار->حد_بندی>p_آخری_Iso_لگاتار->شروع &&
              bd->openings[p_آخری_Iso_لگاتار->حد_بندی-1].position==pOpening->position)
            p_آخری_Iso_لگاتار->حد_بندی--;
    } وإلا {
        int32_t k;
        pOpening->میل=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        طالما(k>=p_آخری_Iso_لگاتار->شروع &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].میل=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        لكل(k=openIdx+1; k<p_آخری_Iso_لگاتار->حد_بندی; k++) {
            qOpening=&bd->openings[k];
            إذا(qOpening->position>=position)
                اكسر;
            إذا(qOpening->میل>0)
                qOpening->میل=0;
        }
    }
    أرجع newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
ساكن UBool                            /* return true if success */
bracketProcessChar(Bracket_انکڑے *bd, int32_t position) {
    Iso_لگاتار *p_آخری_Iso_لگاتار=&bd->isoRuns[bd->iso_لگاتار_آخری];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    إذا(dirProp==ON) {
        char16_t c, میل;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->متن[position];
        لكل(idx=p_آخری_Iso_لگاتار->حد_بندی-1; idx>=p_آخری_Iso_لگاتار->شروع; idx--) {
            إذا(bd->openings[idx].میل!=c)
                استمر;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            إذا(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                اكسر;
            }
            p_آخری_Iso_لگاتار->آخری_ادھار=ON;
            p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(newProp);
            p_آخری_Iso_لگاتار->contextPos=position;
            level=bd->pBiDi->پدھراں[position];
            إذا(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_آخری_Iso_لگاتار->آخری_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                لكل(i=p_آخری_Iso_لگاتار->شروع; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->پدھراں[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->پدھراں[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            أرجع سچ;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        إذا(c)
            میل= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        وإلا
            میل=0;
        إذا(میل!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            إذا(میل==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                إذا(!bracket_جوڑو_Opening(bd, 0x3009, position))
                    أرجع خطأ;
            }
            وإلا إذا(میل==0x3009) {         /* RIGHT ANGLE BRACKET */
                إذا(!bracket_جوڑو_Opening(bd, 0x232A, position))
                    أرجع خطأ;
            }
            إذا(!bracket_جوڑو_Opening(bd, میل, position))
                أرجع خطأ;
        }
    }
    level=bd->pBiDi->پدھراں[position];
    إذا(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        إذا(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_آخری_Iso_لگاتار->آخری_ادھار=newProp;
        p_آخری_Iso_لگاتار->آخری_Strong=newProp;
        p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(newProp);
        p_آخری_Iso_لگاتار->contextPos=position;
    }
    وإلا إذا(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_آخری_Iso_لگاتار->آخری_ادھار=dirProp;
        p_آخری_Iso_لگاتار->آخری_Strong=dirProp;
        p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(newProp);
        p_آخری_Iso_لگاتار->contextPos=position;
    }
    وإلا إذا(dirProp==EN) {
        p_آخری_Iso_لگاتار->آخری_ادھار=EN;
        إذا(p_آخری_Iso_لگاتار->آخری_Strong==L) {
            newProp=L;                  /* W7 */
            إذا(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(L);
            p_آخری_Iso_لگاتار->contextPos=position;
        }
        وإلا {
            newProp=R;                  /* N0 */
            إذا(p_آخری_Iso_لگاتار->آخری_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            وإلا
                dirProps[position]=ENR;
            p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(R);
            p_آخری_Iso_لگاتار->contextPos=position;
        }
    }
    وإلا إذا(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_آخری_Iso_لگاتار->آخری_ادھار=AN;
        p_آخری_Iso_لگاتار->contextDir = static_cast<UBiDiDirection>(R);
        p_آخری_Iso_لگاتار->contextPos=position;
    }
    وإلا إذا(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_آخری_Iso_لگاتار->آخری_ادھار;
        إذا(newProp==ON)
            dirProps[position]=newProp;
    }
    وإلا {
        newProp=dirProp;
        p_آخری_Iso_لگاتار->آخری_ادھار=dirProp;
    }
    إذا(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        لكل(i=p_آخری_Iso_لگاتار->شروع; i<p_آخری_Iso_لگاتار->حد_بندی; i++)
            إذا(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    أرجع سچ;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
ساكن UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    إذا(!(flags&نقاب_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&نقاب_POSSIBLE_N)))) {
        أرجع UBIDI_LTR;
    } وإلا إذا(!(flags&نقاب_LTR)) {
        أرجع UBIDI_RTL;
    } وإلا {
        أرجع UBIDI_MIXED;
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
ساكن UBiDiDirection
resolve_Explicit_پدھراں(UBiDi *pBiDi, UError_کوڈ *p_Error_کوڈ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *پدھراں=pBiDi->پدھراں;
    ثابت char16_t *متن=pBiDi->متن;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_گنتی=0;

    إذا(U_FAILURE(*p_Error_کوڈ)) { أرجع UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    إذا((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        أرجع direction;
    }
    إذا(pBiDi->reordering_منوال > UBIDI_REORDER_آخری_LOGICAL_منزل_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_اشاریہ, شروع, حد_بندی;
        لكل(para_اشاریہ=0; para_اشاریہ<pBiDi->para_گنتی; para_اشاریہ++) {
            إذا(para_اشاریہ==0)
                شروع=0;
            وإلا
                شروع=pBiDi->paras[para_اشاریہ-1].حد_بندی;
            حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_اشاریہ].level);
            لكل(i=شروع; i<حد_بندی; i++)
                پدھراں[i]=level;
        }
        أرجع direction;   /* no bracket matching for inverse BiDi */
    }
    إذا(!(flags&(نقاب_EXPLICIT|نقاب_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_اشاریہ, شروع, حد_بندی;
        Bracket_انکڑے bracket_انکڑے;
        bracketInit(pBiDi, &bracket_انکڑے);
        لكل(para_اشاریہ=0; para_اشاریہ<pBiDi->para_گنتی; para_اشاریہ++) {
            إذا(para_اشاریہ==0)
                شروع=0;
            وإلا
                شروع=pBiDi->paras[para_اشاریہ-1].حد_بندی;
            حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_اشاریہ].level);
            لكل(i=شروع; i<حد_بندی; i++) {
                پدھراں[i]=level;
                dirProp=dirProps[i];
                إذا(dirProp==BN)
                    استمر;
                إذا(dirProp==B) {
                    إذا((i+1)<length) {
                        إذا(متن[i]==CR && متن[i+1]==LF)
                            استمر;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_انکڑے, level);
                    }
                    استمر;
                }
                إذا(!bracketProcessChar(&bracket_انکڑے, i)) {
                    *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                    أرجع UBIDI_LTR;
                }
            }
        }
        أرجع direction;
    }
    {
        /* continue to perform (Xn) */

        /* (X1) level is set for all codes, embeddingLevel keeps track of the push/pop operations */
        /* both variables may carry the UBIDI_LEVEL_OVERRIDE flag to indicate the override status */
        UBiDiLevel embeddingLevel=level, newLevel;
        UBiDiLevel پچھلا_Level=level;     /* previous level for regular (not CC) characters */
        int32_t آخری_Cc_Pos=0;                /* index of last effective LRx,RLx, PDx */

        /* The following stack remembers the embedding level and the ISOLATE flag of level runs.
           stackLast points to its current entry. */
        uint16_t ڈھیر_5[UBIDI_MAX_EXPLICIT_LEVEL+2];   /* we never push anything >=UBIDI_MAX_EXPLICIT_LEVEL
                                                        but we need one more entry as base */
        uint32_t ڈھیر_آخری=0;
        int32_t حد_توں_ودھ_Isolate_گنتی=0;
        int32_t حد_توں_ودھ_Embedding_گنتی=0;
        int32_t valid_Isolate_گنتی=0;
        Bracket_انکڑے bracket_انکڑے;
        bracketInit(pBiDi, &bracket_انکڑے);
        ڈھیر_5[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        لكل(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            بدل(dirProp) {
            حالة LRE:
            حالة RLE:
            حالة LRO:
            حالة RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                پدھراں[i]=پچھلا_Level;
                إذا (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                وإلا
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                إذا(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && حد_توں_ودھ_Isolate_گنتی==0 &&
                                                         حد_توں_ودھ_Embedding_گنتی==0) {
                    آخری_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    إذا(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    ڈھیر_آخری++;
                    ڈھیر_5[ڈھیر_آخری]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } وإلا {
                    إذا(حد_توں_ودھ_Isolate_گنتی==0)
                        حد_توں_ودھ_Embedding_گنتی++;
                }
                اكسر;
            حالة PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                پدھراں[i]=پچھلا_Level;
                /* handle all the overflow cases first */
                إذا(حد_توں_ودھ_Isolate_گنتی) {
                    اكسر;
                }
                إذا(حد_توں_ودھ_Embedding_گنتی) {
                    حد_توں_ودھ_Embedding_گنتی--;
                    اكسر;
                }
                إذا(ڈھیر_آخری>0 && ڈھیر_5[ڈھیر_آخری]<ISOLATE) {   /* not an isolate entry */
                    آخری_Cc_Pos=i;
                    ڈھیر_آخری--;
                    embeddingLevel = static_cast<UBiDiLevel>(ڈھیر_5[ڈھیر_آخری]);
                }
                اكسر;
            حالة LRI:
            حالة RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                پدھراں[i]=NO_OVERRIDE(embeddingLevel);
                إذا(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_انکڑے, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                پچھلا_Level=embeddingLevel;
                /* (X5a, X5b) */
                إذا(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                وإلا
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                إذا(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && حد_توں_ودھ_Isolate_گنتی==0 &&
                                                         حد_توں_ودھ_Embedding_گنتی==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    آخری_Cc_Pos=i;
                    valid_Isolate_گنتی++;
                    إذا(valid_Isolate_گنتی>pBiDi->isolate_گنتی)
                        pBiDi->isolate_گنتی=valid_Isolate_گنتی;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    ڈھیر_آخری++;
                    ڈھیر_5[ڈھیر_آخری]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_انکڑے, embeddingLevel);
                } وإلا {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    حد_توں_ودھ_Isolate_گنتی++;
                }
                اكسر;
            حالة PDI:
                إذا(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_انکڑے, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                إذا(حد_توں_ودھ_Isolate_گنتی) {
                    حد_توں_ودھ_Isolate_گنتی--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                وإلا إذا(valid_Isolate_گنتی) {
                    flags|=DIRPROP_FLAG(PDI);
                    آخری_Cc_Pos=i;
                    حد_توں_ودھ_Embedding_گنتی=0;
                    طالما(ڈھیر_5[ڈھیر_آخری]<ISOLATE) /* pop embedding entries */
                        ڈھیر_آخری--;                /* until the last isolate entry */
                    ڈھیر_آخری--;                    /* pop also the last isolate entry */
                    valid_Isolate_گنتی--;
                    bracketProcessPDI(&bracket_انکڑے);
                } وإلا
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(ڈھیر_5[ڈھیر_آخری]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                پچھلا_Level=embeddingLevel;
                پدھراں[i]=NO_OVERRIDE(embeddingLevel);
                اكسر;
            حالة B:
                flags|=DIRPROP_FLAG(B);
                پدھراں[i]=GET_PARALEVEL(pBiDi, i);
                إذا((i+1)<length) {
                    إذا(متن[i]==CR && متن[i+1]==LF)
                        اكسر;          /* skip CR when followed by LF */
                    حد_توں_ودھ_Embedding_گنتی=حد_توں_ودھ_Isolate_گنتی=0;
                    valid_Isolate_گنتی=0;
                    ڈھیر_آخری=0;
                    پچھلا_Level=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    ڈھیر_5[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_انکڑے, embeddingLevel);
                }
                اكسر;
            حالة BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                پدھراں[i]=پچھلا_Level;
                flags|=DIRPROP_FLAG(BN);
                اكسر;
            افتراضي:
                /* all other types are normal characters and get the "real" level */
                إذا(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_انکڑے, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    إذا(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    وإلا
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                پچھلا_Level=embeddingLevel;
                پدھراں[i]=embeddingLevel;
                إذا(!bracketProcessChar(&bracket_انکڑے, i))
                    أرجع static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                اكسر;
            }
        }
        إذا(flags&نقاب_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        إذا(pBiDi->ترتیب_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    أرجع direction;
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
ساكن UBiDiDirection
check_Explicit_پدھراں(UBiDi *pBiDi, UError_کوڈ *p_Error_کوڈ) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *پدھراں=pBiDi->پدھراں;
    int32_t isolate_گنتی=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_گنتی=0;

    int32_t current_Para_اشاریہ = 0;
    int32_t current_Para_حد_بندی = pBiDi->paras[0].حد_بندی;
    int32_t currentParaLevel = pBiDi->paraLevel;

    لكل(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=پدھراں[i];
        DirProp dirProp=dirProps[i];
        إذا(dirProp==LRI || dirProp==RLI) {
            isolate_گنتی++;
            إذا(isolate_گنتی>pBiDi->isolate_گنتی)
                pBiDi->isolate_گنتی=isolate_گنتی;
        }
        وإلا إذا(dirProp==PDI)
            isolate_گنتی--;
        وإلا إذا(dirProp==B)
            isolate_گنتی=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        إذا (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_حد_بندی && (current_Para_اشاریہ + 1) < pBiDi->para_گنتی) {
            currentParaLevel = pBiDi->paras[++current_Para_اشاریہ].level;
            current_Para_حد_بندی = pBiDi->paras[current_Para_اشاریہ].حد_بندی;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        إذا (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            إذا (level == 0) {
                إذا (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } وإلا {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    پدھراں[i] = level | overrideFlag;
                }
            } وإلا {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
                أرجع UBIDI_LTR;
            }
        }
        إذا (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } وإلا {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    إذا(flags&نقاب_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    أرجع directionFromFlags(pBiDi);
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
#تعریف_کرو IMPTABPROPS_COLUMNS 16
#تعریف_کرو IMPTABPROPS_RES (IMPTABPROPS_COLUMNS - 1)
#تعریف_کرو GET_STATEPROPS(cell) ((cell)&0x1f)
#تعریف_کرو GET_ACTIONPROPS(cell) ((cell)>>5)
#تعریف_کرو s(action, newState) ((uint8_t)(newState+(action<<5)))

ساكن ثابت uint8_t گروہ_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
تعداد { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

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
ساكن ثابت uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
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
#تعریف_رد s

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
#تعریف_کرو IMPTABLEVELS_COLUMNS (DirProp_B + 2)
#تعریف_کرو IMPTABLEVELS_RES (IMPTABLEVELS_COLUMNS - 1)
#تعریف_کرو GET_STATE(cell) ((cell)&0x0f)
#تعریف_کرو GET_ACTION(cell) ((cell)>>4)
#تعریف_کرو s(action, newState) ((uint8_t)(newState+(action<<4)))

تعريفنوع uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
تعريفنوع uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
تعريفنوع بنية ImpTabPair {
    ثابت فراغ * pImpTab[2];
    ثابت فراغ * pImpAct[2];
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

ساكن ثابت ImpTab impTabL_DEFAULT =   /* Even paragraph level */
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
ساكن ثابت ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
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
ساكن ثابت ImpAct impAct0 = {0,1,2,3,4};
ساكن ثابت ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

ساكن ثابت ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
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
ساكن ثابت ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

ساكن ثابت ImpTab imp_Tab_L_گروہ_NUMBERS_WITH_R =
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
ساكن ثابت ImpTab imp_Tab_R_گروہ_NUMBERS_WITH_R =
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
ساكن ثابت ImpTabPair imp_Tab_گروہ_NUMBERS_WITH_R = {
                        {&imp_Tab_L_گروہ_NUMBERS_WITH_R,
                         &imp_Tab_R_گروہ_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


ساكن ثابت ImpTab impTabL_INVERSE_NUMBERS_AS_L =
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
ساكن ثابت ImpTab impTabR_INVERSE_NUMBERS_AS_L =
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
ساكن ثابت ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

ساكن ثابت ImpTab imp_Tab_R_INVERSE_LIKE_سدھا =   /* Odd  paragraph level */
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
ساكن ثابت ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
ساكن ثابت ImpTabPair imp_Tab_INVERSE_LIKE_سدھا = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_سدھا},
                        {&impAct0, &impAct1}};

ساكن ثابت ImpTab imp_Tab_L_INVERSE_LIKE_سدھا_WITH_MARKS =
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
ساكن ثابت ImpTab imp_Tab_R_INVERSE_LIKE_سدھا_WITH_MARKS =
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
ساكن ثابت ImpAct impAct2 = {0,1,2,5,6,7,8};
ساكن ثابت ImpAct impAct3 = {0,1,9,10,11,12};
ساكن ثابت ImpTabPair imp_Tab_INVERSE_LIKE_سدھا_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_سدھا_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_سدھا_WITH_MARKS},
                        {&impAct2, &impAct3}};

ساكن ثابت ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_سدھا},
                        {&impAct0, &impAct1}};

ساكن ثابت ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
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
ساكن ثابت ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_سدھا_WITH_MARKS},
                        {&impAct2, &impAct3}};

#تعریف_رد s

تعريفنوع بنية {
    ثابت ImpTab * pImpTab;             /* level table pointer          */
    ثابت ImpAct * pImpAct;             /* action map array             */
    int32_t شروع_ON;                    /* start of ON sequence         */
    int32_t شروع_L2_EN;                  /* start of level 2 sequence    */
    int32_t آخری_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t لگاتار_شروع;                   /* start position of the run    */
    UBiDiLevel لگاتار_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

ساكن فراغ
جوڑو_نقطہ(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#تعریف_کرو FIRSTALLOC  10
    نقطہ نقطہ_2;
    داخل_کرو_Points_2 * p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);

    إذا (p_داخل_کرو_Points->capacity == 0)
    {
        p_داخل_کرو_Points->points=static_cast<نقطہ *>(uprv_malloc(حجم(نقطہ)*FIRSTALLOC));
        إذا (p_داخل_کرو_Points->points == خالی_سوچک)
        {
            p_داخل_کرو_Points->error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        p_داخل_کرو_Points->capacity=FIRSTALLOC;
    }
    إذا (p_داخل_کرو_Points->size >= p_داخل_کرو_Points->capacity) /* no room for new point */
    {
        نقطہ * savePoints=p_داخل_کرو_Points->points;
        p_داخل_کرو_Points->points=static_cast<نقطہ *>(uprv_realloc(p_داخل_کرو_Points->points,
                                           p_داخل_کرو_Points->capacity*2*حجم(نقطہ)));
        إذا (p_داخل_کرو_Points->points == خالی_سوچک)
        {
            p_داخل_کرو_Points->points=savePoints;
            p_داخل_کرو_Points->error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        وإلا  p_داخل_کرو_Points->capacity*=2;
    }
    نقطہ_2.pos=pos;
    نقطہ_2.flag=flag;
    p_داخل_کرو_Points->points[p_داخل_کرو_Points->size]=نقطہ_2;
    p_داخل_کرو_Points->size++;
#تعریف_رد FIRSTALLOC
}

ساكن فراغ
سیٹ_پدھراں_Outside_Isolates(UBiDi *pBiDi, int32_t شروع, int32_t حد_بندی, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *پدھراں=pBiDi->پدھراں;
    int32_t isolate_گنتی=0, k;
    لكل(k=شروع; k<حد_بندی; k++) {
        dirProp=dirProps[k];
        إذا(dirProp==PDI)
            isolate_گنتی--;
        إذا(isolate_گنتی==0)
            پدھراں[k]=level;
        إذا(dirProp==LRI || dirProp==RLI)
            isolate_گنتی++;
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

ساكن فراغ
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t شروع, int32_t حد_بندی) {
    uint8_t cell, oldStateSeq, actionSeq;
    ثابت ImpTab * pImpTab=pLevState->pImpTab;
    ثابت ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * پدھراں=pBiDi->پدھراں;
    UBiDiLevel level, جوڑو_Level;
    داخل_کرو_Points_2 * p_داخل_کرو_Points;
    int32_t start0, k;

    start0=شروع;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    جوڑو_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    إذا(actionSeq) {
        بدل(actionSeq) {
        حالة 1:                         /* init ON seq */
            pLevState->شروع_ON=start0;
            اكسر;

        حالة 2:                         /* prepend ON seq to current seq */
            شروع=pLevState->شروع_ON;
            اكسر;

        حالة 3:                         /* EN/AN after R+ON */
            level=pLevState->لگاتار_Level+1;
            سیٹ_پدھراں_Outside_Isolates(pBiDi, pLevState->شروع_ON, start0, level);
            اكسر;

        حالة 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->لگاتار_Level+2;
            سیٹ_پدھراں_Outside_Isolates(pBiDi, pLevState->شروع_ON, start0, level);
            اكسر;

        حالة 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            إذا (pLevState->شروع_L2_EN >= 0) {
                جوڑو_نقطہ(pBiDi, pLevState->شروع_L2_EN, LRM_BEFORE);
            }
            pLevState->شروع_L2_EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            إذا ((p_داخل_کرو_Points->capacity == 0) ||
                (p_داخل_کرو_Points->size <= p_داخل_کرو_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->آخری_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                إذا ((level & 1) && (pLevState->شروع_ON > 0)) {  /* after ON */
                    شروع=pLevState->شروع_ON;   /* reset to basic run level */
                }
                إذا (_prop == DirProp_S)                /* add LRM before S */
                {
                    جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                    p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
                }
                اكسر;
            }
            /* reset previous RTL cont to level for LTR text */
            لكل (k=pLevState->آخری_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                پدھراں[k]=(پدھراں[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            pLevState->آخری_Strong_RTL=-1;
            إذا (_prop == DirProp_S)            /* add LRM before S */
            {
                جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            }
            اكسر;

        حالة 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            إذا (p_داخل_کرو_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_داخل_کرو_Points->size=p_داخل_کرو_Points->confirmed;
            pLevState->شروع_ON=-1;
            pLevState->شروع_L2_EN=-1;
            pLevState->آخری_Strong_RTL=حد_بندی - 1;
            اكسر;

        حالة 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            إذا ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_منوال!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                إذا (pLevState->شروع_L2_EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->آخری_Strong_RTL=حد_بندی - 1;
                    اكسر;
                }
                إذا (pLevState->شروع_L2_EN >= 0)  /* after EN, no AN */
                {
                    جوڑو_نقطہ(pBiDi, pLevState->شروع_L2_EN, LRM_BEFORE);
                    pLevState->شروع_L2_EN=-2;
                }
                /* note AN */
                جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                اكسر;
            }
            /* if first EN/AN after R/AL */
            إذا (pLevState->شروع_L2_EN == -1) {
                pLevState->شروع_L2_EN=start0;
            }
            اكسر;

        حالة 8:                         /* note location of latest R/AL */
            pLevState->آخری_Strong_RTL=حد_بندی - 1;
            pLevState->شروع_ON=-1;
            اكسر;

        حالة 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            لكل (k=start0-1; k>=0 && !(پدھراں[k]&1); k--);
            إذا(k>=0) {
                جوڑو_نقطہ(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;   /* confirm it */
            }
            pLevState->شروع_ON=start0;
            اكسر;

        حالة 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            جوڑو_نقطہ(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            اكسر;

        حالة 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            p_داخل_کرو_Points->size=p_داخل_کرو_Points->confirmed;
            إذا (_prop == DirProp_S)            /* add RLM before S */
            {
                جوڑو_نقطہ(pBiDi, start0, RLM_BEFORE);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            }
            اكسر;

        حالة 12:                        /* L after L+ON/AN */
            level=pLevState->لگاتار_Level + جوڑو_Level;
            لكل(k=pLevState->شروع_ON; k<start0; k++) {
                إذا (پدھراں[k]<level)
                    پدھراں[k]=level;
            }
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;   /* confirm inserts */
            pLevState->شروع_ON=start0;
            اكسر;

        حالة 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->لگاتار_Level;
            لكل(k=start0-1; k>=pLevState->شروع_ON; k--) {
                إذا(پدھراں[k]==level+3) {
                    طالما(پدھراں[k]==level+3) {
                        پدھراں[k--]-=2;
                    }
                    طالما(پدھراں[k]==level) {
                        k--;
                    }
                }
                إذا(پدھراں[k]==level+2) {
                    پدھراں[k]=level;
                    استمر;
                }
                پدھراں[k]=level+1;
            }
            اكسر;

        حالة 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->لگاتار_Level+1;
            لكل(k=start0-1; k>=pLevState->شروع_ON; k--) {
                إذا(پدھراں[k]>level) {
                    پدھراں[k]-=2;
                }
            }
            اكسر;

        افتراضي:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    إذا((جوڑو_Level) || (شروع < start0)) {
        level=pLevState->لگاتار_Level + جوڑو_Level;
        إذا(شروع>=pLevState->لگاتار_شروع) {
            لكل(k=شروع; k<حد_بندی; k++) {
                پدھراں[k]=level;
            }
        } وإلا {
            سیٹ_پدھراں_Outside_Isolates(pBiDi, شروع, حد_بندی, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
ساكن DirProp
آخری_L_R_AL(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->prologue;
    int32_t length=pBiDi->pro_لمائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    لكل(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_PREV(متن, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        إذا(dirProp==L) {
            أرجع DirProp_L;
        }
        إذا(dirProp==R || dirProp==AL) {
            أرجع DirProp_R;
        }
        إذا(dirProp==B) {
            أرجع DirProp_ON;
        }
    }
    أرجع DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
ساكن DirProp
پہلا_L_R_AL_EN_AN(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->epilogue;
    int32_t length=pBiDi->epi_لمائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    لكل(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        إذا(dirProp==L) {
            أرجع DirProp_L;
        }
        إذا(dirProp==R || dirProp==AL) {
            أرجع DirProp_R;
        }
        إذا(dirProp==EN) {
            أرجع DirProp_EN;
        }
        إذا(dirProp==AN) {
            أرجع DirProp_AN;
        }
    }
    أرجع DirProp_ON;
}

ساكن فراغ
resolve_Implicit_پدھراں(UBiDi *pBiDi,
                      int32_t شروع, int32_t حد_بندی,
                      DirProp sor, DirProp eor) {
    ثابت DirProp *dirProps=pBiDi->dirProps;
    DirProp dirProp;
    LevState levState;
    int32_t i, start1, start2;
    uint16_t oldStateImp, stateImp, actionImp;
    uint8_t gprop, resProp, cell;
    UBool inverseRTL;
    DirProp اگلا_Strong_Prop=R;
    int32_t اگلا_Strong_Pos=-1;

    /* check for RTL inverse BiDi mode */
    /* FOOD FOR THOUGHT: in case of RTL inverse BiDi, it would make sense to
     * loop on the text characters from end to start.
     * This would need a different properties state table (at least different
     * actions) and different levels state tables (maybe very similar to the
     * LTR corresponding ones.
     */
    inverseRTL =
        static_cast<UBool>((شروع < pBiDi->آخری_Arabic_Pos) && (GET_PARALEVEL(pBiDi, شروع) & 1) &&
                           (pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_LIKE_سدھا ||
                            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.شروع_L2_EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.آخری_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.لگاتار_شروع=شروع;
    levState.لگاتار_Level=pBiDi->پدھراں[شروع];
    levState.pImpTab = static_cast<ثابت ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.لگاتار_Level & 1]);
    levState.pImpAct = static_cast<ثابت ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.لگاتار_Level & 1]);
    إذا(شروع==0 && pBiDi->pro_لمائی>0) {
        DirProp آخری_Strong=آخری_L_R_AL(pBiDi);
        إذا(آخری_Strong!=DirProp_ON) {
            sor=آخری_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    إذا(dirProps[شروع]==PDI  && pBiDi->isolate_گنتی >= 0) {
        levState.شروع_ON=pBiDi->isolates[pBiDi->isolate_گنتی].شروع_ON;
        start1=pBiDi->isolates[pBiDi->isolate_گنتی].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_گنتی].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_گنتی].state;
        pBiDi->isolate_گنتی--;
    } وإلا {
        levState.شروع_ON=-1;
        start1=شروع;
        إذا(dirProps[شروع]==NSM)
            stateImp = 1 + sor;
        وإلا
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, شروع, شروع);
    }
    start2=شروع;                       /* to make Java compiler happy */

    لكل(i=شروع; i<=حد_بندی; i++) {
        إذا(i>=حد_بندی) {
            int32_t k;
            لكل(k=حد_بندی-1; k>شروع&&(DIRPROP_FLAG(dirProps[k])&نقاب_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            إذا(dirProp==LRI || dirProp==RLI)
                اكسر;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } وإلا {
            DirProp prop, prop1;
            prop=dirProps[i];
            إذا(prop==B) {
                pBiDi->isolate_گنتی=-1; /* current isolates stack entry == none */
            }
            إذا(inverseRTL) {
                إذا(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } وإلا إذا(prop==EN) {
                    إذا(اگلا_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        اگلا_Strong_Prop=R;   /* set default */
                        اگلا_Strong_Pos=حد_بندی;
                        لكل(j=i+1; j<حد_بندی; j++) {
                            prop1=dirProps[j];
                            إذا(prop1==L || prop1==R || prop1==AL) {
                                اگلا_Strong_Prop=prop1;
                                اگلا_Strong_Pos=j;
                                اكسر;
                            }
                        }
                    }
                    إذا(اگلا_Strong_Prop==AL) {
                        prop=AN;
                    }
                }
            }
            gprop=گروہ_Prop[prop];
        }
        oldStateImp=stateImp;
        cell=impTabProps[oldStateImp][gprop];
        stateImp=GET_STATEPROPS(cell);      /* isolate the new state */
        actionImp=GET_ACTIONPROPS(cell);    /* isolate the action */
        إذا((i==حد_بندی) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        إذا(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            بدل(actionImp) {
            حالة 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                اكسر;
            حالة 2:             /* init new seq2 */
                start2=i;
                اكسر;
            حالة 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                اكسر;
            حالة 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                اكسر;
            افتراضي:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    إذا(حد_بندی==pBiDi->length && pBiDi->epi_لمائی>0) {
        DirProp پہلا_Strong=پہلا_L_R_AL_EN_AN(pBiDi);
        إذا(پہلا_Strong!=DirProp_ON) {
            eor=پہلا_Strong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    لكل(i=حد_بندی-1; i>شروع&&(DIRPROP_FLAG(dirProps[i])&نقاب_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    إذا((dirProp==LRI || dirProp==RLI) && حد_بندی<pBiDi->length) {
        pBiDi->isolate_گنتی++;
        pBiDi->isolates[pBiDi->isolate_گنتی].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_گنتی].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_گنتی].start1=start1;
        pBiDi->isolates[pBiDi->isolate_گنتی].شروع_ON=levState.شروع_ON;
    }
    وإلا
        processPropertySeq(pBiDi, &levState, eor, حد_بندی, حد_بندی);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
ساكن فراغ
adjustWSLevels(UBiDi *pBiDi) {
    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *پدھراں=pBiDi->پدھراں;
    int32_t i;

    إذا(pBiDi->flags&نقاب_WS) {
        UBool ترتیب_Paragraphs_LTR=pBiDi->ترتیب_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        طالما(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            طالما(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&نقاب_WS) {
                إذا(ترتیب_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    پدھراں[i]=0;
                } وإلا {
                    پدھراں[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            طالما(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                إذا(flag&نقاب_BN_EXPLICIT) {
                    پدھراں[i]=پدھراں[i+1];
                } وإلا إذا(ترتیب_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    پدھراں[i]=0;
                    اكسر;
                } وإلا إذا(flag&نقاب_B_S) {
                    پدھراں[i]=GET_PARALEVEL(pBiDi, i);
                    اكسر;
                }
            }
        }
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Context(UBiDi *pBiDi,
                 ثابت char16_t *prologue, int32_t pro_لمائی,
                 ثابت char16_t *epilogue, int32_t epi_لمائی,
                 UError_کوڈ *p_Error_کوڈ) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    إذا(pBiDi==خالی_سوچک || pro_لمائی<-1 || epi_لمائی<-1 ||
       (prologue==خالی_سوچک && pro_لمائی!=0) || (epilogue==خالی_سوچک && epi_لمائی!=0)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(pro_لمائی==-1) {
        pBiDi->pro_لمائی=u_strlen(prologue);
    } وإلا {
        pBiDi->pro_لمائی=pro_لمائی;
    }
    إذا(epi_لمائی==-1) {
        pBiDi->epi_لمائی=u_strlen(epilogue);
    } وإلا {
        pBiDi->epi_لمائی=epi_لمائی;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

ساكن فراغ
سیٹ_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_لمائی=0;                 /* forget the last context */
    pBiDi->epi_لمائی=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#تعریف_کرو BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#تعریف_کرو BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

ساكن فراغ
سیٹ_Para_Runs_Only(UBiDi *pBiDi, ثابت char16_t *متن, int32_t length,
                UBiDiLevel paraLevel, UError_کوڈ *p_Error_کوڈ) {
    int32_t *runs_Only_حافظہ = خالی_سوچک;
    int32_t *visualMap;
    char16_t *visual_متن;
    int32_t save_لمائی, saveTrailingWSStart;
    ثابت UBiDiLevel *پدھراں;
    UBiDiLevel *save_پدھراں;
    UBiDiDirection saveDirection;
    UBool save_May_Allocate_متن;
    لگاتار *runs;
    int32_t visual_لمائی, i, j, visual_شروع, logical_شروع,
            لگاتار_گنتی, لگاتار_لمائی, addedRuns, داخل_کرو_Remove,
            شروع, حد_بندی, قدم, اشاریہ_Odd_ثنائی_ہندسہ, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_منوال=UBIDI_REORDER_DEFAULT;
    إذا(length==0) {
        ubidi_سیٹ_Para(pBiDi, متن, length, paraLevel, خالی_سوچک, p_Error_کوڈ);
        اذهب cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_حافظہ=static_cast<int32_t *>(uprv_malloc(length*(حجم(int32_t)+حجم(char16_t)+حجم(UBiDiLevel))));
    إذا(runs_Only_حافظہ==خالی_سوچک) {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        اذهب cleanup3;
    }
    visualMap=runs_Only_حافظہ;
    visual_متن = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_پدھراں = reinterpret_cast<UBiDiLevel*>(&visual_متن[length]);
    saveOptions=pBiDi->reorderingOptions;
    إذا(saveOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_داخل_کرو_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_سیٹ_Para(pBiDi, متن, length, paraLevel, خالی_سوچک, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        اذهب cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    پدھراں=ubidi_get_پدھراں(pBiDi, p_Error_کوڈ);
    uprv_memcpy(save_پدھراں, پدھراں, (size_t)pBiDi->length*حجم(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_لمائی=pBiDi->length;
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
    visual_لمائی=ubidi_writeReordered(pBiDi, visual_متن, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_اندرونی_RUNS_ONLY, p_Error_کوڈ);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        اذهب cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_منوال=UBIDI_REORDER_INVERSE_LIKE_سدھا;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    save_May_Allocate_متن=pBiDi->may_Allocate_متن;
    pBiDi->may_Allocate_متن=خطأ;
    ubidi_سیٹ_Para(pBiDi, visual_متن, visual_لمائی, paraLevel, خالی_سوچک, p_Error_کوڈ);
    pBiDi->may_Allocate_متن=save_May_Allocate_متن;
    ubidi_getRuns(pBiDi, p_Error_کوڈ);
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        اذهب cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    لگاتار_گنتی=pBiDi->لگاتار_گنتی;
    runs=pBiDi->runs;
    visual_شروع=0;
    لكل(i=0; i<لگاتار_گنتی; i++, visual_شروع+=لگاتار_لمائی) {
        لگاتار_لمائی=runs[i].visual_حد_بندی-visual_شروع;
        إذا(لگاتار_لمائی<2) {
            استمر;
        }
        logical_شروع=GET_اشاریہ(runs[i].logical_شروع);
        لكل(j=logical_شروع+1; j<logical_شروع+لگاتار_لمائی; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            إذا((BIDI_ABS(index0-index1)!=1) || (save_پدھراں[index0]!=save_پدھراں[index1])) {
                addedRuns++;
            }
        }
    }
    إذا(addedRuns) {
        إذا(get_Runs_حافظہ(pBiDi, لگاتار_گنتی+addedRuns)) {
            إذا(لگاتار_گنتی==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_حافظہ[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_حافظہ;
            pBiDi->لگاتار_گنتی+=addedRuns;
        } وإلا {
            *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
            اذهب cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    لكل(i=لگاتار_گنتی-1; i>=0; i--) {
        لگاتار_لمائی= i==0 ? runs[0].visual_حد_بندی :
                          runs[i].visual_حد_بندی-runs[i-1].visual_حد_بندی;
        logical_شروع=runs[i].logical_شروع;
        اشاریہ_Odd_ثنائی_ہندسہ=GET_ODD_ثنائی_ہندسہ(logical_شروع);
        logical_شروع=GET_اشاریہ(logical_شروع);
        إذا(لگاتار_لمائی<2) {
            إذا(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logical_شروع];
            runs[i+addedRuns].logical_شروع=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_پدھراں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
            استمر;
        }
        إذا(اشاریہ_Odd_ثنائی_ہندسہ) {
            شروع=logical_شروع;
            حد_بندی=logical_شروع+لگاتار_لمائی-1;
            قدم=1;
        } وإلا {
            شروع=logical_شروع+لگاتار_لمائی-1;
            حد_بندی=logical_شروع;
            قدم=-1;
        }
        لكل(j=شروع; j!=حد_بندی; j+=قدم) {
            index0=visualMap[j];
            index1=visualMap[j+قدم];
            إذا((BIDI_ABS(index0-index1)!=1) || (save_پدھراں[index0]!=save_پدھراں[index1])) {
                logicalPos=BIDI_MIN(visualMap[شروع], index0);
                runs[i+addedRuns].logical_شروع=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_پدھراں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
                runs[i+addedRuns].visual_حد_بندی=runs[i].visual_حد_بندی;
                runs[i].visual_حد_بندی-=BIDI_ABS(j-شروع)+1;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].داخل_کرو_Remove=داخل_کرو_Remove;
                runs[i].داخل_کرو_Remove&=~داخل_کرو_Remove;
                شروع=j+قدم;
                addedRuns--;
            }
        }
        إذا(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[شروع], visualMap[حد_بندی]);
        runs[i+addedRuns].logical_شروع=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_پدھراں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->متن=متن;
    pBiDi->length=save_لمائی;
    pBiDi->original_لمائی=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    إذا(save_لمائی>pBiDi->پدھراں_ناپ) {
        save_لمائی=pBiDi->پدھراں_ناپ;
    }
    uprv_memcpy(pBiDi->پدھراں, save_پدھراں, (size_t)save_لمائی*حجم(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    إذا(pBiDi->لگاتار_گنتی>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_حافظہ);

    pBiDi->reordering_منوال=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Para(UBiDi *pBiDi, ثابت char16_t *متن, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_پدھراں,
              UError_کوڈ *p_Error_کوڈ) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    إذا(pBiDi==خالی_سوچک || متن==خالی_سوچک || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا(length==-1) {
        length=u_strlen(متن);
    }

    /* special treatment for RUNS_ONLY mode */
    إذا(pBiDi->reordering_منوال==UBIDI_REORDER_RUNS_ONLY) {
        سیٹ_Para_Runs_Only(pBiDi, متن, length, paraLevel, p_Error_کوڈ);
        أرجع;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=خالی_سوچک;          /* mark unfinished setPara */
    pBiDi->متن=متن;
    pBiDi->length=pBiDi->original_لمائی=pBiDi->نتیجہ_لمائی=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_گنتی=1;

    pBiDi->dirProps=خالی_سوچک;
    pBiDi->پدھراں=خالی_سوچک;
    pBiDi->runs=خالی_سوچک;
    pBiDi->داخل_کرو_Points.size=0;         /* clean up from last call */
    pBiDi->داخل_کرو_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    إذا(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        إذا(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->لگاتار_گنتی=0;
        pBiDi->para_گنتی=0;
        سیٹ_Para_Success(pBiDi);          /* mark successful setPara */
        أرجع;
    }

    pBiDi->لگاتار_گنتی=-1;

    /* allocate paras memory */
    إذا(pBiDi->paras_حافظہ)
        pBiDi->paras=pBiDi->paras_حافظہ;
    وإلا
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    إذا(get_Dir_Props_حافظہ(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_حافظہ;
        إذا(!getDirProps(pBiDi)) {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
    } وإلا {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    إذا(embedding_پدھراں==خالی_سوچک) {
        /* no: determine explicit levels according to the (Xn) rules */\
        إذا(get_پدھراں_حافظہ(pBiDi, length)) {
            pBiDi->پدھراں=pBiDi->پدھراں_حافظہ;
            direction=resolve_Explicit_پدھراں(pBiDi, p_Error_کوڈ);
            إذا(U_FAILURE(*p_Error_کوڈ)) {
                أرجع;
            }
        } وإلا {
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
    } وإلا {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->پدھراں=embedding_پدھراں;
        direction=check_Explicit_پدھراں(pBiDi, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع;
        }
    }

    /* allocate isolate memory */
    إذا(pBiDi->isolate_گنتی<=SIMPLE_ISOLATES_گنتی)
        pBiDi->isolates=pBiDi->simpleIsolates;
    وإلا
        إذا((int32_t)(pBiDi->isolate_گنتی*حجم(Isolate))<=pBiDi->isolates_ناپ)
            pBiDi->isolates=pBiDi->isolates_حافظہ;
        وإلا {
            إذا(get_Initial_Isolates_حافظہ(pBiDi, pBiDi->isolate_گنتی)) {
                pBiDi->isolates=pBiDi->isolates_حافظہ;
            } وإلا {
                *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
        }
    pBiDi->isolate_گنتی=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    بدل(direction) {
    حالة UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        اكسر;
    حالة UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        اكسر;
    افتراضي:
        /*
         *  Choose the right implicit state table
         */
        بدل(pBiDi->reordering_منوال) {
        حالة UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            اكسر;
        حالة UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            اكسر;
        حالة UBIDI_REORDER_گروہ_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_گروہ_NUMBERS_WITH_R;
            اكسر;
        حالة UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            اكسر;
        حالة UBIDI_REORDER_INVERSE_LIKE_سدھا:
            إذا (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_سدھا_WITH_MARKS;
            } وإلا {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_سدھا;
            }
            اكسر;
        حالة UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            إذا (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } وإلا {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            اكسر;
        افتراضي:
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
        إذا(embedding_پدھراں==خالی_سوچک && pBiDi->para_گنتی<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_پدھراں(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } وإلا {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *پدھراں=pBiDi->پدھراں;
            int32_t شروع, حد_بندی=0;
            UBiDiLevel level, اگلا_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            اگلا_Level=پدھراں[0];
            إذا(level<اگلا_Level) {
                eor=GET_LR_FROM_LEVEL(اگلا_Level);
            } وإلا {
                eor=GET_LR_FROM_LEVEL(level);
            }

            افعل {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                شروع=حد_بندی;
                level=اگلا_Level;
                إذا((شروع>0) && (dirProps[شروع-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, شروع));
                } وإلا {
                    sor=eor;
                }

                /* search for the limit of this run */
                طالما((++حد_بندی<length) &&
                      ((پدھراں[حد_بندی]==level) ||
                       (DIRPROP_FLAG(dirProps[حد_بندی])&نقاب_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                إذا(حد_بندی<length) {
                    اگلا_Level=پدھراں[حد_بندی];
                } وإلا {
                    اگلا_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                إذا(NO_OVERRIDE(level)<NO_OVERRIDE(اگلا_Level)) {
                    eor=GET_LR_FROM_LEVEL(اگلا_Level);
                } وإلا {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                إذا(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_پدھراں(pBiDi, شروع, حد_بندی, sor, eor);
                } وإلا {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    افعل {
                        پدھراں[شروع++]&=~UBIDI_LEVEL_OVERRIDE;
                    } طالما(شروع<حد_بندی);
                }
            } طالما(حد_بندی<length);
        }
        /* check if we got any memory shortage while adding insert points */
        إذا (U_FAILURE(pBiDi->داخل_کرو_Points.error_کوڈ))
        {
            *p_Error_کوڈ=pBiDi->داخل_کرو_Points.error_کوڈ;
            أرجع;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        اكسر;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    إذا((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) &&
       ((pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_LIKE_سدھا) ||
        (pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, شروع, آخری;
        UBiDiLevel level;
        DirProp dirProp;
        لكل(i=0; i<pBiDi->para_گنتی; i++) {
            آخری=(pBiDi->paras[i].حد_بندی)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            إذا(level==0)
                استمر;           /* LTR paragraph */
            شروع= i==0 ? 0 : pBiDi->paras[i-1].حد_بندی;
            لكل(j=آخری; j>=شروع; j--) {
                dirProp=dirProps[j];
                إذا(dirProp==L) {
                    إذا(j<آخری) {
                        طالما(dirProps[آخری]==B) {
                            آخری--;
                        }
                    }
                    جوڑو_نقطہ(pBiDi, آخری, RLM_BEFORE);
                    اكسر;
                }
                إذا(DIRPROP_FLAG(dirProp) & نقاب_R_AL) {
                    اكسر;
                }
            }
        }
    }

    إذا(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->نتیجہ_لمائی -= pBiDi->control_گنتی;
    } وإلا {
        pBiDi->نتیجہ_لمائی += pBiDi->داخل_کرو_Points.size;
    }
    سیٹ_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI فراغ U_EXPORT2
ubidi_ترتیب_Paragraphs_LTR(UBiDi *pBiDi, UBool ترتیب_Paragraphs_LTR) {
    إذا(pBiDi!=خالی_سوچک) {
        pBiDi->ترتیب_Paragraphs_LTR=ترتیب_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_ترتیب_Paragraphs_LTR(UBiDi *pBiDi) {
    إذا(pBiDi!=خالی_سوچک) {
        أرجع pBiDi->ترتیب_Paragraphs_LTR;
    } وإلا {
        أرجع خطأ;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(ثابت UBiDi *pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->direction;
    } وإلا {
        أرجع UBIDI_LTR;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
ubidi_get_متن(ثابت UBiDi* pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->متن;
    } وإلا {
        أرجع خالی_سوچک;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_لمائی(ثابت UBiDi *pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->original_لمائی;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_لمائی(ثابت UBiDi *pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->length;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_نتیجہ_لمائی(ثابت UBiDi *pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->نتیجہ_لمائی;
    } وإلا {
        أرجع 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(ثابت UBiDi *pBiDi) {
    إذا(IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع pBiDi->paraLevel;
    } وإلا {
        أرجع 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_گنتی_Paragraphs(UBiDi *pBiDi) {
    إذا(!IS_VALID_PARA_OR_سطر(pBiDi)) {
        أرجع 0;
    } وإلا {
        أرجع pBiDi->para_گنتی;
    }
}

U_CAPI فراغ U_EXPORT2
ubidi_get_Paragraph_By_اشاریہ(ثابت UBiDi *pBiDi, int32_t para_اشاریہ,
                          int32_t *p_Para_شروع, int32_t *p_Para_حد_بندی,
                          UBiDiLevel *pParaLevel, UError_کوڈ *p_Error_کوڈ) {
    int32_t para_شروع;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ);
    RETURN_VOID_IF_BAD_RANGE(para_اشاریہ, 0, pBiDi->para_گنتی, *p_Error_کوڈ);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    إذا(para_اشاریہ) {
        para_شروع=pBiDi->paras[para_اشاریہ-1].حد_بندی;
    } وإلا {
        para_شروع=0;
    }
    إذا(p_Para_شروع!=خالی_سوچک) {
        *p_Para_شروع=para_شروع;
    }
    إذا(p_Para_حد_بندی!=خالی_سوچک) {
        *p_Para_حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
    }
    إذا(pParaLevel!=خالی_سوچک) {
        *pParaLevel=GET_PARALEVEL(pBiDi, para_شروع);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(ثابت UBiDi *pBiDi, int32_t char_اشاریہ,
                          int32_t *p_Para_شروع, int32_t *p_Para_حد_بندی,
                          UBiDiLevel *pParaLevel, UError_کوڈ *p_Error_کوڈ) {
    int32_t para_اشاریہ;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_کوڈ, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_اشاریہ, 0, pBiDi->length, *p_Error_کوڈ, -1);

    لكل(para_اشاریہ=0; char_اشاریہ>=pBiDi->paras[para_اشاریہ].حد_بندی; para_اشاریہ++);
    ubidi_get_Paragraph_By_اشاریہ(pBiDi, para_اشاریہ, p_Para_شروع, p_Para_حد_بندی, pParaLevel, p_Error_کوڈ);
    أرجع para_اشاریہ;
}

U_CAPI فراغ U_EXPORT2
ubidi_سیٹ_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       ثابت فراغ *newContext, UBiDiClassCallback **oldFn,
                       ثابت فراغ **oldContext, UError_کوڈ *p_Error_کوڈ)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_کوڈ);
    إذا(pBiDi==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }
    إذا( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    إذا( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI فراغ U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, ثابت فراغ **context)
{
    إذا(pBiDi==خالی_سوچک) {
        أرجع;
    }
    إذا( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    إذا( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    إذا( pBiDi->fnClassCallback == خالی_سوچک ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    إذا(dir >= U_CHAR_DIRECTION_گنتی) {
        dir = (UCharDirection)ON;
    }
    أرجع dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#إذا U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(ثابت UnicodeString& s) {
    إذا (s == u"ltr") {
        أرجع U_MF_BIDI_OPTION_LTR;
    }
    إذا (s == u"rtl") {
        أرجع U_MF_BIDI_OPTION_RTL;
    }
    إذا (s == u"auto") {
        أرجع U_MF_BIDI_OPTION_AUTO;
    }
    أرجع U_MF_BIDI_OPTION_INHERIT; // inherit is default
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
    إذا (bidiIsolationStrategy == U_MF_BIDI_OFF)
        أرجع &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    منطقي isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    بدل (dir) {
        حالة U_MF_DIRECTIONALITY_LTR:
            إذا (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                أرجع &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            إذا (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } وإلا {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            اكسر; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        حالة U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            إذا (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } وإلا {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            اكسر; // End of 2iv.
        // 2v. Else:
        افتراضي:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            إذا (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } وإلا {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            اكسر; // End of 2v
    } // `fmt` now contains the isolated string
    أرجع &fmt;
}

U_NAMESPACE_سرا

#جے_انت // #if SHOW_CPLUSPLUS_API

