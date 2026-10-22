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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_FORMATTING

#ضمن "unicode/messagepattern.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf16.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "messageimpl.h"
#ضمن "patternprops.h"
#ضمن "putilimp.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ساكن ثابت char16_t u_pound=0x23;
ساكن ثابت char16_t u_apos=0x27;
ساكن ثابت char16_t u_plus=0x2B;
ساكن ثابت char16_t u_comma=0x2C;
ساكن ثابت char16_t u_minus=0x2D;
ساكن ثابت char16_t u_dot=0x2E;
ساكن ثابت char16_t u_colon=0x3A;
ساكن ثابت char16_t u_lessThan=0x3C;
ساكن ثابت char16_t u_equal=0x3D;
ساكن ثابت char16_t u_A=0x41;
ساكن ثابت char16_t u_C=0x43;
ساكن ثابت char16_t u_D=0x44;
ساكن ثابت char16_t u_E=0x45;
ساكن ثابت char16_t u_H=0x48;
ساكن ثابت char16_t u_I=0x49;
ساكن ثابت char16_t u_L=0x4C;
ساكن ثابت char16_t u_N=0x4E;
ساكن ثابت char16_t u_O=0x4F;
ساكن ثابت char16_t u_P=0x50;
ساكن ثابت char16_t u_R=0x52;
ساكن ثابت char16_t u_S=0x53;
ساكن ثابت char16_t u_T=0x54;
ساكن ثابت char16_t u_U=0x55;
ساكن ثابت char16_t u_Z=0x5A;
ساكن ثابت char16_t u_a=0x61;
ساكن ثابت char16_t u_c=0x63;
ساكن ثابت char16_t u_d=0x64;
ساكن ثابت char16_t u_e=0x65;
ساكن ثابت char16_t u_f=0x66;
ساكن ثابت char16_t u_h=0x68;
ساكن ثابت char16_t u_i=0x69;
ساكن ثابت char16_t u_l=0x6C;
ساكن ثابت char16_t u_n=0x6E;
ساكن ثابت char16_t u_o=0x6F;
ساكن ثابت char16_t u_p=0x70;
ساكن ثابت char16_t u_r=0x72;
ساكن ثابت char16_t u_s=0x73;
ساكن ثابت char16_t u_t=0x74;
ساكن ثابت char16_t u_u=0x75;
ساكن ثابت char16_t u_z=0x7A;
ساكن ثابت char16_t u_leftCurlyBrace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_rightCurlyBrace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t stackCapacity>
صنف MessagePatternList : عام UMemory {
عام:
    MessagePatternList() {}
    فراغ copyFrom(ثابت MessagePatternList<T, stackCapacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ثابت MessagePatternList<T, stackCapacity> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            ܐܢ(a[i]!=other.a[i]) { ܦܢܐ ܕܓܠܐ; }
        }
        ܦܢܐ ܫܪܪܐ;
    }

    MaybeStackArray<T, stackCapacity> a;
};

template<typename T, int32_t stackCapacity>
فراغ
MessagePatternList<T, stackCapacity>::copyFrom(
        ثابت MessagePatternList<T, stackCapacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    ܐܢ(U_SUCCESS(errorCode) && length>0) {
        ܐܢ(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

template<typename T, int32_t stackCapacity>
UBool
MessagePatternList<T, stackCapacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܐܢ(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        ܦܢܐ ܫܪܪܐ;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    ܦܢܐ ܕܓܠܐ;
}

// MessagePatternList specializations -------------------------------------- ***

صنف MessagePatternDoubleList : عام MessagePatternList<مزدوج, 8> {
};

صنف MessagePatternPartsList : عام MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(ܕܓܠܐ), hasArgNumbers(ܕܓܠܐ), needsAutoQuoting(ܕܓܠܐ) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(ܕܓܠܐ), hasArgNumbers(ܕܓܠܐ), needsAutoQuoting(ܕܓܠܐ) {
    init(errorCode);
}

MessagePattern::MessagePattern(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(ܕܓܠܐ), hasArgNumbers(ܕܓܠܐ), needsAutoQuoting(ܕܓܠܐ) {
    ܐܢ(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    partsList=جديد MessagePatternPartsList();
    ܐܢ(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ ܕܓܠܐ;
    }
    parts=partsList->a.getAlias();
    ܦܢܐ ܫܪܪܐ;
}

MessagePattern::MessagePattern(ثابت MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), msg(other.msg),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ܐܢ(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ثابت MessagePattern& other) U_LIFETIME_BOUND {
    ܐܢ(هذا==&other) {
        ܦܢܐ *هذا;
    }
    aposMode=other.aposMode;
    msg=other.msg;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    ܐܢ(!copyStorage(other, errorCode)) {
        clear();
    }
    ܦܢܐ *هذا;
}

UBool
MessagePattern::copyStorage(ثابت MessagePattern &other, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ ܕܓܠܐ;
    }
    parts=nullptr;
    partsLength=0;
    numericValues=nullptr;
    numericValuesLength=0;
    ܐܢ(partsList==nullptr) {
        partsList=جديد MessagePatternPartsList();
        ܐܢ(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ ܕܓܠܐ;
        }
        parts=partsList->a.getAlias();
    }
    ܐܢ(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ ܕܓܠܐ;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    ܐܢ(other.numericValuesLength>0) {
        ܐܢ(numericValuesList==nullptr) {
            numericValuesList=جديد MessagePatternDoubleList();
            ܐܢ(numericValuesList==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ܦܢܐ ܕܓܠܐ;
            }
            numericValues=numericValuesList->a.getAlias();
        }
        numericValuesList->copyFrom(
            *other.numericValuesList, other.numericValuesLength, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ ܕܓܠܐ;
        }
        numericValues=numericValuesList->a.getAlias();
        numericValuesLength=other.numericValuesLength;
    }
    ܦܢܐ ܫܪܪܐ;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numericValuesList;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ثابت UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    ܦܢܐ *هذا;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    ܦܢܐ *هذا;
}

MessagePattern&
MessagePattern::parsePluralStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    ܦܢܐ *هذا;
}

MessagePattern&
MessagePattern::parseSelectStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    ܦܢܐ *هذا;
}

فراغ
MessagePattern::clear() {
    // Mostly the same as preParse().
    msg.remove();
    hasArgNames=hasArgNumbers=ܕܓܠܐ;
    needsAutoQuoting=ܕܓܠܐ;
    partsLength=0;
    numericValuesLength=0;
}

منطقي
MessagePattern::operator==(ثابت MessagePattern &other) ثابت {
    ܐܢ(هذا==&other) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܦܢܐ
        aposMode==other.aposMode &&
        msg==other.msg &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ثابت {
    int32_t hash=(aposMode*37+msg.hashCode())*37+partsLength;
    لكل(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    ܦܢܐ hash;
}

int32_t
MessagePattern::validateArgumentName(ثابت UnicodeString &name) {
    ܐܢ(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        ܦܢܐ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ܦܢܐ parseArgNumber(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ثابت {
    ܐܢ(!needsAutoQuoting) {
        ܦܢܐ msg;
    }
    UnicodeString modified(msg);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=countParts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        ܐܢ(part.getType()==UMSGPAT_PART_TYPE_INSERT_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    ܦܢܐ modified;
}

مزدوج
MessagePattern::getNumericValue(ثابت Part &part) ثابت {
    UMessagePatternPartType type=part.type;
    ܐܢ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ܦܢܐ part.value;
    } ܐܚܪܢܐ ܐܢ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ܦܢܐ numericValues[part.value];
    } ܐܚܪܢܐ {
        ܦܢܐ UMSGPAT_NO_NUMERIC_VALUE;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
مزدوج
MessagePattern::getPluralOffset(int32_t pluralStart) ثابت {
    ثابت Part &part=getPart(pluralStart);
    ܐܢ(Part::hasNumericValue(part.type)) {
        ܦܢܐ getNumericValue(part);
    } ܐܚܪܢܐ {
        ܦܢܐ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
MessagePattern::Part::operator==(ثابت Part &other) ثابت {
    ܐܢ(هذا==&other) {
        ܦܢܐ ܫܪܪܐ;
    }
    ܦܢܐ
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

فراغ
MessagePattern::preParse(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ܐܢ(parseError!=nullptr) {
        parseError->line=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    msg=pattern;
    hasArgNames=hasArgNumbers=ܕܓܠܐ;
    needsAutoQuoting=ܕܓܠܐ;
    partsLength=0;
    numericValuesLength=0;
}

فراغ
MessagePattern::postParse() {
    ܐܢ(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    ܐܢ(numericValuesList!=nullptr) {
        numericValues=numericValuesList->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t msgStartLength,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    ܐܢ(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ܦܢܐ 0;
    }
    int32_t msgStart=partsLength;
    addPart(UMSGPAT_PART_TYPE_MSG_START, index, msgStartLength, nestingLevel, errorCode);
    index+=msgStartLength;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ 0;
        }
        ܐܢ(index>=msg.length()) {
            اكسر;
        }
        char16_t c=msg.charAt(index++);
        ܐܢ(c==u_apos) {
            ܐܢ(index==msg.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=ܫܪܪܐ;
            } ܐܚܪܢܐ {
                c=msg.charAt(index);
                ܐܢ(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } ܐܚܪܢܐ ܐܢ(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_leftCurlyBrace || c==u_rightCurlyBrace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        index=msg.indexOf(u_apos, index+1);
                        ܐܢ(index>=0) {
                            ܐܢ(/*(index+1)<msg.length() &&*/ msg.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } ܐܚܪܢܐ {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                اكسر;
                            }
                        } ܐܚܪܢܐ {
                            // The quoted text reaches to the end of the of the message.
                            index=msg.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=ܫܪܪܐ;
                            اكسر;
                        }
                    }
                } ܐܚܪܢܐ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=ܫܪܪܐ;
                }
            }
        } ܐܚܪܢܐ ܐܢ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_NUMBER, index-1, 1, 0, errorCode);
        } ܐܚܪܢܐ ܐܢ(c==u_leftCurlyBrace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } ܐܚܪܢܐ ܐܢ((nestingLevel>0 && c==u_rightCurlyBrace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_rightCurlyBrace) ? 0 : 1;
            addLimitPart(msgStart, UMSGPAT_PART_TYPE_MSG_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            ܐܢ(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ܦܢܐ index-1;
            } ܐܚܪܢܐ {
                // continue parsing after the '}'
                ܦܢܐ index;
            }
        }  // else: c is part of literal text
    }
    ܐܢ(nestingLevel>0 && !inTopLevelChoiceMessage(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ܦܢܐ 0;
    }
    addLimitPart(msgStart, UMSGPAT_PART_TYPE_MSG_LIMIT, index, 0, nestingLevel, errorCode);
    ܦܢܐ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    ܐܢ(index==msg.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ܦܢܐ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parseArgNumber(nameIndex, index);
    ܐܢ(number>=0) {
        int32_t length=index-nameIndex;
        ܐܢ(length>Part::MAX_LENGTH || number>Part::MAX_VALUE) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }
        hasArgNumbers=ܫܪܪܐ;
        addPart(UMSGPAT_PART_TYPE_ARG_NUMBER, nameIndex, length, number, errorCode);
    } ܐܚܪܢܐ ܐܢ(number==UMSGPAT_ARG_NAME_NOT_NUMBER) {
        int32_t length=index-nameIndex;
        ܐܢ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }
        hasArgNames=ܫܪܪܐ;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } ܐܚܪܢܐ {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ܦܢܐ 0;
    }
    index=skipWhiteSpace(index);
    ܐܢ(index==msg.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ܦܢܐ 0;
    }
    char16_t c=msg.charAt(index);
    ܐܢ(c==u_rightCurlyBrace) {
        // all done
    } ܐܚܪܢܐ ܐܢ(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ܦܢܐ 0;
    } ܐܚܪܢܐ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        ܟܡܐ(index<msg.length() && isArgTypeChar(msg.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        ܐܢ(index==msg.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            ܦܢܐ 0;
        }
        ܐܢ(length==0 || ((c=msg.charAt(index))!=u_comma && c!=u_rightCurlyBrace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ܦܢܐ 0;
        }
        ܐܢ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ܐܢ(length==6) {
            // case-insensitive comparisons for complex-type names
            ܐܢ(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } ܐܚܪܢܐ ܐܢ(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } ܐܚܪܢܐ ܐܢ(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } ܐܚܪܢܐ ܐܢ(length==13) {
            ܐܢ(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        ܐܢ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        ܐܢ(c==u_rightCurlyBrace) {
            ܐܢ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
        } ܐܚܪܢܐ /* ',' */ {
            ++index;
            ܐܢ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } ܐܚܪܢܐ ܐܢ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } ܐܚܪܢܐ {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    ܦܢܐ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    ܟܡܐ(index<msg.length()) {
        char16_t c=msg.charAt(index++);
        ܐܢ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=msg.indexOf(u_apos, index);
            ܐܢ(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } ܐܚܪܢܐ ܐܢ(c==u_leftCurlyBrace) {
            ++nestedBraces;
        } ܐܚܪܢܐ ܐܢ(c==u_rightCurlyBrace) {
            ܐܢ(nestedBraces>0) {
                --nestedBraces;
            } ܐܚܪܢܐ {
                int32_t length=--index-start;
                ܐܢ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ܦܢܐ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                ܦܢܐ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    ܦܢܐ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    ܐܢ(index==msg.length() || msg.charAt(index)==u_rightCurlyBrace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ܦܢܐ 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t numberIndex=index;
        index=skipDouble(index);
        int32_t length=index-numberIndex;
        ܐܢ(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ܦܢܐ 0;
        }
        ܐܢ(length>Part::MAX_LENGTH) {
            setParseError(parseError, numberIndex);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ 0;
        }
        parseDouble(numberIndex, index, ܫܪܪܐ, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        ܐܢ(index==msg.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ܦܢܐ 0;
        }
        char16_t c=msg.charAt(index);
        ܐܢ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ܦܢܐ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ܐܢ(index==msg.length()) {
            ܦܢܐ index;
        }
        ܐܢ(msg.charAt(index)==u_rightCurlyBrace) {
            ܐܢ(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
            ܦܢܐ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ 0;
    }
    int32_t start=index;
    UBool isEmpty=ܫܪܪܐ;
    UBool hasOther=ܕܓܠܐ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==msg.length();
        ܐܢ(eos || msg.charAt(index)==u_rightCurlyBrace) {
            ܐܢ(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
            ܐܢ(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                ܦܢܐ 0;
            }
            ܦܢܐ index;
        }
        int32_t selectorIndex=index;
        ܐܢ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && msg.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            ܐܢ(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
            ܐܢ(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ܦܢܐ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, ܕܓܠܐ,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } ܐܚܪܢܐ {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            ܐܢ(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ܦܢܐ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ܐܢ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<msg.length() &&
                0==msg.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                ܐܢ(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ܦܢܐ 0;
                }
                // allow whitespace between offset: and its value
                int32_t valueIndex=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(valueIndex);
                ܐܢ(index==valueIndex) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ܦܢܐ 0;
                }
                ܐܢ((index-valueIndex)>Part::MAX_LENGTH) {
                    setParseError(parseError, valueIndex);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ܦܢܐ 0;
                }
                parseDouble(valueIndex, index, ܕܓܠܐ,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                ܐܢ(U_FAILURE(errorCode)) {
                    ܦܢܐ 0;
                }
                isEmpty=ܕܓܠܐ;
                استمر;  // no message fragment after the offset
            } ܐܚܪܢܐ {
                // normal selector word
                ܐܢ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ܦܢܐ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                ܐܢ(0==msg.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=ܫܪܪܐ;
                }
            }
        }
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        ܐܢ(index==msg.length() || msg.charAt(index)!=u_leftCurlyBrace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ܦܢܐ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        ܐܢ(U_FAILURE(errorCode)) {
            ܦܢܐ 0;
        }
        isEmpty=ܕܓܠܐ;
    }
}

int32_t
MessagePattern::parseArgNumber(ثابت UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ܐܢ(start>=limit) {
        ܦܢܐ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool badNumber;
    char16_t c=s.charAt(start++);
    ܐܢ(c==0x30) {
        ܐܢ(start==limit) {
            ܦܢܐ 0;
        } ܐܚܪܢܐ {
            number=0;
            badNumber=ܫܪܪܐ;  // leading zero
        }
    } ܐܚܪܢܐ ܐܢ(0x31<=c && c<=0x39) {
        number=c-0x30;
        badNumber=ܕܓܠܐ;
    } ܐܚܪܢܐ {
        ܦܢܐ UMSGPAT_ARG_NAME_NOT_NUMBER;
    }
    ܟܡܐ(start<limit) {
        c=s.charAt(start++);
        ܐܢ(0x30<=c && c<=0x39) {
            ܐܢ(number>=INT32_MAX/10) {
                badNumber=ܫܪܪܐ;  // overflow
            } ܐܚܪܢܐ {
                number=number*10+(c-0x30);
            }
        } ܐܚܪܢܐ {
            ܦܢܐ UMSGPAT_ARG_NAME_NOT_NUMBER;
        }
    }
    // There are only ASCII digits.
    ܐܢ(badNumber) {
        ܦܢܐ UMSGPAT_ARG_NAME_NOT_VALID;
    } ܐܚܪܢܐ {
        ܦܢܐ number;
    }
}

فراغ
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=msg.charAt(index++);
        ܐܢ(c==u_minus) {
            isNegative=1;
            ܐܢ(index==limit) {
                اكسر;  // no number
            }
            c=msg.charAt(index++);
        } ܐܚܪܢܐ ܐܢ(c==u_plus) {
            ܐܢ(index==limit) {
                اكسر;  // no number
            }
            c=msg.charAt(index++);
        }
        ܐܢ(c==0x221e) {  // infinity
            ܐܢ(allowInfinity && index==limit) {
                مزدوج infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                ܦܢܐ;
            } ܐܚܪܢܐ {
                اكسر;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ܟܡܐ('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ܐܢ(value>(Part::MAX_VALUE+isNegative)) {
                اكسر;  // not a small-enough integer
            }
            ܐܢ(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                ܦܢܐ;
            }
            c=msg.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف numberChars[128];
        int32_t capacity = static_cast<int32_t>(حجم(numberChars));
        int32_t length=limit-start;
        ܐܢ(length>=capacity) {
            اكسر;  // number too long
        }
        msg.extract(start, length, numberChars, capacity, US_INV);
        ܐܢ (static_cast<int32_t>(uprv_strlen(numberChars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *end;
        مزدوج numericValue=uprv_strtod(numberChars, &end);
        ܐܢ(end!=(numberChars+length)) {
            اكسر;  // parsing error
        }
        addArgDoublePart(numericValue, start, length, errorCode);
        ܦܢܐ;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ثابت char16_t *s=msg.getBuffer();
    int32_t msgLength=msg.length();
    ثابت char16_t *t=PatternProps::skipWhiteSpace(s+index, msgLength-index);
    ܦܢܐ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ثابت char16_t *s=msg.getBuffer();
    int32_t msgLength=msg.length();
    ثابت char16_t *t=PatternProps::skipIdentifier(s+index, msgLength-index);
    ܦܢܐ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t msgLength=msg.length();
    ܟܡܐ(index<msgLength) {
        char16_t c=msg.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ܐܢ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++index;
    }
    ܦܢܐ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    ܦܢܐ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    ܦܢܐ
        ((c=msg.charAt(index++))==u_c || c==u_C) &&
        ((c=msg.charAt(index++))==u_h || c==u_H) &&
        ((c=msg.charAt(index++))==u_o || c==u_O) &&
        ((c=msg.charAt(index++))==u_i || c==u_I) &&
        ((c=msg.charAt(index++))==u_c || c==u_C) &&
        ((c=msg.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    ܦܢܐ
        ((c=msg.charAt(index++))==u_p || c==u_P) &&
        ((c=msg.charAt(index++))==u_l || c==u_L) &&
        ((c=msg.charAt(index++))==u_u || c==u_U) &&
        ((c=msg.charAt(index++))==u_r || c==u_R) &&
        ((c=msg.charAt(index++))==u_a || c==u_A) &&
        ((c=msg.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    ܦܢܐ
        ((c=msg.charAt(index++))==u_s || c==u_S) &&
        ((c=msg.charAt(index++))==u_e || c==u_E) &&
        ((c=msg.charAt(index++))==u_l || c==u_L) &&
        ((c=msg.charAt(index++))==u_e || c==u_E) &&
        ((c=msg.charAt(index++))==u_c || c==u_C) &&
        ((c=msg.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    ܦܢܐ
        ((c=msg.charAt(index++))==u_o || c==u_O) &&
        ((c=msg.charAt(index++))==u_r || c==u_R) &&
        ((c=msg.charAt(index++))==u_d || c==u_D) &&
        ((c=msg.charAt(index++))==u_i || c==u_I) &&
        ((c=msg.charAt(index++))==u_n || c==u_N) &&
        ((c=msg.charAt(index++))==u_a || c==u_A) &&
        ((c=msg.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    ܦܢܐ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_MSG_START;
}

UBool
MessagePattern::inTopLevelChoiceMessage(int32_t nestingLevel, UMessagePatternArgType parentType) {
    ܦܢܐ
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_MSG_START;
}

فراغ
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    ܐܢ(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

فراغ
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

فراغ
MessagePattern::addArgDoublePart(مزدوج numericValue, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    int32_t numericIndex=numericValuesLength;
    ܐܢ(numericValuesList==nullptr) {
        numericValuesList=جديد MessagePatternDoubleList();
        ܐܢ(numericValuesList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ܦܢܐ;
        }
    } ܐܚܪܢܐ ܐܢ(!numericValuesList->ensureCapacityForOneMore(numericValuesLength, errorCode)) {
        ܦܢܐ;
    } ܐܚܪܢܐ {
        ܐܢ(numericIndex>Part::MAX_VALUE) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ܦܢܐ;
        }
    }
    numericValuesList->a[numericValuesLength++]=numericValue;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

فراغ
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    ܐܢ(parseError==nullptr) {
        ܦܢܐ;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    ܐܢ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ܐܢ(length>0 && U16_IS_TRAIL(msg[index-length])) {
            --length;
        }
    }
    msg.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=msg.length()-index;
    ܐܢ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ܐܢ(length>0 && U16_IS_LEAD(msg[index+length-1])) {
            --length;
        }
    }
    msg.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
MessageImpl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.indexOf(u_apos, start);
        ܐܢ(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            اكسر;
        }
        ܐܢ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } ܐܚܪܢܐ {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ثابت MessagePattern& msgPattern,
                                               int32_t msgStart,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ثابت UnicodeString &msgString=msgPattern.getPatternString();
    int32_t prevIndex=msgPattern.getPart(msgStart).getLimit();
    لكل(int32_t i=msgStart;;) {
        ثابت MessagePattern::Part &part=msgPattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        ܐܢ(type==UMSGPAT_PART_TYPE_MSG_LIMIT) {
            ܦܢܐ result.append(msgString, prevIndex, index-prevIndex);
        } ܐܚܪܢܐ ܐܢ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(msgString, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } ܐܚܪܢܐ ܐܢ(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(msgString, prevIndex, index-prevIndex);
            prevIndex=index;
            i=msgPattern.getLimitPartIndex(i);
            index=msgPattern.getPart(i).getLimit();
            appendReducedApostrophes(msgString, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
