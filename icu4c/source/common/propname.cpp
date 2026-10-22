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
#include "propname.h"
#include "unicode/uchar.h"
#include "unicode/udata.h"
#include "unicode/uscript.h"
#include "umutex.h"
#include "cmemory.h"
#include "cstring.h"
#include "uarrsort.h"
#include "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#include "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
static anta_hstha int32_t
getASCIIPropertyNameChar(const var_na *name) {
    int32_t i;
    var_na c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    ສຳລັບ(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ຖ້າ(c!=0) {
        ສົ່ງຄືນ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
static anta_hstha int32_t
getEBCDICPropertyNameChar(const var_na *name) {
    int32_t i;
    var_na c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    ສຳລັບ(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ຖ້າ(c!=0) {
        ສົ່ງຄືນ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ i<<8;
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
uprv_compareASCIIPropertyNames(const var_na *name1, const var_na *name2) {
    int32_t rc, r1, r2;

    ສຳລັບ(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ຖ້າ(((r1|r2)&0xff)==0) {
            ສົ່ງຄືນ 0;
        }

        /* Compare the lowercased characters */
        ຖ້າ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ຖ້າ(rc!=0) {
                ສົ່ງຄືນ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(const var_na *name1, const var_na *name2) {
    int32_t rc, r1, r2;

    ສຳລັບ(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ຖ້າ(((r1|r2)&0xff)==0) {
            ສົ່ງຄືນ 0;
        }

        /* Compare the lowercased characters */
        ຖ້າ(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ຖ້າ(rc!=0) {
                ສົ່ງຄືນ rc;
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
    ສຳລັບ(int32_t numRanges=valueMaps[0]; numRanges>0; --numRanges) {
        // Read and skip the start and limit of this range.
        int32_t start=valueMaps[i];
        int32_t limit=valueMaps[i+1];
        i+=2;
        ຖ້າ(property<start) {
            bha_nga;
        }
        ຖ້າ(property<limit) {
            ສົ່ງຄືນ i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    ສົ່ງຄືນ 0;
}

int32_t PropNameData::findPropertyValueNameGroup(int32_t valueMapIndex, int32_t value) {
    ຖ້າ(valueMapIndex==0) {
        ສົ່ງຄືນ 0;  // The property does not have named values.
    }
    ++valueMapIndex;  // Skip the BytesTrie offset.
    int32_t numRanges=valueMaps[valueMapIndex++];
    ຖ້າ(numRanges<0x10) {
        // Ranges of values.
        ສຳລັບ(; numRanges>0; --numRanges) {
            // Read and skip the start and limit of this range.
            int32_t start=valueMaps[valueMapIndex];
            int32_t limit=valueMaps[valueMapIndex+1];
            valueMapIndex+=2;
            ຖ້າ(value<start) {
                bha_nga;
            }
            ຖ້າ(value<limit) {
                ສົ່ງຄືນ valueMaps[valueMapIndex+value-start];
            }
            valueMapIndex+=limit-start;  // Skip all entries for this range.
        }
    } ບໍ່ດັ່ງນັ້ນ {
        // List of values.
        int32_t valuesStart=valueMapIndex;
        int32_t nameGroupOffsetsStart=valueMapIndex+numRanges-0x10;
        do {
            int32_t v=valueMaps[valueMapIndex];
            ຖ້າ(value<v) {
                bha_nga;
            }
            ຖ້າ(value==v) {
                ສົ່ງຄືນ valueMaps[nameGroupOffsetsStart+valueMapIndex-valuesStart];
            }
        } ໃນຂະນະທີ່(++valueMapIndex<nameGroupOffsetsStart);
    }
    ສົ່ງຄືນ 0;
}

const var_na *PropNameData::getName(const var_na *nameGroup, int32_t nameIndex) {
    int32_t numNames=*nameGroup++;
    ຖ້າ(nameIndex<0 || numNames<=nameIndex) {
        ສົ່ງຄືນ nullptr;
    }
    // Skip nameIndex names.
    ສຳລັບ(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    ຖ້າ(*nameGroup==0) {
        ສົ່ງຄືນ nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ສົ່ງຄືນ nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, const var_na *name) {
    ຖ້າ(name==nullptr) {
        ສົ່ງຄືນ false;
    }
    UStringTrieResult result=USTRINGTRIE_NO_VALUE;
    var_na c;
    ໃນຂະນະທີ່((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ຖ້າ(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            continue;
        }
        ຖ້າ(!USTRINGTRIE_HAS_NEXT(result)) {
            ສົ່ງຄືນ false;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    ສົ່ງຄືນ USTRINGTRIE_HAS_VALUE(result);
}

const var_na *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    ຖ້າ(valueMapIndex==0) {
        ສົ່ງຄືນ nullptr;  // Not a known property.
    }
    ສົ່ງຄືນ getName(nameGroups+valueMaps[valueMapIndex], nameChoice);
}

const var_na *PropNameData::getPropertyValueName(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    ຖ້າ(valueMapIndex==0) {
        ສົ່ງຄືນ nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=findPropertyValueNameGroup(valueMaps[valueMapIndex+1], value);
    ຖ້າ(nameGroupOffset==0) {
        ສົ່ງຄືນ nullptr;
    }
    ສົ່ງຄືນ getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::getPropertyOrValueEnum(int32_t bytesTrieOffset, const var_na *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    ຖ້າ(containsName(trie, alias)) {
        ສົ່ງຄືນ trie.getValue();
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(const var_na *alias) {
    ສົ່ງຄືນ getPropertyOrValueEnum(0, alias);
}

int32_t PropNameData::getPropertyValueEnum(int32_t property, const var_na *alias) {
    int32_t valueMapIndex=findProperty(property);
    ຖ້າ(valueMapIndex==0) {
        ສົ່ງຄືນ UCHAR_INVALID_CODE;  // Not a known property.
    }
    valueMapIndex=valueMaps[valueMapIndex+1];
    ຖ້າ(valueMapIndex==0) {
        ສົ່ງຄືນ UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ສົ່ງຄືນ getPropertyOrValueEnum(valueMaps[valueMapIndex], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI const var_na* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ສົ່ງຄືນ PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(const var_na* alias) {
    U_NAMESPACE_USE
    ສົ່ງຄືນ (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI const var_na* U_EXPORT2
u_getPropertyValueName(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ສົ່ງຄືນ PropNameData::getPropertyValueName(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_getPropertyValueEnum(UProperty property,
                       const var_na* alias) {
    U_NAMESPACE_USE
    ສົ່ງຄືນ PropNameData::getPropertyValueEnum(property, alias);
}

U_CAPI const var_na*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    ສົ່ງຄືນ u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI const var_na*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    ສົ່ງຄືນ u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
