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
#അന്തര്ഭാവ "propname.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uarrsort.h"
#അന്തര്ഭാവ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#അന്തര്ഭാവ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
സ്ഥിതം വരിയിൽ int32_t
getASCIIPropertyNameChar(സ്ഥിരം അക്ഷരം *name) {
    int32_t i;
    അക്ഷരം c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    വേണ്ടി(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    എങ്കിൽ(c!=0) {
        തിരികെ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } അല്ലെങ്കിൽ {
        തിരികെ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
സ്ഥിതം വരിയിൽ int32_t
getEBCDICPropertyNameChar(സ്ഥിരം അക്ഷരം *name) {
    int32_t i;
    അക്ഷരം c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    വേണ്ടി(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    എങ്കിൽ(c!=0) {
        തിരികെ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } അല്ലെങ്കിൽ {
        തിരികെ i<<8;
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
uprv_compareASCIIPropertyNames(സ്ഥിരം അക്ഷരം *name1, സ്ഥിരം അക്ഷരം *name2) {
    int32_t rc, r1, r2;

    വേണ്ടി(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        എങ്കിൽ(((r1|r2)&0xff)==0) {
            തിരികെ 0;
        }

        /* Compare the lowercased characters */
        എങ്കിൽ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            എങ്കിൽ(rc!=0) {
                തിരികെ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(സ്ഥിരം അക്ഷരം *name1, സ്ഥിരം അക്ഷരം *name2) {
    int32_t rc, r1, r2;

    വേണ്ടി(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        എങ്കിൽ(((r1|r2)&0xff)==0) {
            തിരികെ 0;
        }

        /* Compare the lowercased characters */
        എങ്കിൽ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            എങ്കിൽ(rc!=0) {
                തിരികെ rc;
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
    വേണ്ടി(int32_t അക്കം_Ranges=മൂല്യം_Maps[0]; അക്കം_Ranges>0; --അക്കം_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=മൂല്യം_Maps[i];
        int32_t limit=മൂല്യം_Maps[i+1];
        i+=2;
        എങ്കിൽ(property<start) {
            നിർത്തുക;
        }
        എങ്കിൽ(property<limit) {
            തിരികെ i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    തിരികെ 0;
}

int32_t PropNameData::find_Property_മൂല്യം_Name_Group(int32_t മൂല്യം_Map_Index, int32_t value) {
    എങ്കിൽ(മൂല്യം_Map_Index==0) {
        തിരികെ 0;  // The property does not have named values.
    }
    ++മൂല്യം_Map_Index;  // Skip the BytesTrie offset.
    int32_t അക്കം_Ranges=മൂല്യം_Maps[മൂല്യം_Map_Index++];
    എങ്കിൽ(അക്കം_Ranges<0x10) {
        // Ranges of values.
        വേണ്ടി(; അക്കം_Ranges>0; --അക്കം_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=മൂല്യം_Maps[മൂല്യം_Map_Index];
            int32_t limit=മൂല്യം_Maps[മൂല്യം_Map_Index+1];
            മൂല്യം_Map_Index+=2;
            എങ്കിൽ(value<start) {
                നിർത്തുക;
            }
            എങ്കിൽ(value<limit) {
                തിരികെ മൂല്യം_Maps[മൂല്യം_Map_Index+value-start];
            }
            മൂല്യം_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } അല്ലെങ്കിൽ {
        // List of values.
        int32_t മൂല്യങ്ങൾ_Start=മൂല്യം_Map_Index;
        int32_t nameGroupOffsetsStart=മൂല്യം_Map_Index+അക്കം_Ranges-0x10;
        ചെയ്യുക {
            int32_t v=മൂല്യം_Maps[മൂല്യം_Map_Index];
            എങ്കിൽ(value<v) {
                നിർത്തുക;
            }
            എങ്കിൽ(value==v) {
                തിരികെ മൂല്യം_Maps[nameGroupOffsetsStart+മൂല്യം_Map_Index-മൂല്യങ്ങൾ_Start];
            }
        } ആകുമ്പോൾ(++മൂല്യം_Map_Index<nameGroupOffsetsStart);
    }
    തിരികെ 0;
}

സ്ഥിരം അക്ഷരം *PropNameData::getName(സ്ഥിരം അക്ഷരം *nameGroup, int32_t nameIndex) {
    int32_t അക്കം_Names=*nameGroup++;
    എങ്കിൽ(nameIndex<0 || അക്കം_Names<=nameIndex) {
        തിരികെ nullptr;
    }
    // Skip nameIndex names.
    വേണ്ടി(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    എങ്കിൽ(*nameGroup==0) {
        തിരികെ nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    തിരികെ nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, സ്ഥിരം അക്ഷരം *name) {
    എങ്കിൽ(name==nullptr) {
        തിരികെ അസത്യം;
    }
    UStringTrieResult result=USTRINGTRIE_NO_മൂല്യം;
    അക്ഷരം c;
    ആകുമ്പോൾ((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        എങ്കിൽ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            തുടരുക;
        }
        എങ്കിൽ(!USTRINGTRIE_HAS_NEXT(result)) {
            തിരികെ അസത്യം;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    തിരികെ USTRINGTRIE_HAS_മൂല്യം(result);
}

സ്ഥിരം അക്ഷരം *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t മൂല്യം_Map_Index=findProperty(property);
    എങ്കിൽ(മൂല്യം_Map_Index==0) {
        തിരികെ nullptr;  // Not a known property.
    }
    തിരികെ getName(nameGroups+മൂല്യം_Maps[മൂല്യം_Map_Index], nameChoice);
}

സ്ഥിരം അക്ഷരം *PropNameData::get_Property_മൂല്യം_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t മൂല്യം_Map_Index=findProperty(property);
    എങ്കിൽ(മൂല്യം_Map_Index==0) {
        തിരികെ nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_മൂല്യം_Name_Group(മൂല്യം_Maps[മൂല്യം_Map_Index+1], value);
    എങ്കിൽ(nameGroupOffset==0) {
        തിരികെ nullptr;
    }
    തിരികെ getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_മൂല്യം_Enum(int32_t bytesTrieOffset, സ്ഥിരം അക്ഷരം *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    എങ്കിൽ(containsName(trie, alias)) {
        തിരികെ trie.get_മൂല്യം();
    } അല്ലെങ്കിൽ {
        തിരികെ UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(സ്ഥിരം അക്ഷരം *alias) {
    തിരികെ get_Property_Or_മൂല്യം_Enum(0, alias);
}

int32_t PropNameData::get_Property_മൂല്യം_Enum(int32_t property, സ്ഥിരം അക്ഷരം *alias) {
    int32_t മൂല്യം_Map_Index=findProperty(property);
    എങ്കിൽ(മൂല്യം_Map_Index==0) {
        തിരികെ UCHAR_INVALID_CODE;  // Not a known property.
    }
    മൂല്യം_Map_Index=മൂല്യം_Maps[മൂല്യം_Map_Index+1];
    എങ്കിൽ(മൂല്യം_Map_Index==0) {
        തിരികെ UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    തിരികെ get_Property_Or_മൂല്യം_Enum(മൂല്യം_Maps[മൂല്യം_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    തിരികെ PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(സ്ഥിരം അക്ഷരം* alias) {
    U_NAMESPACE_USE
    തിരികെ (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI സ്ഥിരം അക്ഷരം* U_EXPORT2
u_get_Property_മൂല്യം_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    തിരികെ PropNameData::get_Property_മൂല്യം_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_മൂല്യം_Enum(UProperty property,
                       സ്ഥിരം അക്ഷരം* alias) {
    U_NAMESPACE_USE
    തിരികെ PropNameData::get_Property_മൂല്യം_Enum(property, alias);
}

U_CAPI സ്ഥിരം അക്ഷരം*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    തിരികെ u_get_Property_മൂല്യം_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI സ്ഥിരം അക്ഷരം*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    തിരികെ u_get_Property_മൂല്യം_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
