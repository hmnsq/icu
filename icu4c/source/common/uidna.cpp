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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_IDNA

#অন্তর্ভাব "unicode/uidna.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/usprep.h"
#অন্তর্ভাব "punycode.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
স্থির ধ্রুবক char16_t ACE_PREFIX[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#define ACE_PREFIX_দৈর্ঘ্য 4

#define MAX_LABEL_দৈর্ঘ্য 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#define MAX_LABEL_বাফার_আকার 100

#define MAX_DOMAIN_NAME_দৈর্ঘ্য 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#define MAX_IDN_বাফার_আকার   MAX_DOMAIN_NAME_দৈর্ঘ্য+1

#define LOWER_CASE_DELTA 0x0020
#define HYPHEN           0x002D
#define FULL_STOP        0x002E
#define CAPITAL_A        0x0041
#define CAPITAL_Z        0x005A

অন্তঃরেখ স্থির char16_t
toASCIILower(char16_t ch){
    যদি(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        ফেরত ch + LOWER_CASE_DELTA;
    }
    ফেরত ch;
}

অন্তঃরেখ স্থির UBool 
startsWithPrefix(ধ্রুবক char16_t* src , int32_t src_দৈর্ঘ্য){
    যদি(src_দৈর্ঘ্য < ACE_PREFIX_দৈর্ঘ্য){
        ফেরত মিথ্যা;
    }

    জন্য(int8_t i=0; i< ACE_PREFIX_দৈর্ঘ্য; i++){
        যদি(toASCIILower(src[i]) != ACE_PREFIX[i]){
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}


অন্তঃরেখ স্থির int32_t
compareCaseInsensitiveASCII(ধ্রুবক char16_t* s1, int32_t s1Len,
                            ধ্রুবক char16_t* s2, int32_t s2Len){
    
    int32_t min_দৈর্ঘ্য;
    int32_t দৈর্ঘ্য_ফলাফল;

    // are we comparing different lengths?
    যদি(s1Len != s2Len) {
        যদি(s1Len < s2Len) {
            min_দৈর্ঘ্য = s1Len;
            দৈর্ঘ্য_ফলাফল = -1;
        } অন্যথা {
            min_দৈর্ঘ্য = s2Len;
            দৈর্ঘ্য_ফলাফল = 1;
        }
    } অন্যথা {
        // ok the lengths are equal
        min_দৈর্ঘ্য = s1Len;
        দৈর্ঘ্য_ফলাফল = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    জন্য(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        যদি(i == min_দৈর্ঘ্য) {
            ফেরত দৈর্ঘ্য_ফলাফল;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        যদি(c1!=c2) {
            rc = static_cast<int32_t>(toASCIILower(c1)) - static_cast<int32_t>(toASCIILower(c2));
            যদি(rc!=0) {
                দৈর্ঘ্য_ফলাফল=rc;
                বিরতি;
            }
        }
    }
    ফেরত দৈর্ঘ্য_ফলাফল;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
স্থির অন্তঃরেখ UBool isLabelSeparator(char16_t ch){
    নির্বাচন(ch){
        ক্ষেত্র 0x002e:
        ক্ষেত্র 0x3002:
        ক্ষেত্র 0xFF0E:
        ক্ষেত্র 0xFF61:
            ফেরত সত্য;
        পূর্বনির্ধারিত:
            ফেরত মিথ্যা;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
স্থির অন্তঃরেখ int32_t
get_পরবর্তী_Separator(char16_t *src, int32_t src_দৈর্ঘ্য,
                 char16_t **limit, UBool *done){
    যদি(src_দৈর্ঘ্য == -1){
        int32_t i;
        জন্য(i=0 ; ;i++){
            যদি(src[i] == 0){
                *limit = src + i; // point to null
                *done = সত্য;
                ফেরত i;
            }
            যদি(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                ফেরত i;
                
            }
        }
    }অন্যথা{
        int32_t i;
        জন্য(i=0;i<src_দৈর্ঘ্য;i++){
            যদি(isLabelSeparator(src[i])){
                *limit = src + (i+1); // go past the delimiter
                ফেরত i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *limit = src+src_দৈর্ঘ্য;
        *done = সত্য;

        ফেরত i;
    }
}
স্থির অন্তঃরেখ UBool isLDHChar(char16_t ch){
    // high runner case
    যদি(ch>0x007A){
        ফেরত মিথ্যা;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    যদি( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        ফেরত সত্য;
    }
    ফেরত মিথ্যা;
}

স্থির int32_t 
_internal_toASCII(ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
                  char16_t* dest, int32_t destCapacity,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UErrorCode* status)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_স্তূপ[MAX_LABEL_বাফার_আকার], b2_স্তূপ[MAX_LABEL_বাফার_আকার];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_স্তূপ, *b2 = b2_স্তূপ;
    int32_t b1Len=0, b2Len, 
            b1Capacity = MAX_LABEL_বাফার_আকার, 
            b2Capacity = MAX_LABEL_বাফার_আকার ,
            req_দৈর্ঘ্য=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = nullptr;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = সত্য;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = সত্য; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_RULES) != 0);

    int32_t failPos = -1;
    
    যদি(src_দৈর্ঘ্য == -1){
        src_দৈর্ঘ্য = u_strlen(src);
    }
    
    যদি(src_দৈর্ঘ্য > b1Capacity){
        b1 = static_cast<char16_t*>(uprv_malloc(src_দৈর্ঘ্য * U_SIZEOF_UCHAR));
        যদি(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            যাও CLEANUP;
        }
        b1Capacity = src_দৈর্ঘ্য;
    }

    // step 1 
    জন্য( j=0;j<src_দৈর্ঘ্য;j++){
        যদি(src[j] > 0x7F){
            srcIsASCII = মিথ্যা;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    যদি(srcIsASCII == মিথ্যা){
        
        // step 2    
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_দৈর্ঘ্য, b1, b1Capacity, namePrepOptions, parseError, &বাফার_Status);

        যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            যদি(b1 != b1_স্তূপ){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            যদি(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                যাও CLEANUP;
            }

            বাফার_Status = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_দৈর্ঘ্য, b1, b1Len, namePrepOptions, parseError, &বাফার_Status);
        }
        যদি (U_FAILURE(বাফার_Status)) {
            *status = বাফার_Status;
        }
    }
    // error bail out
    যদি(U_FAILURE(*status)){
        যাও CLEANUP;
    }
    যদি(b1Len == 0){
        *status = U_IDNA_ZERO_দৈর্ঘ্য_LABEL_ERROR;
        যাও CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = সত্য;
    জন্য( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        যদি(b1[j] > 0x7F){
            srcIsASCII = মিথ্যা;
        }অন্যথা যদি(isLDHChar(b1[j])==মিথ্যা){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = মিথ্যা;
            failPos = j;
        }
    }
    যদি(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        যদি( srcIsLDH == মিথ্যা /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *status = U_IDNA_STD3_ASCII_RULES_ERROR;

            /* populate the parseError struct */
            যদি(srcIsLDH==মিথ্যা){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }অন্যথা যদি(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }অন্যথা{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            যাও CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    যদি(srcIsASCII){
        যদি(b1Len <= destCapacity){
            u_memmove(dest, b1, b1Len);
            req_দৈর্ঘ্য = b1Len;
        }অন্যথা{
            req_দৈর্ঘ্য = b1Len;
            যাও CLEANUP;
        }
    }অন্যথা{
        // step 5 : verify the sequence does not begin with ACE prefix
        যদি(!startsWithPrefix(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UErrorCode বাফার_Status = U_ZERO_ERROR;
            b2Len = u_strToPunycode(b1,b1Len,b2,b2Capacity,caseFlags,&বাফার_Status);

            যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                যদি(b2 == nullptr){
                    *status = U_MEMORY_ALLOCATION_ERROR;
                    যাও CLEANUP;
                }

                বাফার_Status = U_ZERO_ERROR; // reset error

                b2Len = u_strToPunycode(b1,b1Len,b2,b2Len,caseFlags,&বাফার_Status);
            }
            //error bail out
            যদি(U_FAILURE(বাফার_Status)){
                *status = বাফার_Status;
                যাও CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_দৈর্ঘ্য = b2Len+ACE_PREFIX_দৈর্ঘ্য;

            যদি(req_দৈর্ঘ্য > destCapacity){
                *status = U_বাফার_OVERFLOW_ERROR;
                যাও CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_PREFIX, ACE_PREFIX_দৈর্ঘ্য);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_PREFIX_দৈর্ঘ্য, b2, b2Len);

        }অন্যথা{
            *status = U_IDNA_ACE_PREFIX_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            যাও CLEANUP;
        }
    }
    // step 8: verify the length of label
    যদি(req_দৈর্ঘ্য > MAX_LABEL_দৈর্ঘ্য){
        *status = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    যদি(b1 != b1_স্তূপ){
        uprv_free(b1);
    }
    যদি(b2 != b2_স্তূপ){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    ফেরত u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, status);
}

স্থির int32_t
_internal_toUnicode(ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
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
    char16_t b1_স্তূপ[MAX_LABEL_বাফার_আকার], b2_স্তূপ[MAX_LABEL_বাফার_আকার], b3_স্তূপ[MAX_LABEL_বাফার_আকার];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_স্তূপ, *b2 = b2_স্তূপ, *b1_মৌলিক=nullptr, *b3=b3_স্তূপ;
    int32_t b1Len = 0, b2Len, b1_মৌলিক_Len, b3Len,
            b1Capacity = MAX_LABEL_বাফার_আকার, 
            b2Capacity = MAX_LABEL_বাফার_আকার,
            b3Capacity = MAX_LABEL_বাফার_আকার,
            req_দৈর্ঘ্য=0;

    UBool* caseFlags = nullptr;

    UBool srcIsASCII = সত্য;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    যদি(src_দৈর্ঘ্য==-1){
        src_দৈর্ঘ্য = 0;
        জন্য(;src[src_দৈর্ঘ্য]!=0;){
            যদি(src[src_দৈর্ঘ্য]> 0x7f){
                srcIsASCII = মিথ্যা;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_দৈর্ঘ্য++;
        }
    }অন্যথা যদি(src_দৈর্ঘ্য > 0){
        জন্য(int32_t j=0; j<src_দৈর্ঘ্য; j++){
            যদি(src[j]> 0x7f){
                srcIsASCII = মিথ্যা;
                বিরতি;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }অন্যথা{
        ফেরত 0;
    }
    
    যদি(srcIsASCII == মিথ্যা){
        // step 2: process the string
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_দৈর্ঘ্য, b1, b1Capacity, namePrepOptions, parseError, &বাফার_Status);
        যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            যদি(b1==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                যাও CLEANUP;
            }

            বাফার_Status = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_দৈর্ঘ্য, b1, b1Len, namePrepOptions, parseError, &বাফার_Status);
        }
        //bail out on error
        যদি(U_FAILURE(বাফার_Status)){
            *status = বাফার_Status;
            যাও CLEANUP;
        }
    }অন্যথা{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_দৈর্ঘ্য;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    যদি(startsWithPrefix(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_মৌলিক = b1 + ACE_PREFIX_দৈর্ঘ্য;
        b1_মৌলিক_Len  = b1Len - ACE_PREFIX_দৈর্ঘ্য;

        //step 5: Decode using punycode
        UErrorCode বাফার_Status = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_মৌলিক, b1_মৌলিক_Len, b2, b2Capacity, caseFlags, &বাফার_Status);

        যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            যদি(b2==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                যাও CLEANUP;
            }

            বাফার_Status = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_মৌলিক, b1_মৌলিক_Len, b2, b2Len, caseFlags, &বাফার_Status);
        }


        //step 6:Apply toASCII
        b3Len = uidna_toASCII(b2, b2Len, b3, b3Capacity, options, parseError, &বাফার_Status);

        যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            যদি(b3==nullptr){
                *status = U_MEMORY_ALLOCATION_ERROR;
                যাও CLEANUP;
            }

            বাফার_Status = U_ZERO_ERROR; // reset error

            b3Len =  uidna_toASCII(b2,b2Len,b3,b3Len,options,parseError,&বাফার_Status);

        }
        //bail out on error
        যদি(U_FAILURE(বাফার_Status)){
            *status = বাফার_Status;
            যাও CLEANUP;
        }

        //step 7: verify
        যদি(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *status = U_IDNA_VERIFICATION_ERROR;
            যাও CLEANUP;
        }

        //step 8: return output of step 5
        req_দৈর্ঘ্য = b2Len;
        যদি(b2Len <= destCapacity) {
            u_memmove(dest, b2, b2Len);
        }
    }
    অন্যথা{
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
        যদি(src_দৈর্ঘ্য <= destCapacity){
            u_memmove(dest, src, src_দৈর্ঘ্য);
        }
        req_দৈর্ঘ্য = src_দৈর্ঘ্য;
    }


CLEANUP:

    যদি(b1 != b1_স্তূপ && b1!=src){
        uprv_free(b1);
    }
    যদি(b2 != b2_স্তূপ){
        uprv_free(b2);
    }
    যদি(b3 != b3_স্তূপ){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    যদি(U_FAILURE(*status)){
        //copy the source to destination
        যদি(dest && src_দৈর্ঘ্য <= destCapacity){
            // srcLength should have already been set earlier.
            U_ASSERT(src_দৈর্ঘ্য >= 0);
            u_memmove(dest, src, src_দৈর্ঘ্য);
        }
        req_দৈর্ঘ্য = src_দৈর্ঘ্য;
        *status = U_ZERO_ERROR;
    }

    ফেরত u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, status);
}

U_CAPI int32_t U_EXPORT2
uidna_toASCII(ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
              char16_t* dest, int32_t destCapacity,
              int32_t options,
              UParseError* parseError,
              UErrorCode* status){
    
    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত 0;
    }
    যদি((src==nullptr) || (src_দৈর্ঘ্য < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    যদি(U_FAILURE(*status)){
        ফেরত -1;
    }
    
    int32_t retLen = _internal_toASCII(src, src_দৈর্ঘ্য, dest, destCapacity, options, nameprep, parseError, status);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    ফেরত retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_toUnicode(ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
                char16_t* dest, int32_t destCapacity,
                int32_t options,
                UParseError* parseError,
                UErrorCode* status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত 0;
    }
    যদি( (src==nullptr) || (src_দৈর্ঘ্য < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    যদি(U_FAILURE(*status)){
        ফেরত -1;
    }
    
    int32_t retLen = _internal_toUnicode(src, src_দৈর্ঘ্য, dest, destCapacity, options, nameprep, parseError, status);

    usprep_close(nameprep);
    
    ফেরত retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                   char16_t* dest, int32_t destCapacity,
                   int32_t options,
                   UParseError *parseError,
                   UErrorCode *status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত 0;
    }
    যদি((src==nullptr) || (src_দৈর্ঘ্য < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    int32_t req_দৈর্ঘ্য = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    যদি(U_FAILURE(*status)){
        ফেরত 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_দৈর্ঘ্য;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, label_Req_দৈর্ঘ্য = 0;
    UBool done = মিথ্যা;


    জন্য(;;){

        labelLen = get_পরবর্তী_Separator(labelStart,remainingLen, &delimiter,&done);
        label_Req_দৈর্ঘ্য = 0;
        যদি(!(labelLen==0 && done)){// make sure this is not a root label separator.

            UErrorCode বাফার_Status = U_ZERO_ERROR;
            label_Req_দৈর্ঘ্য = _internal_toASCII( labelStart, labelLen,
                                                currentDest, remainingDestCapacity,
                                                options, nameprep,
                                                parseError, &বাফার_Status);

            যদি (বাফার_Status == U_বাফার_OVERFLOW_ERROR) {
                remainingDestCapacity = 0;
            } অন্যথা যদি (U_FAILURE(বাফার_Status)) {
                *status = বাফার_Status;
                বিরতি;
            }
        }

        req_দৈর্ঘ্য +=label_Req_দৈর্ঘ্য;
        // adjust the destination pointer
        যদি(label_Req_দৈর্ঘ্য < remainingDestCapacity){
            currentDest = currentDest + label_Req_দৈর্ঘ্য;
            remainingDestCapacity -= label_Req_দৈর্ঘ্য;
        }অন্যথা{
            // should never occur
            remainingDestCapacity = 0;
        }

        যদি(done){
            বিরতি;
        }

        // add the label separator
        যদি(remainingDestCapacity > 0){
            *currentDest++ = FULL_STOP;
            remainingDestCapacity--;
        }
        req_দৈর্ঘ্য++;

        labelStart = delimiter;
        যদি(remainingLen >0 ){
            remainingLen = (int32_t)(src_দৈর্ঘ্য - (delimiter - src));
        }

    }

    যদি(req_দৈর্ঘ্য > MAX_DOMAIN_NAME_দৈর্ঘ্য){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ফেরত u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, status);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ধ্রুবক char16_t* src, int32_t src_দৈর্ঘ্য,
                     char16_t* dest, int32_t destCapacity,
                     int32_t options,
                     UParseError* parseError,
                     UErrorCode* status){
    
    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত 0;
    }
    যদি((src==nullptr) || (src_দৈর্ঘ্য < -1) || (destCapacity<0) || (!dest && destCapacity > 0)){
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    int32_t req_দৈর্ঘ্য = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, status);
    
    যদি(U_FAILURE(*status)){
        ফেরত 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* labelStart = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t remainingLen = src_দৈর্ঘ্য;
    int32_t remainingDestCapacity = destCapacity;
    int32_t labelLen = 0, label_Req_দৈর্ঘ্য = 0;
    UBool done = মিথ্যা;

    জন্য(;;){

        labelLen = get_পরবর্তী_Separator(labelStart,remainingLen, &delimiter,&done);
        
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
        
        label_Req_দৈর্ঘ্য = _internal_toUnicode(labelStart, labelLen, 
                                             currentDest, remainingDestCapacity, 
                                             options, nameprep, 
                                             parseError, status);

        যদি(*status == U_বাফার_OVERFLOW_ERROR){
            *status = U_ZERO_ERROR; // reset error
            remainingDestCapacity = 0;
        }

        যদি(U_FAILURE(*status)){
            বিরতি;
        }
        
        req_দৈর্ঘ্য +=label_Req_দৈর্ঘ্য;
        // adjust the destination pointer
        যদি(label_Req_দৈর্ঘ্য < remainingDestCapacity){
            currentDest = currentDest + label_Req_দৈর্ঘ্য;
            remainingDestCapacity -= label_Req_দৈর্ঘ্য;
        }অন্যথা{
            // should never occur
            remainingDestCapacity = 0;
        }

        যদি(done){
            বিরতি;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        যদি(remainingDestCapacity > 0){
            *currentDest++ = *(labelStart + labelLen);
            remainingDestCapacity--;
        }
        req_দৈর্ঘ্য++;

        labelStart = delimiter;
        যদি(remainingLen >0 ){
            remainingLen = (int32_t)(src_দৈর্ঘ্য - (delimiter - src));
        }

    }

    যদি(req_দৈর্ঘ্য > MAX_DOMAIN_NAME_দৈর্ঘ্য){
        *status = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ফেরত u_terminateUChars(dest, destCapacity, req_দৈর্ঘ্য, status);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ধ্রুবক char16_t *s1, int32_t length1,
                ধ্রুবক char16_t *s2, int32_t length2,
                int32_t options,
                UErrorCode* status){

    যদি(status == nullptr || U_FAILURE(*status)){
        ফেরত -1;
    }

    char16_t b1_স্তূপ[MAX_IDN_বাফার_আকার], b2_স্তূপ[MAX_IDN_বাফার_আকার];
    char16_t *b1 = b1_স্তূপ, *b2 = b2_স্তূপ;
    int32_t b1Len, b2Len, b1Capacity = MAX_IDN_বাফার_আকার, b2Capacity = MAX_IDN_বাফার_আকার;
    int32_t ফলাফল=-1;
    
    UParseError parseError;

    UErrorCode বাফার_Status = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1Capacity, options, &parseError, &বাফার_Status);
    যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        যদি(b1==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            যাও CLEANUP;
        }

        বাফার_Status = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &বাফার_Status);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2Capacity, options, &parseError, &বাফার_Status);
    যদি(বাফার_Status == U_বাফার_OVERFLOW_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        যদি(b2==nullptr){
            *status = U_MEMORY_ALLOCATION_ERROR;
            যাও CLEANUP;
        }

        বাফার_Status = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &বাফার_Status);
    }

    যদি (U_FAILURE(বাফার_Status)) {
        *status = বাফার_Status;
        যাও CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    ফলাফল = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    যদি(b1 != b1_স্তূপ){
        uprv_free(b1);
    }

    যদি(b2 != b2_স্তূপ){
        uprv_free(b2);
    }

    ফেরত ফলাফল;
}

#endif /* #if !UCONFIG_NO_IDNA */
