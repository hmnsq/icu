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
#ಅನ್ತರ್ಭಾವ "propname.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#ಅನ್ತರ್ಭಾವ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ int32_t
getASCIIPropertyNameChar(ಧ್ರುವ ವರ್ಣ *name) {
    int32_t i;
    ವರ್ಣ c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    ಪ್ರತಿ(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ಯದಿ(c!=0) {
        ಪ್ರತ್ಯಾಗಮ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ int32_t
getEBCDICPropertyNameChar(ಧ್ರುವ ವರ್ಣ *name) {
    int32_t i;
    ವರ್ಣ c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    ಪ್ರತಿ(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ಯದಿ(c!=0) {
        ಪ್ರತ್ಯಾಗಮ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ i<<8;
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
uprv_compareASCIIPropertyNames(ಧ್ರುವ ವರ್ಣ *name1, ಧ್ರುವ ವರ್ಣ *name2) {
    int32_t rc, r1, r2;

    ಪ್ರತಿ(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ಯದಿ(((r1|r2)&0xff)==0) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Compare the lowercased characters */
        ಯದಿ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ಯದಿ(rc!=0) {
                ಪ್ರತ್ಯಾಗಮ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ಧ್ರುವ ವರ್ಣ *name1, ಧ್ರುವ ವರ್ಣ *name2) {
    int32_t rc, r1, r2;

    ಪ್ರತಿ(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ಯದಿ(((r1|r2)&0xff)==0) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Compare the lowercased characters */
        ಯದಿ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ಯದಿ(rc!=0) {
                ಪ್ರತ್ಯಾಗಮ rc;
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
    ಪ್ರತಿ(int32_t ಅಂಕೆ_Ranges=ಮೌಲ್ಯ_Maps[0]; ಅಂಕೆ_Ranges>0; --ಅಂಕೆ_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=ಮೌಲ್ಯ_Maps[i];
        int32_t limit=ಮೌಲ್ಯ_Maps[i+1];
        i+=2;
        ಯದಿ(property<start) {
            ಭಙ್ಗ;
        }
        ಯದಿ(property<limit) {
            ಪ್ರತ್ಯಾಗಮ i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

int32_t PropNameData::find_Property_ಮೌಲ್ಯ_Name_Group(int32_t ಮೌಲ್ಯ_Map_Index, int32_t value) {
    ಯದಿ(ಮೌಲ್ಯ_Map_Index==0) {
        ಪ್ರತ್ಯಾಗಮ 0;  // The property does not have named values.
    }
    ++ಮೌಲ್ಯ_Map_Index;  // Skip the BytesTrie offset.
    int32_t ಅಂಕೆ_Ranges=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index++];
    ಯದಿ(ಅಂಕೆ_Ranges<0x10) {
        // Ranges of values.
        ಪ್ರತಿ(; ಅಂಕೆ_Ranges>0; --ಅಂಕೆ_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index];
            int32_t limit=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index+1];
            ಮೌಲ್ಯ_Map_Index+=2;
            ಯದಿ(value<start) {
                ಭಙ್ಗ;
            }
            ಯದಿ(value<limit) {
                ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index+value-start];
            }
            ಮೌಲ್ಯ_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } ಅನ್ಯಥಾ {
        // List of values.
        int32_t ಮೌಲ್ಯಗಳು_Start=ಮೌಲ್ಯ_Map_Index;
        int32_t nameGroupOffsetsStart=ಮೌಲ್ಯ_Map_Index+ಅಂಕೆ_Ranges-0x10;
        ಕುರು {
            int32_t v=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index];
            ಯದಿ(value<v) {
                ಭಙ್ಗ;
            }
            ಯದಿ(value==v) {
                ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ_Maps[nameGroupOffsetsStart+ಮೌಲ್ಯ_Map_Index-ಮೌಲ್ಯಗಳು_Start];
            }
        } ಯಾವತ್(++ಮೌಲ್ಯ_Map_Index<nameGroupOffsetsStart);
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ ವರ್ಣ *PropNameData::getName(ಧ್ರುವ ವರ್ಣ *nameGroup, int32_t nameIndex) {
    int32_t ಅಂಕೆ_Names=*nameGroup++;
    ಯದಿ(nameIndex<0 || ಅಂಕೆ_Names<=nameIndex) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    // Skip nameIndex names.
    ಪ್ರತಿ(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    ಯದಿ(*nameGroup==0) {
        ಪ್ರತ್ಯಾಗಮ nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ಪ್ರತ್ಯಾಗಮ nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ಧ್ರುವ ವರ್ಣ *name) {
    ಯದಿ(name==nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    UStringTrieResult result=USTRINGTRIE_NO_ಮೌಲ್ಯ;
    ವರ್ಣ c;
    ಯಾವತ್((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ಯದಿ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ಅನುವರ್ತ;
        }
        ಯದಿ(!USTRINGTRIE_HAS_NEXT(result)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    ಪ್ರತ್ಯಾಗಮ USTRINGTRIE_HAS_ಮೌಲ್ಯ(result);
}

ಧ್ರುವ ವರ್ಣ *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t ಮೌಲ್ಯ_Map_Index=findProperty(property);
    ಯದಿ(ಮೌಲ್ಯ_Map_Index==0) {
        ಪ್ರತ್ಯಾಗಮ nullptr;  // Not a known property.
    }
    ಪ್ರತ್ಯಾಗಮ getName(nameGroups+ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index], nameChoice);
}

ಧ್ರುವ ವರ್ಣ *PropNameData::get_Property_ಮೌಲ್ಯ_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t ಮೌಲ್ಯ_Map_Index=findProperty(property);
    ಯದಿ(ಮೌಲ್ಯ_Map_Index==0) {
        ಪ್ರತ್ಯಾಗಮ nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_ಮೌಲ್ಯ_Name_Group(ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index+1], value);
    ಯದಿ(nameGroupOffset==0) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_ಮೌಲ್ಯ_Enum(int32_t bytesTrieOffset, ಧ್ರುವ ವರ್ಣ *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    ಯದಿ(containsName(trie, alias)) {
        ಪ್ರತ್ಯಾಗಮ trie.get_ಮೌಲ್ಯ();
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ಧ್ರುವ ವರ್ಣ *alias) {
    ಪ್ರತ್ಯಾಗಮ get_Property_Or_ಮೌಲ್ಯ_Enum(0, alias);
}

int32_t PropNameData::get_Property_ಮೌಲ್ಯ_Enum(int32_t property, ಧ್ರುವ ವರ್ಣ *alias) {
    int32_t ಮೌಲ್ಯ_Map_Index=findProperty(property);
    ಯದಿ(ಮೌಲ್ಯ_Map_Index==0) {
        ಪ್ರತ್ಯಾಗಮ UCHAR_INVALID_CODE;  // Not a known property.
    }
    ಮೌಲ್ಯ_Map_Index=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index+1];
    ಯದಿ(ಮೌಲ್ಯ_Map_Index==0) {
        ಪ್ರತ್ಯಾಗಮ UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ಪ್ರತ್ಯಾಗಮ get_Property_Or_ಮೌಲ್ಯ_Enum(ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ಪ್ರತ್ಯಾಗಮ PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ಧ್ರುವ ವರ್ಣ* alias) {
    U_NAMESPACE_USE
    ಪ್ರತ್ಯಾಗಮ (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ಧ್ರುವ ವರ್ಣ* U_EXPORT2
u_get_Property_ಮೌಲ್ಯ_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ಪ್ರತ್ಯಾಗಮ PropNameData::get_Property_ಮೌಲ್ಯ_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_ಮೌಲ್ಯ_Enum(UProperty property,
                       ಧ್ರುವ ವರ್ಣ* alias) {
    U_NAMESPACE_USE
    ಪ್ರತ್ಯಾಗಮ PropNameData::get_Property_ಮೌಲ್ಯ_Enum(property, alias);
}

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    ಪ್ರತ್ಯಾಗಮ u_get_Property_ಮೌಲ್ಯ_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ಧ್ರುವ ವರ್ಣ*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    ಪ್ರತ್ಯಾಗಮ u_get_Property_ಮೌಲ್ಯ_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
