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
#ضمن "propname.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/uscript.h"
#ضمن "umutex.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "uarrsort.h"
#ضمن "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#ضمن "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ساكن مضمن int32_t
getASCIIPropertyNameChar(ثابت حرف *name) {
    int32_t i;
    حرف c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    لكل(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    إذا(c!=0) {
        إرجاع (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } وإلا {
        إرجاع i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ساكن مضمن int32_t
getEBCDICPropertyNameChar(ثابت حرف *name) {
    int32_t i;
    حرف c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    لكل(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    إذا(c!=0) {
        إرجاع (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } وإلا {
        إرجاع i<<8;
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
uprv_compareASCIIPropertyNames(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    لكل(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        إذا(((r1|r2)&0xff)==0) {
            إرجاع 0;
        }

        /* Compare the lowercased characters */
        إذا(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            إذا(rc!=0) {
                إرجاع rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    لكل(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        إذا(((r1|r2)&0xff)==0) {
            إرجاع 0;
        }

        /* Compare the lowercased characters */
        إذا(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            إذا(rc!=0) {
                إرجاع rc;
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
    لكل(int32_t رقم_Ranges=قيمة_Maps[0]; رقم_Ranges>0; --رقم_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=قيمة_Maps[i];
        int32_t limit=قيمة_Maps[i+1];
        i+=2;
        إذا(property<start) {
            اكسر;
        }
        إذا(property<limit) {
            إرجاع i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    إرجاع 0;
}

int32_t PropNameData::find_Property_قيمة_Name_Group(int32_t قيمة_Map_Index, int32_t value) {
    إذا(قيمة_Map_Index==0) {
        إرجاع 0;  // The property does not have named values.
    }
    ++قيمة_Map_Index;  // Skip the BytesTrie offset.
    int32_t رقم_Ranges=قيمة_Maps[قيمة_Map_Index++];
    إذا(رقم_Ranges<0x10) {
        // Ranges of values.
        لكل(; رقم_Ranges>0; --رقم_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=قيمة_Maps[قيمة_Map_Index];
            int32_t limit=قيمة_Maps[قيمة_Map_Index+1];
            قيمة_Map_Index+=2;
            إذا(value<start) {
                اكسر;
            }
            إذا(value<limit) {
                إرجاع قيمة_Maps[قيمة_Map_Index+value-start];
            }
            قيمة_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } وإلا {
        // List of values.
        int32_t قيم_Start=قيمة_Map_Index;
        int32_t nameGroupOffsetsStart=قيمة_Map_Index+رقم_Ranges-0x10;
        افعل {
            int32_t v=قيمة_Maps[قيمة_Map_Index];
            إذا(value<v) {
                اكسر;
            }
            إذا(value==v) {
                إرجاع قيمة_Maps[nameGroupOffsetsStart+قيمة_Map_Index-قيم_Start];
            }
        } بينما(++قيمة_Map_Index<nameGroupOffsetsStart);
    }
    إرجاع 0;
}

ثابت حرف *PropNameData::getName(ثابت حرف *nameGroup, int32_t nameIndex) {
    int32_t رقم_Names=*nameGroup++;
    إذا(nameIndex<0 || رقم_Names<=nameIndex) {
        إرجاع nullptr;
    }
    // Skip nameIndex names.
    لكل(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    إذا(*nameGroup==0) {
        إرجاع nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    إرجاع nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ثابت حرف *name) {
    إذا(name==nullptr) {
        إرجاع خطأ;
    }
    UStringTrieResult result=USTRINGTRIE_NO_قيمة;
    حرف c;
    بينما((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        إذا(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            استمر;
        }
        إذا(!USTRINGTRIE_HAS_NEXT(result)) {
            إرجاع خطأ;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    إرجاع USTRINGTRIE_HAS_قيمة(result);
}

ثابت حرف *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t قيمة_Map_Index=findProperty(property);
    إذا(قيمة_Map_Index==0) {
        إرجاع nullptr;  // Not a known property.
    }
    إرجاع getName(nameGroups+قيمة_Maps[قيمة_Map_Index], nameChoice);
}

ثابت حرف *PropNameData::get_Property_قيمة_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t قيمة_Map_Index=findProperty(property);
    إذا(قيمة_Map_Index==0) {
        إرجاع nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_قيمة_Name_Group(قيمة_Maps[قيمة_Map_Index+1], value);
    إذا(nameGroupOffset==0) {
        إرجاع nullptr;
    }
    إرجاع getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_قيمة_Enum(int32_t bytesTrieOffset, ثابت حرف *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    إذا(containsName(trie, alias)) {
        إرجاع trie.get_قيمة();
    } وإلا {
        إرجاع UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ثابت حرف *alias) {
    إرجاع get_Property_Or_قيمة_Enum(0, alias);
}

int32_t PropNameData::get_Property_قيمة_Enum(int32_t property, ثابت حرف *alias) {
    int32_t قيمة_Map_Index=findProperty(property);
    إذا(قيمة_Map_Index==0) {
        إرجاع UCHAR_INVALID_CODE;  // Not a known property.
    }
    قيمة_Map_Index=قيمة_Maps[قيمة_Map_Index+1];
    إذا(قيمة_Map_Index==0) {
        إرجاع UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    إرجاع get_Property_Or_قيمة_Enum(قيمة_Maps[قيمة_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    إرجاع PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    إرجاع (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_get_Property_قيمة_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    إرجاع PropNameData::get_Property_قيمة_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_قيمة_Enum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    إرجاع PropNameData::get_Property_قيمة_Enum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    إرجاع u_get_Property_قيمة_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    إرجاع u_get_Property_قيمة_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
