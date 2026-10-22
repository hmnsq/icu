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

#تعریف_کړه INCLUDED_FROM_PROPNAME_CPP
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
uprv_compare_ASCIIProperty_نومونه(ثابت حرف *name1, ثابت حرف *name2) {
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
uprv_compare_EBCDICProperty_نومونه(ثابت حرف *name1, ثابت حرف *name2) {
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

U_CDECL_پای_ټکی

U_NAMESPACE_BEGIN

int32_t Prop_Name_معلومات::پیدا_کړه_Property(int32_t property) {
    int32_t i=1;  // valueMaps index, initially after numRanges
    لكل(int32_t عدد_Ranges=ارزښت_Maps[0]; عدد_Ranges>0; --عدد_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t پیل=ارزښت_Maps[i];
        int32_t حد=ارزښت_Maps[i+1];
        i+=2;
        إذا(property<پیل) {
            اكسر;
        }
        إذا(property<حد) {
            أرجع i+(property-پیل)*2;
        }
        i+=(حد-پیل)*2;  // Skip all entries for this range.
    }
    أرجع 0;
}

int32_t Prop_Name_معلومات::پیدا_کړه_Property_ارزښت_Name_ډله(int32_t ارزښت_Map_شاخص, int32_t value) {
    إذا(ارزښت_Map_شاخص==0) {
        أرجع 0;  // The property does not have named values.
    }
    ++ارزښت_Map_شاخص;  // Skip the BytesTrie offset.
    int32_t عدد_Ranges=ارزښت_Maps[ارزښت_Map_شاخص++];
    إذا(عدد_Ranges<0x10) {
        // Ranges of values.
        لكل(; عدد_Ranges>0; --عدد_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t پیل=ارزښت_Maps[ارزښت_Map_شاخص];
            int32_t حد=ارزښت_Maps[ارزښت_Map_شاخص+1];
            ارزښت_Map_شاخص+=2;
            إذا(value<پیل) {
                اكسر;
            }
            إذا(value<حد) {
                أرجع ارزښت_Maps[ارزښت_Map_شاخص+value-پیل];
            }
            ارزښت_Map_شاخص+=حد-پیل;  // Skip all entries for this range.
        }
    } وإلا {
        // List of values.
        int32_t ارزښتونه_پیل=ارزښت_Map_شاخص;
        int32_t name_ډله_Offsets_پیل=ارزښت_Map_شاخص+عدد_Ranges-0x10;
        افعل {
            int32_t v=ارزښت_Maps[ارزښت_Map_شاخص];
            إذا(value<v) {
                اكسر;
            }
            إذا(value==v) {
                أرجع ارزښت_Maps[name_ډله_Offsets_پیل+ارزښت_Map_شاخص-ارزښتونه_پیل];
            }
        } طالما(++ارزښت_Map_شاخص<name_ډله_Offsets_پیل);
    }
    أرجع 0;
}

ثابت حرف *Prop_Name_معلومات::getName(ثابت حرف *name_ډله, int32_t name_شاخص) {
    int32_t عدد_نومونه=*name_ډله++;
    إذا(name_شاخص<0 || عدد_نومونه<=name_شاخص) {
        أرجع تش_اشاره;
    }
    // Skip nameIndex names.
    لكل(; name_شاخص>0; --name_شاخص) {
        name_ډله=uprv_strchr(name_ډله, 0)+1;
    }
    إذا(*name_ډله==0) {
        أرجع تش_اشاره;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    أرجع name_ډله;
}

UBool Prop_Name_معلومات::لري_Name(بایټونه_Trie &trie, ثابت حرف *name) {
    إذا(name==تش_اشاره) {
        أرجع خطأ;
    }
    UString_Trie_پایله پایله=USTRINGTRIE_NO_ارزښت;
    حرف c;
    طالما((c=*name++)!=0) {
        c=uprv_inv_Char_منزل_Lowercase_Ascii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        إذا(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            استمر;
        }
        إذا(!USTRINGTRIE_HAS_بل(پایله)) {
            أرجع خطأ;
        }
        پایله = trie.next(static_cast<uint8_t>(c));
    }
    أرجع USTRINGTRIE_HAS_ارزښت(پایله);
}

ثابت حرف *Prop_Name_معلومات::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t ارزښت_Map_شاخص=پیدا_کړه_Property(property);
    إذا(ارزښت_Map_شاخص==0) {
        أرجع تش_اشاره;  // Not a known property.
    }
    أرجع getName(nameGroups+ارزښت_Maps[ارزښت_Map_شاخص], nameChoice);
}

ثابت حرف *Prop_Name_معلومات::get_Property_ارزښت_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t ارزښت_Map_شاخص=پیدا_کړه_Property(property);
    إذا(ارزښت_Map_شاخص==0) {
        أرجع تش_اشاره;  // Not a known property.
    }
    int32_t name_ډله_دننه_واټن=پیدا_کړه_Property_ارزښت_Name_ډله(ارزښت_Maps[ارزښت_Map_شاخص+1], value);
    إذا(name_ډله_دننه_واټن==0) {
        أرجع تش_اشاره;
    }
    أرجع getName(nameGroups+name_ډله_دننه_واټن, nameChoice);
}

int32_t Prop_Name_معلومات::get_Property_Or_ارزښت_Enum(int32_t بایټونه_Trie_دننه_واټن, ثابت حرف *alias) {
    بایټونه_Trie trie(بایټونه_Tries+بایټونه_Trie_دننه_واټن);
    إذا(لري_Name(trie, alias)) {
        أرجع trie.get_ارزښت();
    } وإلا {
        أرجع UCHAR_INVALID_کوډ;
    }
}

int32_t Prop_Name_معلومات::getPropertyEnum(ثابت حرف *alias) {
    أرجع get_Property_Or_ارزښت_Enum(0, alias);
}

int32_t Prop_Name_معلومات::get_Property_ارزښت_Enum(int32_t property, ثابت حرف *alias) {
    int32_t ارزښت_Map_شاخص=پیدا_کړه_Property(property);
    إذا(ارزښت_Map_شاخص==0) {
        أرجع UCHAR_INVALID_کوډ;  // Not a known property.
    }
    ارزښت_Map_شاخص=ارزښت_Maps[ارزښت_Map_شاخص+1];
    إذا(ارزښت_Map_شاخص==0) {
        أرجع UCHAR_INVALID_کوډ;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    أرجع get_Property_Or_ارزښت_Enum(ارزښت_Maps[ارزښت_Map_شاخص], alias);
}
U_NAMESPACE_پای_ټکی

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    أرجع Prop_Name_معلومات::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع (UProperty)Prop_Name_معلومات::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_get_Property_ارزښت_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    أرجع Prop_Name_معلومات::get_Property_ارزښت_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_ارزښت_Enum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع Prop_Name_معلومات::get_Property_ارزښت_Enum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScript_کوډ script_کوډ){
    أرجع u_get_Property_ارزښت_Name(UCHAR_SCRIPT, script_کوډ,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScript_کوډ script_کوډ){
    أرجع u_get_Property_ارزښت_Name(UCHAR_SCRIPT, script_کوډ,
                                  U_SHORT_PROPERTY_NAME);
}
