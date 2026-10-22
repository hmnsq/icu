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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_FORMATTING

#অন্তর্ভাব "unicode/messagepattern.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "messageimpl.h"
#অন্তর্ভাব "patternprops.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

স্থির ধ্রুব char16_t u_pound=0x23;
স্থির ধ্রুব char16_t u_apos=0x27;
স্থির ধ্রুব char16_t u_plus=0x2B;
স্থির ধ্রুব char16_t u_comma=0x2C;
স্থির ধ্রুব char16_t u_minus=0x2D;
স্থির ধ্রুব char16_t u_dot=0x2E;
স্থির ধ্রুব char16_t u_colon=0x3A;
স্থির ধ্রুব char16_t u_lessThan=0x3C;
স্থির ধ্রুব char16_t u_equal=0x3D;
স্থির ধ্রুব char16_t u_A=0x41;
স্থির ধ্রুব char16_t u_C=0x43;
স্থির ধ্রুব char16_t u_D=0x44;
স্থির ধ্রুব char16_t u_E=0x45;
স্থির ধ্রুব char16_t u_H=0x48;
স্থির ধ্রুব char16_t u_I=0x49;
স্থির ধ্রুব char16_t u_L=0x4C;
স্থির ধ্রুব char16_t u_N=0x4E;
স্থির ধ্রুব char16_t u_O=0x4F;
স্থির ধ্রুব char16_t u_P=0x50;
স্থির ধ্রুব char16_t u_R=0x52;
স্থির ধ্রুব char16_t u_S=0x53;
স্থির ধ্রুব char16_t u_T=0x54;
স্থির ধ্রুব char16_t u_U=0x55;
স্থির ধ্রুব char16_t u_Z=0x5A;
স্থির ধ্রুব char16_t u_a=0x61;
স্থির ধ্রুব char16_t u_c=0x63;
স্থির ধ্রুব char16_t u_d=0x64;
স্থির ধ্রুব char16_t u_e=0x65;
স্থির ধ্রুব char16_t u_f=0x66;
স্থির ধ্রুব char16_t u_h=0x68;
স্থির ধ্রুব char16_t u_i=0x69;
স্থির ধ্রুব char16_t u_l=0x6C;
স্থির ধ্রুব char16_t u_n=0x6E;
স্থির ধ্রুব char16_t u_o=0x6F;
স্থির ধ্রুব char16_t u_p=0x70;
স্থির ধ্রুব char16_t u_r=0x72;
স্থির ধ্রুব char16_t u_s=0x73;
স্থির ধ্রুব char16_t u_t=0x74;
স্থির ধ্রুব char16_t u_u=0x75;
স্থির ধ্রুব char16_t u_z=0x7A;
স্থির ধ্রুব char16_t u_বাম_Curly_Brace=0x7B;
স্থির ধ্রুব char16_t u_pipe=0x7C;
স্থির ধ্রুব char16_t u_ডান_Curly_Brace=0x7D;
স্থির ধ্রুব char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

স্থির ধ্রুব char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

স্থির ধ্রুব char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t স্তূপ_Capacity>
বর্গ MessagePatternList : সার্বজনিক UMemory {
সার্বজনিক:
    MessagePatternList() {}
    শূন্য copyFrom(ধ্রুব MessagePatternList<T, স্তূপ_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(ধ্রুব MessagePatternList<T, স্তূপ_Capacity> &other, int32_t length) ধ্রুব {
        প্রতি(int32_t i=0; i<length; ++i) {
            যদি(a[i]!=other.a[i]) { প্রত্যাগম অসত্য; }
        }
        প্রত্যাগম সত্য;
    }

    Maybe_স্তূপ_Array<T, স্তূপ_Capacity> a;
};

template<typename T, int32_t স্তূপ_Capacity>
শূন্য
MessagePatternList<T, স্তূপ_Capacity>::copyFrom(
        ধ্রুব MessagePatternList<T, স্তূপ_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    যদি(U_SUCCESS(errorCode) && length>0) {
        যদি(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*পরিমাণ(T));
    }
}

template<typename T, int32_t স্তূপ_Capacity>
UBool
MessagePatternList<T, স্তূপ_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম অসত্য;
    }
    যদি(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        প্রত্যাগম সত্য;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    প্রত্যাগম অসত্য;
}

// MessagePatternList specializations -------------------------------------- ***

বর্গ MessagePatternDoubleList : সার্বজনিক MessagePatternList<দ্বিগুণ, 8> {
};

বর্গ MessagePatternPartsList : সার্বজনিক MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_Length(0),
          hasArgNames(অসত্য), hasArgNumbers(অসত্য), needsAutoQuoting(অসত্য) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_Length(0),
          hasArgNames(অসত্য), hasArgNumbers(অসত্য), needsAutoQuoting(অসত্য) {
    init(errorCode);
}

MessagePattern::MessagePattern(ধ্রুব UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_Length(0),
          hasArgNames(অসত্য), hasArgNumbers(অসত্য), needsAutoQuoting(অসত্য) {
    যদি(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম অসত্য;
    }
    partsList=নূতন MessagePatternPartsList();
    যদি(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম অসত্য;
    }
    parts=partsList->a.getAlias();
    প্রত্যাগম সত্য;
}

MessagePattern::MessagePattern(ধ্রুব MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), বার্তা(other.বার্তা),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    যদি(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ধ্রুব MessagePattern& other) U_LIFETIME_BOUND {
    যদি(এতত্==&other) {
        প্রত্যাগম *এতত্;
    }
    aposMode=other.aposMode;
    বার্তা=other.বার্তা;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    যদি(!copyStorage(other, errorCode)) {
        clear();
    }
    প্রত্যাগম *এতত্;
}

UBool
MessagePattern::copyStorage(ধ্রুব MessagePattern &other, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম অসত্য;
    }
    parts=nullptr;
    partsLength=0;
    numeric_মানসমূহ=nullptr;
    numeric_মানসমূহ_Length=0;
    যদি(partsList==nullptr) {
        partsList=নূতন MessagePatternPartsList();
        যদি(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম অসত্য;
        }
        parts=partsList->a.getAlias();
    }
    যদি(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম অসত্য;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    যদি(other.numeric_মানসমূহ_Length>0) {
        যদি(numeric_মানসমূহ_List==nullptr) {
            numeric_মানসমূহ_List=নূতন MessagePatternDoubleList();
            যদি(numeric_মানসমূহ_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম অসত্য;
            }
            numeric_মানসমূহ=numeric_মানসমূহ_List->a.getAlias();
        }
        numeric_মানসমূহ_List->copyFrom(
            *other.numeric_মানসমূহ_List, other.numeric_মানসমূহ_Length, errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম অসত্য;
        }
        numeric_মানসমূহ=numeric_মানসমূহ_List->a.getAlias();
        numeric_মানসমূহ_Length=other.numeric_মানসমূহ_Length;
    }
    প্রত্যাগম সত্য;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_মানসমূহ_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ধ্রুব UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    প্রত্যাগম *এতত্;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ধ্রুব UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    প্রত্যাগম *এতত্;
}

MessagePattern&
MessagePattern::parsePluralStyle(ধ্রুব UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    প্রত্যাগম *এতত্;
}

MessagePattern&
MessagePattern::parseSelectStyle(ধ্রুব UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    প্রত্যাগম *এতত্;
}

শূন্য
MessagePattern::clear() {
    // Mostly the same as preParse().
    বার্তা.remove();
    hasArgNames=hasArgNumbers=অসত্য;
    needsAutoQuoting=অসত্য;
    partsLength=0;
    numeric_মানসমূহ_Length=0;
}

তার্কিক
MessagePattern::operator==(ধ্রুব MessagePattern &other) ধ্রুব {
    যদি(এতত্==&other) {
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম
        aposMode==other.aposMode &&
        বার্তা==other.বার্তা &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ধ্রুব {
    int32_t hash=(aposMode*37+বার্তা.hashCode())*37+partsLength;
    প্রতি(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    প্রত্যাগম hash;
}

int32_t
MessagePattern::validateArgumentName(ধ্রুব UnicodeString &name) {
    যদি(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        প্রত্যাগম UMSGPAT_ARG_NAME_NOT_VALID;
    }
    প্রত্যাগম parse_Arg_সংখ্যা(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ধ্রুব {
    যদি(!needsAutoQuoting) {
        প্রত্যাগম বার্তা;
    }
    UnicodeString modified(বার্তা);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=গুনতি_Parts();
    প্রতি(int32_t i=count; i>0;) {
        ধ্রুব Part &part=getPart(--i);
        যদি(part.getType()==UMSGPAT_PART_TYPE_ঢোকাও_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    প্রত্যাগম modified;
}

দ্বিগুণ
MessagePattern::get_Numeric_মান(ধ্রুব Part &part) ধ্রুব {
    UMessagePatternPartType type=part.type;
    যদি(type==UMSGPAT_PART_TYPE_ARG_INT) {
        প্রত্যাগম part.value;
    } অন্যথা যদি(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        প্রত্যাগম numeric_মানসমূহ[part.value];
    } অন্যথা {
        প্রত্যাগম UMSGPAT_NO_NUMERIC_মান;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
দ্বিগুণ
MessagePattern::getPluralOffset(int32_t pluralStart) ধ্রুব {
    ধ্রুব Part &part=getPart(pluralStart);
    যদি(Part::has_Numeric_মান(part.type)) {
        প্রত্যাগম get_Numeric_মান(part);
    } অন্যথা {
        প্রত্যাগম 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

তার্কিক
MessagePattern::Part::operator==(ধ্রুব Part &other) ধ্রুব {
    যদি(এতত্==&other) {
        প্রত্যাগম সত্য;
    }
    প্রত্যাগম
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

শূন্য
MessagePattern::preParse(ধ্রুব UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    যদি(parseError!=nullptr) {
        parseError->সারি=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    বার্তা=pattern;
    hasArgNames=hasArgNumbers=অসত্য;
    needsAutoQuoting=অসত্য;
    partsLength=0;
    numeric_মানসমূহ_Length=0;
}

শূন্য
MessagePattern::postParse() {
    যদি(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    যদি(numeric_মানসমূহ_List!=nullptr) {
        numeric_মানসমূহ=numeric_মানসমূহ_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t বার্তা_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    যদি(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        প্রত্যাগম 0;
    }
    int32_t বার্তা_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_বার্তা_START, index, বার্তা_Start_Length, nestingLevel, errorCode);
    index+=বার্তা_Start_Length;
    প্রতি(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
        যদি(index>=বার্তা.length()) {
            ভঙ্গ;
        }
        char16_t c=বার্তা.charAt(index++);
        যদি(c==u_apos) {
            যদি(index==বার্তা.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=সত্য;
            } অন্যথা {
                c=বার্তা.charAt(index);
                যদি(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } অন্যথা যদি(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_বাম_Curly_Brace || c==u_ডান_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    প্রতি(;;) {
                        index=বার্তা.indexOf(u_apos, index+1);
                        যদি(index>=0) {
                            যদি(/*(index+1)<msg.length() &&*/ বার্তা.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } অন্যথা {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                ভঙ্গ;
                            }
                        } অন্যথা {
                            // The quoted text reaches to the end of the of the message.
                            index=বার্তা.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=সত্য;
                            ভঙ্গ;
                        }
                    }
                } অন্যথা {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=সত্য;
                }
            }
        } অন্যথা যদি(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_সংখ্যা, index-1, 1, 0, errorCode);
        } অন্যথা যদি(c==u_বাম_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } অন্যথা যদি((nestingLevel>0 && c==u_ডান_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_ডান_Curly_Brace) ? 0 : 1;
            addLimitPart(বার্তা_Start, UMSGPAT_PART_TYPE_বার্তা_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            যদি(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                প্রত্যাগম index-1;
            } অন্যথা {
                // continue parsing after the '}'
                প্রত্যাগম index;
            }
        }  // else: c is part of literal text
    }
    যদি(nestingLevel>0 && !in_শীর্ষ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        প্রত্যাগম 0;
    }
    addLimitPart(বার্তা_Start, UMSGPAT_PART_TYPE_বার্তা_LIMIT, index, 0, nestingLevel, errorCode);
    প্রত্যাগম index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    যদি(index==বার্তা.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        প্রত্যাগম 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_সংখ্যা(nameIndex, index);
    যদি(number>=0) {
        int32_t length=index-nameIndex;
        যদি(length>Part::MAX_LENGTH || number>Part::MAX_মান) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
        hasArgNumbers=সত্য;
        addPart(UMSGPAT_PART_TYPE_ARG_সংখ্যা, nameIndex, length, number, errorCode);
    } অন্যথা যদি(number==UMSGPAT_ARG_NAME_NOT_সংখ্যা) {
        int32_t length=index-nameIndex;
        যদি(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
        hasArgNames=সত্য;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } অন্যথা {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        প্রত্যাগম 0;
    }
    index=skipWhiteSpace(index);
    যদি(index==বার্তা.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        প্রত্যাগম 0;
    }
    char16_t c=বার্তা.charAt(index);
    যদি(c==u_ডান_Curly_Brace) {
        // all done
    } অন্যথা যদি(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        প্রত্যাগম 0;
    } অন্যথা /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        যাবত্(index<বার্তা.length() && isArgTypeChar(বার্তা.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        যদি(index==বার্তা.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            প্রত্যাগম 0;
        }
        যদি(length==0 || ((c=বার্তা.charAt(index))!=u_comma && c!=u_ডান_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            প্রত্যাগম 0;
        }
        যদি(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        যদি(length==6) {
            // case-insensitive comparisons for complex-type names
            যদি(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } অন্যথা যদি(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } অন্যথা যদি(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } অন্যথা যদি(length==13) {
            যদি(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        যদি(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        যদি(c==u_ডান_Curly_Brace) {
            যদি(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
        } অন্যথা /* ',' */ {
            ++index;
            যদি(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } অন্যথা যদি(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } অন্যথা {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    প্রত্যাগম index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    যাবত্(index<বার্তা.length()) {
        char16_t c=বার্তা.charAt(index++);
        যদি(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=বার্তা.indexOf(u_apos, index);
            যদি(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } অন্যথা যদি(c==u_বাম_Curly_Brace) {
            ++nestedBraces;
        } অন্যথা যদি(c==u_ডান_Curly_Brace) {
            যদি(nestedBraces>0) {
                --nestedBraces;
            } অন্যথা {
                int32_t length=--index-start;
                যদি(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    প্রত্যাগম 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                প্রত্যাগম index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    প্রত্যাগম 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    যদি(index==বার্তা.length() || বার্তা.charAt(index)==u_ডান_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        প্রত্যাগম 0;
    }
    প্রতি(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t সংখ্যা_Index=index;
        index=skipDouble(index);
        int32_t length=index-সংখ্যা_Index;
        যদি(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            প্রত্যাগম 0;
        }
        যদি(length>Part::MAX_LENGTH) {
            setParseError(parseError, সংখ্যা_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
        parseDouble(সংখ্যা_Index, index, সত্য, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        যদি(index==বার্তা.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            প্রত্যাগম 0;
        }
        char16_t c=বার্তা.charAt(index);
        যদি(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            প্রত্যাগম 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        যদি(index==বার্তা.length()) {
            প্রত্যাগম index;
        }
        যদি(বার্তা.charAt(index)==u_ডান_Curly_Brace) {
            যদি(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
            প্রত্যাগম index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম 0;
    }
    int32_t start=index;
    UBool isEmpty=সত্য;
    UBool hasOther=অসত্য;
    প্রতি(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==বার্তা.length();
        যদি(eos || বার্তা.charAt(index)==u_ডান_Curly_Brace) {
            যদি(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
            যদি(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                প্রত্যাগম 0;
            }
            প্রত্যাগম index;
        }
        int32_t selectorIndex=index;
        যদি(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && বার্তা.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            যদি(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
            যদি(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                প্রত্যাগম 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, অসত্য,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } অন্যথা {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            যদি(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                প্রত্যাগম 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            যদি( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<বার্তা.length() &&
                0==বার্তা.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                যদি(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    প্রত্যাগম 0;
                }
                // allow whitespace between offset: and its value
                int32_t মান_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(মান_Index);
                যদি(index==মান_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    প্রত্যাগম 0;
                }
                যদি((index-মান_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, মান_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    প্রত্যাগম 0;
                }
                parseDouble(মান_Index, index, অসত্য,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                যদি(U_FAILURE(errorCode)) {
                    প্রত্যাগম 0;
                }
                isEmpty=অসত্য;
                অনুবর্ত;  // no message fragment after the offset
            } অন্যথা {
                // normal selector word
                যদি(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    প্রত্যাগম 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                যদি(0==বার্তা.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=সত্য;
                }
            }
        }
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        যদি(index==বার্তা.length() || বার্তা.charAt(index)!=u_বাম_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            প্রত্যাগম 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        যদি(U_FAILURE(errorCode)) {
            প্রত্যাগম 0;
        }
        isEmpty=অসত্য;
    }
}

int32_t
MessagePattern::parse_Arg_সংখ্যা(ধ্রুব UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    যদি(start>=limit) {
        প্রত্যাগম UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_সংখ্যা;
    char16_t c=s.charAt(start++);
    যদি(c==0x30) {
        যদি(start==limit) {
            প্রত্যাগম 0;
        } অন্যথা {
            number=0;
            bad_সংখ্যা=সত্য;  // leading zero
        }
    } অন্যথা যদি(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_সংখ্যা=অসত্য;
    } অন্যথা {
        প্রত্যাগম UMSGPAT_ARG_NAME_NOT_সংখ্যা;
    }
    যাবত্(start<limit) {
        c=s.charAt(start++);
        যদি(0x30<=c && c<=0x39) {
            যদি(number>=INT32_MAX/10) {
                bad_সংখ্যা=সত্য;  // overflow
            } অন্যথা {
                number=number*10+(c-0x30);
            }
        } অন্যথা {
            প্রত্যাগম UMSGPAT_ARG_NAME_NOT_সংখ্যা;
        }
    }
    // There are only ASCII digits.
    যদি(bad_সংখ্যা) {
        প্রত্যাগম UMSGPAT_ARG_NAME_NOT_VALID;
    } অন্যথা {
        প্রত্যাগম number;
    }
}

শূন্য
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    প্রতি(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=বার্তা.charAt(index++);
        যদি(c==u_minus) {
            isNegative=1;
            যদি(index==limit) {
                ভঙ্গ;  // no number
            }
            c=বার্তা.charAt(index++);
        } অন্যথা যদি(c==u_plus) {
            যদি(index==limit) {
                ভঙ্গ;  // no number
            }
            c=বার্তা.charAt(index++);
        }
        যদি(c==0x221e) {  // infinity
            যদি(allowInfinity && index==limit) {
                দ্বিগুণ infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                প্রত্যাগম;
            } অন্যথা {
                ভঙ্গ;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        যাবত্('0'<=c && c<='9') {
            value=value*10+(c-'0');
            যদি(value>(Part::MAX_মান+isNegative)) {
                ভঙ্গ;  // not a small-enough integer
            }
            যদি(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                প্রত্যাগম;
            }
            c=বার্তা.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        বর্ণ সংখ্যা_Chars[128];
        int32_t capacity = static_cast<int32_t>(পরিমাণ(সংখ্যা_Chars));
        int32_t length=limit-start;
        যদি(length>=capacity) {
            ভঙ্গ;  // number too long
        }
        বার্তা.extract(start, length, সংখ্যা_Chars, capacity, US_INV);
        যদি (static_cast<int32_t>(uprv_strlen(সংখ্যা_Chars)) < length) {
            ভঙ্গ;  // contains non-invariant character that was turned into NUL
        }
        বর্ণ *end;
        দ্বিগুণ numeric_মান=uprv_strtod(সংখ্যা_Chars, &end);
        যদি(end!=(সংখ্যা_Chars+length)) {
            ভঙ্গ;  // parsing error
        }
        addArgDoublePart(numeric_মান, start, length, errorCode);
        প্রত্যাগম;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    ধ্রুব char16_t *s=বার্তা.getBuffer();
    int32_t বার্তা_Length=বার্তা.length();
    ধ্রুব char16_t *t=PatternProps::skipWhiteSpace(s+index, বার্তা_Length-index);
    প্রত্যাগম static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    ধ্রুব char16_t *s=বার্তা.getBuffer();
    int32_t বার্তা_Length=বার্তা.length();
    ধ্রুব char16_t *t=PatternProps::skipIdentifier(s+index, বার্তা_Length-index);
    প্রত্যাগম static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t বার্তা_Length=বার্তা.length();
    যাবত্(index<বার্তা_Length) {
        char16_t c=বার্তা.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        যদি((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ভঙ্গ;
        }
        ++index;
    }
    প্রত্যাগম index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    প্রত্যাগম (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    প্রত্যাগম
        ((c=বার্তা.charAt(index++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(index++))==u_h || c==u_H) &&
        ((c=বার্তা.charAt(index++))==u_o || c==u_O) &&
        ((c=বার্তা.charAt(index++))==u_i || c==u_I) &&
        ((c=বার্তা.charAt(index++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    প্রত্যাগম
        ((c=বার্তা.charAt(index++))==u_p || c==u_P) &&
        ((c=বার্তা.charAt(index++))==u_l || c==u_L) &&
        ((c=বার্তা.charAt(index++))==u_u || c==u_U) &&
        ((c=বার্তা.charAt(index++))==u_r || c==u_R) &&
        ((c=বার্তা.charAt(index++))==u_a || c==u_A) &&
        ((c=বার্তা.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    প্রত্যাগম
        ((c=বার্তা.charAt(index++))==u_s || c==u_S) &&
        ((c=বার্তা.charAt(index++))==u_e || c==u_E) &&
        ((c=বার্তা.charAt(index++))==u_l || c==u_L) &&
        ((c=বার্তা.charAt(index++))==u_e || c==u_E) &&
        ((c=বার্তা.charAt(index++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    প্রত্যাগম
        ((c=বার্তা.charAt(index++))==u_o || c==u_O) &&
        ((c=বার্তা.charAt(index++))==u_r || c==u_R) &&
        ((c=বার্তা.charAt(index++))==u_d || c==u_D) &&
        ((c=বার্তা.charAt(index++))==u_i || c==u_I) &&
        ((c=বার্তা.charAt(index++))==u_n || c==u_N) &&
        ((c=বার্তা.charAt(index++))==u_a || c==u_A) &&
        ((c=বার্তা.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    প্রত্যাগম nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_বার্তা_START;
}

UBool
MessagePattern::in_শীর্ষ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    প্রত্যাগম
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_বার্তা_START;
}

শূন্য
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    যদি(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

শূন্য
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

শূন্য
MessagePattern::addArgDoublePart(দ্বিগুণ numeric_মান, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    int32_t numericIndex=numeric_মানসমূহ_Length;
    যদি(numeric_মানসমূহ_List==nullptr) {
        numeric_মানসমূহ_List=নূতন MessagePatternDoubleList();
        যদি(numeric_মানসমূহ_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম;
        }
    } অন্যথা যদি(!numeric_মানসমূহ_List->ensureCapacityForOneMore(numeric_মানসমূহ_Length, errorCode)) {
        প্রত্যাগম;
    } অন্যথা {
        যদি(numericIndex>Part::MAX_মান) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম;
        }
    }
    numeric_মানসমূহ_List->a[numeric_মানসমূহ_Length++]=numeric_মান;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

শূন্য
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    যদি(parseError==nullptr) {
        প্রত্যাগম;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    যদি(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        যদি(length>0 && U16_IS_TRAIL(বার্তা[index-length])) {
            --length;
        }
    }
    বার্তা.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=বার্তা.length()-index;
    যদি(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        যদি(length>0 && U16_IS_LEAD(বার্তা[index+length-1])) {
            --length;
        }
    }
    বার্তা.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

শূন্য
MessageImpl::appendReducedApostrophes(ধ্রুব UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    প্রতি(;;) {
        int32_t i=s.indexOf(u_apos, start);
        যদি(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            ভঙ্গ;
        }
        যদি(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } অন্যথা {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(ধ্রুব MessagePattern& বার্তা_Pattern,
                                               int32_t বার্তা_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    ধ্রুব UnicodeString &বার্তা_String=বার্তা_Pattern.getPatternString();
    int32_t prevIndex=বার্তা_Pattern.getPart(বার্তা_Start).getLimit();
    প্রতি(int32_t i=বার্তা_Start;;) {
        ধ্রুব MessagePattern::Part &part=বার্তা_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        যদি(type==UMSGPAT_PART_TYPE_বার্তা_LIMIT) {
            প্রত্যাগম result.append(বার্তা_String, prevIndex, index-prevIndex);
        } অন্যথা যদি(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(বার্তা_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } অন্যথা যদি(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(বার্তা_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=বার্তা_Pattern.getLimitPartIndex(i);
            index=বার্তা_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(বার্তা_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
