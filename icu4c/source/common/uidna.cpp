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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_IDNA

#ಅನ್ತರ್ಭಾವ "unicode/uidna.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/usprep.h"
#ಅನ್ತರ್ಭಾವ "punycode.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "sprpimpl.h"

/* it is official IDNA ACE Prefix is "xn--" */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t ACE_ಪೂರ್ವಪ್ರತ್ಯಯ[] ={ 0x0078,0x006E,0x002d,0x002d } ;
#ವ್ಯಾಖ್ಯಾನಿಸು ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ 4

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_LABEL_ಉದ್ದಳತೆ 63
/* The Max length of the labels should not be more than MAX_LABEL_LENGTH */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_LABEL_ಬಫರ್_ಗಾತ್ರ 100

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DOMAIN_NAME_ಉದ್ದಳತೆ 255
/* The Max length of the domain names should not be more than MAX_DOMAIN_NAME_LENGTH */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_IDN_ಬಫರ್_ಗಾತ್ರ   MAX_DOMAIN_NAME_ಉದ್ದಳತೆ+1

#ವ್ಯಾಖ್ಯಾನಿಸು ಅಧೋ_CASE_DELTA 0x0020
#ವ್ಯಾಖ್ಯಾನಿಸು HYPHEN           0x002D
#ವ್ಯಾಖ್ಯಾನಿಸು ತುಂಬಿದ_STOP        0x002E
#ವ್ಯಾಖ್ಯಾನಿಸು CAPITAL_A        0x0041
#ವ್ಯಾಖ್ಯಾನಿಸು CAPITAL_Z        0x005A

ಸಾಲೊಳಗೆ ಸ್ಥಿರ char16_t
ಗಮ್ಯ_ASCIILower(char16_t ch){
    ಆದರೆ(CAPITAL_A <= ch && ch <= CAPITAL_Z){
        ಹಿಂತಿರುಗಿ ch + ಅಧೋ_CASE_DELTA;
    }
    ಹಿಂತಿರುಗಿ ch;
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರ UBool 
starts_With_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ char16_t* src , int32_t src_ಉದ್ದಳತೆ){
    ಆದರೆ(src_ಉದ್ದಳತೆ < ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಗಾಗಿ(int8_t i=0; i< ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ; i++){
        ಆದರೆ(ಗಮ್ಯ_ASCIILower(src[i]) != ACE_ಪೂರ್ವಪ್ರತ್ಯಯ[i]){
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಸಾಲೊಳಗೆ ಸ್ಥಿರ int32_t
compareCaseInsensitiveASCII(ಸ್ಥಿರಾಂಕ char16_t* s1, int32_t s1Len,
                            ಸ್ಥಿರಾಂಕ char16_t* s2, int32_t s2Len){
    
    int32_t min_ಉದ್ದಳತೆ;
    int32_t ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;

    // are we comparing different lengths?
    ಆದರೆ(s1Len != s2Len) {
        ಆದರೆ(s1Len < s2Len) {
            min_ಉದ್ದಳತೆ = s1Len;
            ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = -1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            min_ಉದ್ದಳತೆ = s2Len;
            ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = 1;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // ok the lengths are equal
        min_ಉದ್ದಳತೆ = s1Len;
        ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = 0;
    }

    char16_t c1,c2;
    int32_t rc;

    ಗಾಗಿ(int32_t i =0;/* no condition */;i++) {

        /* If we reach the ends of both strings then they match */
        ಆದರೆ(i == min_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;
        }
        
        c1 = s1[i];
        c2 = s2[i];
        
        /* Case-insensitive comparison */
        ಆದರೆ(c1!=c2) {
            rc = static_cast<int32_t>(ಗಮ್ಯ_ASCIILower(c1)) - static_cast<int32_t>(ಗಮ್ಯ_ASCIILower(c2));
            ಆದರೆ(rc!=0) {
                ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ=rc;
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;
}


/**
 * Ascertain if the given code point is a label separator as 
 * defined by the IDNA RFC
 * 
 * @param ch The code point to be ascertained
 * @return true if the char is a label separator
 * @stable ICU 2.8
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool isLabelSeparator(char16_t ch){
    ಆಯ್ಕೆ(ch){
        ಸ್ಥಿತಿ 0x002e:
        ಸ್ಥಿತಿ 0x3002:
        ಸ್ಥಿತಿ 0xFF0E:
        ಸ್ಥಿತಿ 0xFF61:
            ಹಿಂತಿರುಗಿ ನಿಜ;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;           
    }
}

// returns the length of the label excluding the separator
// if *limit == separator then the length returned does not include 
// the separtor.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
get_ಮುಂದಿನ_Separator(char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                 char16_t **ಎಲ್ಲೆ, UBool *ಮುಗಿದ){
    ಆದರೆ(src_ಉದ್ದಳತೆ == -1){
        int32_t i;
        ಗಾಗಿ(i=0 ; ;i++){
            ಆದರೆ(src[i] == 0){
                *ಎಲ್ಲೆ = src + i; // point to null
                *ಮುಗಿದ = ನಿಜ;
                ಹಿಂತಿರುಗಿ i;
            }
            ಆದರೆ(isLabelSeparator(src[i])){
                *ಎಲ್ಲೆ = src + (i+1); // go past the delimiter
                ಹಿಂತಿರುಗಿ i;
                
            }
        }
    }ಇಲ್ಲದಿದ್ದರೆ{
        int32_t i;
        ಗಾಗಿ(i=0;i<src_ಉದ್ದಳತೆ;i++){
            ಆದರೆ(isLabelSeparator(src[i])){
                *ಎಲ್ಲೆ = src + (i+1); // go past the delimiter
                ಹಿಂತಿರುಗಿ i;
            }
        }
        // we have not found the delimiter
        // if(i==srcLength)
        *ಎಲ್ಲೆ = src+src_ಉದ್ದಳತೆ;
        *ಮುಗಿದ = ನಿಜ;

        ಹಿಂತಿರುಗಿ i;
    }
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UBool isLDHChar(char16_t ch){
    // high runner case
    ಆದರೆ(ch>0x007A){
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    //[\\u002D \\u0030-\\u0039 \\u0041-\\u005A \\u0061-\\u007A]
    ಆದರೆ( (ch==0x002D) || 
        (0x0030 <= ch && ch <= 0x0039) ||
        (0x0041 <= ch && ch <= 0x005A) ||
        (0x0061 <= ch && ch <= 0x007A)
      ){
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಸ್ಥಿರ int32_t 
ಆಂತರಿಕ_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
                  char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  int32_t options,
                  UStringPrepProfile* nameprep,
                  UParseError* parseError,
                  UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ರಾಶಿ[MAX_LABEL_ಬಫರ್_ಗಾತ್ರ], b2_ರಾಶಿ[MAX_LABEL_ಬಫರ್_ಗಾತ್ರ];
    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ರಾಶಿ, *b2 = b2_ರಾಶಿ;
    int32_t b1Len=0, b2Len, 
            b1_ಸಾಮರ್ಥ್ಯ = MAX_LABEL_ಬಫರ್_ಗಾತ್ರ, 
            b2_ಸಾಮರ್ಥ್ಯ = MAX_LABEL_ಬಫರ್_ಗಾತ್ರ ,
            req_ಉದ್ದಳತೆ=0;

    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0;
    UBool* caseFlags = ಶೂನ್ಯಸೂಚಕ;
    
    // the source contains all ascii codepoints
    UBool srcIsASCII  = ನಿಜ;
    // assume the source contains all LDH codepoints
    UBool srcIsLDH = ನಿಜ; 

    int32_t j=0;

    //get the options
    UBool useSTD3ASCIIRules = static_cast<UBool>((options & UIDNA_USE_STD3_ನಿಯಮಗಳು) != 0);

    int32_t failPos = -1;
    
    ಆದರೆ(src_ಉದ್ದಳತೆ == -1){
        src_ಉದ್ದಳತೆ = u_strlen(src);
    }
    
    ಆದರೆ(src_ಉದ್ದಳತೆ > b1_ಸಾಮರ್ಥ್ಯ){
        b1 = static_cast<char16_t*>(uprv_malloc(src_ಉದ್ದಳತೆ * U_SIZEOF_UCHAR));
        ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು CLEANUP;
        }
        b1_ಸಾಮರ್ಥ್ಯ = src_ಉದ್ದಳತೆ;
    }

    // step 1 
    ಗಾಗಿ( j=0;j<src_ಉದ್ದಳತೆ;j++){
        ಆದರೆ(src[j] > 0x7F){
            srcIsASCII = ಸುಳ್ಳು;
        }
        b1[b1Len++] = src[j];
    }
    
    // step 2 is performed only if the source contains non ASCII
    ಆದರೆ(srcIsASCII == ಸುಳ್ಳು){
        
        // step 2    
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_ಉದ್ದಳತೆ, b1, b1_ಸಾಮರ್ಥ್ಯ, namePrepOptions, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);

        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            // redo processing of string
            // we do not have enough room so grow the buffer
            ಆದರೆ(b1 != b1_ರಾಶಿ){
                uprv_free(b1);
            }
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು CLEANUP;
            }

            ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_ಉದ್ದಳತೆ, b1, b1Len, namePrepOptions, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
        }
        ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
            *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
        }
    }
    // error bail out
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹೋಗು CLEANUP;
    }
    ಆದರೆ(b1Len == 0){
        *ಸ್ಥಿತಿ_2 = U_IDNA_ZERO_ಉದ್ದಳತೆ_LABEL_ERROR;
        ಹೋಗು CLEANUP;
    }

    // for step 3 & 4
    srcIsASCII = ನಿಜ;
    ಗಾಗಿ( j=0;j<b1Len;j++){
        // check if output of usprep_prepare is all ASCII 
        ಆದರೆ(b1[j] > 0x7F){
            srcIsASCII = ಸುಳ್ಳು;
        }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isLDHChar(b1[j])==ಸುಳ್ಳು){  // if the char is in ASCII range verify that it is an LDH character
            srcIsLDH = ಸುಳ್ಳು;
            failPos = j;
        }
    }
    ಆದರೆ(useSTD3ASCIIRules){
        // verify 3a and 3b
        // 3(a) Verify the absence of non-LDH ASCII code points; that is, the
        //  absence of 0..2C, 2E..2F, 3A..40, 5B..60, and 7B..7F.
        // 3(b) Verify the absence of leading and trailing hyphen-minus; that
        //  is, the absence of U+002D at the beginning and end of the
        //  sequence.
        ಆದರೆ( srcIsLDH == ಸುಳ್ಳು /* source at this point should not contain anyLDH characters */
            || b1[0] ==  HYPHEN || b1[b1Len-1] == HYPHEN){
            *ಸ್ಥಿತಿ_2 = U_IDNA_STD3_ASCII_ನಿಯಮಗಳು_ERROR;

            /* populate the parseError struct */
            ಆದರೆ(srcIsLDH==ಸುಳ್ಳು){
                // failPos is always set the index of failure
                uprv_syntaxError(b1,failPos, b1Len,parseError);
            }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b1[0] == HYPHEN){
                // fail position is 0 
                uprv_syntaxError(b1,0,b1Len,parseError);
            }ಇಲ್ಲದಿದ್ದರೆ{
                // the last index in the source is always length-1
                uprv_syntaxError(b1, (b1Len>0) ? b1Len-1 : b1Len, b1Len,parseError);
            }

            ಹೋಗು CLEANUP;
        }
    }
    // Step 4: if the source is ASCII then proceed to step 8
    ಆದರೆ(srcIsASCII){
        ಆದರೆ(b1Len <= dest_ಸಾಮರ್ಥ್ಯ){
            u_memmove(dest, b1, b1Len);
            req_ಉದ್ದಳತೆ = b1Len;
        }ಇಲ್ಲದಿದ್ದರೆ{
            req_ಉದ್ದಳತೆ = b1Len;
            ಹೋಗು CLEANUP;
        }
    }ಇಲ್ಲದಿದ್ದರೆ{
        // step 5 : verify the sequence does not begin with ACE prefix
        ಆದರೆ(!starts_With_ಪೂರ್ವಪ್ರತ್ಯಯ(b1,b1Len)){

            //step 6: encode the sequence with punycode

            // do not preserve the case flags for now!
            // TODO: Preserve the case while implementing the RFE
            // caseFlags = (UBool*) uprv_malloc(b1Len * sizeof(UBool));
            // uprv_memset(caseFlags,true,b1Len);

            UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            b2Len = u_str_ಗಮ್ಯ_Punycode(b1,b1Len,b2,b2_ಸಾಮರ್ಥ್ಯ,caseFlags,&ಬಫರ್_ಸ್ಥಿತಿ);

            ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
                // redo processing of string
                /* we do not have enough room so grow the buffer*/
                b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
                ಆದರೆ(b2 == ಶೂನ್ಯಸೂಚಕ){
                    *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ಹೋಗು CLEANUP;
                }

                ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

                b2Len = u_str_ಗಮ್ಯ_Punycode(b1,b1Len,b2,b2Len,caseFlags,&ಬಫರ್_ಸ್ಥಿತಿ);
            }
            //error bail out
            ಆದರೆ(U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)){
                *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
                ಹೋಗು CLEANUP;
            }
            // TODO : Reconsider while implementing the case preserve RFE
            // convert all codepoints to lower case ASCII
            // toASCIILower(b2,b2Len);
            req_ಉದ್ದಳತೆ = b2Len+ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;

            ಆದರೆ(req_ಉದ್ದಳತೆ > dest_ಸಾಮರ್ಥ್ಯ){
                *ಸ್ಥಿತಿ_2 = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ಹೋಗು CLEANUP;
            }
            //Step 7: prepend the ACE prefix
            u_memcpy(dest, ACE_ಪೂರ್ವಪ್ರತ್ಯಯ, ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ);
            //Step 6: copy the contents in b2 into dest
            u_memcpy(dest+ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ, b2, b2Len);

        }ಇಲ್ಲದಿದ್ದರೆ{
            *ಸ್ಥಿತಿ_2 = U_IDNA_ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ERROR; 
            //position of failure is 0
            uprv_syntaxError(b1,0,b1Len,parseError);
            ಹೋಗು CLEANUP;
        }
    }
    // step 8: verify the length of label
    ಆದರೆ(req_ಉದ್ದಳತೆ > MAX_LABEL_ಉದ್ದಳತೆ){
        *ಸ್ಥಿತಿ_2 = U_IDNA_LABEL_TOO_LONG_ERROR;
    }

CLEANUP:
    ಆದರೆ(b1 != b1_ರಾಶಿ){
        uprv_free(b1);
    }
    ಆದರೆ(b2 != b2_ರಾಶಿ){
        uprv_free(b2);
    }
    uprv_free(caseFlags);
    
    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
}

ಸ್ಥಿರ int32_t
ಆಂತರಿಕ_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
                    char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                    int32_t options,
                    UStringPrepProfile* nameprep,
                    UParseError* parseError,
                    UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2)
{

    //get the options
    //UBool useSTD3ASCIIRules = (UBool)((options & UIDNA_USE_STD3_RULES) != 0);
    int32_t namePrepOptions = ((options & UIDNA_ALLOW_UNASSIGNED) != 0) ? USPREP_ALLOW_UNASSIGNED: 0; 

    // TODO Revisit buffer handling. The label should not be over 63 ASCII characters. ICU4J may need to be updated too.
    char16_t b1_ರಾಶಿ[MAX_LABEL_ಬಫರ್_ಗಾತ್ರ], b2_ರಾಶಿ[MAX_LABEL_ಬಫರ್_ಗಾತ್ರ], b3_ರಾಶಿ[MAX_LABEL_ಬಫರ್_ಗಾತ್ರ];

    //initialize pointers to stack buffers
    char16_t  *b1 = b1_ರಾಶಿ, *b2 = b2_ರಾಶಿ, *b1_ಅವಿಭಾಜ್ಯ=ಶೂನ್ಯಸೂಚಕ, *b3=b3_ರಾಶಿ;
    int32_t b1Len = 0, b2Len, b1_ಅವಿಭಾಜ್ಯ_Len, b3Len,
            b1_ಸಾಮರ್ಥ್ಯ = MAX_LABEL_ಬಫರ್_ಗಾತ್ರ, 
            b2_ಸಾಮರ್ಥ್ಯ = MAX_LABEL_ಬಫರ್_ಗಾತ್ರ,
            b3_ಸಾಮರ್ಥ್ಯ = MAX_LABEL_ಬಫರ್_ಗಾತ್ರ,
            req_ಉದ್ದಳತೆ=0;

    UBool* caseFlags = ಶೂನ್ಯಸೂಚಕ;

    UBool srcIsASCII = ನಿಜ;
    /*UBool srcIsLDH = true;
    int32_t failPos =0;*/

    // step 1: find out if all the codepoints in src are ASCII  
    ಆದರೆ(src_ಉದ್ದಳತೆ==-1){
        src_ಉದ್ದಳತೆ = 0;
        ಗಾಗಿ(;src[src_ಉದ್ದಳತೆ]!=0;){
            ಆದರೆ(src[src_ಉದ್ದಳತೆ]> 0x7f){
                srcIsASCII = ಸುಳ್ಳು;
            }/*else if(isLDHChar(src[srcLength])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = srcLength;
            }*/
            src_ಉದ್ದಳತೆ++;
        }
    }ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(src_ಉದ್ದಳತೆ > 0){
        ಗಾಗಿ(int32_t j=0; j<src_ಉದ್ದಳತೆ; j++){
            ಆದರೆ(src[j]> 0x7f){
                srcIsASCII = ಸುಳ್ಳು;
                ನಿಲ್ಲಿಸು;
            }/*else if(isLDHChar(src[j])==false){
                // here we do not assemble surrogates
                // since we know that LDH code points
                // are in the ASCII range only
                srcIsLDH = false;
                failPos = j;
            }*/
        }
    }ಇಲ್ಲದಿದ್ದರೆ{
        ಹಿಂತಿರುಗಿ 0;
    }
    
    ಆದರೆ(srcIsASCII == ಸುಳ್ಳು){
        // step 2: process the string
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        b1Len = usprep_prepare(nameprep, src, src_ಉದ್ದಳತೆ, b1, b1_ಸಾಮರ್ಥ್ಯ, namePrepOptions, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b1 = static_cast<char16_t*>(uprv_malloc(b1Len * U_SIZEOF_UCHAR));
            ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು CLEANUP;
            }

            ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

            b1Len = usprep_prepare(nameprep, src, src_ಉದ್ದಳತೆ, b1, b1Len, namePrepOptions, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
        }
        //bail out on error
        ಆದರೆ(U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)){
            *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
            ಹೋಗು CLEANUP;
        }
    }ಇಲ್ಲದಿದ್ದರೆ{

        //just point src to b1
        b1 = const_cast<char16_t*>(src);
        b1Len = src_ಉದ್ದಳತೆ;
    }

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>

    //step 3: verify ACE Prefix
    ಆದರೆ(starts_With_ಪೂರ್ವಪ್ರತ್ಯಯ(b1,b1Len)){

        //step 4: Remove the ACE Prefix
        b1_ಅವಿಭಾಜ್ಯ = b1 + ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        b1_ಅವಿಭಾಜ್ಯ_Len  = b1Len - ACE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;

        //step 5: Decode using punycode
        UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
        b2Len = u_strFromPunycode(b1_ಅವಿಭಾಜ್ಯ, b1_ಅವಿಭಾಜ್ಯ_Len, b2, b2_ಸಾಮರ್ಥ್ಯ, caseFlags, &ಬಫರ್_ಸ್ಥಿತಿ);

        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b2 = static_cast<char16_t*>(uprv_malloc(b2Len * U_SIZEOF_UCHAR));
            ಆದರೆ(b2==ಶೂನ್ಯಸೂಚಕ){
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು CLEANUP;
            }

            ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

            b2Len =  u_strFromPunycode(b1_ಅವಿಭಾಜ್ಯ, b1_ಅವಿಭಾಜ್ಯ_Len, b2, b2Len, caseFlags, &ಬಫರ್_ಸ್ಥಿತಿ);
        }


        //step 6:Apply toASCII
        b3Len = uidna_ಗಮ್ಯ_ASCII(b2, b2Len, b3, b3_ಸಾಮರ್ಥ್ಯ, options, parseError, &ಬಫರ್_ಸ್ಥಿತಿ);

        ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            // redo processing of string
            /* we do not have enough room so grow the buffer*/
            b3 = static_cast<char16_t*>(uprv_malloc(b3Len * U_SIZEOF_UCHAR));
            ಆದರೆ(b3==ಶೂನ್ಯಸೂಚಕ){
                *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹೋಗು CLEANUP;
            }

            ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

            b3Len =  uidna_ಗಮ್ಯ_ASCII(b2,b2Len,b3,b3Len,options,parseError,&ಬಫರ್_ಸ್ಥಿತಿ);

        }
        //bail out on error
        ಆದರೆ(U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)){
            *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
            ಹೋಗು CLEANUP;
        }

        //step 7: verify
        ಆದರೆ(compareCaseInsensitiveASCII(b1, b1Len, b3, b3Len) !=0){
            // Cause the original to be returned.
            *ಸ್ಥಿತಿ_2 = U_IDNA_VERIFICATION_ERROR;
            ಹೋಗು CLEANUP;
        }

        //step 8: return output of step 5
        req_ಉದ್ದಳತೆ = b2Len;
        ಆದರೆ(b2Len <= dest_ಸಾಮರ್ಥ್ಯ) {
            u_memmove(dest, b2, b2Len);
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ{
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
        ಆದರೆ(src_ಉದ್ದಳತೆ <= dest_ಸಾಮರ್ಥ್ಯ){
            u_memmove(dest, src, src_ಉದ್ದಳತೆ);
        }
        req_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
    }


CLEANUP:

    ಆದರೆ(b1 != b1_ರಾಶಿ && b1!=src){
        uprv_free(b1);
    }
    ಆದರೆ(b2 != b2_ರಾಶಿ){
        uprv_free(b2);
    }
    ಆದರೆ(b3 != b3_ರಾಶಿ){
        uprv_free(b3);
    }
    uprv_free(caseFlags);

    // The RFC states that 
    // <quote>
    // ToUnicode never fails. If any step fails, then the original input
    // is returned immediately in that step.
    // </quote>
    // So if any step fails lets copy source to destination
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        //copy the source to destination
        ಆದರೆ(dest && src_ಉದ್ದಳತೆ <= dest_ಸಾಮರ್ಥ್ಯ){
            // srcLength should have already been set earlier.
            U_ASSERT(src_ಉದ್ದಳತೆ >= 0);
            u_memmove(dest, src, src_ಉದ್ದಳತೆ);
        }
        req_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
        *ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
    }

    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uidna_ಗಮ್ಯ_ASCII(ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
              char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              int32_t options,
              UParseError* parseError,
              UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){
    
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ((src==ಶೂನ್ಯಸೂಚಕ) || (src_ಉದ್ದಳತೆ < -1) || (dest_ಸಾಮರ್ಥ್ಯ<0) || (!dest && dest_ಸಾಮರ್ಥ್ಯ > 0)){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ -1;
    }
    
    int32_t retLen = ಆಂತರಿಕ_ಗಮ್ಯ_ASCII(src, src_ಉದ್ದಳತೆ, dest, dest_ಸಾಮರ್ಥ್ಯ, options, nameprep, parseError, ಸ್ಥಿತಿ_2);
    
    /* close the profile*/
    usprep_close(nameprep);
    
    ಹಿಂತಿರುಗಿ retLen;
}

U_CAPI int32_t U_EXPORT2
uidna_ಗಮ್ಯ_Unicode(ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
                char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                int32_t options,
                UParseError* parseError,
                UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){

    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ( (src==ಶೂನ್ಯಸೂಚಕ) || (src_ಉದ್ದಳತೆ < -1) || (dest_ಸಾಮರ್ಥ್ಯ<0) || (!dest && dest_ಸಾಮರ್ಥ್ಯ > 0)){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }  

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ -1;
    }
    
    int32_t retLen = ಆಂತರಿಕ_ಗಮ್ಯ_Unicode(src, src_ಉದ್ದಳತೆ, dest, dest_ಸಾಮರ್ಥ್ಯ, options, nameprep, parseError, ಸ್ಥಿತಿ_2);

    usprep_close(nameprep);
    
    ಹಿಂತಿರುಗಿ retLen;
}


U_CAPI int32_t U_EXPORT2
uidna_IDNToASCII(  ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                   char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                   int32_t options,
                   UParseError *parseError,
                   UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2){

    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ((src==ಶೂನ್ಯಸೂಚಕ) || (src_ಉದ್ದಳತೆ < -1) || (dest_ಸಾಮರ್ಥ್ಯ<0) || (!dest && dest_ಸಾಮರ್ಥ್ಯ > 0)){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t req_ಉದ್ದಳತೆ = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }

    //initialize pointers 
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ಆರಂಭ = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t ಉಳಿದ_Len = src_ಉದ್ದಳತೆ;
    int32_t ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = dest_ಸಾಮರ್ಥ್ಯ;
    int32_t labelLen = 0, label_Req_ಉದ್ದಳತೆ = 0;
    UBool ಮುಗಿದ = ಸುಳ್ಳು;


    ಗಾಗಿ(;;){

        labelLen = get_ಮುಂದಿನ_Separator(label_ಆರಂಭ,ಉಳಿದ_Len, &delimiter,&ಮುಗಿದ);
        label_Req_ಉದ್ದಳತೆ = 0;
        ಆದರೆ(!(labelLen==0 && ಮುಗಿದ)){// make sure this is not a root label separator.

            UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
            label_Req_ಉದ್ದಳತೆ = ಆಂತರಿಕ_ಗಮ್ಯ_ASCII( label_ಆರಂಭ, labelLen,
                                                currentDest, ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ,
                                                options, nameprep,
                                                parseError, &ಬಫರ್_ಸ್ಥಿತಿ);

            ಆದರೆ (ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
                ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = 0;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
                *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
                ನಿಲ್ಲಿಸು;
            }
        }

        req_ಉದ್ದಳತೆ +=label_Req_ಉದ್ದಳತೆ;
        // adjust the destination pointer
        ಆದರೆ(label_Req_ಉದ್ದಳತೆ < ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ){
            currentDest = currentDest + label_Req_ಉದ್ದಳತೆ;
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ -= label_Req_ಉದ್ದಳತೆ;
        }ಇಲ್ಲದಿದ್ದರೆ{
            // should never occur
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = 0;
        }

        ಆದರೆ(ಮುಗಿದ){
            ನಿಲ್ಲಿಸು;
        }

        // add the label separator
        ಆದರೆ(ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ > 0){
            *currentDest++ = ತುಂಬಿದ_STOP;
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ--;
        }
        req_ಉದ್ದಳತೆ++;

        label_ಆರಂಭ = delimiter;
        ಆದರೆ(ಉಳಿದ_Len >0 ){
            ಉಳಿದ_Len = (int32_t)(src_ಉದ್ದಳತೆ - (delimiter - src));
        }

    }

    ಆದರೆ(req_ಉದ್ದಳತೆ > MAX_DOMAIN_NAME_ಉದ್ದಳತೆ){
        *ಸ್ಥಿತಿ_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uidna_IDNToUnicode(  ಸ್ಥಿರಾಂಕ char16_t* src, int32_t src_ಉದ್ದಳತೆ,
                     char16_t* dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                     int32_t options,
                     UParseError* parseError,
                     UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){
    
    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ((src==ಶೂನ್ಯಸೂಚಕ) || (src_ಉದ್ದಳತೆ < -1) || (dest_ಸಾಮರ್ಥ್ಯ<0) || (!dest && dest_ಸಾಮರ್ಥ್ಯ > 0)){
        *ಸ್ಥಿತಿ_2 = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    int32_t req_ಉದ್ದಳತೆ = 0;

    UStringPrepProfile* nameprep = usprep_openByType(USPREP_RFC3491_NAMEPREP, ಸ್ಥಿತಿ_2);
    
    ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ 0;
    }

    //initialize pointers
    char16_t* delimiter = const_cast<char16_t*>(src);
    char16_t* label_ಆರಂಭ = const_cast<char16_t*>(src);
    char16_t* currentDest = dest;
    int32_t ಉಳಿದ_Len = src_ಉದ್ದಳತೆ;
    int32_t ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = dest_ಸಾಮರ್ಥ್ಯ;
    int32_t labelLen = 0, label_Req_ಉದ್ದಳತೆ = 0;
    UBool ಮುಗಿದ = ಸುಳ್ಳು;

    ಗಾಗಿ(;;){

        labelLen = get_ಮುಂದಿನ_Separator(label_ಆರಂಭ,ಉಳಿದ_Len, &delimiter,&ಮುಗಿದ);
        
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
        
        label_Req_ಉದ್ದಳತೆ = ಆಂತರಿಕ_ಗಮ್ಯ_Unicode(label_ಆರಂಭ, labelLen, 
                                             currentDest, ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ, 
                                             options, nameprep, 
                                             parseError, ಸ್ಥಿತಿ_2);

        ಆದರೆ(*ಸ್ಥಿತಿ_2 == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
            *ಸ್ಥಿತಿ_2 = U_ZERO_ERROR; // reset error
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = 0;
        }

        ಆದರೆ(U_FAILURE(*ಸ್ಥಿತಿ_2)){
            ನಿಲ್ಲಿಸು;
        }
        
        req_ಉದ್ದಳತೆ +=label_Req_ಉದ್ದಳತೆ;
        // adjust the destination pointer
        ಆದರೆ(label_Req_ಉದ್ದಳತೆ < ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ){
            currentDest = currentDest + label_Req_ಉದ್ದಳತೆ;
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ -= label_Req_ಉದ್ದಳತೆ;
        }ಇಲ್ಲದಿದ್ದರೆ{
            // should never occur
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ = 0;
        }

        ಆದರೆ(ಮುಗಿದ){
            ನಿಲ್ಲಿಸು;
        }

        // add the label separator
        // Unlike the ToASCII operation we don't normalize the label separators
        ಆದರೆ(ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ > 0){
            *currentDest++ = *(label_ಆರಂಭ + labelLen);
            ಉಳಿದ_Dest_ಸಾಮರ್ಥ್ಯ--;
        }
        req_ಉದ್ದಳತೆ++;

        label_ಆರಂಭ = delimiter;
        ಆದರೆ(ಉಳಿದ_Len >0 ){
            ಉಳಿದ_Len = (int32_t)(src_ಉದ್ದಳತೆ - (delimiter - src));
        }

    }

    ಆದರೆ(req_ಉದ್ದಳತೆ > MAX_DOMAIN_NAME_ಉದ್ದಳತೆ){
        *ಸ್ಥಿತಿ_2 = U_IDNA_DOMAIN_NAME_TOO_LONG_ERROR;
    }

    usprep_close(nameprep);
    
    ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, req_ಉದ್ದಳತೆ, ಸ್ಥಿತಿ_2);
}

U_CAPI int32_t U_EXPORT2
uidna_compare(  ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
                ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
                int32_t options,
                UError_ಸಂಕೇತ* ಸ್ಥಿತಿ_2){

    ಆದರೆ(ಸ್ಥಿತಿ_2 == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ -1;
    }

    char16_t b1_ರಾಶಿ[MAX_IDN_ಬಫರ್_ಗಾತ್ರ], b2_ರಾಶಿ[MAX_IDN_ಬಫರ್_ಗಾತ್ರ];
    char16_t *b1 = b1_ರಾಶಿ, *b2 = b2_ರಾಶಿ;
    int32_t b1Len, b2Len, b1_ಸಾಮರ್ಥ್ಯ = MAX_IDN_ಬಫರ್_ಗಾತ್ರ, b2_ಸಾಮರ್ಥ್ಯ = MAX_IDN_ಬಫರ್_ಗಾತ್ರ;
    int32_t ಫಲಿತಾಂಶ=-1;
    
    UParseError parseError;

    UError_ಸಂಕೇತ ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR;
    b1Len = uidna_IDNToASCII(s1, length1, b1, b1_ಸಾಮರ್ಥ್ಯ, options, &parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
    ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
        // redo processing of string
        b1 = (char16_t*) uprv_malloc(b1Len * U_SIZEOF_UCHAR);
        ಆದರೆ(b1==ಶೂನ್ಯಸೂಚಕ){
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು CLEANUP;
        }

        ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

        b1Len = uidna_IDNToASCII(s1,length1,b1,b1Len, options, &parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
    }

    b2Len = uidna_IDNToASCII(s2,length2, b2,b2_ಸಾಮರ್ಥ್ಯ, options, &parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
    ಆದರೆ(ಬಫರ್_ಸ್ಥಿತಿ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR){
        // redo processing of string
        b2 = (char16_t*) uprv_malloc(b2Len * U_SIZEOF_UCHAR);
        ಆದರೆ(b2==ಶೂನ್ಯಸೂಚಕ){
            *ಸ್ಥಿತಿ_2 = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹೋಗು CLEANUP;
        }

        ಬಫರ್_ಸ್ಥಿತಿ = U_ZERO_ERROR; // reset error

        b2Len = uidna_IDNToASCII(s2, length2, b2, b2Len, options, &parseError, &ಬಫರ್_ಸ್ಥಿತಿ);
    }

    ಆದರೆ (U_FAILURE(ಬಫರ್_ಸ್ಥಿತಿ)) {
        *ಸ್ಥಿತಿ_2 = ಬಫರ್_ಸ್ಥಿತಿ;
        ಹೋಗು CLEANUP;
    }

    // when toASCII is applied all label separators are replaced with FULL_STOP
    ಫಲಿತಾಂಶ = compareCaseInsensitiveASCII(b1,b1Len,b2,b2Len);

CLEANUP:
    ಆದರೆ(b1 != b1_ರಾಶಿ){
        uprv_free(b1);
    }

    ಆದರೆ(b2 != b2_ರಾಶಿ){
        uprv_free(b2);
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_IDNA */
