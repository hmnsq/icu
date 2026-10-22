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

#include "unicode/utypes.h"

#if !UCONFIG_NO_CONVERSION

#include "unicode/ucnv_cb.h"
#include "ucnv_bld.h"
#include "ucnv_cnv.h"
#include "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI void  U_EXPORT2
ucnv_cbFromUWriteBytes (UConverter_From_Unicode_मगा_कातविरान *मगा_कातविरान_2,
                       const char* source,
                       int32_t length,
                       int32_t offsetIndex,
                       UErrorCode * err)
{
    if(U_FAILURE(*err)) {
        return;
    }

    ucnv_fromUWriteBytes(
        मगा_कातविरान_2->converter,
        source, length,
        &मगा_कातविरान_2->target, मगा_कातविरान_2->targetLimit,
        &मगा_कातविरान_2->offsets, offsetIndex,
        err);
}

U_CAPI void  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverter_From_Unicode_मगा_कातविरान *मगा_कातविरान_2,
                             const char16_t** source,
                             const char16_t*  sourceLimit,
                             int32_t offsetIndex,
                             UErrorCode * err)
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

    char *oldTarget;

    if(U_FAILURE(*err))
    {
        return;
    }

    oldTarget = मगा_कातविरान_2->target;

    ucnv_fromUnicode(मगा_कातविरान_2->converter,
        &मगा_कातविरान_2->target,
        मगा_कातविरान_2->targetLimit,
        source,
        sourceLimit,
        nullptr, /* no offsets */
        false, /* no flush */
        err);

    if(मगा_कातविरान_2->offsets)
    {
        while (मगा_कातविरान_2->target != oldTarget)  /* if it moved at all.. */
        {
            *(मगा_कातविरान_2->offsets)++ = offsetIndex;
            oldTarget++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    if(*err == U_BUFFER_OVERFLOW_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        char *newTarget;
        const char *newTargetLimit;
        UErrorCode err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = मगा_कातविरान_2->converter->charErrorBufferLength;

        /* start the new target at the first free slot in the errbuff.. */
        newTarget = (char *)(मगा_कातविरान_2->converter->charErrorBuffer + errBuffLen);

        newTargetLimit = (char *)(मगा_कातविरान_2->converter->charErrorBuffer +
            sizeof(मगा_कातविरान_2->converter->charErrorBuffer));

        if(newTarget >= newTargetLimit)
        {
            *err = U_INTERNAL_PROGRAM_ERROR;
            return;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        मगा_कातविरान_2->converter->charErrorBufferLength = 0;

        ucnv_fromUnicode(मगा_कातविरान_2->converter,
                         &newTarget,
                         newTargetLimit,
                         source,
                         sourceLimit,
                         nullptr,
                         false,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        मगा_कातविरान_2->converter->charErrorBufferLength = (int8_t)(
            newTarget - (char*)मगा_कातविरान_2->converter->charErrorBuffer);

        if((newTarget >= newTargetLimit) || (err2 == U_BUFFER_OVERFLOW_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_INTERNAL_PROGRAM_ERROR;
            return;
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

U_CAPI void  U_EXPORT2
ucnv_cbFromUWriteSub (UConverter_From_Unicode_मगा_कातविरान *मगा_कातविरान_2,
                           int32_t offsetIndex,
                           UErrorCode * err)
{
    UConverter *converter;
    int32_t length;

    if(U_FAILURE(*err)) {
        return;
    }
    converter = मगा_कातविरान_2->converter;
    length = converter->subCharLen;

    if(length == 0) {
        return;
    }

    if(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        const char16_t *source = (const char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(मगा_कातविरान_2, &source, source - length, offsetIndex, err);
        return;
    }

    if(converter->sharedData->impl->writeSub!=nullptr) {
        converter->sharedData->impl->writeSub(मगा_कातविरान_2, offsetIndex, err);
    }
    else if(converter->subChar1!=0 && (uint16_t)converter->invalidUCharBuffer[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cbFromUWriteBytes(मगा_कातविरान_2,
                               (const char *)&converter->subChar1, 1,
                               offsetIndex, err);
    }
    else {
        ucnv_cbFromUWriteBytes(मगा_कातविरान_2,
                               (const char *)converter->subChars, length,
                               offsetIndex, err);
    }
}

U_CAPI void  U_EXPORT2
ucnv_cbToUWriteUChars (UConverter_To_Unicode_मगा_कातविरान *मगा_कातविरान_2,
                            const char16_t* source,
                            int32_t length,
                            int32_t offsetIndex,
                            UErrorCode * err)
{
    if(U_FAILURE(*err)) {
        return;
    }

    ucnv_toUWriteUChars(
        मगा_कातविरान_2->converter,
        source, length,
        &मगा_कातविरान_2->target, मगा_कातविरान_2->targetLimit,
        &मगा_कातविरान_2->offsets, offsetIndex,
        err);
}

U_CAPI void  U_EXPORT2
ucnv_cbToUWriteSub (UConverter_To_Unicode_मगा_कातविरान *मगा_कातविरान_2,
                         int32_t offsetIndex,
                       UErrorCode * err)
{
    static const char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    if(मगा_कातविरान_2->converter->invalidCharLength == 1 && मगा_कातविरान_2->converter->subChar1 != 0) {
        ucnv_cbToUWriteUChars(मगा_कातविरान_2, &kSubstituteChar1, 1, offsetIndex, err);
    } else {
        ucnv_cbToUWriteUChars(मगा_कातविरान_2, &kSubstituteChar, 1, offsetIndex, err);
    }
}

#endif
