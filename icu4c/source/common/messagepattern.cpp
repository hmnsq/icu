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

#जर !UCONFIG_NO_FORMATTING

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

स्थिर स्थिरांक char16_t u_pound=0x23;
स्थिर स्थिरांक char16_t u_apos=0x27;
स्थिर स्थिरांक char16_t u_plus=0x2B;
स्थिर स्थिरांक char16_t u_comma=0x2C;
स्थिर स्थिरांक char16_t u_minus=0x2D;
स्थिर स्थिरांक char16_t u_dot=0x2E;
स्थिर स्थिरांक char16_t u_colon=0x3A;
स्थिर स्थिरांक char16_t u_lessThan=0x3C;
स्थिर स्थिरांक char16_t u_equal=0x3D;
स्थिर स्थिरांक char16_t u_A=0x41;
स्थिर स्थिरांक char16_t u_C=0x43;
स्थिर स्थिरांक char16_t u_D=0x44;
स्थिर स्थिरांक char16_t u_E=0x45;
स्थिर स्थिरांक char16_t u_H=0x48;
स्थिर स्थिरांक char16_t u_I=0x49;
स्थिर स्थिरांक char16_t u_L=0x4C;
स्थिर स्थिरांक char16_t u_N=0x4E;
स्थिर स्थिरांक char16_t u_O=0x4F;
स्थिर स्थिरांक char16_t u_P=0x50;
स्थिर स्थिरांक char16_t u_R=0x52;
स्थिर स्थिरांक char16_t u_S=0x53;
स्थिर स्थिरांक char16_t u_T=0x54;
स्थिर स्थिरांक char16_t u_U=0x55;
स्थिर स्थिरांक char16_t u_Z=0x5A;
स्थिर स्थिरांक char16_t u_a=0x61;
स्थिर स्थिरांक char16_t u_c=0x63;
स्थिर स्थिरांक char16_t u_d=0x64;
स्थिर स्थिरांक char16_t u_e=0x65;
स्थिर स्थिरांक char16_t u_f=0x66;
स्थिर स्थिरांक char16_t u_h=0x68;
स्थिर स्थिरांक char16_t u_i=0x69;
स्थिर स्थिरांक char16_t u_l=0x6C;
स्थिर स्थिरांक char16_t u_n=0x6E;
स्थिर स्थिरांक char16_t u_o=0x6F;
स्थिर स्थिरांक char16_t u_p=0x70;
स्थिर स्थिरांक char16_t u_r=0x72;
स्थिर स्थिरांक char16_t u_s=0x73;
स्थिर स्थिरांक char16_t u_t=0x74;
स्थिर स्थिरांक char16_t u_u=0x75;
स्थिर स्थिरांक char16_t u_z=0x7A;
स्थिर स्थिरांक char16_t u_डावा_Curly_Brace=0x7B;
स्थिर स्थिरांक char16_t u_pipe=0x7C;
स्थिर स्थिरांक char16_t u_उजवा_Curly_Brace=0x7D;
स्थिर स्थिरांक char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

स्थिर स्थिरांक char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

स्थिर स्थिरांक char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

ढाँचा<typename T, int32_t चळत_Capacity>
वर्ग MessagePatternList : सार्वजनिक UMemory {
सार्वजनिक:
    MessagePatternList() {}
    शून्य copyFrom(स्थिरांक MessagePatternList<T, चळत_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(स्थिरांक MessagePatternList<T, चळत_Capacity> &other, int32_t length) स्थिरांक {
        साठी(int32_t i=0; i<length; ++i) {
            जर(a[i]!=other.a[i]) { परत असत्य; }
        }
        परत सत्य;
    }

    Maybe_चळत_Array<T, चळत_Capacity> a;
};

ढाँचा<typename T, int32_t चळत_Capacity>
शून्य
MessagePatternList<T, चळत_Capacity>::copyFrom(
        स्थिरांक MessagePatternList<T, चळत_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    जर(U_SUCCESS(errorCode) && length>0) {
        जर(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (माप_प्रकार)length*आकार(T));
    }
}

ढाँचा<typename T, int32_t चळत_Capacity>
UBool
MessagePatternList<T, चळत_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    जर(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        परत सत्य;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    परत असत्य;
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
          numeric_मूल्ये_List(nullptr), numeric_मूल्ये(nullptr), numeric_मूल्ये_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मूल्ये_List(nullptr), numeric_मूल्ये(nullptr), numeric_मूल्ये_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(errorCode);
}

MessagePattern::MessagePattern(स्थिरांक UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मूल्ये_List(nullptr), numeric_मूल्ये(nullptr), numeric_मूल्ये_Length(0),
          hasArgNames(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    जर(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    partsList=नवीन MessagePatternPartsList();
    जर(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत असत्य;
    }
    parts=partsList->a.getAlias();
    परत सत्य;
}

MessagePattern::MessagePattern(स्थिरांक MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), संदेश(other.संदेश),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_मूल्ये_List(nullptr), numeric_मूल्ये(nullptr), numeric_मूल्ये_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    जर(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::चालक=(स्थिरांक MessagePattern& other) U_LIFETIME_BOUND {
    जर(हे==&other) {
        परत *हे;
    }
    aposMode=other.aposMode;
    संदेश=other.संदेश;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    जर(!copyStorage(other, errorCode)) {
        clear();
    }
    परत *हे;
}

UBool
MessagePattern::copyStorage(स्थिरांक MessagePattern &other, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत असत्य;
    }
    parts=nullptr;
    partsLength=0;
    numeric_मूल्ये=nullptr;
    numeric_मूल्ये_Length=0;
    जर(partsList==nullptr) {
        partsList=नवीन MessagePatternPartsList();
        जर(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत असत्य;
        }
        parts=partsList->a.getAlias();
    }
    जर(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत असत्य;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    जर(other.numeric_मूल्ये_Length>0) {
        जर(numeric_मूल्ये_List==nullptr) {
            numeric_मूल्ये_List=नवीन MessagePatternDoubleList();
            जर(numeric_मूल्ये_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                परत असत्य;
            }
            numeric_मूल्ये=numeric_मूल्ये_List->a.getAlias();
        }
        numeric_मूल्ये_List->copyFrom(
            *other.numeric_मूल्ये_List, other.numeric_मूल्ये_Length, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत असत्य;
        }
        numeric_मूल्ये=numeric_मूल्ये_List->a.getAlias();
        numeric_मूल्ये_Length=other.numeric_मूल्ये_Length;
    }
    परत सत्य;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_मूल्ये_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(स्थिरांक UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    परत *हे;
}

MessagePattern&
MessagePattern::parseChoiceStyle(स्थिरांक UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    परत *हे;
}

MessagePattern&
MessagePattern::parsePluralStyle(स्थिरांक UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    परत *हे;
}

MessagePattern&
MessagePattern::parseSelectStyle(स्थिरांक UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    परत *हे;
}

शून्य
MessagePattern::clear() {
    // Mostly the same as preParse().
    संदेश.हटाओ();
    hasArgNames=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    partsLength=0;
    numeric_मूल्ये_Length=0;
}

तार्किक
MessagePattern::चालक==(स्थिरांक MessagePattern &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    परत
        aposMode==other.aposMode &&
        संदेश==other.संदेश &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() स्थिरांक {
    int32_t hash=(aposMode*37+संदेश.hashCode())*37+partsLength;
    साठी(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    परत hash;
}

int32_t
MessagePattern::validateArgumentName(स्थिरांक UnicodeString &name) {
    जर(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        परत UMSGPAT_ARG_NAME_NOT_VALID;
    }
    परत parse_Arg_संख्या(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() स्थिरांक {
    जर(!needsAutoQuoting) {
        परत संदेश;
    }
    UnicodeString modified(संदेश);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=मोजणी_Parts();
    साठी(int32_t i=count; i>0;) {
        स्थिरांक Part &part=getPart(--i);
        जर(part.getType()==UMSGPAT_PART_TYPE_घाला_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    परत modified;
}

द्विगुण
MessagePattern::get_Numeric_मूल्य(स्थिरांक Part &part) स्थिरांक {
    UMessagePatternPartType type=part.type;
    जर(type==UMSGPAT_PART_TYPE_ARG_INT) {
        परत part.value;
    } अन्यथा जर(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        परत numeric_मूल्ये[part.value];
    } अन्यथा {
        परत UMSGPAT_NO_NUMERIC_मूल्य;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
द्विगुण
MessagePattern::getPluralOffset(int32_t pluralStart) स्थिरांक {
    स्थिरांक Part &part=getPart(pluralStart);
    जर(Part::has_Numeric_मूल्य(part.type)) {
        परत get_Numeric_मूल्य(part);
    } अन्यथा {
        परत 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

तार्किक
MessagePattern::Part::चालक==(स्थिरांक Part &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    परत
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

शून्य
MessagePattern::preParse(स्थिरांक UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    जर(parseError!=nullptr) {
        parseError->ओळ=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    संदेश=pattern;
    hasArgNames=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    partsLength=0;
    numeric_मूल्ये_Length=0;
}

शून्य
MessagePattern::postParse() {
    जर(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    जर(numeric_मूल्ये_List!=nullptr) {
        numeric_मूल्ये=numeric_मूल्ये_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t संदेश_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    जर(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        परत 0;
    }
    int32_t संदेश_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_संदेश_START, index, संदेश_Start_Length, nestingLevel, errorCode);
    index+=संदेश_Start_Length;
    साठी(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
        जर(index>=संदेश.length()) {
            थांबा;
        }
        char16_t c=संदेश.charAt(index++);
        जर(c==u_apos) {
            जर(index==संदेश.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_घाला_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=सत्य;
            } अन्यथा {
                c=संदेश.charAt(index);
                जर(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } अन्यथा जर(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_डावा_Curly_Brace || c==u_उजवा_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    साठी(;;) {
                        index=संदेश.indexOf(u_apos, index+1);
                        जर(index>=0) {
                            जर(/*(index+1)<msg.length() &&*/ संदेश.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } अन्यथा {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                थांबा;
                            }
                        } अन्यथा {
                            // The quoted text reaches to the end of the of the message.
                            index=संदेश.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_घाला_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=सत्य;
                            थांबा;
                        }
                    }
                } अन्यथा {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_घाला_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=सत्य;
                }
            }
        } अन्यथा जर(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_संख्या, index-1, 1, 0, errorCode);
        } अन्यथा जर(c==u_डावा_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } अन्यथा जर((nestingLevel>0 && c==u_उजवा_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_उजवा_Curly_Brace) ? 0 : 1;
            addLimitPart(संदेश_Start, UMSGPAT_PART_TYPE_संदेश_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            जर(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                परत index-1;
            } अन्यथा {
                // continue parsing after the '}'
                परत index;
            }
        }  // else: c is part of literal text
    }
    जर(nestingLevel>0 && !in_शीर्ष_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        परत 0;
    }
    addLimitPart(संदेश_Start, UMSGPAT_PART_TYPE_संदेश_LIMIT, index, 0, nestingLevel, errorCode);
    परत index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    जर(index==संदेश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        परत 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_संख्या(nameIndex, index);
    जर(number>=0) {
        int32_t length=index-nameIndex;
        जर(length>Part::MAX_LENGTH || number>Part::MAX_मूल्य) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
        hasArgNumbers=सत्य;
        addPart(UMSGPAT_PART_TYPE_ARG_संख्या, nameIndex, length, number, errorCode);
    } अन्यथा जर(number==UMSGPAT_ARG_NAME_NOT_संख्या) {
        int32_t length=index-nameIndex;
        जर(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
        hasArgNames=सत्य;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } अन्यथा {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        परत 0;
    }
    index=skipWhiteSpace(index);
    जर(index==संदेश.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        परत 0;
    }
    char16_t c=संदेश.charAt(index);
    जर(c==u_उजवा_Curly_Brace) {
        // all done
    } अन्यथा जर(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        परत 0;
    } अन्यथा /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        पर्यंत(index<संदेश.length() && isArgTypeChar(संदेश.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        जर(index==संदेश.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            परत 0;
        }
        जर(length==0 || ((c=संदेश.charAt(index))!=u_comma && c!=u_उजवा_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            परत 0;
        }
        जर(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        जर(length==6) {
            // case-insensitive comparisons for complex-type names
            जर(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } अन्यथा जर(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } अन्यथा जर(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } अन्यथा जर(length==13) {
            जर(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        जर(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        जर(c==u_उजवा_Curly_Brace) {
            जर(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
        } अन्यथा /* ',' */ {
            ++index;
            जर(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } अन्यथा जर(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } अन्यथा {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    परत index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    पर्यंत(index<संदेश.length()) {
        char16_t c=संदेश.charAt(index++);
        जर(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=संदेश.indexOf(u_apos, index);
            जर(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } अन्यथा जर(c==u_डावा_Curly_Brace) {
            ++nestedBraces;
        } अन्यथा जर(c==u_उजवा_Curly_Brace) {
            जर(nestedBraces>0) {
                --nestedBraces;
            } अन्यथा {
                int32_t length=--index-start;
                जर(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    परत 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                परत index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    परत 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    जर(index==संदेश.length() || संदेश.charAt(index)==u_उजवा_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        परत 0;
    }
    साठी(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t संख्या_Index=index;
        index=skipDouble(index);
        int32_t length=index-संख्या_Index;
        जर(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            परत 0;
        }
        जर(length>Part::MAX_LENGTH) {
            setParseError(parseError, संख्या_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत 0;
        }
        parseDouble(संख्या_Index, index, सत्य, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        जर(index==संदेश.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            परत 0;
        }
        char16_t c=संदेश.charAt(index);
        जर(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            परत 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        जर(index==संदेश.length()) {
            परत index;
        }
        जर(संदेश.charAt(index)==u_उजवा_Curly_Brace) {
            जर(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
            परत index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत 0;
    }
    int32_t start=index;
    UBool isEmpty=सत्य;
    UBool hasOther=असत्य;
    साठी(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==संदेश.length();
        जर(eos || संदेश.charAt(index)==u_उजवा_Curly_Brace) {
            जर(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
            जर(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                परत 0;
            }
            परत index;
        }
        int32_t selectorIndex=index;
        जर(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && संदेश.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            जर(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
            जर(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                परत 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, असत्य,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } अन्यथा {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            जर(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                परत 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            जर( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<संदेश.length() &&
                0==संदेश.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                जर(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    परत 0;
                }
                // allow whitespace between offset: and its value
                int32_t मूल्य_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(मूल्य_Index);
                जर(index==मूल्य_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    परत 0;
                }
                जर((index-मूल्य_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, मूल्य_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    परत 0;
                }
                parseDouble(मूल्य_Index, index, असत्य,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                जर(U_FAILURE(errorCode)) {
                    परत 0;
                }
                isEmpty=असत्य;
                सुरूठेवा;  // no message fragment after the offset
            } अन्यथा {
                // normal selector word
                जर(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    परत 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                जर(0==संदेश.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=सत्य;
                }
            }
        }
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        जर(index==संदेश.length() || संदेश.charAt(index)!=u_डावा_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            परत 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        जर(U_FAILURE(errorCode)) {
            परत 0;
        }
        isEmpty=असत्य;
    }
}

int32_t
MessagePattern::parse_Arg_संख्या(स्थिरांक UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    जर(start>=limit) {
        परत UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_संख्या;
    char16_t c=s.charAt(start++);
    जर(c==0x30) {
        जर(start==limit) {
            परत 0;
        } अन्यथा {
            number=0;
            bad_संख्या=सत्य;  // leading zero
        }
    } अन्यथा जर(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_संख्या=असत्य;
    } अन्यथा {
        परत UMSGPAT_ARG_NAME_NOT_संख्या;
    }
    पर्यंत(start<limit) {
        c=s.charAt(start++);
        जर(0x30<=c && c<=0x39) {
            जर(number>=INT32_MAX/10) {
                bad_संख्या=सत्य;  // overflow
            } अन्यथा {
                number=number*10+(c-0x30);
            }
        } अन्यथा {
            परत UMSGPAT_ARG_NAME_NOT_संख्या;
        }
    }
    // There are only ASCII digits.
    जर(bad_संख्या) {
        परत UMSGPAT_ARG_NAME_NOT_VALID;
    } अन्यथा {
        परत number;
    }
}

शून्य
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    साठी(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=संदेश.charAt(index++);
        जर(c==u_minus) {
            isNegative=1;
            जर(index==limit) {
                थांबा;  // no number
            }
            c=संदेश.charAt(index++);
        } अन्यथा जर(c==u_plus) {
            जर(index==limit) {
                थांबा;  // no number
            }
            c=संदेश.charAt(index++);
        }
        जर(c==0x221e) {  // infinity
            जर(allowInfinity && index==limit) {
                द्विगुण infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                परत;
            } अन्यथा {
                थांबा;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        पर्यंत('0'<=c && c<='9') {
            value=value*10+(c-'0');
            जर(value>(Part::MAX_मूल्य+isNegative)) {
                थांबा;  // not a small-enough integer
            }
            जर(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                परत;
            }
            c=संदेश.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        अक्षर संख्या_Chars[128];
        int32_t capacity = static_cast<int32_t>(आकार(संख्या_Chars));
        int32_t length=limit-start;
        जर(length>=capacity) {
            थांबा;  // number too long
        }
        संदेश.extract(start, length, संख्या_Chars, capacity, US_INV);
        जर (static_cast<int32_t>(uprv_strlen(संख्या_Chars)) < length) {
            थांबा;  // contains non-invariant character that was turned into NUL
        }
        अक्षर *end;
        द्विगुण numeric_मूल्य=uprv_strtod(संख्या_Chars, &end);
        जर(end!=(संख्या_Chars+length)) {
            थांबा;  // parsing error
        }
        addArgDoublePart(numeric_मूल्य, start, length, errorCode);
        परत;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    स्थिरांक char16_t *s=संदेश.getBuffer();
    int32_t संदेश_Length=संदेश.length();
    स्थिरांक char16_t *t=PatternProps::skipWhiteSpace(s+index, संदेश_Length-index);
    परत static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    स्थिरांक char16_t *s=संदेश.getBuffer();
    int32_t संदेश_Length=संदेश.length();
    स्थिरांक char16_t *t=PatternProps::skipIdentifier(s+index, संदेश_Length-index);
    परत static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t संदेश_Length=संदेश.length();
    पर्यंत(index<संदेश_Length) {
        char16_t c=संदेश.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        जर((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            थांबा;
        }
        ++index;
    }
    परत index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    परत (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    परत
        ((c=संदेश.charAt(index++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(index++))==u_h || c==u_H) &&
        ((c=संदेश.charAt(index++))==u_o || c==u_O) &&
        ((c=संदेश.charAt(index++))==u_i || c==u_I) &&
        ((c=संदेश.charAt(index++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    परत
        ((c=संदेश.charAt(index++))==u_p || c==u_P) &&
        ((c=संदेश.charAt(index++))==u_l || c==u_L) &&
        ((c=संदेश.charAt(index++))==u_u || c==u_U) &&
        ((c=संदेश.charAt(index++))==u_r || c==u_R) &&
        ((c=संदेश.charAt(index++))==u_a || c==u_A) &&
        ((c=संदेश.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    परत
        ((c=संदेश.charAt(index++))==u_s || c==u_S) &&
        ((c=संदेश.charAt(index++))==u_e || c==u_E) &&
        ((c=संदेश.charAt(index++))==u_l || c==u_L) &&
        ((c=संदेश.charAt(index++))==u_e || c==u_E) &&
        ((c=संदेश.charAt(index++))==u_c || c==u_C) &&
        ((c=संदेश.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    परत
        ((c=संदेश.charAt(index++))==u_o || c==u_O) &&
        ((c=संदेश.charAt(index++))==u_r || c==u_R) &&
        ((c=संदेश.charAt(index++))==u_d || c==u_D) &&
        ((c=संदेश.charAt(index++))==u_i || c==u_I) &&
        ((c=संदेश.charAt(index++))==u_n || c==u_N) &&
        ((c=संदेश.charAt(index++))==u_a || c==u_A) &&
        ((c=संदेश.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    परत nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_संदेश_START;
}

UBool
MessagePattern::in_शीर्ष_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    परत
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_संदेश_START;
}

शून्य
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    जर(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
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
MessagePattern::addArgDoublePart(द्विगुण numeric_मूल्य, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    int32_t numericIndex=numeric_मूल्ये_Length;
    जर(numeric_मूल्ये_List==nullptr) {
        numeric_मूल्ये_List=नवीन MessagePatternDoubleList();
        जर(numeric_मूल्ये_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            परत;
        }
    } अन्यथा जर(!numeric_मूल्ये_List->ensureCapacityForOneMore(numeric_मूल्ये_Length, errorCode)) {
        परत;
    } अन्यथा {
        जर(numericIndex>Part::MAX_मूल्य) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            परत;
        }
    }
    numeric_मूल्ये_List->a[numeric_मूल्ये_Length++]=numeric_मूल्य;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

शून्य
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    जर(parseError==nullptr) {
        परत;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    जर(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        जर(length>0 && U16_IS_TRAIL(संदेश[index-length])) {
            --length;
        }
    }
    संदेश.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=संदेश.length()-index;
    जर(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        जर(length>0 && U16_IS_LEAD(संदेश[index+length-1])) {
            --length;
        }
    }
    संदेश.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

शून्य
MessageImpl::appendReducedApostrophes(स्थिरांक UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    साठी(;;) {
        int32_t i=s.indexOf(u_apos, start);
        जर(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            थांबा;
        }
        जर(i==doubleApos) {
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
MessageImpl::appendSubMessageWithoutSkipSyntax(स्थिरांक MessagePattern& संदेश_Pattern,
                                               int32_t संदेश_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    स्थिरांक UnicodeString &संदेश_String=संदेश_Pattern.getPatternString();
    int32_t prevIndex=संदेश_Pattern.getPart(संदेश_Start).getLimit();
    साठी(int32_t i=संदेश_Start;;) {
        स्थिरांक MessagePattern::Part &part=संदेश_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        जर(type==UMSGPAT_PART_TYPE_संदेश_LIMIT) {
            परत result.append(संदेश_String, prevIndex, index-prevIndex);
        } अन्यथा जर(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(संदेश_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } अन्यथा जर(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(संदेश_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=संदेश_Pattern.getLimitPartIndex(i);
            index=संदेश_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(संदेश_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
