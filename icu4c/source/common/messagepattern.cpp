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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_FORMATTING

#समावेश "unicode/messagepattern.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/utf16.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "messageimpl.h"
#समावेश "patternprops.h"
#समावेश "putilimp.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

अटल स्थिर char16_t u_pound=0x23;
अटल स्थिर char16_t u_apos=0x27;
अटल स्थिर char16_t u_plus=0x2B;
अटल स्थिर char16_t u_comma=0x2C;
अटल स्थिर char16_t u_minus=0x2D;
अटल स्थिर char16_t u_dot=0x2E;
अटल स्थिर char16_t u_colon=0x3A;
अटल स्थिर char16_t u_lessThan=0x3C;
अटल स्थिर char16_t u_equal=0x3D;
अटल स्थिर char16_t u_A=0x41;
अटल स्थिर char16_t u_C=0x43;
अटल स्थिर char16_t u_D=0x44;
अटल स्थिर char16_t u_E=0x45;
अटल स्थिर char16_t u_H=0x48;
अटल स्थिर char16_t u_I=0x49;
अटल स्थिर char16_t u_L=0x4C;
अटल स्थिर char16_t u_N=0x4E;
अटल स्थिर char16_t u_O=0x4F;
अटल स्थिर char16_t u_P=0x50;
अटल स्थिर char16_t u_R=0x52;
अटल स्थिर char16_t u_S=0x53;
अटल स्थिर char16_t u_T=0x54;
अटल स्थिर char16_t u_U=0x55;
अटल स्थिर char16_t u_Z=0x5A;
अटल स्थिर char16_t u_a=0x61;
अटल स्थिर char16_t u_c=0x63;
अटल स्थिर char16_t u_d=0x64;
अटल स्थिर char16_t u_e=0x65;
अटल स्थिर char16_t u_f=0x66;
अटल स्थिर char16_t u_h=0x68;
अटल स्थिर char16_t u_i=0x69;
अटल स्थिर char16_t u_l=0x6C;
अटल स्थिर char16_t u_n=0x6E;
अटल स्थिर char16_t u_o=0x6F;
अटल स्थिर char16_t u_p=0x70;
अटल स्थिर char16_t u_r=0x72;
अटल स्थिर char16_t u_s=0x73;
अटल स्थिर char16_t u_t=0x74;
अटल स्थिर char16_t u_u=0x75;
अटल स्थिर char16_t u_z=0x7A;
अटल स्थिर char16_t u_बायाँ_Curly_Brace=0x7B;
अटल स्थिर char16_t u_pipe=0x7C;
अटल स्थिर char16_t u_दायाँ_Curly_Brace=0x7D;
अटल स्थिर char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

अटल स्थिर char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

अटल स्थिर char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

ढाँचा<typename T, int32_t ढेर_Capacity>
श्रेणी MessagePatternList : खुला UMemory {
खुला:
    MessagePatternList() {}
    व्योम copyFrom(स्थिर MessagePatternList<T, ढेर_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t old_लंबाई, UErrorCode &errorCode);
    UBool equals(स्थिर MessagePatternList<T, ढेर_Capacity> &other, int32_t length) स्थिर {
        क्रम(int32_t i=0; i<length; ++i) {
            अगर(a[i]!=other.a[i]) { वापस false; }
        }
        वापस true;
    }

    Maybe_ढेर_Array<T, ढेर_Capacity> a;
};

ढाँचा<typename T, int32_t ढेर_Capacity>
व्योम
MessagePatternList<T, ढेर_Capacity>::copyFrom(
        स्थिर MessagePatternList<T, ढेर_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    अगर(U_SUCCESS(errorCode) && length>0) {
        अगर(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (माप_प्रकार)length*माप(T));
    }
}

ढाँचा<typename T, int32_t ढेर_Capacity>
UBool
MessagePatternList<T, ढेर_Capacity>::ensureCapacityForOneMore(int32_t old_लंबाई, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    अगर(a.getCapacity()>old_लंबाई || a.resize(2*old_लंबाई, old_लंबाई)!=nullptr) {
        वापस true;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    वापस false;
}

// MessagePatternList specializations -------------------------------------- ***

श्रेणी MessagePatternDoubleList : खुला MessagePatternList<द्विगुन, 8> {
};

श्रेणी MessagePatternPartsList : खुला MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), parts_लंबाई(0),
          numeric_मूल्यावली_List(nullptr), numeric_मूल्यावली(nullptr), numeric_मूल्यावली_लंबाई(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), parts_लंबाई(0),
          numeric_मूल्यावली_List(nullptr), numeric_मूल्यावली(nullptr), numeric_मूल्यावली_लंबाई(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    init(errorCode);
}

MessagePattern::MessagePattern(स्थिर UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), parts_लंबाई(0),
          numeric_मूल्यावली_List(nullptr), numeric_मूल्यावली(nullptr), numeric_मूल्यावली_लंबाई(0),
          hasArgNames(false), hasArgNumbers(false), needsAutoQuoting(false) {
    अगर(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    partsList=नया MessagePatternPartsList();
    अगर(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस false;
    }
    parts=partsList->a.getAlias();
    वापस true;
}

MessagePattern::MessagePattern(स्थिर MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), संदेश(other.संदेश),
          partsList(nullptr), parts(nullptr), parts_लंबाई(0),
          numeric_मूल्यावली_List(nullptr), numeric_मूल्यावली(nullptr), numeric_मूल्यावली_लंबाई(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    अगर(!copyStorage(other, errorCode)) {
        सफा();
    }
}

MessagePattern&
MessagePattern::चालक=(स्थिर MessagePattern& other) U_LIFETIME_BOUND {
    अगर(यह==&other) {
        वापस *यह;
    }
    aposMode=other.aposMode;
    संदेश=other.संदेश;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    अगर(!copyStorage(other, errorCode)) {
        सफा();
    }
    वापस *यह;
}

UBool
MessagePattern::copyStorage(स्थिर MessagePattern &other, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस false;
    }
    parts=nullptr;
    parts_लंबाई=0;
    numeric_मूल्यावली=nullptr;
    numeric_मूल्यावली_लंबाई=0;
    अगर(partsList==nullptr) {
        partsList=नया MessagePatternPartsList();
        अगर(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस false;
        }
        parts=partsList->a.getAlias();
    }
    अगर(other.parts_लंबाई>0) {
        partsList->copyFrom(*other.partsList, other.parts_लंबाई, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        parts=partsList->a.getAlias();
        parts_लंबाई=other.parts_लंबाई;
    }
    अगर(other.numeric_मूल्यावली_लंबाई>0) {
        अगर(numeric_मूल्यावली_List==nullptr) {
            numeric_मूल्यावली_List=नया MessagePatternDoubleList();
            अगर(numeric_मूल्यावली_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                वापस false;
            }
            numeric_मूल्यावली=numeric_मूल्यावली_List->a.getAlias();
        }
        numeric_मूल्यावली_List->copyFrom(
            *other.numeric_मूल्यावली_List, other.numeric_मूल्यावली_लंबाई, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस false;
        }
        numeric_मूल्यावली=numeric_मूल्यावली_List->a.getAlias();
        numeric_मूल्यावली_लंबाई=other.numeric_मूल्यावली_लंबाई;
    }
    वापस true;
}

MessagePattern::~MessagePattern() {
    मिटाओ partsList;
    मिटाओ numeric_मूल्यावली_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(स्थिर UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    वापस *यह;
}

MessagePattern&
MessagePattern::parseChoiceStyle(स्थिर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    वापस *यह;
}

MessagePattern&
MessagePattern::parsePluralStyle(स्थिर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    वापस *यह;
}

MessagePattern&
MessagePattern::parseSelectStyle(स्थिर UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    वापस *यह;
}

व्योम
MessagePattern::सफा() {
    // Mostly the same as preParse().
    संदेश.हटाओ();
    hasArgNames=hasArgNumbers=false;
    needsAutoQuoting=false;
    parts_लंबाई=0;
    numeric_मूल्यावली_लंबाई=0;
}

bool
MessagePattern::चालक==(स्थिर MessagePattern &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    वापस
        aposMode==other.aposMode &&
        संदेश==other.संदेश &&
        // parts.equals(o.parts)
        parts_लंबाई==other.parts_लंबाई &&
        (parts_लंबाई==0 || partsList->equals(*other.partsList, parts_लंबाई));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() स्थिर {
    int32_t hash=(aposMode*37+संदेश.hashCode())*37+parts_लंबाई;
    क्रम(int32_t i=0; i<parts_लंबाई; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    वापस hash;
}

int32_t
MessagePattern::validateArgumentName(स्थिर UnicodeString &name) {
    अगर(!PatternProps::isIdentifier(name.get_बफर(), name.length())) {
        वापस UMSGPAT_ARG_NAME_NOT_VALID;
    }
    वापस parse_Arg_संख्या(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() स्थिर {
    अगर(!needsAutoQuoting) {
        वापस संदेश;
    }
    UnicodeString modified(संदेश);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=गिनती_Parts();
    क्रम(int32_t i=count; i>0;) {
        स्थिर Part &part=getPart(--i);
        अगर(part.getType()==UMSGPAT_PART_TYPE_जोड़ो_CHAR) {
           modified.insert(part.सूचकांक, static_cast<char16_t>(part.value));
        }
    }
    वापस modified;
}

द्विगुन
MessagePattern::get_Numeric_मूल्य(स्थिर Part &part) स्थिर {
    UMessagePatternPartType type=part.type;
    अगर(type==UMSGPAT_PART_TYPE_ARG_INT) {
        वापस part.value;
    } अन्यथा अगर(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        वापस numeric_मूल्यावली[part.value];
    } अन्यथा {
        वापस UMSGPAT_NO_NUMERIC_मूल्य;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
द्विगुन
MessagePattern::getPluralOffset(int32_t pluralStart) स्थिर {
    स्थिर Part &part=getPart(pluralStart);
    अगर(Part::has_Numeric_मूल्य(part.type)) {
        वापस get_Numeric_मूल्य(part);
    } अन्यथा {
        वापस 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

bool
MessagePattern::Part::चालक==(स्थिर Part &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    वापस
        type==other.type &&
        सूचकांक==other.सूचकांक &&
        length==other.length &&
        value==other.value &&
        limit_Part_सूचकांक==other.limit_Part_सूचकांक;
}

// MessagePattern parser --------------------------------------------------- ***

व्योम
MessagePattern::preParse(स्थिर UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    अगर(parseError!=nullptr) {
        parseError->पंक्ति=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    संदेश=pattern;
    hasArgNames=hasArgNumbers=false;
    needsAutoQuoting=false;
    parts_लंबाई=0;
    numeric_मूल्यावली_लंबाई=0;
}

व्योम
MessagePattern::postParse() {
    अगर(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    अगर(numeric_मूल्यावली_List!=nullptr) {
        numeric_मूल्यावली=numeric_मूल्यावली_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t सूचकांक, int32_t संदेश_Start_लंबाई,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    अगर(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
        वापस 0;
    }
    int32_t संदेश_Start=parts_लंबाई;
    addPart(UMSGPAT_PART_TYPE_संदेश_START, सूचकांक, संदेश_Start_लंबाई, nestingLevel, errorCode);
    सूचकांक+=संदेश_Start_लंबाई;
    क्रम(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
        अगर(सूचकांक>=संदेश.length()) {
            अवरोध;
        }
        char16_t c=संदेश.charAt(सूचकांक++);
        अगर(c==u_apos) {
            अगर(सूचकांक==संदेश.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_जोड़ो_CHAR, सूचकांक, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=true;
            } अन्यथा {
                c=संदेश.charAt(सूचकांक);
                अगर(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक++, 1, 0, errorCode);
                } अन्यथा अगर(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_बायाँ_Curly_Brace || c==u_दायाँ_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    क्रम(;;) {
                        सूचकांक=संदेश.सूचकांक_Of(u_apos, सूचकांक+1);
                        अगर(सूचकांक>=0) {
                            अगर(/*(index+1)<msg.length() &&*/ संदेश.charAt(सूचकांक+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++सूचकांक, 1, 0, errorCode);
                            } अन्यथा {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक++, 1, 0, errorCode);
                                अवरोध;
                            }
                        } अन्यथा {
                            // The quoted text reaches to the end of the of the message.
                            सूचकांक=संदेश.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_जोड़ो_CHAR, सूचकांक, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=true;
                            अवरोध;
                        }
                    }
                } अन्यथा {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_जोड़ो_CHAR, सूचकांक, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=true;
                }
            }
        } अन्यथा अगर(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_संख्या, सूचकांक-1, 1, 0, errorCode);
        } अन्यथा अगर(c==u_बायाँ_Curly_Brace) {
            सूचकांक=parseArg(सूचकांक-1, 1, nestingLevel, parseError, errorCode);
        } अन्यथा अगर((nestingLevel>0 && c==u_दायाँ_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limit_लंबाई=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_दायाँ_Curly_Brace) ? 0 : 1;
            addLimitPart(संदेश_Start, UMSGPAT_PART_TYPE_संदेश_LIMIT, सूचकांक-1, limit_लंबाई,
                         nestingLevel, errorCode);
            अगर(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                वापस सूचकांक-1;
            } अन्यथा {
                // continue parsing after the '}'
                वापस सूचकांक;
            }
        }  // else: c is part of literal text
    }
    अगर(nestingLevel>0 && !in_शीर्ष_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        वापस 0;
    }
    addLimitPart(संदेश_Start, UMSGPAT_PART_TYPE_संदेश_LIMIT, सूचकांक, 0, nestingLevel, errorCode);
    वापस सूचकांक;
}

int32_t
MessagePattern::parseArg(int32_t सूचकांक, int32_t arg_Start_लंबाई, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=parts_लंबाई;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, सूचकांक, arg_Start_लंबाई, argType, errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    int32_t name_सूचकांक=सूचकांक=skipWhiteSpace(सूचकांक+arg_Start_लंबाई);
    अगर(सूचकांक==संदेश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        वापस 0;
    }
    // parse argument name or number
    सूचकांक=skipIdentifier(सूचकांक);
    int32_t number=parse_Arg_संख्या(name_सूचकांक, सूचकांक);
    अगर(number>=0) {
        int32_t length=सूचकांक-name_सूचकांक;
        अगर(length>Part::MAX_लंबाई || number>Part::MAX_मूल्य) {
            setParseError(parseError, name_सूचकांक);  // Argument number too large.
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
        hasArgNumbers=true;
        addPart(UMSGPAT_PART_TYPE_ARG_संख्या, name_सूचकांक, length, number, errorCode);
    } अन्यथा अगर(number==UMSGPAT_ARG_NAME_NOT_संख्या) {
        int32_t length=सूचकांक-name_सूचकांक;
        अगर(length>Part::MAX_लंबाई) {
            setParseError(parseError, name_सूचकांक);  // Argument name too long.
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
        hasArgNames=true;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, name_सूचकांक, length, 0, errorCode);
    } अन्यथा {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, name_सूचकांक);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        वापस 0;
    }
    सूचकांक=skipWhiteSpace(सूचकांक);
    अगर(सूचकांक==संदेश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        वापस 0;
    }
    char16_t c=संदेश.charAt(सूचकांक);
    अगर(c==u_दायाँ_Curly_Brace) {
        // all done
    } अन्यथा अगर(c!=u_comma) {
        setParseError(parseError, name_सूचकांक);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        वापस 0;
    } अन्यथा /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_सूचकांक=सूचकांक=skipWhiteSpace(सूचकांक+1);
        जबतक(सूचकांक<संदेश.length() && isArgTypeChar(संदेश.charAt(सूचकांक))) {
            ++सूचकांक;
        }
        int32_t length=सूचकांक-type_सूचकांक;
        सूचकांक=skipWhiteSpace(सूचकांक);
        अगर(सूचकांक==संदेश.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            वापस 0;
        }
        अगर(length==0 || ((c=संदेश.charAt(सूचकांक))!=u_comma && c!=u_दायाँ_Curly_Brace)) {
            setParseError(parseError, name_सूचकांक);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            वापस 0;
        }
        अगर(length>Part::MAX_लंबाई) {
            setParseError(parseError, name_सूचकांक);  // Argument type name too long.
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        अगर(length==6) {
            // case-insensitive comparisons for complex-type names
            अगर(isChoice(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } अन्यथा अगर(isPlural(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } अन्यथा अगर(isSelect(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } अन्यथा अगर(length==13) {
            अगर(isSelect(type_सूचकांक) && isOrdinal(type_सूचकांक+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        अगर(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, type_सूचकांक, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        अगर(c==u_दायाँ_Curly_Brace) {
            अगर(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, name_सूचकांक);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
        } अन्यथा /* ',' */ {
            ++सूचकांक;
            अगर(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                सूचकांक=parseSimpleStyle(सूचकांक, parseError, errorCode);
            } अन्यथा अगर(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                सूचकांक=parseChoiceStyle(सूचकांक, nestingLevel, parseError, errorCode);
            } अन्यथा {
                सूचकांक=parsePluralOrSelectStyle(argType, सूचकांक, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, सूचकांक, 1, argType, errorCode);
    वापस सूचकांक+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t सूचकांक, UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    int32_t start=सूचकांक;
    int32_t nestedBraces=0;
    जबतक(सूचकांक<संदेश.length()) {
        char16_t c=संदेश.charAt(सूचकांक++);
        अगर(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            सूचकांक=संदेश.सूचकांक_Of(u_apos, सूचकांक);
            अगर(सूचकांक<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
            // skip the quote-ending apostrophe
            ++सूचकांक;
        } अन्यथा अगर(c==u_बायाँ_Curly_Brace) {
            ++nestedBraces;
        } अन्यथा अगर(c==u_दायाँ_Curly_Brace) {
            अगर(nestedBraces>0) {
                --nestedBraces;
            } अन्यथा {
                int32_t length=--सूचकांक-start;
                अगर(length>Part::MAX_लंबाई) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    वापस 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                वापस सूचकांक;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    वापस 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t सूचकांक, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    int32_t start=सूचकांक;
    सूचकांक=skipWhiteSpace(सूचकांक);
    अगर(सूचकांक==संदेश.length() || संदेश.charAt(सूचकांक)==u_दायाँ_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        वापस 0;
    }
    क्रम(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t संख्या_सूचकांक=सूचकांक;
        सूचकांक=skipDouble(सूचकांक);
        int32_t length=सूचकांक-संख्या_सूचकांक;
        अगर(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            वापस 0;
        }
        अगर(length>Part::MAX_लंबाई) {
            setParseError(parseError, संख्या_सूचकांक);  // Choice number too long.
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
        parseDouble(संख्या_सूचकांक, सूचकांक, true, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
        // Parse the separator.
        सूचकांक=skipWhiteSpace(सूचकांक);
        अगर(सूचकांक==संदेश.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            वापस 0;
        }
        char16_t c=संदेश.charAt(सूचकांक);
        अगर(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            वापस 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, सूचकांक, 1, 0, errorCode);
        // Parse the message fragment.
        सूचकांक=parseMessage(++सूचकांक, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        अगर(सूचकांक==संदेश.length()) {
            वापस सूचकांक;
        }
        अगर(संदेश.charAt(सूचकांक)==u_दायाँ_Curly_Brace) {
            अगर(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
            वापस सूचकांक;
        }  // else the terminator is '|'
        सूचकांक=skipWhiteSpace(सूचकांक+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t सूचकांक, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस 0;
    }
    int32_t start=सूचकांक;
    UBool isEmpty=true;
    UBool hasOther=false;
    क्रम(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        सूचकांक=skipWhiteSpace(सूचकांक);
        UBool eos=सूचकांक==संदेश.length();
        अगर(eos || संदेश.charAt(सूचकांक)==u_दायाँ_Curly_Brace) {
            अगर(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
            अगर(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                वापस 0;
            }
            वापस सूचकांक;
        }
        int32_t selector_सूचकांक=सूचकांक;
        अगर(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && संदेश.charAt(selector_सूचकांक)==u_equal) {
            // explicit-value plural selector: =double
            सूचकांक=skipDouble(सूचकांक+1);
            int32_t length=सूचकांक-selector_सूचकांक;
            अगर(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
            अगर(length>Part::MAX_लंबाई) {
                setParseError(parseError, selector_सूचकांक);  // Argument selector too long.
                errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_सूचकांक, length, 0, errorCode);
            parseDouble(selector_सूचकांक+1, सूचकांक, false,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } अन्यथा {
            सूचकांक=skipIdentifier(सूचकांक);
            int32_t length=सूचकांक-selector_सूचकांक;
            अगर(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                वापस 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            अगर( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && सूचकांक<संदेश.length() &&
                0==संदेश.compare(selector_सूचकांक, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                अगर(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    वापस 0;
                }
                // allow whitespace between offset: and its value
                int32_t मूल्य_सूचकांक=skipWhiteSpace(सूचकांक+1);  // The ':' is at index.
                सूचकांक=skipDouble(मूल्य_सूचकांक);
                अगर(सूचकांक==मूल्य_सूचकांक) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    वापस 0;
                }
                अगर((सूचकांक-मूल्य_सूचकांक)>Part::MAX_लंबाई) {
                    setParseError(parseError, मूल्य_सूचकांक);  // Plural offset value too long.
                    errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    वापस 0;
                }
                parseDouble(मूल्य_सूचकांक, सूचकांक, false,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                अगर(U_FAILURE(errorCode)) {
                    वापस 0;
                }
                isEmpty=false;
                जारी;  // no message fragment after the offset
            } अन्यथा {
                // normal selector word
                अगर(length>Part::MAX_लंबाई) {
                    setParseError(parseError, selector_सूचकांक);  // Argument selector too long.
                    errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    वापस 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_सूचकांक, length, 0, errorCode);
                अगर(0==संदेश.compare(selector_सूचकांक, length, kOther, 0, 5)) {
                    hasOther=true;
                }
            }
        }
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }

        // parse the message fragment following the selector
        सूचकांक=skipWhiteSpace(सूचकांक);
        अगर(सूचकांक==संदेश.length() || संदेश.charAt(सूचकांक)!=u_बायाँ_Curly_Brace) {
            setParseError(parseError, selector_सूचकांक);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            वापस 0;
        }
        सूचकांक=parseMessage(सूचकांक, 1, nestingLevel+1, argType, parseError, errorCode);
        अगर(U_FAILURE(errorCode)) {
            वापस 0;
        }
        isEmpty=false;
    }
}

int32_t
MessagePattern::parse_Arg_संख्या(स्थिर UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    अगर(start>=limit) {
        वापस UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_संख्या;
    char16_t c=s.charAt(start++);
    अगर(c==0x30) {
        अगर(start==limit) {
            वापस 0;
        } अन्यथा {
            number=0;
            bad_संख्या=true;  // leading zero
        }
    } अन्यथा अगर(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_संख्या=false;
    } अन्यथा {
        वापस UMSGPAT_ARG_NAME_NOT_संख्या;
    }
    जबतक(start<limit) {
        c=s.charAt(start++);
        अगर(0x30<=c && c<=0x39) {
            अगर(number>=INT32_MAX/10) {
                bad_संख्या=true;  // overflow
            } अन्यथा {
                number=number*10+(c-0x30);
            }
        } अन्यथा {
            वापस UMSGPAT_ARG_NAME_NOT_संख्या;
        }
    }
    // There are only ASCII digits.
    अगर(bad_संख्या) {
        वापस UMSGPAT_ARG_NAME_NOT_VALID;
    } अन्यथा {
        वापस number;
    }
}

व्योम
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    क्रम(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t सूचकांक=start;
        char16_t c=संदेश.charAt(सूचकांक++);
        अगर(c==u_minus) {
            isNegative=1;
            अगर(सूचकांक==limit) {
                अवरोध;  // no number
            }
            c=संदेश.charAt(सूचकांक++);
        } अन्यथा अगर(c==u_plus) {
            अगर(सूचकांक==limit) {
                अवरोध;  // no number
            }
            c=संदेश.charAt(सूचकांक++);
        }
        अगर(c==0x221e) {  // infinity
            अगर(allowInfinity && सूचकांक==limit) {
                द्विगुन infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                वापस;
            } अन्यथा {
                अवरोध;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        जबतक('0'<=c && c<='9') {
            value=value*10+(c-'0');
            अगर(value>(Part::MAX_मूल्य+isNegative)) {
                अवरोध;  // not a small-enough integer
            }
            अगर(सूचकांक==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                वापस;
            }
            c=संदेश.charAt(सूचकांक++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        अक्षर संख्या_Chars[128];
        int32_t capacity = static_cast<int32_t>(माप(संख्या_Chars));
        int32_t length=limit-start;
        अगर(length>=capacity) {
            अवरोध;  // number too long
        }
        संदेश.extract(start, length, संख्या_Chars, capacity, US_INV);
        अगर (static_cast<int32_t>(uprv_strlen(संख्या_Chars)) < length) {
            अवरोध;  // contains non-invariant character that was turned into NUL
        }
        अक्षर *end;
        द्विगुन numeric_मूल्य=uprv_strtod(संख्या_Chars, &end);
        अगर(end!=(संख्या_Chars+length)) {
            अवरोध;  // parsing error
        }
        addArgDoublePart(numeric_मूल्य, start, length, errorCode);
        वापस;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t सूचकांक) {
    स्थिर char16_t *s=संदेश.get_बफर();
    int32_t संदेश_लंबाई=संदेश.length();
    स्थिर char16_t *t=PatternProps::skipWhiteSpace(s+सूचकांक, संदेश_लंबाई-सूचकांक);
    वापस static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t सूचकांक) {
    स्थिर char16_t *s=संदेश.get_बफर();
    int32_t संदेश_लंबाई=संदेश.length();
    स्थिर char16_t *t=PatternProps::skipIdentifier(s+सूचकांक, संदेश_लंबाई-सूचकांक);
    वापस static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t सूचकांक) {
    int32_t संदेश_लंबाई=संदेश.length();
    जबतक(सूचकांक<संदेश_लंबाई) {
        char16_t c=संदेश.charAt(सूचकांक);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        अगर((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            अवरोध;
        }
        ++सूचकांक;
    }
    वापस सूचकांक;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    वापस (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t सूचकांक) {
    char16_t c;
    वापस
        ((c=संदेश.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(सूचकांक++))==u_h || c==u_H) &&
        ((c=संदेश.charAt(सूचकांक++))==u_o || c==u_O) &&
        ((c=संदेश.charAt(सूचकांक++))==u_i || c==u_I) &&
        ((c=संदेश.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(सूचकांक))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t सूचकांक) {
    char16_t c;
    वापस
        ((c=संदेश.charAt(सूचकांक++))==u_p || c==u_P) &&
        ((c=संदेश.charAt(सूचकांक++))==u_l || c==u_L) &&
        ((c=संदेश.charAt(सूचकांक++))==u_u || c==u_U) &&
        ((c=संदेश.charAt(सूचकांक++))==u_r || c==u_R) &&
        ((c=संदेश.charAt(सूचकांक++))==u_a || c==u_A) &&
        ((c=संदेश.charAt(सूचकांक))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t सूचकांक) {
    char16_t c;
    वापस
        ((c=संदेश.charAt(सूचकांक++))==u_s || c==u_S) &&
        ((c=संदेश.charAt(सूचकांक++))==u_e || c==u_E) &&
        ((c=संदेश.charAt(सूचकांक++))==u_l || c==u_L) &&
        ((c=संदेश.charAt(सूचकांक++))==u_e || c==u_E) &&
        ((c=संदेश.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(सूचकांक))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t सूचकांक) {
    char16_t c;
    वापस
        ((c=संदेश.charAt(सूचकांक++))==u_o || c==u_O) &&
        ((c=संदेश.charAt(सूचकांक++))==u_r || c==u_R) &&
        ((c=संदेश.charAt(सूचकांक++))==u_d || c==u_D) &&
        ((c=संदेश.charAt(सूचकांक++))==u_i || c==u_I) &&
        ((c=संदेश.charAt(सूचकांक++))==u_n || c==u_N) &&
        ((c=संदेश.charAt(सूचकांक++))==u_a || c==u_A) &&
        ((c=संदेश.charAt(सूचकांक))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    वापस nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_संदेश_START;
}

UBool
MessagePattern::in_शीर्ष_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    वापस
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_संदेश_START;
}

व्योम
MessagePattern::addPart(UMessagePatternPartType type, int32_t सूचकांक, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    अगर(partsList->ensureCapacityForOneMore(parts_लंबाई, errorCode)) {
        Part &part=partsList->a[parts_लंबाई++];
        part.type=type;
        part.सूचकांक=सूचकांक;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limit_Part_सूचकांक=0;
    }
}

व्योम
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t सूचकांक, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limit_Part_सूचकांक=parts_लंबाई;
    addPart(type, सूचकांक, length, value, errorCode);
}

व्योम
MessagePattern::addArgDoublePart(द्विगुन numeric_मूल्य, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    int32_t numeric_सूचकांक=numeric_मूल्यावली_लंबाई;
    अगर(numeric_मूल्यावली_List==nullptr) {
        numeric_मूल्यावली_List=नया MessagePatternDoubleList();
        अगर(numeric_मूल्यावली_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस;
        }
    } अन्यथा अगर(!numeric_मूल्यावली_List->ensureCapacityForOneMore(numeric_मूल्यावली_लंबाई, errorCode)) {
        वापस;
    } अन्यथा {
        अगर(numeric_सूचकांक>Part::MAX_मूल्य) {
            errorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस;
        }
    }
    numeric_मूल्यावली_List->a[numeric_मूल्यावली_लंबाई++]=numeric_मूल्य;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numeric_सूचकांक, errorCode);
}

व्योम
MessagePattern::setParseError(UParseError *parseError, int32_t सूचकांक) {
    अगर(parseError==nullptr) {
        वापस;
    }
    parseError->offset=सूचकांक;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=सूचकांक;
    अगर(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        अगर(length>0 && U16_IS_TRAIL(संदेश[सूचकांक-length])) {
            --length;
        }
    }
    संदेश.extract(सूचकांक-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=संदेश.length()-सूचकांक;
    अगर(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        अगर(length>0 && U16_IS_LEAD(संदेश[सूचकांक+length-1])) {
            --length;
        }
    }
    संदेश.extract(सूचकांक, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

व्योम
MessageImpl::appendReducedApostrophes(स्थिर UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    क्रम(;;) {
        int32_t i=s.सूचकांक_Of(u_apos, start);
        अगर(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            अवरोध;
        }
        अगर(i==doubleApos) {
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
MessageImpl::appendSubMessageWithoutSkipSyntax(स्थिर MessagePattern& संदेश_Pattern,
                                               int32_t संदेश_Start,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) {
    स्थिर UnicodeString &संदेश_String=संदेश_Pattern.getPatternString();
    int32_t पूर्व_सूचकांक=संदेश_Pattern.getPart(संदेश_Start).getLimit();
    क्रम(int32_t i=संदेश_Start;;) {
        स्थिर MessagePattern::Part &part=संदेश_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t सूचकांक=part.get_सूचकांक();
        अगर(type==UMSGPAT_PART_TYPE_संदेश_LIMIT) {
            वापस परिणाम.append(संदेश_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
        } अन्यथा अगर(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            परिणाम.append(संदेश_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
            पूर्व_सूचकांक=part.getLimit();
        } अन्यथा अगर(type==UMSGPAT_PART_TYPE_ARG_START) {
            परिणाम.append(संदेश_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
            पूर्व_सूचकांक=सूचकांक;
            i=संदेश_Pattern.get_Limit_Part_सूचकांक(i);
            सूचकांक=संदेश_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(संदेश_String, पूर्व_सूचकांक, सूचकांक, परिणाम);
            पूर्व_सूचकांक=सूचकांक;
        }
    }
}

U_NAMESPACE_END

#पूर्ण_अगर  // !UCONFIG_NO_FORMATTING
