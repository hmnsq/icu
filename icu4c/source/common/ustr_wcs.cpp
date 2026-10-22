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

#define স্তূপ_বাফার_CAPACITY 1000
#define বাফার_CAPACITY_MULTIPLIER 2

#যদি !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
স্থির অন্তঃরেখ UBool 
u_grow_Any_বাফার_From_Static(শূন্য *context,
                       শূন্য **p_বাফার, int32_t *pCapacity, int32_t reqCapacity,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    অক্ষর *new_বাফার=(অক্ষর *)uprv_malloc(reqCapacity*size);
    যদি(new_বাফার!=nullptr) {
        যদি(length>0) {
            uprv_memcpy(new_বাফার, *p_বাফার, (size_t)length*size);
        }
        *pCapacity=reqCapacity;
    } অন্যথা {
        *pCapacity=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    যদি(*p_বাফার!=(অক্ষর *)context) {
        uprv_free(*p_বাফার);
    }

    *p_বাফার=new_বাফার;
    ফেরত new_বাফার!=nullptr;
}

/* helper function */
স্থির wchar_t* 
_strToWCS(wchar_t *dest, 
           int32_t destCapacity,
           int32_t *p_Dest_দৈর্ঘ্য,
           ধ্রুবক char16_t *src,
           int32_t src_দৈর্ঘ্য,
           UErrorCode *pErrorCode){

    অক্ষর স্তূপ_বাফার [স্তূপ_বাফার_CAPACITY];
    অক্ষর* অস্থায়ী_Buf = স্তূপ_বাফার;
    int32_t অস্থায়ী_Buf_Capacity = স্তূপ_বাফার_CAPACITY;
    অক্ষর* অস্থায়ী_Buf_Limit = স্তূপ_বাফার + অস্থায়ী_Buf_Capacity;
    UConverter* conv = nullptr;
    অক্ষর* saveBuf = অস্থায়ী_Buf;
    wchar_t* intTarget=nullptr;
    int32_t intTargetCapacity=0;
    পূর্ণসংখ্যা count=0,retVal=0;
    
    ধ্রুবক char16_t *pSrcLimit =nullptr;
    ধ্রুবক char16_t *pSrc = src;

    conv = u_getDefaultConverter(pErrorCode);
    
    যদি(U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
    
    যদি(src_দৈর্ঘ্য == -1){
        src_দৈর্ঘ্য = u_strlen(pSrc);
    }
    
    pSrcLimit = pSrc + src_দৈর্ঘ্য;

    জন্য(;;) {
        UErrorCode বাফার_Status = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&অস্থায়ী_Buf,অস্থায়ী_Buf_Limit,&pSrc,pSrcLimit,nullptr,(UBool)(pSrc==pSrcLimit),&বাফার_Status);
        count =(অস্থায়ী_Buf - saveBuf);
        
        /* This should rarely occur */
        যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR){
            অস্থায়ী_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t newCapacity = 2 * src_দৈর্ঘ্য;
            যদি(newCapacity <= অস্থায়ী_Buf_Capacity) {
                newCapacity = বাফার_CAPACITY_MULTIPLIER * অস্থায়ী_Buf_Capacity;
            }
            যদি(!u_grow_Any_বাফার_From_Static(স্তূপ_বাফার,(শূন্য**) &অস্থায়ী_Buf, &অস্থায়ী_Buf_Capacity,
                    newCapacity, count, 1)) {
                যাও cleanup;
            }
          
           saveBuf = অস্থায়ী_Buf;
           অস্থায়ী_Buf_Limit = অস্থায়ী_Buf + অস্থায়ী_Buf_Capacity;
           অস্থায়ী_Buf = অস্থায়ী_Buf + count;
        } অন্যথা {
            যদি (U_FAILURE(বাফার_Status)) {
                *pErrorCode = বাফার_Status;
                যাও cleanup;
            }
            বিরতি;
        }
    }

    /* done with conversion null terminate the char buffer */
    যদি(count>=অস্থায়ী_Buf_Capacity){
        অস্থায়ী_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        যদি(!u_grow_Any_বাফার_From_Static(স্তূপ_বাফার,(শূন্য**) &অস্থায়ী_Buf, &অস্থায়ী_Buf_Capacity, 
                count+1, count, 1)) {
            যাও cleanup;
        }              
       saveBuf = অস্থায়ী_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    intTargetCapacity =  (count * বাফার_CAPACITY_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( intTargetCapacity * আকার(wchar_t) );

    যদি(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = intTargetCapacity;
        wchar_t* pIntTarget=intTarget;
        অস্থায়ী_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        জন্য(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(অস্থায়ী_Buf+nulLen),remaining);
            
            যদি(retVal==-1){
                *pErrorCode = U_INVALID_CHAR_FOUND;
                বিরতি;
            }অন্যথা যদি(retVal== remaining){/* should never occur */
                পূর্ণসংখ্যা অঙ্ক_Written = (pIntTarget-intTarget);
                u_grow_Any_বাফার_From_Static(nullptr,(শূন্য**) &intTarget,
                                          &intTargetCapacity,
                                          intTargetCapacity * বাফার_CAPACITY_MULTIPLIER,
                                          অঙ্ক_Written,
                                          আকার(wchar_t));
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
                যখন(অস্থায়ী_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_দৈর্ঘ্য) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                যদি(nulLen>=(count)){
                    বিরতি;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        যদি(0 < count && count <= destCapacity){
            uprv_memcpy(dest, intTarget, (size_t)count*আকার(wchar_t));
        }  

        যদি(p_Dest_দৈর্ঘ্য){
            *p_Dest_দৈর্ঘ্য = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }অন্যথা{
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    যদি(স্তূপ_বাফার != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,destCapacity,count,pErrorCode);

    u_releaseDefaultConverter(conv);

    ফেরত dest;
}
#endif

U_CAPI wchar_t* U_EXPORT2
u_strToWCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t destCapacity,
           int32_t* p_Dest_দৈর্ঘ্য,
           ধ্রুবক char16_t* src,
           int32_t src_দৈর্ঘ্য,
           UErrorCode* pErrorCode) {

    /* args check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }
        
    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    
#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    যদি(src_দৈর্ঘ্য == -1){
        src_দৈর্ঘ্য = u_strlen(src);
    }
    যদি(0 < src_দৈর্ঘ্য && src_দৈর্ঘ্য <= destCapacity){
        u_memcpy((char16_t *)dest, src, src_দৈর্ঘ্য);
    }
    যদি(p_Dest_দৈর্ঘ্য){
       *p_Dest_দৈর্ঘ্য = src_দৈর্ঘ্য;
    }

    u_terminateUChars((char16_t *)dest,destCapacity,src_দৈর্ঘ্য,pErrorCode);

    ফেরত dest;

#নতুবা_যদি defined U_WCHAR_IS_UTF32
    
    ফেরত (wchar_t*)u_strToUTF32((UChar32*)dest, destCapacity, p_Dest_দৈর্ঘ্য,
                                  src, src_দৈর্ঘ্য, pErrorCode);

#অন্যথা
    
    ফেরত _strToWCS(dest,destCapacity,p_Dest_দৈর্ঘ্য,src,src_দৈর্ঘ্য, pErrorCode);
    
#endif

}

#যদি !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
স্থির char16_t*
_strFromWCS( char16_t   *dest,
             int32_t destCapacity, 
             int32_t *p_Dest_দৈর্ঘ্য,
             ধ্রুবক wchar_t *src,
             int32_t src_দৈর্ঘ্য,
             UErrorCode *pErrorCode)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = nullptr;
    char16_t* pTarget = nullptr;
    char16_t* pTargetLimit = nullptr;
    char16_t* target = nullptr;
    
    char16_t u_স্তূপ [স্তূপ_বাফার_CAPACITY];

    wchar_t w_স্তূপ[স্তূপ_বাফার_CAPACITY];
    wchar_t* pWStack = w_স্তূপ;


    অক্ষর c_স্তূপ[স্তূপ_বাফার_CAPACITY];
    int32_t c_স্তূপ_Cap = স্তূপ_বাফার_CAPACITY;
    অক্ষর* pCSrc=c_স্তূপ;
    অক্ষর* pCSave=pCSrc;
    অক্ষর* pCSrcLimit=nullptr;

    ধ্রুবক wchar_t* pSrc = src;
    ধ্রুবক wchar_t* pSrcLimit = nullptr;

    যদি(src_দৈর্ঘ্য ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        জন্য(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_স্তূপ_Cap);
    
            যদি(retVal == -1){
                *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                যাও cleanup;
            }অন্যথা যদি(retVal >= (c_স্তূপ_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_বাফার_From_Static(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                    c_স্তূপ_Cap * বাফার_CAPACITY_MULTIPLIER, 0, আকার(অক্ষর));
                pCSave = pCSrc;
            }অন্যথা{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                বিরতি;
            }
        }
        
    }অন্যথা{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_স্তূপ_Cap;
        
        pSrcLimit = src + src_দৈর্ঘ্য;

        জন্য(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            যখন(nulLen<src_দৈর্ঘ্য && pSrc[nulLen++]!=0){
            }

            যদি((pSrc+nulLen) < pSrcLimit){
                /* check if we have enough room in pCSrc */
                যদি(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_বাফার_From_Static(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                           বাফার_CAPACITY_MULTIPLIER*c_স্তূপ_Cap+(nulLen*MB_CUR_MAX),len,আকার(অক্ষর));

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
                    যাও cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_দৈর্ঘ্য-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }অন্যথা{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                যদি(nulLen >= স্তূপ_বাফার_CAPACITY){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(আকার(wchar_t) * (nulLen + 1));
                    যদি(pWStack==nullptr){
                        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
                        যাও cleanup;
                    }
                }
                যদি(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*আকার(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                যদি(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_বাফার_From_Static(c_স্তূপ,(শূন্য**)&pCSrc,&c_স্তূপ_Cap,
                           c_স্তূপ_Cap+(nulLen*MB_CUR_MAX),len,আকার(অক্ষর));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_স্তূপ_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_দৈর্ঘ্য-=nulLen; /* decrement the srcLength */
                বিরতি;
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
        যাও cleanup;
    }
    
    জন্য(;;) {
        UErrorCode বাফার_Status = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_toUnicode(conv,&pTarget,pTargetLimit,(ধ্রুবক অক্ষর**)&pCSrc,pCSrcLimit,nullptr,(UBool)(pCSrc==pCSrcLimit),&বাফার_Status);

        /* increment count to number written to stack */
        count+= pTarget - target;

        যদি(বাফার_Status==U_বাফার_OVERFLOW_ERROR){
            target = u_স্তূপ;
            pTarget = u_স্তূপ;
            pTargetLimit = u_স্তূপ + স্তূপ_বাফার_CAPACITY;
        } অন্যথা {
            যদি (U_FAILURE(বাফার_Status)) {
                *pErrorCode = বাফার_Status;
            }
            বিরতি;
        }
        
    }
    
    যদি(p_Dest_দৈর্ঘ্য){
        *p_Dest_দৈর্ঘ্য =count;
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

    ফেরত dest;
}
#endif

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t destCapacity,
             int32_t* p_Dest_দৈর্ঘ্য,
             ধ্রুবক wchar_t* src,
             int32_t src_দৈর্ঘ্য,
             UErrorCode* pErrorCode)
{

    /* args check */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)){
        ফেরত nullptr;
    }

    যদি( (src==nullptr && src_দৈর্ঘ্য!=0) || src_দৈর্ঘ্য < -1 ||
        (destCapacity<0) || (dest == nullptr && destCapacity > 0)
    ) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }

#ifdef U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    যদি(src_দৈর্ঘ্য == -1){
        src_দৈর্ঘ্য = u_strlen((ধ্রুবক char16_t *)src);
    }
    যদি(0 < src_দৈর্ঘ্য && src_দৈর্ঘ্য <= destCapacity){
        u_memcpy(dest, (ধ্রুবক char16_t *)src, src_দৈর্ঘ্য);
    }
    যদি(p_Dest_দৈর্ঘ্য){
       *p_Dest_দৈর্ঘ্য = src_দৈর্ঘ্য;
    }

    u_terminateUChars(dest,destCapacity,src_দৈর্ঘ্য,pErrorCode);

    ফেরত dest;

#নতুবা_যদি defined U_WCHAR_IS_UTF32
    
    ফেরত u_strFromUTF32(dest, destCapacity, p_Dest_দৈর্ঘ্য,
                          (UChar32*)src, src_দৈর্ঘ্য, pErrorCode);

#অন্যথা

    ফেরত _strFromWCS(dest,destCapacity,p_Dest_দৈর্ঘ্য,src,src_দৈর্ঘ্য,pErrorCode);  

#endif

}

#endif /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
