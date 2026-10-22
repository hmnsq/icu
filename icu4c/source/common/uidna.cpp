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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_IDNA

#شامل "unicode/uidna.h"
#شامل "unicode/ustring.h"
#شامل "unicode/usprep.h"
#شامل "punycode.h"
#شامل "ustr_imp.h"
#شامل "cmemory.h"
#شامل "uassert.h"
#شامل "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
ساکن ثابت char16_t ACE_سابقہ[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#تعریف_کرو ACE_سابقہ_لمبائی 4

#تعریف_کرو MAX_LABEL_لمبائی 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#تعریف_کرو MAX_LABEL_بفر_سائز 100

#تعریف_کرو MAX_DOMAIN_NAME_لمبائی 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#تعریف_کرو MAX_IDN_بفر_سائز   MAX_DOMAIN_NAME_لمبائی+1

#تعریف_کرو زیریں_CASE_DELTA 0x0020
#تعریف_کرو HYPHEN           0x002D
#تعریف_کرو بھرا_STOP        0x002E
#تعریف_کرو CAPITAL_A        0x0041
#تعریف_کرو CAPITAL_Z        0x005A

اندرونی ساکن char16_t
مقصد_ASCIILower(char16_t ch){
    اگر(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        واپس ch + زیریں_CASE_DELTA;
    }
    واپس ch;
}

اندرونی ساکن UBool 
starts_With_سابقہ(ثابت char16_t* src , int32_t src_لمبائی){
    اگر(src_لمبائی < ACE_سابقہ_لمبائی){
        واپس جھوٹ;
    }

    برائے(int8_t i=0; i< ACE_سابقہ_لمبائی; i++){
        اگر(مقصد_ASCIILower(src[i]) != ACE_سابقہ[i]){
            واپس جھوٹ;
        }
    }
    واپس سچ;
}


اندرونی ساکن int32_t
compareCaseInsensitiveASCII(ثابت char16_t* s1, int32_t s1Len,
                            ثابت char16_t* s2, int32_t s2Len){
    
    int32_t min_لمبائی;
    int32_t لمبائی_نتیجہ;

    // are we comparing different lengths?
    اگر(s1Len != s2Len) {
        اگر(s1Len < s2Len) {
            min_لمبائی = s1Len;
            لمبائی_نتیجہ = -1;
        } ورنہ {
            min_لمبائی = s2Len;
            لمبائی_نتیجہ = 1;
        }
    } ورنہ {
        // ok the lengths are equal
        min_لمبائی = s1Len;
        لمبائی_نتیجہ = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    برائے(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        اگر(i == min_لمبائی) {
            واپس لمبائی_نتیجہ;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        اگر(c1!=c2) {
            rc = static_cast<int32_t>(مقصد_ASCIILower(c1)) - static_cast<int32_t>(مقصد_ASCIILower(c2));
            اگر(rc!=0) {
                لمبائی_نتیجہ=rc;
                توڑو;
            }
        }
    }
    واپس لمبائی_نتیجہ;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
ساکن اندرونی UBool isLabelSeparator(char16_t ch){
    انتخاب(ch){
        حالت 0x002e:
        حالت 0x3002:
        حالت 0xFF0E:
        حالت 0xFF61:
            واپس سچ;
        طےشدہ:
            واپس جھوٹ;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
ساکن اندرونی int32_t
get_اگلا_Separator(char16_t *src, int32_t src_لمبائی,
                 char16_t **حد_بندی, UBool *مکمل){
    اگر(src_لمبائی == -1){
        int32_t i;
        برائے(i=0 ; ;i++){
            اگر(src[i] == 0){
                *حد_بندی = src + i; // point to null
                *مکمل = سچ;
                واپس i;
            }
            اگر(isLabelSeparator(src[i])){
                *حد_بندی = src + (i+1); // go past the delimiter
                واپس i;
                
            }
        }
    }ورنہ{
        int32_t i;
        برائے(i=0;i<src_لمبائی;i++){
            اگر(isLabelSeparator(src[i])){
                *حد_بندی = src + (i+1); // go past the delimiter
                واپس i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *حد_بندی = src+src_لمبائی;
        *مکمل = سچ;

        واپس i;
    }
}
ساکن اندرونی UBool isLDHChar(char16_t ch){
    // high runner case
    اگر(ch>0x007A){
        واپس جھوٹ;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    اگر( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        واپس سچ;
    }
    واپس جھوٹ;
}

ساکن int32_t 
اندرونی_مقصد_ASCII(ثابت char16_t* src, int32_t src_لمبائی,
                  char16_t* dest, int32_t dest_استعداد,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_رمز* حالت_2)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ڈھیر[MAX_LABEL_بفر_سائز], b2_ڈھیر[MAX_LABEL_بفر_سائز];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ڈھیر, *b2 = b2_ڈھیر;
    int32_t b1Len=0, b2Len, 
            b1_استعداد = MAX_LABEL_بفر_سائز, 
            b2_استعداد = MAX_LABEL_بفر_سائز ,
            req_لمبائی=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = خالی_اشاریہ;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = سچ;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = سچ; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_قواعد) != 0);

    int32_t failPos = -1;
    
    اگر(src_لمبائی == -1){
        src_لمبائی = u_strlen(src);
    }
    
    اگر(src_لمبائی > b1_استعداد){
        b1 = static_cast<char16_t*>(uprv_malloc(src_لمبائی * U_SIZEOF_UCHAR));
        اگر(b1==خالی_اشاریہ){
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ CLEANUP;
        }
        b1_استعداد = src_لمبائی;
    }

    // step 1 
    برائے( j=0;j<src_لمبائی;j++){
        اگر(src[j] > 0x7F){
            srcIsASCII = جھوٹ;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    اگر(srcIsASCII == جھوٹ){
        
        // step 2    
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_لمبائی, b1, b1_استعداد, namePrepOptions, parseError, &بفر_حالت);

        اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            اگر(b1 != b1_ڈھیر){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            اگر(b1==خالی_اشاریہ){
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                جاؤ CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_لمبائی, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
        }
        اگر (U_FAILURE(بفر_حالت)) {
            *حالت_2 = بفر_حالت;
        }
    }
    // error bail out
    اگر(U_FAILURE(*حالت_2)){
        جاؤ CLEANUP;
    }
    اگر(b1Len == 0){
        *حالت_2 = U_IDNA_ZERO_لمبائی_LABEL_ERROR;
        جاؤ CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = سچ;
    برائے( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        اگر(b1[j] > 0x7F){
            srcIsASCII = جھوٹ;
        }ورنہ اگر(isLDHChar(b1[j])==جھوٹ){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = جھوٹ;
            failPos = j;
        }
    }
    اگر(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        اگر( srcIsLDH == جھوٹ /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *حالت_2 = U_IDNA_STD3_ASCII_قواعد_ERROR;

            /* populate the parseError struct */
            اگر(srcIsLDH==جھوٹ){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }ورنہ اگر(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }ورنہ{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            جاؤ CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    اگر(srcIsASCII){
        اگر(b1Len <= dest_استعداد){
            u_memmove(dest, b1, b1Len);
            req_لمبائی = b1Len;
        }ورنہ{
            req_لمبائی = b1Len;
            جاؤ CLEANUP;
        }
    }ورنہ{
        // step 5 : verify the sequence does not begin with ACE prefix
        اگر(!starts_With_سابقہ(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_رمز بفر_حالت = U_ZERO_ERROR;
            b2Len = u_str_مقصد_Punycode(b1,b1Len,b2,b2_استعداد,caseFlags,&بفر_حالت);

            اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                اگر(b2 == خالی_اشاریہ){
                    *حالت_2 = U_حافظہ_تخصیص_ERROR;
                    جاؤ CLEANUP;
                }

                بفر_حالت = U_ZERO_ERROR; // reset error

                b2Len = u_str_مقصد_Punycode(b1,b1Len,b2,b2Len,caseFlags,&بفر_حالت);
            }
            //error bail out
            اگر(U_FAILURE(بفر_حالت)){
                *حالت_2 = بفر_حالت;
                جاؤ CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_لمبائی = b2Len+ACE_سابقہ_لمبائی;

            اگر(req_لمبائی > dest_استعداد){
                *حالت_2 = U_بفر_حد_سے_تجاوز_ERROR;
                جاؤ CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_سابقہ, ACE_سابقہ_لمبائی);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_سابقہ_لمبائی, b2, b2Len);

        }ورنہ{
            *حالت_2 = U_IDNA_ACE_سابقہ_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            جاؤ CLEANUP;
        }
    }
    // step 8: verify the length of label
    اگر(req_لمبائی > MAX_LABEL_لمبائی){
        *حالت_2 = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    اگر(b1 != b1_ڈھیر){
        uprv_free(b1);
    }
    اگر(b2 != b2_ڈھیر){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    واپس u_terminateUChars(dest, dest_استعداد, req_لمبائی, حالت_2);
}

ساکن int32_t
اندرونی_مقصد_Unicode(ثابت char16_t* src, int32_t src_لمبائی,
                    char16_t* dest, int32_t dest_استعداد,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_رمز* حالت_2)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ڈھیر[MAX_LABEL_بفر_سائز], b2_ڈھیر[MAX_LABEL_بفر_سائز], b3_ڈھیر[MAX_LABEL_بفر_سائز];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ڈھیر, *b2 = b2_ڈھیر, *b1_مفرد=خالی_اشاریہ, *b3=b3_ڈھیر;
    int32_t b1Len = 0, b2Len, b1_مفرد_Len, b3Len,
            b1_استعداد = MAX_LABEL_بفر_سائز, 
            b2_استعداد = MAX_LABEL_بفر_سائز,
            b3_استعداد = MAX_LABEL_بفر_سائز,
            req_لمبائی=0;

    UBool* caseFlags = خالی_اشاریہ;

    UBool srcIsASCII = سچ;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    اگر(src_لمبائی==-1){
        src_لمبائی = 0;
        برائے(;src[src_لمبائی]!=0;){
            اگر(src[src_لمبائی]> 0x7f){
                srcIsASCII = جھوٹ;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_لمبائی++;
        }
    }ورنہ اگر(src_لمبائی > 0){
        برائے(int32_t j=0; j<src_لمبائی; j++){
            اگر(src[j]> 0x7f){
                srcIsASCII = جھوٹ;
                توڑو;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }ورنہ{
        واپس 0;
    }
    
    اگر(srcIsASCII == جھوٹ){
        // step 2: process the string
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_لمبائی, b1, b1_استعداد, namePrepOptions, parseError, &بفر_حالت);
        اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            اگر(b1==خالی_اشاریہ){
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                جاؤ CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_لمبائی, b1, b1Len, namePrepOptions, parseError, &بفر_حالت);
        }
        //bail out on error
        اگر(U_FAILURE(بفر_حالت)){
            *حالت_2 = بفر_حالت;
            جاؤ CLEANUP;
        }
    }ورنہ{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_لمبائی;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    اگر(starts_With_سابقہ(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_مفرد = b1 + ACE_سابقہ_لمبائی;
        b1_مفرد_Len  = b1Len - ACE_سابقہ_لمبائی;

        //step 5: Decode using punycode
        UError_رمز بفر_حالت = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_مفرد, b1_مفرد_Len, b2, b2_استعداد, caseFlags, &بفر_حالت);

        اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            اگر(b2==خالی_اشاریہ){
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                جاؤ CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_مفرد, b1_مفرد_Len, b2, b2Len, caseFlags, &بفر_حالت);
        }


        //step 6:Apply toASCII
        b3Len = uidna_مقصد_ASCII(b2, b2Len, b3, b3_استعداد, options, parseError, &بفر_حالت);

        اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            اگر(b3==خالی_اشاریہ){
                *حالت_2 = U_حافظہ_تخصیص_ERROR;
                جاؤ CLEANUP;
            }

            بفر_حالت = U_ZERO_ERROR; // reset error

            b3Len =  uidna_مقصد_ASCII(b2,b2Len,b3,b3Len,options,parseError,&بفر_حالت);

        }
        //bail out on error
        اگر(U_FAILURE(بفر_حالت)){
            *حالت_2 = بفر_حالت;
            جاؤ CLEANUP;
        }

        //step 7: verify
        اگر(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *حالت_2 = U_IDNA_VERIFICATION_ERROR;
            جاؤ CLEANUP;
        }

        //step 8: return output of step 5
        req_لمبائی = b2Len;
        اگر(b2Len <= dest_استعداد) {
            u_memmove(dest, b2, b2Len);
        }
    }
    ورنہ{
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
        اگر(src_لمبائی <= dest_استعداد){
            u_memmove(dest, src, src_لمبائی);
        }
        req_لمبائی = src_لمبائی;
    }


CLEANUP:

    اگر(b1 != b1_ڈھیر && b1!=src){
        uprv_free(b1);
    }
    اگر(b2 != b2_ڈھیر){
        uprv_free(b2);
    }
    اگر(b3 != b3_ڈھیر){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    اگر(U_FAILURE(*حالت_2)){
        //copy the source to destination
        اگر(dest && src_لمبائی <= dest_استعداد){
            // srcLength should have already been set earlier.
            U_ASSERT(src_لمبائی >= 0);
            u_memmove(dest, src, src_لمبائی);
        }
        req_لمبائی = src_لمبائی;
        *حالت_2 = U_ZERO_ERROR;
    }

    واپس u_terminateUChars(dest, dest_استعداد, req_لمبائی, حالت_2);
}

U_CAPI int32_t U_EXPORT2
uidna_مقصد_ASCII(ثابت char16_t* src, int32_t src_لمبائی,
              char16_t* dest, int32_t dest_استعداد,
              int32_t options,
              UParseError* parseError,
              UError_رمز* حالت_2){
    
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس 0;
    }
    اگر((src==خالی_اشاریہ) || (src_لمبائی < -1) || (dest_استعداد<0) || (!dest && dest_استعداد > 0)){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت_2);
    
    اگر(U_FAILURE(*حالت_2)){
        واپس -1;
    }
    
    int32_t retLen = اندرونی_مقصد_ASCII(src, src_لمبائی, dest, dest_استعداد, options, nameprep, parseError, حالت_2);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    واپس retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_مقصد_Unicode(ثابت char16_t* src, int32_t src_لمبائی,
                char16_t* dest, int32_t dest_استعداد,
                int32_t options,
                UParseError* parseError,
                UError_رمز* حالت_2){

    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس 0;
    }
    اگر( (src==خالی_اشاریہ) || (src_لمبائی < -1) || (dest_استعداد<0) || (!dest && dest_استعداد > 0)){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت_2);
    
    اگر(U_FAILURE(*حالت_2)){
        واپس -1;
    }
    
    int32_t retLen = اندرونی_مقصد_Unicode(src, src_لمبائی, dest, dest_استعداد, options, nameprep, parseError, حالت_2);

    usprep_close(nameprep);
    
    واپس retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ثابت char16_t *src, int32_t src_لمبائی,
                   char16_t* dest, int32_t dest_استعداد,
                   int32_t options,
                   UParseError *parseError,
                   UError_رمز *حالت_2){

    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس 0;
    }
    اگر((src==خالی_اشاریہ) || (src_لمبائی < -1) || (dest_استعداد<0) || (!dest && dest_استعداد > 0)){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    int32_t req_لمبائی = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت_2);
    
    اگر(U_FAILURE(*حالت_2)){
        واپس 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_آغاز = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t بقایا_Len = src_لمبائی;
    int32_t بقایا_Dest_استعداد = dest_استعداد;
    int32_t labelLen = 0, label_Req_لمبائی = 0;
    UBool مکمل = جھوٹ;


    برائے(;;){

        labelLen = get_اگلا_Separator(label_آغاز,بقایا_Len, &delimiter,&مکمل);
        label_Req_لمبائی = 0;
        اگر(!(labelLen==0 && مکمل)){// make sure this is not a root label separator.

            UError_رمز بفر_حالت = U_ZERO_ERROR;
            label_Req_لمبائی = اندرونی_مقصد_ASCII( label_آغاز, labelLen,
                                                currentDest, بقایا_Dest_استعداد,
                                                options, nameprep,
                                                parseError, &بفر_حالت);

            اگر (بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR) {
                بقایا_Dest_استعداد = 0;
            } ورنہ اگر (U_FAILURE(بفر_حالت)) {
                *حالت_2 = بفر_حالت;
                توڑو;
            }
        }

        req_لمبائی +=label_Req_لمبائی;
        // adjust the destination pointer
        اگر(label_Req_لمبائی < بقایا_Dest_استعداد){
            currentDest = currentDest + label_Req_لمبائی;
            بقایا_Dest_استعداد -= label_Req_لمبائی;
        }ورنہ{
            // should never occur
            بقایا_Dest_استعداد = 0;
        }

        اگر(مکمل){
            توڑو;
        }

        // add the label separator
        اگر(بقایا_Dest_استعداد > 0){
            *currentDest++ = بھرا_STOP;
            بقایا_Dest_استعداد--;
        }
        req_لمبائی++;

        label_آغاز = delimiter;
        اگر(بقایا_Len >0 ){
            بقایا_Len = (int32_t)(src_لمبائی - (delimiter - src));
        }

    }

    اگر(req_لمبائی > MAX_DOMAIN_NAME_لمبائی){
        *حالت_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    واپس u_terminateUChars(dest, dest_استعداد, req_لمبائی, حالت_2);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ثابت char16_t* src, int32_t src_لمبائی,
                     char16_t* dest, int32_t dest_استعداد,
                     int32_t options,
                     UParseError* parseError,
                     UError_رمز* حالت_2){
    
    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس 0;
    }
    اگر((src==خالی_اشاریہ) || (src_لمبائی < -1) || (dest_استعداد<0) || (!dest && dest_استعداد > 0)){
        *حالت_2 = U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    int32_t req_لمبائی = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, حالت_2);
    
    اگر(U_FAILURE(*حالت_2)){
        واپس 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_آغاز = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t بقایا_Len = src_لمبائی;
    int32_t بقایا_Dest_استعداد = dest_استعداد;
    int32_t labelLen = 0, label_Req_لمبائی = 0;
    UBool مکمل = جھوٹ;

    برائے(;;){

        labelLen = get_اگلا_Separator(label_آغاز,بقایا_Len, &delimiter,&مکمل);
        
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
        
        label_Req_لمبائی = اندرونی_مقصد_Unicode(label_آغاز, labelLen, 
                                             currentDest, بقایا_Dest_استعداد, 
                                             options, nameprep, 
                                             parseError, حالت_2);

        اگر(*حالت_2 == U_بفر_حد_سے_تجاوز_ERROR){
            *حالت_2 = U_ZERO_ERROR; // reset error
            بقایا_Dest_استعداد = 0;
        }

        اگر(U_FAILURE(*حالت_2)){
            توڑو;
        }
        
        req_لمبائی +=label_Req_لمبائی;
        // adjust the destination pointer
        اگر(label_Req_لمبائی < بقایا_Dest_استعداد){
            currentDest = currentDest + label_Req_لمبائی;
            بقایا_Dest_استعداد -= label_Req_لمبائی;
        }ورنہ{
            // should never occur
            بقایا_Dest_استعداد = 0;
        }

        اگر(مکمل){
            توڑو;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        اگر(بقایا_Dest_استعداد > 0){
            *currentDest++ = *(label_آغاز + labelLen);
            بقایا_Dest_استعداد--;
        }
        req_لمبائی++;

        label_آغاز = delimiter;
        اگر(بقایا_Len >0 ){
            بقایا_Len = (int32_t)(src_لمبائی - (delimiter - src));
        }

    }

    اگر(req_لمبائی > MAX_DOMAIN_NAME_لمبائی){
        *حالت_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    واپس u_terminateUChars(dest, dest_استعداد, req_لمبائی, حالت_2);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ثابت char16_t *s1, int32_t length1,
                ثابت char16_t *s2, int32_t length2,
                int32_t options,
                UError_رمز* حالت_2){

    اگر(حالت_2 == خالی_اشاریہ || U_FAILURE(*حالت_2)){
        واپس -1;
    }

    char16_t b1_ڈھیر[MAX_IDN_بفر_سائز], b2_ڈھیر[MAX_IDN_بفر_سائز];
    char16_t *b1 = b1_ڈھیر, *b2 = b2_ڈھیر;
    int32_t b1Len, b2Len, b1_استعداد = MAX_IDN_بفر_سائز, b2_استعداد = MAX_IDN_بفر_سائز;
    int32_t نتیجہ=-1;
    
    UParseError parseError;

    UError_رمز بفر_حالت = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_استعداد, options, &parseError, &بفر_حالت);
    اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        اگر(b1==خالی_اشاریہ){
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ CLEANUP;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &بفر_حالت);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_استعداد, options, &parseError, &بفر_حالت);
    اگر(بفر_حالت == U_بفر_حد_سے_تجاوز_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        اگر(b2==خالی_اشاریہ){
            *حالت_2 = U_حافظہ_تخصیص_ERROR;
            جاؤ CLEANUP;
        }

        بفر_حالت = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &بفر_حالت);
    }

    اگر (U_FAILURE(بفر_حالت)) {
        *حالت_2 = بفر_حالت;
        جاؤ CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    نتیجہ = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    اگر(b1 != b1_ڈھیر){
        uprv_free(b1);
    }

    اگر(b2 != b2_ڈھیر){
        uprv_free(b2);
    }

    واپس نتیجہ;
}

#اگر_ختم /* #if !UCONFIG_NO_IDNA */
