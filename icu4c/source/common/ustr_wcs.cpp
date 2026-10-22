// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2001-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ustr_wcs.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004sep07
*   created by: Markus W. Scherer
*
*   u_strToWCS() and u_strFromWCS() functions
*   moved here from ustrtrns.c for better modularization.
*/

#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "cstring.h"
#ضمن "cwchar.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"
#ضمن "ustr_cnv.h"

#ܐܢ defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define _STACK_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#ܐܢ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_growAnyBufferFromStatic(فراغ *context,
                       فراغ **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *newBuffer=(حرف *)uprv_malloc(reqCapacity*size);
    ܐܢ(newBuffer!=nullptr) {
        ܐܢ(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } ܐܚܪܢܐ {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ܐܢ(*pBuffer!=(حرف *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    ܦܢܐ newBuffer!=nullptr;
}

/* helper function */
ساكن wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ثابت char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    حرف stackBuffer [_STACK_BUFFER_CAPACITY];
    حرف* tempBuf = stackBuffer;
    int32_t tempBufCapacity = _STACK_BUFFER_CAPACITY;
    حرف* tempBufLimit = stackBuffer + tempBufCapacity;
    UConverter* conv = nullptr;
    حرف* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *pSrcLimit =nullptr;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    ܐܢ(U_FAILURE(*pErrorCode)){
        ܦܢܐ nullptr;
    }
    
    ܐܢ(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    لكل(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        ܐܢ(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            ܐܢ(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            ܐܢ(!u_growAnyBufferFromStatic(stackBuffer,(فراغ**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } ܐܚܪܢܐ {
            ܐܢ (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    ܐܢ(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ܐܢ(!u_growAnyBufferFromStatic(stackBuffer,(فراغ**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            اذهب cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * حجم(wchar_t) );

    ܐܢ(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        لكل(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            ܐܢ(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                اكسر;
            }ܐܚܪܢܐ ܐܢ(retVal== remaining){/* should never occur */
                صحيح numWritten = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(فراغ**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          numWritten,
                                          حجم(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                ܐܢ(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=numWritten;
                    remaining-=numWritten;
                }

            }ܐܚܪܢܐ{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ܟܡܐ(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ܐܢ(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        ܐܢ(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*حجم(wchar_t));
        }  

        ܐܢ(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }ܐܚܪܢܐ{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ܐܢ(stackBuffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    ܦܢܐ dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           ثابت char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    ܐܢ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ܦܢܐ nullptr;
    }
        
    ܐܢ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ܐܢ(srcLength == -1){
        srcLength = u_strlen(src);
    }
    ܐܢ(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    ܐܢ(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    ܦܢܐ dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    ܦܢܐ (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#ܐܚܪܢܐ
    
    ܦܢܐ _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#ܐܢ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساكن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             ثابت wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t uStack [_STACK_BUFFER_CAPACITY];

    wchar_t wStack[_STACK_BUFFER_CAPACITY];
    wchar_t* pWStack = wStack;


    حرف cStack[_STACK_BUFFER_CAPACITY];
    int32_t cStackCap = _STACK_BUFFER_CAPACITY;
    حرف* pCSrc=cStack;
    حرف* pCSave=pCSrc;
    حرف* pCSrcLimit=nullptr;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* pSrcLimit = nullptr;

    ܐܢ(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, cStackCap);
    
            ܐܢ(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                اذهب cleanup;
            }ܐܚܪܢܐ ܐܢ(retVal >= (cStackCap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(cStack,(فراغ**)&pCSrc,&cStackCap,
                    cStackCap * _BUFFER_CAPACITY_MULTIPLIER, 0, حجم(حرف));
                pCSave = pCSrc;
            }ܐܚܪܢܐ{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                اكسر;
            }
        }
        
    }ܐܚܪܢܐ{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =cStackCap;
        
        pSrcLimit = src + srcLength;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ܟܡܐ(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            ܐܢ((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                ܐܢ(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(فراغ**)&pCSrc,&cStackCap,
                           _BUFFER_CAPACITY_MULTIPLIER*cStackCap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                ܐܢ(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }ܐܚܪܢܐ{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ܐܢ(nulLen >= _STACK_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    ܐܢ(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        اذهب cleanup;
                    }
                }
                ܐܢ(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ܐܢ(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(فراغ**)&pCSrc,&cStackCap,
                           cStackCap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                اكسر;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    pCSrcLimit = pCSrc;
    pCSrc = pCSave;
    pTarget = target= dest;
    pTargetLimit = dest + destCapacity;    
    
    conv= u_getDefaultConverter(pErrorCode);
    
    ܐܢ(U_FAILURE(*pErrorCode)|| conv==nullptr){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ثابت حرف**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        ܐܢ(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = uStack;
            pTarget = uStack;
            pTargetLimit = uStack + _STACK_BUFFER_CAPACITY;
        } ܐܚܪܢܐ {
            ܐܢ (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            اكسر;
        }
        
    }
    
    ܐܢ(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    ܐܢ(cStack != pCSave){
        uprv_free(pCSave);
    }

    ܐܢ(wStack != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    ܦܢܐ dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ثابت wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    ܐܢ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ܦܢܐ nullptr;
    }

    ܐܢ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ܐܢ(srcLength == -1){
        srcLength = u_strlen((ثابت char16_t *)src);
    }
    ܐܢ(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ثابت char16_t *)src, srcLength);
    }
    ܐܢ(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    ܦܢܐ dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    ܦܢܐ u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#ܐܚܪܢܐ

    ܦܢܐ _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
