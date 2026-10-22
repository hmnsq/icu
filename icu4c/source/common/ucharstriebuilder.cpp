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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/ucharstrie.h"
#अन्तर्भाव "unicode/ucharstriebuilder.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_BEGIN

/*
 * Note: This builder implementation stores (string, value) pairs with full copies
 * of the 16-bit-unit sequences, until the UCharsTrie is built.
 * It might(!) take less memory if we collected the data in a temporary, dynamic trie.
 */

वर्ग UCharsTrieElement : सार्वजनिक UMemory {
सार्वजनिक:
    // Use compiler's default constructor, initializes nothing.

    शून्य समुच्चयः_गन्तव्यम्(ध्रुव UnicodeString &s, int32_t val, UnicodeString &वाक्यानि, UError_संहिता &error_संहिता);

    UnicodeString getString(ध्रुव UnicodeString& वाक्यानि U_LIFETIME_BOUND) ध्रुव {
        int32_t length=वाक्यानि[string_विस्थापनम्];
        प्रत्यागम वाक्यानि.अस्थायी_Sub_String(string_विस्थापनम्+1, length);
    }
    int32_t get_String_दीर्घता(ध्रुव UnicodeString &वाक्यानि) ध्रुव {
        प्रत्यागम वाक्यानि[string_विस्थापनम्];
    }

    char16_t charAt(int32_t सूचकांक, ध्रुव UnicodeString &वाक्यानि) ध्रुव {
        प्रत्यागम वाक्यानि[string_विस्थापनम्+1+सूचकांक];
    }

    int32_t get_मानम्() ध्रुव { प्रत्यागम value; }

    int32_t compare_String_गन्तव्यम्(ध्रुव UCharsTrieElement &o, ध्रुव UnicodeString &वाक्यानि) ध्रुव;

गुप्त:
    // The first strings unit contains the string length.
    // (Compared with a stringLength field here, this saves 2 bytes per string.)
    int32_t string_विस्थापनम्;
    int32_t value;
};

शून्य
UCharsTrieElement::समुच्चयः_गन्तव्यम्(ध्रुव UnicodeString &s, int32_t val,
                         UnicodeString &वाक्यानि, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    int32_t length=s.length();
    यदि(length>0xffff) {
        // Too long: We store the length in 1 unit.
        error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम;
    }
    string_विस्थापनम्=वाक्यानि.length();
    वाक्यानि.append(static_cast<char16_t>(length));
    value=val;
    वाक्यानि.append(s);
}

int32_t
UCharsTrieElement::compare_String_गन्तव्यम्(ध्रुव UCharsTrieElement &other, ध्रुव UnicodeString &वाक्यानि) ध्रुव {
    प्रत्यागम getString(वाक्यानि).compare(other.getString(वाक्यानि));
}

UCharsTrieBuilder::UCharsTrieBuilder(UError_संहिता & /*errorCode*/)
        : elements(शून्यसूचकः), elements_क्षमता(0), elements_दीर्घता(0),
          uchars(शून्यसूचकः), uchars_क्षमता(0), uchars_दीर्घता(0) {}

UCharsTrieBuilder::~UCharsTrieBuilder() {
    विलोपय[] elements;
    uprv_free(uchars);
}

UCharsTrieBuilder&
UCharsTrieBuilder::सङ्कलय(ध्रुव UnicodeString& s,
                       int32_t value,
                       UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम *एतत्;
    }
    यदि(uchars_दीर्घता>0) {
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
        UCharsTrieElement *newElements=नूतन UCharsTrieElement[new_क्षमता];
        यदि(newElements==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम *एतत्;
        }
        यदि(elements_दीर्घता>0) {
            uprv_memcpy(newElements, elements, (size_t)elements_दीर्घता*परिमाण(UCharsTrieElement));
        }
        विलोपय[] elements;
        elements=newElements;
        elements_क्षमता=new_क्षमता;
    }
    elements[elements_दीर्घता++].समुच्चयः_गन्तव्यम्(s, value, वाक्यानि, error_संहिता);
    यदि(U_SUCCESS(error_संहिता) && वाक्यानि.isBogus()) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
    प्रत्यागम *एतत्;
}

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
compare_Element_वाक्यानि(ध्रुव शून्य *context, ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    ध्रुव UnicodeString *वाक्यानि=static_cast<ध्रुव UnicodeString *>(context);
    ध्रुव UCharsTrieElement *वामः_Element=static_cast<ध्रुव UCharsTrieElement *>(वामतः);
    ध्रुव UCharsTrieElement *दक्षिणः_Element=static_cast<ध्रुव UCharsTrieElement *>(दक्षिणतः);
    प्रत्यागम वामः_Element->compare_String_गन्तव्यम्(*दक्षिणः_Element, *वाक्यानि);
}

U_CDECL_अन्तः

UCharsTrie *
UCharsTrieBuilder::build(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता) {
    buildUChars(buildOption, error_संहिता);
    UCharsTrie *newTrie=शून्यसूचकः;
    यदि(U_SUCCESS(error_संहिता)) {
        newTrie=नूतन UCharsTrie(uchars, uchars+(uchars_क्षमता-uchars_दीर्घता));
        यदि(newTrie==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            uchars=शून्यसूचकः;  // The new trie now owns the array.
            uchars_क्षमता=0;
        }
    }
    प्रत्यागम newTrie;
}

UnicodeString&
UCharsTrieBuilder::buildUnicodeString(UStringTrieBuildOption buildOption,
                                      UnicodeString& परिणाम U_LIFETIME_BOUND,
                                      UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    buildUChars(buildOption, error_संहिता);
    यदि(U_SUCCESS(error_संहिता)) {
        परिणाम.समुच्चयः_गन्तव्यम्(असत्य, uchars+(uchars_क्षमता-uchars_दीर्घता), uchars_दीर्घता);
    }
    प्रत्यागम परिणाम;
}

शून्य
UCharsTrieBuilder::buildUChars(UStringTrieBuildOption buildOption, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि(uchars!=शून्यसूचकः && uchars_दीर्घता>0) {
        // Already built.
        प्रत्यागम;
    }
    यदि(uchars_दीर्घता==0) {
        यदि(elements_दीर्घता==0) {
            error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम;
        }
        यदि(वाक्यानि.isBogus()) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        uprv_क्रमय_Array(elements, elements_दीर्घता, static_cast<int32_t>(परिमाण(UCharsTrieElement)),
                      compare_Element_वाक्यानि, &वाक्यानि,
                      असत्य,  // need not be a stable sort
                      &error_संहिता);
        यदि(U_FAILURE(error_संहिता)) {
            प्रत्यागम;
        }
        // Duplicate strings are not allowed.
        UnicodeString पूर्व=elements[0].getString(वाक्यानि);
        प्रति(int32_t i=1; i<elements_दीर्घता; ++i) {
            UnicodeString current=elements[i].getString(वाक्यानि);
            यदि(पूर्व==current) {
                error_संहिता=U_ILLEGAL_तर्कः_ERROR;
                प्रत्यागम;
            }
            पूर्व.fastCopyFrom(current);
        }
    }
    // Create and char16_t-serialize the trie for the elements.
    uchars_दीर्घता=0;
    int32_t capacity=वाक्यानि.length();
    यदि(capacity<1024) {
        capacity=1024;
    }
    यदि(uchars_क्षमता<capacity) {
        uprv_free(uchars);
        uchars=static_cast<char16_t *>(uprv_malloc(capacity*2));
        यदि(uchars==शून्यसूचकः) {
            error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            uchars_क्षमता=0;
            प्रत्यागम;
        }
        uchars_क्षमता=capacity;
    }
    StringTrieBuilder::build(buildOption, elements_दीर्घता, error_संहिता);
    यदि(uchars==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

int32_t
UCharsTrieBuilder::get_Element_String_दीर्घता(int32_t i) ध्रुव {
    प्रत्यागम elements[i].get_String_दीर्घता(वाक्यानि);
}

char16_t
UCharsTrieBuilder::getElementUnit(int32_t i, int32_t unit_सूचकांक) ध्रुव {
    प्रत्यागम elements[i].charAt(unit_सूचकांक, वाक्यानि);
}

int32_t
UCharsTrieBuilder::get_Element_मानम्(int32_t i) ध्रुव {
    प्रत्यागम elements[i].get_मानम्();
}

int32_t
UCharsTrieBuilder::get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(int32_t first, int32_t चरमः, int32_t unit_सूचकांक) ध्रुव {
    ध्रुव UCharsTrieElement &firstElement=elements[first];
    ध्रुव UCharsTrieElement &चरमः_Element=elements[चरमः];
    int32_t min_String_दीर्घता=firstElement.get_String_दीर्घता(वाक्यानि);
    यावत्(++unit_सूचकांक<min_String_दीर्घता &&
            firstElement.charAt(unit_सूचकांक, वाक्यानि)==
            चरमः_Element.charAt(unit_सूचकांक, वाक्यानि)) {}
    प्रत्यागम unit_सूचकांक;
}

int32_t
UCharsTrieBuilder::गणनम्_Element_Units(int32_t start, int32_t सीमा, int32_t unit_सूचकांक) ध्रुव {
    int32_t length=0;  // Number of different units at unitIndex.
    int32_t i=start;
    कुरु {
        char16_t unit=elements[i++].charAt(unit_सूचकांक, वाक्यानि);
        यावत्(i<सीमा && unit==elements[i].charAt(unit_सूचकांक, वाक्यानि)) {
            ++i;
        }
        ++length;
    } यावत्(i<सीमा);
    प्रत्यागम length;
}

int32_t
UCharsTrieBuilder::skipElementsBySomeUnits(int32_t i, int32_t unit_सूचकांक, int32_t count) ध्रुव {
    कुरु {
        char16_t unit=elements[i++].charAt(unit_सूचकांक, वाक्यानि);
        यावत्(unit==elements[i].charAt(unit_सूचकांक, वाक्यानि)) {
            ++i;
        }
    } यावत्(--count>0);
    प्रत्यागम i;
}

int32_t
UCharsTrieBuilder::सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(int32_t i, int32_t unit_सूचकांक, char16_t unit) ध्रुव {
    यावत्(unit==elements[i].charAt(unit_सूचकांक, वाक्यानि)) {
        ++i;
    }
    प्रत्यागम i;
}

UCharsTrieBuilder::UCTLinear_मेलनम्_ग्रन्थिः::UCTLinear_मेलनम्_ग्रन्थिः(ध्रुव char16_t *units, int32_t len, Node *अग्र_ग्रन्थिः)
        : रैखिकम्_मेलनम्_ग्रन्थिः(len, अग्र_ग्रन्थिः), s(units) {
    hash=hash*37u+ustr_hashUCharsN(units, len);
}

तार्किक
UCharsTrieBuilder::UCTLinear_मेलनम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!रैखिकम्_मेलनम्_ग्रन्थिः::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव UCTLinear_मेलनम्_ग्रन्थिः &o=static_cast<ध्रुव UCTLinear_मेलनम्_ग्रन्थिः &>(other);
    प्रत्यागम 0==u_memcmp(s, o.s, length);
}

शून्य
UCharsTrieBuilder::UCTLinear_मेलनम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    UCharsTrieBuilder &b=(UCharsTrieBuilder &)builder;
    next->लिख(builder);
    b.लिख(s, length);
    विस्थापनम्=b.write_मानम्_And_Type(has_मानम्, value, b.get_Min_रैखिकम्_मेलनम्()+length-1);
}

StringTrieBuilder::Node *
UCharsTrieBuilder::create_रैखिकम्_मेलनम्_ग्रन्थिः(int32_t i, int32_t unit_सूचकांक, int32_t length,
                                         Node *अग्र_ग्रन्थिः) ध्रुव {
    प्रत्यागम नूतन UCTLinear_मेलनम्_ग्रन्थिः(
            elements[i].getString(वाक्यानि).get_बफर()+unit_सूचकांक,
            length,
            अग्र_ग्रन्थिः);
}

UBool
UCharsTrieBuilder::ensure_क्षमता(int32_t length) {
    यदि(uchars==शून्यसूचकः) {
        प्रत्यागम असत्य;  // previous memory allocation had failed
    }
    यदि(length>uchars_क्षमता) {
        int32_t new_क्षमता=uchars_क्षमता;
        कुरु {
            new_क्षमता*=2;
        } यावत्(new_क्षमता<=length);
        char16_t *newUChars=static_cast<char16_t *>(uprv_malloc(new_क्षमता*2));
        यदि(newUChars==शून्यसूचकः) {
            // unable to allocate memory
            uprv_free(uchars);
            uchars=शून्यसूचकः;
            uchars_क्षमता=0;
            प्रत्यागम असत्य;
        }
        u_memcpy(newUChars+(new_क्षमता-uchars_दीर्घता),
                 uchars+(uchars_क्षमता-uchars_दीर्घता), uchars_दीर्घता);
        uprv_free(uchars);
        uchars=newUChars;
        uchars_क्षमता=new_क्षमता;
    }
    प्रत्यागम सत्य;
}

int32_t
UCharsTrieBuilder::लिख(int32_t unit) {
    int32_t new_दीर्घता=uchars_दीर्घता+1;
    यदि(ensure_क्षमता(new_दीर्घता)) {
        uchars_दीर्घता=new_दीर्घता;
        uchars[uchars_क्षमता - uchars_दीर्घता] = static_cast<char16_t>(unit);
    }
    प्रत्यागम uchars_दीर्घता;
}

int32_t
UCharsTrieBuilder::लिख(ध्रुव char16_t *s, int32_t length) {
    int32_t new_दीर्घता=uchars_दीर्घता+length;
    यदि(ensure_क्षमता(new_दीर्घता)) {
        uchars_दीर्घता=new_दीर्घता;
        u_memcpy(uchars+(uchars_क्षमता-uchars_दीर्घता), s, length);
    }
    प्रत्यागम uchars_दीर्घता;
}

int32_t
UCharsTrieBuilder::writeElementUnits(int32_t i, int32_t unit_सूचकांक, int32_t length) {
    प्रत्यागम लिख(elements[i].getString(वाक्यानि).get_बफर()+unit_सूचकांक, length);
}

int32_t
UCharsTrieBuilder::write_मानम्_And_Final(int32_t i, UBool isFinal) {
    यदि(0<=i && i<=UCharsTrie::k_Max_One_Unit_मानम्) {
        प्रत्यागम लिख(i|(isFinal<<15));
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(i<0 || i>UCharsTrie::k_Max_Two_Unit_मानम्) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_मानम्_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(i) >> 16);
        intUnits[2] = static_cast<char16_t>(i);
        length=3;
    // } else if(i<=UCharsTrie::kMaxOneUnitValue) {
    //     intUnits[0]=(char16_t)(i);
    //     length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_मानम्_Lead + (i >> 16));
        intUnits[1] = static_cast<char16_t>(i);
        length=2;
    }
    intUnits[0] = static_cast<char16_t>(intUnits[0] | (isFinal << 15));
    प्रत्यागम लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_मानम्_And_Type(UBool has_मानम्, int32_t value, int32_t ग्रन्थिः) {
    यदि(!has_मानम्) {
        प्रत्यागम लिख(ग्रन्थिः);
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(value<0 || value>UCharsTrie::k_Max_Two_Unit_ग्रन्थिः_मानम्) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Three_Unit_ग्रन्थिः_मानम्_Lead);
        intUnits[1] = static_cast<char16_t>(static_cast<uint32_t>(value) >> 16);
        intUnits[2] = static_cast<char16_t>(value);
        length=3;
    } अन्यथा यदि(value<=UCharsTrie::k_Max_One_Unit_ग्रन्थिः_मानम्) {
        intUnits[0] = static_cast<char16_t>((value + 1) << 6);
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::k_Min_Two_Unit_ग्रन्थिः_मानम्_Lead + ((value >> 10) & 0x7fc0));
        intUnits[1] = static_cast<char16_t>(value);
        length=2;
    }
    intUnits[0] |= static_cast<char16_t>(ग्रन्थिः);
    प्रत्यागम लिख(intUnits, length);
}

int32_t
UCharsTrieBuilder::write_Delta_गन्तव्यम्(int32_t jumpTarget) {
    int32_t i=uchars_दीर्घता-jumpTarget;
    U_ASSERT(i>=0);
    यदि(i<=UCharsTrie::kMaxOneUnitDelta) {
        प्रत्यागम लिख(i);
    }
    char16_t intUnits[3];
    int32_t length;
    यदि(i<=UCharsTrie::kMaxTwoUnitDelta) {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kMinTwoUnitDeltaLead + (i >> 16));
        length=1;
    } अन्यथा {
        intUnits[0] = static_cast<char16_t>(UCharsTrie::kThreeUnitDeltaLead);
        intUnits[1] = static_cast<char16_t>(i >> 16);
        length=2;
    }
    intUnits[length++] = static_cast<char16_t>(i);
    प्रत्यागम लिख(intUnits, length);
}

U_NAMESPACE_अन्तः
