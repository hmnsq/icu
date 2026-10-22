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

#شامل <limits>

#شامل "cmemory.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uchar.h"
#شامل "unicode/ubidi.h"
#شامل "unicode/utf16.h"
#شامل "ubidi_props.h"
#شامل "ubidiimp.h"
#شامل "uassert.h"

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
ساکن ثابت Flags flagLR[2]={ DIRPROP_FLAG(L), DIRPROP_FLAG(R) };
ساکن ثابت Flags flagE[2]={ DIRPROP_FLAG(LRE), DIRPROP_FLAG(RLE) };
ساکن ثابت Flags flagO[2]={ DIRPROP_FLAG(LRO), DIRPROP_FLAG(RLO) };

#تعریف_کرو DIRPROP_FLAG_LR(level) flagLR[(level)&1]
#تعریف_کرو DIRPROP_FLAG_E(level)  flagE[(level)&1]
#تعریف_کرو DIRPROP_FLAG_O(level)  flagO[(level)&1]

#تعریف_کرو DIR_FROM_STRONG(strong) ((strong)==L ? L : R)

#تعریف_کرو NO_OVERRIDE(level)  ((level)&~UBIDI_LEVEL_OVERRIDE)

/* UBiDi object management -------------------------------------------------- */

U_CAPI UBiDi * U_EXPORT2
ubidi_open()
{
    UError_رمز error_رمز=U_ZERO_ERROR;
    واپس ubidi_openSized(0, 0, &error_رمز);
}

U_CAPI UBiDi * U_EXPORT2
ubidi_openSized(int32_t max_لمبائی_2, int32_t max_تسلسل_گنتی, UError_رمز *p_Error_رمز) {
    UBiDi *pBiDi;

    /* check the argument values */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    } ورنہ اگر(max_لمبائی_2<0 || max_تسلسل_گنتی<0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;    /* invalid arguments */
    }

    /* allocate memory for the object */
    pBiDi=(UBiDi *)uprv_malloc(حجم(UBiDi));
    اگر(pBiDi==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    /* reset the object, all pointers nullptr, all flags false, all sizes 0 */
    uprv_memset(pBiDi, 0, حجم(UBiDi));

    /* allocate memory for arrays as requested */
    اگر(max_لمبائی_2>0) {
        اگر( !get_Initial_Dir_Props_حافظہ(pBiDi, max_لمبائی_2) ||
            !get_Initial_سطحیں_حافظہ(pBiDi, max_لمبائی_2)
        ) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        }
    } ورنہ {
        pBiDi->may_Allocate_متن=سچ;
    }

    اگر(max_تسلسل_گنتی>0) {
        اگر(max_تسلسل_گنتی==1) {
            /* use simpleRuns[] */
            pBiDi->runs_سائز=حجم(تسلسل);
        } ورنہ اگر(!get_Initial_Runs_حافظہ(pBiDi, max_تسلسل_گنتی)) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        }
    } ورنہ {
        pBiDi->mayAllocateRuns=سچ;
    }

    اگر(U_SUCCESS(*p_Error_رمز)) {
        واپس pBiDi;
    } ورنہ {
        ubidi_close(pBiDi);
        واپس خالی_اشاریہ;
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
ubidi_get_حافظہ(Bidi_حافظہ_For_تخصیص *bidiMem, int32_t *p_سائز, UBool mayAllocate, size_t سائز_Needed) {
    اگر (سائز_Needed > std::numeric_limits<int32_t>::max()) {
      // TODO(egg): Ugly guard for ICU-23397.  A cleaner fix would be to change the pSize to size_t*
      // and update callers throughout to use size_t for sizes (just like uprv_malloc already does).
      // Not that anyone should be running the UBA on text with that many runs…
      واپس جھوٹ;
    }
    خالی **p_حافظہ = (خالی **)bidiMem;
    /* check for existing memory */
    اگر(*p_حافظہ==خالی_اشاریہ) {
        /* we need to allocate memory */
        اگر(mayAllocate && (*p_حافظہ=uprv_malloc(سائز_Needed))!=خالی_اشاریہ) {
            *p_سائز=static_cast<int32_t>(سائز_Needed);
            واپس سچ;
        } ورنہ {
            واپس جھوٹ;
        }
    } ورنہ {
        اگر(سائز_Needed<=static_cast<size_t>(*p_سائز)) {
            /* there is already enough memory */
            واپس سچ;
        }
        ورنہ اگر(!mayAllocate) {
            /* not enough memory, and we must not allocate */
            واپس جھوٹ;
        } ورنہ {
            /* we try to grow */
            خالی *حافظہ;
            /* in most cases, we do not need the copy-old-data part of
             * realloc, but it is needed when adding runs using getRunsMemory()
             * in setParaRunsOnly()
             */
            اگر((حافظہ=uprv_realloc(*p_حافظہ, سائز_Needed))!=خالی_اشاریہ) {
                *p_حافظہ=حافظہ;
                *p_سائز=static_cast<int32_t>(سائز_Needed);
                واپس سچ;
            } ورنہ {
                /* we failed to grow */
                واپس جھوٹ;
            }
        }
    }
}

U_CAPI خالی U_EXPORT2
ubidi_close(UBiDi *pBiDi) {
    اگر(pBiDi!=خالی_اشاریہ) {
        pBiDi->pParaBiDi=خالی_اشاریہ;          /* in case one tries to reuse this block */
        اگر(pBiDi->dir_Props_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->dir_Props_حافظہ);
        }
        اگر(pBiDi->سطحیں_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->سطحیں_حافظہ);
        }
        اگر(pBiDi->openings_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->openings_حافظہ);
        }
        اگر(pBiDi->paras_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->paras_حافظہ);
        }
        اگر(pBiDi->runs_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->runs_حافظہ);
        }
        اگر(pBiDi->isolates_حافظہ!=خالی_اشاریہ) {
            uprv_free(pBiDi->isolates_حافظہ);
        }
        اگر(pBiDi->داخل_کرو_Points.points!=خالی_اشاریہ) {
            uprv_free(pBiDi->داخل_کرو_Points.points);
        }

        uprv_free(pBiDi);
    }
}

/* set to approximate "inverse BiDi" ---------------------------------------- */

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Inverse(UBiDi *pBiDi, UBool isInverse) {
    اگر(pBiDi!=خالی_اشاریہ) {
        pBiDi->isInverse=isInverse;
        pBiDi->reordering_منوال = isInverse ? UBIDI_REORDER_INVERSE_NUMBERS_AS_L
                                          : UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_isInverse(UBiDi *pBiDi) {
    اگر(pBiDi!=خالی_اشاریہ) {
        واپس pBiDi->isInverse;
    } ورنہ {
        واپس جھوٹ;
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
U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Reordering_منوال(UBiDi *pBiDi, UBi_Di_Reordering_منوال reordering_منوال) UPRV_NO_SANITIZE_UNDEFINED {
    اگر ((pBiDi!=خالی_اشاریہ) && (reordering_منوال >= UBIDI_REORDER_DEFAULT)
                        && (reordering_منوال < UBIDI_REORDER_گنتی)) {
        pBiDi->reordering_منوال = reordering_منوال;
        pBiDi->isInverse = reordering_منوال == UBIDI_REORDER_INVERSE_NUMBERS_AS_L;
    }
}

U_CAPI UBi_Di_Reordering_منوال U_EXPORT2
ubidi_get_Reordering_منوال(UBiDi *pBiDi) {
    اگر (pBiDi!=خالی_اشاریہ) {
        واپس pBiDi->reordering_منوال;
    } ورنہ {
        واپس UBIDI_REORDER_DEFAULT;
    }
}

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Reordering_Options(UBiDi *pBiDi, uint32_t reorderingOptions) {
    اگر (reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        reorderingOptions&=~UBIDI_OPTION_داخل_کرو_MARKS;
    }
    اگر (pBiDi!=خالی_اشاریہ) {
        pBiDi->reorderingOptions=reorderingOptions;
    }
}

U_CAPI uint32_t U_EXPORT2
ubidi_getReorderingOptions(UBiDi *pBiDi) {
    اگر (pBiDi!=خالی_اشاریہ) {
        واپس pBiDi->reorderingOptions;
    } ورنہ {
        واپس 0;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_get_بنیاد_Direction(ثابت char16_t *متن,
int32_t length){

    int32_t i;
    UChar32 uchar;
    UCharDirection dir;

    اگر( متن==خالی_اشاریہ || length<-1 ){
        واپس UBIDI_NEUTRAL;
    }

    اگر(length==-1) {
        length=u_strlen(متن);
    }

    برائے( i = 0 ; i < length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dir = u_charDirection(uchar);
        اگر( dir == U_بایاں_مقصد_دایاں )
                واپس UBIDI_LTR;
        اگر( dir == U_دایاں_مقصد_بایاں || dir ==U_دایاں_مقصد_بایاں_ARABIC )
                واپس UBIDI_RTL;
    }
    واپس UBIDI_NEUTRAL;
}

/* perform (P2)..(P3) ------------------------------------------------------- */

/**
 * Returns the directionality of the first strong character
 * after the last B in prologue, if any.
 * Requires prologue!=null.
 */
ساکن DirProp
پہلا_L_R_AL(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->prologue;
    int32_t length=pBiDi->pro_لمبائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp, نتیجہ=ON;
    برائے(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        اگر(نتیجہ==ON) {
            اگر(dirProp==L || dirProp==R || dirProp==AL) {
                نتیجہ=dirProp;
            }
        } ورنہ {
            اگر(dirProp==B) {
                نتیجہ=ON;
            }
        }
    }
    واپس نتیجہ;
}

/*
 * Check that there are enough entries in the array pointed to by pBiDi->paras
 */
ساکن UBool
check_Para_گنتی(UBiDi *pBiDi) {
    int32_t count=pBiDi->para_گنتی;
    اگر(pBiDi->paras==pBiDi->simpleParas) {
        اگر(count<=SIMPLE_PARAS_گنتی)
            واپس سچ;
        اگر(!get_Initial_Paras_حافظہ(pBiDi, SIMPLE_PARAS_گنتی * 2))
            واپس جھوٹ;
        pBiDi->paras=pBiDi->paras_حافظہ;
        uprv_memcpy(pBiDi->paras_حافظہ, pBiDi->simpleParas, SIMPLE_PARAS_گنتی * حجم(Para));
        واپس سچ;
    }
    اگر(!get_Initial_Paras_حافظہ(pBiDi, count * 2))
        واپس جھوٹ;
    pBiDi->paras=pBiDi->paras_حافظہ;
    واپس سچ;
}

/*
 * Get the directional properties for the text, calculate the flags bit-set, and
 * determine the paragraph level if necessary (in pBiDi->paras[i].level).
 * FSI initiators are also resolved and their dirProp replaced with LRI or RLI.
 * When encountering an FSI, it is initially replaced with an LRI, which is the
 * default. Only if a strong R or AL is found within its scope will the LRI be
 * replaced by an RLI.
 */
ساکن UBool
getDirProps(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->متن;
    DirProp *dirProps=pBiDi->dir_Props_حافظہ;    /* pBiDi->dirProps is const */

    int32_t i=0, original_لمبائی=pBiDi->original_لمبائی;
    Flags flags=0;      /* collect all directionalities in the text */
    UChar32 uchar;
    DirProp dirProp=0, defaultParaLevel=0;  /* initialize to avoid compiler warnings */
    UBool isDefaultLevel=IS_DEFAULT_LEVEL(pBiDi->paraLevel);
    /* for inverse BiDi, the default para level is set to RTL if there is a
       strong R or AL character at either end of the text                            */
    UBool isDefaultLevelInverse = isDefaultLevel && static_cast<UBool>(
            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_LIKE_براہ_راست ||
            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL);
    int32_t آخری_Arabic_Pos=-1;
    int32_t control_گنتی=0;
    UBool removeBiDiControls =
        static_cast<UBool>(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS);

    شمار State {
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
    int32_t isolate_آغاز_ڈھیر[UBIDI_MAX_EXPLICIT_LEVEL+1];
    /* The following stack contains the last known state before
       encountering the initiator of an isolate sequence */
    State  پچھلا_State_ڈھیر[UBIDI_MAX_EXPLICIT_LEVEL+1];
    int32_t ڈھیر_آخری=-1;

    اگر(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING)
        pBiDi->length=0;
    defaultParaLevel=pBiDi->paraLevel&1;
    اگر(isDefaultLevel) {
        pBiDi->paras[0].level=defaultParaLevel;
        آخری_Strong=defaultParaLevel;
        اگر(pBiDi->pro_لمبائی>0 &&                    /* there is a prologue */
           (dirProp=پہلا_L_R_AL(pBiDi))!=ON) {  /* with a strong character */
            اگر(dirProp==L)
                pBiDi->paras[0].level=0;    /* set the default para level */
            ورنہ
                pBiDi->paras[0].level=1;    /* set the default para level */
            state=NOT_SEEKING_STRONG;
        } ورنہ {
            state=SEEKING_STRONG_FOR_PARA;
        }
    } ورنہ {
        pBiDi->paras[0].level=pBiDi->paraLevel;
        state=NOT_SEEKING_STRONG;
    }
    /* count paragraphs and determine the paragraph level (P2..P3) */
    /*
     * see comment in ubidi.h:
     * the UBIDI_DEFAULT_XXX values are designed so that
     * their bit 0 alone yields the intended default
     */
    برائے( /* i=0 above */ ; i<original_لمبائی; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, original_لمبائی, uchar);
        flags|=DIRPROP_FLAG(dirProp=(DirProp)ubidi_getCustomizedClass(pBiDi, uchar));
        dirProps[i-1]=dirProp;
        اگر(uchar>0xffff) {  /* set the lead surrogate's property to BN */
            flags|=DIRPROP_FLAG(BN);
            dirProps[i-2]=BN;
        }
        اگر(removeBiDiControls && IS_BIDI_CONTROL_CHAR(uchar))
            control_گنتی++;
        اگر(dirProp==L) {
            اگر(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_گنتی-1].level=0;
                state=NOT_SEEKING_STRONG;
            }
            ورنہ اگر(state==SEEKING_STRONG_FOR_FSI) {
                اگر(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
                state=LOOKING_FOR_PDI;
            }
            آخری_Strong=L;
            جاری;
        }
        اگر(dirProp==R || dirProp==AL) {
            اگر(state==SEEKING_STRONG_FOR_PARA) {
                pBiDi->paras[pBiDi->para_گنتی-1].level=1;
                state=NOT_SEEKING_STRONG;
            }
            ورنہ اگر(state==SEEKING_STRONG_FOR_FSI) {
                اگر(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    dirProps[isolate_آغاز_ڈھیر[ڈھیر_آخری]]=RLI;
                    flags|=DIRPROP_FLAG(RLI);
                }
                state=LOOKING_FOR_PDI;
            }
            آخری_Strong=R;
            اگر(dirProp==AL)
                آخری_Arabic_Pos=i-1;
            جاری;
        }
        اگر(dirProp>=FSI && dirProp<=RLI) {  /* FSI, LRI or RLI */
            ڈھیر_آخری++;
            اگر(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                isolate_آغاز_ڈھیر[ڈھیر_آخری]=i-1;
                پچھلا_State_ڈھیر[ڈھیر_آخری]=state;
            }
            اگر(dirProp==FSI) {
                dirProps[i-1]=LRI;      /* default if no strong char */
                state=SEEKING_STRONG_FOR_FSI;
            }
            ورنہ
                state=LOOKING_FOR_PDI;
            جاری;
        }
        اگر(dirProp==PDI) {
            اگر(state==SEEKING_STRONG_FOR_FSI) {
                اگر(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL) {
                    /* no need for next statement, already set by default */
                    /* dirProps[isolateStartStack[stackLast]]=LRI; */
                    flags|=DIRPROP_FLAG(LRI);
                }
            }
            اگر(ڈھیر_آخری>=0) {
                اگر(ڈھیر_آخری<=UBIDI_MAX_EXPLICIT_LEVEL)
                    state=پچھلا_State_ڈھیر[ڈھیر_آخری];
                ڈھیر_آخری--;
            }
            جاری;
        }
        اگر(dirProp==B) {
            اگر(i<original_لمبائی && uchar==CR && متن[i]==LF) /* do nothing on the CR */
                جاری;
            pBiDi->paras[pBiDi->para_گنتی-1].حد_بندی=i;
            اگر(isDefaultLevelInverse && آخری_Strong==R)
                pBiDi->paras[pBiDi->para_گنتی-1].level=1;
            اگر(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
                /* When streaming, we only process whole paragraphs
                   thus some updates are only done on paragraph boundaries */
                pBiDi->length=i;        /* i is index to next character */
                pBiDi->control_گنتی=control_گنتی;
            }
            اگر(i<original_لمبائی) {              /* B not last char in text */
                pBiDi->para_گنتی++;
                اگر(check_Para_گنتی(pBiDi)==جھوٹ)    /* not enough memory for a new para entry */
                    واپس جھوٹ;
                اگر(isDefaultLevel) {
                    pBiDi->paras[pBiDi->para_گنتی-1].level=defaultParaLevel;
                    state=SEEKING_STRONG_FOR_PARA;
                    آخری_Strong=defaultParaLevel;
                } ورنہ {
                    pBiDi->paras[pBiDi->para_گنتی-1].level=pBiDi->paraLevel;
                    state=NOT_SEEKING_STRONG;
                }
                ڈھیر_آخری=-1;
            }
            جاری;
        }
    }
    /* Ignore still open isolate sequences with overflow */
    اگر(ڈھیر_آخری>UBIDI_MAX_EXPLICIT_LEVEL) {
        ڈھیر_آخری=UBIDI_MAX_EXPLICIT_LEVEL;
        state=SEEKING_STRONG_FOR_FSI;   /* to be on the safe side */
    }
    /* Resolve direction of still unresolved open FSI sequences */
    جب(ڈھیر_آخری>=0) {
        اگر(state==SEEKING_STRONG_FOR_FSI) {
            /* no need for next statement, already set by default */
            /* dirProps[isolateStartStack[stackLast]]=LRI; */
            flags|=DIRPROP_FLAG(LRI);
            توڑو;
        }
        state=پچھلا_State_ڈھیر[ڈھیر_آخری];
        ڈھیر_آخری--;
    }
    /* When streaming, ignore text after the last paragraph separator */
    اگر(pBiDi->reorderingOptions & UBIDI_OPTION_STREAMING) {
        اگر(pBiDi->length<original_لمبائی)
            pBiDi->para_گنتی--;
    } ورنہ {
        pBiDi->paras[pBiDi->para_گنتی-1].حد_بندی=original_لمبائی;
        pBiDi->control_گنتی=control_گنتی;
    }
    /* For inverse bidi, default para direction is RTL if there is
       a strong R or AL at either end of the paragraph */
    اگر(isDefaultLevelInverse && آخری_Strong==R) {
        pBiDi->paras[pBiDi->para_گنتی-1].level=1;
    }
    اگر(isDefaultLevel) {
        pBiDi->paraLevel=static_cast<UBiDiLevel>(pBiDi->paras[0].level);
    }
    /* The following is needed to resolve the text direction for default level
       paragraphs containing no strong character */
    برائے(i=0; i<pBiDi->para_گنتی; i++)
        flags|=DIRPROP_FLAG_LR(pBiDi->paras[i].level);

    اگر(pBiDi->ترتیب_Paragraphs_LTR && (flags&DIRPROP_FLAG(B))) {
        flags|=DIRPROP_FLAG(L);
    }
    pBiDi->flags=flags;
    pBiDi->آخری_Arabic_Pos=آخری_Arabic_Pos;
    واپس سچ;
}

/* determine the paragraph level at position index */
U_CFUNC UBiDiLevel
ubidi_get_Para_Level_At_اشاریہ(ثابت UBiDi *pBiDi, int32_t pindex) {
    int32_t i;
    برائے(i=0; i<pBiDi->para_گنتی; i++)
        اگر(pindex<pBiDi->paras[i].حد_بندی)
            توڑو;
    اگر(i>=pBiDi->para_گنتی)
        i=pBiDi->para_گنتی-1;
    واپس (UBiDiLevel)(pBiDi->paras[i].level);
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

ساکن خالی
bracketInit(UBiDi *pBiDi, Bracket_ڈیٹا *bd) {
    bd->pBiDi=pBiDi;
    bd->iso_تسلسل_آخری=0;
    bd->isoRuns[0].آغاز=0;
    bd->isoRuns[0].حد_بندی=0;
    bd->isoRuns[0].level=GET_PARALEVEL(pBiDi, 0);
    UBiDiLevel t = GET_PARALEVEL(pBiDi, 0) & 1;
    bd->isoRuns[0].آخری_Strong = bd->isoRuns[0].آخری_بنیاد = t;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(t);
    bd->isoRuns[0].contextPos=0;
    اگر(pBiDi->openings_حافظہ) {
        bd->openings=pBiDi->openings_حافظہ;
        bd->openings_گنتی=pBiDi->openings_سائز / حجم(Opening);
    } ورنہ {
        bd->openings=bd->simpleOpenings;
        bd->openings_گنتی=SIMPLE_OPENINGS_گنتی;
    }
    bd->isNumbersSpecial=bd->pBiDi->reordering_منوال==UBIDI_REORDER_NUMBERS_SPECIAL ||
                         bd->pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL;
}

/* paragraph boundary */
ساکن خالی
bracketProcessB(Bracket_ڈیٹا *bd, UBiDiLevel level) {
    bd->iso_تسلسل_آخری=0;
    bd->isoRuns[0].حد_بندی=0;
    bd->isoRuns[0].level=level;
    bd->isoRuns[0].آخری_Strong=bd->isoRuns[0].آخری_بنیاد=level&1;
    bd->isoRuns[0].contextDir = static_cast<UBiDiDirection>(level & 1);
    bd->isoRuns[0].contextPos=0;
}

/* LRE, LRO, RLE, RLO, PDF */
ساکن خالی
bracketProcessBoundary(Bracket_ڈیٹا *bd, int32_t آخری_Cc_Pos,
                       UBiDiLevel contextLevel, UBiDiLevel embeddingLevel) {
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    DirProp *dirProps=bd->pBiDi->dirProps;
    اگر(DIRPROP_FLAG(dirProps[آخری_Cc_Pos])&نقاب_ISO)  /* after an isolate */
        واپس;
    اگر(NO_OVERRIDE(embeddingLevel)>NO_OVERRIDE(contextLevel))   /* not a PDF */
        contextLevel=embeddingLevel;
    p_آخری_Iso_تسلسل->حد_بندی=p_آخری_Iso_تسلسل->آغاز;
    p_آخری_Iso_تسلسل->level=embeddingLevel;
    p_آخری_Iso_تسلسل->آخری_Strong=p_آخری_Iso_تسلسل->آخری_بنیاد=contextLevel&1;
    p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(contextLevel & 1);
    p_آخری_Iso_تسلسل->contextPos = static_cast<UBiDiDirection>(آخری_Cc_Pos);
}

/* LRI or RLI */
ساکن خالی
bracketProcessLRI_RLI(Bracket_ڈیٹا *bd, UBiDiLevel level) {
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    int16_t آخری_حد_بندی;
    p_آخری_Iso_تسلسل->آخری_بنیاد=ON;
    آخری_حد_بندی=p_آخری_Iso_تسلسل->حد_بندی;
    bd->iso_تسلسل_آخری++;
    p_آخری_Iso_تسلسل++;
    p_آخری_Iso_تسلسل->آغاز=p_آخری_Iso_تسلسل->حد_بندی=آخری_حد_بندی;
    p_آخری_Iso_تسلسل->level=level;
    p_آخری_Iso_تسلسل->آخری_Strong=p_آخری_Iso_تسلسل->آخری_بنیاد=level&1;
    p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(level & 1);
    p_آخری_Iso_تسلسل->contextPos=0;
}

/* PDI */
ساکن خالی
bracketProcessPDI(Bracket_ڈیٹا *bd) {
    Iso_تسلسل *p_آخری_Iso_تسلسل;
    bd->iso_تسلسل_آخری--;
    p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    p_آخری_Iso_تسلسل->آخری_بنیاد=ON;
}

/* newly found opening bracket: create an openings entry */
ساکن UBool                            /* return true if success */
bracket_جوڑو_Opening(Bracket_ڈیٹا *bd, char16_t مطابقت, int32_t position) {
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    Opening *pOpening;
    اگر(p_آخری_Iso_تسلسل->حد_بندی>=bd->openings_گنتی) {  /* no available new entry */
        UBiDi *pBiDi=bd->pBiDi;
        اگر(!get_Initial_Openings_حافظہ(pBiDi, p_آخری_Iso_تسلسل->حد_بندی * 2))
            واپس جھوٹ;
        اگر(bd->openings==bd->simpleOpenings)
            uprv_memcpy(pBiDi->openings_حافظہ, bd->simpleOpenings,
                        SIMPLE_OPENINGS_گنتی * حجم(Opening));
        bd->openings=pBiDi->openings_حافظہ;     /* may have changed */
        bd->openings_گنتی=pBiDi->openings_سائز / حجم(Opening);
    }
    pOpening=&bd->openings[p_آخری_Iso_تسلسل->حد_بندی];
    pOpening->position=position;
    pOpening->مطابقت=مطابقت;
    pOpening->contextDir=p_آخری_Iso_تسلسل->contextDir;
    pOpening->contextPos=p_آخری_Iso_تسلسل->contextPos;
    pOpening->flags=0;
    p_آخری_Iso_تسلسل->حد_بندی++;
    واپس سچ;
}

/* change N0c1 to N0c2 when a preceding bracket is assigned the embedding level */
ساکن خالی
fixN0c(Bracket_ڈیٹا *bd, int32_t opening_اشاریہ, int32_t newPropPosition, DirProp newProp) {
    /* This function calls itself recursively */
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    Opening *qOpening;
    DirProp *dirProps=bd->pBiDi->dirProps;
    int32_t k, openingPosition, closingPosition;
    برائے(k=opening_اشاریہ+1, qOpening=&bd->openings[k]; k<p_آخری_Iso_تسلسل->حد_بندی; k++, qOpening++) {
        اگر(qOpening->مطابقت>=0)      /* not an N0c match */
            جاری;
        اگر(newPropPosition<qOpening->contextPos)
            توڑو;
        اگر(newPropPosition>=qOpening->position)
            جاری;
        اگر(newProp==qOpening->contextDir)
            توڑو;
        openingPosition=qOpening->position;
        dirProps[openingPosition]=newProp;
        closingPosition=-(qOpening->مطابقت);
        dirProps[closingPosition]=newProp;
        qOpening->مطابقت=0;                      /* prevent further changes */
        fixN0c(bd, k, openingPosition, newProp);
        fixN0c(bd, k, closingPosition, newProp);
    }
}

/* process closing bracket */
ساکن DirProp              /* return L or R if N0b or N0c, ON if N0d */
bracketProcessClosing(Bracket_ڈیٹا *bd, int32_t openIdx, int32_t position) {
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    Opening *pOpening, *qOpening;
    UBiDiDirection direction;
    UBool stable;
    DirProp newProp;
    pOpening=&bd->openings[openIdx];
    direction = static_cast<UBiDiDirection>(p_آخری_Iso_تسلسل->level & 1);
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

    اگر((direction==0 && pOpening->flags&ملا_L) ||
       (direction==1 && pOpening->flags&ملا_R)) {                         /* N0b */
        newProp=static_cast<DirProp>(direction);
    }
    ورنہ اگر(pOpening->flags&(ملا_L|ملا_R)) {                            /* N0c */
        /* it is stable if there is no containing pair or in
           conditions too complicated and not worth checking */
        stable=(openIdx==p_آخری_Iso_تسلسل->آغاز);
        اگر(direction!=pOpening->contextDir)
            newProp= static_cast<DirProp>(pOpening->contextDir);           /* N0c1 */
        ورنہ
            newProp= static_cast<DirProp>(direction);                      /* N0c2 */
    } ورنہ {
        /* forget this and any brackets nested within this pair */
        p_آخری_Iso_تسلسل->حد_بندی= static_cast<uint16_t>(openIdx);
        واپس ON;                                                          /* N0d */
    }
    bd->pBiDi->dirProps[pOpening->position]=newProp;
    bd->pBiDi->dirProps[position]=newProp;
    /* Update nested N0c pairs that may be affected */
    fixN0c(bd, openIdx, pOpening->position, newProp);
    اگر(stable) {
        p_آخری_Iso_تسلسل->حد_بندی= static_cast<uint16_t>(openIdx); /* forget any brackets nested within this pair */
        /* remove lower located synonyms if any */
        جب(p_آخری_Iso_تسلسل->حد_بندی>p_آخری_Iso_تسلسل->آغاز &&
              bd->openings[p_آخری_Iso_تسلسل->حد_بندی-1].position==pOpening->position)
            p_آخری_Iso_تسلسل->حد_بندی--;
    } ورنہ {
        int32_t k;
        pOpening->مطابقت=-position;
        /* neutralize lower located synonyms if any */
        k=openIdx-1;
        جب(k>=p_آخری_Iso_تسلسل->آغاز &&
              bd->openings[k].position==pOpening->position)
            bd->openings[k--].مطابقت=0;
        /* neutralize any unmatched opening between the current pair;
           this will also neutralize higher located synonyms if any */
        برائے(k=openIdx+1; k<p_آخری_Iso_تسلسل->حد_بندی; k++) {
            qOpening=&bd->openings[k];
            اگر(qOpening->position>=position)
                توڑو;
            اگر(qOpening->مطابقت>0)
                qOpening->مطابقت=0;
        }
    }
    واپس newProp;
}

/* handle strong characters, digits and candidates for closing brackets */
ساکن UBool                            /* return true if success */
bracketProcessChar(Bracket_ڈیٹا *bd, int32_t position) {
    Iso_تسلسل *p_آخری_Iso_تسلسل=&bd->isoRuns[bd->iso_تسلسل_آخری];
    DirProp *dirProps, dirProp, newProp;
    UBiDiLevel level;
    dirProps=bd->pBiDi->dirProps;
    dirProp=dirProps[position];
    اگر(dirProp==ON) {
        char16_t c, مطابقت;
        int32_t idx;
        /* First see if it is a matching closing bracket. Hopefully, this is
           more efficient than checking if it is a closing bracket at all */
        c=bd->pBiDi->متن[position];
        برائے(idx=p_آخری_Iso_تسلسل->حد_بندی-1; idx>=p_آخری_Iso_تسلسل->آغاز; idx--) {
            اگر(bd->openings[idx].مطابقت!=c)
                جاری;
            /* We have a match */
            newProp=bracketProcessClosing(bd, idx, position);
            اگر(newProp==ON) {           /* N0d */
                c=0;        /* prevent handling as an opening */
                توڑو;
            }
            p_آخری_Iso_تسلسل->آخری_بنیاد=ON;
            p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(newProp);
            p_آخری_Iso_تسلسل->contextPos=position;
            level=bd->pBiDi->سطحیں[position];
            اگر(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
                uint16_t flag;
                int32_t i;
                newProp=level&1;
                p_آخری_Iso_تسلسل->آخری_Strong=newProp;
                flag=DIRPROP_FLAG(newProp);
                برائے(i=p_آخری_Iso_تسلسل->آغاز; i<idx; i++)
                    bd->openings[i].flags|=flag;
                /* matching brackets are not overridden by LRO/RLO */
                bd->pBiDi->سطحیں[position]&=~UBIDI_LEVEL_OVERRIDE;
            }
            /* matching brackets are not overridden by LRO/RLO */
            bd->pBiDi->سطحیں[bd->openings[idx].position]&=~UBIDI_LEVEL_OVERRIDE;
            واپس سچ;
        }
        /* We get here only if the ON character is not a matching closing
           bracket or it is a case of N0d */
        /* Now see if it is an opening bracket */
        اگر(c)
            مطابقت= static_cast<char16_t>(u_getBidiPairedBracket(c));    /* get the matching char */
        ورنہ
            مطابقت=0;
        اگر(مطابقت!=c &&                  /* has a matching char */
           ubidi_getPairedBracketType(c)==U_BPT_OPEN) { /* opening bracket */
            /* special case: process synonyms
               create an opening entry for each synonym */
            اگر(مطابقت==0x232A) {     /* RIGHT-POINTING ANGLE BRACKET */
                اگر(!bracket_جوڑو_Opening(bd, 0x3009, position))
                    واپس جھوٹ;
            }
            ورنہ اگر(مطابقت==0x3009) {         /* RIGHT ANGLE BRACKET */
                اگر(!bracket_جوڑو_Opening(bd, 0x232A, position))
                    واپس جھوٹ;
            }
            اگر(!bracket_جوڑو_Opening(bd, مطابقت, position))
                واپس جھوٹ;
        }
    }
    level=bd->pBiDi->سطحیں[position];
    اگر(level&UBIDI_LEVEL_OVERRIDE) {    /* X4, X5 */
        newProp=level&1;
        اگر(dirProp!=S && dirProp!=WS && dirProp!=ON)
            dirProps[position]=newProp;
        p_آخری_Iso_تسلسل->آخری_بنیاد=newProp;
        p_آخری_Iso_تسلسل->آخری_Strong=newProp;
        p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(newProp);
        p_آخری_Iso_تسلسل->contextPos=position;
    }
    ورنہ اگر(dirProp<=R || dirProp==AL) {
        newProp= static_cast<DirProp>(DIR_FROM_STRONG(dirProp));
        p_آخری_Iso_تسلسل->آخری_بنیاد=dirProp;
        p_آخری_Iso_تسلسل->آخری_Strong=dirProp;
        p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(newProp);
        p_آخری_Iso_تسلسل->contextPos=position;
    }
    ورنہ اگر(dirProp==EN) {
        p_آخری_Iso_تسلسل->آخری_بنیاد=EN;
        اگر(p_آخری_Iso_تسلسل->آخری_Strong==L) {
            newProp=L;                  /* W7 */
            اگر(!bd->isNumbersSpecial)
                dirProps[position]=ENL;
            p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(L);
            p_آخری_Iso_تسلسل->contextPos=position;
        }
        ورنہ {
            newProp=R;                  /* N0 */
            اگر(p_آخری_Iso_تسلسل->آخری_Strong==AL)
                dirProps[position]=AN;  /* W2 */
            ورنہ
                dirProps[position]=ENR;
            p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(R);
            p_آخری_Iso_تسلسل->contextPos=position;
        }
    }
    ورنہ اگر(dirProp==AN) {
        newProp=R;                      /* N0 */
        p_آخری_Iso_تسلسل->آخری_بنیاد=AN;
        p_آخری_Iso_تسلسل->contextDir = static_cast<UBiDiDirection>(R);
        p_آخری_Iso_تسلسل->contextPos=position;
    }
    ورنہ اگر(dirProp==NSM) {
        /* if the last real char was ON, change NSM to ON so that it
           will stay ON even if the last real char is a bracket which
           may be changed to L or R */
        newProp=p_آخری_Iso_تسلسل->آخری_بنیاد;
        اگر(newProp==ON)
            dirProps[position]=newProp;
    }
    ورنہ {
        newProp=dirProp;
        p_آخری_Iso_تسلسل->آخری_بنیاد=dirProp;
    }
    اگر(newProp<=R || newProp==AL) {
        int32_t i;
        uint16_t flag=DIRPROP_FLAG(DIR_FROM_STRONG(newProp));
        برائے(i=p_آخری_Iso_تسلسل->آغاز; i<p_آخری_Iso_تسلسل->حد_بندی; i++)
            اگر(position>bd->openings[i].position)
                bd->openings[i].flags|=flag;
    }
    واپس سچ;
}

/* perform (X1)..(X9) ------------------------------------------------------- */

/* determine if the text is mixed-directional or single-directional */
ساکن UBiDiDirection
directionFromFlags(UBiDi *pBiDi) {
    Flags flags=pBiDi->flags;
    /* if the text contains AN and neutrals, then some neutrals may become RTL */
    اگر(!(flags&نقاب_RTL || ((flags&DIRPROP_FLAG(AN)) && (flags&نقاب_POSSIBLE_N)))) {
        واپس UBIDI_LTR;
    } ورنہ اگر(!(flags&نقاب_LTR)) {
        واپس UBIDI_RTL;
    } ورنہ {
        واپس UBIDI_MIXED;
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
ساکن UBiDiDirection
resolve_Explicit_سطحیں(UBiDi *pBiDi, UError_رمز *p_Error_رمز) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *سطحیں=pBiDi->سطحیں;
    ثابت char16_t *متن=pBiDi->متن;

    int32_t i=0, length=pBiDi->length;
    Flags flags=pBiDi->flags;       /* collect all directionalities in the text */
    DirProp dirProp;
    UBiDiLevel level=GET_PARALEVEL(pBiDi, 0);
    UBiDiDirection direction;
    pBiDi->isolate_گنتی=0;

    اگر(U_FAILURE(*p_Error_رمز)) { واپس UBIDI_LTR; }

    /* determine if the text is mixed-directional or single-directional */
    direction=directionFromFlags(pBiDi);

    /* we may not need to resolve any explicit levels */
    اگر((direction!=UBIDI_MIXED)) {
        /* not mixed directionality: levels don't matter - trailingWSStart will be 0 */
        واپس direction;
    }
    اگر(pBiDi->reordering_منوال > UBIDI_REORDER_آخری_LOGICAL_مقصد_VISUAL) {
        /* inverse BiDi: mixed, but all characters are at the same embedding level */
        /* set all levels to the paragraph level */
        int32_t para_اشاریہ, آغاز, حد_بندی;
        برائے(para_اشاریہ=0; para_اشاریہ<pBiDi->para_گنتی; para_اشاریہ++) {
            اگر(para_اشاریہ==0)
                آغاز=0;
            ورنہ
                آغاز=pBiDi->paras[para_اشاریہ-1].حد_بندی;
            حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_اشاریہ].level);
            برائے(i=آغاز; i<حد_بندی; i++)
                سطحیں[i]=level;
        }
        واپس direction;   /* no bracket matching for inverse BiDi */
    }
    اگر(!(flags&(نقاب_EXPLICIT|نقاب_ISO))) {
        /* no embeddings, set all levels to the paragraph level */
        /* we still have to perform bracket matching */
        int32_t para_اشاریہ, آغاز, حد_بندی;
        Bracket_ڈیٹا bracket_ڈیٹا;
        bracketInit(pBiDi, &bracket_ڈیٹا);
        برائے(para_اشاریہ=0; para_اشاریہ<pBiDi->para_گنتی; para_اشاریہ++) {
            اگر(para_اشاریہ==0)
                آغاز=0;
            ورنہ
                آغاز=pBiDi->paras[para_اشاریہ-1].حد_بندی;
            حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
            level= static_cast<UBiDiLevel>(pBiDi->paras[para_اشاریہ].level);
            برائے(i=آغاز; i<حد_بندی; i++) {
                سطحیں[i]=level;
                dirProp=dirProps[i];
                اگر(dirProp==BN)
                    جاری;
                اگر(dirProp==B) {
                    اگر((i+1)<length) {
                        اگر(متن[i]==CR && متن[i+1]==LF)
                            جاری;   /* skip CR when followed by LF */
                        bracketProcessB(&bracket_ڈیٹا, level);
                    }
                    جاری;
                }
                اگر(!bracketProcessChar(&bracket_ڈیٹا, i)) {
                    *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                    واپس UBIDI_LTR;
                }
            }
        }
        واپس direction;
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
        int32_t حد_سے_تجاوز_Isolate_گنتی=0;
        int32_t حد_سے_تجاوز_Embedding_گنتی=0;
        int32_t valid_Isolate_گنتی=0;
        Bracket_ڈیٹا bracket_ڈیٹا;
        bracketInit(pBiDi, &bracket_ڈیٹا);
        ڈھیر_5[0]=level;     /* initialize base entry to para level, no override, no isolate */

        /* recalculate the flags */
        flags=0;

        برائے(i=0; i<length; ++i) {
            dirProp=dirProps[i];
            انتخاب(dirProp) {
            حالت LRE:
            حالت RLE:
            حالت LRO:
            حالت RLO:
                /* (X2, X3, X4, X5) */
                flags|=DIRPROP_FLAG(BN);
                سطحیں[i]=پچھلا_Level;
                اگر (dirProp==LRE || dirProp==LRO)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ورنہ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                اگر(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && حد_سے_تجاوز_Isolate_گنتی==0 &&
                                                         حد_سے_تجاوز_Embedding_گنتی==0) {
                    آخری_Cc_Pos=i;
                    embeddingLevel=newLevel;
                    اگر(dirProp==LRO || dirProp==RLO)
                        embeddingLevel|=UBIDI_LEVEL_OVERRIDE;
                    ڈھیر_آخری++;
                    ڈھیر_5[ڈھیر_آخری]=embeddingLevel;
                    /* we don't need to set UBIDI_LEVEL_OVERRIDE off for LRE and RLE
                       since this has already been done for newLevel which is
                       the source for embeddingLevel.
                     */
                } ورنہ {
                    اگر(حد_سے_تجاوز_Isolate_گنتی==0)
                        حد_سے_تجاوز_Embedding_گنتی++;
                }
                توڑو;
            حالت PDF:
                /* (X7) */
                flags|=DIRPROP_FLAG(BN);
                سطحیں[i]=پچھلا_Level;
                /* handle all the overflow cases first */
                اگر(حد_سے_تجاوز_Isolate_گنتی) {
                    توڑو;
                }
                اگر(حد_سے_تجاوز_Embedding_گنتی) {
                    حد_سے_تجاوز_Embedding_گنتی--;
                    توڑو;
                }
                اگر(ڈھیر_آخری>0 && ڈھیر_5[ڈھیر_آخری]<ISOLATE) {   /* not an isolate entry */
                    آخری_Cc_Pos=i;
                    ڈھیر_آخری--;
                    embeddingLevel = static_cast<UBiDiLevel>(ڈھیر_5[ڈھیر_آخری]);
                }
                توڑو;
            حالت LRI:
            حالت RLI:
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                سطحیں[i]=NO_OVERRIDE(embeddingLevel);
                اگر(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_ڈیٹا, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                پچھلا_Level=embeddingLevel;
                /* (X5a, X5b) */
                اگر(dirProp==LRI)
                    /* least greater even level */
                    newLevel = static_cast<UBiDiLevel>((embeddingLevel + 2) & ~(UBIDI_LEVEL_OVERRIDE | 1));
                ورنہ
                    /* least greater odd level */
                    newLevel = static_cast<UBiDiLevel>((NO_OVERRIDE(embeddingLevel) + 1) | 1);
                اگر(newLevel<=UBIDI_MAX_EXPLICIT_LEVEL && حد_سے_تجاوز_Isolate_گنتی==0 &&
                                                         حد_سے_تجاوز_Embedding_گنتی==0) {
                    flags|=DIRPROP_FLAG(dirProp);
                    آخری_Cc_Pos=i;
                    valid_Isolate_گنتی++;
                    اگر(valid_Isolate_گنتی>pBiDi->isolate_گنتی)
                        pBiDi->isolate_گنتی=valid_Isolate_گنتی;
                    embeddingLevel=newLevel;
                    /* we can increment stackLast without checking because newLevel
                       will exceed UBIDI_MAX_EXPLICIT_LEVEL before stackLast overflows */
                    ڈھیر_آخری++;
                    ڈھیر_5[ڈھیر_آخری]=embeddingLevel+ISOLATE;
                    bracketProcessLRI_RLI(&bracket_ڈیٹا, embeddingLevel);
                } ورنہ {
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                    حد_سے_تجاوز_Isolate_گنتی++;
                }
                توڑو;
            حالت PDI:
                اگر(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_ڈیٹا, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                }
                /* (X6a) */
                اگر(حد_سے_تجاوز_Isolate_گنتی) {
                    حد_سے_تجاوز_Isolate_گنتی--;
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                }
                ورنہ اگر(valid_Isolate_گنتی) {
                    flags|=DIRPROP_FLAG(PDI);
                    آخری_Cc_Pos=i;
                    حد_سے_تجاوز_Embedding_گنتی=0;
                    جب(ڈھیر_5[ڈھیر_آخری]<ISOLATE) /* pop embedding entries */
                        ڈھیر_آخری--;                /* until the last isolate entry */
                    ڈھیر_آخری--;                    /* pop also the last isolate entry */
                    valid_Isolate_گنتی--;
                    bracketProcessPDI(&bracket_ڈیٹا);
                } ورنہ
                    /* make it WS so that it is handled by adjustWSLevels() */
                    dirProps[i]=WS;
                embeddingLevel = static_cast<UBiDiLevel>(ڈھیر_5[ڈھیر_آخری]) & ~ISOLATE;
                flags|=(DIRPROP_FLAG(ON)|DIRPROP_FLAG_LR(embeddingLevel));
                پچھلا_Level=embeddingLevel;
                سطحیں[i]=NO_OVERRIDE(embeddingLevel);
                توڑو;
            حالت B:
                flags|=DIRPROP_FLAG(B);
                سطحیں[i]=GET_PARALEVEL(pBiDi, i);
                اگر((i+1)<length) {
                    اگر(متن[i]==CR && متن[i+1]==LF)
                        توڑو;          /* skip CR when followed by LF */
                    حد_سے_تجاوز_Embedding_گنتی=حد_سے_تجاوز_Isolate_گنتی=0;
                    valid_Isolate_گنتی=0;
                    ڈھیر_آخری=0;
                    پچھلا_Level=embeddingLevel=GET_PARALEVEL(pBiDi, i+1);
                    ڈھیر_5[0]=embeddingLevel; /* initialize base entry to para level, no override, no isolate */
                    bracketProcessB(&bracket_ڈیٹا, embeddingLevel);
                }
                توڑو;
            حالت BN:
                /* BN, LRE, RLE, and PDF are supposed to be removed (X9) */
                /* they will get their levels set correctly in adjustWSLevels() */
                سطحیں[i]=پچھلا_Level;
                flags|=DIRPROP_FLAG(BN);
                توڑو;
            طےشدہ:
                /* all other types are normal characters and get the "real" level */
                اگر(NO_OVERRIDE(embeddingLevel)!=NO_OVERRIDE(پچھلا_Level)) {
                    bracketProcessBoundary(&bracket_ڈیٹا, آخری_Cc_Pos,
                                           پچھلا_Level, embeddingLevel);
                    flags|=DIRPROP_FLAG_MULTI_RUNS;
                    اگر(embeddingLevel&UBIDI_LEVEL_OVERRIDE)
                        flags|=DIRPROP_FLAG_O(embeddingLevel);
                    ورنہ
                        flags|=DIRPROP_FLAG_E(embeddingLevel);
                }
                پچھلا_Level=embeddingLevel;
                سطحیں[i]=embeddingLevel;
                اگر(!bracketProcessChar(&bracket_ڈیٹا, i))
                    واپس static_cast<UBiDiDirection>(-1);
                /* the dirProp may have been changed in bracketProcessChar() */
                flags|=DIRPROP_FLAG(dirProps[i]);
                توڑو;
            }
        }
        اگر(flags&نقاب_EMBEDDING)
            flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
        اگر(pBiDi->ترتیب_Paragraphs_LTR && (flags&DIRPROP_FLAG(B)))
            flags|=DIRPROP_FLAG(L);
        /* again, determine if the text is mixed-directional or single-directional */
        pBiDi->flags=flags;
        direction=directionFromFlags(pBiDi);
    }
    واپس direction;
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
ساکن UBiDiDirection
check_Explicit_سطحیں(UBiDi *pBiDi, UError_رمز *p_Error_رمز) {
    DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *سطحیں=pBiDi->سطحیں;
    int32_t isolate_گنتی=0;

    int32_t length=pBiDi->length;
    Flags flags=0;  /* collect all directionalities in the text */
    pBiDi->isolate_گنتی=0;

    int32_t current_Para_اشاریہ = 0;
    int32_t current_Para_حد_بندی = pBiDi->paras[0].حد_بندی;
    int32_t currentParaLevel = pBiDi->paraLevel;

    برائے(int32_t i=0; i<length; ++i) {
        UBiDiLevel level=سطحیں[i];
        DirProp dirProp=dirProps[i];
        اگر(dirProp==LRI || dirProp==RLI) {
            isolate_گنتی++;
            اگر(isolate_گنتی>pBiDi->isolate_گنتی)
                pBiDi->isolate_گنتی=isolate_گنتی;
        }
        ورنہ اگر(dirProp==PDI)
            isolate_گنتی--;
        ورنہ اگر(dirProp==B)
            isolate_گنتی=0;

        // optimized version of  int32_t currentParaLevel = GET_PARALEVEL(pBiDi, i);
        اگر (pBiDi->defaultParaLevel != 0 &&
                i == current_Para_حد_بندی && (current_Para_اشاریہ + 1) < pBiDi->para_گنتی) {
            currentParaLevel = pBiDi->paras[++current_Para_اشاریہ].level;
            current_Para_حد_بندی = pBiDi->paras[current_Para_اشاریہ].حد_بندی;
        }

        UBiDiLevel overrideFlag = level & UBIDI_LEVEL_OVERRIDE;
        level &= ~UBIDI_LEVEL_OVERRIDE;
        اگر (level < currentParaLevel || UBIDI_MAX_EXPLICIT_LEVEL < level) {
            اگر (level == 0) {
                اگر (dirProp == B) {
                    // Paragraph separators are ok with explicit level 0.
                    // Prevents reordering of paragraphs.
                } ورنہ {
                    // Treat explicit level 0 as a wildcard for the paragraph level.
                    // Avoid making the caller guess what the paragraph level would be.
                    level = static_cast<UBiDiLevel>(currentParaLevel);
                    سطحیں[i] = level | overrideFlag;
                }
            } ورنہ {
                // 1 <= level < currentParaLevel or UBIDI_MAX_EXPLICIT_LEVEL < level
                /* level out of bounds */
                *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
                واپس UBIDI_LTR;
            }
        }
        اگر (overrideFlag != 0) {
            /* keep the override flag in levels[i] but adjust the flags */
            flags|=DIRPROP_FLAG_O(level);
        } ورنہ {
            /* set the flags */
            flags|=DIRPROP_FLAG_E(level)|DIRPROP_FLAG(dirProp);
        }
    }
    اگر(flags&نقاب_EMBEDDING)
        flags|=DIRPROP_FLAG_LR(pBiDi->paraLevel);
    /* determine if the text is mixed-directional or single-directional */
    pBiDi->flags=flags;
    واپس directionFromFlags(pBiDi);
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

ساکن ثابت uint8_t گروہ_Prop[] =          /* dirProp regrouped */
{
/*  L   R   EN  ES  ET  AN  CS  B   S   WS  ON  LRE LRO AL  RLE RLO PDF NSM BN  FSI LRI RLI PDI ENL ENR */
    0,  1,  2,  7,  8,  3,  9,  6,  5,  4,  4,  10, 10, 12, 10, 10, 10, 11, 10, 4,  4,  4,  4,  13, 14
};
شمار { DirProp_L=0, DirProp_R=1, DirProp_EN=2, DirProp_AN=3, DirProp_ON=4, DirProp_S=5, DirProp_B=6 }; /* reduced dirProp */

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
ساکن ثابت uint8_t impTabProps[][IMPTABPROPS_COLUMNS] =
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
#تعریف_منسوخ s

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

قسمتعریف uint8_t ImpTab[][IMPTABLEVELS_COLUMNS];
قسمتعریف uint8_t ImpAct[];

/* FOOD FOR THOUGHT: each ImpTab should have its associated ImpAct,
 * instead of having a pair of ImpTab and a pair of ImpAct.
 */
قسمتعریف ساخت ImpTabPair {
    ثابت خالی * pImpTab[2];
    ثابت خالی * pImpAct[2];
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

ساکن ثابت ImpTab impTabL_DEFAULT =   /* Even paragraph level */
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
ساکن ثابت ImpTab impTabR_DEFAULT =   /* Odd  paragraph level */
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
ساکن ثابت ImpAct impAct0 = {0,1,2,3,4};
ساکن ثابت ImpTabPair impTab_DEFAULT = {{&impTabL_DEFAULT,
                                           &impTabR_DEFAULT},
                                          {&impAct0, &impAct0}};

ساکن ثابت ImpTab impTabL_NUMBERS_SPECIAL =   /* Even paragraph level */
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
ساکن ثابت ImpTabPair impTab_NUMBERS_SPECIAL = {{&impTabL_NUMBERS_SPECIAL,
                                                   &impTabR_DEFAULT},
                                                  {&impAct0, &impAct0}};

ساکن ثابت ImpTab imp_Tab_L_گروہ_NUMBERS_WITH_R =
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
ساکن ثابت ImpTab imp_Tab_R_گروہ_NUMBERS_WITH_R =
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
ساکن ثابت ImpTabPair imp_Tab_گروہ_NUMBERS_WITH_R = {
                        {&imp_Tab_L_گروہ_NUMBERS_WITH_R,
                         &imp_Tab_R_گروہ_NUMBERS_WITH_R},
                        {&impAct0, &impAct0}};


ساکن ثابت ImpTab impTabL_INVERSE_NUMBERS_AS_L =
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
ساکن ثابت ImpTab impTabR_INVERSE_NUMBERS_AS_L =
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
ساکن ثابت ImpTabPair impTab_INVERSE_NUMBERS_AS_L = {
                        {&impTabL_INVERSE_NUMBERS_AS_L,
                         &impTabR_INVERSE_NUMBERS_AS_L},
                        {&impAct0, &impAct0}};

ساکن ثابت ImpTab imp_Tab_R_INVERSE_LIKE_براہ_راست =   /* Odd  paragraph level */
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
ساکن ثابت ImpAct impAct1 = {0,1,13,14};
/* FOOD FOR THOUGHT: in LTR table below, check case "JKL 123abc"
 */
ساکن ثابت ImpTabPair imp_Tab_INVERSE_LIKE_براہ_راست = {
                        {&impTabL_DEFAULT,
                         &imp_Tab_R_INVERSE_LIKE_براہ_راست},
                        {&impAct0, &impAct1}};

ساکن ثابت ImpTab imp_Tab_L_INVERSE_LIKE_براہ_راست_WITH_MARKS =
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
ساکن ثابت ImpTab imp_Tab_R_INVERSE_LIKE_براہ_راست_WITH_MARKS =
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
ساکن ثابت ImpAct impAct2 = {0,1,2,5,6,7,8};
ساکن ثابت ImpAct impAct3 = {0,1,9,10,11,12};
ساکن ثابت ImpTabPair imp_Tab_INVERSE_LIKE_براہ_راست_WITH_MARKS = {
                        {&imp_Tab_L_INVERSE_LIKE_براہ_راست_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_براہ_راست_WITH_MARKS},
                        {&impAct2, &impAct3}};

ساکن ثابت ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL = {
                        {&impTabL_NUMBERS_SPECIAL,
                         &imp_Tab_R_INVERSE_LIKE_براہ_راست},
                        {&impAct0, &impAct1}};

ساکن ثابت ImpTab impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS =
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
ساکن ثابت ImpTabPair impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS = {
                        {&impTabL_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS,
                         &imp_Tab_R_INVERSE_LIKE_براہ_راست_WITH_MARKS},
                        {&impAct2, &impAct3}};

#تعریف_منسوخ s

قسمتعریف ساخت {
    ثابت ImpTab * pImpTab;             /* level table pointer          */
    ثابت ImpAct * pImpAct;             /* action map array             */
    int32_t آغاز_ON;                    /* start of ON sequence         */
    int32_t آغاز_L2_EN;                  /* start of level 2 sequence    */
    int32_t آخری_Strong_RTL;              /* index of last found R or AL  */
    int32_t state;                      /* current state                */
    int32_t تسلسل_آغاز;                   /* start position of the run    */
    UBiDiLevel تسلسل_Level;                /* run level before implicit solving */
} LevState;

/*------------------------------------------------------------------------*/

ساکن خالی
جوڑو_نقطہ(UBiDi *pBiDi, int32_t pos, int32_t flag)
  /* param pos:     position where to insert
     param flag:    one of LRM_BEFORE, LRM_AFTER, RLM_BEFORE, RLM_AFTER
  */
{
#تعریف_کرو FIRSTALLOC  10
    نقطہ نقطہ_2;
    داخل_کرو_Points_2 * p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);

    اگر (p_داخل_کرو_Points->capacity == 0)
    {
        p_داخل_کرو_Points->points=static_cast<نقطہ *>(uprv_malloc(حجم(نقطہ)*FIRSTALLOC));
        اگر (p_داخل_کرو_Points->points == خالی_اشاریہ)
        {
            p_داخل_کرو_Points->error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        p_داخل_کرو_Points->capacity=FIRSTALLOC;
    }
    اگر (p_داخل_کرو_Points->size >= p_داخل_کرو_Points->capacity) /* no room for new point */
    {
        نقطہ * savePoints=p_داخل_کرو_Points->points;
        p_داخل_کرو_Points->points=static_cast<نقطہ *>(uprv_realloc(p_داخل_کرو_Points->points,
                                           p_داخل_کرو_Points->capacity*2*حجم(نقطہ)));
        اگر (p_داخل_کرو_Points->points == خالی_اشاریہ)
        {
            p_داخل_کرو_Points->points=savePoints;
            p_داخل_کرو_Points->error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        ورنہ  p_داخل_کرو_Points->capacity*=2;
    }
    نقطہ_2.pos=pos;
    نقطہ_2.flag=flag;
    p_داخل_کرو_Points->points[p_داخل_کرو_Points->size]=نقطہ_2;
    p_داخل_کرو_Points->size++;
#تعریف_منسوخ FIRSTALLOC
}

ساکن خالی
سیٹ_سطحیں_Outside_Isolates(UBiDi *pBiDi, int32_t آغاز, int32_t حد_بندی, UBiDiLevel level)
{
    DirProp *dirProps=pBiDi->dirProps, dirProp;
    UBiDiLevel *سطحیں=pBiDi->سطحیں;
    int32_t isolate_گنتی=0, k;
    برائے(k=آغاز; k<حد_بندی; k++) {
        dirProp=dirProps[k];
        اگر(dirProp==PDI)
            isolate_گنتی--;
        اگر(isolate_گنتی==0)
            سطحیں[k]=level;
        اگر(dirProp==LRI || dirProp==RLI)
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

ساکن خالی
processPropertySeq(UBiDi *pBiDi, LevState *pLevState, uint8_t _prop,
                   int32_t آغاز, int32_t حد_بندی) {
    uint8_t cell, oldStateSeq, actionSeq;
    ثابت ImpTab * pImpTab=pLevState->pImpTab;
    ثابت ImpAct * pImpAct=pLevState->pImpAct;
    UBiDiLevel * سطحیں=pBiDi->سطحیں;
    UBiDiLevel level, جوڑو_Level;
    داخل_کرو_Points_2 * p_داخل_کرو_Points;
    int32_t start0, k;

    start0=آغاز;                           /* save original start position */
    oldStateSeq = static_cast<uint8_t>(pLevState->state);
    cell=(*pImpTab)[oldStateSeq][_prop];
    pLevState->state=GET_STATE(cell);       /* isolate the new state */
    actionSeq=(*pImpAct)[GET_ACTION(cell)]; /* isolate the action */
    جوڑو_Level=(*pImpTab)[pLevState->state][IMPTABLEVELS_RES];

    اگر(actionSeq) {
        انتخاب(actionSeq) {
        حالت 1:                         /* init ON seq */
            pLevState->آغاز_ON=start0;
            توڑو;

        حالت 2:                         /* prepend ON seq to current seq */
            آغاز=pLevState->آغاز_ON;
            توڑو;

        حالت 3:                         /* EN/AN after R+ON */
            level=pLevState->تسلسل_Level+1;
            سیٹ_سطحیں_Outside_Isolates(pBiDi, pLevState->آغاز_ON, start0, level);
            توڑو;

        حالت 4:                         /* EN/AN before R for NUMBERS_SPECIAL */
            level=pLevState->تسلسل_Level+2;
            سیٹ_سطحیں_Outside_Isolates(pBiDi, pLevState->آغاز_ON, start0, level);
            توڑو;

        حالت 5:                         /* L or S after possible relevant EN/AN */
            /* check if we had EN after R/AL */
            اگر (pLevState->آغاز_L2_EN >= 0) {
                جوڑو_نقطہ(pBiDi, pLevState->آغاز_L2_EN, LRM_BEFORE);
            }
            pLevState->آغاز_L2_EN=-1;  /* not within previous if since could also be -2 */
            /* check if we had any relevant EN/AN after R/AL */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            اگر ((p_داخل_کرو_Points->capacity == 0) ||
                (p_داخل_کرو_Points->size <= p_داخل_کرو_Points->confirmed))
            {
                /* nothing, just clean up */
                pLevState->آخری_Strong_RTL=-1;
                /* check if we have a pending conditional segment */
                level=(*pImpTab)[oldStateSeq][IMPTABLEVELS_RES];
                اگر ((level & 1) && (pLevState->آغاز_ON > 0)) {  /* after ON */
                    آغاز=pLevState->آغاز_ON;   /* reset to basic run level */
                }
                اگر (_prop == DirProp_S)                /* add LRM before S */
                {
                    جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                    p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
                }
                توڑو;
            }
            /* reset previous RTL cont to level for LTR text */
            برائے (k=pLevState->آخری_Strong_RTL+1; k<start0; k++)
            {
                /* reset odd level, leave runLevel+2 as is */
                سطحیں[k]=(سطحیں[k] - 2) & ~1;
            }
            /* mark insert points as confirmed */
            p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            pLevState->آخری_Strong_RTL=-1;
            اگر (_prop == DirProp_S)            /* add LRM before S */
            {
                جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            }
            توڑو;

        حالت 6:                         /* R/AL after possible relevant EN/AN */
            /* just clean up */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            اگر (p_داخل_کرو_Points->capacity > 0)
                /* remove all non confirmed insert points */
                p_داخل_کرو_Points->size=p_داخل_کرو_Points->confirmed;
            pLevState->آغاز_ON=-1;
            pLevState->آغاز_L2_EN=-1;
            pLevState->آخری_Strong_RTL=حد_بندی - 1;
            توڑو;

        حالت 7:                         /* EN/AN after R/AL + possible cont */
            /* check for real AN */
            اگر ((_prop == DirProp_AN) && (pBiDi->dirProps[start0] == AN) &&
                (pBiDi->reordering_منوال!=UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))
            {
                /* real AN */
                اگر (pLevState->آغاز_L2_EN == -1) /* if no relevant EN already found */
                {
                    /* just note the righmost digit as a strong RTL */
                    pLevState->آخری_Strong_RTL=حد_بندی - 1;
                    توڑو;
                }
                اگر (pLevState->آغاز_L2_EN >= 0)  /* after EN, no AN */
                {
                    جوڑو_نقطہ(pBiDi, pLevState->آغاز_L2_EN, LRM_BEFORE);
                    pLevState->آغاز_L2_EN=-2;
                }
                /* note AN */
                جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);
                توڑو;
            }
            /* if first EN/AN after R/AL */
            اگر (pLevState->آغاز_L2_EN == -1) {
                pLevState->آغاز_L2_EN=start0;
            }
            توڑو;

        حالت 8:                         /* note location of latest R/AL */
            pLevState->آخری_Strong_RTL=حد_بندی - 1;
            pLevState->آغاز_ON=-1;
            توڑو;

        حالت 9:                         /* L after R+ON/EN/AN */
            /* include possible adjacent number on the left */
            برائے (k=start0-1; k>=0 && !(سطحیں[k]&1); k--);
            اگر(k>=0) {
                جوڑو_نقطہ(pBiDi, k, RLM_BEFORE);             /* add RLM before */
                p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;   /* confirm it */
            }
            pLevState->آغاز_ON=start0;
            توڑو;

        حالت 10:                        /* AN after L */
            /* AN numbers between L text on both sides may be trouble. */
            /* tentatively bracket with LRMs; will be confirmed if followed by L */
            جوڑو_نقطہ(pBiDi, start0, LRM_BEFORE);    /* add LRM before */
            جوڑو_نقطہ(pBiDi, start0, LRM_AFTER);     /* add LRM after  */
            توڑو;

        حالت 11:                        /* R after L+ON/EN/AN */
            /* false alert, infirm LRMs around previous AN */
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            p_داخل_کرو_Points->size=p_داخل_کرو_Points->confirmed;
            اگر (_prop == DirProp_S)            /* add RLM before S */
            {
                جوڑو_نقطہ(pBiDi, start0, RLM_BEFORE);
                p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;
            }
            توڑو;

        حالت 12:                        /* L after L+ON/AN */
            level=pLevState->تسلسل_Level + جوڑو_Level;
            برائے(k=pLevState->آغاز_ON; k<start0; k++) {
                اگر (سطحیں[k]<level)
                    سطحیں[k]=level;
            }
            p_داخل_کرو_Points=&(pBiDi->داخل_کرو_Points);
            p_داخل_کرو_Points->confirmed=p_داخل_کرو_Points->size;   /* confirm inserts */
            pLevState->آغاز_ON=start0;
            توڑو;

        حالت 13:                        /* L after L+ON+EN/AN/ON */
            level=pLevState->تسلسل_Level;
            برائے(k=start0-1; k>=pLevState->آغاز_ON; k--) {
                اگر(سطحیں[k]==level+3) {
                    جب(سطحیں[k]==level+3) {
                        سطحیں[k--]-=2;
                    }
                    جب(سطحیں[k]==level) {
                        k--;
                    }
                }
                اگر(سطحیں[k]==level+2) {
                    سطحیں[k]=level;
                    جاری;
                }
                سطحیں[k]=level+1;
            }
            توڑو;

        حالت 14:                        /* R after L+ON+EN/AN/ON */
            level=pLevState->تسلسل_Level+1;
            برائے(k=start0-1; k>=pLevState->آغاز_ON; k--) {
                اگر(سطحیں[k]>level) {
                    سطحیں[k]-=2;
                }
            }
            توڑو;

        طےشدہ:                        /* we should never get here */
            UPRV_UNREACHABLE_EXIT;
        }
    }
    اگر((جوڑو_Level) || (آغاز < start0)) {
        level=pLevState->تسلسل_Level + جوڑو_Level;
        اگر(آغاز>=pLevState->تسلسل_آغاز) {
            برائے(k=آغاز; k<حد_بندی; k++) {
                سطحیں[k]=level;
            }
        } ورنہ {
            سیٹ_سطحیں_Outside_Isolates(pBiDi, آغاز, حد_بندی, level);
        }
    }
}

/**
 * Returns the directionality of the last strong character at the end of the prologue, if any.
 * Requires prologue!=null.
 */
ساکن DirProp
آخری_L_R_AL(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->prologue;
    int32_t length=pBiDi->pro_لمبائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    برائے(i=length; i>0; ) {
        /* i is decremented by U16_PREV */
        U16_پچھلا(متن, 0, i, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        اگر(dirProp==L) {
            واپس DirProp_L;
        }
        اگر(dirProp==R || dirProp==AL) {
            واپس DirProp_R;
        }
        اگر(dirProp==B) {
            واپس DirProp_ON;
        }
    }
    واپس DirProp_ON;
}

/**
 * Returns the directionality of the first strong character, or digit, in the epilogue, if any.
 * Requires epilogue!=null.
 */
ساکن DirProp
پہلا_L_R_AL_EN_AN(UBiDi *pBiDi) {
    ثابت char16_t *متن=pBiDi->epilogue;
    int32_t length=pBiDi->epi_لمبائی;
    int32_t i;
    UChar32 uchar;
    DirProp dirProp;
    برائے(i=0; i<length; ) {
        /* i is incremented by U16_NEXT */
        U16_اگلا(متن, i, length, uchar);
        dirProp = static_cast<DirProp>(ubidi_getCustomizedClass(pBiDi, uchar));
        اگر(dirProp==L) {
            واپس DirProp_L;
        }
        اگر(dirProp==R || dirProp==AL) {
            واپس DirProp_R;
        }
        اگر(dirProp==EN) {
            واپس DirProp_EN;
        }
        اگر(dirProp==AN) {
            واپس DirProp_AN;
        }
    }
    واپس DirProp_ON;
}

ساکن خالی
resolve_Implicit_سطحیں(UBiDi *pBiDi,
                      int32_t آغاز, int32_t حد_بندی,
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
        static_cast<UBool>((آغاز < pBiDi->آخری_Arabic_Pos) && (GET_PARALEVEL(pBiDi, آغاز) & 1) &&
                           (pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_LIKE_براہ_راست ||
                            pBiDi->reordering_منوال == UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL));

    /* initialize for property and levels state tables */
    levState.آغاز_L2_EN=-1;              /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.آخری_Strong_RTL=-1;          /* used for INVERSE_LIKE_DIRECT_WITH_MARKS */
    levState.تسلسل_آغاز=آغاز;
    levState.تسلسل_Level=pBiDi->سطحیں[آغاز];
    levState.pImpTab = static_cast<ثابت ImpTab*>(((pBiDi->pImpTabPair)->pImpTab)[levState.تسلسل_Level & 1]);
    levState.pImpAct = static_cast<ثابت ImpAct*>(((pBiDi->pImpTabPair)->pImpAct)[levState.تسلسل_Level & 1]);
    اگر(آغاز==0 && pBiDi->pro_لمبائی>0) {
        DirProp آخری_Strong=آخری_L_R_AL(pBiDi);
        اگر(آخری_Strong!=DirProp_ON) {
            sor=آخری_Strong;
        }
    }
    /* The isolates[] entries contain enough information to
       resume the bidi algorithm in the same state as it was
       when it was interrupted by an isolate sequence. */
    اگر(dirProps[آغاز]==PDI  && pBiDi->isolate_گنتی >= 0) {
        levState.آغاز_ON=pBiDi->isolates[pBiDi->isolate_گنتی].آغاز_ON;
        start1=pBiDi->isolates[pBiDi->isolate_گنتی].start1;
        stateImp=pBiDi->isolates[pBiDi->isolate_گنتی].stateImp;
        levState.state=pBiDi->isolates[pBiDi->isolate_گنتی].state;
        pBiDi->isolate_گنتی--;
    } ورنہ {
        levState.آغاز_ON=-1;
        start1=آغاز;
        اگر(dirProps[آغاز]==NSM)
            stateImp = 1 + sor;
        ورنہ
            stateImp=0;
        levState.state=0;
        processPropertySeq(pBiDi, &levState, sor, آغاز, آغاز);
    }
    start2=آغاز;                       /* to make Java compiler happy */

    برائے(i=آغاز; i<=حد_بندی; i++) {
        اگر(i>=حد_بندی) {
            int32_t k;
            برائے(k=حد_بندی-1; k>آغاز&&(DIRPROP_FLAG(dirProps[k])&نقاب_BN_EXPLICIT); k--);
            dirProp=dirProps[k];
            اگر(dirProp==LRI || dirProp==RLI)
                توڑو;      /* no forced closing for sequence ending with LRI/RLI */
            gprop=eor;
        } ورنہ {
            DirProp prop, prop1;
            prop=dirProps[i];
            اگر(prop==B) {
                pBiDi->isolate_گنتی=-1; /* current isolates stack entry == none */
            }
            اگر(inverseRTL) {
                اگر(prop==AL) {
                    /* AL before EN does not make it AN */
                    prop=R;
                } ورنہ اگر(prop==EN) {
                    اگر(اگلا_Strong_Pos<=i) {
                        /* look for next strong char (L/R/AL) */
                        int32_t j;
                        اگلا_Strong_Prop=R;   /* set default */
                        اگلا_Strong_Pos=حد_بندی;
                        برائے(j=i+1; j<حد_بندی; j++) {
                            prop1=dirProps[j];
                            اگر(prop1==L || prop1==R || prop1==AL) {
                                اگلا_Strong_Prop=prop1;
                                اگلا_Strong_Pos=j;
                                توڑو;
                            }
                        }
                    }
                    اگر(اگلا_Strong_Prop==AL) {
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
        اگر((i==حد_بندی) && (actionImp==0)) {
            /* there is an unprocessed sequence if its property == eor   */
            actionImp=1;                    /* process the last sequence */
        }
        اگر(actionImp) {
            resProp=impTabProps[oldStateImp][IMPTABPROPS_RES];
            انتخاب(actionImp) {
            حالت 1:             /* process current seq1, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, i);
                start1=i;
                توڑو;
            حالت 2:             /* init new seq2 */
                start2=i;
                توڑو;
            حالت 3:             /* process seq1, process seq2, init new seq1 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                processPropertySeq(pBiDi, &levState, DirProp_ON, start2, i);
                start1=i;
                توڑو;
            حالت 4:             /* process seq1, set seq1=seq2, init new seq2 */
                processPropertySeq(pBiDi, &levState, resProp, start1, start2);
                start1=start2;
                start2=i;
                توڑو;
            طےشدہ:            /* we should never get here */
                UPRV_UNREACHABLE_EXIT;
            }
        }
    }

    /* flush possible pending sequence, e.g. ON */
    اگر(حد_بندی==pBiDi->length && pBiDi->epi_لمبائی>0) {
        DirProp پہلا_Strong=پہلا_L_R_AL_EN_AN(pBiDi);
        اگر(پہلا_Strong!=DirProp_ON) {
            eor=پہلا_Strong;
        }
    }

    /* look for the last char not a BN or LRE/RLE/LRO/RLO/PDF */
    برائے(i=حد_بندی-1; i>آغاز&&(DIRPROP_FLAG(dirProps[i])&نقاب_BN_EXPLICIT); i--);
    dirProp=dirProps[i];
    اگر((dirProp==LRI || dirProp==RLI) && حد_بندی<pBiDi->length) {
        pBiDi->isolate_گنتی++;
        pBiDi->isolates[pBiDi->isolate_گنتی].stateImp=stateImp;
        pBiDi->isolates[pBiDi->isolate_گنتی].state=levState.state;
        pBiDi->isolates[pBiDi->isolate_گنتی].start1=start1;
        pBiDi->isolates[pBiDi->isolate_گنتی].آغاز_ON=levState.آغاز_ON;
    }
    ورنہ
        processPropertySeq(pBiDi, &levState, eor, حد_بندی, حد_بندی);
}

/* perform (L1) and (X9) ---------------------------------------------------- */

/*
 * Reset the embedding levels for some non-graphic characters (L1).
 * This function also sets appropriate levels for BN, and
 * explicit embedding types that are supposed to have been removed
 * from the paragraph in (X9).
 */
ساکن خالی
adjustWSLevels(UBiDi *pBiDi) {
    ثابت DirProp *dirProps=pBiDi->dirProps;
    UBiDiLevel *سطحیں=pBiDi->سطحیں;
    int32_t i;

    اگر(pBiDi->flags&نقاب_WS) {
        UBool ترتیب_Paragraphs_LTR=pBiDi->ترتیب_Paragraphs_LTR;
        Flags flag;

        i=pBiDi->trailingWSStart;
        جب(i>0) {
            /* reset a sequence of WS/BN before eop and B/S to the paragraph paraLevel */
            جب(i>0 && (flag=DIRPROP_FLAG(dirProps[--i]))&نقاب_WS) {
                اگر(ترتیب_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    سطحیں[i]=0;
                } ورنہ {
                    سطحیں[i]=GET_PARALEVEL(pBiDi, i);
                }
            }

            /* reset BN to the next character's paraLevel until B/S, which restarts above loop */
            /* here, i+1 is guaranteed to be <length */
            جب(i>0) {
                flag=DIRPROP_FLAG(dirProps[--i]);
                اگر(flag&نقاب_BN_EXPLICIT) {
                    سطحیں[i]=سطحیں[i+1];
                } ورنہ اگر(ترتیب_Paragraphs_LTR&&(flag&DIRPROP_FLAG(B))) {
                    سطحیں[i]=0;
                    توڑو;
                } ورنہ اگر(flag&نقاب_B_S) {
                    سطحیں[i]=GET_PARALEVEL(pBiDi, i);
                    توڑو;
                }
            }
        }
    }
}

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Context(UBiDi *pBiDi,
                 ثابت char16_t *prologue, int32_t pro_لمبائی,
                 ثابت char16_t *epilogue, int32_t epi_لمبائی,
                 UError_رمز *p_Error_رمز) {
    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    اگر(pBiDi==خالی_اشاریہ || pro_لمبائی<-1 || epi_لمبائی<-1 ||
       (prologue==خالی_اشاریہ && pro_لمبائی!=0) || (epilogue==خالی_اشاریہ && epi_لمبائی!=0)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر(pro_لمبائی==-1) {
        pBiDi->pro_لمبائی=u_strlen(prologue);
    } ورنہ {
        pBiDi->pro_لمبائی=pro_لمبائی;
    }
    اگر(epi_لمبائی==-1) {
        pBiDi->epi_لمبائی=u_strlen(epilogue);
    } ورنہ {
        pBiDi->epi_لمبائی=epi_لمبائی;
    }
    pBiDi->prologue=prologue;
    pBiDi->epilogue=epilogue;
}

ساکن خالی
سیٹ_Para_Success(UBiDi *pBiDi) {
    pBiDi->pro_لمبائی=0;                 /* forget the last context */
    pBiDi->epi_لمبائی=0;
    pBiDi->pParaBiDi=pBiDi;             /* mark successful setPara */
}

#تعریف_کرو BIDI_MIN(x, y)   ((x)<(y) ? (x) : (y))
#تعریف_کرو BIDI_ABS(x)      ((x)>=0  ? (x) : (-(x)))

ساکن خالی
سیٹ_Para_Runs_Only(UBiDi *pBiDi, ثابت char16_t *متن, int32_t length,
                UBiDiLevel paraLevel, UError_رمز *p_Error_رمز) {
    int32_t *runs_Only_حافظہ = خالی_اشاریہ;
    int32_t *visualMap;
    char16_t *visual_متن;
    int32_t save_لمبائی, saveTrailingWSStart;
    ثابت UBiDiLevel *سطحیں;
    UBiDiLevel *save_سطحیں;
    UBiDiDirection saveDirection;
    UBool save_May_Allocate_متن;
    تسلسل *runs;
    int32_t visual_لمبائی, i, j, visual_آغاز, logical_آغاز,
            تسلسل_گنتی, تسلسل_لمبائی, addedRuns, داخل_کرو_Remove,
            آغاز, حد_بندی, قدم, اشاریہ_Odd_ثنائی_ہندسہ, logicalPos,
            index0, index1;
    uint32_t saveOptions;

    pBiDi->reordering_منوال=UBIDI_REORDER_DEFAULT;
    اگر(length==0) {
        ubidi_سیٹ_Para(pBiDi, متن, length, paraLevel, خالی_اشاریہ, p_Error_رمز);
        جاؤ cleanup3;
    }
    /* obtain memory for mapping table and visual text */
    runs_Only_حافظہ=static_cast<int32_t *>(uprv_malloc(length*(حجم(int32_t)+حجم(char16_t)+حجم(UBiDiLevel))));
    اگر(runs_Only_حافظہ==خالی_اشاریہ) {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        جاؤ cleanup3;
    }
    visualMap=runs_Only_حافظہ;
    visual_متن = reinterpret_cast<char16_t*>(&visualMap[length]);
    save_سطحیں = reinterpret_cast<UBiDiLevel*>(&visual_متن[length]);
    saveOptions=pBiDi->reorderingOptions;
    اگر(saveOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
        pBiDi->reorderingOptions&=~UBIDI_OPTION_داخل_کرو_MARKS;
        pBiDi->reorderingOptions|=UBIDI_OPTION_REMOVE_CONTROLS;
    }
    paraLevel&=1;                       /* accept only 0 or 1 */
    ubidi_سیٹ_Para(pBiDi, متن, length, paraLevel, خالی_اشاریہ, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        جاؤ cleanup3;
    }
    /* we cannot access directly pBiDi->levels since it is not yet set if
     * direction is not MIXED
     */
    سطحیں=ubidi_get_سطحیں(pBiDi, p_Error_رمز);
    uprv_memcpy(save_سطحیں, سطحیں, (size_t)pBiDi->length*حجم(UBiDiLevel));
    saveTrailingWSStart=pBiDi->trailingWSStart;
    save_لمبائی=pBiDi->length;
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
    visual_لمبائی=ubidi_writeReordered(pBiDi, visual_متن, length,
                                      (saveOptions & UBIDI_DO_MIRRORING) | UBIDI_اندرونی_RUNS_ONLY, p_Error_رمز);
    ubidi_getVisualMap(pBiDi, visualMap, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        جاؤ cleanup2;
    }
    pBiDi->reorderingOptions=saveOptions;

    pBiDi->reordering_منوال=UBIDI_REORDER_INVERSE_LIKE_براہ_راست;
    paraLevel^=1;
    /* Because what we did with reorderingOptions, visualText may be shorter
     * than the original text. But we don't want the levels memory to be
     * reallocated shorter than the original length, since we need to restore
     * the levels as after the first call to ubidi_setpara() before returning.
     * We will force mayAllocateText to false before the second call to
     * ubidi_setpara(), and will restore it afterwards.
     */
    save_May_Allocate_متن=pBiDi->may_Allocate_متن;
    pBiDi->may_Allocate_متن=جھوٹ;
    ubidi_سیٹ_Para(pBiDi, visual_متن, visual_لمبائی, paraLevel, خالی_اشاریہ, p_Error_رمز);
    pBiDi->may_Allocate_متن=save_May_Allocate_متن;
    ubidi_getRuns(pBiDi, p_Error_رمز);
    اگر(U_FAILURE(*p_Error_رمز)) {
        جاؤ cleanup1;
    }
    /* check if some runs must be split, count how many splits */
    addedRuns=0;
    تسلسل_گنتی=pBiDi->تسلسل_گنتی;
    runs=pBiDi->runs;
    visual_آغاز=0;
    برائے(i=0; i<تسلسل_گنتی; i++, visual_آغاز+=تسلسل_لمبائی) {
        تسلسل_لمبائی=runs[i].visual_حد_بندی-visual_آغاز;
        اگر(تسلسل_لمبائی<2) {
            جاری;
        }
        logical_آغاز=GET_اشاریہ(runs[i].logical_آغاز);
        برائے(j=logical_آغاز+1; j<logical_آغاز+تسلسل_لمبائی; j++) {
            index0=visualMap[j];
            index1=visualMap[j-1];
            اگر((BIDI_ABS(index0-index1)!=1) || (save_سطحیں[index0]!=save_سطحیں[index1])) {
                addedRuns++;
            }
        }
    }
    اگر(addedRuns) {
        اگر(get_Runs_حافظہ(pBiDi, تسلسل_گنتی+addedRuns)) {
            اگر(تسلسل_گنتی==1) {
                /* because we switch from UBiDi.simpleRuns to UBiDi.runs */
                pBiDi->runs_حافظہ[0]=runs[0];
            }
            runs=pBiDi->runs=pBiDi->runs_حافظہ;
            pBiDi->تسلسل_گنتی+=addedRuns;
        } ورنہ {
            *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
            جاؤ cleanup1;
        }
    }
    /* split runs which are not consecutive in source text */
    برائے(i=تسلسل_گنتی-1; i>=0; i--) {
        تسلسل_لمبائی= i==0 ? runs[0].visual_حد_بندی :
                          runs[i].visual_حد_بندی-runs[i-1].visual_حد_بندی;
        logical_آغاز=runs[i].logical_آغاز;
        اشاریہ_Odd_ثنائی_ہندسہ=GET_ODD_ثنائی_ہندسہ(logical_آغاز);
        logical_آغاز=GET_اشاریہ(logical_آغاز);
        اگر(تسلسل_لمبائی<2) {
            اگر(addedRuns) {
                runs[i+addedRuns]=runs[i];
            }
            logicalPos=visualMap[logical_آغاز];
            runs[i+addedRuns].logical_آغاز=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_سطحیں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
            جاری;
        }
        اگر(اشاریہ_Odd_ثنائی_ہندسہ) {
            آغاز=logical_آغاز;
            حد_بندی=logical_آغاز+تسلسل_لمبائی-1;
            قدم=1;
        } ورنہ {
            آغاز=logical_آغاز+تسلسل_لمبائی-1;
            حد_بندی=logical_آغاز;
            قدم=-1;
        }
        برائے(j=آغاز; j!=حد_بندی; j+=قدم) {
            index0=visualMap[j];
            index1=visualMap[j+قدم];
            اگر((BIDI_ABS(index0-index1)!=1) || (save_سطحیں[index0]!=save_سطحیں[index1])) {
                logicalPos=BIDI_MIN(visualMap[آغاز], index0);
                runs[i+addedRuns].logical_آغاز=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_سطحیں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
                runs[i+addedRuns].visual_حد_بندی=runs[i].visual_حد_بندی;
                runs[i].visual_حد_بندی-=BIDI_ABS(j-آغاز)+1;
                داخل_کرو_Remove=runs[i].داخل_کرو_Remove&(LRM_AFTER|RLM_AFTER);
                runs[i+addedRuns].داخل_کرو_Remove=داخل_کرو_Remove;
                runs[i].داخل_کرو_Remove&=~داخل_کرو_Remove;
                آغاز=j+قدم;
                addedRuns--;
            }
        }
        اگر(addedRuns) {
            runs[i+addedRuns]=runs[i];
        }
        logicalPos=BIDI_MIN(visualMap[آغاز], visualMap[حد_بندی]);
        runs[i+addedRuns].logical_آغاز=MAKE_اشاریہ_ODD_PAIR(logicalPos,
                                            save_سطحیں[logicalPos]^اشاریہ_Odd_ثنائی_ہندسہ);
    }

  cleanup1:
    /* restore initial paraLevel */
    pBiDi->paraLevel^=1;
  cleanup2:
    /* restore real text */
    pBiDi->متن=متن;
    pBiDi->length=save_لمبائی;
    pBiDi->original_لمبائی=length;
    pBiDi->direction=saveDirection;
    /* the saved levels should never excess levelsSize, but we check anyway */
    اگر(save_لمبائی>pBiDi->سطحیں_سائز) {
        save_لمبائی=pBiDi->سطحیں_سائز;
    }
    uprv_memcpy(pBiDi->سطحیں, save_سطحیں, (size_t)save_لمبائی*حجم(UBiDiLevel));
    pBiDi->trailingWSStart=saveTrailingWSStart;
    اگر(pBiDi->تسلسل_گنتی>1) {
        pBiDi->direction=UBIDI_MIXED;
    }
  cleanup3:
    /* free memory for mapping table and visual text */
    uprv_free(runs_Only_حافظہ);

    pBiDi->reordering_منوال=UBIDI_REORDER_RUNS_ONLY;
}

/* ubidi_setPara ------------------------------------------------------------ */

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Para(UBiDi *pBiDi, ثابت char16_t *متن, int32_t length,
              UBiDiLevel paraLevel, UBiDiLevel *embedding_سطحیں,
              UError_رمز *p_Error_رمز) {
    UBiDiDirection direction;
    DirProp *dirProps;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    اگر(pBiDi==خالی_اشاریہ || متن==خالی_اشاریہ || length<-1 ||
       (paraLevel>UBIDI_MAX_EXPLICIT_LEVEL && paraLevel<UBIDI_DEFAULT_LTR)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر(length==-1) {
        length=u_strlen(متن);
    }

    /* special treatment for RUNS_ONLY mode */
    اگر(pBiDi->reordering_منوال==UBIDI_REORDER_RUNS_ONLY) {
        سیٹ_Para_Runs_Only(pBiDi, متن, length, paraLevel, p_Error_رمز);
        واپس;
    }

    /* initialize the UBiDi structure */
    pBiDi->pParaBiDi=خالی_اشاریہ;          /* mark unfinished setPara */
    pBiDi->متن=متن;
    pBiDi->length=pBiDi->original_لمبائی=pBiDi->نتیجہ_لمبائی=length;
    pBiDi->paraLevel=paraLevel;
    pBiDi->direction=(UBiDiDirection)(paraLevel&1);
    pBiDi->para_گنتی=1;

    pBiDi->dirProps=خالی_اشاریہ;
    pBiDi->سطحیں=خالی_اشاریہ;
    pBiDi->runs=خالی_اشاریہ;
    pBiDi->داخل_کرو_Points.size=0;         /* clean up from last call */
    pBiDi->داخل_کرو_Points.confirmed=0;    /* clean up from last call */

    /*
     * Save the original paraLevel if contextual; otherwise, set to 0.
     */
    pBiDi->defaultParaLevel=IS_DEFAULT_LEVEL(paraLevel);

    اگر(length==0) {
        /*
         * For an empty paragraph, create a UBiDi object with the paraLevel and
         * the flags and the direction set but without allocating zero-length arrays.
         * There is nothing more to do.
         */
        اگر(IS_DEFAULT_LEVEL(paraLevel)) {
            pBiDi->paraLevel&=1;
            pBiDi->defaultParaLevel=0;
        }
        pBiDi->flags=DIRPROP_FLAG_LR(paraLevel);
        pBiDi->تسلسل_گنتی=0;
        pBiDi->para_گنتی=0;
        سیٹ_Para_Success(pBiDi);          /* mark successful setPara */
        واپس;
    }

    pBiDi->تسلسل_گنتی=-1;

    /* allocate paras memory */
    اگر(pBiDi->paras_حافظہ)
        pBiDi->paras=pBiDi->paras_حافظہ;
    ورنہ
        pBiDi->paras=pBiDi->simpleParas;

    /*
     * Get the directional properties,
     * the flags bit-set, and
     * determine the paragraph level if necessary.
     */
    اگر(get_Dir_Props_حافظہ(pBiDi, length)) {
        pBiDi->dirProps=pBiDi->dir_Props_حافظہ;
        اگر(!getDirProps(pBiDi)) {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    } ورنہ {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    dirProps=pBiDi->dirProps;
    /* the processed length may have changed if UBIDI_OPTION_STREAMING */
    length= pBiDi->length;
    pBiDi->trailingWSStart=length;  /* the levels[] will reflect the WS run */

    /* are explicit levels specified? */
    اگر(embedding_سطحیں==خالی_اشاریہ) {
        /* no: determine explicit levels according to the (Xn) rules */\
        اگر(get_سطحیں_حافظہ(pBiDi, length)) {
            pBiDi->سطحیں=pBiDi->سطحیں_حافظہ;
            direction=resolve_Explicit_سطحیں(pBiDi, p_Error_رمز);
            اگر(U_FAILURE(*p_Error_رمز)) {
                واپس;
            }
        } ورنہ {
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس;
        }
    } ورنہ {
        /* set BN for all explicit codes, check that all levels are 0 or paraLevel..UBIDI_MAX_EXPLICIT_LEVEL */
        pBiDi->سطحیں=embedding_سطحیں;
        direction=check_Explicit_سطحیں(pBiDi, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس;
        }
    }

    /* allocate isolate memory */
    اگر(pBiDi->isolate_گنتی<=SIMPLE_ISOLATES_گنتی)
        pBiDi->isolates=pBiDi->simpleIsolates;
    ورنہ
        اگر((int32_t)(pBiDi->isolate_گنتی*حجم(Isolate))<=pBiDi->isolates_سائز)
            pBiDi->isolates=pBiDi->isolates_حافظہ;
        ورنہ {
            اگر(get_Initial_Isolates_حافظہ(pBiDi, pBiDi->isolate_گنتی)) {
                pBiDi->isolates=pBiDi->isolates_حافظہ;
            } ورنہ {
                *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
                واپس;
            }
        }
    pBiDi->isolate_گنتی=-1;             /* current isolates stack entry == none */

    /*
     * The steps after (X9) in the UBiDi algorithm are performed only if
     * the paragraph text has mixed directionality!
     */
    pBiDi->direction=direction;
    انتخاب(direction) {
    حالت UBIDI_LTR:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        توڑو;
    حالت UBIDI_RTL:
        /* all levels are implicitly at paraLevel (important for ubidi_getLevels()) */
        pBiDi->trailingWSStart=0;
        توڑو;
    طےشدہ:
        /*
         *  Choose the right implicit state table
         */
        انتخاب(pBiDi->reordering_منوال) {
        حالت UBIDI_REORDER_DEFAULT:
            pBiDi->pImpTabPair=&impTab_DEFAULT;
            توڑو;
        حالت UBIDI_REORDER_NUMBERS_SPECIAL:
            pBiDi->pImpTabPair=&impTab_NUMBERS_SPECIAL;
            توڑو;
        حالت UBIDI_REORDER_گروہ_NUMBERS_WITH_R:
            pBiDi->pImpTabPair=&imp_Tab_گروہ_NUMBERS_WITH_R;
            توڑو;
        حالت UBIDI_REORDER_INVERSE_NUMBERS_AS_L:
            pBiDi->pImpTabPair=&impTab_INVERSE_NUMBERS_AS_L;
            توڑو;
        حالت UBIDI_REORDER_INVERSE_LIKE_براہ_راست:
            اگر (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_براہ_راست_WITH_MARKS;
            } ورنہ {
                pBiDi->pImpTabPair=&imp_Tab_INVERSE_LIKE_براہ_راست;
            }
            توڑو;
        حالت UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL:
            اگر (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL_WITH_MARKS;
            } ورنہ {
                pBiDi->pImpTabPair=&impTab_INVERSE_FOR_NUMBERS_SPECIAL;
            }
            توڑو;
        طےشدہ:
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
        اگر(embedding_سطحیں==خالی_اشاریہ && pBiDi->para_گنتی<=1 &&
                                   !(pBiDi->flags&DIRPROP_FLAG_MULTI_RUNS)) {
            resolve_Implicit_سطحیں(pBiDi, 0, length,
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, 0)),
                                    GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, length-1)));
        } ورنہ {
            /* sor, eor: start and end types of same-level-run */
            UBiDiLevel *سطحیں=pBiDi->سطحیں;
            int32_t آغاز, حد_بندی=0;
            UBiDiLevel level, اگلا_Level;
            DirProp sor, eor;

            /* determine the first sor and set eor to it because of the loop body (sor=eor there) */
            level=GET_PARALEVEL(pBiDi, 0);
            اگلا_Level=سطحیں[0];
            اگر(level<اگلا_Level) {
                eor=GET_LR_FROM_LEVEL(اگلا_Level);
            } ورنہ {
                eor=GET_LR_FROM_LEVEL(level);
            }

            کرو {
                /* determine start and limit of the run (end points just behind the run) */

                /* the values for this run's start are the same as for the previous run's end */
                آغاز=حد_بندی;
                level=اگلا_Level;
                اگر((آغاز>0) && (dirProps[آغاز-1]==B)) {
                    /* except if this is a new paragraph, then set sor = para level */
                    sor=GET_LR_FROM_LEVEL(GET_PARALEVEL(pBiDi, آغاز));
                } ورنہ {
                    sor=eor;
                }

                /* search for the limit of this run */
                جب((++حد_بندی<length) &&
                      ((سطحیں[حد_بندی]==level) ||
                       (DIRPROP_FLAG(dirProps[حد_بندی])&نقاب_BN_EXPLICIT))) {}

                /* get the correct level of the next run */
                اگر(حد_بندی<length) {
                    اگلا_Level=سطحیں[حد_بندی];
                } ورنہ {
                    اگلا_Level=GET_PARALEVEL(pBiDi, length-1);
                }

                /* determine eor from max(level, nextLevel); sor is last run's eor */
                اگر(NO_OVERRIDE(level)<NO_OVERRIDE(اگلا_Level)) {
                    eor=GET_LR_FROM_LEVEL(اگلا_Level);
                } ورنہ {
                    eor=GET_LR_FROM_LEVEL(level);
                }

                /* if the run consists of overridden directional types, then there
                   are no implicit types to be resolved */
                اگر(!(level&UBIDI_LEVEL_OVERRIDE)) {
                    resolve_Implicit_سطحیں(pBiDi, آغاز, حد_بندی, sor, eor);
                } ورنہ {
                    /* remove the UBIDI_LEVEL_OVERRIDE flags */
                    کرو {
                        سطحیں[آغاز++]&=~UBIDI_LEVEL_OVERRIDE;
                    } جب(آغاز<حد_بندی);
                }
            } جب(حد_بندی<length);
        }
        /* check if we got any memory shortage while adding insert points */
        اگر (U_FAILURE(pBiDi->داخل_کرو_Points.error_رمز))
        {
            *p_Error_رمز=pBiDi->داخل_کرو_Points.error_رمز;
            واپس;
        }
        /* reset the embedding levels for some non-graphic characters (L1), (X9) */
        adjustWSLevels(pBiDi);
        توڑو;
    }
    /* add RLM for inverse Bidi with contextual orientation resolving
     * to RTL which would not round-trip otherwise
     */
    اگر((pBiDi->defaultParaLevel>0) &&
       (pBiDi->reorderingOptions & UBIDI_OPTION_داخل_کرو_MARKS) &&
       ((pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_LIKE_براہ_راست) ||
        (pBiDi->reordering_منوال==UBIDI_REORDER_INVERSE_FOR_NUMBERS_SPECIAL))) {
        int32_t i, j, آغاز, آخری;
        UBiDiLevel level;
        DirProp dirProp;
        برائے(i=0; i<pBiDi->para_گنتی; i++) {
            آخری=(pBiDi->paras[i].حد_بندی)-1;
            level= static_cast<UBiDiLevel>(pBiDi->paras[i].level);
            اگر(level==0)
                جاری;           /* LTR paragraph */
            آغاز= i==0 ? 0 : pBiDi->paras[i-1].حد_بندی;
            برائے(j=آخری; j>=آغاز; j--) {
                dirProp=dirProps[j];
                اگر(dirProp==L) {
                    اگر(j<آخری) {
                        جب(dirProps[آخری]==B) {
                            آخری--;
                        }
                    }
                    جوڑو_نقطہ(pBiDi, آخری, RLM_BEFORE);
                    توڑو;
                }
                اگر(DIRPROP_FLAG(dirProp) & نقاب_R_AL) {
                    توڑو;
                }
            }
        }
    }

    اگر(pBiDi->reorderingOptions & UBIDI_OPTION_REMOVE_CONTROLS) {
        pBiDi->نتیجہ_لمبائی -= pBiDi->control_گنتی;
    } ورنہ {
        pBiDi->نتیجہ_لمبائی += pBiDi->داخل_کرو_Points.size;
    }
    سیٹ_Para_Success(pBiDi);              /* mark successful setPara */
}

U_CAPI خالی U_EXPORT2
ubidi_ترتیب_Paragraphs_LTR(UBiDi *pBiDi, UBool ترتیب_Paragraphs_LTR) {
    اگر(pBiDi!=خالی_اشاریہ) {
        pBiDi->ترتیب_Paragraphs_LTR=ترتیب_Paragraphs_LTR;
    }
}

U_CAPI UBool U_EXPORT2
ubidi_is_ترتیب_Paragraphs_LTR(UBiDi *pBiDi) {
    اگر(pBiDi!=خالی_اشاریہ) {
        واپس pBiDi->ترتیب_Paragraphs_LTR;
    } ورنہ {
        واپس جھوٹ;
    }
}

U_CAPI UBiDiDirection U_EXPORT2
ubidi_getDirection(ثابت UBiDi *pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->direction;
    } ورنہ {
        واپس UBIDI_LTR;
    }
}

U_CAPI ثابت char16_t* U_EXPORT2
ubidi_get_متن(ثابت UBiDi* pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->متن;
    } ورنہ {
        واپس خالی_اشاریہ;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_لمبائی(ثابت UBiDi *pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->original_لمبائی;
    } ورنہ {
        واپس 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_Processed_لمبائی(ثابت UBiDi *pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->length;
    } ورنہ {
        واپس 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_get_نتیجہ_لمبائی(ثابت UBiDi *pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->نتیجہ_لمبائی;
    } ورنہ {
        واپس 0;
    }
}

/* paragraphs API functions ------------------------------------------------- */

U_CAPI UBiDiLevel U_EXPORT2
ubidi_getParaLevel(ثابت UBiDi *pBiDi) {
    اگر(IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس pBiDi->paraLevel;
    } ورنہ {
        واپس 0;
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_گنتی_Paragraphs(UBiDi *pBiDi) {
    اگر(!IS_VALID_PARA_OR_سطر(pBiDi)) {
        واپس 0;
    } ورنہ {
        واپس pBiDi->para_گنتی;
    }
}

U_CAPI خالی U_EXPORT2
ubidi_get_Paragraph_By_اشاریہ(ثابت UBiDi *pBiDi, int32_t para_اشاریہ,
                          int32_t *p_Para_آغاز, int32_t *p_Para_حد_بندی,
                          UBiDiLevel *pParaLevel, UError_رمز *p_Error_رمز) {
    int32_t para_آغاز;

    /* check the argument values */
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    RETURN_VOID_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز);
    RETURN_VOID_IF_BAD_RANGE(para_اشاریہ, 0, pBiDi->para_گنتی, *p_Error_رمز);

    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    اگر(para_اشاریہ) {
        para_آغاز=pBiDi->paras[para_اشاریہ-1].حد_بندی;
    } ورنہ {
        para_آغاز=0;
    }
    اگر(p_Para_آغاز!=خالی_اشاریہ) {
        *p_Para_آغاز=para_آغاز;
    }
    اگر(p_Para_حد_بندی!=خالی_اشاریہ) {
        *p_Para_حد_بندی=pBiDi->paras[para_اشاریہ].حد_بندی;
    }
    اگر(pParaLevel!=خالی_اشاریہ) {
        *pParaLevel=GET_PARALEVEL(pBiDi, para_آغاز);
    }
}

U_CAPI int32_t U_EXPORT2
ubidi_getParagraph(ثابت UBiDi *pBiDi, int32_t char_اشاریہ,
                          int32_t *p_Para_آغاز, int32_t *p_Para_حد_بندی,
                          UBiDiLevel *pParaLevel, UError_رمز *p_Error_رمز) {
    int32_t para_اشاریہ;

    /* check the argument values */
    /* pErrorCode will be checked by the call to ubidi_getParagraphByIndex */
    RETURN_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز, -1);
    RETURN_IF_NOT_VALID_PARA_OR_سطر(pBiDi, *p_Error_رمز, -1);
    pBiDi=pBiDi->pParaBiDi;             /* get Para object if Line object */
    RETURN_IF_BAD_RANGE(char_اشاریہ, 0, pBiDi->length, *p_Error_رمز, -1);

    برائے(para_اشاریہ=0; char_اشاریہ>=pBiDi->paras[para_اشاریہ].حد_بندی; para_اشاریہ++);
    ubidi_get_Paragraph_By_اشاریہ(pBiDi, para_اشاریہ, p_Para_آغاز, p_Para_حد_بندی, pParaLevel, p_Error_رمز);
    واپس para_اشاریہ;
}

U_CAPI خالی U_EXPORT2
ubidi_سیٹ_Class_Callback(UBiDi *pBiDi, UBiDiClassCallback *newFn,
                       ثابت خالی *newContext, UBiDiClassCallback **oldFn,
                       ثابت خالی **oldContext, UError_رمز *p_Error_رمز)
{
    RETURN_VOID_IF_NULL_OR_FAILING_ERRCODE(p_Error_رمز);
    اگر(pBiDi==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }
    اگر( oldFn )
    {
        *oldFn = pBiDi->fnClassCallback;
    }
    اگر( oldContext )
    {
        *oldContext = pBiDi->coClassCallback;
    }
    pBiDi->fnClassCallback = newFn;
    pBiDi->coClassCallback = newContext;
}

U_CAPI خالی U_EXPORT2
ubidi_getClassCallback(UBiDi *pBiDi, UBiDiClassCallback **fn, ثابت خالی **context)
{
    اگر(pBiDi==خالی_اشاریہ) {
        واپس;
    }
    اگر( fn )
    {
        *fn = pBiDi->fnClassCallback;
    }
    اگر( context )
    {
        *context = pBiDi->coClassCallback;
    }
}

U_CAPI UCharDirection U_EXPORT2
ubidi_getCustomizedClass(UBiDi *pBiDi, UChar32 c)
{
    UCharDirection dir;

    اگر( pBiDi->fnClassCallback == خالی_اشاریہ ||
        (dir = (*pBiDi->fnClassCallback)(pBiDi->coClassCallback, c)) == U_BIDI_CLASS_DEFAULT )
    {
        dir = ubidi_getClass(c);
    }
    اگر(dir >= U_CHAR_DIRECTION_گنتی) {
        dir = (UCharDirection)ON;
    }
    واپس dir;
}

/* Utilities for MessageFormat ------------------------------------------------*/

#اگر U_SHOW_CPLUSPLUS_API

U_NAMESPACE_BEGIN

U_CAPI UMFBidiOption U_EXPORT2
ubidi_getMFOption(ثابت UnicodeString& s) {
    اگر (s == u"ltr") {
        واپس U_MF_BIDI_OPTION_LTR;
    }
    اگر (s == u"rtl") {
        واپس U_MF_BIDI_OPTION_RTL;
    }
    اگر (s == u"auto") {
        واپس U_MF_BIDI_OPTION_AUTO;
    }
    واپس U_MF_BIDI_OPTION_INHERIT; // inherit is default
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
    اگر (bidiIsolationStrategy == U_MF_BIDI_OFF)
        واپس &fmt;

    /* 1. Let msgdir be the directionality of the whole message, one of « 'LTR', 'RTL', 'unknown' ». These correspond to the message having left-to-right directionality, right-to-left directionality, and to the message's directionality not being known. */

    // 2i Let fmt be the formatted string representation of the resolved value of exp.
    // (Passed as argument)

    // 2ii Let dir be the directionality of fmt, one of « 'LTR', 'RTL', 'unknown' », with the same meanings as for msgdir
    // (Passed as argument)

    // 2iii. Let the boolean value isolate be True if the u:dir option of the resolved value of exp has a value other than 'inherit', or False otherwise.
    منطقی isolate = uDirOption != U_MF_BIDI_OPTION_INHERIT;

    UnicodeString bdiOpen("<bdi>");
    UnicodeString bdiClose("</bdi>");

    // 2iv. If dir is 'LTR'
    انتخاب (dir) {
        حالت U_MF_DIRECTIONALITY_LTR:
            اگر (msgdir == U_MF_DIRECTIONALITY_LTR && !isolate) {
                // 2iv(a). If msgdir is 'LTR' in the formatted output, let fmt be itself
                واپس &fmt;
            }
            // 2iii(b) Else, in the formatted output, prefix fmt with U+2066 LEFT-TO-RIGHT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            اگر (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, LRI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ورنہ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            توڑو; // End of 2iii
        // 2iv. Else, if dir is 'RTL':
        حالت U_MF_DIRECTIONALITY_RTL:
            // 2iv(a). In the formatted output, prefix fmt with U+2067 RIGHT-TO-LEFT ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            اگر (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, RLI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ورنہ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            توڑو; // End of 2iv.
        // 2v. Else:
        طےشدہ:
            // 2v(a). In the formatted output, prefix fmt with U+2068 FIRST STRONG ISOLATE and postfix it with U+2069 POP DIRECTIONAL ISOLATE.
            اگر (bidiIsolationStyle == U_MF_BIDI_STYLE_CONTROL) {
                fmt.insert(0, FSI_CHAR);
                fmt.insert(fmt.length(), PDI_CHAR);
            } ورنہ {
                fmt.insert(0, bdiOpen);
                fmt.insert(fmt.length(), bdiClose);
            }
            توڑو; // End of 2v
    } // `fmt` now contains the isolated string
    واپس &fmt;
}

U_NAMESPACE_انتہا

#اگر_ختم // #if SHOW_CPLUSPLUS_API

