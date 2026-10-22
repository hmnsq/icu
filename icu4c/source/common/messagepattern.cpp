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

#सचे !UCONFIG_NO_FORMATTING

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

स्थिर ध्रुव char16_t u_pound=0x23;
स्थिर ध्रुव char16_t u_apos=0x27;
स्थिर ध्रुव char16_t u_plus=0x2B;
स्थिर ध्रुव char16_t u_comma=0x2C;
स्थिर ध्रुव char16_t u_minus=0x2D;
स्थिर ध्रुव char16_t u_dot=0x2E;
स्थिर ध्रुव char16_t u_colon=0x3A;
स्थिर ध्रुव char16_t u_lessThan=0x3C;
स्थिर ध्रुव char16_t u_equal=0x3D;
स्थिर ध्रुव char16_t u_A=0x41;
स्थिर ध्रुव char16_t u_C=0x43;
स्थिर ध्रुव char16_t u_D=0x44;
स्थिर ध्रुव char16_t u_E=0x45;
स्थिर ध्रुव char16_t u_H=0x48;
स्थिर ध्रुव char16_t u_I=0x49;
स्थिर ध्रुव char16_t u_L=0x4C;
स्थिर ध्रुव char16_t u_N=0x4E;
स्थिर ध्रुव char16_t u_O=0x4F;
स्थिर ध्रुव char16_t u_P=0x50;
स्थिर ध्रुव char16_t u_R=0x52;
स्थिर ध्रुव char16_t u_S=0x53;
स्थिर ध्रुव char16_t u_T=0x54;
स्थिर ध्रुव char16_t u_U=0x55;
स्थिर ध्रुव char16_t u_Z=0x5A;
स्थिर ध्रुव char16_t u_a=0x61;
स्थिर ध्रुव char16_t u_c=0x63;
स्थिर ध्रुव char16_t u_d=0x64;
स्थिर ध्रुव char16_t u_e=0x65;
स्थिर ध्रुव char16_t u_f=0x66;
स्थिर ध्रुव char16_t u_h=0x68;
स्थिर ध्रुव char16_t u_i=0x69;
स्थिर ध्रुव char16_t u_l=0x6C;
स्थिर ध्रुव char16_t u_n=0x6E;
स्थिर ध्रुव char16_t u_o=0x6F;
स्थिर ध्रुव char16_t u_p=0x70;
स्थिर ध्रुव char16_t u_r=0x72;
स्थिर ध्रुव char16_t u_s=0x73;
स्थिर ध्रुव char16_t u_t=0x74;
स्थिर ध्रुव char16_t u_u=0x75;
स्थिर ध्रुव char16_t u_z=0x7A;
स्थिर ध्रुव char16_t u_वामः_Curly_Brace=0x7B;
स्थिर ध्रुव char16_t u_pipe=0x7C;
स्थिर ध्रुव char16_t u_दक्षिणः_Curly_Brace=0x7D;
स्थिर ध्रुव char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

स्थिर ध्रुव char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

स्थिर ध्रुव char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t राशिः_Capacity>
वग्ग MessagePatternList : सार्वजनिक UMemory {
सार्वजनिक:
    MessagePatternList() {}
    शून्य copyFrom(ध्रुव MessagePatternList<T, राशिः_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ध्रुव MessagePatternList<T, राशिः_Capacity> &other, int32_t length) ध्रुव {
        अत्थाय(int32_t i=0; i<length; ++i) {
            सचे(a[i]!=other.a[i]) { पटिनिवत्त मिच्छा; }
        }
        पटिनिवत्त सच्च;
    }

    Maybe_राशिः_Array<T, राशिः_Capacity> a;
};

template<typename T, int32_t राशिः_Capacity>
शून्य
MessagePatternList<T, राशिः_Capacity>::copyFrom(
        ध्रुव MessagePatternList<T, राशिः_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    सचे(U_SUCCESS(errorCode) && length>0) {
        सचे(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*परिमाण(T));
    }
}

template<typename T, int32_t राशिः_Capacity>
UBool
MessagePatternList<T, राशिः_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    सचे(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        पटिनिवत्त सच्च;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    पटिनिवत्त मिच्छा;
}

// MessagePatternList specializations -------------------------------------- ***

वग्ग MessagePatternDoubleList : सार्वजनिक MessagePatternList<द्विगुण, 8> {
};

वग्ग MessagePatternPartsList : सार्वजनिक MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानानि_List(nullptr), numeric_मानानि(nullptr), numeric_मानानि_Length(0),
          hasArgNames(मिच्छा), hasArgNumbers(मिच्छा), needsAutoQuoting(मिच्छा) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानानि_List(nullptr), numeric_मानानि(nullptr), numeric_मानानि_Length(0),
          hasArgNames(मिच्छा), hasArgNumbers(मिच्छा), needsAutoQuoting(मिच्छा) {
    init(errorCode);
}

MessagePattern::MessagePattern(ध्रुव UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानानि_List(nullptr), numeric_मानानि(nullptr), numeric_मानानि_Length(0),
          hasArgNames(मिच्छा), hasArgNumbers(मिच्छा), needsAutoQuoting(मिच्छा) {
    सचे(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    partsList=नूतन MessagePatternPartsList();
    सचे(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त मिच्छा;
    }
    parts=partsList->a.getAlias();
    पटिनिवत्त सच्च;
}

MessagePattern::MessagePattern(ध्रुव MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), सन्देशः(other.सन्देशः),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मानानि_List(nullptr), numeric_मानानि(nullptr), numeric_मानानि_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    सचे(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ध्रुव MessagePattern& other) U_LIFETIME_BOUND {
    सचे(एतत्==&other) {
        पटिनिवत्त *एतत्;
    }
    aposMode=other.aposMode;
    सन्देशः=other.सन्देशः;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    सचे(!copyStorage(other, errorCode)) {
        clear();
    }
    पटिनिवत्त *एतत्;
}

UBool
MessagePattern::copyStorage(ध्रुव MessagePattern &other, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त मिच्छा;
    }
    parts=nullptr;
    partsLength=0;
    numeric_मानानि=nullptr;
    numeric_मानानि_Length=0;
    सचे(partsList==nullptr) {
        partsList=नूतन MessagePatternPartsList();
        सचे(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त मिच्छा;
        }
        parts=partsList->a.getAlias();
    }
    सचे(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त मिच्छा;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    सचे(other.numeric_मानानि_Length>0) {
        सचे(numeric_मानानि_List==nullptr) {
            numeric_मानानि_List=नूतन MessagePatternDoubleList();
            सचे(numeric_मानानि_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त मिच्छा;
            }
            numeric_मानानि=numeric_मानानि_List->a.getAlias();
        }
        numeric_मानानि_List->copyFrom(
            *other.numeric_मानानि_List, other.numeric_मानानि_Length, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त मिच्छा;
        }
        numeric_मानानि=numeric_मानानि_List->a.getAlias();
        numeric_मानानि_Length=other.numeric_मानानि_Length;
    }
    पटिनिवत्त सच्च;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_मानानि_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ध्रुव UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    पटिनिवत्त *एतत्;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ध्रुव UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    पटिनिवत्त *एतत्;
}

MessagePattern&
MessagePattern::parsePluralStyle(ध्रुव UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    पटिनिवत्त *एतत्;
}

MessagePattern&
MessagePattern::parseSelectStyle(ध्रुव UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    पटिनिवत्त *एतत्;
}

शून्य
MessagePattern::clear() {
    // Mostly the same as preParse().
    सन्देशः.remove();
    hasArgNames=hasArgNumbers=मिच्छा;
    needsAutoQuoting=मिच्छा;
    partsLength=0;
    numeric_मानानि_Length=0;
}

तार्किक
MessagePattern::operator==(ध्रुव MessagePattern &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त
        aposMode==other.aposMode &&
        सन्देशः==other.सन्देशः &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ध्रुव {
    int32_t hash=(aposMode*37+सन्देशः.hashCode())*37+partsLength;
    अत्थाय(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    पटिनिवत्त hash;
}

int32_t
MessagePattern::validateArgumentName(ध्रुव UnicodeString &name) {
    सचे(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        पटिनिवत्त UMSGPAT_ARG_NAME_NOT_VALID;
    }
    पटिनिवत्त parse_Arg_सङ्ख्या(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ध्रुव {
    सचे(!needsAutoQuoting) {
        पटिनिवत्त सन्देशः;
    }
    UnicodeString modified(सन्देशः);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=गणनम्_Parts();
    अत्थाय(int32_t i=count; i>0;) {
        ध्रुव Part &part=getPart(--i);
        सचे(part.getType()==UMSGPAT_PART_TYPE_निवेशय_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    पटिनिवत्त modified;
}

द्विगुण
MessagePattern::get_Numeric_मानम्(ध्रुव Part &part) ध्रुव {
    UMessagePatternPartType type=part.type;
    सचे(type==UMSGPAT_PART_TYPE_ARG_INT) {
        पटिनिवत्त part.value;
    } अञ्ञथा सचे(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        पटिनिवत्त numeric_मानानि[part.value];
    } अञ्ञथा {
        पटिनिवत्त UMSGPAT_NO_NUMERIC_मानम्;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
द्विगुण
MessagePattern::getPluralOffset(int32_t pluralStart) ध्रुव {
    ध्रुव Part &part=getPart(pluralStart);
    सचे(Part::has_Numeric_मानम्(part.type)) {
        पटिनिवत्त get_Numeric_मानम्(part);
    } अञ्ञथा {
        पटिनिवत्त 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

तार्किक
MessagePattern::Part::operator==(ध्रुव Part &other) ध्रुव {
    सचे(एतत्==&other) {
        पटिनिवत्त सच्च;
    }
    पटिनिवत्त
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

शून्य
MessagePattern::preParse(ध्रुव UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    सचे(parseError!=nullptr) {
        parseError->पङ्क्तिः=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    सन्देशः=pattern;
    hasArgNames=hasArgNumbers=मिच्छा;
    needsAutoQuoting=मिच्छा;
    partsLength=0;
    numeric_मानानि_Length=0;
}

शून्य
MessagePattern::postParse() {
    सचे(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    सचे(numeric_मानानि_List!=nullptr) {
        numeric_मानानि=numeric_मानानि_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t सन्देशः_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    सचे(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        पटिनिवत्त 0;
    }
    int32_t सन्देशः_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_सन्देशः_START, index, सन्देशः_Start_Length, nestingLevel, errorCode);
    index+=सन्देशः_Start_Length;
    अत्थाय(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
        सचे(index>=सन्देशः.length()) {
            भङ्ग;
        }
        char16_t c=सन्देशः.charAt(index++);
        सचे(c==u_apos) {
            सचे(index==सन्देशः.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_निवेशय_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=सच्च;
            } अञ्ञथा {
                c=सन्देशः.charAt(index);
                सचे(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } अञ्ञथा सचे(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_वामः_Curly_Brace || c==u_दक्षिणः_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    अत्थाय(;;) {
                        index=सन्देशः.indexOf(u_apos, index+1);
                        सचे(index>=0) {
                            सचे(/*(index+1)<msg.length() &&*/ सन्देशः.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } अञ्ञथा {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                भङ्ग;
                            }
                        } अञ्ञथा {
                            // The quoted text reaches to the end of the of the message.
                            index=सन्देशः.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_निवेशय_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=सच्च;
                            भङ्ग;
                        }
                    }
                } अञ्ञथा {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_निवेशय_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=सच्च;
                }
            }
        } अञ्ञथा सचे(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_सङ्ख्या, index-1, 1, 0, errorCode);
        } अञ्ञथा सचे(c==u_वामः_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } अञ्ञथा सचे((nestingLevel>0 && c==u_दक्षिणः_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_दक्षिणः_Curly_Brace) ? 0 : 1;
            addLimitPart(सन्देशः_Start, UMSGPAT_PART_TYPE_सन्देशः_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            सचे(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                पटिनिवत्त index-1;
            } अञ्ञथा {
                // continue parsing after the '}'
                पटिनिवत्त index;
            }
        }  // else: c is part of literal text
    }
    सचे(nestingLevel>0 && !in_शिखरम्_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        पटिनिवत्त 0;
    }
    addLimitPart(सन्देशः_Start, UMSGPAT_PART_TYPE_सन्देशः_LIMIT, index, 0, nestingLevel, errorCode);
    पटिनिवत्त index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    सचे(index==सन्देशः.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        पटिनिवत्त 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_सङ्ख्या(nameIndex, index);
    सचे(number>=0) {
        int32_t length=index-nameIndex;
        सचे(length>Part::MAX_LENGTH || number>Part::MAX_मानम्) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }
        hasArgNumbers=सच्च;
        addPart(UMSGPAT_PART_TYPE_ARG_सङ्ख्या, nameIndex, length, number, errorCode);
    } अञ्ञथा सचे(number==UMSGPAT_ARG_NAME_NOT_सङ्ख्या) {
        int32_t length=index-nameIndex;
        सचे(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }
        hasArgNames=सच्च;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } अञ्ञथा {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        पटिनिवत्त 0;
    }
    index=skipWhiteSpace(index);
    सचे(index==सन्देशः.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        पटिनिवत्त 0;
    }
    char16_t c=सन्देशः.charAt(index);
    सचे(c==u_दक्षिणः_Curly_Brace) {
        // all done
    } अञ्ञथा सचे(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        पटिनिवत्त 0;
    } अञ्ञथा /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        याव(index<सन्देशः.length() && isArgTypeChar(सन्देशः.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        सचे(index==सन्देशः.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            पटिनिवत्त 0;
        }
        सचे(length==0 || ((c=सन्देशः.charAt(index))!=u_comma && c!=u_दक्षिणः_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            पटिनिवत्त 0;
        }
        सचे(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        सचे(length==6) {
            // case-insensitive comparisons for complex-type names
            सचे(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } अञ्ञथा सचे(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } अञ्ञथा सचे(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } अञ्ञथा सचे(length==13) {
            सचे(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        सचे(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        सचे(c==u_दक्षिणः_Curly_Brace) {
            सचे(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
        } अञ्ञथा /* ',' */ {
            ++index;
            सचे(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } अञ्ञथा सचे(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } अञ्ञथा {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    पटिनिवत्त index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    याव(index<सन्देशः.length()) {
        char16_t c=सन्देशः.charAt(index++);
        सचे(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=सन्देशः.indexOf(u_apos, index);
            सचे(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } अञ्ञथा सचे(c==u_वामः_Curly_Brace) {
            ++nestedBraces;
        } अञ्ञथा सचे(c==u_दक्षिणः_Curly_Brace) {
            सचे(nestedBraces>0) {
                --nestedBraces;
            } अञ्ञथा {
                int32_t length=--index-start;
                सचे(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    पटिनिवत्त 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                पटिनिवत्त index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    पटिनिवत्त 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    सचे(index==सन्देशः.length() || सन्देशः.charAt(index)==u_दक्षिणः_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        पटिनिवत्त 0;
    }
    अत्थाय(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t सङ्ख्या_Index=index;
        index=skipDouble(index);
        int32_t length=index-सङ्ख्या_Index;
        सचे(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            पटिनिवत्त 0;
        }
        सचे(length>Part::MAX_LENGTH) {
            setParseError(parseError, सङ्ख्या_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त 0;
        }
        parseDouble(सङ्ख्या_Index, index, सच्च, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        सचे(index==सन्देशः.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            पटिनिवत्त 0;
        }
        char16_t c=सन्देशः.charAt(index);
        सचे(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            पटिनिवत्त 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        सचे(index==सन्देशः.length()) {
            पटिनिवत्त index;
        }
        सचे(सन्देशः.charAt(index)==u_दक्षिणः_Curly_Brace) {
            सचे(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
            पटिनिवत्त index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त 0;
    }
    int32_t start=index;
    UBool isEmpty=सच्च;
    UBool hasOther=मिच्छा;
    अत्थाय(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==सन्देशः.length();
        सचे(eos || सन्देशः.charAt(index)==u_दक्षिणः_Curly_Brace) {
            सचे(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
            सचे(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                पटिनिवत्त 0;
            }
            पटिनिवत्त index;
        }
        int32_t selectorIndex=index;
        सचे(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && सन्देशः.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            सचे(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
            सचे(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                पटिनिवत्त 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, मिच्छा,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } अञ्ञथा {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            सचे(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                पटिनिवत्त 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            सचे( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<सन्देशः.length() &&
                0==सन्देशः.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                सचे(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    पटिनिवत्त 0;
                }
                // allow whitespace between offset: and its value
                int32_t मानम्_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(मानम्_Index);
                सचे(index==मानम्_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    पटिनिवत्त 0;
                }
                सचे((index-मानम्_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, मानम्_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    पटिनिवत्त 0;
                }
                parseDouble(मानम्_Index, index, मिच्छा,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                सचे(U_FAILURE(errorCode)) {
                    पटिनिवत्त 0;
                }
                isEmpty=मिच्छा;
                अनुवर्त;  // no message fragment after the offset
            } अञ्ञथा {
                // normal selector word
                सचे(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    पटिनिवत्त 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                सचे(0==सन्देशः.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=सच्च;
                }
            }
        }
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        सचे(index==सन्देशः.length() || सन्देशः.charAt(index)!=u_वामः_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            पटिनिवत्त 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        सचे(U_FAILURE(errorCode)) {
            पटिनिवत्त 0;
        }
        isEmpty=मिच्छा;
    }
}

int32_t
MessagePattern::parse_Arg_सङ्ख्या(ध्रुव UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    सचे(start>=limit) {
        पटिनिवत्त UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_सङ्ख्या;
    char16_t c=s.charAt(start++);
    सचे(c==0x30) {
        सचे(start==limit) {
            पटिनिवत्त 0;
        } अञ्ञथा {
            number=0;
            bad_सङ्ख्या=सच्च;  // leading zero
        }
    } अञ्ञथा सचे(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_सङ्ख्या=मिच्छा;
    } अञ्ञथा {
        पटिनिवत्त UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
    }
    याव(start<limit) {
        c=s.charAt(start++);
        सचे(0x30<=c && c<=0x39) {
            सचे(number>=INT32_MAX/10) {
                bad_सङ्ख्या=सच्च;  // overflow
            } अञ्ञथा {
                number=number*10+(c-0x30);
            }
        } अञ्ञथा {
            पटिनिवत्त UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
        }
    }
    // There are only ASCII digits.
    सचे(bad_सङ्ख्या) {
        पटिनिवत्त UMSGPAT_ARG_NAME_NOT_VALID;
    } अञ्ञथा {
        पटिनिवत्त number;
    }
}

शून्य
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    अत्थाय(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=सन्देशः.charAt(index++);
        सचे(c==u_minus) {
            isNegative=1;
            सचे(index==limit) {
                भङ्ग;  // no number
            }
            c=सन्देशः.charAt(index++);
        } अञ्ञथा सचे(c==u_plus) {
            सचे(index==limit) {
                भङ्ग;  // no number
            }
            c=सन्देशः.charAt(index++);
        }
        सचे(c==0x221e) {  // infinity
            सचे(allowInfinity && index==limit) {
                द्विगुण infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                पटिनिवत्त;
            } अञ्ञथा {
                भङ्ग;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        याव('0'<=c && c<='9') {
            value=value*10+(c-'0');
            सचे(value>(Part::MAX_मानम्+isNegative)) {
                भङ्ग;  // not a small-enough integer
            }
            सचे(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                पटिनिवत्त;
            }
            c=सन्देशः.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        वर्ण सङ्ख्या_Chars[128];
        int32_t capacity = static_cast<int32_t>(परिमाण(सङ्ख्या_Chars));
        int32_t length=limit-start;
        सचे(length>=capacity) {
            भङ्ग;  // number too long
        }
        सन्देशः.extract(start, length, सङ्ख्या_Chars, capacity, US_INV);
        सचे (static_cast<int32_t>(uprv_strlen(सङ्ख्या_Chars)) < length) {
            भङ्ग;  // contains non-invariant character that was turned into NUL
        }
        वर्ण *end;
        द्विगुण numeric_मानम्=uprv_strtod(सङ्ख्या_Chars, &end);
        सचे(end!=(सङ्ख्या_Chars+length)) {
            भङ्ग;  // parsing error
        }
        addArgDoublePart(numeric_मानम्, start, length, errorCode);
        पटिनिवत्त;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ध्रुव char16_t *s=सन्देशः.getBuffer();
    int32_t सन्देशः_Length=सन्देशः.length();
    ध्रुव char16_t *t=PatternProps::skipWhiteSpace(s+index, सन्देशः_Length-index);
    पटिनिवत्त static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ध्रुव char16_t *s=सन्देशः.getBuffer();
    int32_t सन्देशः_Length=सन्देशः.length();
    ध्रुव char16_t *t=PatternProps::skipIdentifier(s+index, सन्देशः_Length-index);
    पटिनिवत्त static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t सन्देशः_Length=सन्देशः.length();
    याव(index<सन्देशः_Length) {
        char16_t c=सन्देशः.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        सचे((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            भङ्ग;
        }
        ++index;
    }
    पटिनिवत्त index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    पटिनिवत्त (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    पटिनिवत्त
        ((c=सन्देशः.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(index++))==u_h || c==u_H) &&
        ((c=सन्देशः.charAt(index++))==u_o || c==u_O) &&
        ((c=सन्देशः.charAt(index++))==u_i || c==u_I) &&
        ((c=सन्देशः.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    पटिनिवत्त
        ((c=सन्देशः.charAt(index++))==u_p || c==u_P) &&
        ((c=सन्देशः.charAt(index++))==u_l || c==u_L) &&
        ((c=सन्देशः.charAt(index++))==u_u || c==u_U) &&
        ((c=सन्देशः.charAt(index++))==u_r || c==u_R) &&
        ((c=सन्देशः.charAt(index++))==u_a || c==u_A) &&
        ((c=सन्देशः.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    पटिनिवत्त
        ((c=सन्देशः.charAt(index++))==u_s || c==u_S) &&
        ((c=सन्देशः.charAt(index++))==u_e || c==u_E) &&
        ((c=सन्देशः.charAt(index++))==u_l || c==u_L) &&
        ((c=सन्देशः.charAt(index++))==u_e || c==u_E) &&
        ((c=सन्देशः.charAt(index++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    पटिनिवत्त
        ((c=सन्देशः.charAt(index++))==u_o || c==u_O) &&
        ((c=सन्देशः.charAt(index++))==u_r || c==u_R) &&
        ((c=सन्देशः.charAt(index++))==u_d || c==u_D) &&
        ((c=सन्देशः.charAt(index++))==u_i || c==u_I) &&
        ((c=सन्देशः.charAt(index++))==u_n || c==u_N) &&
        ((c=सन्देशः.charAt(index++))==u_a || c==u_A) &&
        ((c=सन्देशः.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    पटिनिवत्त nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_सन्देशः_START;
}

UBool
MessagePattern::in_शिखरम्_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    पटिनिवत्त
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_सन्देशः_START;
}

शून्य
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    सचे(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
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
MessagePattern::addArgDoublePart(द्विगुण numeric_मानम्, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    int32_t numericIndex=numeric_मानानि_Length;
    सचे(numeric_मानानि_List==nullptr) {
        numeric_मानानि_List=नूतन MessagePatternDoubleList();
        सचे(numeric_मानानि_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            पटिनिवत्त;
        }
    } अञ्ञथा सचे(!numeric_मानानि_List->ensureCapacityForOneMore(numeric_मानानि_Length, errorCode)) {
        पटिनिवत्त;
    } अञ्ञथा {
        सचे(numericIndex>Part::MAX_मानम्) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            पटिनिवत्त;
        }
    }
    numeric_मानानि_List->a[numeric_मानानि_Length++]=numeric_मानम्;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

शून्य
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    सचे(parseError==nullptr) {
        पटिनिवत्त;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    सचे(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        सचे(length>0 && U16_IS_TRAIL(सन्देशः[index-length])) {
            --length;
        }
    }
    सन्देशः.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=सन्देशः.length()-index;
    सचे(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        सचे(length>0 && U16_IS_LEAD(सन्देशः[index+length-1])) {
            --length;
        }
    }
    सन्देशः.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

शून्य
MessageImpl::appendReducedApostrophes(ध्रुव UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    अत्थाय(;;) {
        int32_t i=s.indexOf(u_apos, start);
        सचे(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            भङ्ग;
        }
        सचे(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } अञ्ञथा {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ध्रुव MessagePattern& सन्देशः_Pattern,
                                               int32_t सन्देशः_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ध्रुव UnicodeString &सन्देशः_String=सन्देशः_Pattern.getPatternString();
    int32_t prevIndex=सन्देशः_Pattern.getPart(सन्देशः_Start).getLimit();
    अत्थाय(int32_t i=सन्देशः_Start;;) {
        ध्रुव MessagePattern::Part &part=सन्देशः_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        सचे(type==UMSGPAT_PART_TYPE_सन्देशः_LIMIT) {
            पटिनिवत्त result.append(सन्देशः_String, prevIndex, index-prevIndex);
        } अञ्ञथा सचे(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(सन्देशः_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } अञ्ञथा सचे(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(सन्देशः_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=सन्देशः_Pattern.getLimitPartIndex(i);
            index=सन्देशः_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(सन्देशः_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
