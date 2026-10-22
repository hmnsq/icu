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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_IDNA

#अन्तर्भाव "unicode/uidna.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/usprep.h"
#अन्तर्भाव "punycode.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
स्थिर ध्रुव char16_t ACE_उपसर्गः[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#परिभाषय ACE_उपसर्गः_दीर्घता 4

#परिभाषय MAX_LABEL_दीर्घता 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#परिभाषय MAX_LABEL_बफर_आकार 100

#परिभाषय MAX_DOMAIN_NAME_दीर्घता 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#परिभाषय MAX_IDN_बफर_आकार   MAX_DOMAIN_NAME_दीर्घता+1

#परिभाषय अधस्तनम्_CASE_DELTA 0x0020
#परिभाषय HYPHEN           0x002D
#परिभाषय पूर्णम्_STOP        0x002E
#परिभाषय CAPITAL_A        0x0041
#परिभाषय CAPITAL_Z        0x005A

अन्तःस्थ स्थिर char16_t
गन्तव्यम्_ASCIILower(char16_t ch){
    यदि(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        प्रत्यागम ch + अधस्तनम्_CASE_DELTA;
    }
    प्रत्यागम ch;
}

अन्तःस्थ स्थिर UBool 
starts_With_उपसर्गः(ध्रुव char16_t* src , int32_t src_दीर्घता){
    यदि(src_दीर्घता < ACE_उपसर्गः_दीर्घता){
        प्रत्यागम असत्य;
    }

    प्रति(int8_t i=0; i< ACE_उपसर्गः_दीर्घता; i++){
        यदि(गन्तव्यम्_ASCIILower(src[i]) != ACE_उपसर्गः[i]){
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}


अन्तःस्थ स्थिर int32_t
compareCaseInsensitiveASCII(ध्रुव char16_t* s1, int32_t s1Len,
                            ध्रुव char16_t* s2, int32_t s2Len){
    
    int32_t min_दीर्घता;
    int32_t दीर्घता_परिणाम;

    // are we comparing different lengths?
    यदि(s1Len != s2Len) {
        यदि(s1Len < s2Len) {
            min_दीर्घता = s1Len;
            दीर्घता_परिणाम = -1;
        } अन्यथा {
            min_दीर्घता = s2Len;
            दीर्घता_परिणाम = 1;
        }
    } अन्यथा {
        // ok the lengths are equal
        min_दीर्घता = s1Len;
        दीर्घता_परिणाम = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    प्रति(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        यदि(i == min_दीर्घता) {
            प्रत्यागम दीर्घता_परिणाम;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        यदि(c1!=c2) {
            rc = static_cast<int32_t>(गन्तव्यम्_ASCIILower(c1)) - static_cast<int32_t>(गन्तव्यम्_ASCIILower(c2));
            यदि(rc!=0) {
                दीर्घता_परिणाम=rc;
                भङ्ग;
            }
        }
    }
    प्रत्यागम दीर्घता_परिणाम;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
स्थिर अन्तःस्थ UBool isLabelSeparator(char16_t ch){
    विकल्प(ch){
        पक्ष 0x002e:
        पक्ष 0x3002:
        पक्ष 0xFF0E:
        पक्ष 0xFF61:
            प्रत्यागम सत्य;
        स्वभाव:
            प्रत्यागम असत्य;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
स्थिर अन्तःस्थ int32_t
get_अग्र_Separator(char16_t *src, int32_t src_दीर्घता,
                 char16_t **सीमा, UBool *समाप्तम्){
    यदि(src_दीर्घता == -1){
        int32_t i;
        प्रति(i=0 ; ;i++){
            यदि(src[i] == 0){
                *सीमा = src + i; // point to null
                *समाप्तम् = सत्य;
                प्रत्यागम i;
            }
            यदि(isLabelSeparator(src[i])){
                *सीमा = src + (i+1); // go past the delimiter
                प्रत्यागम i;
                
            }
        }
    }अन्यथा{
        int32_t i;
        प्रति(i=0;i<src_दीर्घता;i++){
            यदि(isLabelSeparator(src[i])){
                *सीमा = src + (i+1); // go past the delimiter
                प्रत्यागम i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *सीमा = src+src_दीर्घता;
        *समाप्तम् = सत्य;

        प्रत्यागम i;
    }
}
स्थिर अन्तःस्थ UBool isLDHChar(char16_t ch){
    // high runner case
    यदि(ch>0x007A){
        प्रत्यागम असत्य;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    यदि( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        प्रत्यागम सत्य;
    }
    प्रत्यागम असत्य;
}

स्थिर int32_t 
आन्तरिकम्_गन्तव्यम्_ASCII(ध्रुव char16_t* src, int32_t src_दीर्घता,
                  char16_t* dest, int32_t dest_क्षमता,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_संहिता* स्थितिः)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_राशिः[MAX_LABEL_बफर_आकार], b2_राशिः[MAX_LABEL_बफर_आकार];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_राशिः, *b2 = b2_राशिः;
    int32_t b1Len=0, b2Len, 
            b1_क्षमता = MAX_LABEL_बफर_आकार, 
            b2_क्षमता = MAX_LABEL_बफर_आकार ,
            req_दीर्घता=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = शून्यसूचकः;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = सत्य;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = सत्य; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_नियमाः) != 0);

    int32_t failPos = -1;
    
    यदि(src_दीर्घता == -1){
        src_दीर्घता = u_strlen(src);
    }
    
    यदि(src_दीर्घता > b1_क्षमता){
        b1 = static_cast<char16_t*>(uprv_malloc(src_दीर्घता * U_SIZEOF_UCHAR));
        यदि(b1==शून्यसूचकः){
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ CLEANUP;
        }
        b1_क्षमता = src_दीर्घता;
    }

    // step 1 
    प्रति( j=0;j<src_दीर्घता;j++){
        यदि(src[j] > 0x7F){
            srcIsASCII = असत्य;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    यदि(srcIsASCII == असत्य){
        
        // step 2    
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_दीर्घता, b1, b1_क्षमता, namePrepOptions, parseError, &बफर_स्थितिः);

        यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            यदि(b1 != b1_राशिः){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            यदि(b1==शून्यसूचकः){
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ CLEANUP;
            }

            बफर_स्थितिः = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_दीर्घता, b1, b1Len, namePrepOptions, parseError, &बफर_स्थितिः);
        }
        यदि (U_FAILURE(बफर_स्थितिः)) {
            *स्थितिः = बफर_स्थितिः;
        }
    }
    // error bail out
    यदि(U_FAILURE(*स्थितिः)){
        गच्छ CLEANUP;
    }
    यदि(b1Len == 0){
        *स्थितिः = U_IDNA_ZERO_दीर्घता_LABEL_ERROR;
        गच्छ CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = सत्य;
    प्रति( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        यदि(b1[j] > 0x7F){
            srcIsASCII = असत्य;
        }अन्यथा यदि(isLDHChar(b1[j])==असत्य){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = असत्य;
            failPos = j;
        }
    }
    यदि(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        यदि( srcIsLDH == असत्य /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *स्थितिः = U_IDNA_STD3_ASCII_नियमाः_ERROR;

            /* populate the parseError struct */
            यदि(srcIsLDH==असत्य){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }अन्यथा यदि(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }अन्यथा{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            गच्छ CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    यदि(srcIsASCII){
        यदि(b1Len <= dest_क्षमता){
            u_memmove(dest, b1, b1Len);
            req_दीर्घता = b1Len;
        }अन्यथा{
            req_दीर्घता = b1Len;
            गच्छ CLEANUP;
        }
    }अन्यथा{
        // step 5 : verify the sequence does not begin with ACE prefix
        यदि(!starts_With_उपसर्गः(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
            b2Len = u_str_गन्तव्यम्_Punycode(b1,b1Len,b2,b2_क्षमता,caseFlags,&बफर_स्थितिः);

            यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                यदि(b2 == शून्यसूचकः){
                    *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                    गच्छ CLEANUP;
                }

                बफर_स्थितिः = U_ZERO_ERROR; // reset error

                b2Len = u_str_गन्तव्यम्_Punycode(b1,b1Len,b2,b2Len,caseFlags,&बफर_स्थितिः);
            }
            //error bail out
            यदि(U_FAILURE(बफर_स्थितिः)){
                *स्थितिः = बफर_स्थितिः;
                गच्छ CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_दीर्घता = b2Len+ACE_उपसर्गः_दीर्घता;

            यदि(req_दीर्घता > dest_क्षमता){
                *स्थितिः = U_बफर_अतिप्रवाहः_ERROR;
                गच्छ CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_उपसर्गः, ACE_उपसर्गः_दीर्घता);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_उपसर्गः_दीर्घता, b2, b2Len);

        }अन्यथा{
            *स्थितिः = U_IDNA_ACE_उपसर्गः_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            गच्छ CLEANUP;
        }
    }
    // step 8: verify the length of label
    यदि(req_दीर्घता > MAX_LABEL_दीर्घता){
        *स्थितिः = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    यदि(b1 != b1_राशिः){
        uprv_free(b1);
    }
    यदि(b2 != b2_राशिः){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, स्थितिः);
}

स्थिर int32_t
आन्तरिकम्_गन्तव्यम्_Unicode(ध्रुव char16_t* src, int32_t src_दीर्घता,
                    char16_t* dest, int32_t dest_क्षमता,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_संहिता* स्थितिः)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_राशिः[MAX_LABEL_बफर_आकार], b2_राशिः[MAX_LABEL_बफर_आकार], b3_राशिः[MAX_LABEL_बफर_आकार];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_राशिः, *b2 = b2_राशिः, *b1_अविभाज्यः=शून्यसूचकः, *b3=b3_राशिः;
    int32_t b1Len = 0, b2Len, b1_अविभाज्यः_Len, b3Len,
            b1_क्षमता = MAX_LABEL_बफर_आकार, 
            b2_क्षमता = MAX_LABEL_बफर_आकार,
            b3_क्षमता = MAX_LABEL_बफर_आकार,
            req_दीर्घता=0;

    UBool* caseFlags = शून्यसूचकः;

    UBool srcIsASCII = सत्य;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    यदि(src_दीर्घता==-1){
        src_दीर्घता = 0;
        प्रति(;src[src_दीर्घता]!=0;){
            यदि(src[src_दीर्घता]> 0x7f){
                srcIsASCII = असत्य;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_दीर्घता++;
        }
    }अन्यथा यदि(src_दीर्घता > 0){
        प्रति(int32_t j=0; j<src_दीर्घता; j++){
            यदि(src[j]> 0x7f){
                srcIsASCII = असत्य;
                भङ्ग;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }अन्यथा{
        प्रत्यागम 0;
    }
    
    यदि(srcIsASCII == असत्य){
        // step 2: process the string
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_दीर्घता, b1, b1_क्षमता, namePrepOptions, parseError, &बफर_स्थितिः);
        यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            यदि(b1==शून्यसूचकः){
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ CLEANUP;
            }

            बफर_स्थितिः = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_दीर्घता, b1, b1Len, namePrepOptions, parseError, &बफर_स्थितिः);
        }
        //bail out on error
        यदि(U_FAILURE(बफर_स्थितिः)){
            *स्थितिः = बफर_स्थितिः;
            गच्छ CLEANUP;
        }
    }अन्यथा{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_दीर्घता;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    यदि(starts_With_उपसर्गः(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_अविभाज्यः = b1 + ACE_उपसर्गः_दीर्घता;
        b1_अविभाज्यः_Len  = b1Len - ACE_उपसर्गः_दीर्घता;

        //step 5: Decode using punycode
        UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_अविभाज्यः, b1_अविभाज्यः_Len, b2, b2_क्षमता, caseFlags, &बफर_स्थितिः);

        यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            यदि(b2==शून्यसूचकः){
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ CLEANUP;
            }

            बफर_स्थितिः = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_अविभाज्यः, b1_अविभाज्यः_Len, b2, b2Len, caseFlags, &बफर_स्थितिः);
        }


        //step 6:Apply toASCII
        b3Len = uidna_गन्तव्यम्_ASCII(b2, b2Len, b3, b3_क्षमता, options, parseError, &बफर_स्थितिः);

        यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            यदि(b3==शून्यसूचकः){
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
                गच्छ CLEANUP;
            }

            बफर_स्थितिः = U_ZERO_ERROR; // reset error

            b3Len =  uidna_गन्तव्यम्_ASCII(b2,b2Len,b3,b3Len,options,parseError,&बफर_स्थितिः);

        }
        //bail out on error
        यदि(U_FAILURE(बफर_स्थितिः)){
            *स्थितिः = बफर_स्थितिः;
            गच्छ CLEANUP;
        }

        //step 7: verify
        यदि(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *स्थितिः = U_IDNA_VERIFICATION_ERROR;
            गच्छ CLEANUP;
        }

        //step 8: return output of step 5
        req_दीर्घता = b2Len;
        यदि(b2Len <= dest_क्षमता) {
            u_memmove(dest, b2, b2Len);
        }
    }
    अन्यथा{
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
        यदि(src_दीर्घता <= dest_क्षमता){
            u_memmove(dest, src, src_दीर्घता);
        }
        req_दीर्घता = src_दीर्घता;
    }


CLEANUP:

    यदि(b1 != b1_राशिः && b1!=src){
        uprv_free(b1);
    }
    यदि(b2 != b2_राशिः){
        uprv_free(b2);
    }
    यदि(b3 != b3_राशिः){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    यदि(U_FAILURE(*स्थितिः)){
        //copy the source to destination
        यदि(dest && src_दीर्घता <= dest_क्षमता){
            // srcLength should have already been set earlier.
            U_ASSERT(src_दीर्घता >= 0);
            u_memmove(dest, src, src_दीर्घता);
        }
        req_दीर्घता = src_दीर्घता;
        *स्थितिः = U_ZERO_ERROR;
    }

    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uidna_गन्तव्यम्_ASCII(ध्रुव char16_t* src, int32_t src_दीर्घता,
              char16_t* dest, int32_t dest_क्षमता,
              int32_t options,
              UParseError* parseError,
              UError_संहिता* स्थितिः){
    
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }
    यदि((src==शून्यसूचकः) || (src_दीर्घता < -1) || (dest_क्षमता<0) || (!dest && dest_क्षमता > 0)){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, स्थितिः);
    
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम -1;
    }
    
    int32_t retLen = आन्तरिकम्_गन्तव्यम्_ASCII(src, src_दीर्घता, dest, dest_क्षमता, options, nameprep, parseError, स्थितिः);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    प्रत्यागम retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_गन्तव्यम्_Unicode(ध्रुव char16_t* src, int32_t src_दीर्घता,
                char16_t* dest, int32_t dest_क्षमता,
                int32_t options,
                UParseError* parseError,
                UError_संहिता* स्थितिः){

    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }
    यदि( (src==शून्यसूचकः) || (src_दीर्घता < -1) || (dest_क्षमता<0) || (!dest && dest_क्षमता > 0)){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, स्थितिः);
    
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम -1;
    }
    
    int32_t retLen = आन्तरिकम्_गन्तव्यम्_Unicode(src, src_दीर्घता, dest, dest_क्षमता, options, nameprep, parseError, स्थितिः);

    usprep_close(nameprep);
    
    प्रत्यागम retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ध्रुव char16_t *src, int32_t src_दीर्घता,
                   char16_t* dest, int32_t dest_क्षमता,
                   int32_t options,
                   UParseError *parseError,
                   UError_संहिता *स्थितिः){

    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }
    यदि((src==शून्यसूचकः) || (src_दीर्घता < -1) || (dest_क्षमता<0) || (!dest && dest_क्षमता > 0)){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    int32_t req_दीर्घता = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, स्थितिः);
    
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_दीर्घता;
    int32_t remaining_Dest_क्षमता = dest_क्षमता;
    int32_t labelLen = 0, label_Req_दीर्घता = 0;
    UBool समाप्तम् = असत्य;


    प्रति(;;){

        labelLen = get_अग्र_Separator(labelStart,remainingLen, &delimiter,&समाप्तम्);
        label_Req_दीर्घता = 0;
        यदि(!(labelLen==0 && समाप्तम्)){// make sure this is not a root label separator.

            UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
            label_Req_दीर्घता = आन्तरिकम्_गन्तव्यम्_ASCII( labelStart, labelLen,
                                                currentDest, remaining_Dest_क्षमता,
                                                options, nameprep,
                                                parseError, &बफर_स्थितिः);

            यदि (बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR) {
                remaining_Dest_क्षमता = 0;
            } अन्यथा यदि (U_FAILURE(बफर_स्थितिः)) {
                *स्थितिः = बफर_स्थितिः;
                भङ्ग;
            }
        }

        req_दीर्घता +=label_Req_दीर्घता;
        // adjust the destination pointer
        यदि(label_Req_दीर्घता < remaining_Dest_क्षमता){
            currentDest = currentDest + label_Req_दीर्घता;
            remaining_Dest_क्षमता -= label_Req_दीर्घता;
        }अन्यथा{
            // should never occur
            remaining_Dest_क्षमता = 0;
        }

        यदि(समाप्तम्){
            भङ्ग;
        }

        // add the label separator
        यदि(remaining_Dest_क्षमता > 0){
            *currentDest++ = पूर्णम्_STOP;
            remaining_Dest_क्षमता--;
        }
        req_दीर्घता++;

        labelStart = delimiter;
        यदि(remainingLen >0 ){
            remainingLen = (int32_t)(src_दीर्घता - (delimiter - src));
        }

    }

    यदि(req_दीर्घता > MAX_DOMAIN_NAME_दीर्घता){
        *स्थितिः = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ध्रुव char16_t* src, int32_t src_दीर्घता,
                     char16_t* dest, int32_t dest_क्षमता,
                     int32_t options,
                     UParseError* parseError,
                     UError_संहिता* स्थितिः){
    
    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }
    यदि((src==शून्यसूचकः) || (src_दीर्घता < -1) || (dest_क्षमता<0) || (!dest && dest_क्षमता > 0)){
        *स्थितिः = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    int32_t req_दीर्घता = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, स्थितिः);
    
    यदि(U_FAILURE(*स्थितिः)){
        प्रत्यागम 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_दीर्घता;
    int32_t remaining_Dest_क्षमता = dest_क्षमता;
    int32_t labelLen = 0, label_Req_दीर्घता = 0;
    UBool समाप्तम् = असत्य;

    प्रति(;;){

        labelLen = get_अग्र_Separator(labelStart,remainingLen, &delimiter,&समाप्तम्);
        
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
        
        label_Req_दीर्घता = आन्तरिकम्_गन्तव्यम्_Unicode(labelStart, labelLen, 
                                             currentDest, remaining_Dest_क्षमता, 
                                             options, nameprep, 
                                             parseError, स्थितिः);

        यदि(*स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
            *स्थितिः = U_ZERO_ERROR; // reset error
            remaining_Dest_क्षमता = 0;
        }

        यदि(U_FAILURE(*स्थितिः)){
            भङ्ग;
        }
        
        req_दीर्घता +=label_Req_दीर्घता;
        // adjust the destination pointer
        यदि(label_Req_दीर्घता < remaining_Dest_क्षमता){
            currentDest = currentDest + label_Req_दीर्घता;
            remaining_Dest_क्षमता -= label_Req_दीर्घता;
        }अन्यथा{
            // should never occur
            remaining_Dest_क्षमता = 0;
        }

        यदि(समाप्तम्){
            भङ्ग;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        यदि(remaining_Dest_क्षमता > 0){
            *currentDest++ = *(labelStart + labelLen);
            remaining_Dest_क्षमता--;
        }
        req_दीर्घता++;

        labelStart = delimiter;
        यदि(remainingLen >0 ){
            remainingLen = (int32_t)(src_दीर्घता - (delimiter - src));
        }

    }

    यदि(req_दीर्घता > MAX_DOMAIN_NAME_दीर्घता){
        *स्थितिः = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    प्रत्यागम u_terminateUChars(dest, dest_क्षमता, req_दीर्घता, स्थितिः);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ध्रुव char16_t *s1, int32_t length1,
                ध्रुव char16_t *s2, int32_t length2,
                int32_t options,
                UError_संहिता* स्थितिः){

    यदि(स्थितिः == शून्यसूचकः || U_FAILURE(*स्थितिः)){
        प्रत्यागम -1;
    }

    char16_t b1_राशिः[MAX_IDN_बफर_आकार], b2_राशिः[MAX_IDN_बफर_आकार];
    char16_t *b1 = b1_राशिः, *b2 = b2_राशिः;
    int32_t b1Len, b2Len, b1_क्षमता = MAX_IDN_बफर_आकार, b2_क्षमता = MAX_IDN_बफर_आकार;
    int32_t परिणाम=-1;
    
    UParseError parseError;

    UError_संहिता बफर_स्थितिः = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_क्षमता, options, &parseError, &बफर_स्थितिः);
    यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        यदि(b1==शून्यसूचकः){
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ CLEANUP;
        }

        बफर_स्थितिः = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &बफर_स्थितिः);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_क्षमता, options, &parseError, &बफर_स्थितिः);
    यदि(बफर_स्थितिः == U_बफर_अतिप्रवाहः_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        यदि(b2==शून्यसूचकः){
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            गच्छ CLEANUP;
        }

        बफर_स्थितिः = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &बफर_स्थितिः);
    }

    यदि (U_FAILURE(बफर_स्थितिः)) {
        *स्थितिः = बफर_स्थितिः;
        गच्छ CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    परिणाम = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    यदि(b1 != b1_राशिः){
        uprv_free(b1);
    }

    यदि(b2 != b2_राशिः){
        uprv_free(b2);
    }

    प्रत्यागम परिणाम;
}

#यदि_अन्तः /* #if !UCONFIG_NO_IDNA */
