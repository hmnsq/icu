/*
******************************************************************************
*
* © 2016 and later: Unicode, Inc. and others.
* License & terms of use: http://www.unicode.org/copyright.html
*
******************************************************************************
*   file name:  ubiditransform.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2016jul24
*   created by: Lina Kemmel
*
*/

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ubidi.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/ushape.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "unicode/ubiditransform.h"

/* Some convenience defines */
#నిర్వచించు LTR                     UBIDI_LTR
#నిర్వచించు RTL                     UBIDI_RTL
#నిర్వచించు LOGICAL                 UBIDI_LOGICAL
#నిర్వచించు VISUAL                  UBIDI_VISUAL
#నిర్వచించు SHAPE_LOGICAL           U_SHAPE_పాఠ్యం_DIRECTION_LOGICAL
#నిర్వచించు SHAPE_VISUAL            U_SHAPE_పాఠ్యం_DIRECTION_VISUAL_LTR

#నిర్వచించు CHECK_LEN(STR, LEN, ERROR) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ (LEN == 0) తిరిగి 0; \
    ఒకవేళ (LEN < -1) { *(ERROR) = U_ILLEGAL_వాదం_ERROR; తిరిగి 0; } \
    ఒకవేళ (LEN == -1) LEN = u_strlen(STR); \
} UPRV_ఖండం_MACRO_చివర

#నిర్వచించు MAX_ACTIONS     7

/**
 * Typedef for a pointer to a function, which performs some operation (such as
 * reordering, setting "inverse" mode, character mirroring, etc.). Return value
 * indicates whether the text was changed in the course of this operation or
 * not.
 */
రకనిర్వచనం UBool (*UBiDiAction)(UBiDiTransform *, UError_సంకేతం *);

/**
 * Structure that holds a predefined reordering scheme, including the following
 * information:
 * <ul>
 * <li>an input base direction,</li>
 * <li>an input order,</li>
 * <li>an output base direction,</li>
 * <li>an output order,</li>
 * <li>a digit shaping direction,</li>
 * <li>a letter shaping direction,</li>
 * <li>a base direction that should be applied when the reordering engine is
 *     invoked (which can not always be derived from the caller-defined
 *     options),</li>
 * <li>an array of pointers to functions that accomplish the bidi layout
 *     transformation.</li>
 * </ul>
 */
రకనిర్వచనం నిర్మాణం {
    UBiDiLevel        లోపల_Level;               /* input level */
    UBi_Di_క్రమం        లోపల_క్రమం;               /* input order */
    UBiDiLevel        బహిర్గతం_Level;              /* output level */
    UBi_Di_క్రమం        బహిర్గతం_క్రమం;              /* output order */
    uint32_t          digitsDir;             /* digit shaping direction */
    uint32_t          lettersDir;            /* letter shaping direction */
    UBiDiLevel        పునాది_Level;             /* paragraph level to be used with setPara */
    స్థిరరాశి UBiDiAction actions[MAX_ACTIONS];  /* array of pointers to functions carrying out the transformation */
} ReorderingScheme;

నిర్మాణం UBiDiTransform {
    UBiDi                   *pBidi;             /* pointer to a UBiDi object */
    స్థిరరాశి ReorderingScheme  *p_చురుకైన_Scheme;     /* effective reordering scheme */
    char16_t                *src;               /* input text */
    char16_t                *dest;              /* output text */
    uint32_t                src_నిడివి;          /* input text length - not really needed as we are zero-terminated and can u_strlen */
    uint32_t                src_కొలత;            /* input text capacity excluding the trailing zero */
    uint32_t                dest_కొలత;           /* output text capacity */
    uint32_t                *p_Dest_నిడివి;       /* number of UChars written to dest */
    uint32_t                reorderingOptions;  /* reordering options - currently only suppot DO_MIRRORING */
    uint32_t                digits;             /* digit option for ArabicShaping */
    uint32_t                letters;            /* letter option for ArabicShaping */
};

U_CAPI UBiDiTransform* U_EXPORT2
ubiditransform_open(UError_సంకేతం *p_Error_సంకేతం)
{
    UBiDiTransform *pBiDiTransform = శూన్యసూచిక;
    ఒకవేళ (U_SUCCESS(*p_Error_సంకేతం)) {
        pBiDiTransform = (UBiDiTransform*) uprv_calloc(1, పరిమాణం(UBiDiTransform));
        ఒకవేళ (pBiDiTransform == శూన్యసూచిక) {
            *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        }
    }
    తిరిగి pBiDiTransform;
}

U_CAPI శూన్య U_EXPORT2
ubiditransform_close(UBiDiTransform *pBiDiTransform)
{
    ఒకవేళ (pBiDiTransform != శూన్యసూచిక) {
        ఒకవేళ (pBiDiTransform->pBidi != శూన్యసూచిక) {
            ubidi_close(pBiDiTransform->pBidi);
        }
        ఒకవేళ (pBiDiTransform->src != శూన్యసూచిక) {
            uprv_free(pBiDiTransform->src);
        }
        uprv_free(pBiDiTransform);
    }
}

/**
 * Performs Bidi resolution of text.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_resolve(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    ubidi_సమితి_Para(pTransform->pBidi, pTransform->src, pTransform->src_నిడివి,
            pTransform->p_చురుకైన_Scheme->పునాది_Level, శూన్యసూచిక, p_Error_సంకేతం);
    తిరిగి అబద్ధం;
}

/**
 * Performs basic reordering of text (Logical -> Visual LTR).
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_reorder(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    ubidi_writeReordered(pTransform->pBidi, pTransform->dest, pTransform->dest_కొలత,
            static_cast<uint16_t>(pTransform->reorderingOptions), p_Error_సంకేతం);

    *pTransform->p_Dest_నిడివి = pTransform->src_నిడివి;
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    తిరిగి నిజం;
}

/**
 * Sets "inverse" mode on the <code>UBiDi</code> object.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_సమితి_Inverse(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    (శూన్య)p_Error_సంకేతం;
    ubidi_సమితి_Inverse(pTransform->pBidi, నిజం);
    ubidi_సమితి_Reordering_బహుళకం(pTransform->pBidi, UBIDI_REORDER_INVERSE_LIKE_ప్రత్యక్ష);
    తిరిగి అబద్ధం;
}

/**
 * Sets "runs only" reordering mode indicating a Logical LTR <-> Logical RTL
 * transformation.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_సమితి_Runs_Only(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    (శూన్య)p_Error_సంకేతం;
    ubidi_సమితి_Reordering_బహుళకం(pTransform->pBidi, UBIDI_REORDER_RUNS_ONLY);
    తిరిగి అబద్ధం;
}

/**
 * Performs string reverse.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_తిరగేయి(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    ubidi_write_తిరగేయి(pTransform->src, pTransform->src_నిడివి,
            pTransform->dest, pTransform->dest_కొలత,
            UBIDI_REORDER_DEFAULT, p_Error_సంకేతం);
    *pTransform->p_Dest_నిడివి = pTransform->src_నిడివి;
    తిరిగి నిజం;
}

/**
 * Applies a new value to the text that serves as input at the current
 * processing step. This value is identical to the original one when we begin
 * the processing, but usually changes as the transformation progresses.
 * 
 * @param pTransform A pointer to the <code>UBiDiTransform</code> structure.
 * @param newSrc A pointer whose value is to be used as input text.
 * @param newLength A length of the new text in <code>char16_t</code>s.
 * @param newSize A new source capacity in <code>char16_t</code>s.
 * @param pErrorCode Pointer to the error code value.
 */
స్థిర శూన్య
updateSrc(UBiDiTransform *pTransform, స్థిరరాశి char16_t *newSrc, uint32_t new_నిడివి,
        uint32_t new_కొలత, UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ (new_కొలత < new_నిడివి) {
        *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి;
    }
    ఒకవేళ (new_కొలత > pTransform->src_కొలత) {
        new_కొలత += 50; // allocate slightly more than needed right now
        ఒకవేళ (pTransform->src != శూన్యసూచిక) {
            uprv_free(pTransform->src);
            pTransform->src = శూన్యసూచిక;
        }
        pTransform->src = static_cast<char16_t*>(uprv_malloc(new_కొలత * పరిమాణం(char16_t)));
        ఒకవేళ (pTransform->src == శూన్యసూచిక) {
            *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            //pTransform->srcLength = pTransform->srcSize = 0;
            తిరిగి;
        }
        pTransform->src_కొలత = new_కొలత;
    }
    u_strncpy(pTransform->src, newSrc, new_నిడివి);
    pTransform->src_నిడివి = u_terminateUChars(pTransform->src,
    		pTransform->src_కొలత, new_నిడివి, p_Error_సంకేతం);
}

/**
 * Calls a lower level shaping function.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param options Shaping options.
 * @param pErrorCode Pointer to the error code value.
 */
స్థిర శూన్య
doShape(UBiDiTransform *pTransform, uint32_t options, UError_సంకేతం *p_Error_సంకేతం)
{
    *pTransform->p_Dest_నిడివి = u_shapeArabic(pTransform->src,
            pTransform->src_నిడివి, pTransform->dest, pTransform->dest_కొలత,
            options, p_Error_సంకేతం);
}

/**
 * Performs digit and letter shaping.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_shapeArabic(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ ((pTransform->letters | pTransform->digits) == 0) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ (pTransform->p_చురుకైన_Scheme->lettersDir == pTransform->p_చురుకైన_Scheme->digitsDir) {
        doShape(pTransform, pTransform->letters | pTransform->digits | pTransform->p_చురుకైన_Scheme->lettersDir,
                p_Error_సంకేతం);
    } లేకపోతే {
        doShape(pTransform, pTransform->digits | pTransform->p_చురుకైన_Scheme->digitsDir, p_Error_సంకేతం);
        ఒకవేళ (U_SUCCESS(*p_Error_సంకేతం)) {
            updateSrc(pTransform, pTransform->dest, *pTransform->p_Dest_నిడివి,
                    *pTransform->p_Dest_నిడివి, p_Error_సంకేతం);
            doShape(pTransform, pTransform->letters | pTransform->p_చురుకైన_Scheme->lettersDir,
                    p_Error_సంకేతం);
        }
    }
    తిరిగి నిజం;
}

/**
 * Performs character mirroring.
 * 
 * @param pTransform Pointer to the <code>UBiDiTransform</code> structure.
 * @param pErrorCode Pointer to the error code value.
 *
 * @return Whether or not this function modifies the text. Besides the return
 * value, the caller should also check <code>U_SUCCESS(*pErrorCode)</code>.
 */
స్థిర UBool
action_mirror(UBiDiTransform *pTransform, UError_సంకేతం *p_Error_సంకేతం)
{
    UChar32 c;
    uint32_t i = 0, j = 0;
    ఒకవేళ (0 == (pTransform->reorderingOptions & UBIDI_DO_MIRRORING)) {
        తిరిగి అబద్ధం;
    }
    చేయి {
        UBool isOdd = ubidi_getLevelAt(pTransform->pBidi, i) & 1;
        U16_తదుపరి(pTransform->src, i, pTransform->src_నిడివి, c); 
        ఒకవేళ (isOdd) {
            c = u_charMirror(c);
        }
        uint32_t k = U16_నిడివి(c);
        ఒకవేళ (j + k <= pTransform->dest_కొలత) {
            U16_APPEND_UNSAFE(pTransform->dest, j, c);
        } లేకపోతే {
            j += k;
        }
    } అయితే (i < pTransform->src_నిడివి);
    
    *pTransform->p_Dest_నిడివి = j;
    ఒకవేళ (pTransform->dest_కొలత < j) {
        *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        తిరిగి అబద్ధం;
    }
    pTransform->reorderingOptions = UBIDI_REORDER_DEFAULT;
    తిరిగి నిజం;
}

/**
 * All possible reordering schemes.
 *
 */
స్థిర స్థిరరాశి ReorderingScheme Schemes[] =
{
    /* 0: Logical LTR => Visual LTR */
    {LTR, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, శూన్యసూచిక}},
    /* 1: Logical RTL => Visual LTR */
    {RTL, LOGICAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, శూన్యసూచిక}},
    /* 2: Logical LTR => Visual RTL */
    {LTR, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_reorder, action_తిరగేయి, శూన్యసూచిక}},
    /* 3: Logical RTL => Visual RTL */
    {RTL, LOGICAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_resolve, action_reorder, action_shapeArabic, action_తిరగేయి, శూన్యసూచిక}},
    /* 4: Visual LTR => Logical RTL */
    {LTR, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_shapeArabic, action_సమితి_Inverse, action_resolve, action_reorder, శూన్యసూచిక}},
    /* 5: Visual RTL => Logical RTL */
    {RTL, VISUAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_VISUAL, RTL,
            {action_తిరగేయి, action_shapeArabic, action_సమితి_Inverse, action_resolve, action_reorder, శూన్యసూచిక}},
    /* 6: Visual LTR => Logical LTR */
    {LTR, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_సమితి_Inverse, action_resolve, action_reorder, action_shapeArabic, శూన్యసూచిక}},
    /* 7: Visual RTL => Logical LTR */
    {RTL, VISUAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_తిరగేయి, action_సమితి_Inverse, action_resolve, action_reorder, action_shapeArabic, శూన్యసూచిక}},
    /* 8: Logical LTR => Logical RTL */
    {LTR, LOGICAL, RTL, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_shapeArabic, action_resolve, action_mirror, action_సమితి_Runs_Only, action_resolve, action_reorder, శూన్యసూచిక}},
    /* 9: Logical RTL => Logical LTR */
    {RTL, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_సమితి_Runs_Only, action_resolve, action_reorder, action_shapeArabic, శూన్యసూచిక}},
    /* 10: Visual LTR => Visual RTL */
    {LTR, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_shapeArabic, action_సమితి_Inverse, action_resolve, action_mirror, action_తిరగేయి, శూన్యసూచిక}},
    /* 11: Visual RTL => Visual LTR */
    {RTL, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_తిరగేయి, action_shapeArabic, action_సమితి_Inverse, action_resolve, action_mirror, శూన్యసూచిక}},
    /* 12: Logical LTR => Logical LTR */
    {LTR, LOGICAL, LTR, LOGICAL, SHAPE_LOGICAL, SHAPE_LOGICAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, శూన్యసూచిక}},
    /* 13: Logical RTL => Logical RTL */
    {RTL, LOGICAL, RTL, LOGICAL, SHAPE_VISUAL, SHAPE_LOGICAL, RTL,
            {action_resolve, action_mirror, action_shapeArabic, శూన్యసూచిక}},
    /* 14: Visual LTR => Visual LTR */
    {LTR, VISUAL, LTR, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_resolve, action_mirror, action_shapeArabic, శూన్యసూచిక}},
    /* 15: Visual RTL => Visual RTL */
    {RTL, VISUAL, RTL, VISUAL, SHAPE_LOGICAL, SHAPE_VISUAL, LTR,
            {action_తిరగేయి, action_resolve, action_mirror, action_shapeArabic, action_తిరగేయి, శూన్యసూచిక}}
};

స్థిర స్థిరరాశి uint32_t nSchemes = పరిమాణం(Schemes) / పరిమాణం(*Schemes);

/**
 * When the direction option is <code>UBIDI_DEFAULT_LTR</code> or
 * <code>UBIDI_DEFAULT_RTL</code>, resolve the base direction according to that
 * of the first strong bidi character.
 */
స్థిర శూన్య
resolve_పునాది_Direction(స్థిరరాశి char16_t *పాఠ్యం, uint32_t length,
        UBiDiLevel *p_లోపల_Level, UBiDiLevel *p_బహిర్గతం_Level)
{
    ఎంపిక (*p_లోపల_Level) {
        స్థితి UBIDI_DEFAULT_LTR:
        స్థితి UBIDI_DEFAULT_RTL: {
            UBiDiLevel level = static_cast<UBiDiLevel>(ubidi_get_పునాది_Direction(పాఠ్యం, length));
            *p_లోపల_Level = static_cast<UBiDiLevel>(level != UBIDI_NEUTRAL) ? level
                    : *p_లోపల_Level == UBIDI_DEFAULT_RTL ? static_cast<UBiDiLevel>(RTL) : static_cast<UBiDiLevel>(LTR);
            ఆపు;
        }
        అప్రమేయ:
            *p_లోపల_Level &= 1;
            ఆపు;
    }
    ఎంపిక (*p_బహిర్గతం_Level) {
        స్థితి UBIDI_DEFAULT_LTR:
        స్థితి UBIDI_DEFAULT_RTL:
            *p_బహిర్గతం_Level = *p_లోపల_Level;
            ఆపు;
        అప్రమేయ:
            *p_బహిర్గతం_Level &= 1;
            ఆపు;
    }
}

/**
 * Finds a valid <code>ReorderingScheme</code> matching the
 * caller-defined scheme.
 * 
 * @return A valid <code>ReorderingScheme</code> object or nullptr
 */
స్థిర స్థిరరాశి ReorderingScheme*
కనుగొను_Matching_Scheme(UBiDiLevel లోపల_Level, UBiDiLevel బహిర్గతం_Level,
        UBi_Di_క్రమం లోపల_క్రమం, UBi_Di_క్రమం బహిర్గతం_క్రమం)
{
    uint32_t i;
    కొరకు (i = 0; i < nSchemes; i++) {
        స్థిరరాశి ReorderingScheme *pScheme = Schemes + i;
        ఒకవేళ (లోపల_Level == pScheme->లోపల_Level && బహిర్గతం_Level == pScheme->బహిర్గతం_Level
                && లోపల_క్రమం == pScheme->లోపల_క్రమం && బహిర్గతం_క్రమం == pScheme->బహిర్గతం_క్రమం) {
            తిరిగి pScheme;
        }
    }
    తిరిగి శూన్యసూచిక;
}

U_CAPI uint32_t U_EXPORT2
ubiditransform_transform(UBiDiTransform *pBiDiTransform,
            స్థిరరాశి char16_t *src, int32_t src_నిడివి,
            char16_t *dest, int32_t dest_కొలత,
            UBiDiLevel లోపల_Para_Level, UBi_Di_క్రమం లోపల_క్రమం,
            UBiDiLevel బహిర్గతం_Para_Level, UBi_Di_క్రమం బహిర్గతం_క్రమం,
            UBiDiMirroring doMirroring, uint32_t shapingOptions,
            UError_సంకేతం *p_Error_సంకేతం)
{
    uint32_t dest_నిడివి = 0;
    UBool పాఠ్యం_మారింది = అబద్ధం;
    స్థిరరాశి UBiDiTransform *pOrigTransform = pBiDiTransform;
    స్థిరరాశి UBiDiAction *action = శూన్యసూచిక;

    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ (src == శూన్యసూచిక || dest == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }
    CHECK_LEN(src, src_నిడివి, p_Error_సంకేతం);
    CHECK_LEN(dest, dest_కొలత, p_Error_సంకేతం);

    ఒకవేళ (pBiDiTransform == శూన్యసూచిక) {
        pBiDiTransform = ubiditransform_open(p_Error_సంకేతం);
        ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి 0;
        }
    }
    /* Current limitation: in multiple paragraphs will be resolved according
       to the 1st paragraph */
    resolve_పునాది_Direction(src, src_నిడివి, &లోపల_Para_Level, &బహిర్గతం_Para_Level);

    pBiDiTransform->p_చురుకైన_Scheme = కనుగొను_Matching_Scheme(లోపల_Para_Level, బహిర్గతం_Para_Level,
            లోపల_క్రమం, బహిర్గతం_క్రమం);
    ఒకవేళ (pBiDiTransform->p_చురుకైన_Scheme == శూన్యసూచిక) {
        వెళ్ళు cleanup;
    }
    pBiDiTransform->reorderingOptions = doMirroring ? UBIDI_DO_MIRRORING
            : UBIDI_REORDER_DEFAULT;

    /* Ignore TEXT_DIRECTION_* flags, as we apply our own depending on the text
       scheme at the time shaping is invoked. */
    shapingOptions &= ~U_SHAPE_పాఠ్యం_DIRECTION_ముసుగు;
    pBiDiTransform->digits = shapingOptions & ~U_SHAPE_LETTERS_ముసుగు;
    pBiDiTransform->letters = shapingOptions & ~U_SHAPE_DIGITS_ముసుగు;

    updateSrc(pBiDiTransform, src, src_నిడివి, dest_కొలత > src_నిడివి ? dest_కొలత : src_నిడివి, p_Error_సంకేతం);
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        వెళ్ళు cleanup;
    }
    ఒకవేళ (pBiDiTransform->pBidi == శూన్యసూచిక) {
        pBiDiTransform->pBidi = ubidi_openSized(0, 0, p_Error_సంకేతం);
        ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
            వెళ్ళు cleanup;
        }
    }
    pBiDiTransform->dest = dest;
    pBiDiTransform->dest_కొలత = dest_కొలత;
    pBiDiTransform->p_Dest_నిడివి = &dest_నిడివి;

    /* Checking for U_SUCCESS() within the loop to bail out on first failure. */
    కొరకు (action = pBiDiTransform->p_చురుకైన_Scheme->actions; *action && U_SUCCESS(*p_Error_సంకేతం); action++) {
        ఒకవేళ ((*action)(pBiDiTransform, p_Error_సంకేతం)) {
            ఒకవేళ (action[1] != శూన్యసూచిక) {
                updateSrc(pBiDiTransform, pBiDiTransform->dest, *pBiDiTransform->p_Dest_నిడివి,
                        *pBiDiTransform->p_Dest_నిడివి, p_Error_సంకేతం);
            }
            పాఠ్యం_మారింది = నిజం;
        }
    }
    ubidi_సమితి_Inverse(pBiDiTransform->pBidi, అబద్ధం);

    ఒకవేళ (!పాఠ్యం_మారింది && U_SUCCESS(*p_Error_సంకేతం)) {
        /* Text was not changed - just copy src to dest */
        ఒకవేళ (dest_కొలత < src_నిడివి) {
            *p_Error_సంకేతం = U_బఫరు_అధికప్రవాహం_ERROR;
        } లేకపోతే {
            u_strncpy(dest, src, src_నిడివి);
            dest_నిడివి = src_నిడివి;
        }
    }
cleanup:
    ఒకవేళ (pOrigTransform != pBiDiTransform) {
        ubiditransform_close(pBiDiTransform);
    } లేకపోతే {
        pBiDiTransform->dest = శూన్యసూచిక;
        pBiDiTransform->p_Dest_నిడివి = శూన్యసూచిక;
        pBiDiTransform->src_నిడివి = 0;
        pBiDiTransform->dest_కొలత = 0;
    }
    తిరిగి (U_FAILURE(*p_Error_సంకేతం) && *p_Error_సంకేతం != U_బఫరు_అధికప్రవాహం_ERROR) ? 0 : dest_నిడివి;
}
