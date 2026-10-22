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
#ਅਨ੍ਤਰ੍ਭਾਵ "propname.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uarrsort.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INCLUDED_FROM_PROPNAME_CPP
#ਅਨ੍ਤਰ੍ਭਾਵ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
getASCIIPropertyNameChar(ਅਚਲ ਅੱਖਰ *name) {
    int32_t i;
    ਅੱਖਰ c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    ਲਈ(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ਜੇ(c!=0) {
        ਵਾਪਸ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ int32_t
getEBCDICPropertyNameChar(ਅਚਲ ਅੱਖਰ *name) {
    int32_t i;
    ਅੱਖਰ c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    ਲਈ(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ਜੇ(c!=0) {
        ਵਾਪਸ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ i<<8;
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
uprv_compare_ASCIIProperty_ਨਾਂ(ਅਚਲ ਅੱਖਰ *name1, ਅਚਲ ਅੱਖਰ *name2) {
    int32_t rc, r1, r2;

    ਲਈ(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ਜੇ(((r1|r2)&0xff)==0) {
            ਵਾਪਸ 0;
        }

        /* Compare the lowercased characters */
        ਜੇ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ਜੇ(rc!=0) {
                ਵਾਪਸ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_ਨਾਂ(ਅਚਲ ਅੱਖਰ *name1, ਅਚਲ ਅੱਖਰ *name2) {
    int32_t rc, r1, r2;

    ਲਈ(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ਜੇ(((r1|r2)&0xff)==0) {
            ਵਾਪਸ 0;
        }

        /* Compare the lowercased characters */
        ਜੇ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ਜੇ(rc!=0) {
                ਵਾਪਸ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_ਸਿਰਾ

U_NAMESPACE_BEGIN

int32_t Prop_Name_ਅੰਕੜੇ::ਲੱਭੋ_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    ਲਈ(int32_t ਅੰਕ_Ranges=ਮੁੱਲ_Maps[0]; ਅੰਕ_Ranges>0; --ਅੰਕ_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t ਸ਼ੁਰੂ=ਮੁੱਲ_Maps[i];
        int32_t ਹੱਦਬੰਦੀ=ਮੁੱਲ_Maps[i+1];
        i+=2;
        ਜੇ(property<ਸ਼ੁਰੂ) {
            ਰੋਕੋ;
        }
        ਜੇ(property<ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ i+(property-ਸ਼ੁਰੂ)*2;
        }
        i+=(ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ)*2;  // Skip all entries for this range.
    }
    ਵਾਪਸ 0;
}

int32_t Prop_Name_ਅੰਕੜੇ::ਲੱਭੋ_Property_ਮੁੱਲ_Name_ਸਮੂਹ(int32_t ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ, int32_t value) {
    ਜੇ(ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ==0) {
        ਵਾਪਸ 0;  // The property does not have named values.
    }
    ++ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ;  // Skip the BytesTrie offset.
    int32_t ਅੰਕ_Ranges=ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ++];
    ਜੇ(ਅੰਕ_Ranges<0x10) {
        // Ranges of values.
        ਲਈ(; ਅੰਕ_Ranges>0; --ਅੰਕ_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t ਸ਼ੁਰੂ=ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ];
            int32_t ਹੱਦਬੰਦੀ=ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+1];
            ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+=2;
            ਜੇ(value<ਸ਼ੁਰੂ) {
                ਰੋਕੋ;
            }
            ਜੇ(value<ਹੱਦਬੰਦੀ) {
                ਵਾਪਸ ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+value-ਸ਼ੁਰੂ];
            }
            ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+=ਹੱਦਬੰਦੀ-ਸ਼ੁਰੂ;  // Skip all entries for this range.
        }
    } ਨਹੀਂਤਾਂ {
        // List of values.
        int32_t ਮੁੱਲਾਂ_ਸ਼ੁਰੂ=ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ;
        int32_t name_ਸਮੂਹ_Offsets_ਸ਼ੁਰੂ=ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+ਅੰਕ_Ranges-0x10;
        ਕਰੋ {
            int32_t v=ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ];
            ਜੇ(value<v) {
                ਰੋਕੋ;
            }
            ਜੇ(value==v) {
                ਵਾਪਸ ਮੁੱਲ_Maps[name_ਸਮੂਹ_Offsets_ਸ਼ੁਰੂ+ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ-ਮੁੱਲਾਂ_ਸ਼ੁਰੂ];
            }
        } ਜਦੋਂ(++ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ<name_ਸਮੂਹ_Offsets_ਸ਼ੁਰੂ);
    }
    ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ *Prop_Name_ਅੰਕੜੇ::getName(ਅਚਲ ਅੱਖਰ *name_ਸਮੂਹ, int32_t name_ਸੂਚਕਾਂਕ) {
    int32_t ਅੰਕ_ਨਾਂ=*name_ਸਮੂਹ++;
    ਜੇ(name_ਸੂਚਕਾਂਕ<0 || ਅੰਕ_ਨਾਂ<=name_ਸੂਚਕਾਂਕ) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // Skip nameIndex names.
    ਲਈ(; name_ਸੂਚਕਾਂਕ>0; --name_ਸੂਚਕਾਂਕ) {
        name_ਸਮੂਹ=uprv_strchr(name_ਸਮੂਹ, 0)+1;
    }
    ਜੇ(*name_ਸਮੂਹ==0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ਵਾਪਸ name_ਸਮੂਹ;
}

UBool Prop_Name_ਅੰਕੜੇ::ਸ਼ਾਮਲ_ਹੈ_Name(ਬਾਈਟਾਂ_Trie &trie, ਅਚਲ ਅੱਖਰ *name) {
    ਜੇ(name==ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ=USTRINGTRIE_NO_ਮੁੱਲ;
    ਅੱਖਰ c;
    ਜਦੋਂ((c=*name++)!=0) {
        c=uprv_inv_Char_ਮੰਜ਼ਿਲ_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ਜੇ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ਜਾਰੀ;
        }
        ਜੇ(!USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਨਤੀਜਾ = trie.next(static_cast<uint8_t>(c));
    }
    ਵਾਪਸ USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ);
}

ਅਚਲ ਅੱਖਰ *Prop_Name_ਅੰਕੜੇ::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ=ਲੱਭੋ_Property(property);
    ਜੇ(ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ==0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;  // Not a known property.
    }
    ਵਾਪਸ getName(nameGroups+ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ], nameChoice);
}

ਅਚਲ ਅੱਖਰ *Prop_Name_ਅੰਕੜੇ::get_Property_ਮੁੱਲ_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ=ਲੱਭੋ_Property(property);
    ਜੇ(ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ==0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;  // Not a known property.
    }
    int32_t name_ਸਮੂਹ_ਵਿੱਥ=ਲੱਭੋ_Property_ਮੁੱਲ_Name_ਸਮੂਹ(ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+1], value);
    ਜੇ(name_ਸਮੂਹ_ਵਿੱਥ==0) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ getName(nameGroups+name_ਸਮੂਹ_ਵਿੱਥ, nameChoice);
}

int32_t Prop_Name_ਅੰਕੜੇ::get_Property_Or_ਮੁੱਲ_Enum(int32_t ਬਾਈਟਾਂ_Trie_ਵਿੱਥ, ਅਚਲ ਅੱਖਰ *alias) {
    ਬਾਈਟਾਂ_Trie trie(ਬਾਈਟਾਂ_Tries+ਬਾਈਟਾਂ_Trie_ਵਿੱਥ);
    ਜੇ(ਸ਼ਾਮਲ_ਹੈ_Name(trie, alias)) {
        ਵਾਪਸ trie.get_ਮੁੱਲ();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ UCHAR_INVALID_ਕੋਡ;
    }
}

int32_t Prop_Name_ਅੰਕੜੇ::getPropertyEnum(ਅਚਲ ਅੱਖਰ *alias) {
    ਵਾਪਸ get_Property_Or_ਮੁੱਲ_Enum(0, alias);
}

int32_t Prop_Name_ਅੰਕੜੇ::get_Property_ਮੁੱਲ_Enum(int32_t property, ਅਚਲ ਅੱਖਰ *alias) {
    int32_t ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ=ਲੱਭੋ_Property(property);
    ਜੇ(ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ==0) {
        ਵਾਪਸ UCHAR_INVALID_ਕੋਡ;  // Not a known property.
    }
    ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ=ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ+1];
    ਜੇ(ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ==0) {
        ਵਾਪਸ UCHAR_INVALID_ਕੋਡ;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ਵਾਪਸ get_Property_Or_ਮੁੱਲ_Enum(ਮੁੱਲ_Maps[ਮੁੱਲ_Map_ਸੂਚਕਾਂਕ], alias);
}
U_NAMESPACE_ਸਿਰਾ

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ਵਾਪਸ Prop_Name_ਅੰਕੜੇ::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ਅਚਲ ਅੱਖਰ* alias) {
    U_NAMESPACE_USE
    ਵਾਪਸ (UProperty)Prop_Name_ਅੰਕੜੇ::getPropertyEnum(alias);
}

U_CAPI ਅਚਲ ਅੱਖਰ* U_EXPORT2
u_get_Property_ਮੁੱਲ_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ਵਾਪਸ Prop_Name_ਅੰਕੜੇ::get_Property_ਮੁੱਲ_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_ਮੁੱਲ_Enum(UProperty property,
                       ਅਚਲ ਅੱਖਰ* alias) {
    U_NAMESPACE_USE
    ਵਾਪਸ Prop_Name_ਅੰਕੜੇ::get_Property_ਮੁੱਲ_Enum(property, alias);
}

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uscript_getName(UScript_ਕੋਡ script_ਕੋਡ){
    ਵਾਪਸ u_get_Property_ਮੁੱਲ_Name(UCHAR_SCRIPT, script_ਕੋਡ,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ਅਚਲ ਅੱਖਰ*  U_EXPORT2
uscript_getShortName(UScript_ਕੋਡ script_ਕੋਡ){
    ਵਾਪਸ u_get_Property_ਮੁੱਲ_Name(UCHAR_SCRIPT, script_ਕੋਡ,
                                  U_SHORT_PROPERTY_NAME);
}
