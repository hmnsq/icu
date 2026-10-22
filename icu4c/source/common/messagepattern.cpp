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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_FORMATTING

#அந்தர்பாவ "unicode/messagepattern.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "messageimpl.h"
#அந்தர்பாவ "patternprops.h"
#அந்தர்பாவ "putilimp.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

நிலையான மாறிலி char16_t u_pound=0x23;
நிலையான மாறிலி char16_t u_apos=0x27;
நிலையான மாறிலி char16_t u_plus=0x2B;
நிலையான மாறிலி char16_t u_comma=0x2C;
நிலையான மாறிலி char16_t u_minus=0x2D;
நிலையான மாறிலி char16_t u_dot=0x2E;
நிலையான மாறிலி char16_t u_colon=0x3A;
நிலையான மாறிலி char16_t u_lessThan=0x3C;
நிலையான மாறிலி char16_t u_equal=0x3D;
நிலையான மாறிலி char16_t u_A=0x41;
நிலையான மாறிலி char16_t u_C=0x43;
நிலையான மாறிலி char16_t u_D=0x44;
நிலையான மாறிலி char16_t u_E=0x45;
நிலையான மாறிலி char16_t u_H=0x48;
நிலையான மாறிலி char16_t u_I=0x49;
நிலையான மாறிலி char16_t u_L=0x4C;
நிலையான மாறிலி char16_t u_N=0x4E;
நிலையான மாறிலி char16_t u_O=0x4F;
நிலையான மாறிலி char16_t u_P=0x50;
நிலையான மாறிலி char16_t u_R=0x52;
நிலையான மாறிலி char16_t u_S=0x53;
நிலையான மாறிலி char16_t u_T=0x54;
நிலையான மாறிலி char16_t u_U=0x55;
நிலையான மாறிலி char16_t u_Z=0x5A;
நிலையான மாறிலி char16_t u_a=0x61;
நிலையான மாறிலி char16_t u_c=0x63;
நிலையான மாறிலி char16_t u_d=0x64;
நிலையான மாறிலி char16_t u_e=0x65;
நிலையான மாறிலி char16_t u_f=0x66;
நிலையான மாறிலி char16_t u_h=0x68;
நிலையான மாறிலி char16_t u_i=0x69;
நிலையான மாறிலி char16_t u_l=0x6C;
நிலையான மாறிலி char16_t u_n=0x6E;
நிலையான மாறிலி char16_t u_o=0x6F;
நிலையான மாறிலி char16_t u_p=0x70;
நிலையான மாறிலி char16_t u_r=0x72;
நிலையான மாறிலி char16_t u_s=0x73;
நிலையான மாறிலி char16_t u_t=0x74;
நிலையான மாறிலி char16_t u_u=0x75;
நிலையான மாறிலி char16_t u_z=0x7A;
நிலையான மாறிலி char16_t u_இடது_Curly_Brace=0x7B;
நிலையான மாறிலி char16_t u_pipe=0x7C;
நிலையான மாறிலி char16_t u_வலது_Curly_Brace=0x7D;
நிலையான மாறிலி char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

நிலையான மாறிலி char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

நிலையான மாறிலி char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t அடுக்கு_Capacity>
வகுப்பு MessagePatternList : பொது UMemory {
பொது:
    MessagePatternList() {}
    வெறுமை copyFrom(மாறிலி MessagePatternList<T, அடுக்கு_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(மாறிலி MessagePatternList<T, அடுக்கு_Capacity> &other, int32_t length) மாறிலி {
        க்கு(int32_t i=0; i<length; ++i) {
            எனில்(a[i]!=other.a[i]) { திரும்பு பொய்; }
        }
        திரும்பு உண்மை;
    }

    Maybe_அடுக்கு_Array<T, அடுக்கு_Capacity> a;
};

template<typename T, int32_t அடுக்கு_Capacity>
வெறுமை
MessagePatternList<T, அடுக்கு_Capacity>::copyFrom(
        மாறிலி MessagePatternList<T, அடுக்கு_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    எனில்(U_SUCCESS(errorCode) && length>0) {
        எனில்(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*அளவு(T));
    }
}

template<typename T, int32_t அடுக்கு_Capacity>
UBool
MessagePatternList<T, அடுக்கு_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    எனில்(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        திரும்பு உண்மை;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    திரும்பு பொய்;
}

// MessagePatternList specializations -------------------------------------- ***

வகுப்பு MessagePatternDoubleList : பொது MessagePatternList<இரட்டை, 8> {
};

வகுப்பு MessagePatternPartsList : பொது MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_மதிப்புகள்_List(nullptr), numeric_மதிப்புகள்(nullptr), numeric_மதிப்புகள்_Length(0),
          hasArgNames(பொய்), hasArgNumbers(பொய்), needsAutoQuoting(பொய்) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_மதிப்புகள்_List(nullptr), numeric_மதிப்புகள்(nullptr), numeric_மதிப்புகள்_Length(0),
          hasArgNames(பொய்), hasArgNumbers(பொய்), needsAutoQuoting(பொய்) {
    init(errorCode);
}

MessagePattern::MessagePattern(மாறிலி UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_மதிப்புகள்_List(nullptr), numeric_மதிப்புகள்(nullptr), numeric_மதிப்புகள்_Length(0),
          hasArgNames(பொய்), hasArgNumbers(பொய்), needsAutoQuoting(பொய்) {
    எனில்(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    partsList=புதிய MessagePatternPartsList();
    எனில்(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு பொய்;
    }
    parts=partsList->a.getAlias();
    திரும்பு உண்மை;
}

MessagePattern::MessagePattern(மாறிலி MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), செய்தி(other.செய்தி),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_மதிப்புகள்_List(nullptr), numeric_மதிப்புகள்(nullptr), numeric_மதிப்புகள்_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    எனில்(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(மாறிலி MessagePattern& other) U_LIFETIME_BOUND {
    எனில்(இது==&other) {
        திரும்பு *இது;
    }
    aposMode=other.aposMode;
    செய்தி=other.செய்தி;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    எனில்(!copyStorage(other, errorCode)) {
        clear();
    }
    திரும்பு *இது;
}

UBool
MessagePattern::copyStorage(மாறிலி MessagePattern &other, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு பொய்;
    }
    parts=nullptr;
    partsLength=0;
    numeric_மதிப்புகள்=nullptr;
    numeric_மதிப்புகள்_Length=0;
    எனில்(partsList==nullptr) {
        partsList=புதிய MessagePatternPartsList();
        எனில்(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு பொய்;
        }
        parts=partsList->a.getAlias();
    }
    எனில்(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு பொய்;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    எனில்(other.numeric_மதிப்புகள்_Length>0) {
        எனில்(numeric_மதிப்புகள்_List==nullptr) {
            numeric_மதிப்புகள்_List=புதிய MessagePatternDoubleList();
            எனில்(numeric_மதிப்புகள்_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                திரும்பு பொய்;
            }
            numeric_மதிப்புகள்=numeric_மதிப்புகள்_List->a.getAlias();
        }
        numeric_மதிப்புகள்_List->copyFrom(
            *other.numeric_மதிப்புகள்_List, other.numeric_மதிப்புகள்_Length, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு பொய்;
        }
        numeric_மதிப்புகள்=numeric_மதிப்புகள்_List->a.getAlias();
        numeric_மதிப்புகள்_Length=other.numeric_மதிப்புகள்_Length;
    }
    திரும்பு உண்மை;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_மதிப்புகள்_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(மாறிலி UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    திரும்பு *இது;
}

MessagePattern&
MessagePattern::parseChoiceStyle(மாறிலி UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    திரும்பு *இது;
}

MessagePattern&
MessagePattern::parsePluralStyle(மாறிலி UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    திரும்பு *இது;
}

MessagePattern&
MessagePattern::parseSelectStyle(மாறிலி UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    திரும்பு *இது;
}

வெறுமை
MessagePattern::clear() {
    // Mostly the same as preParse().
    செய்தி.remove();
    hasArgNames=hasArgNumbers=பொய்;
    needsAutoQuoting=பொய்;
    partsLength=0;
    numeric_மதிப்புகள்_Length=0;
}

தருக்க
MessagePattern::operator==(மாறிலி MessagePattern &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    திரும்பு
        aposMode==other.aposMode &&
        செய்தி==other.செய்தி &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() மாறிலி {
    int32_t hash=(aposMode*37+செய்தி.hashCode())*37+partsLength;
    க்கு(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    திரும்பு hash;
}

int32_t
MessagePattern::validateArgumentName(மாறிலி UnicodeString &name) {
    எனில்(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        திரும்பு UMSGPAT_ARG_NAME_NOT_VALID;
    }
    திரும்பு parse_Arg_எண்(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() மாறிலி {
    எனில்(!needsAutoQuoting) {
        திரும்பு செய்தி;
    }
    UnicodeString modified(செய்தி);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=மொத்தம்_Parts();
    க்கு(int32_t i=count; i>0;) {
        மாறிலி Part &part=getPart(--i);
        எனில்(part.getType()==UMSGPAT_PART_TYPE_செருகு_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    திரும்பு modified;
}

இரட்டை
MessagePattern::get_Numeric_மதிப்பு(மாறிலி Part &part) மாறிலி {
    UMessagePatternPartType type=part.type;
    எனில்(type==UMSGPAT_PART_TYPE_ARG_INT) {
        திரும்பு part.value;
    } இல்லையெனில் எனில்(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        திரும்பு numeric_மதிப்புகள்[part.value];
    } இல்லையெனில் {
        திரும்பு UMSGPAT_NO_NUMERIC_மதிப்பு;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
இரட்டை
MessagePattern::getPluralOffset(int32_t pluralStart) மாறிலி {
    மாறிலி Part &part=getPart(pluralStart);
    எனில்(Part::has_Numeric_மதிப்பு(part.type)) {
        திரும்பு get_Numeric_மதிப்பு(part);
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

தருக்க
MessagePattern::Part::operator==(மாறிலி Part &other) மாறிலி {
    எனில்(இது==&other) {
        திரும்பு உண்மை;
    }
    திரும்பு
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

வெறுமை
MessagePattern::preParse(மாறிலி UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    எனில்(parseError!=nullptr) {
        parseError->வரி=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    செய்தி=pattern;
    hasArgNames=hasArgNumbers=பொய்;
    needsAutoQuoting=பொய்;
    partsLength=0;
    numeric_மதிப்புகள்_Length=0;
}

வெறுமை
MessagePattern::postParse() {
    எனில்(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    எனில்(numeric_மதிப்புகள்_List!=nullptr) {
        numeric_மதிப்புகள்=numeric_மதிப்புகள்_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t செய்தி_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    எனில்(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        திரும்பு 0;
    }
    int32_t செய்தி_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_செய்தி_START, index, செய்தி_Start_Length, nestingLevel, errorCode);
    index+=செய்தி_Start_Length;
    க்கு(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
        எனில்(index>=செய்தி.length()) {
            நிறுத்து;
        }
        char16_t c=செய்தி.charAt(index++);
        எனில்(c==u_apos) {
            எனில்(index==செய்தி.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_செருகு_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=உண்மை;
            } இல்லையெனில் {
                c=செய்தி.charAt(index);
                எனில்(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } இல்லையெனில் எனில்(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_இடது_Curly_Brace || c==u_வலது_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    க்கு(;;) {
                        index=செய்தி.indexOf(u_apos, index+1);
                        எனில்(index>=0) {
                            எனில்(/*(index+1)<msg.length() &&*/ செய்தி.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } இல்லையெனில் {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                நிறுத்து;
                            }
                        } இல்லையெனில் {
                            // The quoted text reaches to the end of the of the message.
                            index=செய்தி.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_செருகு_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=உண்மை;
                            நிறுத்து;
                        }
                    }
                } இல்லையெனில் {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_செருகு_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=உண்மை;
                }
            }
        } இல்லையெனில் எனில்(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_எண், index-1, 1, 0, errorCode);
        } இல்லையெனில் எனில்(c==u_இடது_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } இல்லையெனில் எனில்((nestingLevel>0 && c==u_வலது_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_வலது_Curly_Brace) ? 0 : 1;
            addLimitPart(செய்தி_Start, UMSGPAT_PART_TYPE_செய்தி_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            எனில்(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                திரும்பு index-1;
            } இல்லையெனில் {
                // continue parsing after the '}'
                திரும்பு index;
            }
        }  // else: c is part of literal text
    }
    எனில்(nestingLevel>0 && !in_உச்சி_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        திரும்பு 0;
    }
    addLimitPart(செய்தி_Start, UMSGPAT_PART_TYPE_செய்தி_LIMIT, index, 0, nestingLevel, errorCode);
    திரும்பு index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    எனில்(index==செய்தி.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        திரும்பு 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_எண்(nameIndex, index);
    எனில்(number>=0) {
        int32_t length=index-nameIndex;
        எனில்(length>Part::MAX_LENGTH || number>Part::MAX_மதிப்பு) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
        hasArgNumbers=உண்மை;
        addPart(UMSGPAT_PART_TYPE_ARG_எண், nameIndex, length, number, errorCode);
    } இல்லையெனில் எனில்(number==UMSGPAT_ARG_NAME_NOT_எண்) {
        int32_t length=index-nameIndex;
        எனில்(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
        hasArgNames=உண்மை;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } இல்லையெனில் {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        திரும்பு 0;
    }
    index=skipWhiteSpace(index);
    எனில்(index==செய்தி.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        திரும்பு 0;
    }
    char16_t c=செய்தி.charAt(index);
    எனில்(c==u_வலது_Curly_Brace) {
        // all done
    } இல்லையெனில் எனில்(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        திரும்பு 0;
    } இல்லையெனில் /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        போது(index<செய்தி.length() && isArgTypeChar(செய்தி.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        எனில்(index==செய்தி.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            திரும்பு 0;
        }
        எனில்(length==0 || ((c=செய்தி.charAt(index))!=u_comma && c!=u_வலது_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            திரும்பு 0;
        }
        எனில்(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        எனில்(length==6) {
            // case-insensitive comparisons for complex-type names
            எனில்(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } இல்லையெனில் எனில்(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } இல்லையெனில் எனில்(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } இல்லையெனில் எனில்(length==13) {
            எனில்(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        எனில்(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        எனில்(c==u_வலது_Curly_Brace) {
            எனில்(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
        } இல்லையெனில் /* ',' */ {
            ++index;
            எனில்(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } இல்லையெனில் எனில்(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } இல்லையெனில் {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    திரும்பு index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    போது(index<செய்தி.length()) {
        char16_t c=செய்தி.charAt(index++);
        எனில்(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=செய்தி.indexOf(u_apos, index);
            எனில்(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } இல்லையெனில் எனில்(c==u_இடது_Curly_Brace) {
            ++nestedBraces;
        } இல்லையெனில் எனில்(c==u_வலது_Curly_Brace) {
            எனில்(nestedBraces>0) {
                --nestedBraces;
            } இல்லையெனில் {
                int32_t length=--index-start;
                எனில்(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    திரும்பு 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                திரும்பு index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    திரும்பு 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    எனில்(index==செய்தி.length() || செய்தி.charAt(index)==u_வலது_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        திரும்பு 0;
    }
    க்கு(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t எண்_Index=index;
        index=skipDouble(index);
        int32_t length=index-எண்_Index;
        எனில்(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            திரும்பு 0;
        }
        எனில்(length>Part::MAX_LENGTH) {
            setParseError(parseError, எண்_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
        parseDouble(எண்_Index, index, உண்மை, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        எனில்(index==செய்தி.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            திரும்பு 0;
        }
        char16_t c=செய்தி.charAt(index);
        எனில்(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            திரும்பு 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        எனில்(index==செய்தி.length()) {
            திரும்பு index;
        }
        எனில்(செய்தி.charAt(index)==u_வலது_Curly_Brace) {
            எனில்(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
            திரும்பு index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு 0;
    }
    int32_t start=index;
    UBool isEmpty=உண்மை;
    UBool hasOther=பொய்;
    க்கு(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==செய்தி.length();
        எனில்(eos || செய்தி.charAt(index)==u_வலது_Curly_Brace) {
            எனில்(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
            எனில்(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                திரும்பு 0;
            }
            திரும்பு index;
        }
        int32_t selectorIndex=index;
        எனில்(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && செய்தி.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            எனில்(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
            எனில்(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, பொய்,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } இல்லையெனில் {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            எனில்(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                திரும்பு 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            எனில்( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<செய்தி.length() &&
                0==செய்தி.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                எனில்(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    திரும்பு 0;
                }
                // allow whitespace between offset: and its value
                int32_t மதிப்பு_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(மதிப்பு_Index);
                எனில்(index==மதிப்பு_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    திரும்பு 0;
                }
                எனில்((index-மதிப்பு_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, மதிப்பு_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    திரும்பு 0;
                }
                parseDouble(மதிப்பு_Index, index, பொய்,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                எனில்(U_FAILURE(errorCode)) {
                    திரும்பு 0;
                }
                isEmpty=பொய்;
                தொடர்;  // no message fragment after the offset
            } இல்லையெனில் {
                // normal selector word
                எனில்(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    திரும்பு 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                எனில்(0==செய்தி.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=உண்மை;
                }
            }
        }
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        எனில்(index==செய்தி.length() || செய்தி.charAt(index)!=u_இடது_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            திரும்பு 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        எனில்(U_FAILURE(errorCode)) {
            திரும்பு 0;
        }
        isEmpty=பொய்;
    }
}

int32_t
MessagePattern::parse_Arg_எண்(மாறிலி UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    எனில்(start>=limit) {
        திரும்பு UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_எண்;
    char16_t c=s.charAt(start++);
    எனில்(c==0x30) {
        எனில்(start==limit) {
            திரும்பு 0;
        } இல்லையெனில் {
            number=0;
            bad_எண்=உண்மை;  // leading zero
        }
    } இல்லையெனில் எனில்(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_எண்=பொய்;
    } இல்லையெனில் {
        திரும்பு UMSGPAT_ARG_NAME_NOT_எண்;
    }
    போது(start<limit) {
        c=s.charAt(start++);
        எனில்(0x30<=c && c<=0x39) {
            எனில்(number>=INT32_MAX/10) {
                bad_எண்=உண்மை;  // overflow
            } இல்லையெனில் {
                number=number*10+(c-0x30);
            }
        } இல்லையெனில் {
            திரும்பு UMSGPAT_ARG_NAME_NOT_எண்;
        }
    }
    // There are only ASCII digits.
    எனில்(bad_எண்) {
        திரும்பு UMSGPAT_ARG_NAME_NOT_VALID;
    } இல்லையெனில் {
        திரும்பு number;
    }
}

வெறுமை
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    க்கு(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=செய்தி.charAt(index++);
        எனில்(c==u_minus) {
            isNegative=1;
            எனில்(index==limit) {
                நிறுத்து;  // no number
            }
            c=செய்தி.charAt(index++);
        } இல்லையெனில் எனில்(c==u_plus) {
            எனில்(index==limit) {
                நிறுத்து;  // no number
            }
            c=செய்தி.charAt(index++);
        }
        எனில்(c==0x221e) {  // infinity
            எனில்(allowInfinity && index==limit) {
                இரட்டை infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                திரும்பு;
            } இல்லையெனில் {
                நிறுத்து;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        போது('0'<=c && c<='9') {
            value=value*10+(c-'0');
            எனில்(value>(Part::MAX_மதிப்பு+isNegative)) {
                நிறுத்து;  // not a small-enough integer
            }
            எனில்(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                திரும்பு;
            }
            c=செய்தி.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        எழுத்து எண்_Chars[128];
        int32_t capacity = static_cast<int32_t>(அளவு(எண்_Chars));
        int32_t length=limit-start;
        எனில்(length>=capacity) {
            நிறுத்து;  // number too long
        }
        செய்தி.extract(start, length, எண்_Chars, capacity, US_INV);
        எனில் (static_cast<int32_t>(uprv_strlen(எண்_Chars)) < length) {
            நிறுத்து;  // contains non-invariant character that was turned into NUL
        }
        எழுத்து *end;
        இரட்டை numeric_மதிப்பு=uprv_strtod(எண்_Chars, &end);
        எனில்(end!=(எண்_Chars+length)) {
            நிறுத்து;  // parsing error
        }
        addArgDoublePart(numeric_மதிப்பு, start, length, errorCode);
        திரும்பு;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    மாறிலி char16_t *s=செய்தி.getBuffer();
    int32_t செய்தி_Length=செய்தி.length();
    மாறிலி char16_t *t=PatternProps::skipWhiteSpace(s+index, செய்தி_Length-index);
    திரும்பு static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    மாறிலி char16_t *s=செய்தி.getBuffer();
    int32_t செய்தி_Length=செய்தி.length();
    மாறிலி char16_t *t=PatternProps::skipIdentifier(s+index, செய்தி_Length-index);
    திரும்பு static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t செய்தி_Length=செய்தி.length();
    போது(index<செய்தி_Length) {
        char16_t c=செய்தி.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        எனில்((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            நிறுத்து;
        }
        ++index;
    }
    திரும்பு index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    திரும்பு (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    திரும்பு
        ((c=செய்தி.charAt(index++))==u_c || c==u_C) &&
        ((c=செய்தி.charAt(index++))==u_h || c==u_H) &&
        ((c=செய்தி.charAt(index++))==u_o || c==u_O) &&
        ((c=செய்தி.charAt(index++))==u_i || c==u_I) &&
        ((c=செய்தி.charAt(index++))==u_c || c==u_C) &&
        ((c=செய்தி.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    திரும்பு
        ((c=செய்தி.charAt(index++))==u_p || c==u_P) &&
        ((c=செய்தி.charAt(index++))==u_l || c==u_L) &&
        ((c=செய்தி.charAt(index++))==u_u || c==u_U) &&
        ((c=செய்தி.charAt(index++))==u_r || c==u_R) &&
        ((c=செய்தி.charAt(index++))==u_a || c==u_A) &&
        ((c=செய்தி.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    திரும்பு
        ((c=செய்தி.charAt(index++))==u_s || c==u_S) &&
        ((c=செய்தி.charAt(index++))==u_e || c==u_E) &&
        ((c=செய்தி.charAt(index++))==u_l || c==u_L) &&
        ((c=செய்தி.charAt(index++))==u_e || c==u_E) &&
        ((c=செய்தி.charAt(index++))==u_c || c==u_C) &&
        ((c=செய்தி.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    திரும்பு
        ((c=செய்தி.charAt(index++))==u_o || c==u_O) &&
        ((c=செய்தி.charAt(index++))==u_r || c==u_R) &&
        ((c=செய்தி.charAt(index++))==u_d || c==u_D) &&
        ((c=செய்தி.charAt(index++))==u_i || c==u_I) &&
        ((c=செய்தி.charAt(index++))==u_n || c==u_N) &&
        ((c=செய்தி.charAt(index++))==u_a || c==u_A) &&
        ((c=செய்தி.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    திரும்பு nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_செய்தி_START;
}

UBool
MessagePattern::in_உச்சி_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    திரும்பு
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_செய்தி_START;
}

வெறுமை
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    எனில்(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

வெறுமை
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

வெறுமை
MessagePattern::addArgDoublePart(இரட்டை numeric_மதிப்பு, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    int32_t numericIndex=numeric_மதிப்புகள்_Length;
    எனில்(numeric_மதிப்புகள்_List==nullptr) {
        numeric_மதிப்புகள்_List=புதிய MessagePatternDoubleList();
        எனில்(numeric_மதிப்புகள்_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு;
        }
    } இல்லையெனில் எனில்(!numeric_மதிப்புகள்_List->ensureCapacityForOneMore(numeric_மதிப்புகள்_Length, errorCode)) {
        திரும்பு;
    } இல்லையெனில் {
        எனில்(numericIndex>Part::MAX_மதிப்பு) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு;
        }
    }
    numeric_மதிப்புகள்_List->a[numeric_மதிப்புகள்_Length++]=numeric_மதிப்பு;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

வெறுமை
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    எனில்(parseError==nullptr) {
        திரும்பு;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    எனில்(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        எனில்(length>0 && U16_IS_TRAIL(செய்தி[index-length])) {
            --length;
        }
    }
    செய்தி.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=செய்தி.length()-index;
    எனில்(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        எனில்(length>0 && U16_IS_LEAD(செய்தி[index+length-1])) {
            --length;
        }
    }
    செய்தி.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

வெறுமை
MessageImpl::appendReducedApostrophes(மாறிலி UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    க்கு(;;) {
        int32_t i=s.indexOf(u_apos, start);
        எனில்(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            நிறுத்து;
        }
        எனில்(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } இல்லையெனில் {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(மாறிலி MessagePattern& செய்தி_Pattern,
                                               int32_t செய்தி_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    மாறிலி UnicodeString &செய்தி_String=செய்தி_Pattern.getPatternString();
    int32_t prevIndex=செய்தி_Pattern.getPart(செய்தி_Start).getLimit();
    க்கு(int32_t i=செய்தி_Start;;) {
        மாறிலி MessagePattern::Part &part=செய்தி_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        எனில்(type==UMSGPAT_PART_TYPE_செய்தி_LIMIT) {
            திரும்பு result.append(செய்தி_String, prevIndex, index-prevIndex);
        } இல்லையெனில் எனில்(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(செய்தி_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } இல்லையெனில் எனில்(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(செய்தி_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=செய்தி_Pattern.getLimitPartIndex(i);
            index=செய்தி_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(செய்தி_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
