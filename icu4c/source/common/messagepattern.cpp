// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2011-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  messagepattern.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2011mar14
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_FORMATTING

#ಅನ್ತರ್ಭಾವ "unicode/messagepattern.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "messageimpl.h"
#ಅನ್ತರ್ಭಾವ "patternprops.h"
#ಅನ್ತರ್ಭಾವ "putilimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_pound=0x23;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_apos=0x27;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_plus=0x2B;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_comma=0x2C;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_minus=0x2D;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_dot=0x2E;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_colon=0x3A;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_lessThan=0x3C;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_equal=0x3D;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_A=0x41;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_C=0x43;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_D=0x44;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_E=0x45;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_H=0x48;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_I=0x49;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_L=0x4C;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_N=0x4E;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_O=0x4F;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_P=0x50;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_R=0x52;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_S=0x53;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_T=0x54;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_U=0x55;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_Z=0x5A;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_a=0x61;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_c=0x63;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_d=0x64;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_e=0x65;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_f=0x66;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_h=0x68;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_i=0x69;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_l=0x6C;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_n=0x6E;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_o=0x6F;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_p=0x70;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_r=0x72;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_s=0x73;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_t=0x74;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_u=0x75;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_z=0x7A;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_ಎಡ_Curly_Brace=0x7B;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_pipe=0x7C;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_ಬಲ_Curly_Brace=0x7D;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t k_ಸ್ಥಾನಾಂತರ_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ವರ್ಗ ಸಂದೇಶ_ನಮೂನೆ_List : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    ಸಂದೇಶ_ನಮೂನೆ_List() {}
    ಶೂನ್ಯ copyFrom(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ_List<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> &other,
                  int32_t length,
                  UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    UBool ensure_ಸಾಮರ್ಥ್ಯ_For_One_ಇನ್ನಷ್ಟು(int32_t old_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);
    UBool equals(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ_List<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> &other, int32_t length) ಸ್ಥಿರಾಂಕ {
        ಗಾಗಿ(int32_t i=0; i<length; ++i) {
            ಆದರೆ(a[i]!=other.a[i]) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> a;
};

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ_List<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::copyFrom(
        ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ_List<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ> &other,
        int32_t length,
        UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && length>0) {
        ಆದರೆ(length>a.get_ಸಾಮರ್ಥ್ಯ() && ಶೂನ್ಯಸೂಚಕ==a.resize(length)) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*ಗಾತ್ರ(T));
    }
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, int32_t ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>
UBool
ಸಂದೇಶ_ನಮೂನೆ_List<T, ರಾಶಿ_ಸಾಮರ್ಥ್ಯ>::ensure_ಸಾಮರ್ಥ್ಯ_For_One_ಇನ್ನಷ್ಟು(int32_t old_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಆದರೆ(a.get_ಸಾಮರ್ಥ್ಯ()>old_ಉದ್ದಳತೆ || a.resize(2*old_ಉದ್ದಳತೆ, old_ಉದ್ದಳತೆ)!=ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

// MessagePatternList specializations -------------------------------------- ***

ವರ್ಗ ಸಂದೇಶ_ನಮೂನೆ_Double_List : ಸಾರ್ವಜನಿಕ ಸಂದೇಶ_ನಮೂನೆ_List<ದ್ವಿಗುಣ, 8> {
};

ವರ್ಗ ಸಂದೇಶ_ನಮೂನೆ_Parts_List : ಸಾರ್ವಜನಿಕ ಸಂದೇಶ_ನಮೂನೆ_List<ಸಂದೇಶ_ನಮೂನೆ::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

ಸಂದೇಶ_ನಮೂನೆ::ಸಂದೇಶ_ನಮೂನೆ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : apos_ಬಹುಲಕ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ಬಹುಲಕ),
          partsList(ಶೂನ್ಯಸೂಚಕ), parts(ಶೂನ್ಯಸೂಚಕ), parts_ಉದ್ದಳತೆ(0),
          numeric_ಮೌಲ್ಯಗಳು_List(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ(0),
          has_Arg_ಹೆಸರುಗಳು(ಸುಳ್ಳು), hasArgNumbers(ಸುಳ್ಳು), needsAutoQuoting(ಸುಳ್ಳು) {
    init(error_ಸಂಕೇತ);
}

ಸಂದೇಶ_ನಮೂನೆ::ಸಂದೇಶ_ನಮೂನೆ(UMessage_ನಮೂನೆ_Apostrophe_ಬಹುಲಕ ಬಹುಲಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : apos_ಬಹುಲಕ(ಬಹುಲಕ),
          partsList(ಶೂನ್ಯಸೂಚಕ), parts(ಶೂನ್ಯಸೂಚಕ), parts_ಉದ್ದಳತೆ(0),
          numeric_ಮೌಲ್ಯಗಳು_List(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ(0),
          has_Arg_ಹೆಸರುಗಳು(ಸುಳ್ಳು), hasArgNumbers(ಸುಳ್ಳು), needsAutoQuoting(ಸುಳ್ಳು) {
    init(error_ಸಂಕೇತ);
}

ಸಂದೇಶ_ನಮೂನೆ::ಸಂದೇಶ_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ, UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : apos_ಬಹುಲಕ(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_ಬಹುಲಕ),
          partsList(ಶೂನ್ಯಸೂಚಕ), parts(ಶೂನ್ಯಸೂಚಕ), parts_ಉದ್ದಳತೆ(0),
          numeric_ಮೌಲ್ಯಗಳು_List(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ(0),
          has_Arg_ಹೆಸರುಗಳು(ಸುಳ್ಳು), hasArgNumbers(ಸುಳ್ಳು), needsAutoQuoting(ಸುಳ್ಳು) {
    ಆದರೆ(init(error_ಸಂಕೇತ)) {
        parse(ನಮೂನೆ, parseError, error_ಸಂಕೇತ);
    }
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::init(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    partsList=ಹೊಸ ಸಂದೇಶ_ನಮೂನೆ_Parts_List();
    ಆದರೆ(partsList==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    parts=partsList->a.getAlias();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸಂದೇಶ_ನಮೂನೆ::ಸಂದೇಶ_ನಮೂನೆ(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ &other)
        : UObject(other), apos_ಬಹುಲಕ(other.apos_ಬಹುಲಕ), ಸಂದೇಶ(other.ಸಂದೇಶ),
          partsList(ಶೂನ್ಯಸೂಚಕ), parts(ಶೂನ್ಯಸೂಚಕ), parts_ಉದ್ದಳತೆ(0),
          numeric_ಮೌಲ್ಯಗಳು_List(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು(ಶೂನ್ಯಸೂಚಕ), numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ(0),
          has_Arg_ಹೆಸರುಗಳು(other.has_Arg_ಹೆಸರುಗಳು), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಆದರೆ(!copyStorage(other, error_ಸಂಕೇತ)) {
        clear();
    }
}

ಸಂದೇಶ_ನಮೂನೆ&
ಸಂದೇಶ_ನಮೂನೆ::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ& other) U_LIFETIME_BOUND {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ *ಇದು;
    }
    apos_ಬಹುಲಕ=other.apos_ಬಹುಲಕ;
    ಸಂದೇಶ=other.ಸಂದೇಶ;
    has_Arg_ಹೆಸರುಗಳು=other.has_Arg_ಹೆಸರುಗಳು;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಆದರೆ(!copyStorage(other, error_ಸಂಕೇತ)) {
        clear();
    }
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::copyStorage(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ &other, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    parts=ಶೂನ್ಯಸೂಚಕ;
    parts_ಉದ್ದಳತೆ=0;
    numeric_ಮೌಲ್ಯಗಳು=ಶೂನ್ಯಸೂಚಕ;
    numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ=0;
    ಆದರೆ(partsList==ಶೂನ್ಯಸೂಚಕ) {
        partsList=ಹೊಸ ಸಂದೇಶ_ನಮೂನೆ_Parts_List();
        ಆದರೆ(partsList==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        parts=partsList->a.getAlias();
    }
    ಆದರೆ(other.parts_ಉದ್ದಳತೆ>0) {
        partsList->copyFrom(*other.partsList, other.parts_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        parts=partsList->a.getAlias();
        parts_ಉದ್ದಳತೆ=other.parts_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(other.numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ>0) {
        ಆದರೆ(numeric_ಮೌಲ್ಯಗಳು_List==ಶೂನ್ಯಸೂಚಕ) {
            numeric_ಮೌಲ್ಯಗಳು_List=ಹೊಸ ಸಂದೇಶ_ನಮೂನೆ_Double_List();
            ಆದರೆ(numeric_ಮೌಲ್ಯಗಳು_List==ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
        }
        numeric_ಮೌಲ್ಯಗಳು_List->copyFrom(
            *other.numeric_ಮೌಲ್ಯಗಳು_List, other.numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
        numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ=other.numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸಂದೇಶ_ನಮೂನೆ::~ಸಂದೇಶ_ನಮೂನೆ() {
    ಅಳಿಸು partsList;
    ಅಳಿಸು numeric_ಮೌಲ್ಯಗಳು_List;
}

// MessagePattern API ------------------------------------------------------ ***

ಸಂದೇಶ_ನಮೂನೆ&
ಸಂದೇಶ_ನಮೂನೆ::parse(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                      UParseError* parseError,
                      UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    preParse(ನಮೂನೆ, parseError, error_ಸಂಕೇತ);
    parse_ಸಂದೇಶ(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_ಸಂಕೇತ);
    postParse();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಸಂದೇಶ_ನಮೂನೆ&
ಸಂದೇಶ_ನಮೂನೆ::parseChoiceStyle(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                 UParseError* parseError,
                                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    preParse(ನಮೂನೆ, parseError, error_ಸಂಕೇತ);
    parseChoiceStyle(0, 0, parseError, error_ಸಂಕೇತ);
    postParse();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಸಂದೇಶ_ನಮೂನೆ&
ಸಂದೇಶ_ನಮೂನೆ::parsePluralStyle(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                 UParseError* parseError,
                                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    preParse(ನಮೂನೆ, parseError, error_ಸಂಕೇತ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_ಸಂಕೇತ);
    postParse();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಸಂದೇಶ_ನಮೂನೆ&
ಸಂದೇಶ_ನಮೂನೆ::parseSelectStyle(ಸ್ಥಿರಾಂಕ UnicodeString& ನಮೂನೆ,
                                 UParseError* parseError,
                                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) U_LIFETIME_BOUND {
    preParse(ನಮೂನೆ, parseError, error_ಸಂಕೇತ);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_ಸಂಕೇತ);
    postParse();
    ಹಿಂತಿರುಗಿ *ಇದು;
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::clear() {
    // Mostly the same as preParse().
    ಸಂದೇಶ.remove();
    has_Arg_ಹೆಸರುಗಳು=hasArgNumbers=ಸುಳ್ಳು;
    needsAutoQuoting=ಸುಳ್ಳು;
    parts_ಉದ್ದಳತೆ=0;
    numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ=0;
}

ತಾರ್ಕಿಕ
ಸಂದೇಶ_ನಮೂನೆ::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ
        apos_ಬಹುಲಕ==other.apos_ಬಹುಲಕ &&
        ಸಂದೇಶ==other.ಸಂದೇಶ &&
        // parts.equals(o.parts)
        parts_ಉದ್ದಳತೆ==other.parts_ಉದ್ದಳತೆ &&
        (parts_ಉದ್ದಳತೆ==0 || partsList->equals(*other.partsList, parts_ಉದ್ದಳತೆ));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ {
    int32_t hash=(apos_ಬಹುಲಕ*37+ಸಂದೇಶ.hash_ಸಂಕೇತ())*37+parts_ಉದ್ದಳತೆ;
    ಗಾಗಿ(int32_t i=0; i<parts_ಉದ್ದಳತೆ; ++i) {
        hash=hash*37+parts[i].hash_ಸಂಕೇತ();
    }
    ಹಿಂತಿರುಗಿ hash;
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::validate_ವಾದ_Name(ಸ್ಥಿರಾಂಕ UnicodeString &name) {
    ಆದರೆ(!ನಮೂನೆ_Props::isIdentifier(name.get_ಬಫರ್(), name.length())) {
        ಹಿಂತಿರುಗಿ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ಹಿಂತಿರುಗಿ parse_Arg_ಸಂಖ್ಯೆ(name, 0, name.length());
}

UnicodeString
ಸಂದೇಶ_ನಮೂನೆ::autoQuoteApostropheDeep() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(!needsAutoQuoting) {
        ಹಿಂತಿರುಗಿ ಸಂದೇಶ;
    }
    UnicodeString modified(ಸಂದೇಶ);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ಎಣಿಕೆ_Parts();
    ಗಾಗಿ(int32_t i=count; i>0;) {
        ಸ್ಥಿರಾಂಕ Part &part=getPart(--i);
        ಆದರೆ(part.getType()==UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR) {
           modified.insert(part.ಸೂಚ್ಯಂಕ, static_cast<char16_t>(part.value));
        }
    }
    ಹಿಂತಿರುಗಿ modified;
}

ದ್ವಿಗುಣ
ಸಂದೇಶ_ನಮೂನೆ::get_Numeric_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ Part &part) ಸ್ಥಿರಾಂಕ {
    UMessage_ನಮೂನೆ_Part_Type type=part.type;
    ಆದರೆ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ಹಿಂತಿರುಗಿ part.value;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ಹಿಂತಿರುಗಿ numeric_ಮೌಲ್ಯಗಳು[part.value];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UMSGPAT_NO_NUMERIC_ಮೌಲ್ಯ;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ದ್ವಿಗುಣ
ಸಂದೇಶ_ನಮೂನೆ::get_Plural_ಸ್ಥಾನಾಂತರ(int32_t plural_ಆರಂಭ) ಸ್ಥಿರಾಂಕ {
    ಸ್ಥಿರಾಂಕ Part &part=getPart(plural_ಆರಂಭ);
    ಆದರೆ(Part::has_Numeric_ಮೌಲ್ಯ(part.type)) {
        ಹಿಂತಿರುಗಿ get_Numeric_ಮೌಲ್ಯ(part);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

ತಾರ್ಕಿಕ
ಸಂದೇಶ_ನಮೂನೆ::Part::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Part &other) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ(ಇದು==&other) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಹಿಂತಿರುಗಿ
        type==other.type &&
        ಸೂಚ್ಯಂಕ==other.ಸೂಚ್ಯಂಕ &&
        length==other.length &&
        value==other.value &&
        ಎಲ್ಲೆ_Part_ಸೂಚ್ಯಂಕ==other.ಎಲ್ಲೆ_Part_ಸೂಚ್ಯಂಕ;
}

// MessagePattern parser --------------------------------------------------- ***

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::preParse(ಸ್ಥಿರಾಂಕ UnicodeString &ನಮೂನೆ, UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(parseError!=ಶೂನ್ಯಸೂಚಕ) {
        parseError->ಸಾಲು=0;
        parseError->ಸ್ಥಾನಾಂತರ=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    ಸಂದೇಶ=ನಮೂನೆ;
    has_Arg_ಹೆಸರುಗಳು=hasArgNumbers=ಸುಳ್ಳು;
    needsAutoQuoting=ಸುಳ್ಳು;
    parts_ಉದ್ದಳತೆ=0;
    numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ=0;
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::postParse() {
    ಆದರೆ(partsList!=ಶೂನ್ಯಸೂಚಕ) {
        parts=partsList->a.getAlias();
    }
    ಆದರೆ(numeric_ಮೌಲ್ಯಗಳು_List!=ಶೂನ್ಯಸೂಚಕ) {
        numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
    }
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parse_ಸಂದೇಶ(int32_t ಸೂಚ್ಯಂಕ, int32_t ಸಂದೇಶ_ಆರಂಭ_ಉದ್ದಳತೆ,
                             int32_t nestingLevel, UMessage_ನಮೂನೆ_Arg_Type ಪೋಷಕ_Type,
                             UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(nestingLevel>Part::MAX_NESTED_ಮಟ್ಟಗಳು) {
        error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಸಂದೇಶ_ಆರಂಭ=parts_ಉದ್ದಳತೆ;
    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಆರಂಭ, ಸೂಚ್ಯಂಕ, ಸಂದೇಶ_ಆರಂಭ_ಉದ್ದಳತೆ, nestingLevel, error_ಸಂಕೇತ);
    ಸೂಚ್ಯಂಕ+=ಸಂದೇಶ_ಆರಂಭ_ಉದ್ದಳತೆ;
    ಗಾಗಿ(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ(ಸೂಚ್ಯಂಕ>=ಸಂದೇಶ.length()) {
            ನಿಲ್ಲಿಸು;
        }
        char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        ಆದರೆ(c==u_apos) {
            ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, ಸೂಚ್ಯಂಕ, 0,
                        u_apos, error_ಸಂಕೇತ);  // value=char to be inserted
                needsAutoQuoting=ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ);
                ಆದರೆ(c==u_apos) {
                    // double apostrophe, skip the second one
                    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ಸೂಚ್ಯಂಕ++, 1, 0, error_ಸಂಕೇತ);
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(
                    apos_ಬಹುಲಕ==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ಎಡ_Curly_Brace || c==u_ಬಲ_Curly_Brace ||
                    (ಪೋಷಕ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ಪೋಷಕ_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ಸೂಚ್ಯಂಕ-1, 1, 0, error_ಸಂಕೇತ);
                    // find the end of the quoted literal text
                    ಗಾಗಿ(;;) {
                        ಸೂಚ್ಯಂಕ=ಸಂದೇಶ.ಸೂಚ್ಯಂಕ_ದ(u_apos, ಸೂಚ್ಯಂಕ+1);
                        ಆದರೆ(ಸೂಚ್ಯಂಕ>=0) {
                            ಆದರೆ(/*(index+1)<msg.length() &&*/ ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++ಸೂಚ್ಯಂಕ, 1, 0, error_ಸಂಕೇತ);
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                // skip the quote-ending apostrophe
                                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ಸೂಚ್ಯಂಕ++, 1, 0, error_ಸಂಕೇತ);
                                ನಿಲ್ಲಿಸು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            // The quoted text reaches to the end of the of the message.
                            ಸೂಚ್ಯಂಕ=ಸಂದೇಶ.length();
                            // Add a Part for auto-quoting.
                            ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, ಸೂಚ್ಯಂಕ, 0,
                                    u_apos, error_ಸಂಕೇತ);  // value=char to be inserted
                            needsAutoQuoting=ನಿಜ;
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, ಸೂಚ್ಯಂಕ, 0,
                            u_apos, error_ಸಂಕೇತ);  // value=char to be inserted
                    needsAutoQuoting=ನಿಜ;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(ಪೋಷಕ_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_REPLACE_ಸಂಖ್ಯೆ, ಸೂಚ್ಯಂಕ-1, 1, 0, error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==u_ಎಡ_Curly_Brace) {
            ಸೂಚ್ಯಂಕ=parseArg(ಸೂಚ್ಯಂಕ-1, 1, nestingLevel, parseError, error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((nestingLevel>0 && c==u_ಬಲ_Curly_Brace) ||
                  (ಪೋಷಕ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t ಎಲ್ಲೆ_ಉದ್ದಳತೆ=(ಪೋಷಕ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_ಬಲ_Curly_Brace) ? 0 : 1;
            ಕೂಡಿಸು_ಎಲ್ಲೆ_Part(ಸಂದೇಶ_ಆರಂಭ, UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ-1, ಎಲ್ಲೆ_ಉದ್ದಳತೆ,
                         nestingLevel, error_ಸಂಕೇತ);
            ಆದರೆ(ಪೋಷಕ_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ-1;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // continue parsing after the '}'
                ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
            }
        }  // else: c is part of literal text
    }
    ಆದರೆ(nestingLevel>0 && !ಒಳ_ತುದಿ_Level_Choice_ಸಂದೇಶ(nestingLevel, ಪೋಷಕ_Type)) {
        ಗಣ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ಸಂಕೇತ=U_UNMATCHED_BRACES;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಕೂಡಿಸು_ಎಲ್ಲೆ_Part(ಸಂದೇಶ_ಆರಂಭ, UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, 0, nestingLevel, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parseArg(int32_t ಸೂಚ್ಯಂಕ, int32_t arg_ಆರಂಭ_ಉದ್ದಳತೆ, int32_t nestingLevel,
                         UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    int32_t arg_ಆರಂಭ=parts_ಉದ್ದಳತೆ;
    UMessage_ನಮೂನೆ_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_ಆರಂಭ, ಸೂಚ್ಯಂಕ, arg_ಆರಂಭ_ಉದ್ದಳತೆ, argType, error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t name_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ+arg_ಆರಂಭ_ಉದ್ದಳತೆ);
    ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
        ಗಣ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ಸಂಕೇತ=U_UNMATCHED_BRACES;
        ಹಿಂತಿರುಗಿ 0;
    }
    // parse argument name or number
    ಸೂಚ್ಯಂಕ=skipIdentifier(ಸೂಚ್ಯಂಕ);
    int32_t number=parse_Arg_ಸಂಖ್ಯೆ(name_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ);
    ಆದರೆ(number>=0) {
        int32_t length=ಸೂಚ್ಯಂಕ-name_ಸೂಚ್ಯಂಕ;
        ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ || number>Part::MAX_ಮೌಲ್ಯ) {
            ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Argument number too large.
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        hasArgNumbers=ನಿಜ;
        ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_ಸಂಖ್ಯೆ, name_ಸೂಚ್ಯಂಕ, length, number, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(number==UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ) {
        int32_t length=ಸೂಚ್ಯಂಕ-name_ಸೂಚ್ಯಂಕ;
        ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
            ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Argument name too long.
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        has_Arg_ಹೆಸರುಗಳು=ನಿಜ;
        ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_ಸೂಚ್ಯಂಕ, length, 0, error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {  // number<-1 (ARG_NAME_NOT_VALID)
        ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Bad argument syntax.
        error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
    ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
        ಗಣ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_ಸಂಕೇತ=U_UNMATCHED_BRACES;
        ಹಿಂತಿರುಗಿ 0;
    }
    char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ);
    ಆದರೆ(c==u_ಬಲ_Curly_Brace) {
        // all done
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c!=u_comma) {
        ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Bad argument syntax.
        error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ+1);
        ಆಗ(ಸೂಚ್ಯಂಕ<ಸಂದೇಶ.length() && isArgTypeChar(ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))) {
            ++ಸೂಚ್ಯಂಕ;
        }
        int32_t length=ಸೂಚ್ಯಂಕ-type_ಸೂಚ್ಯಂಕ;
        ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
        ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
            ಗಣ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_ಸಂಕೇತ=U_UNMATCHED_BRACES;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ(length==0 || ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))!=u_comma && c!=u_ಬಲ_Curly_Brace)) {
            ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Bad argument syntax.
            error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
            ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // Argument type name too long.
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ಆದರೆ(length==6) {
            // case-insensitive comparisons for complex-type names
            ಆದರೆ(isChoice(type_ಸೂಚ್ಯಂಕ)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isPlural(type_ಸೂಚ್ಯಂಕ)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(isSelect(type_ಸೂಚ್ಯಂಕ)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length==13) {
            ಆದರೆ(isSelect(type_ಸೂಚ್ಯಂಕ) && isOrdinal(type_ಸೂಚ್ಯಂಕ+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_ಆರಂಭ].value = static_cast<int16_t>(argType);
        ಆದರೆ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_ಸೂಚ್ಯಂಕ, length, 0, error_ಸಂಕೇತ);
        }
        // look for an argument style (pattern)
        ಆದರೆ(c==u_ಬಲ_Curly_Brace) {
            ಆದರೆ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                ಗಣ_Parse_Error(parseError, name_ಸೂಚ್ಯಂಕ);  // No style field for complex argument.
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
        } ಇಲ್ಲದಿದ್ದರೆ /* ',' */ {
            ++ಸೂಚ್ಯಂಕ;
            ಆದರೆ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                ಸೂಚ್ಯಂಕ=parseSimpleStyle(ಸೂಚ್ಯಂಕ, parseError, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                ಸೂಚ್ಯಂಕ=parseChoiceStyle(ಸೂಚ್ಯಂಕ, nestingLevel, parseError, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಸೂಚ್ಯಂಕ=parsePluralOrSelectStyle(argType, ಸೂಚ್ಯಂಕ, nestingLevel, parseError, error_ಸಂಕೇತ);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    ಕೂಡಿಸು_ಎಲ್ಲೆ_Part(arg_ಆರಂಭ, UMSGPAT_PART_TYPE_ARG_ಎಲ್ಲೆ, ಸೂಚ್ಯಂಕ, 1, argType, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ+1;
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parseSimpleStyle(int32_t ಸೂಚ್ಯಂಕ, UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಆರಂಭ=ಸೂಚ್ಯಂಕ;
    int32_t nestedBraces=0;
    ಆಗ(ಸೂಚ್ಯಂಕ<ಸಂದೇಶ.length()) {
        char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        ಆದರೆ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            ಸೂಚ್ಯಂಕ=ಸಂದೇಶ.ಸೂಚ್ಯಂಕ_ದ(u_apos, ಸೂಚ್ಯಂಕ);
            ಆದರೆ(ಸೂಚ್ಯಂಕ<0) {
                // Quoted literal argument style text reaches to the end of the message.
                ಗಣ_Parse_Error(parseError, ಆರಂಭ);
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            // skip the quote-ending apostrophe
            ++ಸೂಚ್ಯಂಕ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==u_ಎಡ_Curly_Brace) {
            ++nestedBraces;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==u_ಬಲ_Curly_Brace) {
            ಆದರೆ(nestedBraces>0) {
                --nestedBraces;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t length=--ಸೂಚ್ಯಂಕ-ಆರಂಭ;
                ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
                    ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Argument style text too long.
                    error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_STYLE, ಆರಂಭ, length, 0, error_ಸಂಕೇತ);
                ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
            }
        }  // c is part of literal text
    }
    ಗಣ_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_ಸಂಕೇತ=U_UNMATCHED_BRACES;
    ಹಿಂತಿರುಗಿ 0;
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parseChoiceStyle(int32_t ಸೂಚ್ಯಂಕ, int32_t nestingLevel,
                                 UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಆರಂಭ=ಸೂಚ್ಯಂಕ;
    ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
    ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length() || ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ)==u_ಬಲ_Curly_Brace) {
        ಗಣ_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }
    ಗಾಗಿ(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t ಸಂಖ್ಯೆ_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
        ಸೂಚ್ಯಂಕ=skipDouble(ಸೂಚ್ಯಂಕ);
        int32_t length=ಸೂಚ್ಯಂಕ-ಸಂಖ್ಯೆ_ಸೂಚ್ಯಂಕ;
        ಆದರೆ(length==0) {
            ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad choice pattern syntax.
            error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
            ಗಣ_Parse_Error(parseError, ಸಂಖ್ಯೆ_ಸೂಚ್ಯಂಕ);  // Choice number too long.
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        parseDouble(ಸಂಖ್ಯೆ_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ, ನಿಜ, parseError, error_ಸಂಕೇತ);  // adds ARG_INT or ARG_DOUBLE
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        // Parse the separator.
        ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
        ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
            ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad choice pattern syntax.
            error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ);
        ಆದರೆ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Expected choice separator (#<\u2264) instead of c.
            error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, ಸೂಚ್ಯಂಕ, 1, 0, error_ಸಂಕೇತ);
        // Parse the message fragment.
        ಸೂಚ್ಯಂಕ=parse_ಸಂದೇಶ(++ಸೂಚ್ಯಂಕ, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length()) {
            ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
        }
        ಆದರೆ(ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ)==u_ಬಲ_Curly_Brace) {
            ಆದರೆ(!ಒಳ_ಸಂದೇಶ_Format_ನಮೂನೆ(nestingLevel)) {
                ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad choice pattern syntax.
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
        }  // else the terminator is '|'
        ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ+1);
    }
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parsePluralOrSelectStyle(UMessage_ನಮೂನೆ_Arg_Type argType,
                                         int32_t ಸೂಚ್ಯಂಕ, int32_t nestingLevel,
                                         UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    int32_t ಆರಂಭ=ಸೂಚ್ಯಂಕ;
    UBool is_ಖಾಲಿಗಳು=ನಿಜ;
    UBool hasOther=ಸುಳ್ಳು;
    ಗಾಗಿ(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
        UBool eos=ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length();
        ಆದರೆ(eos || ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ)==u_ಬಲ_Curly_Brace) {
            ಆದರೆ(eos==ಒಳ_ಸಂದೇಶ_Format_ನಮೂನೆ(nestingLevel)) {
                ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad plural/select pattern syntax.
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಆದರೆ(!hasOther) {
                ಗಣ_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_ಸಂಕೇತ=U_DEFAULT_KEYWORD_MISSING;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
        }
        int32_t selector_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
        ಆದರೆ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && ಸಂದೇಶ.charAt(selector_ಸೂಚ್ಯಂಕ)==u_equal) {
            // explicit-value plural selector: =double
            ಸೂಚ್ಯಂಕ=skipDouble(ಸೂಚ್ಯಂಕ+1);
            int32_t length=ಸೂಚ್ಯಂಕ-selector_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(length==1) {
                ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad plural/select pattern syntax.
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
                ಗಣ_Parse_Error(parseError, selector_ಸೂಚ್ಯಂಕ);  // Argument selector too long.
                error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_ಸೂಚ್ಯಂಕ, length, 0, error_ಸಂಕೇತ);
            parseDouble(selector_ಸೂಚ್ಯಂಕ+1, ಸೂಚ್ಯಂಕ, ಸುಳ್ಳು,
                        parseError, error_ಸಂಕೇತ);  // adds ARG_INT or ARG_DOUBLE
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸೂಚ್ಯಂಕ=skipIdentifier(ಸೂಚ್ಯಂಕ);
            int32_t length=ಸೂಚ್ಯಂಕ-selector_ಸೂಚ್ಯಂಕ;
            ಆದರೆ(length==0) {
                ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Bad plural/select pattern syntax.
                error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ಆದರೆ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && ಸೂಚ್ಯಂಕ<ಸಂದೇಶ.length() &&
                0==ಸಂದೇಶ.compare(selector_ಸೂಚ್ಯಂಕ, 7, k_ಸ್ಥಾನಾಂತರ_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                ಆದರೆ(!is_ಖಾಲಿಗಳು) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    ಗಣ_Parse_Error(parseError, ಆರಂಭ);
                    error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                // allow whitespace between offset: and its value
                int32_t ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ+1);  // The ':' is at index.
                ಸೂಚ್ಯಂಕ=skipDouble(ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ);
                ಆದರೆ(ಸೂಚ್ಯಂಕ==ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ) {
                    ಗಣ_Parse_Error(parseError, ಆರಂಭ);  // Missing value for plural 'offset:'.
                    error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ((ಸೂಚ್ಯಂಕ-ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ)>Part::MAX_ಉದ್ದಳತೆ) {
                    ಗಣ_Parse_Error(parseError, ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ);  // Plural offset value too long.
                    error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                parseDouble(ಮೌಲ್ಯ_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ, ಸುಳ್ಳು,
                            parseError, error_ಸಂಕೇತ);  // adds ARG_INT or ARG_DOUBLE
                ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
                    ಹಿಂತಿರುಗಿ 0;
                }
                is_ಖಾಲಿಗಳು=ಸುಳ್ಳು;
                ಮುಂದುವರಿ;  // no message fragment after the offset
            } ಇಲ್ಲದಿದ್ದರೆ {
                // normal selector word
                ಆದರೆ(length>Part::MAX_ಉದ್ದಳತೆ) {
                    ಗಣ_Parse_Error(parseError, selector_ಸೂಚ್ಯಂಕ);  // Argument selector too long.
                    error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                    ಹಿಂತಿರುಗಿ 0;
                }
                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_ಸೂಚ್ಯಂಕ, length, 0, error_ಸಂಕೇತ);
                ಆದರೆ(0==ಸಂದೇಶ.compare(selector_ಸೂಚ್ಯಂಕ, length, kOther, 0, 5)) {
                    hasOther=ನಿಜ;
                }
            }
        }
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }

        // parse the message fragment following the selector
        ಸೂಚ್ಯಂಕ=skipWhiteSpace(ಸೂಚ್ಯಂಕ);
        ಆದರೆ(ಸೂಚ್ಯಂಕ==ಸಂದೇಶ.length() || ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ)!=u_ಎಡ_Curly_Brace) {
            ಗಣ_Parse_Error(parseError, selector_ಸೂಚ್ಯಂಕ);  // No message fragment after plural/select selector.
            error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
        ಸೂಚ್ಯಂಕ=parse_ಸಂದೇಶ(ಸೂಚ್ಯಂಕ, 1, nestingLevel+1, argType, parseError, error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        is_ಖಾಲಿಗಳು=ಸುಳ್ಳು;
    }
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::parse_Arg_ಸಂಖ್ಯೆ(ಸ್ಥಿರಾಂಕ UnicodeString &s, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ಆದರೆ(ಆರಂಭ>=ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_ಸಂಖ್ಯೆ;
    char16_t c=s.charAt(ಆರಂಭ++);
    ಆದರೆ(c==0x30) {
        ಆದರೆ(ಆರಂಭ==ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            number=0;
            bad_ಸಂಖ್ಯೆ=ನಿಜ;  // leading zero
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_ಸಂಖ್ಯೆ=ಸುಳ್ಳು;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ;
    }
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ) {
        c=s.charAt(ಆರಂಭ++);
        ಆದರೆ(0x30<=c && c<=0x39) {
            ಆದರೆ(number>=INT32_MAX/10) {
                bad_ಸಂಖ್ಯೆ=ನಿಜ;  //ಅತಿಪ್ರವಾಹ
            } ಇಲ್ಲದಿದ್ದರೆ {
                number=number*10+(c-0x30);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ;
        }
    }
    // There are only ASCII digits.
    ಆದರೆ(bad_ಸಂಖ್ಯೆ) {
        ಹಿಂತಿರುಗಿ UMSGPAT_ARG_NAME_NOT_VALID;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ number;
    }
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::parseDouble(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, UBool allowInfinity,
                            UParseError *parseError, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    U_ASSERT(ಆರಂಭ<ಎಲ್ಲೆ);
    // fake loop for easy exit and single throw statement
    ಗಾಗಿ(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t ಸೂಚ್ಯಂಕ=ಆರಂಭ;
        char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        ಆದರೆ(c==u_minus) {
            isNegative=1;
            ಆದರೆ(ಸೂಚ್ಯಂಕ==ಎಲ್ಲೆ) {
                ನಿಲ್ಲಿಸು;  // no number
            }
            c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==u_plus) {
            ಆದರೆ(ಸೂಚ್ಯಂಕ==ಎಲ್ಲೆ) {
                ನಿಲ್ಲಿಸು;  // no number
            }
            c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        }
        ಆದರೆ(c==0x221e) {  // infinity
            ಆದರೆ(allowInfinity && ಸೂಚ್ಯಂಕ==ಎಲ್ಲೆ) {
                ದ್ವಿಗುಣ infinity=uprv_getInfinity();
                ಕೂಡಿಸು_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    ಆರಂಭ, ಎಲ್ಲೆ-ಆರಂಭ, error_ಸಂಕೇತ);
                ಹಿಂತಿರುಗಿ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ನಿಲ್ಲಿಸು;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ಆಗ('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ಆದರೆ(value>(Part::MAX_ಮೌಲ್ಯ+isNegative)) {
                ನಿಲ್ಲಿಸು;  // not a small-enough integer
            }
            ಆದರೆ(ಸೂಚ್ಯಂಕ==ಎಲ್ಲೆ) {
                ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_INT, ಆರಂಭ, ಎಲ್ಲೆ-ಆರಂಭ,
                        isNegative!=0 ? -value : value, error_ಸಂಕೇತ);
                ಹಿಂತಿರುಗಿ;
            }
            c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        ಅಕ್ಷರ ಸಂಖ್ಯೆ_Chars[128];
        int32_t capacity = static_cast<int32_t>(ಗಾತ್ರ(ಸಂಖ್ಯೆ_Chars));
        int32_t length=ಎಲ್ಲೆ-ಆರಂಭ;
        ಆದರೆ(length>=capacity) {
            ನಿಲ್ಲಿಸು;  // number too long
        }
        ಸಂದೇಶ.extract(ಆರಂಭ, length, ಸಂಖ್ಯೆ_Chars, capacity, US_INV);
        ಆದರೆ (static_cast<int32_t>(uprv_strlen(ಸಂಖ್ಯೆ_Chars)) < length) {
            ನಿಲ್ಲಿಸು;  // contains non-invariant character that was turned into NUL
        }
        ಅಕ್ಷರ *ಕೊನೆ;
        ದ್ವಿಗುಣ numeric_ಮೌಲ್ಯ=uprv_strtod(ಸಂಖ್ಯೆ_Chars, &ಕೊನೆ);
        ಆದರೆ(ಕೊನೆ!=(ಸಂಖ್ಯೆ_Chars+length)) {
            ನಿಲ್ಲಿಸು;  // parsing error
        }
        ಕೂಡಿಸು_Arg_Double_Part(numeric_ಮೌಲ್ಯ, ಆರಂಭ, length, error_ಸಂಕೇತ);
        ಹಿಂತಿರುಗಿ;
    }
    ಗಣ_Parse_Error(parseError, ಆರಂಭ /*, limit*/);  // Bad syntax for numeric value.
    error_ಸಂಕೇತ=U_ನಮೂನೆ_SYNTAX_ERROR;
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::skipWhiteSpace(int32_t ಸೂಚ್ಯಂಕ) {
    ಸ್ಥಿರಾಂಕ char16_t *s=ಸಂದೇಶ.get_ಬಫರ್();
    int32_t ಸಂದೇಶ_ಉದ್ದಳತೆ=ಸಂದೇಶ.length();
    ಸ್ಥಿರಾಂಕ char16_t *t=ನಮೂನೆ_Props::skipWhiteSpace(s+ಸೂಚ್ಯಂಕ, ಸಂದೇಶ_ಉದ್ದಳತೆ-ಸೂಚ್ಯಂಕ);
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(t - s);
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::skipIdentifier(int32_t ಸೂಚ್ಯಂಕ) {
    ಸ್ಥಿರಾಂಕ char16_t *s=ಸಂದೇಶ.get_ಬಫರ್();
    int32_t ಸಂದೇಶ_ಉದ್ದಳತೆ=ಸಂದೇಶ.length();
    ಸ್ಥಿರಾಂಕ char16_t *t=ನಮೂನೆ_Props::skipIdentifier(s+ಸೂಚ್ಯಂಕ, ಸಂದೇಶ_ಉದ್ದಳತೆ-ಸೂಚ್ಯಂಕ);
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(t - s);
}

int32_t
ಸಂದೇಶ_ನಮೂನೆ::skipDouble(int32_t ಸೂಚ್ಯಂಕ) {
    int32_t ಸಂದೇಶ_ಉದ್ದಳತೆ=ಸಂದೇಶ.length();
    ಆಗ(ಸೂಚ್ಯಂಕ<ಸಂದೇಶ_ಉದ್ದಳತೆ) {
        char16_t c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ಆದರೆ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ನಿಲ್ಲಿಸು;
        }
        ++ಸೂಚ್ಯಂಕ;
    }
    ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::isArgTypeChar(UChar32 c) {
    ಹಿಂತಿರುಗಿ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::isChoice(int32_t ಸೂಚ್ಯಂಕ) {
    char16_t c;
    ಹಿಂತಿರುಗಿ
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_h || c==u_H) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_o || c==u_O) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_i || c==u_I) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))==u_e || c==u_E);
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::isPlural(int32_t ಸೂಚ್ಯಂಕ) {
    char16_t c;
    ಹಿಂತಿರುಗಿ
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_p || c==u_P) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_l || c==u_L) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_u || c==u_U) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_r || c==u_R) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_a || c==u_A) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))==u_l || c==u_L);
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::isSelect(int32_t ಸೂಚ್ಯಂಕ) {
    char16_t c;
    ಹಿಂತಿರುಗಿ
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_s || c==u_S) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_e || c==u_E) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_l || c==u_L) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_e || c==u_E) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))==u_t || c==u_T);
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::isOrdinal(int32_t ಸೂಚ್ಯಂಕ) {
    char16_t c;
    ಹಿಂತಿರುಗಿ
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_o || c==u_O) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_r || c==u_R) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_d || c==u_D) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_i || c==u_I) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_n || c==u_N) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ++))==u_a || c==u_A) &&
        ((c=ಸಂದೇಶ.charAt(ಸೂಚ್ಯಂಕ))==u_l || c==u_L);
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::ಒಳ_ಸಂದೇಶ_Format_ನಮೂನೆ(int32_t nestingLevel) {
    ಹಿಂತಿರುಗಿ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಆರಂಭ;
}

UBool
ಸಂದೇಶ_ನಮೂನೆ::ಒಳ_ತುದಿ_Level_Choice_ಸಂದೇಶ(int32_t nestingLevel, UMessage_ನಮೂನೆ_Arg_Type ಪೋಷಕ_Type) {
    ಹಿಂತಿರುಗಿ
        nestingLevel==1 &&
        ಪೋಷಕ_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಆರಂಭ;
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::ಕೂಡಿಸು_Part(UMessage_ನಮೂನೆ_Part_Type type, int32_t ಸೂಚ್ಯಂಕ, int32_t length,
                        int32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(partsList->ensure_ಸಾಮರ್ಥ್ಯ_For_One_ಇನ್ನಷ್ಟು(parts_ಉದ್ದಳತೆ, error_ಸಂಕೇತ)) {
        Part &part=partsList->a[parts_ಉದ್ದಳತೆ++];
        part.type=type;
        part.ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.ಎಲ್ಲೆ_Part_ಸೂಚ್ಯಂಕ=0;
    }
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::ಕೂಡಿಸು_ಎಲ್ಲೆ_Part(int32_t ಆರಂಭ,
                             UMessage_ನಮೂನೆ_Part_Type type, int32_t ಸೂಚ್ಯಂಕ, int32_t length,
                             int32_t value, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    partsList->a[ಆರಂಭ].ಎಲ್ಲೆ_Part_ಸೂಚ್ಯಂಕ=parts_ಉದ್ದಳತೆ;
    ಕೂಡಿಸು_Part(type, ಸೂಚ್ಯಂಕ, length, value, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::ಕೂಡಿಸು_Arg_Double_Part(ದ್ವಿಗುಣ numeric_ಮೌಲ್ಯ, int32_t ಆರಂಭ, int32_t length,
                                 UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    int32_t numeric_ಸೂಚ್ಯಂಕ=numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ;
    ಆದರೆ(numeric_ಮೌಲ್ಯಗಳು_List==ಶೂನ್ಯಸೂಚಕ) {
        numeric_ಮೌಲ್ಯಗಳು_List=ಹೊಸ ಸಂದೇಶ_ನಮೂನೆ_Double_List();
        ಆದರೆ(numeric_ಮೌಲ್ಯಗಳು_List==ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!numeric_ಮೌಲ್ಯಗಳು_List->ensure_ಸಾಮರ್ಥ್ಯ_For_One_ಇನ್ನಷ್ಟು(numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ, error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(numeric_ಸೂಚ್ಯಂಕ>Part::MAX_ಮೌಲ್ಯ) {
            error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
    }
    numeric_ಮೌಲ್ಯಗಳು_List->a[numeric_ಮೌಲ್ಯಗಳು_ಉದ್ದಳತೆ++]=numeric_ಮೌಲ್ಯ;
    ಕೂಡಿಸು_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, ಆರಂಭ, length, numeric_ಸೂಚ್ಯಂಕ, error_ಸಂಕೇತ);
}

ಶೂನ್ಯ
ಸಂದೇಶ_ನಮೂನೆ::ಗಣ_Parse_Error(UParseError *parseError, int32_t ಸೂಚ್ಯಂಕ) {
    ಆದರೆ(parseError==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ;
    }
    parseError->ಸ್ಥಾನಾಂತರ=ಸೂಚ್ಯಂಕ;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=ಸೂಚ್ಯಂಕ;
    ಆದರೆ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ಆದರೆ(length>0 && U16_IS_TRAIL(ಸಂದೇಶ[ಸೂಚ್ಯಂಕ-length])) {
            --length;
        }
    }
    ಸಂದೇಶ.extract(ಸೂಚ್ಯಂಕ-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=ಸಂದೇಶ.length()-ಸೂಚ್ಯಂಕ;
    ಆದರೆ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ಆದರೆ(length>0 && U16_IS_LEAD(ಸಂದೇಶ[ಸೂಚ್ಯಂಕ+length-1])) {
            --length;
        }
    }
    ಸಂದೇಶ.extract(ಸೂಚ್ಯಂಕ, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

ಶೂನ್ಯ
ಸಂದೇಶ_Impl::appendReducedApostrophes(ಸ್ಥಿರಾಂಕ UnicodeString &s, int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    ಗಾಗಿ(;;) {
        int32_t i=s.ಸೂಚ್ಯಂಕ_ದ(u_apos, ಆರಂಭ);
        ಆದರೆ(i<0 || i>=ಎಲ್ಲೆ) {
            sb.append(s, ಆರಂಭ, ಎಲ್ಲೆ-ಆರಂಭ);
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++ಆರಂಭ;
            doubleApos=-1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Append text between apostrophes and skip this one.
            sb.append(s, ಆರಂಭ, i-ಆರಂಭ);
            doubleApos=ಆರಂಭ=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
ಸಂದೇಶ_Impl::append_Sub_ಸಂದೇಶ_Without_Skip_Syntax(ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ& ಸಂದೇಶ_ನಮೂನೆ_2,
                                               int32_t ಸಂದೇಶ_ಆರಂಭ,
                                               UnicodeString& ಫಲಿತಾಂಶ U_LIFETIME_BOUND) {
    ಸ್ಥಿರಾಂಕ UnicodeString &ಸಂದೇಶ_String=ಸಂದೇಶ_ನಮೂನೆ_2.get_ನಮೂನೆ_String();
    int32_t prev_ಸೂಚ್ಯಂಕ=ಸಂದೇಶ_ನಮೂನೆ_2.getPart(ಸಂದೇಶ_ಆರಂಭ).get_ಎಲ್ಲೆ();
    ಗಾಗಿ(int32_t i=ಸಂದೇಶ_ಆರಂಭ;;) {
        ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_ನಮೂನೆ::Part &part=ಸಂದೇಶ_ನಮೂನೆ_2.getPart(++i);
        UMessage_ನಮೂನೆ_Part_Type type=part.getType();
        int32_t ಸೂಚ್ಯಂಕ=part.get_ಸೂಚ್ಯಂಕ();
        ಆದರೆ(type==UMSGPAT_PART_TYPE_ಸಂದೇಶ_ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ.append(ಸಂದೇಶ_String, prev_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ-prev_ಸೂಚ್ಯಂಕ);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            ಫಲಿತಾಂಶ.append(ಸಂದೇಶ_String, prev_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ-prev_ಸೂಚ್ಯಂಕ);
            prev_ಸೂಚ್ಯಂಕ=part.get_ಎಲ್ಲೆ();
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type==UMSGPAT_PART_TYPE_ARG_ಆರಂಭ) {
            ಫಲಿತಾಂಶ.append(ಸಂದೇಶ_String, prev_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ-prev_ಸೂಚ್ಯಂಕ);
            prev_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
            i=ಸಂದೇಶ_ನಮೂನೆ_2.get_ಎಲ್ಲೆ_Part_ಸೂಚ್ಯಂಕ(i);
            ಸೂಚ್ಯಂಕ=ಸಂದೇಶ_ನಮೂನೆ_2.getPart(i).get_ಎಲ್ಲೆ();
            appendReducedApostrophes(ಸಂದೇಶ_String, prev_ಸೂಚ್ಯಂಕ, ಸೂಚ್ಯಂಕ, ಫಲಿತಾಂಶ);
            prev_ಸೂಚ್ಯಂಕ=ಸೂಚ್ಯಂಕ;
        }
    }
}

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ  // !UCONFIG_NO_FORMATTING
