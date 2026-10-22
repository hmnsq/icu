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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_FORMATTING

#അന്തര്ഭാവ "unicode/messagepattern.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "messageimpl.h"
#അന്തര്ഭാവ "patternprops.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

സ്ഥിതം സ്ഥിരം char16_t u_pound=0x23;
സ്ഥിതം സ്ഥിരം char16_t u_apos=0x27;
സ്ഥിതം സ്ഥിരം char16_t u_plus=0x2B;
സ്ഥിതം സ്ഥിരം char16_t u_comma=0x2C;
സ്ഥിതം സ്ഥിരം char16_t u_minus=0x2D;
സ്ഥിതം സ്ഥിരം char16_t u_dot=0x2E;
സ്ഥിതം സ്ഥിരം char16_t u_colon=0x3A;
സ്ഥിതം സ്ഥിരം char16_t u_lessThan=0x3C;
സ്ഥിതം സ്ഥിരം char16_t u_equal=0x3D;
സ്ഥിതം സ്ഥിരം char16_t u_A=0x41;
സ്ഥിതം സ്ഥിരം char16_t u_C=0x43;
സ്ഥിതം സ്ഥിരം char16_t u_D=0x44;
സ്ഥിതം സ്ഥിരം char16_t u_E=0x45;
സ്ഥിതം സ്ഥിരം char16_t u_H=0x48;
സ്ഥിതം സ്ഥിരം char16_t u_I=0x49;
സ്ഥിതം സ്ഥിരം char16_t u_L=0x4C;
സ്ഥിതം സ്ഥിരം char16_t u_N=0x4E;
സ്ഥിതം സ്ഥിരം char16_t u_O=0x4F;
സ്ഥിതം സ്ഥിരം char16_t u_P=0x50;
സ്ഥിതം സ്ഥിരം char16_t u_R=0x52;
സ്ഥിതം സ്ഥിരം char16_t u_S=0x53;
സ്ഥിതം സ്ഥിരം char16_t u_T=0x54;
സ്ഥിതം സ്ഥിരം char16_t u_U=0x55;
സ്ഥിതം സ്ഥിരം char16_t u_Z=0x5A;
സ്ഥിതം സ്ഥിരം char16_t u_a=0x61;
സ്ഥിതം സ്ഥിരം char16_t u_c=0x63;
സ്ഥിതം സ്ഥിരം char16_t u_d=0x64;
സ്ഥിതം സ്ഥിരം char16_t u_e=0x65;
സ്ഥിതം സ്ഥിരം char16_t u_f=0x66;
സ്ഥിതം സ്ഥിരം char16_t u_h=0x68;
സ്ഥിതം സ്ഥിരം char16_t u_i=0x69;
സ്ഥിതം സ്ഥിരം char16_t u_l=0x6C;
സ്ഥിതം സ്ഥിരം char16_t u_n=0x6E;
സ്ഥിതം സ്ഥിരം char16_t u_o=0x6F;
സ്ഥിതം സ്ഥിരം char16_t u_p=0x70;
സ്ഥിതം സ്ഥിരം char16_t u_r=0x72;
സ്ഥിതം സ്ഥിരം char16_t u_s=0x73;
സ്ഥിതം സ്ഥിരം char16_t u_t=0x74;
സ്ഥിതം സ്ഥിരം char16_t u_u=0x75;
സ്ഥിതം സ്ഥിരം char16_t u_z=0x7A;
സ്ഥിതം സ്ഥിരം char16_t u_ഇടത്_Curly_Brace=0x7B;
സ്ഥിതം സ്ഥിരം char16_t u_pipe=0x7C;
സ്ഥിതം സ്ഥിരം char16_t u_വലത്_Curly_Brace=0x7D;
സ്ഥിതം സ്ഥിരം char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

സ്ഥിതം സ്ഥിരം char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

സ്ഥിതം സ്ഥിരം char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t അട്ടി_Capacity>
വർഗ്ഗം MessagePatternList : പൊതു UMemory {
പൊതു:
    MessagePatternList() {}
    ശൂന്യം copyFrom(സ്ഥിരം MessagePatternList<T, അട്ടി_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(സ്ഥിരം MessagePatternList<T, അട്ടി_Capacity> &other, int32_t length) സ്ഥിരം {
        വേണ്ടി(int32_t i=0; i<length; ++i) {
            എങ്കിൽ(a[i]!=other.a[i]) { തിരികെ അസത്യം; }
        }
        തിരികെ സത്യം;
    }

    Maybe_അട്ടി_Array<T, അട്ടി_Capacity> a;
};

template<typename T, int32_t അട്ടി_Capacity>
ശൂന്യം
MessagePatternList<T, അട്ടി_Capacity>::copyFrom(
        സ്ഥിരം MessagePatternList<T, അട്ടി_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    എങ്കിൽ(U_SUCCESS(errorCode) && length>0) {
        എങ്കിൽ(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*വലുപ്പം(T));
    }
}

template<typename T, int32_t അട്ടി_Capacity>
UBool
MessagePatternList<T, അട്ടി_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    എങ്കിൽ(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        തിരികെ സത്യം;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    തിരികെ അസത്യം;
}

// MessagePatternList specializations -------------------------------------- ***

വർഗ്ഗം MessagePatternDoubleList : പൊതു MessagePatternList<ഇരട്ട, 8> {
};

വർഗ്ഗം MessagePatternPartsList : പൊതു MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_മൂല്യങ്ങൾ_List(nullptr), numeric_മൂല്യങ്ങൾ(nullptr), numeric_മൂല്യങ്ങൾ_Length(0),
          hasArgNames(അസത്യം), hasArgNumbers(അസത്യം), needsAutoQuoting(അസത്യം) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_മൂല്യങ്ങൾ_List(nullptr), numeric_മൂല്യങ്ങൾ(nullptr), numeric_മൂല്യങ്ങൾ_Length(0),
          hasArgNames(അസത്യം), hasArgNumbers(അസത്യം), needsAutoQuoting(അസത്യം) {
    init(errorCode);
}

MessagePattern::MessagePattern(സ്ഥിരം UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_മൂല്യങ്ങൾ_List(nullptr), numeric_മൂല്യങ്ങൾ(nullptr), numeric_മൂല്യങ്ങൾ_Length(0),
          hasArgNames(അസത്യം), hasArgNumbers(അസത്യം), needsAutoQuoting(അസത്യം) {
    എങ്കിൽ(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    partsList=പുതിയ MessagePatternPartsList();
    എങ്കിൽ(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ അസത്യം;
    }
    parts=partsList->a.getAlias();
    തിരികെ സത്യം;
}

MessagePattern::MessagePattern(സ്ഥിരം MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), സന്ദേശം(other.സന്ദേശം),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_മൂല്യങ്ങൾ_List(nullptr), numeric_മൂല്യങ്ങൾ(nullptr), numeric_മൂല്യങ്ങൾ_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    എങ്കിൽ(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(സ്ഥിരം MessagePattern& other) U_LIFETIME_BOUND {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ *ഇത്;
    }
    aposMode=other.aposMode;
    സന്ദേശം=other.സന്ദേശം;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    എങ്കിൽ(!copyStorage(other, errorCode)) {
        clear();
    }
    തിരികെ *ഇത്;
}

UBool
MessagePattern::copyStorage(സ്ഥിരം MessagePattern &other, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ അസത്യം;
    }
    parts=nullptr;
    partsLength=0;
    numeric_മൂല്യങ്ങൾ=nullptr;
    numeric_മൂല്യങ്ങൾ_Length=0;
    എങ്കിൽ(partsList==nullptr) {
        partsList=പുതിയ MessagePatternPartsList();
        എങ്കിൽ(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ അസത്യം;
        }
        parts=partsList->a.getAlias();
    }
    എങ്കിൽ(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ അസത്യം;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    എങ്കിൽ(other.numeric_മൂല്യങ്ങൾ_Length>0) {
        എങ്കിൽ(numeric_മൂല്യങ്ങൾ_List==nullptr) {
            numeric_മൂല്യങ്ങൾ_List=പുതിയ MessagePatternDoubleList();
            എങ്കിൽ(numeric_മൂല്യങ്ങൾ_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                തിരികെ അസത്യം;
            }
            numeric_മൂല്യങ്ങൾ=numeric_മൂല്യങ്ങൾ_List->a.getAlias();
        }
        numeric_മൂല്യങ്ങൾ_List->copyFrom(
            *other.numeric_മൂല്യങ്ങൾ_List, other.numeric_മൂല്യങ്ങൾ_Length, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ അസത്യം;
        }
        numeric_മൂല്യങ്ങൾ=numeric_മൂല്യങ്ങൾ_List->a.getAlias();
        numeric_മൂല്യങ്ങൾ_Length=other.numeric_മൂല്യങ്ങൾ_Length;
    }
    തിരികെ സത്യം;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_മൂല്യങ്ങൾ_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(സ്ഥിരം UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    തിരികെ *ഇത്;
}

MessagePattern&
MessagePattern::parseChoiceStyle(സ്ഥിരം UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    തിരികെ *ഇത്;
}

MessagePattern&
MessagePattern::parsePluralStyle(സ്ഥിരം UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    തിരികെ *ഇത്;
}

MessagePattern&
MessagePattern::parseSelectStyle(സ്ഥിരം UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    തിരികെ *ഇത്;
}

ശൂന്യം
MessagePattern::clear() {
    // Mostly the same as preParse().
    സന്ദേശം.remove();
    hasArgNames=hasArgNumbers=അസത്യം;
    needsAutoQuoting=അസത്യം;
    partsLength=0;
    numeric_മൂല്യങ്ങൾ_Length=0;
}

തർക്കം
MessagePattern::operator==(സ്ഥിരം MessagePattern &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    തിരികെ
        aposMode==other.aposMode &&
        സന്ദേശം==other.സന്ദേശം &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() സ്ഥിരം {
    int32_t hash=(aposMode*37+സന്ദേശം.hashCode())*37+partsLength;
    വേണ്ടി(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    തിരികെ hash;
}

int32_t
MessagePattern::validateArgumentName(സ്ഥിരം UnicodeString &name) {
    എങ്കിൽ(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        തിരികെ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    തിരികെ parse_Arg_സംഖ്യ(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() സ്ഥിരം {
    എങ്കിൽ(!needsAutoQuoting) {
        തിരികെ സന്ദേശം;
    }
    UnicodeString modified(സന്ദേശം);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=എണ്ണം_Parts();
    വേണ്ടി(int32_t i=count; i>0;) {
        സ്ഥിരം Part &part=getPart(--i);
        എങ്കിൽ(part.getType()==UMSGPAT_PART_TYPE_തിരുകുക_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    തിരികെ modified;
}

ഇരട്ട
MessagePattern::get_Numeric_മൂല്യം(സ്ഥിരം Part &part) സ്ഥിരം {
    UMessagePatternPartType type=part.type;
    എങ്കിൽ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        തിരികെ part.value;
    } അല്ലെങ്കിൽ എങ്കിൽ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        തിരികെ numeric_മൂല്യങ്ങൾ[part.value];
    } അല്ലെങ്കിൽ {
        തിരികെ UMSGPAT_NO_NUMERIC_മൂല്യം;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ഇരട്ട
MessagePattern::getPluralOffset(int32_t pluralStart) സ്ഥിരം {
    സ്ഥിരം Part &part=getPart(pluralStart);
    എങ്കിൽ(Part::has_Numeric_മൂല്യം(part.type)) {
        തിരികെ get_Numeric_മൂല്യം(part);
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

തർക്കം
MessagePattern::Part::operator==(സ്ഥിരം Part &other) സ്ഥിരം {
    എങ്കിൽ(ഇത്==&other) {
        തിരികെ സത്യം;
    }
    തിരികെ
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

ശൂന്യം
MessagePattern::preParse(സ്ഥിരം UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    എങ്കിൽ(parseError!=nullptr) {
        parseError->വരി=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    സന്ദേശം=pattern;
    hasArgNames=hasArgNumbers=അസത്യം;
    needsAutoQuoting=അസത്യം;
    partsLength=0;
    numeric_മൂല്യങ്ങൾ_Length=0;
}

ശൂന്യം
MessagePattern::postParse() {
    എങ്കിൽ(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    എങ്കിൽ(numeric_മൂല്യങ്ങൾ_List!=nullptr) {
        numeric_മൂല്യങ്ങൾ=numeric_മൂല്യങ്ങൾ_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t സന്ദേശം_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        തിരികെ 0;
    }
    int32_t സന്ദേശം_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_സന്ദേശം_START, index, സന്ദേശം_Start_Length, nestingLevel, errorCode);
    index+=സന്ദേശം_Start_Length;
    വേണ്ടി(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
        എങ്കിൽ(index>=സന്ദേശം.length()) {
            നിർത്തുക;
        }
        char16_t c=സന്ദേശം.charAt(index++);
        എങ്കിൽ(c==u_apos) {
            എങ്കിൽ(index==സന്ദേശം.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_തിരുകുക_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=സത്യം;
            } അല്ലെങ്കിൽ {
                c=സന്ദേശം.charAt(index);
                എങ്കിൽ(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } അല്ലെങ്കിൽ എങ്കിൽ(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ഇടത്_Curly_Brace || c==u_വലത്_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    വേണ്ടി(;;) {
                        index=സന്ദേശം.indexOf(u_apos, index+1);
                        എങ്കിൽ(index>=0) {
                            എങ്കിൽ(/*(index+1)<msg.length() &&*/ സന്ദേശം.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } അല്ലെങ്കിൽ {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                നിർത്തുക;
                            }
                        } അല്ലെങ്കിൽ {
                            // The quoted text reaches to the end of the of the message.
                            index=സന്ദേശം.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_തിരുകുക_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=സത്യം;
                            നിർത്തുക;
                        }
                    }
                } അല്ലെങ്കിൽ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_തിരുകുക_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=സത്യം;
                }
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_സംഖ്യ, index-1, 1, 0, errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ(c==u_ഇടത്_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ((nestingLevel>0 && c==u_വലത്_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_വലത്_Curly_Brace) ? 0 : 1;
            addLimitPart(സന്ദേശം_Start, UMSGPAT_PART_TYPE_സന്ദേശം_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            എങ്കിൽ(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                തിരികെ index-1;
            } അല്ലെങ്കിൽ {
                // continue parsing after the '}'
                തിരികെ index;
            }
        }  // else: c is part of literal text
    }
    എങ്കിൽ(nestingLevel>0 && !in_മുകൾ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        തിരികെ 0;
    }
    addLimitPart(സന്ദേശം_Start, UMSGPAT_PART_TYPE_സന്ദേശം_LIMIT, index, 0, nestingLevel, errorCode);
    തിരികെ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    എങ്കിൽ(index==സന്ദേശം.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        തിരികെ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_സംഖ്യ(nameIndex, index);
    എങ്കിൽ(number>=0) {
        int32_t length=index-nameIndex;
        എങ്കിൽ(length>Part::MAX_LENGTH || number>Part::MAX_മൂല്യം) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
        hasArgNumbers=സത്യം;
        addPart(UMSGPAT_PART_TYPE_ARG_സംഖ്യ, nameIndex, length, number, errorCode);
    } അല്ലെങ്കിൽ എങ്കിൽ(number==UMSGPAT_ARG_NAME_NOT_സംഖ്യ) {
        int32_t length=index-nameIndex;
        എങ്കിൽ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
        hasArgNames=സത്യം;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } അല്ലെങ്കിൽ {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        തിരികെ 0;
    }
    index=skipWhiteSpace(index);
    എങ്കിൽ(index==സന്ദേശം.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        തിരികെ 0;
    }
    char16_t c=സന്ദേശം.charAt(index);
    എങ്കിൽ(c==u_വലത്_Curly_Brace) {
        // all done
    } അല്ലെങ്കിൽ എങ്കിൽ(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        തിരികെ 0;
    } അല്ലെങ്കിൽ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        ആകുമ്പോൾ(index<സന്ദേശം.length() && isArgTypeChar(സന്ദേശം.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        എങ്കിൽ(index==സന്ദേശം.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            തിരികെ 0;
        }
        എങ്കിൽ(length==0 || ((c=സന്ദേശം.charAt(index))!=u_comma && c!=u_വലത്_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            തിരികെ 0;
        }
        എങ്കിൽ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        എങ്കിൽ(length==6) {
            // case-insensitive comparisons for complex-type names
            എങ്കിൽ(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } അല്ലെങ്കിൽ എങ്കിൽ(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } അല്ലെങ്കിൽ എങ്കിൽ(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(length==13) {
            എങ്കിൽ(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        എങ്കിൽ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        എങ്കിൽ(c==u_വലത്_Curly_Brace) {
            എങ്കിൽ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
        } അല്ലെങ്കിൽ /* ',' */ {
            ++index;
            എങ്കിൽ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } അല്ലെങ്കിൽ എങ്കിൽ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } അല്ലെങ്കിൽ {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    തിരികെ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    ആകുമ്പോൾ(index<സന്ദേശം.length()) {
        char16_t c=സന്ദേശം.charAt(index++);
        എങ്കിൽ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=സന്ദേശം.indexOf(u_apos, index);
            എങ്കിൽ(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } അല്ലെങ്കിൽ എങ്കിൽ(c==u_ഇടത്_Curly_Brace) {
            ++nestedBraces;
        } അല്ലെങ്കിൽ എങ്കിൽ(c==u_വലത്_Curly_Brace) {
            എങ്കിൽ(nestedBraces>0) {
                --nestedBraces;
            } അല്ലെങ്കിൽ {
                int32_t length=--index-start;
                എങ്കിൽ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    തിരികെ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                തിരികെ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    തിരികെ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    എങ്കിൽ(index==സന്ദേശം.length() || സന്ദേശം.charAt(index)==u_വലത്_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        തിരികെ 0;
    }
    വേണ്ടി(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t സംഖ്യ_Index=index;
        index=skipDouble(index);
        int32_t length=index-സംഖ്യ_Index;
        എങ്കിൽ(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            തിരികെ 0;
        }
        എങ്കിൽ(length>Part::MAX_LENGTH) {
            setParseError(parseError, സംഖ്യ_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
        parseDouble(സംഖ്യ_Index, index, സത്യം, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        എങ്കിൽ(index==സന്ദേശം.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            തിരികെ 0;
        }
        char16_t c=സന്ദേശം.charAt(index);
        എങ്കിൽ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            തിരികെ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        എങ്കിൽ(index==സന്ദേശം.length()) {
            തിരികെ index;
        }
        എങ്കിൽ(സന്ദേശം.charAt(index)==u_വലത്_Curly_Brace) {
            എങ്കിൽ(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
            തിരികെ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ 0;
    }
    int32_t start=index;
    UBool isEmpty=സത്യം;
    UBool hasOther=അസത്യം;
    വേണ്ടി(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==സന്ദേശം.length();
        എങ്കിൽ(eos || സന്ദേശം.charAt(index)==u_വലത്_Curly_Brace) {
            എങ്കിൽ(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
            എങ്കിൽ(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                തിരികെ 0;
            }
            തിരികെ index;
        }
        int32_t selectorIndex=index;
        എങ്കിൽ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && സന്ദേശം.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            എങ്കിൽ(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
            എങ്കിൽ(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, അസത്യം,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } അല്ലെങ്കിൽ {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            എങ്കിൽ(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                തിരികെ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            എങ്കിൽ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<സന്ദേശം.length() &&
                0==സന്ദേശം.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                എങ്കിൽ(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    തിരികെ 0;
                }
                // allow whitespace between offset: and its value
                int32_t മൂല്യം_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(മൂല്യം_Index);
                എങ്കിൽ(index==മൂല്യം_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    തിരികെ 0;
                }
                എങ്കിൽ((index-മൂല്യം_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, മൂല്യം_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    തിരികെ 0;
                }
                parseDouble(മൂല്യം_Index, index, അസത്യം,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                എങ്കിൽ(U_FAILURE(errorCode)) {
                    തിരികെ 0;
                }
                isEmpty=അസത്യം;
                തുടരുക;  // no message fragment after the offset
            } അല്ലെങ്കിൽ {
                // normal selector word
                എങ്കിൽ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    തിരികെ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                എങ്കിൽ(0==സന്ദേശം.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=സത്യം;
                }
            }
        }
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        എങ്കിൽ(index==സന്ദേശം.length() || സന്ദേശം.charAt(index)!=u_ഇടത്_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            തിരികെ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        എങ്കിൽ(U_FAILURE(errorCode)) {
            തിരികെ 0;
        }
        isEmpty=അസത്യം;
    }
}

int32_t
MessagePattern::parse_Arg_സംഖ്യ(സ്ഥിരം UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    എങ്കിൽ(start>=limit) {
        തിരികെ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_സംഖ്യ;
    char16_t c=s.charAt(start++);
    എങ്കിൽ(c==0x30) {
        എങ്കിൽ(start==limit) {
            തിരികെ 0;
        } അല്ലെങ്കിൽ {
            number=0;
            bad_സംഖ്യ=സത്യം;  // leading zero
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_സംഖ്യ=അസത്യം;
    } അല്ലെങ്കിൽ {
        തിരികെ UMSGPAT_ARG_NAME_NOT_സംഖ്യ;
    }
    ആകുമ്പോൾ(start<limit) {
        c=s.charAt(start++);
        എങ്കിൽ(0x30<=c && c<=0x39) {
            എങ്കിൽ(number>=INT32_MAX/10) {
                bad_സംഖ്യ=സത്യം;  // overflow
            } അല്ലെങ്കിൽ {
                number=number*10+(c-0x30);
            }
        } അല്ലെങ്കിൽ {
            തിരികെ UMSGPAT_ARG_NAME_NOT_സംഖ്യ;
        }
    }
    // There are only ASCII digits.
    എങ്കിൽ(bad_സംഖ്യ) {
        തിരികെ UMSGPAT_ARG_NAME_NOT_VALID;
    } അല്ലെങ്കിൽ {
        തിരികെ number;
    }
}

ശൂന്യം
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    വേണ്ടി(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=സന്ദേശം.charAt(index++);
        എങ്കിൽ(c==u_minus) {
            isNegative=1;
            എങ്കിൽ(index==limit) {
                നിർത്തുക;  // no number
            }
            c=സന്ദേശം.charAt(index++);
        } അല്ലെങ്കിൽ എങ്കിൽ(c==u_plus) {
            എങ്കിൽ(index==limit) {
                നിർത്തുക;  // no number
            }
            c=സന്ദേശം.charAt(index++);
        }
        എങ്കിൽ(c==0x221e) {  // infinity
            എങ്കിൽ(allowInfinity && index==limit) {
                ഇരട്ട infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                തിരികെ;
            } അല്ലെങ്കിൽ {
                നിർത്തുക;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ആകുമ്പോൾ('0'<=c && c<='9') {
            value=value*10+(c-'0');
            എങ്കിൽ(value>(Part::MAX_മൂല്യം+isNegative)) {
                നിർത്തുക;  // not a small-enough integer
            }
            എങ്കിൽ(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                തിരികെ;
            }
            c=സന്ദേശം.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        അക്ഷരം സംഖ്യ_Chars[128];
        int32_t capacity = static_cast<int32_t>(വലുപ്പം(സംഖ്യ_Chars));
        int32_t length=limit-start;
        എങ്കിൽ(length>=capacity) {
            നിർത്തുക;  // number too long
        }
        സന്ദേശം.extract(start, length, സംഖ്യ_Chars, capacity, US_INV);
        എങ്കിൽ (static_cast<int32_t>(uprv_strlen(സംഖ്യ_Chars)) < length) {
            നിർത്തുക;  // contains non-invariant character that was turned into NUL
        }
        അക്ഷരം *end;
        ഇരട്ട numeric_മൂല്യം=uprv_strtod(സംഖ്യ_Chars, &end);
        എങ്കിൽ(end!=(സംഖ്യ_Chars+length)) {
            നിർത്തുക;  // parsing error
        }
        addArgDoublePart(numeric_മൂല്യം, start, length, errorCode);
        തിരികെ;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    സ്ഥിരം char16_t *s=സന്ദേശം.getBuffer();
    int32_t സന്ദേശം_Length=സന്ദേശം.length();
    സ്ഥിരം char16_t *t=PatternProps::skipWhiteSpace(s+index, സന്ദേശം_Length-index);
    തിരികെ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    സ്ഥിരം char16_t *s=സന്ദേശം.getBuffer();
    int32_t സന്ദേശം_Length=സന്ദേശം.length();
    സ്ഥിരം char16_t *t=PatternProps::skipIdentifier(s+index, സന്ദേശം_Length-index);
    തിരികെ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t സന്ദേശം_Length=സന്ദേശം.length();
    ആകുമ്പോൾ(index<സന്ദേശം_Length) {
        char16_t c=സന്ദേശം.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        എങ്കിൽ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            നിർത്തുക;
        }
        ++index;
    }
    തിരികെ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    തിരികെ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    തിരികെ
        ((c=സന്ദേശം.charAt(index++))==u_c || c==u_C) &&
        ((c=സന്ദേശം.charAt(index++))==u_h || c==u_H) &&
        ((c=സന്ദേശം.charAt(index++))==u_o || c==u_O) &&
        ((c=സന്ദേശം.charAt(index++))==u_i || c==u_I) &&
        ((c=സന്ദേശം.charAt(index++))==u_c || c==u_C) &&
        ((c=സന്ദേശം.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    തിരികെ
        ((c=സന്ദേശം.charAt(index++))==u_p || c==u_P) &&
        ((c=സന്ദേശം.charAt(index++))==u_l || c==u_L) &&
        ((c=സന്ദേശം.charAt(index++))==u_u || c==u_U) &&
        ((c=സന്ദേശം.charAt(index++))==u_r || c==u_R) &&
        ((c=സന്ദേശം.charAt(index++))==u_a || c==u_A) &&
        ((c=സന്ദേശം.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    തിരികെ
        ((c=സന്ദേശം.charAt(index++))==u_s || c==u_S) &&
        ((c=സന്ദേശം.charAt(index++))==u_e || c==u_E) &&
        ((c=സന്ദേശം.charAt(index++))==u_l || c==u_L) &&
        ((c=സന്ദേശം.charAt(index++))==u_e || c==u_E) &&
        ((c=സന്ദേശം.charAt(index++))==u_c || c==u_C) &&
        ((c=സന്ദേശം.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    തിരികെ
        ((c=സന്ദേശം.charAt(index++))==u_o || c==u_O) &&
        ((c=സന്ദേശം.charAt(index++))==u_r || c==u_R) &&
        ((c=സന്ദേശം.charAt(index++))==u_d || c==u_D) &&
        ((c=സന്ദേശം.charAt(index++))==u_i || c==u_I) &&
        ((c=സന്ദേശം.charAt(index++))==u_n || c==u_N) &&
        ((c=സന്ദേശം.charAt(index++))==u_a || c==u_A) &&
        ((c=സന്ദേശം.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    തിരികെ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_സന്ദേശം_START;
}

UBool
MessagePattern::in_മുകൾ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    തിരികെ
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_സന്ദേശം_START;
}

ശൂന്യം
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    എങ്കിൽ(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

ശൂന്യം
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

ശൂന്യം
MessagePattern::addArgDoublePart(ഇരട്ട numeric_മൂല്യം, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    int32_t numericIndex=numeric_മൂല്യങ്ങൾ_Length;
    എങ്കിൽ(numeric_മൂല്യങ്ങൾ_List==nullptr) {
        numeric_മൂല്യങ്ങൾ_List=പുതിയ MessagePatternDoubleList();
        എങ്കിൽ(numeric_മൂല്യങ്ങൾ_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ;
        }
    } അല്ലെങ്കിൽ എങ്കിൽ(!numeric_മൂല്യങ്ങൾ_List->ensureCapacityForOneMore(numeric_മൂല്യങ്ങൾ_Length, errorCode)) {
        തിരികെ;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ(numericIndex>Part::MAX_മൂല്യം) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ;
        }
    }
    numeric_മൂല്യങ്ങൾ_List->a[numeric_മൂല്യങ്ങൾ_Length++]=numeric_മൂല്യം;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

ശൂന്യം
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    എങ്കിൽ(parseError==nullptr) {
        തിരികെ;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    എങ്കിൽ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        എങ്കിൽ(length>0 && U16_IS_TRAIL(സന്ദേശം[index-length])) {
            --length;
        }
    }
    സന്ദേശം.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=സന്ദേശം.length()-index;
    എങ്കിൽ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        എങ്കിൽ(length>0 && U16_IS_LEAD(സന്ദേശം[index+length-1])) {
            --length;
        }
    }
    സന്ദേശം.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

ശൂന്യം
MessageImpl::appendReducedApostrophes(സ്ഥിരം UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    വേണ്ടി(;;) {
        int32_t i=s.indexOf(u_apos, start);
        എങ്കിൽ(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            നിർത്തുക;
        }
        എങ്കിൽ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } അല്ലെങ്കിൽ {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(സ്ഥിരം MessagePattern& സന്ദേശം_Pattern,
                                               int32_t സന്ദേശം_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    സ്ഥിരം UnicodeString &സന്ദേശം_String=സന്ദേശം_Pattern.getPatternString();
    int32_t prevIndex=സന്ദേശം_Pattern.getPart(സന്ദേശം_Start).getLimit();
    വേണ്ടി(int32_t i=സന്ദേശം_Start;;) {
        സ്ഥിരം MessagePattern::Part &part=സന്ദേശം_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        എങ്കിൽ(type==UMSGPAT_PART_TYPE_സന്ദേശം_LIMIT) {
            തിരികെ result.append(സന്ദേശം_String, prevIndex, index-prevIndex);
        } അല്ലെങ്കിൽ എങ്കിൽ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(സന്ദേശം_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } അല്ലെങ്കിൽ എങ്കിൽ(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(സന്ദേശം_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=സന്ദേശം_Pattern.getLimitPartIndex(i);
            index=സന്ദേശം_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(സന്ദേശം_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
