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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_cb_From_UWrite_ಬೈಟುಗಳು (UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಮೂಲ,
                       int32_t length,
                       int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
                       UError_ಸಂಕೇತ * err)
{
    ಆದರೆ(U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ucnv_from_UWrite_ಬೈಟುಗಳು(
        ವಾದಗಳು_2->converter,
        ಮೂಲ, length,
        &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
        &ವಾದಗಳು_2->offsets, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
        err);
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                             ಸ್ಥಿರಾಂಕ char16_t** ಮೂಲ,
                             ಸ್ಥಿರಾಂಕ char16_t*  ಮೂಲ_ಎಲ್ಲೆ,
                             int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
                             UError_ಸಂಕೇತ * err)
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

    ಅಕ್ಷರ *old_ಗುರಿ;

    ಆದರೆ(U_FAILURE(*err))
    {
        ಹಿಂತಿರುಗಿ;
    }

    old_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;

    ucnv_fromUnicode(ವಾದಗಳು_2->converter,
        &ವಾದಗಳು_2->ಗುರಿ,
        ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
        ಮೂಲ,
        ಮೂಲ_ಎಲ್ಲೆ,
        ಶೂನ್ಯಸೂಚಕ, /* no offsets */
        ಸುಳ್ಳು, /* no flush */
        err);

    ಆದರೆ(ವಾದಗಳು_2->offsets)
    {
        ಆಗ (ವಾದಗಳು_2->ಗುರಿ != old_ಗುರಿ)  /* if it moved at all.. */
        {
            *(ವಾದಗಳು_2->offsets)++ = ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ;
            old_ಗುರಿ++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    ಆದರೆ(*err == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        ಅಕ್ಷರ *new_ಗುರಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *new_ಗುರಿ_ಎಲ್ಲೆ;
        UError_ಸಂಕೇತ err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ;

        /* start the new target at the first free slot in the errbuff.. */
        new_ಗುರಿ = (ಅಕ್ಷರ *)(ವಾದಗಳು_2->converter->char_Error_ಬಫರ್ + errBuffLen);

        new_ಗುರಿ_ಎಲ್ಲೆ = (ಅಕ್ಷರ *)(ವಾದಗಳು_2->converter->char_Error_ಬಫರ್ +
            ಗಾತ್ರ(ವಾದಗಳು_2->converter->char_Error_ಬಫರ್));

        ಆದರೆ(new_ಗುರಿ >= new_ಗುರಿ_ಎಲ್ಲೆ)
        {
            *err = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = 0;

        ucnv_fromUnicode(ವಾದಗಳು_2->converter,
                         &new_ಗುರಿ,
                         new_ಗುರಿ_ಎಲ್ಲೆ,
                         ಮೂಲ,
                         ಮೂಲ_ಎಲ್ಲೆ,
                         ಶೂನ್ಯಸೂಚಕ,
                         ಸುಳ್ಳು,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ = (int8_t)(
            new_ಗುರಿ - (ಅಕ್ಷರ*)ವಾದಗಳು_2->converter->char_Error_ಬಫರ್);

        ಆದರೆ((new_ಗುರಿ >= new_ಗುರಿ_ಎಲ್ಲೆ) || (err2 == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_ಆಂತರಿಕ_ಕಾರ್ಯಕ್ರಮ_ERROR;
            ಹಿಂತಿರುಗಿ;
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

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                           int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
                           UError_ಸಂಕೇತ * err)
{
    UConverter *converter;
    int32_t length;

    ಆದರೆ(U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }
    converter = ವಾದಗಳು_2->converter;
    length = converter->subCharLen;

    ಆದರೆ(length == 0) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ = (ಸ್ಥಿರಾಂಕ char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(ವಾದಗಳು_2, &ಮೂಲ, ಮೂಲ - length, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(converter->shared_ದತ್ತಾಂಶ->impl->writeSub!=ಶೂನ್ಯಸೂಚಕ) {
        converter->shared_ದತ್ತಾಂಶ->impl->writeSub(ವಾದಗಳು_2, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(converter->subChar1!=0 && (uint16_t)converter->invalid_UChar_ಬಫರ್[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cb_From_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2,
                               (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&converter->subChar1, 1,
                               ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_cb_From_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2,
                               (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)converter->subChars, length,
                               ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
    }
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_cb_ಗಮ್ಯ_UWrite_UChars (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                            ಸ್ಥಿರಾಂಕ char16_t* ಮೂಲ,
                            int32_t length,
                            int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
                            UError_ಸಂಕೇತ * err)
{
    ಆದರೆ(U_FAILURE(*err)) {
        ಹಿಂತಿರುಗಿ;
    }

    ucnv_ಗಮ್ಯ_UWrite_UChars(
        ವಾದಗಳು_2->converter,
        ಮೂಲ, length,
        &ವಾದಗಳು_2->ಗುರಿ, ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ,
        &ವಾದಗಳು_2->offsets, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
        err);
}

U_CAPI ಶೂನ್ಯ  U_EXPORT2
ucnv_cb_ಗಮ್ಯ_UWrite_Sub (UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                         int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ,
                       UError_ಸಂಕೇತ * err)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    ಆದರೆ(ವಾದಗಳು_2->converter->invalid_Char_ಉದ್ದಳತೆ == 1 && ವಾದಗಳು_2->converter->subChar1 != 0) {
        ucnv_cb_ಗಮ್ಯ_UWrite_UChars(ವಾದಗಳು_2, &kSubstituteChar1, 1, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_cb_ಗಮ್ಯ_UWrite_UChars(ವಾದಗಳು_2, &kSubstituteChar, 1, ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
    }
}

#ಆದರೆ_ಅಂತ್ಯ
