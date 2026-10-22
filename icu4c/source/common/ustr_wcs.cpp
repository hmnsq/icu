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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "cwchar.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "ustr_cnv.h"

#ఒకవేళ defined(U_WCHAR_IS_UTF16) || defined(U_WCHAR_IS_UTF32) || !UCONFIG_NO_CONVERSION

#నిర్వచించు దొంతర_బఫరు_సామర్థ్యం 1000
#నిర్వచించు బఫరు_సామర్థ్యం_MULTIPLIER 2

#ఒకవేళ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
// TODO: We should use CharString for char buffers and UnicodeString for char16_t buffers.
// Then we could change this to work only with wchar_t buffers.
స్థిర అంతఃవరుస UBool 
u_grow_Any_బఫరు_From_Static(శూన్య *context,
                       శూన్య **p_బఫరు, int32_t *p_సామర్థ్యం, int32_t req_సామర్థ్యం,
                       int32_t length, int32_t size) {
    // Use char* not void* to avoid the compiler's strict-aliasing assumptions
    // and related warnings.
    అక్షరం *new_బఫరు=(అక్షరం *)uprv_malloc(req_సామర్థ్యం*size);
    ఒకవేళ(new_బఫరు!=శూన్యసూచిక) {
        ఒకవేళ(length>0) {
            uprv_memcpy(new_బఫరు, *p_బఫరు, (size_t)length*size);
        }
        *p_సామర్థ్యం=req_సామర్థ్యం;
    } లేకపోతే {
        *p_సామర్థ్యం=0;
    }

    /* release the old pBuffer if it was not statically allocated */
    ఒకవేళ(*p_బఫరు!=(అక్షరం *)context) {
        uprv_free(*p_బఫరు);
    }

    *p_బఫరు=new_బఫరు;
    తిరిగి new_బఫరు!=శూన్యసూచిక;
}

/* helper function */
స్థిర wchar_t* 
str_గమ్యం_WCS(wchar_t *dest, 
           int32_t dest_సామర్థ్యం,
           int32_t *p_Dest_నిడివి,
           స్థిరరాశి char16_t *src,
           int32_t src_నిడివి,
           UError_సంకేతం *p_Error_సంకేతం){

    అక్షరం దొంతర_బఫరు [దొంతర_బఫరు_సామర్థ్యం];
    అక్షరం* తాత్కాలిక_Buf = దొంతర_బఫరు;
    int32_t తాత్కాలిక_Buf_సామర్థ్యం = దొంతర_బఫరు_సామర్థ్యం;
    అక్షరం* తాత్కాలిక_Buf_హద్దు = దొంతర_బఫరు + తాత్కాలిక_Buf_సామర్థ్యం;
    UConverter* conv = శూన్యసూచిక;
    అక్షరం* saveBuf = తాత్కాలిక_Buf;
    wchar_t* int_లక్ష్యం=శూన్యసూచిక;
    int32_t int_లక్ష్యం_సామర్థ్యం=0;
    పూర్ణాంకం count=0,retVal=0;
    
    స్థిరరాశి char16_t *p_Src_హద్దు =శూన్యసూచిక;
    స్థిరరాశి char16_t *pSrc = src;

    conv = u_getDefaultConverter(p_Error_సంకేతం);
    
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
    
    ఒకవేళ(src_నిడివి == -1){
        src_నిడివి = u_strlen(pSrc);
    }
    
    p_Src_హద్దు = pSrc + src_నిడివి;

    కొరకు(;;) {
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;

        /* convert to chars using default converter */
        ucnv_fromUnicode(conv,&తాత్కాలిక_Buf,తాత్కాలిక_Buf_హద్దు,&pSrc,p_Src_హద్దు,శూన్యసూచిక,(UBool)(pSrc==p_Src_హద్దు),&బఫరు_స్థితి);
        count =(తాత్కాలిక_Buf - saveBuf);
        
        /* This should rarely occur */
        ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR){
            తాత్కాలిక_Buf = saveBuf;
            
            /* we don't have enough room on the stack grow the buffer */
            int32_t new_సామర్థ్యం = 2 * src_నిడివి;
            ఒకవేళ(new_సామర్థ్యం <= తాత్కాలిక_Buf_సామర్థ్యం) {
                new_సామర్థ్యం = బఫరు_సామర్థ్యం_MULTIPLIER * తాత్కాలిక_Buf_సామర్థ్యం;
            }
            ఒకవేళ(!u_grow_Any_బఫరు_From_Static(దొంతర_బఫరు,(శూన్య**) &తాత్కాలిక_Buf, &తాత్కాలిక_Buf_సామర్థ్యం,
                    new_సామర్థ్యం, count, 1)) {
                వెళ్ళు cleanup;
            }
          
           saveBuf = తాత్కాలిక_Buf;
           తాత్కాలిక_Buf_హద్దు = తాత్కాలిక_Buf + తాత్కాలిక_Buf_సామర్థ్యం;
           తాత్కాలిక_Buf = తాత్కాలిక_Buf + count;
        } లేకపోతే {
            ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
                *p_Error_సంకేతం = బఫరు_స్థితి;
                వెళ్ళు cleanup;
            }
            ఆపు;
        }
    }

    /* done with conversion null terminate the char buffer */
    ఒకవేళ(count>=తాత్కాలిక_Buf_సామర్థ్యం){
        తాత్కాలిక_Buf = saveBuf;
        /* we don't have enough room on the stack grow the buffer */
        ఒకవేళ(!u_grow_Any_బఫరు_From_Static(దొంతర_బఫరు,(శూన్య**) &తాత్కాలిక_Buf, &తాత్కాలిక_Buf_సామర్థ్యం, 
                count+1, count, 1)) {
            వెళ్ళు cleanup;
        }              
       saveBuf = తాత్కాలిక_Buf;
    }
    
    saveBuf[count]=0;
      

    /* allocate more space than required 
     * here we assume that every char requires 
     * no more than 2 wchar_ts
     */
    int_లక్ష్యం_సామర్థ్యం =  (count * బఫరు_సామర్థ్యం_MULTIPLIER + 1) /*for null termination */;
    int_లక్ష్యం = (wchar_t*)uprv_malloc( int_లక్ష్యం_సామర్థ్యం * పరిమాణం(wchar_t) );

    ఒకవేళ(int_లక్ష్యం){

        int32_t nulLen = 0;
        int32_t మిగిలిన = int_లక్ష్యం_సామర్థ్యం;
        wchar_t* p_Int_లక్ష్యం=int_లక్ష్యం;
        తాత్కాలిక_Buf = saveBuf;
        
        /* now convert the mbs to wcs */
        కొరకు(;;){
            
            /* we can call the system API since we are sure that
             * there is atleast 1 null in the input
             */
            retVal = uprv_mbstowcs(p_Int_లక్ష్యం,(తాత్కాలిక_Buf+nulLen),మిగిలిన);
            
            ఒకవేళ(retVal==-1){
                *p_Error_సంకేతం = U_INVALID_CHAR_దొరికింది;
                ఆపు;
            }లేకపోతే ఒకవేళ(retVal== మిగిలిన){/* should never occur */
                పూర్ణాంకం అంకె_Written = (p_Int_లక్ష్యం-int_లక్ష్యం);
                u_grow_Any_బఫరు_From_Static(శూన్యసూచిక,(శూన్య**) &int_లక్ష్యం,
                                          &int_లక్ష్యం_సామర్థ్యం,
                                          int_లక్ష్యం_సామర్థ్యం * బఫరు_సామర్థ్యం_MULTIPLIER,
                                          అంకె_Written,
                                          పరిమాణం(wchar_t));
                p_Int_లక్ష్యం = int_లక్ష్యం;
                మిగిలిన=int_లక్ష్యం_సామర్థ్యం;

                ఒకవేళ(nulLen!=count){ /*there are embedded nulls*/
                    p_Int_లక్ష్యం+=అంకె_Written;
                    మిగిలిన-=అంకె_Written;
                }

            }లేకపోతే{
                int32_t nulVal;
                /*scan for nulls */
                /* we donot check for limit since tempBuf is null terminated */
                అయితే(తాత్కాలిక_Buf[nulLen++] != 0){
                }
                nulVal = (nulLen < src_నిడివి) ? 1 : 0; 
                p_Int_లక్ష్యం = p_Int_లక్ష్యం + retVal+nulVal;
                మిగిలిన -=(retVal+nulVal);
            
                /* check if we have reached the source limit*/
                ఒకవేళ(nulLen>=(count)){
                    ఆపు;
                }
            }
        }
        count = (int32_t)(p_Int_లక్ష్యం-int_లక్ష్యం);
       
        ఒకవేళ(0 < count && count <= dest_సామర్థ్యం){
            uprv_memcpy(dest, int_లక్ష్యం, (size_t)count*పరిమాణం(wchar_t));
        }  

        ఒకవేళ(p_Dest_నిడివి){
            *p_Dest_నిడివి = count;
        }

        /* free the allocated memory */
        uprv_free(int_లక్ష్యం);

    }లేకపోతే{
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
cleanup:
    /* are we still using stack buffer */
    ఒకవేళ(దొంతర_బఫరు != saveBuf){
        uprv_free(saveBuf);
    }
    u_terminateWChars(dest,dest_సామర్థ్యం,count,p_Error_సంకేతం);

    u_releaseDefaultConverter(conv);

    తిరిగి dest;
}
#అయితే_ముగింపు

U_CAPI wchar_t* U_EXPORT2
u_str_గమ్యం_WCS(wchar_t* dest U_LIFETIME_BOUND,
           int32_t dest_సామర్థ్యం,
           int32_t* p_Dest_నిడివి,
           స్థిరరాశి char16_t* src,
           int32_t src_నిడివి,
           UError_సంకేతం* p_Error_సంకేతం) {

    /* args check */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }
        
    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }
    
#నిర్వచితమైతే U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ఒకవేళ(src_నిడివి == -1){
        src_నిడివి = u_strlen(src);
    }
    ఒకవేళ(0 < src_నిడివి && src_నిడివి <= dest_సామర్థ్యం){
        u_memcpy((char16_t *)dest, src, src_నిడివి);
    }
    ఒకవేళ(p_Dest_నిడివి){
       *p_Dest_నిడివి = src_నిడివి;
    }

    u_terminateUChars((char16_t *)dest,dest_సామర్థ్యం,src_నిడివి,p_Error_సంకేతం);

    తిరిగి dest;

#లేదావేళ defined U_WCHAR_IS_UTF32
    
    తిరిగి (wchar_t*)u_str_గమ్యం_UTF32((UChar32*)dest, dest_సామర్థ్యం, p_Dest_నిడివి,
                                  src, src_నిడివి, p_Error_సంకేతం);

#లేకపోతే
    
    తిరిగి str_గమ్యం_WCS(dest,dest_సామర్థ్యం,p_Dest_నిడివి,src,src_నిడివి, p_Error_సంకేతం);
    
#అయితే_ముగింపు

}

#ఒకవేళ !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32)
/* helper function */
స్థిర char16_t*
_strFromWCS( char16_t   *dest,
             int32_t dest_సామర్థ్యం, 
             int32_t *p_Dest_నిడివి,
             స్థిరరాశి wchar_t *src,
             int32_t src_నిడివి,
             UError_సంకేతం *p_Error_సంకేతం)
{
    int32_t retVal =0, count =0 ;
    UConverter* conv = శూన్యసూచిక;
    char16_t* p_లక్ష్యం = శూన్యసూచిక;
    char16_t* p_లక్ష్యం_హద్దు = శూన్యసూచిక;
    char16_t* లక్ష్యం = శూన్యసూచిక;
    
    char16_t u_దొంతర [దొంతర_బఫరు_సామర్థ్యం];

    wchar_t w_దొంతర[దొంతర_బఫరు_సామర్థ్యం];
    wchar_t* pWStack = w_దొంతర;


    అక్షరం c_దొంతర[దొంతర_బఫరు_సామర్థ్యం];
    int32_t c_దొంతర_Cap = దొంతర_బఫరు_సామర్థ్యం;
    అక్షరం* pCSrc=c_దొంతర;
    అక్షరం* pCSave=pCSrc;
    అక్షరం* p_CSrc_హద్దు=శూన్యసూచిక;

    స్థిరరాశి wchar_t* pSrc = src;
    స్థిరరాశి wchar_t* p_Src_హద్దు = శూన్యసూచిక;

    ఒకవేళ(src_నిడివి ==-1){
        /* if the wchar_t source is null terminated we can safely
         * assume that there are no embedded nulls, this is a fast
         * path for null terminated strings.
         */
        కొరకు(;;){
            /* convert wchars  to chars */
            retVal = uprv_wcstombs(pCSrc,src, c_దొంతర_Cap);
    
            ఒకవేళ(retVal == -1){
                *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
                వెళ్ళు cleanup;
            }లేకపోతే ఒకవేళ(retVal >= (c_దొంతర_Cap-1)){
                /* Should rarely occur */
                u_grow_Any_బఫరు_From_Static(c_దొంతర,(శూన్య**)&pCSrc,&c_దొంతర_Cap,
                    c_దొంతర_Cap * బఫరు_సామర్థ్యం_MULTIPLIER, 0, పరిమాణం(అక్షరం));
                pCSave = pCSrc;
            }లేకపోతే{
                /* converted every thing */
                pCSrc = pCSrc+retVal;
                ఆపు;
            }
        }
        
    }లేకపోతే{
        /* here the source is not null terminated 
         * so it may have nulls embedded and we need to
         * do some extra processing 
         */
        int32_t మిగిలిన =c_దొంతర_Cap;
        
        p_Src_హద్దు = src + src_నిడివి;

        కొరకు(;;){
            int32_t nulLen = 0;

            /* find nulls in the string */
            అయితే(nulLen<src_నిడివి && pSrc[nulLen++]!=0){
            }

            ఒకవేళ((pSrc+nulLen) < p_Src_హద్దు){
                /* check if we have enough room in pCSrc */
                ఒకవేళ(మిగిలిన < (nulLen * MB_CUR_MAX)){
                    /* should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_బఫరు_From_Static(c_దొంతర,(శూన్య**)&pCSrc,&c_దొంతర_Cap,
                           బఫరు_సామర్థ్యం_MULTIPLIER*c_దొంతర_Cap+(nulLen*MB_CUR_MAX),len,పరిమాణం(అక్షరం));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    మిగిలిన = c_దొంతర_Cap-(pCSrc - pCSave);
                }

                /* we have found a null  so convert the 
                 * chunk from beginning of non-null char to null
                 */
                retVal = uprv_wcstombs(pCSrc,pSrc,మిగిలిన);

                ఒకవేళ(retVal==-1){
                    /* an error occurred bail out */
                    *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు cleanup;
                }

                pCSrc += retVal+1 /* already null terminated */;

                pSrc += nulLen; /* skip past the null */
                src_నిడివి-=nulLen; /* decrement the srcLength */
                మిగిలిన -= (pCSrc-pCSave);


            }లేకపోతే{
                /* the source is not null terminated and we are 
                 * end of source so we copy the source to a temp buffer
                 * null terminate it and convert wchar_ts to chars
                 */
                ఒకవేళ(nulLen >= దొంతర_బఫరు_సామర్థ్యం){
                    /* Should rarely occur */
                    /* allocate new buffer buffer */
                    pWStack =(wchar_t*) uprv_malloc(పరిమాణం(wchar_t) * (nulLen + 1));
                    ఒకవేళ(pWStack==శూన్యసూచిక){
                        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        వెళ్ళు cleanup;
                    }
                }
                ఒకవేళ(nulLen>0){
                    /* copy the contents to tempStack */
                    uprv_memcpy(pWStack, pSrc, (size_t)nulLen*పరిమాణం(wchar_t));
                }
            
                /* null terminate the tempBuffer */
                pWStack[nulLen] =0 ;
            
                ఒకవేళ(మిగిలిన < (nulLen * MB_CUR_MAX)){
                    /* Should rarely occur */
                    int32_t len = (pCSrc-pCSave);
                    pCSrc = pCSave;
                    /* we do not have enough room so grow the buffer*/
                    u_grow_Any_బఫరు_From_Static(c_దొంతర,(శూన్య**)&pCSrc,&c_దొంతర_Cap,
                           c_దొంతర_Cap+(nulLen*MB_CUR_MAX),len,పరిమాణం(అక్షరం));

                    pCSave = pCSrc;
                    pCSrc = pCSave+len;
                    మిగిలిన = c_దొంతర_Cap-(pCSrc - pCSave);
                }
                /* convert to chars */
                retVal = uprv_wcstombs(pCSrc,pWStack,మిగిలిన);
            
                pCSrc += retVal;
                pSrc  += nulLen;
                src_నిడివి-=nulLen; /* decrement the srcLength */
                ఆపు;
            }
        }
    }

    /* OK..now we have converted from wchar_ts to chars now 
     * convert chars to UChars 
     */
    p_CSrc_హద్దు = pCSrc;
    pCSrc = pCSave;
    p_లక్ష్యం = లక్ష్యం= dest;
    p_లక్ష్యం_హద్దు = dest + dest_సామర్థ్యం;    
    
    conv= u_getDefaultConverter(p_Error_సంకేతం);
    
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)|| conv==శూన్యసూచిక){
        వెళ్ళు cleanup;
    }
    
    కొరకు(;;) {
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;

        /* convert to stack buffer*/
        ucnv_గమ్యం_Unicode(conv,&p_లక్ష్యం,p_లక్ష్యం_హద్దు,(స్థిరరాశి అక్షరం**)&pCSrc,p_CSrc_హద్దు,శూన్యసూచిక,(UBool)(pCSrc==p_CSrc_హద్దు),&బఫరు_స్థితి);

        /* increment count to number written to stack */
        count+= p_లక్ష్యం - లక్ష్యం;

        ఒకవేళ(బఫరు_స్థితి==U_బఫరు_అధికప్రవాహం_ERROR){
            లక్ష్యం = u_దొంతర;
            p_లక్ష్యం = u_దొంతర;
            p_లక్ష్యం_హద్దు = u_దొంతర + దొంతర_బఫరు_సామర్థ్యం;
        } లేకపోతే {
            ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
                *p_Error_సంకేతం = బఫరు_స్థితి;
            }
            ఆపు;
        }
        
    }
    
    ఒకవేళ(p_Dest_నిడివి){
        *p_Dest_నిడివి =count;
    }

    u_terminateUChars(dest,dest_సామర్థ్యం,count,p_Error_సంకేతం);
    
cleanup:
 
    ఒకవేళ(c_దొంతర != pCSave){
        uprv_free(pCSave);
    }

    ఒకవేళ(w_దొంతర != pWStack){
        uprv_free(pWStack);
    }
    
    u_releaseDefaultConverter(conv);

    తిరిగి dest;
}
#అయితే_ముగింపు

U_CAPI char16_t* U_EXPORT2
u_strFromWCS(char16_t* dest U_LIFETIME_BOUND,
             int32_t dest_సామర్థ్యం,
             int32_t* p_Dest_నిడివి,
             స్థిరరాశి wchar_t* src,
             int32_t src_నిడివి,
             UError_సంకేతం* p_Error_సంకేతం)
{

    /* args check */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)){
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ( (src==శూన్యసూచిక && src_నిడివి!=0) || src_నిడివి < -1 ||
        (dest_సామర్థ్యం<0) || (dest == శూన్యసూచిక && dest_సామర్థ్యం > 0)
    ) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి శూన్యసూచిక;
    }

#నిర్వచితమైతే U_WCHAR_IS_UTF16
    /* wchar_t is UTF-16 just do a memcpy */
    ఒకవేళ(src_నిడివి == -1){
        src_నిడివి = u_strlen((స్థిరరాశి char16_t *)src);
    }
    ఒకవేళ(0 < src_నిడివి && src_నిడివి <= dest_సామర్థ్యం){
        u_memcpy(dest, (స్థిరరాశి char16_t *)src, src_నిడివి);
    }
    ఒకవేళ(p_Dest_నిడివి){
       *p_Dest_నిడివి = src_నిడివి;
    }

    u_terminateUChars(dest,dest_సామర్థ్యం,src_నిడివి,p_Error_సంకేతం);

    తిరిగి dest;

#లేదావేళ defined U_WCHAR_IS_UTF32
    
    తిరిగి u_strFromUTF32(dest, dest_సామర్థ్యం, p_Dest_నిడివి,
                          (UChar32*)src, src_నిడివి, p_Error_సంకేతం);

#లేకపోతే

    తిరిగి _strFromWCS(dest,dest_సామర్థ్యం,p_Dest_నిడివి,src,src_నిడివి,p_Error_సంకేతం);  

#అయితే_ముగింపు

}

#అయితే_ముగింపు /* #if !defined(U_WCHAR_IS_UTF16) && !defined(U_WCHAR_IS_UTF32) && !UCONFIG_NO_CONVERSION */
