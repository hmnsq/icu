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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cwchar.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "ustr_cnv.h"

#यदि defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#परिभाषय राशिः_बफर_क्षमता 1000
#परिभाषय बफर_क्षमता_MULTIPLIER 2

#यदि !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
स्थिर अन्तःस्थ UBool 
u_grow_Any_बफर_From_Static(शून्य *context,
                       शून्य **p_बफर, int32_t *p_क्षमता, int32_t req_क्षमता,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    वर्ण *new_बफर=(वर्ण *)uprv_malloc(req_क्षमता*size);
    यदि(new_बफर!=शून्यसूचकः) {
        यदि(length>0) {
            uprv_memcpy(new_बफर, *p_बफर, (size_t)length*size);
        }
        *p_क्षमता=req_क्षमता;
    } अन्यथा {
        *p_क्षमता=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    यदि(*p_बफर!=(वर्ण *)context) {
        uprv_free(*p_बफर);
    }

    *p_बफर=new_बफर;
    प्रत्यागम new_बफर!=शून्यसूचकः;
}

/* helper function */
स्थिर wchar_t* 
str_गन्तव्यम्_WCS(wchar_t *dest, 
           int32_t dest_क्षमता,
           int32_t *p_Dest_दीर्घता,
           ध्रुव char16_t *src,
           int32_t src_दीर्घता,
           UError_संहिता *p_Error_संहिता){

    वर्ण राशिः_बफर [राशिः_बफर_क्षमता];
    वर्ण* अस्थायी_Buf = राशिः_बफर;
    int32_t अस्थायी_Buf_क्षमता = राशिः_बफर_क्षमता;
    वर्ण* अस्थायी_Buf_सीमा = राशिः_बफर + अस्थायी_Buf_क्षमता;
    UConverter* conv = शून्यसूचकः;
    वर्ण* saveBuf = अस्थायी_Buf;
    wchar_t* intTarget=शून्यसूचकः;
    int32_t int_Target_क्षमता=0;
    पूर्णाङ्क count=0,retVal=0;
    
    ध्रुव char16_t *p_Src_सीमा =शून्यसूचकः;
    ध्रुव char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_संहिता);
    
    यदि(U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
    
    यदि(src_दीर्घता == -1){
        src_दीर्घता = u_strlen(pSrc);
    }
    
    p_Src_सीमा = pSrc + src_दीर्घता;

    प्रति(;;) {
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&अस्थायी_Buf,अस्थायी_Buf_सीमा,&pSrc,p_Src_सीमा,शून्यसूचकः,(UBool)(pSrc==p_Src_सीमा),&बफर_स्थितिः);
        count =(अस्थायी_Buf - saveBuf);
        
        /* This should rarely occur */
        यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR){
            अस्थायी_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_क्षमता = 2 * src_दीर्घता;
            यदि(new_क्षमता <= अस्थायी_Buf_क्षमता) {
                new_क्षमता = बफर_क्षमता_MULTIPLIER * अस्थायी_Buf_क्षमता;
            }
            यदि(!u_grow_Any_बफर_From_Static(राशिः_बफर,(शून्य**) &अस्थायी_Buf, &अस्थायी_Buf_क्षमता,
                    new_क्षमता, count, 1)) {
                गच्छ cleanup;
            }
          
           saveBuf = अस्थायी_Buf;
           अस्थायी_Buf_सीमा = अस्थायी_Buf + अस्थायी_Buf_क्षमता;
           अस्थायी_Buf = अस्थायी_Buf + count;
        } अन्यथा {
            यदि (U_FAILURE(बफर_स्थितिः)) {
                *p_Error_संहिता = बफर_स्थितिः;
                गच्छ cleanup;
            }
            भङ्ग;
        }
    }

    /* done with conversion null terminate the char buffer */
    यदि(count>=अस्थायी_Buf_क्षमता){
        अस्थायी_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        यदि(!u_grow_Any_बफर_From_Static(राशिः_बफर,(शून्य**) &अस्थायी_Buf, &अस्थायी_Buf_क्षमता, 
                count+1, count, 1)) {
            गच्छ cleanup;
        }              
       saveBuf = अस्थायी_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_Target_क्षमता =  (count * बफर_क्षमता_MULTIPLIER + 1) /*for null termination */;
    intTarget = (wchar_t*)uprv_malloc( int_Target_क्षमता * परिमाण(wchar_t) );

    यदि(intTarget){

        int32_t nulLen = 0;
        int32_t remaining = int_Target_क्षमता;
        wchar_t* pIntTarget=intTarget;
        अस्थायी_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        प्रति(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(pIntTarget,(अस्थायी_Buf+nulLen),remaining);
            
            यदि(retVal==-1){
                *p_Error_संहिता = U_INVALID_CHAR_प्राप्तम्;
                भङ्ग;
            }अन्यथा यदि(retVal== remaining){/* should never occur */
                पूर्णाङ्क अङ्कः_Written = (pIntTarget-intTarget);
                u_grow_Any_बफर_From_Static(शून्यसूचकः,(शून्य**) &intTarget,
                                          &int_Target_क्षमता,
                                          int_Target_क्षमता * बफर_क्षमता_MULTIPLIER,
                                          अङ्कः_Written,
                                          परिमाण(wchar_t));
                pIntTarget = intTarget;
                remaining=int_Target_क्षमता;

                यदि(nulLen!=count){ /*there are embedded nulls*/
                    pIntTarget+=अङ्कः_Written;
                    remaining-=अङ्कः_Written;
                }

            }अन्यथा{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                यावत्(अस्थायी_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_दीर्घता) ? 1 : 0; 
                pIntTarget = pIntTarget + retVal+nulVal;
                remaining -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                यदि(nulLen>=(count)){
                    भङ्ग;
                }
            }
        }
        count = (int32_t)(pIntTarget-intTarget);
       
        यदि(0 < count && count <= dest_क्षमता){
            uprv_memcpy(dest, intTarget, (size_t)count*परिमाण(wchar_t));
        }  

        यदि(p_Dest_दीर्घता){
            *p_Dest_दीर्घता = count;
        }

        /* free the allocated memory */
        uprv_free(intTarget);

    }अन्यथा{
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    यदि(राशिः_बफर != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_क्षमता,count,p_Error_संहिता);

    u_releaseDefaultConverter(conv);

    प्रत्यागम dest;
}
#यदि_अन्तः

U_CAPI wchar_t* U_EXPORT2
u_str_गन्तव्यम्_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_क्षमता,
           int32_t* p_Dest_दीर्घता,
           ध्रुव char16_t* src,
           int32_t src_दीर्घता,
           UError_संहिता* p_Error_संहिता) {

    /* args check */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }
        
    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    
#यदि_परिभाषितम् U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    यदि(src_दीर्घता == -1){
        src_दीर्घता = u_strlen(src);
    }
    यदि(0 < src_दीर्घता && src_दीर्घता <= dest_क्षमता){
        u_memcpy((char16_t *)dest, src, src_दीर्घता);
    }
    यदि(p_Dest_दीर्घता){
       *p_Dest_दीर्घता = src_दीर्घता;
    }

    u_terminateUChars((char16_t *)dest,dest_क्षमता,src_दीर्घता,p_Error_संहिता);

    प्रत्यागम dest;

#नोचेद्यदि defined U_WCHAR_IS_UTF32
    
    प्रत्यागम (wchar_t*)u_str_गन्तव्यम्_UTF32((UChar32*)dest, dest_क्षमता, p_Dest_दीर्घता,
                                  src, src_दीर्घता, p_Error_संहिता);

#अन्यथा
    
    प्रत्यागम str_गन्तव्यम्_WCS(dest,dest_क्षमता,p_Dest_दीर्घता,src,src_दीर्घता, p_Error_संहिता);
    
#यदि_अन्तः

}

#यदि !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
स्थिर char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_क्षमता, 
             int32_t *p_Dest_दीर्घता,
             ध्रुव wchar_t *src,
             int32_t src_दीर्घता,
             UError_संहिता *p_Error_संहिता)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = शून्यसूचकः;
    char16_t* pTarget = शून्यसूचकः;
    char16_t* p_Target_सीमा = शून्यसूचकः;
    char16_t* target = शून्यसूचकः;
    
    char16_t u_राशिः [राशिः_बफर_क्षमता];

    wchar_t w_राशिः[राशिः_बफर_क्षमता];
    wchar_t* pWStack = w_राशिः;


    वर्ण c_राशिः[राशिः_बफर_क्षमता];
    int32_t c_राशिः_Cap = राशिः_बफर_क्षमता;
    वर्ण* pCSrc=c_राशिः;
    वर्ण* pCSave=pCSrc;
    वर्ण* p_CSrc_सीमा=शून्यसूचकः;

    ध्रुव wchar_t* pSrc = src;
    ध्रुव wchar_t* p_Src_सीमा = शून्यसूचकः;

    यदि(src_दीर्घता ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        प्रति(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_राशिः_Cap);
    
            यदि(retVal == -1){
                *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
                गच्छ cleanup;
            }अन्यथा यदि(retVal >= (c_राशिः_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_बफर_From_Static(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                    c_राशिः_Cap * बफर_क्षमता_MULTIPLIER, 0, परिमाण(वर्ण));
                pCSave = pCSrc;
            }अन्यथा{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                भङ्ग;
            }
        }
        
    }अन्यथा{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t remaining =c_राशिः_Cap;
        
        p_Src_सीमा = src + src_दीर्घता;

        प्रति(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            यावत्(nulLen<src_दीर्घता && pSrc[nulLen++]!=0){
            }

            यदि((pSrc+nulLen) < p_Src_सीमा){
                /* check if we have enough room in pCSrc */
                यदि(remaining < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_बफर_From_Static(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                           बफर_क्षमता_MULTIPLIER*c_राशिः_Cap+(nulLen*MB_CUR_MAX),len,परिमाण(वर्ण));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_राशिः_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,remaining);

                यदि(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_दीर्घता-=nulLen; /* decrement the srcLength */
                remaining -= (pCSrc-pCSave);


            }अन्यथा{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                यदि(nulLen >= राशिः_बफर_क्षमता){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(परिमाण(wchar_t) * (nulLen + 1));
                    यदि(pWStack==शून्यसूचकः){
                        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                        गच्छ cleanup;
                    }
                }
                यदि(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*परिमाण(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                यदि(remaining < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_बफर_From_Static(c_राशिः,(शून्य**)&pCSrc,&c_राशिः_Cap,
                           c_राशिः_Cap+(nulLen*MB_CUR_MAX),len,परिमाण(वर्ण));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    remaining = c_राशिः_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,remaining);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_दीर्घता-=nulLen; /* decrement the srcLength */
                भङ्ग;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_सीमा = pCSrc;
    pCSrc = pCSave;
    pTarget = target= dest;
    p_Target_सीमा = dest + dest_क्षमता;    
    
    conv= u_getDefaultConverter(p_Error_संहिता);
    
    यदि(U_FAILURE(*p_Error_संहिता)|| conv==शून्यसूचकः){
        गच्छ cleanup;
    }
    
    प्रति(;;) {
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_गन्तव्यम्_Unicode(conv,&pTarget,p_Target_सीमा,(ध्रुव वर्ण**)&pCSrc,p_CSrc_सीमा,शून्यसूचकः,(UBool)(pCSrc==p_CSrc_सीमा),&बफर_स्थितिः);

        /* increment count to number written to stack */
        count+= pTarget - target;

        यदि(बफर_स्थितिः==U_बफर_अतिप्रवाहः_ERROR){
            target = u_राशिः;
            pTarget = u_राशिः;
            p_Target_सीमा = u_राशिः + राशिः_बफर_क्षमता;
        } अन्यथा {
            यदि (U_FAILURE(बफर_स्थितिः)) {
                *p_Error_संहिता = बफर_स्थितिः;
            }
            भङ्ग;
        }
        
    }
    
    यदि(p_Dest_दीर्घता){
        *p_Dest_दीर्घता =count;
    }

    u_terminateUChars(dest,dest_क्षमता,count,p_Error_संहिता);
    
cleanup:
 
    यदि(c_राशिः != pCSave){
        uprv_free(pCSave);
    }

    यदि(w_राशिः != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    प्रत्यागम dest;
}
#यदि_अन्तः

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_क्षमता,
             int32_t* p_Dest_दीर्घता,
             ध्रुव wchar_t* src,
             int32_t src_दीर्घता,
             UError_संहिता* p_Error_संहिता)
{

    /* args check */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)){
        प्रत्यागम शून्यसूचकः;
    }

    यदि( (src==शून्यसूचकः && src_दीर्घता!=0) || src_दीर्घता < -1 ||
        (dest_क्षमता<0) || (dest == शून्यसूचकः && dest_क्षमता > 0)
    ) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

#यदि_परिभाषितम् U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    यदि(src_दीर्घता == -1){
        src_दीर्घता = u_strlen((ध्रुव char16_t *)src);
    }
    यदि(0 < src_दीर्घता && src_दीर्घता <= dest_क्षमता){
        u_memcpy(dest, (ध्रुव char16_t *)src, src_दीर्घता);
    }
    यदि(p_Dest_दीर्घता){
       *p_Dest_दीर्घता = src_दीर्घता;
    }

    u_terminateUChars(dest,dest_क्षमता,src_दीर्घता,p_Error_संहिता);

    प्रत्यागम dest;

#नोचेद्यदि defined U_WCHAR_IS_UTF32
    
    प्रत्यागम u_strFromUTF32(dest, dest_क्षमता, p_Dest_दीर्घता,
                          (UChar32*)src, src_दीर्घता, p_Error_संहिता);

#अन्यथा

    प्रत्यागम _strFromWCS(dest,dest_क्षमता,p_Dest_दीर्घता,src,src_दीर्घता,p_Error_संहिता);  

#यदि_अन्तः

}

#यदि_अन्तः /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
