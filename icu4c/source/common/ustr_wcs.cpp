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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cwchar.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "ustr_cnv.h"

#எனில் defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define அடுக்கு_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#எனில் !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
நிலையான உள்வரி UBool 
u_growAnyBufferFromStatic(வெறுமை *context,
                       வெறுமை **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    எழுத்து *newBuffer=(எழுத்து *)uprv_malloc(reqCapacity*size);
    எனில்(newBuffer!=nullptr) {
        எனில்(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } இல்லையெனில் {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    எனில்(*pBuffer!=(எழுத்து *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    திரும்பு newBuffer!=nullptr;
}

/* helper function */
நிலையான wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           மாறிலி char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    எழுத்து அடுக்கு_Buffer [அடுக்கு_BUFFER_CAPACITY];
    எழுத்து* tempBuf = அடுக்கு_Buffer;
    int32_t tempBufCapacity = அடுக்கு_BUFFER_CAPACITY;
    எழுத்து* tempBufLimit = அடுக்கு_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    எழுத்து* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    முழு count=0,retVal=0;
    
    மாறிலி char16_t *pSrcLimit =nullptr;
    மாறிலி char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    எனில்(U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
    
    எனில்(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    க்கு(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        எனில்(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            எனில்(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            எனில்(!u_growAnyBufferFromStatic(அடுக்கு_Buffer,(வெறுமை**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                செல் cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } இல்லையெனில் {
            எனில் (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                செல் cleanup;
            }
            நிறுத்து;
        }
    }

    /* done with conversion null terminate the char buffer */
    எனில்(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        எனில்(!u_growAnyBufferFromStatic(அடுக்கு_Buffer,(வெறுமை**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            செல் cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * அளவு(wchar_t) );

    எனில்(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        க்கு(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            எனில்(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                நிறுத்து;
            }இல்லையெனில் எனில்(retVal== remaining){/* should never occur */
                முழு இலக்கம்_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(வெறுமை**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          இலக்கம்_Written,
                                          அளவு(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                எனில்(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=இலக்கம்_Written;
                    remaining-=இலக்கம்_Written;
                }

            }இல்லையெனில்{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                போது(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                எனில்(nulLen>=(count)){
                    நிறுத்து;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        எனில்(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*அளவு(wchar_t));
        }  

        எனில்(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }இல்லையெனில்{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    எனில்(அடுக்கு_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    திரும்பு dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           மாறிலி char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }
        
    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    எனில்(srcLength == -1){
        srcLength = u_strlen(src);
    }
    எனில்(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    எனில்(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    திரும்பு dest;

#அல்லதெனில் defined U_WCHAR_IS_UTF32
    
    திரும்பு (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#இல்லையெனில்
    
    திரும்பு _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#எனில் !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
நிலையான char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             மாறிலி wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_அடுக்கு [அடுக்கு_BUFFER_CAPACITY];

    wchar_t w_அடுக்கு[அடுக்கு_BUFFER_CAPACITY];
    wchar_t* pWStack = w_அடுக்கு;


    எழுத்து c_அடுக்கு[அடுக்கு_BUFFER_CAPACITY];
    int32_t c_அடுக்கு_Cap = அடுக்கு_BUFFER_CAPACITY;
    எழுத்து* pCSrc=c_அடுக்கு;
    எழுத்து* pCSave=pCSrc;
    எழுத்து* pCSrcLimit=nullptr;

    மாறிலி wchar_t* pSrc = src;
    மாறிலி wchar_t* pSrcLimit = nullptr;

    எனில்(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        க்கு(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_அடுக்கு_Cap);
    
            எனில்(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                செல் cleanup;
            }இல்லையெனில் எனில்(retVal >= (c_அடுக்கு_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_அடுக்கு,(வெறுமை**)&pCSrc,&c_அடுக்கு_Cap,
                    c_அடுக்கு_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, அளவு(எழுத்து));
                pCSave = pCSrc;
            }இல்லையெனில்{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                நிறுத்து;
            }
        }
        
    }இல்லையெனில்{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_அடுக்கு_Cap;
        
        pSrcLimit = src + srcLength;

        க்கு(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            போது(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            எனில்((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                எனில்(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_அடுக்கு,(வெறுமை**)&pCSrc,&c_அடுக்கு_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_அடுக்கு_Cap+(nulLen*MB_CUR_MAX),len,அளவு(எழுத்து));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_அடுக்கு_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                எனில்(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    செல் cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }இல்லையெனில்{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                எனில்(nulLen >= அடுக்கு_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(அளவு(wchar_t) * (nulLen + 1));
                    எனில்(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        செல் cleanup;
                    }
                }
                எனில்(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*அளவு(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                எனில்(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_அடுக்கு,(வெறுமை**)&pCSrc,&c_அடுக்கு_Cap,
                           c_அடுக்கு_Cap+(nulLen*MB_CUR_MAX),len,அளவு(எழுத்து));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_அடுக்கு_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                நிறுத்து;
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
    
    எனில்(U_FAILURE(*pErrorCode)|| conv==nullptr){
        செல் cleanup;
    }
    
    க்கு(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(மாறிலி எழுத்து**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        எனில்(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_அடுக்கு;
            pTarget = u_அடுக்கு;
            pTargetLimit = u_அடுக்கு + அடுக்கு_BUFFER_CAPACITY;
        } இல்லையெனில் {
            எனில் (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            நிறுத்து;
        }
        
    }
    
    எனில்(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    எனில்(c_அடுக்கு != pCSave){
        uprv_free(pCSave);
    }

    எனில்(w_அடுக்கு != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    திரும்பு dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             மாறிலி wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        திரும்பு nullptr;
    }

    எனில்( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    எனில்(srcLength == -1){
        srcLength = u_strlen((மாறிலி char16_t *)src);
    }
    எனில்(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (மாறிலி char16_t *)src, srcLength);
    }
    எனில்(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    திரும்பு dest;

#அல்லதெனில் defined U_WCHAR_IS_UTF32
    
    திரும்பு u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#இல்லையெனில்

    திரும்பு _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
