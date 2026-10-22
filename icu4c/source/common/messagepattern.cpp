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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_FORMATTING

#અન્તર્ભાવ "unicode/messagepattern.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "messageimpl.h"
#અન્તર્ભાવ "patternprops.h"
#અન્તર્ભાવ "putilimp.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

સ્થિર અચલ char16_t u_pound=0x23;
સ્થિર અચલ char16_t u_apos=0x27;
સ્થિર અચલ char16_t u_plus=0x2B;
સ્થિર અચલ char16_t u_comma=0x2C;
સ્થિર અચલ char16_t u_minus=0x2D;
સ્થિર અચલ char16_t u_dot=0x2E;
સ્થિર અચલ char16_t u_colon=0x3A;
સ્થિર અચલ char16_t u_lessThan=0x3C;
સ્થિર અચલ char16_t u_equal=0x3D;
સ્થિર અચલ char16_t u_A=0x41;
સ્થિર અચલ char16_t u_C=0x43;
સ્થિર અચલ char16_t u_D=0x44;
સ્થિર અચલ char16_t u_E=0x45;
સ્થિર અચલ char16_t u_H=0x48;
સ્થિર અચલ char16_t u_I=0x49;
સ્થિર અચલ char16_t u_L=0x4C;
સ્થિર અચલ char16_t u_N=0x4E;
સ્થિર અચલ char16_t u_O=0x4F;
સ્થિર અચલ char16_t u_P=0x50;
સ્થિર અચલ char16_t u_R=0x52;
સ્થિર અચલ char16_t u_S=0x53;
સ્થિર અચલ char16_t u_T=0x54;
સ્થિર અચલ char16_t u_U=0x55;
સ્થિર અચલ char16_t u_Z=0x5A;
સ્થિર અચલ char16_t u_a=0x61;
સ્થિર અચલ char16_t u_c=0x63;
સ્થિર અચલ char16_t u_d=0x64;
સ્થિર અચલ char16_t u_e=0x65;
સ્થિર અચલ char16_t u_f=0x66;
સ્થિર અચલ char16_t u_h=0x68;
સ્થિર અચલ char16_t u_i=0x69;
સ્થિર અચલ char16_t u_l=0x6C;
સ્થિર અચલ char16_t u_n=0x6E;
સ્થિર અચલ char16_t u_o=0x6F;
સ્થિર અચલ char16_t u_p=0x70;
સ્થિર અચલ char16_t u_r=0x72;
સ્થિર અચલ char16_t u_s=0x73;
સ્થિર અચલ char16_t u_t=0x74;
સ્થિર અચલ char16_t u_u=0x75;
સ્થિર અચલ char16_t u_z=0x7A;
સ્થિર અચલ char16_t u_ડાબું_Curly_Brace=0x7B;
સ્થિર અચલ char16_t u_pipe=0x7C;
સ્થિર અચલ char16_t u_જમણું_Curly_Brace=0x7D;
સ્થિર અચલ char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

સ્થિર અચલ char16_t kOffsetColon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

સ્થિર અચલ char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

template<typename T, int32_t ઢગલો_Capacity>
વર્ગ MessagePatternList : જાહેર UMemory {
જાહેર:
    MessagePatternList() {}
    શૂન્ય copyFrom(અચલ MessagePatternList<T, ઢગલો_Capacity> &other,
                  int32_t length,
                  UErrorCode &errorCode);
    UBool ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode);
    UBool equals(અચલ MessagePatternList<T, ઢગલો_Capacity> &other, int32_t length) અચલ {
        માટે(int32_t i=0; i<length; ++i) {
            જો(a[i]!=other.a[i]) { પાછા ખોટું; }
        }
        પાછા સાચું;
    }

    Maybe_ઢગલો_Array<T, ઢગલો_Capacity> a;
};

template<typename T, int32_t ઢગલો_Capacity>
શૂન્ય
MessagePatternList<T, ઢગલો_Capacity>::copyFrom(
        અચલ MessagePatternList<T, ઢગલો_Capacity> &other,
        int32_t length,
        UErrorCode &errorCode) {
    જો(U_SUCCESS(errorCode) && length>0) {
        જો(length>a.getCapacity() && nullptr==a.resize(length)) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*કદ(T));
    }
}

template<typename T, int32_t ઢગલો_Capacity>
UBool
MessagePatternList<T, ઢગલો_Capacity>::ensureCapacityForOneMore(int32_t oldLength, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    જો(a.getCapacity()>oldLength || a.resize(2*oldLength, oldLength)!=nullptr) {
        પાછા સાચું;
    }
    errorCode=U_MEMORY_ALLOCATION_ERROR;
    પાછા ખોટું;
}

// MessagePatternList specializations -------------------------------------- ***

વર્ગ MessagePatternDoubleList : જાહેર MessagePatternList<દ્વિગુણ, 8> {
};

વર્ગ MessagePatternPartsList : જાહેર MessagePatternList<MessagePattern::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

MessagePattern::MessagePattern(UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_કિંમતો_List(nullptr), numeric_કિંમતો(nullptr), numeric_કિંમતો_Length(0),
          hasArgNames(ખોટું), hasArgNumbers(ખોટું), needsAutoQuoting(ખોટું) {
    init(errorCode);
}

MessagePattern::MessagePattern(UMessagePatternApostropheMode mode, UErrorCode &errorCode)
        : aposMode(mode),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_કિંમતો_List(nullptr), numeric_કિંમતો(nullptr), numeric_કિંમતો_Length(0),
          hasArgNames(ખોટું), hasArgNumbers(ખોટું), needsAutoQuoting(ખોટું) {
    init(errorCode);
}

MessagePattern::MessagePattern(અચલ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode)
        : aposMode(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_MODE),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_કિંમતો_List(nullptr), numeric_કિંમતો(nullptr), numeric_કિંમતો_Length(0),
          hasArgNames(ખોટું), hasArgNumbers(ખોટું), needsAutoQuoting(ખોટું) {
    જો(init(errorCode)) {
        parse(pattern, parseError, errorCode);
    }
}

UBool
MessagePattern::init(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    partsList=નવું MessagePatternPartsList();
    જો(partsList==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા ખોટું;
    }
    parts=partsList->a.getAlias();
    પાછા સાચું;
}

MessagePattern::MessagePattern(અચલ MessagePattern &other)
        : UObject(other), aposMode(other.aposMode), સંદેશ(other.સંદેશ),
          partsList(nullptr), parts(nullptr), partsLength(0),
          numeric_કિંમતો_List(nullptr), numeric_કિંમતો(nullptr), numeric_કિંમતો_Length(0),
          hasArgNames(other.hasArgNames), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UErrorCode errorCode=U_ZERO_ERROR;
    જો(!copyStorage(other, errorCode)) {
        clear();
    }
}

MessagePattern&
MessagePattern::operator=(અચલ MessagePattern& other) U_LIFETIME_BOUND {
    જો(આ==&other) {
        પાછા *આ;
    }
    aposMode=other.aposMode;
    સંદેશ=other.સંદેશ;
    hasArgNames=other.hasArgNames;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UErrorCode errorCode=U_ZERO_ERROR;
    જો(!copyStorage(other, errorCode)) {
        clear();
    }
    પાછા *આ;
}

UBool
MessagePattern::copyStorage(અચલ MessagePattern &other, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા ખોટું;
    }
    parts=nullptr;
    partsLength=0;
    numeric_કિંમતો=nullptr;
    numeric_કિંમતો_Length=0;
    જો(partsList==nullptr) {
        partsList=નવું MessagePatternPartsList();
        જો(partsList==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા ખોટું;
        }
        parts=partsList->a.getAlias();
    }
    જો(other.partsLength>0) {
        partsList->copyFrom(*other.partsList, other.partsLength, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા ખોટું;
        }
        parts=partsList->a.getAlias();
        partsLength=other.partsLength;
    }
    જો(other.numeric_કિંમતો_Length>0) {
        જો(numeric_કિંમતો_List==nullptr) {
            numeric_કિંમતો_List=નવું MessagePatternDoubleList();
            જો(numeric_કિંમતો_List==nullptr) {
                errorCode=U_MEMORY_ALLOCATION_ERROR;
                પાછા ખોટું;
            }
            numeric_કિંમતો=numeric_કિંમતો_List->a.getAlias();
        }
        numeric_કિંમતો_List->copyFrom(
            *other.numeric_કિંમતો_List, other.numeric_કિંમતો_Length, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા ખોટું;
        }
        numeric_કિંમતો=numeric_કિંમતો_List->a.getAlias();
        numeric_કિંમતો_Length=other.numeric_કિંમતો_Length;
    }
    પાછા સાચું;
}

MessagePattern::~MessagePattern() {
    delete partsList;
    delete numeric_કિંમતો_List;
}

// MessagePattern API ------------------------------------------------------ ***

MessagePattern&
MessagePattern::parse(અચલ UnicodeString& pattern,
                      UParseError* parseError,
                      UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseMessage(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, errorCode);
    postParse();
    પાછા *આ;
}

MessagePattern&
MessagePattern::parseChoiceStyle(અચલ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parseChoiceStyle(0, 0, parseError, errorCode);
    postParse();
    પાછા *આ;
}

MessagePattern&
MessagePattern::parsePluralStyle(અચલ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, errorCode);
    postParse();
    પાછા *આ;
}

MessagePattern&
MessagePattern::parseSelectStyle(અચલ UnicodeString& pattern,
                                 UParseError* parseError,
                                 UErrorCode& errorCode) U_LIFETIME_BOUND {
    preParse(pattern, parseError, errorCode);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, errorCode);
    postParse();
    પાછા *આ;
}

શૂન્ય
MessagePattern::clear() {
    // Mostly the same as preParse().
    સંદેશ.remove();
    hasArgNames=hasArgNumbers=ખોટું;
    needsAutoQuoting=ખોટું;
    partsLength=0;
    numeric_કિંમતો_Length=0;
}

તાર્કિક
MessagePattern::operator==(અચલ MessagePattern &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    પાછા
        aposMode==other.aposMode &&
        સંદેશ==other.સંદેશ &&
        // parts.equals(o.parts)
        partsLength==other.partsLength &&
        (partsLength==0 || partsList->equals(*other.partsList, partsLength));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
MessagePattern::hashCode() અચલ {
    int32_t hash=(aposMode*37+સંદેશ.hashCode())*37+partsLength;
    માટે(int32_t i=0; i<partsLength; ++i) {
        hash=hash*37+parts[i].hashCode();
    }
    પાછા hash;
}

int32_t
MessagePattern::validateArgumentName(અચલ UnicodeString &name) {
    જો(!PatternProps::isIdentifier(name.getBuffer(), name.length())) {
        પાછા UMSGPAT_ARG_NAME_NOT_VALID;
    }
    પાછા parse_Arg_સંખ્યા(name, 0, name.length());
}

UnicodeString
MessagePattern::autoQuoteApostropheDeep() અચલ {
    જો(!needsAutoQuoting) {
        પાછા સંદેશ;
    }
    UnicodeString modified(સંદેશ);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=ગણતરી_Parts();
    માટે(int32_t i=count; i>0;) {
        અચલ Part &part=getPart(--i);
        જો(part.getType()==UMSGPAT_PART_TYPE_ઉમેરો_CHAR) {
           modified.insert(part.index, static_cast<char16_t>(part.value));
        }
    }
    પાછા modified;
}

દ્વિગુણ
MessagePattern::get_Numeric_કિંમત(અચલ Part &part) અચલ {
    UMessagePatternPartType type=part.type;
    જો(type==UMSGPAT_PART_TYPE_ARG_INT) {
        પાછા part.value;
    } નહીંતર જો(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        પાછા numeric_કિંમતો[part.value];
    } નહીંતર {
        પાછા UMSGPAT_NO_NUMERIC_કિંમત;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
દ્વિગુણ
MessagePattern::getPluralOffset(int32_t pluralStart) અચલ {
    અચલ Part &part=getPart(pluralStart);
    જો(Part::has_Numeric_કિંમત(part.type)) {
        પાછા get_Numeric_કિંમત(part);
    } નહીંતર {
        પાછા 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

તાર્કિક
MessagePattern::Part::operator==(અચલ Part &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    પાછા
        type==other.type &&
        index==other.index &&
        length==other.length &&
        value==other.value &&
        limitPartIndex==other.limitPartIndex;
}

// MessagePattern parser --------------------------------------------------- ***

શૂન્ય
MessagePattern::preParse(અચલ UnicodeString &pattern, UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    જો(parseError!=nullptr) {
        parseError->લીટી=0;
        parseError->offset=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    સંદેશ=pattern;
    hasArgNames=hasArgNumbers=ખોટું;
    needsAutoQuoting=ખોટું;
    partsLength=0;
    numeric_કિંમતો_Length=0;
}

શૂન્ય
MessagePattern::postParse() {
    જો(partsList!=nullptr) {
        parts=partsList->a.getAlias();
    }
    જો(numeric_કિંમતો_List!=nullptr) {
        numeric_કિંમતો=numeric_કિંમતો_List->a.getAlias();
    }
}

int32_t
MessagePattern::parseMessage(int32_t index, int32_t સંદેશ_Start_Length,
                             int32_t nestingLevel, UMessagePatternArgType parentType,
                             UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    જો(nestingLevel>Part::MAX_NESTED_LEVELS) {
        errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
        પાછા 0;
    }
    int32_t સંદેશ_Start=partsLength;
    addPart(UMSGPAT_PART_TYPE_સંદેશ_START, index, સંદેશ_Start_Length, nestingLevel, errorCode);
    index+=સંદેશ_Start_Length;
    માટે(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
        જો(index>=સંદેશ.length()) {
            તોડો;
        }
        char16_t c=સંદેશ.charAt(index++);
        જો(c==u_apos) {
            જો(index==સંદેશ.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                addPart(UMSGPAT_PART_TYPE_ઉમેરો_CHAR, index, 0,
                        u_apos, errorCode);  // value=char to be inserted
                needsAutoQuoting=સાચું;
            } નહીંતર {
                c=સંદેશ.charAt(index);
                જો(c==u_apos) {
                    // double apostrophe, skip the second one
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                } નહીંતર જો(
                    aposMode==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ડાબું_Curly_Brace || c==u_જમણું_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index-1, 1, 0, errorCode);
                    // find the end of the quoted literal text
                    માટે(;;) {
                        index=સંદેશ.indexOf(u_apos, index+1);
                        જો(index>=0) {
                            જો(/*(index+1)<msg.length() &&*/ સંદેશ.charAt(index+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++index, 1, 0, errorCode);
                            } નહીંતર {
                                // skip the quote-ending apostrophe
                                addPart(UMSGPAT_PART_TYPE_SKIP_SYNTAX, index++, 1, 0, errorCode);
                                તોડો;
                            }
                        } નહીંતર {
                            // The quoted text reaches to the end of the of the message.
                            index=સંદેશ.length();
                            // Add a Part for auto-quoting.
                            addPart(UMSGPAT_PART_TYPE_ઉમેરો_CHAR, index, 0,
                                    u_apos, errorCode);  // value=char to be inserted
                            needsAutoQuoting=સાચું;
                            તોડો;
                        }
                    }
                } નહીંતર {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    addPart(UMSGPAT_PART_TYPE_ઉમેરો_CHAR, index, 0,
                            u_apos, errorCode);  // value=char to be inserted
                    needsAutoQuoting=સાચું;
                }
            }
        } નહીંતર જો(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            addPart(UMSGPAT_PART_TYPE_REPLACE_સંખ્યા, index-1, 1, 0, errorCode);
        } નહીંતર જો(c==u_ડાબું_Curly_Brace) {
            index=parseArg(index-1, 1, nestingLevel, parseError, errorCode);
        } નહીંતર જો((nestingLevel>0 && c==u_જમણું_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t limitLength=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_જમણું_Curly_Brace) ? 0 : 1;
            addLimitPart(સંદેશ_Start, UMSGPAT_PART_TYPE_સંદેશ_LIMIT, index-1, limitLength,
                         nestingLevel, errorCode);
            જો(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                પાછા index-1;
            } નહીંતર {
                // continue parsing after the '}'
                પાછા index;
            }
        }  // else: c is part of literal text
    }
    જો(nestingLevel>0 && !in_ટોચ_Level_Choice_Message(nestingLevel, parentType)) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        પાછા 0;
    }
    addLimitPart(સંદેશ_Start, UMSGPAT_PART_TYPE_સંદેશ_LIMIT, index, 0, nestingLevel, errorCode);
    પાછા index;
}

int32_t
MessagePattern::parseArg(int32_t index, int32_t argStartLength, int32_t nestingLevel,
                         UParseError *parseError, UErrorCode &errorCode) {
    int32_t argStart=partsLength;
    UMessagePatternArgType argType=UMSGPAT_ARG_TYPE_NONE;
    addPart(UMSGPAT_PART_TYPE_ARG_START, index, argStartLength, argType, errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    int32_t nameIndex=index=skipWhiteSpace(index+argStartLength);
    જો(index==સંદેશ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        પાછા 0;
    }
    // parse argument name or number
    index=skipIdentifier(index);
    int32_t number=parse_Arg_સંખ્યા(nameIndex, index);
    જો(number>=0) {
        int32_t length=index-nameIndex;
        જો(length>Part::MAX_LENGTH || number>Part::MAX_કિંમત) {
            setParseError(parseError, nameIndex);  // Argument number too large.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
        hasArgNumbers=સાચું;
        addPart(UMSGPAT_PART_TYPE_ARG_સંખ્યા, nameIndex, length, number, errorCode);
    } નહીંતર જો(number==UMSGPAT_ARG_NAME_NOT_સંખ્યા) {
        int32_t length=index-nameIndex;
        જો(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
        hasArgNames=સાચું;
        addPart(UMSGPAT_PART_TYPE_ARG_NAME, nameIndex, length, 0, errorCode);
    } નહીંતર {  // number<-1 (ARG_NAME_NOT_VALID)
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        પાછા 0;
    }
    index=skipWhiteSpace(index);
    જો(index==સંદેશ.length()) {
        setParseError(parseError, 0);  // Unmatched '{' braces in message.
        errorCode=U_UNMATCHED_BRACES;
        પાછા 0;
    }
    char16_t c=સંદેશ.charAt(index);
    જો(c==u_જમણું_Curly_Brace) {
        // all done
    } નહીંતર જો(c!=u_comma) {
        setParseError(parseError, nameIndex);  // Bad argument syntax.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        પાછા 0;
    } નહીંતર /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t typeIndex=index=skipWhiteSpace(index+1);
        જ્યારે(index<સંદેશ.length() && isArgTypeChar(સંદેશ.charAt(index))) {
            ++index;
        }
        int32_t length=index-typeIndex;
        index=skipWhiteSpace(index);
        જો(index==સંદેશ.length()) {
            setParseError(parseError, 0);  // Unmatched '{' braces in message.
            errorCode=U_UNMATCHED_BRACES;
            પાછા 0;
        }
        જો(length==0 || ((c=સંદેશ.charAt(index))!=u_comma && c!=u_જમણું_Curly_Brace)) {
            setParseError(parseError, nameIndex);  // Bad argument syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            પાછા 0;
        }
        જો(length>Part::MAX_LENGTH) {
            setParseError(parseError, nameIndex);  // Argument type name too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        જો(length==6) {
            // case-insensitive comparisons for complex-type names
            જો(isChoice(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } નહીંતર જો(isPlural(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } નહીંતર જો(isSelect(typeIndex)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } નહીંતર જો(length==13) {
            જો(isSelect(typeIndex) && isOrdinal(typeIndex+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        જો(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            addPart(UMSGPAT_PART_TYPE_ARG_TYPE, typeIndex, length, 0, errorCode);
        }
        // look for an argument style (pattern)
        જો(c==u_જમણું_Curly_Brace) {
            જો(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                setParseError(parseError, nameIndex);  // No style field for complex argument.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
        } નહીંતર /* ',' */ {
            ++index;
            જો(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                index=parseSimpleStyle(index, parseError, errorCode);
            } નહીંતર જો(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                index=parseChoiceStyle(index, nestingLevel, parseError, errorCode);
            } નહીંતર {
                index=parsePluralOrSelectStyle(argType, index, nestingLevel, parseError, errorCode);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    addLimitPart(argStart, UMSGPAT_PART_TYPE_ARG_LIMIT, index, 1, argType, errorCode);
    પાછા index+1;
}

int32_t
MessagePattern::parseSimpleStyle(int32_t index, UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    int32_t start=index;
    int32_t nestedBraces=0;
    જ્યારે(index<સંદેશ.length()) {
        char16_t c=સંદેશ.charAt(index++);
        જો(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            index=સંદેશ.indexOf(u_apos, index);
            જો(index<0) {
                // Quoted literal argument style text reaches to the end of the message.
                setParseError(parseError, start);
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
            // skip the quote-ending apostrophe
            ++index;
        } નહીંતર જો(c==u_ડાબું_Curly_Brace) {
            ++nestedBraces;
        } નહીંતર જો(c==u_જમણું_Curly_Brace) {
            જો(nestedBraces>0) {
                --nestedBraces;
            } નહીંતર {
                int32_t length=--index-start;
                જો(length>Part::MAX_LENGTH) {
                    setParseError(parseError, start);  // Argument style text too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    પાછા 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, errorCode);
                પાછા index;
            }
        }  // c is part of literal text
    }
    setParseError(parseError, 0);  // Unmatched '{' braces in message.
    errorCode=U_UNMATCHED_BRACES;
    પાછા 0;
}

int32_t
MessagePattern::parseChoiceStyle(int32_t index, int32_t nestingLevel,
                                 UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    int32_t start=index;
    index=skipWhiteSpace(index);
    જો(index==સંદેશ.length() || સંદેશ.charAt(index)==u_જમણું_Curly_Brace) {
        setParseError(parseError, 0);  // Missing choice argument pattern.
        errorCode=U_PATTERN_SYNTAX_ERROR;
        પાછા 0;
    }
    માટે(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t સંખ્યા_Index=index;
        index=skipDouble(index);
        int32_t length=index-સંખ્યા_Index;
        જો(length==0) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            પાછા 0;
        }
        જો(length>Part::MAX_LENGTH) {
            setParseError(parseError, સંખ્યા_Index);  // Choice number too long.
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
        parseDouble(સંખ્યા_Index, index, સાચું, parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
        // Parse the separator.
        index=skipWhiteSpace(index);
        જો(index==સંદેશ.length()) {
            setParseError(parseError, start);  // Bad choice pattern syntax.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            પાછા 0;
        }
        char16_t c=સંદેશ.charAt(index);
        જો(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            setParseError(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            પાછા 0;
        }
        addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, index, 1, 0, errorCode);
        // Parse the message fragment.
        index=parseMessage(++index, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        જો(index==સંદેશ.length()) {
            પાછા index;
        }
        જો(સંદેશ.charAt(index)==u_જમણું_Curly_Brace) {
            જો(!inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad choice pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
            પાછા index;
        }  // else the terminator is '|'
        index=skipWhiteSpace(index+1);
    }
}

int32_t
MessagePattern::parsePluralOrSelectStyle(UMessagePatternArgType argType,
                                         int32_t index, int32_t nestingLevel,
                                         UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા 0;
    }
    int32_t start=index;
    UBool isEmpty=સાચું;
    UBool hasOther=ખોટું;
    માટે(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        index=skipWhiteSpace(index);
        UBool eos=index==સંદેશ.length();
        જો(eos || સંદેશ.charAt(index)==u_જમણું_Curly_Brace) {
            જો(eos==inMessageFormatPattern(nestingLevel)) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
            જો(!hasOther) {
                setParseError(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                errorCode=U_DEFAULT_KEYWORD_MISSING;
                પાછા 0;
            }
            પાછા index;
        }
        int32_t selectorIndex=index;
        જો(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && સંદેશ.charAt(selectorIndex)==u_equal) {
            // explicit-value plural selector: =double
            index=skipDouble(index+1);
            int32_t length=index-selectorIndex;
            જો(length==1) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
            જો(length>Part::MAX_LENGTH) {
                setParseError(parseError, selectorIndex);  // Argument selector too long.
                errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                પાછા 0;
            }
            addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
            parseDouble(selectorIndex+1, index, ખોટું,
                        parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
        } નહીંતર {
            index=skipIdentifier(index);
            int32_t length=index-selectorIndex;
            જો(length==0) {
                setParseError(parseError, start);  // Bad plural/select pattern syntax.
                errorCode=U_PATTERN_SYNTAX_ERROR;
                પાછા 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            જો( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && index<સંદેશ.length() &&
                0==સંદેશ.compare(selectorIndex, 7, kOffsetColon, 0, 7)
            ) {
                // plural offset, not a selector
                જો(!isEmpty) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    setParseError(parseError, start);
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    પાછા 0;
                }
                // allow whitespace between offset: and its value
                int32_t કિંમત_Index=skipWhiteSpace(index+1);  // The ':' is at index.
                index=skipDouble(કિંમત_Index);
                જો(index==કિંમત_Index) {
                    setParseError(parseError, start);  // Missing value for plural 'offset:'.
                    errorCode=U_PATTERN_SYNTAX_ERROR;
                    પાછા 0;
                }
                જો((index-કિંમત_Index)>Part::MAX_LENGTH) {
                    setParseError(parseError, કિંમત_Index);  // Plural offset value too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    પાછા 0;
                }
                parseDouble(કિંમત_Index, index, ખોટું,
                            parseError, errorCode);  // adds ARG_INT or ARG_DOUBLE
                જો(U_FAILURE(errorCode)) {
                    પાછા 0;
                }
                isEmpty=ખોટું;
                ચાલુ;  // no message fragment after the offset
            } નહીંતર {
                // normal selector word
                જો(length>Part::MAX_LENGTH) {
                    setParseError(parseError, selectorIndex);  // Argument selector too long.
                    errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                    પાછા 0;
                }
                addPart(UMSGPAT_PART_TYPE_ARG_SELECTOR, selectorIndex, length, 0, errorCode);
                જો(0==સંદેશ.compare(selectorIndex, length, kOther, 0, 5)) {
                    hasOther=સાચું;
                }
            }
        }
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }

        // parse the message fragment following the selector
        index=skipWhiteSpace(index);
        જો(index==સંદેશ.length() || સંદેશ.charAt(index)!=u_ડાબું_Curly_Brace) {
            setParseError(parseError, selectorIndex);  // No message fragment after plural/select selector.
            errorCode=U_PATTERN_SYNTAX_ERROR;
            પાછા 0;
        }
        index=parseMessage(index, 1, nestingLevel+1, argType, parseError, errorCode);
        જો(U_FAILURE(errorCode)) {
            પાછા 0;
        }
        isEmpty=ખોટું;
    }
}

int32_t
MessagePattern::parse_Arg_સંખ્યા(અચલ UnicodeString &s, int32_t start, int32_t limit) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    જો(start>=limit) {
        પાછા UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_સંખ્યા;
    char16_t c=s.charAt(start++);
    જો(c==0x30) {
        જો(start==limit) {
            પાછા 0;
        } નહીંતર {
            number=0;
            bad_સંખ્યા=સાચું;  // leading zero
        }
    } નહીંતર જો(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_સંખ્યા=ખોટું;
    } નહીંતર {
        પાછા UMSGPAT_ARG_NAME_NOT_સંખ્યા;
    }
    જ્યારે(start<limit) {
        c=s.charAt(start++);
        જો(0x30<=c && c<=0x39) {
            જો(number>=INT32_MAX/10) {
                bad_સંખ્યા=સાચું;  // overflow
            } નહીંતર {
                number=number*10+(c-0x30);
            }
        } નહીંતર {
            પાછા UMSGPAT_ARG_NAME_NOT_સંખ્યા;
        }
    }
    // There are only ASCII digits.
    જો(bad_સંખ્યા) {
        પાછા UMSGPAT_ARG_NAME_NOT_VALID;
    } નહીંતર {
        પાછા number;
    }
}

શૂન્ય
MessagePattern::parseDouble(int32_t start, int32_t limit, UBool allowInfinity,
                            UParseError *parseError, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    U_ASSERT(start<limit);
    // fake loop for easy exit and single throw statement
    માટે(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t index=start;
        char16_t c=સંદેશ.charAt(index++);
        જો(c==u_minus) {
            isNegative=1;
            જો(index==limit) {
                તોડો;  // no number
            }
            c=સંદેશ.charAt(index++);
        } નહીંતર જો(c==u_plus) {
            જો(index==limit) {
                તોડો;  // no number
            }
            c=સંદેશ.charAt(index++);
        }
        જો(c==0x221e) {  // infinity
            જો(allowInfinity && index==limit) {
                દ્વિગુણ infinity=uprv_getInfinity();
                addArgDoublePart(
                    isNegative!=0 ? -infinity : infinity,
                    start, limit-start, errorCode);
                પાછા;
            } નહીંતર {
                તોડો;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        જ્યારે('0'<=c && c<='9') {
            value=value*10+(c-'0');
            જો(value>(Part::MAX_કિંમત+isNegative)) {
                તોડો;  // not a small-enough integer
            }
            જો(index==limit) {
                addPart(UMSGPAT_PART_TYPE_ARG_INT, start, limit-start,
                        isNegative!=0 ? -value : value, errorCode);
                પાછા;
            }
            c=સંદેશ.charAt(index++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        અક્ષર સંખ્યા_Chars[128];
        int32_t capacity = static_cast<int32_t>(કદ(સંખ્યા_Chars));
        int32_t length=limit-start;
        જો(length>=capacity) {
            તોડો;  // number too long
        }
        સંદેશ.extract(start, length, સંખ્યા_Chars, capacity, US_INV);
        જો (static_cast<int32_t>(uprv_strlen(સંખ્યા_Chars)) < length) {
            તોડો;  // contains non-invariant character that was turned into NUL
        }
        અક્ષર *end;
        દ્વિગુણ numeric_કિંમત=uprv_strtod(સંખ્યા_Chars, &end);
        જો(end!=(સંખ્યા_Chars+length)) {
            તોડો;  // parsing error
        }
        addArgDoublePart(numeric_કિંમત, start, length, errorCode);
        પાછા;
    }
    setParseError(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    errorCode=U_PATTERN_SYNTAX_ERROR;
}

int32_t
MessagePattern::skipWhiteSpace(int32_t index) {
    અચલ char16_t *s=સંદેશ.getBuffer();
    int32_t સંદેશ_Length=સંદેશ.length();
    અચલ char16_t *t=PatternProps::skipWhiteSpace(s+index, સંદેશ_Length-index);
    પાછા static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipIdentifier(int32_t index) {
    અચલ char16_t *s=સંદેશ.getBuffer();
    int32_t સંદેશ_Length=સંદેશ.length();
    અચલ char16_t *t=PatternProps::skipIdentifier(s+index, સંદેશ_Length-index);
    પાછા static_cast<int32_t>(t - s);
}

int32_t
MessagePattern::skipDouble(int32_t index) {
    int32_t સંદેશ_Length=સંદેશ.length();
    જ્યારે(index<સંદેશ_Length) {
        char16_t c=સંદેશ.charAt(index);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        જો((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            તોડો;
        }
        ++index;
    }
    પાછા index;
}

UBool
MessagePattern::isArgTypeChar(UChar32 c) {
    પાછા (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
MessagePattern::isChoice(int32_t index) {
    char16_t c;
    પાછા
        ((c=સંદેશ.charAt(index++))==u_c || c==u_C) &&
        ((c=સંદેશ.charAt(index++))==u_h || c==u_H) &&
        ((c=સંદેશ.charAt(index++))==u_o || c==u_O) &&
        ((c=સંદેશ.charAt(index++))==u_i || c==u_I) &&
        ((c=સંદેશ.charAt(index++))==u_c || c==u_C) &&
        ((c=સંદેશ.charAt(index))==u_e || c==u_E);
}

UBool
MessagePattern::isPlural(int32_t index) {
    char16_t c;
    પાછા
        ((c=સંદેશ.charAt(index++))==u_p || c==u_P) &&
        ((c=સંદેશ.charAt(index++))==u_l || c==u_L) &&
        ((c=સંદેશ.charAt(index++))==u_u || c==u_U) &&
        ((c=સંદેશ.charAt(index++))==u_r || c==u_R) &&
        ((c=સંદેશ.charAt(index++))==u_a || c==u_A) &&
        ((c=સંદેશ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::isSelect(int32_t index) {
    char16_t c;
    પાછા
        ((c=સંદેશ.charAt(index++))==u_s || c==u_S) &&
        ((c=સંદેશ.charAt(index++))==u_e || c==u_E) &&
        ((c=સંદેશ.charAt(index++))==u_l || c==u_L) &&
        ((c=સંદેશ.charAt(index++))==u_e || c==u_E) &&
        ((c=સંદેશ.charAt(index++))==u_c || c==u_C) &&
        ((c=સંદેશ.charAt(index))==u_t || c==u_T);
}

UBool
MessagePattern::isOrdinal(int32_t index) {
    char16_t c;
    પાછા
        ((c=સંદેશ.charAt(index++))==u_o || c==u_O) &&
        ((c=સંદેશ.charAt(index++))==u_r || c==u_R) &&
        ((c=સંદેશ.charAt(index++))==u_d || c==u_D) &&
        ((c=સંદેશ.charAt(index++))==u_i || c==u_I) &&
        ((c=સંદેશ.charAt(index++))==u_n || c==u_N) &&
        ((c=સંદેશ.charAt(index++))==u_a || c==u_A) &&
        ((c=સંદેશ.charAt(index))==u_l || c==u_L);
}

UBool
MessagePattern::inMessageFormatPattern(int32_t nestingLevel) {
    પાછા nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_સંદેશ_START;
}

UBool
MessagePattern::in_ટોચ_Level_Choice_Message(int32_t nestingLevel, UMessagePatternArgType parentType) {
    પાછા
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_સંદેશ_START;
}

શૂન્ય
MessagePattern::addPart(UMessagePatternPartType type, int32_t index, int32_t length,
                        int32_t value, UErrorCode &errorCode) {
    જો(partsList->ensureCapacityForOneMore(partsLength, errorCode)) {
        Part &part=partsList->a[partsLength++];
        part.type=type;
        part.index=index;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.limitPartIndex=0;
    }
}

શૂન્ય
MessagePattern::addLimitPart(int32_t start,
                             UMessagePatternPartType type, int32_t index, int32_t length,
                             int32_t value, UErrorCode &errorCode) {
    partsList->a[start].limitPartIndex=partsLength;
    addPart(type, index, length, value, errorCode);
}

શૂન્ય
MessagePattern::addArgDoublePart(દ્વિગુણ numeric_કિંમત, int32_t start, int32_t length,
                                 UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    int32_t numericIndex=numeric_કિંમતો_Length;
    જો(numeric_કિંમતો_List==nullptr) {
        numeric_કિંમતો_List=નવું MessagePatternDoubleList();
        જો(numeric_કિંમતો_List==nullptr) {
            errorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા;
        }
    } નહીંતર જો(!numeric_કિંમતો_List->ensureCapacityForOneMore(numeric_કિંમતો_Length, errorCode)) {
        પાછા;
    } નહીંતર {
        જો(numericIndex>Part::MAX_કિંમત) {
            errorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા;
        }
    }
    numeric_કિંમતો_List->a[numeric_કિંમતો_Length++]=numeric_કિંમત;
    addPart(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numericIndex, errorCode);
}

શૂન્ય
MessagePattern::setParseError(UParseError *parseError, int32_t index) {
    જો(parseError==nullptr) {
        પાછા;
    }
    parseError->offset=index;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=index;
    જો(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        જો(length>0 && U16_IS_TRAIL(સંદેશ[index-length])) {
            --length;
        }
    }
    સંદેશ.extract(index-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=સંદેશ.length()-index;
    જો(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        જો(length>0 && U16_IS_LEAD(સંદેશ[index+length-1])) {
            --length;
        }
    }
    સંદેશ.extract(index, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

શૂન્ય
MessageImpl::appendReducedApostrophes(અચલ UnicodeString &s, int32_t start, int32_t limit,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    માટે(;;) {
        int32_t i=s.indexOf(u_apos, start);
        જો(i<0 || i>=limit) {
            sb.append(s, start, limit-start);
            તોડો;
        }
        જો(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++start;
            doubleApos=-1;
        } નહીંતર {
            // Append text between apostrophes and skip this one.
            sb.append(s, start, i-start);
            doubleApos=start=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
MessageImpl::appendSubMessageWithoutSkipSyntax(અચલ MessagePattern& સંદેશ_Pattern,
                                               int32_t સંદેશ_Start,
                                               UnicodeString& result U_LIFETIME_BOUND) {
    અચલ UnicodeString &સંદેશ_String=સંદેશ_Pattern.getPatternString();
    int32_t prevIndex=સંદેશ_Pattern.getPart(સંદેશ_Start).getLimit();
    માટે(int32_t i=સંદેશ_Start;;) {
        અચલ MessagePattern::Part &part=સંદેશ_Pattern.getPart(++i);
        UMessagePatternPartType type=part.getType();
        int32_t index=part.getIndex();
        જો(type==UMSGPAT_PART_TYPE_સંદેશ_LIMIT) {
            પાછા result.append(સંદેશ_String, prevIndex, index-prevIndex);
        } નહીંતર જો(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            result.append(સંદેશ_String, prevIndex, index-prevIndex);
            prevIndex=part.getLimit();
        } નહીંતર જો(type==UMSGPAT_PART_TYPE_ARG_START) {
            result.append(સંદેશ_String, prevIndex, index-prevIndex);
            prevIndex=index;
            i=સંદેશ_Pattern.getLimitPartIndex(i);
            index=સંદેશ_Pattern.getPart(i).getLimit();
            appendReducedApostrophes(સંદેશ_String, prevIndex, index, result);
            prevIndex=index;
        }
    }
}

U_NAMESPACE_END

#endif  // !UCONFIG_NO_FORMATTING
