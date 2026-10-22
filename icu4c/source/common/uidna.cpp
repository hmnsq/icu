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

#include "unicode/utypes.h"

#ꦪꦼꦤ꧀ !UCONFIG_NO_IDNA

#include "unicode/uidna.h"
#include "unicode/ustring.h"
#include "unicode/usprep.h"
#include "punycode.h"
#include "ustr_imp.h"
#include "cmemory.h"
#include "uassert.h"
#include "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
static const char16_t ACE_PREFIX[] ={ 0x0078,0x006E,0x002d,0x002d } ;
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

anta_hstha static char16_t
toASCIILower(char16_t ch){
    ꦪꦼꦤ꧀(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        ꦮꦭꦶ ch + LOWER_CASE_DELTA;
    }
    ꦮꦭꦶ ch;
}

anta_hstha static UBool 
startsWithPrefix(const char16_t* src , int32_t srcLength){
    ꦪꦼꦤ꧀(srcLength < ACE_PREFIX_LENGTH){
        ꦮꦭꦶ false;
    }

    ꦏꦁꦒꦺꦴ(int8_t i=0; i< ACE_PREFIX_LENGTH; i++){
        ꦪꦼꦤ꧀(toASCIILower(src[i]) != ACE_PREFIX[i]){
            ꦮꦭꦶ false;
        }
    }
    ꦮꦭꦶ true;
}


anta_hstha static int32_t
compareCaseInsensitiveASCII(const char16_t* s1, int32_t s1Len,
                            const char16_t* s2, int32_t s2Len){
    
    int32_t minLength;
    int32_t lengthResult;

    // are we comparing different lengths?
    ꦪꦼꦤ꧀(s1Len != s2Len) {
        ꦪꦼꦤ꧀(s1Len < s2Len) {
            minLength = s1Len;
            lengthResult = -1;
        } ꦭꦶꦪ {
            minLength = s2Len;
            lengthResult = 1;
        }
    } ꦭꦶꦪ {
        // ok the lengths are equal
        minLength = s1Len;
        lengthResult = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    ꦏꦁꦒꦺꦴ(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        ꦪꦼꦤ꧀(i == minLength) {
            ꦮꦭꦶ lengthResult;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        ꦪꦼꦤ꧀(c1!=c2) {
            rc = static_cast<int32_t>(toASCIILower(c1)) - static_cast<int32_t>(toASCIILower(c2));
            ꦪꦼꦤ꧀(rc!=0) {
                lengthResult=rc;
                bha_nga;
            }
        }
    }
    ꦮꦭꦶ lengthResult;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
static anta_hstha UBool isLabelSeparator(char16_t ch){
    switch(ch){
        pak_sa 0x002e:
        pak_sa 0x3002:
        pak_sa 0xFF0E:
        pak_sa 0xFF61:
            ꦮꦭꦶ true;
        default:
            ꦮꦭꦶ false;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
static anta_hstha int32_t
getNextSeparator(char16_t *src, int32_t srcLength,
                 char16_t **limit, UBool *done){
    ꦪꦼꦤ꧀(srcLength == -1){
        int32_t i;
        ꦏꦁꦒꦺꦴ(i=0 ; ;i++){
            ꦪꦼꦤ꧀(src[i] == 0){
                *limit = src + i; // point to null
                *done = true;
                ꦮꦭꦶ i;
            }
            ꦪꦼꦤ꧀(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                ꦮꦭꦶ i;
                
            }
        }
    }ꦭꦶꦪ{
        int32_t i;
        ꦏꦁꦒꦺꦴ(i=0;i<srcLength;i++){
            ꦪꦼꦤ꧀(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                ꦮꦭꦶ i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *limit = src+srcLength;
        *done = true;

        ꦮꦭꦶ i;
    }
}
static anta_hstha UBool isLDHChar(char16_t ch){
    // high runner case
    ꦪꦼꦤ꧀(ch>0x007A){
        ꦮꦭꦶ false;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    ꦪꦼꦤ꧀( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        ꦮꦭꦶ true;
    }
    ꦮꦭꦶ false;
}

static int32_t 
_internal_toASCII(const char16_t* src, int32_t srcLength,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UErrorCode* status)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1Stack[MAX_LABEL_BUFFER_SIZE], b2Stack[MAX_LABEL_BUFFER_SIZE];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1Stack, *b2 = b2Stack;
    int32_t b1Len=0, b2Len, 
            b1Capacity = MAX_LABEL_BUFFER_SIZE, 
            b2Capacity = MAX_LABEL_BUFFER_SIZE ,
            reqLength=0;

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
    
    ꦪꦼꦤ꧀(srcLength == -1){
        srcLength = u_strlen(src);
    }
    
    ꦪꦼꦤ꧀(srcLength > b1Capacity){
        b1 = static_cast<char16_t*>(uprv_malloc(srcLength * U_SIZEOF_UCHAR));
        ꦪꦼꦤ꧀(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            goto CLEANUP;
        }
        b1Capacity = srcLength;
    }

    // step 1 
    ꦏꦁꦒꦺꦴ( j=0;j<srcLength;j++){
        ꦪꦼꦤ꧀(src[j] > 0x7F){
            srcIsASCII = false;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    ꦪꦼꦤ꧀(srcIsASCII == false){
        
        // step 2    
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Capacity, namePrepOptions, parseError, &bufferStatus);

        ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            ꦪꦼꦤ꧀(b1 != b1Stack){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ꦪꦼꦤ꧀(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                goto CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Len, namePrepOptions, parseError, &bufferStatus);
        }
        ꦪꦼꦤ꧀ (U_FAILURE(bufferStatus)) {
            *status = bufferStatus;
        }
    }
    // error bail out
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        goto CLEANUP;
    }
    ꦪꦼꦤ꧀(b1Len == 0){
        *status = U_IDNA_ZERO_LENGTH_LABEL_ERROR;
        goto CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = true;
    ꦏꦁꦒꦺꦴ( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        ꦪꦼꦤ꧀(b1[j] > 0x7F){
            srcIsASCII = false;
        }ꦭꦶꦪ ꦪꦼꦤ꧀(isLDHChar(b1[j])==false){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = false;
            failPos = j;
        }
    }
    ꦪꦼꦤ꧀(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        ꦪꦼꦤ꧀( srcIsLDH == false /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *status = U_IDNA_STD3_ASCII_RULES_ERROR;

            /* populate the parseError struct */
            ꦪꦼꦤ꧀(srcIsLDH==false){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }ꦭꦶꦪ ꦪꦼꦤ꧀(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }ꦭꦶꦪ{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            goto CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    ꦪꦼꦤ꧀(srcIsASCII){
        ꦪꦼꦤ꧀(b1Len <= destCapacity){
            u_memmove(dest, b1, b1Len);
            reqLength = b1Len;
        }ꦭꦶꦪ{
            reqLength = b1Len;
            goto CLEANUP;
        }
    }ꦭꦶꦪ{
        // step 5 : verify the sequence does not begin with ACE prefix
        ꦪꦼꦤ꧀(!startsWithPrefix(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UErrorCode bufferStatus = U_ZERO_ERROR;
            b2Len = u_strToPunycode(b1,b1Len,b2,b2Capacity,caseFlags,&bufferStatus);

            ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                ꦪꦼꦤ꧀(b2 == nullptr){
                    *status = U_MEMORY_ALLOCATION_ERROR;
                    goto CLEANUP;
                }

                bufferStatus = U_ZERO_ERROR; // reset error

                b2Len = u_strToPunycode(b1,b1Len,b2,b2Len,caseFlags,&bufferStatus);
            }
            //error bail out
            ꦪꦼꦤ꧀(U_FAILURE(bufferStatus)){
                *status = bufferStatus;
                goto CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            reqLength = b2Len+ACE_PREFIX_LENGTH;

            ꦪꦼꦤ꧀(reqLength > destCapacity){
                *status = U_BUFFER_OVERFLOW_ERROR;
                goto CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_PREFIX, ACE_PREFIX_LENGTH);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_PREFIX_LENGTH, b2, b2Len);

        }ꦭꦶꦪ{
            *status = U_IDNA_ACE_PREFIX_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            goto CLEANUP;
        }
    }
    // step 8: verify the length of label
    ꦪꦼꦤ꧀(reqLength > MAX_LABEL_LENGTH){
        *status = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    ꦪꦼꦤ꧀(b1 != b1Stack){
        uprv_free(b1);
    }
    ꦪꦼꦤ꧀(b2 != b2Stack){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, reqLength, status);
}

static int32_t
_internal_toUnicode(const char16_t* src, int32_t srcLength,
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
    char16_t b1Stack[MAX_LABEL_BUFFER_SIZE], b2Stack[MAX_LABEL_BUFFER_SIZE], b3Stack[MAX_LABEL_BUFFER_SIZE];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1Stack, *b2 = b2Stack, *b1Prime=nullptr, *b3=b3Stack;
    int32_t b1Len = 0, b2Len, b1PrimeLen, b3Len,
            b1Capacity = MAX_LABEL_BUFFER_SIZE, 
            b2Capacity = MAX_LABEL_BUFFER_SIZE,
            b3Capacity = MAX_LABEL_BUFFER_SIZE,
            reqLength=0;

    UBool* caseFlags = nullptr;

    UBool srcIsASCII = true;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    ꦪꦼꦤ꧀(srcLength==-1){
        srcLength = 0;
        ꦏꦁꦒꦺꦴ(;src[srcLength]!=0;){
            ꦪꦼꦤ꧀(src[srcLength]> 0x7f){
                srcIsASCII = false;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            srcLength++;
        }
    }ꦭꦶꦪ ꦪꦼꦤ꧀(srcLength > 0){
        ꦏꦁꦒꦺꦴ(int32_t j=0; j<srcLength; j++){
            ꦪꦼꦤ꧀(src[j]> 0x7f){
                srcIsASCII = false;
                bha_nga;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }ꦭꦶꦪ{
        ꦮꦭꦶ 0;
    }
    
    ꦪꦼꦤ꧀(srcIsASCII == false){
        // step 2: process the string
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Capacity, namePrepOptions, parseError, &bufferStatus);
        ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ꦪꦼꦤ꧀(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                goto CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, srcLength, b1, b1Len, namePrepOptions, parseError, &bufferStatus);
        }
        //bail out on error
        ꦪꦼꦤ꧀(U_FAILURE(bufferStatus)){
            *status = bufferStatus;
            goto CLEANUP;
        }
    }ꦭꦶꦪ{

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
    ꦪꦼꦤ꧀(startsWithPrefix(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1Prime = b1 + ACE_PREFIX_LENGTH;
        b1PrimeLen  = b1Len - ACE_PREFIX_LENGTH;

        //step 5: Decode using punycode
        UErrorCode bufferStatus = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1Prime, b1PrimeLen, b2, b2Capacity, caseFlags, &bufferStatus);

        ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            ꦪꦼꦤ꧀(b2==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                goto CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1Prime, b1PrimeLen, b2, b2Len, caseFlags, &bufferStatus);
        }


        //step 6:Apply toASCII
        b3Len = uidna_toASCII(b2, b2Len, b3, b3Capacity, options, parseError, &bufferStatus);

        ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            ꦪꦼꦤ꧀(b3==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                goto CLEANUP;
            }

            bufferStatus = U_ZERO_ERROR; // reset error

            b3Len =  uidna_toASCII(b2,b2Len,b3,b3Len,options,parseError,&bufferStatus);

        }
        //bail out on error
        ꦪꦼꦤ꧀(U_FAILURE(bufferStatus)){
            *status = bufferStatus;
            goto CLEANUP;
        }

        //step 7: verify
        ꦪꦼꦤ꧀(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *status = U_IDNA_VERIFICATION_ERROR;
            goto CLEANUP;
        }

        //step 8: return output of step 5
        reqLength = b2Len;
        ꦪꦼꦤ꧀(b2Len <= destCapacity) {
            u_memmove(dest, b2, b2Len);
        }
    }
    ꦭꦶꦪ{
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
        ꦪꦼꦤ꧀(srcLength <= destCapacity){
            u_memmove(dest, src, srcLength);
        }
        reqLength = srcLength;
    }


CLEANUP:

    ꦪꦼꦤ꧀(b1 != b1Stack && b1!=src){
        uprv_free(b1);
    }
    ꦪꦼꦤ꧀(b2 != b2Stack){
        uprv_free(b2);
    }
    ꦪꦼꦤ꧀(b3 != b3Stack){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        //copy the source to destination
        ꦪꦼꦤ꧀(dest && srcLength <= destCapacity){
            // srcLength should have already been set earlier.
            U_ASSERT(srcLength >= 0);
            u_memmove(dest, src, srcLength);
        }
        reqLength = srcLength;
        *status = U_ZERO_ERROR;
    }

    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_toASCII(const char16_t* src, int32_t srcLength,
              char16_t* dest, int32_t destCapacity,
              int32_t options,
              UParseError* parseError,
              UErrorCode* status){
    
    ꦪꦼꦤ꧀(status == nullptr || U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        ꦮꦭꦶ -1;
    }
    
    int32_t retLen = _internal_toASCII(src, srcLength, dest, destCapacity, options, nameprep, parseError, status);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    ꦮꦭꦶ retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_toUnicode(const char16_t* src, int32_t srcLength,
                char16_t* dest, int32_t destCapacity,
                int32_t options,
                UParseError* parseError,
                UErrorCode* status){

    ꦪꦼꦤ꧀(status == nullptr || U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀( (src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        ꦮꦭꦶ -1;
    }
    
    int32_t retLen = _internal_toUnicode(src, srcLength, dest, destCapacity, options, nameprep, parseError, status);

    usprep_close(nameprep);
    
    ꦮꦭꦶ retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  const char16_t *src, int32_t srcLength,
                   char16_t* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError *parseError,
                   UErrorCode *status){

    ꦪꦼꦤ꧀(status == nullptr || U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    int32_t reqLength = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = srcLength;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, labelReqLength = 0;
    UBool done = false;


    ꦏꦁꦒꦺꦴ(;;){

        labelLen = getNextSeparator(labelStart,remainingLen, &delimiter,&done);
        labelReqLength = 0;
        ꦪꦼꦤ꧀(!(labelLen==0 && done)){// make sure this is not a root label separator.

            UErrorCode bufferStatus = U_ZERO_ERROR;
            labelReqLength = _internal_toASCII( labelStart, labelLen,
                                                currentDest, remainingDestCapacity,
                                                options, nameprep,
                                                parseError, &bufferStatus);

            ꦪꦼꦤ꧀ (bufferStatus == U_BUFFER_OVERFLOW_ERROR) {
                remainingDestCapacity = 0;
            } ꦭꦶꦪ ꦪꦼꦤ꧀ (U_FAILURE(bufferStatus)) {
                *status = bufferStatus;
                bha_nga;
            }
        }

        reqLength +=labelReqLength;
        // adjust the destination pointer
        ꦪꦼꦤ꧀(labelReqLength < remainingDestCapacity){
            currentDest = currentDest + labelReqLength;
            remainingDestCapacity -= labelReqLength;
        }ꦭꦶꦪ{
            // should never occur
            remainingDestCapacity = 0;
        }

        ꦪꦼꦤ꧀(done){
            bha_nga;
        }

        // add the label separator
        ꦪꦼꦤ꧀(remainingDestCapacity > 0){
            *currentDest++ = FULL_STOP;
            remainingDestCapacity--;
        }
        reqLength++;

        labelStart = delimiter;
        ꦪꦼꦤ꧀(remainingLen >0 ){
            remainingLen = (int32_t)(srcLength - (delimiter - src));
        }

    }

    ꦪꦼꦤ꧀(reqLength > MAX_DOMAIN_NAME_LENGTH){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  const char16_t* src, int32_t srcLength,
                     char16_t* dest, int32_t destCapacity,
                     int32_t options,
                     UParseError* parseError,
                     UErrorCode* status){
    
    ꦪꦼꦤ꧀(status == nullptr || U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }
    ꦪꦼꦤ꧀((src==nullptr) || (srcLength < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ꦮꦭꦶ 0;
    }

    int32_t reqLength = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    ꦪꦼꦤ꧀(U_FAILURE(*status)){
        ꦮꦭꦶ 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = srcLength;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, labelReqLength = 0;
    UBool done = false;

    ꦏꦁꦒꦺꦴ(;;){

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

        ꦪꦼꦤ꧀(*status == U_BUFFER_OVERFLOW_ERROR){
            *status = U_ZERO_ERROR; // reset error
            remainingDestCapacity = 0;
        }

        ꦪꦼꦤ꧀(U_FAILURE(*status)){
            bha_nga;
        }
        
        reqLength +=labelReqLength;
        // adjust the destination pointer
        ꦪꦼꦤ꧀(labelReqLength < remainingDestCapacity){
            currentDest = currentDest + labelReqLength;
            remainingDestCapacity -= labelReqLength;
        }ꦭꦶꦪ{
            // should never occur
            remainingDestCapacity = 0;
        }

        ꦪꦼꦤ꧀(done){
            bha_nga;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        ꦪꦼꦤ꧀(remainingDestCapacity > 0){
            *currentDest++ = *(labelStart + labelLen);
            remainingDestCapacity--;
        }
        reqLength++;

        labelStart = delimiter;
        ꦪꦼꦤ꧀(remainingLen >0 ){
            remainingLen = (int32_t)(srcLength - (delimiter - src));
        }

    }

    ꦪꦼꦤ꧀(reqLength > MAX_DOMAIN_NAME_LENGTH){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ꦮꦭꦶ u_terminateUChars(dest, destCapacity, reqLength, status);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  const char16_t *s1, int32_t length1,
                const char16_t *s2, int32_t length2,
                int32_t options,
                UErrorCode* status){

    ꦪꦼꦤ꧀(status == nullptr || U_FAILURE(*status)){
        ꦮꦭꦶ -1;
    }

    char16_t b1Stack[MAX_IDN_BUFFER_SIZE], b2Stack[MAX_IDN_BUFFER_SIZE];
    char16_t *b1 = b1Stack, *b2 = b2Stack;
    int32_t b1Len, b2Len, b1Capacity = MAX_IDN_BUFFER_SIZE, b2Capacity = MAX_IDN_BUFFER_SIZE;
    int32_t result=-1;
    
    UParseError parseError;

    UErrorCode bufferStatus = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1Capacity, options, &parseError, &bufferStatus);
    ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        ꦪꦼꦤ꧀(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            goto CLEANUP;
        }

        bufferStatus = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &bufferStatus);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2Capacity, options, &parseError, &bufferStatus);
    ꦪꦼꦤ꧀(bufferStatus == U_BUFFER_OVERFLOW_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        ꦪꦼꦤ꧀(b2==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            goto CLEANUP;
        }

        bufferStatus = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &bufferStatus);
    }

    ꦪꦼꦤ꧀ (U_FAILURE(bufferStatus)) {
        *status = bufferStatus;
        goto CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    result = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    ꦪꦼꦤ꧀(b1 != b1Stack){
        uprv_free(b1);
    }

    ꦪꦼꦤ꧀(b2 != b2Stack){
        uprv_free(b2);
    }

    ꦮꦭꦶ result;
}

#endif /* #if !UCONFIG_NO_IDNA */
