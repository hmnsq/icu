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

#إذا !UCONFIG_NO_FORMATTING

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
ساكن ثابت char16_t u_چەپ_Curly_Brace=0x7B;
ساكن ثابت char16_t u_pipe=0x7C;
ساكن ثابت char16_t u_ڕاست_Curly_Brace=0x7D;
ساكن ثابت char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

ساكن ثابت char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

ساكن ثابت char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t کەڵەکە_Capacity>
صنف MessagePatternList : عام UMemory {
عام:
    MessagePatternList() {}
    فراغ copyFrom(ثابت MessagePatternList<T, کەڵەکە_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ثابت MessagePatternList<T, کەڵەکە_Capacity> &other, int32_t length) ثابت {
        لكل(int32_t i=0; i<length; ++i) {
            إذا(a[i]!=other.a[i]) { أرجع خطأ; }
        }
        أرجع true;
    }

    Maybe_کەڵەکە_Array<T, کەڵەکە_Capacity> a;
};

template<typename T, int32_t کەڵەکە_Capacity>
فراغ
MessagePatternList<T, کەڵەکە_Capacity>::copyFrom(
        ثابت MessagePatternList<T, کەڵەکە_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    إذا(U_SUCCESS(errorCode) && length>0) {
        إذا(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*حجم(T));
    }
}

template<typename T, int32_t کەڵەکە_Capacity>
UBool
MessagePatternList<T, کەڵەکە_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    إذا(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        أرجع true;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    أرجع خطأ;
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
          numeric_بەهاکان_List(nullptr), numeric_بەهاکان(nullptr), numeric_بەهاکان_Length(0),
          hasArgNames(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_بەهاکان_List(nullptr), numeric_بەهاکان(nullptr), numeric_بەهاکان_Length(0),
          hasArgNames(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    init(errorCode);
}

MessagePattern::MessagePattern(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_بەهاکان_List(nullptr), numeric_بەهاکان(nullptr), numeric_بەهاکان_Length(0),
          hasArgNames(خطأ), hasArgNumbers(خطأ), needsAutoQuoting(خطأ) {
    إذا(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    partsList=جديد MessagePatternPartsList();
    إذا(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع خطأ;
    }
    parts=partsList->a.getAlias();
    أرجع true;
}

MessagePattern::MessagePattern(ثابت MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), پەیام(other.پەیام),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_بەهاکان_List(nullptr), numeric_بەهاکان(nullptr), numeric_بەهاکان_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    إذا(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ثابت MessagePattern& other) U_LIFETIME_BOUND {
    إذا(هذا==&other) {
        أرجع *هذا;
    }
    aposMode=other.aposMode;
    پەیام=other.پەیام;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    إذا(!copyStorage(other, errorCode)) {
        clear();
    }
    أرجع *هذا;
}

UBool
MessagePattern::copyStorage(ثابت MessagePattern &other, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع خطأ;
    }
    parts=nullptr;
    partsLength=0;
    numeric_بەهاکان=nullptr;
    numeric_بەهاکان_Length=0;
    إذا(partsList==nullptr) {
        partsList=جديد MessagePatternPartsList();
        إذا(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
    }
    إذا(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        إذا(U_FAILURE(errorCode)) {
            أرجع خطأ;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    إذا(other.numeric_بەهاکان_Length>0) {
        إذا(numeric_بەهاکان_List==nullptr) {
            numeric_بەهاکان_List=جديد MessagePatternDoubleList();
            إذا(numeric_بەهاکان_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                أرجع خطأ;
            }
            numeric_بەهاکان=numeric_بەهاکان_List->a.getAlias();
        }
        numeric_بەهاکان_List->copyFrom(
            *other.numeric_بەهاکان_List, other.numeric_بەهاکان_Length, errorCode);
        إذا(U_FAILURE(errorCode)) {
            أرجع خطأ;
        }
        numeric_بەهاکان=numeric_بەهاکان_List->a.getAlias();
        numeric_بەهاکان_Length=other.numeric_بەهاکان_Length;
    }
    أرجع true;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_بەهاکان_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ثابت UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    أرجع *هذا;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    أرجع *هذا;
}

MessagePattern&
MessagePattern::parsePluralStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    أرجع *هذا;
}

MessagePattern&
MessagePattern::parseSelectStyle(ثابت UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    أرجع *هذا;
}

فراغ
MessagePattern::clear() {
    // Mostly the same as preParse().
    پەیام.remove();
    hasArgNames=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    partsLength=0;
    numeric_بەهاکان_Length=0;
}

منطقي
MessagePattern::operator==(ثابت MessagePattern &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    أرجع
        aposMode==other.aposMode &&
        پەیام==other.پەیام &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ثابت {
    int32_t hash=(aposMode*37+پەیام.hashCode())*37+partsLength;
    لكل(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    أرجع hash;
}

int32_t
MessagePattern::validateArgumentName(ثابت UnicodeString &name) {
    إذا(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    أرجع parse_Arg_ژمار(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ثابت {
    إذا(!needsAutoQuoting) {
        أرجع پەیام;
    }
    UnicodeString modified(پەیام);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ژماردن_Parts();
    لكل(int32_t i=count; i>0;) {
        ثابت Part &part=getPart(--i);
        إذا(part.getType()==UMSGPAT_PART_TYPE_زیادکردن_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    أرجع modified;
}

مزدوج
MessagePattern::get_Numeric_بەها(ثابت Part &part) ثابت {
    UMessagePatternPartType type=part.type;
    إذا(type==UMSGPAT_PART_TYPE_ARG_INT) {
        أرجع part.value;
    } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        أرجع numeric_بەهاکان[part.value];
    } وإلا {
        أرجع UMSGPAT_NO_NUMERIC_بەها;
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
    إذا(Part::has_Numeric_بەها(part.type)) {
        أرجع get_Numeric_بەها(part);
    } وإلا {
        أرجع 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

منطقي
MessagePattern::Part::operator==(ثابت Part &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    أرجع
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

فراغ
MessagePattern::preParse(ثابت UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    إذا(parseError!=nullptr) {
        parseError->هێڵ=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    پەیام=pattern;
    hasArgNames=hasArgNumbers=خطأ;
    needsAutoQuoting=خطأ;
    partsLength=0;
    numeric_بەهاکان_Length=0;
}

فراغ
MessagePattern::postParse() {
    إذا(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    إذا(numeric_بەهاکان_List!=nullptr) {
        numeric_بەهاکان=numeric_بەهاکان_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t پەیام_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    إذا(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        أرجع 0;
    }
    int32_t پەیام_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_پەیام_START, index, پەیام_Start_Length, nestingLevel, errorCode);
    index+=پەیام_Start_Length;
    لكل(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        إذا(U_FAILURE(errorCode)) {
            أرجع 0;
        }
        إذا(index>=پەیام.length()) {
            اكسر;
        }
        char16_t c=پەیام.charAt(index++);
        إذا(c==u_apos) {
            إذا(index==پەیام.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_زیادکردن_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=true;
            } وإلا {
                c=پەیام.charAt(index);
                إذا(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } وإلا إذا(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_چەپ_Curly_Brace || c==u_ڕاست_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    لكل(;;) {
                        index=پەیام.indexOf(u_apos, index+1);
                        إذا(index>=0) {
                            إذا(/*(index+1)<msg.length() &&*/ پەیام.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } وإلا {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                اكسر;
                            }
                        } وإلا {
                            // The quoted text reaches to the end of the of the message.
                            index=پەیام.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_زیادکردن_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=true;
                            اكسر;
                        }
                    }
                } وإلا {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_زیادکردن_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=true;
                }
            }
        } وإلا إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_ژمار, index-1, 1, 0, errorCode);
        } وإلا إذا(c==u_چەپ_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } وإلا إذا((nestingLevel>0 && c==u_ڕاست_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_ڕاست_Curly_Brace) ? 0 : 1;
            addLimitPart(پەیام_Start, UMSGPAT_PART_TYPE_پەیام_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            إذا(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                أرجع index-1;
            } وإلا {
                // continue parsing after the '}'
                أرجع index;
            }
        }  // else: c is part of literal text
    }
    إذا(nestingLevel>0 && !in_لووتکە_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    addLimitPart(پەیام_Start, UMSGPAT_PART_TYPE_پەیام_LIMIT, index, 0, nestingLevel, errorCode);
    أرجع index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    إذا(index==پەیام.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_ژمار(nameIndex, index);
    إذا(number>=0) {
        int32_t length=index-nameIndex;
        إذا(length>Part::MAX_LENGTH || number>Part::MAX_بەها) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNumbers=true;
        addPart(UMSGPAT_PART_TYPE_ARG_ژمار, nameIndex, length, number, errorCode);
    } وإلا إذا(number==UMSGPAT_ARG_NAME_NOT_ژمار) {
        int32_t length=index-nameIndex;
        إذا(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        hasArgNames=true;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } وإلا {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        أرجع 0;
    }
    index=skipWhiteSpace(index);
    إذا(index==پەیام.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        أرجع 0;
    }
    char16_t c=پەیام.charAt(index);
    إذا(c==u_ڕاست_Curly_Brace) {
        // all done
    } وإلا إذا(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        أرجع 0;
    } وإلا /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        طالما(index<پەیام.length() && isArgTypeChar(پەیام.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        إذا(index==پەیام.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            أرجع 0;
        }
        إذا(length==0 || ((c=پەیام.charAt(index))!=u_comma && c!=u_ڕاست_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        إذا(length==6) {
            // case-insensitive comparisons for complex-type names
            إذا(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } وإلا إذا(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } وإلا إذا(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } وإلا إذا(length==13) {
            إذا(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        إذا(c==u_ڕاست_Curly_Brace) {
            إذا(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
        } وإلا /* ',' */ {
            ++index;
            إذا(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } وإلا إذا(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } وإلا {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    أرجع index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    طالما(index<پەیام.length()) {
        char16_t c=پەیام.charAt(index++);
        إذا(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=پەیام.indexOf(u_apos, index);
            إذا(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } وإلا إذا(c==u_چەپ_Curly_Brace) {
            ++nestedBraces;
        } وإلا إذا(c==u_ڕاست_Curly_Brace) {
            إذا(nestedBraces>0) {
                --nestedBraces;
            } وإلا {
                int32_t length=--index-start;
                إذا(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                أرجع index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    أرجع 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    إذا(index==پەیام.length() || پەیام.charAt(index)==u_ڕاست_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        أرجع 0;
    }
    لكل(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t ژمار_Index=index;
        index=skipDouble(index);
        int32_t length=index-ژمار_Index;
        إذا(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            أرجع 0;
        }
        إذا(length>Part::MAX_LENGTH) {
            setParseError(parseError, ژمار_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
        parseDouble(ژمار_Index, index, true, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        إذا(U_FAILURE(errorCode)) {
            أرجع 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        إذا(index==پەیام.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            أرجع 0;
        }
        char16_t c=پەیام.charAt(index);
        إذا(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            أرجع 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        إذا(U_FAILURE(errorCode)) {
            أرجع 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        إذا(index==پەیام.length()) {
            أرجع index;
        }
        إذا(پەیام.charAt(index)==u_ڕاست_Curly_Brace) {
            إذا(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
            أرجع index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع 0;
    }
    int32_t start=index;
    UBool isEmpty=true;
    UBool hasOther=خطأ;
    لكل(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==پەیام.length();
        إذا(eos || پەیام.charAt(index)==u_ڕاست_Curly_Brace) {
            إذا(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                أرجع 0;
            }
            أرجع index;
        }
        int32_t selectorIndex=index;
        إذا(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && پەیام.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            إذا(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
            إذا(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, خطأ,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } وإلا {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            إذا(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                أرجع 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            إذا( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<پەیام.length() &&
                0==پەیام.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                إذا(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    أرجع 0;
                }
                // allow whitespace between offset: and its value
                int32_t بەها_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(بەها_Index);
                إذا(index==بەها_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    أرجع 0;
                }
                إذا((index-بەها_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, بەها_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                parseDouble(بەها_Index, index, خطأ,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                إذا(U_FAILURE(errorCode)) {
                    أرجع 0;
                }
                isEmpty=خطأ;
                استمر;  // no message fragment after the offset
            } وإلا {
                // normal selector word
                إذا(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    أرجع 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                إذا(0==پەیام.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=true;
                }
            }
        }
        إذا(U_FAILURE(errorCode)) {
            أرجع 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        إذا(index==پەیام.length() || پەیام.charAt(index)!=u_چەپ_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            أرجع 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        إذا(U_FAILURE(errorCode)) {
            أرجع 0;
        }
        isEmpty=خطأ;
    }
}

int32_t
MessagePattern::parse_Arg_ژمار(ثابت UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    إذا(start>=limit) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_ژمار;
    char16_t c=s.charAt(start++);
    إذا(c==0x30) {
        إذا(start==limit) {
            أرجع 0;
        } وإلا {
            number=0;
            bad_ژمار=true;  // leading zero
        }
    } وإلا إذا(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_ژمار=خطأ;
    } وإلا {
        أرجع UMSGPAT_ARG_NAME_NOT_ژمار;
    }
    طالما(start<limit) {
        c=s.charAt(start++);
        إذا(0x30<=c && c<=0x39) {
            إذا(number>=INT32_MAX/10) {
                bad_ژمار=true;  // overflow
            } وإلا {
                number=number*10+(c-0x30);
            }
        } وإلا {
            أرجع UMSGPAT_ARG_NAME_NOT_ژمار;
        }
    }
    // There are only ASCII digits.
    إذا(bad_ژمار) {
        أرجع UMSGPAT_ARG_NAME_NOT_VALID;
    } وإلا {
        أرجع number;
    }
}

فراغ
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    لكل(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=پەیام.charAt(index++);
        إذا(c==u_minus) {
            isNegative=1;
            إذا(index==limit) {
                اكسر;  // no number
            }
            c=پەیام.charAt(index++);
        } وإلا إذا(c==u_plus) {
            إذا(index==limit) {
                اكسر;  // no number
            }
            c=پەیام.charAt(index++);
        }
        إذا(c==0x221e) {  // infinity
            إذا(allowInfinity && index==limit) {
                مزدوج infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                أرجع;
            } وإلا {
                اكسر;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        طالما('0'<=c && c<='9') {
            value=value*10+(c-'0');
            إذا(value>(Part::MAX_بەها+isNegative)) {
                اكسر;  // not a small-enough integer
            }
            إذا(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                أرجع;
            }
            c=پەیام.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        حرف ژمار_Chars[128];
        int32_t capacity = static_cast<int32_t>(حجم(ژمار_Chars));
        int32_t length=limit-start;
        إذا(length>=capacity) {
            اكسر;  // number too long
        }
        پەیام.extract(start, length, ژمار_Chars, capacity, US_INV);
        إذا (static_cast<int32_t>(uprv_strlen(ژمار_Chars)) < length) {
            اكسر;  // contains non-invariant character that was turned into NUL
        }
        حرف *end;
        مزدوج numeric_بەها=uprv_strtod(ژمار_Chars, &end);
        إذا(end!=(ژمار_Chars+length)) {
            اكسر;  // parsing error
        }
        addArgDoublePart(numeric_بەها, start, length, errorCode);
        أرجع;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ثابت char16_t *s=پەیام.getBuffer();
    int32_t پەیام_Length=پەیام.length();
    ثابت char16_t *t=PatternProps::skipWhiteSpace(s+index, پەیام_Length-index);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ثابت char16_t *s=پەیام.getBuffer();
    int32_t پەیام_Length=پەیام.length();
    ثابت char16_t *t=PatternProps::skipIdentifier(s+index, پەیام_Length-index);
    أرجع static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t پەیام_Length=پەیام.length();
    طالما(index<پەیام_Length) {
        char16_t c=پەیام.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        إذا((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            اكسر;
        }
        ++index;
    }
    أرجع index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    أرجع (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    أرجع
        ((c=پەیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پەیام.charAt(index++))==u_h || c==u_H) &&
        ((c=پەیام.charAt(index++))==u_o || c==u_O) &&
        ((c=پەیام.charAt(index++))==u_i || c==u_I) &&
        ((c=پەیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پەیام.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    أرجع
        ((c=پەیام.charAt(index++))==u_p || c==u_P) &&
        ((c=پەیام.charAt(index++))==u_l || c==u_L) &&
        ((c=پەیام.charAt(index++))==u_u || c==u_U) &&
        ((c=پەیام.charAt(index++))==u_r || c==u_R) &&
        ((c=پەیام.charAt(index++))==u_a || c==u_A) &&
        ((c=پەیام.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    أرجع
        ((c=پەیام.charAt(index++))==u_s || c==u_S) &&
        ((c=پەیام.charAt(index++))==u_e || c==u_E) &&
        ((c=پەیام.charAt(index++))==u_l || c==u_L) &&
        ((c=پەیام.charAt(index++))==u_e || c==u_E) &&
        ((c=پەیام.charAt(index++))==u_c || c==u_C) &&
        ((c=پەیام.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    أرجع
        ((c=پەیام.charAt(index++))==u_o || c==u_O) &&
        ((c=پەیام.charAt(index++))==u_r || c==u_R) &&
        ((c=پەیام.charAt(index++))==u_d || c==u_D) &&
        ((c=پەیام.charAt(index++))==u_i || c==u_I) &&
        ((c=پەیام.charAt(index++))==u_n || c==u_N) &&
        ((c=پەیام.charAt(index++))==u_a || c==u_A) &&
        ((c=پەیام.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    أرجع nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_پەیام_START;
}

UBool
MessagePattern::in_لووتکە_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    أرجع
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_پەیام_START;
}

فراغ
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    إذا(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
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
MessagePattern::addArgDoublePart(مزدوج numeric_بەها, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    int32_t numericIndex=numeric_بەهاکان_Length;
    إذا(numeric_بەهاکان_List==nullptr) {
        numeric_بەهاکان_List=جديد MessagePatternDoubleList();
        إذا(numeric_بەهاکان_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع;
        }
    } وإلا إذا(!numeric_بەهاکان_List->ensureCapacityForOneMore(numeric_بەهاکان_Length, errorCode)) {
        أرجع;
    } وإلا {
        إذا(numericIndex>Part::MAX_بەها) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع;
        }
    }
    numeric_بەهاکان_List->a[numeric_بەهاکان_Length++]=numeric_بەها;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

فراغ
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    إذا(parseError==nullptr) {
        أرجع;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_TRAIL(پەیام[index-length])) {
            --length;
        }
    }
    پەیام.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=پەیام.length()-index;
    إذا(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        إذا(length>0 && U16_IS_LEAD(پەیام[index+length-1])) {
            --length;
        }
    }
    پەیام.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

فراغ
MessageImpl::appendReducedApostrophes(ثابت UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    لكل(;;) {
        int32_t i=s.indexOf(u_apos, start);
        إذا(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            اكسر;
        }
        إذا(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } وإلا {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ثابت MessagePattern& پەیام_Pattern,
                                               int32_t پەیام_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ثابت UnicodeString &پەیام_String=پەیام_Pattern.getPatternString();
    int32_t prevIndex=پەیام_Pattern.getPart(پەیام_Start).getLimit();
    لكل(int32_t i=پەیام_Start;;) {
        ثابت MessagePattern::Part &part=پەیام_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        إذا(type==UMSGPAT_PART_TYPE_پەیام_LIMIT) {
            أرجع result.append(پەیام_String, prevIndex, index-prevIndex);
        } وإلا إذا(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(پەیام_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } وإلا إذا(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(پەیام_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=پەیام_Pattern.getLimitPartIndex(i);
            index=پەیام_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(پەیام_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
