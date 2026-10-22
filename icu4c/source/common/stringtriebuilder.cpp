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

#ضمن "utypeinfo.h"  // for 'typeid' to work
#ضمن "unicode/utypes.h"
#ضمن "unicode/stringtriebuilder.h"
#ضمن "uassert.h"
#ضمن "uhash.h"

U_CDECL_BEGIN

ساكن int32_t U_CALLCONV
hash_String_Trie_گرێ(ثابت UHashTok کلیل) {
    أرجع icu::StringTrieBuilder::hash_گرێ(کلیل.pointer);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t sizeGuess, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        أرجع;
    }
    nodes=uhash_openSize(hash_String_Trie_گرێ, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_کلیل_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elementsLength,
                       UErrorCode &errorCode) {
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_گرێ(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *ڕەگ=make_گرێ(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            ڕەگ->mark_ڕاست_Edges_First(-1);
            ڕەگ->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گرێ(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_بەها=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_بەها(start++);
        إذا(start==limit) {
            أرجع write_بەها_And_Final(value, true);  // final-value node
        }
        has_بەها=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_گرێ(start, limit, lastUnitIndex);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            writeElementUnits(start, lastUnitIndex, maxLinearMatchLength);
            اكتب(getMinLinearMatch()+maxLinearMatchLength-1);
        }
        writeElementUnits(start, unitIndex, length);
        type=getMinLinearMatch()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=ژماردن_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گرێ(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_بەها_And_Type(has_بەها, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گرێ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t ناوەڕاست_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_گرێ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        ناوەڕاست_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_گرێ(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_گرێ_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_گرێ_Length-1];
    int32_t unit_ژمار=0;
    افعل {
        int32_t i=starts[unit_ژمار]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_ژمار]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } طالما(++unit_ژمار<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_ژمار]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_گرێ_Length-1];
    افعل {
        --unit_ژمار;
        إذا(!isFinal[unit_ژمار]) {
            jumpTargets[unit_ژمار]=write_گرێ(starts[unit_ژمار], starts[unit_ژمار+1], unitIndex+1);
        }
    } طالما(unit_ژمار>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ژمار=length-1;
    write_گرێ(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_ژمار>=0) {
        start=starts[unit_ژمار];
        int32_t value;
        إذا(isFinal[unit_ژمار]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_بەها(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_ژمار];
        }
        write_بەها_And_Final(value, isFinal[unit_ژمار]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(ناوەڕاست_Units[ltLength]);
    }
    أرجع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گرێ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    UBool has_بەها=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_بەها(start++);
        إذا(start==limit) {
            أرجع register_Final_بەها(value, errorCode);
        }
        has_بەها=true;
    }
    Node *گرێ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_گرێ=make_گرێ(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            گرێ=create_Linear_Match_گرێ(start, lastUnitIndex, maxLinearMatchLength, next_گرێ);
            next_گرێ=register_گرێ(گرێ, errorCode);
        }
        گرێ=create_Linear_Match_گرێ(start, unitIndex, length, next_گرێ);
    } وإلا {
        // Branch node.
        int32_t length=ژماردن_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گرێ=make_Branch_Sub_گرێ(start, limit, unitIndex, length, errorCode);
        گرێ=جديد Branch_Head_گرێ(length, sub_گرێ);
    }
    إذا(has_بەها && گرێ!=nullptr) {
        إذا(match_Nodes_Can_Have_بەهاکان()) {
            ((بەها_گرێ *)گرێ)->set_بەها(value);
        } وإلا {
            گرێ=جديد Intermediate_بەها_گرێ(value, register_گرێ(گرێ, errorCode));
        }
    }
    أرجع register_گرێ(گرێ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گرێ(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    char16_t ناوەڕاست_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_گرێ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        ناوەڕاست_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_گرێ(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    List_Branch_گرێ *list_گرێ=جديد List_Branch_گرێ();
    إذا(list_گرێ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_ژمار=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_گرێ->add(unit, get_Element_بەها(start));
        } وإلا {
            list_گرێ->add(unit, make_گرێ(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } طالما(++unit_ژمار<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_گرێ->add(unit, get_Element_بەها(start));
    } وإلا {
        list_گرێ->add(unit, make_گرێ(start, limit, unitIndex+1, errorCode));
    }
    Node *گرێ=register_گرێ(list_گرێ, errorCode);
    // Create the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        گرێ=register_گرێ(
            جديد Split_Branch_گرێ(ناوەڕاست_Units[ltLength], lessThan[ltLength], گرێ), errorCode);
    }
    أرجع گرێ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_گرێ(Node* new_گرێ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_گرێ;
        أرجع nullptr;
    }
    إذا(new_گرێ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_گرێ);
    إذا(old!=nullptr) {
        delete new_گرێ;
        أرجع static_cast<Node*>(old->کلیل.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_بەها=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گرێ, 1, &errorCode);
    U_ASSERT(old_بەها==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_گرێ;
        أرجع nullptr;
    }
    أرجع new_گرێ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_بەها(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    Final_بەها_گرێ کلیل(value);
    ثابت UHashElement *old=uhash_find(nodes, &کلیل);
    إذا(old!=nullptr) {
        أرجع static_cast<Node*>(old->کلیل.pointer);
    }
    Node *new_گرێ=جديد Final_بەها_گرێ(value);
    إذا(new_گرێ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_بەها=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گرێ, 1, &errorCode);
    U_ASSERT(old_بەها==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_گرێ;
        أرجع nullptr;
    }
    أرجع new_گرێ;
}

int32_t
StringTrieBuilder::hash_گرێ(ثابت فراغ *گرێ) {
    أرجع static_cast<ثابت Node*>(گرێ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *ڕاست) {
    أرجع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(ڕاست);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        offset=edge_ژمار;
    }
    أرجع edge_ژمار;
}

منطقي
StringTrieBuilder::Final_بەها_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Final_بەها_گرێ &o=static_cast<ثابت Final_بەها_گرێ &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_بەها_گرێ::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_بەها_And_Final(value, true);
}

منطقي
StringTrieBuilder::بەها_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت بەها_گرێ &o=static_cast<ثابت بەها_گرێ &>(other);
    أرجع has_بەها==o.has_بەها && (!has_بەها || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_بەها_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!بەها_گرێ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_بەها_گرێ &o=static_cast<ثابت Intermediate_بەها_گرێ &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_بەها_گرێ::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        offset=edge_ژمار=next->mark_ڕاست_Edges_First(edge_ژمار);
    }
    أرجع edge_ژمار;
}

فراغ
StringTrieBuilder::Intermediate_بەها_گرێ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_بەها_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!بەها_گرێ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Linear_Match_گرێ &o=static_cast<ثابت Linear_Match_گرێ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_گرێ::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        offset=edge_ژمار=next->mark_ڕاست_Edges_First(edge_ژمار);
    }
    أرجع edge_ژمار;
}

منطقي
StringTrieBuilder::List_Branch_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_گرێ &o=static_cast<ثابت List_Branch_گرێ &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || بەهاکان[i]!=o.بەهاکان[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

int32_t
StringTrieBuilder::List_Branch_گرێ::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        first_Edge_ژمار=edge_ژمار;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_ژمار=edge->mark_ڕاست_Edges_First(edge_ژمار-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } طالما(i>0);
        offset=edge_ژمار;
    }
    أرجع edge_ژمار;
}

فراغ
StringTrieBuilder::List_Branch_گرێ::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_ژمار=length-1;
    Node *ڕاست_Edge=equal[unit_ژمار];
    int32_t ڕاست_Edge_ژمار= ڕاست_Edge==nullptr ? first_Edge_ژمار : ڕاست_Edge->getOffset();
    افعل {
        --unit_ژمار;
        إذا(equal[unit_ژمار]!=nullptr) {
            equal[unit_ژمار]->write_Unless_Inside_ڕاست_Edge(first_Edge_ژمار, ڕاست_Edge_ژمار, builder);
        }
    } طالما(unit_ژمار>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ژمار=length-1;
    إذا(ڕاست_Edge==nullptr) {
        builder.write_بەها_And_Final(بەهاکان[unit_ژمار], true);
    } وإلا {
        ڕاست_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_ژمار]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_ژمار>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_ژمار]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=بەهاکان[unit_ژمار];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_ژمار]->getOffset()>0);
            value=offset-equal[unit_ژمار]->getOffset();
            isFinal=خطأ;
        }
        builder.write_بەها_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_ژمار]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Split_Branch_گرێ &o=static_cast<ثابت Split_Branch_گرێ &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_گرێ::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        first_Edge_ژمار=edge_ژمار;
        edge_ژمار=greaterOrEqual->mark_ڕاست_Edges_First(edge_ژمار);
        offset=edge_ژمار=lessThan->mark_ڕاست_Edges_First(edge_ژمار-1);
    }
    أرجع edge_ژمار;
}

فراغ
StringTrieBuilder::Split_Branch_گرێ::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ڕاست_Edge(first_Edge_ژمار, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_گرێ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!بەها_گرێ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_Head_گرێ &o=static_cast<ثابت Branch_Head_گرێ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_گرێ::mark_ڕاست_Edges_First(int32_t edge_ژمار) {
    إذا(offset==0) {
        offset=edge_ژمار=next->mark_ڕاست_Edges_First(edge_ژمار);
    }
    أرجع edge_ژمار;
}

فراغ
StringTrieBuilder::Branch_Head_گرێ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_بەها_And_Type(has_بەها, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_بەها_And_Type(has_بەها, value, 0);
    }
}

U_NAMESPACE_END
