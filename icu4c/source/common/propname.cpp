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
static inline int32_t
getASCIIPropertyNameChar(const char *name) {
    int32_t i;
    char c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    cho(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    nếu(c!=0) {
        trả_về (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } không_thì {
        trả_về i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
static inline int32_t
getEBCDICPropertyNameChar(const char *name) {
    int32_t i;
    char c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    cho(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    nếu(c!=0) {
        trả_về (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } không_thì {
        trả_về i<<8;
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
uprv_compareASCIIPropertyNames(const char *name1, const char *name2) {
    int32_t rc, r1, r2;

    cho(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        nếu(((r1|r2)&0xff)==0) {
            trả_về 0;
        }

        /* Compare the lowercased characters */
        nếu(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            nếu(rc!=0) {
                trả_về rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(const char *name1, const char *name2) {
    int32_t rc, r1, r2;

    cho(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        nếu(((r1|r2)&0xff)==0) {
            trả_về 0;
        }

        /* Compare the lowercased characters */
        nếu(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            nếu(rc!=0) {
                trả_về rc;
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
    cho(int32_t numRanges=valueMaps[0]; numRanges>0; --numRanges) {
        // Read and skip the start and limit of this range.
        int32_t start=valueMaps[i];
        int32_t limit=valueMaps[i+1];
        i+=2;
        nếu(property<start) {
            break;
        }
        nếu(property<limit) {
            trả_về i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    trả_về 0;
}

int32_t PropNameData::findPropertyValueNameGroup(int32_t valueMapIndex, int32_t value) {
    nếu(valueMapIndex==0) {
        trả_về 0;  // The property does not have named values.
    }
    ++valueMapIndex;  // Skip the BytesTrie offset.
    int32_t numRanges=valueMaps[valueMapIndex++];
    nếu(numRanges<0x10) {
        // Ranges of values.
        cho(; numRanges>0; --numRanges) {
            // Read and skip the start and limit of this range.
            int32_t start=valueMaps[valueMapIndex];
            int32_t limit=valueMaps[valueMapIndex+1];
            valueMapIndex+=2;
            nếu(value<start) {
                break;
            }
            nếu(value<limit) {
                trả_về valueMaps[valueMapIndex+value-start];
            }
            valueMapIndex+=limit-start;  // Skip all entries for this range.
        }
    } không_thì {
        // List of values.
        int32_t valuesStart=valueMapIndex;
        int32_t nameGroupOffsetsStart=valueMapIndex+numRanges-0x10;
        do {
            int32_t v=valueMaps[valueMapIndex];
            nếu(value<v) {
                break;
            }
            nếu(value==v) {
                trả_về valueMaps[nameGroupOffsetsStart+valueMapIndex-valuesStart];
            }
        } trong_khi(++valueMapIndex<nameGroupOffsetsStart);
    }
    trả_về 0;
}

const char *PropNameData::getName(const char *nameGroup, int32_t nameIndex) {
    int32_t numNames=*nameGroup++;
    nếu(nameIndex<0 || numNames<=nameIndex) {
        trả_về nullptr;
    }
    // Skip nameIndex names.
    cho(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    nếu(*nameGroup==0) {
        trả_về nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    trả_về nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, const char *name) {
    nếu(name==nullptr) {
        trả_về false;
    }
    UStringTrieResult result=USTRINGTRIE_NO_VALUE;
    char c;
    trong_khi((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        nếu(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            continue;
        }
        nếu(!USTRINGTRIE_HAS_NEXT(result)) {
            trả_về false;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    trả_về USTRINGTRIE_HAS_VALUE(result);
}

const char *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    nếu(valueMapIndex==0) {
        trả_về nullptr;  // Not a known property.
    }
    trả_về getName(nameGroups+valueMaps[valueMapIndex], nameChoice);
}

const char *PropNameData::getPropertyValueName(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t valueMapIndex=findProperty(property);
    nếu(valueMapIndex==0) {
        trả_về nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=findPropertyValueNameGroup(valueMaps[valueMapIndex+1], value);
    nếu(nameGroupOffset==0) {
        trả_về nullptr;
    }
    trả_về getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::getPropertyOrValueEnum(int32_t bytesTrieOffset, const char *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    nếu(containsName(trie, alias)) {
        trả_về trie.getValue();
    } không_thì {
        trả_về UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(const char *alias) {
    trả_về getPropertyOrValueEnum(0, alias);
}

int32_t PropNameData::getPropertyValueEnum(int32_t property, const char *alias) {
    int32_t valueMapIndex=findProperty(property);
    nếu(valueMapIndex==0) {
        trả_về UCHAR_INVALID_CODE;  // Not a known property.
    }
    valueMapIndex=valueMaps[valueMapIndex+1];
    nếu(valueMapIndex==0) {
        trả_về UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    trả_về getPropertyOrValueEnum(valueMaps[valueMapIndex], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI const char* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    trả_về PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(const char* alias) {
    U_NAMESPACE_USE
    trả_về (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI const char* U_EXPORT2
u_getPropertyValueName(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    trả_về PropNameData::getPropertyValueName(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_getPropertyValueEnum(UProperty property,
                       const char* alias) {
    U_NAMESPACE_USE
    trả_về PropNameData::getPropertyValueEnum(property, alias);
}

U_CAPI const char*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    trả_về u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI const char*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    trả_về u_getPropertyValueName(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
