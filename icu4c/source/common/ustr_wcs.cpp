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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cwchar.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "ustr_cnv.h"

#ಆದರೆ defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ 1000
#ವ್ಯಾಖ್ಯಾನಿಸು ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER 2

#ಆದರೆ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool 
u_grow_Any_ಬಫರ್_From_Static(ಶೂನ್ಯ *context,
                       ಶೂನ್ಯ **p_ಬಫರ್, int32_t *p_ಸಾಮರ್ಥ್ಯ, int32_t req_ಸಾಮರ್ಥ್ಯ,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    ಅಕ್ಷರ *new_ಬಫರ್=(ಅಕ್ಷರ *)uprv_malloc(req_ಸಾಮರ್ಥ್ಯ*size);
    ಆದರೆ(new_ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(length>0) {
            uprv_memcpy(new_ಬಫರ್, *p_ಬಫರ್, (size_t)length*size);
        }
        *p_ಸಾಮರ್ಥ್ಯ=req_ಸಾಮರ್ಥ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_ಸಾಮರ್ಥ್ಯ=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ಆದರೆ(*p_ಬಫರ್!=(ಅಕ್ಷರ *)context) {
        uprv_free(*p_ಬಫರ್);
    }

    *p_ಬಫರ್=new_ಬಫರ್;
    ಹಿಂತಿರುಗಿ new_ಬಫರ್!=ಶೂನ್ಯಸೂಚಕ;
}

/* helper function */
ಸ್ಥಿರ wchar_t* 
str_ಗಮ್ಯ_WCS(wchar_t *dest, 
           int32_t dest_ಸಾಮರ್ಥ್ಯ,
           int32_t *p_Dest_ಉದ್ದಳತೆ,
           ಸ್ಥಿರಾಂಕ char16_t *src,
           int32_t src_ಉದ್ದಳತೆ,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ){

    ಅಕ್ಷರ ರಾಶಿ_ಬಫರ್ [ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ];
    ಅಕ್ಷರ* ತಾತ್ಕಾಲಿಕ_Buf = ರಾಶಿ_ಬಫರ್;
    int32_t ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ = ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ;
    ಅಕ್ಷರ* ತಾತ್ಕಾಲಿಕ_Buf_ಎಲ್ಲೆ = ರಾಶಿ_ಬಫರ್ + ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ;
    UConverter* conv = ಶೂನ್ಯಸೂಚಕ;
    ಅಕ್ಷರ* saveBuf = ತಾತ್ಕಾಲಿಕ_Buf;
    wchar_t* int_ಗುರಿ=ಶೂನ್ಯಸೂಚಕ;
    int32_t int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
    ಪೂರ್ಣಾಂಕ count=0,retVal=0;
    
    ಸ್ಥಿರಾಂಕ char16_t *p_Src_ಎಲ್ಲೆ =ಶೂನ್ಯಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_ಸಂಕೇತ);
    
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
    ಆದರೆ(src_ಉದ್ದಳತೆ == -1){
        src_ಉದ್ದಳತೆ = u_strlen(pSrc);
    }
    
    p_Src_ಎಲ್ಲೆ = pSrc + src_ಉದ್ದಳತೆ;

    ಗಾಗಿ(;;) {
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&ತಾತ್ಕಾಲಿಕ_Buf,ತಾತ್ಕಾಲಿಕ_Buf_ಎಲ್ಲೆ,&pSrc,p_Src_ಎಲ್ಲೆ,ಶೂನ್ಯಸೂಚಕ,(UBool)(pSrc==p_Src_ಎಲ್ಲೆ),&ಬಫರ್_ಸ್ಥಿತಿ);
        count =(ತಾತ್ಕಾಲಿಕ_Buf - saveBuf);
        
        /* This should rarely occur */
        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            ತಾತ್ಕಾಲಿಕ_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_ಸಾಮರ್ಥ್ಯ = 2 * src_ಉದ್ದಳತೆ;
            ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ <= ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ) {
                new_ಸಾಮರ್ಥ್ಯ = ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER * ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ;
            }
            ಆದರೆ(!u_grow_Any_ಬಫರ್_From_Static(ರಾಶಿ_ಬಫರ್,(ಶೂನ್ಯ**) &ತಾತ್ಕಾಲಿಕ_Buf, &ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ,
                    new_ಸಾಮರ್ಥ್ಯ, count, 1)) {
                ಹೋಗು cleanup;
            }
          
           saveBuf = ತಾತ್ಕಾಲಿಕ_Buf;
           ತಾತ್ಕಾಲಿಕ_Buf_ಎಲ್ಲೆ = ತಾತ್ಕಾಲಿಕ_Buf + ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ;
           ತಾತ್ಕಾಲಿಕ_Buf = ತಾತ್ಕಾಲಿಕ_Buf + count;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
                *p_Error_ಸಂಕೇತ = ಬಫರ್_ಸ್ಥಿತಿ;
                ಹೋಗು cleanup;
            }
            ನಿಲ್ಲಿಸು;
        }
    }

    /* done with conversion null terminate the char buffer */
    ಆದರೆ(count>=ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ){
        ತಾತ್ಕಾಲಿಕ_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ಆದರೆ(!u_grow_Any_ಬಫರ್_From_Static(ರಾಶಿ_ಬಫರ್,(ಶೂನ್ಯ**) &ತಾತ್ಕಾಲಿಕ_Buf, &ತಾತ್ಕಾಲಿಕ_Buf_ಸಾಮರ್ಥ್ಯ, 
                count+1, count, 1)) {
            ಹೋಗು cleanup;
        }              
       saveBuf = ತಾತ್ಕಾಲಿಕ_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ =  (count * ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER + 1) /*for null termination */;
    int_ಗುರಿ = (wchar_t*)uprv_malloc( int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ * ಗಾತ್ರ(wchar_t) );

    ಆದರೆ(int_ಗುರಿ){

        int32_t nulLen = 0;
        int32_t ಉಳಿದ = int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        wchar_t* p_Int_ಗುರಿ=int_ಗುರಿ;
        ತಾತ್ಕಾಲಿಕ_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        ಗಾಗಿ(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_ಗುರಿ,(ತಾತ್ಕಾಲಿಕ_Buf+nulLen),ಉಳಿದ);
            
            ಆದರೆ(retVal==-1){
                *p_Error_ಸಂಕೇತ = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                ನಿಲ್ಲಿಸು;
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(retVal== ಉಳಿದ){/* should never occur */
                ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Written = (p_Int_ಗುರಿ-int_ಗುರಿ);
                u_grow_Any_ಬಫರ್_From_Static(ಶೂನ್ಯಸೂಚಕ,(ಶೂನ್ಯ**) &int_ಗುರಿ,
                                          &int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                                          int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ * ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER,
                                          ಅಂಕೆ_Written,
                                          ಗಾತ್ರ(wchar_t));
                p_Int_ಗುರಿ = int_ಗುರಿ;
                ಉಳಿದ=int_ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

                ಆದರೆ(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_ಗುರಿ+=ಅಂಕೆ_Written;
                    ಉಳಿದ-=ಅಂಕೆ_Written;
                }

            }ಇಲ್ಲದಿದ್ದರೆ{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                ಆಗ(ತಾತ್ಕಾಲಿಕ_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_ಉದ್ದಳತೆ) ? 1 : 0; 
                p_Int_ಗುರಿ = p_Int_ಗುರಿ + retVal+nulVal;
                ಉಳಿದ -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ಆದರೆ(nulLen>=(count)){
                    ನಿಲ್ಲಿಸು;
                }
            }
        }
        count = (int32_t)(p_Int_ಗುರಿ-int_ಗುರಿ);
       
        ಆದರೆ(0 < count && count <= dest_ಸಾಮರ್ಥ್ಯ){
            uprv_memcpy(dest, int_ಗುರಿ, (size_t)count*ಗಾತ್ರ(wchar_t));
        }  

        ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
            *p_Dest_ಉದ್ದಳತೆ = count;
        }

        /* free the allocated memory */
        uprv_free(int_ಗುರಿ);

    }ಇಲ್ಲದಿದ್ದರೆ{
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ಆದರೆ(ರಾಶಿ_ಬಫರ್ != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_ಸಾಮರ್ಥ್ಯ,count,p_Error_ಸಂಕೇತ);

    u_releaseDefaultConverter(conv);

    ಹಿಂತಿರುಗಿ dest;
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI wchar_t* U_EXPORT2
u_str_ಗಮ್ಯ_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_ಸಾಮರ್ಥ್ಯ,
           int32_t* p_Dest_ಉದ್ದಳತೆ,
           ಸ್ಥಿರಾಂಕ char16_t* src,
           int32_t src_ಉದ್ದಳತೆ,
           UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ) {

    /* args check */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
        
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ಆದರೆ(src_ಉದ್ದಳತೆ == -1){
        src_ಉದ್ದಳತೆ = u_strlen(src);
    }
    ಆದರೆ(0 < src_ಉದ್ದಳತೆ && src_ಉದ್ದಳತೆ <= dest_ಸಾಮರ್ಥ್ಯ){
        u_memcpy((char16_t *)dest, src, src_ಉದ್ದಳತೆ);
    }
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
       *p_Dest_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
    }

    u_terminateUChars((char16_t *)dest,dest_ಸಾಮರ್ಥ್ಯ,src_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;

#ಅಥವಾಆದರೆ defined U_WCHAR_IS_UTF32
    
    ಹಿಂತಿರುಗಿ (wchar_t*)u_str_ಗಮ್ಯ_UTF32((UChar32*)dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
                                  src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);

#ಇಲ್ಲದಿದ್ದರೆ
    
    ಹಿಂತಿರುಗಿ str_ಗಮ್ಯ_WCS(dest,dest_ಸಾಮರ್ಥ್ಯ,p_Dest_ಉದ್ದಳತೆ,src,src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    
#ಆದರೆ_ಅಂತ್ಯ

}

#ಆದರೆ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
ಸ್ಥಿರ char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_ಸಾಮರ್ಥ್ಯ, 
             int32_t *p_Dest_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ wchar_t *src,
             int32_t src_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = ಶೂನ್ಯಸೂಚಕ;
    char16_t* p_ಗುರಿ = ಶೂನ್ಯಸೂಚಕ;
    char16_t* p_ಗುರಿ_ಎಲ್ಲೆ = ಶೂನ್ಯಸೂಚಕ;
    char16_t* ಗುರಿ = ಶೂನ್ಯಸೂಚಕ;
    
    char16_t u_ರಾಶಿ [ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ];

    wchar_t w_ರಾಶಿ[ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ];
    wchar_t* pWStack = w_ರಾಶಿ;


    ಅಕ್ಷರ c_ರಾಶಿ[ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ];
    int32_t c_ರಾಶಿ_Cap = ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ;
    ಅಕ್ಷರ* pCSrc=c_ರಾಶಿ;
    ಅಕ್ಷರ* pCSave=pCSrc;
    ಅಕ್ಷರ* p_CSrc_ಎಲ್ಲೆ=ಶೂನ್ಯಸೂಚಕ;

    ಸ್ಥಿರಾಂಕ wchar_t* pSrc = src;
    ಸ್ಥಿರಾಂಕ wchar_t* p_Src_ಎಲ್ಲೆ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ(src_ಉದ್ದಳತೆ ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        ಗಾಗಿ(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_ರಾಶಿ_Cap);
    
            ಆದರೆ(retVal == -1){
                *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                ಹೋಗು cleanup;
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(retVal >= (c_ರಾಶಿ_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_ಬಫರ್_From_Static(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                    c_ರಾಶಿ_Cap * ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER, 0, ಗಾತ್ರ(ಅಕ್ಷರ));
                pCSave = pCSrc;
            }ಇಲ್ಲದಿದ್ದರೆ{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ನಿಲ್ಲಿಸು;
            }
        }
        
    }ಇಲ್ಲದಿದ್ದರೆ{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t ಉಳಿದ =c_ರಾಶಿ_Cap;
        
        p_Src_ಎಲ್ಲೆ = src + src_ಉದ್ದಳತೆ;

        ಗಾಗಿ(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            ಆಗ(nulLen<src_ಉದ್ದಳತೆ && pSrc[nulLen++]!=0){
            }

            ಆದರೆ((pSrc+nulLen) < p_Src_ಎಲ್ಲೆ){
                /* check if we have enough room in pCSrc */
                ಆದರೆ(ಉಳಿದ < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_ಬಫರ್_From_Static(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                           ಬಫರ್_ಸಾಮರ್ಥ್ಯ_MULTIPLIER*c_ರಾಶಿ_Cap+(nulLen*MB_CUR_MAX),len,ಗಾತ್ರ(ಅಕ್ಷರ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    ಉಳಿದ = c_ರಾಶಿ_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,ಉಳಿದ);

                ಆದರೆ(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_ಉದ್ದಳತೆ-=nulLen; /* decrement the srcLength */
                ಉಳಿದ -= (pCSrc-pCSave);


            }ಇಲ್ಲದಿದ್ದರೆ{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ಆದರೆ(nulLen >= ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(ಗಾತ್ರ(wchar_t) * (nulLen + 1));
                    ಆದರೆ(pWStack==ಶೂನ್ಯಸೂಚಕ){
                        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ಹೋಗು cleanup;
                    }
                }
                ಆದರೆ(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*ಗಾತ್ರ(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ಆದರೆ(ಉಳಿದ < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_ಬಫರ್_From_Static(c_ರಾಶಿ,(ಶೂನ್ಯ**)&pCSrc,&c_ರಾಶಿ_Cap,
                           c_ರಾಶಿ_Cap+(nulLen*MB_CUR_MAX),len,ಗಾತ್ರ(ಅಕ್ಷರ));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    ಉಳಿದ = c_ರಾಶಿ_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,ಉಳಿದ);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_ಉದ್ದಳತೆ-=nulLen; /* decrement the srcLength */
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_ಎಲ್ಲೆ = pCSrc;
    pCSrc = pCSave;
    p_ಗುರಿ = ಗುರಿ= dest;
    p_ಗುರಿ_ಎಲ್ಲೆ = dest + dest_ಸಾಮರ್ಥ್ಯ;    
    
    conv= u_getDefaultConverter(p_Error_ಸಂಕೇತ);
    
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)|| conv==ಶೂನ್ಯಸೂಚಕ){
        ಹೋಗು cleanup;
    }
    
    ಗಾಗಿ(;;) {
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_ಗಮ್ಯ_Unicode(conv,&p_ಗುರಿ,p_ಗುರಿ_ಎಲ್ಲೆ,(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**)&pCSrc,p_CSrc_ಎಲ್ಲೆ,ಶೂನ್ಯಸೂಚಕ,(UBool)(pCSrc==p_CSrc_ಎಲ್ಲೆ),&ಬಫರ್_ಸ್ಥಿತಿ);

        /* increment count to number written to stack */
        count+= p_ಗುರಿ - ಗುರಿ;

        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ==U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            ಗುರಿ = u_ರಾಶಿ;
            p_ಗುರಿ = u_ರಾಶಿ;
            p_ಗುರಿ_ಎಲ್ಲೆ = u_ರಾಶಿ + ರಾಶಿ_ಬಫರ್_ಸಾಮರ್ಥ್ಯ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
                *p_Error_ಸಂಕೇತ = ಬಫರ್_ಸ್ಥಿತಿ;
            }
            ನಿಲ್ಲಿಸು;
        }
        
    }
    
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
        *p_Dest_ಉದ್ದಳತೆ =count;
    }

    u_terminateUChars(dest,dest_ಸಾಮರ್ಥ್ಯ,count,p_Error_ಸಂಕೇತ);
    
cleanup:
 
    ಆದರೆ(c_ರಾಶಿ != pCSave){
        uprv_free(pCSave);
    }

    ಆದರೆ(w_ರಾಶಿ != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    ಹಿಂತಿರುಗಿ dest;
}
#ಆದರೆ_ಅಂತ್ಯ

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_ಸಾಮರ್ಥ್ಯ,
             int32_t* p_Dest_ಉದ್ದಳತೆ,
             ಸ್ಥಿರಾಂಕ wchar_t* src,
             int32_t src_ಉದ್ದಳತೆ,
             UError_ಸಂಕೇತ* p_Error_ಸಂಕೇತ)
{

    /* args check */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ && src_ಉದ್ದಳತೆ!=0) || src_ಉದ್ದಳತೆ < -1 ||
        (dest_ಸಾಮರ್ಥ್ಯ<0) || (dest == ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ > 0)
    ) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ಆದರೆ(src_ಉದ್ದಳತೆ == -1){
        src_ಉದ್ದಳತೆ = u_strlen((ಸ್ಥಿರಾಂಕ char16_t *)src);
    }
    ಆದರೆ(0 < src_ಉದ್ದಳತೆ && src_ಉದ್ದಳತೆ <= dest_ಸಾಮರ್ಥ್ಯ){
        u_memcpy(dest, (ಸ್ಥಿರಾಂಕ char16_t *)src, src_ಉದ್ದಳತೆ);
    }
    ಆದರೆ(p_Dest_ಉದ್ದಳತೆ){
       *p_Dest_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
    }

    u_terminateUChars(dest,dest_ಸಾಮರ್ಥ್ಯ,src_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);

    ಹಿಂತಿರುಗಿ dest;

#ಅಥವಾಆದರೆ defined U_WCHAR_IS_UTF32
    
    ಹಿಂತಿರುಗಿ u_strFromUTF32(dest, dest_ಸಾಮರ್ಥ್ಯ, p_Dest_ಉದ್ದಳತೆ,
                          (UChar32*)src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);

#ಇಲ್ಲದಿದ್ದರೆ

    ಹಿಂತಿರುಗಿ _strFromWCS(dest,dest_ಸಾಮರ್ಥ್ಯ,p_Dest_ಉದ್ದಳತೆ,src,src_ಉದ್ದಳತೆ,p_Error_ಸಂಕೇತ);  

#ಆದರೆ_ಅಂತ್ಯ

}

#ಆದರೆ_ಅಂತ್ಯ /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
