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
#அந்தர்பாவ "propname.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uarrsort.h"
#அந்தர்பாவ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#அந்தர்பாவ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
நிலையான உள்வரி int32_t
getASCIIPropertyNameChar(மாறிலி எழுத்து *name) {
    int32_t i;
    எழுத்து c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    க்கு(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    எனில்(c!=0) {
        திரும்பு (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } இல்லையெனில் {
        திரும்பு i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
நிலையான உள்வரி int32_t
getEBCDICPropertyNameChar(மாறிலி எழுத்து *name) {
    int32_t i;
    எழுத்து c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    க்கு(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    எனில்(c!=0) {
        திரும்பு (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } இல்லையெனில் {
        திரும்பு i<<8;
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
uprv_compareASCIIPropertyNames(மாறிலி எழுத்து *name1, மாறிலி எழுத்து *name2) {
    int32_t rc, r1, r2;

    க்கு(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        எனில்(((r1|r2)&0xff)==0) {
            திரும்பு 0;
        }

        /* Compare the lowercased characters */
        எனில்(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            எனில்(rc!=0) {
                திரும்பு rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(மாறிலி எழுத்து *name1, மாறிலி எழுத்து *name2) {
    int32_t rc, r1, r2;

    க்கு(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        எனில்(((r1|r2)&0xff)==0) {
            திரும்பு 0;
        }

        /* Compare the lowercased characters */
        எனில்(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            எனில்(rc!=0) {
                திரும்பு rc;
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
    க்கு(int32_t இலக்கம்_Ranges=மதிப்பு_Maps[0]; இலக்கம்_Ranges>0; --இலக்கம்_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=மதிப்பு_Maps[i];
        int32_t limit=மதிப்பு_Maps[i+1];
        i+=2;
        எனில்(property<start) {
            நிறுத்து;
        }
        எனில்(property<limit) {
            திரும்பு i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    திரும்பு 0;
}

int32_t PropNameData::find_Property_மதிப்பு_Name_Group(int32_t மதிப்பு_Map_Index, int32_t value) {
    எனில்(மதிப்பு_Map_Index==0) {
        திரும்பு 0;  // The property does not have named values.
    }
    ++மதிப்பு_Map_Index;  // Skip the BytesTrie offset.
    int32_t இலக்கம்_Ranges=மதிப்பு_Maps[மதிப்பு_Map_Index++];
    எனில்(இலக்கம்_Ranges<0x10) {
        // Ranges of values.
        க்கு(; இலக்கம்_Ranges>0; --இலக்கம்_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=மதிப்பு_Maps[மதிப்பு_Map_Index];
            int32_t limit=மதிப்பு_Maps[மதிப்பு_Map_Index+1];
            மதிப்பு_Map_Index+=2;
            எனில்(value<start) {
                நிறுத்து;
            }
            எனில்(value<limit) {
                திரும்பு மதிப்பு_Maps[மதிப்பு_Map_Index+value-start];
            }
            மதிப்பு_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } இல்லையெனில் {
        // List of values.
        int32_t மதிப்புகள்_Start=மதிப்பு_Map_Index;
        int32_t nameGroupOffsetsStart=மதிப்பு_Map_Index+இலக்கம்_Ranges-0x10;
        செய் {
            int32_t v=மதிப்பு_Maps[மதிப்பு_Map_Index];
            எனில்(value<v) {
                நிறுத்து;
            }
            எனில்(value==v) {
                திரும்பு மதிப்பு_Maps[nameGroupOffsetsStart+மதிப்பு_Map_Index-மதிப்புகள்_Start];
            }
        } போது(++மதிப்பு_Map_Index<nameGroupOffsetsStart);
    }
    திரும்பு 0;
}

மாறிலி எழுத்து *PropNameData::getName(மாறிலி எழுத்து *nameGroup, int32_t nameIndex) {
    int32_t இலக்கம்_Names=*nameGroup++;
    எனில்(nameIndex<0 || இலக்கம்_Names<=nameIndex) {
        திரும்பு nullptr;
    }
    // Skip nameIndex names.
    க்கு(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    எனில்(*nameGroup==0) {
        திரும்பு nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    திரும்பு nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, மாறிலி எழுத்து *name) {
    எனில்(name==nullptr) {
        திரும்பு பொய்;
    }
    UStringTrieResult result=USTRINGTRIE_NO_மதிப்பு;
    எழுத்து c;
    போது((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        எனில்(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            தொடர்;
        }
        எனில்(!USTRINGTRIE_HAS_NEXT(result)) {
            திரும்பு பொய்;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    திரும்பு USTRINGTRIE_HAS_மதிப்பு(result);
}

மாறிலி எழுத்து *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t மதிப்பு_Map_Index=findProperty(property);
    எனில்(மதிப்பு_Map_Index==0) {
        திரும்பு nullptr;  // Not a known property.
    }
    திரும்பு getName(nameGroups+மதிப்பு_Maps[மதிப்பு_Map_Index], nameChoice);
}

மாறிலி எழுத்து *PropNameData::get_Property_மதிப்பு_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t மதிப்பு_Map_Index=findProperty(property);
    எனில்(மதிப்பு_Map_Index==0) {
        திரும்பு nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_மதிப்பு_Name_Group(மதிப்பு_Maps[மதிப்பு_Map_Index+1], value);
    எனில்(nameGroupOffset==0) {
        திரும்பு nullptr;
    }
    திரும்பு getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_மதிப்பு_Enum(int32_t bytesTrieOffset, மாறிலி எழுத்து *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    எனில்(containsName(trie, alias)) {
        திரும்பு trie.get_மதிப்பு();
    } இல்லையெனில் {
        திரும்பு UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(மாறிலி எழுத்து *alias) {
    திரும்பு get_Property_Or_மதிப்பு_Enum(0, alias);
}

int32_t PropNameData::get_Property_மதிப்பு_Enum(int32_t property, மாறிலி எழுத்து *alias) {
    int32_t மதிப்பு_Map_Index=findProperty(property);
    எனில்(மதிப்பு_Map_Index==0) {
        திரும்பு UCHAR_INVALID_CODE;  // Not a known property.
    }
    மதிப்பு_Map_Index=மதிப்பு_Maps[மதிப்பு_Map_Index+1];
    எனில்(மதிப்பு_Map_Index==0) {
        திரும்பு UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    திரும்பு get_Property_Or_மதிப்பு_Enum(மதிப்பு_Maps[மதிப்பு_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI மாறிலி எழுத்து* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    திரும்பு PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(மாறிலி எழுத்து* alias) {
    U_NAMESPACE_USE
    திரும்பு (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI மாறிலி எழுத்து* U_EXPORT2
u_get_Property_மதிப்பு_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    திரும்பு PropNameData::get_Property_மதிப்பு_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_மதிப்பு_Enum(UProperty property,
                       மாறிலி எழுத்து* alias) {
    U_NAMESPACE_USE
    திரும்பு PropNameData::get_Property_மதிப்பு_Enum(property, alias);
}

U_CAPI மாறிலி எழுத்து*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    திரும்பு u_get_Property_மதிப்பு_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI மாறிலி எழுத்து*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    திரும்பு u_get_Property_மதிப்பு_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
