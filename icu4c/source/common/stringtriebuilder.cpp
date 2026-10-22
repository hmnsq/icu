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
hash_String_Trie_गाठ(स्थिरांक UHashTok किल्ली) {
    परत icu::StringTrieBuilder::hash_गाठ(किल्ली.pointer);
}

स्थिर UBool U_CALLCONV
equalStringTrieNodes(स्थिरांक UHashTok key1, स्थिरांक UHashTok key2) {
    परत icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

शून्य
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    nodes=uhash_openSize(hash_String_Trie_गाठ, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    जर(U_SUCCESS(errorCode)) {
        जर(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
          uhash_set_किल्ली_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

शून्य
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

शून्य
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    जर(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_गाठ(0, elementsLength, 0);
    } अन्यथा /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *मूळ=make_गाठ(0, elementsLength, 0, errorCode);
        जर(U_SUCCESS(errorCode)) {
            मूळ->mark_उजवा_Edges_First(-1);
            मूळ->लिख(*हे);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_गाठ(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_मूल्य=असत्य;
    int32_t value=0;
    int32_t type;
    जर(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        जर(start==limit) {
            परत write_मूल्य_And_Final(value, सत्य);  // final-value node
        }
        has_मूल्य=सत्य;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    जर(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_गाठ(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        पर्यंत(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            लिख(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } अन्यथा {
        // Branch node.
        int32_t length=मोजणी_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_गाठ(start, limit, unitIndex, length);
        जर(--length<getMinLinearMatch()) {
            type=length;
        } अन्यथा {
            लिख(length);
            type=0;
        }
    }
    परत write_मूल्य_And_Type(has_मूल्य, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_गाठ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t मधला_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    पर्यंत(length>get_Max_Branch_Linear_Sub_गाठ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        मधला_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_गाठ(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_गाठ_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_गाठ_Length-1];
    int32_t unit_संख्या=0;
    करा {
        int32_t i=starts[unit_संख्या]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_संख्या]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } पर्यंत(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_संख्या]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_गाठ_Length-1];
    करा {
        --unit_संख्या;
        जर(!isFinal[unit_संख्या]) {
            jumpTargets[unit_संख्या]=write_गाठ(starts[unit_संख्या], starts[unit_संख्या+1], unitIndex+1);
        }
    } पर्यंत(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    write_गाठ(start, limit, unitIndex+1);
    int32_t offset=लिख(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    पर्यंत(--unit_संख्या>=0) {
        start=starts[unit_संख्या];
        int32_t value;
        जर(isFinal[unit_संख्या]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_मूल्य(start);
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_संख्या];
        }
        write_मूल्य_And_Final(value, isFinal[unit_संख्या]);
        offset=लिख(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    पर्यंत(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=लिख(मधला_Units[ltLength]);
    }
    परत offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_गाठ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    UBool has_मूल्य=असत्य;
    int32_t value=0;
    जर(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_मूल्य(start++);
        जर(start==limit) {
            परत register_Final_मूल्य(value, errorCode);
        }
        has_मूल्य=सत्य;
    }
    Node *गाठ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    जर(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_गाठ=make_गाठ(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        पर्यंत(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            गाठ=create_Linear_Match_गाठ(start, lastUnitIndex, maxLinearMatchLength, next_गाठ);
            next_गाठ=register_गाठ(गाठ, errorCode);
        }
        गाठ=create_Linear_Match_गाठ(start, unitIndex, length, next_गाठ);
    } अन्यथा {
        // Branch node.
        int32_t length=मोजणी_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_गाठ=make_Branch_Sub_गाठ(start, limit, unitIndex, length, errorCode);
        गाठ=नवीन Branch_Head_गाठ(length, sub_गाठ);
    }
    जर(has_मूल्य && गाठ!=nullptr) {
        जर(match_Nodes_Can_Have_मूल्ये()) {
            ((मूल्य_गाठ *)गाठ)->set_मूल्य(value);
        } अन्यथा {
            गाठ=नवीन Intermediate_मूल्य_गाठ(value, register_गाठ(गाठ, errorCode));
        }
    }
    परत register_गाठ(गाठ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_गाठ(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    char16_t मधला_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    पर्यंत(length>get_Max_Branch_Linear_Sub_गाठ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        मधला_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_गाठ(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    List_Branch_गाठ *list_गाठ=नवीन List_Branch_गाठ();
    जर(list_गाठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_संख्या=0;
    करा {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        जर(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_गाठ->add(unit, get_Element_मूल्य(start));
        } अन्यथा {
            list_गाठ->add(unit, make_गाठ(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } पर्यंत(++unit_संख्या<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    जर(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_गाठ->add(unit, get_Element_मूल्य(start));
    } अन्यथा {
        list_गाठ->add(unit, make_गाठ(start, limit, unitIndex+1, errorCode));
    }
    Node *गाठ=register_गाठ(list_गाठ, errorCode);
    // Create the split-branch nodes.
    पर्यंत(ltLength>0) {
        --ltLength;
        गाठ=register_गाठ(
            नवीन Split_Branch_गाठ(मधला_Units[ltLength], lessThan[ltLength], गाठ), errorCode);
    }
    परत गाठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_गाठ(Node* new_गाठ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर(U_FAILURE(errorCode)) {
        delete new_गाठ;
        परत nullptr;
    }
    जर(new_गाठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    स्थिरांक UHashElement *old=uhash_find(nodes, new_गाठ);
    जर(old!=nullptr) {
        delete new_गाठ;
        परत static_cast<Node*>(old->किल्ली.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#जर U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    जर(U_FAILURE(errorCode)) {
        delete new_गाठ;
        परत nullptr;
    }
    परत new_गाठ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_मूल्य(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    Final_मूल्य_गाठ किल्ली(value);
    स्थिरांक UHashElement *old=uhash_find(nodes, &किल्ली);
    जर(old!=nullptr) {
        परत static_cast<Node*>(old->किल्ली.pointer);
    }
    Node *new_गाठ=नवीन Final_मूल्य_गाठ(value);
    जर(new_गाठ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#जर U_DEBUG
    int32_t old_मूल्य=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_गाठ, 1, &errorCode);
    U_ASSERT(old_मूल्य==0);
    जर(U_FAILURE(errorCode)) {
        delete new_गाठ;
        परत nullptr;
    }
    परत new_गाठ;
}

int32_t
StringTrieBuilder::hash_गाठ(स्थिरांक शून्य *गाठ) {
    परत static_cast<स्थिरांक Node*>(गाठ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(स्थिरांक शून्य *left, स्थिरांक शून्य *उजवा) {
    परत *static_cast<स्थिरांक Node*>(left) == *static_cast<स्थिरांक Node*>(उजवा);
}

तार्किक
StringTrieBuilder::Node::चालक==(स्थिरांक Node &other) स्थिरांक {
    परत हे==&other || (typeid(*हे)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        offset=edge_संख्या;
    }
    परत edge_संख्या;
}

तार्किक
StringTrieBuilder::Final_मूल्य_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Node::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक Final_मूल्य_गाठ &o=static_cast<स्थिरांक Final_मूल्य_गाठ &>(other);
    परत value==o.value;
}

शून्य
StringTrieBuilder::Final_मूल्य_गाठ::लिख(StringTrieBuilder &builder) {
    offset=builder.write_मूल्य_And_Final(value, सत्य);
}

तार्किक
StringTrieBuilder::मूल्य_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Node::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक मूल्य_गाठ &o=static_cast<स्थिरांक मूल्य_गाठ &>(other);
    परत has_मूल्य==o.has_मूल्य && (!has_मूल्य || value==o.value);
}

तार्किक
StringTrieBuilder::Intermediate_मूल्य_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!मूल्य_गाठ::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक Intermediate_मूल्य_गाठ &o=static_cast<स्थिरांक Intermediate_मूल्य_गाठ &>(other);
    परत next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_मूल्य_गाठ::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        offset=edge_संख्या=next->mark_उजवा_Edges_First(edge_संख्या);
    }
    परत edge_संख्या;
}

शून्य
StringTrieBuilder::Intermediate_मूल्य_गाठ::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    offset=builder.write_मूल्य_And_Final(value, असत्य);
}

तार्किक
StringTrieBuilder::Linear_Match_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!मूल्य_गाठ::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक Linear_Match_गाठ &o=static_cast<स्थिरांक Linear_Match_गाठ &>(other);
    परत length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_गाठ::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        offset=edge_संख्या=next->mark_उजवा_Edges_First(edge_संख्या);
    }
    परत edge_संख्या;
}

तार्किक
StringTrieBuilder::List_Branch_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Node::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक List_Branch_गाठ &o=static_cast<स्थिरांक List_Branch_गाठ &>(other);
    साठी(int32_t i=0; i<length; ++i) {
        जर(units[i]!=o.units[i] || मूल्ये[i]!=o.मूल्ये[i] || equal[i]!=o.equal[i]) {
            परत असत्य;
        }
    }
    परत सत्य;
}

int32_t
StringTrieBuilder::List_Branch_गाठ::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        int32_t step=0;
        int32_t i=length;
        करा {
            Node *edge=equal[--i];
            जर(edge!=nullptr) {
                edge_संख्या=edge->mark_उजवा_Edges_First(edge_संख्या-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } पर्यंत(i>0);
        offset=edge_संख्या;
    }
    परत edge_संख्या;
}

शून्य
StringTrieBuilder::List_Branch_गाठ::लिख(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_संख्या=length-1;
    Node *उजवा_Edge=equal[unit_संख्या];
    int32_t उजवा_Edge_संख्या= उजवा_Edge==nullptr ? first_Edge_संख्या : उजवा_Edge->getOffset();
    करा {
        --unit_संख्या;
        जर(equal[unit_संख्या]!=nullptr) {
            equal[unit_संख्या]->write_Unless_Inside_उजवा_Edge(first_Edge_संख्या, उजवा_Edge_संख्या, builder);
        }
    } पर्यंत(unit_संख्या>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_संख्या=length-1;
    जर(उजवा_Edge==nullptr) {
        builder.write_मूल्य_And_Final(मूल्ये[unit_संख्या], सत्य);
    } अन्यथा {
        उजवा_Edge->लिख(builder);
    }
    offset=builder.लिख(units[unit_संख्या]);
    // Write the rest of this node's unit-value pairs.
    पर्यंत(--unit_संख्या>=0) {
        int32_t value;
        UBool isFinal;
        जर(equal[unit_संख्या]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=मूल्ये[unit_संख्या];
            isFinal=सत्य;
        } अन्यथा {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_संख्या]->getOffset()>0);
            value=offset-equal[unit_संख्या]->getOffset();
            isFinal=असत्य;
        }
        builder.write_मूल्य_And_Final(value, isFinal);
        offset=builder.लिख(units[unit_संख्या]);
    }
}

तार्किक
StringTrieBuilder::Split_Branch_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!Node::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक Split_Branch_गाठ &o=static_cast<स्थिरांक Split_Branch_गाठ &>(other);
    परत unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_गाठ::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        first_Edge_संख्या=edge_संख्या;
        edge_संख्या=greaterOrEqual->mark_उजवा_Edges_First(edge_संख्या);
        offset=edge_संख्या=lessThan->mark_उजवा_Edges_First(edge_संख्या-1);
    }
    परत edge_संख्या;
}

शून्य
StringTrieBuilder::Split_Branch_गाठ::लिख(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_उजवा_Edge(first_Edge_संख्या, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->लिख(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.लिख(unit);
}

तार्किक
StringTrieBuilder::Branch_Head_गाठ::चालक==(स्थिरांक Node &other) स्थिरांक {
    जर(हे==&other) {
        परत सत्य;
    }
    जर(!मूल्य_गाठ::चालक==(other)) {
        परत असत्य;
    }
    स्थिरांक Branch_Head_गाठ &o=static_cast<स्थिरांक Branch_Head_गाठ &>(other);
    परत length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_गाठ::mark_उजवा_Edges_First(int32_t edge_संख्या) {
    जर(offset==0) {
        offset=edge_संख्या=next->mark_उजवा_Edges_First(edge_संख्या);
    }
    परत edge_संख्या;
}

शून्य
StringTrieBuilder::Branch_Head_गाठ::लिख(StringTrieBuilder &builder) {
    next->लिख(builder);
    जर(length<=builder.getMinLinearMatch()) {
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, length-1);
    } अन्यथा {
        builder.लिख(length-1);
        offset=builder.write_मूल्य_And_Type(has_मूल्य, value, 0);
    }
}

U_NAMESPACE_END
