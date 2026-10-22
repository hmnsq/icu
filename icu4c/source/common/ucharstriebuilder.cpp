// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstriebuilder.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/ucharstrie.h"
#అన్తర్భావ "unicode/ucharstriebuilder.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

తరగతి UCharsTrieElement : బహిరంగ UMemory {
బహిరంగ:
    // Use compiler's default constructor, initializes nothing.

    శూన్య సమితి_గమ్యం(స్థిరరాశి UnicodeString &s, int32_t val, UnicodeString &అక్షరశ్రేణులు, UError_సంకేతం &error_సంకేతం);

    UnicodeString getString(స్థిరరాశి UnicodeString& అక్షరశ్రేణులు U_LIFETIME_BOUND) స్థిరరాశి {
        int32_t length=అక్షరశ్రేణులు[string_స్థానభ్రంశం];
        తిరిగి అక్షరశ్రేణులు.తాత్కాలిక_Sub_String(string_స్థానభ్రంశం+1, length);
    }
    int32_t get_String_నిడివి(స్థిరరాశి UnicodeString &అక్షరశ్రేణులు) స్థిరరాశి {
        తిరిగి అక్షరశ్రేణులు[string_స్థానభ్రంశం];
    }

    char16_t charAt(int32_t సూచీ, స్థిరరాశి UnicodeString &అక్షరశ్రేణులు) స్థిరరాశి {
        తిరిగి అక్షరశ్రేణులు[string_స్థానభ్రంశం+1+సూచీ];
    }

    int32_t get_విలువ() స్థిరరాశి { తిరిగి value; }

    int32_t compare_String_గమ్యం(స్థిరరాశి UCharsTrieElement &o, స్థిరరాశి UnicodeString &అక్షరశ్రేణులు) స్థిరరాశి;

అంతరంగ:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_స్థానభ్రంశం;
    int32_t value;
};

శూన్య
UCharsTrieElement::సమితి_గమ్యం(స్థిరరాశి UnicodeString &s, int32_t val,
                         UnicodeString &అక్షరశ్రేణులు, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    int32_t length=s.length();
    ఒకవేళ(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }
    string_స్థానభ్రంశం=అక్షరశ్రేణులు.length();
    అక్షరశ్రేణులు.append(static_cast<char16_t>(length));
    value=val;
    అక్షరశ్రేణులు.append(s);
}

int32_t
UCharsTrieElement::compare_String_గమ్యం(స్థిరరాశి UCharsTrieElement &other, స్థిరరాశి UnicodeString &అక్షరశ్రేణులు) స్థిరరాశి {
    తిరిగి getString(అక్షరశ్రేణులు).compare(other.getString(అక్షరశ్రేణులు));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_సంకేతం & /*errorCode*/)
        : elements(శూన్యసూచిక), elements_సామర్థ్యం(0), elements_నిడివి(0),
          uchars(శూన్యసూచిక), uchars_సామర్థ్యం(0), uchars_నిడివి(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    తొలగించు[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::కూడు(స్థిరరాశి UnicodeString& s,
                       int32_t value,
                       UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ(uchars_నిడివి>0) {
        // Cannot add elements after building.
        error_సంకేతం=U_NO_WRITE_PERMISSION;
        తిరిగి *ఇది;
    }
    ఒకవేళ(elements_నిడివి==elements_సామర్థ్యం) {
        int32_t new_సామర్థ్యం;
        ఒకవేళ(elements_సామర్థ్యం==0) {
            new_సామర్థ్యం=1024;
        } లేకపోతే {
            new_సామర్థ్యం=4*elements_సామర్థ్యం;
        }
        UCharsTrieElement *newElements=కొత్త UCharsTrieElement[new_సామర్థ్యం];
        ఒకవేళ(newElements==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది;
        }
        ఒకవేళ(elements_నిడివి>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_నిడివి*పరిమాణం(UCharsTrieElement));
        }
        తొలగించు[] elements;
        elements=newElements;
        elements_సామర్థ్యం=new_సామర్థ్యం;
    }
    elements[elements_నిడివి++].సమితి_గమ్యం(s, value, అక్షరశ్రేణులు, error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం) && అక్షరశ్రేణులు.isBogus()) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
    తిరిగి *ఇది;
}

U_CDECL_BEGIN

స్థిర int32_t U_CALLCONV
compare_Element_అక్షరశ్రేణులు(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    స్థిరరాశి UnicodeString *అక్షరశ్రేణులు=static_cast<స్థిరరాశి UnicodeString *>(context);
    స్థిరరాశి UCharsTrieElement *ఎడమ_Element=static_cast<స్థిరరాశి UCharsTrieElement *>(ఎడమ);
    స్థిరరాశి UCharsTrieElement *కుడి_Element=static_cast<స్థిరరాశి UCharsTrieElement *>(కుడి);
    తిరిగి ఎడమ_Element->compare_String_గమ్యం(*కుడి_Element, *అక్షరశ్రేణులు);
}

U_CDECL_చివర

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం) {
    buildUChars(buildOption, error_సంకేతం);
    UCharsTrie *newTrie=శూన్యసూచిక;
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        newTrie=కొత్త UCharsTrie(uchars, uchars+(uchars_సామర్థ్యం-uchars_నిడివి));
        ఒకవేళ(newTrie==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            uchars=శూన్యసూచిక;  // The new trie now owns the array.
            uchars_సామర్థ్యం=0;
        }
    }
    తిరిగి newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& ఫలితం U_LIFETIME_BOUND,
                                      UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_సంకేతం);
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        ఫలితం.సమితి_గమ్యం(అబద్ధం, uchars+(uchars_సామర్థ్యం-uchars_నిడివి), uchars_నిడివి);
    }
    తిరిగి ఫలితం;
}

శూన్య
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(uchars!=శూన్యసూచిక && uchars_నిడివి>0) {
        // Already built.
        తిరిగి;
    }
    ఒకవేళ(uchars_నిడివి==0) {
        ఒకవేళ(elements_నిడివి==0) {
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి;
        }
        ఒకవేళ(అక్షరశ్రేణులు.isBogus()) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        uprv_క్రమీకరించు_Array(elements, elements_నిడివి, static_cast<int32_t>(పరిమాణం(UCharsTrieElement)),
                      compare_Element_అక్షరశ్రేణులు, &అక్షరశ్రేణులు,
                      అబద్ధం,  // need not be a stable sort
                      &error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
        // Duplicate strings are not allowed.
        UnicodeString prev=elements[0].getString(అక్షరశ్రేణులు);
        కొరకు(int32_t i=1; i<elements_నిడివి; ++i) {
            UnicodeString current=elements[i].getString(అక్షరశ్రేణులు);
            ఒకవేళ(prev==current) {
                error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
                తిరిగి;
            }
            prev.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_నిడివి=0;
    int32_t capacity=అక్షరశ్రేణులు.length();
    ఒకవేళ(capacity<1024) {
        capacity=1024;
    }
    ఒకవేళ(uchars_సామర్థ్యం<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        ఒకవేళ(uchars==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            uchars_సామర్థ్యం=0;
            తిరిగి;
        }
        uchars_సామర్థ్యం=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_నిడివి, error_సంకేతం);
    ఒకవేళ(uchars==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_నిడివి(int32_t i) స్థిరరాశి {
    తిరిగి elements[i].get_String_నిడివి(అక్షరశ్రేణులు);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_సూచీ) స్థిరరాశి {
    తిరిగి elements[i].charAt(unit_సూచీ, అక్షరశ్రేణులు);
}

int32_t
UCharsTrieBuilder::get_Element_విలువ(int32_t i) స్థిరరాశి {
    తిరిగి elements[i].get_విలువ();
}

int32_t
UCharsTrieBuilder::get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(int32_t first, int32_t చివరి, int32_t unit_సూచీ) స్థిరరాశి {
    స్థిరరాశి UCharsTrieElement &మొదటి_Element=elements[first];
    స్థిరరాశి UCharsTrieElement &చివరి_Element=elements[చివరి];
    int32_t min_String_నిడివి=మొదటి_Element.get_String_నిడివి(అక్షరశ్రేణులు);
    అయితే(++unit_సూచీ<min_String_నిడివి &&
            మొదటి_Element.charAt(unit_సూచీ, అక్షరశ్రేణులు)==
            చివరి_Element.charAt(unit_సూచీ, అక్షరశ్రేణులు)) {}
    తిరిగి unit_సూచీ;
}

int32_t
UCharsTrieBuilder::లెక్క_Element_Units(int32_t ప్రారంభం, int32_t హద్దు, int32_t unit_సూచీ) స్థిరరాశి {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=ప్రారంభం;
    చేయి {
        char16_t unit=elements[i++].charAt(unit_సూచీ, అక్షరశ్రేణులు);
        అయితే(i<హద్దు && unit==elements[i].charAt(unit_సూచీ, అక్షరశ్రేణులు)) {
            ++i;
        }
        ++length;
    } అయితే(i<హద్దు);
    తిరిగి length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_సూచీ, int32_t count) స్థిరరాశి {
    చేయి {
        char16_t unit=elements[i++].charAt(unit_సూచీ, అక్షరశ్రేణులు);
        అయితే(unit==elements[i].charAt(unit_సూచీ, అక్షరశ్రేణులు)) {
            ++i;
        }
    } అయితే(--count>0);
    తిరిగి i;
}

int32_t
UCharsTrieBuilder::సూచీ_యొక్క_Element_With_తదుపరి_Unit(int32_t i, int32_t unit_సూచీ, char16_t unit) స్థిరరాశి {
    అయితే(unit==elements[i].charAt(unit_సూచీ, అక్షరశ్రేణులు)) {
        ++i;
    }
    తిరిగి i;
}

UCharsTrieBuilder::UCTLinear_సరిపోలిక_కణుపు::UCTLinear_సరిపోలిక_కణుపు(స్థిరరాశి char16_t *units, int32_t len, Node *తదుపరి_కణుపు)
        : రేఖీయ_సరిపోలిక_కణుపు(len, తదుపరి_కణుపు), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

తార్కిక
UCharsTrieBuilder::UCTLinear_సరిపోలిక_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!రేఖీయ_సరిపోలిక_కణుపు::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి UCTLinear_సరిపోలిక_కణుపు &o=static_cast<స్థిరరాశి UCTLinear_సరిపోలిక_కణుపు &>(other);
    తిరిగి 0==u_memcmp(s, o.s, length);
}

శూన్య
UCharsTrieBuilder::UCTLinear_సరిపోలిక_కణుపు::లిఖ(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->లిఖ(builder);
    b.లిఖ(s, length);
    స్థానభ్రంశం=b.write_విలువ_And_Type(has_విలువ, value, b.get_Min_రేఖీయ_సరిపోలిక()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_రేఖీయ_సరిపోలిక_కణుపు(int32_t i, int32_t unit_సూచీ, int32_t length,
                                         Node *తదుపరి_కణుపు) స్థిరరాశి {
    తిరిగి కొత్త UCTLinear_సరిపోలిక_కణుపు(
            elements[i].getString(అక్షరశ్రేణులు).get_బఫరు()+unit_సూచీ,
            length,
            తదుపరి_కణుపు);
}

UBool
UCharsTrieBuilder::ensure_సామర్థ్యం(int32_t length) {
    ఒకవేళ(uchars==శూన్యసూచిక) {
        తిరిగి అబద్ధం;  // previous memory allocation had failed
    }
    ఒకవేళ(length>uchars_సామర్థ్యం) {
        int32_t new_సామర్థ్యం=uchars_సామర్థ్యం;
        చేయి {
            new_సామర్థ్యం*=2;
        } అయితే(new_సామర్థ్యం<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_సామర్థ్యం*2));
        ఒకవేళ(newUChars==శూన్యసూచిక) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=శూన్యసూచిక;
            uchars_సామర్థ్యం=0;
            తిరిగి అబద్ధం;
        }
        u_memcpy(newUChars+(new_సామర్థ్యం-uchars_నిడివి),
                 uchars+(uchars_సామర్థ్యం-uchars_నిడివి), uchars_నిడివి);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_సామర్థ్యం=new_సామర్థ్యం;
    }
    తిరిగి నిజం;
}

int32_t
UCharsTrieBuilder::లిఖ(int32_t unit) {
    int32_t new_నిడివి=uchars_నిడివి+1;
    ఒకవేళ(ensure_సామర్థ్యం(new_నిడివి)) {
        uchars_నిడివి=new_నిడివి;
        uchars[uchars_సామర్థ్యం - uchars_నిడివి] = static_cast<char16_t>(unit);
    }
    తిరిగి uchars_నిడివి;
}

int32_t
UCharsTrieBuilder::లిఖ(స్థిరరాశి char16_t *s, int32_t length) {
    int32_t new_నిడివి=uchars_నిడివి+length;
    ఒకవేళ(ensure_సామర్థ్యం(new_నిడివి)) {
        uchars_నిడివి=new_నిడివి;
        u_memcpy(uchars+(uchars_సామర్థ్యం-uchars_నిడివి), s, length);
    }
    తిరిగి uchars_నిడివి;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_సూచీ, int32_t length) {
    తిరిగి లిఖ(elements[i].getString(అక్షరశ్రేణులు).get_బఫరు()+unit_సూచీ, length);
}

int32_t
UCharsTrieBuilder::write_విలువ_And_Final(int32_t i, UBool isFinal) {
    ఒకవేళ(0<=i && i<=UCharsTrie::k_Max_One_Unit_విలువ) {
        తిరిగి లిఖ(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    ఒకవేళ(i<0 || i>UCharsTrie::k_Max_Two_Unit_విలువ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_విలువ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } లేకపోతే {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_విలువ_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    తిరిగి లిఖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_విలువ_And_Type(UBool has_విలువ, int32_t value, int32_t కణుపు) {
    ఒకవేళ(!has_విలువ) {
        తిరిగి లిఖ(కణుపు);
    }
    char16_t intUnits[3];
    int32_t length;
    ఒకవేళ(value<0 || value>UCharsTrie::k_Max_Two_Unit_కణుపు_విలువ) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_కణుపు_విలువ_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } లేకపోతే ఒకవేళ(value<=UCharsTrie::k_Max_One_Unit_కణుపు_విలువ) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } లేకపోతే {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_కణుపు_విలువ_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(కణుపు);
    తిరిగి లిఖ(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_గమ్యం(int32_t jump_లక్ష్యం) {
    int32_t i=uchars_నిడివి-jump_లక్ష్యం;
    U_ASSERT(i>=0);
    ఒకవేళ(i<=UCharsTrie::kMaxOneUnitDelta) {
        తిరిగి లిఖ(i);
    }
    char16_t intUnits[3];
    int32_t length;
    ఒకవేళ(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } లేకపోతే {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    తిరిగి లిఖ(intUnits, length);
}

U_NAMESPACE_చివర
