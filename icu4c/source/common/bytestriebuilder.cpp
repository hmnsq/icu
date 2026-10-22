// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/bytestriebuilder.h"
#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "uarrsort.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

తరగతి బైట్లు_Trie_Element : బహిరంగ UMemory {
బహిరంగ:
    // Use compiler's default constructor, initializes nothing.

    శూన్య సమితి_గమ్యం(StringPiece s, int32_t val, CharString &అక్షరశ్రేణులు, UError_సంకేతం &error_సంకేతం);

    StringPiece getString(స్థిరరాశి CharString& అక్షరశ్రేణులు U_LIFETIME_BOUND) స్థిరరాశి {
        int32_t స్థానభ్రంశం=string_స్థానభ్రంశం;
        int32_t length;
        ఒకవేళ(స్థానభ్రంశం>=0) {
            length = static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం++]);
        } లేకపోతే {
            స్థానభ్రంశం=~స్థానభ్రంశం;
            length = (static_cast<int32_t>(static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం])) << 8) | static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం + 1]);
            స్థానభ్రంశం+=2;
        }
        తిరిగి StringPiece(అక్షరశ్రేణులు.దత్తాంశం()+స్థానభ్రంశం, length);
    }
    int32_t get_String_నిడివి(స్థిరరాశి CharString &అక్షరశ్రేణులు) స్థిరరాశి {
        int32_t స్థానభ్రంశం=string_స్థానభ్రంశం;
        ఒకవేళ(స్థానభ్రంశం>=0) {
            తిరిగి static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం]);
        } లేకపోతే {
            స్థానభ్రంశం=~స్థానభ్రంశం;
            తిరిగి (static_cast<int32_t>(static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం])) << 8) | static_cast<uint8_t>(అక్షరశ్రేణులు[స్థానభ్రంశం + 1]);
        }
    }

    అక్షరం charAt(int32_t సూచీ, స్థిరరాశి CharString &అక్షరశ్రేణులు) స్థిరరాశి { తిరిగి దత్తాంశం(అక్షరశ్రేణులు)[సూచీ]; }

    int32_t get_విలువ() స్థిరరాశి { తిరిగి value; }

    int32_t compare_String_గమ్యం(స్థిరరాశి బైట్లు_Trie_Element &o, స్థిరరాశి CharString &అక్షరశ్రేణులు) స్థిరరాశి;

అంతరంగ:
    స్థిరరాశి అక్షరం* దత్తాంశం(స్థిరరాశి CharString& అక్షరశ్రేణులు U_LIFETIME_BOUND) స్థిరరాశి {
        int32_t స్థానభ్రంశం=string_స్థానభ్రంశం;
        ఒకవేళ(స్థానభ్రంశం>=0) {
            ++స్థానభ్రంశం;
        } లేకపోతే {
            స్థానభ్రంశం=~స్థానభ్రంశం+2;
        }
        తిరిగి అక్షరశ్రేణులు.దత్తాంశం()+స్థానభ్రంశం;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_స్థానభ్రంశం;
    int32_t value;
};

శూన్య
బైట్లు_Trie_Element::సమితి_గమ్యం(StringPiece s, int32_t val,
                        CharString &అక్షరశ్రేణులు, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    int32_t length=s.length();
    ఒకవేళ(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
        తిరిగి;
    }
    int32_t స్థానభ్రంశం=అక్షరశ్రేణులు.length();
    ఒకవేళ(length>0xff) {
        స్థానభ్రంశం=~స్థానభ్రంశం;
        అక్షరశ్రేణులు.append(static_cast<అక్షరం>(length >> 8), error_సంకేతం);
    }
    అక్షరశ్రేణులు.append(static_cast<అక్షరం>(length), error_సంకేతం);
    string_స్థానభ్రంశం=స్థానభ్రంశం;
    value=val;
    అక్షరశ్రేణులు.append(s, error_సంకేతం);
}

int32_t
బైట్లు_Trie_Element::compare_String_గమ్యం(స్థిరరాశి బైట్లు_Trie_Element &other, స్థిరరాశి CharString &అక్షరశ్రేణులు) స్థిరరాశి {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(అక్షరశ్రేణులు);
    StringPiece otherString=other.getString(అక్షరశ్రేణులు);
    int32_t నిడివి_Diff=thisString.length()-otherString.length();
    int32_t common_నిడివి;
    ఒకవేళ(నిడివి_Diff<=0) {
        common_నిడివి=thisString.length();
    } లేకపోతే {
        common_నిడివి=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.దత్తాంశం(), otherString.దత్తాంశం(), common_నిడివి);
    తిరిగి diff!=0 ? diff : నిడివి_Diff;
}

బైట్లు_Trie_Builder::బైట్లు_Trie_Builder(UError_సంకేతం &error_సంకేతం)
        : అక్షరశ్రేణులు(శూన్యసూచిక), elements(శూన్యసూచిక), elements_సామర్థ్యం(0), elements_నిడివి(0),
          బైట్లు(శూన్యసూచిక), బైట్లు_సామర్థ్యం(0), బైట్లు_నిడివి(0) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    అక్షరశ్రేణులు=కొత్త CharString();
    ఒకవేళ(అక్షరశ్రేణులు==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

బైట్లు_Trie_Builder::~బైట్లు_Trie_Builder() {
    తొలగించు అక్షరశ్రేణులు;
    తొలగించు[] elements;
    uprv_free(బైట్లు);
}

బైట్లు_Trie_Builder&
బైట్లు_Trie_Builder::కూడు(StringPiece s, int32_t value, UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి *ఇది;
    }
    ఒకవేళ(బైట్లు_నిడివి>0) {
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
        బైట్లు_Trie_Element *newElements=కొత్త బైట్లు_Trie_Element[new_సామర్థ్యం];
        ఒకవేళ(newElements==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి *ఇది; // error instead of dereferencing null
        }
        ఒకవేళ(elements_నిడివి>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_నిడివి*పరిమాణం(బైట్లు_Trie_Element));
        }
        తొలగించు[] elements;
        elements=newElements;
        elements_సామర్థ్యం=new_సామర్థ్యం;
    }
    elements[elements_నిడివి++].సమితి_గమ్యం(s, value, *అక్షరశ్రేణులు, error_సంకేతం);
    తిరిగి *ఇది;
}

U_CDECL_BEGIN

స్థిర int32_t U_CALLCONV
compare_Element_అక్షరశ్రేణులు(స్థిరరాశి శూన్య *context, స్థిరరాశి శూన్య *ఎడమ, స్థిరరాశి శూన్య *కుడి) {
    స్థిరరాశి CharString *అక్షరశ్రేణులు=static_cast<స్థిరరాశి CharString *>(context);
    స్థిరరాశి బైట్లు_Trie_Element *ఎడమ_Element=static_cast<స్థిరరాశి బైట్లు_Trie_Element *>(ఎడమ);
    స్థిరరాశి బైట్లు_Trie_Element *కుడి_Element=static_cast<స్థిరరాశి బైట్లు_Trie_Element *>(కుడి);
    తిరిగి ఎడమ_Element->compare_String_గమ్యం(*కుడి_Element, *అక్షరశ్రేణులు);
}

U_CDECL_చివర

బైట్లు_Trie *
బైట్లు_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం) {
    build_బైట్లు(buildOption, error_సంకేతం);
    బైట్లు_Trie *newTrie=శూన్యసూచిక;
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        newTrie=కొత్త బైట్లు_Trie(బైట్లు, బైట్లు+(బైట్లు_సామర్థ్యం-బైట్లు_నిడివి));
        ఒకవేళ(newTrie==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
        } లేకపోతే {
            బైట్లు=శూన్యసూచిక;  // The new trie now owns the array.
            బైట్లు_సామర్థ్యం=0;
        }
    }
    తిరిగి newTrie;
}

StringPiece
బైట్లు_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_సంకేతం& error_సంకేతం) U_LIFETIME_BOUND {
    build_బైట్లు(buildOption, error_సంకేతం);
    StringPiece ఫలితం;
    ఒకవేళ(U_SUCCESS(error_సంకేతం)) {
        ఫలితం.సమితి(బైట్లు+(బైట్లు_సామర్థ్యం-బైట్లు_నిడివి), బైట్లు_నిడివి);
    }
    తిరిగి ఫలితం;
}

శూన్య
బైట్లు_Trie_Builder::build_బైట్లు(UStringTrieBuildOption buildOption, UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ(U_FAILURE(error_సంకేతం)) {
        తిరిగి;
    }
    ఒకవేళ(బైట్లు!=శూన్యసూచిక && బైట్లు_నిడివి>0) {
        // Already built.
        తిరిగి;
    }
    ఒకవేళ(బైట్లు_నిడివి==0) {
        ఒకవేళ(elements_నిడివి==0) {
            error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి;
        }
        uprv_క్రమీకరించు_Array(elements, elements_నిడివి, static_cast<int32_t>(పరిమాణం(బైట్లు_Trie_Element)),
                      compare_Element_అక్షరశ్రేణులు, అక్షరశ్రేణులు,
                      అబద్ధం,  // need not be a stable sort
                      &error_సంకేతం);
        ఒకవేళ(U_FAILURE(error_సంకేతం)) {
            తిరిగి;
        }
        // Duplicate strings are not allowed.
        StringPiece prev=elements[0].getString(*అక్షరశ్రేణులు);
        కొరకు(int32_t i=1; i<elements_నిడివి; ++i) {
            StringPiece current=elements[i].getString(*అక్షరశ్రేణులు);
            ఒకవేళ(prev==current) {
                error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
                తిరిగి;
            }
            prev=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    బైట్లు_నిడివి=0;
    int32_t capacity=అక్షరశ్రేణులు->length();
    ఒకవేళ(capacity<1024) {
        capacity=1024;
    }
    ఒకవేళ(బైట్లు_సామర్థ్యం<capacity) {
        uprv_free(బైట్లు);
        బైట్లు=static_cast<అక్షరం *>(uprv_malloc(capacity));
        ఒకవేళ(బైట్లు==శూన్యసూచిక) {
            error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            బైట్లు_సామర్థ్యం=0;
            తిరిగి;
        }
        బైట్లు_సామర్థ్యం=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_నిడివి, error_సంకేతం);
    ఒకవేళ(బైట్లు==శూన్యసూచిక) {
        error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }
}

బైట్లు_Trie_Builder&
బైట్లు_Trie_Builder::clear() U_LIFETIME_BOUND {
    అక్షరశ్రేణులు->clear();
    elements_నిడివి=0;
    బైట్లు_నిడివి=0;
    తిరిగి *ఇది;
}

int32_t
బైట్లు_Trie_Builder::get_Element_String_నిడివి(int32_t i) స్థిరరాశి {
    తిరిగి elements[i].get_String_నిడివి(*అక్షరశ్రేణులు);
}

char16_t
బైట్లు_Trie_Builder::getElementUnit(int32_t i, int32_t byte_సూచీ) స్థిరరాశి {
    తిరిగి static_cast<uint8_t>(elements[i].charAt(byte_సూచీ, *అక్షరశ్రేణులు));
}

int32_t
బైట్లు_Trie_Builder::get_Element_విలువ(int32_t i) స్థిరరాశి {
    తిరిగి elements[i].get_విలువ();
}

int32_t
బైట్లు_Trie_Builder::get_హద్దు_యొక్క_రేఖీయ_సరిపోలిక(int32_t first, int32_t చివరి, int32_t byte_సూచీ) స్థిరరాశి {
    స్థిరరాశి బైట్లు_Trie_Element &మొదటి_Element=elements[first];
    స్థిరరాశి బైట్లు_Trie_Element &చివరి_Element=elements[చివరి];
    int32_t min_String_నిడివి=మొదటి_Element.get_String_నిడివి(*అక్షరశ్రేణులు);
    అయితే(++byte_సూచీ<min_String_నిడివి &&
            మొదటి_Element.charAt(byte_సూచీ, *అక్షరశ్రేణులు)==
            చివరి_Element.charAt(byte_సూచీ, *అక్షరశ్రేణులు)) {}
    తిరిగి byte_సూచీ;
}

int32_t
బైట్లు_Trie_Builder::లెక్క_Element_Units(int32_t ప్రారంభం, int32_t హద్దు, int32_t byte_సూచీ) స్థిరరాశి {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=ప్రారంభం;
    చేయి {
        అక్షరం byte=elements[i++].charAt(byte_సూచీ, *అక్షరశ్రేణులు);
        అయితే(i<హద్దు && byte==elements[i].charAt(byte_సూచీ, *అక్షరశ్రేణులు)) {
            ++i;
        }
        ++length;
    } అయితే(i<హద్దు);
    తిరిగి length;
}

int32_t
బైట్లు_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_సూచీ, int32_t count) స్థిరరాశి {
    చేయి {
        అక్షరం byte=elements[i++].charAt(byte_సూచీ, *అక్షరశ్రేణులు);
        అయితే(byte==elements[i].charAt(byte_సూచీ, *అక్షరశ్రేణులు)) {
            ++i;
        }
    } అయితే(--count>0);
    తిరిగి i;
}

int32_t
బైట్లు_Trie_Builder::సూచీ_యొక్క_Element_With_తదుపరి_Unit(int32_t i, int32_t byte_సూచీ, char16_t byte) స్థిరరాశి {
    అక్షరం b = static_cast<అక్షరం>(byte);
    అయితే(b==elements[i].charAt(byte_సూచీ, *అక్షరశ్రేణులు)) {
        ++i;
    }
    తిరిగి i;
}

బైట్లు_Trie_Builder::BTLinear_సరిపోలిక_కణుపు::BTLinear_సరిపోలిక_కణుపు(స్థిరరాశి అక్షరం *బైట్లు, int32_t len, Node *తదుపరి_కణుపు)
        : రేఖీయ_సరిపోలిక_కణుపు(len, తదుపరి_కణుపు), s(బైట్లు) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(బైట్లు, len)));
}

తార్కిక
బైట్లు_Trie_Builder::BTLinear_సరిపోలిక_కణుపు::క్రియాకారకం==(స్థిరరాశి Node &other) స్థిరరాశి {
    ఒకవేళ(ఇది==&other) {
        తిరిగి నిజం;
    }
    ఒకవేళ(!రేఖీయ_సరిపోలిక_కణుపు::క్రియాకారకం==(other)) {
        తిరిగి అబద్ధం;
    }
    స్థిరరాశి BTLinear_సరిపోలిక_కణుపు &o=static_cast<స్థిరరాశి BTLinear_సరిపోలిక_కణుపు &>(other);
    తిరిగి 0==uprv_memcmp(s, o.s, length);
}

శూన్య
బైట్లు_Trie_Builder::BTLinear_సరిపోలిక_కణుపు::లిఖ(StringTrieBuilder &builder) {
    బైట్లు_Trie_Builder &b=static_cast<బైట్లు_Trie_Builder &>(builder);
    next->లిఖ(builder);
    b.లిఖ(s, length);
    స్థానభ్రంశం=b.లిఖ(b.get_Min_రేఖీయ_సరిపోలిక()+length-1);
}

StringTrieBuilder::Node *
బైట్లు_Trie_Builder::create_రేఖీయ_సరిపోలిక_కణుపు(int32_t i, int32_t byte_సూచీ, int32_t length,
                                        Node *తదుపరి_కణుపు) స్థిరరాశి {
    తిరిగి కొత్త BTLinear_సరిపోలిక_కణుపు(
            elements[i].getString(*అక్షరశ్రేణులు).దత్తాంశం()+byte_సూచీ,
            length,
            తదుపరి_కణుపు);
}

UBool
బైట్లు_Trie_Builder::ensure_సామర్థ్యం(int32_t length) {
    ఒకవేళ(బైట్లు==శూన్యసూచిక) {
        తిరిగి అబద్ధం;  // previous memory allocation had failed
    }
    ఒకవేళ(length>బైట్లు_సామర్థ్యం) {
        int32_t new_సామర్థ్యం=బైట్లు_సామర్థ్యం;
        చేయి {
            new_సామర్థ్యం*=2;
        } అయితే(new_సామర్థ్యం<=length);
        అక్షరం *new_బైట్లు=static_cast<అక్షరం *>(uprv_malloc(new_సామర్థ్యం));
        ఒకవేళ(new_బైట్లు==శూన్యసూచిక) {
            // unable to allocate memory
            uprv_free(బైట్లు);
            బైట్లు=శూన్యసూచిక;
            బైట్లు_సామర్థ్యం=0;
            తిరిగి అబద్ధం;
        }
        uprv_memcpy(new_బైట్లు+(new_సామర్థ్యం-బైట్లు_నిడివి),
                    బైట్లు+(బైట్లు_సామర్థ్యం-బైట్లు_నిడివి), బైట్లు_నిడివి);
        uprv_free(బైట్లు);
        బైట్లు=new_బైట్లు;
        బైట్లు_సామర్థ్యం=new_సామర్థ్యం;
    }
    తిరిగి నిజం;
}

int32_t
బైట్లు_Trie_Builder::లిఖ(int32_t byte) {
    int32_t new_నిడివి=బైట్లు_నిడివి+1;
    ఒకవేళ(ensure_సామర్థ్యం(new_నిడివి)) {
        బైట్లు_నిడివి=new_నిడివి;
        బైట్లు[బైట్లు_సామర్థ్యం - బైట్లు_నిడివి] = static_cast<అక్షరం>(byte);
    }
    తిరిగి బైట్లు_నిడివి;
}

int32_t
బైట్లు_Trie_Builder::లిఖ(స్థిరరాశి అక్షరం *b, int32_t length) {
    int32_t new_నిడివి=బైట్లు_నిడివి+length;
    ఒకవేళ(ensure_సామర్థ్యం(new_నిడివి)) {
        బైట్లు_నిడివి=new_నిడివి;
        uprv_memcpy(బైట్లు+(బైట్లు_సామర్థ్యం-బైట్లు_నిడివి), b, length);
    }
    తిరిగి బైట్లు_నిడివి;
}

int32_t
బైట్లు_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_సూచీ, int32_t length) {
    తిరిగి లిఖ(elements[i].getString(*అక్షరశ్రేణులు).దత్తాంశం()+byte_సూచీ, length);
}

int32_t
బైట్లు_Trie_Builder::write_విలువ_And_Final(int32_t i, UBool isFinal) {
    ఒకవేళ(0<=i && i<=బైట్లు_Trie::k_Max_One_Byte_విలువ) {
        తిరిగి లిఖ(((బైట్లు_Trie::k_Min_One_Byte_విలువ_Lead+i)<<1)|isFinal);
    }
    అక్షరం int_బైట్లు[5];
    int32_t length=1;
    ఒకవేళ(i<0 || i>0xffffff) {
        int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::k_Five_Byte_విలువ_Lead);
        int_బైట్లు[1] = static_cast<అక్షరం>(static_cast<uint32_t>(i) >> 24);
        int_బైట్లు[2] = static_cast<అక్షరం>(static_cast<uint32_t>(i) >> 16);
        int_బైట్లు[3] = static_cast<అక్షరం>(static_cast<uint32_t>(i) >> 8);
        int_బైట్లు[4] = static_cast<అక్షరం>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } లేకపోతే {
        ఒకవేళ(i<=బైట్లు_Trie::k_Max_Two_Byte_విలువ) {
            int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::k_Min_Two_Byte_విలువ_Lead + (i >> 8));
        } లేకపోతే {
            ఒకవేళ(i<=బైట్లు_Trie::k_Max_Three_Byte_విలువ) {
                int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::k_Min_Three_Byte_విలువ_Lead + (i >> 16));
            } లేకపోతే {
                int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::k_Four_Byte_విలువ_Lead);
                int_బైట్లు[1] = static_cast<అక్షరం>(i >> 16);
                length=2;
            }
            int_బైట్లు[length++] = static_cast<అక్షరం>(i >> 8);
        }
        int_బైట్లు[length++] = static_cast<అక్షరం>(i);
    }
    int_బైట్లు[0] = static_cast<అక్షరం>((int_బైట్లు[0] << 1) | isFinal);
    తిరిగి లిఖ(int_బైట్లు, length);
}

int32_t
బైట్లు_Trie_Builder::write_విలువ_And_Type(UBool has_విలువ, int32_t value, int32_t కణుపు) {
    int32_t స్థానభ్రంశం=లిఖ(కణుపు);
    ఒకవేళ(has_విలువ) {
        స్థానభ్రంశం=write_విలువ_And_Final(value, అబద్ధం);
    }
    తిరిగి స్థానభ్రంశం;
}

int32_t
బైట్లు_Trie_Builder::write_Delta_గమ్యం(int32_t jump_లక్ష్యం) {
    int32_t i=బైట్లు_నిడివి-jump_లక్ష్యం;
    U_ASSERT(i>=0);
    ఒకవేళ(i<=బైట్లు_Trie::kMaxOneByteDelta) {
        తిరిగి లిఖ(i);
    } లేకపోతే {
        అక్షరం int_బైట్లు[5];
        తిరిగి లిఖ(int_బైట్లు, అంతర్గత_Encode_Delta(i, int_బైట్లు));
    }
}

int32_t
బైట్లు_Trie_Builder::అంతర్గత_Encode_Delta(int32_t i, అక్షరం int_బైట్లు[]) {
    U_ASSERT(i>=0);
    ఒకవేళ(i<=బైట్లు_Trie::kMaxOneByteDelta) {
        int_బైట్లు[0] = static_cast<అక్షరం>(i);
        తిరిగి 1;
    }
    int32_t length=1;
    ఒకవేళ(i<=బైట్లు_Trie::kMaxTwoByteDelta) {
        int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } లేకపోతే {
        ఒకవేళ(i<=బైట్లు_Trie::kMaxThreeByteDelta) {
            int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } లేకపోతే {
            ఒకవేళ(i<=0xffffff) {
                int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::kFourByteDeltaLead);
            } లేకపోతే {
                int_బైట్లు[0] = static_cast<అక్షరం>(బైట్లు_Trie::kFiveByteDeltaLead);
                int_బైట్లు[1] = static_cast<అక్షరం>(i >> 24);
                length=2;
            }
            int_బైట్లు[length++] = static_cast<అక్షరం>(i >> 16);
        }
        int_బైట్లు[length++] = static_cast<అక్షరం>(i >> 8);
    }
    int_బైట్లు[length++] = static_cast<అక్షరం>(i);
    తిరిగి length;
}

U_NAMESPACE_చివర
