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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cwchar.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ustr_cnv.h"

#सचे defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define राशिः_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#सचे !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
स्थिर अन्तःस्थ UBool 
u_growAnyBufferFromStatic(शून्य *context,
                       शून्य **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    वर्ण *newBuffer=(वर्ण *)uprv_malloc(reqCapacity*size);
    सचे(newBuffer!=nullptr) {
        सचे(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } अञ्ञथा {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    सचे(*pBuffer!=(वर्ण *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    पटिनिवत्त newBuffer!=nullptr;
}

/* helper function */
स्थिर wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ध्रुव char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    वर्ण राशिः_Buffer [राशिः_BUFFER_CAPACITY];
    वर्ण* tempBuf = राशिः_Buffer;
    int32_t tempBufCapacity = राशिः_BUFFER_CAPACITY;
    वर्ण* tempBufLimit = राशिः_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    वर्ण* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    पूर्णाङ्क count=0,retVal=0;
    
    ध्रुव char16_t *pSrcLimit =nullptr;
    ध्रुव char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    सचे(U_FAILURE(*pErrorCode)){
        पटिनिवत्त nullptr;
    }
    
    सचे(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    अत्थाय(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        सचे(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            सचे(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            सचे(!u_growAnyBufferFromStatic(राशिः_Buffer,(शून्य**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                गच्छ cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } अञ्ञथा {
            सचे (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                गच्छ cleanup;
            }
            भङ्ग;
        }
    }

    /* done with conversion null terminate the char buffer */
    सचे(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        सचे(!u_growAnyBufferFromStatic(राशिः_Buffer,(शून्य**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            गच्छ cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * परिमाण(wchar_t) );

    सचे(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        अत्थाय(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            सचे(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                भङ्ग;
            }अञ्ञथा सचे(retVal== remaining){/* should never occur */
                पूर्णाङ्क अङ्कः_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(शून्य**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          अङ्कः_Written,
                                          परिमाण(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                सचे(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=अङ्कः_Written;
                    remaining-=अङ्कः_Written;
                }

            }अञ्ञथा{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                याव(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                सचे(nulLen>=(count)){
                    भङ्ग;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        सचे(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*परिमाण(wchar_t));
        }  

        सचे(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }अञ्ञथा{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    सचे(राशिः_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    पटिनिवत्त dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           ध्रुव char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    सचे(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        पटिनिवत्त nullptr;
    }
        
    सचे( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    सचे(srcLength == -1){
        srcLength = u_strlen(src);
    }
    सचे(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    सचे(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    पटिनिवत्त dest;

#नोचेद्यदि defined U_WCHAR_IS_UTF32
    
    पटिनिवत्त (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#अञ्ञथा
    
    पटिनिवत्त _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#सचे !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
स्थिर char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             ध्रुव wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_राशिः [राशिः_BUFFER_CAPACITY];

    wchar_t w_राशिः[राशिः_BUFFER_CAPACITY];
    wchar_t* pWStack = w_राशिः;


    वर्ण c_राशिः[राशिः_BUFFER_CAPACITY];
    int32_t c_राशिः_Cap = राशिः_BUFFER_CAPACITY;
    वर्ण* pCSrc=c_राशिः;
    वर्ण* pCSave=pCSrc;
    वर्ण* pCSrcLimit=nullptr;

    ध्रुव wchar_t* pSrc = src;
    ध्रुव wchar_t* pSrcLimit = nullptr;

    सचे(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        अत्थाय(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_राशिः_Cap);
    
            सचे(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                गच्छ cleanup;
            }अञ्ञथा सचे(retVal >= (c_राशिः_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                    c_राशिः_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, परिमाण(वर्ण));
                pCSave = pCSrc;
            }अञ्ञथा{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                भङ्ग;
            }
        }
        
    }अञ्ञथा{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_राशिः_Cap;
        
        pSrcLimit = src + srcLength;

        अत्थाय(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            याव(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            सचे((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                सचे(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_राशिः_Cap+(nulLen*MB_CUR_MAX),len,परिमाण(वर्ण));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_राशिः_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                सचे(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    गच्छ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }अञ्ञथा{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                सचे(nulLen >= राशिः_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(परिमाण(wchar_t) * (nulLen + 1));
                    सचे(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        गच्छ cleanup;
                    }
                }
                सचे(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*परिमाण(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                सचे(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                           c_राशिः_Cap+(nulLen*MB_CUR_MAX),len,परिमाण(वर्ण));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_राशिः_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                भङ्ग;
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
    
    सचे(U_FAILURE(*pErrorCode)|| conv==nullptr){
        गच्छ cleanup;
    }
    
    अत्थाय(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ध्रुव वर्ण**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        सचे(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_राशिः;
            pTarget = u_राशिः;
            pTargetLimit = u_राशिः + राशिः_BUFFER_CAPACITY;
        } अञ्ञथा {
            सचे (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            भङ्ग;
        }
        
    }
    
    सचे(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    सचे(c_राशिः != pCSave){
        uprv_free(pCSave);
    }

    सचे(w_राशिः != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    पटिनिवत्त dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ध्रुव wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    सचे(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        पटिनिवत्त nullptr;
    }

    सचे( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    सचे(srcLength == -1){
        srcLength = u_strlen((ध्रुव char16_t *)src);
    }
    सचे(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ध्रुव char16_t *)src, srcLength);
    }
    सचे(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    पटिनिवत्त dest;

#नोचेद्यदि defined U_WCHAR_IS_UTF32
    
    पटिनिवत्त u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#अञ्ञथा

    पटिनिवत्त _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
