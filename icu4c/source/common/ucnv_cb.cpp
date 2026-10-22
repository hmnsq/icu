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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION

#ضمن "unicode/ucnv_cb.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI فراغ  U_EXPORT2
ucnv_cb_From_UWrite_بایتها (UConverter_From_Unicode_آرگومانها *آرگومانها_2,
                       ثابت حرف* منبع,
                       int32_t length,
                       int32_t جابه_جایی_شاخص,
                       UError_کد * err)
{
    إذا(U_FAILURE(*err)) {
        أرجع;
    }

    ucnv_from_UWrite_بایتها(
        آرگومانها_2->converter,
        منبع, length,
        &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
        &آرگومانها_2->offsets, جابه_جایی_شاخص,
        err);
}

U_CAPI فراغ  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_آرگومانها *آرگومانها_2,
                             ثابت char16_t** منبع,
                             ثابت char16_t*  منبع_حد,
                             int32_t جابه_جایی_شاخص,
                             UError_کد * err)
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

    حرف *old_هدف;

    إذا(U_FAILURE(*err))
    {
        أرجع;
    }

    old_هدف = آرگومانها_2->هدف;

    ucnv_fromUnicode(آرگومانها_2->converter,
        &آرگومانها_2->هدف,
        آرگومانها_2->هدف_حد,
        منبع,
        منبع_حد,
        اشاره_تهی, /* no offsets */
        خطأ, /* no flush */
        err);

    إذا(آرگومانها_2->offsets)
    {
        طالما (آرگومانها_2->هدف != old_هدف)  /* if it moved at all.. */
        {
            *(آرگومانها_2->offsets)++ = جابه_جایی_شاخص;
            old_هدف++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    إذا(*err == U_میانگیر_سرریز_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        حرف *new_هدف;
        ثابت حرف *new_هدف_حد;
        UError_کد err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = آرگومانها_2->converter->char_Error_میانگیر_درازی;

        /* start the new target at the first free slot in the errbuff.. */
        new_هدف = (حرف *)(آرگومانها_2->converter->char_Error_میانگیر + errBuffLen);

        new_هدف_حد = (حرف *)(آرگومانها_2->converter->char_Error_میانگیر +
            حجم(آرگومانها_2->converter->char_Error_میانگیر));

        إذا(new_هدف >= new_هدف_حد)
        {
            *err = U_داخلی_برنامه_ERROR;
            أرجع;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        آرگومانها_2->converter->char_Error_میانگیر_درازی = 0;

        ucnv_fromUnicode(آرگومانها_2->converter,
                         &new_هدف,
                         new_هدف_حد,
                         منبع,
                         منبع_حد,
                         اشاره_تهی,
                         خطأ,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        آرگومانها_2->converter->char_Error_میانگیر_درازی = (int8_t)(
            new_هدف - (حرف*)آرگومانها_2->converter->char_Error_میانگیر);

        إذا((new_هدف >= new_هدف_حد) || (err2 == U_میانگیر_سرریز_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_داخلی_برنامه_ERROR;
            أرجع;
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

U_CAPI فراغ  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_آرگومانها *آرگومانها_2,
                           int32_t جابه_جایی_شاخص,
                           UError_کد * err)
{
    UConverter *converter;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع;
    }
    converter = آرگومانها_2->converter;
    length = converter->subCharLen;

    إذا(length == 0) {
        أرجع;
    }

    إذا(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        ثابت char16_t *منبع = (ثابت char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(آرگومانها_2, &منبع, منبع - length, جابه_جایی_شاخص, err);
        أرجع;
    }

    إذا(converter->shared_داده->impl->writeSub!=اشاره_تهی) {
        converter->shared_داده->impl->writeSub(آرگومانها_2, جابه_جایی_شاخص, err);
    }
    وإلا إذا(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_میانگیر[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_بایتها(آرگومانها_2,
                               (ثابت حرف *)&converter->subChar1, 1,
                               جابه_جایی_شاخص, err);
    }
    وإلا {
        ucnv_cb_From_UWrite_بایتها(آرگومانها_2,
                               (ثابت حرف *)converter->subChars, length,
                               جابه_جایی_شاخص, err);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_cb_مقصد_UWrite_UChars (UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                            ثابت char16_t* منبع,
                            int32_t length,
                            int32_t جابه_جایی_شاخص,
                            UError_کد * err)
{
    إذا(U_FAILURE(*err)) {
        أرجع;
    }

    ucnv_مقصد_UWrite_UChars(
        آرگومانها_2->converter,
        منبع, length,
        &آرگومانها_2->هدف, آرگومانها_2->هدف_حد,
        &آرگومانها_2->offsets, جابه_جایی_شاخص,
        err);
}

U_CAPI فراغ  U_EXPORT2
ucnv_cb_مقصد_UWrite_Sub (UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                         int32_t جابه_جایی_شاخص,
                       UError_کد * err)
{
    ساكن ثابت char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    إذا(آرگومانها_2->converter->invalid_Char_درازی == 1 && آرگومانها_2->converter->subChar1 != 0) {
        ucnv_cb_مقصد_UWrite_UChars(آرگومانها_2, &kSubstituteChar1, 1, جابه_جایی_شاخص, err);
    } وإلا {
        ucnv_cb_مقصد_UWrite_UChars(آرگومانها_2, &kSubstituteChar, 1, جابه_جایی_شاخص, err);
    }
}

#پایان_اگر
