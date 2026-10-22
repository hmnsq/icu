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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cwchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_cnv.h"

#ਜੇ defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ 1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER 2

#ਜੇ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool 
u_grow_Any_ਬਫਰ_From_Static(ਖਾਲੀ *context,
                       ਖਾਲੀ **p_ਬਫਰ, int32_t *p_ਸਮਰੱਥਾ, int32_t req_ਸਮਰੱਥਾ,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    ਅੱਖਰ *new_ਬਫਰ=(ਅੱਖਰ *)uprv_malloc(req_ਸਮਰੱਥਾ*size);
    ਜੇ(new_ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(length>0) {
            uprv_memcpy(new_ਬਫਰ, *p_ਬਫਰ, (size_t)length*size);
        }
        *p_ਸਮਰੱਥਾ=req_ਸਮਰੱਥਾ;
    } ਨਹੀਂਤਾਂ {
        *p_ਸਮਰੱਥਾ=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ਜੇ(*p_ਬਫਰ!=(ਅੱਖਰ *)context) {
        uprv_free(*p_ਬਫਰ);
    }

    *p_ਬਫਰ=new_ਬਫਰ;
    ਵਾਪਸ new_ਬਫਰ!=ਖਾਲੀ_ਸੂਚਕ;
}

/* helper function */
ਸਥਿਰ wchar_t* 
str_ਮੰਜ਼ਿਲ_WCS(wchar_t *dest, 
           int32_t dest_ਸਮਰੱਥਾ,
           int32_t *p_Dest_ਲੰਮਾਈ,
           ਅਚਲ char16_t *src,
           int32_t src_ਲੰਮਾਈ,
           UError_ਕੋਡ *p_Error_ਕੋਡ){

    ਅੱਖਰ ਢੇਰ_ਬਫਰ [ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ];
    ਅੱਖਰ* ਆਰਜ਼ੀ_Buf = ਢੇਰ_ਬਫਰ;
    int32_t ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ = ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ;
    ਅੱਖਰ* ਆਰਜ਼ੀ_Buf_ਹੱਦਬੰਦੀ = ਢੇਰ_ਬਫਰ + ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ;
    UConverter* conv = ਖਾਲੀ_ਸੂਚਕ;
    ਅੱਖਰ* saveBuf = ਆਰਜ਼ੀ_Buf;
    wchar_t* int_ਨਿਸ਼ਾਨਾ=ਖਾਲੀ_ਸੂਚਕ;
    int32_t int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
    ਪੂਰਨ count=0,retVal=0;
    
    ਅਚਲ char16_t *p_Src_ਹੱਦਬੰਦੀ =ਖਾਲੀ_ਸੂਚਕ;
    ਅਚਲ char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_ਕੋਡ);
    
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
    ਜੇ(src_ਲੰਮਾਈ == -1){
        src_ਲੰਮਾਈ = u_strlen(pSrc);
    }
    
    p_Src_ਹੱਦਬੰਦੀ = pSrc + src_ਲੰਮਾਈ;

    ਲਈ(;;) {
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&ਆਰਜ਼ੀ_Buf,ਆਰਜ਼ੀ_Buf_ਹੱਦਬੰਦੀ,&pSrc,p_Src_ਹੱਦਬੰਦੀ,ਖਾਲੀ_ਸੂਚਕ,(UBool)(pSrc==p_Src_ਹੱਦਬੰਦੀ),&ਬਫਰ_ਸਥਿਤੀ);
        count =(ਆਰਜ਼ੀ_Buf - saveBuf);
        
        /* This should rarely occur */
        ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR){
            ਆਰਜ਼ੀ_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_ਸਮਰੱਥਾ = 2 * src_ਲੰਮਾਈ;
            ਜੇ(new_ਸਮਰੱਥਾ <= ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ) {
                new_ਸਮਰੱਥਾ = ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER * ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ;
            }
            ਜੇ(!u_grow_Any_ਬਫਰ_From_Static(ਢੇਰ_ਬਫਰ,(ਖਾਲੀ**) &ਆਰਜ਼ੀ_Buf, &ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ,
                    new_ਸਮਰੱਥਾ, count, 1)) {
                ਜਾਓ cleanup;
            }
          
           saveBuf = ਆਰਜ਼ੀ_Buf;
           ਆਰਜ਼ੀ_Buf_ਹੱਦਬੰਦੀ = ਆਰਜ਼ੀ_Buf + ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ;
           ਆਰਜ਼ੀ_Buf = ਆਰਜ਼ੀ_Buf + count;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
                *p_Error_ਕੋਡ = ਬਫਰ_ਸਥਿਤੀ;
                ਜਾਓ cleanup;
            }
            ਰੋਕੋ;
        }
    }

    /* done with conversion null terminate the char buffer */
    ਜੇ(count>=ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ){
        ਆਰਜ਼ੀ_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ਜੇ(!u_grow_Any_ਬਫਰ_From_Static(ਢੇਰ_ਬਫਰ,(ਖਾਲੀ**) &ਆਰਜ਼ੀ_Buf, &ਆਰਜ਼ੀ_Buf_ਸਮਰੱਥਾ, 
                count+1, count, 1)) {
            ਜਾਓ cleanup;
        }              
       saveBuf = ਆਰਜ਼ੀ_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ =  (count * ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER + 1) /*for null termination */;
    int_ਨਿਸ਼ਾਨਾ = (wchar_t*)uprv_malloc( int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ * ਆਕਾਰ(wchar_t) );

    ਜੇ(int_ਨਿਸ਼ਾਨਾ){

        int32_t nulLen = 0;
        int32_t ਬਚਿਆ = int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        wchar_t* p_Int_ਨਿਸ਼ਾਨਾ=int_ਨਿਸ਼ਾਨਾ;
        ਆਰਜ਼ੀ_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        ਲਈ(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_ਨਿਸ਼ਾਨਾ,(ਆਰਜ਼ੀ_Buf+nulLen),ਬਚਿਆ);
            
            ਜੇ(retVal==-1){
                *p_Error_ਕੋਡ = U_INVALID_CHAR_ਮਿਲਿਆ;
                ਰੋਕੋ;
            }ਨਹੀਂਤਾਂ ਜੇ(retVal== ਬਚਿਆ){/* should never occur */
                ਪੂਰਨ ਅੰਕ_Written = (p_Int_ਨਿਸ਼ਾਨਾ-int_ਨਿਸ਼ਾਨਾ);
                u_grow_Any_ਬਫਰ_From_Static(ਖਾਲੀ_ਸੂਚਕ,(ਖਾਲੀ**) &int_ਨਿਸ਼ਾਨਾ,
                                          &int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                                          int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ * ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER,
                                          ਅੰਕ_Written,
                                          ਆਕਾਰ(wchar_t));
                p_Int_ਨਿਸ਼ਾਨਾ = int_ਨਿਸ਼ਾਨਾ;
                ਬਚਿਆ=int_ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

                ਜੇ(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_ਨਿਸ਼ਾਨਾ+=ਅੰਕ_Written;
                    ਬਚਿਆ-=ਅੰਕ_Written;
                }

            }ਨਹੀਂਤਾਂ{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ਜਦੋਂ(ਆਰਜ਼ੀ_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_ਲੰਮਾਈ) ? 1 : 0; 
                p_Int_ਨਿਸ਼ਾਨਾ = p_Int_ਨਿਸ਼ਾਨਾ + retVal+nulVal;
                ਬਚਿਆ -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ਜੇ(nulLen>=(count)){
                    ਰੋਕੋ;
                }
            }
        }
        count = (int32_t)(p_Int_ਨਿਸ਼ਾਨਾ-int_ਨਿਸ਼ਾਨਾ);
       
        ਜੇ(0 < count && count <= dest_ਸਮਰੱਥਾ){
            uprv_memcpy(dest, int_ਨਿਸ਼ਾਨਾ, (size_t)count*ਆਕਾਰ(wchar_t));
        }  

        ਜੇ(p_Dest_ਲੰਮਾਈ){
            *p_Dest_ਲੰਮਾਈ = count;
        }

        /* free the allocated memory */
        uprv_free(int_ਨਿਸ਼ਾਨਾ);

    }ਨਹੀਂਤਾਂ{
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ਜੇ(ਢੇਰ_ਬਫਰ != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_ਸਮਰੱਥਾ,count,p_Error_ਕੋਡ);

    u_releaseDefaultConverter(conv);

    ਵਾਪਸ dest;
}
#ਜੇ_ਅੰਤ

U_CAPI wchar_t* U_EXPORT2
u_str_ਮੰਜ਼ਿਲ_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_ਸਮਰੱਥਾ,
           int32_t* p_Dest_ਲੰਮਾਈ,
           ਅਚਲ char16_t* src,
           int32_t src_ਲੰਮਾਈ,
           UError_ਕੋਡ* p_Error_ਕੋਡ) {

    /* args check */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
        
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ਜੇ(src_ਲੰਮਾਈ == -1){
        src_ਲੰਮਾਈ = u_strlen(src);
    }
    ਜੇ(0 < src_ਲੰਮਾਈ && src_ਲੰਮਾਈ <= dest_ਸਮਰੱਥਾ){
        u_memcpy((char16_t *)dest, src, src_ਲੰਮਾਈ);
    }
    ਜੇ(p_Dest_ਲੰਮਾਈ){
       *p_Dest_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
    }

    u_terminateUChars((char16_t *)dest,dest_ਸਮਰੱਥਾ,src_ਲੰਮਾਈ,p_Error_ਕੋਡ);

    ਵਾਪਸ dest;

#ਜਾਂਜੇ defined U_WCHAR_IS_UTF32
    
    ਵਾਪਸ (wchar_t*)u_str_ਮੰਜ਼ਿਲ_UTF32((UChar32*)dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
                                  src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);

#ਨਹੀਂਤਾਂ
    
    ਵਾਪਸ str_ਮੰਜ਼ਿਲ_WCS(dest,dest_ਸਮਰੱਥਾ,p_Dest_ਲੰਮਾਈ,src,src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    
#ਜੇ_ਅੰਤ

}

#ਜੇ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ਸਥਿਰ char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_ਸਮਰੱਥਾ, 
             int32_t *p_Dest_ਲੰਮਾਈ,
             ਅਚਲ wchar_t *src,
             int32_t src_ਲੰਮਾਈ,
             UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = ਖਾਲੀ_ਸੂਚਕ;
    char16_t* p_ਨਿਸ਼ਾਨਾ = ਖਾਲੀ_ਸੂਚਕ;
    char16_t* p_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = ਖਾਲੀ_ਸੂਚਕ;
    char16_t* ਨਿਸ਼ਾਨਾ = ਖਾਲੀ_ਸੂਚਕ;
    
    char16_t u_ਢੇਰ [ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ];

    wchar_t w_ਢੇਰ[ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ];
    wchar_t* pWStack = w_ਢੇਰ;


    ਅੱਖਰ c_ਢੇਰ[ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ];
    int32_t c_ਢੇਰ_Cap = ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ;
    ਅੱਖਰ* pCSrc=c_ਢੇਰ;
    ਅੱਖਰ* pCSave=pCSrc;
    ਅੱਖਰ* p_CSrc_ਹੱਦਬੰਦੀ=ਖਾਲੀ_ਸੂਚਕ;

    ਅਚਲ wchar_t* pSrc = src;
    ਅਚਲ wchar_t* p_Src_ਹੱਦਬੰਦੀ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ(src_ਲੰਮਾਈ ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        ਲਈ(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ਢੇਰ_Cap);
    
            ਜੇ(retVal == -1){
                *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                ਜਾਓ cleanup;
            }ਨਹੀਂਤਾਂ ਜੇ(retVal >= (c_ਢੇਰ_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_ਬਫਰ_From_Static(c_ਢੇਰ,(ਖਾਲੀ**)&pCSrc,&c_ਢੇਰ_Cap,
                    c_ਢੇਰ_Cap * ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER, 0, ਆਕਾਰ(ਅੱਖਰ));
                pCSave = pCSrc;
            }ਨਹੀਂਤਾਂ{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ਰੋਕੋ;
            }
        }
        
    }ਨਹੀਂਤਾਂ{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t ਬਚਿਆ =c_ਢੇਰ_Cap;
        
        p_Src_ਹੱਦਬੰਦੀ = src + src_ਲੰਮਾਈ;

        ਲਈ(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ਜਦੋਂ(nulLen<src_ਲੰਮਾਈ && pSrc[nulLen++]!=0){
            }

            ਜੇ((pSrc+nulLen) < p_Src_ਹੱਦਬੰਦੀ){
                /* check if we have enough room in pCSrc */
                ਜੇ(ਬਚਿਆ < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_ਬਫਰ_From_Static(c_ਢੇਰ,(ਖਾਲੀ**)&pCSrc,&c_ਢੇਰ_Cap,
                           ਬਫਰ_ਸਮਰੱਥਾ_MULTIPLIER*c_ਢੇਰ_Cap+(nulLen*MB_CUR_MAX),len,ਆਕਾਰ(ਅੱਖਰ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    ਬਚਿਆ = c_ਢੇਰ_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,ਬਚਿਆ);

                ਜੇ(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_ਲੰਮਾਈ-=nulLen; /* decrement the srcLength */
                ਬਚਿਆ -= (pCSrc-pCSave);


            }ਨਹੀਂਤਾਂ{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ਜੇ(nulLen >= ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(ਆਕਾਰ(wchar_t) * (nulLen + 1));
                    ਜੇ(pWStack==ਖਾਲੀ_ਸੂਚਕ){
                        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਜਾਓ cleanup;
                    }
                }
                ਜੇ(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*ਆਕਾਰ(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ਜੇ(ਬਚਿਆ < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_ਬਫਰ_From_Static(c_ਢੇਰ,(ਖਾਲੀ**)&pCSrc,&c_ਢੇਰ_Cap,
                           c_ਢੇਰ_Cap+(nulLen*MB_CUR_MAX),len,ਆਕਾਰ(ਅੱਖਰ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    ਬਚਿਆ = c_ਢੇਰ_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,ਬਚਿਆ);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_ਲੰਮਾਈ-=nulLen; /* decrement the srcLength */
                ਰੋਕੋ;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_ਹੱਦਬੰਦੀ = pCSrc;
    pCSrc = pCSave;
    p_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ= dest;
    p_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = dest + dest_ਸਮਰੱਥਾ;    
    
    conv= u_getDefaultConverter(p_Error_ਕੋਡ);
    
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)|| conv==ਖਾਲੀ_ਸੂਚਕ){
        ਜਾਓ cleanup;
    }
    
    ਲਈ(;;) {
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_ਮੰਜ਼ਿਲ_Unicode(conv,&p_ਨਿਸ਼ਾਨਾ,p_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ,(ਅਚਲ ਅੱਖਰ**)&pCSrc,p_CSrc_ਹੱਦਬੰਦੀ,ਖਾਲੀ_ਸੂਚਕ,(UBool)(pCSrc==p_CSrc_ਹੱਦਬੰਦੀ),&ਬਫਰ_ਸਥਿਤੀ);

        /* increment count to number written to stack */
        count+= p_ਨਿਸ਼ਾਨਾ - ਨਿਸ਼ਾਨਾ;

        ਜੇ(ਬਫਰ_ਸਥਿਤੀ==U_ਬਫਰ_ਉਛਾਲ_ERROR){
            ਨਿਸ਼ਾਨਾ = u_ਢੇਰ;
            p_ਨਿਸ਼ਾਨਾ = u_ਢੇਰ;
            p_ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ = u_ਢੇਰ + ਢੇਰ_ਬਫਰ_ਸਮਰੱਥਾ;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
                *p_Error_ਕੋਡ = ਬਫਰ_ਸਥਿਤੀ;
            }
            ਰੋਕੋ;
        }
        
    }
    
    ਜੇ(p_Dest_ਲੰਮਾਈ){
        *p_Dest_ਲੰਮਾਈ =count;
    }

    u_terminateUChars(dest,dest_ਸਮਰੱਥਾ,count,p_Error_ਕੋਡ);
    
cleanup:
 
    ਜੇ(c_ਢੇਰ != pCSave){
        uprv_free(pCSave);
    }

    ਜੇ(w_ਢੇਰ != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    ਵਾਪਸ dest;
}
#ਜੇ_ਅੰਤ

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_ਸਮਰੱਥਾ,
             int32_t* p_Dest_ਲੰਮਾਈ,
             ਅਚਲ wchar_t* src,
             int32_t src_ਲੰਮਾਈ,
             UError_ਕੋਡ* p_Error_ਕੋਡ)
{

    /* args check */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ && src_ਲੰਮਾਈ!=0) || src_ਲੰਮਾਈ < -1 ||
        (dest_ਸਮਰੱਥਾ<0) || (dest == ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ > 0)
    ) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ਜੇ(src_ਲੰਮਾਈ == -1){
        src_ਲੰਮਾਈ = u_strlen((ਅਚਲ char16_t *)src);
    }
    ਜੇ(0 < src_ਲੰਮਾਈ && src_ਲੰਮਾਈ <= dest_ਸਮਰੱਥਾ){
        u_memcpy(dest, (ਅਚਲ char16_t *)src, src_ਲੰਮਾਈ);
    }
    ਜੇ(p_Dest_ਲੰਮਾਈ){
       *p_Dest_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
    }

    u_terminateUChars(dest,dest_ਸਮਰੱਥਾ,src_ਲੰਮਾਈ,p_Error_ਕੋਡ);

    ਵਾਪਸ dest;

#ਜਾਂਜੇ defined U_WCHAR_IS_UTF32
    
    ਵਾਪਸ u_strFromUTF32(dest, dest_ਸਮਰੱਥਾ, p_Dest_ਲੰਮਾਈ,
                          (UChar32*)src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);

#ਨਹੀਂਤਾਂ

    ਵਾਪਸ _strFromWCS(dest,dest_ਸਮਰੱਥਾ,p_Dest_ਲੰਮਾਈ,src,src_ਲੰਮਾਈ,p_Error_ਕੋਡ);  

#ਜੇ_ਅੰਤ

}

#ਜੇ_ਅੰਤ /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
