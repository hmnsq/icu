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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cwchar.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"

#ಯದಿ defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define ರಾಶಿ_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#ಯದಿ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UBool 
u_growAnyBufferFromStatic(ಶೂನ್ಯ *context,
                       ಶೂನ್ಯ **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    ವರ್ಣ *newBuffer=(ವರ್ಣ *)uprv_malloc(reqCapacity*size);
    ಯದಿ(newBuffer!=nullptr) {
        ಯದಿ(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } ಅನ್ಯಥಾ {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ಯದಿ(*pBuffer!=(ವರ್ಣ *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    ಪ್ರತ್ಯಾಗಮ newBuffer!=nullptr;
}

/* helper function */
ಸ್ಥಿರ wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ಧ್ರುವ char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    ವರ್ಣ ರಾಶಿ_Buffer [ರಾಶಿ_BUFFER_CAPACITY];
    ವರ್ಣ* tempBuf = ರಾಶಿ_Buffer;
    int32_t tempBufCapacity = ರಾಶಿ_BUFFER_CAPACITY;
    ವರ್ಣ* tempBufLimit = ರಾಶಿ_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    ವರ್ಣ* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    ಪೂರ್ಣಾಙ್ಕ count=0,retVal=0;
    
    ಧ್ರುವ char16_t *pSrcLimit =nullptr;
    ಧ್ರುವ char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    ಯದಿ(U_FAILURE(*pErrorCode)){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
    ಯದಿ(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    ಪ್ರತಿ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        ಯದಿ(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            ಯದಿ(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            ಯದಿ(!u_growAnyBufferFromStatic(ರಾಶಿ_Buffer,(ಶೂನ್ಯ**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                ಗಚ್ಛ cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } ಅನ್ಯಥಾ {
            ಯದಿ (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                ಗಚ್ಛ cleanup;
            }
            ಭಙ್ಗ;
        }
    }

    /* done with conversion null terminate the char buffer */
    ಯದಿ(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ಯದಿ(!u_growAnyBufferFromStatic(ರಾಶಿ_Buffer,(ಶೂನ್ಯ**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            ಗಚ್ಛ cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * ಪರಿಮಾಣ(wchar_t) );

    ಯದಿ(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        ಪ್ರತಿ(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            ಯದಿ(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ಭಙ್ಗ;
            }ಅನ್ಯಥಾ ಯದಿ(retVal== remaining){/* should never occur */
                ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(ಶೂನ್ಯ**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          ಅಂಕೆ_Written,
                                          ಪರಿಮಾಣ(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                ಯದಿ(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=ಅಂಕೆ_Written;
                    remaining-=ಅಂಕೆ_Written;
                }

            }ಅನ್ಯಥಾ{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ಯಾವತ್(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ಯದಿ(nulLen>=(count)){
                    ಭಙ್ಗ;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        ಯದಿ(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*ಪರಿಮಾಣ(wchar_t));
        }  

        ಯದಿ(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }ಅನ್ಯಥಾ{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ಯದಿ(ರಾಶಿ_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    ಪ್ರತ್ಯಾಗಮ dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           ಧ್ರುವ char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
        
    ಯದಿ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ಯದಿ(srcLength == -1){
        srcLength = u_strlen(src);
    }
    ಯದಿ(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    ಯದಿ(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    ಪ್ರತ್ಯಾಗಮ dest;

#ನೋಚೇದ್ಯದಿ defined U_WCHAR_IS_UTF32
    
    ಪ್ರತ್ಯಾಗಮ (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#ಅನ್ಯಥಾ
    
    ಪ್ರತ್ಯಾಗಮ _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#ಯದಿ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ಸ್ಥಿರ char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             ಧ್ರುವ wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_ರಾಶಿ [ರಾಶಿ_BUFFER_CAPACITY];

    wchar_t w_ರಾಶಿ[ರಾಶಿ_BUFFER_CAPACITY];
    wchar_t* pWStack = w_ರಾಶಿ;


    ವರ್ಣ c_ರಾಶಿ[ರಾಶಿ_BUFFER_CAPACITY];
    int32_t c_ರಾಶಿ_Cap = ರಾಶಿ_BUFFER_CAPACITY;
    ವರ್ಣ* pCSrc=c_ರಾಶಿ;
    ವರ್ಣ* pCSave=pCSrc;
    ವರ್ಣ* pCSrcLimit=nullptr;

    ಧ್ರುವ wchar_t* pSrc = src;
    ಧ್ರುವ wchar_t* pSrcLimit = nullptr;

    ಯದಿ(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        ಪ್ರತಿ(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ರಾಶಿ_Cap);
    
            ಯದಿ(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                ಗಚ್ಛ cleanup;
            }ಅನ್ಯಥಾ ಯದಿ(retVal >= (c_ರಾಶಿ_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                    c_ರಾಶಿ_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, ಪರಿಮಾಣ(ವರ್ಣ));
                pCSave = pCSrc;
            }ಅನ್ಯಥಾ{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ಭಙ್ಗ;
            }
        }
        
    }ಅನ್ಯಥಾ{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_ರಾಶಿ_Cap;
        
        pSrcLimit = src + srcLength;

        ಪ್ರತಿ(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ಯಾವತ್(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            ಯದಿ((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                ಯದಿ(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_ರಾಶಿ_Cap+(nulLen*MB_CUR_MAX),len,ಪರಿಮಾಣ(ವರ್ಣ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ರಾಶಿ_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                ಯದಿ(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    ಗಚ್ಛ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }ಅನ್ಯಥಾ{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ಯದಿ(nulLen >= ರಾಶಿ_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(ಪರಿಮಾಣ(wchar_t) * (nulLen + 1));
                    ಯದಿ(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        ಗಚ್ಛ cleanup;
                    }
                }
                ಯದಿ(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*ಪರಿಮಾಣ(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ಯದಿ(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                           c_ರಾಶಿ_Cap+(nulLen*MB_CUR_MAX),len,ಪರಿಮಾಣ(ವರ್ಣ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ರಾಶಿ_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                ಭಙ್ಗ;
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
    
    ಯದಿ(U_FAILURE(*pErrorCode)|| conv==nullptr){
        ಗಚ್ಛ cleanup;
    }
    
    ಪ್ರತಿ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ಧ್ರುವ ವರ್ಣ**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        ಯದಿ(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_ರಾಶಿ;
            pTarget = u_ರಾಶಿ;
            pTargetLimit = u_ರಾಶಿ + ರಾಶಿ_BUFFER_CAPACITY;
        } ಅನ್ಯಥಾ {
            ಯದಿ (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            ಭಙ್ಗ;
        }
        
    }
    
    ಯದಿ(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    ಯದಿ(c_ರಾಶಿ != pCSave){
        uprv_free(pCSave);
    }

    ಯದಿ(w_ರಾಶಿ != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    ಪ್ರತ್ಯಾಗಮ dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ಧ್ರುವ wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    ಯದಿ( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ಯದಿ(srcLength == -1){
        srcLength = u_strlen((ಧ್ರುವ char16_t *)src);
    }
    ಯದಿ(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ಧ್ರುವ char16_t *)src, srcLength);
    }
    ಯದಿ(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    ಪ್ರತ್ಯಾಗಮ dest;

#ನೋಚೇದ್ಯದಿ defined U_WCHAR_IS_UTF32
    
    ಪ್ರತ್ಯಾಗಮ u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#ಅನ್ಯಥಾ

    ಪ್ರತ್ಯಾಗಮ _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
