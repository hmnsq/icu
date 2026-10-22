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

#ಯದಿ !UCONFIG_NO_FORMATTING

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

ಸ್ಥಿರ ಧ್ರುವ char16_t u_pound=0x23;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_apos=0x27;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_plus=0x2B;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_comma=0x2C;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_minus=0x2D;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_dot=0x2E;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_colon=0x3A;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_lessThan=0x3C;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_equal=0x3D;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_A=0x41;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_C=0x43;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_D=0x44;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_E=0x45;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_H=0x48;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_I=0x49;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_L=0x4C;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_N=0x4E;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_O=0x4F;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_P=0x50;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_R=0x52;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_S=0x53;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_T=0x54;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_U=0x55;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_Z=0x5A;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_a=0x61;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_c=0x63;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_d=0x64;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_e=0x65;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_f=0x66;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_h=0x68;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_i=0x69;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_l=0x6C;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_n=0x6E;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_o=0x6F;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_p=0x70;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_r=0x72;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_s=0x73;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_t=0x74;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_u=0x75;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_z=0x7A;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_ಎಡ_Curly_Brace=0x7B;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_pipe=0x7C;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_ಬಲ_Curly_Brace=0x7D;
ಸ್ಥಿರ ಧ್ರುವ char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ಸ್ಥಿರ ಧ್ರುವ char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ಸ್ಥಿರ ಧ್ರುವ char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t ರಾಶಿ_Capacity>
ವರ್ಗ MessagePatternList : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
    MessagePatternList() {}
    ಶೂನ್ಯ copyFrom(ಧ್ರುವ MessagePatternList<T, ರಾಶಿ_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ಧ್ರುವ MessagePatternList<T, ರಾಶಿ_Capacity> &other, int32_t length) ಧ್ರುವ {
        ಪ್ರತಿ(int32_t i=0; i<length; ++i) {
            ಯದಿ(a[i]!=other.a[i]) { ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ; }
        }
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    Maybe_ರಾಶಿ_Array<T, ರಾಶಿ_Capacity> a;
};

template<typename T, int32_t ರಾಶಿ_Capacity>
ಶೂನ್ಯ
MessagePatternList<T, ರಾಶಿ_Capacity>::copyFrom(
        ಧ್ರುವ MessagePatternList<T, ರಾಶಿ_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    ಯದಿ(U_SUCCESS(errorCode) && length>0) {
        ಯದಿ(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*ಪರಿಮಾಣ(T));
    }
}

template<typename T, int32_t ರಾಶಿ_Capacity>
UBool
MessagePatternList<T, ರಾಶಿ_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಯದಿ(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

// MessagePatternList specializations -------------------------------------- ***

ವರ್ಗ MessagePatternDoubleList : ಸಾರ್ವಜನಿಕ MessagePatternList<ದ್ವಿಗುಣ, 8> {
};

ವರ್ಗ MessagePatternPartsList : ಸಾರ್ವಜನಿಕ MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ಮೌಲ್ಯಗಳು_List(nullptr), numeric_ಮೌಲ್ಯಗಳು(nullptr), numeric_ಮೌಲ್ಯಗಳು_Length(0),
          hasArgNames(ಅಸತ್ಯ), hasArgNumbers(ಅಸತ್ಯ), needsAutoQuoting(ಅಸತ್ಯ) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ಮೌಲ್ಯಗಳು_List(nullptr), numeric_ಮೌಲ್ಯಗಳು(nullptr), numeric_ಮೌಲ್ಯಗಳು_Length(0),
          hasArgNames(ಅಸತ್ಯ), hasArgNumbers(ಅಸತ್ಯ), needsAutoQuoting(ಅಸತ್ಯ) {
    init(errorCode);
}

MessagePattern::MessagePattern(ಧ್ರುವ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ಮೌಲ್ಯಗಳು_List(nullptr), numeric_ಮೌಲ್ಯಗಳು(nullptr), numeric_ಮೌಲ್ಯಗಳು_Length(0),
          hasArgNames(ಅಸತ್ಯ), hasArgNumbers(ಅಸತ್ಯ), needsAutoQuoting(ಅಸತ್ಯ) {
    ಯದಿ(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    partsList=ನೂತನ MessagePatternPartsList();
    ಯದಿ(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    parts=partsList->a.getAlias();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

MessagePattern::MessagePattern(ಧ್ರುವ MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), ಸಂದೇಶ(other.ಸಂದೇಶ),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ಮೌಲ್ಯಗಳು_List(nullptr), numeric_ಮೌಲ್ಯಗಳು(nullptr), numeric_ಮೌಲ್ಯಗಳು_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಯದಿ(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ಧ್ರುವ MessagePattern& other) U_LIFETIME_BOUND {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
    }
    aposMode=other.aposMode;
    ಸಂದೇಶ=other.ಸಂದೇಶ;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    ಯದಿ(!copyStorage(other, errorCode)) {
        clear();
    }
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

UBool
MessagePattern::copyStorage(ಧ್ರುವ MessagePattern &other, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    parts=nullptr;
    partsLength=0;
    numeric_ಮೌಲ್ಯಗಳು=nullptr;
    numeric_ಮೌಲ್ಯಗಳು_Length=0;
    ಯದಿ(partsList==nullptr) {
        partsList=ನೂತನ MessagePatternPartsList();
        ಯದಿ(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        parts=partsList->a.getAlias();
    }
    ಯದಿ(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    ಯದಿ(other.numeric_ಮೌಲ್ಯಗಳು_Length>0) {
        ಯದಿ(numeric_ಮೌಲ್ಯಗಳು_List==nullptr) {
            numeric_ಮೌಲ್ಯಗಳು_List=ನೂತನ MessagePatternDoubleList();
            ಯದಿ(numeric_ಮೌಲ್ಯಗಳು_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
        }
        numeric_ಮೌಲ್ಯಗಳು_List->copyFrom(
            *other.numeric_ಮೌಲ್ಯಗಳು_List, other.numeric_ಮೌಲ್ಯಗಳು_Length, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
        numeric_ಮೌಲ್ಯಗಳು_Length=other.numeric_ಮೌಲ್ಯಗಳು_Length;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_ಮೌಲ್ಯಗಳು_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ಧ್ರುವ UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ಧ್ರುವ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

MessagePattern&
MessagePattern::parsePluralStyle(ಧ್ರುವ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

MessagePattern&
MessagePattern::parseSelectStyle(ಧ್ರುವ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

ಶೂನ್ಯ
MessagePattern::clear() {
    // Mostly the same as preParse().
    ಸಂದೇಶ.remove();
    hasArgNames=hasArgNumbers=ಅಸತ್ಯ;
    needsAutoQuoting=ಅಸತ್ಯ;
    partsLength=0;
    numeric_ಮೌಲ್ಯಗಳು_Length=0;
}

ತಾರ್ಕಿಕ
MessagePattern::operator==(ಧ್ರುವ MessagePattern &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ
        aposMode==other.aposMode &&
        ಸಂದೇಶ==other.ಸಂದೇಶ &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ಧ್ರುವ {
    int32_t hash=(aposMode*37+ಸಂದೇಶ.hashCode())*37+partsLength;
    ಪ್ರತಿ(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    ಪ್ರತ್ಯಾಗಮ hash;
}

int32_t
MessagePattern::validateArgumentName(ಧ್ರುವ UnicodeString &name) {
    ಯದಿ(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        ಪ್ರತ್ಯಾಗಮ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ಪ್ರತ್ಯಾಗಮ parse_Arg_ಸಂಖ್ಯೆ(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ಧ್ರುವ {
    ಯದಿ(!needsAutoQuoting) {
        ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ;
    }
    UnicodeString modified(ಸಂದೇಶ);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ಎಣಿಕೆ_Parts();
    ಪ್ರತಿ(int32_t i=count; i>0;) {
        ಧ್ರುವ Part &part=getPart(--i);
        ಯದಿ(part.getType()==UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    ಪ್ರತ್ಯಾಗಮ modified;
}

ದ್ವಿಗುಣ
MessagePattern::get_Numeric_ಮೌಲ್ಯ(ಧ್ರುವ Part &part) ಧ್ರುವ {
    UMessagePatternPartType type=part.type;
    ಯದಿ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ಪ್ರತ್ಯಾಗಮ part.value;
    } ಅನ್ಯಥಾ ಯದಿ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ಪ್ರತ್ಯಾಗಮ numeric_ಮೌಲ್ಯಗಳು[part.value];
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ UMSGPAT_NO_NUMERIC_ಮೌಲ್ಯ;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ದ್ವಿಗುಣ
MessagePattern::getPluralOffset(int32_t pluralStart) ಧ್ರುವ {
    ಧ್ರುವ Part &part=getPart(pluralStart);
    ಯದಿ(Part::has_Numeric_ಮೌಲ್ಯ(part.type)) {
        ಪ್ರತ್ಯಾಗಮ get_Numeric_ಮೌಲ್ಯ(part);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

ತಾರ್ಕಿಕ
MessagePattern::Part::operator==(ಧ್ರುವ Part &other) ಧ್ರುವ {
    ಯದಿ(ಏತತ್==&other) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

ಶೂನ್ಯ
MessagePattern::preParse(ಧ್ರುವ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(parseError!=nullptr) {
        parseError->ಸಾಲು=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    ಸಂದೇಶ=pattern;
    hasArgNames=hasArgNumbers=ಅಸತ್ಯ;
    needsAutoQuoting=ಅಸತ್ಯ;
    partsLength=0;
    numeric_ಮೌಲ್ಯಗಳು_Length=0;
}

ಶೂನ್ಯ
MessagePattern::postParse() {
    ಯದಿ(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    ಯದಿ(numeric_ಮೌಲ್ಯಗಳು_List!=nullptr) {
        numeric_ಮೌಲ್ಯಗಳು=numeric_ಮೌಲ್ಯಗಳು_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t ಸಂದೇಶ_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t ಸಂದೇಶ_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_ಸಂದೇಶ_START, index, ಸಂದೇಶ_Start_Length, nestingLevel, errorCode);
    index+=ಸಂದೇಶ_Start_Length;
    ಪ್ರತಿ(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ(index>=ಸಂದೇಶ.length()) {
            ಭಙ್ಗ;
        }
        char16_t c=ಸಂದೇಶ.charAt(index++);
        ಯದಿ(c==u_apos) {
            ಯದಿ(index==ಸಂದೇಶ.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=ಸತ್ಯ;
            } ಅನ್ಯಥಾ {
                c=ಸಂದೇಶ.charAt(index);
                ಯದಿ(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } ಅನ್ಯಥಾ ಯದಿ(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ಎಡ_Curly_Brace || c==u_ಬಲ_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    ಪ್ರತಿ(;;) {
                        index=ಸಂದೇಶ.indexOf(u_apos, index+1);
                        ಯದಿ(index>=0) {
                            ಯದಿ(/*(index+1)<msg.length() &&*/ ಸಂದೇಶ.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } ಅನ್ಯಥಾ {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                ಭಙ್ಗ;
                            }
                        } ಅನ್ಯಥಾ {
                            // The quoted text reaches to the end of the of the message.
                            index=ಸಂದೇಶ.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=ಸತ್ಯ;
                            ಭಙ್ಗ;
                        }
                    }
                } ಅನ್ಯಥಾ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_ಸೇರಿಸು_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=ಸತ್ಯ;
                }
            }
        } ಅನ್ಯಥಾ ಯದಿ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_ಸಂಖ್ಯೆ, index-1, 1, 0, errorCode);
        } ಅನ್ಯಥಾ ಯದಿ(c==u_ಎಡ_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } ಅನ್ಯಥಾ ಯದಿ((nestingLevel>0 && c==u_ಬಲ_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_ಬಲ_Curly_Brace) ? 0 : 1;
            addLimitPart(ಸಂದೇಶ_Start, UMSGPAT_PART_TYPE_ಸಂದೇಶ_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            ಯದಿ(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ಪ್ರತ್ಯಾಗಮ index-1;
            } ಅನ್ಯಥಾ {
                // continue parsing after the '}'
                ಪ್ರತ್ಯಾಗಮ index;
            }
        }  // else: c is part of literal text
    }
    ಯದಿ(nestingLevel>0 && !in_ತುದಿ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    addLimitPart(ಸಂದೇಶ_Start, UMSGPAT_PART_TYPE_ಸಂದೇಶ_LIMIT, index, 0, nestingLevel, errorCode);
    ಪ್ರತ್ಯಾಗಮ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    ಯದಿ(index==ಸಂದೇಶ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_ಸಂಖ್ಯೆ(nameIndex, index);
    ಯದಿ(number>=0) {
        int32_t length=index-nameIndex;
        ಯದಿ(length>Part::MAX_LENGTH || number>Part::MAX_ಮೌಲ್ಯ) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        hasArgNumbers=ಸತ್ಯ;
        addPart(UMSGPAT_PART_TYPE_ARG_ಸಂಖ್ಯೆ, nameIndex, length, number, errorCode);
    } ಅನ್ಯಥಾ ಯದಿ(number==UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ) {
        int32_t length=index-nameIndex;
        ಯದಿ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        hasArgNames=ಸತ್ಯ;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } ಅನ್ಯಥಾ {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    index=skipWhiteSpace(index);
    ಯದಿ(index==ಸಂದೇಶ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    char16_t c=ಸಂದೇಶ.charAt(index);
    ಯದಿ(c==u_ಬಲ_Curly_Brace) {
        // all done
    } ಅನ್ಯಥಾ ಯದಿ(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        ಯಾವತ್(index<ಸಂದೇಶ.length() && isArgTypeChar(ಸಂದೇಶ.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        ಯದಿ(index==ಸಂದೇಶ.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ(length==0 || ((c=ಸಂದೇಶ.charAt(index))!=u_comma && c!=u_ಬಲ_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ಯದಿ(length==6) {
            // case-insensitive comparisons for complex-type names
            ಯದಿ(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ಅನ್ಯಥಾ ಯದಿ(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ಅನ್ಯಥಾ ಯದಿ(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ಅನ್ಯಥಾ ಯದಿ(length==13) {
            ಯದಿ(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        ಯದಿ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        ಯದಿ(c==u_ಬಲ_Curly_Brace) {
            ಯದಿ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        } ಅನ್ಯಥಾ /* ',' */ {
            ++index;
            ಯದಿ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } ಅನ್ಯಥಾ ಯದಿ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } ಅನ್ಯಥಾ {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    ಪ್ರತ್ಯಾಗಮ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    ಯಾವತ್(index<ಸಂದೇಶ.length()) {
        char16_t c=ಸಂದೇಶ.charAt(index++);
        ಯದಿ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=ಸಂದೇಶ.indexOf(u_apos, index);
            ಯದಿ(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } ಅನ್ಯಥಾ ಯದಿ(c==u_ಎಡ_Curly_Brace) {
            ++nestedBraces;
        } ಅನ್ಯಥಾ ಯದಿ(c==u_ಬಲ_Curly_Brace) {
            ಯದಿ(nestedBraces>0) {
                --nestedBraces;
            } ಅನ್ಯಥಾ {
                int32_t length=--index-start;
                ಯದಿ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                ಪ್ರತ್ಯಾಗಮ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    ಪ್ರತ್ಯಾಗಮ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    ಯದಿ(index==ಸಂದೇಶ.length() || ಸಂದೇಶ.charAt(index)==u_ಬಲ_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತಿ(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t ಸಂಖ್ಯೆ_Index=index;
        index=skipDouble(index);
        int32_t length=index-ಸಂಖ್ಯೆ_Index;
        ಯದಿ(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ(length>Part::MAX_LENGTH) {
            setParseError(parseError, ಸಂಖ್ಯೆ_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        parseDouble(ಸಂಖ್ಯೆ_Index, index, ಸತ್ಯ, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        ಯದಿ(index==ಸಂದೇಶ.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        char16_t c=ಸಂದೇಶ.charAt(index);
        ಯದಿ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ಯದಿ(index==ಸಂದೇಶ.length()) {
            ಪ್ರತ್ಯಾಗಮ index;
        }
        ಯದಿ(ಸಂದೇಶ.charAt(index)==u_ಬಲ_Curly_Brace) {
            ಯದಿ(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            ಪ್ರತ್ಯಾಗಮ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    int32_t start=index;
    UBool isEmpty=ಸತ್ಯ;
    UBool hasOther=ಅಸತ್ಯ;
    ಪ್ರತಿ(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==ಸಂದೇಶ.length();
        ಯದಿ(eos || ಸಂದೇಶ.charAt(index)==u_ಬಲ_Curly_Brace) {
            ಯದಿ(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            ಯದಿ(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            ಪ್ರತ್ಯಾಗಮ index;
        }
        int32_t selectorIndex=index;
        ಯದಿ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && ಸಂದೇಶ.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            ಯದಿ(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            ಯದಿ(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, ಅಸತ್ಯ,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } ಅನ್ಯಥಾ {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            ಯದಿ(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ಯದಿ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<ಸಂದೇಶ.length() &&
                0==ಸಂದೇಶ.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                ಯದಿ(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                // allow whitespace between offset: and its value
                int32_t ಮೌಲ್ಯ_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(ಮೌಲ್ಯ_Index);
                ಯದಿ(index==ಮೌಲ್ಯ_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ((index-ಮೌಲ್ಯ_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, ಮೌಲ್ಯ_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                parseDouble(ಮೌಲ್ಯ_Index, index, ಅಸತ್ಯ,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                ಯದಿ(U_FAILURE(errorCode)) {
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                isEmpty=ಅಸತ್ಯ;
                ಅನುವರ್ತ;  // no message fragment after the offset
            } ಅನ್ಯಥಾ {
                // normal selector word
                ಯದಿ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                ಯದಿ(0==ಸಂದೇಶ.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=ಸತ್ಯ;
                }
            }
        }
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        ಯದಿ(index==ಸಂದೇಶ.length() || ಸಂದೇಶ.charAt(index)!=u_ಎಡ_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        ಯದಿ(U_FAILURE(errorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        isEmpty=ಅಸತ್ಯ;
    }
}

int32_t
MessagePattern::parse_Arg_ಸಂಖ್ಯೆ(ಧ್ರುವ UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ಯದಿ(start>=limit) {
        ಪ್ರತ್ಯಾಗಮ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_ಸಂಖ್ಯೆ;
    char16_t c=s.charAt(start++);
    ಯದಿ(c==0x30) {
        ಯದಿ(start==limit) {
            ಪ್ರತ್ಯಾಗಮ 0;
        } ಅನ್ಯಥಾ {
            number=0;
            bad_ಸಂಖ್ಯೆ=ಸತ್ಯ;  // leading zero
        }
    } ಅನ್ಯಥಾ ಯದಿ(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_ಸಂಖ್ಯೆ=ಅಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ;
    }
    ಯಾವತ್(start<limit) {
        c=s.charAt(start++);
        ಯದಿ(0x30<=c && c<=0x39) {
            ಯದಿ(number>=INT32_MAX/10) {
                bad_ಸಂಖ್ಯೆ=ಸತ್ಯ;  // overflow
            } ಅನ್ಯಥಾ {
                number=number*10+(c-0x30);
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ UMSGPAT_ARG_NAME_NOT_ಸಂಖ್ಯೆ;
        }
    }
    // There are only ASCII digits.
    ಯದಿ(bad_ಸಂಖ್ಯೆ) {
        ಪ್ರತ್ಯಾಗಮ UMSGPAT_ARG_NAME_NOT_VALID;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ number;
    }
}

ಶೂನ್ಯ
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    ಪ್ರತಿ(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=ಸಂದೇಶ.charAt(index++);
        ಯದಿ(c==u_minus) {
            isNegative=1;
            ಯದಿ(index==limit) {
                ಭಙ್ಗ;  // no number
            }
            c=ಸಂದೇಶ.charAt(index++);
        } ಅನ್ಯಥಾ ಯದಿ(c==u_plus) {
            ಯದಿ(index==limit) {
                ಭಙ್ಗ;  // no number
            }
            c=ಸಂದೇಶ.charAt(index++);
        }
        ಯದಿ(c==0x221e) {  // infinity
            ಯದಿ(allowInfinity && index==limit) {
                ದ್ವಿಗುಣ infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                ಪ್ರತ್ಯಾಗಮ;
            } ಅನ್ಯಥಾ {
                ಭಙ್ಗ;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ಯಾವತ್('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ಯದಿ(value>(Part::MAX_ಮೌಲ್ಯ+isNegative)) {
                ಭಙ್ಗ;  // not a small-enough integer
            }
            ಯದಿ(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                ಪ್ರತ್ಯಾಗಮ;
            }
            c=ಸಂದೇಶ.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        ವರ್ಣ ಸಂಖ್ಯೆ_Chars[128];
        int32_t capacity = static_cast<int32_t>(ಪರಿಮಾಣ(ಸಂಖ್ಯೆ_Chars));
        int32_t length=limit-start;
        ಯದಿ(length>=capacity) {
            ಭಙ್ಗ;  // number too long
        }
        ಸಂದೇಶ.extract(start, length, ಸಂಖ್ಯೆ_Chars, capacity, US_INV);
        ಯದಿ (static_cast<int32_t>(uprv_strlen(ಸಂಖ್ಯೆ_Chars)) < length) {
            ಭಙ್ಗ;  // contains non-invariant character that was turned into NUL
        }
        ವರ್ಣ *end;
        ದ್ವಿಗುಣ numeric_ಮೌಲ್ಯ=uprv_strtod(ಸಂಖ್ಯೆ_Chars, &end);
        ಯದಿ(end!=(ಸಂಖ್ಯೆ_Chars+length)) {
            ಭಙ್ಗ;  // parsing error
        }
        addArgDoublePart(numeric_ಮೌಲ್ಯ, start, length, errorCode);
        ಪ್ರತ್ಯಾಗಮ;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ಧ್ರುವ char16_t *s=ಸಂದೇಶ.getBuffer();
    int32_t ಸಂದೇಶ_Length=ಸಂದೇಶ.length();
    ಧ್ರುವ char16_t *t=PatternProps::skipWhiteSpace(s+index, ಸಂದೇಶ_Length-index);
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ಧ್ರುವ char16_t *s=ಸಂದೇಶ.getBuffer();
    int32_t ಸಂದೇಶ_Length=ಸಂದೇಶ.length();
    ಧ್ರುವ char16_t *t=PatternProps::skipIdentifier(s+index, ಸಂದೇಶ_Length-index);
    ಪ್ರತ್ಯಾಗಮ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t ಸಂದೇಶ_Length=ಸಂದೇಶ.length();
    ಯಾವತ್(index<ಸಂದೇಶ_Length) {
        char16_t c=ಸಂದೇಶ.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ಯದಿ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ಭಙ್ಗ;
        }
        ++index;
    }
    ಪ್ರತ್ಯಾಗಮ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    ಪ್ರತ್ಯಾಗಮ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    ಪ್ರತ್ಯಾಗಮ
        ((c=ಸಂದೇಶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_h || c==u_H) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_o || c==u_O) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_i || c==u_I) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    ಪ್ರತ್ಯಾಗಮ
        ((c=ಸಂದೇಶ.charAt(index++))==u_p || c==u_P) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_l || c==u_L) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_u || c==u_U) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_r || c==u_R) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_a || c==u_A) &&
        ((c=ಸಂದೇಶ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    ಪ್ರತ್ಯಾಗಮ
        ((c=ಸಂದೇಶ.charAt(index++))==u_s || c==u_S) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_e || c==u_E) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_l || c==u_L) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_e || c==u_E) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ಸಂದೇಶ.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    ಪ್ರತ್ಯಾಗಮ
        ((c=ಸಂದೇಶ.charAt(index++))==u_o || c==u_O) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_r || c==u_R) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_d || c==u_D) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_i || c==u_I) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_n || c==u_N) &&
        ((c=ಸಂದೇಶ.charAt(index++))==u_a || c==u_A) &&
        ((c=ಸಂದೇಶ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    ಪ್ರತ್ಯಾಗಮ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_ಸಂದೇಶ_START;
}

UBool
MessagePattern::in_ತುದಿ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    ಪ್ರತ್ಯಾಗಮ
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_ಸಂದೇಶ_START;
}

ಶೂನ್ಯ
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    ಯದಿ(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

ಶೂನ್ಯ
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

ಶೂನ್ಯ
MessagePattern::addArgDoublePart(ದ್ವಿಗುಣ numeric_ಮೌಲ್ಯ, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    int32_t numericIndex=numeric_ಮೌಲ್ಯಗಳು_Length;
    ಯದಿ(numeric_ಮೌಲ್ಯಗಳು_List==nullptr) {
        numeric_ಮೌಲ್ಯಗಳು_List=ನೂತನ MessagePatternDoubleList();
        ಯದಿ(numeric_ಮೌಲ್ಯಗಳು_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
    } ಅನ್ಯಥಾ ಯದಿ(!numeric_ಮೌಲ್ಯಗಳು_List->ensureCapacityForOneMore(numeric_ಮೌಲ್ಯಗಳು_Length, errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    } ಅನ್ಯಥಾ {
        ಯದಿ(numericIndex>Part::MAX_ಮೌಲ್ಯ) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ;
        }
    }
    numeric_ಮೌಲ್ಯಗಳು_List->a[numeric_ಮೌಲ್ಯಗಳು_Length++]=numeric_ಮೌಲ್ಯ;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

ಶೂನ್ಯ
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    ಯದಿ(parseError==nullptr) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    ಯದಿ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ಯದಿ(length>0 && U16_IS_TRAIL(ಸಂದೇಶ[index-length])) {
            --length;
        }
    }
    ಸಂದೇಶ.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=ಸಂದೇಶ.length()-index;
    ಯದಿ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ಯದಿ(length>0 && U16_IS_LEAD(ಸಂದೇಶ[index+length-1])) {
            --length;
        }
    }
    ಸಂದೇಶ.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

ಶೂನ್ಯ
MessageImpl::appendReducedApostrophes(ಧ್ರುವ UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    ಪ್ರತಿ(;;) {
        int32_t i=s.indexOf(u_apos, start);
        ಯದಿ(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            ಭಙ್ಗ;
        }
        ಯದಿ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } ಅನ್ಯಥಾ {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ಧ್ರುವ MessagePattern& ಸಂದೇಶ_Pattern,
                                               int32_t ಸಂದೇಶ_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ಧ್ರುವ UnicodeString &ಸಂದೇಶ_String=ಸಂದೇಶ_Pattern.getPatternString();
    int32_t prevIndex=ಸಂದೇಶ_Pattern.getPart(ಸಂದೇಶ_Start).getLimit();
    ಪ್ರತಿ(int32_t i=ಸಂದೇಶ_Start;;) {
        ಧ್ರುವ MessagePattern::Part &part=ಸಂದೇಶ_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        ಯದಿ(type==UMSGPAT_PART_TYPE_ಸಂದೇಶ_LIMIT) {
            ಪ್ರತ್ಯಾಗಮ result.append(ಸಂದೇಶ_String, prevIndex, index-prevIndex);
        } ಅನ್ಯಥಾ ಯದಿ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(ಸಂದೇಶ_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } ಅನ್ಯಥಾ ಯದಿ(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(ಸಂದೇಶ_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=ಸಂದೇಶ_Pattern.getLimitPartIndex(i);
            index=ಸಂದೇಶ_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(ಸಂದೇಶ_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
