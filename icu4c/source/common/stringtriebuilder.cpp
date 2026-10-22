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

#समावेश "utypeinfo.h"  // for 'typeid' to work
#समावेश "unicode/utypes.h"
#समावेश "unicode/stringtriebuilder.h"
#समावेश "uassert.h"
#समावेश "uhash.h"

U_CDECL_BEGIN

अटल int32_t U_CALLCONV
hash_String_Trie_गाँठ(स्थिर UHashTok कुंजी) {
    वापस icu::StringTrieBuilder::hash_गाँठ(कुंजी.pointer);
}

अटल UBool U_CALLCONV
equalStringTrieNodes(स्थिर UHashTok key1, स्थिर UHashTok key2) {
    वापस icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

व्योम
StringTrieBuilder::createCompactBuilder(int32_t आकार_Guess, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    nodes=uhash_open_आकार(hash_String_Trie_गाँठ, equalStringTrieNodes, nullptr,
                         आकार_Guess, &errorCode);
    अगर(U_SUCCESS(errorCode)) {
        अगर(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
          uhash_set_कुंजी_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

व्योम
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

व्योम
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_लंबाई,
                       UErrorCode &errorCode) {
    अगर(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_गाँठ(0, elements_लंबाई, 0);
    } अन्यथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_लंबाई, errorCode);
        Node *मूल=make_गाँठ(0, elements_लंबाई, 0, errorCode);
        अगर(U_SUCCESS(errorCode)) {
            मूल->mark_दायाँ_Edges_First(-1);
            मूल->ग_लिखो(*यह);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_गाँठ(int32_t start, int32_t limit, int32_t unit_सूचकांक) {
    UBool has_मूल्य=false;
    int32_t value=0;
    int32_t type;
    अगर(unit_सूचकांक==get_Element_String_लंबाई(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        अगर(start==limit) {
            वापस write_मूल्य_And_Final(value, true);  // final-value node
        }
        has_मूल्य=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_सूचकांक);
    int32_t maxUnit=getElementUnit(limit-1, unit_सूचकांक);
    अगर(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t last_Unit_सूचकांक=getLimitOfLinearMatch(start, limit-1, unit_सूचकांक);
        write_गाँठ(start, limit, last_Unit_सूचकांक);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=last_Unit_सूचकांक-unit_सूचकांक;
        int32_t max_Linear_Match_लंबाई=get_Max_Linear_Match_लंबाई();
        जबतक(length>max_Linear_Match_लंबाई) {
            last_Unit_सूचकांक-=max_Linear_Match_लंबाई;
            length-=max_Linear_Match_लंबाई;
            writeElementUnits(start, last_Unit_सूचकांक, max_Linear_Match_लंबाई);
            ग_लिखो(getMinLinearMatch()+max_Linear_Match_लंबाई-1);
        }
        writeElementUnits(start, unit_सूचकांक, length);
        type=getMinLinearMatch()+length-1;
    } अन्यथा {
        // Branch node.
        int32_t length=गिनती_Element_Units(start, limit, unit_सूचकांक);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_गाँठ(start, limit, unit_सूचकांक, length);
        अगर(--length<getMinLinearMatch()) {
            type=length;
        } अन्यथा {
            ग_लिखो(length);
            type=0;
        }
    }
    वापस write_मूल्य_And_Type(has_मूल्य, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_गाँठ(int32_t start, int32_t limit, int32_t unit_सूचकांक, int32_t length) {
    char16_t मध्य_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t lt_लंबाई=0;
    जबतक(length>get_Max_Branch_Linear_Sub_गाँठ_लंबाई()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_सूचकांक, length/2);
        // Encode the less-than branch first.
        मध्य_Units[lt_लंबाई]=getElementUnit(i, unit_सूचकांक);  // middle unit
        lessThan[lt_लंबाई]=write_Branch_Sub_गाँठ(start, i, unit_सूचकांक, length/2);
        ++lt_लंबाई;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_गाँठ_लंबाई];
    UBool isFinal[k_Max_Branch_Linear_Sub_गाँठ_लंबाई-1];
    int32_t unit_संख्या=0;
    करो {
        int32_t i=starts[unit_संख्या]=start;
        char16_t unit=getElementUnit(i++, unit_सूचकांक);
        i=सूचकांक_Of_Element_With_अग्र_Unit(i, unit_सूचकांक, unit);
        isFinal[unit_संख्या]= start==i-1 && unit_सूचकांक+1==get_Element_String_लंबाई(start);
        start=i;
    } जबतक(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_संख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_गाँठ_लंबाई-1];
    करो {
        --unit_संख्या;
        अगर(!isFinal[unit_संख्या]) {
            jumpTargets[unit_संख्या]=write_गाँठ(starts[unit_संख्या], starts[unit_संख्या+1], unit_सूचकांक+1);
        }
    } जबतक(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    write_गाँठ(start, limit, unit_सूचकांक+1);
    int32_t offset=ग_लिखो(getElementUnit(start, unit_सूचकांक));
    // Write the rest of this node's unit-value pairs.
    जबतक(--unit_संख्या>=0) {
        start=starts[unit_संख्या];
        int32_t value;
        अगर(isFinal[unit_संख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मूल्य(start);
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_संख्या];
        }
        write_मूल्य_And_Final(value, isFinal[unit_संख्या]);
        offset=ग_लिखो(getElementUnit(start, unit_सूचकांक));
    }
    // Write the split-branch nodes.
    जबतक(lt_लंबाई>0) {
        --lt_लंबाई;
        writeDeltaTo(lessThan[lt_लंबाई]);
        offset=ग_लिखो(मध्य_Units[lt_लंबाई]);
    }
    वापस offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_गाँठ(int32_t start,
                            int32_t limit,
                            int32_t unit_सूचकांक,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    UBool has_मूल्य=false;
    int32_t value=0;
    अगर(unit_सूचकांक==get_Element_String_लंबाई(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        अगर(start==limit) {
            वापस register_Final_मूल्य(value, errorCode);
        }
        has_मूल्य=true;
    }
    Node *गाँठ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_सूचकांक);
    int32_t maxUnit=getElementUnit(limit-1, unit_सूचकांक);
    अगर(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t last_Unit_सूचकांक=getLimitOfLinearMatch(start, limit-1, unit_सूचकांक);
        Node *अग्र_गाँठ=make_गाँठ(start, limit, last_Unit_सूचकांक, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=last_Unit_सूचकांक-unit_सूचकांक;
        int32_t max_Linear_Match_लंबाई=get_Max_Linear_Match_लंबाई();
        जबतक(length>max_Linear_Match_लंबाई) {
            last_Unit_सूचकांक-=max_Linear_Match_लंबाई;
            length-=max_Linear_Match_लंबाई;
            गाँठ=create_Linear_Match_गाँठ(start, last_Unit_सूचकांक, max_Linear_Match_लंबाई, अग्र_गाँठ);
            अग्र_गाँठ=register_गाँठ(गाँठ, errorCode);
        }
        गाँठ=create_Linear_Match_गाँठ(start, unit_सूचकांक, length, अग्र_गाँठ);
    } अन्यथा {
        // Branch node.
        int32_t length=गिनती_Element_Units(start, limit, unit_सूचकांक);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_गाँठ=make_Branch_Sub_गाँठ(start, limit, unit_सूचकांक, length, errorCode);
        गाँठ=नया Branch_शीर्ष_गाँठ(length, sub_गाँठ);
    }
    अगर(has_मूल्य && गाँठ!=nullptr) {
        अगर(match_Nodes_Can_Have_मूल्यावली()) {
            ((मूल्य_गाँठ *)गाँठ)->set_मूल्य(value);
        } अन्यथा {
            गाँठ=नया Intermediate_मूल्य_गाँठ(value, register_गाँठ(गाँठ, errorCode));
        }
    }
    वापस register_गाँठ(गाँठ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_गाँठ(int32_t start,
                                     int32_t limit,
                                     int32_t unit_सूचकांक,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    char16_t मध्य_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t lt_लंबाई=0;
    जबतक(length>get_Max_Branch_Linear_Sub_गाँठ_लंबाई()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_सूचकांक, length/2);
        // Create the less-than branch.
        मध्य_Units[lt_लंबाई]=getElementUnit(i, unit_सूचकांक);  // middle unit
        lessThan[lt_लंबाई]=make_Branch_Sub_गाँठ(start, i, unit_सूचकांक, length/2, errorCode);
        ++lt_लंबाई;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    List_Branch_गाँठ *list_गाँठ=नया List_Branch_गाँठ();
    अगर(list_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_संख्या=0;
    करो {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unit_सूचकांक);
        i=सूचकांक_Of_Element_With_अग्र_Unit(i, unit_सूचकांक, unit);
        अगर(start==i-1 && unit_सूचकांक+1==get_Element_String_लंबाई(start)) {
            list_गाँठ->add(unit, get_Element_मूल्य(start));
        } अन्यथा {
            list_गाँठ->add(unit, make_गाँठ(start, i, unit_सूचकांक+1, errorCode));
        }
        start=i;
    } जबतक(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unit_सूचकांक);
    अगर(start==limit-1 && unit_सूचकांक+1==get_Element_String_लंबाई(start)) {
        list_गाँठ->add(unit, get_Element_मूल्य(start));
    } अन्यथा {
        list_गाँठ->add(unit, make_गाँठ(start, limit, unit_सूचकांक+1, errorCode));
    }
    Node *गाँठ=register_गाँठ(list_गाँठ, errorCode);
    // Create the split-branch nodes.
    जबतक(lt_लंबाई>0) {
        --lt_लंबाई;
        गाँठ=register_गाँठ(
            नया Split_Branch_गाँठ(मध्य_Units[lt_लंबाई], lessThan[lt_लंबाई], गाँठ), errorCode);
    }
    वापस गाँठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_गाँठ(Node* new_गाँठ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        मिटाओ new_गाँठ;
        वापस nullptr;
    }
    अगर(new_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    स्थिर UHashElement *old=uhash_find(nodes, new_गाँठ);
    अगर(old!=nullptr) {
        मिटाओ new_गाँठ;
        वापस static_cast<Node*>(old->कुंजी.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#अगर U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#पूर्ण_अगर
    uhash_puti(nodes, new_गाँठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    अगर(U_FAILURE(errorCode)) {
        मिटाओ new_गाँठ;
        वापस nullptr;
    }
    वापस new_गाँठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_मूल्य(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    Final_मूल्य_गाँठ कुंजी(value);
    स्थिर UHashElement *old=uhash_find(nodes, &कुंजी);
    अगर(old!=nullptr) {
        वापस static_cast<Node*>(old->कुंजी.pointer);
    }
    Node *new_गाँठ=नया Final_मूल्य_गाँठ(value);
    अगर(new_गाँठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#अगर U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#पूर्ण_अगर
    uhash_puti(nodes, new_गाँठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    अगर(U_FAILURE(errorCode)) {
        मिटाओ new_गाँठ;
        वापस nullptr;
    }
    वापस new_गाँठ;
}

int32_t
StringTrieBuilder::hash_गाँठ(स्थिर व्योम *गाँठ) {
    वापस static_cast<स्थिर Node*>(गाँठ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(स्थिर व्योम *बाँए, स्थिर व्योम *दाँए) {
    वापस *static_cast<स्थिर Node*>(बाँए) == *static_cast<स्थिर Node*>(दाँए);
}

bool
StringTrieBuilder::Node::चालक==(स्थिर Node &other) स्थिर {
    वापस यह==&other || (typeid(*यह)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        offset=edge_संख्या;
    }
    वापस edge_संख्या;
}

bool
StringTrieBuilder::Final_मूल्य_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Node::चालक==(other)) {
        वापस false;
    }
    स्थिर Final_मूल्य_गाँठ &o=static_cast<स्थिर Final_मूल्य_गाँठ &>(other);
    वापस value==o.value;
}

व्योम
StringTrieBuilder::Final_मूल्य_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    offset=builder.write_मूल्य_And_Final(value, true);
}

bool
StringTrieBuilder::मूल्य_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Node::चालक==(other)) {
        वापस false;
    }
    स्थिर मूल्य_गाँठ &o=static_cast<स्थिर मूल्य_गाँठ &>(other);
    वापस has_मूल्य==o.has_मूल्य && (!has_मूल्य || value==o.value);
}

bool
StringTrieBuilder::Intermediate_मूल्य_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!मूल्य_गाँठ::चालक==(other)) {
        वापस false;
    }
    स्थिर Intermediate_मूल्य_गाँठ &o=static_cast<स्थिर Intermediate_मूल्य_गाँठ &>(other);
    वापस next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मूल्य_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    वापस edge_संख्या;
}

व्योम
StringTrieBuilder::Intermediate_मूल्य_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    next->ग_लिखो(builder);
    offset=builder.write_मूल्य_And_Final(value, false);
}

bool
StringTrieBuilder::Linear_Match_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!मूल्य_गाँठ::चालक==(other)) {
        वापस false;
    }
    स्थिर Linear_Match_गाँठ &o=static_cast<स्थिर Linear_Match_गाँठ &>(other);
    वापस length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    वापस edge_संख्या;
}

bool
StringTrieBuilder::List_Branch_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Node::चालक==(other)) {
        वापस false;
    }
    स्थिर List_Branch_गाँठ &o=static_cast<स्थिर List_Branch_गाँठ &>(other);
    क्रम(int32_t i=0; i<length; ++i) {
        अगर(units[i]!=o.units[i] || मूल्यावली[i]!=o.मूल्यावली[i] || equal[i]!=o.equal[i]) {
            वापस false;
        }
    }
    वापस true;
}

int32_t
StringTrieBuilder::List_Branch_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        int32_t step=0;
        int32_t i=length;
        करो {
            Node *edge=equal[--i];
            अगर(edge!=nullptr) {
                edge_संख्या=edge->mark_दायाँ_Edges_First(edge_संख्या-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } जबतक(i>0);
        offset=edge_संख्या;
    }
    वापस edge_संख्या;
}

व्योम
StringTrieBuilder::List_Branch_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_संख्या=length-1;
    Node *दायाँ_Edge=equal[unit_संख्या];
    int32_t दायाँ_Edge_संख्या= दायाँ_Edge==nullptr ? first_Edge_संख्या : दायाँ_Edge->getOffset();
    करो {
        --unit_संख्या;
        अगर(equal[unit_संख्या]!=nullptr) {
            equal[unit_संख्या]->write_Unless_Inside_दायाँ_Edge(first_Edge_संख्या, दायाँ_Edge_संख्या, builder);
        }
    } जबतक(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    अगर(दायाँ_Edge==nullptr) {
        builder.write_मूल्य_And_Final(मूल्यावली[unit_संख्या], true);
    } अन्यथा {
        दायाँ_Edge->ग_लिखो(builder);
    }
    offset=builder.ग_लिखो(units[unit_संख्या]);
    // Write the rest of this node's unit-value pairs.
    जबतक(--unit_संख्या>=0) {
        int32_t value;
        UBool isFinal;
        अगर(equal[unit_संख्या]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मूल्यावली[unit_संख्या];
            isFinal=true;
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_संख्या]->getOffset()>0);
            value=offset-equal[unit_संख्या]->getOffset();
            isFinal=false;
        }
        builder.write_मूल्य_And_Final(value, isFinal);
        offset=builder.ग_लिखो(units[unit_संख्या]);
    }
}

bool
StringTrieBuilder::Split_Branch_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!Node::चालक==(other)) {
        वापस false;
    }
    स्थिर Split_Branch_गाँठ &o=static_cast<स्थिर Split_Branch_गाँठ &>(other);
    वापस unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        edge_संख्या=greaterOrEqual->mark_दायाँ_Edges_First(edge_संख्या);
        offset=edge_संख्या=lessThan->mark_दायाँ_Edges_First(edge_संख्या-1);
    }
    वापस edge_संख्या;
}

व्योम
StringTrieBuilder::Split_Branch_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_दायाँ_Edge(first_Edge_संख्या, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ग_लिखो(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.ग_लिखो(unit);
}

bool
StringTrieBuilder::Branch_शीर्ष_गाँठ::चालक==(स्थिर Node &other) स्थिर {
    अगर(यह==&other) {
        वापस true;
    }
    अगर(!मूल्य_गाँठ::चालक==(other)) {
        वापस false;
    }
    स्थिर Branch_शीर्ष_गाँठ &o=static_cast<स्थिर Branch_शीर्ष_गाँठ &>(other);
    वापस length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_शीर्ष_गाँठ::mark_दायाँ_Edges_First(int32_t edge_संख्या) {
    अगर(offset==0) {
        offset=edge_संख्या=next->mark_दायाँ_Edges_First(edge_संख्या);
    }
    वापस edge_संख्या;
}

व्योम
StringTrieBuilder::Branch_शीर्ष_गाँठ::ग_लिखो(StringTrieBuilder &builder) {
    next->ग_लिखो(builder);
    अगर(length<=builder.getMinLinearMatch()) {
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, length-1);
    } अन्यथा {
        builder.ग_लिखो(length-1);
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, 0);
    }
}

U_NAMESPACE_END
