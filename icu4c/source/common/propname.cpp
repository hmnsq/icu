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
#અન્તર્ભાવ "propname.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uarrsort.h"
#અન્તર્ભાવ "uinvchar.h"

#define INCLUDED_FROM_PROPNAME_CPP
#અન્તર્ભાવ "propname_data.h"

U_CDECL_BEGIN

/**
 * Get the next non-ignorable ASCII character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
સ્થિર અંતઃરેખ int32_t
getASCIIPropertyNameChar(અચલ અક્ષર *name) {
    int32_t i;
    અક્ષર c;

    /* Ignore delimiters '-', '_', and ASCII White_Space */
    માટે(i=0;
        (c=name[i++])==0x2d || c==0x5f ||
        c==0x20 || (0x09<=c && c<=0x0d);
    ) {}

    જો(c!=0) {
        પાછા (i << 8) | static_cast<uint8_t>(uprv_asciitolower(c));
    } નહીંતર {
        પાછા i<<8;
    }
}

/**
 * Get the next non-ignorable EBCDIC character from a property name
 * and lowercases it.
 * @return ((advance count for the name)<<8)|character
 */
સ્થિર અંતઃરેખ int32_t
getEBCDICPropertyNameChar(અચલ અક્ષર *name) {
    int32_t i;
    અક્ષર c;

    /* Ignore delimiters '-', '_', and EBCDIC White_Space */
    માટે(i=0;
        (c=name[i++])==0x60 || c==0x6d ||
        c==0x40 || c==0x05 || c==0x15 || c==0x25 || c==0x0b || c==0x0c || c==0x0d;
    ) {}

    જો(c!=0) {
        પાછા (i << 8) | static_cast<uint8_t>(uprv_ebcdictolower(c));
    } નહીંતર {
        પાછા i<<8;
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
uprv_compareASCIIPropertyNames(અચલ અક્ષર *name1, અચલ અક્ષર *name2) {
    int32_t rc, r1, r2;

    માટે(;;) {
        r1=getASCIIPropertyNameChar(name1);
        r2=getASCIIPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        જો(((r1|r2)&0xff)==0) {
            પાછા 0;
        }

        /* Compare the lowercased characters */
        જો(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            જો(rc!=0) {
                પાછા rc;
            }
        }

        name1+=r1>>8;
        name2+=r2>>8;
    }
}

U_CAPI int32_t U_EXPORT2
uprv_compareEBCDICPropertyNames(અચલ અક્ષર *name1, અચલ અક્ષર *name2) {
    int32_t rc, r1, r2;

    માટે(;;) {
        r1=getEBCDICPropertyNameChar(name1);
        r2=getEBCDICPropertyNameChar(name2);

        /* If we reach the ends of both strings then they match */
        જો(((r1|r2)&0xff)==0) {
            પાછા 0;
        }

        /* Compare the lowercased characters */
        જો(r1!=r2) {
            rc=(r1&0xff)-(r2&0xff);
            જો(rc!=0) {
                પાછા rc;
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
    માટે(int32_t અંક_Ranges=કિંમત_Maps[0]; અંક_Ranges>0; --અંક_Ranges) {
        // Read and skip the start and limit of this range.
        int32_t start=કિંમત_Maps[i];
        int32_t limit=કિંમત_Maps[i+1];
        i+=2;
        જો(property<start) {
            તોડો;
        }
        જો(property<limit) {
            પાછા i+(property-start)*2;
        }
        i+=(limit-start)*2;  // Skip all entries for this range.
    }
    પાછા 0;
}

int32_t PropNameData::find_Property_કિંમત_Name_Group(int32_t કિંમત_Map_Index, int32_t value) {
    જો(કિંમત_Map_Index==0) {
        પાછા 0;  // The property does not have named values.
    }
    ++કિંમત_Map_Index;  // Skip the BytesTrie offset.
    int32_t અંક_Ranges=કિંમત_Maps[કિંમત_Map_Index++];
    જો(અંક_Ranges<0x10) {
        // Ranges of values.
        માટે(; અંક_Ranges>0; --અંક_Ranges) {
            // Read and skip the start and limit of this range.
            int32_t start=કિંમત_Maps[કિંમત_Map_Index];
            int32_t limit=કિંમત_Maps[કિંમત_Map_Index+1];
            કિંમત_Map_Index+=2;
            જો(value<start) {
                તોડો;
            }
            જો(value<limit) {
                પાછા કિંમત_Maps[કિંમત_Map_Index+value-start];
            }
            કિંમત_Map_Index+=limit-start;  // Skip all entries for this range.
        }
    } નહીંતર {
        // List of values.
        int32_t કિંમતો_Start=કિંમત_Map_Index;
        int32_t nameGroupOffsetsStart=કિંમત_Map_Index+અંક_Ranges-0x10;
        કરો {
            int32_t v=કિંમત_Maps[કિંમત_Map_Index];
            જો(value<v) {
                તોડો;
            }
            જો(value==v) {
                પાછા કિંમત_Maps[nameGroupOffsetsStart+કિંમત_Map_Index-કિંમતો_Start];
            }
        } જ્યારે(++કિંમત_Map_Index<nameGroupOffsetsStart);
    }
    પાછા 0;
}

અચલ અક્ષર *PropNameData::getName(અચલ અક્ષર *nameGroup, int32_t nameIndex) {
    int32_t અંક_Names=*nameGroup++;
    જો(nameIndex<0 || અંક_Names<=nameIndex) {
        પાછા nullptr;
    }
    // Skip nameIndex names.
    માટે(; nameIndex>0; --nameIndex) {
        nameGroup=uprv_strchr(nameGroup, 0)+1;
    }
    જો(*nameGroup==0) {
        પાછા nullptr;  // no name (Property[Value]Aliases.txt has "n/a")
    }
    પાછા nameGroup;
}

UBool PropNameData::containsName(BytesTrie &trie, અચલ અક્ષર *name) {
    જો(name==nullptr) {
        પાછા ખોટું;
    }
    UStringTrieResult result=USTRINGTRIE_NO_કિંમત;
    અક્ષર c;
    જ્યારે((c=*name++)!=0) {
        c=uprv_invCharToLowercaseAscii(c);
        // Ignore delimiters '-', '_', and ASCII White_Space.
        જો(c==0x2d || c==0x5f || c==0x20 || (0x09<=c && c<=0x0d)) {
            ચાલુ;
        }
        જો(!USTRINGTRIE_HAS_NEXT(result)) {
            પાછા ખોટું;
        }
        result = trie.next(static_cast<uint8_t>(c));
    }
    પાછા USTRINGTRIE_HAS_કિંમત(result);
}

અચલ અક્ષર *PropNameData::getPropertyName(int32_t property, int32_t nameChoice) {
    int32_t કિંમત_Map_Index=findProperty(property);
    જો(કિંમત_Map_Index==0) {
        પાછા nullptr;  // Not a known property.
    }
    પાછા getName(nameGroups+કિંમત_Maps[કિંમત_Map_Index], nameChoice);
}

અચલ અક્ષર *PropNameData::get_Property_કિંમત_Name(int32_t property, int32_t value, int32_t nameChoice) {
    int32_t કિંમત_Map_Index=findProperty(property);
    જો(કિંમત_Map_Index==0) {
        પાછા nullptr;  // Not a known property.
    }
    int32_t nameGroupOffset=find_Property_કિંમત_Name_Group(કિંમત_Maps[કિંમત_Map_Index+1], value);
    જો(nameGroupOffset==0) {
        પાછા nullptr;
    }
    પાછા getName(nameGroups+nameGroupOffset, nameChoice);
}

int32_t PropNameData::get_Property_Or_કિંમત_Enum(int32_t bytesTrieOffset, અચલ અક્ષર *alias) {
    BytesTrie trie(bytesTries+bytesTrieOffset);
    જો(containsName(trie, alias)) {
        પાછા trie.get_કિંમત();
    } નહીંતર {
        પાછા UCHAR_INVALID_CODE;
    }
}

int32_t PropNameData::getPropertyEnum(અચલ અક્ષર *alias) {
    પાછા get_Property_Or_કિંમત_Enum(0, alias);
}

int32_t PropNameData::get_Property_કિંમત_Enum(int32_t property, અચલ અક્ષર *alias) {
    int32_t કિંમત_Map_Index=findProperty(property);
    જો(કિંમત_Map_Index==0) {
        પાછા UCHAR_INVALID_CODE;  // Not a known property.
    }
    કિંમત_Map_Index=કિંમત_Maps[કિંમત_Map_Index+1];
    જો(કિંમત_Map_Index==0) {
        પાછા UCHAR_INVALID_CODE;  // The property does not have named values.
    }
    // valueMapIndex is the start of the property's valueMap,
    // where the first word is the BytesTrie offset.
    પાછા get_Property_Or_કિંમત_Enum(કિંમત_Maps[કિંમત_Map_Index], alias);
}
U_NAMESPACE_END

//----------------------------------------------------------------------
// Public API implementation

U_CAPI અચલ અક્ષર* U_EXPORT2
u_getPropertyName(UProperty property,
                  UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    પાછા PropNameData::getPropertyName(property, nameChoice);
}

U_CAPI UProperty U_EXPORT2
u_getPropertyEnum(અચલ અક્ષર* alias) {
    U_NAMESPACE_USE
    પાછા (UProperty)PropNameData::getPropertyEnum(alias);
}

U_CAPI અચલ અક્ષર* U_EXPORT2
u_get_Property_કિંમત_Name(UProperty property,
                       int32_t value,
                       UPropertyNameChoice nameChoice) UPRV_NO_SANITIZE_UNDEFINED {
    // The nameChoice is really an integer with a couple of named constants.
    // Unicode allows for names other than short and long ones.
    // If present, these will be returned for U_LONG_PROPERTY_NAME + i, where i=1, 2,...
    U_NAMESPACE_USE
    પાછા PropNameData::get_Property_કિંમત_Name(property, value, nameChoice);
}

U_CAPI int32_t U_EXPORT2
u_get_Property_કિંમત_Enum(UProperty property,
                       અચલ અક્ષર* alias) {
    U_NAMESPACE_USE
    પાછા PropNameData::get_Property_કિંમત_Enum(property, alias);
}

U_CAPI અચલ અક્ષર*  U_EXPORT2
uscript_getName(UScriptCode scriptCode){
    પાછા u_get_Property_કિંમત_Name(UCHAR_SCRIPT, scriptCode,
                                  U_LONG_PROPERTY_NAME);
}

U_CAPI અચલ અક્ષર*  U_EXPORT2
uscript_getShortName(UScriptCode scriptCode){
    પાછા u_get_Property_કિંમત_Name(UCHAR_SCRIPT, scriptCode,
                                  U_SHORT_PROPERTY_NAME);
}
