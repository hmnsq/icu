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

#تعریف_کرو INCLUDED_FROM_PROPNAME_CPP
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
        أرجع (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } وإلا {
        أرجع i<<8;
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
        أرجع (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } وإلا {
        أرجع i<<8;
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
uprv_compare_ASCIIProperty_ناں(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    لكل(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        إذا(((r1|r2)&0xff)==0) {
            أرجع 0;
        }

        /* Compare the lowercased characters */
        إذا(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            إذا(rc!=0) {
                أرجع rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_ناں(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    لكل(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        إذا(((r1|r2)&0xff)==0) {
            أرجع 0;
        }

        /* Compare the lowercased characters */
        إذا(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            إذا(rc!=0) {
                أرجع rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_سرا

U_NAMESPACE_BEGIN

int32_t Prop_Name_انکڑے::لبھو_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    لكل(int32_t عدد_Ranges=قدر_Maps[0]; عدد_Ranges>0; --عدد_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t شروع=قدر_Maps[i];
        int32_t حد_بندی=قدر_Maps[i+1];
        i+=2;
        إذا(property<شروع) {
            اكسر;
        }
        إذا(property<حد_بندی) {
            أرجع i+(property-شروع)*2;
        }
        i+=(حد_بندی-شروع)*2;  // Skip all entries for this range.
    }
    أرجع 0;
}

int32_t Prop_Name_انکڑے::لبھو_Property_قدر_Name_گروہ(int32_t قدر_Map_اشاریہ, int32_t value) {
    إذا(قدر_Map_اشاریہ==0) {
        أرجع 0;  // The property does not have named values.
    }
    ++قدر_Map_اشاریہ;  // Skip the BytesTrie offset.
    int32_t عدد_Ranges=قدر_Maps[قدر_Map_اشاریہ++];
    إذا(عدد_Ranges<0x10) {
        // Ranges of values.
        لكل(; عدد_Ranges>0; --عدد_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t شروع=قدر_Maps[قدر_Map_اشاریہ];
            int32_t حد_بندی=قدر_Maps[قدر_Map_اشاریہ+1];
            قدر_Map_اشاریہ+=2;
            إذا(value<شروع) {
                اكسر;
            }
            إذا(value<حد_بندی) {
                أرجع قدر_Maps[قدر_Map_اشاریہ+value-شروع];
            }
            قدر_Map_اشاریہ+=حد_بندی-شروع;  // Skip all entries for this range.
        }
    } وإلا {
        // List of values.
        int32_t اقدار_شروع=قدر_Map_اشاریہ;
        int32_t name_گروہ_Offsets_شروع=قدر_Map_اشاریہ+عدد_Ranges-0x10;
        افعل {
            int32_t v=قدر_Maps[قدر_Map_اشاریہ];
            إذا(value<v) {
                اكسر;
            }
            إذا(value==v) {
                أرجع قدر_Maps[name_گروہ_Offsets_شروع+قدر_Map_اشاریہ-اقدار_شروع];
            }
        } طالما(++قدر_Map_اشاریہ<name_گروہ_Offsets_شروع);
    }
    أرجع 0;
}

ثابت حرف *Prop_Name_انکڑے::getName(ثابت حرف *name_گروہ, int32_t name_اشاریہ) {
    int32_t عدد_ناں=*name_گروہ++;
    إذا(name_اشاریہ<0 || عدد_ناں<=name_اشاریہ) {
        أرجع خالی_سوچک;
    }
    // Skip nameIndex names.
    لكل(; name_اشاریہ>0; --name_اشاریہ) {
        name_گروہ=uprv_strchr(name_گروہ, 0)+1;
    }
    إذا(*name_گروہ==0) {
        أرجع خالی_سوچک;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    أرجع name_گروہ;
}

UBool Prop_Name_انکڑے::شامل_اے_Name(بائٹاں_Trie &trie, ثابت حرف *name) {
    إذا(name==خالی_سوچک) {
        أرجع خطأ;
    }
    UString_Trie_نتیجہ نتیجہ=USTRINGTRIE_NO_قدر;
    حرف c;
    طالما((c=*name++)!=0) {
        c=uprv_inv_Char_منزل_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        إذا(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            استمر;
        }
        إذا(!USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            أرجع خطأ;
        }
        نتیجہ = trie.next(static_cast<uint8_t>(c));
    }
    أرجع USTRINGTRIE_HAS_قدر(نتیجہ);
}

ثابت حرف *Prop_Name_انکڑے::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t قدر_Map_اشاریہ=لبھو_Property(property);
    إذا(قدر_Map_اشاریہ==0) {
        أرجع خالی_سوچک;  // Not a known property.
    }
    أرجع getName(nameGroups+قدر_Maps[قدر_Map_اشاریہ], nameChoice);
}

ثابت حرف *Prop_Name_انکڑے::get_Property_قدر_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t قدر_Map_اشاریہ=لبھو_Property(property);
    إذا(قدر_Map_اشاریہ==0) {
        أرجع خالی_سوچک;  // Not a known property.
    }
    int32_t name_گروہ_وتھ=لبھو_Property_قدر_Name_گروہ(قدر_Maps[قدر_Map_اشاریہ+1], value);
    إذا(name_گروہ_وتھ==0) {
        أرجع خالی_سوچک;
    }
    أرجع getName(nameGroups+name_گروہ_وتھ, nameChoice);
}

int32_t Prop_Name_انکڑے::get_Property_Or_قدر_Enum(int32_t بائٹاں_Trie_وتھ, ثابت حرف *alias) {
    بائٹاں_Trie trie(بائٹاں_Tries+بائٹاں_Trie_وتھ);
    إذا(شامل_اے_Name(trie, alias)) {
        أرجع trie.get_قدر();
    } وإلا {
        أرجع UCHAR_INVALID_کوڈ;
    }
}

int32_t Prop_Name_انکڑے::getPropertyEnum(ثابت حرف *alias) {
    أرجع get_Property_Or_قدر_Enum(0, alias);
}

int32_t Prop_Name_انکڑے::get_Property_قدر_Enum(int32_t property, ثابت حرف *alias) {
    int32_t قدر_Map_اشاریہ=لبھو_Property(property);
    إذا(قدر_Map_اشاریہ==0) {
        أرجع UCHAR_INVALID_کوڈ;  // Not a known property.
    }
    قدر_Map_اشاریہ=قدر_Maps[قدر_Map_اشاریہ+1];
    إذا(قدر_Map_اشاریہ==0) {
        أرجع UCHAR_INVALID_کوڈ;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    أرجع get_Property_Or_قدر_Enum(قدر_Maps[قدر_Map_اشاریہ], alias);
}
U_NAMESPACE_سرا

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    أرجع Prop_Name_انکڑے::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع (UProperty)Prop_Name_انکڑے::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_get_Property_قدر_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    أرجع Prop_Name_انکڑے::get_Property_قدر_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_قدر_Enum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع Prop_Name_انکڑے::get_Property_قدر_Enum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScript_کوڈ script_کوڈ){
    أرجع u_get_Property_قدر_Name(UCHAR_SCRIPT, script_کوڈ,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScript_کوڈ script_کوڈ){
    أرجع u_get_Property_قدر_Name(UCHAR_SCRIPT, script_کوڈ,
                                  U_SHORT_PROPERTY_NAME);
}
