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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/bytestriebuilder.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (bytes, value) pairs with full copies
 * of the byte sequences, until the BytesTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

वर्ग अष्टकानि_Trie_Element : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य समुच्चयः_गन्तव्यम्(StringPiece s, int32_t val, CharString &वाक्यानि, UError_संहिता &error_संहिता);

    StringPiece getString(ध्रुव CharString& वाक्यानि U_LIFETIME_BOUND) ध्रुव {
        int32_t विस्थापनम्=string_विस्थापनम्;
        int32_t length;
        यदि(विस्थापनम्>=0) {
            length = static_cast<uint8_t>(वाक्यानि[विस्थापनम्++]);
        } अन्यथा {
            विस्थापनम्=~विस्थापनम्;
            length = (static_cast<int32_t>(static_cast<uint8_t>(वाक्यानि[विस्थापनम्])) << 8) | static_cast<uint8_t>(वाक्यानि[विस्थापनम् + 1]);
            विस्थापनम्+=2;
        }
        प्रत्यागम StringPiece(वाक्यानि.दत्त()+विस्थापनम्, length);
    }
    int32_t get_String_दीर्घता(ध्रुव CharString &वाक्यानि) ध्रुव {
        int32_t विस्थापनम्=string_विस्थापनम्;
        यदि(विस्थापनम्>=0) {
            प्रत्यागम static_cast<uint8_t>(वाक्यानि[विस्थापनम्]);
        } अन्यथा {
            विस्थापनम्=~विस्थापनम्;
            प्रत्यागम (static_cast<int32_t>(static_cast<uint8_t>(वाक्यानि[विस्थापनम्])) << 8) | static_cast<uint8_t>(वाक्यानि[विस्थापनम् + 1]);
        }
    }

    वर्ण charAt(int32_t सूचकांक, ध्रुव CharString &वाक्यानि) ध्रुव { प्रत्यागम दत्त(वाक्यानि)[सूचकांक]; }

    int32_t get_मानम्() ध्रुव { प्रत्यागम value; }

    int32_t compare_String_गन्तव्यम्(ध्रुव अष्टकानि_Trie_Element &o, ध्रुव CharString &वाक्यानि) ध्रुव;

गुप्त:
    ध्रुव वर्ण* दत्त(ध्रुव CharString& वाक्यानि U_LIFETIME_BOUND) ध्रुव {
        int32_t विस्थापनम्=string_विस्थापनम्;
        यदि(विस्थापनम्>=0) {
            ++विस्थापनम्;
        } अन्यथा {
            विस्थापनम्=~विस्थापनम्+2;
        }
        प्रत्यागम वाक्यानि.दत्त()+विस्थापनम्;
    }

    // If the stringOffset is non-negative, then the first strings byte contains
    // the string length.
    // If the stringOffset is negative, then the first two strings bytes contain
    // the string length (big-endian), and the offset needs to be bit-inverted.
    // (Compared with a stringLength field here, this saves 3 bytes per string for most strings.)
    int32_t string_विस्थापनम्;
    int32_t value;
};

शून्य
अष्टकानि_Trie_Element::समुच्चयः_गन्तव्यम्(StringPiece s, int32_t val,
                        CharString &वाक्यानि, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    int32_t length=s.length();
    यदि(length>0xffff) {
        // Too long: We store the length in 1 or 2 bytes.
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    int32_t विस्थापनम्=वाक्यानि.length();
    यदि(length>0xff) {
        विस्थापनम्=~विस्थापनम्;
        वाक्यानि.append(static_cast<वर्ण>(length >> 8), error_संहिता);
    }
    वाक्यानि.append(static_cast<वर्ण>(length), error_संहिता);
    string_विस्थापनम्=विस्थापनम्;
    value=val;
    वाक्यानि.append(s, error_संहिता);
}

int32_t
अष्टकानि_Trie_Element::compare_String_गन्तव्यम्(ध्रुव अष्टकानि_Trie_Element &other, ध्रुव CharString &वाक्यानि) ध्रुव {
    // TODO: add StringPiece::compare(), see ticket #8187
    StringPiece thisString=getString(वाक्यानि);
    StringPiece otherString=other.getString(वाक्यानि);
    int32_t दीर्घता_Diff=thisString.length()-otherString.length();
    int32_t common_दीर्घता;
    यदि(दीर्घता_Diff<=0) {
        common_दीर्घता=thisString.length();
    } अन्यथा {
        common_दीर्घता=otherString.length();
    }
    int32_t diff=uprv_memcmp(thisString.दत्त(), otherString.दत्त(), common_दीर्घता);
    प्रत्यागम diff!=0 ? diff : दीर्घता_Diff;
}

अष्टकानि_Trie_Builder::अष्टकानि_Trie_Builder(UError_संहिता &error_संहिता)
        : वाक्यानि(शून्यसूचकः), elements(शून्यसूचकः), elements_क्षमता(0), elements_दीर्घता(0),
          अष्टकानि(शून्यसूचकः), अष्टकानि_क्षमता(0), अष्टकानि_दीर्घता(0) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    वाक्यानि=नूतन CharString();
    यदि(वाक्यानि==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

अष्टकानि_Trie_Builder::~अष्टकानि_Trie_Builder() {
    विलोपय वाक्यानि;
    विलोपय[] elements;
    uprv_free(अष्टकानि);
}

अष्टकानि_Trie_Builder&
अष्टकानि_Trie_Builder::सङ्कलय(StringPiece s, int32_t value, UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    यदि(अष्टकानि_दीर्घता>0) {
        // Cannot add elements after building.
        error_संहिता=U_NO_WRITE_PERMISSION;
        प्रत्यागम *एतत्;
    }
    यदि(elements_दीर्घता==elements_क्षमता) {
        int32_t new_क्षमता;
        यदि(elements_क्षमता==0) {
            new_क्षमता=1024;
        } अन्यथा {
            new_क्षमता=4*elements_क्षमता;
        }
        अष्टकानि_Trie_Element *newElements=नूतन अष्टकानि_Trie_Element[new_क्षमता];
        यदि(newElements==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्; // error instead of dereferencing null
        }
        यदि(elements_दीर्घता>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_दीर्घता*परिमाण(अष्टकानि_Trie_Element));
        }
        विलोपय[] elements;
        elements=newElements;
        elements_क्षमता=new_क्षमता;
    }
    elements[elements_दीर्घता++].समुच्चयः_गन्तव्यम्(s, value, *वाक्यानि, error_संहिता);
    प्रत्यागम *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compare_Element_वाक्यानि(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    ध्रुव CharString *वाक्यानि=static_cast<ध्रुव CharString *>(context);
    ध्रुव अष्टकानि_Trie_Element *वामः_Element=static_cast<ध्रुव अष्टकानि_Trie_Element *>(वामतः);
    ध्रुव अष्टकानि_Trie_Element *दक्षिणः_Element=static_cast<ध्रुव अष्टकानि_Trie_Element *>(दक्षिणतः);
    प्रत्यागम वामः_Element->compare_String_गन्तव्यम्(*दक्षिणः_Element, *वाक्यानि);
}

U_CDECL_अन्तः

अष्टकानि_Trie *
अष्टकानि_Trie_Builder::build(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता) {
    build_अष्टकानि(buildOption, error_संहिता);
    अष्टकानि_Trie *newTrie=शून्यसूचकः;
    यदि(U_SUCCESS(error_संहिता)) {
        newTrie=नूतन अष्टकानि_Trie(अष्टकानि, अष्टकानि+(अष्टकानि_क्षमता-अष्टकानि_दीर्घता));
        यदि(newTrie==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            अष्टकानि=शून्यसूचकः;  // The new trie now owns the array.
            अष्टकानि_क्षमता=0;
        }
    }
    प्रत्यागम newTrie;
}

StringPiece
अष्टकानि_Trie_Builder::buildStringPiece(UStringTrieBuildOption buildOption,
                                   UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    build_अष्टकानि(buildOption, error_संहिता);
    StringPiece परिणाम;
    यदि(U_SUCCESS(error_संहिता)) {
        परिणाम.समुच्चयः(अष्टकानि+(अष्टकानि_क्षमता-अष्टकानि_दीर्घता), अष्टकानि_दीर्घता);
    }
    प्रत्यागम परिणाम;
}

शून्य
अष्टकानि_Trie_Builder::build_अष्टकानि(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि(अष्टकानि!=शून्यसूचकः && अष्टकानि_दीर्घता>0) {
        // Already built.
        प्रत्यागम;
    }
    यदि(अष्टकानि_दीर्घता==0) {
        यदि(elements_दीर्घता==0) {
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        uprv_क्रमय_Array(elements, elements_दीर्घता, static_cast<int32_t>(परिमाण(अष्टकानि_Trie_Element)),
                      compare_Element_वाक्यानि, वाक्यानि,
                      असत्य,  // need not be a stable sort
                      &error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
        // Duplicate strings are not allowed.
        StringPiece पूर्व=elements[0].getString(*वाक्यानि);
        प्रति(int32_t i=1; i<elements_दीर्घता; ++i) {
            StringPiece current=elements[i].getString(*वाक्यानि);
            यदि(पूर्व==current) {
                error_संहिता=U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम;
            }
            पूर्व=current;
        }
    }
    // Create and byte-serialize the trie for the elements.
    अष्टकानि_दीर्घता=0;
    int32_t capacity=वाक्यानि->length();
    यदि(capacity<1024) {
        capacity=1024;
    }
    यदि(अष्टकानि_क्षमता<capacity) {
        uprv_free(अष्टकानि);
        अष्टकानि=static_cast<वर्ण *>(uprv_malloc(capacity));
        यदि(अष्टकानि==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            अष्टकानि_क्षमता=0;
            प्रत्यागम;
        }
        अष्टकानि_क्षमता=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_दीर्घता, error_संहिता);
    यदि(अष्टकानि==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

अष्टकानि_Trie_Builder&
अष्टकानि_Trie_Builder::clear() U_LIFETIME_BOUND {
    वाक्यानि->clear();
    elements_दीर्घता=0;
    अष्टकानि_दीर्घता=0;
    प्रत्यागम *एतत्;
}

int32_t
अष्टकानि_Trie_Builder::get_Element_String_दीर्घता(int32_t i) ध्रुव {
    प्रत्यागम elements[i].get_String_दीर्घता(*वाक्यानि);
}

char16_t
अष्टकानि_Trie_Builder::getElementUnit(int32_t i, int32_t byte_सूचकांक) ध्रुव {
    प्रत्यागम static_cast<uint8_t>(elements[i].charAt(byte_सूचकांक, *वाक्यानि));
}

int32_t
अष्टकानि_Trie_Builder::get_Element_मानम्(int32_t i) ध्रुव {
    प्रत्यागम elements[i].get_मानम्();
}

int32_t
अष्टकानि_Trie_Builder::get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(int32_t first, int32_t चरमः, int32_t byte_सूचकांक) ध्रुव {
    ध्रुव अष्टकानि_Trie_Element &firstElement=elements[first];
    ध्रुव अष्टकानि_Trie_Element &चरमः_Element=elements[चरमः];
    int32_t min_String_दीर्घता=firstElement.get_String_दीर्घता(*वाक्यानि);
    यावत्(++byte_सूचकांक<min_String_दीर्घता &&
            firstElement.charAt(byte_सूचकांक, *वाक्यानि)==
            चरमः_Element.charAt(byte_सूचकांक, *वाक्यानि)) {}
    प्रत्यागम byte_सूचकांक;
}

int32_t
अष्टकानि_Trie_Builder::गणनम्_Element_Units(int32_t start, int32_t सीमा, int32_t byte_सूचकांक) ध्रुव {
    int32_t length=0;  // Number of different bytes at byteIndex.
    int32_t i=start;
    कुरु {
        वर्ण byte=elements[i++].charAt(byte_सूचकांक, *वाक्यानि);
        यावत्(i<सीमा && byte==elements[i].charAt(byte_सूचकांक, *वाक्यानि)) {
            ++i;
        }
        ++length;
    } यावत्(i<सीमा);
    प्रत्यागम length;
}

int32_t
अष्टकानि_Trie_Builder::skipElementsBySomeUnits(int32_t i, int32_t byte_सूचकांक, int32_t count) ध्रुव {
    कुरु {
        वर्ण byte=elements[i++].charAt(byte_सूचकांक, *वाक्यानि);
        यावत्(byte==elements[i].charAt(byte_सूचकांक, *वाक्यानि)) {
            ++i;
        }
    } यावत्(--count>0);
    प्रत्यागम i;
}

int32_t
अष्टकानि_Trie_Builder::सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(int32_t i, int32_t byte_सूचकांक, char16_t byte) ध्रुव {
    वर्ण b = static_cast<वर्ण>(byte);
    यावत्(b==elements[i].charAt(byte_सूचकांक, *वाक्यानि)) {
        ++i;
    }
    प्रत्यागम i;
}

अष्टकानि_Trie_Builder::BTLinear_मेलनम्_ग्रन्थिः::BTLinear_मेलनम्_ग्रन्थिः(ध्रुव वर्ण *अष्टकानि, int32_t len, Node *अग्र_ग्रन्थिः)
        : रैखिकम्_मेलनम्_ग्रन्थिः(len, अग्र_ग्रन्थिः), s(अष्टकानि) {
    hash=static_cast<int32_t>(
        static_cast<uint32_t>(hash)*37u + static_cast<uint32_t>(ustr_hashCharsN(अष्टकानि, len)));
}

तार्किक
अष्टकानि_Trie_Builder::BTLinear_मेलनम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!रैखिकम्_मेलनम्_ग्रन्थिः::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव BTLinear_मेलनम्_ग्रन्थिः &o=static_cast<ध्रुव BTLinear_मेलनम्_ग्रन्थिः &>(other);
    प्रत्यागम 0==uprv_memcmp(s, o.s, length);
}

शून्य
अष्टकानि_Trie_Builder::BTLinear_मेलनम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    अष्टकानि_Trie_Builder &b=static_cast<अष्टकानि_Trie_Builder &>(builder);
    next->लिख(builder);
    b.लिख(s, length);
    विस्थापनम्=b.लिख(b.get_Min_रैखिकम्_मेलनम्()+length-1);
}

StringTrieBuilder::Node *
अष्टकानि_Trie_Builder::create_रैखिकम्_मेलनम्_ग्रन्थिः(int32_t i, int32_t byte_सूचकांक, int32_t length,
                                        Node *अग्र_ग्रन्थिः) ध्रुव {
    प्रत्यागम नूतन BTLinear_मेलनम्_ग्रन्थिः(
            elements[i].getString(*वाक्यानि).दत्त()+byte_सूचकांक,
            length,
            अग्र_ग्रन्थिः);
}

UBool
अष्टकानि_Trie_Builder::ensure_क्षमता(int32_t length) {
    यदि(अष्टकानि==शून्यसूचकः) {
        प्रत्यागम असत्य;  // previous memory allocation had failed
    }
    यदि(length>अष्टकानि_क्षमता) {
        int32_t new_क्षमता=अष्टकानि_क्षमता;
        कुरु {
            new_क्षमता*=2;
        } यावत्(new_क्षमता<=length);
        वर्ण *new_अष्टकानि=static_cast<वर्ण *>(uprv_malloc(new_क्षमता));
        यदि(new_अष्टकानि==शून्यसूचकः) {
            // unable to allocate memory
            uprv_free(अष्टकानि);
            अष्टकानि=शून्यसूचकः;
            अष्टकानि_क्षमता=0;
            प्रत्यागम असत्य;
        }
        uprv_memcpy(new_अष्टकानि+(new_क्षमता-अष्टकानि_दीर्घता),
                    अष्टकानि+(अष्टकानि_क्षमता-अष्टकानि_दीर्घता), अष्टकानि_दीर्घता);
        uprv_free(अष्टकानि);
        अष्टकानि=new_अष्टकानि;
        अष्टकानि_क्षमता=new_क्षमता;
    }
    प्रत्यागम सत्य;
}

int32_t
अष्टकानि_Trie_Builder::लिख(int32_t byte) {
    int32_t new_दीर्घता=अष्टकानि_दीर्घता+1;
    यदि(ensure_क्षमता(new_दीर्घता)) {
        अष्टकानि_दीर्घता=new_दीर्घता;
        अष्टकानि[अष्टकानि_क्षमता - अष्टकानि_दीर्घता] = static_cast<वर्ण>(byte);
    }
    प्रत्यागम अष्टकानि_दीर्घता;
}

int32_t
अष्टकानि_Trie_Builder::लिख(ध्रुव वर्ण *b, int32_t length) {
    int32_t new_दीर्घता=अष्टकानि_दीर्घता+length;
    यदि(ensure_क्षमता(new_दीर्घता)) {
        अष्टकानि_दीर्घता=new_दीर्घता;
        uprv_memcpy(अष्टकानि+(अष्टकानि_क्षमता-अष्टकानि_दीर्घता), b, length);
    }
    प्रत्यागम अष्टकानि_दीर्घता;
}

int32_t
अष्टकानि_Trie_Builder::writeElementUnits(int32_t i, int32_t byte_सूचकांक, int32_t length) {
    प्रत्यागम लिख(elements[i].getString(*वाक्यानि).दत्त()+byte_सूचकांक, length);
}

int32_t
अष्टकानि_Trie_Builder::write_मानम्_And_Final(int32_t i, UBool isFinal) {
    यदि(0<=i && i<=अष्टकानि_Trie::k_Max_One_Byte_मानम्) {
        प्रत्यागम लिख(((अष्टकानि_Trie::k_Min_One_Byte_मानम्_Lead+i)<<1)|isFinal);
    }
    वर्ण int_अष्टकानि[5];
    int32_t length=1;
    यदि(i<0 || i>0xffffff) {
        int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::k_Five_Byte_मानम्_Lead);
        int_अष्टकानि[1] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 24);
        int_अष्टकानि[2] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 16);
        int_अष्टकानि[3] = static_cast<वर्ण>(static_cast<uint32_t>(i) >> 8);
        int_अष्टकानि[4] = static_cast<वर्ण>(i);
        length=5;
    // } else if(i<=BytesTrie::kMaxOneByteValue) {
    //     intBytes[0]=(char)(BytesTrie::kMinOneByteValueLead+i);
    } अन्यथा {
        यदि(i<=अष्टकानि_Trie::k_Max_Two_Byte_मानम्) {
            int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::k_Min_Two_Byte_मानम्_Lead + (i >> 8));
        } अन्यथा {
            यदि(i<=अष्टकानि_Trie::k_Max_Three_Byte_मानम्) {
                int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::k_Min_Three_Byte_मानम्_Lead + (i >> 16));
            } अन्यथा {
                int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::k_Four_Byte_मानम्_Lead);
                int_अष्टकानि[1] = static_cast<वर्ण>(i >> 16);
                length=2;
            }
            int_अष्टकानि[length++] = static_cast<वर्ण>(i >> 8);
        }
        int_अष्टकानि[length++] = static_cast<वर्ण>(i);
    }
    int_अष्टकानि[0] = static_cast<वर्ण>((int_अष्टकानि[0] << 1) | isFinal);
    प्रत्यागम लिख(int_अष्टकानि, length);
}

int32_t
अष्टकानि_Trie_Builder::write_मानम्_And_Type(UBool has_मानम्, int32_t value, int32_t ग्रन्थिः) {
    int32_t विस्थापनम्=लिख(ग्रन्थिः);
    यदि(has_मानम्) {
        विस्थापनम्=write_मानम्_And_Final(value, असत्य);
    }
    प्रत्यागम विस्थापनम्;
}

int32_t
अष्टकानि_Trie_Builder::write_Delta_गन्तव्यम्(int32_t jumpTarget) {
    int32_t i=अष्टकानि_दीर्घता-jumpTarget;
    U_ASSERT(i>=0);
    यदि(i<=अष्टकानि_Trie::kMaxOneByteDelta) {
        प्रत्यागम लिख(i);
    } अन्यथा {
        वर्ण int_अष्टकानि[5];
        प्रत्यागम लिख(int_अष्टकानि, आन्तरिकम्_Encode_Delta(i, int_अष्टकानि));
    }
}

int32_t
अष्टकानि_Trie_Builder::आन्तरिकम्_Encode_Delta(int32_t i, वर्ण int_अष्टकानि[]) {
    U_ASSERT(i>=0);
    यदि(i<=अष्टकानि_Trie::kMaxOneByteDelta) {
        int_अष्टकानि[0] = static_cast<वर्ण>(i);
        प्रत्यागम 1;
    }
    int32_t length=1;
    यदि(i<=अष्टकानि_Trie::kMaxTwoByteDelta) {
        int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::kMinTwoByteDeltaLead + (i >> 8));
    } अन्यथा {
        यदि(i<=अष्टकानि_Trie::kMaxThreeByteDelta) {
            int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::kMinThreeByteDeltaLead + (i >> 16));
        } अन्यथा {
            यदि(i<=0xffffff) {
                int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::kFourByteDeltaLead);
            } अन्यथा {
                int_अष्टकानि[0] = static_cast<वर्ण>(अष्टकानि_Trie::kFiveByteDeltaLead);
                int_अष्टकानि[1] = static_cast<वर्ण>(i >> 24);
                length=2;
            }
            int_अष्टकानि[length++] = static_cast<वर्ण>(i >> 16);
        }
        int_अष्टकानि[length++] = static_cast<वर्ण>(i >> 8);
    }
    int_अष्टकानि[length++] = static_cast<वर्ण>(i);
    प्रत्यागम length;
}

U_NAMESPACE_अन्तः
