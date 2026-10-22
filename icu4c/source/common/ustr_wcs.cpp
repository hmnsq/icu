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

#ضمن "unicode/utypes.h"
#ضمن "unicode/ustring.h"
#ضمن "cstring.h"
#ضمن "cwchar.h"
#ضمن "cmemory.h"
#ضمن "ustr_imp.h"
#ضمن "ustr_cnv.h"

#إذا defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#تعریف_کړه ډېری_بفر_ظرفیت 1000
#تعریف_کړه بفر_ظرفیت_MULTIPLIER 2

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_grow_Any_بفر_From_Static(فراغ *context,
                       فراغ **p_بفر, int32_t *p_ظرفیت, int32_t req_ظرفیت,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *new_بفر=(حرف *)uprv_malloc(req_ظرفیت*size);
    إذا(new_بفر!=تش_اشاره) {
        إذا(length>0) {
            uprv_memcpy(new_بفر, *p_بفر, (size_t)length*size);
        }
        *p_ظرفیت=req_ظرفیت;
    } وإلا {
        *p_ظرفیت=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    إذا(*p_بفر!=(حرف *)context) {
        uprv_free(*p_بفر);
    }

    *p_بفر=new_بفر;
    أرجع new_بفر!=تش_اشاره;
}

/* helper function */
ساكن wchar_t* 
str_منزل_WCS(wchar_t *dest, 
           int32_t dest_ظرفیت,
           int32_t *p_Dest_اوږدتیا,
           ثابت char16_t *src,
           int32_t src_اوږدتیا,
           UError_کوډ *p_Error_کوډ){

    حرف ډېری_بفر [ډېری_بفر_ظرفیت];
    حرف* لنډمهاله_Buf = ډېری_بفر;
    int32_t لنډمهاله_Buf_ظرفیت = ډېری_بفر_ظرفیت;
    حرف* لنډمهاله_Buf_حد = ډېری_بفر + لنډمهاله_Buf_ظرفیت;
    UConverter* conv = تش_اشاره;
    حرف* saveBuf = لنډمهاله_Buf;
    wchar_t* int_موخه=تش_اشاره;
    int32_t int_موخه_ظرفیت=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *p_Src_حد =تش_اشاره;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_کوډ);
    
    إذا(U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
    
    إذا(src_اوږدتیا == -1){
        src_اوږدتیا = u_strlen(pSrc);
    }
    
    p_Src_حد = pSrc + src_اوږدتیا;

    لكل(;;) {
        UError_کوډ بفر_حالت = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&لنډمهاله_Buf,لنډمهاله_Buf_حد,&pSrc,p_Src_حد,تش_اشاره,(UBool)(pSrc==p_Src_حد),&بفر_حالت);
        count =(لنډمهاله_Buf - saveBuf);
        
        /* This should rarely occur */
        إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR){
            لنډمهاله_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_ظرفیت = 2 * src_اوږدتیا;
            إذا(new_ظرفیت <= لنډمهاله_Buf_ظرفیت) {
                new_ظرفیت = بفر_ظرفیت_MULTIPLIER * لنډمهاله_Buf_ظرفیت;
            }
            إذا(!u_grow_Any_بفر_From_Static(ډېری_بفر,(فراغ**) &لنډمهاله_Buf, &لنډمهاله_Buf_ظرفیت,
                    new_ظرفیت, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = لنډمهاله_Buf;
           لنډمهاله_Buf_حد = لنډمهاله_Buf + لنډمهاله_Buf_ظرفیت;
           لنډمهاله_Buf = لنډمهاله_Buf + count;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_کوډ = بفر_حالت;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    إذا(count>=لنډمهاله_Buf_ظرفیت){
        لنډمهاله_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        إذا(!u_grow_Any_بفر_From_Static(ډېری_بفر,(فراغ**) &لنډمهاله_Buf, &لنډمهاله_Buf_ظرفیت, 
                count+1, count, 1)) {
            اذهب cleanup;
        }              
       saveBuf = لنډمهاله_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_موخه_ظرفیت =  (count * بفر_ظرفیت_MULTIPLIER + 1) /*for null termination */;
    int_موخه = (wchar_t*)uprv_malloc( int_موخه_ظرفیت * حجم(wchar_t) );

    إذا(int_موخه){

        int32_t nulLen = 0;
        int32_t پاتې_شوی = int_موخه_ظرفیت;
        wchar_t* p_Int_موخه=int_موخه;
        لنډمهاله_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        لكل(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_موخه,(لنډمهاله_Buf+nulLen),پاتې_شوی);
            
            إذا(retVal==-1){
                *p_Error_کوډ = U_INVALID_CHAR_وموندل;
                اكسر;
            }وإلا إذا(retVal== پاتې_شوی){/* should never occur */
                صحيح عدد_Written = (p_Int_موخه-int_موخه);
                u_grow_Any_بفر_From_Static(تش_اشاره,(فراغ**) &int_موخه,
                                          &int_موخه_ظرفیت,
                                          int_موخه_ظرفیت * بفر_ظرفیت_MULTIPLIER,
                                          عدد_Written,
                                          حجم(wchar_t));
                p_Int_موخه = int_موخه;
                پاتې_شوی=int_موخه_ظرفیت;

                إذا(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_موخه+=عدد_Written;
                    پاتې_شوی-=عدد_Written;
                }

            }وإلا{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                طالما(لنډمهاله_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_اوږدتیا) ? 1 : 0; 
                p_Int_موخه = p_Int_موخه + retVal+nulVal;
                پاتې_شوی -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                إذا(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(p_Int_موخه-int_موخه);
       
        إذا(0 < count && count <= dest_ظرفیت){
            uprv_memcpy(dest, int_موخه, (size_t)count*حجم(wchar_t));
        }  

        إذا(p_Dest_اوږدتیا){
            *p_Dest_اوږدتیا = count;
        }

        /* free the allocated memory */
        uprv_free(int_موخه);

    }وإلا{
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    إذا(ډېری_بفر != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_ظرفیت,count,p_Error_کوډ);

    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#د_که_پای

U_CAPI wchar_t* U_EXPORT2
u_str_منزل_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_ظرفیت,
           int32_t* p_Dest_اوږدتیا,
           ثابت char16_t* src,
           int32_t src_اوږدتیا,
           UError_کوډ* p_Error_کوډ) {

    /* args check */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }
        
    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }
    
#که_تعریف_شوی U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_اوږدتیا == -1){
        src_اوږدتیا = u_strlen(src);
    }
    إذا(0 < src_اوږدتیا && src_اوږدتیا <= dest_ظرفیت){
        u_memcpy((char16_t *)dest, src, src_اوږدتیا);
    }
    إذا(p_Dest_اوږدتیا){
       *p_Dest_اوږدتیا = src_اوږدتیا;
    }

    u_terminateUChars((char16_t *)dest,dest_ظرفیت,src_اوږدتیا,p_Error_کوډ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع (wchar_t*)u_str_منزل_UTF32((UChar32*)dest, dest_ظرفیت, p_Dest_اوږدتیا,
                                  src, src_اوږدتیا, p_Error_کوډ);

#وإلا
    
    أرجع str_منزل_WCS(dest,dest_ظرفیت,p_Dest_اوږدتیا,src,src_اوږدتیا, p_Error_کوډ);
    
#د_که_پای

}

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساكن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_ظرفیت, 
             int32_t *p_Dest_اوږدتیا,
             ثابت wchar_t *src,
             int32_t src_اوږدتیا,
             UError_کوډ *p_Error_کوډ)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = تش_اشاره;
    char16_t* p_موخه = تش_اشاره;
    char16_t* p_موخه_حد = تش_اشاره;
    char16_t* موخه = تش_اشاره;
    
    char16_t u_ډېری [ډېری_بفر_ظرفیت];

    wchar_t w_ډېری[ډېری_بفر_ظرفیت];
    wchar_t* pWStack = w_ډېری;


    حرف c_ډېری[ډېری_بفر_ظرفیت];
    int32_t c_ډېری_Cap = ډېری_بفر_ظرفیت;
    حرف* pCSrc=c_ډېری;
    حرف* pCSave=pCSrc;
    حرف* p_CSrc_حد=تش_اشاره;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* p_Src_حد = تش_اشاره;

    إذا(src_اوږدتیا ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ډېری_Cap);
    
            إذا(retVal == -1){
                *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
                اذهب cleanup;
            }وإلا إذا(retVal >= (c_ډېری_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_بفر_From_Static(c_ډېری,(فراغ**)&pCSrc,&c_ډېری_Cap,
                    c_ډېری_Cap * بفر_ظرفیت_MULTIPLIER, 0, حجم(حرف));
                pCSave = pCSrc;
            }وإلا{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                اكسر;
            }
        }
        
    }وإلا{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t پاتې_شوی =c_ډېری_Cap;
        
        p_Src_حد = src + src_اوږدتیا;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            طالما(nulLen<src_اوږدتیا && pSrc[nulLen++]!=0){
            }

            إذا((pSrc+nulLen) < p_Src_حد){
                /* check if we have enough room in pCSrc */
                إذا(پاتې_شوی < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ډېری,(فراغ**)&pCSrc,&c_ډېری_Cap,
                           بفر_ظرفیت_MULTIPLIER*c_ډېری_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    پاتې_شوی = c_ډېری_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,پاتې_شوی);

                إذا(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_اوږدتیا-=nulLen; /* decrement the srcLength */
                پاتې_شوی -= (pCSrc-pCSave);


            }وإلا{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                إذا(nulLen >= ډېری_بفر_ظرفیت){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    إذا(pWStack==تش_اشاره){
                        *p_Error_کوډ = U_حافظه_ویش_ERROR;
                        اذهب cleanup;
                    }
                }
                إذا(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                إذا(پاتې_شوی < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ډېری,(فراغ**)&pCSrc,&c_ډېری_Cap,
                           c_ډېری_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    پاتې_شوی = c_ډېری_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,پاتې_شوی);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_اوږدتیا-=nulLen; /* decrement the srcLength */
                اكسر;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_حد = pCSrc;
    pCSrc = pCSave;
    p_موخه = موخه= dest;
    p_موخه_حد = dest + dest_ظرفیت;    
    
    conv= u_getDefaultConverter(p_Error_کوډ);
    
    إذا(U_FAILURE(*p_Error_کوډ)|| conv==تش_اشاره){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UError_کوډ بفر_حالت = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_منزل_Unicode(conv,&p_موخه,p_موخه_حد,(ثابت حرف**)&pCSrc,p_CSrc_حد,تش_اشاره,(UBool)(pCSrc==p_CSrc_حد),&بفر_حالت);

        /* increment count to number written to stack */
        count+= p_موخه - موخه;

        إذا(بفر_حالت==U_بفر_ډکېدنه_ERROR){
            موخه = u_ډېری;
            p_موخه = u_ډېری;
            p_موخه_حد = u_ډېری + ډېری_بفر_ظرفیت;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_کوډ = بفر_حالت;
            }
            اكسر;
        }
        
    }
    
    إذا(p_Dest_اوږدتیا){
        *p_Dest_اوږدتیا =count;
    }

    u_terminateUChars(dest,dest_ظرفیت,count,p_Error_کوډ);
    
cleanup:
 
    إذا(c_ډېری != pCSave){
        uprv_free(pCSave);
    }

    إذا(w_ډېری != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#د_که_پای

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_ظرفیت,
             int32_t* p_Dest_اوږدتیا,
             ثابت wchar_t* src,
             int32_t src_اوږدتیا,
             UError_کوډ* p_Error_کوډ)
{

    /* args check */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)){
        أرجع تش_اشاره;
    }

    إذا( (src==تش_اشاره && src_اوږدتیا!=0) || src_اوږدتیا < -1 ||
        (dest_ظرفیت<0) || (dest == تش_اشاره && dest_ظرفیت > 0)
    ) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع تش_اشاره;
    }

#که_تعریف_شوی U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_اوږدتیا == -1){
        src_اوږدتیا = u_strlen((ثابت char16_t *)src);
    }
    إذا(0 < src_اوږدتیا && src_اوږدتیا <= dest_ظرفیت){
        u_memcpy(dest, (ثابت char16_t *)src, src_اوږدتیا);
    }
    إذا(p_Dest_اوږدتیا){
       *p_Dest_اوږدتیا = src_اوږدتیا;
    }

    u_terminateUChars(dest,dest_ظرفیت,src_اوږدتیا,p_Error_کوډ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع u_strFromUTF32(dest, dest_ظرفیت, p_Dest_اوږدتیا,
                          (UChar32*)src, src_اوږدتیا, p_Error_کوډ);

#وإلا

    أرجع _strFromWCS(dest,dest_ظرفیت,p_Dest_اوږدتیا,src,src_اوږدتیا,p_Error_کوډ);  

#د_که_پای

}

#د_که_پای /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
