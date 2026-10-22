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

#અન્તર્ભાવ "utypeinfo.h"  // for 'typeid' to work
#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/stringtriebuilder.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "uhash.h"

U_CDECL_BEGIN

સ્થિર int32_t U_CALLCONV
hash_String_Trie_ગાંઠ(અચલ UHashTok ચાવી) {
    પાછા icu::StringTrieBuilder::hash_ગાંઠ(ચાવી.pointer);
}

સ્થિર UBool U_CALLCONV
equalStringTrieNodes(અચલ UHashTok key1, અચલ UHashTok key2) {
    પાછા icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

શૂન્ય
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    nodes=uhash_openSize(hash_String_Trie_ગાંઠ, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    જો(U_SUCCESS(errorCode)) {
        જો(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
          uhash_set_ચાવી_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

શૂન્ય
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

શૂન્ય
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    જો(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ગાંઠ(0, elementsLength, 0);
    } નહીંતર /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *મૂળ=make_ગાંઠ(0, elementsLength, 0, errorCode);
        જો(U_SUCCESS(errorCode)) {
            મૂળ->mark_જમણું_Edges_First(-1);
            મૂળ->લિખ(*આ);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ગાંઠ(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_કિંમત=ખોટું;
    int32_t value=0;
    int32_t type;
    જો(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_કિંમત(start++);
        જો(start==limit) {
            પાછા write_કિંમત_And_Final(value, સાચું);  // final-value node
        }
        has_કિંમત=સાચું;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    જો(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_ગાંઠ(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        જ્યારે(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            લિખ(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } નહીંતર {
        // Branch node.
        int32_t length=ગણતરી_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ગાંઠ(start, limit, unitIndex, length);
        જો(--length<getMinLinearMatch()) {
            type=length;
        } નહીંતર {
            લિખ(length);
            type=0;
        }
    }
    પાછા write_કિંમત_And_Type(has_કિંમત, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ગાંઠ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t મધ્ય_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    જ્યારે(length>get_Max_Branch_Linear_Sub_ગાંઠ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        મધ્ય_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_ગાંઠ(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_ગાંઠ_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_ગાંઠ_Length-1];
    int32_t unit_સંખ્યા=0;
    કરો {
        int32_t i=starts[unit_સંખ્યા]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_સંખ્યા]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } જ્યારે(++unit_સંખ્યા<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_સંખ્યા]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_ગાંઠ_Length-1];
    કરો {
        --unit_સંખ્યા;
        જો(!isFinal[unit_સંખ્યા]) {
            jumpTargets[unit_સંખ્યા]=write_ગાંઠ(starts[unit_સંખ્યા], starts[unit_સંખ્યા+1], unitIndex+1);
        }
    } જ્યારે(unit_સંખ્યા>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_સંખ્યા=length-1;
    write_ગાંઠ(start, limit, unitIndex+1);
    int32_t offset=લિખ(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    જ્યારે(--unit_સંખ્યા>=0) {
        start=starts[unit_સંખ્યા];
        int32_t value;
        જો(isFinal[unit_સંખ્યા]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_કિંમત(start);
        } નહીંતર {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_સંખ્યા];
        }
        write_કિંમત_And_Final(value, isFinal[unit_સંખ્યા]);
        offset=લિખ(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    જ્યારે(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=લિખ(મધ્ય_Units[ltLength]);
    }
    પાછા offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ગાંઠ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    UBool has_કિંમત=ખોટું;
    int32_t value=0;
    જો(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_કિંમત(start++);
        જો(start==limit) {
            પાછા register_Final_કિંમત(value, errorCode);
        }
        has_કિંમત=સાચું;
    }
    Node *ગાંઠ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    જો(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_ગાંઠ=make_ગાંઠ(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        જ્યારે(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            ગાંઠ=create_Linear_Match_ગાંઠ(start, lastUnitIndex, maxLinearMatchLength, next_ગાંઠ);
            next_ગાંઠ=register_ગાંઠ(ગાંઠ, errorCode);
        }
        ગાંઠ=create_Linear_Match_ગાંઠ(start, unitIndex, length, next_ગાંઠ);
    } નહીંતર {
        // Branch node.
        int32_t length=ગણતરી_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ગાંઠ=make_Branch_Sub_ગાંઠ(start, limit, unitIndex, length, errorCode);
        ગાંઠ=નવું Branch_Head_ગાંઠ(length, sub_ગાંઠ);
    }
    જો(has_કિંમત && ગાંઠ!=nullptr) {
        જો(match_Nodes_Can_Have_કિંમતો()) {
            ((કિંમત_ગાંઠ *)ગાંઠ)->set_કિંમત(value);
        } નહીંતર {
            ગાંઠ=નવું Intermediate_કિંમત_ગાંઠ(value, register_ગાંઠ(ગાંઠ, errorCode));
        }
    }
    પાછા register_ગાંઠ(ગાંઠ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ગાંઠ(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    char16_t મધ્ય_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    જ્યારે(length>get_Max_Branch_Linear_Sub_ગાંઠ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        મધ્ય_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_ગાંઠ(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    List_Branch_ગાંઠ *list_ગાંઠ=નવું List_Branch_ગાંઠ();
    જો(list_ગાંઠ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_સંખ્યા=0;
    કરો {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        જો(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_ગાંઠ->add(unit, get_Element_કિંમત(start));
        } નહીંતર {
            list_ગાંઠ->add(unit, make_ગાંઠ(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } જ્યારે(++unit_સંખ્યા<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    જો(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_ગાંઠ->add(unit, get_Element_કિંમત(start));
    } નહીંતર {
        list_ગાંઠ->add(unit, make_ગાંઠ(start, limit, unitIndex+1, errorCode));
    }
    Node *ગાંઠ=register_ગાંઠ(list_ગાંઠ, errorCode);
    // Create the split-branch nodes.
    જ્યારે(ltLength>0) {
        --ltLength;
        ગાંઠ=register_ગાંઠ(
            નવું Split_Branch_ગાંઠ(મધ્ય_Units[ltLength], lessThan[ltLength], ગાંઠ), errorCode);
    }
    પાછા ગાંઠ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_ગાંઠ(Node* new_ગાંઠ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો(U_FAILURE(errorCode)) {
        delete new_ગાંઠ;
        પાછા nullptr;
    }
    જો(new_ગાંઠ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    અચલ UHashElement *old=uhash_find(nodes, new_ગાંઠ);
    જો(old!=nullptr) {
        delete new_ગાંઠ;
        પાછા static_cast<Node*>(old->ચાવી.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#જો U_DEBUG
    int32_t old_કિંમત=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ગાંઠ, 1, &errorCode);
    U_ASSERT(old_કિંમત==0);
    જો(U_FAILURE(errorCode)) {
        delete new_ગાંઠ;
        પાછા nullptr;
    }
    પાછા new_ગાંઠ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_કિંમત(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    Final_કિંમત_ગાંઠ ચાવી(value);
    અચલ UHashElement *old=uhash_find(nodes, &ચાવી);
    જો(old!=nullptr) {
        પાછા static_cast<Node*>(old->ચાવી.pointer);
    }
    Node *new_ગાંઠ=નવું Final_કિંમત_ગાંઠ(value);
    જો(new_ગાંઠ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#જો U_DEBUG
    int32_t old_કિંમત=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_ગાંઠ, 1, &errorCode);
    U_ASSERT(old_કિંમત==0);
    જો(U_FAILURE(errorCode)) {
        delete new_ગાંઠ;
        પાછા nullptr;
    }
    પાછા new_ગાંઠ;
}

int32_t
StringTrieBuilder::hash_ગાંઠ(અચલ શૂન્ય *ગાંઠ) {
    પાછા static_cast<અચલ Node*>(ગાંઠ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(અચલ શૂન્ય *left, અચલ શૂન્ય *જમણું) {
    પાછા *static_cast<અચલ Node*>(left) == *static_cast<અચલ Node*>(જમણું);
}

તાર્કિક
StringTrieBuilder::Node::operator==(અચલ Node &other) અચલ {
    પાછા આ==&other || (typeid(*આ)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        offset=edge_સંખ્યા;
    }
    પાછા edge_સંખ્યા;
}

તાર્કિક
StringTrieBuilder::Final_કિંમત_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Node::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ Final_કિંમત_ગાંઠ &o=static_cast<અચલ Final_કિંમત_ગાંઠ &>(other);
    પાછા value==o.value;
}

શૂન્ય
StringTrieBuilder::Final_કિંમત_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    offset=builder.write_કિંમત_And_Final(value, સાચું);
}

તાર્કિક
StringTrieBuilder::કિંમત_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Node::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ કિંમત_ગાંઠ &o=static_cast<અચલ કિંમત_ગાંઠ &>(other);
    પાછા has_કિંમત==o.has_કિંમત && (!has_કિંમત || value==o.value);
}

તાર્કિક
StringTrieBuilder::Intermediate_કિંમત_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!કિંમત_ગાંઠ::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ Intermediate_કિંમત_ગાંઠ &o=static_cast<અચલ Intermediate_કિંમત_ગાંઠ &>(other);
    પાછા next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_કિંમત_ગાંઠ::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        offset=edge_સંખ્યા=next->mark_જમણું_Edges_First(edge_સંખ્યા);
    }
    પાછા edge_સંખ્યા;
}

શૂન્ય
StringTrieBuilder::Intermediate_કિંમત_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    next->લિખ(builder);
    offset=builder.write_કિંમત_And_Final(value, ખોટું);
}

તાર્કિક
StringTrieBuilder::Linear_Match_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!કિંમત_ગાંઠ::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ Linear_Match_ગાંઠ &o=static_cast<અચલ Linear_Match_ગાંઠ &>(other);
    પાછા length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_ગાંઠ::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        offset=edge_સંખ્યા=next->mark_જમણું_Edges_First(edge_સંખ્યા);
    }
    પાછા edge_સંખ્યા;
}

તાર્કિક
StringTrieBuilder::List_Branch_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Node::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ List_Branch_ગાંઠ &o=static_cast<અચલ List_Branch_ગાંઠ &>(other);
    માટે(int32_t i=0; i<length; ++i) {
        જો(units[i]!=o.units[i] || કિંમતો[i]!=o.કિંમતો[i] || equal[i]!=o.equal[i]) {
            પાછા ખોટું;
        }
    }
    પાછા સાચું;
}

int32_t
StringTrieBuilder::List_Branch_ગાંઠ::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        first_Edge_સંખ્યા=edge_સંખ્યા;
        int32_t step=0;
        int32_t i=length;
        કરો {
            Node *edge=equal[--i];
            જો(edge!=nullptr) {
                edge_સંખ્યા=edge->mark_જમણું_Edges_First(edge_સંખ્યા-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } જ્યારે(i>0);
        offset=edge_સંખ્યા;
    }
    પાછા edge_સંખ્યા;
}

શૂન્ય
StringTrieBuilder::List_Branch_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_સંખ્યા=length-1;
    Node *જમણું_Edge=equal[unit_સંખ્યા];
    int32_t જમણું_Edge_સંખ્યા= જમણું_Edge==nullptr ? first_Edge_સંખ્યા : જમણું_Edge->getOffset();
    કરો {
        --unit_સંખ્યા;
        જો(equal[unit_સંખ્યા]!=nullptr) {
            equal[unit_સંખ્યા]->write_Unless_Inside_જમણું_Edge(first_Edge_સંખ્યા, જમણું_Edge_સંખ્યા, builder);
        }
    } જ્યારે(unit_સંખ્યા>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_સંખ્યા=length-1;
    જો(જમણું_Edge==nullptr) {
        builder.write_કિંમત_And_Final(કિંમતો[unit_સંખ્યા], સાચું);
    } નહીંતર {
        જમણું_Edge->લિખ(builder);
    }
    offset=builder.લિખ(units[unit_સંખ્યા]);
    // Write the rest of this node's unit-value pairs.
    જ્યારે(--unit_સંખ્યા>=0) {
        int32_t value;
        UBool isFinal;
        જો(equal[unit_સંખ્યા]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=કિંમતો[unit_સંખ્યા];
            isFinal=સાચું;
        } નહીંતર {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_સંખ્યા]->getOffset()>0);
            value=offset-equal[unit_સંખ્યા]->getOffset();
            isFinal=ખોટું;
        }
        builder.write_કિંમત_And_Final(value, isFinal);
        offset=builder.લિખ(units[unit_સંખ્યા]);
    }
}

તાર્કિક
StringTrieBuilder::Split_Branch_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!Node::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ Split_Branch_ગાંઠ &o=static_cast<અચલ Split_Branch_ગાંઠ &>(other);
    પાછા unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_ગાંઠ::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        first_Edge_સંખ્યા=edge_સંખ્યા;
        edge_સંખ્યા=greaterOrEqual->mark_જમણું_Edges_First(edge_સંખ્યા);
        offset=edge_સંખ્યા=lessThan->mark_જમણું_Edges_First(edge_સંખ્યા-1);
    }
    પાછા edge_સંખ્યા;
}

શૂન્ય
StringTrieBuilder::Split_Branch_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_જમણું_Edge(first_Edge_સંખ્યા, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->લિખ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.લિખ(unit);
}

તાર્કિક
StringTrieBuilder::Branch_Head_ગાંઠ::operator==(અચલ Node &other) અચલ {
    જો(આ==&other) {
        પાછા સાચું;
    }
    જો(!કિંમત_ગાંઠ::operator==(other)) {
        પાછા ખોટું;
    }
    અચલ Branch_Head_ગાંઠ &o=static_cast<અચલ Branch_Head_ગાંઠ &>(other);
    પાછા length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_ગાંઠ::mark_જમણું_Edges_First(int32_t edge_સંખ્યા) {
    જો(offset==0) {
        offset=edge_સંખ્યા=next->mark_જમણું_Edges_First(edge_સંખ્યા);
    }
    પાછા edge_સંખ્યા;
}

શૂન્ય
StringTrieBuilder::Branch_Head_ગાંઠ::લિખ(StringTrieBuilder &builder) {
    next->લિખ(builder);
    જો(length<=builder.getMinLinearMatch()) {
        offset=builder.write_કિંમત_And_Type(has_કિંમત, value, length-1);
    } નહીંતર {
        builder.લિખ(length-1);
        offset=builder.write_કિંમત_And_Type(has_કિંમત, value, 0);
    }
}

U_NAMESPACE_END
