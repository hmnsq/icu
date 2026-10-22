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

    𐡄𐡍(c!=0) {
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

    𐡄𐡍(c!=0) {
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
uprv_compareASCIIPropertyNames(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    لكل(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        𐡄𐡍(((r1|r2)&0xff)==0) {
            أرجع 0;
        }

        /* Compare the lowercased characters */
        𐡄𐡍(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            𐡄𐡍(rc!=0) {
                أرجع rc;
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
        𐡄𐡍(((r1|r2)&0xff)==0) {
            أرجع 0;
        }

        /* Compare the lowercased characters */
        𐡄𐡍(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            𐡄𐡍(rc!=0) {
                أرجع rc;
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
    لكل(int32_t numRanges=valueMaps[0]; numRanges>0; --numRanges) {
        // Read and skip the start and limit of this range.
        int32_t start=valueMaps[i];
        int32_t limit=valueMaps[i+1];
        i+=2;
        𐡄𐡍(property<start) {
            اكسر;
        }
        𐡄𐡍(property<limit) {
            أرجع i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    أرجع 0;
}

int32_t PropNameData::findPropertyValueNameGroup(int32_t valueMapIndex, int32_t value) {
    𐡄𐡍(valueMapIndex==0) {
        أرجع 0;  // The property does not have named values.
    }
    ++valueMapIndex;  // Skip the BytesTrie offset.
    int32_t numRanges=valueMaps[valueMapIndex++];
    𐡄𐡍(numRanges<0x10) {
        // Ranges of values.
        لكل(; numRanges>0; --numRanges) {
            // Read and skip the start and limit of this range.
            int32_t start=valueMaps[valueMapIndex];
            int32_t limit=valueMaps[valueMapIndex+1];
            valueMapIndex+=2;
            𐡄𐡍(value<start) {
                اكسر;
            }
            𐡄𐡍(value<limit) {
                أرجع valueMaps[valueMapIndex+value-start];
            }
            valueMapIndex+=limit-start;  // Skip all entries for this range.
        }
    } وإلا {
        // List of values.
        int32_t valuesStart=valueMapIndex;
        int32_t nameGroupOffsetsStart=valueMapIndex+numRanges-0x10;
        𐡏𐡁𐡃 {
            int32_t v=valueMaps[valueMapIndex];
            𐡄𐡍(value<v) {
                اكسر;
            }
            𐡄𐡍(value==v) {
                أرجع valueMaps[nameGroupOffsetsStart+valueMapIndex-valuesStart];
            }
        } طالما(++valueMapIndex<nameGroupOffsetsStart);
    }
    أرجع 0;
}

ثابت حرف *PropNameData::getName(ثابت حرف *nameGroup, int32_t nameIndex) {
    int32_t numNames=*nameGroup++;
    𐡄𐡍(nameIndex<0 || numNames<=nameIndex) {
        أرجع nullptr;
    }
    // Skip nameIndex names.
    لكل(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    𐡄𐡍(*nameGroup==0) {
        أرجع nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    أرجع nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ثابت حرف *name) {
    𐡄𐡍(name==nullptr) {
        أرجع خطأ;
    }
    UStringTrieResult result=USTRINGTRIE_NO_VALUE;
    حرف c;
    طالما((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        𐡄𐡍(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            استمر;
        }
        𐡄𐡍(!USTRINGTRIE_HAS_NEXT(result)) {
            أرجع خطأ;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    أرجع USTRINGTRIE_HAS_VALUE(result);
}

ثابت حرف *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    𐡄𐡍(valueMapIndex==0) {
        أرجع nullptr;  // Not a known property.
    }
    أرجع getName(nameGroups+valueMaps[valueMapIndex], nameChoice);
}

ثابت حرف *PropNameData::getPropertyValueName(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    𐡄𐡍(valueMapIndex==0) {
        أرجع nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=findPropertyValueNameGroup(valueMaps[valueMapIndex+1], value);
    𐡄𐡍(nameGroupOffset==0) {
        أرجع nullptr;
    }
    أرجع getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::getPropertyOrValueEnum(int32_t bytesTrieOffset, ثابت حرف *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    𐡄𐡍(containsName(trie, alias)) {
        أرجع trie.getValue();
    } وإلا {
        أرجع UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ثابت حرف *alias) {
    أرجع getPropertyOrValueEnum(0, alias);
}

int32_t PropNameData::getPropertyValueEnum(int32_t property, ثابت حرف *alias) {
    int32_t valueMapIndex=findProperty(property);
    𐡄𐡍(valueMapIndex==0) {
        أرجع UCHAR_INVALID_CODE;  // Not a known property.
    }
    valueMapIndex=valueMaps[valueMapIndex+1];
    𐡄𐡍(valueMapIndex==0) {
        أرجع UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    أرجع getPropertyOrValueEnum(valueMaps[valueMapIndex], alias);
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
    أرجع PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyValueName(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    أرجع PropNameData::getPropertyValueName(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_getPropertyValueEnum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    أرجع PropNameData::getPropertyValueEnum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    أرجع u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    أرجع u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
