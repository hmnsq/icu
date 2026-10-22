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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION

#അന്തര്ഭാവ "unicode/ucnv_cb.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "cmemory.h"

/* need to update the offsets when the target moves. */
/* Note: Recursion may occur in the cb functions, be sure to update the offsets correctly
if you don't use ucnv_cbXXX functions.  Make sure you don't use the same callback within
the same call stack if the complexity arises. */
U_CAPI ശൂന്യം  U_EXPORT2
ucnv_cbFromUWriteBytes (UConverterFromUnicodeArgs *args,
                       സ്ഥിരം അക്ഷരം* source,
                       int32_t length,
                       int32_t offsetIndex,
                       UErrorCode * err)
{
    എങ്കിൽ(U_FAILURE(*err)) {
        തിരികെ;
    }

    ucnv_fromUWriteBytes(
        args->converter,
        source, length,
        &args->target, args->targetLimit,
        &args->offsets, offsetIndex,
        err);
}

U_CAPI ശൂന്യം  U_EXPORT2
ucnv_cbFromUWriteUChars(UConverterFromUnicodeArgs *args,
                             സ്ഥിരം char16_t** source,
                             സ്ഥിരം char16_t*  sourceLimit,
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

    അക്ഷരം *oldTarget;

    എങ്കിൽ(U_FAILURE(*err))
    {
        തിരികെ;
    }

    oldTarget = args->target;

    ucnv_fromUnicode(args->converter,
        &args->target,
        args->targetLimit,
        source,
        sourceLimit,
        nullptr, /* no offsets */
        അസത്യം, /* no flush */
        err);

    എങ്കിൽ(args->offsets)
    {
        ആകുമ്പോൾ (args->target != oldTarget)  /* if it moved at all.. */
        {
            *(args->offsets)++ = offsetIndex;
            oldTarget++;
        }
    }

    /*
    Note, if you did something like used a Stop subcallback, things would get interesting.
    In fact, here's where we want to return the partially consumed in-source!
    */
    എങ്കിൽ(*err == U_BUFFER_OVERFLOW_ERROR)
    /* && (*source < sourceLimit && args->target >= args->targetLimit)
    -- S. Hrcek */
    {
        /* Overflowed the target.  Now, we'll write into the charErrorBuffer.
        It's a fixed size. If we overflow it... Hmm */
        അക്ഷരം *newTarget;
        സ്ഥിരം അക്ഷരം *newTargetLimit;
        UErrorCode err2 = U_ZERO_ERROR;

        int8_t errBuffLen;

        errBuffLen  = args->converter->charErrorBufferLength;

        /* start the new target at the first free slot in the errbuff.. */
        newTarget = (അക്ഷരം *)(args->converter->charErrorBuffer + errBuffLen);

        newTargetLimit = (അക്ഷരം *)(args->converter->charErrorBuffer +
            വലുപ്പം(args->converter->charErrorBuffer));

        എങ്കിൽ(newTarget >= newTargetLimit)
        {
            *err = U_INTERNAL_PROGRAM_ERROR;
            തിരികെ;
        }

        /* We're going to tell the converter that the errbuff len is empty.
        This prevents the existing errbuff from being 'flushed' out onto
        itself.  If the errbuff is needed by the converter this time,
        we're hosed - we're out of space! */

        args->converter->charErrorBufferLength = 0;

        ucnv_fromUnicode(args->converter,
                         &newTarget,
                         newTargetLimit,
                         source,
                         sourceLimit,
                         nullptr,
                         അസത്യം,
                         &err2);

        /* We can go ahead and overwrite the  length here. We know just how
        to recalculate it. */

        args->converter->charErrorBufferLength = (int8_t)(
            newTarget - (അക്ഷരം*)args->converter->charErrorBuffer);

        എങ്കിൽ((newTarget >= newTargetLimit) || (err2 == U_BUFFER_OVERFLOW_ERROR))
        {
            /* now we're REALLY in trouble.
            Internal program error - callback shouldn't have written this much
            data!
            */
            *err = U_INTERNAL_PROGRAM_ERROR;
            തിരികെ;
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

U_CAPI ശൂന്യം  U_EXPORT2
ucnv_cbFromUWriteSub (UConverterFromUnicodeArgs *args,
                           int32_t offsetIndex,
                           UErrorCode * err)
{
    UConverter *converter;
    int32_t length;

    എങ്കിൽ(U_FAILURE(*err)) {
        തിരികെ;
    }
    converter = args->converter;
    length = converter->subCharLen;

    എങ്കിൽ(length == 0) {
        തിരികെ;
    }

    എങ്കിൽ(length < 0) {
        /*
         * Write/convert the substitution string. Its real length is -length.
         * Unlike the escape callback, we need not change the converter's
         * callback function because ucnv_setSubstString() verified that
         * the string can be converted, so we will not get a conversion error
         * and will not recurse.
         * At worst we should get a U_BUFFER_OVERFLOW_ERROR.
         */
        സ്ഥിരം char16_t *source = (സ്ഥിരം char16_t *)converter->subChars;
        ucnv_cbFromUWriteUChars(args, &source, source - length, offsetIndex, err);
        തിരികെ;
    }

    എങ്കിൽ(converter->sharedData->impl->writeSub!=nullptr) {
        converter->sharedData->impl->writeSub(args, offsetIndex, err);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(converter->subChar1!=0 && (uint16_t)converter->invalidUCharBuffer[0]<=(uint16_t)0xffu) {
        /*
        TODO: Is this untestable because the MBCS converter has a writeSub function to call
        and the other converters don't use subChar1?
        */
        ucnv_cbFromUWriteBytes(args,
                               (സ്ഥിരം അക്ഷരം *)&converter->subChar1, 1,
                               offsetIndex, err);
    }
    അല്ലെങ്കിൽ {
        ucnv_cbFromUWriteBytes(args,
                               (സ്ഥിരം അക്ഷരം *)converter->subChars, length,
                               offsetIndex, err);
    }
}

U_CAPI ശൂന്യം  U_EXPORT2
ucnv_cbToUWriteUChars (UConverterToUnicodeArgs *args,
                            സ്ഥിരം char16_t* source,
                            int32_t length,
                            int32_t offsetIndex,
                            UErrorCode * err)
{
    എങ്കിൽ(U_FAILURE(*err)) {
        തിരികെ;
    }

    ucnv_toUWriteUChars(
        args->converter,
        source, length,
        &args->target, args->targetLimit,
        &args->offsets, offsetIndex,
        err);
}

U_CAPI ശൂന്യം  U_EXPORT2
ucnv_cbToUWriteSub (UConverterToUnicodeArgs *args,
                         int32_t offsetIndex,
                       UErrorCode * err)
{
    സ്ഥിതം സ്ഥിരം char16_t kSubstituteChar1 = 0x1A, kSubstituteChar = 0xFFFD;

    /* could optimize this case, just one uchar */
    എങ്കിൽ(args->converter->invalidCharLength == 1 && args->converter->subChar1 != 0) {
        ucnv_cbToUWriteUChars(args, &kSubstituteChar1, 1, offsetIndex, err);
    } അല്ലെങ്കിൽ {
        ucnv_cbToUWriteUChars(args, &kSubstituteChar, 1, offsetIndex, err);
    }
}

#endif
