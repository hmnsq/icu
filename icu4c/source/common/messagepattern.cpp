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

#include "unicode/utypes.h"

#ប្រសិនបើ !UCONFIG_NO_FORMATTING

#include "unicode/messagepattern.h"
#include "unicode/unistr.h"
#include "unicode/utf16.h"
#include "cmemory.h"
#include "cstring.h"
#include "messageimpl.h"
#include "patternprops.h"
#include "putilimp.h"
#include "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

static const char16_t u_pound=0x23;
static const char16_t u_apos=0x27;
static const char16_t u_plus=0x2B;
static const char16_t u_comma=0x2C;
static const char16_t u_minus=0x2D;
static const char16_t u_dot=0x2E;
static const char16_t u_colon=0x3A;
static const char16_t u_lessThan=0x3C;
static const char16_t u_equal=0x3D;
static const char16_t u_A=0x41;
static const char16_t u_C=0x43;
static const char16_t u_D=0x44;
static const char16_t u_E=0x45;
static const char16_t u_H=0x48;
static const char16_t u_I=0x49;
static const char16_t u_L=0x4C;
static const char16_t u_N=0x4E;
static const char16_t u_O=0x4F;
static const char16_t u_P=0x50;
static const char16_t u_R=0x52;
static const char16_t u_S=0x53;
static const char16_t u_T=0x54;
static const char16_t u_U=0x55;
static const char16_t u_Z=0x5A;
static const char16_t u_a=0x61;
static const char16_t u_c=0x63;
static const char16_t u_d=0x64;
static const char16_t u_e=0x65;
static const char16_t u_f=0x66;
static const char16_t u_h=0x68;
static const char16_t u_i=0x69;
static const char16_t u_l=0x6C;
static const char16_t u_n=0x6E;
static const char16_t u_o=0x6F;
static const char16_t u_p=0x70;
static const char16_t u_r=0x72;
static const char16_t u_s=0x73;
static const char16_t u_t=0x74;
static const char16_t u_u=0x75;
static const char16_t u_z=0x7A;
static const char16_t u_leftCurlyBrace=0x7B;
static const char16_t u_pipe=0x7C;
static const char16_t u_rightCurlyBrace=0x7D;
static const char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

static const char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

static const char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t stackCapacity>
class MessagePatternList : public UMemory {
public:
    MessagePatternList() {}
    void copyFrom(const MessagePatternList<T, stackCapacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(const MessagePatternList<T, stackCapacity> &other, int32_t length) const {
        សម្រាប់(int32_t i=0; i<length; ++i) {
            ប្រសិនបើ(a[i]!=other.a[i]) { ត្រឡប់ false; }
        }
        ត្រឡប់ true;
    }

    MaybeStackArray<T, stackCapacity> a;
};

template<typename T, int32_t stackCapacity>
void
MessagePatternList<T, stackCapacity>::copyFrom(
        const MessagePatternList<T, stackCapacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    ប្រសិនបើ(U_SUCCESS(errorCode) && length>0) {
        ប្រសិនបើ(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*parimaa_na(T));
    }
}

template<typename T, int32_t stackCapacity>
UBool
MessagePatternList<T, stackCapacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ false;
    }
    ប្រសិនបើ(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        ត្រឡប់ true;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    ត្រឡប់ false;
}

// MessagePatternList specializations -------------------------------------- ***

class MessagePatternDoubleList : public MessagePatternList<dviguu_na, 8> {
};

class MessagePatternPartsList : public MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    init(errorCode);
}

MessagePattern::MessagePattern(const UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    ប្រសិនបើ(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ false;
    }
    partsList=new MessagePatternPartsList();
    ប្រសិនបើ(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ត្រឡប់ false;
    }
    parts=partsList->a.getAlias();
    ត្រឡប់ true;
}

MessagePattern::MessagePattern(const MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), msg(other.msg),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numericValuesList(nullptr), numericValues(nullptr), numericValuesLength(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    ប្រសិនបើ(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(const MessagePattern& other) U_LIFETIME_BOUND {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ *this;
    }
    aposMode=other.aposMode;
    msg=other.msg;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    ប្រសិនបើ(!copyStorage(other, errorCode)) {
        clear();
    }
    ត្រឡប់ *this;
}

UBool
MessagePattern::copyStorage(const MessagePattern &other, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ false;
    }
    parts=nullptr;
    partsLength=0;
    numericValues=nullptr;
    numericValuesLength=0;
    ប្រសិនបើ(partsList==nullptr) {
        partsList=new MessagePatternPartsList();
        ប្រសិនបើ(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់ false;
        }
        parts=partsList->a.getAlias();
    }
    ប្រសិនបើ(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ false;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    ប្រសិនបើ(other.numericValuesLength>0) {
        ប្រសិនបើ(numericValuesList==nullptr) {
            numericValuesList=new MessagePatternDoubleList();
            ប្រសិនបើ(numericValuesList==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ត្រឡប់ false;
            }
            numericValues=numericValuesList->a.getAlias();
        }
        numericValuesList->copyFrom(
            *other.numericValuesList, other.numericValuesLength, errorCode);
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ false;
        }
        numericValues=numericValuesList->a.getAlias();
        numericValuesLength=other.numericValuesLength;
    }
    ត្រឡប់ true;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numericValuesList;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(const UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    ត្រឡប់ *this;
}

MessagePattern&
MessagePattern::parseChoiceStyle(const UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    ត្រឡប់ *this;
}

MessagePattern&
MessagePattern::parsePluralStyle(const UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    ត្រឡប់ *this;
}

MessagePattern&
MessagePattern::parseSelectStyle(const UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    ត្រឡប់ *this;
}

void
MessagePattern::clear() {
    // Mostly the same as preParse().
    msg.remove();
    hasArgNames=hasArgNumbers=false;
    needsAutoQuoting=false;
    partsLength=0;
    numericValuesLength=0;
}

bool
MessagePattern::operator==(const MessagePattern &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ត្រឡប់
        aposMode==other.aposMode &&
        msg==other.msg &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() const {
    int32_t hash=(aposMode*37+msg.hashCode())*37+partsLength;
    សម្រាប់(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    ត្រឡប់ hash;
}

int32_t
MessagePattern::validateArgumentName(const UnicodeString &name) {
    ប្រសិនបើ(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        ត្រឡប់ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ត្រឡប់ parseArgNumber(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() const {
    ប្រសិនបើ(!needsAutoQuoting) {
        ត្រឡប់ msg;
    }
    UnicodeString modified(msg);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=countParts();
    សម្រាប់(int32_t i=count; i>0;) {
        const Part &part=getPart(--i);
        ប្រសិនបើ(part.getType()==UMSGPAT_PART_TYPE_INSERT_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    ត្រឡប់ modified;
}

dviguu_na
MessagePattern::getNumericValue(const Part &part) const {
    UMessagePatternPartType type=part.type;
    ប្រសិនបើ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ត្រឡប់ part.value;
    } បើមិនដូច្នេះ ប្រសិនបើ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ត្រឡប់ numericValues[part.value];
    } បើមិនដូច្នេះ {
        ត្រឡប់ UMSGPAT_NO_NUMERIC_VALUE;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
dviguu_na
MessagePattern::getPluralOffset(int32_t pluralStart) const {
    const Part &part=getPart(pluralStart);
    ប្រសិនបើ(Part::hasNumericValue(part.type)) {
        ត្រឡប់ getNumericValue(part);
    } បើមិនដូច្នេះ {
        ត្រឡប់ 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

bool
MessagePattern::Part::operator==(const Part &other) const {
    ប្រសិនបើ(this==&other) {
        ត្រឡប់ true;
    }
    ត្រឡប់
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

void
MessagePattern::preParse(const UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    ប្រសិនបើ(parseError!=nullptr) {
        parseError->line=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    msg=pattern;
    hasArgNames=hasArgNumbers=false;
    needsAutoQuoting=false;
    partsLength=0;
    numericValuesLength=0;
}

void
MessagePattern::postParse() {
    ប្រសិនបើ(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    ប្រសិនបើ(numericValuesList!=nullptr) {
        numericValues=numericValuesList->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t msgStartLength,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    ប្រសិនបើ(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        ត្រឡប់ 0;
    }
    int32_t msgStart=partsLength;
    addPart(UMSGPAT_PART_TYPE_MSG_START, index, msgStartLength, nestingLevel, errorCode);
    index+=msgStartLength;
    សម្រាប់(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ 0;
        }
        ប្រសិនបើ(index>=msg.length()) {
            bha_nga;
        }
        char16_t c=msg.charAt(index++);
        ប្រសិនបើ(c==u_apos) {
            ប្រសិនបើ(index==msg.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=true;
            } បើមិនដូច្នេះ {
                c=msg.charAt(index);
                ប្រសិនបើ(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } បើមិនដូច្នេះ ប្រសិនបើ(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_leftCurlyBrace || c==u_rightCurlyBrace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    សម្រាប់(;;) {
                        index=msg.indexOf(u_apos, index+1);
                        ប្រសិនបើ(index>=0) {
                            ប្រសិនបើ(/*(index+1)<msg.length() &&*/ msg.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } បើមិនដូច្នេះ {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                bha_nga;
                            }
                        } បើមិនដូច្នេះ {
                            // The quoted text reaches to the end of the of the message.
                            index=msg.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=true;
                            bha_nga;
                        }
                    }
                } បើមិនដូច្នេះ {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_INSERT_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=true;
                }
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_NUMBER, index-1, 1, 0, errorCode);
        } បើមិនដូច្នេះ ប្រសិនបើ(c==u_leftCurlyBrace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } បើមិនដូច្នេះ ប្រសិនបើ((nestingLevel>0 && c==u_rightCurlyBrace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_rightCurlyBrace) ? 0 : 1;
            addLimitPart(msgStart, UMSGPAT_PART_TYPE_MSG_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            ប្រសិនបើ(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ត្រឡប់ index-1;
            } បើមិនដូច្នេះ {
                // continue parsing after the '}'
                ត្រឡប់ index;
            }
        }  // else: c is part of literal text
    }
    ប្រសិនបើ(nestingLevel>0 && !inTopLevelChoiceMessage(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ត្រឡប់ 0;
    }
    addLimitPart(msgStart, UMSGPAT_PART_TYPE_MSG_LIMIT, index, 0, nestingLevel, errorCode);
    ត្រឡប់ index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    ប្រសិនបើ(index==msg.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ត្រឡប់ 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parseArgNumber(nameIndex, index);
    ប្រសិនបើ(number>=0) {
        int32_t length=index-nameIndex;
        ប្រសិនបើ(length>Part::MAX_LENGTH || number>Part::MAX_VALUE) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់ 0;
        }
        hasArgNumbers=true;
        addPart(UMSGPAT_PART_TYPE_ARG_NUMBER, nameIndex, length, number, errorCode);
    } បើមិនដូច្នេះ ប្រសិនបើ(number==UMSGPAT_ARG_NAME_NOT_NUMBER) {
        int32_t length=index-nameIndex;
        ប្រសិនបើ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់ 0;
        }
        hasArgNames=true;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } បើមិនដូច្នេះ {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ត្រឡប់ 0;
    }
    index=skipWhiteSpace(index);
    ប្រសិនបើ(index==msg.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ត្រឡប់ 0;
    }
    char16_t c=msg.charAt(index);
    ប្រសិនបើ(c==u_rightCurlyBrace) {
        // all done
    } បើមិនដូច្នេះ ប្រសិនបើ(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ត្រឡប់ 0;
    } បើមិនដូច្នេះ /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        ខណៈពេល(index<msg.length() && isArgTypeChar(msg.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        ប្រសិនបើ(index==msg.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            ត្រឡប់ 0;
        }
        ប្រសិនបើ(length==0 || ((c=msg.charAt(index))!=u_comma && c!=u_rightCurlyBrace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ត្រឡប់ 0;
        }
        ប្រសិនបើ(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់ 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ប្រសិនបើ(length==6) {
            // case-insensitive comparisons for complex-type names
            ប្រសិនបើ(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } បើមិនដូច្នេះ ប្រសិនបើ(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } បើមិនដូច្នេះ ប្រសិនបើ(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } បើមិនដូច្នេះ ប្រសិនបើ(length==13) {
            ប្រសិនបើ(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        ប្រសិនបើ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        ប្រសិនបើ(c==u_rightCurlyBrace) {
            ប្រសិនបើ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
        } បើមិនដូច្នេះ /* ',' */ {
            ++index;
            ប្រសិនបើ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } បើមិនដូច្នេះ ប្រសិនបើ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } បើមិនដូច្នេះ {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    ត្រឡប់ index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    ខណៈពេល(index<msg.length()) {
        char16_t c=msg.charAt(index++);
        ប្រសិនបើ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=msg.indexOf(u_apos, index);
            ប្រសិនបើ(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } បើមិនដូច្នេះ ប្រសិនបើ(c==u_leftCurlyBrace) {
            ++nestedBraces;
        } បើមិនដូច្នេះ ប្រសិនបើ(c==u_rightCurlyBrace) {
            ប្រសិនបើ(nestedBraces>0) {
                --nestedBraces;
            } បើមិនដូច្នេះ {
                int32_t length=--index-start;
                ប្រសិនបើ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ត្រឡប់ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                ត្រឡប់ index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    ត្រឡប់ 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    ប្រសិនបើ(index==msg.length() || msg.charAt(index)==u_rightCurlyBrace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ត្រឡប់ 0;
    }
    សម្រាប់(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t numberIndex=index;
        index=skipDouble(index);
        int32_t length=index-numberIndex;
        ប្រសិនបើ(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ត្រឡប់ 0;
        }
        ប្រសិនបើ(length>Part::MAX_LENGTH) {
            setParseError(parseError, numberIndex);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់ 0;
        }
        parseDouble(numberIndex, index, true, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        ប្រសិនបើ(index==msg.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ត្រឡប់ 0;
        }
        char16_t c=msg.charAt(index);
        ប្រសិនបើ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ត្រឡប់ 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ប្រសិនបើ(index==msg.length()) {
            ត្រឡប់ index;
        }
        ប្រសិនបើ(msg.charAt(index)==u_rightCurlyBrace) {
            ប្រសិនបើ(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
            ត្រឡប់ index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់ 0;
    }
    int32_t start=index;
    UBool isEmpty=true;
    UBool hasOther=false;
    សម្រាប់(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==msg.length();
        ប្រសិនបើ(eos || msg.charAt(index)==u_rightCurlyBrace) {
            ប្រសិនបើ(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
            ប្រសិនបើ(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                ត្រឡប់ 0;
            }
            ត្រឡប់ index;
        }
        int32_t selectorIndex=index;
        ប្រសិនបើ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && msg.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            ប្រសិនបើ(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
            ប្រសិនបើ(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ត្រឡប់ 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, false,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } បើមិនដូច្នេះ {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            ប្រសិនបើ(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ត្រឡប់ 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ប្រសិនបើ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<msg.length() &&
                0==msg.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                ប្រសិនបើ(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ត្រឡប់ 0;
                }
                // allow whitespace between offset: and its value
                int32_t valueIndex=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(valueIndex);
                ប្រសិនបើ(index==valueIndex) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ត្រឡប់ 0;
                }
                ប្រសិនបើ((index-valueIndex)>Part::MAX_LENGTH) {
                    setParseError(parseError, valueIndex);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ត្រឡប់ 0;
                }
                parseDouble(valueIndex, index, false,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                ប្រសិនបើ(U_FAILURE(errorCode)) {
                    ត្រឡប់ 0;
                }
                isEmpty=false;
                continue;  // no message fragment after the offset
            } បើមិនដូច្នេះ {
                // normal selector word
                ប្រសិនបើ(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    ត្រឡប់ 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                ប្រសិនបើ(0==msg.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=true;
                }
            }
        }
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        ប្រសិនបើ(index==msg.length() || msg.charAt(index)!=u_leftCurlyBrace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ត្រឡប់ 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        ប្រសិនបើ(U_FAILURE(errorCode)) {
            ត្រឡប់ 0;
        }
        isEmpty=false;
    }
}

int32_t
MessagePattern::parseArgNumber(const UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ប្រសិនបើ(start>=limit) {
        ត្រឡប់ UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool badNumber;
    char16_t c=s.charAt(start++);
    ប្រសិនបើ(c==0x30) {
        ប្រសិនបើ(start==limit) {
            ត្រឡប់ 0;
        } បើមិនដូច្នេះ {
            number=0;
            badNumber=true;  // leading zero
        }
    } បើមិនដូច្នេះ ប្រសិនបើ(0x31<=c && c<=0x39) {
        number=c-0x30;
        badNumber=false;
    } បើមិនដូច្នេះ {
        ត្រឡប់ UMSGPAT_ARG_NAME_NOT_NUMBER;
    }
    ខណៈពេល(start<limit) {
        c=s.charAt(start++);
        ប្រសិនបើ(0x30<=c && c<=0x39) {
            ប្រសិនបើ(number>=INT32_MAX/10) {
                badNumber=true;  // overflow
            } បើមិនដូច្នេះ {
                number=number*10+(c-0x30);
            }
        } បើមិនដូច្នេះ {
            ត្រឡប់ UMSGPAT_ARG_NAME_NOT_NUMBER;
        }
    }
    // There are only ASCII digits.
    ប្រសិនបើ(badNumber) {
        ត្រឡប់ UMSGPAT_ARG_NAME_NOT_VALID;
    } បើមិនដូច្នេះ {
        ត្រឡប់ number;
    }
}

void
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    សម្រាប់(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=msg.charAt(index++);
        ប្រសិនបើ(c==u_minus) {
            isNegative=1;
            ប្រសិនបើ(index==limit) {
                bha_nga;  // no number
            }
            c=msg.charAt(index++);
        } បើមិនដូច្នេះ ប្រសិនបើ(c==u_plus) {
            ប្រសិនបើ(index==limit) {
                bha_nga;  // no number
            }
            c=msg.charAt(index++);
        }
        ប្រសិនបើ(c==0x221e) {  // infinity
            ប្រសិនបើ(allowInfinity && index==limit) {
                dviguu_na infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                ត្រឡប់;
            } បើមិនដូច្នេះ {
                bha_nga;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        ខណៈពេល('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ប្រសិនបើ(value>(Part::MAX_VALUE+isNegative)) {
                bha_nga;  // not a small-enough integer
            }
            ប្រសិនបើ(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                ត្រឡប់;
            }
            c=msg.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        var_na numberChars[128];
        int32_t capacity = static_cast<int32_t>(parimaa_na(numberChars));
        int32_t length=limit-start;
        ប្រសិនបើ(length>=capacity) {
            bha_nga;  // number too long
        }
        msg.extract(start, length, numberChars, capacity, US_INV);
        ប្រសិនបើ (static_cast<int32_t>(uprv_strlen(numberChars)) < length) {
            bha_nga;  // contains non-invariant character that was turned into NUL
        }
        var_na *end;
        dviguu_na numericValue=uprv_strtod(numberChars, &end);
        ប្រសិនបើ(end!=(numberChars+length)) {
            bha_nga;  // parsing error
        }
        addArgDoublePart(numericValue, start, length, errorCode);
        ត្រឡប់;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    const char16_t *s=msg.getBuffer();
    int32_t msgLength=msg.length();
    const char16_t *t=PatternProps::skipWhiteSpace(s+index, msgLength-index);
    ត្រឡប់ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    const char16_t *s=msg.getBuffer();
    int32_t msgLength=msg.length();
    const char16_t *t=PatternProps::skipIdentifier(s+index, msgLength-index);
    ត្រឡប់ static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t msgLength=msg.length();
    ខណៈពេល(index<msgLength) {
        char16_t c=msg.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ប្រសិនបើ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            bha_nga;
        }
        ++index;
    }
    ត្រឡប់ index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    ត្រឡប់ (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    ត្រឡប់
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
    ត្រឡប់
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
    ត្រឡប់
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
    ត្រឡប់
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
    ត្រឡប់ nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_MSG_START;
}

UBool
MessagePattern::inTopLevelChoiceMessage(int32_t nestingLevel, UMessagePatternArgType parentType) {
    ត្រឡប់
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_MSG_START;
}

void
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    ប្រសិនបើ(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

void
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

void
MessagePattern::addArgDoublePart(dviguu_na numericValue, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    ប្រសិនបើ(U_FAILURE(errorCode)) {
        ត្រឡប់;
    }
    int32_t numericIndex=numericValuesLength;
    ប្រសិនបើ(numericValuesList==nullptr) {
        numericValuesList=new MessagePatternDoubleList();
        ប្រសិនបើ(numericValuesList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ត្រឡប់;
        }
    } បើមិនដូច្នេះ ប្រសិនបើ(!numericValuesList->ensureCapacityForOneMore(numericValuesLength, errorCode)) {
        ត្រឡប់;
    } បើមិនដូច្នេះ {
        ប្រសិនបើ(numericIndex>Part::MAX_VALUE) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ត្រឡប់;
        }
    }
    numericValuesList->a[numericValuesLength++]=numericValue;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

void
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    ប្រសិនបើ(parseError==nullptr) {
        ត្រឡប់;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    ប្រសិនបើ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ប្រសិនបើ(length>0 && U16_IS_TRAIL(msg[index-length])) {
            --length;
        }
    }
    msg.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=msg.length()-index;
    ប្រសិនបើ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ប្រសិនបើ(length>0 && U16_IS_LEAD(msg[index+length-1])) {
            --length;
        }
    }
    msg.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

void
MessageImpl::appendReducedApostrophes(const UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    សម្រាប់(;;) {
        int32_t i=s.indexOf(u_apos, start);
        ប្រសិនបើ(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            bha_nga;
        }
        ប្រសិនបើ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } បើមិនដូច្នេះ {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(const MessagePattern& msgPattern,
                                               int32_t msgStart,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    const UnicodeString &msgString=msgPattern.getPatternString();
    int32_t prevIndex=msgPattern.getPart(msgStart).getLimit();
    សម្រាប់(int32_t i=msgStart;;) {
        const MessagePattern::Part &part=msgPattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        ប្រសិនបើ(type==UMSGPAT_PART_TYPE_MSG_LIMIT) {
            ត្រឡប់ result.append(msgString, prevIndex, index-prevIndex);
        } បើមិនដូច្នេះ ប្រសិនបើ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(msgString, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } បើមិនដូច្នេះ ប្រសិនបើ(type==UMSGPAT_PART_TYPE_ARG_START) {
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
