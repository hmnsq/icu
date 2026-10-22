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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_cb_From_UWrite_ਬਾਈਟਾਂ (UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                       ਅਚਲ ਅੱਖਰ* ਸਰੋਤ,
                       int32_t length,
                       int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
                       UError_ਕੋਡ * err)
{
    ਜੇ(U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ucnv_from_UWrite_ਬਾਈਟਾਂ(
        ਦਲੀਲਾਂ_2->converter,
        ਸਰੋਤ, length,
        &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
        &ਦਲੀਲਾਂ_2->offsets, ਵਿੱਥ_ਸੂਚਕਾਂਕ,
        err);
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                             ਅਚਲ char16_t** ਸਰੋਤ,
                             ਅਚਲ char16_t*  ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                             int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
                             UError_ਕੋਡ * err)
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

    ਅੱਖਰ *old_ਨਿਸ਼ਾਨਾ;

    ਜੇ(U_FAILURE(*err))
    {
        ਵਾਪਸ;
    }

    old_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;

    ucnv_fromUnicode(ਦਲੀਲਾਂ_2->converter,
        &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ,
        ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
        ਸਰੋਤ,
        ਸਰੋਤ_ਹੱਦਬੰਦੀ,
        ਖਾਲੀ_ਸੂਚਕ, /* no offsets */
        ਝੂਠ, /* no flush */
        err);

    ਜੇ(ਦਲੀਲਾਂ_2->offsets)
    {
        ਜਦੋਂ (ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ != old_ਨਿਸ਼ਾਨਾ)  /* if it moved at all.. */
        {
            *(ਦਲੀਲਾਂ_2->offsets)++ = ਵਿੱਥ_ਸੂਚਕਾਂਕ;
            old_ਨਿਸ਼ਾਨਾ++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    ਜੇ(*err == U_ਬਫਰ_ਉਛਾਲ_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        ਅੱਖਰ *new_ਨਿਸ਼ਾਨਾ;
        ਅਚਲ ਅੱਖਰ *new_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
        UError_ਕੋਡ err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ;

        /* start the new target at the first free slot in the errbuff.. */
        new_ਨਿਸ਼ਾਨਾ = (ਅੱਖਰ *)(ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ + errBuffLen);

        new_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = (ਅੱਖਰ *)(ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ +
            ਆਕਾਰ(ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ));

        ਜੇ(new_ਨਿਸ਼ਾਨਾ >= new_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ)
        {
            *err = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਵਾਪਸ;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ = 0;

        ucnv_fromUnicode(ਦਲੀਲਾਂ_2->converter,
                         &new_ਨਿਸ਼ਾਨਾ,
                         new_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
                         ਸਰੋਤ,
                         ਸਰੋਤ_ਹੱਦਬੰਦੀ,
                         ਖਾਲੀ_ਸੂਚਕ,
                         ਝੂਠ,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ = (int8_t)(
            new_ਨਿਸ਼ਾਨਾ - (ਅੱਖਰ*)ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ);

        ਜੇ((new_ਨਿਸ਼ਾਨਾ >= new_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) || (err2 == U_ਬਫਰ_ਉਛਾਲ_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_ਅੰਦਰੂਨੀ_ਪ੍ਰੋਗਰਾਮ_ERROR;
            ਵਾਪਸ;
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

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                           int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
                           UError_ਕੋਡ * err)
{
    UConverter *converter;
    int32_t length;

    ਜੇ(U_FAILURE(*err)) {
        ਵਾਪਸ;
    }
    converter = ਦਲੀਲਾਂ_2->converter;
    length = converter->subCharLen;

    ਜੇ(length == 0) {
        ਵਾਪਸ;
    }

    ਜੇ(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        ਅਚਲ char16_t *ਸਰੋਤ = (ਅਚਲ char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(ਦਲੀਲਾਂ_2, &ਸਰੋਤ, ਸਰੋਤ - length, ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
        ਵਾਪਸ;
    }

    ਜੇ(converter->shared_ਅੰਕੜੇ->impl->writeSub!=ਖਾਲੀ_ਸੂਚਕ) {
        converter->shared_ਅੰਕੜੇ->impl->writeSub(ਦਲੀਲਾਂ_2, ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
    }
    ਨਹੀਂਤਾਂ ਜੇ(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_ਬਫਰ[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2,
                               (ਅਚਲ ਅੱਖਰ *)&converter->subChar1, 1,
                               ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
    }
    ਨਹੀਂਤਾਂ {
        ucnv_cb_From_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2,
                               (ਅਚਲ ਅੱਖਰ *)converter->subChars, length,
                               ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
    }
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_UChars (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                            ਅਚਲ char16_t* ਸਰੋਤ,
                            int32_t length,
                            int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
                            UError_ਕੋਡ * err)
{
    ਜੇ(U_FAILURE(*err)) {
        ਵਾਪਸ;
    }

    ucnv_ਮੰਜ਼ਿਲ_UWrite_UChars(
        ਦਲੀਲਾਂ_2->converter,
        ਸਰੋਤ, length,
        &ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ, ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,
        &ਦਲੀਲਾਂ_2->offsets, ਵਿੱਥ_ਸੂਚਕਾਂਕ,
        err);
}

U_CAPI ਖਾਲੀ  U_EXPORT2
ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_Sub (UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                         int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ,
                       UError_ਕੋਡ * err)
{
    ਸਥਿਰ ਅਚਲ char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    ਜੇ(ਦਲੀਲਾਂ_2->converter->invalid_Char_ਲੰਮਾਈ == 1 && ਦਲੀਲਾਂ_2->converter->subChar1 != 0) {
        ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_UChars(ਦਲੀਲਾਂ_2, &kSubstituteChar1, 1, ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
    } ਨਹੀਂਤਾਂ {
        ucnv_cb_ਮੰਜ਼ਿਲ_UWrite_UChars(ਦਲੀਲਾਂ_2, &kSubstituteChar, 1, ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
    }
}

#ਜੇ_ਅੰਤ
