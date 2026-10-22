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

স্থির ধ্রুবক char16_t u_pound=0x23;
স্থির ধ্রুবক char16_t u_apos=0x27;
স্থির ধ্রুবক char16_t u_plus=0x2B;
স্থির ধ্রুবক char16_t u_comma=0x2C;
স্থির ধ্রুবক char16_t u_minus=0x2D;
স্থির ধ্রুবক char16_t u_dot=0x2E;
স্থির ধ্রুবক char16_t u_colon=0x3A;
স্থির ধ্রুবক char16_t u_lessThan=0x3C;
স্থির ধ্রুবক char16_t u_equal=0x3D;
স্থির ধ্রুবক char16_t u_A=0x41;
স্থির ধ্রুবক char16_t u_C=0x43;
স্থির ধ্রুবক char16_t u_D=0x44;
স্থির ধ্রুবক char16_t u_E=0x45;
স্থির ধ্রুবক char16_t u_H=0x48;
স্থির ধ্রুবক char16_t u_I=0x49;
স্থির ধ্রুবক char16_t u_L=0x4C;
স্থির ধ্রুবক char16_t u_N=0x4E;
স্থির ধ্রুবক char16_t u_O=0x4F;
স্থির ধ্রুবক char16_t u_P=0x50;
স্থির ধ্রুবক char16_t u_R=0x52;
স্থির ধ্রুবক char16_t u_S=0x53;
স্থির ধ্রুবক char16_t u_T=0x54;
স্থির ধ্রুবক char16_t u_U=0x55;
স্থির ধ্রুবক char16_t u_Z=0x5A;
স্থির ধ্রুবক char16_t u_a=0x61;
স্থির ধ্রুবক char16_t u_c=0x63;
স্থির ধ্রুবক char16_t u_d=0x64;
স্থির ধ্রুবক char16_t u_e=0x65;
স্থির ধ্রুবক char16_t u_f=0x66;
স্থির ধ্রুবক char16_t u_h=0x68;
স্থির ধ্রুবক char16_t u_i=0x69;
স্থির ধ্রুবক char16_t u_l=0x6C;
স্থির ধ্রুবক char16_t u_n=0x6E;
স্থির ধ্রুবক char16_t u_o=0x6F;
স্থির ধ্রুবক char16_t u_p=0x70;
স্থির ধ্রুবক char16_t u_r=0x72;
স্থির ধ্রুবক char16_t u_s=0x73;
স্থির ধ্রুবক char16_t u_t=0x74;
স্থির ধ্রুবক char16_t u_u=0x75;
স্থির ধ্রুবক char16_t u_z=0x7A;
স্থির ধ্রুবক char16_t u_বাম_Curly_Brace=0x7B;
স্থির ধ্রুবক char16_t u_pipe=0x7C;
স্থির ধ্রুবক char16_t u_ডান_Curly_Brace=0x7D;
স্থির ধ্রুবক char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

স্থির ধ্রুবক char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

স্থির ধ্রুবক char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t স্তূপ_Capacity>
শ্রেণী MessagePatternList : সর্বজনীন UMemory {
সর্বজনীন:
    MessagePatternList() {}
    শূন্য copyFrom(ধ্রুবক MessagePatternList<T, স্তূপ_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t old_দৈর্ঘ্য, UErrorCode &errorCode);
    UBool equals(ধ্রুবক MessagePatternList<T, স্তূপ_Capacity> &other, int32_t length) ধ্রুবক {
        জন্য(int32_t i=0; i<length; ++i) {
            যদি(a[i]!=other.a[i]) { ফেরত মিথ্যা; }
        }
        ফেরত সত্য;
    }

    Maybe_স্তূপ_Array<T, স্তূপ_Capacity> a;
};

template<typename T, int32_t স্তূপ_Capacity>
শূন্য
MessagePatternList<T, স্তূপ_Capacity>::copyFrom(
        ধ্রুবক MessagePatternList<T, স্তূপ_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    যদি(U_SUCCESS(errorCode) && length>0) {
        যদি(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*আকার(T));
    }
}

template<typename T, int32_t স্তূপ_Capacity>
UBool
MessagePatternList<T, স্তূপ_Capacity>::ensureCapacityForOneMore(int32_t old_দৈর্ঘ্য, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    যদি(a.getCapacity()>old_দৈর্ঘ্য || a.resize(2*old_দৈর্ঘ্য, old_দৈর্ঘ্য)!=nullptr) {
        ফেরত সত্য;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    ফেরত মিথ্যা;
}

// MessagePatternList specializations -------------------------------------- ***

শ্রেণী MessagePatternDoubleList : সর্বজনীন MessagePatternList<দ্বিগুণ, 8> {
};

শ্রেণী MessagePatternPartsList : সর্বজনীন MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), parts_দৈর্ঘ্য(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_দৈর্ঘ্য(0),
          hasArgNames(মিথ্যা), hasArgNumbers(মিথ্যা), needsAutoQuoting(মিথ্যা) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), parts_দৈর্ঘ্য(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_দৈর্ঘ্য(0),
          hasArgNames(মিথ্যা), hasArgNumbers(মিথ্যা), needsAutoQuoting(মিথ্যা) {
    init(errorCode);
}

MessagePattern::MessagePattern(ধ্রুবক UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), parts_দৈর্ঘ্য(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_দৈর্ঘ্য(0),
          hasArgNames(মিথ্যা), hasArgNumbers(মিথ্যা), needsAutoQuoting(মিথ্যা) {
    যদি(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    partsList=নতুন MessagePatternPartsList();
    যদি(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত মিথ্যা;
    }
    parts=partsList->a.getAlias();
    ফেরত সত্য;
}

MessagePattern::MessagePattern(ধ্রুবক MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), বার্তা(other.বার্তা),
          partsList(nullptr), parts(nullptr), parts_দৈর্ঘ্য(0),
          numeric_মানসমূহ_List(nullptr), numeric_মানসমূহ(nullptr), numeric_মানসমূহ_দৈর্ঘ্য(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    যদি(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(ধ্রুবক MessagePattern& other) U_LIFETIME_BOUND {
    যদি(এই==&other) {
        ফেরত *এই;
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
    ফেরত *এই;
}

UBool
MessagePattern::copyStorage(ধ্রুবক MessagePattern &other, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত মিথ্যা;
    }
    parts=nullptr;
    parts_দৈর্ঘ্য=0;
    numeric_মানসমূহ=nullptr;
    numeric_মানসমূহ_দৈর্ঘ্য=0;
    যদি(partsList==nullptr) {
        partsList=নতুন MessagePatternPartsList();
        যদি(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত মিথ্যা;
        }
        parts=partsList->a.getAlias();
    }
    যদি(other.parts_দৈর্ঘ্য>0) {
        partsList->copyFrom(*other.partsList, other.parts_দৈর্ঘ্য, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত মিথ্যা;
        }
        parts=partsList->a.getAlias();
        parts_দৈর্ঘ্য=other.parts_দৈর্ঘ্য;
    }
    যদি(other.numeric_মানসমূহ_দৈর্ঘ্য>0) {
        যদি(numeric_মানসমূহ_List==nullptr) {
            numeric_মানসমূহ_List=নতুন MessagePatternDoubleList();
            যদি(numeric_মানসমূহ_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                ফেরত মিথ্যা;
            }
            numeric_মানসমূহ=numeric_মানসমূহ_List->a.getAlias();
        }
        numeric_মানসমূহ_List->copyFrom(
            *other.numeric_মানসমূহ_List, other.numeric_মানসমূহ_দৈর্ঘ্য, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত মিথ্যা;
        }
        numeric_মানসমূহ=numeric_মানসমূহ_List->a.getAlias();
        numeric_মানসমূহ_দৈর্ঘ্য=other.numeric_মানসমূহ_দৈর্ঘ্য;
    }
    ফেরত সত্য;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_মানসমূহ_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(ধ্রুবক UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    ফেরত *এই;
}

MessagePattern&
MessagePattern::parseChoiceStyle(ধ্রুবক UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    ফেরত *এই;
}

MessagePattern&
MessagePattern::parsePluralStyle(ধ্রুবক UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    ফেরত *এই;
}

MessagePattern&
MessagePattern::parseSelectStyle(ধ্রুবক UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    ফেরত *এই;
}

শূন্য
MessagePattern::clear() {
    // Mostly the same as preParse().
    বার্তা.remove();
    hasArgNames=hasArgNumbers=মিথ্যা;
    needsAutoQuoting=মিথ্যা;
    parts_দৈর্ঘ্য=0;
    numeric_মানসমূহ_দৈর্ঘ্য=0;
}

যৌক্তিক
MessagePattern::operator==(ধ্রুবক MessagePattern &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    ফেরত
        aposMode==other.aposMode &&
        বার্তা==other.বার্তা &&
        // parts.equals(o.parts)
        parts_দৈর্ঘ্য==other.parts_দৈর্ঘ্য &&
        (parts_দৈর্ঘ্য==0 || partsList->equals(*other.partsList, parts_দৈর্ঘ্য));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() ধ্রুবক {
    int32_t hash=(aposMode*37+বার্তা.hashCode())*37+parts_দৈর্ঘ্য;
    জন্য(int32_t i=0; i<parts_দৈর্ঘ্য; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    ফেরত hash;
}

int32_t
MessagePattern::validateArgumentName(ধ্রুবক UnicodeString &name) {
    যদি(!PatternProps::isIdentifier(name.get_বাফার(), name.length())) {
        ফেরত UMSGPAT_ARG_NAME_NOT_VALID;
    }
    ফেরত parse_Arg_সংখ্যা(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() ধ্রুবক {
    যদি(!needsAutoQuoting) {
        ফেরত বার্তা;
    }
    UnicodeString modified(বার্তা);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=গুনতি_Parts();
    জন্য(int32_t i=count; i>0;) {
        ধ্রুবক Part &part=getPart(--i);
        যদি(part.getType()==UMSGPAT_PART_TYPE_ঢোকাও_CHAR) {
           modified.insert(part.সূচক, static_cast<char16_t>(part.value));
        }
    }
    ফেরত modified;
}

দ্বিগুণ
MessagePattern::get_Numeric_মান(ধ্রুবক Part &part) ধ্রুবক {
    UMessagePatternPartType type=part.type;
    যদি(type==UMSGPAT_PART_TYPE_ARG_INT) {
        ফেরত part.value;
    } অন্যথা যদি(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        ফেরত numeric_মানসমূহ[part.value];
    } অন্যথা {
        ফেরত UMSGPAT_NO_NUMERIC_মান;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
দ্বিগুণ
MessagePattern::getPluralOffset(int32_t pluralStart) ধ্রুবক {
    ধ্রুবক Part &part=getPart(pluralStart);
    যদি(Part::has_Numeric_মান(part.type)) {
        ফেরত get_Numeric_মান(part);
    } অন্যথা {
        ফেরত 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

যৌক্তিক
MessagePattern::Part::operator==(ধ্রুবক Part &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    ফেরত
        type==other.type &&
        সূচক==other.সূচক &&
        length==other.length &&
        value==other.value &&
        limit_Part_সূচক==other.limit_Part_সূচক;
}

// MessagePattern parser --------------------------------------------------- ***

শূন্য
MessagePattern::preParse(ধ্রুবক UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    যদি(parseError!=nullptr) {
        parseError->সারি=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    বার্তা=pattern;
    hasArgNames=hasArgNumbers=মিথ্যা;
    needsAutoQuoting=মিথ্যা;
    parts_দৈর্ঘ্য=0;
    numeric_মানসমূহ_দৈর্ঘ্য=0;
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
MessagePattern::parseMessage(int32_t সূচক, int32_t বার্তা_Start_দৈর্ঘ্য,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    যদি(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
        ফেরত 0;
    }
    int32_t বার্তা_Start=parts_দৈর্ঘ্য;
    addPart(UMSGPAT_PART_TYPE_বার্তা_START, সূচক, বার্তা_Start_দৈর্ঘ্য, nestingLevel, errorCode);
    সূচক+=বার্তা_Start_দৈর্ঘ্য;
    জন্য(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
        যদি(সূচক>=বার্তা.length()) {
            বিরতি;
        }
        char16_t c=বার্তা.charAt(সূচক++);
        যদি(c==u_apos) {
            যদি(সূচক==বার্তা.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, সূচক, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=সত্য;
            } অন্যথা {
                c=বার্তা.charAt(সূচক);
                যদি(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, সূচক++, 1, 0, errorCode);
                } অন্যথা যদি(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_বাম_Curly_Brace || c==u_ডান_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, সূচক-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    জন্য(;;) {
                        সূচক=বার্তা.সূচক_Of(u_apos, সূচক+1);
                        যদি(সূচক>=0) {
                            যদি(/*(index+1)<msg.length() &&*/ বার্তা.charAt(সূচক+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++সূচক, 1, 0, errorCode);
                            } অন্যথা {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, সূচক++, 1, 0, errorCode);
                                বিরতি;
                            }
                        } অন্যথা {
                            // The quoted text reaches to the end of the of the message.
                            সূচক=বার্তা.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, সূচক, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=সত্য;
                            বিরতি;
                        }
                    }
                } অন্যথা {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_ঢোকাও_CHAR, সূচক, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=সত্য;
                }
            }
        } অন্যথা যদি(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_সংখ্যা, সূচক-1, 1, 0, errorCode);
        } অন্যথা যদি(c==u_বাম_Curly_Brace) {
            সূচক=parseArg(সূচক-1, 1, nestingLevel, parseError, errorCode);
        } অন্যথা যদি((nestingLevel>0 && c==u_ডান_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limit_দৈর্ঘ্য=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_ডান_Curly_Brace) ? 0 : 1;
            addLimitPart(বার্তা_Start, UMSGPAT_PART_TYPE_বার্তা_LIMIT, সূচক-1, limit_দৈর্ঘ্য,
                         nestingLevel, errorCode);
            যদি(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                ফেরত সূচক-1;
            } অন্যথা {
                // continue parsing after the '}'
                ফেরত সূচক;
            }
        }  // else: c is part of literal text
    }
    যদি(nestingLevel>0 && !in_শীর্ষ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ফেরত 0;
    }
    addLimitPart(বার্তা_Start, UMSGPAT_PART_TYPE_বার্তা_LIMIT, সূচক, 0, nestingLevel, errorCode);
    ফেরত সূচক;
}

int32_t
MessagePattern::parseArg(int32_t সূচক, int32_t arg_Start_দৈর্ঘ্য, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=parts_দৈর্ঘ্য;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, সূচক, arg_Start_দৈর্ঘ্য, argType, errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    int32_t name_সূচক=সূচক=skipWhiteSpace(সূচক+arg_Start_দৈর্ঘ্য);
    যদি(সূচক==বার্তা.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ফেরত 0;
    }
    // parse argument name or number
    সূচক=skipIdentifier(সূচক);
    int32_t number=parse_Arg_সংখ্যা(name_সূচক, সূচক);
    যদি(number>=0) {
        int32_t length=সূচক-name_সূচক;
        যদি(length>Part::MAX_দৈর্ঘ্য || number>Part::MAX_মান) {
            setParseError(parseError, name_সূচক);  // Argument number too large.
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
        hasArgNumbers=সত্য;
        addPart(UMSGPAT_PART_TYPE_ARG_সংখ্যা, name_সূচক, length, number, errorCode);
    } অন্যথা যদি(number==UMSGPAT_ARG_NAME_NOT_সংখ্যা) {
        int32_t length=সূচক-name_সূচক;
        যদি(length>Part::MAX_দৈর্ঘ্য) {
            setParseError(parseError, name_সূচক);  // Argument name too long.
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
        hasArgNames=সত্য;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, name_সূচক, length, 0, errorCode);
    } অন্যথা {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, name_সূচক);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ফেরত 0;
    }
    সূচক=skipWhiteSpace(সূচক);
    যদি(সূচক==বার্তা.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        ফেরত 0;
    }
    char16_t c=বার্তা.charAt(সূচক);
    যদি(c==u_ডান_Curly_Brace) {
        // all done
    } অন্যথা যদি(c!=u_comma) {
        setParseError(parseError, name_সূচক);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ফেরত 0;
    } অন্যথা /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_সূচক=সূচক=skipWhiteSpace(সূচক+1);
        যখন(সূচক<বার্তা.length() && isArgTypeChar(বার্তা.charAt(সূচক))) {
            ++সূচক;
        }
        int32_t length=সূচক-type_সূচক;
        সূচক=skipWhiteSpace(সূচক);
        যদি(সূচক==বার্তা.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            ফেরত 0;
        }
        যদি(length==0 || ((c=বার্তা.charAt(সূচক))!=u_comma && c!=u_ডান_Curly_Brace)) {
            setParseError(parseError, name_সূচক);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ফেরত 0;
        }
        যদি(length>Part::MAX_দৈর্ঘ্য) {
            setParseError(parseError, name_সূচক);  // Argument type name too long.
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        যদি(length==6) {
            // case-insensitive comparisons for complex-type names
            যদি(isChoice(type_সূচক)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } অন্যথা যদি(isPlural(type_সূচক)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } অন্যথা যদি(isSelect(type_সূচক)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } অন্যথা যদি(length==13) {
            যদি(isSelect(type_সূচক) && isOrdinal(type_সূচক+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        যদি(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, type_সূচক, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        যদি(c==u_ডান_Curly_Brace) {
            যদি(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, name_সূচক);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
        } অন্যথা /* ',' */ {
            ++সূচক;
            যদি(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                সূচক=parseSimpleStyle(সূচক, parseError, errorCode);
            } অন্যথা যদি(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                সূচক=parseChoiceStyle(সূচক, nestingLevel, parseError, errorCode);
            } অন্যথা {
                সূচক=parsePluralOrSelectStyle(argType, সূচক, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, সূচক, 1, argType, errorCode);
    ফেরত সূচক+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t সূচক, UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    int32_t start=সূচক;
    int32_t nestedBraces=0;
    যখন(সূচক<বার্তা.length()) {
        char16_t c=বার্তা.charAt(সূচক++);
        যদি(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            সূচক=বার্তা.সূচক_Of(u_apos, সূচক);
            যদি(সূচক<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
            // skip the quote-ending apostrophe
            ++সূচক;
        } অন্যথা যদি(c==u_বাম_Curly_Brace) {
            ++nestedBraces;
        } অন্যথা যদি(c==u_ডান_Curly_Brace) {
            যদি(nestedBraces>0) {
                --nestedBraces;
            } অন্যথা {
                int32_t length=--সূচক-start;
                যদি(length>Part::MAX_দৈর্ঘ্য) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                    ফেরত 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                ফেরত সূচক;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    ফেরত 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t সূচক, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    int32_t start=সূচক;
    সূচক=skipWhiteSpace(সূচক);
    যদি(সূচক==বার্তা.length() || বার্তা.charAt(সূচক)==u_ডান_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        ফেরত 0;
    }
    জন্য(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t সংখ্যা_সূচক=সূচক;
        সূচক=skipDouble(সূচক);
        int32_t length=সূচক-সংখ্যা_সূচক;
        যদি(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ফেরত 0;
        }
        যদি(length>Part::MAX_দৈর্ঘ্য) {
            setParseError(parseError, সংখ্যা_সূচক);  // Choice number too long.
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত 0;
        }
        parseDouble(সংখ্যা_সূচক, সূচক, সত্য, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
        // Parse the separator.
        সূচক=skipWhiteSpace(সূচক);
        যদি(সূচক==বার্তা.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ফেরত 0;
        }
        char16_t c=বার্তা.charAt(সূচক);
        যদি(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ফেরত 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, সূচক, 1, 0, errorCode);
        // Parse the message fragment.
        সূচক=parseMessage(++সূচক, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        যদি(সূচক==বার্তা.length()) {
            ফেরত সূচক;
        }
        যদি(বার্তা.charAt(সূচক)==u_ডান_Curly_Brace) {
            যদি(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
            ফেরত সূচক;
        }  // else the terminator is '|'
        সূচক=skipWhiteSpace(সূচক+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t সূচক, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত 0;
    }
    int32_t start=সূচক;
    UBool isEmpty=সত্য;
    UBool hasOther=মিথ্যা;
    জন্য(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        সূচক=skipWhiteSpace(সূচক);
        UBool eos=সূচক==বার্তা.length();
        যদি(eos || বার্তা.charAt(সূচক)==u_ডান_Curly_Brace) {
            যদি(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
            যদি(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                ফেরত 0;
            }
            ফেরত সূচক;
        }
        int32_t selector_সূচক=সূচক;
        যদি(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && বার্তা.charAt(selector_সূচক)==u_equal) {
            // explicit-value plural selector: =double
            সূচক=skipDouble(সূচক+1);
            int32_t length=সূচক-selector_সূচক;
            যদি(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
            যদি(length>Part::MAX_দৈর্ঘ্য) {
                setParseError(parseError, selector_সূচক);  // Argument selector too long.
                errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                ফেরত 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_সূচক, length, 0, errorCode);
            parseDouble(selector_সূচক+1, সূচক, মিথ্যা,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } অন্যথা {
            সূচক=skipIdentifier(সূচক);
            int32_t length=সূচক-selector_সূচক;
            যদি(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                ফেরত 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            যদি( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && সূচক<বার্তা.length() &&
                0==বার্তা.compare(selector_সূচক, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                যদি(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ফেরত 0;
                }
                // allow whitespace between offset: and its value
                int32_t মান_সূচক=skipWhiteSpace(সূচক+1);  // The ':' is at index.
                সূচক=skipDouble(মান_সূচক);
                যদি(সূচক==মান_সূচক) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    ফেরত 0;
                }
                যদি((সূচক-মান_সূচক)>Part::MAX_দৈর্ঘ্য) {
                    setParseError(parseError, মান_সূচক);  // Plural offset value too long.
                    errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                    ফেরত 0;
                }
                parseDouble(মান_সূচক, সূচক, মিথ্যা,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                যদি(U_FAILURE(errorCode)) {
                    ফেরত 0;
                }
                isEmpty=মিথ্যা;
                চালু;  // no message fragment after the offset
            } অন্যথা {
                // normal selector word
                যদি(length>Part::MAX_দৈর্ঘ্য) {
                    setParseError(parseError, selector_সূচক);  // Argument selector too long.
                    errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
                    ফেরত 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_সূচক, length, 0, errorCode);
                যদি(0==বার্তা.compare(selector_সূচক, length, kOther, 0, 5)) {
                    hasOther=সত্য;
                }
            }
        }
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }

        // parse the message fragment following the selector
        সূচক=skipWhiteSpace(সূচক);
        যদি(সূচক==বার্তা.length() || বার্তা.charAt(সূচক)!=u_বাম_Curly_Brace) {
            setParseError(parseError, selector_সূচক);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            ফেরত 0;
        }
        সূচক=parseMessage(সূচক, 1, nestingLevel+1, argType, parseError, errorCode);
        যদি(U_FAILURE(errorCode)) {
            ফেরত 0;
        }
        isEmpty=মিথ্যা;
    }
}

int32_t
MessagePattern::parse_Arg_সংখ্যা(ধ্রুবক UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    যদি(start>=limit) {
        ফেরত UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_সংখ্যা;
    char16_t c=s.charAt(start++);
    যদি(c==0x30) {
        যদি(start==limit) {
            ফেরত 0;
        } অন্যথা {
            number=0;
            bad_সংখ্যা=সত্য;  // leading zero
        }
    } অন্যথা যদি(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_সংখ্যা=মিথ্যা;
    } অন্যথা {
        ফেরত UMSGPAT_ARG_NAME_NOT_সংখ্যা;
    }
    যখন(start<limit) {
        c=s.charAt(start++);
        যদি(0x30<=c && c<=0x39) {
            যদি(number>=INT32_MAX/10) {
                bad_সংখ্যা=সত্য;  // overflow
            } অন্যথা {
                number=number*10+(c-0x30);
            }
        } অন্যথা {
            ফেরত UMSGPAT_ARG_NAME_NOT_সংখ্যা;
        }
    }
    // There are only ASCII digits.
    যদি(bad_সংখ্যা) {
        ফেরত UMSGPAT_ARG_NAME_NOT_VALID;
    } অন্যথা {
        ফেরত number;
    }
}

শূন্য
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    জন্য(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t সূচক=start;
        char16_t c=বার্তা.charAt(সূচক++);
        যদি(c==u_minus) {
            isNegative=1;
            যদি(সূচক==limit) {
                বিরতি;  // no number
            }
            c=বার্তা.charAt(সূচক++);
        } অন্যথা যদি(c==u_plus) {
            যদি(সূচক==limit) {
                বিরতি;  // no number
            }
            c=বার্তা.charAt(সূচক++);
        }
        যদি(c==0x221e) {  // infinity
            যদি(allowInfinity && সূচক==limit) {
                দ্বিগুণ infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                ফেরত;
            } অন্যথা {
                বিরতি;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        যখন('0'<=c && c<='9') {
            value=value*10+(c-'0');
            যদি(value>(Part::MAX_মান+isNegative)) {
                বিরতি;  // not a small-enough integer
            }
            যদি(সূচক==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                ফেরত;
            }
            c=বার্তা.charAt(সূচক++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        অক্ষর সংখ্যা_Chars[128];
        int32_t capacity = static_cast<int32_t>(আকার(সংখ্যা_Chars));
        int32_t length=limit-start;
        যদি(length>=capacity) {
            বিরতি;  // number too long
        }
        বার্তা.extract(start, length, সংখ্যা_Chars, capacity, US_INV);
        যদি (static_cast<int32_t>(uprv_strlen(সংখ্যা_Chars)) < length) {
            বিরতি;  // contains non-invariant character that was turned into NUL
        }
        অক্ষর *end;
        দ্বিগুণ numeric_মান=uprv_strtod(সংখ্যা_Chars, &end);
        যদি(end!=(সংখ্যা_Chars+length)) {
            বিরতি;  // parsing error
        }
        addArgDoublePart(numeric_মান, start, length, errorCode);
        ফেরত;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t সূচক) {
    ধ্রুবক char16_t *s=বার্তা.get_বাফার();
    int32_t বার্তা_দৈর্ঘ্য=বার্তা.length();
    ধ্রুবক char16_t *t=PatternProps::skipWhiteSpace(s+সূচক, বার্তা_দৈর্ঘ্য-সূচক);
    ফেরত static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t সূচক) {
    ধ্রুবক char16_t *s=বার্তা.get_বাফার();
    int32_t বার্তা_দৈর্ঘ্য=বার্তা.length();
    ধ্রুবক char16_t *t=PatternProps::skipIdentifier(s+সূচক, বার্তা_দৈর্ঘ্য-সূচক);
    ফেরত static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t সূচক) {
    int32_t বার্তা_দৈর্ঘ্য=বার্তা.length();
    যখন(সূচক<বার্তা_দৈর্ঘ্য) {
        char16_t c=বার্তা.charAt(সূচক);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        যদি((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            বিরতি;
        }
        ++সূচক;
    }
    ফেরত সূচক;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    ফেরত (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t সূচক) {
    char16_t c;
    ফেরত
        ((c=বার্তা.charAt(সূচক++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(সূচক++))==u_h || c==u_H) &&
        ((c=বার্তা.charAt(সূচক++))==u_o || c==u_O) &&
        ((c=বার্তা.charAt(সূচক++))==u_i || c==u_I) &&
        ((c=বার্তা.charAt(সূচক++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(সূচক))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t সূচক) {
    char16_t c;
    ফেরত
        ((c=বার্তা.charAt(সূচক++))==u_p || c==u_P) &&
        ((c=বার্তা.charAt(সূচক++))==u_l || c==u_L) &&
        ((c=বার্তা.charAt(সূচক++))==u_u || c==u_U) &&
        ((c=বার্তা.charAt(সূচক++))==u_r || c==u_R) &&
        ((c=বার্তা.charAt(সূচক++))==u_a || c==u_A) &&
        ((c=বার্তা.charAt(সূচক))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t সূচক) {
    char16_t c;
    ফেরত
        ((c=বার্তা.charAt(সূচক++))==u_s || c==u_S) &&
        ((c=বার্তা.charAt(সূচক++))==u_e || c==u_E) &&
        ((c=বার্তা.charAt(সূচক++))==u_l || c==u_L) &&
        ((c=বার্তা.charAt(সূচক++))==u_e || c==u_E) &&
        ((c=বার্তা.charAt(সূচক++))==u_c || c==u_C) &&
        ((c=বার্তা.charAt(সূচক))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t সূচক) {
    char16_t c;
    ফেরত
        ((c=বার্তা.charAt(সূচক++))==u_o || c==u_O) &&
        ((c=বার্তা.charAt(সূচক++))==u_r || c==u_R) &&
        ((c=বার্তা.charAt(সূচক++))==u_d || c==u_D) &&
        ((c=বার্তা.charAt(সূচক++))==u_i || c==u_I) &&
        ((c=বার্তা.charAt(সূচক++))==u_n || c==u_N) &&
        ((c=বার্তা.charAt(সূচক++))==u_a || c==u_A) &&
        ((c=বার্তা.charAt(সূচক))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    ফেরত nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_বার্তা_START;
}

UBool
MessagePattern::in_শীর্ষ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    ফেরত
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_বার্তা_START;
}

শূন্য
MessagePattern::addPart(UMessagePatternPartType type, int32_t সূচক, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    যদি(partsList->ensureCapacityForOneMore(parts_দৈর্ঘ্য, errorCode)) {
        Part &part=partsList->a[parts_দৈর্ঘ্য++];
        part.type=type;
        part.সূচক=সূচক;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limit_Part_সূচক=0;
    }
}

শূন্য
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t সূচক, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limit_Part_সূচক=parts_দৈর্ঘ্য;
    addPart(type, সূচক, length, value, errorCode);
}

শূন্য
MessagePattern::addArgDoublePart(দ্বিগুণ numeric_মান, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    int32_t numeric_সূচক=numeric_মানসমূহ_দৈর্ঘ্য;
    যদি(numeric_মানসমূহ_List==nullptr) {
        numeric_মানসমূহ_List=নতুন MessagePatternDoubleList();
        যদি(numeric_মানসমূহ_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            ফেরত;
        }
    } অন্যথা যদি(!numeric_মানসমূহ_List->ensureCapacityForOneMore(numeric_মানসমূহ_দৈর্ঘ্য, errorCode)) {
        ফেরত;
    } অন্যথা {
        যদি(numeric_সূচক>Part::MAX_মান) {
            errorCode=U_সূচক_OUTOFBOUNDS_ERROR;
            ফেরত;
        }
    }
    numeric_মানসমূহ_List->a[numeric_মানসমূহ_দৈর্ঘ্য++]=numeric_মান;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numeric_সূচক, errorCode);
}

শূন্য
MessagePattern::setParseError(UParseError *parseError, int32_t সূচক) {
    যদি(parseError==nullptr) {
        ফেরত;
    }
    parseError->offset=সূচক;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=সূচক;
    যদি(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        যদি(length>0 && U16_IS_TRAIL(বার্তা[সূচক-length])) {
            --length;
        }
    }
    বার্তা.extract(সূচক-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=বার্তা.length()-সূচক;
    যদি(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        যদি(length>0 && U16_IS_LEAD(বার্তা[সূচক+length-1])) {
            --length;
        }
    }
    বার্তা.extract(সূচক, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

শূন্য
MessageImpl::appendReducedApostrophes(ধ্রুবক UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    জন্য(;;) {
        int32_t i=s.সূচক_Of(u_apos, start);
        যদি(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            বিরতি;
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
MessageImpl::appendSubMessageWithoutSkipSyntax(ধ্রুবক MessagePattern& বার্তা_Pattern,
                                               int32_t বার্তা_Start,
                                               UnicodeString& ফলাফল U_LIFETIME_BOUND) {
    ধ্রুবক UnicodeString &বার্তা_String=বার্তা_Pattern.getPatternString();
    int32_t পূর্ব_সূচক=বার্তা_Pattern.getPart(বার্তা_Start).getLimit();
    জন্য(int32_t i=বার্তা_Start;;) {
        ধ্রুবক MessagePattern::Part &part=বার্তা_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t সূচক=part.get_সূচক();
        যদি(type==UMSGPAT_PART_TYPE_বার্তা_LIMIT) {
            ফেরত ফলাফল.append(বার্তা_String, পূর্ব_সূচক, সূচক-পূর্ব_সূচক);
        } অন্যথা যদি(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            ফলাফল.append(বার্তা_String, পূর্ব_সূচক, সূচক-পূর্ব_সূচক);
            পূর্ব_সূচক=part.getLimit();
        } অন্যথা যদি(type==UMSGPAT_PART_TYPE_ARG_START) {
            ফলাফল.append(বার্তা_String, পূর্ব_সূচক, সূচক-পূর্ব_সূচক);
            পূর্ব_সূচক=সূচক;
            i=বার্তা_Pattern.get_Limit_Part_সূচক(i);
            সূচক=বার্তা_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(বার্তা_String, পূর্ব_সূচক, সূচক, ফলাফল);
            পূর্ব_সূচক=সূচক;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
