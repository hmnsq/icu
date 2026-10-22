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

#यदि !UCONFIG_NO_FORMATTING

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

स्थिर ध्रुव char16_t k_विस्थापनम्_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

स्थिर ध्रुव char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

प्रतिरूपम्<प्रकारनाम T, int32_t राशिः_क्षमता>
वर्ग सन्देशः_आदर्शः_List : सार्वजनिक UMemory {
सार्वजनिक:
    सन्देशः_आदर्शः_List() {}
    शून्य copyFrom(ध्रुव सन्देशः_आदर्शः_List<T, राशिः_क्षमता> &other,
                  int32_t length,
                  UError_संहिता &error_संहिता);
    UBool ensure_क्षमता_For_One_अधिकम्(int32_t old_दीर्घता, UError_संहिता &error_संहिता);
    UBool equals(ध्रुव सन्देशः_आदर्शः_List<T, राशिः_क्षमता> &other, int32_t length) ध्रुव {
        प्रति(int32_t i=0; i<length; ++i) {
            यदि(a[i]!=other.a[i]) { प्रत्यागम असत्य; }
        }
        प्रत्यागम सत्य;
    }

    Maybe_राशिः_Array<T, राशिः_क्षमता> a;
};

प्रतिरूपम्<प्रकारनाम T, int32_t राशिः_क्षमता>
शून्य
सन्देशः_आदर्शः_List<T, राशिः_क्षमता>::copyFrom(
        ध्रुव सन्देशः_आदर्शः_List<T, राशिः_क्षमता> &other,
        int32_t length,
        UError_संहिता &error_संहिता) {
    यदि(U_SUCCESS(error_संहिता) && length>0) {
        यदि(length>a.get_क्षमता() && शून्यसूचकः==a.resize(length)) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*परिमाण(T));
    }
}

प्रतिरूपम्<प्रकारनाम T, int32_t राशिः_क्षमता>
UBool
सन्देशः_आदर्शः_List<T, राशिः_क्षमता>::ensure_क्षमता_For_One_अधिकम्(int32_t old_दीर्घता, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    यदि(a.get_क्षमता()>old_दीर्घता || a.resize(2*old_दीर्घता, old_दीर्घता)!=शून्यसूचकः) {
        प्रत्यागम सत्य;
    }
    error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    प्रत्यागम असत्य;
}

// MessagePatternList specializations -------------------------------------- ***

वर्ग सन्देशः_आदर्शः_Double_List : सार्वजनिक सन्देशः_आदर्शः_List<द्विगुण, 8> {
};

वर्ग सन्देशः_आदर्शः_Parts_List : सार्वजनिक सन्देशः_आदर्शः_List<सन्देशः_आदर्शः::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

सन्देशः_आदर्शः::सन्देशः_आदर्शः(UError_संहिता &error_संहिता)
        : apos_बहुलकम्(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_बहुलकम्),
          partsList(शून्यसूचकः), parts(शून्यसूचकः), parts_दीर्घता(0),
          numeric_मानानि_List(शून्यसूचकः), numeric_मानानि(शून्यसूचकः), numeric_मानानि_दीर्घता(0),
          has_Arg_नामानि(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(error_संहिता);
}

सन्देशः_आदर्शः::सन्देशः_आदर्शः(UMessage_आदर्शः_Apostrophe_बहुलकम् बहुलकम्, UError_संहिता &error_संहिता)
        : apos_बहुलकम्(बहुलकम्),
          partsList(शून्यसूचकः), parts(शून्यसूचकः), parts_दीर्घता(0),
          numeric_मानानि_List(शून्यसूचकः), numeric_मानानि(शून्यसूचकः), numeric_मानानि_दीर्घता(0),
          has_Arg_नामानि(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    init(error_संहिता);
}

सन्देशः_आदर्शः::सन्देशः_आदर्शः(ध्रुव UnicodeString &आदर्शः, UParseError *parseError, UError_संहिता &error_संहिता)
        : apos_बहुलकम्(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_बहुलकम्),
          partsList(शून्यसूचकः), parts(शून्यसूचकः), parts_दीर्घता(0),
          numeric_मानानि_List(शून्यसूचकः), numeric_मानानि(शून्यसूचकः), numeric_मानानि_दीर्घता(0),
          has_Arg_नामानि(असत्य), hasArgNumbers(असत्य), needsAutoQuoting(असत्य) {
    यदि(init(error_संहिता)) {
        parse(आदर्शः, parseError, error_संहिता);
    }
}

UBool
सन्देशः_आदर्शः::init(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    partsList=नूतन सन्देशः_आदर्शः_Parts_List();
    यदि(partsList==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम असत्य;
    }
    parts=partsList->a.getAlias();
    प्रत्यागम सत्य;
}

सन्देशः_आदर्शः::सन्देशः_आदर्शः(ध्रुव सन्देशः_आदर्शः &other)
        : UObject(other), apos_बहुलकम्(other.apos_बहुलकम्), सन्देशः(other.सन्देशः),
          partsList(शून्यसूचकः), parts(शून्यसूचकः), parts_दीर्घता(0),
          numeric_मानानि_List(शून्यसूचकः), numeric_मानानि(शून्यसूचकः), numeric_मानानि_दीर्घता(0),
          has_Arg_नामानि(other.has_Arg_नामानि), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    यदि(!copyStorage(other, error_संहिता)) {
        clear();
    }
}

सन्देशः_आदर्शः&
सन्देशः_आदर्शः::संक्रियकः=(ध्रुव सन्देशः_आदर्शः& other) U_LIFETIME_BOUND {
    यदि(एतत्==&other) {
        प्रत्यागम *एतत्;
    }
    apos_बहुलकम्=other.apos_बहुलकम्;
    सन्देशः=other.सन्देशः;
    has_Arg_नामानि=other.has_Arg_नामानि;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    यदि(!copyStorage(other, error_संहिता)) {
        clear();
    }
    प्रत्यागम *एतत्;
}

UBool
सन्देशः_आदर्शः::copyStorage(ध्रुव सन्देशः_आदर्शः &other, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    parts=शून्यसूचकः;
    parts_दीर्घता=0;
    numeric_मानानि=शून्यसूचकः;
    numeric_मानानि_दीर्घता=0;
    यदि(partsList==शून्यसूचकः) {
        partsList=नूतन सन्देशः_आदर्शः_Parts_List();
        यदि(partsList==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम असत्य;
        }
        parts=partsList->a.getAlias();
    }
    यदि(other.parts_दीर्घता>0) {
        partsList->copyFrom(*other.partsList, other.parts_दीर्घता, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम असत्य;
        }
        parts=partsList->a.getAlias();
        parts_दीर्घता=other.parts_दीर्घता;
    }
    यदि(other.numeric_मानानि_दीर्घता>0) {
        यदि(numeric_मानानि_List==शून्यसूचकः) {
            numeric_मानानि_List=नूतन सन्देशः_आदर्शः_Double_List();
            यदि(numeric_मानानि_List==शून्यसूचकः) {
                error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम असत्य;
            }
            numeric_मानानि=numeric_मानानि_List->a.getAlias();
        }
        numeric_मानानि_List->copyFrom(
            *other.numeric_मानानि_List, other.numeric_मानानि_दीर्घता, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम असत्य;
        }
        numeric_मानानि=numeric_मानानि_List->a.getAlias();
        numeric_मानानि_दीर्घता=other.numeric_मानानि_दीर्घता;
    }
    प्रत्यागम सत्य;
}

सन्देशः_आदर्शः::~सन्देशः_आदर्शः() {
    विलोपय partsList;
    विलोपय numeric_मानानि_List;
}

// MessagePattern API ------------------------------------------------------ ***

सन्देशः_आदर्शः&
सन्देशः_आदर्शः::parse(ध्रुव UnicodeString& आदर्शः,
                      UParseError* parseError,
                      UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    preParse(आदर्शः, parseError, error_संहिता);
    parse_सन्देशः(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_संहिता);
    postParse();
    प्रत्यागम *एतत्;
}

सन्देशः_आदर्शः&
सन्देशः_आदर्शः::parseChoiceStyle(ध्रुव UnicodeString& आदर्शः,
                                 UParseError* parseError,
                                 UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    preParse(आदर्शः, parseError, error_संहिता);
    parseChoiceStyle(0, 0, parseError, error_संहिता);
    postParse();
    प्रत्यागम *एतत्;
}

सन्देशः_आदर्शः&
सन्देशः_आदर्शः::parsePluralStyle(ध्रुव UnicodeString& आदर्शः,
                                 UParseError* parseError,
                                 UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    preParse(आदर्शः, parseError, error_संहिता);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_संहिता);
    postParse();
    प्रत्यागम *एतत्;
}

सन्देशः_आदर्शः&
सन्देशः_आदर्शः::parseSelectStyle(ध्रुव UnicodeString& आदर्शः,
                                 UParseError* parseError,
                                 UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    preParse(आदर्शः, parseError, error_संहिता);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_संहिता);
    postParse();
    प्रत्यागम *एतत्;
}

शून्य
सन्देशः_आदर्शः::clear() {
    // Mostly the same as preParse().
    सन्देशः.remove();
    has_Arg_नामानि=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    parts_दीर्घता=0;
    numeric_मानानि_दीर्घता=0;
}

तार्किक
सन्देशः_आदर्शः::संक्रियकः==(ध्रुव सन्देशः_आदर्शः &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    प्रत्यागम
        apos_बहुलकम्==other.apos_बहुलकम् &&
        सन्देशः==other.सन्देशः &&
        // parts.equals(o.parts)
        parts_दीर्घता==other.parts_दीर्घता &&
        (parts_दीर्घता==0 || partsList->equals(*other.partsList, parts_दीर्घता));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
सन्देशः_आदर्शः::hash_संहिता() ध्रुव {
    int32_t hash=(apos_बहुलकम्*37+सन्देशः.hash_संहिता())*37+parts_दीर्घता;
    प्रति(int32_t i=0; i<parts_दीर्घता; ++i) {
        hash=hash*37+parts[i].hash_संहिता();
    }
    प्रत्यागम hash;
}

int32_t
सन्देशः_आदर्शः::validate_तर्कः_Name(ध्रुव UnicodeString &name) {
    यदि(!आदर्शः_Props::isIdentifier(name.get_बफर(), name.length())) {
        प्रत्यागम UMSGPAT_ARG_NAME_NOT_VALID;
    }
    प्रत्यागम parse_Arg_सङ्ख्या(name, 0, name.length());
}

UnicodeString
सन्देशः_आदर्शः::autoQuoteApostropheDeep() ध्रुव {
    यदि(!needsAutoQuoting) {
        प्रत्यागम सन्देशः;
    }
    UnicodeString modified(सन्देशः);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=गणनम्_Parts();
    प्रति(int32_t i=count; i>0;) {
        ध्रुव Part &part=getPart(--i);
        यदि(part.getType()==UMSGPAT_PART_TYPE_निवेशय_CHAR) {
           modified.insert(part.सूचकांक, static_cast<char16_t>(part.value));
        }
    }
    प्रत्यागम modified;
}

द्विगुण
सन्देशः_आदर्शः::get_Numeric_मानम्(ध्रुव Part &part) ध्रुव {
    UMessage_आदर्शः_Part_Type type=part.type;
    यदि(type==UMSGPAT_PART_TYPE_ARG_INT) {
        प्रत्यागम part.value;
    } अन्यथा यदि(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        प्रत्यागम numeric_मानानि[part.value];
    } अन्यथा {
        प्रत्यागम UMSGPAT_NO_NUMERIC_मानम्;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
द्विगुण
सन्देशः_आदर्शः::get_Plural_विस्थापनम्(int32_t pluralStart) ध्रुव {
    ध्रुव Part &part=getPart(pluralStart);
    यदि(Part::has_Numeric_मानम्(part.type)) {
        प्रत्यागम get_Numeric_मानम्(part);
    } अन्यथा {
        प्रत्यागम 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

तार्किक
सन्देशः_आदर्शः::Part::संक्रियकः==(ध्रुव Part &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    प्रत्यागम
        type==other.type &&
        सूचकांक==other.सूचकांक &&
        length==other.length &&
        value==other.value &&
        सीमा_Part_सूचकांक==other.सीमा_Part_सूचकांक;
}

// MessagePattern parser --------------------------------------------------- ***

शून्य
सन्देशः_आदर्शः::preParse(ध्रुव UnicodeString &आदर्शः, UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि(parseError!=शून्यसूचकः) {
        parseError->पङ्क्तिः=0;
        parseError->विस्थापनम्=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    सन्देशः=आदर्शः;
    has_Arg_नामानि=hasArgNumbers=असत्य;
    needsAutoQuoting=असत्य;
    parts_दीर्घता=0;
    numeric_मानानि_दीर्घता=0;
}

शून्य
सन्देशः_आदर्शः::postParse() {
    यदि(partsList!=शून्यसूचकः) {
        parts=partsList->a.getAlias();
    }
    यदि(numeric_मानानि_List!=शून्यसूचकः) {
        numeric_मानानि=numeric_मानानि_List->a.getAlias();
    }
}

int32_t
सन्देशः_आदर्शः::parse_सन्देशः(int32_t सूचकांक, int32_t सन्देशः_Start_दीर्घता,
                             int32_t nestingLevel, UMessage_आदर्शः_Arg_Type parentType,
                             UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(nestingLevel>Part::MAX_NESTED_स्तराः) {
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0;
    }
    int32_t सन्देशः_Start=parts_दीर्घता;
    सङ्कलय_Part(UMSGPAT_PART_TYPE_सन्देशः_START, सूचकांक, सन्देशः_Start_दीर्घता, nestingLevel, error_संहिता);
    सूचकांक+=सन्देशः_Start_दीर्घता;
    प्रति(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
        यदि(सूचकांक>=सन्देशः.length()) {
            भङ्ग;
        }
        char16_t c=सन्देशः.charAt(सूचकांक++);
        यदि(c==u_apos) {
            यदि(सूचकांक==सन्देशः.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                सङ्कलय_Part(UMSGPAT_PART_TYPE_निवेशय_CHAR, सूचकांक, 0,
                        u_apos, error_संहिता);  // value=char to be inserted
                needsAutoQuoting=सत्य;
            } अन्यथा {
                c=सन्देशः.charAt(सूचकांक);
                यदि(c==u_apos) {
                    // double apostrophe, skip the second one
                    सङ्कलय_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक++, 1, 0, error_संहिता);
                } अन्यथा यदि(
                    apos_बहुलकम्==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_वामः_Curly_Brace || c==u_दक्षिणः_Curly_Brace ||
                    (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    सङ्कलय_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक-1, 1, 0, error_संहिता);
                    // find the end of the quoted literal text
                    प्रति(;;) {
                        सूचकांक=सन्देशः.सूचकांक_सम्बन्धि(u_apos, सूचकांक+1);
                        यदि(सूचकांक>=0) {
                            यदि(/*(index+1)<msg.length() &&*/ सन्देशः.charAt(सूचकांक+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                सङ्कलय_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++सूचकांक, 1, 0, error_संहिता);
                            } अन्यथा {
                                // skip the quote-ending apostrophe
                                सङ्कलय_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, सूचकांक++, 1, 0, error_संहिता);
                                भङ्ग;
                            }
                        } अन्यथा {
                            // The quoted text reaches to the end of the of the message.
                            सूचकांक=सन्देशः.length();
                            // Add a Part for auto-quoting.
                            सङ्कलय_Part(UMSGPAT_PART_TYPE_निवेशय_CHAR, सूचकांक, 0,
                                    u_apos, error_संहिता);  // value=char to be inserted
                            needsAutoQuoting=सत्य;
                            भङ्ग;
                        }
                    }
                } अन्यथा {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    सङ्कलय_Part(UMSGPAT_PART_TYPE_निवेशय_CHAR, सूचकांक, 0,
                            u_apos, error_संहिता);  // value=char to be inserted
                    needsAutoQuoting=सत्य;
                }
            }
        } अन्यथा यदि(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(parentType) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            सङ्कलय_Part(UMSGPAT_PART_TYPE_REPLACE_सङ्ख्या, सूचकांक-1, 1, 0, error_संहिता);
        } अन्यथा यदि(c==u_वामः_Curly_Brace) {
            सूचकांक=parseArg(सूचकांक-1, 1, nestingLevel, parseError, error_संहिता);
        } अन्यथा यदि((nestingLevel>0 && c==u_दक्षिणः_Curly_Brace) ||
                  (parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t सीमा_दीर्घता=(parentType==UMSGPAT_ARG_TYPE_CHOICE && c==u_दक्षिणः_Curly_Brace) ? 0 : 1;
            सङ्कलय_सीमा_Part(सन्देशः_Start, UMSGPAT_PART_TYPE_सन्देशः_सीमा, सूचकांक-1, सीमा_दीर्घता,
                         nestingLevel, error_संहिता);
            यदि(parentType==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                प्रत्यागम सूचकांक-1;
            } अन्यथा {
                // continue parsing after the '}'
                प्रत्यागम सूचकांक;
            }
        }  // else: c is part of literal text
    }
    यदि(nestingLevel>0 && !अन्तर्_शिखरम्_Level_Choice_सन्देशः(nestingLevel, parentType)) {
        समुच्चयः_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_संहिता=U_UNMATCHED_BRACES;
        प्रत्यागम 0;
    }
    सङ्कलय_सीमा_Part(सन्देशः_Start, UMSGPAT_PART_TYPE_सन्देशः_सीमा, सूचकांक, 0, nestingLevel, error_संहिता);
    प्रत्यागम सूचकांक;
}

int32_t
सन्देशः_आदर्शः::parseArg(int32_t सूचकांक, int32_t arg_Start_दीर्घता, int32_t nestingLevel,
                         UParseError *parseError, UError_संहिता &error_संहिता) {
    int32_t argStart=parts_दीर्घता;
    UMessage_आदर्शः_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_START, सूचकांक, arg_Start_दीर्घता, argType, error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    int32_t name_सूचकांक=सूचकांक=skipWhiteSpace(सूचकांक+arg_Start_दीर्घता);
    यदि(सूचकांक==सन्देशः.length()) {
        समुच्चयः_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_संहिता=U_UNMATCHED_BRACES;
        प्रत्यागम 0;
    }
    // parse argument name or number
    सूचकांक=skipIdentifier(सूचकांक);
    int32_t number=parse_Arg_सङ्ख्या(name_सूचकांक, सूचकांक);
    यदि(number>=0) {
        int32_t length=सूचकांक-name_सूचकांक;
        यदि(length>Part::MAX_दीर्घता || number>Part::MAX_मानम्) {
            समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Argument number too large.
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
        hasArgNumbers=सत्य;
        सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_सङ्ख्या, name_सूचकांक, length, number, error_संहिता);
    } अन्यथा यदि(number==UMSGPAT_ARG_NAME_NOT_सङ्ख्या) {
        int32_t length=सूचकांक-name_सूचकांक;
        यदि(length>Part::MAX_दीर्घता) {
            समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Argument name too long.
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
        has_Arg_नामानि=सत्य;
        सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_सूचकांक, length, 0, error_संहिता);
    } अन्यथा {  // number<-1 (ARG_NAME_NOT_VALID)
        समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Bad argument syntax.
        error_संहिता=U_आदर्शः_SYNTAX_ERROR;
        प्रत्यागम 0;
    }
    सूचकांक=skipWhiteSpace(सूचकांक);
    यदि(सूचकांक==सन्देशः.length()) {
        समुच्चयः_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_संहिता=U_UNMATCHED_BRACES;
        प्रत्यागम 0;
    }
    char16_t c=सन्देशः.charAt(सूचकांक);
    यदि(c==u_दक्षिणः_Curly_Brace) {
        // all done
    } अन्यथा यदि(c!=u_comma) {
        समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Bad argument syntax.
        error_संहिता=U_आदर्शः_SYNTAX_ERROR;
        प्रत्यागम 0;
    } अन्यथा /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_सूचकांक=सूचकांक=skipWhiteSpace(सूचकांक+1);
        यावत्(सूचकांक<सन्देशः.length() && isArgTypeChar(सन्देशः.charAt(सूचकांक))) {
            ++सूचकांक;
        }
        int32_t length=सूचकांक-type_सूचकांक;
        सूचकांक=skipWhiteSpace(सूचकांक);
        यदि(सूचकांक==सन्देशः.length()) {
            समुच्चयः_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_संहिता=U_UNMATCHED_BRACES;
            प्रत्यागम 0;
        }
        यदि(length==0 || ((c=सन्देशः.charAt(सूचकांक))!=u_comma && c!=u_दक्षिणः_Curly_Brace)) {
            समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Bad argument syntax.
            error_संहिता=U_आदर्शः_SYNTAX_ERROR;
            प्रत्यागम 0;
        }
        यदि(length>Part::MAX_दीर्घता) {
            समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // Argument type name too long.
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        यदि(length==6) {
            // case-insensitive comparisons for complex-type names
            यदि(isChoice(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } अन्यथा यदि(isPlural(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } अन्यथा यदि(isSelect(type_सूचकांक)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } अन्यथा यदि(length==13) {
            यदि(isSelect(type_सूचकांक) && isOrdinal(type_सूचकांक+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[argStart].value = static_cast<int16_t>(argType);
        यदि(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_सूचकांक, length, 0, error_संहिता);
        }
        // look for an argument style (pattern)
        यदि(c==u_दक्षिणः_Curly_Brace) {
            यदि(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                समुच्चयः_Parse_Error(parseError, name_सूचकांक);  // No style field for complex argument.
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
        } अन्यथा /* ',' */ {
            ++सूचकांक;
            यदि(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                सूचकांक=parseSimpleStyle(सूचकांक, parseError, error_संहिता);
            } अन्यथा यदि(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                सूचकांक=parseChoiceStyle(सूचकांक, nestingLevel, parseError, error_संहिता);
            } अन्यथा {
                सूचकांक=parsePluralOrSelectStyle(argType, सूचकांक, nestingLevel, parseError, error_संहिता);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    सङ्कलय_सीमा_Part(argStart, UMSGPAT_PART_TYPE_ARG_सीमा, सूचकांक, 1, argType, error_संहिता);
    प्रत्यागम सूचकांक+1;
}

int32_t
सन्देशः_आदर्शः::parseSimpleStyle(int32_t सूचकांक, UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    int32_t start=सूचकांक;
    int32_t nestedBraces=0;
    यावत्(सूचकांक<सन्देशः.length()) {
        char16_t c=सन्देशः.charAt(सूचकांक++);
        यदि(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            सूचकांक=सन्देशः.सूचकांक_सम्बन्धि(u_apos, सूचकांक);
            यदि(सूचकांक<0) {
                // Quoted literal argument style text reaches to the end of the message.
                समुच्चयः_Parse_Error(parseError, start);
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
            // skip the quote-ending apostrophe
            ++सूचकांक;
        } अन्यथा यदि(c==u_वामः_Curly_Brace) {
            ++nestedBraces;
        } अन्यथा यदि(c==u_दक्षिणः_Curly_Brace) {
            यदि(nestedBraces>0) {
                --nestedBraces;
            } अन्यथा {
                int32_t length=--सूचकांक-start;
                यदि(length>Part::MAX_दीर्घता) {
                    समुच्चयः_Parse_Error(parseError, start);  // Argument style text too long.
                    error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    प्रत्यागम 0;
                }
                सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_STYLE, start, length, 0, error_संहिता);
                प्रत्यागम सूचकांक;
            }
        }  // c is part of literal text
    }
    समुच्चयः_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_संहिता=U_UNMATCHED_BRACES;
    प्रत्यागम 0;
}

int32_t
सन्देशः_आदर्शः::parseChoiceStyle(int32_t सूचकांक, int32_t nestingLevel,
                                 UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    int32_t start=सूचकांक;
    सूचकांक=skipWhiteSpace(सूचकांक);
    यदि(सूचकांक==सन्देशः.length() || सन्देशः.charAt(सूचकांक)==u_दक्षिणः_Curly_Brace) {
        समुच्चयः_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_संहिता=U_आदर्शः_SYNTAX_ERROR;
        प्रत्यागम 0;
    }
    प्रति(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t सङ्ख्या_सूचकांक=सूचकांक;
        सूचकांक=skipDouble(सूचकांक);
        int32_t length=सूचकांक-सङ्ख्या_सूचकांक;
        यदि(length==0) {
            समुच्चयः_Parse_Error(parseError, start);  // Bad choice pattern syntax.
            error_संहिता=U_आदर्शः_SYNTAX_ERROR;
            प्रत्यागम 0;
        }
        यदि(length>Part::MAX_दीर्घता) {
            समुच्चयः_Parse_Error(parseError, सङ्ख्या_सूचकांक);  // Choice number too long.
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
        parseDouble(सङ्ख्या_सूचकांक, सूचकांक, सत्य, parseError, error_संहिता);  // adds ARG_INT or ARG_DOUBLE
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
        // Parse the separator.
        सूचकांक=skipWhiteSpace(सूचकांक);
        यदि(सूचकांक==सन्देशः.length()) {
            समुच्चयः_Parse_Error(parseError, start);  // Bad choice pattern syntax.
            error_संहिता=U_आदर्शः_SYNTAX_ERROR;
            प्रत्यागम 0;
        }
        char16_t c=सन्देशः.charAt(सूचकांक);
        यदि(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            समुच्चयः_Parse_Error(parseError, start);  // Expected choice separator (#<\u2264) instead of c.
            error_संहिता=U_आदर्शः_SYNTAX_ERROR;
            प्रत्यागम 0;
        }
        सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, सूचकांक, 1, 0, error_संहिता);
        // Parse the message fragment.
        सूचकांक=parse_सन्देशः(++सूचकांक, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        यदि(सूचकांक==सन्देशः.length()) {
            प्रत्यागम सूचकांक;
        }
        यदि(सन्देशः.charAt(सूचकांक)==u_दक्षिणः_Curly_Brace) {
            यदि(!अन्तर्_सन्देशः_Format_आदर्शः(nestingLevel)) {
                समुच्चयः_Parse_Error(parseError, start);  // Bad choice pattern syntax.
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
            प्रत्यागम सूचकांक;
        }  // else the terminator is '|'
        सूचकांक=skipWhiteSpace(सूचकांक+1);
    }
}

int32_t
सन्देशः_आदर्शः::parsePluralOrSelectStyle(UMessage_आदर्शः_Arg_Type argType,
                                         int32_t सूचकांक, int32_t nestingLevel,
                                         UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम 0;
    }
    int32_t start=सूचकांक;
    UBool is_रिक्तानि=सत्य;
    UBool hasOther=असत्य;
    प्रति(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        सूचकांक=skipWhiteSpace(सूचकांक);
        UBool eos=सूचकांक==सन्देशः.length();
        यदि(eos || सन्देशः.charAt(सूचकांक)==u_दक्षिणः_Curly_Brace) {
            यदि(eos==अन्तर्_सन्देशः_Format_आदर्शः(nestingLevel)) {
                समुच्चयः_Parse_Error(parseError, start);  // Bad plural/select pattern syntax.
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
            यदि(!hasOther) {
                समुच्चयः_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_संहिता=U_DEFAULT_KEYWORD_MISSING;
                प्रत्यागम 0;
            }
            प्रत्यागम सूचकांक;
        }
        int32_t selector_सूचकांक=सूचकांक;
        यदि(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && सन्देशः.charAt(selector_सूचकांक)==u_equal) {
            // explicit-value plural selector: =double
            सूचकांक=skipDouble(सूचकांक+1);
            int32_t length=सूचकांक-selector_सूचकांक;
            यदि(length==1) {
                समुच्चयः_Parse_Error(parseError, start);  // Bad plural/select pattern syntax.
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
            यदि(length>Part::MAX_दीर्घता) {
                समुच्चयः_Parse_Error(parseError, selector_सूचकांक);  // Argument selector too long.
                error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }
            सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_सूचकांक, length, 0, error_संहिता);
            parseDouble(selector_सूचकांक+1, सूचकांक, असत्य,
                        parseError, error_संहिता);  // adds ARG_INT or ARG_DOUBLE
        } अन्यथा {
            सूचकांक=skipIdentifier(सूचकांक);
            int32_t length=सूचकांक-selector_सूचकांक;
            यदि(length==0) {
                समुच्चयः_Parse_Error(parseError, start);  // Bad plural/select pattern syntax.
                error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                प्रत्यागम 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            यदि( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && सूचकांक<सन्देशः.length() &&
                0==सन्देशः.compare(selector_सूचकांक, 7, k_विस्थापनम्_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                यदि(!is_रिक्तानि) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    समुच्चयः_Parse_Error(parseError, start);
                    error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                    प्रत्यागम 0;
                }
                // allow whitespace between offset: and its value
                int32_t मानम्_सूचकांक=skipWhiteSpace(सूचकांक+1);  // The ':' is at index.
                सूचकांक=skipDouble(मानम्_सूचकांक);
                यदि(सूचकांक==मानम्_सूचकांक) {
                    समुच्चयः_Parse_Error(parseError, start);  // Missing value for plural 'offset:'.
                    error_संहिता=U_आदर्शः_SYNTAX_ERROR;
                    प्रत्यागम 0;
                }
                यदि((सूचकांक-मानम्_सूचकांक)>Part::MAX_दीर्घता) {
                    समुच्चयः_Parse_Error(parseError, मानम्_सूचकांक);  // Plural offset value too long.
                    error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    प्रत्यागम 0;
                }
                parseDouble(मानम्_सूचकांक, सूचकांक, असत्य,
                            parseError, error_संहिता);  // adds ARG_INT or ARG_DOUBLE
                यदि(U_FAILURE(error_संहिता)) {
                    प्रत्यागम 0;
                }
                is_रिक्तानि=असत्य;
                अनुवर्त;  // no message fragment after the offset
            } अन्यथा {
                // normal selector word
                यदि(length>Part::MAX_दीर्घता) {
                    समुच्चयः_Parse_Error(parseError, selector_सूचकांक);  // Argument selector too long.
                    error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                    प्रत्यागम 0;
                }
                सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_सूचकांक, length, 0, error_संहिता);
                यदि(0==सन्देशः.compare(selector_सूचकांक, length, kOther, 0, 5)) {
                    hasOther=सत्य;
                }
            }
        }
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }

        // parse the message fragment following the selector
        सूचकांक=skipWhiteSpace(सूचकांक);
        यदि(सूचकांक==सन्देशः.length() || सन्देशः.charAt(सूचकांक)!=u_वामः_Curly_Brace) {
            समुच्चयः_Parse_Error(parseError, selector_सूचकांक);  // No message fragment after plural/select selector.
            error_संहिता=U_आदर्शः_SYNTAX_ERROR;
            प्रत्यागम 0;
        }
        सूचकांक=parse_सन्देशः(सूचकांक, 1, nestingLevel+1, argType, parseError, error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम 0;
        }
        is_रिक्तानि=असत्य;
    }
}

int32_t
सन्देशः_आदर्शः::parse_Arg_सङ्ख्या(ध्रुव UnicodeString &s, int32_t start, int32_t सीमा) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    यदि(start>=सीमा) {
        प्रत्यागम UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_सङ्ख्या;
    char16_t c=s.charAt(start++);
    यदि(c==0x30) {
        यदि(start==सीमा) {
            प्रत्यागम 0;
        } अन्यथा {
            number=0;
            bad_सङ्ख्या=सत्य;  // leading zero
        }
    } अन्यथा यदि(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_सङ्ख्या=असत्य;
    } अन्यथा {
        प्रत्यागम UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
    }
    यावत्(start<सीमा) {
        c=s.charAt(start++);
        यदि(0x30<=c && c<=0x39) {
            यदि(number>=INT32_MAX/10) {
                bad_सङ्ख्या=सत्य;  //अतिप्रवाहः
            } अन्यथा {
                number=number*10+(c-0x30);
            }
        } अन्यथा {
            प्रत्यागम UMSGPAT_ARG_NAME_NOT_सङ्ख्या;
        }
    }
    // There are only ASCII digits.
    यदि(bad_सङ्ख्या) {
        प्रत्यागम UMSGPAT_ARG_NAME_NOT_VALID;
    } अन्यथा {
        प्रत्यागम number;
    }
}

शून्य
सन्देशः_आदर्शः::parseDouble(int32_t start, int32_t सीमा, UBool allowInfinity,
                            UParseError *parseError, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    U_ASSERT(start<सीमा);
    // fake loop for easy exit and single throw statement
    प्रति(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t सूचकांक=start;
        char16_t c=सन्देशः.charAt(सूचकांक++);
        यदि(c==u_minus) {
            isNegative=1;
            यदि(सूचकांक==सीमा) {
                भङ्ग;  // no number
            }
            c=सन्देशः.charAt(सूचकांक++);
        } अन्यथा यदि(c==u_plus) {
            यदि(सूचकांक==सीमा) {
                भङ्ग;  // no number
            }
            c=सन्देशः.charAt(सूचकांक++);
        }
        यदि(c==0x221e) {  // infinity
            यदि(allowInfinity && सूचकांक==सीमा) {
                द्विगुण infinity=uprv_getInfinity();
                सङ्कलय_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    start, सीमा-start, error_संहिता);
                प्रत्यागम;
            } अन्यथा {
                भङ्ग;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        यावत्('0'<=c && c<='9') {
            value=value*10+(c-'0');
            यदि(value>(Part::MAX_मानम्+isNegative)) {
                भङ्ग;  // not a small-enough integer
            }
            यदि(सूचकांक==सीमा) {
                सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_INT, start, सीमा-start,
                        isNegative!=0 ? -value : value, error_संहिता);
                प्रत्यागम;
            }
            c=सन्देशः.charAt(सूचकांक++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        वर्ण सङ्ख्या_Chars[128];
        int32_t capacity = static_cast<int32_t>(परिमाण(सङ्ख्या_Chars));
        int32_t length=सीमा-start;
        यदि(length>=capacity) {
            भङ्ग;  // number too long
        }
        सन्देशः.extract(start, length, सङ्ख्या_Chars, capacity, US_INV);
        यदि (static_cast<int32_t>(uprv_strlen(सङ्ख्या_Chars)) < length) {
            भङ्ग;  // contains non-invariant character that was turned into NUL
        }
        वर्ण *अन्तः;
        द्विगुण numeric_मानम्=uprv_strtod(सङ्ख्या_Chars, &अन्तः);
        यदि(अन्तः!=(सङ्ख्या_Chars+length)) {
            भङ्ग;  // parsing error
        }
        सङ्कलय_Arg_Double_Part(numeric_मानम्, start, length, error_संहिता);
        प्रत्यागम;
    }
    समुच्चयः_Parse_Error(parseError, start /*, limit*/);  // Bad syntax for numeric value.
    error_संहिता=U_आदर्शः_SYNTAX_ERROR;
}

int32_t
सन्देशः_आदर्शः::skipWhiteSpace(int32_t सूचकांक) {
    ध्रुव char16_t *s=सन्देशः.get_बफर();
    int32_t सन्देशः_दीर्घता=सन्देशः.length();
    ध्रुव char16_t *t=आदर्शः_Props::skipWhiteSpace(s+सूचकांक, सन्देशः_दीर्घता-सूचकांक);
    प्रत्यागम static_cast<int32_t>(t - s);
}

int32_t
सन्देशः_आदर्शः::skipIdentifier(int32_t सूचकांक) {
    ध्रुव char16_t *s=सन्देशः.get_बफर();
    int32_t सन्देशः_दीर्घता=सन्देशः.length();
    ध्रुव char16_t *t=आदर्शः_Props::skipIdentifier(s+सूचकांक, सन्देशः_दीर्घता-सूचकांक);
    प्रत्यागम static_cast<int32_t>(t - s);
}

int32_t
सन्देशः_आदर्शः::skipDouble(int32_t सूचकांक) {
    int32_t सन्देशः_दीर्घता=सन्देशः.length();
    यावत्(सूचकांक<सन्देशः_दीर्घता) {
        char16_t c=सन्देशः.charAt(सूचकांक);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        यदि((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            भङ्ग;
        }
        ++सूचकांक;
    }
    प्रत्यागम सूचकांक;
}

UBool
सन्देशः_आदर्शः::isArgTypeChar(UChar32 c) {
    प्रत्यागम (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
सन्देशः_आदर्शः::isChoice(int32_t सूचकांक) {
    char16_t c;
    प्रत्यागम
        ((c=सन्देशः.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_h || c==u_H) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_o || c==u_O) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_i || c==u_I) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(सूचकांक))==u_e || c==u_E);
}

UBool
सन्देशः_आदर्शः::isPlural(int32_t सूचकांक) {
    char16_t c;
    प्रत्यागम
        ((c=सन्देशः.charAt(सूचकांक++))==u_p || c==u_P) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_l || c==u_L) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_u || c==u_U) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_r || c==u_R) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_a || c==u_A) &&
        ((c=सन्देशः.charAt(सूचकांक))==u_l || c==u_L);
}

UBool
सन्देशः_आदर्शः::isSelect(int32_t सूचकांक) {
    char16_t c;
    प्रत्यागम
        ((c=सन्देशः.charAt(सूचकांक++))==u_s || c==u_S) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_e || c==u_E) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_l || c==u_L) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_e || c==u_E) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_c || c==u_C) &&
        ((c=सन्देशः.charAt(सूचकांक))==u_t || c==u_T);
}

UBool
सन्देशः_आदर्शः::isOrdinal(int32_t सूचकांक) {
    char16_t c;
    प्रत्यागम
        ((c=सन्देशः.charAt(सूचकांक++))==u_o || c==u_O) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_r || c==u_R) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_d || c==u_D) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_i || c==u_I) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_n || c==u_N) &&
        ((c=सन्देशः.charAt(सूचकांक++))==u_a || c==u_A) &&
        ((c=सन्देशः.charAt(सूचकांक))==u_l || c==u_L);
}

UBool
सन्देशः_आदर्शः::अन्तर्_सन्देशः_Format_आदर्शः(int32_t nestingLevel) {
    प्रत्यागम nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_सन्देशः_START;
}

UBool
सन्देशः_आदर्शः::अन्तर्_शिखरम्_Level_Choice_सन्देशः(int32_t nestingLevel, UMessage_आदर्शः_Arg_Type parentType) {
    प्रत्यागम
        nestingLevel==1 &&
        parentType==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_सन्देशः_START;
}

शून्य
सन्देशः_आदर्शः::सङ्कलय_Part(UMessage_आदर्शः_Part_Type type, int32_t सूचकांक, int32_t length,
                        int32_t value, UError_संहिता &error_संहिता) {
    यदि(partsList->ensure_क्षमता_For_One_अधिकम्(parts_दीर्घता, error_संहिता)) {
        Part &part=partsList->a[parts_दीर्घता++];
        part.type=type;
        part.सूचकांक=सूचकांक;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.सीमा_Part_सूचकांक=0;
    }
}

शून्य
सन्देशः_आदर्शः::सङ्कलय_सीमा_Part(int32_t start,
                             UMessage_आदर्शः_Part_Type type, int32_t सूचकांक, int32_t length,
                             int32_t value, UError_संहिता &error_संहिता) {
    partsList->a[start].सीमा_Part_सूचकांक=parts_दीर्घता;
    सङ्कलय_Part(type, सूचकांक, length, value, error_संहिता);
}

शून्य
सन्देशः_आदर्शः::सङ्कलय_Arg_Double_Part(द्विगुण numeric_मानम्, int32_t start, int32_t length,
                                 UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    int32_t numeric_सूचकांक=numeric_मानानि_दीर्घता;
    यदि(numeric_मानानि_List==शून्यसूचकः) {
        numeric_मानानि_List=नूतन सन्देशः_आदर्शः_Double_List();
        यदि(numeric_मानानि_List==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
    } अन्यथा यदि(!numeric_मानानि_List->ensure_क्षमता_For_One_अधिकम्(numeric_मानानि_दीर्घता, error_संहिता)) {
        प्रत्यागम;
    } अन्यथा {
        यदि(numeric_सूचकांक>Part::MAX_मानम्) {
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
    }
    numeric_मानानि_List->a[numeric_मानानि_दीर्घता++]=numeric_मानम्;
    सङ्कलय_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, start, length, numeric_सूचकांक, error_संहिता);
}

शून्य
सन्देशः_आदर्शः::समुच्चयः_Parse_Error(UParseError *parseError, int32_t सूचकांक) {
    यदि(parseError==शून्यसूचकः) {
        प्रत्यागम;
    }
    parseError->विस्थापनम्=सूचकांक;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=सूचकांक;
    यदि(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        यदि(length>0 && U16_IS_TRAIL(सन्देशः[सूचकांक-length])) {
            --length;
        }
    }
    सन्देशः.extract(सूचकांक-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=सन्देशः.length()-सूचकांक;
    यदि(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        यदि(length>0 && U16_IS_LEAD(सन्देशः[सूचकांक+length-1])) {
            --length;
        }
    }
    सन्देशः.extract(सूचकांक, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

शून्य
सन्देशः_Impl::appendReducedApostrophes(ध्रुव UnicodeString &s, int32_t start, int32_t सीमा,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    प्रति(;;) {
        int32_t i=s.सूचकांक_सम्बन्धि(u_apos, start);
        यदि(i<0 || i>=सीमा) {
            sb.append(s, start, सीमा-start);
            भङ्ग;
        }
        यदि(i==doubleApos) {
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
सन्देशः_Impl::append_Sub_सन्देशः_Without_Skip_Syntax(ध्रुव सन्देशः_आदर्शः& सन्देशः_आदर्शः_2,
                                               int32_t सन्देशः_Start,
                                               UnicodeString& परिणाम U_LIFETIME_BOUND) {
    ध्रुव UnicodeString &सन्देशः_String=सन्देशः_आदर्शः_2.get_आदर्शः_String();
    int32_t पूर्व_सूचकांक=सन्देशः_आदर्शः_2.getPart(सन्देशः_Start).get_सीमा();
    प्रति(int32_t i=सन्देशः_Start;;) {
        ध्रुव सन्देशः_आदर्शः::Part &part=सन्देशः_आदर्शः_2.getPart(++i);
        UMessage_आदर्शः_Part_Type type=part.getType();
        int32_t सूचकांक=part.get_सूचकांक();
        यदि(type==UMSGPAT_PART_TYPE_सन्देशः_सीमा) {
            प्रत्यागम परिणाम.append(सन्देशः_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
        } अन्यथा यदि(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            परिणाम.append(सन्देशः_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
            पूर्व_सूचकांक=part.get_सीमा();
        } अन्यथा यदि(type==UMSGPAT_PART_TYPE_ARG_START) {
            परिणाम.append(सन्देशः_String, पूर्व_सूचकांक, सूचकांक-पूर्व_सूचकांक);
            पूर्व_सूचकांक=सूचकांक;
            i=सन्देशः_आदर्शः_2.get_सीमा_Part_सूचकांक(i);
            सूचकांक=सन्देशः_आदर्शः_2.getPart(i).get_सीमा();
            appendReducedApostrophes(सन्देशः_String, पूर्व_सूचकांक, सूचकांक, परिणाम);
            पूर्व_सूचकांक=सूचकांक;
        }
    }
}

U_NAMESPACE_अन्तः

#यदि_अन्तः  // !UCONFIG_NO_FORMATTING
