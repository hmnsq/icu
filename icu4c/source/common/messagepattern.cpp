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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_FORMATTING

#अन्तर्भाव "unicode/messagepattern.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "messageimpl.h"
#अन्तर्भाव "patternprops.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

स्थिर अचर char16_t u_pound=0x23;
स्थिर अचर char16_t u_apos=0x27;
स्थिर अचर char16_t u_plus=0x2B;
स्थिर अचर char16_t u_comma=0x2C;
स्थिर अचर char16_t u_minus=0x2D;
स्थिर अचर char16_t u_dot=0x2E;
स्थिर अचर char16_t u_colon=0x3A;
स्थिर अचर char16_t u_lessThan=0x3C;
स्थिर अचर char16_t u_equal=0x3D;
स्थिर अचर char16_t u_A=0x41;
स्थिर अचर char16_t u_C=0x43;
स्थिर अचर char16_t u_D=0x44;
स्थिर अचर char16_t u_E=0x45;
स्थिर अचर char16_t u_H=0x48;
स्थिर अचर char16_t u_I=0x49;
स्थिर अचर char16_t u_L=0x4C;
स्थिर अचर char16_t u_N=0x4E;
स्थिर अचर char16_t u_O=0x4F;
स्थिर अचर char16_t u_P=0x50;
स्थिर अचर char16_t u_R=0x52;
स्थिर अचर char16_t u_S=0x53;
स्थिर अचर char16_t u_T=0x54;
स्थिर अचर char16_t u_U=0x55;
स्थिर अचर char16_t u_Z=0x5A;
स्थिर अचर char16_t u_a=0x61;
स्थिर अचर char16_t u_c=0x63;
स्थिर अचर char16_t u_d=0x64;
स्थिर अचर char16_t u_e=0x65;
स्थिर अचर char16_t u_f=0x66;
स्थिर अचर char16_t u_h=0x68;
स्थिर अचर char16_t u_i=0x69;
स्थिर अचर char16_t u_l=0x6C;
स्थिर अचर char16_t u_n=0x6E;
स्थिर अचर char16_t u_o=0x6F;
स्थिर अचर char16_t u_p=0x70;
स्थिर अचर char16_t u_r=0x72;
स्थिर अचर char16_t u_s=0x73;
स्थिर अचर char16_t u_t=0x74;
स्थिर अचर char16_t u_u=0x75;
स्थिर अचर char16_t u_z=0x7A;
स्थिर अचर char16_t u_बायाँ_Curly_Brace=0x7B;
स्थिर अचर char16_t u_pipe=0x7C;
स्थिर अचर char16_t u_दायाँ_Curly_Brace=0x7D;
स्थिर अचर char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

स्थिर अचर char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

स्थिर अचर char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

ढाँचा<typename T, int32_t थाक_Capacity>
वर्ग MessagePatternList : सार्वजनिक UMemory {
सार्वजनिक:
    MessagePatternList() {}
    शून्य copyFrom(अचर MessagePatternList<T, थाक_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(अचर MessagePatternList<T, थाक_Capacity> &other, int32_t length) अचर {
        लागि(int32_t i=0; i<length; ++i) {
            यदि(a[i]!=other.a[i]) { फर्काऊ असत्य; }
        }
        फर्काऊ सत्य;
    }

    Maybe_थाक_Array<T, थाक_Capacity> a;
};

ढाँचा<typename T, int32_t थाक_Capacity>
शून्य
MessagePatternList<T, थाक_Capacity>::copyFrom(
        अचर MessagePatternList<T, थाक_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    यदि(U_SUCCESS(errorCode) && length>0) {
        यदि(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (माप_प्रकार)length*परिमाण(T));
    }
}

ढाँचा<typename T, int32_t थाक_Capacity>
UBool
MessagePatternList<T, थाक_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    यदि(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        फर्काऊ सत्य;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    फर्काऊ असत्य;
}

// MessagePatternList specializations -------------------------------------- ***

वर्ग MessagePatternDoubleList : सार्वजनिक MessagePatternList<द्विगुण, 8> {
};

वर्ग MessagePatternPartsList : सार्वजनिक MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानहरू_List(nullptr), numeric_मानहरू(nullptr), numeric_मानहरू_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानहरू_List(nullptr), numeric_मानहरू(nullptr), numeric_मानहरू_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(errorCode);
}

MessagePattern::MessagePattern(अचर UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानहरू_List(nullptr), numeric_मानहरू(nullptr), numeric_मानहरू_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    यदि(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    partsList=नूतन MessagePatternPartsList();
    यदि(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ असत्य;
    }
    parts=partsList->a.getAlias();
    फर्काऊ सत्य;
}

MessagePattern::MessagePattern(अचर MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), सन्देश(other.सन्देश),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानहरू_List(nullptr), numeric_मानहरू(nullptr), numeric_मानहरू_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    यदि(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::चालक=(अचर MessagePattern& other) U_LIFETIME_BOUND {
    यदि(एतत्==&other) {
        फर्काऊ *एतत्;
    }
    aposMode=other.aposMode;
    सन्देश=other.सन्देश;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    यदि(!copyStorage(other, errorCode)) {
        clear();
    }
    फर्काऊ *एतत्;
}

UBool
MessagePattern::copyStorage(अचर MessagePattern &other, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ असत्य;
    }
    parts=nullptr;
    partsLength=0;
    numeric_मानहरू=nullptr;
    numeric_मानहरू_Length=0;
    यदि(partsList==nullptr) {
        partsList=नूतन MessagePatternPartsList();
        यदि(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ असत्य;
        }
        parts=partsList->a.getAlias();
    }
    यदि(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ असत्य;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    यदि(other.numeric_मानहरू_Length>0) {
        यदि(numeric_मानहरू_List==nullptr) {
            numeric_मानहरू_List=नूतन MessagePatternDoubleList();
            यदि(numeric_मानहरू_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ असत्य;
            }
            numeric_मानहरू=numeric_मानहरू_List->a.getAlias();
        }
        numeric_मानहरू_List->copyFrom(
            *other.numeric_मानहरू_List, other.numeric_मानहरू_Length, errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ असत्य;
        }
        numeric_मानहरू=numeric_मानहरू_List->a.getAlias();
        numeric_मानहरू_Length=other.numeric_मानहरू_Length;
    }
    फर्काऊ सत्य;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_मानहरू_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(अचर UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    फर्काऊ *एतत्;
}

MessagePattern&
MessagePattern::parseChoiceStyle(अचर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    फर्काऊ *एतत्;
}

MessagePattern&
MessagePattern::parsePluralStyle(अचर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    फर्काऊ *एतत्;
}

MessagePattern&
MessagePattern::parseSelectStyle(अचर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    फर्काऊ *एतत्;
}

शून्य
MessagePattern::clear() {
    // Mostly the same as preParse().
    सन्देश.हटाओ();
    hasArgNames=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    partsLength=0;
    numeric_मानहरू_Length=0;
}

तार्किक
MessagePattern::चालक==(अचर MessagePattern &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    फर्काऊ
        aposMode==other.aposMode &&
        सन्देश==other.सन्देश &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() अचर {
    int32_t hash=(aposMode*37+सन्देश.hashCode())*37+partsLength;
    लागि(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    फर्काऊ hash;
}

int32_t
MessagePattern::validateArgumentName(अचर UnicodeString &name) {
    यदि(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        फर्काऊ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    फर्काऊ parse_Arg_सङ्ख्या(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() अचर {
    यदि(!needsAutoQuoting) {
        फर्काऊ सन्देश;
    }
    UnicodeString modified(सन्देश);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=गन्ती_Parts();
    लागि(int32_t i=count; i>0;) {
        अचर Part &part=getPart(--i);
        यदि(part.getType()==UMSGPAT_PART_TYPE_घुसाउ_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    फर्काऊ modified;
}

द्विगुण
MessagePattern::get_Numeric_मान(अचर Part &part) अचर {
    UMessagePatternPartType type=part.type;
    यदि(type==UMSGPAT_PART_TYPE_ARG_INT) {
        फर्काऊ part.value;
    } अन्यथा यदि(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        फर्काऊ numeric_मानहरू[part.value];
    } अन्यथा {
        फर्काऊ UMSGPAT_NO_NUMERIC_मान;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
द्विगुण
MessagePattern::getPluralOffset(int32_t pluralStart) अचर {
    अचर Part &part=getPart(pluralStart);
    यदि(Part::has_Numeric_मान(part.type)) {
        फर्काऊ get_Numeric_मान(part);
    } अन्यथा {
        फर्काऊ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

तार्किक
MessagePattern::Part::चालक==(अचर Part &other) अचर {
    यदि(एतत्==&other) {
        फर्काऊ सत्य;
    }
    फर्काऊ
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

शून्य
MessagePattern::preParse(अचर UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    यदि(parseError!=nullptr) {
        parseError->पङ्क्ति=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    सन्देश=pattern;
    hasArgNames=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    partsLength=0;
    numeric_मानहरू_Length=0;
}

शून्य
MessagePattern::postParse() {
    यदि(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    यदि(numeric_मानहरू_List!=nullptr) {
        numeric_मानहरू=numeric_मानहरू_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t सन्देश_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ 0;
    }
    यदि(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        फर्काऊ 0;
    }
    int32_t सन्देश_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_सन्देश_START, index, सन्देश_Start_Length, nestingLevel, errorCode);
    index+=सन्देश_Start_Length;
    लागि(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }
        यदि(index>=सन्देश.length()) {
            रोक;
        }
        char16_t c=सन्देश.charAt(index++);
        यदि(c==u_apos) {
            यदि(index==सन्देश.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_घुसाउ_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=सत्य;
            } अन्यथा {
                c=सन्देश.charAt(index);
                यदि(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } अन्यथा यदि(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_बायाँ_Curly_Brace || c==u_दायाँ_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    लागि(;;) {
                        index=सन्देश.indexOf(u_apos, index+1);
                        यदि(index>=0) {
                            यदि(/*(index+1)<msg.length() &&*/ सन्देश.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } अन्यथा {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                रोक;
                            }
                        } अन्यथा {
                            // The quoted text reaches to the end of the of the message.
                            index=सन्देश.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_घुसाउ_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=सत्य;
                            रोक;
                        }
                    }
                } अन्यथा {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_घुसाउ_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=सत्य;
                }
            }
        } अन्यथा यदि(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_सङ्ख्या, index-1, 1, 0, errorCode);
        } अन्यथा यदि(c==u_बायाँ_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } अन्यथा यदि((nestingLevel>0 && c==u_दायाँ_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_दायाँ_Curly_Brace) ? 0 : 1;
            addLimitPart(सन्देश_Start, UMSGPAT_PART_TYPE_सन्देश_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            यदि(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                फर्काऊ index-1;
            } अन्यथा {
                // continue parsing after the '}'
                फर्काऊ index;
            }
        }  // else: c is part of literal text
    }
    यदि(nestingLevel>0 && !in_शीर्ष_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        फर्काऊ 0;
    }
    addLimitPart(सन्देश_Start, UMSGPAT_PART_TYPE_सन्देश_LIMIT, index, 0, nestingLevel, errorCode);
    फर्काऊ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    यदि(index==सन्देश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        फर्काऊ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_सङ्ख्या(nameIndex, index);
    यदि(number>=0) {
        int32_t length=index-nameIndex;
        यदि(length>Part::MAX_LENGTH || number>Part::MAX_मान) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }
        hasArgNumbers=सत्य;
        addPart(UMSGPAT_PART_TYPE_ARG_सङ्ख्या, nameIndex, length, number, errorCode);
    } अन्यथा यदि(number==UMSGPAT_ARG_NAME_NOT_सङ्ख्या) {
        int32_t length=index-nameIndex;
        यदि(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }
        hasArgNames=सत्य;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } अन्यथा {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        फर्काऊ 0;
    }
    index=skipWhiteSpace(index);
    यदि(index==सन्देश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        फर्काऊ 0;
    }
    char16_t c=सन्देश.charAt(index);
    यदि(c==u_दायाँ_Curly_Brace) {
        // all done
    } अन्यथा यदि(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        फर्काऊ 0;
    } अन्यथा /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        जबसम्म(index<सन्देश.length() && isArgTypeChar(सन्देश.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        यदि(index==सन्देश.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            फर्काऊ 0;
        }
        यदि(length==0 || ((c=सन्देश.charAt(index))!=u_comma && c!=u_दायाँ_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            फर्काऊ 0;
        }
        यदि(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        यदि(length==6) {
            // case-insensitive comparisons for complex-type names
            यदि(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } अन्यथा यदि(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } अन्यथा यदि(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } अन्यथा यदि(length==13) {
            यदि(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        यदि(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        यदि(c==u_दायाँ_Curly_Brace) {
            यदि(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
        } अन्यथा /* ',' */ {
            ++index;
            यदि(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } अन्यथा यदि(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } अन्यथा {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    फर्काऊ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    जबसम्म(index<सन्देश.length()) {
        char16_t c=सन्देश.charAt(index++);
        यदि(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=सन्देश.indexOf(u_apos, index);
            यदि(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } अन्यथा यदि(c==u_बायाँ_Curly_Brace) {
            ++nestedBraces;
        } अन्यथा यदि(c==u_दायाँ_Curly_Brace) {
            यदि(nestedBraces>0) {
                --nestedBraces;
            } अन्यथा {
                int32_t length=--index-start;
                यदि(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    फर्काऊ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                फर्काऊ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    फर्काऊ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    यदि(index==सन्देश.length() || सन्देश.charAt(index)==u_दायाँ_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        फर्काऊ 0;
    }
    लागि(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t सङ्ख्या_Index=index;
        index=skipDouble(index);
        int32_t length=index-सङ्ख्या_Index;
        यदि(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            फर्काऊ 0;
        }
        यदि(length>Part::MAX_LENGTH) {
            setParseError(parseError, सङ्ख्या_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ 0;
        }
        parseDouble(सङ्ख्या_Index, index, सत्य, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        यदि(index==सन्देश.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            फर्काऊ 0;
        }
        char16_t c=सन्देश.charAt(index);
        यदि(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            फर्काऊ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        यदि(index==सन्देश.length()) {
            फर्काऊ index;
        }
        यदि(सन्देश.charAt(index)==u_दायाँ_Curly_Brace) {
            यदि(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
            फर्काऊ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ 0;
    }
    int32_t start=index;
    UBool isEmpty=सत्य;
    UBool hasOther=असत्य;
    लागि(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==सन्देश.length();
        यदि(eos || सन्देश.charAt(index)==u_दायाँ_Curly_Brace) {
            यदि(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
            यदि(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                फर्काऊ 0;
            }
            फर्काऊ index;
        }
        int32_t selectorIndex=index;
        यदि(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && सन्देश.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            यदि(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
            यदि(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                फर्काऊ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, असत्य,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } अन्यथा {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            यदि(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                फर्काऊ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            यदि( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<सन्देश.length() &&
                0==सन्देश.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                यदि(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    फर्काऊ 0;
                }
                // allow whitespace between offset: and its value
                int32_t मान_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(मान_Index);
                यदि(index==मान_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    फर्काऊ 0;
                }
                यदि((index-मान_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, मान_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    फर्काऊ 0;
                }
                parseDouble(मान_Index, index, असत्य,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                यदि(U_FAILURE(errorCode)) {
                    फर्काऊ 0;
                }
                isEmpty=असत्य;
                जारी;  // no message fragment after the offset
            } अन्यथा {
                // normal selector word
                यदि(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    फर्काऊ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                यदि(0==सन्देश.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=सत्य;
                }
            }
        }
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        यदि(index==सन्देश.length() || सन्देश.charAt(index)!=u_बायाँ_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            फर्काऊ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        यदि(U_FAILURE(errorCode)) {
            फर्काऊ 0;
        }
        isEmpty=असत्य;
    }
}

int32_t
MessagePattern::parse_Arg_सङ्ख्या(अचर UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    यदि(start>=limit) {
        फर्काऊ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_सङ्ख्या;
    char16_t c=s.charAt(start++);
    यदि(c==0x30) {
        यदि(start==limit) {
            फर्काऊ 0;
        } अन्यथा {
            number=0;
            bad_सङ्ख्या=सत्य;  // leading zero
        }
    } अन्यथा यदि(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_सङ्ख्या=असत्य;
    } अन्यथा {
        फर्काऊ UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
    }
    जबसम्म(start<limit) {
        c=s.charAt(start++);
        यदि(0x30<=c && c<=0x39) {
            यदि(number>=INT32_MAX/10) {
                bad_सङ्ख्या=सत्य;  // overflow
            } अन्यथा {
                number=number*10+(c-0x30);
            }
        } अन्यथा {
            फर्काऊ UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
        }
    }
    // There are only ASCII digits.
    यदि(bad_सङ्ख्या) {
        फर्काऊ UMSGPAT_ARG_NAME_NOT_VALID;
    } अन्यथा {
        फर्काऊ number;
    }
}

शून्य
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    लागि(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=सन्देश.charAt(index++);
        यदि(c==u_minus) {
            isNegative=1;
            यदि(index==limit) {
                रोक;  // no number
            }
            c=सन्देश.charAt(index++);
        } अन्यथा यदि(c==u_plus) {
            यदि(index==limit) {
                रोक;  // no number
            }
            c=सन्देश.charAt(index++);
        }
        यदि(c==0x221e) {  // infinity
            यदि(allowInfinity && index==limit) {
                द्विगुण infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                फर्काऊ;
            } अन्यथा {
                रोक;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        जबसम्म('0'<=c && c<='9') {
            value=value*10+(c-'0');
            यदि(value>(Part::MAX_मान+isNegative)) {
                रोक;  // not a small-enough integer
            }
            यदि(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                फर्काऊ;
            }
            c=सन्देश.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        वर्ण सङ्ख्या_Chars[128];
        int32_t capacity = static_cast<int32_t>(परिमाण(सङ्ख्या_Chars));
        int32_t length=limit-start;
        यदि(length>=capacity) {
            रोक;  // number too long
        }
        सन्देश.extract(start, length, सङ्ख्या_Chars, capacity, US_INV);
        यदि (static_cast<int32_t>(uprv_strlen(सङ्ख्या_Chars)) < length) {
            रोक;  // contains non-invariant character that was turned into NUL
        }
        वर्ण *end;
        द्विगुण numeric_मान=uprv_strtod(सङ्ख्या_Chars, &end);
        यदि(end!=(सङ्ख्या_Chars+length)) {
            रोक;  // parsing error
        }
        addArgDoublePart(numeric_मान, start, length, errorCode);
        फर्काऊ;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    अचर char16_t *s=सन्देश.getBuffer();
    int32_t सन्देश_Length=सन्देश.length();
    अचर char16_t *t=PatternProps::skipWhiteSpace(s+index, सन्देश_Length-index);
    फर्काऊ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    अचर char16_t *s=सन्देश.getBuffer();
    int32_t सन्देश_Length=सन्देश.length();
    अचर char16_t *t=PatternProps::skipIdentifier(s+index, सन्देश_Length-index);
    फर्काऊ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t सन्देश_Length=सन्देश.length();
    जबसम्म(index<सन्देश_Length) {
        char16_t c=सन्देश.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        यदि((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            रोक;
        }
        ++index;
    }
    फर्काऊ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    फर्काऊ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    फर्काऊ
        ((c=सन्देश.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देश.charAt(index++))==u_h || c==u_H) &&
        ((c=सन्देश.charAt(index++))==u_o || c==u_O) &&
        ((c=सन्देश.charAt(index++))==u_i || c==u_I) &&
        ((c=सन्देश.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देश.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    फर्काऊ
        ((c=सन्देश.charAt(index++))==u_p || c==u_P) &&
        ((c=सन्देश.charAt(index++))==u_l || c==u_L) &&
        ((c=सन्देश.charAt(index++))==u_u || c==u_U) &&
        ((c=सन्देश.charAt(index++))==u_r || c==u_R) &&
        ((c=सन्देश.charAt(index++))==u_a || c==u_A) &&
        ((c=सन्देश.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    फर्काऊ
        ((c=सन्देश.charAt(index++))==u_s || c==u_S) &&
        ((c=सन्देश.charAt(index++))==u_e || c==u_E) &&
        ((c=सन्देश.charAt(index++))==u_l || c==u_L) &&
        ((c=सन्देश.charAt(index++))==u_e || c==u_E) &&
        ((c=सन्देश.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देश.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    फर्काऊ
        ((c=सन्देश.charAt(index++))==u_o || c==u_O) &&
        ((c=सन्देश.charAt(index++))==u_r || c==u_R) &&
        ((c=सन्देश.charAt(index++))==u_d || c==u_D) &&
        ((c=सन्देश.charAt(index++))==u_i || c==u_I) &&
        ((c=सन्देश.charAt(index++))==u_n || c==u_N) &&
        ((c=सन्देश.charAt(index++))==u_a || c==u_A) &&
        ((c=सन्देश.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    फर्काऊ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_सन्देश_START;
}

UBool
MessagePattern::in_शीर्ष_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    फर्काऊ
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_सन्देश_START;
}

शून्य
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    यदि(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

शून्य
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

शून्य
MessagePattern::addArgDoublePart(द्विगुण numeric_मान, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    int32_t numericIndex=numeric_मानहरू_Length;
    यदि(numeric_मानहरू_List==nullptr) {
        numeric_मानहरू_List=नूतन MessagePatternDoubleList();
        यदि(numeric_मानहरू_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            फर्काऊ;
        }
    } अन्यथा यदि(!numeric_मानहरू_List->ensureCapacityForOneMore(numeric_मानहरू_Length, errorCode)) {
        फर्काऊ;
    } अन्यथा {
        यदि(numericIndex>Part::MAX_मान) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            फर्काऊ;
        }
    }
    numeric_मानहरू_List->a[numeric_मानहरू_Length++]=numeric_मान;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

शून्य
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    यदि(parseError==nullptr) {
        फर्काऊ;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    यदि(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        यदि(length>0 && U16_IS_TRAIL(सन्देश[index-length])) {
            --length;
        }
    }
    सन्देश.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=सन्देश.length()-index;
    यदि(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        यदि(length>0 && U16_IS_LEAD(सन्देश[index+length-1])) {
            --length;
        }
    }
    सन्देश.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

शून्य
MessageImpl::appendReducedApostrophes(अचर UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    लागि(;;) {
        int32_t i=s.indexOf(u_apos, start);
        यदि(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            रोक;
        }
        यदि(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } अन्यथा {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(अचर MessagePattern& सन्देश_Pattern,
                                               int32_t सन्देश_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    अचर UnicodeString &सन्देश_String=सन्देश_Pattern.getPatternString();
    int32_t prevIndex=सन्देश_Pattern.getPart(सन्देश_Start).getLimit();
    लागि(int32_t i=सन्देश_Start;;) {
        अचर MessagePattern::Part &part=सन्देश_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        यदि(type==UMSGPAT_PART_TYPE_सन्देश_LIMIT) {
            फर्काऊ result.append(सन्देश_String, prevIndex, index-prevIndex);
        } अन्यथा यदि(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(सन्देश_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } अन्यथा यदि(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(सन्देश_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=सन्देश_Pattern.getLimitPartIndex(i);
            index=सन्देश_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(सन्देश_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
