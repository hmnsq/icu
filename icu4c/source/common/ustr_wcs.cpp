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

#وصف_ڪريو ڍير_بفر_صلاحيت 1000
#وصف_ڪريو بفر_صلاحيت_MULTIPLIER 2

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_grow_Any_بفر_From_Static(فراغ *context,
                       فراغ **p_بفر, int32_t *p_صلاحيت, int32_t req_صلاحيت,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *new_بفر=(حرف *)uprv_malloc(req_صلاحيت*size);
    إذا(new_بفر!=خالي_اشارو) {
        إذا(length>0) {
            uprv_memcpy(new_بفر, *p_بفر, (size_t)length*size);
        }
        *p_صلاحيت=req_صلاحيت;
    } وإلا {
        *p_صلاحيت=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    إذا(*p_بفر!=(حرف *)context) {
        uprv_free(*p_بفر);
    }

    *p_بفر=new_بفر;
    أرجع new_بفر!=خالي_اشارو;
}

/* helper function */
ساكن wchar_t* 
str_منزل_WCS(wchar_t *dest, 
           int32_t dest_صلاحيت,
           int32_t *p_Dest_ڊيگهائي,
           ثابت char16_t *src,
           int32_t src_ڊيگهائي,
           UError_ڪوڊ *p_Error_ڪوڊ){

    حرف ڍير_بفر [ڍير_بفر_صلاحيت];
    حرف* عارضي_Buf = ڍير_بفر;
    int32_t عارضي_Buf_صلاحيت = ڍير_بفر_صلاحيت;
    حرف* عارضي_Buf_حد_بندي = ڍير_بفر + عارضي_Buf_صلاحيت;
    UConverter* conv = خالي_اشارو;
    حرف* saveBuf = عارضي_Buf;
    wchar_t* int_هدف=خالي_اشارو;
    int32_t int_هدف_صلاحيت=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *p_Src_حد_بندي =خالي_اشارو;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_ڪوڊ);
    
    إذا(U_FAILURE(*p_Error_ڪوڊ)){
        أرجع خالي_اشارو;
    }
    
    إذا(src_ڊيگهائي == -1){
        src_ڊيگهائي = u_strlen(pSrc);
    }
    
    p_Src_حد_بندي = pSrc + src_ڊيگهائي;

    لكل(;;) {
        UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&عارضي_Buf,عارضي_Buf_حد_بندي,&pSrc,p_Src_حد_بندي,خالي_اشارو,(UBool)(pSrc==p_Src_حد_بندي),&بفر_حالت);
        count =(عارضي_Buf - saveBuf);
        
        /* This should rarely occur */
        إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR){
            عارضي_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_صلاحيت = 2 * src_ڊيگهائي;
            إذا(new_صلاحيت <= عارضي_Buf_صلاحيت) {
                new_صلاحيت = بفر_صلاحيت_MULTIPLIER * عارضي_Buf_صلاحيت;
            }
            إذا(!u_grow_Any_بفر_From_Static(ڍير_بفر,(فراغ**) &عارضي_Buf, &عارضي_Buf_صلاحيت,
                    new_صلاحيت, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = عارضي_Buf;
           عارضي_Buf_حد_بندي = عارضي_Buf + عارضي_Buf_صلاحيت;
           عارضي_Buf = عارضي_Buf + count;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_ڪوڊ = بفر_حالت;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    إذا(count>=عارضي_Buf_صلاحيت){
        عارضي_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        إذا(!u_grow_Any_بفر_From_Static(ڍير_بفر,(فراغ**) &عارضي_Buf, &عارضي_Buf_صلاحيت, 
                count+1, count, 1)) {
            اذهب cleanup;
        }              
       saveBuf = عارضي_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_هدف_صلاحيت =  (count * بفر_صلاحيت_MULTIPLIER + 1) /*for null termination */;
    int_هدف = (wchar_t*)uprv_malloc( int_هدف_صلاحيت * حجم(wchar_t) );

    إذا(int_هدف){

        int32_t nulLen = 0;
        int32_t بچيل = int_هدف_صلاحيت;
        wchar_t* p_Int_هدف=int_هدف;
        عارضي_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        لكل(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_هدف,(عارضي_Buf+nulLen),بچيل);
            
            إذا(retVal==-1){
                *p_Error_ڪوڊ = U_INVALID_CHAR_مليو;
                اكسر;
            }وإلا إذا(retVal== بچيل){/* should never occur */
                صحيح انگ_Written = (p_Int_هدف-int_هدف);
                u_grow_Any_بفر_From_Static(خالي_اشارو,(فراغ**) &int_هدف,
                                          &int_هدف_صلاحيت,
                                          int_هدف_صلاحيت * بفر_صلاحيت_MULTIPLIER,
                                          انگ_Written,
                                          حجم(wchar_t));
                p_Int_هدف = int_هدف;
                بچيل=int_هدف_صلاحيت;

                إذا(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_هدف+=انگ_Written;
                    بچيل-=انگ_Written;
                }

            }وإلا{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                طالما(عارضي_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_ڊيگهائي) ? 1 : 0; 
                p_Int_هدف = p_Int_هدف + retVal+nulVal;
                بچيل -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                إذا(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(p_Int_هدف-int_هدف);
       
        إذا(0 < count && count <= dest_صلاحيت){
            uprv_memcpy(dest, int_هدف, (size_t)count*حجم(wchar_t));
        }  

        إذا(p_Dest_ڊيگهائي){
            *p_Dest_ڊيگهائي = count;
        }

        /* free the allocated memory */
        uprv_free(int_هدف);

    }وإلا{
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    إذا(ڍير_بفر != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_صلاحيت,count,p_Error_ڪوڊ);

    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#جيڪڏهن_پورو

U_CAPI wchar_t* U_EXPORT2
u_str_منزل_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_صلاحيت,
           int32_t* p_Dest_ڊيگهائي,
           ثابت char16_t* src,
           int32_t src_ڊيگهائي,
           UError_ڪوڊ* p_Error_ڪوڊ) {

    /* args check */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)){
        أرجع خالي_اشارو;
    }
        
    إذا( (src==خالي_اشارو && src_ڊيگهائي!=0) || src_ڊيگهائي < -1 ||
        (dest_صلاحيت<0) || (dest == خالي_اشارو && dest_صلاحيت > 0)
    ) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }
    
#جيڪڏهن_وصف_ٿيل U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_ڊيگهائي == -1){
        src_ڊيگهائي = u_strlen(src);
    }
    إذا(0 < src_ڊيگهائي && src_ڊيگهائي <= dest_صلاحيت){
        u_memcpy((char16_t *)dest, src, src_ڊيگهائي);
    }
    إذا(p_Dest_ڊيگهائي){
       *p_Dest_ڊيگهائي = src_ڊيگهائي;
    }

    u_terminateUChars((char16_t *)dest,dest_صلاحيت,src_ڊيگهائي,p_Error_ڪوڊ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع (wchar_t*)u_str_منزل_UTF32((UChar32*)dest, dest_صلاحيت, p_Dest_ڊيگهائي,
                                  src, src_ڊيگهائي, p_Error_ڪوڊ);

#وإلا
    
    أرجع str_منزل_WCS(dest,dest_صلاحيت,p_Dest_ڊيگهائي,src,src_ڊيگهائي, p_Error_ڪوڊ);
    
#جيڪڏهن_پورو

}

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساكن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_صلاحيت, 
             int32_t *p_Dest_ڊيگهائي,
             ثابت wchar_t *src,
             int32_t src_ڊيگهائي,
             UError_ڪوڊ *p_Error_ڪوڊ)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = خالي_اشارو;
    char16_t* p_هدف = خالي_اشارو;
    char16_t* p_هدف_حد_بندي = خالي_اشارو;
    char16_t* هدف = خالي_اشارو;
    
    char16_t u_ڍير [ڍير_بفر_صلاحيت];

    wchar_t w_ڍير[ڍير_بفر_صلاحيت];
    wchar_t* pWStack = w_ڍير;


    حرف c_ڍير[ڍير_بفر_صلاحيت];
    int32_t c_ڍير_Cap = ڍير_بفر_صلاحيت;
    حرف* pCSrc=c_ڍير;
    حرف* pCSave=pCSrc;
    حرف* p_CSrc_حد_بندي=خالي_اشارو;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* p_Src_حد_بندي = خالي_اشارو;

    إذا(src_ڊيگهائي ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ڍير_Cap);
    
            إذا(retVal == -1){
                *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
                اذهب cleanup;
            }وإلا إذا(retVal >= (c_ڍير_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_بفر_From_Static(c_ڍير,(فراغ**)&pCSrc,&c_ڍير_Cap,
                    c_ڍير_Cap * بفر_صلاحيت_MULTIPLIER, 0, حجم(حرف));
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
        int32_t بچيل =c_ڍير_Cap;
        
        p_Src_حد_بندي = src + src_ڊيگهائي;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            طالما(nulLen<src_ڊيگهائي && pSrc[nulLen++]!=0){
            }

            إذا((pSrc+nulLen) < p_Src_حد_بندي){
                /* check if we have enough room in pCSrc */
                إذا(بچيل < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڍير,(فراغ**)&pCSrc,&c_ڍير_Cap,
                           بفر_صلاحيت_MULTIPLIER*c_ڍير_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بچيل = c_ڍير_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,بچيل);

                إذا(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_ڊيگهائي-=nulLen; /* decrement the srcLength */
                بچيل -= (pCSrc-pCSave);


            }وإلا{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                إذا(nulLen >= ڍير_بفر_صلاحيت){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    إذا(pWStack==خالي_اشارو){
                        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                        اذهب cleanup;
                    }
                }
                إذا(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                إذا(بچيل < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڍير,(فراغ**)&pCSrc,&c_ڍير_Cap,
                           c_ڍير_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بچيل = c_ڍير_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,بچيل);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_ڊيگهائي-=nulLen; /* decrement the srcLength */
                اكسر;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_حد_بندي = pCSrc;
    pCSrc = pCSave;
    p_هدف = هدف= dest;
    p_هدف_حد_بندي = dest + dest_صلاحيت;    
    
    conv= u_getDefaultConverter(p_Error_ڪوڊ);
    
    إذا(U_FAILURE(*p_Error_ڪوڊ)|| conv==خالي_اشارو){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UError_ڪوڊ بفر_حالت = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_منزل_Unicode(conv,&p_هدف,p_هدف_حد_بندي,(ثابت حرف**)&pCSrc,p_CSrc_حد_بندي,خالي_اشارو,(UBool)(pCSrc==p_CSrc_حد_بندي),&بفر_حالت);

        /* increment count to number written to stack */
        count+= p_هدف - هدف;

        إذا(بفر_حالت==U_بفر_حد_کان_مٿي_ERROR){
            هدف = u_ڍير;
            p_هدف = u_ڍير;
            p_هدف_حد_بندي = u_ڍير + ڍير_بفر_صلاحيت;
        } وإلا {
            إذا (U_FAILURE(بفر_حالت)) {
                *p_Error_ڪوڊ = بفر_حالت;
            }
            اكسر;
        }
        
    }
    
    إذا(p_Dest_ڊيگهائي){
        *p_Dest_ڊيگهائي =count;
    }

    u_terminateUChars(dest,dest_صلاحيت,count,p_Error_ڪوڊ);
    
cleanup:
 
    إذا(c_ڍير != pCSave){
        uprv_free(pCSave);
    }

    إذا(w_ڍير != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#جيڪڏهن_پورو

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_صلاحيت,
             int32_t* p_Dest_ڊيگهائي,
             ثابت wchar_t* src,
             int32_t src_ڊيگهائي,
             UError_ڪوڊ* p_Error_ڪوڊ)
{

    /* args check */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)){
        أرجع خالي_اشارو;
    }

    إذا( (src==خالي_اشارو && src_ڊيگهائي!=0) || src_ڊيگهائي < -1 ||
        (dest_صلاحيت<0) || (dest == خالي_اشارو && dest_صلاحيت > 0)
    ) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع خالي_اشارو;
    }

#جيڪڏهن_وصف_ٿيل U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_ڊيگهائي == -1){
        src_ڊيگهائي = u_strlen((ثابت char16_t *)src);
    }
    إذا(0 < src_ڊيگهائي && src_ڊيگهائي <= dest_صلاحيت){
        u_memcpy(dest, (ثابت char16_t *)src, src_ڊيگهائي);
    }
    إذا(p_Dest_ڊيگهائي){
       *p_Dest_ڊيگهائي = src_ڊيگهائي;
    }

    u_terminateUChars(dest,dest_صلاحيت,src_ڊيگهائي,p_Error_ڪوڊ);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع u_strFromUTF32(dest, dest_صلاحيت, p_Dest_ڊيگهائي,
                          (UChar32*)src, src_ڊيگهائي, p_Error_ڪوڊ);

#وإلا

    أرجع _strFromWCS(dest,dest_صلاحيت,p_Dest_ڊيگهائي,src,src_ڊيگهائي,p_Error_ڪوڊ);  

#جيڪڏهن_پورو

}

#جيڪڏهن_پورو /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
