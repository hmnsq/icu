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

#تعریف_کرو ڈھیر_بفر_سمرتھا 1000
#تعریف_کرو بفر_سمرتھا_MULTIPLIER 2

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_grow_Any_بفر_From_Static(فراغ *context,
                       فراغ **p_بفر, int32_t *p_سمرتھا, int32_t req_سمرتھا,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *new_بفر=(حرف *)uprv_malloc(req_سمرتھا*size);
    إذا(new_بفر!=خالی_سوچک) {
        إذا(length>0) {
            uprv_memcpy(new_بفر, *p_بفر, (size_t)length*size);
        }
        *p_سمرتھا=req_سمرتھا;
    } وإلا {
        *p_سمرتھا=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    إذا(*p_بفر!=(حرف *)context) {
        uprv_free(*p_بفر);
    }

    *p_بفر=new_بفر;
    أرجع new_بفر!=خالی_سوچک;
}

/* helper function */
ساكن wchar_t* 
str_منزل_WCS(wchar_t *dest, 
           int32_t dest_سمرتھا,
           int32_t *p_Dest_لمائی,
           ثابت char16_t *src,
           int32_t src_لمائی,
           UError_کوڈ *p_Error_کوڈ){

    حرف ڈھیر_بفر [ڈھیر_بفر_سمرتھا];
    حرف* عارضی_Buf = ڈھیر_بفر;
    int32_t عارضی_Buf_سمرتھا = ڈھیر_بفر_سمرتھا;
    حرف* عارضی_Buf_حد_بندی = ڈھیر_بفر + عارضی_Buf_سمرتھا;
    UConverter* conv = خالی_سوچک;
    حرف* saveBuf = عارضی_Buf;
    wchar_t* int_نشانہ=خالی_سوچک;
    int32_t int_نشانہ_سمرتھا=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *p_Src_حد_بندی =خالی_سوچک;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_کوڈ);
    
    إذا(U_FAILURE(*p_Error_کوڈ)){
        أرجع خالی_سوچک;
    }
    
    إذا(src_لمائی == -1){
        src_لمائی = u_strlen(pSrc);
    }
    
    p_Src_حد_بندی = pSrc + src_لمائی;

    لكل(;;) {
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&عارضی_Buf,عارضی_Buf_حد_بندی,&pSrc,p_Src_حد_بندی,خالی_سوچک,(UBool)(pSrc==p_Src_حد_بندی),&بفر_حالت);
        count =(عارضی_Buf - saveBuf);
        
        /* This should rarely occur */
        إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR){
            عارضی_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_سمرتھا = 2 * src_لمائی;
            إذا(new_سمرتھا <= عارضی_Buf_سمرتھا) {
                new_سمرتھا = بفر_سمرتھا_MULTIPLIER * عارضی_Buf_سمرتھا;
            }
            إذا(!u_grow_Any_بفر_From_Static(ڈھیر_بفر,(فراغ**) &عارضی_Buf, &عارضی_Buf_سمرتھا,
                    new_سمرتھا, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = عارضی_Buf;
           عارضی_Buf_حد_بندی = عارضی_Buf + عارضی_Buf_سمرتھا;
           عارضی_Buf = عارضی_Buf + count;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_کوڈ = بفر_حالت;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    إذا(count>=عارضی_Buf_سمرتھا){
        عارضی_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        إذا(!u_grow_Any_بفر_From_Static(ڈھیر_بفر,(فراغ**) &عارضی_Buf, &عارضی_Buf_سمرتھا, 
                count+1, count, 1)) {
            اذهب cleanup;
        }              
       saveBuf = عارضی_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_نشانہ_سمرتھا =  (count * بفر_سمرتھا_MULTIPLIER + 1) /*for null termination */;
    int_نشانہ = (wchar_t*)uprv_malloc( int_نشانہ_سمرتھا * حجم(wchar_t) );

    إذا(int_نشانہ){

        int32_t nulLen = 0;
        int32_t بچیا = int_نشانہ_سمرتھا;
        wchar_t* p_Int_نشانہ=int_نشانہ;
        عارضی_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        لكل(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_نشانہ,(عارضی_Buf+nulLen),بچیا);
            
            إذا(retVal==-1){
                *p_Error_کوڈ = U_INVALID_CHAR_لبھیا;
                اكسر;
            }وإلا إذا(retVal== بچیا){/* should never occur */
                صحيح عدد_Written = (p_Int_نشانہ-int_نشانہ);
                u_grow_Any_بفر_From_Static(خالی_سوچک,(فراغ**) &int_نشانہ,
                                          &int_نشانہ_سمرتھا,
                                          int_نشانہ_سمرتھا * بفر_سمرتھا_MULTIPLIER,
                                          عدد_Written,
                                          حجم(wchar_t));
                p_Int_نشانہ = int_نشانہ;
                بچیا=int_نشانہ_سمرتھا;

                إذا(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_نشانہ+=عدد_Written;
                    بچیا-=عدد_Written;
                }

            }وإلا{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                طالما(عارضی_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_لمائی) ? 1 : 0; 
                p_Int_نشانہ = p_Int_نشانہ + retVal+nulVal;
                بچیا -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                إذا(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(p_Int_نشانہ-int_نشانہ);
       
        إذا(0 < count && count <= dest_سمرتھا){
            uprv_memcpy(dest, int_نشانہ, (size_t)count*حجم(wchar_t));
        }  

        إذا(p_Dest_لمائی){
            *p_Dest_لمائی = count;
        }

        /* free the allocated memory */
        uprv_free(int_نشانہ);

    }وإلا{
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    إذا(ڈھیر_بفر != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_سمرتھا,count,p_Error_کوڈ);

    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#جے_انت

U_CAPI wchar_t* U_EXPORT2
u_str_منزل_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_سمرتھا,
           int32_t* p_Dest_لمائی,
           ثابت char16_t* src,
           int32_t src_لمائی,
           UError_کوڈ* p_Error_کوڈ) {

    /* args check */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)){
        أرجع خالی_سوچک;
    }
        
    إذا( (src==خالی_سوچک && src_لمائی!=0) || src_لمائی < -1 ||
        (dest_سمرتھا<0) || (dest == خالی_سوچک && dest_سمرتھا > 0)
    ) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }
    
#جے_تعریف_شدہ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_لمائی == -1){
        src_لمائی = u_strlen(src);
    }
    إذا(0 < src_لمائی && src_لمائی <= dest_سمرتھا){
        u_memcpy((char16_t *)dest, src, src_لمائی);
    }
    إذا(p_Dest_لمائی){
       *p_Dest_لمائی = src_لمائی;
    }

    u_terminateUChars((char16_t *)dest,dest_سمرتھا,src_لمائی,p_Error_کوڈ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع (wchar_t*)u_str_منزل_UTF32((UChar32*)dest, dest_سمرتھا, p_Dest_لمائی,
                                  src, src_لمائی, p_Error_کوڈ);

#وإلا
    
    أرجع str_منزل_WCS(dest,dest_سمرتھا,p_Dest_لمائی,src,src_لمائی, p_Error_کوڈ);
    
#جے_انت

}

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساكن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_سمرتھا, 
             int32_t *p_Dest_لمائی,
             ثابت wchar_t *src,
             int32_t src_لمائی,
             UError_کوڈ *p_Error_کوڈ)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = خالی_سوچک;
    char16_t* p_نشانہ = خالی_سوچک;
    char16_t* p_نشانہ_حد_بندی = خالی_سوچک;
    char16_t* نشانہ = خالی_سوچک;
    
    char16_t u_ڈھیر [ڈھیر_بفر_سمرتھا];

    wchar_t w_ڈھیر[ڈھیر_بفر_سمرتھا];
    wchar_t* pWStack = w_ڈھیر;


    حرف c_ڈھیر[ڈھیر_بفر_سمرتھا];
    int32_t c_ڈھیر_Cap = ڈھیر_بفر_سمرتھا;
    حرف* pCSrc=c_ڈھیر;
    حرف* pCSave=pCSrc;
    حرف* p_CSrc_حد_بندی=خالی_سوچک;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* p_Src_حد_بندی = خالی_سوچک;

    إذا(src_لمائی ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ڈھیر_Cap);
    
            إذا(retVal == -1){
                *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
                اذهب cleanup;
            }وإلا إذا(retVal >= (c_ڈھیر_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_بفر_From_Static(c_ڈھیر,(فراغ**)&pCSrc,&c_ڈھیر_Cap,
                    c_ڈھیر_Cap * بفر_سمرتھا_MULTIPLIER, 0, حجم(حرف));
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
        int32_t بچیا =c_ڈھیر_Cap;
        
        p_Src_حد_بندی = src + src_لمائی;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            طالما(nulLen<src_لمائی && pSrc[nulLen++]!=0){
            }

            إذا((pSrc+nulLen) < p_Src_حد_بندی){
                /* check if we have enough room in pCSrc */
                إذا(بچیا < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڈھیر,(فراغ**)&pCSrc,&c_ڈھیر_Cap,
                           بفر_سمرتھا_MULTIPLIER*c_ڈھیر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بچیا = c_ڈھیر_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,بچیا);

                إذا(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_لمائی-=nulLen; /* decrement the srcLength */
                بچیا -= (pCSrc-pCSave);


            }وإلا{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                إذا(nulLen >= ڈھیر_بفر_سمرتھا){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    إذا(pWStack==خالی_سوچک){
                        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
                        اذهب cleanup;
                    }
                }
                إذا(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                إذا(بچیا < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڈھیر,(فراغ**)&pCSrc,&c_ڈھیر_Cap,
                           c_ڈھیر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بچیا = c_ڈھیر_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,بچیا);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_لمائی-=nulLen; /* decrement the srcLength */
                اكسر;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_حد_بندی = pCSrc;
    pCSrc = pCSave;
    p_نشانہ = نشانہ= dest;
    p_نشانہ_حد_بندی = dest + dest_سمرتھا;    
    
    conv= u_getDefaultConverter(p_Error_کوڈ);
    
    إذا(U_FAILURE(*p_Error_کوڈ)|| conv==خالی_سوچک){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_منزل_Unicode(conv,&p_نشانہ,p_نشانہ_حد_بندی,(ثابت حرف**)&pCSrc,p_CSrc_حد_بندی,خالی_سوچک,(UBool)(pCSrc==p_CSrc_حد_بندی),&بفر_حالت);

        /* increment count to number written to stack */
        count+= p_نشانہ - نشانہ;

        إذا(بفر_حالت==U_بفر_حد_توں_ودھ_ERROR){
            نشانہ = u_ڈھیر;
            p_نشانہ = u_ڈھیر;
            p_نشانہ_حد_بندی = u_ڈھیر + ڈھیر_بفر_سمرتھا;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_کوڈ = بفر_حالت;
            }
            اكسر;
        }
        
    }
    
    إذا(p_Dest_لمائی){
        *p_Dest_لمائی =count;
    }

    u_terminateUChars(dest,dest_سمرتھا,count,p_Error_کوڈ);
    
cleanup:
 
    إذا(c_ڈھیر != pCSave){
        uprv_free(pCSave);
    }

    إذا(w_ڈھیر != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#جے_انت

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_سمرتھا,
             int32_t* p_Dest_لمائی,
             ثابت wchar_t* src,
             int32_t src_لمائی,
             UError_کوڈ* p_Error_کوڈ)
{

    /* args check */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)){
        أرجع خالی_سوچک;
    }

    إذا( (src==خالی_سوچک && src_لمائی!=0) || src_لمائی < -1 ||
        (dest_سمرتھا<0) || (dest == خالی_سوچک && dest_سمرتھا > 0)
    ) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع خالی_سوچک;
    }

#جے_تعریف_شدہ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_لمائی == -1){
        src_لمائی = u_strlen((ثابت char16_t *)src);
    }
    إذا(0 < src_لمائی && src_لمائی <= dest_سمرتھا){
        u_memcpy(dest, (ثابت char16_t *)src, src_لمائی);
    }
    إذا(p_Dest_لمائی){
       *p_Dest_لمائی = src_لمائی;
    }

    u_terminateUChars(dest,dest_سمرتھا,src_لمائی,p_Error_کوڈ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع u_strFromUTF32(dest, dest_سمرتھا, p_Dest_لمائی,
                          (UChar32*)src, src_لمائی, p_Error_کوڈ);

#وإلا

    أرجع _strFromWCS(dest,dest_سمرتھا,p_Dest_لمائی,src,src_لمائی,p_Error_کوڈ);  

#جے_انت

}

#جے_انت /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
