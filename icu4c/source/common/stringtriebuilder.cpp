// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  stringtriebuilder.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec24
*   created by: Markus W. Scherer
*/

#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/stringtriebuilder.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "uhash.h"

U_CDECL_BEGIN

स्थिर int32_t U_CALLCONV
hash_String_Trie_ग्रन्थिः(ध्रुव UHashTok कुञ्चिका) {
    प्रत्यागम icu::StringTrieBuilder::hash_ग्रन्थिः(कुञ्चिका.pointer);
}

स्थिर UBool U_CALLCONV
equalStringTrieNodes(ध्रुव UHashTok key1, ध्रुव UHashTok key2) {
    प्रत्यागम icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_अन्तः

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(शून्यसूचकः) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

शून्य
StringTrieBuilder::createCompactBuilder(int32_t आकार_Guess, UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    nodes=uhash_open_आकार(hash_String_Trie_ग्रन्थिः, equalStringTrieNodes, शून्यसूचकः,
                         आकार_Guess, &error_संहिता);
    यदि(U_SUCCESS(error_संहिता)) {
        यदि(nodes==शून्यसूचकः) {
          error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
          uhash_समुच्चयः_कुञ्चिका_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

शून्य
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=शून्यसूचकः;
}

शून्य
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_दीर्घता,
                       UError_संहिता &error_संहिता) {
    यदि(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ग्रन्थिः(0, elements_दीर्घता, 0);
    } अन्यथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_दीर्घता, error_संहिता);
        Node *मूलम्=make_ग्रन्थिः(0, elements_दीर्घता, 0, error_संहिता);
        यदि(U_SUCCESS(error_संहिता)) {
            मूलम्->mark_दक्षिणः_धाराः_First(-1);
            मूलम्->लिख(*एतत्);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ग्रन्थिः(int32_t start, int32_t सीमा, int32_t unit_सूचकांक) {
    UBool has_मानम्=असत्य;
    int32_t value=0;
    int32_t type;
    यदि(unit_सूचकांक==get_Element_String_दीर्घता(start)) {
        // An intermediate or final value.
        value=get_Element_मानम्(start++);
        यदि(start==सीमा) {
            प्रत्यागम write_मानम्_And_Final(value, सत्य);  // final-value node
        }
        has_मानम्=सत्य;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_सूचकांक);
    int32_t maxUnit=getElementUnit(सीमा-1, unit_सूचकांक);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t चरमः_Unit_सूचकांक=get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(start, सीमा-1, unit_सूचकांक);
        write_ग्रन्थिः(start, सीमा, चरमः_Unit_सूचकांक);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=चरमः_Unit_सूचकांक-unit_सूचकांक;
        int32_t max_रैखिकम्_मेलनम्_दीर्घता=get_Max_रैखिकम्_मेलनम्_दीर्घता();
        यावत्(length>max_रैखिकम्_मेलनम्_दीर्घता) {
            चरमः_Unit_सूचकांक-=max_रैखिकम्_मेलनम्_दीर्घता;
            length-=max_रैखिकम्_मेलनम्_दीर्घता;
            writeElementUnits(start, चरमः_Unit_सूचकांक, max_रैखिकम्_मेलनम्_दीर्घता);
            लिख(get_Min_रैखिकम्_मेलनम्()+max_रैखिकम्_मेलनम्_दीर्घता-1);
        }
        writeElementUnits(start, unit_सूचकांक, length);
        type=get_Min_रैखिकम्_मेलनम्()+length-1;
    } अन्यथा {
        // Branch node.
        int32_t length=गणनम्_Element_Units(start, सीमा, unit_सूचकांक);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ग्रन्थिः(start, सीमा, unit_सूचकांक, length);
        यदि(--length<get_Min_रैखिकम्_मेलनम्()) {
            type=length;
        } अन्यथा {
            लिख(length);
            type=0;
        }
    }
    प्रत्यागम write_मानम्_And_Type(has_मानम्, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ग्रन्थिः(int32_t start, int32_t सीमा, int32_t unit_सूचकांक, int32_t length) {
    char16_t मध्यः_Units[k_Max_विभज_Branch_स्तराः];
    int32_t lessThan[k_Max_विभज_Branch_स्तराः];
    int32_t lt_दीर्घता=0;
    यावत्(length>get_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_सूचकांक, length/2);
        // Encode the less-than branch first.
        मध्यः_Units[lt_दीर्घता]=getElementUnit(i, unit_सूचकांक);  // middle unit
        lessThan[lt_दीर्घता]=write_Branch_Sub_ग्रन्थिः(start, i, unit_सूचकांक, length/2);
        ++lt_दीर्घता;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता];
    UBool isFinal[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता-1];
    int32_t unit_सङ्ख्या=0;
    कुरु {
        int32_t i=starts[unit_सङ्ख्या]=start;
        char16_t unit=getElementUnit(i++, unit_सूचकांक);
        i=सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(i, unit_सूचकांक, unit);
        isFinal[unit_सङ्ख्या]= start==i-1 && unit_सूचकांक+1==get_Element_String_दीर्घता(start);
        start=i;
    } यावत्(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_सङ्ख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता-1];
    कुरु {
        --unit_सङ्ख्या;
        यदि(!isFinal[unit_सङ्ख्या]) {
            jumpTargets[unit_सङ्ख्या]=write_ग्रन्थिः(starts[unit_सङ्ख्या], starts[unit_सङ्ख्या+1], unit_सूचकांक+1);
        }
    } यावत्(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    write_ग्रन्थिः(start, सीमा, unit_सूचकांक+1);
    int32_t विस्थापनम्=लिख(getElementUnit(start, unit_सूचकांक));
    // Write the rest of this node's unit-value pairs.
    यावत्(--unit_सङ्ख्या>=0) {
        start=starts[unit_सङ्ख्या];
        int32_t value;
        यदि(isFinal[unit_सङ्ख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मानम्(start);
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            value=विस्थापनम्-jumpTargets[unit_सङ्ख्या];
        }
        write_मानम्_And_Final(value, isFinal[unit_सङ्ख्या]);
        विस्थापनम्=लिख(getElementUnit(start, unit_सूचकांक));
    }
    // Write the split-branch nodes.
    यावत्(lt_दीर्घता>0) {
        --lt_दीर्घता;
        write_Delta_गन्तव्यम्(lessThan[lt_दीर्घता]);
        विस्थापनम्=लिख(मध्यः_Units[lt_दीर्घता]);
    }
    प्रत्यागम विस्थापनम्;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ग्रन्थिः(int32_t start,
                            int32_t सीमा,
                            int32_t unit_सूचकांक,
                            UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    UBool has_मानम्=असत्य;
    int32_t value=0;
    यदि(unit_सूचकांक==get_Element_String_दीर्घता(start)) {
        // An intermediate or final value.
        value=get_Element_मानम्(start++);
        यदि(start==सीमा) {
            प्रत्यागम पञ्जिकाकोशः_Final_मानम्(value, error_संहिता);
        }
        has_मानम्=सत्य;
    }
    Node *ग्रन्थिः;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_सूचकांक);
    int32_t maxUnit=getElementUnit(सीमा-1, unit_सूचकांक);
    यदि(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t चरमः_Unit_सूचकांक=get_सीमा_सम्बन्धि_रैखिकम्_मेलनम्(start, सीमा-1, unit_सूचकांक);
        Node *अग्र_ग्रन्थिः=make_ग्रन्थिः(start, सीमा, चरमः_Unit_सूचकांक, error_संहिता);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=चरमः_Unit_सूचकांक-unit_सूचकांक;
        int32_t max_रैखिकम्_मेलनम्_दीर्घता=get_Max_रैखिकम्_मेलनम्_दीर्घता();
        यावत्(length>max_रैखिकम्_मेलनम्_दीर्घता) {
            चरमः_Unit_सूचकांक-=max_रैखिकम्_मेलनम्_दीर्घता;
            length-=max_रैखिकम्_मेलनम्_दीर्घता;
            ग्रन्थिः=create_रैखिकम्_मेलनम्_ग्रन्थिः(start, चरमः_Unit_सूचकांक, max_रैखिकम्_मेलनम्_दीर्घता, अग्र_ग्रन्थिः);
            अग्र_ग्रन्थिः=पञ्जिकाकोशः_ग्रन्थिः(ग्रन्थिः, error_संहिता);
        }
        ग्रन्थिः=create_रैखिकम्_मेलनम्_ग्रन्थिः(start, unit_सूचकांक, length, अग्र_ग्रन्थिः);
    } अन्यथा {
        // Branch node.
        int32_t length=गणनम्_Element_Units(start, सीमा, unit_सूचकांक);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ग्रन्थिः=make_Branch_Sub_ग्रन्थिः(start, सीमा, unit_सूचकांक, length, error_संहिता);
        ग्रन्थिः=नूतन Branch_शीर्ष_ग्रन्थिः(length, sub_ग्रन्थिः);
    }
    यदि(has_मानम् && ग्रन्थिः!=शून्यसूचकः) {
        यदि(मेलनम्_Nodes_Can_Have_मानानि()) {
            ((मानम्_ग्रन्थिः *)ग्रन्थिः)->समुच्चयः_मानम्(value);
        } अन्यथा {
            ग्रन्थिः=नूतन Intermediate_मानम्_ग्रन्थिः(value, पञ्जिकाकोशः_ग्रन्थिः(ग्रन्थिः, error_संहिता));
        }
    }
    प्रत्यागम पञ्जिकाकोशः_ग्रन्थिः(ग्रन्थिः, error_संहिता);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ग्रन्थिः(int32_t start,
                                     int32_t सीमा,
                                     int32_t unit_सूचकांक,
                                     int32_t length,
                                     UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    char16_t मध्यः_Units[k_Max_विभज_Branch_स्तराः];
    Node *lessThan[k_Max_विभज_Branch_स्तराः];
    int32_t lt_दीर्घता=0;
    यावत्(length>get_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_सूचकांक, length/2);
        // Create the less-than branch.
        मध्यः_Units[lt_दीर्घता]=getElementUnit(i, unit_सूचकांक);  // middle unit
        lessThan[lt_दीर्घता]=make_Branch_Sub_ग्रन्थिः(start, i, unit_सूचकांक, length/2, error_संहिता);
        ++lt_दीर्घता;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    List_Branch_ग्रन्थिः *list_ग्रन्थिः=नूतन List_Branch_ग्रन्थिः();
    यदि(list_ग्रन्थिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_सङ्ख्या=0;
    कुरु {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unit_सूचकांक);
        i=सूचकांक_सम्बन्धि_Element_With_अग्र_Unit(i, unit_सूचकांक, unit);
        यदि(start==i-1 && unit_सूचकांक+1==get_Element_String_दीर्घता(start)) {
            list_ग्रन्थिः->सङ्कलय(unit, get_Element_मानम्(start));
        } अन्यथा {
            list_ग्रन्थिः->सङ्कलय(unit, make_ग्रन्थिः(start, i, unit_सूचकांक+1, error_संहिता));
        }
        start=i;
    } यावत्(++unit_सङ्ख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unit_सूचकांक);
    यदि(start==सीमा-1 && unit_सूचकांक+1==get_Element_String_दीर्घता(start)) {
        list_ग्रन्थिः->सङ्कलय(unit, get_Element_मानम्(start));
    } अन्यथा {
        list_ग्रन्थिः->सङ्कलय(unit, make_ग्रन्थिः(start, सीमा, unit_सूचकांक+1, error_संहिता));
    }
    Node *ग्रन्थिः=पञ्जिकाकोशः_ग्रन्थिः(list_ग्रन्थिः, error_संहिता);
    // Create the split-branch nodes.
    यावत्(lt_दीर्घता>0) {
        --lt_दीर्घता;
        ग्रन्थिः=पञ्जिकाकोशः_ग्रन्थिः(
            नूतन विभज_Branch_ग्रन्थिः(मध्यः_Units[lt_दीर्घता], lessThan[lt_दीर्घता], ग्रन्थिः), error_संहिता);
    }
    प्रत्यागम ग्रन्थिः;
}

StringTrieBuilder::Node*
StringTrieBuilder::पञ्जिकाकोशः_ग्रन्थिः(Node* new_ग्रन्थिः, UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        विलोपय new_ग्रन्थिः;
        प्रत्यागम शून्यसूचकः;
    }
    यदि(new_ग्रन्थिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव UHashElement *old=uhash_find(nodes, new_ग्रन्थिः);
    यदि(old!=शून्यसूचकः) {
        विलोपय new_ग्रन्थिः;
        प्रत्यागम static_cast<Node*>(old->कुञ्चिका.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मानम्=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#यदि_अन्तः
    uhash_puti(nodes, new_ग्रन्थिः, 1, &error_संहिता);
    U_ASSERT(old_मानम्==0);
    यदि(U_FAILURE(error_संहिता)) {
        विलोपय new_ग्रन्थिः;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम new_ग्रन्थिः;
}

StringTrieBuilder::Node*
StringTrieBuilder::पञ्जिकाकोशः_Final_मानम्(int32_t value, UError_संहिता& error_संहिता) U_LIFETIME_BOUND {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }
    Final_मानम्_ग्रन्थिः कुञ्चिका(value);
    ध्रुव UHashElement *old=uhash_find(nodes, &कुञ्चिका);
    यदि(old!=शून्यसूचकः) {
        प्रत्यागम static_cast<Node*>(old->कुञ्चिका.pointer);
    }
    Node *new_ग्रन्थिः=नूतन Final_मानम्_ग्रन्थिः(value);
    यदि(new_ग्रन्थिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#यदि U_DEBUG
    int32_t old_मानम्=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#यदि_अन्तः
    uhash_puti(nodes, new_ग्रन्थिः, 1, &error_संहिता);
    U_ASSERT(old_मानम्==0);
    यदि(U_FAILURE(error_संहिता)) {
        विलोपय new_ग्रन्थिः;
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम new_ग्रन्थिः;
}

int32_t
StringTrieBuilder::hash_ग्रन्थिः(ध्रुव शून्य *ग्रन्थिः) {
    प्रत्यागम static_cast<ध्रुव Node*>(ग्रन्थिः)->hash_संहिता();
}

UBool
StringTrieBuilder::equalNodes(ध्रुव शून्य *वामतः, ध्रुव शून्य *दक्षिणतः) {
    प्रत्यागम *static_cast<ध्रुव Node*>(वामतः) == *static_cast<ध्रुव Node*>(दक्षिणतः);
}

तार्किक
StringTrieBuilder::Node::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    प्रत्यागम एतत्==&other || (typeid(*एतत्)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        विस्थापनम्=edge_सङ्ख्या;
    }
    प्रत्यागम edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::Final_मानम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Final_मानम्_ग्रन्थिः &o=static_cast<ध्रुव Final_मानम्_ग्रन्थिः &>(other);
    प्रत्यागम value==o.value;
}

शून्य
StringTrieBuilder::Final_मानम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    विस्थापनम्=builder.write_मानम्_And_Final(value, सत्य);
}

तार्किक
StringTrieBuilder::मानम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव मानम्_ग्रन्थिः &o=static_cast<ध्रुव मानम्_ग्रन्थिः &>(other);
    प्रत्यागम has_मानम्==o.has_मानम् && (!has_मानम् || value==o.value);
}

तार्किक
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मानम्_ग्रन्थिः::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Intermediate_मानम्_ग्रन्थिः &o=static_cast<ध्रुव Intermediate_मानम्_ग्रन्थिः &>(other);
    प्रत्यागम next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        विस्थापनम्=edge_सङ्ख्या=next->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या);
    }
    प्रत्यागम edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Intermediate_मानम्_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    विस्थापनम्=builder.write_मानम्_And_Final(value, असत्य);
}

तार्किक
StringTrieBuilder::रैखिकम्_मेलनम्_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मानम्_ग्रन्थिः::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव रैखिकम्_मेलनम्_ग्रन्थिः &o=static_cast<ध्रुव रैखिकम्_मेलनम्_ग्रन्थिः &>(other);
    प्रत्यागम length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::रैखिकम्_मेलनम्_ग्रन्थिः::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        विस्थापनम्=edge_सङ्ख्या=next->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या);
    }
    प्रत्यागम edge_सङ्ख्या;
}

तार्किक
StringTrieBuilder::List_Branch_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव List_Branch_ग्रन्थिः &o=static_cast<ध्रुव List_Branch_ग्रन्थिः &>(other);
    प्रति(int32_t i=0; i<length; ++i) {
        यदि(units[i]!=o.units[i] || मानानि[i]!=o.मानानि[i] || equal[i]!=o.equal[i]) {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम सत्य;
}

int32_t
StringTrieBuilder::List_Branch_ग्रन्थिः::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        int32_t चरणः=0;
        int32_t i=length;
        कुरु {
            Node *edge=equal[--i];
            यदि(edge!=शून्यसूचकः) {
                edge_सङ्ख्या=edge->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या-चरणः);
            }
            // For all but the rightmost edge, decrement the edge number.
            चरणः=1;
        } यावत्(i>0);
        विस्थापनम्=edge_सङ्ख्या;
    }
    प्रत्यागम edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::List_Branch_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_सङ्ख्या=length-1;
    Node *दक्षिणः_Edge=equal[unit_सङ्ख्या];
    int32_t दक्षिणः_Edge_सङ्ख्या= दक्षिणः_Edge==शून्यसूचकः ? first_Edge_सङ्ख्या : दक्षिणः_Edge->get_विस्थापनम्();
    कुरु {
        --unit_सङ्ख्या;
        यदि(equal[unit_सङ्ख्या]!=शून्यसूचकः) {
            equal[unit_सङ्ख्या]->write_Unless_Inside_दक्षिणः_Edge(first_Edge_सङ्ख्या, दक्षिणः_Edge_सङ्ख्या, builder);
        }
    } यावत्(unit_सङ्ख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_सङ्ख्या=length-1;
    यदि(दक्षिणः_Edge==शून्यसूचकः) {
        builder.write_मानम्_And_Final(मानानि[unit_सङ्ख्या], सत्य);
    } अन्यथा {
        दक्षिणः_Edge->लिख(builder);
    }
    विस्थापनम्=builder.लिख(units[unit_सङ्ख्या]);
    // Write the rest of this node's unit-value pairs.
    यावत्(--unit_सङ्ख्या>=0) {
        int32_t value;
        UBool isFinal;
        यदि(equal[unit_सङ्ख्या]==शून्यसूचकः) {
            // Write the final value for the one string ending with this unit.
            value=मानानि[unit_सङ्ख्या];
            isFinal=सत्य;
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_सङ्ख्या]->get_विस्थापनम्()>0);
            value=विस्थापनम्-equal[unit_सङ्ख्या]->get_विस्थापनम्();
            isFinal=असत्य;
        }
        builder.write_मानम्_And_Final(value, isFinal);
        विस्थापनम्=builder.लिख(units[unit_सङ्ख्या]);
    }
}

तार्किक
StringTrieBuilder::विभज_Branch_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!Node::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव विभज_Branch_ग्रन्थिः &o=static_cast<ध्रुव विभज_Branch_ग्रन्थिः &>(other);
    प्रत्यागम unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::विभज_Branch_ग्रन्थिः::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        first_Edge_सङ्ख्या=edge_सङ्ख्या;
        edge_सङ्ख्या=greaterOrEqual->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या);
        विस्थापनम्=edge_सङ्ख्या=lessThan->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या-1);
    }
    प्रत्यागम edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::विभज_Branch_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_दक्षिणः_Edge(first_Edge_सङ्ख्या, greaterOrEqual->get_विस्थापनम्(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->लिख(builder);
    // Write this node.
    U_ASSERT(lessThan->get_विस्थापनम्()>0);
    builder.write_Delta_गन्तव्यम्(lessThan->get_विस्थापनम्());  // less-than
    विस्थापनम्=builder.लिख(unit);
}

तार्किक
StringTrieBuilder::Branch_शीर्ष_ग्रन्थिः::संक्रियकः==(ध्रुव Node &other) ध्रुव {
    यदि(एतत्==&other) {
        प्रत्यागम सत्य;
    }
    यदि(!मानम्_ग्रन्थिः::संक्रियकः==(other)) {
        प्रत्यागम असत्य;
    }
    ध्रुव Branch_शीर्ष_ग्रन्थिः &o=static_cast<ध्रुव Branch_शीर्ष_ग्रन्थिः &>(other);
    प्रत्यागम length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_शीर्ष_ग्रन्थिः::mark_दक्षिणः_धाराः_First(int32_t edge_सङ्ख्या) {
    यदि(विस्थापनम्==0) {
        विस्थापनम्=edge_सङ्ख्या=next->mark_दक्षिणः_धाराः_First(edge_सङ्ख्या);
    }
    प्रत्यागम edge_सङ्ख्या;
}

शून्य
StringTrieBuilder::Branch_शीर्ष_ग्रन्थिः::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    यदि(length<=builder.get_Min_रैखिकम्_मेलनम्()) {
        विस्थापनम्=builder.write_मानम्_And_Type(has_मानम्, value, length-1);
    } अन्यथा {
        builder.लिख(length-1);
        विस्थापनम्=builder.write_मानम्_And_Type(has_मानम्, value, 0);
    }
}

U_NAMESPACE_अन्तः
