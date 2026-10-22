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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_FORMATTING

#అన్తర్భావ "unicode/messagepattern.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "messageimpl.h"
#అన్తర్భావ "patternprops.h"
#అన్తర్భావ "putilimp.h"
#అన్తర్భావ "uassert.h"

U_NAMESPACE_BEGIN

// Unicode character/code point constants ---------------------------------- ***

స్థిర స్థిరరాశి char16_t u_pound=0x23;
స్థిర స్థిరరాశి char16_t u_apos=0x27;
స్థిర స్థిరరాశి char16_t u_plus=0x2B;
స్థిర స్థిరరాశి char16_t u_comma=0x2C;
స్థిర స్థిరరాశి char16_t u_minus=0x2D;
స్థిర స్థిరరాశి char16_t u_dot=0x2E;
స్థిర స్థిరరాశి char16_t u_colon=0x3A;
స్థిర స్థిరరాశి char16_t u_lessThan=0x3C;
స్థిర స్థిరరాశి char16_t u_equal=0x3D;
స్థిర స్థిరరాశి char16_t u_A=0x41;
స్థిర స్థిరరాశి char16_t u_C=0x43;
స్థిర స్థిరరాశి char16_t u_D=0x44;
స్థిర స్థిరరాశి char16_t u_E=0x45;
స్థిర స్థిరరాశి char16_t u_H=0x48;
స్థిర స్థిరరాశి char16_t u_I=0x49;
స్థిర స్థిరరాశి char16_t u_L=0x4C;
స్థిర స్థిరరాశి char16_t u_N=0x4E;
స్థిర స్థిరరాశి char16_t u_O=0x4F;
స్థిర స్థిరరాశి char16_t u_P=0x50;
స్థిర స్థిరరాశి char16_t u_R=0x52;
స్థిర స్థిరరాశి char16_t u_S=0x53;
స్థిర స్థిరరాశి char16_t u_T=0x54;
స్థిర స్థిరరాశి char16_t u_U=0x55;
స్థిర స్థిరరాశి char16_t u_Z=0x5A;
స్థిర స్థిరరాశి char16_t u_a=0x61;
స్థిర స్థిరరాశి char16_t u_c=0x63;
స్థిర స్థిరరాశి char16_t u_d=0x64;
స్థిర స్థిరరాశి char16_t u_e=0x65;
స్థిర స్థిరరాశి char16_t u_f=0x66;
స్థిర స్థిరరాశి char16_t u_h=0x68;
స్థిర స్థిరరాశి char16_t u_i=0x69;
స్థిర స్థిరరాశి char16_t u_l=0x6C;
స్థిర స్థిరరాశి char16_t u_n=0x6E;
స్థిర స్థిరరాశి char16_t u_o=0x6F;
స్థిర స్థిరరాశి char16_t u_p=0x70;
స్థిర స్థిరరాశి char16_t u_r=0x72;
స్థిర స్థిరరాశి char16_t u_s=0x73;
స్థిర స్థిరరాశి char16_t u_t=0x74;
స్థిర స్థిరరాశి char16_t u_u=0x75;
స్థిర స్థిరరాశి char16_t u_z=0x7A;
స్థిర స్థిరరాశి char16_t u_ఎడమ_Curly_Brace=0x7B;
స్థిర స్థిరరాశి char16_t u_pipe=0x7C;
స్థిర స్థిరరాశి char16_t u_కుడి_Curly_Brace=0x7D;
స్థిర స్థిరరాశి char16_t u_lessOrEqual=0x2264;  // U+2264 is <=

స్థిర స్థిరరాశి char16_t k_స్థానభ్రంశం_Colon[]={  // "offset:"
    u_o, u_f, u_f, u_s, u_e, u_t, u_colon
};

స్థిర స్థిరరాశి char16_t kOther[]={  // "other"
    u_o, u_t, u_h, u_e, u_r
};

// MessagePatternList ------------------------------------------------------ ***

మూస<రకం_పేరు T, int32_t దొంతర_సామర్థ్యం>
తరగతి సందేశం_నమూనా_List : బహిరంగ UMemory {
బహిరంగ:
    సందేశం_నమూనా_List() {}
    శూన్య copyFrom(స్థిరరాశి సందేశం_నమూనా_List<T, దొంతర_సామర్థ్యం> &other,
                  int32_t length,
                  UError_సంకేతం &error_సంకేతం);
    UBool ensure_సామర్థ్యం_For_One_మరిన్ని(int32_t old_నిడివి, UError_సంకేతం &error_సంకేతం);
    UBool equals(స్థిరరాశి సందేశం_నమూనా_List<T, దొంతర_సామర్థ్యం> &other, int32_t length) స్థిరరాశి {
        కొరకు(int32_t i=0; i<length; ++i) {
            ఒకవేళ(a[i]!=other.a[i]) { తిరిగి అబద్ధం; }
        }
        తిరిగి నిజం;
    }

    Maybe_దొంతర_Array<T, దొంతర_సామర్థ్యం> a;
};

మూస<రకం_పేరు T, int32_t దొంతర_సామర్థ్యం>
శూన్య
సందేశం_నమూనా_List<T, దొంతర_సామర్థ్యం>::copyFrom(
        స్థిరరాశి సందేశం_నమూనా_List<T, దొంతర_సామర్థ్యం> &other,
        int32_t length,
        UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && length>0) {
        ఒకవేళ(length>a.get_సామర్థ్యం() && శూన్యసూచిక==a.resize(length)) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        uprv_memcpy(a.getAlias(), other.a.getAlias(), (size_t)length*పరిమాణం(T));
    }
}

మూస<రకం_పేరు T, int32_t దొంతర_సామర్థ్యం>
UBool
సందేశం_నమూనా_List<T, దొంతర_సామర్థ్యం>::ensure_సామర్థ్యం_For_One_మరిన్ని(int32_t old_నిడివి, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    ఒకవేళ(a.get_సామర్థ్యం()>old_నిడివి || a.resize(2*old_నిడివి, old_నిడివి)!=శూన్యసూచిక) {
        తిరిగి నిజం;
    }
    error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    తిరిగి అబద్ధం;
}

// MessagePatternList specializations -------------------------------------- ***

తరగతి సందేశం_నమూనా_Double_List : బహిరంగ సందేశం_నమూనా_List<ద్విగుణ, 8> {
};

తరగతి సందేశం_నమూనా_Parts_List : బహిరంగ సందేశం_నమూనా_List<సందేశం_నమూనా::Part, 32> {
};

// MessagePattern constructors etc. ---------------------------------------- ***

సందేశం_నమూనా::సందేశం_నమూనా(UError_సంకేతం &error_సంకేతం)
        : apos_బహుళకం(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_బహుళకం),
          partsList(శూన్యసూచిక), parts(శూన్యసూచిక), parts_నిడివి(0),
          numeric_విలువలు_List(శూన్యసూచిక), numeric_విలువలు(శూన్యసూచిక), numeric_విలువలు_నిడివి(0),
          has_Arg_పేర్లు(అబద్ధం), hasArgNumbers(అబద్ధం), needsAutoQuoting(అబద్ధం) {
    init(error_సంకేతం);
}

సందేశం_నమూనా::సందేశం_నమూనా(UMessage_నమూనా_Apostrophe_బహుళకం బహుళకం, UError_సంకేతం &error_సంకేతం)
        : apos_బహుళకం(బహుళకం),
          partsList(శూన్యసూచిక), parts(శూన్యసూచిక), parts_నిడివి(0),
          numeric_విలువలు_List(శూన్యసూచిక), numeric_విలువలు(శూన్యసూచిక), numeric_విలువలు_నిడివి(0),
          has_Arg_పేర్లు(అబద్ధం), hasArgNumbers(అబద్ధం), needsAutoQuoting(అబద్ధం) {
    init(error_సంకేతం);
}

సందేశం_నమూనా::సందేశం_నమూనా(స్థిరరాశి UnicodeString &నమూనా, UParseError *parseError, UError_సంకేతం &error_సంకేతం)
        : apos_బహుళకం(UCONFIG_MSGPAT_DEFAULT_APOSTROPHE_బహుళకం),
          partsList(శూన్యసూచిక), parts(శూన్యసూచిక), parts_నిడివి(0),
          numeric_విలువలు_List(శూన్యసూచిక), numeric_విలువలు(శూన్యసూచిక), numeric_విలువలు_నిడివి(0),
          has_Arg_పేర్లు(అబద్ధం), hasArgNumbers(అబద్ధం), needsAutoQuoting(అబద్ధం) {
    ఒకవేళ(init(error_సంకేతం)) {
        parse(నమూనా, parseError, error_సంకేతం);
    }
}

UBool
సందేశం_నమూనా::init(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    partsList=కొత్త సందేశం_నమూనా_Parts_List();
    ఒకవేళ(partsList==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి అబద్ధం;
    }
    parts=partsList->a.getAlias();
    తిరిగి నిజం;
}

సందేశం_నమూనా::సందేశం_నమూనా(స్థిరరాశి సందేశం_నమూనా &other)
        : UObject(other), apos_బహుళకం(other.apos_బహుళకం), సందేశం(other.సందేశం),
          partsList(శూన్యసూచిక), parts(శూన్యసూచిక), parts_నిడివి(0),
          numeric_విలువలు_List(శూన్యసూచిక), numeric_విలువలు(శూన్యసూచిక), numeric_విలువలు_నిడివి(0),
          has_Arg_పేర్లు(other.has_Arg_పేర్లు), hasArgNumbers(other.hasArgNumbers),
          needsAutoQuoting(other.needsAutoQuoting) {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    ఒకవేళ(!copyStorage(other, error_సంకేతం)) {
        clear();
    }
}

సందేశం_నమూనా&
సందేశం_నమూనా::క్రియాకారకం=(స్థిరరాశి సందేశం_నమూనా& other) U_LIFETIME_BOUND {
    ఒకవేళ(ఇది==&other) {
        తిరిగి *ఇది;
    }
    apos_బహుళకం=other.apos_బహుళకం;
    సందేశం=other.సందేశం;
    has_Arg_పేర్లు=other.has_Arg_పేర్లు;
    hasArgNumbers=other.hasArgNumbers;
    needsAutoQuoting=other.needsAutoQuoting;
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    ఒకవేళ(!copyStorage(other, error_సంకేతం)) {
        clear();
    }
    తిరిగి *ఇది;
}

UBool
సందేశం_నమూనా::copyStorage(స్థిరరాశి సందేశం_నమూనా &other, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }
    parts=శూన్యసూచిక;
    parts_నిడివి=0;
    numeric_విలువలు=శూన్యసూచిక;
    numeric_విలువలు_నిడివి=0;
    ఒకవేళ(partsList==శూన్యసూచిక) {
        partsList=కొత్త సందేశం_నమూనా_Parts_List();
        ఒకవేళ(partsList==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి అబద్ధం;
        }
        parts=partsList->a.getAlias();
    }
    ఒకవేళ(other.parts_నిడివి>0) {
        partsList->copyFrom(*other.partsList, other.parts_నిడివి, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి అబద్ధం;
        }
        parts=partsList->a.getAlias();
        parts_నిడివి=other.parts_నిడివి;
    }
    ఒకవేళ(other.numeric_విలువలు_నిడివి>0) {
        ఒకవేళ(numeric_విలువలు_List==శూన్యసూచిక) {
            numeric_విలువలు_List=కొత్త సందేశం_నమూనా_Double_List();
            ఒకవేళ(numeric_విలువలు_List==శూన్యసూచిక) {
                error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి అబద్ధం;
            }
            numeric_విలువలు=numeric_విలువలు_List->a.getAlias();
        }
        numeric_విలువలు_List->copyFrom(
            *other.numeric_విలువలు_List, other.numeric_విలువలు_నిడివి, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి అబద్ధం;
        }
        numeric_విలువలు=numeric_విలువలు_List->a.getAlias();
        numeric_విలువలు_నిడివి=other.numeric_విలువలు_నిడివి;
    }
    తిరిగి నిజం;
}

సందేశం_నమూనా::~సందేశం_నమూనా() {
    తొలగించు partsList;
    తొలగించు numeric_విలువలు_List;
}

// MessagePattern API ------------------------------------------------------ ***

సందేశం_నమూనా&
సందేశం_నమూనా::parse(స్థిరరాశి UnicodeString& నమూనా,
                      UParseError* parseError,
                      UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    preParse(నమూనా, parseError, error_సంకేతం);
    parse_సందేశం(0, 0, 0, UMSGPAT_ARG_TYPE_NONE, parseError, error_సంకేతం);
    postParse();
    తిరిగి *ఇది;
}

సందేశం_నమూనా&
సందేశం_నమూనా::parseChoiceStyle(స్థిరరాశి UnicodeString& నమూనా,
                                 UParseError* parseError,
                                 UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    preParse(నమూనా, parseError, error_సంకేతం);
    parseChoiceStyle(0, 0, parseError, error_సంకేతం);
    postParse();
    తిరిగి *ఇది;
}

సందేశం_నమూనా&
సందేశం_నమూనా::parsePluralStyle(స్థిరరాశి UnicodeString& నమూనా,
                                 UParseError* parseError,
                                 UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    preParse(నమూనా, parseError, error_సంకేతం);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_PLURAL, 0, 0, parseError, error_సంకేతం);
    postParse();
    తిరిగి *ఇది;
}

సందేశం_నమూనా&
సందేశం_నమూనా::parseSelectStyle(స్థిరరాశి UnicodeString& నమూనా,
                                 UParseError* parseError,
                                 UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    preParse(నమూనా, parseError, error_సంకేతం);
    parsePluralOrSelectStyle(UMSGPAT_ARG_TYPE_SELECT, 0, 0, parseError, error_సంకేతం);
    postParse();
    తిరిగి *ఇది;
}

శూన్య
సందేశం_నమూనా::clear() {
    // Mostly the same as preParse().
    సందేశం.remove();
    has_Arg_పేర్లు=hasArgNumbers=అబద్ధం;
    needsAutoQuoting=అబద్ధం;
    parts_నిడివి=0;
    numeric_విలువలు_నిడివి=0;
}

తార్కిక
సందేశం_నమూనా::క్రియాకారకం==(స్థిరరాశి సందేశం_నమూనా &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    తిరిగి
        apos_బహుళకం==other.apos_బహుళకం &&
        సందేశం==other.సందేశం &&
        // parts.equals(o.parts)
        parts_నిడివి==other.parts_నిడివి &&
        (parts_నిడివి==0 || partsList->equals(*other.partsList, parts_నిడివి));
    // No need to compare numericValues if msg and parts are the same.
}

int32_t
సందేశం_నమూనా::hash_సంకేతం() స్థిరరాశి {
    int32_t hash=(apos_బహుళకం*37+సందేశం.hash_సంకేతం())*37+parts_నిడివి;
    కొరకు(int32_t i=0; i<parts_నిడివి; ++i) {
        hash=hash*37+parts[i].hash_సంకేతం();
    }
    తిరిగి hash;
}

int32_t
సందేశం_నమూనా::validate_వాదం_Name(స్థిరరాశి UnicodeString &name) {
    ఒకవేళ(!నమూనా_Props::isIdentifier(name.get_బఫరు(), name.length())) {
        తిరిగి UMSGPAT_ARG_NAME_NOT_VALID;
    }
    తిరిగి parse_Arg_సంఖ్య(name, 0, name.length());
}

UnicodeString
సందేశం_నమూనా::autoQuoteApostropheDeep() స్థిరరాశి {
    ఒకవేళ(!needsAutoQuoting) {
        తిరిగి సందేశం;
    }
    UnicodeString modified(సందేశం);
    // Iterate backward so that the insertion indexes do not change.
    int32_t count=లెక్క_Parts();
    కొరకు(int32_t i=count; i>0;) {
        స్థిరరాశి Part &part=getPart(--i);
        ఒకవేళ(part.getType()==UMSGPAT_PART_TYPE_చొప్పించు_CHAR) {
           modified.insert(part.సూచీ, static_cast<char16_t>(part.value));
        }
    }
    తిరిగి modified;
}

ద్విగుణ
సందేశం_నమూనా::get_Numeric_విలువ(స్థిరరాశి Part &part) స్థిరరాశి {
    UMessage_నమూనా_Part_Type type=part.type;
    ఒకవేళ(type==UMSGPAT_PART_TYPE_ARG_INT) {
        తిరిగి part.value;
    } లేకపోతే ఒకవేళ(type==UMSGPAT_PART_TYPE_ARG_DOUBLE) {
        తిరిగి numeric_విలువలు[part.value];
    } లేకపోతే {
        తిరిగి UMSGPAT_NO_NUMERIC_విలువ;
    }
}

/**
  * Returns the "offset:" value of a PluralFormat argument, or 0 if none is specified.
  * @param pluralStart the index of the first PluralFormat argument style part. (0..countParts()-1)
  * @return the "offset:" value.
  * @draft ICU 4.8
  */
ద్విగుణ
సందేశం_నమూనా::get_Plural_స్థానభ్రంశం(int32_t plural_ప్రారంభం) స్థిరరాశి {
    స్థిరరాశి Part &part=getPart(plural_ప్రారంభం);
    ఒకవేళ(Part::has_Numeric_విలువ(part.type)) {
        తిరిగి get_Numeric_విలువ(part);
    } లేకపోతే {
        తిరిగి 0;
    }
}

// MessagePattern::Part ---------------------------------------------------- ***

తార్కిక
సందేశం_నమూనా::Part::క్రియాకారకం==(స్థిరరాశి Part &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    తిరిగి
        type==other.type &&
        సూచీ==other.సూచీ &&
        length==other.length &&
        value==other.value &&
        హద్దు_Part_సూచీ==other.హద్దు_Part_సూచీ;
}

// MessagePattern parser --------------------------------------------------- ***

శూన్య
సందేశం_నమూనా::preParse(స్థిరరాశి UnicodeString &నమూనా, UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(parseError!=శూన్యసూచిక) {
        parseError->పంక్తి=0;
        parseError->స్థానభ్రంశం=0;
        parseError->preContext[0]=0;
        parseError->postContext[0]=0;
    }
    సందేశం=నమూనా;
    has_Arg_పేర్లు=hasArgNumbers=అబద్ధం;
    needsAutoQuoting=అబద్ధం;
    parts_నిడివి=0;
    numeric_విలువలు_నిడివి=0;
}

శూన్య
సందేశం_నమూనా::postParse() {
    ఒకవేళ(partsList!=శూన్యసూచిక) {
        parts=partsList->a.getAlias();
    }
    ఒకవేళ(numeric_విలువలు_List!=శూన్యసూచిక) {
        numeric_విలువలు=numeric_విలువలు_List->a.getAlias();
    }
}

int32_t
సందేశం_నమూనా::parse_సందేశం(int32_t సూచీ, int32_t సందేశం_ప్రారంభం_నిడివి,
                             int32_t nestingLevel, UMessage_నమూనా_Arg_Type మాతృ_Type,
                             UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(nestingLevel>Part::MAX_NESTED_స్థాయిలు) {
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి 0;
    }
    int32_t సందేశం_ప్రారంభం=parts_నిడివి;
    కూడు_Part(UMSGPAT_PART_TYPE_సందేశం_ప్రారంభం, సూచీ, సందేశం_ప్రారంభం_నిడివి, nestingLevel, error_సంకేతం);
    సూచీ+=సందేశం_ప్రారంభం_నిడివి;
    కొరకు(;;) {  // while(index<msg.length()) with U_FAILURE(errorCode) check
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
        ఒకవేళ(సూచీ>=సందేశం.length()) {
            ఆపు;
        }
        char16_t c=సందేశం.charAt(సూచీ++);
        ఒకవేళ(c==u_apos) {
            ఒకవేళ(సూచీ==సందేశం.length()) {
                // The apostrophe is the last character in the pattern. 
                // Add a Part for auto-quoting.
                కూడు_Part(UMSGPAT_PART_TYPE_చొప్పించు_CHAR, సూచీ, 0,
                        u_apos, error_సంకేతం);  // value=char to be inserted
                needsAutoQuoting=నిజం;
            } లేకపోతే {
                c=సందేశం.charAt(సూచీ);
                ఒకవేళ(c==u_apos) {
                    // double apostrophe, skip the second one
                    కూడు_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, సూచీ++, 1, 0, error_సంకేతం);
                } లేకపోతే ఒకవేళ(
                    apos_బహుళకం==UMSGPAT_APOS_DOUBLE_REQUIRED ||
                    c==u_ఎడమ_Curly_Brace || c==u_కుడి_Curly_Brace ||
                    (మాతృ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe) ||
                    (UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(మాతృ_Type) && c==u_pound)
                ) {
                    // skip the quote-starting apostrophe
                    కూడు_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, సూచీ-1, 1, 0, error_సంకేతం);
                    // find the end of the quoted literal text
                    కొరకు(;;) {
                        సూచీ=సందేశం.సూచీ_యొక్క(u_apos, సూచీ+1);
                        ఒకవేళ(సూచీ>=0) {
                            ఒకవేళ(/*(index+1)<msg.length() &&*/ సందేశం.charAt(సూచీ+1)==u_apos) {
                                // double apostrophe inside quoted literal text
                                // still encodes a single apostrophe, skip the second one
                                కూడు_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, ++సూచీ, 1, 0, error_సంకేతం);
                            } లేకపోతే {
                                // skip the quote-ending apostrophe
                                కూడు_Part(UMSGPAT_PART_TYPE_SKIP_SYNTAX, సూచీ++, 1, 0, error_సంకేతం);
                                ఆపు;
                            }
                        } లేకపోతే {
                            // The quoted text reaches to the end of the of the message.
                            సూచీ=సందేశం.length();
                            // Add a Part for auto-quoting.
                            కూడు_Part(UMSGPAT_PART_TYPE_చొప్పించు_CHAR, సూచీ, 0,
                                    u_apos, error_సంకేతం);  // value=char to be inserted
                            needsAutoQuoting=నిజం;
                            ఆపు;
                        }
                    }
                } లేకపోతే {
                    // Interpret the apostrophe as literal text.
                    // Add a Part for auto-quoting.
                    కూడు_Part(UMSGPAT_PART_TYPE_చొప్పించు_CHAR, సూచీ, 0,
                            u_apos, error_సంకేతం);  // value=char to be inserted
                    needsAutoQuoting=నిజం;
                }
            }
        } లేకపోతే ఒకవేళ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(మాతృ_Type) && c==u_pound) {
            // The unquoted # in a plural message fragment will be replaced
            // with the (number-offset).
            కూడు_Part(UMSGPAT_PART_TYPE_REPLACE_సంఖ్య, సూచీ-1, 1, 0, error_సంకేతం);
        } లేకపోతే ఒకవేళ(c==u_ఎడమ_Curly_Brace) {
            సూచీ=parseArg(సూచీ-1, 1, nestingLevel, parseError, error_సంకేతం);
        } లేకపోతే ఒకవేళ((nestingLevel>0 && c==u_కుడి_Curly_Brace) ||
                  (మాతృ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_pipe)) {
            // Finish the message before the terminator.
            // In a choice style, report the "}" substring only for the following ARG_LIMIT,
            // not for this MSG_LIMIT.
            int32_t హద్దు_నిడివి=(మాతృ_Type==UMSGPAT_ARG_TYPE_CHOICE && c==u_కుడి_Curly_Brace) ? 0 : 1;
            కూడు_హద్దు_Part(సందేశం_ప్రారంభం, UMSGPAT_PART_TYPE_సందేశం_హద్దు, సూచీ-1, హద్దు_నిడివి,
                         nestingLevel, error_సంకేతం);
            ఒకవేళ(మాతృ_Type==UMSGPAT_ARG_TYPE_CHOICE) {
                // Let the choice style parser see the '}' or '|'.
                తిరిగి సూచీ-1;
            } లేకపోతే {
                // continue parsing after the '}'
                తిరిగి సూచీ;
            }
        }  // else: c is part of literal text
    }
    ఒకవేళ(nestingLevel>0 && !లోపల_శిఖరం_Level_Choice_సందేశం(nestingLevel, మాతృ_Type)) {
        సమితి_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_సంకేతం=U_UNMATCHED_BRACES;
        తిరిగి 0;
    }
    కూడు_హద్దు_Part(సందేశం_ప్రారంభం, UMSGPAT_PART_TYPE_సందేశం_హద్దు, సూచీ, 0, nestingLevel, error_సంకేతం);
    తిరిగి సూచీ;
}

int32_t
సందేశం_నమూనా::parseArg(int32_t సూచీ, int32_t arg_ప్రారంభం_నిడివి, int32_t nestingLevel,
                         UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    int32_t arg_ప్రారంభం=parts_నిడివి;
    UMessage_నమూనా_Arg_Type argType=UMSGPAT_ARG_TYPE_NONE;
    కూడు_Part(UMSGPAT_PART_TYPE_ARG_ప్రారంభం, సూచీ, arg_ప్రారంభం_నిడివి, argType, error_సంకేతం);
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    int32_t name_సూచీ=సూచీ=skipWhiteSpace(సూచీ+arg_ప్రారంభం_నిడివి);
    ఒకవేళ(సూచీ==సందేశం.length()) {
        సమితి_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_సంకేతం=U_UNMATCHED_BRACES;
        తిరిగి 0;
    }
    // parse argument name or number
    సూచీ=skipIdentifier(సూచీ);
    int32_t number=parse_Arg_సంఖ్య(name_సూచీ, సూచీ);
    ఒకవేళ(number>=0) {
        int32_t length=సూచీ-name_సూచీ;
        ఒకవేళ(length>Part::MAX_నిడివి || number>Part::MAX_విలువ) {
            సమితి_Parse_Error(parseError, name_సూచీ);  // Argument number too large.
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
        hasArgNumbers=నిజం;
        కూడు_Part(UMSGPAT_PART_TYPE_ARG_సంఖ్య, name_సూచీ, length, number, error_సంకేతం);
    } లేకపోతే ఒకవేళ(number==UMSGPAT_ARG_NAME_NOT_సంఖ్య) {
        int32_t length=సూచీ-name_సూచీ;
        ఒకవేళ(length>Part::MAX_నిడివి) {
            సమితి_Parse_Error(parseError, name_సూచీ);  // Argument name too long.
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
        has_Arg_పేర్లు=నిజం;
        కూడు_Part(UMSGPAT_PART_TYPE_ARG_NAME, name_సూచీ, length, 0, error_సంకేతం);
    } లేకపోతే {  // number<-1 (ARG_NAME_NOT_VALID)
        సమితి_Parse_Error(parseError, name_సూచీ);  // Bad argument syntax.
        error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
        తిరిగి 0;
    }
    సూచీ=skipWhiteSpace(సూచీ);
    ఒకవేళ(సూచీ==సందేశం.length()) {
        సమితి_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
        error_సంకేతం=U_UNMATCHED_BRACES;
        తిరిగి 0;
    }
    char16_t c=సందేశం.charAt(సూచీ);
    ఒకవేళ(c==u_కుడి_Curly_Brace) {
        // all done
    } లేకపోతే ఒకవేళ(c!=u_comma) {
        సమితి_Parse_Error(parseError, name_సూచీ);  // Bad argument syntax.
        error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
        తిరిగి 0;
    } లేకపోతే /* ',' */ {
        // parse argument type: case-sensitive a-zA-Z
        int32_t type_సూచీ=సూచీ=skipWhiteSpace(సూచీ+1);
        అయితే(సూచీ<సందేశం.length() && isArgTypeChar(సందేశం.charAt(సూచీ))) {
            ++సూచీ;
        }
        int32_t length=సూచీ-type_సూచీ;
        సూచీ=skipWhiteSpace(సూచీ);
        ఒకవేళ(సూచీ==సందేశం.length()) {
            సమితి_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
            error_సంకేతం=U_UNMATCHED_BRACES;
            తిరిగి 0;
        }
        ఒకవేళ(length==0 || ((c=సందేశం.charAt(సూచీ))!=u_comma && c!=u_కుడి_Curly_Brace)) {
            సమితి_Parse_Error(parseError, name_సూచీ);  // Bad argument syntax.
            error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
            తిరిగి 0;
        }
        ఒకవేళ(length>Part::MAX_నిడివి) {
            సమితి_Parse_Error(parseError, name_సూచీ);  // Argument type name too long.
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
        argType=UMSGPAT_ARG_TYPE_SIMPLE;
        ఒకవేళ(length==6) {
            // case-insensitive comparisons for complex-type names
            ఒకవేళ(isChoice(type_సూచీ)) {
                argType=UMSGPAT_ARG_TYPE_CHOICE;
            } లేకపోతే ఒకవేళ(isPlural(type_సూచీ)) {
                argType=UMSGPAT_ARG_TYPE_PLURAL;
            } లేకపోతే ఒకవేళ(isSelect(type_సూచీ)) {
                argType=UMSGPAT_ARG_TYPE_SELECT;
            }
        } లేకపోతే ఒకవేళ(length==13) {
            ఒకవేళ(isSelect(type_సూచీ) && isOrdinal(type_సూచీ+6)) {
                argType=UMSGPAT_ARG_TYPE_SELECTORDINAL;
            }
        }
        // change the ARG_START type from NONE to argType
        partsList->a[arg_ప్రారంభం].value = static_cast<int16_t>(argType);
        ఒకవేళ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
            కూడు_Part(UMSGPAT_PART_TYPE_ARG_TYPE, type_సూచీ, length, 0, error_సంకేతం);
        }
        // look for an argument style (pattern)
        ఒకవేళ(c==u_కుడి_Curly_Brace) {
            ఒకవేళ(argType!=UMSGPAT_ARG_TYPE_SIMPLE) {
                సమితి_Parse_Error(parseError, name_సూచీ);  // No style field for complex argument.
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
        } లేకపోతే /* ',' */ {
            ++సూచీ;
            ఒకవేళ(argType==UMSGPAT_ARG_TYPE_SIMPLE) {
                సూచీ=parseSimpleStyle(సూచీ, parseError, error_సంకేతం);
            } లేకపోతే ఒకవేళ(argType==UMSGPAT_ARG_TYPE_CHOICE) {
                సూచీ=parseChoiceStyle(సూచీ, nestingLevel, parseError, error_సంకేతం);
            } లేకపోతే {
                సూచీ=parsePluralOrSelectStyle(argType, సూచీ, nestingLevel, parseError, error_సంకేతం);
            }
        }
    }
    // Argument parsing stopped on the '}'.
    కూడు_హద్దు_Part(arg_ప్రారంభం, UMSGPAT_PART_TYPE_ARG_హద్దు, సూచీ, 1, argType, error_సంకేతం);
    తిరిగి సూచీ+1;
}

int32_t
సందేశం_నమూనా::parseSimpleStyle(int32_t సూచీ, UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    int32_t ప్రారంభం=సూచీ;
    int32_t nestedBraces=0;
    అయితే(సూచీ<సందేశం.length()) {
        char16_t c=సందేశం.charAt(సూచీ++);
        ఒకవేళ(c==u_apos) {
            // Treat apostrophe as quoting but include it in the style part.
            // Find the end of the quoted literal text.
            సూచీ=సందేశం.సూచీ_యొక్క(u_apos, సూచీ);
            ఒకవేళ(సూచీ<0) {
                // Quoted literal argument style text reaches to the end of the message.
                సమితి_Parse_Error(parseError, ప్రారంభం);
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
            // skip the quote-ending apostrophe
            ++సూచీ;
        } లేకపోతే ఒకవేళ(c==u_ఎడమ_Curly_Brace) {
            ++nestedBraces;
        } లేకపోతే ఒకవేళ(c==u_కుడి_Curly_Brace) {
            ఒకవేళ(nestedBraces>0) {
                --nestedBraces;
            } లేకపోతే {
                int32_t length=--సూచీ-ప్రారంభం;
                ఒకవేళ(length>Part::MAX_నిడివి) {
                    సమితి_Parse_Error(parseError, ప్రారంభం);  // Argument style text too long.
                    error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                    తిరిగి 0;
                }
                కూడు_Part(UMSGPAT_PART_TYPE_ARG_STYLE, ప్రారంభం, length, 0, error_సంకేతం);
                తిరిగి సూచీ;
            }
        }  // c is part of literal text
    }
    సమితి_Parse_Error(parseError, 0);  // Unmatched '{' braces in message.
    error_సంకేతం=U_UNMATCHED_BRACES;
    తిరిగి 0;
}

int32_t
సందేశం_నమూనా::parseChoiceStyle(int32_t సూచీ, int32_t nestingLevel,
                                 UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    int32_t ప్రారంభం=సూచీ;
    సూచీ=skipWhiteSpace(సూచీ);
    ఒకవేళ(సూచీ==సందేశం.length() || సందేశం.charAt(సూచీ)==u_కుడి_Curly_Brace) {
        సమితి_Parse_Error(parseError, 0);  // Missing choice argument pattern.
        error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
        తిరిగి 0;
    }
    కొరకు(;;) {
        // The choice argument style contains |-separated (number, separator, message) triples.
        // Parse the number.
        int32_t సంఖ్య_సూచీ=సూచీ;
        సూచీ=skipDouble(సూచీ);
        int32_t length=సూచీ-సంఖ్య_సూచీ;
        ఒకవేళ(length==0) {
            సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad choice pattern syntax.
            error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
            తిరిగి 0;
        }
        ఒకవేళ(length>Part::MAX_నిడివి) {
            సమితి_Parse_Error(parseError, సంఖ్య_సూచీ);  // Choice number too long.
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
        parseDouble(సంఖ్య_సూచీ, సూచీ, నిజం, parseError, error_సంకేతం);  // adds ARG_INT or ARG_DOUBLE
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
        // Parse the separator.
        సూచీ=skipWhiteSpace(సూచీ);
        ఒకవేళ(సూచీ==సందేశం.length()) {
            సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad choice pattern syntax.
            error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
            తిరిగి 0;
        }
        char16_t c=సందేశం.charAt(సూచీ);
        ఒకవేళ(!(c==u_pound || c==u_lessThan || c==u_lessOrEqual)) {  // U+2264 is <=
            సమితి_Parse_Error(parseError, ప్రారంభం);  // Expected choice separator (#<\u2264) instead of c.
            error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
            తిరిగి 0;
        }
        కూడు_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, సూచీ, 1, 0, error_సంకేతం);
        // Parse the message fragment.
        సూచీ=parse_సందేశం(++సూచీ, 0, nestingLevel+1, UMSGPAT_ARG_TYPE_CHOICE, parseError, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
        // parseMessage(..., CHOICE) returns the index of the terminator, or msg.length().
        ఒకవేళ(సూచీ==సందేశం.length()) {
            తిరిగి సూచీ;
        }
        ఒకవేళ(సందేశం.charAt(సూచీ)==u_కుడి_Curly_Brace) {
            ఒకవేళ(!లోపల_సందేశం_Format_నమూనా(nestingLevel)) {
                సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad choice pattern syntax.
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
            తిరిగి సూచీ;
        }  // else the terminator is '|'
        సూచీ=skipWhiteSpace(సూచీ+1);
    }
}

int32_t
సందేశం_నమూనా::parsePluralOrSelectStyle(UMessage_నమూనా_Arg_Type argType,
                                         int32_t సూచీ, int32_t nestingLevel,
                                         UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి 0;
    }
    int32_t ప్రారంభం=సూచీ;
    UBool is_ఖాళీలు=నిజం;
    UBool hasOther=అబద్ధం;
    కొరకు(;;) {
        // First, collect the selector looking for a small set of terminators.
        // It would be a little faster to consider the syntax of each possible
        // token right here, but that makes the code too complicated.
        సూచీ=skipWhiteSpace(సూచీ);
        UBool eos=సూచీ==సందేశం.length();
        ఒకవేళ(eos || సందేశం.charAt(సూచీ)==u_కుడి_Curly_Brace) {
            ఒకవేళ(eos==లోపల_సందేశం_Format_నమూనా(nestingLevel)) {
                సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad plural/select pattern syntax.
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
            ఒకవేళ(!hasOther) {
                సమితి_Parse_Error(parseError, 0);  // Missing 'other' keyword in plural/select pattern.
                error_సంకేతం=U_DEFAULT_KEYWORD_MISSING;
                తిరిగి 0;
            }
            తిరిగి సూచీ;
        }
        int32_t selector_సూచీ=సూచీ;
        ఒకవేళ(UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && సందేశం.charAt(selector_సూచీ)==u_equal) {
            // explicit-value plural selector: =double
            సూచీ=skipDouble(సూచీ+1);
            int32_t length=సూచీ-selector_సూచీ;
            ఒకవేళ(length==1) {
                సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad plural/select pattern syntax.
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
            ఒకవేళ(length>Part::MAX_నిడివి) {
                సమితి_Parse_Error(parseError, selector_సూచీ);  // Argument selector too long.
                error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }
            కూడు_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_సూచీ, length, 0, error_సంకేతం);
            parseDouble(selector_సూచీ+1, సూచీ, అబద్ధం,
                        parseError, error_సంకేతం);  // adds ARG_INT or ARG_DOUBLE
        } లేకపోతే {
            సూచీ=skipIdentifier(సూచీ);
            int32_t length=సూచీ-selector_సూచీ;
            ఒకవేళ(length==0) {
                సమితి_Parse_Error(parseError, ప్రారంభం);  // Bad plural/select pattern syntax.
                error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                తిరిగి 0;
            }
            // Note: The ':' in "offset:" is just beyond the skipIdentifier() range.
            ఒకవేళ( UMSGPAT_ARG_TYPE_HAS_PLURAL_STYLE(argType) && length==6 && సూచీ<సందేశం.length() &&
                0==సందేశం.compare(selector_సూచీ, 7, k_స్థానభ్రంశం_Colon, 0, 7)
            ) {
                // plural offset, not a selector
                ఒకవేళ(!is_ఖాళీలు) {
                    // Plural argument 'offset:' (if present) must precede key-message pairs.
                    సమితి_Parse_Error(parseError, ప్రారంభం);
                    error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                    తిరిగి 0;
                }
                // allow whitespace between offset: and its value
                int32_t విలువ_సూచీ=skipWhiteSpace(సూచీ+1);  // The ':' is at index.
                సూచీ=skipDouble(విలువ_సూచీ);
                ఒకవేళ(సూచీ==విలువ_సూచీ) {
                    సమితి_Parse_Error(parseError, ప్రారంభం);  // Missing value for plural 'offset:'.
                    error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
                    తిరిగి 0;
                }
                ఒకవేళ((సూచీ-విలువ_సూచీ)>Part::MAX_నిడివి) {
                    సమితి_Parse_Error(parseError, విలువ_సూచీ);  // Plural offset value too long.
                    error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                    తిరిగి 0;
                }
                parseDouble(విలువ_సూచీ, సూచీ, అబద్ధం,
                            parseError, error_సంకేతం);  // adds ARG_INT or ARG_DOUBLE
                ఒకవేళ(U_FAILURE(error_సంకేతం)) {
                    తిరిగి 0;
                }
                is_ఖాళీలు=అబద్ధం;
                కొనసాగు;  // no message fragment after the offset
            } లేకపోతే {
                // normal selector word
                ఒకవేళ(length>Part::MAX_నిడివి) {
                    సమితి_Parse_Error(parseError, selector_సూచీ);  // Argument selector too long.
                    error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                    తిరిగి 0;
                }
                కూడు_Part(UMSGPAT_PART_TYPE_ARG_SELECTOR, selector_సూచీ, length, 0, error_సంకేతం);
                ఒకవేళ(0==సందేశం.compare(selector_సూచీ, length, kOther, 0, 5)) {
                    hasOther=నిజం;
                }
            }
        }
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }

        // parse the message fragment following the selector
        సూచీ=skipWhiteSpace(సూచీ);
        ఒకవేళ(సూచీ==సందేశం.length() || సందేశం.charAt(సూచీ)!=u_ఎడమ_Curly_Brace) {
            సమితి_Parse_Error(parseError, selector_సూచీ);  // No message fragment after plural/select selector.
            error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
            తిరిగి 0;
        }
        సూచీ=parse_సందేశం(సూచీ, 1, nestingLevel+1, argType, parseError, error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి 0;
        }
        is_ఖాళీలు=అబద్ధం;
    }
}

int32_t
సందేశం_నమూనా::parse_Arg_సంఖ్య(స్థిరరాశి UnicodeString &s, int32_t ప్రారంభం, int32_t హద్దు) {
    // If the identifier contains only ASCII digits, then it is an argument _number_
    // and must not have leading zeros (except "0" itself).
    // Otherwise it is an argument _name_.
    ఒకవేళ(ప్రారంభం>=హద్దు) {
        తిరిగి UMSGPAT_ARG_NAME_NOT_VALID;
    }
    int32_t number;
    // Defer numeric errors until we know there are only digits.
    UBool bad_సంఖ్య;
    char16_t c=s.charAt(ప్రారంభం++);
    ఒకవేళ(c==0x30) {
        ఒకవేళ(ప్రారంభం==హద్దు) {
            తిరిగి 0;
        } లేకపోతే {
            number=0;
            bad_సంఖ్య=నిజం;  // leading zero
        }
    } లేకపోతే ఒకవేళ(0x31<=c && c<=0x39) {
        number=c-0x30;
        bad_సంఖ్య=అబద్ధం;
    } లేకపోతే {
        తిరిగి UMSGPAT_ARG_NAME_NOT_సంఖ్య;
    }
    అయితే(ప్రారంభం<హద్దు) {
        c=s.charAt(ప్రారంభం++);
        ఒకవేళ(0x30<=c && c<=0x39) {
            ఒకవేళ(number>=INT32_MAX/10) {
                bad_సంఖ్య=నిజం;  //అధికప్రవాహం
            } లేకపోతే {
                number=number*10+(c-0x30);
            }
        } లేకపోతే {
            తిరిగి UMSGPAT_ARG_NAME_NOT_సంఖ్య;
        }
    }
    // There are only ASCII digits.
    ఒకవేళ(bad_సంఖ్య) {
        తిరిగి UMSGPAT_ARG_NAME_NOT_VALID;
    } లేకపోతే {
        తిరిగి number;
    }
}

శూన్య
సందేశం_నమూనా::parseDouble(int32_t ప్రారంభం, int32_t హద్దు, UBool allowInfinity,
                            UParseError *parseError, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    U_ASSERT(ప్రారంభం<హద్దు);
    // fake loop for easy exit and single throw statement
    కొరకు(;;) { /*loop doesn't iterate*/
        // fast path for small integers and infinity
        int32_t value=0;
        int32_t isNegative=0;  // not boolean so that we can easily add it to value
        int32_t సూచీ=ప్రారంభం;
        char16_t c=సందేశం.charAt(సూచీ++);
        ఒకవేళ(c==u_minus) {
            isNegative=1;
            ఒకవేళ(సూచీ==హద్దు) {
                ఆపు;  // no number
            }
            c=సందేశం.charAt(సూచీ++);
        } లేకపోతే ఒకవేళ(c==u_plus) {
            ఒకవేళ(సూచీ==హద్దు) {
                ఆపు;  // no number
            }
            c=సందేశం.charAt(సూచీ++);
        }
        ఒకవేళ(c==0x221e) {  // infinity
            ఒకవేళ(allowInfinity && సూచీ==హద్దు) {
                ద్విగుణ infinity=uprv_getInfinity();
                కూడు_Arg_Double_Part(
                    isNegative!=0 ? -infinity : infinity,
                    ప్రారంభం, హద్దు-ప్రారంభం, error_సంకేతం);
                తిరిగి;
            } లేకపోతే {
                ఆపు;
            }
        }
        // try to parse the number as a small integer but fall back to a double
        అయితే('0'<=c && c<='9') {
            value=value*10+(c-'0');
            ఒకవేళ(value>(Part::MAX_విలువ+isNegative)) {
                ఆపు;  // not a small-enough integer
            }
            ఒకవేళ(సూచీ==హద్దు) {
                కూడు_Part(UMSGPAT_PART_TYPE_ARG_INT, ప్రారంభం, హద్దు-ప్రారంభం,
                        isNegative!=0 ? -value : value, error_సంకేతం);
                తిరిగి;
            }
            c=సందేశం.charAt(సూచీ++);
        }
        // Let Double.parseDouble() throw a NumberFormatException.
        అక్షరం సంఖ్య_Chars[128];
        int32_t capacity = static_cast<int32_t>(పరిమాణం(సంఖ్య_Chars));
        int32_t length=హద్దు-ప్రారంభం;
        ఒకవేళ(length>=capacity) {
            ఆపు;  // number too long
        }
        సందేశం.extract(ప్రారంభం, length, సంఖ్య_Chars, capacity, US_INV);
        ఒకవేళ (static_cast<int32_t>(uprv_strlen(సంఖ్య_Chars)) < length) {
            ఆపు;  // contains non-invariant character that was turned into NUL
        }
        అక్షరం *చివర;
        ద్విగుణ numeric_విలువ=uprv_strtod(సంఖ్య_Chars, &చివర);
        ఒకవేళ(చివర!=(సంఖ్య_Chars+length)) {
            ఆపు;  // parsing error
        }
        కూడు_Arg_Double_Part(numeric_విలువ, ప్రారంభం, length, error_సంకేతం);
        తిరిగి;
    }
    సమితి_Parse_Error(parseError, ప్రారంభం /*, limit*/);  // Bad syntax for numeric value.
    error_సంకేతం=U_నమూనా_SYNTAX_ERROR;
}

int32_t
సందేశం_నమూనా::skipWhiteSpace(int32_t సూచీ) {
    స్థిరరాశి char16_t *s=సందేశం.get_బఫరు();
    int32_t సందేశం_నిడివి=సందేశం.length();
    స్థిరరాశి char16_t *t=నమూనా_Props::skipWhiteSpace(s+సూచీ, సందేశం_నిడివి-సూచీ);
    తిరిగి static_cast<int32_t>(t - s);
}

int32_t
సందేశం_నమూనా::skipIdentifier(int32_t సూచీ) {
    స్థిరరాశి char16_t *s=సందేశం.get_బఫరు();
    int32_t సందేశం_నిడివి=సందేశం.length();
    స్థిరరాశి char16_t *t=నమూనా_Props::skipIdentifier(s+సూచీ, సందేశం_నిడివి-సూచీ);
    తిరిగి static_cast<int32_t>(t - s);
}

int32_t
సందేశం_నమూనా::skipDouble(int32_t సూచీ) {
    int32_t సందేశం_నిడివి=సందేశం.length();
    అయితే(సూచీ<సందేశం_నిడివి) {
        char16_t c=సందేశం.charAt(సూచీ);
        // U+221E: Allow the infinity symbol, for ChoiceFormat patterns.
        ఒకవేళ((c<0x30 && c!=u_plus && c!=u_minus && c!=u_dot) || (c>0x39 && c!=u_e && c!=u_E && c!=0x221e)) {
            ఆపు;
        }
        ++సూచీ;
    }
    తిరిగి సూచీ;
}

UBool
సందేశం_నమూనా::isArgTypeChar(UChar32 c) {
    తిరిగి (u_a<=c && c<=u_z) || (u_A<=c && c<=u_Z);
}

UBool
సందేశం_నమూనా::isChoice(int32_t సూచీ) {
    char16_t c;
    తిరిగి
        ((c=సందేశం.charAt(సూచీ++))==u_c || c==u_C) &&
        ((c=సందేశం.charAt(సూచీ++))==u_h || c==u_H) &&
        ((c=సందేశం.charAt(సూచీ++))==u_o || c==u_O) &&
        ((c=సందేశం.charAt(సూచీ++))==u_i || c==u_I) &&
        ((c=సందేశం.charAt(సూచీ++))==u_c || c==u_C) &&
        ((c=సందేశం.charAt(సూచీ))==u_e || c==u_E);
}

UBool
సందేశం_నమూనా::isPlural(int32_t సూచీ) {
    char16_t c;
    తిరిగి
        ((c=సందేశం.charAt(సూచీ++))==u_p || c==u_P) &&
        ((c=సందేశం.charAt(సూచీ++))==u_l || c==u_L) &&
        ((c=సందేశం.charAt(సూచీ++))==u_u || c==u_U) &&
        ((c=సందేశం.charAt(సూచీ++))==u_r || c==u_R) &&
        ((c=సందేశం.charAt(సూచీ++))==u_a || c==u_A) &&
        ((c=సందేశం.charAt(సూచీ))==u_l || c==u_L);
}

UBool
సందేశం_నమూనా::isSelect(int32_t సూచీ) {
    char16_t c;
    తిరిగి
        ((c=సందేశం.charAt(సూచీ++))==u_s || c==u_S) &&
        ((c=సందేశం.charAt(సూచీ++))==u_e || c==u_E) &&
        ((c=సందేశం.charAt(సూచీ++))==u_l || c==u_L) &&
        ((c=సందేశం.charAt(సూచీ++))==u_e || c==u_E) &&
        ((c=సందేశం.charAt(సూచీ++))==u_c || c==u_C) &&
        ((c=సందేశం.charAt(సూచీ))==u_t || c==u_T);
}

UBool
సందేశం_నమూనా::isOrdinal(int32_t సూచీ) {
    char16_t c;
    తిరిగి
        ((c=సందేశం.charAt(సూచీ++))==u_o || c==u_O) &&
        ((c=సందేశం.charAt(సూచీ++))==u_r || c==u_R) &&
        ((c=సందేశం.charAt(సూచీ++))==u_d || c==u_D) &&
        ((c=సందేశం.charAt(సూచీ++))==u_i || c==u_I) &&
        ((c=సందేశం.charAt(సూచీ++))==u_n || c==u_N) &&
        ((c=సందేశం.charAt(సూచీ++))==u_a || c==u_A) &&
        ((c=సందేశం.charAt(సూచీ))==u_l || c==u_L);
}

UBool
సందేశం_నమూనా::లోపల_సందేశం_Format_నమూనా(int32_t nestingLevel) {
    తిరిగి nestingLevel>0 || partsList->a[0].type==UMSGPAT_PART_TYPE_సందేశం_ప్రారంభం;
}

UBool
సందేశం_నమూనా::లోపల_శిఖరం_Level_Choice_సందేశం(int32_t nestingLevel, UMessage_నమూనా_Arg_Type మాతృ_Type) {
    తిరిగి
        nestingLevel==1 &&
        మాతృ_Type==UMSGPAT_ARG_TYPE_CHOICE &&
        partsList->a[0].type!=UMSGPAT_PART_TYPE_సందేశం_ప్రారంభం;
}

శూన్య
సందేశం_నమూనా::కూడు_Part(UMessage_నమూనా_Part_Type type, int32_t సూచీ, int32_t length,
                        int32_t value, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(partsList->ensure_సామర్థ్యం_For_One_మరిన్ని(parts_నిడివి, error_సంకేతం)) {
        Part &part=partsList->a[parts_నిడివి++];
        part.type=type;
        part.సూచీ=సూచీ;
        part.length = static_cast<uint16_t>(length);
        part.value = static_cast<int16_t>(value);
        part.హద్దు_Part_సూచీ=0;
    }
}

శూన్య
సందేశం_నమూనా::కూడు_హద్దు_Part(int32_t ప్రారంభం,
                             UMessage_నమూనా_Part_Type type, int32_t సూచీ, int32_t length,
                             int32_t value, UError_సంకేతం &error_సంకేతం) {
    partsList->a[ప్రారంభం].హద్దు_Part_సూచీ=parts_నిడివి;
    కూడు_Part(type, సూచీ, length, value, error_సంకేతం);
}

శూన్య
సందేశం_నమూనా::కూడు_Arg_Double_Part(ద్విగుణ numeric_విలువ, int32_t ప్రారంభం, int32_t length,
                                 UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    int32_t numeric_సూచీ=numeric_విలువలు_నిడివి;
    ఒకవేళ(numeric_విలువలు_List==శూన్యసూచిక) {
        numeric_విలువలు_List=కొత్త సందేశం_నమూనా_Double_List();
        ఒకవేళ(numeric_విలువలు_List==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
    } లేకపోతే ఒకవేళ(!numeric_విలువలు_List->ensure_సామర్థ్యం_For_One_మరిన్ని(numeric_విలువలు_నిడివి, error_సంకేతం)) {
        తిరిగి;
    } లేకపోతే {
        ఒకవేళ(numeric_సూచీ>Part::MAX_విలువ) {
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి;
        }
    }
    numeric_విలువలు_List->a[numeric_విలువలు_నిడివి++]=numeric_విలువ;
    కూడు_Part(UMSGPAT_PART_TYPE_ARG_DOUBLE, ప్రారంభం, length, numeric_సూచీ, error_సంకేతం);
}

శూన్య
సందేశం_నమూనా::సమితి_Parse_Error(UParseError *parseError, int32_t సూచీ) {
    ఒకవేళ(parseError==శూన్యసూచిక) {
        తిరిగి;
    }
    parseError->స్థానభ్రంశం=సూచీ;

    // Set preContext to some of msg before index.
    // Avoid splitting a surrogate pair.
    int32_t length=సూచీ;
    ఒకవేళ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ఒకవేళ(length>0 && U16_IS_TRAIL(సందేశం[సూచీ-length])) {
            --length;
        }
    }
    సందేశం.extract(సూచీ-length, length, parseError->preContext);
    parseError->preContext[length]=0;

    // Set postContext to some of msg starting at index.
    length=సందేశం.length()-సూచీ;
    ఒకవేళ(length>=U_PARSE_CONTEXT_LEN) {
        length=U_PARSE_CONTEXT_LEN-1;
        ఒకవేళ(length>0 && U16_IS_LEAD(సందేశం[సూచీ+length-1])) {
            --length;
        }
    }
    సందేశం.extract(సూచీ, length, parseError->postContext);
    parseError->postContext[length]=0;
}

// MessageImpl ------------------------------------------------------------- ***

శూన్య
సందేశం_Impl::appendReducedApostrophes(స్థిరరాశి UnicodeString &s, int32_t ప్రారంభం, int32_t హద్దు,
                                      UnicodeString &sb) {
    int32_t doubleApos=-1;
    కొరకు(;;) {
        int32_t i=s.సూచీ_యొక్క(u_apos, ప్రారంభం);
        ఒకవేళ(i<0 || i>=హద్దు) {
            sb.append(s, ప్రారంభం, హద్దు-ప్రారంభం);
            ఆపు;
        }
        ఒకవేళ(i==doubleApos) {
            // Double apostrophe at start-1 and start==i, append one.
            sb.append(u_apos);
            ++ప్రారంభం;
            doubleApos=-1;
        } లేకపోతే {
            // Append text between apostrophes and skip this one.
            sb.append(s, ప్రారంభం, i-ప్రారంభం);
            doubleApos=ప్రారంభం=i+1;
        }
    }
}

// Ported from second half of ICU4J SelectFormat.format(String).
UnicodeString&
సందేశం_Impl::append_Sub_సందేశం_Without_Skip_Syntax(స్థిరరాశి సందేశం_నమూనా& సందేశం_నమూనా_2,
                                               int32_t సందేశం_ప్రారంభం,
                                               UnicodeString& ఫలితం U_LIFETIME_BOUND) {
    స్థిరరాశి UnicodeString &సందేశం_String=సందేశం_నమూనా_2.get_నమూనా_String();
    int32_t prev_సూచీ=సందేశం_నమూనా_2.getPart(సందేశం_ప్రారంభం).get_హద్దు();
    కొరకు(int32_t i=సందేశం_ప్రారంభం;;) {
        స్థిరరాశి సందేశం_నమూనా::Part &part=సందేశం_నమూనా_2.getPart(++i);
        UMessage_నమూనా_Part_Type type=part.getType();
        int32_t సూచీ=part.get_సూచీ();
        ఒకవేళ(type==UMSGPAT_PART_TYPE_సందేశం_హద్దు) {
            తిరిగి ఫలితం.append(సందేశం_String, prev_సూచీ, సూచీ-prev_సూచీ);
        } లేకపోతే ఒకవేళ(type==UMSGPAT_PART_TYPE_SKIP_SYNTAX) {
            ఫలితం.append(సందేశం_String, prev_సూచీ, సూచీ-prev_సూచీ);
            prev_సూచీ=part.get_హద్దు();
        } లేకపోతే ఒకవేళ(type==UMSGPAT_PART_TYPE_ARG_ప్రారంభం) {
            ఫలితం.append(సందేశం_String, prev_సూచీ, సూచీ-prev_సూచీ);
            prev_సూచీ=సూచీ;
            i=సందేశం_నమూనా_2.get_హద్దు_Part_సూచీ(i);
            సూచీ=సందేశం_నమూనా_2.getPart(i).get_హద్దు();
            appendReducedApostrophes(సందేశం_String, prev_సూచీ, సూచీ, ఫలితం);
            prev_సూచీ=సూచీ;
        }
    }
}

U_NAMESPACE_చివర

#అయితే_ముగింపు  // !UCONFIG_NO_FORMATTING
