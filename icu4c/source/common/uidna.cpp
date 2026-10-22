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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_IDNA

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uidna.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/usprep.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "punycode.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
ਸਥਿਰ ਅਚਲ char16_t ACE_ਅਗੇਤਰ[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ACE_ਅਗੇਤਰ_ਲੰਮਾਈ 4

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_LABEL_ਲੰਮਾਈ 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_LABEL_ਬਫਰ_ਮਾਪ 100

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DOMAIN_NAME_ਲੰਮਾਈ 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_IDN_ਬਫਰ_ਮਾਪ   MAX_DOMAIN_NAME_ਲੰਮਾਈ+1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੇਠਲਾ_CASE_DELTA 0x0020
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HYPHEN           0x002D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਭਰਿਆ_STOP        0x002E
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPITAL_A        0x0041
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPITAL_Z        0x005A

ਸਤਰਅੰਦਰ ਸਥਿਰ char16_t
ਮੰਜ਼ਿਲ_ASCIILower(char16_t ch){
    ਜੇ(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        ਵਾਪਸ ch + ਹੇਠਲਾ_CASE_DELTA;
    }
    ਵਾਪਸ ch;
}

ਸਤਰਅੰਦਰ ਸਥਿਰ UBool 
starts_With_ਅਗੇਤਰ(ਅਚਲ char16_t* src , int32_t src_ਲੰਮਾਈ){
    ਜੇ(src_ਲੰਮਾਈ < ACE_ਅਗੇਤਰ_ਲੰਮਾਈ){
        ਵਾਪਸ ਝੂਠ;
    }

    ਲਈ(int8_t i=0; i< ACE_ਅਗੇਤਰ_ਲੰਮਾਈ; i++){
        ਜੇ(ਮੰਜ਼ਿਲ_ASCIILower(src[i]) != ACE_ਅਗੇਤਰ[i]){
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}


ਸਤਰਅੰਦਰ ਸਥਿਰ int32_t
compareCaseInsensitiveASCII(ਅਚਲ char16_t* s1, int32_t s1Len,
                            ਅਚਲ char16_t* s2, int32_t s2Len){
    
    int32_t min_ਲੰਮਾਈ;
    int32_t ਲੰਮਾਈ_ਨਤੀਜਾ;

    // are we comparing different lengths?
    ਜੇ(s1Len != s2Len) {
        ਜੇ(s1Len < s2Len) {
            min_ਲੰਮਾਈ = s1Len;
            ਲੰਮਾਈ_ਨਤੀਜਾ = -1;
        } ਨਹੀਂਤਾਂ {
            min_ਲੰਮਾਈ = s2Len;
            ਲੰਮਾਈ_ਨਤੀਜਾ = 1;
        }
    } ਨਹੀਂਤਾਂ {
        // ok the lengths are equal
        min_ਲੰਮਾਈ = s1Len;
        ਲੰਮਾਈ_ਨਤੀਜਾ = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    ਲਈ(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        ਜੇ(i == min_ਲੰਮਾਈ) {
            ਵਾਪਸ ਲੰਮਾਈ_ਨਤੀਜਾ;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        ਜੇ(c1!=c2) {
            rc = static_cast<int32_t>(ਮੰਜ਼ਿਲ_ASCIILower(c1)) - static_cast<int32_t>(ਮੰਜ਼ਿਲ_ASCIILower(c2));
            ਜੇ(rc!=0) {
                ਲੰਮਾਈ_ਨਤੀਜਾ=rc;
                ਰੋਕੋ;
            }
        }
    }
    ਵਾਪਸ ਲੰਮਾਈ_ਨਤੀਜਾ;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool isLabelSeparator(char16_t ch){
    ਚੋਣ(ch){
        ਹਾਲਤ 0x002e:
        ਹਾਲਤ 0x3002:
        ਹਾਲਤ 0xFF0E:
        ਹਾਲਤ 0xFF61:
            ਵਾਪਸ ਸੱਚ;
        ਮੂਲ:
            ਵਾਪਸ ਝੂਠ;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
get_ਅਗਲਾ_Separator(char16_t *src, int32_t src_ਲੰਮਾਈ,
                 char16_t **ਹੱਦਬੰਦੀ, UBool *ਪੂਰਾ){
    ਜੇ(src_ਲੰਮਾਈ == -1){
        int32_t i;
        ਲਈ(i=0 ; ;i++){
            ਜੇ(src[i] == 0){
                *ਹੱਦਬੰਦੀ = src + i; // point to null
                *ਪੂਰਾ = ਸੱਚ;
                ਵਾਪਸ i;
            }
            ਜੇ(isLabelSeparator(src[i])){
                *ਹੱਦਬੰਦੀ = src + (i+1); // go past the delimiter
                ਵਾਪਸ i;
                
            }
        }
    }ਨਹੀਂਤਾਂ{
        int32_t i;
        ਲਈ(i=0;i<src_ਲੰਮਾਈ;i++){
            ਜੇ(isLabelSeparator(src[i])){
                *ਹੱਦਬੰਦੀ = src + (i+1); // go past the delimiter
                ਵਾਪਸ i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *ਹੱਦਬੰਦੀ = src+src_ਲੰਮਾਈ;
        *ਪੂਰਾ = ਸੱਚ;

        ਵਾਪਸ i;
    }
}
ਸਥਿਰ ਸਤਰਅੰਦਰ UBool isLDHChar(char16_t ch){
    // high runner case
    ਜੇ(ch>0x007A){
        ਵਾਪਸ ਝੂਠ;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    ਜੇ( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        ਵਾਪਸ ਸੱਚ;
    }
    ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ int32_t 
ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
                  char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_ਕੋਡ* ਸਥਿਤੀ)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ਢੇਰ[MAX_LABEL_ਬਫਰ_ਮਾਪ], b2_ਢੇਰ[MAX_LABEL_ਬਫਰ_ਮਾਪ];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ਢੇਰ, *b2 = b2_ਢੇਰ;
    int32_t b1Len=0, b2Len, 
            b1_ਸਮਰੱਥਾ = MAX_LABEL_ਬਫਰ_ਮਾਪ, 
            b2_ਸਮਰੱਥਾ = MAX_LABEL_ਬਫਰ_ਮਾਪ ,
            req_ਲੰਮਾਈ=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = ਖਾਲੀ_ਸੂਚਕ;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = ਸੱਚ;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = ਸੱਚ; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_ਨਿਯਮਾਂ) != 0);

    int32_t failPos = -1;
    
    ਜੇ(src_ਲੰਮਾਈ == -1){
        src_ਲੰਮਾਈ = u_strlen(src);
    }
    
    ਜੇ(src_ਲੰਮਾਈ > b1_ਸਮਰੱਥਾ){
        b1 = static_cast<char16_t*>(uprv_malloc(src_ਲੰਮਾਈ * U_SIZEOF_UCHAR));
        ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ CLEANUP;
        }
        b1_ਸਮਰੱਥਾ = src_ਲੰਮਾਈ;
    }

    // step 1 
    ਲਈ( j=0;j<src_ਲੰਮਾਈ;j++){
        ਜੇ(src[j] > 0x7F){
            srcIsASCII = ਝੂਠ;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    ਜੇ(srcIsASCII == ਝੂਠ){
        
        // step 2    
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_ਲੰਮਾਈ, b1, b1_ਸਮਰੱਥਾ, namePrepOptions, parseError, &ਬਫਰ_ਸਥਿਤੀ);

        ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            ਜੇ(b1 != b1_ਢੇਰ){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ CLEANUP;
            }

            ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_ਲੰਮਾਈ, b1, b1Len, namePrepOptions, parseError, &ਬਫਰ_ਸਥਿਤੀ);
        }
        ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
            *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
        }
    }
    // error bail out
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਜਾਓ CLEANUP;
    }
    ਜੇ(b1Len == 0){
        *ਸਥਿਤੀ = U_IDNA_ZERO_ਲੰਮਾਈ_LABEL_ERROR;
        ਜਾਓ CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = ਸੱਚ;
    ਲਈ( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        ਜੇ(b1[j] > 0x7F){
            srcIsASCII = ਝੂਠ;
        }ਨਹੀਂਤਾਂ ਜੇ(isLDHChar(b1[j])==ਝੂਠ){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = ਝੂਠ;
            failPos = j;
        }
    }
    ਜੇ(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        ਜੇ( srcIsLDH == ਝੂਠ /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *ਸਥਿਤੀ = U_IDNA_STD3_ASCII_ਨਿਯਮਾਂ_ERROR;

            /* populate the parseError struct */
            ਜੇ(srcIsLDH==ਝੂਠ){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }ਨਹੀਂਤਾਂ ਜੇ(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }ਨਹੀਂਤਾਂ{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            ਜਾਓ CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    ਜੇ(srcIsASCII){
        ਜੇ(b1Len <= dest_ਸਮਰੱਥਾ){
            u_memmove(dest, b1, b1Len);
            req_ਲੰਮਾਈ = b1Len;
        }ਨਹੀਂਤਾਂ{
            req_ਲੰਮਾਈ = b1Len;
            ਜਾਓ CLEANUP;
        }
    }ਨਹੀਂਤਾਂ{
        // step 5 : verify the sequence does not begin with ACE prefix
        ਜੇ(!starts_With_ਅਗੇਤਰ(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
            b2Len = u_str_ਮੰਜ਼ਿਲ_Punycode(b1,b1Len,b2,b2_ਸਮਰੱਥਾ,caseFlags,&ਬਫਰ_ਸਥਿਤੀ);

            ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                ਜੇ(b2 == ਖਾਲੀ_ਸੂਚਕ){
                    *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਜਾਓ CLEANUP;
                }

                ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

                b2Len = u_str_ਮੰਜ਼ਿਲ_Punycode(b1,b1Len,b2,b2Len,caseFlags,&ਬਫਰ_ਸਥਿਤੀ);
            }
            //error bail out
            ਜੇ(U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)){
                *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
                ਜਾਓ CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_ਲੰਮਾਈ = b2Len+ACE_ਅਗੇਤਰ_ਲੰਮਾਈ;

            ਜੇ(req_ਲੰਮਾਈ > dest_ਸਮਰੱਥਾ){
                *ਸਥਿਤੀ = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਜਾਓ CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_ਅਗੇਤਰ, ACE_ਅਗੇਤਰ_ਲੰਮਾਈ);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_ਅਗੇਤਰ_ਲੰਮਾਈ, b2, b2Len);

        }ਨਹੀਂਤਾਂ{
            *ਸਥਿਤੀ = U_IDNA_ACE_ਅਗੇਤਰ_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            ਜਾਓ CLEANUP;
        }
    }
    // step 8: verify the length of label
    ਜੇ(req_ਲੰਮਾਈ > MAX_LABEL_ਲੰਮਾਈ){
        *ਸਥਿਤੀ = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    ਜੇ(b1 != b1_ਢੇਰ){
        uprv_free(b1);
    }
    ਜੇ(b2 != b2_ਢੇਰ){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, ਸਥਿਤੀ);
}

ਸਥਿਰ int32_t
ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
                    char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_ਕੋਡ* ਸਥਿਤੀ)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ਢੇਰ[MAX_LABEL_ਬਫਰ_ਮਾਪ], b2_ਢੇਰ[MAX_LABEL_ਬਫਰ_ਮਾਪ], b3_ਢੇਰ[MAX_LABEL_ਬਫਰ_ਮਾਪ];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ਢੇਰ, *b2 = b2_ਢੇਰ, *b1_ਅਭਾਜ=ਖਾਲੀ_ਸੂਚਕ, *b3=b3_ਢੇਰ;
    int32_t b1Len = 0, b2Len, b1_ਅਭਾਜ_Len, b3Len,
            b1_ਸਮਰੱਥਾ = MAX_LABEL_ਬਫਰ_ਮਾਪ, 
            b2_ਸਮਰੱਥਾ = MAX_LABEL_ਬਫਰ_ਮਾਪ,
            b3_ਸਮਰੱਥਾ = MAX_LABEL_ਬਫਰ_ਮਾਪ,
            req_ਲੰਮਾਈ=0;

    UBool* caseFlags = ਖਾਲੀ_ਸੂਚਕ;

    UBool srcIsASCII = ਸੱਚ;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    ਜੇ(src_ਲੰਮਾਈ==-1){
        src_ਲੰਮਾਈ = 0;
        ਲਈ(;src[src_ਲੰਮਾਈ]!=0;){
            ਜੇ(src[src_ਲੰਮਾਈ]> 0x7f){
                srcIsASCII = ਝੂਠ;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_ਲੰਮਾਈ++;
        }
    }ਨਹੀਂਤਾਂ ਜੇ(src_ਲੰਮਾਈ > 0){
        ਲਈ(int32_t j=0; j<src_ਲੰਮਾਈ; j++){
            ਜੇ(src[j]> 0x7f){
                srcIsASCII = ਝੂਠ;
                ਰੋਕੋ;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }ਨਹੀਂਤਾਂ{
        ਵਾਪਸ 0;
    }
    
    ਜੇ(srcIsASCII == ਝੂਠ){
        // step 2: process the string
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_ਲੰਮਾਈ, b1, b1_ਸਮਰੱਥਾ, namePrepOptions, parseError, &ਬਫਰ_ਸਥਿਤੀ);
        ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ CLEANUP;
            }

            ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_ਲੰਮਾਈ, b1, b1Len, namePrepOptions, parseError, &ਬਫਰ_ਸਥਿਤੀ);
        }
        //bail out on error
        ਜੇ(U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)){
            *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
            ਜਾਓ CLEANUP;
        }
    }ਨਹੀਂਤਾਂ{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_ਲੰਮਾਈ;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    ਜੇ(starts_With_ਅਗੇਤਰ(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_ਅਭਾਜ = b1 + ACE_ਅਗੇਤਰ_ਲੰਮਾਈ;
        b1_ਅਭਾਜ_Len  = b1Len - ACE_ਅਗੇਤਰ_ਲੰਮਾਈ;

        //step 5: Decode using punycode
        UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_ਅਭਾਜ, b1_ਅਭਾਜ_Len, b2, b2_ਸਮਰੱਥਾ, caseFlags, &ਬਫਰ_ਸਥਿਤੀ);

        ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            ਜੇ(b2==ਖਾਲੀ_ਸੂਚਕ){
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ CLEANUP;
            }

            ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_ਅਭਾਜ, b1_ਅਭਾਜ_Len, b2, b2Len, caseFlags, &ਬਫਰ_ਸਥਿਤੀ);
        }


        //step 6:Apply toASCII
        b3Len = uidna_ਮੰਜ਼ਿਲ_ASCII(b2, b2Len, b3, b3_ਸਮਰੱਥਾ, options, parseError, &ਬਫਰ_ਸਥਿਤੀ);

        ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            ਜੇ(b3==ਖਾਲੀ_ਸੂਚਕ){
                *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਜਾਓ CLEANUP;
            }

            ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

            b3Len =  uidna_ਮੰਜ਼ਿਲ_ASCII(b2,b2Len,b3,b3Len,options,parseError,&ਬਫਰ_ਸਥਿਤੀ);

        }
        //bail out on error
        ਜੇ(U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)){
            *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
            ਜਾਓ CLEANUP;
        }

        //step 7: verify
        ਜੇ(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *ਸਥਿਤੀ = U_IDNA_VERIFICATION_ERROR;
            ਜਾਓ CLEANUP;
        }

        //step 8: return output of step 5
        req_ਲੰਮਾਈ = b2Len;
        ਜੇ(b2Len <= dest_ਸਮਰੱਥਾ) {
            u_memmove(dest, b2, b2Len);
        }
    }
    ਨਹੀਂਤਾਂ{
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
        ਜੇ(src_ਲੰਮਾਈ <= dest_ਸਮਰੱਥਾ){
            u_memmove(dest, src, src_ਲੰਮਾਈ);
        }
        req_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
    }


CLEANUP:

    ਜੇ(b1 != b1_ਢੇਰ && b1!=src){
        uprv_free(b1);
    }
    ਜੇ(b2 != b2_ਢੇਰ){
        uprv_free(b2);
    }
    ਜੇ(b3 != b3_ਢੇਰ){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        //copy the source to destination
        ਜੇ(dest && src_ਲੰਮਾਈ <= dest_ਸਮਰੱਥਾ){
            // srcLength should have already been set earlier.
            U_ASSERT(src_ਲੰਮਾਈ >= 0);
            u_memmove(dest, src, src_ਲੰਮਾਈ);
        }
        req_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
        *ਸਥਿਤੀ = U_ZERO_ERROR;
    }

    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uidna_ਮੰਜ਼ਿਲ_ASCII(ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
              char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
              int32_t options,
              UParseError* parseError,
              UError_ਕੋਡ* ਸਥਿਤੀ){
    
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }
    ਜੇ((src==ਖਾਲੀ_ਸੂਚਕ) || (src_ਲੰਮਾਈ < -1) || (dest_ਸਮਰੱਥਾ<0) || (!dest && dest_ਸਮਰੱਥਾ > 0)){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ਸਥਿਤੀ);
    
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ -1;
    }
    
    int32_t retLen = ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ASCII(src, src_ਲੰਮਾਈ, dest, dest_ਸਮਰੱਥਾ, options, nameprep, parseError, ਸਥਿਤੀ);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    ਵਾਪਸ retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_ਮੰਜ਼ਿਲ_Unicode(ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
                char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                int32_t options,
                UParseError* parseError,
                UError_ਕੋਡ* ਸਥਿਤੀ){

    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }
    ਜੇ( (src==ਖਾਲੀ_ਸੂਚਕ) || (src_ਲੰਮਾਈ < -1) || (dest_ਸਮਰੱਥਾ<0) || (!dest && dest_ਸਮਰੱਥਾ > 0)){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ਸਥਿਤੀ);
    
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ -1;
    }
    
    int32_t retLen = ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Unicode(src, src_ਲੰਮਾਈ, dest, dest_ਸਮਰੱਥਾ, options, nameprep, parseError, ਸਥਿਤੀ);

    usprep_close(nameprep);
    
    ਵਾਪਸ retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                   char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                   int32_t options,
                   UParseError *parseError,
                   UError_ਕੋਡ *ਸਥਿਤੀ){

    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }
    ਜੇ((src==ਖਾਲੀ_ਸੂਚਕ) || (src_ਲੰਮਾਈ < -1) || (dest_ਸਮਰੱਥਾ<0) || (!dest && dest_ਸਮਰੱਥਾ > 0)){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t req_ਲੰਮਾਈ = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ਸਥਿਤੀ);
    
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ਸ਼ੁਰੂ = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t ਬਚਿਆ_Len = src_ਲੰਮਾਈ;
    int32_t ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = dest_ਸਮਰੱਥਾ;
    int32_t labelLen = 0, label_Req_ਲੰਮਾਈ = 0;
    UBool ਪੂਰਾ = ਝੂਠ;


    ਲਈ(;;){

        labelLen = get_ਅਗਲਾ_Separator(label_ਸ਼ੁਰੂ,ਬਚਿਆ_Len, &delimiter,&ਪੂਰਾ);
        label_Req_ਲੰਮਾਈ = 0;
        ਜੇ(!(labelLen==0 && ਪੂਰਾ)){// make sure this is not a root label separator.

            UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
            label_Req_ਲੰਮਾਈ = ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_ASCII( label_ਸ਼ੁਰੂ, labelLen,
                                                currentDest, ਬਚਿਆ_Dest_ਸਮਰੱਥਾ,
                                                options, nameprep,
                                                parseError, &ਬਫਰ_ਸਥਿਤੀ);

            ਜੇ (ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
                ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = 0;
            } ਨਹੀਂਤਾਂ ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
                *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
                ਰੋਕੋ;
            }
        }

        req_ਲੰਮਾਈ +=label_Req_ਲੰਮਾਈ;
        // adjust the destination pointer
        ਜੇ(label_Req_ਲੰਮਾਈ < ਬਚਿਆ_Dest_ਸਮਰੱਥਾ){
            currentDest = currentDest + label_Req_ਲੰਮਾਈ;
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ -= label_Req_ਲੰਮਾਈ;
        }ਨਹੀਂਤਾਂ{
            // should never occur
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = 0;
        }

        ਜੇ(ਪੂਰਾ){
            ਰੋਕੋ;
        }

        // add the label separator
        ਜੇ(ਬਚਿਆ_Dest_ਸਮਰੱਥਾ > 0){
            *currentDest++ = ਭਰਿਆ_STOP;
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ--;
        }
        req_ਲੰਮਾਈ++;

        label_ਸ਼ੁਰੂ = delimiter;
        ਜੇ(ਬਚਿਆ_Len >0 ){
            ਬਚਿਆ_Len = (int32_t)(src_ਲੰਮਾਈ - (delimiter - src));
        }

    }

    ਜੇ(req_ਲੰਮਾਈ > MAX_DOMAIN_NAME_ਲੰਮਾਈ){
        *ਸਥਿਤੀ = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ਅਚਲ char16_t* src, int32_t src_ਲੰਮਾਈ,
                     char16_t* dest, int32_t dest_ਸਮਰੱਥਾ,
                     int32_t options,
                     UParseError* parseError,
                     UError_ਕੋਡ* ਸਥਿਤੀ){
    
    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }
    ਜੇ((src==ਖਾਲੀ_ਸੂਚਕ) || (src_ਲੰਮਾਈ < -1) || (dest_ਸਮਰੱਥਾ<0) || (!dest && dest_ਸਮਰੱਥਾ > 0)){
        *ਸਥਿਤੀ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    int32_t req_ਲੰਮਾਈ = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ਸਥਿਤੀ);
    
    ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ਸ਼ੁਰੂ = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t ਬਚਿਆ_Len = src_ਲੰਮਾਈ;
    int32_t ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = dest_ਸਮਰੱਥਾ;
    int32_t labelLen = 0, label_Req_ਲੰਮਾਈ = 0;
    UBool ਪੂਰਾ = ਝੂਠ;

    ਲਈ(;;){

        labelLen = get_ਅਗਲਾ_Separator(label_ਸ਼ੁਰੂ,ਬਚਿਆ_Len, &delimiter,&ਪੂਰਾ);
        
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
        
        label_Req_ਲੰਮਾਈ = ਅੰਦਰੂਨੀ_ਮੰਜ਼ਿਲ_Unicode(label_ਸ਼ੁਰੂ, labelLen, 
                                             currentDest, ਬਚਿਆ_Dest_ਸਮਰੱਥਾ, 
                                             options, nameprep, 
                                             parseError, ਸਥਿਤੀ);

        ਜੇ(*ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
            *ਸਥਿਤੀ = U_ZERO_ERROR; // reset error
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = 0;
        }

        ਜੇ(U_FAILURE(*ਸਥਿਤੀ)){
            ਰੋਕੋ;
        }
        
        req_ਲੰਮਾਈ +=label_Req_ਲੰਮਾਈ;
        // adjust the destination pointer
        ਜੇ(label_Req_ਲੰਮਾਈ < ਬਚਿਆ_Dest_ਸਮਰੱਥਾ){
            currentDest = currentDest + label_Req_ਲੰਮਾਈ;
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ -= label_Req_ਲੰਮਾਈ;
        }ਨਹੀਂਤਾਂ{
            // should never occur
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ = 0;
        }

        ਜੇ(ਪੂਰਾ){
            ਰੋਕੋ;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        ਜੇ(ਬਚਿਆ_Dest_ਸਮਰੱਥਾ > 0){
            *currentDest++ = *(label_ਸ਼ੁਰੂ + labelLen);
            ਬਚਿਆ_Dest_ਸਮਰੱਥਾ--;
        }
        req_ਲੰਮਾਈ++;

        label_ਸ਼ੁਰੂ = delimiter;
        ਜੇ(ਬਚਿਆ_Len >0 ){
            ਬਚਿਆ_Len = (int32_t)(src_ਲੰਮਾਈ - (delimiter - src));
        }

    }

    ਜੇ(req_ਲੰਮਾਈ > MAX_DOMAIN_NAME_ਲੰਮਾਈ){
        *ਸਥਿਤੀ = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, req_ਲੰਮਾਈ, ਸਥਿਤੀ);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ਅਚਲ char16_t *s1, int32_t length1,
                ਅਚਲ char16_t *s2, int32_t length2,
                int32_t options,
                UError_ਕੋਡ* ਸਥਿਤੀ){

    ਜੇ(ਸਥਿਤੀ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ -1;
    }

    char16_t b1_ਢੇਰ[MAX_IDN_ਬਫਰ_ਮਾਪ], b2_ਢੇਰ[MAX_IDN_ਬਫਰ_ਮਾਪ];
    char16_t *b1 = b1_ਢੇਰ, *b2 = b2_ਢੇਰ;
    int32_t b1Len, b2Len, b1_ਸਮਰੱਥਾ = MAX_IDN_ਬਫਰ_ਮਾਪ, b2_ਸਮਰੱਥਾ = MAX_IDN_ਬਫਰ_ਮਾਪ;
    int32_t ਨਤੀਜਾ=-1;
    
    UParseError parseError;

    UError_ਕੋਡ ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_ਸਮਰੱਥਾ, options, &parseError, &ਬਫਰ_ਸਥਿਤੀ);
    ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        ਜੇ(b1==ਖਾਲੀ_ਸੂਚਕ){
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ CLEANUP;
        }

        ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &ਬਫਰ_ਸਥਿਤੀ);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_ਸਮਰੱਥਾ, options, &parseError, &ਬਫਰ_ਸਥਿਤੀ);
    ਜੇ(ਬਫਰ_ਸਥਿਤੀ == U_ਬਫਰ_ਉਛਾਲ_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        ਜੇ(b2==ਖਾਲੀ_ਸੂਚਕ){
            *ਸਥਿਤੀ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਜਾਓ CLEANUP;
        }

        ਬਫਰ_ਸਥਿਤੀ = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &ਬਫਰ_ਸਥਿਤੀ);
    }

    ਜੇ (U_FAILURE(ਬਫਰ_ਸਥਿਤੀ)) {
        *ਸਥਿਤੀ = ਬਫਰ_ਸਥਿਤੀ;
        ਜਾਓ CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    ਨਤੀਜਾ = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    ਜੇ(b1 != b1_ਢੇਰ){
        uprv_free(b1);
    }

    ਜੇ(b2 != b2_ਢੇਰ){
        uprv_free(b2);
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_IDNA */
