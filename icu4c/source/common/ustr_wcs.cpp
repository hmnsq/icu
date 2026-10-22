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

#تعریف_کن پشته_میانگیر_ظرفیت 1000
#تعریف_کن میانگیر_ظرفیت_MULTIPLIER 2

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساكن مضمن UBool 
u_grow_Any_میانگیر_From_Static(فراغ *context,
                       فراغ **p_میانگیر, int32_t *p_ظرفیت, int32_t req_ظرفیت,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *new_میانگیر=(حرف *)uprv_malloc(req_ظرفیت*size);
    إذا(new_میانگیر!=اشاره_تهی) {
        إذا(length>0) {
            uprv_memcpy(new_میانگیر, *p_میانگیر, (size_t)length*size);
        }
        *p_ظرفیت=req_ظرفیت;
    } وإلا {
        *p_ظرفیت=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    إذا(*p_میانگیر!=(حرف *)context) {
        uprv_free(*p_میانگیر);
    }

    *p_میانگیر=new_میانگیر;
    أرجع new_میانگیر!=اشاره_تهی;
}

/* helper function */
ساكن wchar_t* 
str_مقصد_WCS(wchar_t *dest, 
           int32_t dest_ظرفیت,
           int32_t *p_Dest_درازی,
           ثابت char16_t *src,
           int32_t src_درازی,
           UError_کد *p_Error_کد){

    حرف پشته_میانگیر [پشته_میانگیر_ظرفیت];
    حرف* موقت_Buf = پشته_میانگیر;
    int32_t موقت_Buf_ظرفیت = پشته_میانگیر_ظرفیت;
    حرف* موقت_Buf_حد = پشته_میانگیر + موقت_Buf_ظرفیت;
    UConverter* conv = اشاره_تهی;
    حرف* saveBuf = موقت_Buf;
    wchar_t* int_هدف=اشاره_تهی;
    int32_t int_هدف_ظرفیت=0;
    صحيح count=0,retVal=0;
    
    ثابت char16_t *p_Src_حد =اشاره_تهی;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_کد);
    
    إذا(U_FAILURE(*p_Error_کد)){
        أرجع اشاره_تهی;
    }
    
    إذا(src_درازی == -1){
        src_درازی = u_strlen(pSrc);
    }
    
    p_Src_حد = pSrc + src_درازی;

    لكل(;;) {
        UError_کد میانگیر_وضعیت = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&موقت_Buf,موقت_Buf_حد,&pSrc,p_Src_حد,اشاره_تهی,(UBool)(pSrc==p_Src_حد),&میانگیر_وضعیت);
        count =(موقت_Buf - saveBuf);
        
        /* This should rarely occur */
        إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR){
            موقت_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_ظرفیت = 2 * src_درازی;
            إذا(new_ظرفیت <= موقت_Buf_ظرفیت) {
                new_ظرفیت = میانگیر_ظرفیت_MULTIPLIER * موقت_Buf_ظرفیت;
            }
            إذا(!u_grow_Any_میانگیر_From_Static(پشته_میانگیر,(فراغ**) &موقت_Buf, &موقت_Buf_ظرفیت,
                    new_ظرفیت, count, 1)) {
                اذهب cleanup;
            }
          
           saveBuf = موقت_Buf;
           موقت_Buf_حد = موقت_Buf + موقت_Buf_ظرفیت;
           موقت_Buf = موقت_Buf + count;
        } وإلا {
            إذا (U_FAILURE(میانگیر_وضعیت)) {
                *p_Error_کد = میانگیر_وضعیت;
                اذهب cleanup;
            }
            اكسر;
        }
    }

    /* done with conversion null terminate the char buffer */
    إذا(count>=موقت_Buf_ظرفیت){
        موقت_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        إذا(!u_grow_Any_میانگیر_From_Static(پشته_میانگیر,(فراغ**) &موقت_Buf, &موقت_Buf_ظرفیت, 
                count+1, count, 1)) {
            اذهب cleanup;
        }              
       saveBuf = موقت_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_هدف_ظرفیت =  (count * میانگیر_ظرفیت_MULTIPLIER + 1) /*for null termination */;
    int_هدف = (wchar_t*)uprv_malloc( int_هدف_ظرفیت * حجم(wchar_t) );

    إذا(int_هدف){

        int32_t nulLen = 0;
        int32_t مانده = int_هدف_ظرفیت;
        wchar_t* p_Int_هدف=int_هدف;
        موقت_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        لكل(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_هدف,(موقت_Buf+nulLen),مانده);
            
            إذا(retVal==-1){
                *p_Error_کد = U_INVALID_CHAR_یافت;
                اكسر;
            }وإلا إذا(retVal== مانده){/* should never occur */
                صحيح رقم_Written = (p_Int_هدف-int_هدف);
                u_grow_Any_میانگیر_From_Static(اشاره_تهی,(فراغ**) &int_هدف,
                                          &int_هدف_ظرفیت,
                                          int_هدف_ظرفیت * میانگیر_ظرفیت_MULTIPLIER,
                                          رقم_Written,
                                          حجم(wchar_t));
                p_Int_هدف = int_هدف;
                مانده=int_هدف_ظرفیت;

                إذا(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_هدف+=رقم_Written;
                    مانده-=رقم_Written;
                }

            }وإلا{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                طالما(موقت_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_درازی) ? 1 : 0; 
                p_Int_هدف = p_Int_هدف + retVal+nulVal;
                مانده -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                إذا(nulLen>=(count)){
                    اكسر;
                }
            }
        }
        count = (int32_t)(p_Int_هدف-int_هدف);
       
        إذا(0 < count && count <= dest_ظرفیت){
            uprv_memcpy(dest, int_هدف, (size_t)count*حجم(wchar_t));
        }  

        إذا(p_Dest_درازی){
            *p_Dest_درازی = count;
        }

        /* free the allocated memory */
        uprv_free(int_هدف);

    }وإلا{
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    إذا(پشته_میانگیر != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_ظرفیت,count,p_Error_کد);

    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#پایان_اگر

U_CAPI wchar_t* U_EXPORT2
u_str_مقصد_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_ظرفیت,
           int32_t* p_Dest_درازی,
           ثابت char16_t* src,
           int32_t src_درازی,
           UError_کد* p_Error_کد) {

    /* args check */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)){
        أرجع اشاره_تهی;
    }
        
    إذا( (src==اشاره_تهی && src_درازی!=0) || src_درازی < -1 ||
        (dest_ظرفیت<0) || (dest == اشاره_تهی && dest_ظرفیت > 0)
    ) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }
    
#اگر_تعریف_شده U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_درازی == -1){
        src_درازی = u_strlen(src);
    }
    إذا(0 < src_درازی && src_درازی <= dest_ظرفیت){
        u_memcpy((char16_t *)dest, src, src_درازی);
    }
    إذا(p_Dest_درازی){
       *p_Dest_درازی = src_درازی;
    }

    u_terminateUChars((char16_t *)dest,dest_ظرفیت,src_درازی,p_Error_کد);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع (wchar_t*)u_str_مقصد_UTF32((UChar32*)dest, dest_ظرفیت, p_Dest_درازی,
                                  src, src_درازی, p_Error_کد);

#وإلا
    
    أرجع str_مقصد_WCS(dest,dest_ظرفیت,p_Dest_درازی,src,src_درازی, p_Error_کد);
    
#پایان_اگر

}

#إذا !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساكن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_ظرفیت, 
             int32_t *p_Dest_درازی,
             ثابت wchar_t *src,
             int32_t src_درازی,
             UError_کد *p_Error_کد)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = اشاره_تهی;
    char16_t* p_هدف = اشاره_تهی;
    char16_t* p_هدف_حد = اشاره_تهی;
    char16_t* هدف = اشاره_تهی;
    
    char16_t u_پشته [پشته_میانگیر_ظرفیت];

    wchar_t w_پشته[پشته_میانگیر_ظرفیت];
    wchar_t* pWStack = w_پشته;


    حرف c_پشته[پشته_میانگیر_ظرفیت];
    int32_t c_پشته_Cap = پشته_میانگیر_ظرفیت;
    حرف* pCSrc=c_پشته;
    حرف* pCSave=pCSrc;
    حرف* p_CSrc_حد=اشاره_تهی;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* p_Src_حد = اشاره_تهی;

    إذا(src_درازی ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        لكل(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_پشته_Cap);
    
            إذا(retVal == -1){
                *p_Error_کد = U_ILLEGAL_CHAR_یافت;
                اذهب cleanup;
            }وإلا إذا(retVal >= (c_پشته_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_میانگیر_From_Static(c_پشته,(فراغ**)&pCSrc,&c_پشته_Cap,
                    c_پشته_Cap * میانگیر_ظرفیت_MULTIPLIER, 0, حجم(حرف));
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
        int32_t مانده =c_پشته_Cap;
        
        p_Src_حد = src + src_درازی;

        لكل(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            طالما(nulLen<src_درازی && pSrc[nulLen++]!=0){
            }

            إذا((pSrc+nulLen) < p_Src_حد){
                /* check if we have enough room in pCSrc */
                إذا(مانده < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_میانگیر_From_Static(c_پشته,(فراغ**)&pCSrc,&c_پشته_Cap,
                           میانگیر_ظرفیت_MULTIPLIER*c_پشته_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    مانده = c_پشته_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,مانده);

                إذا(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_کد = U_ILLEGAL_CHAR_یافت;
                    اذهب cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_درازی-=nulLen; /* decrement the srcLength */
                مانده -= (pCSrc-pCSave);


            }وإلا{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                إذا(nulLen >= پشته_میانگیر_ظرفیت){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    إذا(pWStack==اشاره_تهی){
                        *p_Error_کد = U_حافظه_تخصیص_ERROR;
                        اذهب cleanup;
                    }
                }
                إذا(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                إذا(مانده < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_میانگیر_From_Static(c_پشته,(فراغ**)&pCSrc,&c_پشته_Cap,
                           c_پشته_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    مانده = c_پشته_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,مانده);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_درازی-=nulLen; /* decrement the srcLength */
                اكسر;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_حد = pCSrc;
    pCSrc = pCSave;
    p_هدف = هدف= dest;
    p_هدف_حد = dest + dest_ظرفیت;    
    
    conv= u_getDefaultConverter(p_Error_کد);
    
    إذا(U_FAILURE(*p_Error_کد)|| conv==اشاره_تهی){
        اذهب cleanup;
    }
    
    لكل(;;) {
        UError_کد میانگیر_وضعیت = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_مقصد_Unicode(conv,&p_هدف,p_هدف_حد,(ثابت حرف**)&pCSrc,p_CSrc_حد,اشاره_تهی,(UBool)(pCSrc==p_CSrc_حد),&میانگیر_وضعیت);

        /* increment count to number written to stack */
        count+= p_هدف - هدف;

        إذا(میانگیر_وضعیت==U_میانگیر_سرریز_ERROR){
            هدف = u_پشته;
            p_هدف = u_پشته;
            p_هدف_حد = u_پشته + پشته_میانگیر_ظرفیت;
        } وإلا {
            إذا (U_FAILURE(میانگیر_وضعیت)) {
                *p_Error_کد = میانگیر_وضعیت;
            }
            اكسر;
        }
        
    }
    
    إذا(p_Dest_درازی){
        *p_Dest_درازی =count;
    }

    u_terminateUChars(dest,dest_ظرفیت,count,p_Error_کد);
    
cleanup:
 
    إذا(c_پشته != pCSave){
        uprv_free(pCSave);
    }

    إذا(w_پشته != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    أرجع dest;
}
#پایان_اگر

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_ظرفیت,
             int32_t* p_Dest_درازی,
             ثابت wchar_t* src,
             int32_t src_درازی,
             UError_کد* p_Error_کد)
{

    /* args check */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)){
        أرجع اشاره_تهی;
    }

    إذا( (src==اشاره_تهی && src_درازی!=0) || src_درازی < -1 ||
        (dest_ظرفیت<0) || (dest == اشاره_تهی && dest_ظرفیت > 0)
    ) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع اشاره_تهی;
    }

#اگر_تعریف_شده U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    إذا(src_درازی == -1){
        src_درازی = u_strlen((ثابت char16_t *)src);
    }
    إذا(0 < src_درازی && src_درازی <= dest_ظرفیت){
        u_memcpy(dest, (ثابت char16_t *)src, src_درازی);
    }
    إذا(p_Dest_درازی){
       *p_Dest_درازی = src_درازی;
    }

    u_terminateUChars(dest,dest_ظرفیت,src_درازی,p_Error_کد);

    أرجع dest;

#وإلاإذا defined U_WCHAR_IS_UTF32
    
    أرجع u_strFromUTF32(dest, dest_ظرفیت, p_Dest_درازی,
                          (UChar32*)src, src_درازی, p_Error_کد);

#وإلا

    أرجع _strFromWCS(dest,dest_ظرفیت,p_Dest_درازی,src,src_درازی,p_Error_کد);  

#پایان_اگر

}

#پایان_اگر /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
