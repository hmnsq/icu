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
    עבור(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    אם(c!=0) {
        החזר (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } אחרת {
        החזר i<<8;
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
    עבור(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    אם(c!=0) {
        החזר (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } אחרת {
        החזר i<<8;
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

    עבור(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        אם(((r1|r2)&0xff)==0) {
            החזר 0;
        }

        /* Compare the lowercased characters */
        אם(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            אם(rc!=0) {
                החזר rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ثابت حرف *name1, ثابت حرف *name2) {
    int32_t rc, r1, r2;

    עבור(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        אם(((r1|r2)&0xff)==0) {
            החזר 0;
        }

        /* Compare the lowercased characters */
        אם(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            אם(rc!=0) {
                החזר rc;
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
    עבור(int32_t numRanges=valueMaps[0]; numRanges>0; --numRanges) {
        // Read and skip the start and limit of this range.
        int32_t start=valueMaps[i];
        int32_t limit=valueMaps[i+1];
        i+=2;
        אם(property<start) {
            اكسر;
        }
        אם(property<limit) {
            החזר i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    החזר 0;
}

int32_t PropNameData::findPropertyValueNameGroup(int32_t valueMapIndex, int32_t value) {
    אם(valueMapIndex==0) {
        החזר 0;  // The property does not have named values.
    }
    ++valueMapIndex;  // Skip the BytesTrie offset.
    int32_t numRanges=valueMaps[valueMapIndex++];
    אם(numRanges<0x10) {
        // Ranges of values.
        עבור(; numRanges>0; --numRanges) {
            // Read and skip the start and limit of this range.
            int32_t start=valueMaps[valueMapIndex];
            int32_t limit=valueMaps[valueMapIndex+1];
            valueMapIndex+=2;
            אם(value<start) {
                اكسر;
            }
            אם(value<limit) {
                החזר valueMaps[valueMapIndex+value-start];
            }
            valueMapIndex+=limit-start;  // Skip all entries for this range.
        }
    } אחרת {
        // List of values.
        int32_t valuesStart=valueMapIndex;
        int32_t nameGroupOffsetsStart=valueMapIndex+numRanges-0x10;
        עשה {
            int32_t v=valueMaps[valueMapIndex];
            אם(value<v) {
                اكسر;
            }
            אם(value==v) {
                החזר valueMaps[nameGroupOffsetsStart+valueMapIndex-valuesStart];
            }
        } כלעוד(++valueMapIndex<nameGroupOffsetsStart);
    }
    החזר 0;
}

ثابت حرف *PropNameData::getName(ثابت حرف *nameGroup, int32_t nameIndex) {
    int32_t numNames=*nameGroup++;
    אם(nameIndex<0 || numNames<=nameIndex) {
        החזר nullptr;
    }
    // Skip nameIndex names.
    עבור(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    אם(*nameGroup==0) {
        החזר nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    החזר nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ثابت حرف *name) {
    אם(name==nullptr) {
        החזר שקר;
    }
    UStringTrieResult result=USTRINGTRIE_NO_VALUE;
    حرف c;
    כלעוד((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        אם(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            استمر;
        }
        אם(!USTRINGTRIE_HAS_NEXT(result)) {
            החזר שקר;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    החזר USTRINGTRIE_HAS_VALUE(result);
}

ثابت حرف *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    אם(valueMapIndex==0) {
        החזר nullptr;  // Not a known property.
    }
    החזר getName(nameGroups+valueMaps[valueMapIndex], nameChoice);
}

ثابت حرف *PropNameData::getPropertyValueName(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    אם(valueMapIndex==0) {
        החזר nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=findPropertyValueNameGroup(valueMaps[valueMapIndex+1], value);
    אם(nameGroupOffset==0) {
        החזר nullptr;
    }
    החזר getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::getPropertyOrValueEnum(int32_t bytesTrieOffset, ثابت حرف *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    אם(containsName(trie, alias)) {
        החזר trie.getValue();
    } אחרת {
        החזר UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ثابت حرف *alias) {
    החזר getPropertyOrValueEnum(0, alias);
}

int32_t PropNameData::getPropertyValueEnum(int32_t property, ثابت حرف *alias) {
    int32_t valueMapIndex=findProperty(property);
    אם(valueMapIndex==0) {
        החזר UCHAR_INVALID_CODE;  // Not a known property.
    }
    valueMapIndex=valueMaps[valueMapIndex+1];
    אם(valueMapIndex==0) {
        החזר UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    החזר getPropertyOrValueEnum(valueMaps[valueMapIndex], alias);
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
    החזר PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ثابت حرف* alias) {
    U_NAMESPACE_USE
    החזר (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ثابت حرف* U_EXPORT2
u_getPropertyValueName(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    החזר PropNameData::getPropertyValueName(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_getPropertyValueEnum(UProperty property,
                       ثابت حرف* alias) {
    U_NAMESPACE_USE
    החזר PropNameData::getPropertyValueEnum(property, alias);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    החזר u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ثابت حرف*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    החזר u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
