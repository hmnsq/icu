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

#समावेश "unicode/utypes.h"
#समावेश "unicode/ustring.h"
#समावेश "cstring.h"
#समावेश "cwchar.h"
#समावेश "cmemory.h"
#समावेश "ustr_imp.h"
#समावेश "ustr_cnv.h"

#अगर defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#घोषणा ढेर_बफर_CAPACITY 1000
#घोषणा बफर_CAPACITY_MULTIPLIER 2

#अगर !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
अटल अंतरभूत UBool 
u_grow_Any_बफर_From_Static(व्योम *context,
                       व्योम **p_बफर, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    अक्षर *new_बफर=(अक्षर *)uprv_malloc(reqCapacity*size);
    अगर(new_बफर!=nullptr) {
        अगर(length>0) {
            uprv_memcpy(new_बफर, *p_बफर, (माप_प्रकार)length*size);
        }
        *pCapacity=reqCapacity;
    } अन्यथा {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    अगर(*p_बफर!=(अक्षर *)context) {
        uprv_free(*p_बफर);
    }

    *p_बफर=new_बफर;
    वापस new_बफर!=nullptr;
}

/* helper function */
अटल ब_अक्षर_प्रकार* 
_strToWCS(ब_अक्षर_प्रकार *dest, 
           int32_t destCapacity,
           int32_t *p_Dest_लंबाई,
           स्थिर char16_t *src,
           int32_t src_लंबाई,
           UErrorCode *pErrorCode){

    अक्षर ढेर_बफर [ढेर_बफर_CAPACITY];
    अक्षर* अस्थायी_Buf = ढेर_बफर;
    int32_t अस्थायी_Buf_Capacity = ढेर_बफर_CAPACITY;
    अक्षर* अस्थायी_Buf_Limit = ढेर_बफर + अस्थायी_Buf_Capacity;
    UConverter* conv = nullptr;
    अक्षर* saveBuf = अस्थायी_Buf;
    ब_अक्षर_प्रकार* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    पूर्णांक count=0,retVal=0;
    
    स्थिर char16_t *pSrcLimit =nullptr;
    स्थिर char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    अगर(U_FAILURE(*pErrorCode)){
        वापस nullptr;
    }
    
    अगर(src_लंबाई == -1){
        src_लंबाई = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + src_लंबाई;

    क्रम(;;) {
        UErrorCode बफर_Status = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&अस्थायी_Buf,अस्थायी_Buf_Limit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&बफर_Status);
        count =(अस्थायी_Buf - saveBuf);
        
        /* This should rarely occur */
        अगर(बफर_Status==U_बफर_OVERFLOW_ERROR){
            अस्थायी_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * src_लंबाई;
            अगर(newCapacity <= अस्थायी_Buf_Capacity) {
                newCapacity = बफर_CAPACITY_MULTIPLIER * अस्थायी_Buf_Capacity;
            }
            अगर(!u_grow_Any_बफर_From_Static(ढेर_बफर,(व्योम**) &अस्थायी_Buf, &अस्थायी_Buf_Capacity,
                    newCapacity, count, 1)) {
                जाओ cleanup;
            }
          
           saveBuf = अस्थायी_Buf;
           अस्थायी_Buf_Limit = अस्थायी_Buf + अस्थायी_Buf_Capacity;
           अस्थायी_Buf = अस्थायी_Buf + count;
        } अन्यथा {
            अगर (U_FAILURE(बफर_Status)) {
                *pErrorCode = बफर_Status;
                जाओ cleanup;
            }
            अवरोध;
        }
    }

    /* done with conversion null terminate the char buffer */
    अगर(count>=अस्थायी_Buf_Capacity){
        अस्थायी_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        अगर(!u_grow_Any_बफर_From_Static(ढेर_बफर,(व्योम**) &अस्थायी_Buf, &अस्थायी_Buf_Capacity, 
                count+1, count, 1)) {
            जाओ cleanup;
        }              
       saveBuf = अस्थायी_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * बफर_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (ब_अक्षर_प्रकार*)uprv_malloc( intTargetCapacity * माप(ब_अक्षर_प्रकार) );

    अगर(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        ब_अक्षर_प्रकार* pIntTarget=intTarget;
        अस्थायी_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        क्रम(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(अस्थायी_Buf+nulLen),remaining);
            
            अगर(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                अवरोध;
            }अन्यथा अगर(retVal== remaining){/* should never occur */
                पूर्णांक अंक_Written = (pIntTarget-intTarget);
                u_grow_Any_बफर_From_Static(nullptr,(व्योम**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * बफर_CAPACITY_MULTIPLIER,
                                          अंक_Written,
                                          माप(ब_अक्षर_प्रकार));
                pIntTarget = intTarget;
                remaining=intTargetCapacity;

                अगर(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=अंक_Written;
                    remaining-=अंक_Written;
                }

            }अन्यथा{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                जबतक(अस्थायी_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_लंबाई) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                अगर(nulLen>=(count)){
                    अवरोध;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        अगर(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (माप_प्रकार)count*माप(ब_अक्षर_प्रकार));
        }  

        अगर(p_Dest_लंबाई){
            *p_Dest_लंबाई = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }अन्यथा{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    अगर(ढेर_बफर != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    वापस dest;
}
#पूर्ण_अगर

U_CAPI ब_अक्षर_प्रकार* U_EXPORT2
u_strToWCS(ब_अक्षर_प्रकार* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* p_Dest_लंबाई,
           स्थिर char16_t* src,
           int32_t src_लंबाई,
           UErrorCode* pErrorCode) {

    /* args check */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        वापस nullptr;
    }
        
    अगर( (src==nullptr && src_लंबाई!=0) || src_लंबाई < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    
#अगर_घोषित U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    अगर(src_लंबाई == -1){
        src_लंबाई = u_strlen(src);
    }
    अगर(0 < src_लंबाई && src_लंबाई <= destCapacity){
        u_memcpy((char16_t *)dest, src, src_लंबाई);
    }
    अगर(p_Dest_लंबाई){
       *p_Dest_लंबाई = src_लंबाई;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,src_लंबाई,pErrorCode);

    वापस dest;

#या_अगर defined U_WCHAR_IS_UTF32
    
    वापस (ब_अक्षर_प्रकार*)u_strToUTF32((UChar32*)dest, destCapacity, p_Dest_लंबाई,
                                  src, src_लंबाई, pErrorCode);

#अन्यथा
    
    वापस _strToWCS(dest,destCapacity,p_Dest_लंबाई,src,src_लंबाई, pErrorCode);
    
#पूर्ण_अगर

}

#अगर !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
अटल char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *p_Dest_लंबाई,
             स्थिर ब_अक्षर_प्रकार *src,
             int32_t src_लंबाई,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_ढेर [ढेर_बफर_CAPACITY];

    ब_अक्षर_प्रकार w_ढेर[ढेर_बफर_CAPACITY];
    ब_अक्षर_प्रकार* pWStack = w_ढेर;


    अक्षर c_ढेर[ढेर_बफर_CAPACITY];
    int32_t c_ढेर_Cap = ढेर_बफर_CAPACITY;
    अक्षर* pCSrc=c_ढेर;
    अक्षर* pCSave=pCSrc;
    अक्षर* pCSrcLimit=nullptr;

    स्थिर ब_अक्षर_प्रकार* pSrc = src;
    स्थिर ब_अक्षर_प्रकार* pSrcLimit = nullptr;

    अगर(src_लंबाई ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        क्रम(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ढेर_Cap);
    
            अगर(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                जाओ cleanup;
            }अन्यथा अगर(retVal >= (c_ढेर_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_बफर_From_Static(c_ढेर,(व्योम**)&pCSrc,&c_ढेर_Cap,
                    c_ढेर_Cap * बफर_CAPACITY_MULTIPLIER, 0, माप(अक्षर));
                pCSave = pCSrc;
            }अन्यथा{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                अवरोध;
            }
        }
        
    }अन्यथा{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_ढेर_Cap;
        
        pSrcLimit = src + src_लंबाई;

        क्रम(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            जबतक(nulLen<src_लंबाई && pSrc[nulLen++]!=0){
            }

            अगर((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                अगर(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_बफर_From_Static(c_ढेर,(व्योम**)&pCSrc,&c_ढेर_Cap,
                           बफर_CAPACITY_MULTIPLIER*c_ढेर_Cap+(nulLen*MB_CUR_MAX),len,माप(अक्षर));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ढेर_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                अगर(retVal==-1){
                    /* an error occurred bail out */
                    *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                    जाओ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_लंबाई-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }अन्यथा{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                अगर(nulLen >= ढेर_बफर_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(ब_अक्षर_प्रकार*) uprv_malloc(माप(ब_अक्षर_प्रकार) * (nulLen + 1));
                    अगर(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        जाओ cleanup;
                    }
                }
                अगर(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (माप_प्रकार)nulLen*माप(ब_अक्षर_प्रकार));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                अगर(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_बफर_From_Static(c_ढेर,(व्योम**)&pCSrc,&c_ढेर_Cap,
                           c_ढेर_Cap+(nulLen*MB_CUR_MAX),len,माप(अक्षर));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_ढेर_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_लंबाई-=nulLen; /* decrement the srcLength */
                अवरोध;
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
    
    अगर(U_FAILURE(*pErrorCode)|| conv==nullptr){
        जाओ cleanup;
    }
    
    क्रम(;;) {
        UErrorCode बफर_Status = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(स्थिर अक्षर**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&बफर_Status);

        /* increment count to number written to stack */
        count+= pTarget - target;

        अगर(बफर_Status==U_बफर_OVERFLOW_ERROR){
            target = u_ढेर;
            pTarget = u_ढेर;
            pTargetLimit = u_ढेर + ढेर_बफर_CAPACITY;
        } अन्यथा {
            अगर (U_FAILURE(बफर_Status)) {
                *pErrorCode = बफर_Status;
            }
            अवरोध;
        }
        
    }
    
    अगर(p_Dest_लंबाई){
        *p_Dest_लंबाई =count;
    }

    u_terminateUChars(dest,destCapacity,count,pErrorCode);
    
cleanup:
 
    अगर(c_ढेर != pCSave){
        uprv_free(pCSave);
    }

    अगर(w_ढेर != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    वापस dest;
}
#पूर्ण_अगर

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* p_Dest_लंबाई,
             स्थिर ब_अक्षर_प्रकार* src,
             int32_t src_लंबाई,
             UErrorCode* pErrorCode)
{

    /* args check */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        वापस nullptr;
    }

    अगर( (src==nullptr && src_लंबाई!=0) || src_लंबाई < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }

#अगर_घोषित U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    अगर(src_लंबाई == -1){
        src_लंबाई = u_strlen((स्थिर char16_t *)src);
    }
    अगर(0 < src_लंबाई && src_लंबाई <= destCapacity){
        u_memcpy(dest, (स्थिर char16_t *)src, src_लंबाई);
    }
    अगर(p_Dest_लंबाई){
       *p_Dest_लंबाई = src_लंबाई;
    }

    u_terminateUChars(dest,destCapacity,src_लंबाई,pErrorCode);

    वापस dest;

#या_अगर defined U_WCHAR_IS_UTF32
    
    वापस u_strFromUTF32(dest, destCapacity, p_Dest_लंबाई,
                          (UChar32*)src, src_लंबाई, pErrorCode);

#अन्यथा

    वापस _strFromWCS(dest,destCapacity,p_Dest_लंबाई,src,src_लंबाई,pErrorCode);  

#पूर्ण_अगर

}

#पूर्ण_अगर /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
