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

#ถ้า defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define _STACK_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#ถ้า !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
static anta_hstha UBool 
u_growAnyBufferFromStatic(void *context,
                       void **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    var_na *newBuffer=(var_na *)uprv_malloc(reqCapacity*size);
    ถ้า(newBuffer!=nullptr) {
        ถ้า(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } มิฉะนั้น {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ถ้า(*pBuffer!=(var_na *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    คืน newBuffer!=nullptr;
}

/* helper function */
static wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           const char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    var_na stackBuffer [_STACK_BUFFER_CAPACITY];
    var_na* tempBuf = stackBuffer;
    int32_t tempBufCapacity = _STACK_BUFFER_CAPACITY;
    var_na* tempBufLimit = stackBuffer + tempBufCapacity;
    UConverter* conv = nullptr;
    var_na* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    จำนวนเต็ม count=0,retVal=0;
    
    const char16_t *pSrcLimit =nullptr;
    const char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    ถ้า(U_FAILURE(*pErrorCode)){
        คืน nullptr;
    }
    
    ถ้า(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    สำหรับ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        ถ้า(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            ถ้า(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            ถ้า(!u_growAnyBufferFromStatic(stackBuffer,(void**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                goto cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } มิฉะนั้น {
            ถ้า (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                goto cleanup;
            }
            bha_nga;
        }
    }

    /* done with conversion null terminate the char buffer */
    ถ้า(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ถ้า(!u_growAnyBufferFromStatic(stackBuffer,(void**) &tempBuf, &tempBufCapacity, 
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
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * parimaa_na(wchar_t) );

    ถ้า(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        สำหรับ(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            ถ้า(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                bha_nga;
            }มิฉะนั้น ถ้า(retVal== remaining){/* should never occur */
                จำนวนเต็ม numWritten = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(void**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          numWritten,
                                          parimaa_na(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                ถ้า(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=numWritten;
                    remaining-=numWritten;
                }

            }มิฉะนั้น{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ขณะ(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ถ้า(nulLen>=(count)){
                    bha_nga;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        ถ้า(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*parimaa_na(wchar_t));
        }  

        ถ้า(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }มิฉะนั้น{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ถ้า(stackBuffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    คืน dest;
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
    ถ้า(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        คืน nullptr;
    }
        
    ถ้า( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ถ้า(srcLength == -1){
        srcLength = u_strlen(src);
    }
    ถ้า(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    ถ้า(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    คืน dest;

#elif defined U_WCHAR_IS_UTF32
    
    คืน (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#มิฉะนั้น
    
    คืน _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#ถ้า !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
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


    var_na cStack[_STACK_BUFFER_CAPACITY];
    int32_t cStackCap = _STACK_BUFFER_CAPACITY;
    var_na* pCSrc=cStack;
    var_na* pCSave=pCSrc;
    var_na* pCSrcLimit=nullptr;

    const wchar_t* pSrc = src;
    const wchar_t* pSrcLimit = nullptr;

    ถ้า(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        สำหรับ(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, cStackCap);
    
            ถ้า(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                goto cleanup;
            }มิฉะนั้น ถ้า(retVal >= (cStackCap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                    cStackCap * _BUFFER_CAPACITY_MULTIPLIER, 0, parimaa_na(var_na));
                pCSave = pCSrc;
            }มิฉะนั้น{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                bha_nga;
            }
        }
        
    }มิฉะนั้น{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =cStackCap;
        
        pSrcLimit = src + srcLength;

        สำหรับ(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ขณะ(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            ถ้า((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                ถ้า(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                           _BUFFER_CAPACITY_MULTIPLIER*cStackCap+(nulLen*MB_CUR_MAX),len,parimaa_na(var_na));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                ถ้า(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    goto cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }มิฉะนั้น{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ถ้า(nulLen >= _STACK_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(parimaa_na(wchar_t) * (nulLen + 1));
                    ถ้า(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        goto cleanup;
                    }
                }
                ถ้า(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*parimaa_na(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ถ้า(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(cStack,(void**)&pCSrc,&cStackCap,
                           cStackCap+(nulLen*MB_CUR_MAX),len,parimaa_na(var_na));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = cStackCap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                bha_nga;
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
    
    ถ้า(U_FAILURE(*pErrorCode)|| conv==nullptr){
        goto cleanup;
    }
    
    สำหรับ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(const var_na**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        ถ้า(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = uStack;
            pTarget = uStack;
            pTargetLimit = uStack + _STACK_BUFFER_CAPACITY;
        } มิฉะนั้น {
            ถ้า (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            bha_nga;
        }
        
    }
    
    ถ้า(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    ถ้า(cStack != pCSave){
        uprv_free(pCSave);
    }

    ถ้า(wStack != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    คืน dest;
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
    ถ้า(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        คืน nullptr;
    }

    ถ้า( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        คืน nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ถ้า(srcLength == -1){
        srcLength = u_strlen((const char16_t *)src);
    }
    ถ้า(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (const char16_t *)src, srcLength);
    }
    ถ้า(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    คืน dest;

#elif defined U_WCHAR_IS_UTF32
    
    คืน u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#มิฉะนั้น

    คืน _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
