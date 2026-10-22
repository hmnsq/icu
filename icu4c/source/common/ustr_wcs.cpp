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

#जर defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define चळत_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#जर !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
स्थिर ओळीत UBool 
u_growAnyBufferFromStatic(शून्य *context,
                       शून्य **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    अक्षर *newBuffer=(अक्षर *)uprv_malloc(reqCapacity*size);
    जर(newBuffer!=nullptr) {
        जर(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (माप_प्रकार)length*size);
        }
        *pCapacity=reqCapacity;
    } अन्यथा {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    जर(*pBuffer!=(अक्षर *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    परत newBuffer!=nullptr;
}

/* helper function */
स्थिर ब_अक्षर_प्रकार* 
_strToWCS(ब_अक्षर_प्रकार *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           स्थिरांक char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    अक्षर चळत_Buffer [चळत_BUFFER_CAPACITY];
    अक्षर* tempBuf = चळत_Buffer;
    int32_t tempBufCapacity = चळत_BUFFER_CAPACITY;
    अक्षर* tempBufLimit = चळत_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    अक्षर* saveBuf = tempBuf;
    ब_अक्षर_प्रकार* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    पूर्णांक count=0,retVal=0;
    
    स्थिरांक char16_t *pSrcLimit =nullptr;
    स्थिरांक char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    जर(U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
    
    जर(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    साठी(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        जर(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            जर(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            जर(!u_growAnyBufferFromStatic(चळत_Buffer,(शून्य**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                जा cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } अन्यथा {
            जर (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                जा cleanup;
            }
            थांबा;
        }
    }

    /* done with conversion null terminate the char buffer */
    जर(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        जर(!u_growAnyBufferFromStatic(चळत_Buffer,(शून्य**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            जा cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (ब_अक्षर_प्रकार*)uprv_malloc( intTargetCapacity * आकार(ब_अक्षर_प्रकार) );

    जर(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        ब_अक्षर_प्रकार* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        साठी(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            जर(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                थांबा;
            }अन्यथा जर(retVal== remaining){/* should never occur */
                पूर्णांक अंक_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(शून्य**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          अंक_Written,
                                          आकार(ब_अक्षर_प्रकार));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                जर(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=अंक_Written;
                    remaining-=अंक_Written;
                }

            }अन्यथा{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                पर्यंत(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                जर(nulLen>=(count)){
                    थांबा;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        जर(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (माप_प्रकार)count*आकार(ब_अक्षर_प्रकार));
        }  

        जर(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }अन्यथा{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    जर(चळत_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    परत dest;
}
#endif

U_CAPI ब_अक्षर_प्रकार* U_EXPORT2
u_strToWCS(ब_अक्षर_प्रकार* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           स्थिरांक char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        परत nullptr;
    }
        
    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    जर(srcLength == -1){
        srcLength = u_strlen(src);
    }
    जर(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    जर(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    परत dest;

#किंवाजर defined U_WCHAR_IS_UTF32
    
    परत (ब_अक्षर_प्रकार*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#अन्यथा
    
    परत _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#जर !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
स्थिर char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             स्थिरांक ब_अक्षर_प्रकार *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_चळत [चळत_BUFFER_CAPACITY];

    ब_अक्षर_प्रकार w_चळत[चळत_BUFFER_CAPACITY];
    ब_अक्षर_प्रकार* pWStack = w_चळत;


    अक्षर c_चळत[चळत_BUFFER_CAPACITY];
    int32_t c_चळत_Cap = चळत_BUFFER_CAPACITY;
    अक्षर* pCSrc=c_चळत;
    अक्षर* pCSave=pCSrc;
    अक्षर* pCSrcLimit=nullptr;

    स्थिरांक ब_अक्षर_प्रकार* pSrc = src;
    स्थिरांक ब_अक्षर_प्रकार* pSrcLimit = nullptr;

    जर(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        साठी(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_चळत_Cap);
    
            जर(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                जा cleanup;
            }अन्यथा जर(retVal >= (c_चळत_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_चळत,(शून्य**)&pCSrc,&c_चळत_Cap,
                    c_चळत_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, आकार(अक्षर));
                pCSave = pCSrc;
            }अन्यथा{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                थांबा;
            }
        }
        
    }अन्यथा{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_चळत_Cap;
        
        pSrcLimit = src + srcLength;

        साठी(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            पर्यंत(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            जर((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                जर(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_चळत,(शून्य**)&pCSrc,&c_चळत_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_चळत_Cap+(nulLen*MB_CUR_MAX),len,आकार(अक्षर));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_चळत_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                जर(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    जा cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }अन्यथा{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                जर(nulLen >= चळत_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(ब_अक्षर_प्रकार*) uprv_malloc(आकार(ब_अक्षर_प्रकार) * (nulLen + 1));
                    जर(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        जा cleanup;
                    }
                }
                जर(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (माप_प्रकार)nulLen*आकार(ब_अक्षर_प्रकार));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                जर(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_चळत,(शून्य**)&pCSrc,&c_चळत_Cap,
                           c_चळत_Cap+(nulLen*MB_CUR_MAX),len,आकार(अक्षर));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_चळत_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                थांबा;
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
    
    जर(U_FAILURE(*pErrorCode)|| conv==nullptr){
        जा cleanup;
    }
    
    साठी(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(स्थिरांक अक्षर**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        जर(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_चळत;
            pTarget = u_चळत;
            pTargetLimit = u_चळत + चळत_BUFFER_CAPACITY;
        } अन्यथा {
            जर (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            थांबा;
        }
        
    }
    
    जर(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    जर(c_चळत != pCSave){
        uprv_free(pCSave);
    }

    जर(w_चळत != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    परत dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             स्थिरांक ब_अक्षर_प्रकार* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    जर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        परत nullptr;
    }

    जर( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    जर(srcLength == -1){
        srcLength = u_strlen((स्थिरांक char16_t *)src);
    }
    जर(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (स्थिरांक char16_t *)src, srcLength);
    }
    जर(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    परत dest;

#किंवाजर defined U_WCHAR_IS_UTF32
    
    परत u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#अन्यथा

    परत _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
