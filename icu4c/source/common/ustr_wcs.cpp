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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cwchar.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "ustr_cnv.h"

#જો defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#define ઢગલો_BUFFER_CAPACITY 1000
#define _BUFFER_CAPACITY_MULTIPLIER 2

#જો !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
સ્થિર અંતઃરેખ UBool 
u_growAnyBufferFromStatic(શૂન્ય *context,
                       શૂન્ય **pBuffer, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    અક્ષર *newBuffer=(અક્ષર *)uprv_malloc(reqCapacity*size);
    જો(newBuffer!=nullptr) {
        જો(length>0) {
            uprv_memcpy(newBuffer, *pBuffer, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } નહીંતર {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    જો(*pBuffer!=(અક્ષર *)context) {
        uprv_free(*pBuffer);
    }

    *pBuffer=newBuffer;
    પાછા newBuffer!=nullptr;
}

/* helper function */
સ્થિર wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *pDestLength,
           અચલ char16_t *src,
           int32_t srcLength,
           UErrorCode *pErrorCode){

    અક્ષર ઢગલો_Buffer [ઢગલો_BUFFER_CAPACITY];
    અક્ષર* tempBuf = ઢગલો_Buffer;
    int32_t tempBufCapacity = ઢગલો_BUFFER_CAPACITY;
    અક્ષર* tempBufLimit = ઢગલો_Buffer + tempBufCapacity;
    UConverter* conv = nullptr;
    અક્ષર* saveBuf = tempBuf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    પૂર્ણાંક count=0,retVal=0;
    
    અચલ char16_t *pSrcLimit =nullptr;
    અચલ char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    જો(U_FAILURE(*pErrorCode)){
        પાછા nullptr;
    }
    
    જો(srcLength == -1){
        srcLength = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + srcLength;

    માટે(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&tempBuf,tempBufLimit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&bufferStatus);
        count =(tempBuf - saveBuf);
        
        /* This should rarely occur */
        જો(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            tempBuf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * srcLength;
            જો(newCapacity <= tempBufCapacity) {
                newCapacity = _BUFFER_CAPACITY_MULTIPLIER * tempBufCapacity;
            }
            જો(!u_growAnyBufferFromStatic(ઢગલો_Buffer,(શૂન્ય**) &tempBuf, &tempBufCapacity,
                    newCapacity, count, 1)) {
                જાઓ cleanup;
            }
          
           saveBuf = tempBuf;
           tempBufLimit = tempBuf + tempBufCapacity;
           tempBuf = tempBuf + count;
        } નહીંતર {
            જો (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
                જાઓ cleanup;
            }
            તોડો;
        }
    }

    /* done with conversion null terminate the char buffer */
    જો(count>=tempBufCapacity){
        tempBuf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        જો(!u_growAnyBufferFromStatic(ઢગલો_Buffer,(શૂન્ય**) &tempBuf, &tempBufCapacity, 
                count+1, count, 1)) {
            જાઓ cleanup;
        }              
       saveBuf = tempBuf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * _BUFFER_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * કદ(wchar_t) );

    જો(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        tempBuf = saveBuf;
        
        /* now convert the mbs to wcs */
        માટે(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(tempBuf+nulLen),remaining);
            
            જો(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                તોડો;
            }નહીંતર જો(retVal== remaining){/* should never occur */
                પૂર્ણાંક અંક_Written = (pIntTarget-intTarget);
                u_growAnyBufferFromStatic(nullptr,(શૂન્ય**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * _BUFFER_CAPACITY_MULTIPLIER,
                                          અંક_Written,
                                          કદ(wchar_t));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                જો(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=અંક_Written;
                    remaining-=અંક_Written;
                }

            }નહીંતર{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                જ્યારે(tempBuf[nulLen++] != 0){
                }
                nulVal = (nulLen < srcLength) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                જો(nulLen>=(count)){
                    તોડો;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        જો(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*કદ(wchar_t));
        }  

        જો(pDestLength){
            *pDestLength = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }નહીંતર{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    જો(ઢગલો_Buffer != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    પાછા dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* pDestLength,
           અચલ char16_t* src,
           int32_t srcLength,
           UErrorCode* pErrorCode) {

    /* args check */
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        પાછા nullptr;
    }
        
    જો( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    જો(srcLength == -1){
        srcLength = u_strlen(src);
    }
    જો(0 < srcLength && srcLength <= destCapacity){
        u_memcpy((char16_t *)dest, src, srcLength);
    }
    જો(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,srcLength,pErrorCode);

    પાછા dest;

#અથવાજો defined U_WCHAR_IS_UTF32
    
    પાછા (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, pDestLength,
                                  src, srcLength, pErrorCode);

#નહીંતર
    
    પાછા _strToWCS(dest,destCapacity,pDestLength,src,srcLength, pErrorCode);
    
#endif

}

#જો !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
સ્થિર char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *pDestLength,
             અચલ wchar_t *src,
             int32_t srcLength,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_ઢગલો [ઢગલો_BUFFER_CAPACITY];

    wchar_t w_ઢગલો[ઢગલો_BUFFER_CAPACITY];
    wchar_t* pWStack = w_ઢગલો;


    અક્ષર c_ઢગલો[ઢગલો_BUFFER_CAPACITY];
    int32_t c_ઢગલો_Cap = ઢગલો_BUFFER_CAPACITY;
    અક્ષર* pCSrc=c_ઢગલો;
    અક્ષર* pCSave=pCSrc;
    અક્ષર* pCSrcLimit=nullptr;

    અચલ wchar_t* pSrc = src;
    અચલ wchar_t* pSrcLimit = nullptr;

    જો(srcLength ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        માટે(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ઢગલો_Cap);
    
            જો(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                જાઓ cleanup;
            }નહીંતર જો(retVal >= (c_ઢગલો_Cap-1)){
                /* Should rarely occur */
                u_growAnyBufferFromStatic(c_ઢગલો,(શૂન્ય**)&pCSrc,&c_ઢગલો_Cap,
                    c_ઢગલો_Cap * _BUFFER_CAPACITY_MULTIPLIER, 0, કદ(અક્ષર));
                pCSave = pCSrc;
            }નહીંતર{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                તોડો;
            }
        }
        
    }નહીંતર{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_ઢગલો_Cap;
        
        pSrcLimit = src + srcLength;

        માટે(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            જ્યારે(nulLen<srcLength && pSrc[nulLen++]!=0){
            }

            જો((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                જો(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ઢગલો,(શૂન્ય**)&pCSrc,&c_ઢગલો_Cap,
                           _BUFFER_CAPACITY_MULTIPLIER*c_ઢગલો_Cap+(nulLen*MB_CUR_MAX),len,કદ(અક્ષર));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ઢગલો_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                જો(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    જાઓ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                srcLength-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }નહીંતર{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                જો(nulLen >= ઢગલો_BUFFER_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(કદ(wchar_t) * (nulLen + 1));
                    જો(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        જાઓ cleanup;
                    }
                }
                જો(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*કદ(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                જો(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_growAnyBufferFromStatic(c_ઢગલો,(શૂન્ય**)&pCSrc,&c_ઢગલો_Cap,
                           c_ઢગલો_Cap+(nulLen*MB_CUR_MAX),len,કદ(અક્ષર));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ઢગલો_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                srcLength-=nulLen; /* decrement the srcLength */
                તોડો;
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
    
    જો(U_FAILURE(*pErrorCode)|| conv==nullptr){
        જાઓ cleanup;
    }
    
    માટે(;;) {
        UErrorCode bufferStatus = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(અચલ અક્ષર**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&bufferStatus);

        /* increment count to number written to stack */
        count+= pTarget - target;

        જો(bufferStatus==U_BUFFER_OVERFLOW_ERROR){
            target = u_ઢગલો;
            pTarget = u_ઢગલો;
            pTargetLimit = u_ઢગલો + ઢગલો_BUFFER_CAPACITY;
        } નહીંતર {
            જો (U_FAILURE(bufferStatus)) {
                *pErrorCode = bufferStatus;
            }
            તોડો;
        }
        
    }
    
    જો(pDestLength){
        *pDestLength =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    જો(c_ઢગલો != pCSave){
        uprv_free(pCSave);
    }

    જો(w_ઢગલો != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    પાછા dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* pDestLength,
             અચલ wchar_t* src,
             int32_t srcLength,
             UErrorCode* pErrorCode)
{

    /* args check */
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        પાછા nullptr;
    }

    જો( (src==nullptr && srcLength!=0) || srcLength < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    જો(srcLength == -1){
        srcLength = u_strlen((અચલ char16_t *)src);
    }
    જો(0 < srcLength && srcLength <= destCapacity){
        u_memcpy(dest, (અચલ char16_t *)src, srcLength);
    }
    જો(pDestLength){
       *pDestLength = srcLength;
    }

    u_terminateUChars(dest,destCapacity,srcLength,pErrorCode);

    પાછા dest;

#અથવાજો defined U_WCHAR_IS_UTF32
    
    પાછા u_strFromUTF32(dest, destCapacity, pDestLength,
                          (UChar32*)src, srcLength, pErrorCode);

#નહીંતર

    પાછા _strFromWCS(dest,destCapacity,pDestLength,src,srcLength,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
