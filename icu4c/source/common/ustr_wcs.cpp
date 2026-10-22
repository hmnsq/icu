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

#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "cstring.h"
#include "cwchar.h"
#include "cmemory.h"
#include "ustr_imp.h"
#include "ustr_cnv.h"

#nếu defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define _STACK_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#nếu !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
static inline UBool 
u_growAnyBufferFromStatic(void *context,
                       void **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    char *newBuffer=(char *)uprv_malloc(reqCapacity*size);
    nếu(newBuffer!=nullptr) {
        nếu(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } không_thì {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    nếu(*pBuffer!=(char *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    trả_về newBuffer!=nullptr;
}

/* helper function */
static wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           const char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    char stackBuffer [_STACK_BUFFER_CAPACITY];
    char* tempBuf = stackBuffer;
    int32_t tempBufCapacity = _STACK_BUFFER_CAPACITY;
    char* tempBufLimit = stackBuffer + tempBufCapacity;
    UConverter* conv = nullptr;
    char* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    số_nguyên count=0,retVal=0;
    
    const char16_t *pSrcLimit =nullptr;
    const char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    nếu(U_FAILURE(*pErrorCode)){
        trả_về nullptr;
    }
    
    nếu(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    cho(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        nếu(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            nếu(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            nếu(!u_growAnyBufferFromStatic(stackBuffer,(void**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                goto cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } không_thì {
            nếu (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                goto cleanup;
            }
            break;
        }
    }

    /* done with conversion null terminate the char buffer */
    nếu(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        nếu(!u_growAnyBufferFromStatic(stackBuffer,(void**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            goto cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * sizeof(wchar_t) );

    nếu(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        cho(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            nếu(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                break;
            }không_thì nếu(retVal== remaining){/* should never occur */
                số_nguyên numWritten = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(void**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          numWritten,
                                          sizeof(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                nếu(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=numWritten;
                    remaining-=numWritten;
                }

            }không_thì{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                trong_khi(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                nếu(nulLen>=(count)){
                    break;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        nếu(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*sizeof(wchar_t));
        }  

        nếu(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }không_thì{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    nếu(stackBuffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    trả_về dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           const char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    nếu(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        trả_về nullptr;
    }
        
    nếu( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    nếu(srcLength == -1){
        srcLength = u_strlen(src);
    }
    nếu(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    nếu(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    trả_về dest;

#elif defined U_WCHAR_IS_UTF32
    
    trả_về (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#không_thì
    
    trả_về _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#nếu !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
static char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             const wchar_t *src,
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


    char cStack[_STACK_BUFFER_CAPACITY];
    int32_t cStackCap = _STACK_BUFFER_CAPACITY;
    char* pCSrc=cStack;
    char* pCSave=pCSrc;
    char* pCSrcLimit=nullptr;

    const wchar_t* pSrc = src;
    const wchar_t* pSrcLimit = nullptr;

    nếu(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        cho(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, cStackCap);
    
            nếu(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                goto cleanup;
            }không_thì nếu(retVal >= (cStackCap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                    cStackCap * _BUFFER_CAPACITY_MULTIPLIER, 0, sizeof(char));
                pCSave = pCSrc;
            }không_thì{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                break;
            }
        }
        
    }không_thì{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =cStackCap;
        
        pSrcLimit = src + srcLength;

        cho(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            trong_khi(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            nếu((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                nếu(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                           _BUFFER_CAPACITY_MULTIPLIER*cStackCap+(nulLen*MB_CUR_MAX),len,sizeof(char));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                nếu(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    goto cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }không_thì{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                nếu(nulLen >= _STACK_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(sizeof(wchar_t) * (nulLen + 1));
                    nếu(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        goto cleanup;
                    }
                }
                nếu(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*sizeof(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                nếu(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                           cStackCap+(nulLen*MB_CUR_MAX),len,sizeof(char));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                break;
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
    
    nếu(U_FAILURE(*pErrorCode)|| conv==nullptr){
        goto cleanup;
    }
    
    cho(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(const char**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        nếu(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = uStack;
            pTarget = uStack;
            pTargetLimit = uStack + _STACK_BUFFER_CAPACITY;
        } không_thì {
            nếu (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            break;
        }
        
    }
    
    nếu(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    nếu(cStack != pCSave){
        uprv_free(pCSave);
    }

    nếu(wStack != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    trả_về dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             const wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    nếu(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        trả_về nullptr;
    }

    nếu( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    nếu(srcLength == -1){
        srcLength = u_strlen((const char16_t *)src);
    }
    nếu(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (const char16_t *)src, srcLength);
    }
    nếu(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    trả_về dest;

#elif defined U_WCHAR_IS_UTF32
    
    trả_về u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#không_thì

    trả_về _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
