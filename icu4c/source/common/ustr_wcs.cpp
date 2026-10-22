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

#تعریف_کرو ڈھیر_بفر_استعداد 1000
#تعریف_کرو بفر_استعداد_MULTIPLIER 2

#اگر !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ساکن اندرونی UBool 
u_grow_Any_بفر_From_Static(خالی *context,
                       خالی **p_بفر, int32_t *p_استعداد, int32_t req_استعداد,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    حرف *new_بفر=(حرف *)uprv_malloc(req_استعداد*size);
    اگر(new_بفر!=خالی_اشاریہ) {
        اگر(length>0) {
            uprv_memcpy(new_بفر, *p_بفر, (size_t)length*size);
        }
        *p_استعداد=req_استعداد;
    } ورنہ {
        *p_استعداد=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    اگر(*p_بفر!=(حرف *)context) {
        uprv_free(*p_بفر);
    }

    *p_بفر=new_بفر;
    واپس new_بفر!=خالی_اشاریہ;
}

/* helper function */
ساکن wchar_t* 
str_مقصد_WCS(wchar_t *dest, 
           int32_t dest_استعداد,
           int32_t *p_Dest_لمبائی,
           ثابت char16_t *src,
           int32_t src_لمبائی,
           UError_رمز *p_Error_رمز){

    حرف ڈھیر_بفر [ڈھیر_بفر_استعداد];
    حرف* عارضی_Buf = ڈھیر_بفر;
    int32_t عارضی_Buf_استعداد = ڈھیر_بفر_استعداد;
    حرف* عارضی_Buf_حد_بندی = ڈھیر_بفر + عارضی_Buf_استعداد;
    UConverter* conv = خالی_اشاریہ;
    حرف* saveBuf = عارضی_Buf;
    wchar_t* int_ہدف=خالی_اشاریہ;
    int32_t int_ہدف_استعداد=0;
    صحیح count=0,retVal=0;
    
    ثابت char16_t *p_Src_حد_بندی =خالی_اشاریہ;
    ثابت char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_رمز);
    
    اگر(U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
    
    اگر(src_لمبائی == -1){
        src_لمبائی = u_strlen(pSrc);
    }
    
    p_Src_حد_بندی = pSrc + src_لمبائی;

    برائے(;;) {
        UError_رمز بفر_حالت = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&عارضی_Buf,عارضی_Buf_حد_بندی,&pSrc,p_Src_حد_بندی,خالی_اشاریہ,(UBool)(pSrc==p_Src_حد_بندی),&بفر_حالت);
        count =(عارضی_Buf - saveBuf);
        
        /* This should rarely occur */
        اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR){
            عارضی_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_استعداد = 2 * src_لمبائی;
            اگر(new_استعداد <= عارضی_Buf_استعداد) {
                new_استعداد = بفر_استعداد_MULTIPLIER * عارضی_Buf_استعداد;
            }
            اگر(!u_grow_Any_بفر_From_Static(ڈھیر_بفر,(خالی**) &عارضی_Buf, &عارضی_Buf_استعداد,
                    new_استعداد, count, 1)) {
                جاؤ cleanup;
            }
          
           saveBuf = عارضی_Buf;
           عارضی_Buf_حد_بندی = عارضی_Buf + عارضی_Buf_استعداد;
           عارضی_Buf = عارضی_Buf + count;
        } ورنہ {
            اگر (U_FAILURE(بفر_حالت)) {
                *p_Error_رمز = بفر_حالت;
                جاؤ cleanup;
            }
            توڑو;
        }
    }

    /* done with conversion null terminate the char buffer */
    اگر(count>=عارضی_Buf_استعداد){
        عارضی_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        اگر(!u_grow_Any_بفر_From_Static(ڈھیر_بفر,(خالی**) &عارضی_Buf, &عارضی_Buf_استعداد, 
                count+1, count, 1)) {
            جاؤ cleanup;
        }              
       saveBuf = عارضی_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_ہدف_استعداد =  (count * بفر_استعداد_MULTIPLIER + 1) /*for null termination */;
    int_ہدف = (wchar_t*)uprv_malloc( int_ہدف_استعداد * حجم(wchar_t) );

    اگر(int_ہدف){

        int32_t nulLen = 0;
        int32_t بقایا = int_ہدف_استعداد;
        wchar_t* p_Int_ہدف=int_ہدف;
        عارضی_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        برائے(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_ہدف,(عارضی_Buf+nulLen),بقایا);
            
            اگر(retVal==-1){
                *p_Error_رمز = U_INVALID_CHAR_ملا;
                توڑو;
            }ورنہ اگر(retVal== بقایا){/* should never occur */
                صحیح عدد_Written = (p_Int_ہدف-int_ہدف);
                u_grow_Any_بفر_From_Static(خالی_اشاریہ,(خالی**) &int_ہدف,
                                          &int_ہدف_استعداد,
                                          int_ہدف_استعداد * بفر_استعداد_MULTIPLIER,
                                          عدد_Written,
                                          حجم(wchar_t));
                p_Int_ہدف = int_ہدف;
                بقایا=int_ہدف_استعداد;

                اگر(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_ہدف+=عدد_Written;
                    بقایا-=عدد_Written;
                }

            }ورنہ{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                جب(عارضی_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_لمبائی) ? 1 : 0; 
                p_Int_ہدف = p_Int_ہدف + retVal+nulVal;
                بقایا -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                اگر(nulLen>=(count)){
                    توڑو;
                }
            }
        }
        count = (int32_t)(p_Int_ہدف-int_ہدف);
       
        اگر(0 < count && count <= dest_استعداد){
            uprv_memcpy(dest, int_ہدف, (size_t)count*حجم(wchar_t));
        }  

        اگر(p_Dest_لمبائی){
            *p_Dest_لمبائی = count;
        }

        /* free the allocated memory */
        uprv_free(int_ہدف);

    }ورنہ{
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    اگر(ڈھیر_بفر != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_استعداد,count,p_Error_رمز);

    u_releaseDefaultConverter(conv);

    واپس dest;
}
#اگر_ختم

U_CAPI wchar_t* U_EXPORT2
u_str_مقصد_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_استعداد,
           int32_t* p_Dest_لمبائی,
           ثابت char16_t* src,
           int32_t src_لمبائی,
           UError_رمز* p_Error_رمز) {

    /* args check */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }
        
    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }
    
#اگر_تعریف_شدہ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    اگر(src_لمبائی == -1){
        src_لمبائی = u_strlen(src);
    }
    اگر(0 < src_لمبائی && src_لمبائی <= dest_استعداد){
        u_memcpy((char16_t *)dest, src, src_لمبائی);
    }
    اگر(p_Dest_لمبائی){
       *p_Dest_لمبائی = src_لمبائی;
    }

    u_terminateUChars((char16_t *)dest,dest_استعداد,src_لمبائی,p_Error_رمز);

    واپس dest;

#وگرنہ_اگر defined U_WCHAR_IS_UTF32
    
    واپس (wchar_t*)u_str_مقصد_UTF32((UChar32*)dest, dest_استعداد, p_Dest_لمبائی,
                                  src, src_لمبائی, p_Error_رمز);

#ورنہ
    
    واپس str_مقصد_WCS(dest,dest_استعداد,p_Dest_لمبائی,src,src_لمبائی, p_Error_رمز);
    
#اگر_ختم

}

#اگر !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ساکن char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_استعداد, 
             int32_t *p_Dest_لمبائی,
             ثابت wchar_t *src,
             int32_t src_لمبائی,
             UError_رمز *p_Error_رمز)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = خالی_اشاریہ;
    char16_t* p_ہدف = خالی_اشاریہ;
    char16_t* p_ہدف_حد_بندی = خالی_اشاریہ;
    char16_t* ہدف = خالی_اشاریہ;
    
    char16_t u_ڈھیر [ڈھیر_بفر_استعداد];

    wchar_t w_ڈھیر[ڈھیر_بفر_استعداد];
    wchar_t* pWStack = w_ڈھیر;


    حرف c_ڈھیر[ڈھیر_بفر_استعداد];
    int32_t c_ڈھیر_Cap = ڈھیر_بفر_استعداد;
    حرف* pCSrc=c_ڈھیر;
    حرف* pCSave=pCSrc;
    حرف* p_CSrc_حد_بندی=خالی_اشاریہ;

    ثابت wchar_t* pSrc = src;
    ثابت wchar_t* p_Src_حد_بندی = خالی_اشاریہ;

    اگر(src_لمبائی ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        برائے(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ڈھیر_Cap);
    
            اگر(retVal == -1){
                *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
                جاؤ cleanup;
            }ورنہ اگر(retVal >= (c_ڈھیر_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_بفر_From_Static(c_ڈھیر,(خالی**)&pCSrc,&c_ڈھیر_Cap,
                    c_ڈھیر_Cap * بفر_استعداد_MULTIPLIER, 0, حجم(حرف));
                pCSave = pCSrc;
            }ورنہ{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                توڑو;
            }
        }
        
    }ورنہ{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t بقایا =c_ڈھیر_Cap;
        
        p_Src_حد_بندی = src + src_لمبائی;

        برائے(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            جب(nulLen<src_لمبائی && pSrc[nulLen++]!=0){
            }

            اگر((pSrc+nulLen) < p_Src_حد_بندی){
                /* check if we have enough room in pCSrc */
                اگر(بقایا < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڈھیر,(خالی**)&pCSrc,&c_ڈھیر_Cap,
                           بفر_استعداد_MULTIPLIER*c_ڈھیر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بقایا = c_ڈھیر_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,بقایا);

                اگر(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
                    جاؤ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_لمبائی-=nulLen; /* decrement the srcLength */
                بقایا -= (pCSrc-pCSave);


            }ورنہ{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                اگر(nulLen >= ڈھیر_بفر_استعداد){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(حجم(wchar_t) * (nulLen + 1));
                    اگر(pWStack==خالی_اشاریہ){
                        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
                        جاؤ cleanup;
                    }
                }
                اگر(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*حجم(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                اگر(بقایا < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_بفر_From_Static(c_ڈھیر,(خالی**)&pCSrc,&c_ڈھیر_Cap,
                           c_ڈھیر_Cap+(nulLen*MB_CUR_MAX),len,حجم(حرف));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    بقایا = c_ڈھیر_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,بقایا);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_لمبائی-=nulLen; /* decrement the srcLength */
                توڑو;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_حد_بندی = pCSrc;
    pCSrc = pCSave;
    p_ہدف = ہدف= dest;
    p_ہدف_حد_بندی = dest + dest_استعداد;    
    
    conv= u_getDefaultConverter(p_Error_رمز);
    
    اگر(U_FAILURE(*p_Error_رمز)|| conv==خالی_اشاریہ){
        جاؤ cleanup;
    }
    
    برائے(;;) {
        UError_رمز بفر_حالت = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_مقصد_Unicode(conv,&p_ہدف,p_ہدف_حد_بندی,(ثابت حرف**)&pCSrc,p_CSrc_حد_بندی,خالی_اشاریہ,(UBool)(pCSrc==p_CSrc_حد_بندی),&بفر_حالت);

        /* increment count to number written to stack */
        count+= p_ہدف - ہدف;

        اگر(بفر_حالت==U_بفر_حد_سے_تجاوز_ERROR){
            ہدف = u_ڈھیر;
            p_ہدف = u_ڈھیر;
            p_ہدف_حد_بندی = u_ڈھیر + ڈھیر_بفر_استعداد;
        } ورنہ {
            اگر (U_FAILURE(بفر_حالت)) {
                *p_Error_رمز = بفر_حالت;
            }
            توڑو;
        }
        
    }
    
    اگر(p_Dest_لمبائی){
        *p_Dest_لمبائی =count;
    }

    u_terminateUChars(dest,dest_استعداد,count,p_Error_رمز);
    
cleanup:
 
    اگر(c_ڈھیر != pCSave){
        uprv_free(pCSave);
    }

    اگر(w_ڈھیر != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    واپس dest;
}
#اگر_ختم

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_استعداد,
             int32_t* p_Dest_لمبائی,
             ثابت wchar_t* src,
             int32_t src_لمبائی,
             UError_رمز* p_Error_رمز)
{

    /* args check */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)){
        واپس خالی_اشاریہ;
    }

    اگر( (src==خالی_اشاریہ && src_لمبائی!=0) || src_لمبائی < -1 ||
        (dest_استعداد<0) || (dest == خالی_اشاریہ && dest_استعداد > 0)
    ) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس خالی_اشاریہ;
    }

#اگر_تعریف_شدہ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    اگر(src_لمبائی == -1){
        src_لمبائی = u_strlen((ثابت char16_t *)src);
    }
    اگر(0 < src_لمبائی && src_لمبائی <= dest_استعداد){
        u_memcpy(dest, (ثابت char16_t *)src, src_لمبائی);
    }
    اگر(p_Dest_لمبائی){
       *p_Dest_لمبائی = src_لمبائی;
    }

    u_terminateUChars(dest,dest_استعداد,src_لمبائی,p_Error_رمز);

    واپس dest;

#وگرنہ_اگر defined U_WCHAR_IS_UTF32
    
    واپس u_strFromUTF32(dest, dest_استعداد, p_Dest_لمبائی,
                          (UChar32*)src, src_لمبائی, p_Error_رمز);

#ورنہ

    واپس _strFromWCS(dest,dest_استعداد,p_Dest_لمبائی,src,src_لمبائی,p_Error_رمز);  

#اگر_ختم

}

#اگر_ختم /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
