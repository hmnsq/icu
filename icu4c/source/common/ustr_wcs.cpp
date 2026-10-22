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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cwchar.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "ustr_cnv.h"

#ଯଦି defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define ଗଦା_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#ଯଦି !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UBool 
u_growAnyBufferFromStatic(ଶୂନ୍ୟ *context,
                       ଶୂନ୍ୟ **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    ଅକ୍ଷର *newBuffer=(ଅକ୍ଷର *)uprv_malloc(reqCapacity*size);
    ଯଦି(newBuffer!=nullptr) {
        ଯଦି(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } ଅନ୍ୟଥା {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ଯଦି(*pBuffer!=(ଅକ୍ଷର *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    ଫେରନ୍ତୁ newBuffer!=nullptr;
}

/* helper function */
ସ୍ଥିର wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ଅଚଳ char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    ଅକ୍ଷର ଗଦା_Buffer [ଗଦା_BUFFER_CAPACITY];
    ଅକ୍ଷର* tempBuf = ଗଦା_Buffer;
    int32_t tempBufCapacity = ଗଦା_BUFFER_CAPACITY;
    ଅକ୍ଷର* tempBufLimit = ଗଦା_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    ଅକ୍ଷର* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା count=0,retVal=0;
    
    ଅଚଳ char16_t *pSrcLimit =nullptr;
    ଅଚଳ char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    ଯଦି(U_FAILURE(*pErrorCode)){
        ଫେରନ୍ତୁ nullptr;
    }
    
    ଯଦି(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    ପାଇଁ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        ଯଦି(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            ଯଦି(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            ଯଦି(!u_growAnyBufferFromStatic(ଗଦା_Buffer,(ଶୂନ୍ୟ**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                ଯାଅ cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } ଅନ୍ୟଥା {
            ଯଦି (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                ଯାଅ cleanup;
            }
            ଭାଙ୍ଗ;
        }
    }

    /* done with conversion null terminate the char buffer */
    ଯଦି(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ଯଦି(!u_growAnyBufferFromStatic(ଗଦା_Buffer,(ଶୂନ୍ୟ**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            ଯାଅ cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * ଆକାର(wchar_t) );

    ଯଦି(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        ପାଇଁ(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            ଯଦି(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                ଭାଙ୍ଗ;
            }ଅନ୍ୟଥା ଯଦି(retVal== remaining){/* should never occur */
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(ଶୂନ୍ୟ**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          ଅଙ୍କ_Written,
                                          ଆକାର(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                ଯଦି(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=ଅଙ୍କ_Written;
                    remaining-=ଅଙ୍କ_Written;
                }

            }ଅନ୍ୟଥା{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ଯେତେବେଳେ(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ଯଦି(nulLen>=(count)){
                    ଭାଙ୍ଗ;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        ଯଦି(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*ଆକାର(wchar_t));
        }  

        ଯଦି(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }ଅନ୍ୟଥା{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ଯଦି(ଗଦା_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    ଫେରନ୍ତୁ dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           ଅଚଳ char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ଫେରନ୍ତୁ nullptr;
    }
        
    ଯଦି( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ଯଦି(srcLength == -1){
        srcLength = u_strlen(src);
    }
    ଯଦି(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    ଯଦି(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    ଫେରନ୍ତୁ dest;

#ନଚେତଯଦି defined U_WCHAR_IS_UTF32
    
    ଫେରନ୍ତୁ (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#ଅନ୍ୟଥା
    
    ଫେରନ୍ତୁ _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#ଯଦି !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ସ୍ଥିର char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             ଅଚଳ wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_ଗଦା [ଗଦା_BUFFER_CAPACITY];

    wchar_t w_ଗଦା[ଗଦା_BUFFER_CAPACITY];
    wchar_t* pWStack = w_ଗଦା;


    ଅକ୍ଷର c_ଗଦା[ଗଦା_BUFFER_CAPACITY];
    int32_t c_ଗଦା_Cap = ଗଦା_BUFFER_CAPACITY;
    ଅକ୍ଷର* pCSrc=c_ଗଦା;
    ଅକ୍ଷର* pCSave=pCSrc;
    ଅକ୍ଷର* pCSrcLimit=nullptr;

    ଅଚଳ wchar_t* pSrc = src;
    ଅଚଳ wchar_t* pSrcLimit = nullptr;

    ଯଦି(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        ପାଇଁ(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ଗଦା_Cap);
    
            ଯଦି(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                ଯାଅ cleanup;
            }ଅନ୍ୟଥା ଯଦି(retVal >= (c_ଗଦା_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_ଗଦା,(ଶୂନ୍ୟ**)&pCSrc,&c_ଗଦା_Cap,
                    c_ଗଦା_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, ଆକାର(ଅକ୍ଷର));
                pCSave = pCSrc;
            }ଅନ୍ୟଥା{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ଭାଙ୍ଗ;
            }
        }
        
    }ଅନ୍ୟଥା{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_ଗଦା_Cap;
        
        pSrcLimit = src + srcLength;

        ପାଇଁ(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ଯେତେବେଳେ(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            ଯଦି((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                ଯଦି(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ଗଦା,(ଶୂନ୍ୟ**)&pCSrc,&c_ଗଦା_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_ଗଦା_Cap+(nulLen*MB_CUR_MAX),len,ଆକାର(ଅକ୍ଷର));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ଗଦା_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                ଯଦି(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    ଯାଅ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }ଅନ୍ୟଥା{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ଯଦି(nulLen >= ଗଦା_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(ଆକାର(wchar_t) * (nulLen + 1));
                    ଯଦି(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        ଯାଅ cleanup;
                    }
                }
                ଯଦି(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*ଆକାର(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ଯଦି(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ଗଦା,(ଶୂନ୍ୟ**)&pCSrc,&c_ଗଦା_Cap,
                           c_ଗଦା_Cap+(nulLen*MB_CUR_MAX),len,ଆକାର(ଅକ୍ଷର));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ଗଦା_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                ଭାଙ୍ଗ;
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
    
    ଯଦି(U_FAILURE(*pErrorCode)|| conv==nullptr){
        ଯାଅ cleanup;
    }
    
    ପାଇଁ(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ଅଚଳ ଅକ୍ଷର**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        ଯଦି(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_ଗଦା;
            pTarget = u_ଗଦା;
            pTargetLimit = u_ଗଦା + ଗଦା_BUFFER_CAPACITY;
        } ଅନ୍ୟଥା {
            ଯଦି (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            ଭାଙ୍ଗ;
        }
        
    }
    
    ଯଦି(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    ଯଦି(c_ଗଦା != pCSave){
        uprv_free(pCSave);
    }

    ଯଦି(w_ଗଦା != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    ଫେରନ୍ତୁ dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             ଅଚଳ wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    ଯଦି(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ଫେରନ୍ତୁ nullptr;
    }

    ଯଦି( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ଯଦି(srcLength == -1){
        srcLength = u_strlen((ଅଚଳ char16_t *)src);
    }
    ଯଦି(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ଅଚଳ char16_t *)src, srcLength);
    }
    ଯଦି(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    ଫେରନ୍ତୁ dest;

#ନଚେତଯଦି defined U_WCHAR_IS_UTF32
    
    ଫେରନ୍ତୁ u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#ଅନ୍ୟଥା

    ଫେରନ୍ତୁ _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
