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

#تعریف_کرو INCLUDED_FROM_PROPNAME_CPP
#شامل "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ساکن اندرونی int32_t
getASCIIPropertyNameChar(ثابت حرف *name) {
    int32_t i;
    حرف c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    برائے(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    اگر(c!=0) {
        واپس (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ورنہ {
        واپس i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ساکن اندرونی int32_t
getEBCDICPropertyNameChar(ثابت حرف *name) {
    int32_t i;
    حرف c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    برائے(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    اگر(c!=0) {
        واپس (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ورنہ {
        واپس i<<8;
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
uprv_compare_ASCIIProperty_نام(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    برائے(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        اگر(((r1|r2)&0xff)==0) {
            واپس 0;
        }

        /* Compare the lowercased characters */
        اگر(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            اگر(rc!=0) {
                واپس rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_نام(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    برائے(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        اگر(((r1|r2)&0xff)==0) {
            واپس 0;
        }

        /* Compare the lowercased characters */
        اگر(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            اگر(rc!=0) {
                واپس rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_انتہا

U_NAMESPACE_BEGIN

int32_t Prop_Name_ڈیٹا::ڈھونڈو_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    برائے(int32_t عدد_Ranges=قدر_Maps[0]; عدد_Ranges>0; --عدد_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t آغاز=قدر_Maps[i];
        int32_t حد_بندی=قدر_Maps[i+1];
        i+=2;
        اگر(property<آغاز) {
            توڑو;
        }
        اگر(property<حد_بندی) {
            واپس i+(property-آغاز)*2;
        }
        i+=(حد_بندی-آغاز)*2;  // Skip all entries for this range.
    }
    واپس 0;
}

int32_t Prop_Name_ڈیٹا::ڈھونڈو_Property_قدر_Name_گروہ(int32_t قدر_Map_اشاریہ, int32_t value) {
    اگر(قدر_Map_اشاریہ==0) {
        واپس 0;  // The property does not have named values.
    }
    ++قدر_Map_اشاریہ;  // Skip the BytesTrie offset.
    int32_t عدد_Ranges=قدر_Maps[قدر_Map_اشاریہ++];
    اگر(عدد_Ranges<0x10) {
        // Ranges of values.
        برائے(; عدد_Ranges>0; --عدد_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t آغاز=قدر_Maps[قدر_Map_اشاریہ];
            int32_t حد_بندی=قدر_Maps[قدر_Map_اشاریہ+1];
            قدر_Map_اشاریہ+=2;
            اگر(value<آغاز) {
                توڑو;
            }
            اگر(value<حد_بندی) {
                واپس قدر_Maps[قدر_Map_اشاریہ+value-آغاز];
            }
            قدر_Map_اشاریہ+=حد_بندی-آغاز;  // Skip all entries for this range.
        }
    } ورنہ {
        // List of values.
        int32_t اقدار_آغاز=قدر_Map_اشاریہ;
        int32_t name_گروہ_Offsets_آغاز=قدر_Map_اشاریہ+عدد_Ranges-0x10;
        کرو {
            int32_t v=قدر_Maps[قدر_Map_اشاریہ];
            اگر(value<v) {
                توڑو;
            }
            اگر(value==v) {
                واپس قدر_Maps[name_گروہ_Offsets_آغاز+قدر_Map_اشاریہ-اقدار_آغاز];
            }
        } جب(++قدر_Map_اشاریہ<name_گروہ_Offsets_آغاز);
    }
    واپس 0;
}

ثابت حرف *Prop_Name_ڈیٹا::getName(ثابت حرف *name_گروہ, int32_t name_اشاریہ) {
    int32_t عدد_نام=*name_گروہ++;
    اگر(name_اشاریہ<0 || عدد_نام<=name_اشاریہ) {
        واپس خالی_اشاریہ;
    }
    // Skip nameIndex names.
    برائے(; name_اشاریہ>0; --name_اشاریہ) {
        name_گروہ=uprv_strchr(name_گروہ, 0)+1;
    }
    اگر(*name_گروہ==0) {
        واپس خالی_اشاریہ;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    واپس name_گروہ;
}

UBool Prop_Name_ڈیٹا::شامل_ہے_Name(بائٹیں_Trie &trie, ثابت حرف *name) {
    اگر(name==خالی_اشاریہ) {
        واپس جھوٹ;
    }
    UString_Trie_نتیجہ نتیجہ=USTRINGTRIE_NO_قدر;
    حرف c;
    جب((c=*name++)!=0) {
        c=uprv_inv_Char_مقصد_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        اگر(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            جاری;
        }
        اگر(!USTRINGTRIE_HAS_اگلا(نتیجہ)) {
            واپس جھوٹ;
        }
        نتیجہ = trie.next(static_cast<uint8_t>(c));
    }
    واپس USTRINGTRIE_HAS_قدر(نتیجہ);
}

ثابت حرف *Prop_Name_ڈیٹا::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t قدر_Map_اشاریہ=ڈھونڈو_Property(property);
    اگر(قدر_Map_اشاریہ==0) {
        واپس خالی_اشاریہ;  // Not a known property.
    }
    واپس getName(nameGroups+قدر_Maps[قدر_Map_اشاریہ], nameChoice);
}

ثابت حرف *Prop_Name_ڈیٹا::get_Property_قدر_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t قدر_Map_اشاریہ=ڈھونڈو_Property(property);
    اگر(قدر_Map_اشاریہ==0) {
        واپس خالی_اشاریہ;  // Not a known property.
    }
    int32_t name_گروہ_انحراف=ڈھونڈو_Property_قدر_Name_گروہ(قدر_Maps[قدر_Map_اشاریہ+1], value);
    اگر(name_گروہ_انحراف==0) {
        واپس خالی_اشاریہ;
    }
    واپس getName(nameGroups+name_گروہ_انحراف, nameChoice);
}

int32_t Prop_Name_ڈیٹا::get_Property_Or_قدر_Enum(int32_t بائٹیں_Trie_انحراف, ثابت حرف *alias) {
    بائٹیں_Trie trie(بائٹیں_Tries+بائٹیں_Trie_انحراف);
    اگر(شامل_ہے_Name(trie, alias)) {
        واپس trie.get_قدر();
    } ورنہ {
        واپس UCHAR_INVALID_رمز;
    }
}

int32_t Prop_Name_ڈیٹا::getPropertyEnum(ثابت حرف *alias) {
    واپس get_Property_Or_قدر_Enum(0, alias);
}

int32_t Prop_Name_ڈیٹا::get_Property_قدر_Enum(int32_t property, ثابت حرف *alias) {
    int32_t قدر_Map_اشاریہ=ڈھونڈو_Property(property);
    اگر(قدر_Map_اشاریہ==0) {
        واپس UCHAR_INVALID_رمز;  // Not a known property.
    }
    قدر_Map_اشاریہ=قدر_Maps[قدر_Map_اشاریہ+1];
    اگر(قدر_Map_اشاریہ==0) {
        واپس UCHAR_INVALID_رمز;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    واپس get_Property_Or_قدر_Enum(قدر_Maps[قدر_Map_اشاریہ], alias);
}
U_NAMESPACE_انتہا

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    واپس Prop_Name_ڈیٹا::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    واپس (UProperty)Prop_Name_ڈیٹا::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_get_Property_قدر_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    واپس Prop_Name_ڈیٹا::get_Property_قدر_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_قدر_Enum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    واپس Prop_Name_ڈیٹا::get_Property_قدر_Enum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScript_رمز script_رمز){
    واپس u_get_Property_قدر_Name(UCHAR_SCRIPT, script_رمز,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScript_رمز script_رمز){
    واپس u_get_Property_قدر_Name(UCHAR_SCRIPT, script_رمز,
                                  U_SHORT_PROPERTY_NAME);
}
