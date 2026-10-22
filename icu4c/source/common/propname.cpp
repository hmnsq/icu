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
getASCIIPropertyNameChar(ध्रुव वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    अत्थाय(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    सचे(c!=0) {
        पटिनिवत्त (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अञ्ञथा {
        पटिनिवत्त i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
स्थिर अन्तःस्थ int32_t
getEBCDICPropertyNameChar(ध्रुव वर्ण *name) {
    int32_t i;
    वर्ण c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    अत्थाय(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    सचे(c!=0) {
        पटिनिवत्त (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अञ्ञथा {
        पटिनिवत्त i<<8;
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
uprv_compareASCIIPropertyNames(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    int32_t rc, r1, r2;

    अत्थाय(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        सचे(((r1|r2)&0xff)==0) {
            पटिनिवत्त 0;
        }

        /* Compare the lowercased characters */
        सचे(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            सचे(rc!=0) {
                पटिनिवत्त rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    int32_t rc, r1, r2;

    अत्थाय(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        सचे(((r1|r2)&0xff)==0) {
            पटिनिवत्त 0;
        }

        /* Compare the lowercased characters */
        सचे(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            सचे(rc!=0) {
                पटिनिवत्त rc;
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
    अत्थाय(int32_t अङ्कः_Ranges=मानम्_Maps[0]; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मानम्_Maps[i];
        int32_t limit=मानम्_Maps[i+1];
        i+=2;
        सचे(property<start) {
            भङ्ग;
        }
        सचे(property<limit) {
            पटिनिवत्त i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    पटिनिवत्त 0;
}

int32_t PropNameData::find_Property_मानम्_Name_Group(int32_t मानम्_Map_Index, int32_t value) {
    सचे(मानम्_Map_Index==0) {
        पटिनिवत्त 0;  // The property does not have named values.
    }
    ++मानम्_Map_Index;  // Skip the BytesTrie offset.
    int32_t अङ्कः_Ranges=मानम्_Maps[मानम्_Map_Index++];
    सचे(अङ्कः_Ranges<0x10) {
        // Ranges of values.
        अत्थाय(; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मानम्_Maps[मानम्_Map_Index];
            int32_t limit=मानम्_Maps[मानम्_Map_Index+1];
            मानम्_Map_Index+=2;
            सचे(value<start) {
                भङ्ग;
            }
            सचे(value<limit) {
                पटिनिवत्त मानम्_Maps[मानम्_Map_Index+value-start];
            }
            मानम्_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } अञ्ञथा {
        // List of values.
        int32_t मानानि_Start=मानम्_Map_Index;
        int32_t nameGroupOffsetsStart=मानम्_Map_Index+अङ्कः_Ranges-0x10;
        करोहि {
            int32_t v=मानम्_Maps[मानम्_Map_Index];
            सचे(value<v) {
                भङ्ग;
            }
            सचे(value==v) {
                पटिनिवत्त मानम्_Maps[nameGroupOffsetsStart+मानम्_Map_Index-मानानि_Start];
            }
        } याव(++मानम्_Map_Index<nameGroupOffsetsStart);
    }
    पटिनिवत्त 0;
}

ध्रुव वर्ण *PropNameData::getName(ध्रुव वर्ण *nameGroup, int32_t nameIndex) {
    int32_t अङ्कः_Names=*nameGroup++;
    सचे(nameIndex<0 || अङ्कः_Names<=nameIndex) {
        पटिनिवत्त nullptr;
    }
    // Skip nameIndex names.
    अत्थाय(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    सचे(*nameGroup==0) {
        पटिनिवत्त nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    पटिनिवत्त nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ध्रुव वर्ण *name) {
    सचे(name==nullptr) {
        पटिनिवत्त मिच्छा;
    }
    UStringTrieResult result=USTRINGTRIE_NO_मानम्;
    वर्ण c;
    याव((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        सचे(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            अनुवर्त;
        }
        सचे(!USTRINGTRIE_HAS_NEXT(result)) {
            पटिनिवत्त मिच्छा;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    पटिनिवत्त USTRINGTRIE_HAS_मानम्(result);
}

ध्रुव वर्ण *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मानम्_Map_Index=findProperty(property);
    सचे(मानम्_Map_Index==0) {
        पटिनिवत्त nullptr;  // Not a known property.
    }
    पटिनिवत्त getName(nameGroups+मानम्_Maps[मानम्_Map_Index], nameChoice);
}

ध्रुव वर्ण *PropNameData::get_Property_मानम्_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मानम्_Map_Index=findProperty(property);
    सचे(मानम्_Map_Index==0) {
        पटिनिवत्त nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_मानम्_Name_Group(मानम्_Maps[मानम्_Map_Index+1], value);
    सचे(nameGroupOffset==0) {
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_मानम्_Enum(int32_t bytesTrieOffset, ध्रुव वर्ण *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    सचे(containsName(trie, alias)) {
        पटिनिवत्त trie.get_मानम्();
    } अञ्ञथा {
        पटिनिवत्त UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ध्रुव वर्ण *alias) {
    पटिनिवत्त get_Property_Or_मानम्_Enum(0, alias);
}

int32_t PropNameData::get_Property_मानम्_Enum(int32_t property, ध्रुव वर्ण *alias) {
    int32_t मानम्_Map_Index=findProperty(property);
    सचे(मानम्_Map_Index==0) {
        पटिनिवत्त UCHAR_INVALID_CODE;  // Not a known property.
    }
    मानम्_Map_Index=मानम्_Maps[मानम्_Map_Index+1];
    सचे(मानम्_Map_Index==0) {
        पटिनिवत्त UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    पटिनिवत्त get_Property_Or_मानम्_Enum(मानम्_Maps[मानम्_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ध्रुव वर्ण* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    पटिनिवत्त PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    पटिनिवत्त (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
u_get_Property_मानम्_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    पटिनिवत्त PropNameData::get_Property_मानम्_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मानम्_Enum(UProperty property,
                       ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    पटिनिवत्त PropNameData::get_Property_मानम्_Enum(property, alias);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    पटिनिवत्त u_get_Property_मानम्_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    पटिनिवत्त u_get_Property_मानम्_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
