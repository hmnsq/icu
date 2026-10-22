// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2000-2006, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
 *  ucnv_cb.c:
 *  External APIs for the ICU's codeset conversion library
 *  Helena Shih
 *
 * Modification History:
 *
 *   Date        Name        Description
 *   7/28/2000   srl         Implementation
 */

/**
 * @name Character Conversion C API
 *
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION

#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI శూన్య  U_EXPORT2
ucnv_cb_From_UWrite_బైట్లు (UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                       స్థిరరాశి అక్షరం* ఆధారం,
                       int32_t length,
                       int32_t స్థానభ్రంశం_సూచీ,
                       UError_సంకేతం * err)
{
    ఒకవేళ(U_FAILURE(*err)) {
        తిరిగి;
    }

    ucnv_from_UWrite_బైట్లు(
        వాదనలు_2->converter,
        ఆధారం, length,
        &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
        &వాదనలు_2->offsets, స్థానభ్రంశం_సూచీ,
        err);
}

U_CAPI శూన్య  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                             స్థిరరాశి char16_t** ఆధారం,
                             స్థిరరాశి char16_t*  ఆధారం_హద్దు,
                             int32_t స్థానభ్రంశం_సూచీ,
                             UError_సంకేతం * err)
{
    /*
    This is a fun one.  Recursion can occur - we're basically going to
    just retry shoving data through the same converter. Note, if you got
    here through some kind of invalid sequence, you maybe should emit a
    reset sequence of some kind and/or call ucnv_reset().  Since this
    IS an actual conversion, take care that you've changed the callback
    or the data, or you'll get an infinite loop.

    Please set the err value to something reasonable before calling
    into this.
    */

    అక్షరం *old_లక్ష్యం;

    ఒకవేళ(U_FAILURE(*err))
    {
        తిరిగి;
    }

    old_లక్ష్యం = వాదనలు_2->లక్ష్యం;

    ucnv_fromUnicode(వాదనలు_2->converter,
        &వాదనలు_2->లక్ష్యం,
        వాదనలు_2->లక్ష్యం_హద్దు,
        ఆధారం,
        ఆధారం_హద్దు,
        శూన్యసూచిక, /* no offsets */
        అబద్ధం, /* no flush */
        err);

    ఒకవేళ(వాదనలు_2->offsets)
    {
        అయితే (వాదనలు_2->లక్ష్యం != old_లక్ష్యం)  /* if it moved at all.. */
        {
            *(వాదనలు_2->offsets)++ = స్థానభ్రంశం_సూచీ;
            old_లక్ష్యం++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    ఒకవేళ(*err == U_బఫరు_అధికప్రవాహం_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        అక్షరం *new_లక్ష్యం;
        స్థిరరాశి అక్షరం *new_లక్ష్యం_హద్దు;
        UError_సంకేతం err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = వాదనలు_2->converter->char_Error_బఫరు_నిడివి;

        /* start the new target at the first free slot in the errbuff.. */
        new_లక్ష్యం = (అక్షరం *)(వాదనలు_2->converter->char_Error_బఫరు + errBuffLen);

        new_లక్ష్యం_హద్దు = (అక్షరం *)(వాదనలు_2->converter->char_Error_బఫరు +
            పరిమాణం(వాదనలు_2->converter->char_Error_బఫరు));

        ఒకవేళ(new_లక్ష్యం >= new_లక్ష్యం_హద్దు)
        {
            *err = U_అంతర్గత_కార్యక్రమం_ERROR;
            తిరిగి;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        వాదనలు_2->converter->char_Error_బఫరు_నిడివి = 0;

        ucnv_fromUnicode(వాదనలు_2->converter,
                         &new_లక్ష్యం,
                         new_లక్ష్యం_హద్దు,
                         ఆధారం,
                         ఆధారం_హద్దు,
                         శూన్యసూచిక,
                         అబద్ధం,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        వాదనలు_2->converter->char_Error_బఫరు_నిడివి = (int8_t)(
            new_లక్ష్యం - (అక్షరం*)వాదనలు_2->converter->char_Error_బఫరు);

        ఒకవేళ((new_లక్ష్యం >= new_లక్ష్యం_హద్దు) || (err2 == U_బఫరు_అధికప్రవాహం_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_అంతర్గత_కార్యక్రమం_ERROR;
            తిరిగి;
        }
        /*else {*/
            /* sub errs could be invalid/truncated/illegal chars or w/e.
            These might want to be passed on up.. But the problem is, we already
            need to pass U_BUFFER_OVERFLOW_ERROR. That has to override these
            other errs.. */

            /*
            if(U_FAILURE(err2))
            ??
            */
        /*}*/
    }
}

U_CAPI శూన్య  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_వాదనలు *వాదనలు_2,
                           int32_t స్థానభ్రంశం_సూచీ,
                           UError_సంకేతం * err)
{
    UConverter *converter;
    int32_t length;

    ఒకవేళ(U_FAILURE(*err)) {
        తిరిగి;
    }
    converter = వాదనలు_2->converter;
    length = converter->subCharLen;

    ఒకవేళ(length == 0) {
        తిరిగి;
    }

    ఒకవేళ(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        స్థిరరాశి char16_t *ఆధారం = (స్థిరరాశి char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(వాదనలు_2, &ఆధారం, ఆధారం - length, స్థానభ్రంశం_సూచీ, err);
        తిరిగి;
    }

    ఒకవేళ(converter->shared_దత్తాంశం->impl->writeSub!=శూన్యసూచిక) {
        converter->shared_దత్తాంశం->impl->writeSub(వాదనలు_2, స్థానభ్రంశం_సూచీ, err);
    }
    లేకపోతే ఒకవేళ(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_బఫరు[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_బైట్లు(వాదనలు_2,
                               (స్థిరరాశి అక్షరం *)&converter->subChar1, 1,
                               స్థానభ్రంశం_సూచీ, err);
    }
    లేకపోతే {
        ucnv_cb_From_UWrite_బైట్లు(వాదనలు_2,
                               (స్థిరరాశి అక్షరం *)converter->subChars, length,
                               స్థానభ్రంశం_సూచీ, err);
    }
}

U_CAPI శూన్య  U_EXPORT2
ucnv_cb_గమ్యం_UWrite_UChars (UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                            స్థిరరాశి char16_t* ఆధారం,
                            int32_t length,
                            int32_t స్థానభ్రంశం_సూచీ,
                            UError_సంకేతం * err)
{
    ఒకవేళ(U_FAILURE(*err)) {
        తిరిగి;
    }

    ucnv_గమ్యం_UWrite_UChars(
        వాదనలు_2->converter,
        ఆధారం, length,
        &వాదనలు_2->లక్ష్యం, వాదనలు_2->లక్ష్యం_హద్దు,
        &వాదనలు_2->offsets, స్థానభ్రంశం_సూచీ,
        err);
}

U_CAPI శూన్య  U_EXPORT2
ucnv_cb_గమ్యం_UWrite_Sub (UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                         int32_t స్థానభ్రంశం_సూచీ,
                       UError_సంకేతం * err)
{
    స్థిర స్థిరరాశి char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    ఒకవేళ(వాదనలు_2->converter->invalid_Char_నిడివి == 1 && వాదనలు_2->converter->subChar1 != 0) {
        ucnv_cb_గమ్యం_UWrite_UChars(వాదనలు_2, &kSubstituteChar1, 1, స్థానభ్రంశం_సూచీ, err);
    } లేకపోతే {
        ucnv_cb_గమ్యం_UWrite_UChars(వాదనలు_2, &kSubstituteChar, 1, స్థానభ్రంశం_సూచీ, err);
    }
}

#అయితే_ముగింపు
