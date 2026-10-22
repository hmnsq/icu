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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cwchar.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "ustr_cnv.h"

#যদি defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define স্তূপ_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#যদি !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
স্থির অন্তঃস্থ UBool 
u_growAnyBufferFromStatic(শূন্য *context,
                       শূন্য **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    বর্ণ *newBuffer=(বর্ণ *)uprv_malloc(reqCapacity*size);
    যদি(newBuffer!=nullptr) {
        যদি(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } অন্যথা {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    যদি(*pBuffer!=(বর্ণ *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    প্রত্যাগম newBuffer!=nullptr;
}

/* helper function */
স্থির wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ধ্রুব char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    বর্ণ স্তূপ_Buffer [স্তূপ_BUFFER_CAPACITY];
    বর্ণ* tempBuf = স্তূপ_Buffer;
    int32_t tempBufCapacity = স্তূপ_BUFFER_CAPACITY;
    বর্ণ* tempBufLimit = স্তূপ_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    বর্ণ* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    পূর্ণাঙ্ক count=0,retVal=0;
    
    ধ্রুব char16_t *pSrcLimit =nullptr;
    ধ্রুব char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    যদি(U_FAILURE(*pErrorCode)){
        প্রত্যাগম nullptr;
    }
    
    যদি(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    প্রতি(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        যদি(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            যদি(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            যদি(!u_growAnyBufferFromStatic(স্তূপ_Buffer,(শূন্য**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                গচ্ছ cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } অন্যথা {
            যদি (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                গচ্ছ cleanup;
            }
            ভঙ্গ;
        }
    }

    /* done with conversion null terminate the char buffer */
    যদি(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        যদি(!u_growAnyBufferFromStatic(স্তূপ_Buffer,(শূন্য**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            গচ্ছ cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * পরিমাণ(wchar_t) );

    যদি(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        প্রতি(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            যদি(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ভঙ্গ;
            }অন্যথা যদি(retVal== remaining){/* should never occur */
                পূর্ণাঙ্ক অঙ্ক_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(শূন্য**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          অঙ্ক_Written,
                                          পরিমাণ(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                যদি(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=অঙ্ক_Written;
                    remaining-=অঙ্ক_Written;
                }

            }অন্যথা{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                যাবত্(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                যদি(nulLen>=(count)){
                    ভঙ্গ;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        যদি(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*পরিমাণ(wchar_t));
        }  

        যদি(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }অন্যথা{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    যদি(স্তূপ_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    প্রত্যাগম dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           ধ্রুব char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        প্রত্যাগম nullptr;
    }
        
    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    যদি(srcLength == -1){
        srcLength = u_strlen(src);
    }
    যদি(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    যদি(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    প্রত্যাগম dest;

#নোচেদ্যদি defined U_WCHAR_IS_UTF32
    
    প্রত্যাগম (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#অন্যথা
    
    প্রত্যাগম _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#যদি !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
স্থির char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             ধ্রুব wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_স্তূপ [স্তূপ_BUFFER_CAPACITY];

    wchar_t w_স্তূপ[স্তূপ_BUFFER_CAPACITY];
    wchar_t* pWStack = w_স্তূপ;


    বর্ণ c_স্তূপ[স্তূপ_BUFFER_CAPACITY];
    int32_t c_স্তূপ_Cap = স্তূপ_BUFFER_CAPACITY;
    বর্ণ* pCSrc=c_স্তূপ;
    বর্ণ* pCSave=pCSrc;
    বর্ণ* pCSrcLimit=nullptr;

    ধ্রুব wchar_t* pSrc = src;
    ধ্রুব wchar_t* pSrcLimit = nullptr;

    যদি(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        প্রতি(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_স্তূপ_Cap);
    
            যদি(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                গচ্ছ cleanup;
            }অন্যথা যদি(retVal >= (c_স্তূপ_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                    c_স্তূপ_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, পরিমাণ(বর্ণ));
                pCSave = pCSrc;
            }অন্যথা{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ভঙ্গ;
            }
        }
        
    }অন্যথা{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_স্তূপ_Cap;
        
        pSrcLimit = src + srcLength;

        প্রতি(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            যাবত্(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            যদি((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                যদি(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_স্তূপ_Cap+(nulLen*MB_CUR_MAX),len,পরিমাণ(বর্ণ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_স্তূপ_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                যদি(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    গচ্ছ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }অন্যথা{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                যদি(nulLen >= স্তূপ_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(পরিমাণ(wchar_t) * (nulLen + 1));
                    যদি(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        গচ্ছ cleanup;
                    }
                }
                যদি(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*পরিমাণ(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                যদি(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                           c_স্তূপ_Cap+(nulLen*MB_CUR_MAX),len,পরিমাণ(বর্ণ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_স্তূপ_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                ভঙ্গ;
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
    
    যদি(U_FAILURE(*pErrorCode)|| conv==nullptr){
        গচ্ছ cleanup;
    }
    
    প্রতি(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ধ্রুব বর্ণ**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        যদি(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_স্তূপ;
            pTarget = u_স্তূপ;
            pTargetLimit = u_স্তূপ + স্তূপ_BUFFER_CAPACITY;
        } অন্যথা {
            যদি (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            ভঙ্গ;
        }
        
    }
    
    যদি(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    যদি(c_স্তূপ != pCSave){
        uprv_free(pCSave);
    }

    যদি(w_স্তূপ != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    প্রত্যাগম dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ধ্রুব wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        প্রত্যাগম nullptr;
    }

    যদি( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    যদি(srcLength == -1){
        srcLength = u_strlen((ধ্রুব char16_t *)src);
    }
    যদি(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ধ্রুব char16_t *)src, srcLength);
    }
    যদি(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    প্রত্যাগম dest;

#নোচেদ্যদি defined U_WCHAR_IS_UTF32
    
    প্রত্যাগম u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#অন্যথা

    প্রত্যাগম _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
