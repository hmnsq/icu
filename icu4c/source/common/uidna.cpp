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
स्थिर अचर char16_t ACE_PREFIX[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#define ACE_PREFIX_LENGTH 4

#define MAX_LABEL_LENGTH 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#define MAX_LABEL_BUFFER_SIZE 100

#define MAX_DOMAIN_NAME_LENGTH 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#define MAX_IDN_BUFFER_SIZE   MAX_DOMAIN_NAME_LENGTH+1

#define LOWER_CASE_DELTA 0x0020
#define HYPHEN           0x002D
#define FULL_STOP        0x002E
#define CAPITAL_A        0x0041
#define CAPITAL_Z        0x005A

अन्तःस्थ स्थिर char16_t
toASCIILower(char16_t ch){
    यदि(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        फर्काऊ ch + LOWER_CASE_DELTA;
    }
    फर्काऊ ch;
}

अन्तःस्थ स्थिर UBool 
startsWithPrefix(अचर char16_t* src , int32_t srcLength){
    यदि(srcLength < ACE_PREFIX_LENGTH){
        फर्काऊ असत्य;
    }

    लागि(int8_t i=0; i< ACE_PREFIX_LENGTH; i++){
        यदि(toASCIILower(src[i]) != ACE_PREFIX[i]){
            फर्काऊ असत्य;
        }
    }
    फर्काऊ सत्य;
}


अन्तःस्थ स्थिर int32_t
compareCaseInsensitiveASCII(अचर char16_t* s1, int32_t s1Len,
                            अचर char16_t* s2, int32_t s2Len){
    
    int32_t minLength;
    int32_t lengthResult;

    // are we comparing different lengths?
    यदि(s1Len != s2Len) {
        यदि(s1Len < s2Len) {
            minLength = s1Len;
            lengthResult = -1;
        } अन्यथा {
            minLength = s2Len;
            lengthResult = 1;
        }
    } अन्यथा {
        // ok the lengths are equal
        minLength = s1Len;
        lengthResult = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    लागि(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        यदि(i == minLength) {
            फर्काऊ lengthResult;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        यदि(c1!=c2) {
            rc = static_cast<int32_t>(toASCIILower(c1)) - static_cast<int32_t>(toASCIILower(c2));
            यदि(rc!=0) {
                lengthResult=rc;
                रोक;
            }
        }
    }
    फर्काऊ lengthResult;
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
            फर्काऊ सत्य;
        स्वभाव:
            फर्काऊ असत्य;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
स्थिर अन्तःस्थ int32_t
getNextSeparator(char16_t *src, int32_t srcLength,
                 char16_t **limit, UBool *done){
    यदि(srcLength == -1){
        int32_t i;
        लागि(i=0 ; ;i++){
            यदि(src[i] == 0){
                *limit = src + i; // point to null
                *done = सत्य;
                फर्काऊ i;
            }
            यदि(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                फर्काऊ i;
                
            }
        }
    }अन्यथा{
        int32_t i;
        लागि(i=0;i<srcLength;i++){
            यदि(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                फर्काऊ i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *limit = src+srcLength;
        *done = सत्य;

        फर्काऊ i;
    }
}
स्थिर अन्तःस्थ UBool isLDHChar(char16_t ch){
    // high runner case
    यदि(ch>0x007A){
        फर्काऊ असत्य;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    यदि( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        फर्काऊ सत्य;
    }
    फर्काऊ असत्य;
}

स्थिर int32_t 
_internal_toASCII(अचर char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UErrorCode* status)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_थाक[MAX_LABEL_BUFFER_SIZE], b2_थाक[MAX_LABEL_BUFFER_SIZE];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_थाक, *b2 = b2_थाक;
    int32_t b1Len=0, b2Len, 
            b1Capacity = MAX_LABEL_BUFFER_SIZE, 
            b2Capacity = MAX_LABEL_BUFFER_SIZE ,
            reqLength=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = nullptr;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = सत्य;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = सत्य; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_RULES) != 0);

    int32_t failPos = -1;
    
    यदि(srcLength == -1){
        srcLength = u_strlen(src);
    }
    
    यदि(srcLength > b1Capacity){
        b1 = static_cast<char16_t*>(uprv_malloc(srcLength * U_SIZEOF_UCHAR));
        यदि(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            गच्छ CLEANUP;
        }
        b1Capacity = srcLength;
    }

    // step 1 
    लागि( j=0;j<srcLength;j++){
        यदि(src[j] > 0x7F){
            srcIsASCII = असत्य;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    यदि(srcIsASCII == असत्य){
        
        // step 2    
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Capacity, namePrepOptions, parseError, &bufferStatus);

        यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            यदि(b1 != b1_थाक){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            यदि(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                गच्छ CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Len, namePrepOptions, parseError, &bufferStatus);
        }
        यदि (U_FAILURE(bufferStatus)) {
            *status = bufferStatus;
        }
    }
    // error bail out
    यदि(U_FAILURE(*status)){
        गच्छ CLEANUP;
    }
    यदि(b1Len == 0){
        *status = U_IDNA_ZERO_LENGTH_LABEL_ERROR;
        गच्छ CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = सत्य;
    लागि( j=0;j<b1Len;j++){
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
            *status = U_IDNA_STD3_ASCII_RULES_ERROR;

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
        यदि(b1Len <= destCapacity){
            u_memmove(dest, b1, b1Len);
            reqLength = b1Len;
        }अन्यथा{
            reqLength = b1Len;
            गच्छ CLEANUP;
        }
    }अन्यथा{
        // step 5 : verify the sequence does not begin with ACE prefix
        यदि(!startsWithPrefix(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UErrorCode bufferStatus = U_ZERO_ERROR;
            b2Len = u_strToPunycode(b1,b1Len,b2,b2Capacity,caseFlags,&bufferStatus);

            यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                यदि(b2 == nullptr){
                    *status = U_MEMORY_ALLOCATION_ERROR;
                    गच्छ CLEANUP;
                }

                bufferStatus = U_ZERO_ERROR; // reset error

                b2Len = u_strToPunycode(b1,b1Len,b2,b2Len,caseFlags,&bufferStatus);
            }
            //error bail out
            यदि(U_FAILURE(bufferStatus)){
                *status = bufferStatus;
                गच्छ CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            reqLength = b2Len+ACE_PREFIX_LENGTH;

            यदि(reqLength > destCapacity){
                *status = U_BUFFER_OVERFLOW_ERROR;
                गच्छ CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_PREFIX, ACE_PREFIX_LENGTH);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_PREFIX_LENGTH, b2, b2Len);

        }अन्यथा{
            *status = U_IDNA_ACE_PREFIX_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            गच्छ CLEANUP;
        }
    }
    // step 8: verify the length of label
    यदि(reqLength > MAX_LABEL_LENGTH){
        *status = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    यदि(b1 != b1_थाक){
        uprv_free(b1);
    }
    यदि(b2 != b2_थाक){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    फर्काऊ u_terminateUChars(dest, destCapacity, reqLength, status);
}

स्थिर int32_t
_internal_toUnicode(अचर char16_t* src, int32_t srcLength,
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
    char16_t b1_थाक[MAX_LABEL_BUFFER_SIZE], b2_थाक[MAX_LABEL_BUFFER_SIZE], b3_थाक[MAX_LABEL_BUFFER_SIZE];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_थाक, *b2 = b2_थाक, *b1_अविभाज्य=nullptr, *b3=b3_थाक;
    int32_t b1Len = 0, b2Len, b1_अविभाज्य_Len, b3Len,
            b1Capacity = MAX_LABEL_BUFFER_SIZE, 
            b2Capacity = MAX_LABEL_BUFFER_SIZE,
            b3Capacity = MAX_LABEL_BUFFER_SIZE,
            reqLength=0;

    UBool* caseFlags = nullptr;

    UBool srcIsASCII = सत्य;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    यदि(srcLength==-1){
        srcLength = 0;
        लागि(;src[srcLength]!=0;){
            यदि(src[srcLength]> 0x7f){
                srcIsASCII = असत्य;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            srcLength++;
        }
    }अन्यथा यदि(srcLength > 0){
        लागि(int32_t j=0; j<srcLength; j++){
            यदि(src[j]> 0x7f){
                srcIsASCII = असत्य;
                रोक;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }अन्यथा{
        फर्काऊ 0;
    }
    
    यदि(srcIsASCII == असत्य){
        // step 2: process the string
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Capacity, namePrepOptions, parseError, &bufferStatus);
        यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            यदि(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                गच्छ CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Len, namePrepOptions, parseError, &bufferStatus);
        }
        //bail out on error
        यदि(U_FAILURE(bufferStatus)){
            *status = bufferStatus;
            गच्छ CLEANUP;
        }
    }अन्यथा{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = srcLength;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    यदि(startsWithPrefix(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_अविभाज्य = b1 + ACE_PREFIX_LENGTH;
        b1_अविभाज्य_Len  = b1Len - ACE_PREFIX_LENGTH;

        //step 5: Decode using punycode
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_अविभाज्य, b1_अविभाज्य_Len, b2, b2Capacity, caseFlags, &bufferStatus);

        यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            यदि(b2==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                गच्छ CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_अविभाज्य, b1_अविभाज्य_Len, b2, b2Len, caseFlags, &bufferStatus);
        }


        //step 6:Apply toASCII
        b3Len = uidna_toASCII(b2, b2Len, b3, b3Capacity, options, parseError, &bufferStatus);

        यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            यदि(b3==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                गच्छ CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b3Len =  uidna_toASCII(b2,b2Len,b3,b3Len,options,parseError,&bufferStatus);

        }
        //bail out on error
        यदि(U_FAILURE(bufferStatus)){
            *status = bufferStatus;
            गच्छ CLEANUP;
        }

        //step 7: verify
        यदि(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *status = U_IDNA_VERIFICATION_ERROR;
            गच्छ CLEANUP;
        }

        //step 8: return output of step 5
        reqLength = b2Len;
        यदि(b2Len <= destCapacity) {
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
        यदि(srcLength <= destCapacity){
            u_memmove(dest, src, srcLength);
        }
        reqLength = srcLength;
    }


CLEANUP:

    यदि(b1 != b1_थाक && b1!=src){
        uprv_free(b1);
    }
    यदि(b2 != b2_थाक){
        uprv_free(b2);
    }
    यदि(b3 != b3_थाक){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    यदि(U_FAILURE(*status)){
        //copy the source to destination
        यदि(dest && srcLength <= destCapacity){
            // srcLength should have already been set earlier.
            U_ASSERT(srcLength >= 0);
            u_memmove(dest, src, srcLength);
        }
        reqLength = srcLength;
        *status = U_ZERO_ERROR;
    }

    फर्काऊ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_toASCII(अचर char16_t* src, int32_t srcLength,
              char16_t* dest, int32_t destCapacity,
              int32_t options,
              UParseError* parseError,
              UErrorCode* status){
    
    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ 0;
    }
    यदि((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    यदि(U_FAILURE(*status)){
        फर्काऊ -1;
    }
    
    int32_t retLen = _internal_toASCII(src, srcLength, dest, destCapacity, options, nameprep, parseError, status);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    फर्काऊ retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_toUnicode(अचर char16_t* src, int32_t srcLength,
                char16_t* dest, int32_t destCapacity,
                int32_t options,
                UParseError* parseError,
                UErrorCode* status){

    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ 0;
    }
    यदि( (src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    यदि(U_FAILURE(*status)){
        फर्काऊ -1;
    }
    
    int32_t retLen = _internal_toUnicode(src, srcLength, dest, destCapacity, options, nameprep, parseError, status);

    usprep_close(nameprep);
    
    फर्काऊ retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  अचर char16_t *src, int32_t srcLength,
                   char16_t* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError *parseError,
                   UErrorCode *status){

    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ 0;
    }
    यदि((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    int32_t reqLength = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    यदि(U_FAILURE(*status)){
        फर्काऊ 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = srcLength;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, labelReqLength = 0;
    UBool done = असत्य;


    लागि(;;){

        labelLen = getNextSeparator(labelStart,remainingLen, &delimiter,&done);
        labelReqLength = 0;
        यदि(!(labelLen==0 && done)){// make sure this is not a root label separator.

            UErrorCode bufferStatus = U_ZERO_ERROR;
            labelReqLength = _internal_toASCII( labelStart, labelLen,
                                                currentDest, remainingDestCapacity,
                                                options, nameprep,
                                                parseError, &bufferStatus);

            यदि (bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
                remainingDestCapacity = 0;
            } अन्यथा यदि (U_FAILURE(bufferStatus)) {
                *status = bufferStatus;
                रोक;
            }
        }

        reqLength +=labelReqLength;
        // adjust the destination pointer
        यदि(labelReqLength < remainingDestCapacity){
            currentDest = currentDest + labelReqLength;
            remainingDestCapacity -= labelReqLength;
        }अन्यथा{
            // should never occur
            remainingDestCapacity = 0;
        }

        यदि(done){
            रोक;
        }

        // add the label separator
        यदि(remainingDestCapacity > 0){
            *currentDest++ = FULL_STOP;
            remainingDestCapacity--;
        }
        reqLength++;

        labelStart = delimiter;
        यदि(remainingLen >0 ){
            remainingLen = (int32_t)(srcLength - (delimiter - src));
        }

    }

    यदि(reqLength > MAX_DOMAIN_NAME_LENGTH){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    फर्काऊ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  अचर char16_t* src, int32_t srcLength,
                     char16_t* dest, int32_t destCapacity,
                     int32_t options,
                     UParseError* parseError,
                     UErrorCode* status){
    
    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ 0;
    }
    यदि((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    int32_t reqLength = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    यदि(U_FAILURE(*status)){
        फर्काऊ 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = srcLength;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, labelReqLength = 0;
    UBool done = असत्य;

    लागि(;;){

        labelLen = getNextSeparator(labelStart,remainingLen, &delimiter,&done);
        
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
        
        labelReqLength = _internal_toUnicode(labelStart, labelLen, 
                                             currentDest, remainingDestCapacity, 
                                             options, nameprep, 
                                             parseError, status);

        यदि(*status == U_BUFFER_OVERFLOW_ERROR){
            *status = U_ZERO_ERROR; // reset error
            remainingDestCapacity = 0;
        }

        यदि(U_FAILURE(*status)){
            रोक;
        }
        
        reqLength +=labelReqLength;
        // adjust the destination pointer
        यदि(labelReqLength < remainingDestCapacity){
            currentDest = currentDest + labelReqLength;
            remainingDestCapacity -= labelReqLength;
        }अन्यथा{
            // should never occur
            remainingDestCapacity = 0;
        }

        यदि(done){
            रोक;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        यदि(remainingDestCapacity > 0){
            *currentDest++ = *(labelStart + labelLen);
            remainingDestCapacity--;
        }
        reqLength++;

        labelStart = delimiter;
        यदि(remainingLen >0 ){
            remainingLen = (int32_t)(srcLength - (delimiter - src));
        }

    }

    यदि(reqLength > MAX_DOMAIN_NAME_LENGTH){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    फर्काऊ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  अचर char16_t *s1, int32_t length1,
                अचर char16_t *s2, int32_t length2,
                int32_t options,
                UErrorCode* status){

    यदि(status == nullptr || U_FAILURE(*status)){
        फर्काऊ -1;
    }

    char16_t b1_थाक[MAX_IDN_BUFFER_SIZE], b2_थाक[MAX_IDN_BUFFER_SIZE];
    char16_t *b1 = b1_थाक, *b2 = b2_थाक;
    int32_t b1Len, b2Len, b1Capacity = MAX_IDN_BUFFER_SIZE, b2Capacity = MAX_IDN_BUFFER_SIZE;
    int32_t result=-1;
    
    UParseError parseError;

    UErrorCode bufferStatus = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1Capacity, options, &parseError, &bufferStatus);
    यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        यदि(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            गच्छ CLEANUP;
        }

        bufferStatus = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &bufferStatus);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2Capacity, options, &parseError, &bufferStatus);
    यदि(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        यदि(b2==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            गच्छ CLEANUP;
        }

        bufferStatus = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &bufferStatus);
    }

    यदि (U_FAILURE(bufferStatus)) {
        *status = bufferStatus;
        गच्छ CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    result = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    यदि(b1 != b1_थाक){
        uprv_free(b1);
    }

    यदि(b2 != b2_थाक){
        uprv_free(b2);
    }

    फर्काऊ result;
}

#endif /* #if !UCONFIG_NO_IDNA */
