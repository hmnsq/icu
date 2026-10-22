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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_FORMATTING

#ଅନ୍ତର୍ଭାଵ "unicode/messagepattern.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "messageimpl.h"
#ଅନ୍ତର୍ଭାଵ "patternprops.h"
#ଅନ୍ତର୍ଭାଵ "putilimp.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ସ୍ଥିର ଅଚଳ char16_t u_pound=0x23;
ସ୍ଥିର ଅଚଳ char16_t u_apos=0x27;
ସ୍ଥିର ଅଚଳ char16_t u_plus=0x2B;
ସ୍ଥିର ଅଚଳ char16_t u_comma=0x2C;
ସ୍ଥିର ଅଚଳ char16_t u_minus=0x2D;
ସ୍ଥିର ଅଚଳ char16_t u_dot=0x2E;
ସ୍ଥିର ଅଚଳ char16_t u_colon=0x3A;
ସ୍ଥିର ଅଚଳ char16_t u_lessThan=0x3C;
ସ୍ଥିର ଅଚଳ char16_t u_equal=0x3D;
ସ୍ଥିର ଅଚଳ char16_t u_A=0x41;
ସ୍ଥିର ଅଚଳ char16_t u_C=0x43;
ସ୍ଥିର ଅଚଳ char16_t u_D=0x44;
ସ୍ଥିର ଅଚଳ char16_t u_E=0x45;
ସ୍ଥିର ଅଚଳ char16_t u_H=0x48;
ସ୍ଥିର ଅଚଳ char16_t u_I=0x49;
ସ୍ଥିର ଅଚଳ char16_t u_L=0x4C;
ସ୍ଥିର ଅଚଳ char16_t u_N=0x4E;
ସ୍ଥିର ଅଚଳ char16_t u_O=0x4F;
ସ୍ଥିର ଅଚଳ char16_t u_P=0x50;
ସ୍ଥିର ଅଚଳ char16_t u_R=0x52;
ସ୍ଥିର ଅଚଳ char16_t u_S=0x53;
ସ୍ଥିର ଅଚଳ char16_t u_T=0x54;
ସ୍ଥିର ଅଚଳ char16_t u_U=0x55;
ସ୍ଥିର ଅଚଳ char16_t u_Z=0x5A;
ସ୍ଥିର ଅଚଳ char16_t u_a=0x61;
ସ୍ଥିର ଅଚଳ char16_t u_c=0x63;
ସ୍ଥିର ଅଚଳ char16_t u_d=0x64;
ସ୍ଥିର ଅଚଳ char16_t u_e=0x65;
ସ୍ଥିର ଅଚଳ char16_t u_f=0x66;
ସ୍ଥିର ଅଚଳ char16_t u_h=0x68;
ସ୍ଥିର ଅଚଳ char16_t u_i=0x69;
ସ୍ଥିର ଅଚଳ char16_t u_l=0x6C;
ସ୍ଥିର ଅଚଳ char16_t u_n=0x6E;
ସ୍ଥିର ଅଚଳ char16_t u_o=0x6F;
ସ୍ଥିର ଅଚଳ char16_t u_p=0x70;
ସ୍ଥିର ଅଚଳ char16_t u_r=0x72;
ସ୍ଥିର ଅଚଳ char16_t u_s=0x73;
ସ୍ଥିର ଅଚଳ char16_t u_t=0x74;
ସ୍ଥିର ଅଚଳ char16_t u_u=0x75;
ସ୍ଥିର ଅଚଳ char16_t u_z=0x7A;
ସ୍ଥିର ଅଚଳ char16_t u_ବାମ_Curly_Brace=0x7B;
ସ୍ଥିର ଅଚଳ char16_t u_pipe=0x7C;
ସ୍ଥିର ଅଚଳ char16_t u_ଡାହାଣ_Curly_Brace=0x7D;
ସ୍ଥିର ଅଚଳ char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ସ୍ଥିର ଅଚଳ char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ସ୍ଥିର ଅଚଳ char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t ଗଦା_Capacity>
ଶ୍ରେଣୀ MessagePatternList : ସର୍ବସାଧାରଣ UMemory {
ସର୍ବସାଧାରଣ:
    MessagePatternList() {}
    ଶୂନ୍ୟ copyFrom(ଅଚଳ MessagePatternList<T, ଗଦା_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ଅଚଳ MessagePatternList<T, ଗଦା_Capacity> &other, int32_t length) ଅଚଳ {
        ପାଇଁ(int32_t i=0; i<length; ++i) {
            ଯଦି(a[i]!=other.a[i]) { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
        }
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }

    Maybe_ଗଦା_Array<T, ଗଦା_Capacity> a;
};

template<typename T, int32_t ଗଦା_Capacity>
ଶୂନ୍ୟ
MessagePatternList<T, ଗଦା_Capacity>::copyFrom(
        ଅଚଳ MessagePatternList<T, ଗଦା_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    ଯଦି(U_SUCCESS(errorCode) && length>0) {
        ଯଦି(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*ଆକାର(T));
    }
}

template<typename T, int32_t ଗଦା_Capacity>
UBool
MessagePatternList<T, ଗଦା_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଯଦି(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

// MessagePatternList specializations -------------------------------------- ***

ଶ୍ରେଣୀ MessagePatternDoubleList : ସର୍ବସାଧାରଣ MessagePatternList<ଦ୍ୱିଗୁଣ, 8> {
};

ଶ୍ରେଣୀ MessagePatternPartsList : ସର୍ବସାଧାରଣ MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length(0),
          hasArgNames(ମିଥ୍ୟା), hasArgNumbers(ମିଥ୍ୟା), needsAutoQuoting(ମିଥ୍ୟା) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length(0),
          hasArgNames(ମିଥ୍ୟା), hasArgNumbers(ମିଥ୍ୟା), needsAutoQuoting(ମିଥ୍ୟା) {
    init(errorCode);
}

MessagePattern::MessagePattern(ଅଚଳ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length(0),
          hasArgNames(ମିଥ୍ୟା), hasArgNumbers(ମିଥ୍ୟା), needsAutoQuoting(ମିଥ୍ୟା) {
    ଯଦି(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    partsList=ନୂତନ MessagePatternPartsList();
    ଯଦି(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    parts=partsList->a.getAlias();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

MessagePattern::MessagePattern(ଅଚଳ MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), ସନ୍ଦେଶ(other.ସନ୍ଦେଶ),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ(nullptr), numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ଯଦି(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ଅଚଳ MessagePattern& other) U_LIFETIME_BOUND {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ *ଏହି;
    }
    aposMode=other.aposMode;
    ସନ୍ଦେଶ=other.ସନ୍ଦେଶ;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    ଯଦି(!copyStorage(other, errorCode)) {
        clear();
    }
    ଫେରନ୍ତୁ *ଏହି;
}

UBool
MessagePattern::copyStorage(ଅଚଳ MessagePattern &other, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    parts=nullptr;
    partsLength=0;
    numeric_ମୂଲ୍ୟଗୁଡ଼ିକ=nullptr;
    numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length=0;
    ଯଦି(partsList==nullptr) {
        partsList=ନୂତନ MessagePatternPartsList();
        ଯଦି(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        parts=partsList->a.getAlias();
    }
    ଯଦି(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    ଯଦି(other.numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length>0) {
        ଯଦି(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List==nullptr) {
            numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List=ନୂତନ MessagePatternDoubleList();
            ଯଦି(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            numeric_ମୂଲ୍ୟଗୁଡ଼ିକ=numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->a.getAlias();
        }
        numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->copyFrom(
            *other.numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List, other.numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        numeric_ମୂଲ୍ୟଗୁଡ଼ିକ=numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->a.getAlias();
        numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length=other.numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ଅଚଳ UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    ଫେରନ୍ତୁ *ଏହି;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ଅଚଳ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    ଫେରନ୍ତୁ *ଏହି;
}

MessagePattern&
MessagePattern::parsePluralStyle(ଅଚଳ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    ଫେରନ୍ତୁ *ଏହି;
}

MessagePattern&
MessagePattern::parseSelectStyle(ଅଚଳ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    ଫେରନ୍ତୁ *ଏହି;
}

ଶୂନ୍ୟ
MessagePattern::clear() {
    // Mostly the same as preParse().
    ସନ୍ଦେଶ.remove();
    hasArgNames=hasArgNumbers=ମିଥ୍ୟା;
    needsAutoQuoting=ମିଥ୍ୟା;
    partsLength=0;
    numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length=0;
}

ତାର୍କିକ
MessagePattern::operator==(ଅଚଳ MessagePattern &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ
        aposMode==other.aposMode &&
        ସନ୍ଦେଶ==other.ସନ୍ଦେଶ &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ଅଚଳ {
    int32_t hash=(aposMode*37+ସନ୍ଦେଶ.hashCode())*37+partsLength;
    ପାଇଁ(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    ଫେରନ୍ତୁ hash;
}

int32_t
MessagePattern::validateArgumentName(ଅଚଳ UnicodeString &name) {
    ଯଦି(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        ଫେରନ୍ତୁ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ଫେରନ୍ତୁ parse_Arg_ସଂଖ୍ୟା(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ଅଚଳ {
    ଯଦି(!needsAutoQuoting) {
        ଫେରନ୍ତୁ ସନ୍ଦେଶ;
    }
    UnicodeString modified(ସନ୍ଦେଶ);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ଗଣନା_Parts();
    ପାଇଁ(int32_t i=count; i>0;) {
        ଅଚଳ Part &part=getPart(--i);
        ଯଦି(part.getType()==UMSGPAT_PART_TYPE_ଭର୍ତ୍ତି_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    ଫେରନ୍ତୁ modified;
}

ଦ୍ୱିଗୁଣ
MessagePattern::get_Numeric_ମୂଲ୍ୟ(ଅଚଳ Part &part) ଅଚଳ {
    UMessagePatternPartType type=part.type;
    ଯଦି(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ଫେରନ୍ତୁ part.value;
    } ଅନ୍ୟଥା ଯଦି(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ଫେରନ୍ତୁ numeric_ମୂଲ୍ୟଗୁଡ଼ିକ[part.value];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ UMSGPAT_NO_NUMERIC_ମୂଲ୍ୟ;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ଦ୍ୱିଗୁଣ
MessagePattern::getPluralOffset(int32_t pluralStart) ଅଚଳ {
    ଅଚଳ Part &part=getPart(pluralStart);
    ଯଦି(Part::has_Numeric_ମୂଲ୍ୟ(part.type)) {
        ଫେରନ୍ତୁ get_Numeric_ମୂଲ୍ୟ(part);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

ତାର୍କିକ
MessagePattern::Part::operator==(ଅଚଳ Part &other) ଅଚଳ {
    ଯଦି(ଏହି==&other) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଫେରନ୍ତୁ
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

ଶୂନ୍ୟ
MessagePattern::preParse(ଅଚଳ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଯଦି(parseError!=nullptr) {
        parseError->ଧାଡ଼ି=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    ସନ୍ଦେଶ=pattern;
    hasArgNames=hasArgNumbers=ମିଥ୍ୟା;
    needsAutoQuoting=ମିଥ୍ୟା;
    partsLength=0;
    numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length=0;
}

ଶୂନ୍ୟ
MessagePattern::postParse() {
    ଯଦି(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    ଯଦି(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List!=nullptr) {
        numeric_ମୂଲ୍ୟଗୁଡ଼ିକ=numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t ସନ୍ଦେଶ_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    int32_t ସନ୍ଦେଶ_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_START, index, ସନ୍ଦେଶ_Start_Length, nestingLevel, errorCode);
    index+=ସନ୍ଦେଶ_Start_Length;
    ପାଇଁ(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି(index>=ସନ୍ଦେଶ.length()) {
            ଭାଙ୍ଗ;
        }
        char16_t c=ସନ୍ଦେଶ.charAt(index++);
        ଯଦି(c==u_apos) {
            ଯଦି(index==ସନ୍ଦେଶ.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_ଭର୍ତ୍ତି_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=ସତ୍ୟ;
            } ଅନ୍ୟଥା {
                c=ସନ୍ଦେଶ.charAt(index);
                ଯଦି(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } ଅନ୍ୟଥା ଯଦି(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ବାମ_Curly_Brace || c==u_ଡାହାଣ_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    ପାଇଁ(;;) {
                        index=ସନ୍ଦେଶ.indexOf(u_apos, index+1);
                        ଯଦି(index>=0) {
                            ଯଦି(/*(index+1)<msg.length() &&*/ ସନ୍ଦେଶ.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } ଅନ୍ୟଥା {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                ଭାଙ୍ଗ;
                            }
                        } ଅନ୍ୟଥା {
                            // The quoted text reaches to the end of the of the message.
                            index=ସନ୍ଦେଶ.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_ଭର୍ତ୍ତି_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=ସତ୍ୟ;
                            ଭାଙ୍ଗ;
                        }
                    }
                } ଅନ୍ୟଥା {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_ଭର୍ତ୍ତି_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=ସତ୍ୟ;
                }
            }
        } ଅନ୍ୟଥା ଯଦି(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_ସଂଖ୍ୟା, index-1, 1, 0, errorCode);
        } ଅନ୍ୟଥା ଯଦି(c==u_ବାମ_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } ଅନ୍ୟଥା ଯଦି((nestingLevel>0 && c==u_ଡାହାଣ_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_ଡାହାଣ_Curly_Brace) ? 0 : 1;
            addLimitPart(ସନ୍ଦେଶ_Start, UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            ଯଦି(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ଫେରନ୍ତୁ index-1;
            } ଅନ୍ୟଥା {
                // continue parsing after the '}'
                ଫେରନ୍ତୁ index;
            }
        }  // else: c is part of literal text
    }
    ଯଦି(nestingLevel>0 && !in_ଶୀର୍ଷ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ଫେରନ୍ତୁ 0;
    }
    addLimitPart(ସନ୍ଦେଶ_Start, UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_LIMIT, index, 0, nestingLevel, errorCode);
    ଫେରନ୍ତୁ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    ଯଦି(index==ସନ୍ଦେଶ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ଫେରନ୍ତୁ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_ସଂଖ୍ୟା(nameIndex, index);
    ଯଦି(number>=0) {
        int32_t length=index-nameIndex;
        ଯଦି(length>Part::MAX_LENGTH || number>Part::MAX_ମୂଲ୍ୟ) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        hasArgNumbers=ସତ୍ୟ;
        addPart(UMSGPAT_PART_TYPE_ARG_ସଂଖ୍ୟା, nameIndex, length, number, errorCode);
    } ଅନ୍ୟଥା ଯଦି(number==UMSGPAT_ARG_NAME_NOT_ସଂଖ୍ୟା) {
        int32_t length=index-nameIndex;
        ଯଦି(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        hasArgNames=ସତ୍ୟ;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } ଅନ୍ୟଥା {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    index=skipWhiteSpace(index);
    ଯଦି(index==ସନ୍ଦେଶ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ଫେରନ୍ତୁ 0;
    }
    char16_t c=ସନ୍ଦେଶ.charAt(index);
    ଯଦି(c==u_ଡାହାଣ_Curly_Brace) {
        // all done
    } ଅନ୍ୟଥା ଯଦି(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ଫେରନ୍ତୁ 0;
    } ଅନ୍ୟଥା /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        ଯେତେବେଳେ(index<ସନ୍ଦେଶ.length() && isArgTypeChar(ସନ୍ଦେଶ.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        ଯଦି(index==ସନ୍ଦେଶ.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି(length==0 || ((c=ସନ୍ଦେଶ.charAt(index))!=u_comma && c!=u_ଡାହାଣ_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ଯଦି(length==6) {
            // case-insensitive comparisons for complex-type names
            ଯଦି(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ଅନ୍ୟଥା ଯଦି(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ଅନ୍ୟଥା ଯଦି(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ଅନ୍ୟଥା ଯଦି(length==13) {
            ଯଦି(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        ଯଦି(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        ଯଦି(c==u_ଡାହାଣ_Curly_Brace) {
            ଯଦି(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
        } ଅନ୍ୟଥା /* ',' */ {
            ++index;
            ଯଦି(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } ଅନ୍ୟଥା ଯଦି(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } ଅନ୍ୟଥା {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    ଫେରନ୍ତୁ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    ଯେତେବେଳେ(index<ସନ୍ଦେଶ.length()) {
        char16_t c=ସନ୍ଦେଶ.charAt(index++);
        ଯଦି(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=ସନ୍ଦେଶ.indexOf(u_apos, index);
            ଯଦି(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } ଅନ୍ୟଥା ଯଦି(c==u_ବାମ_Curly_Brace) {
            ++nestedBraces;
        } ଅନ୍ୟଥା ଯଦି(c==u_ଡାହାଣ_Curly_Brace) {
            ଯଦି(nestedBraces>0) {
                --nestedBraces;
            } ଅନ୍ୟଥା {
                int32_t length=--index-start;
                ଯଦି(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                ଫେରନ୍ତୁ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    ଫେରନ୍ତୁ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    ଯଦି(index==ସନ୍ଦେଶ.length() || ସନ୍ଦେଶ.charAt(index)==u_ଡାହାଣ_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ଫେରନ୍ତୁ 0;
    }
    ପାଇଁ(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t ସଂଖ୍ୟା_Index=index;
        index=skipDouble(index);
        int32_t length=index-ସଂଖ୍ୟା_Index;
        ଯଦି(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        ଯଦି(length>Part::MAX_LENGTH) {
            setParseError(parseError, ସଂଖ୍ୟା_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        parseDouble(ସଂଖ୍ୟା_Index, index, ସତ୍ୟ, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        ଯଦି(index==ସନ୍ଦେଶ.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        char16_t c=ସନ୍ଦେଶ.charAt(index);
        ଯଦି(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ଯଦି(index==ସନ୍ଦେଶ.length()) {
            ଫେରନ୍ତୁ index;
        }
        ଯଦି(ସନ୍ଦେଶ.charAt(index)==u_ଡାହାଣ_Curly_Brace) {
            ଯଦି(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            ଫେରନ୍ତୁ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    int32_t start=index;
    UBool isEmpty=ସତ୍ୟ;
    UBool hasOther=ମିଥ୍ୟା;
    ପାଇଁ(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==ସନ୍ଦେଶ.length();
        ଯଦି(eos || ସନ୍ଦେଶ.charAt(index)==u_ଡାହାଣ_Curly_Brace) {
            ଯଦି(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            ଯଦି(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                ଫେରନ୍ତୁ 0;
            }
            ଫେରନ୍ତୁ index;
        }
        int32_t selectorIndex=index;
        ଯଦି(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && ସନ୍ଦେଶ.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            ଯଦି(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            ଯଦି(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, ମିଥ୍ୟା,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } ଅନ୍ୟଥା {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            ଯଦି(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ଫେରନ୍ତୁ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ଯଦି( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<ସନ୍ଦେଶ.length() &&
                0==ସନ୍ଦେଶ.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                ଯଦି(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                // allow whitespace between offset: and its value
                int32_t ମୂଲ୍ୟ_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(ମୂଲ୍ୟ_Index);
                ଯଦି(index==ମୂଲ୍ୟ_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                ଯଦି((index-ମୂଲ୍ୟ_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, ମୂଲ୍ୟ_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                parseDouble(ମୂଲ୍ୟ_Index, index, ମିଥ୍ୟା,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                ଯଦି(U_FAILURE(errorCode)) {
                    ଫେରନ୍ତୁ 0;
                }
                isEmpty=ମିଥ୍ୟା;
                ଜାରି;  // no message fragment after the offset
            } ଅନ୍ୟଥା {
                // normal selector word
                ଯଦି(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ଫେରନ୍ତୁ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                ଯଦି(0==ସନ୍ଦେଶ.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=ସତ୍ୟ;
                }
            }
        }
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        ଯଦି(index==ସନ୍ଦେଶ.length() || ସନ୍ଦେଶ.charAt(index)!=u_ବାମ_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ଫେରନ୍ତୁ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        ଯଦି(U_FAILURE(errorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        isEmpty=ମିଥ୍ୟା;
    }
}

int32_t
MessagePattern::parse_Arg_ସଂଖ୍ୟା(ଅଚଳ UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ଯଦି(start>=limit) {
        ଫେରନ୍ତୁ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_ସଂଖ୍ୟା;
    char16_t c=s.charAt(start++);
    ଯଦି(c==0x30) {
        ଯଦି(start==limit) {
            ଫେରନ୍ତୁ 0;
        } ଅନ୍ୟଥା {
            number=0;
            bad_ସଂଖ୍ୟା=ସତ୍ୟ;  // leading zero
        }
    } ଅନ୍ୟଥା ଯଦି(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_ସଂଖ୍ୟା=ମିଥ୍ୟା;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ UMSGPAT_ARG_NAME_NOT_ସଂଖ୍ୟା;
    }
    ଯେତେବେଳେ(start<limit) {
        c=s.charAt(start++);
        ଯଦି(0x30<=c && c<=0x39) {
            ଯଦି(number>=INT32_MAX/10) {
                bad_ସଂଖ୍ୟା=ସତ୍ୟ;  // overflow
            } ଅନ୍ୟଥା {
                number=number*10+(c-0x30);
            }
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ UMSGPAT_ARG_NAME_NOT_ସଂଖ୍ୟା;
        }
    }
    // There are only ASCII digits.
    ଯଦି(bad_ସଂଖ୍ୟା) {
        ଫେରନ୍ତୁ UMSGPAT_ARG_NAME_NOT_VALID;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ number;
    }
}

ଶୂନ୍ୟ
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    ପାଇଁ(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=ସନ୍ଦେଶ.charAt(index++);
        ଯଦି(c==u_minus) {
            isNegative=1;
            ଯଦି(index==limit) {
                ଭାଙ୍ଗ;  // no number
            }
            c=ସନ୍ଦେଶ.charAt(index++);
        } ଅନ୍ୟଥା ଯଦି(c==u_plus) {
            ଯଦି(index==limit) {
                ଭାଙ୍ଗ;  // no number
            }
            c=ସନ୍ଦେଶ.charAt(index++);
        }
        ଯଦି(c==0x221e) {  // infinity
            ଯଦି(allowInfinity && index==limit) {
                ଦ୍ୱିଗୁଣ infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                ଫେରନ୍ତୁ;
            } ଅନ୍ୟଥା {
                ଭାଙ୍ଗ;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ଯେତେବେଳେ('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ଯଦି(value>(Part::MAX_ମୂଲ୍ୟ+isNegative)) {
                ଭାଙ୍ଗ;  // not a small-enough integer
            }
            ଯଦି(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                ଫେରନ୍ତୁ;
            }
            c=ସନ୍ଦେଶ.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        ଅକ୍ଷର ସଂଖ୍ୟା_Chars[128];
        int32_t capacity = static_cast<int32_t>(ଆକାର(ସଂଖ୍ୟା_Chars));
        int32_t length=limit-start;
        ଯଦି(length>=capacity) {
            ଭାଙ୍ଗ;  // number too long
        }
        ସନ୍ଦେଶ.extract(start, length, ସଂଖ୍ୟା_Chars, capacity, US_INV);
        ଯଦି (static_cast<int32_t>(uprv_strlen(ସଂଖ୍ୟା_Chars)) < length) {
            ଭାଙ୍ଗ;  // contains non-invariant character that was turned into NUL
        }
        ଅକ୍ଷର *end;
        ଦ୍ୱିଗୁଣ numeric_ମୂଲ୍ୟ=uprv_strtod(ସଂଖ୍ୟା_Chars, &end);
        ଯଦି(end!=(ସଂଖ୍ୟା_Chars+length)) {
            ଭାଙ୍ଗ;  // parsing error
        }
        addArgDoublePart(numeric_ମୂଲ୍ୟ, start, length, errorCode);
        ଫେରନ୍ତୁ;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ଅଚଳ char16_t *s=ସନ୍ଦେଶ.getBuffer();
    int32_t ସନ୍ଦେଶ_Length=ସନ୍ଦେଶ.length();
    ଅଚଳ char16_t *t=PatternProps::skipWhiteSpace(s+index, ସନ୍ଦେଶ_Length-index);
    ଫେରନ୍ତୁ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ଅଚଳ char16_t *s=ସନ୍ଦେଶ.getBuffer();
    int32_t ସନ୍ଦେଶ_Length=ସନ୍ଦେଶ.length();
    ଅଚଳ char16_t *t=PatternProps::skipIdentifier(s+index, ସନ୍ଦେଶ_Length-index);
    ଫେରନ୍ତୁ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t ସନ୍ଦେଶ_Length=ସନ୍ଦେଶ.length();
    ଯେତେବେଳେ(index<ସନ୍ଦେଶ_Length) {
        char16_t c=ସନ୍ଦେଶ.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ଯଦି((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ଭାଙ୍ଗ;
        }
        ++index;
    }
    ଫେରନ୍ତୁ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    ଫେରନ୍ତୁ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    ଫେରନ୍ତୁ
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_h || c==u_H) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_o || c==u_O) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_i || c==u_I) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ସନ୍ଦେଶ.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    ଫେରନ୍ତୁ
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_p || c==u_P) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_l || c==u_L) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_u || c==u_U) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_r || c==u_R) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_a || c==u_A) &&
        ((c=ସନ୍ଦେଶ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    ଫେରନ୍ତୁ
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_s || c==u_S) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_e || c==u_E) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_l || c==u_L) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_e || c==u_E) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_c || c==u_C) &&
        ((c=ସନ୍ଦେଶ.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    ଫେରନ୍ତୁ
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_o || c==u_O) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_r || c==u_R) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_d || c==u_D) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_i || c==u_I) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_n || c==u_N) &&
        ((c=ସନ୍ଦେଶ.charAt(index++))==u_a || c==u_A) &&
        ((c=ସନ୍ଦେଶ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    ଫେରନ୍ତୁ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_START;
}

UBool
MessagePattern::in_ଶୀର୍ଷ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    ଫେରନ୍ତୁ
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_START;
}

ଶୂନ୍ୟ
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    ଯଦି(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

ଶୂନ୍ୟ
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

ଶୂନ୍ୟ
MessagePattern::addArgDoublePart(ଦ୍ୱିଗୁଣ numeric_ମୂଲ୍ୟ, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    int32_t numericIndex=numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length;
    ଯଦି(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List==nullptr) {
        numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List=ନୂତନ MessagePatternDoubleList();
        ଯଦି(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ଫେରନ୍ତୁ;
        }
    } ଅନ୍ୟଥା ଯଦି(!numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->ensureCapacityForOneMore(numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length, errorCode)) {
        ଫେରନ୍ତୁ;
    } ଅନ୍ୟଥା {
        ଯଦି(numericIndex>Part::MAX_ମୂଲ୍ୟ) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ଫେରନ୍ତୁ;
        }
    }
    numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_List->a[numeric_ମୂଲ୍ୟଗୁଡ଼ିକ_Length++]=numeric_ମୂଲ୍ୟ;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

ଶୂନ୍ୟ
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    ଯଦି(parseError==nullptr) {
        ଫେରନ୍ତୁ;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    ଯଦି(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ଯଦି(length>0 && U16_IS_TRAIL(ସନ୍ଦେଶ[index-length])) {
            --length;
        }
    }
    ସନ୍ଦେଶ.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=ସନ୍ଦେଶ.length()-index;
    ଯଦି(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ଯଦି(length>0 && U16_IS_LEAD(ସନ୍ଦେଶ[index+length-1])) {
            --length;
        }
    }
    ସନ୍ଦେଶ.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

ଶୂନ୍ୟ
MessageImpl::appendReducedApostrophes(ଅଚଳ UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    ପାଇଁ(;;) {
        int32_t i=s.indexOf(u_apos, start);
        ଯଦି(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            ଭାଙ୍ଗ;
        }
        ଯଦି(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } ଅନ୍ୟଥା {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ଅଚଳ MessagePattern& ସନ୍ଦେଶ_Pattern,
                                               int32_t ସନ୍ଦେଶ_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ଅଚଳ UnicodeString &ସନ୍ଦେଶ_String=ସନ୍ଦେଶ_Pattern.getPatternString();
    int32_t prevIndex=ସନ୍ଦେଶ_Pattern.getPart(ସନ୍ଦେଶ_Start).getLimit();
    ପାଇଁ(int32_t i=ସନ୍ଦେଶ_Start;;) {
        ଅଚଳ MessagePattern::Part &part=ସନ୍ଦେଶ_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        ଯଦି(type==UMSGPAT_PART_TYPE_ସନ୍ଦେଶ_LIMIT) {
            ଫେରନ୍ତୁ result.append(ସନ୍ଦେଶ_String, prevIndex, index-prevIndex);
        } ଅନ୍ୟଥା ଯଦି(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(ସନ୍ଦେଶ_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } ଅନ୍ୟଥା ଯଦି(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(ସନ୍ଦେଶ_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=ସନ୍ଦେଶ_Pattern.getLimitPartIndex(i);
            index=ସନ୍ଦେଶ_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(ସନ୍ଦେଶ_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
