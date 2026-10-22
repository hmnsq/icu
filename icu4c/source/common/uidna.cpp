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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_IDNA

#समावेश "unicode/uidna.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/usprep.h"
#समावेश "punycode.h"
#समावेश "ustr_imp.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"
#समावेश "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
अटल स्थिर char16_t ACE_PREFIX[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#घोषणा ACE_PREFIX_लंबाई 4

#घोषणा MAX_LABEL_लंबाई 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#घोषणा MAX_LABEL_बफर_आकार 100

#घोषणा MAX_DOMAIN_NAME_लंबाई 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#घोषणा MAX_IDN_बफर_आकार   MAX_DOMAIN_NAME_लंबाई+1

#घोषणा LOWER_CASE_DELTA 0x0020
#घोषणा HYPHEN           0x002D
#घोषणा FULL_STOP        0x002E
#घोषणा CAPITAL_A        0x0041
#घोषणा CAPITAL_Z        0x005A

अंतरभूत अटल char16_t
toASCIILower(char16_t ch){
    अगर(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        वापस ch + LOWER_CASE_DELTA;
    }
    वापस ch;
}

अंतरभूत अटल UBool 
startsWithPrefix(स्थिर char16_t* src , int32_t src_लंबाई){
    अगर(src_लंबाई < ACE_PREFIX_लंबाई){
        वापस false;
    }

    क्रम(int8_t i=0; i< ACE_PREFIX_लंबाई; i++){
        अगर(toASCIILower(src[i]) != ACE_PREFIX[i]){
            वापस false;
        }
    }
    वापस true;
}


अंतरभूत अटल int32_t
compareCaseInsensitiveASCII(स्थिर char16_t* s1, int32_t s1Len,
                            स्थिर char16_t* s2, int32_t s2Len){
    
    int32_t min_लंबाई;
    int32_t लंबाई_परिणाम;

    // are we comparing different lengths?
    अगर(s1Len != s2Len) {
        अगर(s1Len < s2Len) {
            min_लंबाई = s1Len;
            लंबाई_परिणाम = -1;
        } अन्यथा {
            min_लंबाई = s2Len;
            लंबाई_परिणाम = 1;
        }
    } अन्यथा {
        // ok the lengths are equal
        min_लंबाई = s1Len;
        लंबाई_परिणाम = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    क्रम(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        अगर(i == min_लंबाई) {
            वापस लंबाई_परिणाम;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        अगर(c1!=c2) {
            rc = static_cast<int32_t>(toASCIILower(c1)) - static_cast<int32_t>(toASCIILower(c2));
            अगर(rc!=0) {
                लंबाई_परिणाम=rc;
                अवरोध;
            }
        }
    }
    वापस लंबाई_परिणाम;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
अटल अंतरभूत UBool isLabelSeparator(char16_t ch){
    चयन(ch){
        हाल 0x002e:
        हाल 0x3002:
        हाल 0xFF0E:
        हाल 0xFF61:
            वापस true;
        शेष:
            वापस false;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
अटल अंतरभूत int32_t
get_अग्र_Separator(char16_t *src, int32_t src_लंबाई,
                 char16_t **limit, UBool *done){
    अगर(src_लंबाई == -1){
        int32_t i;
        क्रम(i=0 ; ;i++){
            अगर(src[i] == 0){
                *limit = src + i; // point to null
                *done = true;
                वापस i;
            }
            अगर(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                वापस i;
                
            }
        }
    }अन्यथा{
        int32_t i;
        क्रम(i=0;i<src_लंबाई;i++){
            अगर(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                वापस i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *limit = src+src_लंबाई;
        *done = true;

        वापस i;
    }
}
अटल अंतरभूत UBool isLDHChar(char16_t ch){
    // high runner case
    अगर(ch>0x007A){
        वापस false;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    अगर( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        वापस true;
    }
    वापस false;
}

अटल int32_t 
_internal_toASCII(स्थिर char16_t* src, int32_t src_लंबाई,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UErrorCode* status)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ढेर[MAX_LABEL_बफर_आकार], b2_ढेर[MAX_LABEL_बफर_आकार];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ढेर, *b2 = b2_ढेर;
    int32_t b1Len=0, b2Len, 
            b1Capacity = MAX_LABEL_बफर_आकार, 
            b2Capacity = MAX_LABEL_बफर_आकार ,
            req_लंबाई=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = nullptr;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = true;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = true; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_RULES) != 0);

    int32_t failPos = -1;
    
    अगर(src_लंबाई == -1){
        src_लंबाई = u_strlen(src);
    }
    
    अगर(src_लंबाई > b1Capacity){
        b1 = static_cast<char16_t*>(uprv_malloc(src_लंबाई * U_SIZEOF_UCHAR));
        अगर(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            जाओ CLEANUP;
        }
        b1Capacity = src_लंबाई;
    }

    // step 1 
    क्रम( j=0;j<src_लंबाई;j++){
        अगर(src[j] > 0x7F){
            srcIsASCII = false;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    अगर(srcIsASCII == false){
        
        // step 2    
        UErrorCode बफर_Status = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_लंबाई, b1, b1Capacity, namePrepOptions, parseError, &बफर_Status);

        अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            अगर(b1 != b1_ढेर){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            अगर(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                जाओ CLEANUP;
            }

            बफर_Status = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_लंबाई, b1, b1Len, namePrepOptions, parseError, &बफर_Status);
        }
        अगर (U_FAILURE(बफर_Status)) {
            *status = बफर_Status;
        }
    }
    // error bail out
    अगर(U_FAILURE(*status)){
        जाओ CLEANUP;
    }
    अगर(b1Len == 0){
        *status = U_IDNA_ZERO_लंबाई_LABEL_ERROR;
        जाओ CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = true;
    क्रम( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        अगर(b1[j] > 0x7F){
            srcIsASCII = false;
        }अन्यथा अगर(isLDHChar(b1[j])==false){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = false;
            failPos = j;
        }
    }
    अगर(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        अगर( srcIsLDH == false /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *status = U_IDNA_STD3_ASCII_RULES_ERROR;

            /* populate the parseError struct */
            अगर(srcIsLDH==false){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }अन्यथा अगर(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }अन्यथा{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            जाओ CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    अगर(srcIsASCII){
        अगर(b1Len <= destCapacity){
            u_memmove(dest, b1, b1Len);
            req_लंबाई = b1Len;
        }अन्यथा{
            req_लंबाई = b1Len;
            जाओ CLEANUP;
        }
    }अन्यथा{
        // step 5 : verify the sequence does not begin with ACE prefix
        अगर(!startsWithPrefix(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UErrorCode बफर_Status = U_ZERO_ERROR;
            b2Len = u_strToPunycode(b1,b1Len,b2,b2Capacity,caseFlags,&बफर_Status);

            अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                अगर(b2 == nullptr){
                    *status = U_MEMORY_ALLOCATION_ERROR;
                    जाओ CLEANUP;
                }

                बफर_Status = U_ZERO_ERROR; // reset error

                b2Len = u_strToPunycode(b1,b1Len,b2,b2Len,caseFlags,&बफर_Status);
            }
            //error bail out
            अगर(U_FAILURE(बफर_Status)){
                *status = बफर_Status;
                जाओ CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_लंबाई = b2Len+ACE_PREFIX_लंबाई;

            अगर(req_लंबाई > destCapacity){
                *status = U_बफर_OVERFLOW_ERROR;
                जाओ CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_PREFIX, ACE_PREFIX_लंबाई);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_PREFIX_लंबाई, b2, b2Len);

        }अन्यथा{
            *status = U_IDNA_ACE_PREFIX_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            जाओ CLEANUP;
        }
    }
    // step 8: verify the length of label
    अगर(req_लंबाई > MAX_LABEL_लंबाई){
        *status = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    अगर(b1 != b1_ढेर){
        uprv_free(b1);
    }
    अगर(b2 != b2_ढेर){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    वापस u_terminateUChars(dest, destCapacity, req_लंबाई, status);
}

अटल int32_t
_internal_toUnicode(स्थिर char16_t* src, int32_t src_लंबाई,
                    char16_t* dest, int32_t destCapacity,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UErrorCode* status)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ढेर[MAX_LABEL_बफर_आकार], b2_ढेर[MAX_LABEL_बफर_आकार], b3_ढेर[MAX_LABEL_बफर_आकार];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ढेर, *b2 = b2_ढेर, *b1_अभाज्य=nullptr, *b3=b3_ढेर;
    int32_t b1Len = 0, b2Len, b1_अभाज्य_Len, b3Len,
            b1Capacity = MAX_LABEL_बफर_आकार, 
            b2Capacity = MAX_LABEL_बफर_आकार,
            b3Capacity = MAX_LABEL_बफर_आकार,
            req_लंबाई=0;

    UBool* caseFlags = nullptr;

    UBool srcIsASCII = true;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    अगर(src_लंबाई==-1){
        src_लंबाई = 0;
        क्रम(;src[src_लंबाई]!=0;){
            अगर(src[src_लंबाई]> 0x7f){
                srcIsASCII = false;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_लंबाई++;
        }
    }अन्यथा अगर(src_लंबाई > 0){
        क्रम(int32_t j=0; j<src_लंबाई; j++){
            अगर(src[j]> 0x7f){
                srcIsASCII = false;
                अवरोध;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }अन्यथा{
        वापस 0;
    }
    
    अगर(srcIsASCII == false){
        // step 2: process the string
        UErrorCode बफर_Status = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_लंबाई, b1, b1Capacity, namePrepOptions, parseError, &बफर_Status);
        अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            अगर(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                जाओ CLEANUP;
            }

            बफर_Status = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_लंबाई, b1, b1Len, namePrepOptions, parseError, &बफर_Status);
        }
        //bail out on error
        अगर(U_FAILURE(बफर_Status)){
            *status = बफर_Status;
            जाओ CLEANUP;
        }
    }अन्यथा{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_लंबाई;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    अगर(startsWithPrefix(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_अभाज्य = b1 + ACE_PREFIX_लंबाई;
        b1_अभाज्य_Len  = b1Len - ACE_PREFIX_लंबाई;

        //step 5: Decode using punycode
        UErrorCode बफर_Status = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_अभाज्य, b1_अभाज्य_Len, b2, b2Capacity, caseFlags, &बफर_Status);

        अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            अगर(b2==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                जाओ CLEANUP;
            }

            बफर_Status = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_अभाज्य, b1_अभाज्य_Len, b2, b2Len, caseFlags, &बफर_Status);
        }


        //step 6:Apply toASCII
        b3Len = uidna_toASCII(b2, b2Len, b3, b3Capacity, options, parseError, &बफर_Status);

        अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            अगर(b3==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                जाओ CLEANUP;
            }

            बफर_Status = U_ZERO_ERROR; // reset error

            b3Len =  uidna_toASCII(b2,b2Len,b3,b3Len,options,parseError,&बफर_Status);

        }
        //bail out on error
        अगर(U_FAILURE(बफर_Status)){
            *status = बफर_Status;
            जाओ CLEANUP;
        }

        //step 7: verify
        अगर(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *status = U_IDNA_VERIFICATION_ERROR;
            जाओ CLEANUP;
        }

        //step 8: return output of step 5
        req_लंबाई = b2Len;
        अगर(b2Len <= destCapacity) {
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
        अगर(src_लंबाई <= destCapacity){
            u_memmove(dest, src, src_लंबाई);
        }
        req_लंबाई = src_लंबाई;
    }


CLEANUP:

    अगर(b1 != b1_ढेर && b1!=src){
        uprv_free(b1);
    }
    अगर(b2 != b2_ढेर){
        uprv_free(b2);
    }
    अगर(b3 != b3_ढेर){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    अगर(U_FAILURE(*status)){
        //copy the source to destination
        अगर(dest && src_लंबाई <= destCapacity){
            // srcLength should have already been set earlier.
            U_ASSERT(src_लंबाई >= 0);
            u_memmove(dest, src, src_लंबाई);
        }
        req_लंबाई = src_लंबाई;
        *status = U_ZERO_ERROR;
    }

    वापस u_terminateUChars(dest, destCapacity, req_लंबाई, status);
}

U_CAPI int32_t U_EXPORT2
uidna_toASCII(स्थिर char16_t* src, int32_t src_लंबाई,
              char16_t* dest, int32_t destCapacity,
              int32_t options,
              UParseError* parseError,
              UErrorCode* status){
    
    अगर(status == nullptr || U_FAILURE(*status)){
        वापस 0;
    }
    अगर((src==nullptr) || (src_लंबाई < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    अगर(U_FAILURE(*status)){
        वापस -1;
    }
    
    int32_t retLen = _internal_toASCII(src, src_लंबाई, dest, destCapacity, options, nameprep, parseError, status);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    वापस retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_toUnicode(स्थिर char16_t* src, int32_t src_लंबाई,
                char16_t* dest, int32_t destCapacity,
                int32_t options,
                UParseError* parseError,
                UErrorCode* status){

    अगर(status == nullptr || U_FAILURE(*status)){
        वापस 0;
    }
    अगर( (src==nullptr) || (src_लंबाई < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    अगर(U_FAILURE(*status)){
        वापस -1;
    }
    
    int32_t retLen = _internal_toUnicode(src, src_लंबाई, dest, destCapacity, options, nameprep, parseError, status);

    usprep_close(nameprep);
    
    वापस retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  स्थिर char16_t *src, int32_t src_लंबाई,
                   char16_t* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError *parseError,
                   UErrorCode *status){

    अगर(status == nullptr || U_FAILURE(*status)){
        वापस 0;
    }
    अगर((src==nullptr) || (src_लंबाई < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    int32_t req_लंबाई = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    अगर(U_FAILURE(*status)){
        वापस 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_लंबाई;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, label_Req_लंबाई = 0;
    UBool done = false;


    क्रम(;;){

        labelLen = get_अग्र_Separator(labelStart,remainingLen, &delimiter,&done);
        label_Req_लंबाई = 0;
        अगर(!(labelLen==0 && done)){// make sure this is not a root label separator.

            UErrorCode बफर_Status = U_ZERO_ERROR;
            label_Req_लंबाई = _internal_toASCII( labelStart, labelLen,
                                                currentDest, remainingDestCapacity,
                                                options, nameprep,
                                                parseError, &बफर_Status);

            अगर (बफर_Status == U_बफर_OVERFLOW_ERROR) {
                remainingDestCapacity = 0;
            } अन्यथा अगर (U_FAILURE(बफर_Status)) {
                *status = बफर_Status;
                अवरोध;
            }
        }

        req_लंबाई +=label_Req_लंबाई;
        // adjust the destination pointer
        अगर(label_Req_लंबाई < remainingDestCapacity){
            currentDest = currentDest + label_Req_लंबाई;
            remainingDestCapacity -= label_Req_लंबाई;
        }अन्यथा{
            // should never occur
            remainingDestCapacity = 0;
        }

        अगर(done){
            अवरोध;
        }

        // add the label separator
        अगर(remainingDestCapacity > 0){
            *currentDest++ = FULL_STOP;
            remainingDestCapacity--;
        }
        req_लंबाई++;

        labelStart = delimiter;
        अगर(remainingLen >0 ){
            remainingLen = (int32_t)(src_लंबाई - (delimiter - src));
        }

    }

    अगर(req_लंबाई > MAX_DOMAIN_NAME_लंबाई){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    वापस u_terminateUChars(dest, destCapacity, req_लंबाई, status);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  स्थिर char16_t* src, int32_t src_लंबाई,
                     char16_t* dest, int32_t destCapacity,
                     int32_t options,
                     UParseError* parseError,
                     UErrorCode* status){
    
    अगर(status == nullptr || U_FAILURE(*status)){
        वापस 0;
    }
    अगर((src==nullptr) || (src_लंबाई < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    int32_t req_लंबाई = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    अगर(U_FAILURE(*status)){
        वापस 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_लंबाई;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, label_Req_लंबाई = 0;
    UBool done = false;

    क्रम(;;){

        labelLen = get_अग्र_Separator(labelStart,remainingLen, &delimiter,&done);
        
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
        
        label_Req_लंबाई = _internal_toUnicode(labelStart, labelLen, 
                                             currentDest, remainingDestCapacity, 
                                             options, nameprep, 
                                             parseError, status);

        अगर(*status == U_बफर_OVERFLOW_ERROR){
            *status = U_ZERO_ERROR; // reset error
            remainingDestCapacity = 0;
        }

        अगर(U_FAILURE(*status)){
            अवरोध;
        }
        
        req_लंबाई +=label_Req_लंबाई;
        // adjust the destination pointer
        अगर(label_Req_लंबाई < remainingDestCapacity){
            currentDest = currentDest + label_Req_लंबाई;
            remainingDestCapacity -= label_Req_लंबाई;
        }अन्यथा{
            // should never occur
            remainingDestCapacity = 0;
        }

        अगर(done){
            अवरोध;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        अगर(remainingDestCapacity > 0){
            *currentDest++ = *(labelStart + labelLen);
            remainingDestCapacity--;
        }
        req_लंबाई++;

        labelStart = delimiter;
        अगर(remainingLen >0 ){
            remainingLen = (int32_t)(src_लंबाई - (delimiter - src));
        }

    }

    अगर(req_लंबाई > MAX_DOMAIN_NAME_लंबाई){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    वापस u_terminateUChars(dest, destCapacity, req_लंबाई, status);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  स्थिर char16_t *s1, int32_t length1,
                स्थिर char16_t *s2, int32_t length2,
                int32_t options,
                UErrorCode* status){

    अगर(status == nullptr || U_FAILURE(*status)){
        वापस -1;
    }

    char16_t b1_ढेर[MAX_IDN_बफर_आकार], b2_ढेर[MAX_IDN_बफर_आकार];
    char16_t *b1 = b1_ढेर, *b2 = b2_ढेर;
    int32_t b1Len, b2Len, b1Capacity = MAX_IDN_बफर_आकार, b2Capacity = MAX_IDN_बफर_आकार;
    int32_t परिणाम=-1;
    
    UParseError parseError;

    UErrorCode बफर_Status = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1Capacity, options, &parseError, &बफर_Status);
    अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        अगर(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            जाओ CLEANUP;
        }

        बफर_Status = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &बफर_Status);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2Capacity, options, &parseError, &बफर_Status);
    अगर(बफर_Status == U_बफर_OVERFLOW_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        अगर(b2==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            जाओ CLEANUP;
        }

        बफर_Status = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &बफर_Status);
    }

    अगर (U_FAILURE(बफर_Status)) {
        *status = बफर_Status;
        जाओ CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    परिणाम = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    अगर(b1 != b1_ढेर){
        uprv_free(b1);
    }

    अगर(b2 != b2_ढेर){
        uprv_free(b2);
    }

    वापस परिणाम;
}

#पूर्ण_अगर /* #if !UCONFIG_NO_IDNA */
