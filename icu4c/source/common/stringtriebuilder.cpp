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

#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/stringtriebuilder.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"

U_CDECL_BEGIN

ਸਥਿਰ int32_t U_CALLCONV
hash_String_Trie_ਗੰਢ(ਅਚਲ UHashTok ਕੁੰਜੀ) {
    ਵਾਪਸ icu::StringTrieBuilder::hash_ਗੰਢ(ਕੁੰਜੀ.ਸੂਚਕ);
}

ਸਥਿਰ UBool U_CALLCONV
equalStringTrieNodes(ਅਚਲ UHashTok key1, ਅਚਲ UHashTok key2) {
    ਵਾਪਸ icu::StringTrieBuilder::equalNodes(key1.ਸੂਚਕ, key2.ਸੂਚਕ);
}

U_CDECL_ਸਿਰਾ

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(ਖਾਲੀ_ਸੂਚਕ) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

ਖਾਲੀ
StringTrieBuilder::createCompactBuilder(int32_t ਮਾਪ_Guess, UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    nodes=uhash_open_ਮਾਪ(hash_String_Trie_ਗੰਢ, equalStringTrieNodes, ਖਾਲੀ_ਸੂਚਕ,
                         ਮਾਪ_Guess, &error_ਕੋਡ);
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
        ਜੇ(nodes==ਖਾਲੀ_ਸੂਚਕ) {
          error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        } ਨਹੀਂਤਾਂ {
          uhash_ਸੈੱਟ_ਕੁੰਜੀ_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

ਖਾਲੀ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=ਖਾਲੀ_ਸੂਚਕ;
}

ਖਾਲੀ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_ਲੰਮਾਈ,
                       UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ਗੰਢ(0, elements_ਲੰਮਾਈ, 0);
    } ਨਹੀਂਤਾਂ /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_ਲੰਮਾਈ, error_ਕੋਡ);
        Node *ਜੜ੍ਹ=make_ਗੰਢ(0, elements_ਲੰਮਾਈ, 0, error_ਕੋਡ);
        ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
            ਜੜ੍ਹ->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(-1);
            ਜੜ੍ਹ->ਲਿਖ(*ਇਹ);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ਗੰਢ(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ) {
    UBool has_ਮੁੱਲ=ਝੂਠ;
    int32_t value=0;
    int32_t type;
    ਜੇ(unit_ਸੂਚਕਾਂਕ==get_Element_String_ਲੰਮਾਈ(ਸ਼ੁਰੂ)) {
        // An intermediate or final value.
        value=get_Element_ਮੁੱਲ(ਸ਼ੁਰੂ++);
        ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ write_ਮੁੱਲ_And_Final(value, ਸੱਚ);  // final-value node
        }
        has_ਮੁੱਲ=ਸੱਚ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ);
    int32_t maxUnit=getElementUnit(ਹੱਦਬੰਦੀ-1, unit_ਸੂਚਕਾਂਕ);
    ਜੇ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ=get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-1, unit_ਸੂਚਕਾਂਕ);
        write_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ-unit_ਸੂਚਕਾਂਕ;
        int32_t max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ=get_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ();
        ਜਦੋਂ(length>max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ) {
            ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ-=max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ;
            length-=max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ;
            writeElementUnits(ਸ਼ੁਰੂ, ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ, max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ);
            ਲਿਖ(get_Min_ਰੇਖਿਕ_ਮੇਲ()+max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ-1);
        }
        writeElementUnits(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ, length);
        type=get_Min_ਰੇਖਿਕ_ਮੇਲ()+length-1;
    } ਨਹੀਂਤਾਂ {
        // Branch node.
        int32_t length=ਗਿਣਤੀ_Element_Units(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ, length);
        ਜੇ(--length<get_Min_ਰੇਖਿਕ_ਮੇਲ()) {
            type=length;
        } ਨਹੀਂਤਾਂ {
            ਲਿਖ(length);
            type=0;
        }
    }
    ਵਾਪਸ write_ਮੁੱਲ_And_Type(has_ਮੁੱਲ, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_ਗੰਢ(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t unit_ਸੂਚਕਾਂਕ, int32_t length) {
    char16_t ਵਿਚਕਾਰ_Units[k_Max_ਵੰਡੋ_Branch_ਪੱਧਰ];
    int32_t lessThan[k_Max_ਵੰਡੋ_Branch_ਪੱਧਰ];
    int32_t lt_ਲੰਮਾਈ=0;
    ਜਦੋਂ(length>get_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ, length/2);
        // Encode the less-than branch first.
        ਵਿਚਕਾਰ_Units[lt_ਲੰਮਾਈ]=getElementUnit(i, unit_ਸੂਚਕਾਂਕ);  // middle unit
        lessThan[lt_ਲੰਮਾਈ]=write_Branch_Sub_ਗੰਢ(ਸ਼ੁਰੂ, i, unit_ਸੂਚਕਾਂਕ, length/2);
        ++lt_ਲੰਮਾਈ;
        // Continue for the greater-or-equal branch.
        ਸ਼ੁਰੂ=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ];
    UBool isFinal[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ-1];
    int32_t unit_ਸੰਖਿਆ=0;
    ਕਰੋ {
        int32_t i=starts[unit_ਸੰਖਿਆ]=ਸ਼ੁਰੂ;
        char16_t unit=getElementUnit(i++, unit_ਸੂਚਕਾਂਕ);
        i=ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(i, unit_ਸੂਚਕਾਂਕ, unit);
        isFinal[unit_ਸੰਖਿਆ]= ਸ਼ੁਰੂ==i-1 && unit_ਸੂਚਕਾਂਕ+1==get_Element_String_ਲੰਮਾਈ(ਸ਼ੁਰੂ);
        ਸ਼ੁਰੂ=i;
    } ਜਦੋਂ(++unit_ਸੰਖਿਆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_ਸੰਖਿਆ]=ਸ਼ੁਰੂ;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ-1];
    ਕਰੋ {
        --unit_ਸੰਖਿਆ;
        ਜੇ(!isFinal[unit_ਸੰਖਿਆ]) {
            jumpTargets[unit_ਸੰਖਿਆ]=write_ਗੰਢ(starts[unit_ਸੰਖਿਆ], starts[unit_ਸੰਖਿਆ+1], unit_ਸੂਚਕਾਂਕ+1);
        }
    } ਜਦੋਂ(unit_ਸੰਖਿਆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ਸੰਖਿਆ=length-1;
    write_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ+1);
    int32_t ਵਿੱਥ=ਲਿਖ(getElementUnit(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ));
    // Write the rest of this node's unit-value pairs.
    ਜਦੋਂ(--unit_ਸੰਖਿਆ>=0) {
        ਸ਼ੁਰੂ=starts[unit_ਸੰਖਿਆ];
        int32_t value;
        ਜੇ(isFinal[unit_ਸੰਖਿਆ]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_ਮੁੱਲ(ਸ਼ੁਰੂ);
        } ਨਹੀਂਤਾਂ {
            // Write the delta to the start position of the sub-node.
            value=ਵਿੱਥ-jumpTargets[unit_ਸੰਖਿਆ];
        }
        write_ਮੁੱਲ_And_Final(value, isFinal[unit_ਸੰਖਿਆ]);
        ਵਿੱਥ=ਲਿਖ(getElementUnit(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ));
    }
    // Write the split-branch nodes.
    ਜਦੋਂ(lt_ਲੰਮਾਈ>0) {
        --lt_ਲੰਮਾਈ;
        write_Delta_ਮੰਜ਼ਿਲ(lessThan[lt_ਲੰਮਾਈ]);
        ਵਿੱਥ=ਲਿਖ(ਵਿਚਕਾਰ_Units[lt_ਲੰਮਾਈ]);
    }
    ਵਾਪਸ ਵਿੱਥ;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ਗੰਢ(int32_t ਸ਼ੁਰੂ,
                            int32_t ਹੱਦਬੰਦੀ,
                            int32_t unit_ਸੂਚਕਾਂਕ,
                            UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    UBool has_ਮੁੱਲ=ਝੂਠ;
    int32_t value=0;
    ਜੇ(unit_ਸੂਚਕਾਂਕ==get_Element_String_ਲੰਮਾਈ(ਸ਼ੁਰੂ)) {
        // An intermediate or final value.
        value=get_Element_ਮੁੱਲ(ਸ਼ੁਰੂ++);
        ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ) {
            ਵਾਪਸ ਰਜਿਸਟਰ_Final_ਮੁੱਲ(value, error_ਕੋਡ);
        }
        has_ਮੁੱਲ=ਸੱਚ;
    }
    Node *ਗੰਢ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ);
    int32_t maxUnit=getElementUnit(ਹੱਦਬੰਦੀ-1, unit_ਸੂਚਕਾਂਕ);
    ਜੇ(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ=get_ਹੱਦਬੰਦੀ_ਦਾ_ਰੇਖਿਕ_ਮੇਲ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-1, unit_ਸੂਚਕਾਂਕ);
        Node *ਅਗਲਾ_ਗੰਢ=make_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ, error_ਕੋਡ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ-unit_ਸੂਚਕਾਂਕ;
        int32_t max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ=get_Max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ();
        ਜਦੋਂ(length>max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ) {
            ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ-=max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ;
            length-=max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ;
            ਗੰਢ=create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(ਸ਼ੁਰੂ, ਆਖਰੀ_Unit_ਸੂਚਕਾਂਕ, max_ਰੇਖਿਕ_ਮੇਲ_ਲੰਮਾਈ, ਅਗਲਾ_ਗੰਢ);
            ਅਗਲਾ_ਗੰਢ=ਰਜਿਸਟਰ_ਗੰਢ(ਗੰਢ, error_ਕੋਡ);
        }
        ਗੰਢ=create_ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ, length, ਅਗਲਾ_ਗੰਢ);
    } ਨਹੀਂਤਾਂ {
        // Branch node.
        int32_t length=ਗਿਣਤੀ_Element_Units(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ਗੰਢ=make_Branch_Sub_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ, length, error_ਕੋਡ);
        ਗੰਢ=ਨਵਾਂ Branch_ਸਿਰ_ਗੰਢ(length, sub_ਗੰਢ);
    }
    ਜੇ(has_ਮੁੱਲ && ਗੰਢ!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(ਮੇਲ_Nodes_Can_Have_ਮੁੱਲਾਂ()) {
            ((ਮੁੱਲ_ਗੰਢ *)ਗੰਢ)->ਸੈੱਟ_ਮੁੱਲ(value);
        } ਨਹੀਂਤਾਂ {
            ਗੰਢ=ਨਵਾਂ Intermediate_ਮੁੱਲ_ਗੰਢ(value, ਰਜਿਸਟਰ_ਗੰਢ(ਗੰਢ, error_ਕੋਡ));
        }
    }
    ਵਾਪਸ ਰਜਿਸਟਰ_ਗੰਢ(ਗੰਢ, error_ਕੋਡ);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ਗੰਢ(int32_t ਸ਼ੁਰੂ,
                                     int32_t ਹੱਦਬੰਦੀ,
                                     int32_t unit_ਸੂਚਕਾਂਕ,
                                     int32_t length,
                                     UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    char16_t ਵਿਚਕਾਰ_Units[k_Max_ਵੰਡੋ_Branch_ਪੱਧਰ];
    Node *lessThan[k_Max_ਵੰਡੋ_Branch_ਪੱਧਰ];
    int32_t lt_ਲੰਮਾਈ=0;
    ਜਦੋਂ(length>get_Max_Branch_ਰੇਖਿਕ_Sub_ਗੰਢ_ਲੰਮਾਈ()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ, length/2);
        // Create the less-than branch.
        ਵਿਚਕਾਰ_Units[lt_ਲੰਮਾਈ]=getElementUnit(i, unit_ਸੂਚਕਾਂਕ);  // middle unit
        lessThan[lt_ਲੰਮਾਈ]=make_Branch_Sub_ਗੰਢ(ਸ਼ੁਰੂ, i, unit_ਸੂਚਕਾਂਕ, length/2, error_ਕੋਡ);
        ++lt_ਲੰਮਾਈ;
        // Continue for the greater-or-equal branch.
        ਸ਼ੁਰੂ=i;
        length=length-length/2;
    }
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    List_Branch_ਗੰਢ *list_ਗੰਢ=ਨਵਾਂ List_Branch_ਗੰਢ();
    ਜੇ(list_ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_ਸੰਖਿਆ=0;
    ਕਰੋ {
        int32_t i=ਸ਼ੁਰੂ;
        char16_t unit=getElementUnit(i++, unit_ਸੂਚਕਾਂਕ);
        i=ਸੂਚਕਾਂਕ_ਦਾ_Element_With_ਅਗਲਾ_Unit(i, unit_ਸੂਚਕਾਂਕ, unit);
        ਜੇ(ਸ਼ੁਰੂ==i-1 && unit_ਸੂਚਕਾਂਕ+1==get_Element_String_ਲੰਮਾਈ(ਸ਼ੁਰੂ)) {
            list_ਗੰਢ->ਜੋੜੋ(unit, get_Element_ਮੁੱਲ(ਸ਼ੁਰੂ));
        } ਨਹੀਂਤਾਂ {
            list_ਗੰਢ->ਜੋੜੋ(unit, make_ਗੰਢ(ਸ਼ੁਰੂ, i, unit_ਸੂਚਕਾਂਕ+1, error_ਕੋਡ));
        }
        ਸ਼ੁਰੂ=i;
    } ਜਦੋਂ(++unit_ਸੰਖਿਆ<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(ਸ਼ੁਰੂ, unit_ਸੂਚਕਾਂਕ);
    ਜੇ(ਸ਼ੁਰੂ==ਹੱਦਬੰਦੀ-1 && unit_ਸੂਚਕਾਂਕ+1==get_Element_String_ਲੰਮਾਈ(ਸ਼ੁਰੂ)) {
        list_ਗੰਢ->ਜੋੜੋ(unit, get_Element_ਮੁੱਲ(ਸ਼ੁਰੂ));
    } ਨਹੀਂਤਾਂ {
        list_ਗੰਢ->ਜੋੜੋ(unit, make_ਗੰਢ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, unit_ਸੂਚਕਾਂਕ+1, error_ਕੋਡ));
    }
    Node *ਗੰਢ=ਰਜਿਸਟਰ_ਗੰਢ(list_ਗੰਢ, error_ਕੋਡ);
    // Create the split-branch nodes.
    ਜਦੋਂ(lt_ਲੰਮਾਈ>0) {
        --lt_ਲੰਮਾਈ;
        ਗੰਢ=ਰਜਿਸਟਰ_ਗੰਢ(
            ਨਵਾਂ ਵੰਡੋ_Branch_ਗੰਢ(ਵਿਚਕਾਰ_Units[lt_ਲੰਮਾਈ], lessThan[lt_ਲੰਮਾਈ], ਗੰਢ), error_ਕੋਡ);
    }
    ਵਾਪਸ ਗੰਢ;
}

StringTrieBuilder::Node*
StringTrieBuilder::ਰਜਿਸਟਰ_ਗੰਢ(Node* new_ਗੰਢ, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਮਿਟਾਓ new_ਗੰਢ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(new_ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਅਚਲ UHashElement *old=uhash_ਲੱਭੋ(nodes, new_ਗੰਢ);
    ਜੇ(old!=ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ new_ਗੰਢ;
        ਵਾਪਸ static_cast<Node*>(old->ਕੁੰਜੀ.ਸੂਚਕ);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ਜੇ U_DEBUG
    int32_t old_ਮੁੱਲ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#ਜੇ_ਅੰਤ
    uhash_puti(nodes, new_ਗੰਢ, 1, &error_ਕੋਡ);
    U_ASSERT(old_ਮੁੱਲ==0);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਮਿਟਾਓ new_ਗੰਢ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ new_ਗੰਢ;
}

StringTrieBuilder::Node*
StringTrieBuilder::ਰਜਿਸਟਰ_Final_ਮੁੱਲ(int32_t value, UError_ਕੋਡ& error_ਕੋਡ) U_LIFETIME_BOUND {
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    Final_ਮੁੱਲ_ਗੰਢ ਕੁੰਜੀ(value);
    ਅਚਲ UHashElement *old=uhash_ਲੱਭੋ(nodes, &ਕੁੰਜੀ);
    ਜੇ(old!=ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ static_cast<Node*>(old->ਕੁੰਜੀ.ਸੂਚਕ);
    }
    Node *new_ਗੰਢ=ਨਵਾਂ Final_ਮੁੱਲ_ਗੰਢ(value);
    ਜੇ(new_ਗੰਢ==ਖਾਲੀ_ਸੂਚਕ) {
        error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#ਜੇ U_DEBUG
    int32_t old_ਮੁੱਲ=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#ਜੇ_ਅੰਤ
    uhash_puti(nodes, new_ਗੰਢ, 1, &error_ਕੋਡ);
    U_ASSERT(old_ਮੁੱਲ==0);
    ਜੇ(U_FAILURE(error_ਕੋਡ)) {
        ਮਿਟਾਓ new_ਗੰਢ;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }
    ਵਾਪਸ new_ਗੰਢ;
}

int32_t
StringTrieBuilder::hash_ਗੰਢ(ਅਚਲ ਖਾਲੀ *ਗੰਢ) {
    ਵਾਪਸ static_cast<ਅਚਲ Node*>(ਗੰਢ)->hash_ਕੋਡ();
}

UBool
StringTrieBuilder::equalNodes(ਅਚਲ ਖਾਲੀ *ਖੱਬੇ, ਅਚਲ ਖਾਲੀ *ਸੱਜੇ) {
    ਵਾਪਸ *static_cast<ਅਚਲ Node*>(ਖੱਬੇ) == *static_cast<ਅਚਲ Node*>(ਸੱਜੇ);
}

ਤਾਰਕਿਕ
StringTrieBuilder::Node::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਵਾਪਸ ਇਹ==&other || (typeid(*ਇਹ)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ;
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਤਾਰਕਿਕ
StringTrieBuilder::Final_ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!Node::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ Final_ਮੁੱਲ_ਗੰਢ &o=static_cast<ਅਚਲ Final_ਮੁੱਲ_ਗੰਢ &>(other);
    ਵਾਪਸ value==o.value;
}

ਖਾਲੀ
StringTrieBuilder::Final_ਮੁੱਲ_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    ਵਿੱਥ=builder.write_ਮੁੱਲ_And_Final(value, ਸੱਚ);
}

ਤਾਰਕਿਕ
StringTrieBuilder::ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!Node::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਮੁੱਲ_ਗੰਢ &o=static_cast<ਅਚਲ ਮੁੱਲ_ਗੰਢ &>(other);
    ਵਾਪਸ has_ਮੁੱਲ==o.has_ਮੁੱਲ && (!has_ਮੁੱਲ || value==o.value);
}

ਤਾਰਕਿਕ
StringTrieBuilder::Intermediate_ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ Intermediate_ਮੁੱਲ_ਗੰਢ &o=static_cast<ਅਚਲ Intermediate_ਮੁੱਲ_ਗੰਢ &>(other);
    ਵਾਪਸ next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_ਮੁੱਲ_ਗੰਢ::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ=next->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ);
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਖਾਲੀ
StringTrieBuilder::Intermediate_ਮੁੱਲ_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    next->ਲਿਖ(builder);
    ਵਿੱਥ=builder.write_ਮੁੱਲ_And_Final(value, ਝੂਠ);
}

ਤਾਰਕਿਕ
StringTrieBuilder::ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ &o=static_cast<ਅਚਲ ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ &>(other);
    ਵਾਪਸ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::ਰੇਖਿਕ_ਮੇਲ_ਗੰਢ::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ=next->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ);
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਤਾਰਕਿਕ
StringTrieBuilder::List_Branch_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!Node::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ List_Branch_ਗੰਢ &o=static_cast<ਅਚਲ List_Branch_ਗੰਢ &>(other);
    ਲਈ(int32_t i=0; i<length; ++i) {
        ਜੇ(units[i]!=o.units[i] || ਮੁੱਲਾਂ[i]!=o.ਮੁੱਲਾਂ[i] || equal[i]!=o.equal[i]) {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

int32_t
StringTrieBuilder::List_Branch_ਗੰਢ::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ=ਕਿਨਾਰਾ_ਸੰਖਿਆ;
        int32_t ਕਦਮ=0;
        int32_t i=length;
        ਕਰੋ {
            Node *ਕਿਨਾਰਾ=equal[--i];
            ਜੇ(ਕਿਨਾਰਾ!=ਖਾਲੀ_ਸੂਚਕ) {
                ਕਿਨਾਰਾ_ਸੰਖਿਆ=ਕਿਨਾਰਾ->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ-ਕਦਮ);
            }
            // For all but the rightmost edge, decrement the edge number.
            ਕਦਮ=1;
        } ਜਦੋਂ(i>0);
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ;
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਖਾਲੀ
StringTrieBuilder::List_Branch_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_ਸੰਖਿਆ=length-1;
    Node *ਸੱਜਾ_ਕਿਨਾਰਾ=equal[unit_ਸੰਖਿਆ];
    int32_t ਸੱਜਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ= ਸੱਜਾ_ਕਿਨਾਰਾ==ਖਾਲੀ_ਸੂਚਕ ? ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ : ਸੱਜਾ_ਕਿਨਾਰਾ->get_ਵਿੱਥ();
    ਕਰੋ {
        --unit_ਸੰਖਿਆ;
        ਜੇ(equal[unit_ਸੰਖਿਆ]!=ਖਾਲੀ_ਸੂਚਕ) {
            equal[unit_ਸੰਖਿਆ]->write_Unless_Inside_ਸੱਜਾ_ਕਿਨਾਰਾ(ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ, ਸੱਜਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ, builder);
        }
    } ਜਦੋਂ(unit_ਸੰਖਿਆ>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_ਸੰਖਿਆ=length-1;
    ਜੇ(ਸੱਜਾ_ਕਿਨਾਰਾ==ਖਾਲੀ_ਸੂਚਕ) {
        builder.write_ਮੁੱਲ_And_Final(ਮੁੱਲਾਂ[unit_ਸੰਖਿਆ], ਸੱਚ);
    } ਨਹੀਂਤਾਂ {
        ਸੱਜਾ_ਕਿਨਾਰਾ->ਲਿਖ(builder);
    }
    ਵਿੱਥ=builder.ਲਿਖ(units[unit_ਸੰਖਿਆ]);
    // Write the rest of this node's unit-value pairs.
    ਜਦੋਂ(--unit_ਸੰਖਿਆ>=0) {
        int32_t value;
        UBool isFinal;
        ਜੇ(equal[unit_ਸੰਖਿਆ]==ਖਾਲੀ_ਸੂਚਕ) {
            // Write the final value for the one string ending with this unit.
            value=ਮੁੱਲਾਂ[unit_ਸੰਖਿਆ];
            isFinal=ਸੱਚ;
        } ਨਹੀਂਤਾਂ {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_ਸੰਖਿਆ]->get_ਵਿੱਥ()>0);
            value=ਵਿੱਥ-equal[unit_ਸੰਖਿਆ]->get_ਵਿੱਥ();
            isFinal=ਝੂਠ;
        }
        builder.write_ਮੁੱਲ_And_Final(value, isFinal);
        ਵਿੱਥ=builder.ਲਿਖ(units[unit_ਸੰਖਿਆ]);
    }
}

ਤਾਰਕਿਕ
StringTrieBuilder::ਵੰਡੋ_Branch_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!Node::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ ਵੰਡੋ_Branch_ਗੰਢ &o=static_cast<ਅਚਲ ਵੰਡੋ_Branch_ਗੰਢ &>(other);
    ਵਾਪਸ unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::ਵੰਡੋ_Branch_ਗੰਢ::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ=ਕਿਨਾਰਾ_ਸੰਖਿਆ;
        ਕਿਨਾਰਾ_ਸੰਖਿਆ=greaterOrEqual->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ);
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ=lessThan->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ-1);
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਖਾਲੀ
StringTrieBuilder::ਵੰਡੋ_Branch_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ਸੱਜਾ_ਕਿਨਾਰਾ(ਪਹਿਲਾ_ਕਿਨਾਰਾ_ਸੰਖਿਆ, greaterOrEqual->get_ਵਿੱਥ(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->ਲਿਖ(builder);
    // Write this node.
    U_ASSERT(lessThan->get_ਵਿੱਥ()>0);
    builder.write_Delta_ਮੰਜ਼ਿਲ(lessThan->get_ਵਿੱਥ());  // less-than
    ਵਿੱਥ=builder.ਲਿਖ(unit);
}

ਤਾਰਕਿਕ
StringTrieBuilder::Branch_ਸਿਰ_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Node &other) ਅਚਲ {
    ਜੇ(ਇਹ==&other) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ(!ਮੁੱਲ_ਗੰਢ::ਸੰਚਾਲਕ==(other)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਅਚਲ Branch_ਸਿਰ_ਗੰਢ &o=static_cast<ਅਚਲ Branch_ਸਿਰ_ਗੰਢ &>(other);
    ਵਾਪਸ length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_ਸਿਰ_ਗੰਢ::mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(int32_t ਕਿਨਾਰਾ_ਸੰਖਿਆ) {
    ਜੇ(ਵਿੱਥ==0) {
        ਵਿੱਥ=ਕਿਨਾਰਾ_ਸੰਖਿਆ=next->mark_ਸੱਜਾ_ਕਿਨਾਰੇ_ਪਹਿਲਾ(ਕਿਨਾਰਾ_ਸੰਖਿਆ);
    }
    ਵਾਪਸ ਕਿਨਾਰਾ_ਸੰਖਿਆ;
}

ਖਾਲੀ
StringTrieBuilder::Branch_ਸਿਰ_ਗੰਢ::ਲਿਖ(StringTrieBuilder &builder) {
    next->ਲਿਖ(builder);
    ਜੇ(length<=builder.get_Min_ਰੇਖਿਕ_ਮੇਲ()) {
        ਵਿੱਥ=builder.write_ਮੁੱਲ_And_Type(has_ਮੁੱਲ, value, length-1);
    } ਨਹੀਂਤਾਂ {
        builder.ਲਿਖ(length-1);
        ਵਿੱਥ=builder.write_ਮੁੱਲ_And_Type(has_ਮੁੱਲ, value, 0);
    }
}

U_NAMESPACE_ਸਿਰਾ
