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

#অন্তর্ভাব "utypeinfo.h"  // for 'typeid' to work
#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/stringtriebuilder.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "uhash.h"

U_CDECL_BEGIN

স্থির int32_t U_CALLCONV
hash_String_Trie_গ্রন্থি(ধ্রুবক UHashTok চাবি) {
    ফেরত icu::StringTrieBuilder::hash_গ্রন্থি(চাবি.pointer);
}

স্থির UBool U_CALLCONV
equalStringTrieNodes(ধ্রুবক UHashTok key1, ধ্রুবক UHashTok key2) {
    ফেরত icu::StringTrieBuilder::equalNodes(key1.pointer, key2.pointer);
}

U_CDECL_END

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(nullptr) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

শূন্য
StringTrieBuilder::createCompactBuilder(int32_t আকার_Guess, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    nodes=uhash_open_আকার(hash_String_Trie_গ্রন্থি, equalStringTrieNodes, nullptr,
                         আকার_Guess, &errorCode);
    যদি(U_SUCCESS(errorCode)) {
        যদি(nodes==nullptr) {
          errorCode=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
          uhash_set_চাবি_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

শূন্য
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=nullptr;
}

শূন্য
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_দৈর্ঘ্য,
                       UErrorCode &errorCode) {
    যদি(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_গ্রন্থি(0, elements_দৈর্ঘ্য, 0);
    } অন্যথা /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_দৈর্ঘ্য, errorCode);
        Node *মূল=make_গ্রন্থি(0, elements_দৈর্ঘ্য, 0, errorCode);
        যদি(U_SUCCESS(errorCode)) {
            মূল->mark_ডান_Edges_First(-1);
            মূল->লিখ(*এই);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_গ্রন্থি(int32_t start, int32_t limit, int32_t unit_সূচক) {
    UBool has_মান=মিথ্যা;
    int32_t value=0;
    int32_t type;
    যদি(unit_সূচক==get_Element_String_দৈর্ঘ্য(start)) {
        // An intermediate or final value.
        value=get_Element_মান(start++);
        যদি(start==limit) {
            ফেরত write_মান_And_Final(value, সত্য);  // final-value node
        }
        has_মান=সত্য;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_সূচক);
    int32_t maxUnit=getElementUnit(limit-1, unit_সূচক);
    যদি(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t last_Unit_সূচক=getLimitOfLinearMatch(start, limit-1, unit_সূচক);
        write_গ্রন্থি(start, limit, last_Unit_সূচক);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=last_Unit_সূচক-unit_সূচক;
        int32_t max_Linear_Match_দৈর্ঘ্য=get_Max_Linear_Match_দৈর্ঘ্য();
        যখন(length>max_Linear_Match_দৈর্ঘ্য) {
            last_Unit_সূচক-=max_Linear_Match_দৈর্ঘ্য;
            length-=max_Linear_Match_দৈর্ঘ্য;
            writeElementUnits(start, last_Unit_সূচক, max_Linear_Match_দৈর্ঘ্য);
            লিখ(getMinLinearMatch()+max_Linear_Match_দৈর্ঘ্য-1);
        }
        writeElementUnits(start, unit_সূচক, length);
        type=getMinLinearMatch()+length-1;
    } অন্যথা {
        // Branch node.
        int32_t length=গুনতি_Element_Units(start, limit, unit_সূচক);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_গ্রন্থি(start, limit, unit_সূচক, length);
        যদি(--length<getMinLinearMatch()) {
            type=length;
        } অন্যথা {
            লিখ(length);
            type=0;
        }
    }
    ফেরত write_মান_And_Type(has_মান, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_গ্রন্থি(int32_t start, int32_t limit, int32_t unit_সূচক, int32_t length) {
    char16_t মধ্য_Units[kMaxSplitBranchLevels];
    int32_t lessThan[kMaxSplitBranchLevels];
    int32_t lt_দৈর্ঘ্য=0;
    যখন(length>get_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_সূচক, length/2);
        // Encode the less-than branch first.
        মধ্য_Units[lt_দৈর্ঘ্য]=getElementUnit(i, unit_সূচক);  // middle unit
        lessThan[lt_দৈর্ঘ্য]=write_Branch_Sub_গ্রন্থি(start, i, unit_সূচক, length/2);
        ++lt_দৈর্ঘ্য;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য];
    UBool isFinal[k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য-1];
    int32_t unit_সংখ্যা=0;
    করো {
        int32_t i=starts[unit_সংখ্যা]=start;
        char16_t unit=getElementUnit(i++, unit_সূচক);
        i=সূচক_Of_Element_With_পরবর্তী_Unit(i, unit_সূচক, unit);
        isFinal[unit_সংখ্যা]= start==i-1 && unit_সূচক+1==get_Element_String_দৈর্ঘ্য(start);
        start=i;
    } যখন(++unit_সংখ্যা<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_সংখ্যা]=start;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য-1];
    করো {
        --unit_সংখ্যা;
        যদি(!isFinal[unit_সংখ্যা]) {
            jumpTargets[unit_সংখ্যা]=write_গ্রন্থি(starts[unit_সংখ্যা], starts[unit_সংখ্যা+1], unit_সূচক+1);
        }
    } যখন(unit_সংখ্যা>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_সংখ্যা=length-1;
    write_গ্রন্থি(start, limit, unit_সূচক+1);
    int32_t offset=লিখ(getElementUnit(start, unit_সূচক));
    // Write the rest of this node's unit-value pairs.
    যখন(--unit_সংখ্যা>=0) {
        start=starts[unit_সংখ্যা];
        int32_t value;
        যদি(isFinal[unit_সংখ্যা]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_মান(start);
        } অন্যথা {
            // Write the delta to the start position of the sub-node.
            value=offset-jumpTargets[unit_সংখ্যা];
        }
        write_মান_And_Final(value, isFinal[unit_সংখ্যা]);
        offset=লিখ(getElementUnit(start, unit_সূচক));
    }
    // Write the split-branch nodes.
    যখন(lt_দৈর্ঘ্য>0) {
        --lt_দৈর্ঘ্য;
        writeDeltaTo(lessThan[lt_দৈর্ঘ্য]);
        offset=লিখ(মধ্য_Units[lt_দৈর্ঘ্য]);
    }
    ফেরত offset;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_গ্রন্থি(int32_t start,
                            int32_t limit,
                            int32_t unit_সূচক,
                            UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    UBool has_মান=মিথ্যা;
    int32_t value=0;
    যদি(unit_সূচক==get_Element_String_দৈর্ঘ্য(start)) {
        // An intermediate or final value.
        value=get_Element_মান(start++);
        যদি(start==limit) {
            ফেরত register_Final_মান(value, errorCode);
        }
        has_মান=সত্য;
    }
    Node *গ্রন্থি;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(start, unit_সূচক);
    int32_t maxUnit=getElementUnit(limit-1, unit_সূচক);
    যদি(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t last_Unit_সূচক=getLimitOfLinearMatch(start, limit-1, unit_সূচক);
        Node *পরবর্তী_গ্রন্থি=make_গ্রন্থি(start, limit, last_Unit_সূচক, errorCode);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=last_Unit_সূচক-unit_সূচক;
        int32_t max_Linear_Match_দৈর্ঘ্য=get_Max_Linear_Match_দৈর্ঘ্য();
        যখন(length>max_Linear_Match_দৈর্ঘ্য) {
            last_Unit_সূচক-=max_Linear_Match_দৈর্ঘ্য;
            length-=max_Linear_Match_দৈর্ঘ্য;
            গ্রন্থি=create_Linear_Match_গ্রন্থি(start, last_Unit_সূচক, max_Linear_Match_দৈর্ঘ্য, পরবর্তী_গ্রন্থি);
            পরবর্তী_গ্রন্থি=register_গ্রন্থি(গ্রন্থি, errorCode);
        }
        গ্রন্থি=create_Linear_Match_গ্রন্থি(start, unit_সূচক, length, পরবর্তী_গ্রন্থি);
    } অন্যথা {
        // Branch node.
        int32_t length=গুনতি_Element_Units(start, limit, unit_সূচক);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_গ্রন্থি=make_Branch_Sub_গ্রন্থি(start, limit, unit_সূচক, length, errorCode);
        গ্রন্থি=নতুন Branch_শীর্ষ_গ্রন্থি(length, sub_গ্রন্থি);
    }
    যদি(has_মান && গ্রন্থি!=nullptr) {
        যদি(match_Nodes_Can_Have_মানসমূহ()) {
            ((মান_গ্রন্থি *)গ্রন্থি)->set_মান(value);
        } অন্যথা {
            গ্রন্থি=নতুন Intermediate_মান_গ্রন্থি(value, register_গ্রন্থি(গ্রন্থি, errorCode));
        }
    }
    ফেরত register_গ্রন্থি(গ্রন্থি, errorCode);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_গ্রন্থি(int32_t start,
                                     int32_t limit,
                                     int32_t unit_সূচক,
                                     int32_t length,
                                     UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    char16_t মধ্য_Units[kMaxSplitBranchLevels];
    Node *lessThan[kMaxSplitBranchLevels];
    int32_t lt_দৈর্ঘ্য=0;
    যখন(length>get_Max_Branch_Linear_Sub_গ্রন্থি_দৈর্ঘ্য()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(start, unit_সূচক, length/2);
        // Create the less-than branch.
        মধ্য_Units[lt_দৈর্ঘ্য]=getElementUnit(i, unit_সূচক);  // middle unit
        lessThan[lt_দৈর্ঘ্য]=make_Branch_Sub_গ্রন্থি(start, i, unit_সূচক, length/2, errorCode);
        ++lt_দৈর্ঘ্য;
        // Continue for the greater-or-equal branch.
        start=i;
        length=length-length/2;
    }
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    List_Branch_গ্রন্থি *list_গ্রন্থি=নতুন List_Branch_গ্রন্থি();
    যদি(list_গ্রন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_সংখ্যা=0;
    করো {
        int32_t i=start;
        char16_t unit=getElementUnit(i++, unit_সূচক);
        i=সূচক_Of_Element_With_পরবর্তী_Unit(i, unit_সূচক, unit);
        যদি(start==i-1 && unit_সূচক+1==get_Element_String_দৈর্ঘ্য(start)) {
            list_গ্রন্থি->add(unit, get_Element_মান(start));
        } অন্যথা {
            list_গ্রন্থি->add(unit, make_গ্রন্থি(start, i, unit_সূচক+1, errorCode));
        }
        start=i;
    } যখন(++unit_সংখ্যা<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(start, unit_সূচক);
    যদি(start==limit-1 && unit_সূচক+1==get_Element_String_দৈর্ঘ্য(start)) {
        list_গ্রন্থি->add(unit, get_Element_মান(start));
    } অন্যথা {
        list_গ্রন্থি->add(unit, make_গ্রন্থি(start, limit, unit_সূচক+1, errorCode));
    }
    Node *গ্রন্থি=register_গ্রন্থি(list_গ্রন্থি, errorCode);
    // Create the split-branch nodes.
    যখন(lt_দৈর্ঘ্য>0) {
        --lt_দৈর্ঘ্য;
        গ্রন্থি=register_গ্রন্থি(
            নতুন Split_Branch_গ্রন্থি(মধ্য_Units[lt_দৈর্ঘ্য], lessThan[lt_দৈর্ঘ্য], গ্রন্থি), errorCode);
    }
    ফেরত গ্রন্থি;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_গ্রন্থি(Node* new_গ্রন্থি, UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্রন্থি;
        ফেরত nullptr;
    }
    যদি(new_গ্রন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    ধ্রুবক UHashElement *old=uhash_find(nodes, new_গ্রন্থি);
    যদি(old!=nullptr) {
        delete new_গ্রন্থি;
        ফেরত static_cast<Node*>(old->চাবি.pointer);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#যদি U_DEBUG
    int32_t old_মান=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_গ্রন্থি, 1, &errorCode);
    U_ASSERT(old_মান==0);
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্রন্থি;
        ফেরত nullptr;
    }
    ফেরত new_গ্রন্থি;
}

StringTrieBuilder::Node*
StringTrieBuilder::register_Final_মান(int32_t value, UErrorCode& errorCode) U_LIFETIME_BOUND {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    Final_মান_গ্রন্থি চাবি(value);
    ধ্রুবক UHashElement *old=uhash_find(nodes, &চাবি);
    যদি(old!=nullptr) {
        ফেরত static_cast<Node*>(old->চাবি.pointer);
    }
    Node *new_গ্রন্থি=নতুন Final_মান_গ্রন্থি(value);
    যদি(new_গ্রন্থি==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#যদি U_DEBUG
    int32_t old_মান=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#endif
    uhash_puti(nodes, new_গ্রন্থি, 1, &errorCode);
    U_ASSERT(old_মান==0);
    যদি(U_FAILURE(errorCode)) {
        delete new_গ্রন্থি;
        ফেরত nullptr;
    }
    ফেরত new_গ্রন্থি;
}

int32_t
StringTrieBuilder::hash_গ্রন্থি(ধ্রুবক শূন্য *গ্রন্থি) {
    ফেরত static_cast<ধ্রুবক Node*>(গ্রন্থি)->hashCode();
}

UBool
StringTrieBuilder::equalNodes(ধ্রুবক শূন্য *left, ধ্রুবক শূন্য *ডান) {
    ফেরত *static_cast<ধ্রুবক Node*>(left) == *static_cast<ধ্রুবক Node*>(ডান);
}

যৌক্তিক
StringTrieBuilder::Node::operator==(ধ্রুবক Node &other) ধ্রুবক {
    ফেরত এই==&other || (typeid(*এই)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা;
    }
    ফেরত edge_সংখ্যা;
}

যৌক্তিক
StringTrieBuilder::Final_মান_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Node::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক Final_মান_গ্রন্থি &o=static_cast<ধ্রুবক Final_মান_গ্রন্থি &>(other);
    ফেরত value==o.value;
}

শূন্য
StringTrieBuilder::Final_মান_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    offset=builder.write_মান_And_Final(value, সত্য);
}

যৌক্তিক
StringTrieBuilder::মান_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Node::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক মান_গ্রন্থি &o=static_cast<ধ্রুবক মান_গ্রন্থি &>(other);
    ফেরত has_মান==o.has_মান && (!has_মান || value==o.value);
}

যৌক্তিক
StringTrieBuilder::Intermediate_মান_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!মান_গ্রন্থি::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক Intermediate_মান_গ্রন্থি &o=static_cast<ধ্রুবক Intermediate_মান_গ্রন্থি &>(other);
    ফেরত next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_মান_গ্রন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ফেরত edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Intermediate_মান_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    next->লিখ(builder);
    offset=builder.write_মান_And_Final(value, মিথ্যা);
}

যৌক্তিক
StringTrieBuilder::Linear_Match_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!মান_গ্রন্থি::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক Linear_Match_গ্রন্থি &o=static_cast<ধ্রুবক Linear_Match_গ্রন্থি &>(other);
    ফেরত length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Linear_Match_গ্রন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ফেরত edge_সংখ্যা;
}

যৌক্তিক
StringTrieBuilder::List_Branch_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Node::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক List_Branch_গ্রন্থি &o=static_cast<ধ্রুবক List_Branch_গ্রন্থি &>(other);
    জন্য(int32_t i=0; i<length; ++i) {
        যদি(units[i]!=o.units[i] || মানসমূহ[i]!=o.মানসমূহ[i] || equal[i]!=o.equal[i]) {
            ফেরত মিথ্যা;
        }
    }
    ফেরত সত্য;
}

int32_t
StringTrieBuilder::List_Branch_গ্রন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        first_Edge_সংখ্যা=edge_সংখ্যা;
        int32_t step=0;
        int32_t i=length;
        করো {
            Node *edge=equal[--i];
            যদি(edge!=nullptr) {
                edge_সংখ্যা=edge->mark_ডান_Edges_First(edge_সংখ্যা-step);
            }
            // For all but the rightmost edge, decrement the edge number.
            step=1;
        } যখন(i>0);
        offset=edge_সংখ্যা;
    }
    ফেরত edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::List_Branch_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_সংখ্যা=length-1;
    Node *ডান_Edge=equal[unit_সংখ্যা];
    int32_t ডান_Edge_সংখ্যা= ডান_Edge==nullptr ? first_Edge_সংখ্যা : ডান_Edge->getOffset();
    করো {
        --unit_সংখ্যা;
        যদি(equal[unit_সংখ্যা]!=nullptr) {
            equal[unit_সংখ্যা]->write_Unless_Inside_ডান_Edge(first_Edge_সংখ্যা, ডান_Edge_সংখ্যা, builder);
        }
    } যখন(unit_সংখ্যা>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_সংখ্যা=length-1;
    যদি(ডান_Edge==nullptr) {
        builder.write_মান_And_Final(মানসমূহ[unit_সংখ্যা], সত্য);
    } অন্যথা {
        ডান_Edge->লিখ(builder);
    }
    offset=builder.লিখ(units[unit_সংখ্যা]);
    // Write the rest of this node's unit-value pairs.
    যখন(--unit_সংখ্যা>=0) {
        int32_t value;
        UBool isFinal;
        যদি(equal[unit_সংখ্যা]==nullptr) {
            // Write the final value for the one string ending with this unit.
            value=মানসমূহ[unit_সংখ্যা];
            isFinal=সত্য;
        } অন্যথা {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_সংখ্যা]->getOffset()>0);
            value=offset-equal[unit_সংখ্যা]->getOffset();
            isFinal=মিথ্যা;
        }
        builder.write_মান_And_Final(value, isFinal);
        offset=builder.লিখ(units[unit_সংখ্যা]);
    }
}

যৌক্তিক
StringTrieBuilder::Split_Branch_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!Node::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক Split_Branch_গ্রন্থি &o=static_cast<ধ্রুবক Split_Branch_গ্রন্থি &>(other);
    ফেরত unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::Split_Branch_গ্রন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        first_Edge_সংখ্যা=edge_সংখ্যা;
        edge_সংখ্যা=greaterOrEqual->mark_ডান_Edges_First(edge_সংখ্যা);
        offset=edge_সংখ্যা=lessThan->mark_ডান_Edges_First(edge_সংখ্যা-1);
    }
    ফেরত edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Split_Branch_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ডান_Edge(first_Edge_সংখ্যা, greaterOrEqual->getOffset(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->লিখ(builder);
    // Write this node.
    U_ASSERT(lessThan->getOffset()>0);
    builder.writeDeltaTo(lessThan->getOffset());  // less-than
    offset=builder.লিখ(unit);
}

যৌক্তিক
StringTrieBuilder::Branch_শীর্ষ_গ্রন্থি::operator==(ধ্রুবক Node &other) ধ্রুবক {
    যদি(এই==&other) {
        ফেরত সত্য;
    }
    যদি(!মান_গ্রন্থি::operator==(other)) {
        ফেরত মিথ্যা;
    }
    ধ্রুবক Branch_শীর্ষ_গ্রন্থি &o=static_cast<ধ্রুবক Branch_শীর্ষ_গ্রন্থি &>(other);
    ফেরত length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_শীর্ষ_গ্রন্থি::mark_ডান_Edges_First(int32_t edge_সংখ্যা) {
    যদি(offset==0) {
        offset=edge_সংখ্যা=next->mark_ডান_Edges_First(edge_সংখ্যা);
    }
    ফেরত edge_সংখ্যা;
}

শূন্য
StringTrieBuilder::Branch_শীর্ষ_গ্রন্থি::লিখ(StringTrieBuilder &builder) {
    next->লিখ(builder);
    যদি(length<=builder.getMinLinearMatch()) {
        offset=builder.write_মান_And_Type(has_মান, value, length-1);
    } অন্যথা {
        builder.লিখ(length-1);
        offset=builder.write_মান_And_Type(has_মান, value, 0);
    }
}

U_NAMESPACE_END
