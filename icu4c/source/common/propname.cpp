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
#অন্তর্ভাব "propname.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/uscript.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uarrsort.h"
#অন্তর্ভাব "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#অন্তর্ভাব "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
স্থির অন্তঃরেখ int32_t
getASCIIPropertyNameChar(ধ্রুবক অক্ষর *name) {
    int32_t i;
    অক্ষর c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    জন্য(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    যদি(c!=0) {
        ফেরত (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } অন্যথা {
        ফেরত i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
স্থির অন্তঃরেখ int32_t
getEBCDICPropertyNameChar(ধ্রুবক অক্ষর *name) {
    int32_t i;
    অক্ষর c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    জন্য(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    যদি(c!=0) {
        ফেরত (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } অন্যথা {
        ফেরত i<<8;
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
uprv_compareASCIIPropertyNames(ধ্রুবক অক্ষর *name1, ধ্রুবক অক্ষর *name2) {
    int32_t rc, r1, r2;

    জন্য(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        যদি(((r1|r2)&0xff)==0) {
            ফেরত 0;
        }

        /* Compare the lowercased characters */
        যদি(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            যদি(rc!=0) {
                ফেরত rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ধ্রুবক অক্ষর *name1, ধ্রুবক অক্ষর *name2) {
    int32_t rc, r1, r2;

    জন্য(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        যদি(((r1|r2)&0xff)==0) {
            ফেরত 0;
        }

        /* Compare the lowercased characters */
        যদি(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            যদি(rc!=0) {
                ফেরত rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_END

U_NAMESPACE_BEGIN

int32_t Prop_Name_তথ্য::findProperty(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    জন্য(int32_t অঙ্ক_Ranges=মান_Maps[0]; অঙ্ক_Ranges>0; --অঙ্ক_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=মান_Maps[i];
        int32_t limit=মান_Maps[i+1];
        i+=2;
        যদি(property<start) {
            বিরতি;
        }
        যদি(property<limit) {
            ফেরত i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    ফেরত 0;
}

int32_t Prop_Name_তথ্য::find_Property_মান_Name_Group(int32_t মান_Map_সূচক, int32_t value) {
    যদি(মান_Map_সূচক==0) {
        ফেরত 0;  // The property does not have named values.
    }
    ++মান_Map_সূচক;  // Skip the BytesTrie offset.
    int32_t অঙ্ক_Ranges=মান_Maps[মান_Map_সূচক++];
    যদি(অঙ্ক_Ranges<0x10) {
        // Ranges of values.
        জন্য(; অঙ্ক_Ranges>0; --অঙ্ক_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=মান_Maps[মান_Map_সূচক];
            int32_t limit=মান_Maps[মান_Map_সূচক+1];
            মান_Map_সূচক+=2;
            যদি(value<start) {
                বিরতি;
            }
            যদি(value<limit) {
                ফেরত মান_Maps[মান_Map_সূচক+value-start];
            }
            মান_Map_সূচক+=limit-start;  // Skip all entries for this range.
        }
    } অন্যথা {
        // List of values.
        int32_t মানসমূহ_Start=মান_Map_সূচক;
        int32_t nameGroupOffsetsStart=মান_Map_সূচক+অঙ্ক_Ranges-0x10;
        করো {
            int32_t v=মান_Maps[মান_Map_সূচক];
            যদি(value<v) {
                বিরতি;
            }
            যদি(value==v) {
                ফেরত মান_Maps[nameGroupOffsetsStart+মান_Map_সূচক-মানসমূহ_Start];
            }
        } যখন(++মান_Map_সূচক<nameGroupOffsetsStart);
    }
    ফেরত 0;
}

ধ্রুবক অক্ষর *Prop_Name_তথ্য::getName(ধ্রুবক অক্ষর *nameGroup, int32_t name_সূচক) {
    int32_t অঙ্ক_Names=*nameGroup++;
    যদি(name_সূচক<0 || অঙ্ক_Names<=name_সূচক) {
        ফেরত nullptr;
    }
    // Skip nameIndex names.
    জন্য(; name_সূচক>0; --name_সূচক) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    যদি(*nameGroup==0) {
        ফেরত nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ফেরত nameGroup;
}

UBool Prop_Name_তথ্য::containsName(BytesTrie &trie, ধ্রুবক অক্ষর *name) {
    যদি(name==nullptr) {
        ফেরত মিথ্যা;
    }
    UString_Trie_ফলাফল ফলাফল=USTRINGTRIE_NO_মান;
    অক্ষর c;
    যখন((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        যদি(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            চালু;
        }
        যদি(!USTRINGTRIE_HAS_পরবর্তী(ফলাফল)) {
            ফেরত মিথ্যা;
        }
        ফলাফল = trie.next(static_cast<uint8_t>(c));
    }
    ফেরত USTRINGTRIE_HAS_মান(ফলাফল);
}

ধ্রুবক অক্ষর *Prop_Name_তথ্য::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t মান_Map_সূচক=findProperty(property);
    যদি(মান_Map_সূচক==0) {
        ফেরত nullptr;  // Not a known property.
    }
    ফেরত getName(nameGroups+মান_Maps[মান_Map_সূচক], nameChoice);
}

ধ্রুবক অক্ষর *Prop_Name_তথ্য::get_Property_মান_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t মান_Map_সূচক=findProperty(property);
    যদি(মান_Map_সূচক==0) {
        ফেরত nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_মান_Name_Group(মান_Maps[মান_Map_সূচক+1], value);
    যদি(nameGroupOffset==0) {
        ফেরত nullptr;
    }
    ফেরত getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t Prop_Name_তথ্য::get_Property_Or_মান_Enum(int32_t bytesTrieOffset, ধ্রুবক অক্ষর *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    যদি(containsName(trie, alias)) {
        ফেরত trie.get_মান();
    } অন্যথা {
        ফেরত UCHAR_INVALID_CODE;
    }
}

int32_t Prop_Name_তথ্য::getPropertyEnum(ধ্রুবক অক্ষর *alias) {
    ফেরত get_Property_Or_মান_Enum(0, alias);
}

int32_t Prop_Name_তথ্য::get_Property_মান_Enum(int32_t property, ধ্রুবক অক্ষর *alias) {
    int32_t মান_Map_সূচক=findProperty(property);
    যদি(মান_Map_সূচক==0) {
        ফেরত UCHAR_INVALID_CODE;  // Not a known property.
    }
    মান_Map_সূচক=মান_Maps[মান_Map_সূচক+1];
    যদি(মান_Map_সূচক==0) {
        ফেরত UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ফেরত get_Property_Or_মান_Enum(মান_Maps[মান_Map_সূচক], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ফেরত Prop_Name_তথ্য::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ধ্রুবক অক্ষর* alias) {
    U_NAMESPACE_USE
    ফেরত (UProperty)Prop_Name_তথ্য::getPropertyEnum(alias);
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
u_get_Property_মান_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ফেরত Prop_Name_তথ্য::get_Property_মান_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_মান_Enum(UProperty property,
                       ধ্রুবক অক্ষর* alias) {
    U_NAMESPACE_USE
    ফেরত Prop_Name_তথ্য::get_Property_মান_Enum(property, alias);
}

U_CAPI ধ্রুবক অক্ষর*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    ফেরত u_get_Property_মান_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ধ্রুবক অক্ষর*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    ফেরত u_get_Property_মান_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
