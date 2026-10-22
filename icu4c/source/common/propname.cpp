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
#شامل "propname.h"
#شامل "unicode/uchar.h"
#شامل "unicode/udata.h"
#شامل "unicode/uscript.h"
#شامل "umutex.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "uarrsort.h"
#شامل "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#شامل "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ایستا درونخطی int32_t
getASCIIPropertyNameChar(ثابت نویسه *name) {
    int32_t i;
    نویسه c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    برای(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    اگر(c!=0) {
        بازگشت (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } وگرنه {
        بازگشت i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ایستا درونخطی int32_t
getEBCDICPropertyNameChar(ثابت نویسه *name) {
    int32_t i;
    نویسه c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    برای(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    اگر(c!=0) {
        بازگشت (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } وگرنه {
        بازگشت i<<8;
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
uprv_compareASCIIPropertyNames(ثابت نویسه *name1, ثابت نویسه *name2) {
    int32_t rc, r1, r2;

    برای(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        اگر(((r1|r2)&0xff)==0) {
            بازگشت 0;
        }

        /* Compare the lowercased characters */
        اگر(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            اگر(rc!=0) {
                بازگشت rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ثابت نویسه *name1, ثابت نویسه *name2) {
    int32_t rc, r1, r2;

    برای(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        اگر(((r1|r2)&0xff)==0) {
            بازگشت 0;
        }

        /* Compare the lowercased characters */
        اگر(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            اگر(rc!=0) {
                بازگشت rc;
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
    برای(int32_t رقم_Ranges=مقدار_Maps[0]; رقم_Ranges>0; --رقم_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=مقدار_Maps[i];
        int32_t limit=مقدار_Maps[i+1];
        i+=2;
        اگر(property<start) {
            بشکن;
        }
        اگر(property<limit) {
            بازگشت i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    بازگشت 0;
}

int32_t PropNameData::find_Property_مقدار_Name_Group(int32_t مقدار_Map_Index, int32_t value) {
    اگر(مقدار_Map_Index==0) {
        بازگشت 0;  // The property does not have named values.
    }
    ++مقدار_Map_Index;  // Skip the BytesTrie offset.
    int32_t رقم_Ranges=مقدار_Maps[مقدار_Map_Index++];
    اگر(رقم_Ranges<0x10) {
        // Ranges of values.
        برای(; رقم_Ranges>0; --رقم_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=مقدار_Maps[مقدار_Map_Index];
            int32_t limit=مقدار_Maps[مقدار_Map_Index+1];
            مقدار_Map_Index+=2;
            اگر(value<start) {
                بشکن;
            }
            اگر(value<limit) {
                بازگشت مقدار_Maps[مقدار_Map_Index+value-start];
            }
            مقدار_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } وگرنه {
        // List of values.
        int32_t مقادیر_Start=مقدار_Map_Index;
        int32_t nameGroupOffsetsStart=مقدار_Map_Index+رقم_Ranges-0x10;
        انجامبده {
            int32_t v=مقدار_Maps[مقدار_Map_Index];
            اگر(value<v) {
                بشکن;
            }
            اگر(value==v) {
                بازگشت مقدار_Maps[nameGroupOffsetsStart+مقدار_Map_Index-مقادیر_Start];
            }
        } وقتی(++مقدار_Map_Index<nameGroupOffsetsStart);
    }
    بازگشت 0;
}

ثابت نویسه *PropNameData::getName(ثابت نویسه *nameGroup, int32_t nameIndex) {
    int32_t رقم_Names=*nameGroup++;
    اگر(nameIndex<0 || رقم_Names<=nameIndex) {
        بازگشت nullptr;
    }
    // Skip nameIndex names.
    برای(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    اگر(*nameGroup==0) {
        بازگشت nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    بازگشت nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ثابت نویسه *name) {
    اگر(name==nullptr) {
        بازگشت نادرست;
    }
    UStringTrieResult result=USTRINGTRIE_NO_مقدار;
    نویسه c;
    وقتی((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        اگر(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ادامه;
        }
        اگر(!USTRINGTRIE_HAS_NEXT(result)) {
            بازگشت نادرست;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    بازگشت USTRINGTRIE_HAS_مقدار(result);
}

ثابت نویسه *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t مقدار_Map_Index=findProperty(property);
    اگر(مقدار_Map_Index==0) {
        بازگشت nullptr;  // Not a known property.
    }
    بازگشت getName(nameGroups+مقدار_Maps[مقدار_Map_Index], nameChoice);
}

ثابت نویسه *PropNameData::get_Property_مقدار_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t مقدار_Map_Index=findProperty(property);
    اگر(مقدار_Map_Index==0) {
        بازگشت nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_مقدار_Name_Group(مقدار_Maps[مقدار_Map_Index+1], value);
    اگر(nameGroupOffset==0) {
        بازگشت nullptr;
    }
    بازگشت getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_مقدار_Enum(int32_t bytesTrieOffset, ثابت نویسه *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    اگر(containsName(trie, alias)) {
        بازگشت trie.get_مقدار();
    } وگرنه {
        بازگشت UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ثابت نویسه *alias) {
    بازگشت get_Property_Or_مقدار_Enum(0, alias);
}

int32_t PropNameData::get_Property_مقدار_Enum(int32_t property, ثابت نویسه *alias) {
    int32_t مقدار_Map_Index=findProperty(property);
    اگر(مقدار_Map_Index==0) {
        بازگشت UCHAR_INVALID_CODE;  // Not a known property.
    }
    مقدار_Map_Index=مقدار_Maps[مقدار_Map_Index+1];
    اگر(مقدار_Map_Index==0) {
        بازگشت UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    بازگشت get_Property_Or_مقدار_Enum(مقدار_Maps[مقدار_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ثابت نویسه* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    بازگشت PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت نویسه* alias) {
    U_NAMESPACE_USE
    بازگشت (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ثابت نویسه* U_EXPORT2
u_get_Property_مقدار_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    بازگشت PropNameData::get_Property_مقدار_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_مقدار_Enum(UProperty property,
                       ثابت نویسه* alias) {
    U_NAMESPACE_USE
    بازگشت PropNameData::get_Property_مقدار_Enum(property, alias);
}

U_CAPI ثابت نویسه*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    بازگشت u_get_Property_مقدار_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت نویسه*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    بازگشت u_get_Property_مقدار_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
