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
#ଅନ୍ତର୍ଭାଵ "propname.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uchar.h"
#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uscript.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uarrsort.h"
#ଅନ୍ତର୍ଭାଵ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#ଅନ୍ତର୍ଭାଵ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
getASCIIPropertyNameChar(ଅଚଳ ଅକ୍ଷର *name) {
    int32_t i;
    ଅକ୍ଷର c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    ପାଇଁ(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    ଯଦି(c!=0) {
        ଫେରନ୍ତୁ (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ int32_t
getEBCDICPropertyNameChar(ଅଚଳ ଅକ୍ଷର *name) {
    int32_t i;
    ଅକ୍ଷର c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    ପାଇଁ(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    ଯଦି(c!=0) {
        ଫେରନ୍ତୁ (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ i<<8;
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
uprv_compareASCIIPropertyNames(ଅଚଳ ଅକ୍ଷର *name1, ଅଚଳ ଅକ୍ଷର *name2) {
    int32_t rc, r1, r2;

    ପାଇଁ(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ଯଦି(((r1|r2)&0xff)==0) {
            ଫେରନ୍ତୁ 0;
        }

        /* Compare the lowercased characters */
        ଯଦି(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ଯଦି(rc!=0) {
                ଫେରନ୍ତୁ rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(ଅଚଳ ଅକ୍ଷର *name1, ଅଚଳ ଅକ୍ଷର *name2) {
    int32_t rc, r1, r2;

    ପାଇଁ(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        ଯଦି(((r1|r2)&0xff)==0) {
            ଫେରନ୍ତୁ 0;
        }

        /* Compare the lowercased characters */
        ଯଦି(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            ଯଦି(rc!=0) {
                ଫେରନ୍ତୁ rc;
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
    ପାଇଁ(int32_t ଅଙ୍କ_Ranges=ମୂଲ୍ୟ_Maps[0]; ଅଙ୍କ_Ranges>0; --ଅଙ୍କ_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=ମୂଲ୍ୟ_Maps[i];
        int32_t limit=ମୂଲ୍ୟ_Maps[i+1];
        i+=2;
        ଯଦି(property<start) {
            ଭାଙ୍ଗ;
        }
        ଯଦି(property<limit) {
            ଫେରନ୍ତୁ i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    ଫେରନ୍ତୁ 0;
}

int32_t PropNameData::find_Property_ମୂଲ୍ୟ_Name_Group(int32_t ମୂଲ୍ୟ_Map_Index, int32_t value) {
    ଯଦି(ମୂଲ୍ୟ_Map_Index==0) {
        ଫେରନ୍ତୁ 0;  // The property does not have named values.
    }
    ++ମୂଲ୍ୟ_Map_Index;  // Skip the BytesTrie offset.
    int32_t ଅଙ୍କ_Ranges=ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index++];
    ଯଦି(ଅଙ୍କ_Ranges<0x10) {
        // Ranges of values.
        ପାଇଁ(; ଅଙ୍କ_Ranges>0; --ଅଙ୍କ_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index];
            int32_t limit=ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index+1];
            ମୂଲ୍ୟ_Map_Index+=2;
            ଯଦି(value<start) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(value<limit) {
                ଫେରନ୍ତୁ ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index+value-start];
            }
            ମୂଲ୍ୟ_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } ଅନ୍ୟଥା {
        // List of values.
        int32_t ମୂଲ୍ୟଗୁଡ଼ିକ_Start=ମୂଲ୍ୟ_Map_Index;
        int32_t nameGroupOffsetsStart=ମୂଲ୍ୟ_Map_Index+ଅଙ୍କ_Ranges-0x10;
        କର {
            int32_t v=ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index];
            ଯଦି(value<v) {
                ଭାଙ୍ଗ;
            }
            ଯଦି(value==v) {
                ଫେରନ୍ତୁ ମୂଲ୍ୟ_Maps[nameGroupOffsetsStart+ମୂଲ୍ୟ_Map_Index-ମୂଲ୍ୟଗୁଡ଼ିକ_Start];
            }
        } ଯେତେବେଳେ(++ମୂଲ୍ୟ_Map_Index<nameGroupOffsetsStart);
    }
    ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଅକ୍ଷର *PropNameData::getName(ଅଚଳ ଅକ୍ଷର *nameGroup, int32_t nameIndex) {
    int32_t ଅଙ୍କ_Names=*nameGroup++;
    ଯଦି(nameIndex<0 || ଅଙ୍କ_Names<=nameIndex) {
        ଫେରନ୍ତୁ nullptr;
    }
    // Skip nameIndex names.
    ପାଇଁ(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    ଯଦି(*nameGroup==0) {
        ଫେରନ୍ତୁ nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    ଫେରନ୍ତୁ nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, ଅଚଳ ଅକ୍ଷର *name) {
    ଯଦି(name==nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    UStringTrieResult result=USTRINGTRIE_NO_ମୂଲ୍ୟ;
    ଅକ୍ଷର c;
    ଯେତେବେଳେ((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        ଯଦି(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ଜାରି;
        }
        ଯଦି(!USTRINGTRIE_HAS_NEXT(result)) {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    ଫେରନ୍ତୁ USTRINGTRIE_HAS_ମୂଲ୍ୟ(result);
}

ଅଚଳ ଅକ୍ଷର *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t ମୂଲ୍ୟ_Map_Index=findProperty(property);
    ଯଦି(ମୂଲ୍ୟ_Map_Index==0) {
        ଫେରନ୍ତୁ nullptr;  // Not a known property.
    }
    ଫେରନ୍ତୁ getName(nameGroups+ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index], nameChoice);
}

ଅଚଳ ଅକ୍ଷର *PropNameData::get_Property_ମୂଲ୍ୟ_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t ମୂଲ୍ୟ_Map_Index=findProperty(property);
    ଯଦି(ମୂଲ୍ୟ_Map_Index==0) {
        ଫେରନ୍ତୁ nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_ମୂଲ୍ୟ_Name_Group(ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index+1], value);
    ଯଦି(nameGroupOffset==0) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_ମୂଲ୍ୟ_Enum(int32_t bytesTrieOffset, ଅଚଳ ଅକ୍ଷର *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    ଯଦି(containsName(trie, alias)) {
        ଫେରନ୍ତୁ trie.get_ମୂଲ୍ୟ();
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(ଅଚଳ ଅକ୍ଷର *alias) {
    ଫେରନ୍ତୁ get_Property_Or_ମୂଲ୍ୟ_Enum(0, alias);
}

int32_t PropNameData::get_Property_ମୂଲ୍ୟ_Enum(int32_t property, ଅଚଳ ଅକ୍ଷର *alias) {
    int32_t ମୂଲ୍ୟ_Map_Index=findProperty(property);
    ଯଦି(ମୂଲ୍ୟ_Map_Index==0) {
        ଫେରନ୍ତୁ UCHAR_INVALID_CODE;  // Not a known property.
    }
    ମୂଲ୍ୟ_Map_Index=ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index+1];
    ଯଦି(ମୂଲ୍ୟ_Map_Index==0) {
        ଫେରନ୍ତୁ UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    ଫେରନ୍ତୁ get_Property_Or_ମୂଲ୍ୟ_Enum(ମୂଲ୍ୟ_Maps[ମୂଲ୍ୟ_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ଫେରନ୍ତୁ PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(ଅଚଳ ଅକ୍ଷର* alias) {
    U_NAMESPACE_USE
    ଫେରନ୍ତୁ (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI ଅଚଳ ଅକ୍ଷର* U_EXPORT2
u_get_Property_ମୂଲ୍ୟ_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    ଫେରନ୍ତୁ PropNameData::get_Property_ମୂଲ୍ୟ_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_ମୂଲ୍ୟ_Enum(UProperty property,
                       ଅଚଳ ଅକ୍ଷର* alias) {
    U_NAMESPACE_USE
    ଫେରନ୍ତୁ PropNameData::get_Property_ମୂଲ୍ୟ_Enum(property, alias);
}

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    ଫେରନ୍ତୁ u_get_Property_ମୂଲ୍ୟ_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI ଅଚଳ ଅକ୍ଷର*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    ଫେରନ୍ତୁ u_get_Property_ମୂଲ୍ୟ_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
