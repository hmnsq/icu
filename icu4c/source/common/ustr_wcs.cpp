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

#شامل "unicode/utypes.h"
#شامل "unicode/ustring.h"
#شامل "cstring.h"
#شامل "cwchar.h"
#شامل "cmemory.h"
#شامل "ustr_imp.h"
#شامل "ustr_cnv.h"

#اگر defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define پشته_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#اگر !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ایستا درونخطی UBool 
u_growAnyBufferFromStatic(تهی *context,
                       تهی **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    نویسه *newBuffer=(نویسه *)uprv_malloc(reqCapacity*size);
    اگر(newBuffer!=nullptr) {
        اگر(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } وگرنه {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    اگر(*pBuffer!=(نویسه *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    بازگشت newBuffer!=nullptr;
}

/* helper function */
ایستا wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           ثابت char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    نویسه پشته_Buffer [پشته_BUFFER_CAPACITY];
    نویسه* tempBuf = پشته_Buffer;
    int32_t tempBufCapacity = پشته_BUFFER_CAPACITY;
    نویسه* tempBufLimit = پشته_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    نویسه* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    صحیح count=0,retVal=0;
    
    ثابت char16_t *pSrcLimit =nullptr;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    اگر(U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
    
    اگر(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    برای(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        اگر(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            اگر(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            اگر(!u_growAnyBufferFromStatic(پشته_Buffer,(تهی**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                برو cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } وگرنه {
            اگر (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                برو cleanup;
            }
            بشکن;
        }
    }

    /* done with conversion null terminate the char buffer */
    اگر(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        اگر(!u_growAnyBufferFromStatic(پشته_Buffer,(تهی**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            برو cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * اندازه(wchar_t) );

    اگر(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        برای(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            اگر(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                بشکن;
            }وگرنه اگر(retVal== remaining){/* should never occur */
                صحیح رقم_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(تهی**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          رقم_Written,
                                          اندازه(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                اگر(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=رقم_Written;
                    remaining-=رقم_Written;
                }

            }وگرنه{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                وقتی(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                اگر(nulLen>=(count)){
                    بشکن;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        اگر(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*اندازه(wchar_t));
        }  

        اگر(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }وگرنه{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    اگر(پشته_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    بازگشت dest;
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
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }
        
    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    اگر(srcLength == -1){
        srcLength = u_strlen(src);
    }
    اگر(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    اگر(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    بازگشت dest;

#وگرنهاگر defined U_WCHAR_IS_UTF32
    
    بازگشت (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#وگرنه
    
    بازگشت _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#اگر !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ایستا char16_t*
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
    
    char16_t u_پشته [پشته_BUFFER_CAPACITY];

    wchar_t w_پشته[پشته_BUFFER_CAPACITY];
    wchar_t* pWStack = w_پشته;


    نویسه c_پشته[پشته_BUFFER_CAPACITY];
    int32_t c_پشته_Cap = پشته_BUFFER_CAPACITY;
    نویسه* pCSrc=c_پشته;
    نویسه* pCSave=pCSrc;
    نویسه* pCSrcLimit=nullptr;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* pSrcLimit = nullptr;

    اگر(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        برای(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_پشته_Cap);
    
            اگر(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                برو cleanup;
            }وگرنه اگر(retVal >= (c_پشته_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_پشته,(تهی**)&pCSrc,&c_پشته_Cap,
                    c_پشته_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, اندازه(نویسه));
                pCSave = pCSrc;
            }وگرنه{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                بشکن;
            }
        }
        
    }وگرنه{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_پشته_Cap;
        
        pSrcLimit = src + srcLength;

        برای(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            وقتی(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            اگر((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                اگر(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_پشته,(تهی**)&pCSrc,&c_پشته_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_پشته_Cap+(nulLen*MB_CUR_MAX),len,اندازه(نویسه));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_پشته_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                اگر(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    برو cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }وگرنه{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                اگر(nulLen >= پشته_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(اندازه(wchar_t) * (nulLen + 1));
                    اگر(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        برو cleanup;
                    }
                }
                اگر(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*اندازه(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                اگر(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_پشته,(تهی**)&pCSrc,&c_پشته_Cap,
                           c_پشته_Cap+(nulLen*MB_CUR_MAX),len,اندازه(نویسه));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_پشته_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                بشکن;
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
    
    اگر(U_FAILURE(*pErrorCode)|| conv==nullptr){
        برو cleanup;
    }
    
    برای(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ثابت نویسه**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        اگر(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_پشته;
            pTarget = u_پشته;
            pTargetLimit = u_پشته + پشته_BUFFER_CAPACITY;
        } وگرنه {
            اگر (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            بشکن;
        }
        
    }
    
    اگر(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    اگر(c_پشته != pCSave){
        uprv_free(pCSave);
    }

    اگر(w_پشته != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    بازگشت dest;
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
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        بازگشت nullptr;
    }

    اگر( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    اگر(srcLength == -1){
        srcLength = u_strlen((ثابت char16_t *)src);
    }
    اگر(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (ثابت char16_t *)src, srcLength);
    }
    اگر(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    بازگشت dest;

#وگرنهاگر defined U_WCHAR_IS_UTF32
    
    بازگشت u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#وگرنه

    بازگشت _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
