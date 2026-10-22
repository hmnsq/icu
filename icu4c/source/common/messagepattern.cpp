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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_FORMATTING

#شامل "unicode/messagepattern.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf16.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "messageimpl.h"
#شامل "patternprops.h"
#شامل "putilimp.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

ایستا ثابت char16_t u_pound=0x23;
ایستا ثابت char16_t u_apos=0x27;
ایستا ثابت char16_t u_plus=0x2B;
ایستا ثابت char16_t u_comma=0x2C;
ایستا ثابت char16_t u_minus=0x2D;
ایستا ثابت char16_t u_dot=0x2E;
ایستا ثابت char16_t u_colon=0x3A;
ایستا ثابت char16_t u_lessThan=0x3C;
ایستا ثابت char16_t u_equal=0x3D;
ایستا ثابت char16_t u_A=0x41;
ایستا ثابت char16_t u_C=0x43;
ایستا ثابت char16_t u_D=0x44;
ایستا ثابت char16_t u_E=0x45;
ایستا ثابت char16_t u_H=0x48;
ایستا ثابت char16_t u_I=0x49;
ایستا ثابت char16_t u_L=0x4C;
ایستا ثابت char16_t u_N=0x4E;
ایستا ثابت char16_t u_O=0x4F;
ایستا ثابت char16_t u_P=0x50;
ایستا ثابت char16_t u_R=0x52;
ایستا ثابت char16_t u_S=0x53;
ایستا ثابت char16_t u_T=0x54;
ایستا ثابت char16_t u_U=0x55;
ایستا ثابت char16_t u_Z=0x5A;
ایستا ثابت char16_t u_a=0x61;
ایستا ثابت char16_t u_c=0x63;
ایستا ثابت char16_t u_d=0x64;
ایستا ثابت char16_t u_e=0x65;
ایستا ثابت char16_t u_f=0x66;
ایستا ثابت char16_t u_h=0x68;
ایستا ثابت char16_t u_i=0x69;
ایستا ثابت char16_t u_l=0x6C;
ایستا ثابت char16_t u_n=0x6E;
ایستا ثابت char16_t u_o=0x6F;
ایستا ثابت char16_t u_p=0x70;
ایستا ثابت char16_t u_r=0x72;
ایستا ثابت char16_t u_s=0x73;
ایستا ثابت char16_t u_t=0x74;
ایستا ثابت char16_t u_u=0x75;
ایستا ثابت char16_t u_z=0x7A;
ایستا ثابت char16_t u_چپ_Curly_Brace=0x7B;
ایستا ثابت char16_t u_pipe=0x7C;
ایستا ثابت char16_t u_راست_Curly_Brace=0x7D;
ایستا ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ایستا ثابت char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ایستا ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t پشته_Capacity>
رده MessagePatternList : عمومی UMemory {
عمومی:
    MessagePatternList() {}
    تهی copyFrom(ثابت MessagePatternList<T, پشته_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ثابت MessagePatternList<T, پشته_Capacity> &other, int32_t length) ثابت {
        برای(int32_t i=0; i<length; ++i) {
            اگر(a[i]!=other.a[i]) { بازگشت نادرست; }
        }
        بازگشت درست;
    }

    Maybe_پشته_Array<T, پشته_Capacity> a;
};

template<typename T, int32_t پشته_Capacity>
تهی
MessagePatternList<T, پشته_Capacity>::copyFrom(
        ثابت MessagePatternList<T, پشته_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    اگر(U_SUCCESS(errorCode) && length>0) {
        اگر(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*اندازه(T));
    }
}

template<typename T, int32_t پشته_Capacity>
UBool
MessagePatternList<T, پشته_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    اگر(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        بازگشت درست;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    بازگشت نادرست;
}

// MessagePatternList specializations -------------------------------------- ***

رده MessagePatternDoubleList : عمومی MessagePatternList<دوگانه, 8> {
};

رده MessagePatternPartsList : عمومی MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_مقادیر_List(nullptr), numeric_مقادیر(nullptr), numeric_مقادیر_Length(0),
          hasArgNames(نادرست), hasArgNumbers(نادرست), needsAutoQuoting(نادرست) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_مقادیر_List(nullptr), numeric_مقادیر(nullptr), numeric_مقادیر_Length(0),
          hasArgNames(نادرست), hasArgNumbers(نادرست), needsAutoQuoting(نادرست) {
    init(errorCode);
}

MessagePattern::MessagePattern(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_مقادیر_List(nullptr), numeric_مقادیر(nullptr), numeric_مقادیر_Length(0),
          hasArgNames(نادرست), hasArgNumbers(نادرست), needsAutoQuoting(نادرست) {
    اگر(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    partsList=جدید MessagePatternPartsList();
    اگر(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت نادرست;
    }
    parts=partsList->a.getAlias();
    بازگشت درست;
}

MessagePattern::MessagePattern(ثابت MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), پیام(other.پیام),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_مقادیر_List(nullptr), numeric_مقادیر(nullptr), numeric_مقادیر_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    اگر(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ثابت MessagePattern& other) U_LIFETIME_BOUND {
    اگر(این==&other) {
        بازگشت *این;
    }
    aposMode=other.aposMode;
    پیام=other.پیام;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    اگر(!copyStorage(other, errorCode)) {
        clear();
    }
    بازگشت *این;
}

UBool
MessagePattern::copyStorage(ثابت MessagePattern &other, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت نادرست;
    }
    parts=nullptr;
    partsLength=0;
    numeric_مقادیر=nullptr;
    numeric_مقادیر_Length=0;
    اگر(partsList==nullptr) {
        partsList=جدید MessagePatternPartsList();
        اگر(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت نادرست;
        }
        parts=partsList->a.getAlias();
    }
    اگر(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت نادرست;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    اگر(other.numeric_مقادیر_Length>0) {
        اگر(numeric_مقادیر_List==nullptr) {
            numeric_مقادیر_List=جدید MessagePatternDoubleList();
            اگر(numeric_مقادیر_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                بازگشت نادرست;
            }
            numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
        }
        numeric_مقادیر_List->copyFrom(
            *other.numeric_مقادیر_List, other.numeric_مقادیر_Length, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت نادرست;
        }
        numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
        numeric_مقادیر_Length=other.numeric_مقادیر_Length;
    }
    بازگشت درست;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_مقادیر_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ثابت UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    بازگشت *این;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    بازگشت *این;
}

MessagePattern&
MessagePattern::parsePluralStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    بازگشت *این;
}

MessagePattern&
MessagePattern::parseSelectStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    بازگشت *این;
}

تهی
MessagePattern::clear() {
    // Mostly the same as preParse().
    پیام.remove();
    hasArgNames=hasArgNumbers=نادرست;
    needsAutoQuoting=نادرست;
    partsLength=0;
    numeric_مقادیر_Length=0;
}

منطقی
MessagePattern::operator==(ثابت MessagePattern &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    بازگشت
        aposMode==other.aposMode &&
        پیام==other.پیام &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ثابت {
    int32_t hash=(aposMode*37+پیام.hashCode())*37+partsLength;
    برای(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    بازگشت hash;
}

int32_t
MessagePattern::validateArgumentName(ثابت UnicodeString &name) {
    اگر(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        بازگشت UMSGPAT_ARG_NAME_NOT_VALID;
    }
    بازگشت parse_Arg_عدد(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ثابت {
    اگر(!needsAutoQuoting) {
        بازگشت پیام;
    }
    UnicodeString modified(پیام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=شمار_Parts();
    برای(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        اگر(part.getType()==UMSGPAT_PART_TYPE_درج_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    بازگشت modified;
}

دوگانه
MessagePattern::get_Numeric_مقدار(ثابت Part &part) ثابت {
    UMessagePatternPartType type=part.type;
    اگر(type==UMSGPAT_PART_TYPE_ARG_INT) {
        بازگشت part.value;
    } وگرنه اگر(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        بازگشت numeric_مقادیر[part.value];
    } وگرنه {
        بازگشت UMSGPAT_NO_NUMERIC_مقدار;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
دوگانه
MessagePattern::getPluralOffset(int32_t pluralStart) ثابت {
    ثابت Part &part=getPart(pluralStart);
    اگر(Part::has_Numeric_مقدار(part.type)) {
        بازگشت get_Numeric_مقدار(part);
    } وگرنه {
        بازگشت 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقی
MessagePattern::Part::operator==(ثابت Part &other) ثابت {
    اگر(این==&other) {
        بازگشت درست;
    }
    بازگشت
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

تهی
MessagePattern::preParse(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    اگر(parseError!=nullptr) {
        parseError->سطر=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پیام=pattern;
    hasArgNames=hasArgNumbers=نادرست;
    needsAutoQuoting=نادرست;
    partsLength=0;
    numeric_مقادیر_Length=0;
}

تهی
MessagePattern::postParse() {
    اگر(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    اگر(numeric_مقادیر_List!=nullptr) {
        numeric_مقادیر=numeric_مقادیر_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t پیام_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    اگر(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        بازگشت 0;
    }
    int32_t پیام_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_پیام_START, index, پیام_Start_Length, nestingLevel, errorCode);
    index+=پیام_Start_Length;
    برای(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
        اگر(index>=پیام.length()) {
            بشکن;
        }
        char16_t c=پیام.charAt(index++);
        اگر(c==u_apos) {
            اگر(index==پیام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_درج_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=درست;
            } وگرنه {
                c=پیام.charAt(index);
                اگر(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } وگرنه اگر(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_چپ_Curly_Brace || c==u_راست_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    برای(;;) {
                        index=پیام.indexOf(u_apos, index+1);
                        اگر(index>=0) {
                            اگر(/*(index+1)<msg.length() &&*/ پیام.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } وگرنه {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                بشکن;
                            }
                        } وگرنه {
                            // The quoted text reaches to the end of the of the message.
                            index=پیام.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_درج_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=درست;
                            بشکن;
                        }
                    }
                } وگرنه {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_درج_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=درست;
                }
            }
        } وگرنه اگر(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_عدد, index-1, 1, 0, errorCode);
        } وگرنه اگر(c==u_چپ_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } وگرنه اگر((nestingLevel>0 && c==u_راست_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_راست_Curly_Brace) ? 0 : 1;
            addLimitPart(پیام_Start, UMSGPAT_PART_TYPE_پیام_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            اگر(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                بازگشت index-1;
            } وگرنه {
                // continue parsing after the '}'
                بازگشت index;
            }
        }  // else: c is part of literal text
    }
    اگر(nestingLevel>0 && !in_بالاترین_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        بازگشت 0;
    }
    addLimitPart(پیام_Start, UMSGPAT_PART_TYPE_پیام_LIMIT, index, 0, nestingLevel, errorCode);
    بازگشت index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    اگر(index==پیام.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        بازگشت 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_عدد(nameIndex, index);
    اگر(number>=0) {
        int32_t length=index-nameIndex;
        اگر(length>Part::MAX_LENGTH || number>Part::MAX_مقدار) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
        hasArgNumbers=درست;
        addPart(UMSGPAT_PART_TYPE_ARG_عدد, nameIndex, length, number, errorCode);
    } وگرنه اگر(number==UMSGPAT_ARG_NAME_NOT_عدد) {
        int32_t length=index-nameIndex;
        اگر(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
        hasArgNames=درست;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } وگرنه {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        بازگشت 0;
    }
    index=skipWhiteSpace(index);
    اگر(index==پیام.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        بازگشت 0;
    }
    char16_t c=پیام.charAt(index);
    اگر(c==u_راست_Curly_Brace) {
        // all done
    } وگرنه اگر(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        بازگشت 0;
    } وگرنه /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        وقتی(index<پیام.length() && isArgTypeChar(پیام.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        اگر(index==پیام.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            بازگشت 0;
        }
        اگر(length==0 || ((c=پیام.charAt(index))!=u_comma && c!=u_راست_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            بازگشت 0;
        }
        اگر(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        اگر(length==6) {
            // case-insensitive comparisons for complex-type names
            اگر(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } وگرنه اگر(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } وگرنه اگر(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } وگرنه اگر(length==13) {
            اگر(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        اگر(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        اگر(c==u_راست_Curly_Brace) {
            اگر(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
        } وگرنه /* ',' */ {
            ++index;
            اگر(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } وگرنه اگر(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } وگرنه {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    بازگشت index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    وقتی(index<پیام.length()) {
        char16_t c=پیام.charAt(index++);
        اگر(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=پیام.indexOf(u_apos, index);
            اگر(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } وگرنه اگر(c==u_چپ_Curly_Brace) {
            ++nestedBraces;
        } وگرنه اگر(c==u_راست_Curly_Brace) {
            اگر(nestedBraces>0) {
                --nestedBraces;
            } وگرنه {
                int32_t length=--index-start;
                اگر(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    بازگشت 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                بازگشت index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    بازگشت 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    اگر(index==پیام.length() || پیام.charAt(index)==u_راست_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        بازگشت 0;
    }
    برای(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t عدد_Index=index;
        index=skipDouble(index);
        int32_t length=index-عدد_Index;
        اگر(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            بازگشت 0;
        }
        اگر(length>Part::MAX_LENGTH) {
            setParseError(parseError, عدد_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
        parseDouble(عدد_Index, index, درست, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        اگر(index==پیام.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            بازگشت 0;
        }
        char16_t c=پیام.charAt(index);
        اگر(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            بازگشت 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        اگر(index==پیام.length()) {
            بازگشت index;
        }
        اگر(پیام.charAt(index)==u_راست_Curly_Brace) {
            اگر(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
            بازگشت index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت 0;
    }
    int32_t start=index;
    UBool isEmpty=درست;
    UBool hasOther=نادرست;
    برای(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==پیام.length();
        اگر(eos || پیام.charAt(index)==u_راست_Curly_Brace) {
            اگر(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
            اگر(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                بازگشت 0;
            }
            بازگشت index;
        }
        int32_t selectorIndex=index;
        اگر(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پیام.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            اگر(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
            اگر(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                بازگشت 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, نادرست,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } وگرنه {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            اگر(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                بازگشت 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            اگر( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<پیام.length() &&
                0==پیام.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                اگر(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    بازگشت 0;
                }
                // allow whitespace between offset: and its value
                int32_t مقدار_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(مقدار_Index);
                اگر(index==مقدار_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    بازگشت 0;
                }
                اگر((index-مقدار_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, مقدار_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    بازگشت 0;
                }
                parseDouble(مقدار_Index, index, نادرست,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                اگر(U_FAILURE(errorCode)) {
                    بازگشت 0;
                }
                isEmpty=نادرست;
                ادامه;  // no message fragment after the offset
            } وگرنه {
                // normal selector word
                اگر(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    بازگشت 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                اگر(0==پیام.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=درست;
                }
            }
        }
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        اگر(index==پیام.length() || پیام.charAt(index)!=u_چپ_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            بازگشت 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        اگر(U_FAILURE(errorCode)) {
            بازگشت 0;
        }
        isEmpty=نادرست;
    }
}

int32_t
MessagePattern::parse_Arg_عدد(ثابت UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    اگر(start>=limit) {
        بازگشت UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_عدد;
    char16_t c=s.charAt(start++);
    اگر(c==0x30) {
        اگر(start==limit) {
            بازگشت 0;
        } وگرنه {
            number=0;
            bad_عدد=درست;  // leading zero
        }
    } وگرنه اگر(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_عدد=نادرست;
    } وگرنه {
        بازگشت UMSGPAT_ARG_NAME_NOT_عدد;
    }
    وقتی(start<limit) {
        c=s.charAt(start++);
        اگر(0x30<=c && c<=0x39) {
            اگر(number>=INT32_MAX/10) {
                bad_عدد=درست;  // overflow
            } وگرنه {
                number=number*10+(c-0x30);
            }
        } وگرنه {
            بازگشت UMSGPAT_ARG_NAME_NOT_عدد;
        }
    }
    // There are only ASCII digits.
    اگر(bad_عدد) {
        بازگشت UMSGPAT_ARG_NAME_NOT_VALID;
    } وگرنه {
        بازگشت number;
    }
}

تهی
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    برای(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=پیام.charAt(index++);
        اگر(c==u_minus) {
            isNegative=1;
            اگر(index==limit) {
                بشکن;  // no number
            }
            c=پیام.charAt(index++);
        } وگرنه اگر(c==u_plus) {
            اگر(index==limit) {
                بشکن;  // no number
            }
            c=پیام.charAt(index++);
        }
        اگر(c==0x221e) {  // infinity
            اگر(allowInfinity && index==limit) {
                دوگانه infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                بازگشت;
            } وگرنه {
                بشکن;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        وقتی('0'<=c && c<='9') {
            value=value*10+(c-'0');
            اگر(value>(Part::MAX_مقدار+isNegative)) {
                بشکن;  // not a small-enough integer
            }
            اگر(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                بازگشت;
            }
            c=پیام.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        نویسه عدد_Chars[128];
        int32_t capacity = static_cast<int32_t>(اندازه(عدد_Chars));
        int32_t length=limit-start;
        اگر(length>=capacity) {
            بشکن;  // number too long
        }
        پیام.extract(start, length, عدد_Chars, capacity, US_INV);
        اگر (static_cast<int32_t>(uprv_strlen(عدد_Chars)) < length) {
            بشکن;  // contains non-invariant character that was turned into NUL
        }
        نویسه *end;
        دوگانه numeric_مقدار=uprv_strtod(عدد_Chars, &end);
        اگر(end!=(عدد_Chars+length)) {
            بشکن;  // parsing error
        }
        addArgDoublePart(numeric_مقدار, start, length, errorCode);
        بازگشت;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ثابت char16_t *s=پیام.getBuffer();
    int32_t پیام_Length=پیام.length();
    ثابت char16_t *t=PatternProps::skipWhiteSpace(s+index, پیام_Length-index);
    بازگشت static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ثابت char16_t *s=پیام.getBuffer();
    int32_t پیام_Length=پیام.length();
    ثابت char16_t *t=PatternProps::skipIdentifier(s+index, پیام_Length-index);
    بازگشت static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t پیام_Length=پیام.length();
    وقتی(index<پیام_Length) {
        char16_t c=پیام.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        اگر((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            بشکن;
        }
        ++index;
    }
    بازگشت index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    بازگشت (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    بازگشت
        ((c=پیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پیام.charAt(index++))==u_h || c==u_H) &&
        ((c=پیام.charAt(index++))==u_o || c==u_O) &&
        ((c=پیام.charAt(index++))==u_i || c==u_I) &&
        ((c=پیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پیام.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    بازگشت
        ((c=پیام.charAt(index++))==u_p || c==u_P) &&
        ((c=پیام.charAt(index++))==u_l || c==u_L) &&
        ((c=پیام.charAt(index++))==u_u || c==u_U) &&
        ((c=پیام.charAt(index++))==u_r || c==u_R) &&
        ((c=پیام.charAt(index++))==u_a || c==u_A) &&
        ((c=پیام.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    بازگشت
        ((c=پیام.charAt(index++))==u_s || c==u_S) &&
        ((c=پیام.charAt(index++))==u_e || c==u_E) &&
        ((c=پیام.charAt(index++))==u_l || c==u_L) &&
        ((c=پیام.charAt(index++))==u_e || c==u_E) &&
        ((c=پیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پیام.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    بازگشت
        ((c=پیام.charAt(index++))==u_o || c==u_O) &&
        ((c=پیام.charAt(index++))==u_r || c==u_R) &&
        ((c=پیام.charAt(index++))==u_d || c==u_D) &&
        ((c=پیام.charAt(index++))==u_i || c==u_I) &&
        ((c=پیام.charAt(index++))==u_n || c==u_N) &&
        ((c=پیام.charAt(index++))==u_a || c==u_A) &&
        ((c=پیام.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    بازگشت nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پیام_START;
}

UBool
MessagePattern::in_بالاترین_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    بازگشت
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پیام_START;
}

تهی
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    اگر(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

تهی
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

تهی
MessagePattern::addArgDoublePart(دوگانه numeric_مقدار, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    int32_t numericIndex=numeric_مقادیر_Length;
    اگر(numeric_مقادیر_List==nullptr) {
        numeric_مقادیر_List=جدید MessagePatternDoubleList();
        اگر(numeric_مقادیر_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت;
        }
    } وگرنه اگر(!numeric_مقادیر_List->ensureCapacityForOneMore(numeric_مقادیر_Length, errorCode)) {
        بازگشت;
    } وگرنه {
        اگر(numericIndex>Part::MAX_مقدار) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت;
        }
    }
    numeric_مقادیر_List->a[numeric_مقادیر_Length++]=numeric_مقدار;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

تهی
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    اگر(parseError==nullptr) {
        بازگشت;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    اگر(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        اگر(length>0 && U16_IS_TRAIL(پیام[index-length])) {
            --length;
        }
    }
    پیام.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پیام.length()-index;
    اگر(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        اگر(length>0 && U16_IS_LEAD(پیام[index+length-1])) {
            --length;
        }
    }
    پیام.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

تهی
MessageImpl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    برای(;;) {
        int32_t i=s.indexOf(u_apos, start);
        اگر(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            بشکن;
        }
        اگر(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } وگرنه {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ثابت MessagePattern& پیام_Pattern,
                                               int32_t پیام_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ثابت UnicodeString &پیام_String=پیام_Pattern.getPatternString();
    int32_t prevIndex=پیام_Pattern.getPart(پیام_Start).getLimit();
    برای(int32_t i=پیام_Start;;) {
        ثابت MessagePattern::Part &part=پیام_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        اگر(type==UMSGPAT_PART_TYPE_پیام_LIMIT) {
            بازگشت result.append(پیام_String, prevIndex, index-prevIndex);
        } وگرنه اگر(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(پیام_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } وگرنه اگر(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(پیام_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=پیام_Pattern.getLimitPartIndex(i);
            index=پیام_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(پیام_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
