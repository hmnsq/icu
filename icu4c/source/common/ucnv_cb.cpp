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
ucnv_cb_From_UWrite_بائٹاں (UConverter_From_Unicode_دلیلاں *دلیلاں_2,
                       ثابت حرف* سروت,
                       int32_t length,
                       int32_t وتھ_اشاریہ,
                       UError_کوڈ * err)
{
    إذا(U_FAILURE(*err)) {
        أرجع;
    }

    ucnv_from_UWrite_بائٹاں(
        دلیلاں_2->converter,
        سروت, length,
        &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
        &دلیلاں_2->offsets, وتھ_اشاریہ,
        err);
}

U_CAPI فراغ  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_دلیلاں *دلیلاں_2,
                             ثابت char16_t** سروت,
                             ثابت char16_t*  سروت_حد_بندی,
                             int32_t وتھ_اشاریہ,
                             UError_کوڈ * err)
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

    حرف *old_نشانہ;

    إذا(U_FAILURE(*err))
    {
        أرجع;
    }

    old_نشانہ = دلیلاں_2->نشانہ;

    ucnv_fromUnicode(دلیلاں_2->converter,
        &دلیلاں_2->نشانہ,
        دلیلاں_2->نشانہ_حد_بندی,
        سروت,
        سروت_حد_بندی,
        خالی_سوچک, /* no offsets */
        خطأ, /* no flush */
        err);

    إذا(دلیلاں_2->offsets)
    {
        طالما (دلیلاں_2->نشانہ != old_نشانہ)  /* if it moved at all.. */
        {
            *(دلیلاں_2->offsets)++ = وتھ_اشاریہ;
            old_نشانہ++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    إذا(*err == U_بفر_حد_توں_ودھ_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        حرف *new_نشانہ;
        ثابت حرف *new_نشانہ_حد_بندی;
        UError_کوڈ err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = دلیلاں_2->converter->char_Error_بفر_لمائی;

        /* start the new target at the first free slot in the errbuff.. */
        new_نشانہ = (حرف *)(دلیلاں_2->converter->char_Error_بفر + errBuffLen);

        new_نشانہ_حد_بندی = (حرف *)(دلیلاں_2->converter->char_Error_بفر +
            حجم(دلیلاں_2->converter->char_Error_بفر));

        إذا(new_نشانہ >= new_نشانہ_حد_بندی)
        {
            *err = U_اندرونی_پروگرام_ERROR;
            أرجع;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        دلیلاں_2->converter->char_Error_بفر_لمائی = 0;

        ucnv_fromUnicode(دلیلاں_2->converter,
                         &new_نشانہ,
                         new_نشانہ_حد_بندی,
                         سروت,
                         سروت_حد_بندی,
                         خالی_سوچک,
                         خطأ,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        دلیلاں_2->converter->char_Error_بفر_لمائی = (int8_t)(
            new_نشانہ - (حرف*)دلیلاں_2->converter->char_Error_بفر);

        إذا((new_نشانہ >= new_نشانہ_حد_بندی) || (err2 == U_بفر_حد_توں_ودھ_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_اندرونی_پروگرام_ERROR;
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
ucnv_cbFromUWriteSub (UConverter_From_Unicode_دلیلاں *دلیلاں_2,
                           int32_t وتھ_اشاریہ,
                           UError_کوڈ * err)
{
    UConverter *converter;
    int32_t length;

    إذا(U_FAILURE(*err)) {
        أرجع;
    }
    converter = دلیلاں_2->converter;
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
        ثابت char16_t *سروت = (ثابت char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(دلیلاں_2, &سروت, سروت - length, وتھ_اشاریہ, err);
        أرجع;
    }

    إذا(converter->shared_انکڑے->impl->writeSub!=خالی_سوچک) {
        converter->shared_انکڑے->impl->writeSub(دلیلاں_2, وتھ_اشاریہ, err);
    }
    وإلا إذا(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_بفر[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_بائٹاں(دلیلاں_2,
                               (ثابت حرف *)&converter->subChar1, 1,
                               وتھ_اشاریہ, err);
    }
    وإلا {
        ucnv_cb_From_UWrite_بائٹاں(دلیلاں_2,
                               (ثابت حرف *)converter->subChars, length,
                               وتھ_اشاریہ, err);
    }
}

U_CAPI فراغ  U_EXPORT2
ucnv_cb_منزل_UWrite_UChars (UConverter_منزل_Unicode_دلیلاں *دلیلاں_2,
                            ثابت char16_t* سروت,
                            int32_t length,
                            int32_t وتھ_اشاریہ,
                            UError_کوڈ * err)
{
    إذا(U_FAILURE(*err)) {
        أرجع;
    }

    ucnv_منزل_UWrite_UChars(
        دلیلاں_2->converter,
        سروت, length,
        &دلیلاں_2->نشانہ, دلیلاں_2->نشانہ_حد_بندی,
        &دلیلاں_2->offsets, وتھ_اشاریہ,
        err);
}

U_CAPI فراغ  U_EXPORT2
ucnv_cb_منزل_UWrite_Sub (UConverter_منزل_Unicode_دلیلاں *دلیلاں_2,
                         int32_t وتھ_اشاریہ,
                       UError_کوڈ * err)
{
    ساكن ثابت char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    إذا(دلیلاں_2->converter->invalid_Char_لمائی == 1 && دلیلاں_2->converter->subChar1 != 0) {
        ucnv_cb_منزل_UWrite_UChars(دلیلاں_2, &kSubstituteChar1, 1, وتھ_اشاریہ, err);
    } وإلا {
        ucnv_cb_منزل_UWrite_UChars(دلیلاں_2, &kSubstituteChar, 1, وتھ_اشاریہ, err);
    }
}

#جے_انت
