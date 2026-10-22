// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: October 30 2002
* Since: ICU 2.4
* 2010nov19 Markus Scherer  Rewrite for formatVersion 2.
**********************************************************************
*/
#अन्तर्भाव "propname.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uarrsort.h"
#अन्तर्भाव "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#अन्तर्भाव "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर ओळीत int32_t
getASCIIPropertyNameChar(स्थिरांक अक्षर *name) {
    int32_t i;
    अक्षर c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    साठी(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    जर(c!=0) {
        परत (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अन्यथा {
        परत i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर ओळीत int32_t
getEBCDICPropertyNameChar(स्थिरांक अक्षर *name) {
    int32_t i;
    अक्षर c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    साठी(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    जर(c!=0) {
        परत (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अन्यथा {
        परत i<<8;
    }
}

/**
 * Unicode property names and property value names are compared "loosely".
 *
 * UCD.html 4.0.1 says:
 *   For all property names, property value names, and for property values for
 *   Enumerated, Binary, or Catalog properties, use the following
 *   loose matching rule:
 *
 *   LM3. Ignore case, whitespace, underscore ('_'), and hyphens.
 *
 * This function does just that, for (char *) name strings.
 * It is almost identical to ucnv_compareNames() but also ignores
 * C0 White_Space characters (U+0009..U+000d, and U+0085 on EBCDIC).
 *
 * @internal
 */

U_CAPI int32_t U_EXPORT2
uprv_compareASCIIPropertyNames(स्थिरांक अक्षर *name1, स्थिरांक अक्षर *name2) {
    int32_t rc, r1, r2;

    साठी(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        जर(((r1|r2)&0xff)==0) {
            परत 0;
        }

        /* Compare the lowercased characters */
        जर(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            जर(rc!=0) {
                परत rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(स्थिरांक अक्षर *name1, स्थिरांक अक्षर *name2) {
    int32_t rc, r1, r2;

    साठी(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        जर(((r1|r2)&0xff)==0) {
            परत 0;
        }

        /* Compare the lowercased characters */
        जर(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            जर(rc!=0) {
                परत rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_END

U_NAMESPACE_BEGIN

int32_t PropNameData::findProperty(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    साठी(int32_t अंक_Ranges=मूल्य_Maps[0]; अंक_Ranges>0; --अंक_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मूल्य_Maps[i];
        int32_t limit=मूल्य_Maps[i+1];
        i+=2;
        जर(property<start) {
            थांबा;
        }
        जर(property<limit) {
            परत i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    परत 0;
}

int32_t PropNameData::find_Property_मूल्य_Name_Group(int32_t मूल्य_Map_Index, int32_t value) {
    जर(मूल्य_Map_Index==0) {
        परत 0;  // The property does not have named values.
    }
    ++मूल्य_Map_Index;  // Skip the BytesTrie offset.
    int32_t अंक_Ranges=मूल्य_Maps[मूल्य_Map_Index++];
    जर(अंक_Ranges<0x10) {
        // Ranges of values.
        साठी(; अंक_Ranges>0; --अंक_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मूल्य_Maps[मूल्य_Map_Index];
            int32_t limit=मूल्य_Maps[मूल्य_Map_Index+1];
            मूल्य_Map_Index+=2;
            जर(value<start) {
                थांबा;
            }
            जर(value<limit) {
                परत मूल्य_Maps[मूल्य_Map_Index+value-start];
            }
            मूल्य_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } अन्यथा {
        // List of values.
        int32_t मूल्ये_Start=मूल्य_Map_Index;
        int32_t nameGroupOffsetsStart=मूल्य_Map_Index+अंक_Ranges-0x10;
        करा {
            int32_t v=मूल्य_Maps[मूल्य_Map_Index];
            जर(value<v) {
                थांबा;
            }
            जर(value==v) {
                परत मूल्य_Maps[nameGroupOffsetsStart+मूल्य_Map_Index-मूल्ये_Start];
            }
        } पर्यंत(++मूल्य_Map_Index<nameGroupOffsetsStart);
    }
    परत 0;
}

स्थिरांक अक्षर *PropNameData::getName(स्थिरांक अक्षर *nameGroup, int32_t nameIndex) {
    int32_t अंक_Names=*nameGroup++;
    जर(nameIndex<0 || अंक_Names<=nameIndex) {
        परत nullptr;
    }
    // Skip nameIndex names.
    साठी(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    जर(*nameGroup==0) {
        परत nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    परत nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, स्थिरांक अक्षर *name) {
    जर(name==nullptr) {
        परत असत्य;
    }
    UStringTrieResult result=USTRINGTRIE_NO_मूल्य;
    अक्षर c;
    पर्यंत((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        जर(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            सुरूठेवा;
        }
        जर(!USTRINGTRIE_HAS_NEXT(result)) {
            परत असत्य;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    परत USTRINGTRIE_HAS_मूल्य(result);
}

स्थिरांक अक्षर *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मूल्य_Map_Index=findProperty(property);
    जर(मूल्य_Map_Index==0) {
        परत nullptr;  // Not a known property.
    }
    परत getName(nameGroups+मूल्य_Maps[मूल्य_Map_Index], nameChoice);
}

स्थिरांक अक्षर *PropNameData::get_Property_मूल्य_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मूल्य_Map_Index=findProperty(property);
    जर(मूल्य_Map_Index==0) {
        परत nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_मूल्य_Name_Group(मूल्य_Maps[मूल्य_Map_Index+1], value);
    जर(nameGroupOffset==0) {
        परत nullptr;
    }
    परत getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_मूल्य_Enum(int32_t bytesTrieOffset, स्थिरांक अक्षर *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    जर(containsName(trie, alias)) {
        परत trie.get_मूल्य();
    } अन्यथा {
        परत UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(स्थिरांक अक्षर *alias) {
    परत get_Property_Or_मूल्य_Enum(0, alias);
}

int32_t PropNameData::get_Property_मूल्य_Enum(int32_t property, स्थिरांक अक्षर *alias) {
    int32_t मूल्य_Map_Index=findProperty(property);
    जर(मूल्य_Map_Index==0) {
        परत UCHAR_INVALID_CODE;  // Not a known property.
    }
    मूल्य_Map_Index=मूल्य_Maps[मूल्य_Map_Index+1];
    जर(मूल्य_Map_Index==0) {
        परत UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    परत get_Property_Or_मूल्य_Enum(मूल्य_Maps[मूल्य_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI स्थिरांक अक्षर* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    परत PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(स्थिरांक अक्षर* alias) {
    U_NAMESPACE_USE
    परत (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI स्थिरांक अक्षर* U_EXPORT2
u_get_Property_मूल्य_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    परत PropNameData::get_Property_मूल्य_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मूल्य_Enum(UProperty property,
                       स्थिरांक अक्षर* alias) {
    U_NAMESPACE_USE
    परत PropNameData::get_Property_मूल्य_Enum(property, alias);
}

U_CAPI स्थिरांक अक्षर*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    परत u_get_Property_मूल्य_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI स्थिरांक अक्षर*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    परत u_get_Property_मूल्य_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
