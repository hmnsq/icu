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
#ಅನ್ತರ್ಭಾವ "propname.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uarrsort.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"

#ವ್ಯಾಖ್ಯಾನಿಸು INCLUDED_FROM_PROPNAME_CPP
#ಅನ್ತರ್ಭಾವ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
getASCIIPropertyNameChar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
    int32_t i;
    ಅಕ್ಷರ c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    ಗಾಗಿ(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ಆದರೆ(c!=0) {
        ಹಿಂತಿರುಗಿ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ int32_t
getEBCDICPropertyNameChar(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
    int32_t i;
    ಅಕ್ಷರ c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    ಗಾಗಿ(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ಆದರೆ(c!=0) {
        ಹಿಂತಿರುಗಿ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ i<<8;
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
uprv_compare_ASCIIProperty_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name2) {
    int32_t rc, r1, r2;

    ಗಾಗಿ(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ಆದರೆ(((r1|r2)&0xff)==0) {
            ಹಿಂತಿರುಗಿ 0;
        }

        /* Compare the lowercased characters */
        ಆದರೆ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ಆದರೆ(rc!=0) {
                ಹಿಂತಿರುಗಿ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compare_EBCDICProperty_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name1, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name2) {
    int32_t rc, r1, r2;

    ಗಾಗಿ(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ಆದರೆ(((r1|r2)&0xff)==0) {
            ಹಿಂತಿರುಗಿ 0;
        }

        /* Compare the lowercased characters */
        ಆದರೆ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ಆದರೆ(rc!=0) {
                ಹಿಂತಿರುಗಿ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CDECL_ಕೊನೆ

U_NAMESPACE_BEGIN

int32_t Prop_Name_ದತ್ತಾಂಶ::ಕಂಡುಹಿಡಿ_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    ಗಾಗಿ(int32_t ಅಂಕೆ_Ranges=ಮೌಲ್ಯ_Maps[0]; ಅಂಕೆ_Ranges>0; --ಅಂಕೆ_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t ಆರಂಭ=ಮೌಲ್ಯ_Maps[i];
        int32_t ಎಲ್ಲೆ=ಮೌಲ್ಯ_Maps[i+1];
        i+=2;
        ಆದರೆ(property<ಆರಂಭ) {
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(property<ಎಲ್ಲೆ) {
            ಹಿಂತಿರುಗಿ i+(property-ಆರಂಭ)*2;
        }
        i+=(ಎಲ್ಲೆ-ಆರಂಭ)*2;  // Skip all entries for this range.
    }
    ಹಿಂತಿರುಗಿ 0;
}

int32_t Prop_Name_ದತ್ತಾಂಶ::ಕಂಡುಹಿಡಿ_Property_ಮೌಲ್ಯ_Name_ಗುಂಪು(int32_t ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ, int32_t value) {
    ಆದರೆ(ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ==0) {
        ಹಿಂತಿರುಗಿ 0;  // The property does not have named values.
    }
    ++ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ;  // Skip the BytesTrie offset.
    int32_t ಅಂಕೆ_Ranges=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ++];
    ಆದರೆ(ಅಂಕೆ_Ranges<0x10) {
        // Ranges of values.
        ಗಾಗಿ(; ಅಂಕೆ_Ranges>0; --ಅಂಕೆ_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t ಆರಂಭ=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ];
            int32_t ಎಲ್ಲೆ=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+1];
            ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+=2;
            ಆದರೆ(value<ಆರಂಭ) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(value<ಎಲ್ಲೆ) {
                ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+value-ಆರಂಭ];
            }
            ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+=ಎಲ್ಲೆ-ಆರಂಭ;  // Skip all entries for this range.
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        // List of values.
        int32_t ಮೌಲ್ಯಗಳು_ಆರಂಭ=ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ;
        int32_t name_ಗುಂಪು_Offsets_ಆರಂಭ=ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+ಅಂಕೆ_Ranges-0x10;
        ಮಾಡು {
            int32_t v=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ];
            ಆದರೆ(value<v) {
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ(value==v) {
                ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ_Maps[name_ಗುಂಪು_Offsets_ಆರಂಭ+ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ-ಮೌಲ್ಯಗಳು_ಆರಂಭ];
            }
        } ಆಗ(++ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ<name_ಗುಂಪು_Offsets_ಆರಂಭ);
    }
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Prop_Name_ದತ್ತಾಂಶ::getName(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name_ಗುಂಪು, int32_t name_ಸೂಚ್ಯಂಕ) {
    int32_t ಅಂಕೆ_ಹೆಸರುಗಳು=*name_ಗುಂಪು++;
    ಆದರೆ(name_ಸೂಚ್ಯಂಕ<0 || ಅಂಕೆ_ಹೆಸರುಗಳು<=name_ಸೂಚ್ಯಂಕ) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    // Skip nameIndex names.
    ಗಾಗಿ(; name_ಸೂಚ್ಯಂಕ>0; --name_ಸೂಚ್ಯಂಕ) {
        name_ಗುಂಪು=uprv_strchr(name_ಗುಂಪು, 0)+1;
    }
    ಆದರೆ(*name_ಗುಂಪು==0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ಹಿಂತಿರುಗಿ name_ಗುಂಪು;
}

UBool Prop_Name_ದತ್ತಾಂಶ::ಒಳಗೊಂಡಿದೆ_Name(ಬೈಟುಗಳು_Trie &trie, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
    ಆದರೆ(name==ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ=USTRINGTRIE_NO_ಮೌಲ್ಯ;
    ಅಕ್ಷರ c;
    ಆಗ((c=*name++)!=0) {
        c=uprv_inv_Char_ಗಮ್ಯ_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ಆದರೆ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ಮುಂದುವರಿ;
        }
        ಆದರೆ(!USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಫಲಿತಾಂಶ = trie.next(static_cast<uint8_t>(c));
    }
    ಹಿಂತಿರುಗಿ USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Prop_Name_ದತ್ತಾಂಶ::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ=ಕಂಡುಹಿಡಿ_Property(property);
    ಆದರೆ(ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ==0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;  // Not a known property.
    }
    ಹಿಂತಿರುಗಿ getName(nameGroups+ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ], nameChoice);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Prop_Name_ದತ್ತಾಂಶ::get_Property_ಮೌಲ್ಯ_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ=ಕಂಡುಹಿಡಿ_Property(property);
    ಆದರೆ(ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ==0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;  // Not a known property.
    }
    int32_t name_ಗುಂಪು_ಸ್ಥಾನಾಂತರ=ಕಂಡುಹಿಡಿ_Property_ಮೌಲ್ಯ_Name_ಗುಂಪು(ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+1], value);
    ಆದರೆ(name_ಗುಂಪು_ಸ್ಥಾನಾಂತರ==0) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }
    ಹಿಂತಿರುಗಿ getName(nameGroups+name_ಗುಂಪು_ಸ್ಥಾನಾಂತರ, nameChoice);
}

int32_t Prop_Name_ದತ್ತಾಂಶ::get_Property_Or_ಮೌಲ್ಯ_Enum(int32_t ಬೈಟುಗಳು_Trie_ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias) {
    ಬೈಟುಗಳು_Trie trie(ಬೈಟುಗಳು_Tries+ಬೈಟುಗಳು_Trie_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ(ಒಳಗೊಂಡಿದೆ_Name(trie, alias)) {
        ಹಿಂತಿರುಗಿ trie.get_ಮೌಲ್ಯ();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ UCHAR_INVALID_ಸಂಕೇತ;
    }
}

int32_t Prop_Name_ದತ್ತಾಂಶ::getPropertyEnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias) {
    ಹಿಂತಿರುಗಿ get_Property_Or_ಮೌಲ್ಯ_Enum(0, alias);
}

int32_t Prop_Name_ದತ್ತಾಂಶ::get_Property_ಮೌಲ್ಯ_Enum(int32_t property, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias) {
    int32_t ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ=ಕಂಡುಹಿಡಿ_Property(property);
    ಆದರೆ(ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ==0) {
        ಹಿಂತಿರುಗಿ UCHAR_INVALID_ಸಂಕೇತ;  // Not a known property.
    }
    ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ=ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ+1];
    ಆದರೆ(ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ==0) {
        ಹಿಂತಿರುಗಿ UCHAR_INVALID_ಸಂಕೇತ;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ಹಿಂತಿರುಗಿ get_Property_Or_ಮೌಲ್ಯ_Enum(ಮೌಲ್ಯ_Maps[ಮೌಲ್ಯ_Map_ಸೂಚ್ಯಂಕ], alias);
}
U_NAMESPACE_ಕೊನೆ

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ಹಿಂತಿರುಗಿ Prop_Name_ದತ್ತಾಂಶ::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* alias) {
    U_NAMESPACE_USE
    ಹಿಂತಿರುಗಿ (UProperty)Prop_Name_ದತ್ತಾಂಶ::getPropertyEnum(alias);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* U_EXPORT2
u_get_Property_ಮೌಲ್ಯ_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ಹಿಂತಿರುಗಿ Prop_Name_ದತ್ತಾಂಶ::get_Property_ಮೌಲ್ಯ_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_ಮೌಲ್ಯ_Enum(UProperty property,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* alias) {
    U_NAMESPACE_USE
    ಹಿಂತಿರುಗಿ Prop_Name_ದತ್ತಾಂಶ::get_Property_ಮೌಲ್ಯ_Enum(property, alias);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uscript_getName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ){
    ಹಿಂತಿರುಗಿ u_get_Property_ಮೌಲ್ಯ_Name(UCHAR_SCRIPT, script_ಸಂಕೇತ,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*  U_EXPORT2
uscript_getShortName(UScript_ಸಂಕೇತ script_ಸಂಕೇತ){
    ಹಿಂತಿರುಗಿ u_get_Property_ಮೌಲ್ಯ_Name(UCHAR_SCRIPT, script_ಸಂಕೇತ,
                                  U_SHORT_PROPERTY_NAME);
}
