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
    प्रति(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    जइ(c!=0) {
        प्रत्यागम (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } अण्णहा {
        प्रत्यागम i<<8;
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
    प्रति(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    जइ(c!=0) {
        प्रत्यागम (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } अण्णहा {
        प्रत्यागम i<<8;
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

    प्रति(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        जइ(((r1|r2)&0xff)==0) {
            प्रत्यागम 0;
        }

        /* Compare the lowercased characters */
        जइ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            जइ(rc!=0) {
                प्रत्यागम rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ध्रुव वर्ण *name1, ध्रुव वर्ण *name2) {
    int32_t rc, r1, r2;

    प्रति(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        जइ(((r1|r2)&0xff)==0) {
            प्रत्यागम 0;
        }

        /* Compare the lowercased characters */
        जइ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            जइ(rc!=0) {
                प्रत्यागम rc;
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
    प्रति(int32_t अङ्कः_Ranges=मानम्_Maps[0]; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=मानम्_Maps[i];
        int32_t limit=मानम्_Maps[i+1];
        i+=2;
        जइ(property<start) {
            भङ्ग;
        }
        जइ(property<limit) {
            प्रत्यागम i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    प्रत्यागम 0;
}

int32_t PropNameData::find_Property_मानम्_Name_Group(int32_t मानम्_Map_Index, int32_t value) {
    जइ(मानम्_Map_Index==0) {
        प्रत्यागम 0;  // The property does not have named values.
    }
    ++मानम्_Map_Index;  // Skip the BytesTrie offset.
    int32_t अङ्कः_Ranges=मानम्_Maps[मानम्_Map_Index++];
    जइ(अङ्कः_Ranges<0x10) {
        // Ranges of values.
        प्रति(; अङ्कः_Ranges>0; --अङ्कः_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=मानम्_Maps[मानम्_Map_Index];
            int32_t limit=मानम्_Maps[मानम्_Map_Index+1];
            मानम्_Map_Index+=2;
            जइ(value<start) {
                भङ्ग;
            }
            जइ(value<limit) {
                प्रत्यागम मानम्_Maps[मानम्_Map_Index+value-start];
            }
            मानम्_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } अण्णहा {
        // List of values.
        int32_t मानानि_Start=मानम्_Map_Index;
        int32_t nameGroupOffsetsStart=मानम्_Map_Index+अङ्कः_Ranges-0x10;
        करेहि {
            int32_t v=मानम्_Maps[मानम्_Map_Index];
            जइ(value<v) {
                भङ्ग;
            }
            जइ(value==v) {
                प्रत्यागम मानम्_Maps[nameGroupOffsetsStart+मानम्_Map_Index-मानानि_Start];
            }
        } जाव(++मानम्_Map_Index<nameGroupOffsetsStart);
    }
    प्रत्यागम 0;
}

ध्रुव वर्ण *PropNameData::getName(ध्रुव वर्ण *nameGroup, int32_t nameIndex) {
    int32_t अङ्कः_Names=*nameGroup++;
    जइ(nameIndex<0 || अङ्कः_Names<=nameIndex) {
        प्रत्यागम nullptr;
    }
    // Skip nameIndex names.
    प्रति(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    जइ(*nameGroup==0) {
        प्रत्यागम nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    प्रत्यागम nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ध्रुव वर्ण *name) {
    जइ(name==nullptr) {
        प्रत्यागम मिच्छ;
    }
    UStringTrieResult result=USTRINGTRIE_NO_मानम्;
    वर्ण c;
    जाव((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        जइ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            अनुवर्त;
        }
        जइ(!USTRINGTRIE_HAS_NEXT(result)) {
            प्रत्यागम मिच्छ;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    प्रत्यागम USTRINGTRIE_HAS_मानम्(result);
}

ध्रुव वर्ण *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t मानम्_Map_Index=findProperty(property);
    जइ(मानम्_Map_Index==0) {
        प्रत्यागम nullptr;  // Not a known property.
    }
    प्रत्यागम getName(nameGroups+मानम्_Maps[मानम्_Map_Index], nameChoice);
}

ध्रुव वर्ण *PropNameData::get_Property_मानम्_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t मानम्_Map_Index=findProperty(property);
    जइ(मानम्_Map_Index==0) {
        प्रत्यागम nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_मानम्_Name_Group(मानम्_Maps[मानम्_Map_Index+1], value);
    जइ(nameGroupOffset==0) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_मानम्_Enum(int32_t bytesTrieOffset, ध्रुव वर्ण *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    जइ(containsName(trie, alias)) {
        प्रत्यागम trie.get_मानम्();
    } अण्णहा {
        प्रत्यागम UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ध्रुव वर्ण *alias) {
    प्रत्यागम get_Property_Or_मानम्_Enum(0, alias);
}

int32_t PropNameData::get_Property_मानम्_Enum(int32_t property, ध्रुव वर्ण *alias) {
    int32_t मानम्_Map_Index=findProperty(property);
    जइ(मानम्_Map_Index==0) {
        प्रत्यागम UCHAR_INVALID_CODE;  // Not a known property.
    }
    मानम्_Map_Index=मानम्_Maps[मानम्_Map_Index+1];
    जइ(मानम्_Map_Index==0) {
        प्रत्यागम UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    प्रत्यागम get_Property_Or_मानम्_Enum(मानम्_Maps[मानम्_Map_Index], alias);
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
    प्रत्यागम PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    प्रत्यागम (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
u_get_Property_मानम्_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    प्रत्यागम PropNameData::get_Property_मानम्_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_मानम्_Enum(UProperty property,
                       ध्रुव वर्ण* alias) {
    U_NAMESPACE_USE
    प्रत्यागम PropNameData::get_Property_मानम्_Enum(property, alias);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    प्रत्यागम u_get_Property_मानम्_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ध्रुव वर्ण*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    प्रत्यागम u_get_Property_मानम्_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
