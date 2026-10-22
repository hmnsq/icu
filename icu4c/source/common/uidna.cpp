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
ساكن ثابت char16_t ACE_اگیتر[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#تعریف_کرو ACE_اگیتر_لمائی 4

#تعریف_کرو MAX_LABEL_لمائی 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#تعریف_کرو MAX_LABEL_بفر_ناپ 100

#تعریف_کرو MAX_DOMAIN_NAME_لمائی 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#تعریف_کرو MAX_IDN_بفر_ناپ   MAX_DOMAIN_NAME_لمائی+1

#تعریف_کرو ہیٹھلا_CASE_DELTA 0x0020
#تعریف_کرو HYPHEN           0x002D
#تعریف_کرو بھریا_STOP        0x002E
#تعریف_کرو CAPITAL_A        0x0041
#تعریف_کرو CAPITAL_Z        0x005A

مضمن ساكن char16_t
منزل_ASCIILower(char16_t ch){
    إذا(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        أرجع ch + ہیٹھلا_CASE_DELTA;
    }
    أرجع ch;
}

مضمن ساكن UBool 
starts_With_اگیتر(ثابت char16_t* src , int32_t src_لمائی){
    إذا(src_لمائی < ACE_اگیتر_لمائی){
        أرجع خطأ;
    }

    لكل(int8_t i=0; i< ACE_اگیتر_لمائی; i++){
        إذا(منزل_ASCIILower(src[i]) != ACE_اگیتر[i]){
            أرجع خطأ;
        }
    }
    أرجع سچ;
}


مضمن ساكن int32_t
compareCaseInsensitiveASCII(ثابت char16_t* s1, int32_t s1Len,
                            ثابت char16_t* s2, int32_t s2Len){
    
    int32_t min_لمائی;
    int32_t لمائی_نتیجہ;

    // are we comparing different lengths?
    إذا(s1Len != s2Len) {
        إذا(s1Len < s2Len) {
            min_لمائی = s1Len;
            لمائی_نتیجہ = -1;
        } وإلا {
            min_لمائی = s2Len;
            لمائی_نتیجہ = 1;
        }
    } وإلا {
        // ok the lengths are equal
        min_لمائی = s1Len;
        لمائی_نتیجہ = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    لكل(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        إذا(i == min_لمائی) {
            أرجع لمائی_نتیجہ;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        إذا(c1!=c2) {
            rc = static_cast<int32_t>(منزل_ASCIILower(c1)) - static_cast<int32_t>(منزل_ASCIILower(c2));
            إذا(rc!=0) {
                لمائی_نتیجہ=rc;
                اكسر;
            }
        }
    }
    أرجع لمائی_نتیجہ;
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
            أرجع سچ;
        افتراضي:
            أرجع خطأ;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
ساكن مضمن int32_t
get_اگلا_Separator(char16_t *src, int32_t src_لمائی,
                 char16_t **حد_بندی, UBool *پورا){
    إذا(src_لمائی == -1){
        int32_t i;
        لكل(i=0 ; ;i++){
            إذا(src[i] == 0){
                *حد_بندی = src + i; // point to null
                *پورا = سچ;
                أرجع i;
            }
            إذا(isLabelSeparator(src[i])){
                *حد_بندی = src + (i+1); // go past the delimiter
                أرجع i;
                
            }
        }
    }وإلا{
        int32_t i;
        لكل(i=0;i<src_لمائی;i++){
            إذا(isLabelSeparator(src[i])){
                *حد_بندی = src + (i+1); // go past the delimiter
                أرجع i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *حد_بندی = src+src_لمائی;
        *پورا = سچ;

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
        أرجع سچ;
    }
    أرجع خطأ;
}

ساكن int32_t 
اندرونی_منزل_ASCII(ثابت char16_t* src, int32_t src_لمائی,
                  char16_t* dest, int32_t dest_سمرتھا,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_کوڈ* حالت)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ڈھیر[MAX_LABEL_بفر_ناپ], b2_ڈھیر[MAX_LABEL_بفر_ناپ];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ڈھیر, *b2 = b2_ڈھیر;
    int32_t b1Len=0, b2Len, 
            b1_سمرتھا = MAX_LABEL_بفر_ناپ, 
            b2_سمرتھا = MAX_LABEL_بفر_ناپ ,
            req_لمائی=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = خالی_سوچک;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = سچ;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = سچ; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_قاعدے) != 0);

    int32_t failPos = -1;
    
    إذا(src_لمائی == -1){
        src_لمائی = u_strlen(src);
    }
    
    إذا(src_لمائی > b1_سمرتھا){
        b1 = static_cast<char16_t*>(uprv_malloc(src_لمائی * U_SIZEOF_UCHAR));
        إذا(b1==خالی_سوچک){
            *حالت = U_حافظہ_ونڈ_ERROR;
            اذهب CLEANUP;
        }
        b1_سمرتھا = src_لمائی;
    }

    // step 1 
    لكل( j=0;j<src_لمائی;j++){
        إذا(src[j] > 0x7F){
            srcIsASCII = خطأ;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    إذا(srcIsASCII == خطأ){
        
        // step 2    
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_لمائی, b1, b1_سمرتھا, namePrepOptions, parseError, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            إذا(b1 != b1_ڈھیر){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            إذا(b1==خالی_سوچک){
                *حالت = U_حافظہ_ونڈ_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_لمائی, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
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
        *حالت = U_IDNA_ZERO_لمائی_LABEL_ERROR;
        اذهب CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = سچ;
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
            *حالت = U_IDNA_STD3_ASCII_قاعدے_ERROR;

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
        إذا(b1Len <= dest_سمرتھا){
            u_memmove(dest, b1, b1Len);
            req_لمائی = b1Len;
        }وإلا{
            req_لمائی = b1Len;
            اذهب CLEANUP;
        }
    }وإلا{
        // step 5 : verify the sequence does not begin with ACE prefix
        إذا(!starts_With_اگیتر(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_کوڈ بفر_حالت = U_ZERO_ERROR;
            b2Len = u_str_منزل_Punycode(b1,b1Len,b2,b2_سمرتھا,caseFlags,&بفر_حالت);

            إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                إذا(b2 == خالی_سوچک){
                    *حالت = U_حافظہ_ونڈ_ERROR;
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
            req_لمائی = b2Len+ACE_اگیتر_لمائی;

            إذا(req_لمائی > dest_سمرتھا){
                *حالت = U_بفر_حد_توں_ودھ_ERROR;
                اذهب CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_اگیتر, ACE_اگیتر_لمائی);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_اگیتر_لمائی, b2, b2Len);

        }وإلا{
            *حالت = U_IDNA_ACE_اگیتر_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            اذهب CLEANUP;
        }
    }
    // step 8: verify the length of label
    إذا(req_لمائی > MAX_LABEL_لمائی){
        *حالت = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    إذا(b1 != b1_ڈھیر){
        uprv_free(b1);
    }
    إذا(b2 != b2_ڈھیر){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    أرجع u_terminateUChars(dest, dest_سمرتھا, req_لمائی, حالت);
}

ساكن int32_t
اندرونی_منزل_Unicode(ثابت char16_t* src, int32_t src_لمائی,
                    char16_t* dest, int32_t dest_سمرتھا,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_کوڈ* حالت)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ڈھیر[MAX_LABEL_بفر_ناپ], b2_ڈھیر[MAX_LABEL_بفر_ناپ], b3_ڈھیر[MAX_LABEL_بفر_ناپ];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ڈھیر, *b2 = b2_ڈھیر, *b1_مفرد=خالی_سوچک, *b3=b3_ڈھیر;
    int32_t b1Len = 0, b2Len, b1_مفرد_Len, b3Len,
            b1_سمرتھا = MAX_LABEL_بفر_ناپ, 
            b2_سمرتھا = MAX_LABEL_بفر_ناپ,
            b3_سمرتھا = MAX_LABEL_بفر_ناپ,
            req_لمائی=0;

    UBool* caseFlags = خالی_سوچک;

    UBool srcIsASCII = سچ;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    إذا(src_لمائی==-1){
        src_لمائی = 0;
        لكل(;src[src_لمائی]!=0;){
            إذا(src[src_لمائی]> 0x7f){
                srcIsASCII = خطأ;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_لمائی++;
        }
    }وإلا إذا(src_لمائی > 0){
        لكل(int32_t j=0; j<src_لمائی; j++){
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
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_لمائی, b1, b1_سمرتھا, namePrepOptions, parseError, &بفر_حالت);
        إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            إذا(b1==خالی_سوچک){
                *حالت = U_حافظہ_ونڈ_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_لمائی, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
        }
        //bail out on error
        إذا(U_FAILURE(بفر_حالت)){
            *حالت = بفر_حالت;
            اذهب CLEANUP;
        }
    }وإلا{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_لمائی;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    إذا(starts_With_اگیتر(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_مفرد = b1 + ACE_اگیتر_لمائی;
        b1_مفرد_Len  = b1Len - ACE_اگیتر_لمائی;

        //step 5: Decode using punycode
        UError_کوڈ بفر_حالت = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_مفرد, b1_مفرد_Len, b2, b2_سمرتھا, caseFlags, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            إذا(b2==خالی_سوچک){
                *حالت = U_حافظہ_ونڈ_ERROR;
                اذهب CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_مفرد, b1_مفرد_Len, b2, b2Len, caseFlags, &بفر_حالت);
        }


        //step 6:Apply toASCII
        b3Len = uidna_منزل_ASCII(b2, b2Len, b3, b3_سمرتھا, options, parseError, &بفر_حالت);

        إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            إذا(b3==خالی_سوچک){
                *حالت = U_حافظہ_ونڈ_ERROR;
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
        req_لمائی = b2Len;
        إذا(b2Len <= dest_سمرتھا) {
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
        إذا(src_لمائی <= dest_سمرتھا){
            u_memmove(dest, src, src_لمائی);
        }
        req_لمائی = src_لمائی;
    }


CLEANUP:

    إذا(b1 != b1_ڈھیر && b1!=src){
        uprv_free(b1);
    }
    إذا(b2 != b2_ڈھیر){
        uprv_free(b2);
    }
    إذا(b3 != b3_ڈھیر){
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
        إذا(dest && src_لمائی <= dest_سمرتھا){
            // srcLength should have already been set earlier.
            U_ASSERT(src_لمائی >= 0);
            u_memmove(dest, src, src_لمائی);
        }
        req_لمائی = src_لمائی;
        *حالت = U_ZERO_ERROR;
    }

    أرجع u_terminateUChars(dest, dest_سمرتھا, req_لمائی, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_منزل_ASCII(ثابت char16_t* src, int32_t src_لمائی,
              char16_t* dest, int32_t dest_سمرتھا,
              int32_t options,
              UParseError* parseError,
              UError_کوڈ* حالت){
    
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==خالی_سوچک) || (src_لمائی < -1) || (dest_سمرتھا<0) || (!dest && dest_سمرتھا > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع -1;
    }
    
    int32_t retLen = اندرونی_منزل_ASCII(src, src_لمائی, dest, dest_سمرتھا, options, nameprep, parseError, حالت);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    أرجع retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_منزل_Unicode(ثابت char16_t* src, int32_t src_لمائی,
                char16_t* dest, int32_t dest_سمرتھا,
                int32_t options,
                UParseError* parseError,
                UError_کوڈ* حالت){

    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا( (src==خالی_سوچک) || (src_لمائی < -1) || (dest_سمرتھا<0) || (!dest && dest_سمرتھا > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع -1;
    }
    
    int32_t retLen = اندرونی_منزل_Unicode(src, src_لمائی, dest, dest_سمرتھا, options, nameprep, parseError, حالت);

    usprep_close(nameprep);
    
    أرجع retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ثابت char16_t *src, int32_t src_لمائی,
                   char16_t* dest, int32_t dest_سمرتھا,
                   int32_t options,
                   UParseError *parseError,
                   UError_کوڈ *حالت){

    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==خالی_سوچک) || (src_لمائی < -1) || (dest_سمرتھا<0) || (!dest && dest_سمرتھا > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t req_لمائی = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_شروع = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t بچیا_Len = src_لمائی;
    int32_t بچیا_Dest_سمرتھا = dest_سمرتھا;
    int32_t labelLen = 0, label_Req_لمائی = 0;
    UBool پورا = خطأ;


    لكل(;;){

        labelLen = get_اگلا_Separator(label_شروع,بچیا_Len, &delimiter,&پورا);
        label_Req_لمائی = 0;
        إذا(!(labelLen==0 && پورا)){// make sure this is not a root label separator.

            UError_کوڈ بفر_حالت = U_ZERO_ERROR;
            label_Req_لمائی = اندرونی_منزل_ASCII( label_شروع, labelLen,
                                                currentDest, بچیا_Dest_سمرتھا,
                                                options, nameprep,
                                                parseError, &بفر_حالت);

            إذا (بفر_حالت == U_بفر_حد_توں_ودھ_ERROR) {
                بچیا_Dest_سمرتھا = 0;
            } وإلا إذا (U_FAILURE(بفر_حالت)) {
                *حالت = بفر_حالت;
                اكسر;
            }
        }

        req_لمائی +=label_Req_لمائی;
        // adjust the destination pointer
        إذا(label_Req_لمائی < بچیا_Dest_سمرتھا){
            currentDest = currentDest + label_Req_لمائی;
            بچیا_Dest_سمرتھا -= label_Req_لمائی;
        }وإلا{
            // should never occur
            بچیا_Dest_سمرتھا = 0;
        }

        إذا(پورا){
            اكسر;
        }

        // add the label separator
        إذا(بچیا_Dest_سمرتھا > 0){
            *currentDest++ = بھریا_STOP;
            بچیا_Dest_سمرتھا--;
        }
        req_لمائی++;

        label_شروع = delimiter;
        إذا(بچیا_Len >0 ){
            بچیا_Len = (int32_t)(src_لمائی - (delimiter - src));
        }

    }

    إذا(req_لمائی > MAX_DOMAIN_NAME_لمائی){
        *حالت = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    أرجع u_terminateUChars(dest, dest_سمرتھا, req_لمائی, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ثابت char16_t* src, int32_t src_لمائی,
                     char16_t* dest, int32_t dest_سمرتھا,
                     int32_t options,
                     UParseError* parseError,
                     UError_کوڈ* حالت){
    
    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع 0;
    }
    إذا((src==خالی_سوچک) || (src_لمائی < -1) || (dest_سمرتھا<0) || (!dest && dest_سمرتھا > 0)){
        *حالت = U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    int32_t req_لمائی = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت);
    
    إذا(U_FAILURE(*حالت)){
        أرجع 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_شروع = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t بچیا_Len = src_لمائی;
    int32_t بچیا_Dest_سمرتھا = dest_سمرتھا;
    int32_t labelLen = 0, label_Req_لمائی = 0;
    UBool پورا = خطأ;

    لكل(;;){

        labelLen = get_اگلا_Separator(label_شروع,بچیا_Len, &delimiter,&پورا);
        
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
        
        label_Req_لمائی = اندرونی_منزل_Unicode(label_شروع, labelLen, 
                                             currentDest, بچیا_Dest_سمرتھا, 
                                             options, nameprep, 
                                             parseError, حالت);

        إذا(*حالت == U_بفر_حد_توں_ودھ_ERROR){
            *حالت = U_ZERO_ERROR; // reset error
            بچیا_Dest_سمرتھا = 0;
        }

        إذا(U_FAILURE(*حالت)){
            اكسر;
        }
        
        req_لمائی +=label_Req_لمائی;
        // adjust the destination pointer
        إذا(label_Req_لمائی < بچیا_Dest_سمرتھا){
            currentDest = currentDest + label_Req_لمائی;
            بچیا_Dest_سمرتھا -= label_Req_لمائی;
        }وإلا{
            // should never occur
            بچیا_Dest_سمرتھا = 0;
        }

        إذا(پورا){
            اكسر;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        إذا(بچیا_Dest_سمرتھا > 0){
            *currentDest++ = *(label_شروع + labelLen);
            بچیا_Dest_سمرتھا--;
        }
        req_لمائی++;

        label_شروع = delimiter;
        إذا(بچیا_Len >0 ){
            بچیا_Len = (int32_t)(src_لمائی - (delimiter - src));
        }

    }

    إذا(req_لمائی > MAX_DOMAIN_NAME_لمائی){
        *حالت = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    أرجع u_terminateUChars(dest, dest_سمرتھا, req_لمائی, حالت);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                int32_t options,
                UError_کوڈ* حالت){

    إذا(حالت == خالی_سوچک || U_FAILURE(*حالت)){
        أرجع -1;
    }

    char16_t b1_ڈھیر[MAX_IDN_بفر_ناپ], b2_ڈھیر[MAX_IDN_بفر_ناپ];
    char16_t *b1 = b1_ڈھیر, *b2 = b2_ڈھیر;
    int32_t b1Len, b2Len, b1_سمرتھا = MAX_IDN_بفر_ناپ, b2_سمرتھا = MAX_IDN_بفر_ناپ;
    int32_t نتیجہ=-1;
    
    UParseError parseError;

    UError_کوڈ بفر_حالت = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_سمرتھا, options, &parseError, &بفر_حالت);
    إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        إذا(b1==خالی_سوچک){
            *حالت = U_حافظہ_ونڈ_ERROR;
            اذهب CLEANUP;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &بفر_حالت);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_سمرتھا, options, &parseError, &بفر_حالت);
    إذا(بفر_حالت == U_بفر_حد_توں_ودھ_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        إذا(b2==خالی_سوچک){
            *حالت = U_حافظہ_ونڈ_ERROR;
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
    نتیجہ = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    إذا(b1 != b1_ڈھیر){
        uprv_free(b1);
    }

    إذا(b2 != b2_ڈھیر){
        uprv_free(b2);
    }

    أرجع نتیجہ;
}

#جے_انت /* #if !UCONFIG_NO_IDNA */
