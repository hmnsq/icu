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
hash_String_Trie_گرہ(ثابت UHashTok کلید) {
    أرجع icu::StringTrieBuilder::hash_گرہ(کلید.pointer);
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
    nodes=uhash_openSize(hash_String_Trie_گرہ, equalStringTrieNodes, nullptr,
                         sizeGuess, &errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إذا(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } وإلا {
          uhash_set_کلید_Deleter(nodes, uprv_deleteUObject);
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
        write_گرہ(0, elementsLength, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elementsLength, errorCode);
        Node *جڑ=make_گرہ(0, elementsLength, 0, errorCode);
        إذا(U_SUCCESS(errorCode)) {
            جڑ->mark_دایاں_Edges_First(-1);
            جڑ->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گرہ(int32_t start, int32_t limit, int32_t unitIndex) {
    UBool has_قدر=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قدر(start++);
        إذا(start==limit) {
            أرجع write_قدر_And_Final(value, true);  // final-value node
        }
        has_قدر=true;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        write_گرہ(start, limit, lastUnitIndex);
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
        int32_t length=گنتی_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گرہ(start, limit, unitIndex, length);
        إذا(--length<getMinLinearMatch()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_قدر_And_Type(has_قدر, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گرہ(int32_t start, int32_t limit, int32_t unitIndex, int32_t length) {
    char16_t درمیان_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_گرہ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Encode the less-than branch first.
        درمیان_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=write_Branch_Sub_گرہ(start, i, unitIndex, length/2);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_گرہ_Length];
    UBool isFinal[k_Max_Branch_Linear_Sub_گرہ_Length-1];
    int32_t unit_نمبر=0;
    افعل {
        int32_t i=starts[unit_نمبر]=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        isFinal[unit_نمبر]= start==i-1 && unitIndex+1==getElementStringLength(start);
        start=i;
    } طالما(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_نمبر]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_گرہ_Length-1];
    افعل {
        --unit_نمبر;
        إذا(!isFinal[unit_نمبر]) {
            jumpTargets[unit_نمبر]=write_گرہ(starts[unit_نمبر], starts[unit_نمبر+1], unitIndex+1);
        }
    } طالما(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    write_گرہ(start, limit, unitIndex+1);
    int32_t offset=اكتب(getElementUnit(start, unitIndex));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نمبر>=0) {
        start=starts[unit_نمبر];
        int32_t value;
        إذا(isFinal[unit_نمبر]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قدر(start);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_نمبر];
        }
        write_قدر_And_Final(value, isFinal[unit_نمبر]);
        offset=اكتب(getElementUnit(start, unitIndex));
    }
    // Write the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        writeDeltaTo(lessThan[ltLength]);
        offset=اكتب(درمیان_Units[ltLength]);
    }
    أرجع offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گرہ(int32_t start,
                            int32_t limit,
                            int32_t unitIndex,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    UBool has_قدر=خطأ;
    int32_t value=0;
    إذا(unitIndex==getElementStringLength(start)) {
        // An intermediate or final value.
        value=get_Element_قدر(start++);
        إذا(start==limit) {
            أرجع register_Final_قدر(value, errorCode);
        }
        has_قدر=true;
    }
    Node *گرہ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unitIndex);
    int32_t maxUnit=getElementUnit(limit-1, unitIndex);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t lastUnitIndex=getLimitOfLinearMatch(start, limit-1, unitIndex);
        Node *next_گرہ=make_گرہ(start, limit, lastUnitIndex, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=lastUnitIndex-unitIndex;
        int32_t maxLinearMatchLength=getMaxLinearMatchLength();
        طالما(length>maxLinearMatchLength) {
            lastUnitIndex-=maxLinearMatchLength;
            length-=maxLinearMatchLength;
            گرہ=create_Linear_Match_گرہ(start, lastUnitIndex, maxLinearMatchLength, next_گرہ);
            next_گرہ=register_گرہ(گرہ, errorCode);
        }
        گرہ=create_Linear_Match_گرہ(start, unitIndex, length, next_گرہ);
    } وإلا {
        // Branch node.
        int32_t length=گنتی_Element_Units(start, limit, unitIndex);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گرہ=make_Branch_Sub_گرہ(start, limit, unitIndex, length, errorCode);
        گرہ=جديد Branch_Head_گرہ(length, sub_گرہ);
    }
    إذا(has_قدر && گرہ!=nullptr) {
        إذا(match_Nodes_Can_Have_اقدار()) {
            ((قدر_گرہ *)گرہ)->set_قدر(value);
        } وإلا {
            گرہ=جديد Intermediate_قدر_گرہ(value, register_گرہ(گرہ, errorCode));
        }
    }
    أرجع register_گرہ(گرہ, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گرہ(int32_t start,
                                     int32_t limit,
                                     int32_t unitIndex,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    char16_t درمیان_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t ltLength=0;
    طالما(length>get_Max_Branch_Linear_Sub_گرہ_Length()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unitIndex, length/2);
        // Create the less-than branch.
        درمیان_Units[ltLength]=getElementUnit(i, unitIndex);  // middle unit
        lessThan[ltLength]=make_Branch_Sub_گرہ(start, i, unitIndex, length/2, errorCode);
        ++ltLength;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    List_Branch_گرہ *list_گرہ=جديد List_Branch_گرہ();
    إذا(list_گرہ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_نمبر=0;
    افعل {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unitIndex);
        i=indexOfElementWithNextUnit(i, unitIndex, unit);
        إذا(start==i-1 && unitIndex+1==getElementStringLength(start)) {
            list_گرہ->add(unit, get_Element_قدر(start));
        } وإلا {
            list_گرہ->add(unit, make_گرہ(start, i, unitIndex+1, errorCode));
        }
        start=i;
    } طالما(++unit_نمبر<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unitIndex);
    إذا(start==limit-1 && unitIndex+1==getElementStringLength(start)) {
        list_گرہ->add(unit, get_Element_قدر(start));
    } وإلا {
        list_گرہ->add(unit, make_گرہ(start, limit, unitIndex+1, errorCode));
    }
    Node *گرہ=register_گرہ(list_گرہ, errorCode);
    // Create the split-branch nodes.
    طالما(ltLength>0) {
        --ltLength;
        گرہ=register_گرہ(
            جديد Split_Branch_گرہ(درمیان_Units[ltLength], lessThan[ltLength], گرہ), errorCode);
    }
    أرجع گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_گرہ(Node* new_گرہ, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        delete new_گرہ;
        أرجع nullptr;
    }
    إذا(new_گرہ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    ثابت UHashElement *old=uhash_find(nodes, new_گرہ);
    إذا(old!=nullptr) {
        delete new_گرہ;
        أرجع static_cast<Node*>(old->کلید.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گرہ, 1, &errorCode);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_گرہ;
        أرجع nullptr;
    }
    أرجع new_گرہ;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_قدر(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    إذا(U_FAILURE(errorCode)) {
        أرجع nullptr;
    }
    Final_قدر_گرہ کلید(value);
    ثابت UHashElement *old=uhash_find(nodes, &کلید);
    إذا(old!=nullptr) {
        أرجع static_cast<Node*>(old->کلید.pointer);
    }
    Node *new_گرہ=جديد Final_قدر_گرہ(value);
    إذا(new_گرہ==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_گرہ, 1, &errorCode);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(errorCode)) {
        delete new_گرہ;
        أرجع nullptr;
    }
    أرجع new_گرہ;
}

int32_t
StringTrieBuilder::hash_گرہ(ثابت فراغ *گرہ) {
    أرجع static_cast<ثابت Node*>(گرہ)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *left, ثابت فراغ *دایاں) {
    أرجع *static_cast<ثابت Node*>(left) == *static_cast<ثابت Node*>(دایاں);
}

منطقي
StringTrieBuilder::Node::operator==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        offset=edge_نمبر;
    }
    أرجع edge_نمبر;
}

منطقي
StringTrieBuilder::Final_قدر_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Final_قدر_گرہ &o=static_cast<ثابت Final_قدر_گرہ &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    offset=builder.write_قدر_And_Final(value, true);
}

منطقي
StringTrieBuilder::قدر_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت قدر_گرہ &o=static_cast<ثابت قدر_گرہ &>(other);
    أرجع has_قدر==o.has_قدر && (!has_قدر || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_قدر_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قدر_گرہ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_قدر_گرہ &o=static_cast<ثابت Intermediate_قدر_گرہ &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قدر_گرہ::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        offset=edge_نمبر=next->mark_دایاں_Edges_First(edge_نمبر);
    }
    أرجع edge_نمبر;
}

فراغ
StringTrieBuilder::Intermediate_قدر_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    offset=builder.write_قدر_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::Linear_Match_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قدر_گرہ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Linear_Match_گرہ &o=static_cast<ثابت Linear_Match_گرہ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_گرہ::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        offset=edge_نمبر=next->mark_دایاں_Edges_First(edge_نمبر);
    }
    أرجع edge_نمبر;
}

منطقي
StringTrieBuilder::List_Branch_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_گرہ &o=static_cast<ثابت List_Branch_گرہ &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || اقدار[i]!=o.اقدار[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع true;
}

int32_t
StringTrieBuilder::List_Branch_گرہ::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        first_Edge_نمبر=edge_نمبر;
        int32_t step=0;
        int32_t i=length;
        افعل {
            Node *edge=equal[--i];
            إذا(edge!=nullptr) {
                edge_نمبر=edge->mark_دایاں_Edges_First(edge_نمبر-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } طالما(i>0);
        offset=edge_نمبر;
    }
    أرجع edge_نمبر;
}

فراغ
StringTrieBuilder::List_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_نمبر=length-1;
    Node *دایاں_Edge=equal[unit_نمبر];
    int32_t دایاں_Edge_نمبر= دایاں_Edge==nullptr ? first_Edge_نمبر : دایاں_Edge->getOffset();
    افعل {
        --unit_نمبر;
        إذا(equal[unit_نمبر]!=nullptr) {
            equal[unit_نمبر]->write_Unless_Inside_دایاں_Edge(first_Edge_نمبر, دایاں_Edge_نمبر, builder);
        }
    } طالما(unit_نمبر>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_نمبر=length-1;
    إذا(دایاں_Edge==nullptr) {
        builder.write_قدر_And_Final(اقدار[unit_نمبر], true);
    } وإلا {
        دایاں_Edge->اكتب(builder);
    }
    offset=builder.اكتب(units[unit_نمبر]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_نمبر>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_نمبر]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=اقدار[unit_نمبر];
            isFinal=true;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_نمبر]->getOffset()>0);
            value=offset-equal[unit_نمبر]->getOffset();
            isFinal=خطأ;
        }
        builder.write_قدر_And_Final(value, isFinal);
        offset=builder.اكتب(units[unit_نمبر]);
    }
}

منطقي
StringTrieBuilder::Split_Branch_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!Node::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Split_Branch_گرہ &o=static_cast<ثابت Split_Branch_گرہ &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_گرہ::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        first_Edge_نمبر=edge_نمبر;
        edge_نمبر=greaterOrEqual->mark_دایاں_Edges_First(edge_نمبر);
        offset=edge_نمبر=lessThan->mark_دایاں_Edges_First(edge_نمبر-1);
    }
    أرجع edge_نمبر;
}

فراغ
StringTrieBuilder::Split_Branch_گرہ::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_دایاں_Edge(first_Edge_نمبر, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_Head_گرہ::operator==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع true;
    }
    إذا(!قدر_گرہ::operator==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_Head_گرہ &o=static_cast<ثابت Branch_Head_گرہ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_Head_گرہ::mark_دایاں_Edges_First(int32_t edge_نمبر) {
    إذا(offset==0) {
        offset=edge_نمبر=next->mark_دایاں_Edges_First(edge_نمبر);
    }
    أرجع edge_نمبر;
}

فراغ
StringTrieBuilder::Branch_Head_گرہ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.getMinLinearMatch()) {
        offset=builder.write_قدر_And_Type(has_قدر, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        offset=builder.write_قدر_And_Type(has_قدر, value, 0);
    }
}

U_NAMESPACE_END
