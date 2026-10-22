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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_IDNA

#ضمن "unicode/uidna.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/usprep.h"
#ضمن "punycode.h"
#ضمن "ustr_imp.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"
#ضمن "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
ساكن ثابت char16_t ACE_مختاړی[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#تعریف_کړه ACE_مختاړی_اوږدتیا 4

#تعریف_کړه MAX_LABEL_اوږدتیا 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#تعریف_کړه MAX_LABEL_بفر_اندازه 100

#تعریف_کړه MAX_DOMAIN_NAME_اوږدتیا 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#تعریف_کړه MAX_IDN_بفر_اندازه   MAX_DOMAIN_NAME_اوږدتیا+1

#تعریف_کړه ښکتنی_CASE_DELTA 0x0020
#تعریف_کړه HYPHEN           0x002D
#تعریف_کړه ډک_STOP        0x002E
#تعریف_کړه CAPITAL_A        0x0041
#تعریف_کړه CAPITAL_Z        0x005A

مضمن ساكن char16_t
منزل_ASCIILower(char16_t ch){
    إذا(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        أرجع ch + ښکتنی_CASE_DELTA;
    }
    أرجع ch;
}

مضمن ساكن UBool 
starts_With_مختاړی(ثابت char16_t* src , int32_t src_اوږدتیا){
    إذا(src_اوږدتیا < ACE_مختاړی_اوږدتیا){
        أرجع خطأ;
    }

    لكل(int8_t i=0; i< ACE_مختاړی_اوږدتیا; i++){
        إذا(منزل_ASCIILower(src[i]) != ACE_مختاړی[i]){
            أرجع خطأ;
        }
    }
    أرجع ریښتیا;
}


مضمن ساكن int32_t
compareCaseInsensitiveASCII(ثابت char16_t* s1, int32_t s1Len,
                            ثابت char16_t* s2, int32_t s2Len){
    
    int32_t min_اوږدتیا;
    int32_t اوږدتیا_پایله;

    // are we comparing different lengths?
    إذا(s1Len != s2Len) {
        إذا(s1Len < s2Len) {
            min_اوږدتیا = s1Len;
            اوږدتیا_پایله = -1;
        } وإلا {
            min_اوږدتیا = s2Len;
            اوږدتیا_پایله = 1;
        }
    } وإلا {
        // ok the lengths are equal
        min_اوږدتیا = s1Len;
        اوږدتیا_پایله = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    لكل(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        إذا(i == min_اوږدتیا) {
            أرجع اوږدتیا_پایله;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        إذا(c1!=c2) {
            rc = static_cast<int32_t>(منزل_ASCIILower(c1)) - static_cast<int32_t>(منزل_ASCIILower(c2));
            إذا(rc!=0) {
                اوږدتیا_پایله=rc;
                اكسر;
            }
        }
    }
    أرجع اوږدتیا_پایله;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
ساكن مضمن UBool isLabelSeparator(char16_t ch){
    بدل(ch){
        حالة 0x002e:
        حالة 0x3002:
        حالة 0xFF0E:
        حالة 0xFF61:
            أرجع ریښتیا;
        افتراضي:
            أرجع خطأ;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
ساكن مضمن int32_t
get_بل_Separator(char16_t *src, int32_t src_اوږدتیا,
                 char16_t **حد, UBool *بشپړ){
    إذا(src_اوږدتیا == -1){
        int32_t i;
        لكل(i=0 ; ;i++){
            إذا(src[i] == 0){
                *حد = src + i; // point to null
                *بشپړ = ریښتیا;
                أرجع i;
            }
            إذا(isLabelSeparator(src[i])){
                *حد = src + (i+1); // go past the delimiter
                أرجع i;
                
            }
        }
    }وإلا{
        int32_t i;
        لكل(i=0;i<src_اوږدتیا;i++){
            إذا(isLabelSeparator(src[i])){
                *حد = src + (i+1); // go past the delimiter
                أرجع i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *حد = src+src_اوږدتیا;
        *بشپړ = ریښتیا;

        أرجع i;
    }
}
ساكن مضمن UBool isLDHChar(char16_t ch){
    // high runner case
    إذا(ch>0x007A){
        أرجع خطأ;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    إذا( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        أرجع ریښتیا;
    }
    أرجع خطأ;
}

ساكن int32_t 
دننی_منزل_ASCII(ثابت char16_t* src, int32_t src_اوږدتیا,
                  char16_t* dest, int32_t dest_ظرفیت,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_کوډ* حالت)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ډېری[MAX_LABEL_بفر_اندازه], b2_ډېری[MAX_LABEL_بفر_اندازه];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ډېری, *b2 = b2_ډېری;
    int32_t b1Len=0, b2Len, 
            b1_ظرفیت = MAX_LABEL_بفر_اندازه, 
            b2_ظرفیت = MAX_LABEL_بفر_اندازه ,
            req_اوږدتیا=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = تش_اشاره;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = ریښتیا;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = ریښتیا; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_قواعد) != 0);

    int32_t failPos = -1;
    
    إذا(src_اوږدتیا == -1){
        src_اوږدتیا = u_strlen(src);
    }
    
    إذا(src_اوږدتیا > b1_ظرفیت){
        b1 = static_cast<char16_t*>(uprv_malloc(src_اوږدتیا * U_SIZEOF_UCHAR));
        إذا(b1==تش_اشاره){
            *حالت = U_حافظه_ویش_ERROR;
            اذهب CLEANUP;
        }
        b1_ظرفیت = src_اوږدتیا;
    }

    // step 1 
    لكل( j=0;j<src_اوږدتیا;j++){
        إذا(src[j] > 0x7F){
            srcIsASCII = خطأ;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    إذا(srcIsASCII == خطأ){
        
        // step 2    
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_اوږدتیا, b1, b1_ظرفیت, namePrepOptions, parseError, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            إذا(b1 != b1_ډېری){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            إذا(b1==تش_اشاره){
                *حالت = U_حافظه_ویش_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_اوږدتیا, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
        }
        إذا (U_FAILURE(بفر_حالت)) {
            *حالت = بفر_حالت;
        }
    }
    // error bail out
    إذا(U_FAILURE(*حالت)){
        اذهب CLEANUP;
    }
    إذا(b1Len == 0){
        *حالت = U_IDNA_ZERO_اوږدتیا_LABEL_ERROR;
        اذهب CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = ریښتیا;
    لكل( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        إذا(b1[j] > 0x7F){
            srcIsASCII = خطأ;
        }وإلا إذا(isLDHChar(b1[j])==خطأ){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = خطأ;
            failPos = j;
        }
    }
    إذا(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        إذا( srcIsLDH == خطأ /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *حالت = U_IDNA_STD3_ASCII_قواعد_ERROR;

            /* populate the parseError struct */
            إذا(srcIsLDH==خطأ){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }وإلا إذا(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }وإلا{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            اذهب CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    إذا(srcIsASCII){
        إذا(b1Len <= dest_ظرفیت){
            u_memmove(dest, b1, b1Len);
            req_اوږدتیا = b1Len;
        }وإلا{
            req_اوږدتیا = b1Len;
            اذهب CLEANUP;
        }
    }وإلا{
        // step 5 : verify the sequence does not begin with ACE prefix
        إذا(!starts_With_مختاړی(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_کوډ بفر_حالت = U_ZERO_ERROR;
            b2Len = u_str_منزل_Punycode(b1,b1Len,b2,b2_ظرفیت,caseFlags,&بفر_حالت);

            إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                إذا(b2 == تش_اشاره){
                    *حالت = U_حافظه_ویش_ERROR;
                    اذهب CLEANUP;
                }

                بفر_حالت = U_ZERO_ERROR; // reset error

                b2Len = u_str_منزل_Punycode(b1,b1Len,b2,b2Len,caseFlags,&بفر_حالت);
            }
            //error bail out
            إذا(U_FAILURE(بفر_حالت)){
                *حالت = بفر_حالت;
                اذهب CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_اوږدتیا = b2Len+ACE_مختاړی_اوږدتیا;

            إذا(req_اوږدتیا > dest_ظرفیت){
                *حالت = U_بفر_ډکېدنه_ERROR;
                اذهب CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_مختاړی, ACE_مختاړی_اوږدتیا);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_مختاړی_اوږدتیا, b2, b2Len);

        }وإلا{
            *حالت = U_IDNA_ACE_مختاړی_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            اذهب CLEANUP;
        }
    }
    // step 8: verify the length of label
    إذا(req_اوږدتیا > MAX_LABEL_اوږدتیا){
        *حالت = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    إذا(b1 != b1_ډېری){
        uprv_free(b1);
    }
    إذا(b2 != b2_ډېری){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    أرجع u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, حالت);
}

ساكن int32_t
دننی_منزل_Unicode(ثابت char16_t* src, int32_t src_اوږدتیا,
                    char16_t* dest, int32_t dest_ظرفیت,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_کوډ* حالت)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ډېری[MAX_LABEL_بفر_اندازه], b2_ډېری[MAX_LABEL_بفر_اندازه], b3_ډېری[MAX_LABEL_بفر_اندازه];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ډېری, *b2 = b2_ډېری, *b1_اولي=تش_اشاره, *b3=b3_ډېری;
    int32_t b1Len = 0, b2Len, b1_اولي_Len, b3Len,
            b1_ظرفیت = MAX_LABEL_بفر_اندازه, 
            b2_ظرفیت = MAX_LABEL_بفر_اندازه,
            b3_ظرفیت = MAX_LABEL_بفر_اندازه,
            req_اوږدتیا=0;

    UBool* caseFlags = تش_اشاره;

    UBool srcIsASCII = ریښتیا;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    إذا(src_اوږدتیا==-1){
        src_اوږدتیا = 0;
        لكل(;src[src_اوږدتیا]!=0;){
            إذا(src[src_اوږدتیا]> 0x7f){
                srcIsASCII = خطأ;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_اوږدتیا++;
        }
    }وإلا إذا(src_اوږدتیا > 0){
        لكل(int32_t j=0; j<src_اوږدتیا; j++){
            إذا(src[j]> 0x7f){
                srcIsASCII = خطأ;
                اكسر;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }وإلا{
        أرجع 0;
    }
    
    إذا(srcIsASCII == خطأ){
        // step 2: process the string
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_اوږدتیا, b1, b1_ظرفیت, namePrepOptions, parseError, &بفر_حالت);
        إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            إذا(b1==تش_اشاره){
                *حالت = U_حافظه_ویش_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_اوږدتیا, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
        }
        //bail out on error
        إذا(U_FAILURE(بفر_حالت)){
            *حالت = بفر_حالت;
            اذهب CLEANUP;
        }
    }وإلا{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_اوږدتیا;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    إذا(starts_With_مختاړی(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_اولي = b1 + ACE_مختاړی_اوږدتیا;
        b1_اولي_Len  = b1Len - ACE_مختاړی_اوږدتیا;

        //step 5: Decode using punycode
        UError_کوډ بفر_حالت = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_اولي, b1_اولي_Len, b2, b2_ظرفیت, caseFlags, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            إذا(b2==تش_اشاره){
                *حالت = U_حافظه_ویش_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_اولي, b1_اولي_Len, b2, b2Len, caseFlags, &بفر_حالت);
        }


        //step 6:Apply toASCII
        b3Len = uidna_منزل_ASCII(b2, b2Len, b3, b3_ظرفیت, options, parseError, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            إذا(b3==تش_اشاره){
                *حالت = U_حافظه_ویش_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b3Len =  uidna_منزل_ASCII(b2,b2Len,b3,b3Len,options,parseError,&بفر_حالت);

        }
        //bail out on error
        إذا(U_FAILURE(بفر_حالت)){
            *حالت = بفر_حالت;
            اذهب CLEANUP;
        }

        //step 7: verify
        إذا(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *حالت = U_IDNA_VERIFICATION_ERROR;
            اذهب CLEANUP;
        }

        //step 8: return output of step 5
        req_اوږدتیا = b2Len;
        إذا(b2Len <= dest_ظرفیت) {
            u_memmove(dest, b2, b2Len);
        }
    }
    وإلا{
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
        إذا(src_اوږدتیا <= dest_ظرفیت){
            u_memmove(dest, src, src_اوږدتیا);
        }
        req_اوږدتیا = src_اوږدتیا;
    }


CLEANUP:

    إذا(b1 != b1_ډېری && b1!=src){
        uprv_free(b1);
    }
    إذا(b2 != b2_ډېری){
        uprv_free(b2);
    }
    إذا(b3 != b3_ډېری){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    إذا(U_FAILURE(*حالت)){
        //copy the source to destination
        إذا(dest && src_اوږدتیا <= dest_ظرفیت){
            // srcLength should have already been set earlier.
            U_ASSERT(src_اوږدتیا >= 0);
            u_memmove(dest, src, src_اوږدتیا);
        }
        req_اوږدتیا = src_اوږدتیا;
        *حالت = U_ZERO_ERROR;
    }

    أرجع u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_منزل_ASCII(ثابت char16_t* src, int32_t src_اوږدتیا,
              char16_t* dest, int32_t dest_ظرفیت,
              int32_t options,
              UParseError* parseError,
              UError_کوډ* حالت){
    
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==تش_اشاره) || (src_اوږدتیا < -1) || (dest_ظرفیت<0) || (!dest && dest_ظرفیت > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع -1;
    }
    
    int32_t retLen = دننی_منزل_ASCII(src, src_اوږدتیا, dest, dest_ظرفیت, options, nameprep, parseError, حالت);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    أرجع retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_منزل_Unicode(ثابت char16_t* src, int32_t src_اوږدتیا,
                char16_t* dest, int32_t dest_ظرفیت,
                int32_t options,
                UParseError* parseError,
                UError_کوډ* حالت){

    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا( (src==تش_اشاره) || (src_اوږدتیا < -1) || (dest_ظرفیت<0) || (!dest && dest_ظرفیت > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع -1;
    }
    
    int32_t retLen = دننی_منزل_Unicode(src, src_اوږدتیا, dest, dest_ظرفیت, options, nameprep, parseError, حالت);

    usprep_close(nameprep);
    
    أرجع retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ثابت char16_t *src, int32_t src_اوږدتیا,
                   char16_t* dest, int32_t dest_ظرفیت,
                   int32_t options,
                   UParseError *parseError,
                   UError_کوډ *حالت){

    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==تش_اشاره) || (src_اوږدتیا < -1) || (dest_ظرفیت<0) || (!dest && dest_ظرفیت > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t req_اوږدتیا = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_پیل = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t پاتې_شوی_Len = src_اوږدتیا;
    int32_t پاتې_شوی_Dest_ظرفیت = dest_ظرفیت;
    int32_t labelLen = 0, label_Req_اوږدتیا = 0;
    UBool بشپړ = خطأ;


    لكل(;;){

        labelLen = get_بل_Separator(label_پیل,پاتې_شوی_Len, &delimiter,&بشپړ);
        label_Req_اوږدتیا = 0;
        إذا(!(labelLen==0 && بشپړ)){// make sure this is not a root label separator.

            UError_کوډ بفر_حالت = U_ZERO_ERROR;
            label_Req_اوږدتیا = دننی_منزل_ASCII( label_پیل, labelLen,
                                                currentDest, پاتې_شوی_Dest_ظرفیت,
                                                options, nameprep,
                                                parseError, &بفر_حالت);

            إذا (بفر_حالت == U_بفر_ډکېدنه_ERROR) {
                پاتې_شوی_Dest_ظرفیت = 0;
            } وإلا إذا (U_FAILURE(بفر_حالت)) {
                *حالت = بفر_حالت;
                اكسر;
            }
        }

        req_اوږدتیا +=label_Req_اوږدتیا;
        // adjust the destination pointer
        إذا(label_Req_اوږدتیا < پاتې_شوی_Dest_ظرفیت){
            currentDest = currentDest + label_Req_اوږدتیا;
            پاتې_شوی_Dest_ظرفیت -= label_Req_اوږدتیا;
        }وإلا{
            // should never occur
            پاتې_شوی_Dest_ظرفیت = 0;
        }

        إذا(بشپړ){
            اكسر;
        }

        // add the label separator
        إذا(پاتې_شوی_Dest_ظرفیت > 0){
            *currentDest++ = ډک_STOP;
            پاتې_شوی_Dest_ظرفیت--;
        }
        req_اوږدتیا++;

        label_پیل = delimiter;
        إذا(پاتې_شوی_Len >0 ){
            پاتې_شوی_Len = (int32_t)(src_اوږدتیا - (delimiter - src));
        }

    }

    إذا(req_اوږدتیا > MAX_DOMAIN_NAME_اوږدتیا){
        *حالت = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    أرجع u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ثابت char16_t* src, int32_t src_اوږدتیا,
                     char16_t* dest, int32_t dest_ظرفیت,
                     int32_t options,
                     UParseError* parseError,
                     UError_کوډ* حالت){
    
    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==تش_اشاره) || (src_اوږدتیا < -1) || (dest_ظرفیت<0) || (!dest && dest_ظرفیت > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t req_اوږدتیا = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_پیل = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t پاتې_شوی_Len = src_اوږدتیا;
    int32_t پاتې_شوی_Dest_ظرفیت = dest_ظرفیت;
    int32_t labelLen = 0, label_Req_اوږدتیا = 0;
    UBool بشپړ = خطأ;

    لكل(;;){

        labelLen = get_بل_Separator(label_پیل,پاتې_شوی_Len, &delimiter,&بشپړ);
        
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
        
        label_Req_اوږدتیا = دننی_منزل_Unicode(label_پیل, labelLen, 
                                             currentDest, پاتې_شوی_Dest_ظرفیت, 
                                             options, nameprep, 
                                             parseError, حالت);

        إذا(*حالت == U_بفر_ډکېدنه_ERROR){
            *حالت = U_ZERO_ERROR; // reset error
            پاتې_شوی_Dest_ظرفیت = 0;
        }

        إذا(U_FAILURE(*حالت)){
            اكسر;
        }
        
        req_اوږدتیا +=label_Req_اوږدتیا;
        // adjust the destination pointer
        إذا(label_Req_اوږدتیا < پاتې_شوی_Dest_ظرفیت){
            currentDest = currentDest + label_Req_اوږدتیا;
            پاتې_شوی_Dest_ظرفیت -= label_Req_اوږدتیا;
        }وإلا{
            // should never occur
            پاتې_شوی_Dest_ظرفیت = 0;
        }

        إذا(بشپړ){
            اكسر;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        إذا(پاتې_شوی_Dest_ظرفیت > 0){
            *currentDest++ = *(label_پیل + labelLen);
            پاتې_شوی_Dest_ظرفیت--;
        }
        req_اوږدتیا++;

        label_پیل = delimiter;
        إذا(پاتې_شوی_Len >0 ){
            پاتې_شوی_Len = (int32_t)(src_اوږدتیا - (delimiter - src));
        }

    }

    إذا(req_اوږدتیا > MAX_DOMAIN_NAME_اوږدتیا){
        *حالت = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    أرجع u_terminateUChars(dest, dest_ظرفیت, req_اوږدتیا, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                int32_t options,
                UError_کوډ* حالت){

    إذا(حالت == تش_اشاره || U_FAILURE(*حالت)){
        أرجع -1;
    }

    char16_t b1_ډېری[MAX_IDN_بفر_اندازه], b2_ډېری[MAX_IDN_بفر_اندازه];
    char16_t *b1 = b1_ډېری, *b2 = b2_ډېری;
    int32_t b1Len, b2Len, b1_ظرفیت = MAX_IDN_بفر_اندازه, b2_ظرفیت = MAX_IDN_بفر_اندازه;
    int32_t پایله=-1;
    
    UParseError parseError;

    UError_کوډ بفر_حالت = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_ظرفیت, options, &parseError, &بفر_حالت);
    إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        إذا(b1==تش_اشاره){
            *حالت = U_حافظه_ویش_ERROR;
            اذهب CLEANUP;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &بفر_حالت);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_ظرفیت, options, &parseError, &بفر_حالت);
    إذا(بفر_حالت == U_بفر_ډکېدنه_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        إذا(b2==تش_اشاره){
            *حالت = U_حافظه_ویش_ERROR;
            اذهب CLEANUP;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &بفر_حالت);
    }

    إذا (U_FAILURE(بفر_حالت)) {
        *حالت = بفر_حالت;
        اذهب CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    پایله = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    إذا(b1 != b1_ډېری){
        uprv_free(b1);
    }

    إذا(b2 != b2_ډېری){
        uprv_free(b2);
    }

    أرجع پایله;
}

#د_که_پای /* #if !UCONFIG_NO_IDNA */
