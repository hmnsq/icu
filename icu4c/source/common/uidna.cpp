// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
 *******************************************************************************
 *
 *   Copyright (C) 2003-2014, International Business Machines
 *   Corporation and others.  All Rights Reserved.
 *
 *******************************************************************************
 *   file name:  uidna.cpp
 *   encoding:   UTF-8
 *   tab size:   8 (not used)
 *   indentation:4
 *
 *   created on: 2003feb1
 *   created by: Ram Viswanadha
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_IDNA

#అన్తర్భావ "unicode/uidna.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/usprep.h"
#అన్తర్భావ "punycode.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
స్థిర స్థిరరాశి char16_t ACE_ఉపసర్గ[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#నిర్వచించు ACE_ఉపసర్గ_నిడివి 4

#నిర్వచించు MAX_LABEL_నిడివి 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#నిర్వచించు MAX_LABEL_బఫరు_కొలత 100

#నిర్వచించు MAX_DOMAIN_NAME_నిడివి 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#నిర్వచించు MAX_IDN_బఫరు_కొలత   MAX_DOMAIN_NAME_నిడివి+1

#నిర్వచించు క్రింది_CASE_DELTA 0x0020
#నిర్వచించు HYPHEN           0x002D
#నిర్వచించు నిండిన_STOP        0x002E
#నిర్వచించు CAPITAL_A        0x0041
#నిర్వచించు CAPITAL_Z        0x005A

అంతఃవరుస స్థిర char16_t
గమ్యం_ASCIILower(char16_t ch){
    ఒకవేళ(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        తిరిగి ch + క్రింది_CASE_DELTA;
    }
    తిరిగి ch;
}

అంతఃవరుస స్థిర UBool 
starts_With_ఉపసర్గ(స్థిరరాశి char16_t* src , int32_t src_నిడివి){
    ఒకవేళ(src_నిడివి < ACE_ఉపసర్గ_నిడివి){
        తిరిగి అబద్ధం;
    }

    కొరకు(int8_t i=0; i< ACE_ఉపసర్గ_నిడివి; i++){
        ఒకవేళ(గమ్యం_ASCIILower(src[i]) != ACE_ఉపసర్గ[i]){
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి నిజం;
}


అంతఃవరుస స్థిర int32_t
compareCaseInsensitiveASCII(స్థిరరాశి char16_t* s1, int32_t s1Len,
                            స్థిరరాశి char16_t* s2, int32_t s2Len){
    
    int32_t min_నిడివి;
    int32_t నిడివి_ఫలితం;

    // are we comparing different lengths?
    ఒకవేళ(s1Len != s2Len) {
        ఒకవేళ(s1Len < s2Len) {
            min_నిడివి = s1Len;
            నిడివి_ఫలితం = -1;
        } లేకపోతే {
            min_నిడివి = s2Len;
            నిడివి_ఫలితం = 1;
        }
    } లేకపోతే {
        // ok the lengths are equal
        min_నిడివి = s1Len;
        నిడివి_ఫలితం = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    కొరకు(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        ఒకవేళ(i == min_నిడివి) {
            తిరిగి నిడివి_ఫలితం;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        ఒకవేళ(c1!=c2) {
            rc = static_cast<int32_t>(గమ్యం_ASCIILower(c1)) - static_cast<int32_t>(గమ్యం_ASCIILower(c2));
            ఒకవేళ(rc!=0) {
                నిడివి_ఫలితం=rc;
                ఆపు;
            }
        }
    }
    తిరిగి నిడివి_ఫలితం;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
స్థిర అంతఃవరుస UBool isLabelSeparator(char16_t ch){
    ఎంపిక(ch){
        స్థితి 0x002e:
        స్థితి 0x3002:
        స్థితి 0xFF0E:
        స్థితి 0xFF61:
            తిరిగి నిజం;
        అప్రమేయ:
            తిరిగి అబద్ధం;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
స్థిర అంతఃవరుస int32_t
get_తదుపరి_Separator(char16_t *src, int32_t src_నిడివి,
                 char16_t **హద్దు, UBool *పూర్తి){
    ఒకవేళ(src_నిడివి == -1){
        int32_t i;
        కొరకు(i=0 ; ;i++){
            ఒకవేళ(src[i] == 0){
                *హద్దు = src + i; // point to null
                *పూర్తి = నిజం;
                తిరిగి i;
            }
            ఒకవేళ(isLabelSeparator(src[i])){
                *హద్దు = src + (i+1); // go past the delimiter
                తిరిగి i;
                
            }
        }
    }లేకపోతే{
        int32_t i;
        కొరకు(i=0;i<src_నిడివి;i++){
            ఒకవేళ(isLabelSeparator(src[i])){
                *హద్దు = src + (i+1); // go past the delimiter
                తిరిగి i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *హద్దు = src+src_నిడివి;
        *పూర్తి = నిజం;

        తిరిగి i;
    }
}
స్థిర అంతఃవరుస UBool isLDHChar(char16_t ch){
    // high runner case
    ఒకవేళ(ch>0x007A){
        తిరిగి అబద్ధం;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    ఒకవేళ( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        తిరిగి నిజం;
    }
    తిరిగి అబద్ధం;
}

స్థిర int32_t 
అంతర్గత_గమ్యం_ASCII(స్థిరరాశి char16_t* src, int32_t src_నిడివి,
                  char16_t* dest, int32_t dest_సామర్థ్యం,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_సంకేతం* స్థితి_2)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_దొంతర[MAX_LABEL_బఫరు_కొలత], b2_దొంతర[MAX_LABEL_బఫరు_కొలత];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_దొంతర, *b2 = b2_దొంతర;
    int32_t b1Len=0, b2Len, 
            b1_సామర్థ్యం = MAX_LABEL_బఫరు_కొలత, 
            b2_సామర్థ్యం = MAX_LABEL_బఫరు_కొలత ,
            req_నిడివి=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = శూన్యసూచిక;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = నిజం;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = నిజం; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_నియమాలు) != 0);

    int32_t failPos = -1;
    
    ఒకవేళ(src_నిడివి == -1){
        src_నిడివి = u_strlen(src);
    }
    
    ఒకవేళ(src_నిడివి > b1_సామర్థ్యం){
        b1 = static_cast<char16_t*>(uprv_malloc(src_నిడివి * U_SIZEOF_UCHAR));
        ఒకవేళ(b1==శూన్యసూచిక){
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు CLEANUP;
        }
        b1_సామర్థ్యం = src_నిడివి;
    }

    // step 1 
    కొరకు( j=0;j<src_నిడివి;j++){
        ఒకవేళ(src[j] > 0x7F){
            srcIsASCII = అబద్ధం;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    ఒకవేళ(srcIsASCII == అబద్ధం){
        
        // step 2    
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_నిడివి, b1, b1_సామర్థ్యం, namePrepOptions, parseError, &బఫరు_స్థితి);

        ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            ఒకవేళ(b1 != b1_దొంతర){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ఒకవేళ(b1==శూన్యసూచిక){
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు CLEANUP;
            }

            బఫరు_స్థితి = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_నిడివి, b1, b1Len, namePrepOptions, parseError, &బఫరు_స్థితి);
        }
        ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
            *స్థితి_2 = బఫరు_స్థితి;
        }
    }
    // error bail out
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        వెళ్ళు CLEANUP;
    }
    ఒకవేళ(b1Len == 0){
        *స్థితి_2 = U_IDNA_ZERO_నిడివి_LABEL_ERROR;
        వెళ్ళు CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = నిజం;
    కొరకు( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        ఒకవేళ(b1[j] > 0x7F){
            srcIsASCII = అబద్ధం;
        }లేకపోతే ఒకవేళ(isLDHChar(b1[j])==అబద్ధం){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = అబద్ధం;
            failPos = j;
        }
    }
    ఒకవేళ(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        ఒకవేళ( srcIsLDH == అబద్ధం /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *స్థితి_2 = U_IDNA_STD3_ASCII_నియమాలు_ERROR;

            /* populate the parseError struct */
            ఒకవేళ(srcIsLDH==అబద్ధం){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }లేకపోతే ఒకవేళ(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }లేకపోతే{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            వెళ్ళు CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    ఒకవేళ(srcIsASCII){
        ఒకవేళ(b1Len <= dest_సామర్థ్యం){
            u_memmove(dest, b1, b1Len);
            req_నిడివి = b1Len;
        }లేకపోతే{
            req_నిడివి = b1Len;
            వెళ్ళు CLEANUP;
        }
    }లేకపోతే{
        // step 5 : verify the sequence does not begin with ACE prefix
        ఒకవేళ(!starts_With_ఉపసర్గ(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
            b2Len = u_str_గమ్యం_Punycode(b1,b1Len,b2,b2_సామర్థ్యం,caseFlags,&బఫరు_స్థితి);

            ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                ఒకవేళ(b2 == శూన్యసూచిక){
                    *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    వెళ్ళు CLEANUP;
                }

                బఫరు_స్థితి = U_ZERO_ERROR; // reset error

                b2Len = u_str_గమ్యం_Punycode(b1,b1Len,b2,b2Len,caseFlags,&బఫరు_స్థితి);
            }
            //error bail out
            ఒకవేళ(U_FAILURE(బఫరు_స్థితి)){
                *స్థితి_2 = బఫరు_స్థితి;
                వెళ్ళు CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_నిడివి = b2Len+ACE_ఉపసర్గ_నిడివి;

            ఒకవేళ(req_నిడివి > dest_సామర్థ్యం){
                *స్థితి_2 = U_బఫరు_అధికప్రవాహం_ERROR;
                వెళ్ళు CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_ఉపసర్గ, ACE_ఉపసర్గ_నిడివి);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_ఉపసర్గ_నిడివి, b2, b2Len);

        }లేకపోతే{
            *స్థితి_2 = U_IDNA_ACE_ఉపసర్గ_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            వెళ్ళు CLEANUP;
        }
    }
    // step 8: verify the length of label
    ఒకవేళ(req_నిడివి > MAX_LABEL_నిడివి){
        *స్థితి_2 = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    ఒకవేళ(b1 != b1_దొంతర){
        uprv_free(b1);
    }
    ఒకవేళ(b2 != b2_దొంతర){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, స్థితి_2);
}

స్థిర int32_t
అంతర్గత_గమ్యం_Unicode(స్థిరరాశి char16_t* src, int32_t src_నిడివి,
                    char16_t* dest, int32_t dest_సామర్థ్యం,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_సంకేతం* స్థితి_2)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_దొంతర[MAX_LABEL_బఫరు_కొలత], b2_దొంతర[MAX_LABEL_బఫరు_కొలత], b3_దొంతర[MAX_LABEL_బఫరు_కొలత];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_దొంతర, *b2 = b2_దొంతర, *b1_ప్రధానసంఖ్య=శూన్యసూచిక, *b3=b3_దొంతర;
    int32_t b1Len = 0, b2Len, b1_ప్రధానసంఖ్య_Len, b3Len,
            b1_సామర్థ్యం = MAX_LABEL_బఫరు_కొలత, 
            b2_సామర్థ్యం = MAX_LABEL_బఫరు_కొలత,
            b3_సామర్థ్యం = MAX_LABEL_బఫరు_కొలత,
            req_నిడివి=0;

    UBool* caseFlags = శూన్యసూచిక;

    UBool srcIsASCII = నిజం;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    ఒకవేళ(src_నిడివి==-1){
        src_నిడివి = 0;
        కొరకు(;src[src_నిడివి]!=0;){
            ఒకవేళ(src[src_నిడివి]> 0x7f){
                srcIsASCII = అబద్ధం;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_నిడివి++;
        }
    }లేకపోతే ఒకవేళ(src_నిడివి > 0){
        కొరకు(int32_t j=0; j<src_నిడివి; j++){
            ఒకవేళ(src[j]> 0x7f){
                srcIsASCII = అబద్ధం;
                ఆపు;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }లేకపోతే{
        తిరిగి 0;
    }
    
    ఒకవేళ(srcIsASCII == అబద్ధం){
        // step 2: process the string
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_నిడివి, b1, b1_సామర్థ్యం, namePrepOptions, parseError, &బఫరు_స్థితి);
        ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ఒకవేళ(b1==శూన్యసూచిక){
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు CLEANUP;
            }

            బఫరు_స్థితి = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_నిడివి, b1, b1Len, namePrepOptions, parseError, &బఫరు_స్థితి);
        }
        //bail out on error
        ఒకవేళ(U_FAILURE(బఫరు_స్థితి)){
            *స్థితి_2 = బఫరు_స్థితి;
            వెళ్ళు CLEANUP;
        }
    }లేకపోతే{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_నిడివి;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    ఒకవేళ(starts_With_ఉపసర్గ(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_ప్రధానసంఖ్య = b1 + ACE_ఉపసర్గ_నిడివి;
        b1_ప్రధానసంఖ్య_Len  = b1Len - ACE_ఉపసర్గ_నిడివి;

        //step 5: Decode using punycode
        UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_ప్రధానసంఖ్య, b1_ప్రధానసంఖ్య_Len, b2, b2_సామర్థ్యం, caseFlags, &బఫరు_స్థితి);

        ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            ఒకవేళ(b2==శూన్యసూచిక){
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు CLEANUP;
            }

            బఫరు_స్థితి = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_ప్రధానసంఖ్య, b1_ప్రధానసంఖ్య_Len, b2, b2Len, caseFlags, &బఫరు_స్థితి);
        }


        //step 6:Apply toASCII
        b3Len = uidna_గమ్యం_ASCII(b2, b2Len, b3, b3_సామర్థ్యం, options, parseError, &బఫరు_స్థితి);

        ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            ఒకవేళ(b3==శూన్యసూచిక){
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                వెళ్ళు CLEANUP;
            }

            బఫరు_స్థితి = U_ZERO_ERROR; // reset error

            b3Len =  uidna_గమ్యం_ASCII(b2,b2Len,b3,b3Len,options,parseError,&బఫరు_స్థితి);

        }
        //bail out on error
        ఒకవేళ(U_FAILURE(బఫరు_స్థితి)){
            *స్థితి_2 = బఫరు_స్థితి;
            వెళ్ళు CLEANUP;
        }

        //step 7: verify
        ఒకవేళ(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *స్థితి_2 = U_IDNA_VERIFICATION_ERROR;
            వెళ్ళు CLEANUP;
        }

        //step 8: return output of step 5
        req_నిడివి = b2Len;
        ఒకవేళ(b2Len <= dest_సామర్థ్యం) {
            u_memmove(dest, b2, b2Len);
        }
    }
    లేకపోతే{
        // See the start of this if statement for why this is commented out.
        // verify that STD3 ASCII rules are satisfied
        /*if(useSTD3ASCIIRules == true){
            if( srcIsLDH == false // source contains some non-LDH characters
                || src[0] ==  HYPHEN || src[srcLength-1] == HYPHEN){
                *status = U_IDNA_STD3_ASCII_RULES_ERROR;

                // populate the parseError struct
                if(srcIsLDH==false){
                    // failPos is always set the index of failure
                    uprv_syntaxError(src,failPos, srcLength,parseError);
                }else if(src[0] == HYPHEN){
                    // fail position is 0 
                    uprv_syntaxError(src,0,srcLength,parseError);
                }else{
                    // the last index in the source is always length-1
                    uprv_syntaxError(src, (srcLength>0) ? srcLength-1 : srcLength, srcLength,parseError);
                }

                goto CLEANUP;
            }
        }*/
        // just return the source
        //copy the source to destination
        ఒకవేళ(src_నిడివి <= dest_సామర్థ్యం){
            u_memmove(dest, src, src_నిడివి);
        }
        req_నిడివి = src_నిడివి;
    }


CLEANUP:

    ఒకవేళ(b1 != b1_దొంతర && b1!=src){
        uprv_free(b1);
    }
    ఒకవేళ(b2 != b2_దొంతర){
        uprv_free(b2);
    }
    ఒకవేళ(b3 != b3_దొంతర){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        //copy the source to destination
        ఒకవేళ(dest && src_నిడివి <= dest_సామర్థ్యం){
            // srcLength should have already been set earlier.
            U_ASSERT(src_నిడివి >= 0);
            u_memmove(dest, src, src_నిడివి);
        }
        req_నిడివి = src_నిడివి;
        *స్థితి_2 = U_ZERO_ERROR;
    }

    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uidna_గమ్యం_ASCII(స్థిరరాశి char16_t* src, int32_t src_నిడివి,
              char16_t* dest, int32_t dest_సామర్థ్యం,
              int32_t options,
              UParseError* parseError,
              UError_సంకేతం* స్థితి_2){
    
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }
    ఒకవేళ((src==శూన్యసూచిక) || (src_నిడివి < -1) || (dest_సామర్థ్యం<0) || (!dest && dest_సామర్థ్యం > 0)){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, స్థితి_2);
    
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి -1;
    }
    
    int32_t retLen = అంతర్గత_గమ్యం_ASCII(src, src_నిడివి, dest, dest_సామర్థ్యం, options, nameprep, parseError, స్థితి_2);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    తిరిగి retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_గమ్యం_Unicode(స్థిరరాశి char16_t* src, int32_t src_నిడివి,
                char16_t* dest, int32_t dest_సామర్థ్యం,
                int32_t options,
                UParseError* parseError,
                UError_సంకేతం* స్థితి_2){

    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }
    ఒకవేళ( (src==శూన్యసూచిక) || (src_నిడివి < -1) || (dest_సామర్థ్యం<0) || (!dest && dest_సామర్థ్యం > 0)){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, స్థితి_2);
    
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి -1;
    }
    
    int32_t retLen = అంతర్గత_గమ్యం_Unicode(src, src_నిడివి, dest, dest_సామర్థ్యం, options, nameprep, parseError, స్థితి_2);

    usprep_close(nameprep);
    
    తిరిగి retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                   char16_t* dest, int32_t dest_సామర్థ్యం,
                   int32_t options,
                   UParseError *parseError,
                   UError_సంకేతం *స్థితి_2){

    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }
    ఒకవేళ((src==శూన్యసూచిక) || (src_నిడివి < -1) || (dest_సామర్థ్యం<0) || (!dest && dest_సామర్థ్యం > 0)){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    int32_t req_నిడివి = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, స్థితి_2);
    
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ప్రారంభం = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t మిగిలిన_Len = src_నిడివి;
    int32_t మిగిలిన_Dest_సామర్థ్యం = dest_సామర్థ్యం;
    int32_t labelLen = 0, label_Req_నిడివి = 0;
    UBool పూర్తి = అబద్ధం;


    కొరకు(;;){

        labelLen = get_తదుపరి_Separator(label_ప్రారంభం,మిగిలిన_Len, &delimiter,&పూర్తి);
        label_Req_నిడివి = 0;
        ఒకవేళ(!(labelLen==0 && పూర్తి)){// make sure this is not a root label separator.

            UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
            label_Req_నిడివి = అంతర్గత_గమ్యం_ASCII( label_ప్రారంభం, labelLen,
                                                currentDest, మిగిలిన_Dest_సామర్థ్యం,
                                                options, nameprep,
                                                parseError, &బఫరు_స్థితి);

            ఒకవేళ (బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR) {
                మిగిలిన_Dest_సామర్థ్యం = 0;
            } లేకపోతే ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
                *స్థితి_2 = బఫరు_స్థితి;
                ఆపు;
            }
        }

        req_నిడివి +=label_Req_నిడివి;
        // adjust the destination pointer
        ఒకవేళ(label_Req_నిడివి < మిగిలిన_Dest_సామర్థ్యం){
            currentDest = currentDest + label_Req_నిడివి;
            మిగిలిన_Dest_సామర్థ్యం -= label_Req_నిడివి;
        }లేకపోతే{
            // should never occur
            మిగిలిన_Dest_సామర్థ్యం = 0;
        }

        ఒకవేళ(పూర్తి){
            ఆపు;
        }

        // add the label separator
        ఒకవేళ(మిగిలిన_Dest_సామర్థ్యం > 0){
            *currentDest++ = నిండిన_STOP;
            మిగిలిన_Dest_సామర్థ్యం--;
        }
        req_నిడివి++;

        label_ప్రారంభం = delimiter;
        ఒకవేళ(మిగిలిన_Len >0 ){
            మిగిలిన_Len = (int32_t)(src_నిడివి - (delimiter - src));
        }

    }

    ఒకవేళ(req_నిడివి > MAX_DOMAIN_NAME_నిడివి){
        *స్థితి_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  స్థిరరాశి char16_t* src, int32_t src_నిడివి,
                     char16_t* dest, int32_t dest_సామర్థ్యం,
                     int32_t options,
                     UParseError* parseError,
                     UError_సంకేతం* స్థితి_2){
    
    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }
    ఒకవేళ((src==శూన్యసూచిక) || (src_నిడివి < -1) || (dest_సామర్థ్యం<0) || (!dest && dest_సామర్థ్యం > 0)){
        *స్థితి_2 = U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    int32_t req_నిడివి = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, స్థితి_2);
    
    ఒకవేళ(U_FAILURE(*స్థితి_2)){
        తిరిగి 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ప్రారంభం = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t మిగిలిన_Len = src_నిడివి;
    int32_t మిగిలిన_Dest_సామర్థ్యం = dest_సామర్థ్యం;
    int32_t labelLen = 0, label_Req_నిడివి = 0;
    UBool పూర్తి = అబద్ధం;

    కొరకు(;;){

        labelLen = get_తదుపరి_Separator(label_ప్రారంభం,మిగిలిన_Len, &delimiter,&పూర్తి);
        
        // The RFC states that 
        // <quote>
        // ToUnicode never fails. If any step fails, then the original input
        // is returned immediately in that step.
        // </quote>
        // _internal_toUnicode will copy the label.
        /*if(labelLen==0 && done==false){ 
            *status = U_IDNA_ZERO_LENGTH_LABEL_ERROR;
            break;
        }*/
        
        label_Req_నిడివి = అంతర్గత_గమ్యం_Unicode(label_ప్రారంభం, labelLen, 
                                             currentDest, మిగిలిన_Dest_సామర్థ్యం, 
                                             options, nameprep, 
                                             parseError, స్థితి_2);

        ఒకవేళ(*స్థితి_2 == U_బఫరు_అధికప్రవాహం_ERROR){
            *స్థితి_2 = U_ZERO_ERROR; // reset error
            మిగిలిన_Dest_సామర్థ్యం = 0;
        }

        ఒకవేళ(U_FAILURE(*స్థితి_2)){
            ఆపు;
        }
        
        req_నిడివి +=label_Req_నిడివి;
        // adjust the destination pointer
        ఒకవేళ(label_Req_నిడివి < మిగిలిన_Dest_సామర్థ్యం){
            currentDest = currentDest + label_Req_నిడివి;
            మిగిలిన_Dest_సామర్థ్యం -= label_Req_నిడివి;
        }లేకపోతే{
            // should never occur
            మిగిలిన_Dest_సామర్థ్యం = 0;
        }

        ఒకవేళ(పూర్తి){
            ఆపు;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        ఒకవేళ(మిగిలిన_Dest_సామర్థ్యం > 0){
            *currentDest++ = *(label_ప్రారంభం + labelLen);
            మిగిలిన_Dest_సామర్థ్యం--;
        }
        req_నిడివి++;

        label_ప్రారంభం = delimiter;
        ఒకవేళ(మిగిలిన_Len >0 ){
            మిగిలిన_Len = (int32_t)(src_నిడివి - (delimiter - src));
        }

    }

    ఒకవేళ(req_నిడివి > MAX_DOMAIN_NAME_నిడివి){
        *స్థితి_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, req_నిడివి, స్థితి_2);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  స్థిరరాశి char16_t *s1, int32_t length1,
                స్థిరరాశి char16_t *s2, int32_t length2,
                int32_t options,
                UError_సంకేతం* స్థితి_2){

    ఒకవేళ(స్థితి_2 == శూన్యసూచిక || U_FAILURE(*స్థితి_2)){
        తిరిగి -1;
    }

    char16_t b1_దొంతర[MAX_IDN_బఫరు_కొలత], b2_దొంతర[MAX_IDN_బఫరు_కొలత];
    char16_t *b1 = b1_దొంతర, *b2 = b2_దొంతర;
    int32_t b1Len, b2Len, b1_సామర్థ్యం = MAX_IDN_బఫరు_కొలత, b2_సామర్థ్యం = MAX_IDN_బఫరు_కొలత;
    int32_t ఫలితం=-1;
    
    UParseError parseError;

    UError_సంకేతం బఫరు_స్థితి = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_సామర్థ్యం, options, &parseError, &బఫరు_స్థితి);
    ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        ఒకవేళ(b1==శూన్యసూచిక){
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు CLEANUP;
        }

        బఫరు_స్థితి = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &బఫరు_స్థితి);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_సామర్థ్యం, options, &parseError, &బఫరు_స్థితి);
    ఒకవేళ(బఫరు_స్థితి == U_బఫరు_అధికప్రవాహం_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        ఒకవేళ(b2==శూన్యసూచిక){
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            వెళ్ళు CLEANUP;
        }

        బఫరు_స్థితి = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &బఫరు_స్థితి);
    }

    ఒకవేళ (U_FAILURE(బఫరు_స్థితి)) {
        *స్థితి_2 = బఫరు_స్థితి;
        వెళ్ళు CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    ఫలితం = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    ఒకవేళ(b1 != b1_దొంతర){
        uprv_free(b1);
    }

    ఒకవేళ(b2 != b2_దొంతర){
        uprv_free(b2);
    }

    తిరిగి ఫలితం;
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_IDNA */
