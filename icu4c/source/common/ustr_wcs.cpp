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

#إذا defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define تر_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_growAnyBufferFromStatic(فراغ *context,
                       فراغ **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *newBuffer=(حرف *)uprv_malloc(reqCapacity*size);
    إذا(newBuffer!=nullptr) {
        إذا(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } وإلا {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    إذا(*pBuffer!=(حرف *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    أرجع newBuffer!=nullptr;
}

/* helper function */
ساكن wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ثابت char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    حرف تر_Buffer [تر_BUFFER_CAPACITY];
    حرف* tempBuf = تر_Buffer;
    int32_t tempBufCapacity = تر_BUFFER_CAPACITY;
    حرف* tempBufLimit = تر_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    حرف* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *pSrcLimit =nullptr;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    إذا(U_FAILURE(*pErrorCode)){
        أرجع nullptr;
    }
    
    إذا(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    لكل(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        إذا(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            إذا(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            إذا(!u_growAnyBufferFromStatic(تر_Buffer,(فراغ**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } وإلا {
            إذا (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    إذا(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        إذا(!u_growAnyBufferFromStatic(تر_Buffer,(فراغ**) &tempBuf, &tempBufCapacity, 
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

    إذا(intTarget){

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
            
            إذا(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                اكسر;
            }وإلا إذا(retVal== remaining){/* should never occur */
                صحيح لمب_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(فراغ**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          لمب_Written,
                                          حجم(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                إذا(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=لمب_Written;
                    remaining-=لمب_Written;
                }

            }وإلا{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                طالما(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                إذا(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        إذا(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*حجم(wchar_t));
        }  

        إذا(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }وإلا{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    إذا(تر_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    أرجع dest;
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
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        أرجع nullptr;
    }
        
    إذا( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(srcLength == -1){
        srcLength = u_strlen(src);
    }
    إذا(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    إذا(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#وإلا
    
    أرجع _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
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
    
    char16_t u_تر [تر_BUFFER_CAPACITY];

    wchar_t w_تر[تر_BUFFER_CAPACITY];
    wchar_t* pWStack = w_تر;


    حرف c_تر[تر_BUFFER_CAPACITY];
    int32_t c_تر_Cap = تر_BUFFER_CAPACITY;
    حرف* pCSrc=c_تر;
    حرف* pCSave=pCSrc;
    حرف* pCSrcLimit=nullptr;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* pSrcLimit = nullptr;

    إذا(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_تر_Cap);
    
            إذا(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                اذهب cleanup;
            }وإلا إذا(retVal >= (c_تر_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_تر,(فراغ**)&pCSrc,&c_تر_Cap,
                    c_تر_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, حجم(حرف));
                pCSave = pCSrc;
            }وإلا{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                اكسر;
            }
        }
        
    }وإلا{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_تر_Cap;
        
        pSrcLimit = src + srcLength;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            طالما(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            إذا((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                إذا(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_تر,(فراغ**)&pCSrc,&c_تر_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_تر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_تر_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                إذا(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }وإلا{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                إذا(nulLen >= تر_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    إذا(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        اذهب cleanup;
                    }
                }
                إذا(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                إذا(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_تر,(فراغ**)&pCSrc,&c_تر_Cap,
                           c_تر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_تر_Cap-(pCSrc - pCSave);
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
    
    إذا(U_FAILURE(*pErrorCode)|| conv==nullptr){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ثابت حرف**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        إذا(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_تر;
            pTarget = u_تر;
            pTargetLimit = u_تر + تر_BUFFER_CAPACITY;
        } وإلا {
            إذا (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            اكسر;
        }
        
    }
    
    إذا(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    إذا(c_تر != pCSave){
        uprv_free(pCSave);
    }

    إذا(w_تر != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    أرجع dest;
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
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        أرجع nullptr;
    }

    إذا( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(srcLength == -1){
        srcLength = u_strlen((ثابت char16_t *)src);
    }
    إذا(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ثابت char16_t *)src, srcLength);
    }
    إذا(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#وإلا

    أرجع _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
