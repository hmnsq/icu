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
#অন্তর্ভাৱ "propname.h"
#অন্তর্ভাৱ "unicode/uchar.h"
#অন্তর্ভাৱ "unicode/udata.h"
#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "uarrsort.h"
#অন্তর্ভাৱ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#অন্তর্ভাৱ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
স্থির অন্তঃস্থ int32_t
getASCIIPropertyNameChar(ধ্রুৱ ৱর্ণ *name) {
    int32_t i;
    ৱর্ণ c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    বাবে(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    যদি(c!=0) {
        ঘূৰাই (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } অন্যথা {
        ঘূৰাই i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
স্থির অন্তঃস্থ int32_t
getEBCDICPropertyNameChar(ধ্রুৱ ৱর্ণ *name) {
    int32_t i;
    ৱর্ণ c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    বাবে(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    যদি(c!=0) {
        ঘূৰাই (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } অন্যথা {
        ঘূৰাই i<<8;
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
uprv_compareASCIIPropertyNames(ধ্রুৱ ৱর্ণ *name1, ধ্রুৱ ৱর্ণ *name2) {
    int32_t rc, r1, r2;

    বাবে(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        যদি(((r1|r2)&0xff)==0) {
            ঘূৰাই 0;
        }

        /* Compare the lowercased characters */
        যদি(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            যদি(rc!=0) {
                ঘূৰাই rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ধ্রুৱ ৱর্ণ *name1, ধ্রুৱ ৱর্ণ *name2) {
    int32_t rc, r1, r2;

    বাবে(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        যদি(((r1|r2)&0xff)==0) {
            ঘূৰাই 0;
        }

        /* Compare the lowercased characters */
        যদি(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            যদি(rc!=0) {
                ঘূৰাই rc;
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
    বাবে(int32_t অঙ্ক_Ranges=মান_Maps[0]; অঙ্ক_Ranges>0; --অঙ্ক_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=মান_Maps[i];
        int32_t limit=মান_Maps[i+1];
        i+=2;
        যদি(property<start) {
            ভঙ্গ;
        }
        যদি(property<limit) {
            ঘূৰাই i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    ঘূৰাই 0;
}

int32_t PropNameData::find_Property_মান_Name_Group(int32_t মান_Map_Index, int32_t value) {
    যদি(মান_Map_Index==0) {
        ঘূৰাই 0;  // The property does not have named values.
    }
    ++মান_Map_Index;  // Skip the BytesTrie offset.
    int32_t অঙ্ক_Ranges=মান_Maps[মান_Map_Index++];
    যদি(অঙ্ক_Ranges<0x10) {
        // Ranges of values.
        বাবে(; অঙ্ক_Ranges>0; --অঙ্ক_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=মান_Maps[মান_Map_Index];
            int32_t limit=মান_Maps[মান_Map_Index+1];
            মান_Map_Index+=2;
            যদি(value<start) {
                ভঙ্গ;
            }
            যদি(value<limit) {
                ঘূৰাই মান_Maps[মান_Map_Index+value-start];
            }
            মান_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } অন্যথা {
        // List of values.
        int32_t মানসমূহ_Start=মান_Map_Index;
        int32_t nameGroupOffsetsStart=মান_Map_Index+অঙ্ক_Ranges-0x10;
        কৰা {
            int32_t v=মান_Maps[মান_Map_Index];
            যদি(value<v) {
                ভঙ্গ;
            }
            যদি(value==v) {
                ঘূৰাই মান_Maps[nameGroupOffsetsStart+মান_Map_Index-মানসমূহ_Start];
            }
        } যেতিয়ালৈ(++মান_Map_Index<nameGroupOffsetsStart);
    }
    ঘূৰাই 0;
}

ধ্রুৱ ৱর্ণ *PropNameData::getName(ধ্রুৱ ৱর্ণ *nameGroup, int32_t nameIndex) {
    int32_t অঙ্ক_Names=*nameGroup++;
    যদি(nameIndex<0 || অঙ্ক_Names<=nameIndex) {
        ঘূৰাই nullptr;
    }
    // Skip nameIndex names.
    বাবে(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    যদি(*nameGroup==0) {
        ঘূৰাই nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ঘূৰাই nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ধ্রুৱ ৱর্ণ *name) {
    যদি(name==nullptr) {
        ঘূৰাই মিছা;
    }
    UStringTrieResult result=USTRINGTRIE_NO_মান;
    ৱর্ণ c;
    যেতিয়ালৈ((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        যদি(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            অনুৱর্ত;
        }
        যদি(!USTRINGTRIE_HAS_NEXT(result)) {
            ঘূৰাই মিছা;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    ঘূৰাই USTRINGTRIE_HAS_মান(result);
}

ধ্রুৱ ৱর্ণ *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t মান_Map_Index=findProperty(property);
    যদি(মান_Map_Index==0) {
        ঘূৰাই nullptr;  // Not a known property.
    }
    ঘূৰাই getName(nameGroups+মান_Maps[মান_Map_Index], nameChoice);
}

ধ্রুৱ ৱর্ণ *PropNameData::get_Property_মান_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t মান_Map_Index=findProperty(property);
    যদি(মান_Map_Index==0) {
        ঘূৰাই nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_মান_Name_Group(মান_Maps[মান_Map_Index+1], value);
    যদি(nameGroupOffset==0) {
        ঘূৰাই nullptr;
    }
    ঘূৰাই getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_মান_Enum(int32_t bytesTrieOffset, ধ্রুৱ ৱর্ণ *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    যদি(containsName(trie, alias)) {
        ঘূৰাই trie.get_মান();
    } অন্যথা {
        ঘূৰাই UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ধ্রুৱ ৱর্ণ *alias) {
    ঘূৰাই get_Property_Or_মান_Enum(0, alias);
}

int32_t PropNameData::get_Property_মান_Enum(int32_t property, ধ্রুৱ ৱর্ণ *alias) {
    int32_t মান_Map_Index=findProperty(property);
    যদি(মান_Map_Index==0) {
        ঘূৰাই UCHAR_INVALID_CODE;  // Not a known property.
    }
    মান_Map_Index=মান_Maps[মান_Map_Index+1];
    যদি(মান_Map_Index==0) {
        ঘূৰাই UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ঘূৰাই get_Property_Or_মান_Enum(মান_Maps[মান_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ঘূৰাই PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ধ্রুৱ ৱর্ণ* alias) {
    U_NAMESPACE_USE
    ঘূৰাই (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ধ্রুৱ ৱর্ণ* U_EXPORT2
u_get_Property_মান_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ঘূৰাই PropNameData::get_Property_মান_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_মান_Enum(UProperty property,
                       ধ্রুৱ ৱর্ণ* alias) {
    U_NAMESPACE_USE
    ঘূৰাই PropNameData::get_Property_মান_Enum(property, alias);
}

U_CAPI ধ্রুৱ ৱর্ণ*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    ঘূৰাই u_get_Property_মান_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ধ্রুৱ ৱর্ণ*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    ঘূৰাই u_get_Property_মান_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
