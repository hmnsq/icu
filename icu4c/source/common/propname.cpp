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
स्थिर अन्तःस्थ int32_t
getASCIIPropertyNameChar(अचर वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    लागि(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    यदि(c!=0) {
        फर्काऊ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अन्यथा {
        फर्काऊ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर अन्तःस्थ int32_t
getEBCDICPropertyNameChar(अचर वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    लागि(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    यदि(c!=0) {
        फर्काऊ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अन्यथा {
        फर्काऊ i<<8;
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
uprv_compareASCIIPropertyNames(अचर वर्ण *name1, अचर वर्ण *name2) {
    int32_t rc, r1, r2;

    लागि(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        यदि(((r1|r2)&0xff)==0) {
            फर्काऊ 0;
        }

        /* Compare the lowercased characters */
        यदि(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            यदि(rc!=0) {
                फर्काऊ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(अचर वर्ण *name1, अचर वर्ण *name2) {
    int32_t rc, r1, r2;

    लागि(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        यदि(((r1|r2)&0xff)==0) {
            फर्काऊ 0;
        }

        /* Compare the lowercased characters */
        यदि(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            यदि(rc!=0) {
                फर्काऊ rc;
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
    लागि(int32_t अङ्क_Ranges=मान_Maps[0]; अङ्क_Ranges>0; --अङ्क_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मान_Maps[i];
        int32_t limit=मान_Maps[i+1];
        i+=2;
        यदि(property<start) {
            रोक;
        }
        यदि(property<limit) {
            फर्काऊ i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    फर्काऊ 0;
}

int32_t PropNameData::find_Property_मान_Name_Group(int32_t मान_Map_Index, int32_t value) {
    यदि(मान_Map_Index==0) {
        फर्काऊ 0;  // The property does not have named values.
    }
    ++मान_Map_Index;  // Skip the BytesTrie offset.
    int32_t अङ्क_Ranges=मान_Maps[मान_Map_Index++];
    यदि(अङ्क_Ranges<0x10) {
        // Ranges of values.
        लागि(; अङ्क_Ranges>0; --अङ्क_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मान_Maps[मान_Map_Index];
            int32_t limit=मान_Maps[मान_Map_Index+1];
            मान_Map_Index+=2;
            यदि(value<start) {
                रोक;
            }
            यदि(value<limit) {
                फर्काऊ मान_Maps[मान_Map_Index+value-start];
            }
            मान_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } अन्यथा {
        // List of values.
        int32_t मानहरू_Start=मान_Map_Index;
        int32_t nameGroupOffsetsStart=मान_Map_Index+अङ्क_Ranges-0x10;
        गर {
            int32_t v=मान_Maps[मान_Map_Index];
            यदि(value<v) {
                रोक;
            }
            यदि(value==v) {
                फर्काऊ मान_Maps[nameGroupOffsetsStart+मान_Map_Index-मानहरू_Start];
            }
        } जबसम्म(++मान_Map_Index<nameGroupOffsetsStart);
    }
    फर्काऊ 0;
}

अचर वर्ण *PropNameData::getName(अचर वर्ण *nameGroup, int32_t nameIndex) {
    int32_t अङ्क_Names=*nameGroup++;
    यदि(nameIndex<0 || अङ्क_Names<=nameIndex) {
        फर्काऊ nullptr;
    }
    // Skip nameIndex names.
    लागि(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    यदि(*nameGroup==0) {
        फर्काऊ nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    फर्काऊ nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, अचर वर्ण *name) {
    यदि(name==nullptr) {
        फर्काऊ असत्य;
    }
    UStringTrieResult result=USTRINGTRIE_NO_मान;
    वर्ण c;
    जबसम्म((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        यदि(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            जारी;
        }
        यदि(!USTRINGTRIE_HAS_NEXT(result)) {
            फर्काऊ असत्य;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    फर्काऊ USTRINGTRIE_HAS_मान(result);
}

अचर वर्ण *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मान_Map_Index=findProperty(property);
    यदि(मान_Map_Index==0) {
        फर्काऊ nullptr;  // Not a known property.
    }
    फर्काऊ getName(nameGroups+मान_Maps[मान_Map_Index], nameChoice);
}

अचर वर्ण *PropNameData::get_Property_मान_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मान_Map_Index=findProperty(property);
    यदि(मान_Map_Index==0) {
        फर्काऊ nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_मान_Name_Group(मान_Maps[मान_Map_Index+1], value);
    यदि(nameGroupOffset==0) {
        फर्काऊ nullptr;
    }
    फर्काऊ getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_मान_Enum(int32_t bytesTrieOffset, अचर वर्ण *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    यदि(containsName(trie, alias)) {
        फर्काऊ trie.get_मान();
    } अन्यथा {
        फर्काऊ UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(अचर वर्ण *alias) {
    फर्काऊ get_Property_Or_मान_Enum(0, alias);
}

int32_t PropNameData::get_Property_मान_Enum(int32_t property, अचर वर्ण *alias) {
    int32_t मान_Map_Index=findProperty(property);
    यदि(मान_Map_Index==0) {
        फर्काऊ UCHAR_INVALID_CODE;  // Not a known property.
    }
    मान_Map_Index=मान_Maps[मान_Map_Index+1];
    यदि(मान_Map_Index==0) {
        फर्काऊ UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    फर्काऊ get_Property_Or_मान_Enum(मान_Maps[मान_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI अचर वर्ण* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    फर्काऊ PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(अचर वर्ण* alias) {
    U_NAMESPACE_USE
    फर्काऊ (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI अचर वर्ण* U_EXPORT2
u_get_Property_मान_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    फर्काऊ PropNameData::get_Property_मान_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मान_Enum(UProperty property,
                       अचर वर्ण* alias) {
    U_NAMESPACE_USE
    फर्काऊ PropNameData::get_Property_मान_Enum(property, alias);
}

U_CAPI अचर वर्ण*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    फर्काऊ u_get_Property_मान_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI अचर वर्ण*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    फर्काऊ u_get_Property_मान_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
