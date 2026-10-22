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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION

#شامل "unicode/ucnv_cb.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI خالی  U_EXPORT2
ucnv_cb_From_UWrite_بائٹیں (UConverter_From_Unicode_دلائل *دلائل_2,
                       ثابت حرف* ماخذ,
                       int32_t length,
                       int32_t انحراف_اشاریہ,
                       UError_رمز * err)
{
    اگر(U_FAILURE(*err)) {
        واپس;
    }

    ucnv_from_UWrite_بائٹیں(
        دلائل_2->converter,
        ماخذ, length,
        &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
        &دلائل_2->offsets, انحراف_اشاریہ,
        err);
}

U_CAPI خالی  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_دلائل *دلائل_2,
                             ثابت char16_t** ماخذ,
                             ثابت char16_t*  ماخذ_حد_بندی,
                             int32_t انحراف_اشاریہ,
                             UError_رمز * err)
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

    حرف *old_ہدف;

    اگر(U_FAILURE(*err))
    {
        واپس;
    }

    old_ہدف = دلائل_2->ہدف;

    ucnv_fromUnicode(دلائل_2->converter,
        &دلائل_2->ہدف,
        دلائل_2->ہدف_حد_بندی,
        ماخذ,
        ماخذ_حد_بندی,
        خالی_اشاریہ, /* no offsets */
        جھوٹ, /* no flush */
        err);

    اگر(دلائل_2->offsets)
    {
        جب (دلائل_2->ہدف != old_ہدف)  /* if it moved at all.. */
        {
            *(دلائل_2->offsets)++ = انحراف_اشاریہ;
            old_ہدف++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    اگر(*err == U_بفر_حد_سے_تجاوز_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        حرف *new_ہدف;
        ثابت حرف *new_ہدف_حد_بندی;
        UError_رمز err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = دلائل_2->converter->char_Error_بفر_لمبائی;

        /* start the new target at the first free slot in the errbuff.. */
        new_ہدف = (حرف *)(دلائل_2->converter->char_Error_بفر + errBuffLen);

        new_ہدف_حد_بندی = (حرف *)(دلائل_2->converter->char_Error_بفر +
            حجم(دلائل_2->converter->char_Error_بفر));

        اگر(new_ہدف >= new_ہدف_حد_بندی)
        {
            *err = U_اندرونی_پروگرام_ERROR;
            واپس;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        دلائل_2->converter->char_Error_بفر_لمبائی = 0;

        ucnv_fromUnicode(دلائل_2->converter,
                         &new_ہدف,
                         new_ہدف_حد_بندی,
                         ماخذ,
                         ماخذ_حد_بندی,
                         خالی_اشاریہ,
                         جھوٹ,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        دلائل_2->converter->char_Error_بفر_لمبائی = (int8_t)(
            new_ہدف - (حرف*)دلائل_2->converter->char_Error_بفر);

        اگر((new_ہدف >= new_ہدف_حد_بندی) || (err2 == U_بفر_حد_سے_تجاوز_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_اندرونی_پروگرام_ERROR;
            واپس;
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

U_CAPI خالی  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_دلائل *دلائل_2,
                           int32_t انحراف_اشاریہ,
                           UError_رمز * err)
{
    UConverter *converter;
    int32_t length;

    اگر(U_FAILURE(*err)) {
        واپس;
    }
    converter = دلائل_2->converter;
    length = converter->subCharLen;

    اگر(length == 0) {
        واپس;
    }

    اگر(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        ثابت char16_t *ماخذ = (ثابت char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(دلائل_2, &ماخذ, ماخذ - length, انحراف_اشاریہ, err);
        واپس;
    }

    اگر(converter->shared_ڈیٹا->impl->writeSub!=خالی_اشاریہ) {
        converter->shared_ڈیٹا->impl->writeSub(دلائل_2, انحراف_اشاریہ, err);
    }
    ورنہ اگر(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_بفر[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_بائٹیں(دلائل_2,
                               (ثابت حرف *)&converter->subChar1, 1,
                               انحراف_اشاریہ, err);
    }
    ورنہ {
        ucnv_cb_From_UWrite_بائٹیں(دلائل_2,
                               (ثابت حرف *)converter->subChars, length,
                               انحراف_اشاریہ, err);
    }
}

U_CAPI خالی  U_EXPORT2
ucnv_cb_مقصد_UWrite_UChars (UConverter_مقصد_Unicode_دلائل *دلائل_2,
                            ثابت char16_t* ماخذ,
                            int32_t length,
                            int32_t انحراف_اشاریہ,
                            UError_رمز * err)
{
    اگر(U_FAILURE(*err)) {
        واپس;
    }

    ucnv_مقصد_UWrite_UChars(
        دلائل_2->converter,
        ماخذ, length,
        &دلائل_2->ہدف, دلائل_2->ہدف_حد_بندی,
        &دلائل_2->offsets, انحراف_اشاریہ,
        err);
}

U_CAPI خالی  U_EXPORT2
ucnv_cb_مقصد_UWrite_Sub (UConverter_مقصد_Unicode_دلائل *دلائل_2,
                         int32_t انحراف_اشاریہ,
                       UError_رمز * err)
{
    ساکن ثابت char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    اگر(دلائل_2->converter->invalid_Char_لمبائی == 1 && دلائل_2->converter->subChar1 != 0) {
        ucnv_cb_مقصد_UWrite_UChars(دلائل_2, &kSubstituteChar1, 1, انحراف_اشاریہ, err);
    } ورنہ {
        ucnv_cb_مقصد_UWrite_UChars(دلائل_2, &kSubstituteChar, 1, انحراف_اشاریہ, err);
    }
}

#اگر_ختم
