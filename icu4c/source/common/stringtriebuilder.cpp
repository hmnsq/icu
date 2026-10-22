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
hash_String_Trie_ڳنڍ(ثابت UHashTok ڪنجي) {
    أرجع icu::StringTrieBuilder::hash_ڳنڍ(ڪنجي.اشارو);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.اشارو, key2.اشارو);
}

U_CDECL_پڇاڙي

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(خالي_اشارو) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t سائيز_Guess, UError_ڪوڊ &error_ڪوڊ) {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع;
    }
    nodes=uhash_open_سائيز(hash_String_Trie_ڳنڍ, equalStringTrieNodes, خالي_اشارو,
                         سائيز_Guess, &error_ڪوڊ);
    إذا(U_SUCCESS(error_ڪوڊ)) {
        إذا(nodes==خالي_اشارو) {
          error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        } وإلا {
          uhash_سيٽ_ڪنجي_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=خالي_اشارو;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_ڊيگهائي,
                       UError_ڪوڊ &error_ڪوڊ) {
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_ڳنڍ(0, elements_ڊيگهائي, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_ڊيگهائي, error_ڪوڊ);
        Node *پاڙ=make_ڳنڍ(0, elements_ڊيگهائي, 0, error_ڪوڊ);
        إذا(U_SUCCESS(error_ڪوڊ)) {
            پاڙ->mark_ساڄو_ڪنارا_پهريون(-1);
            پاڙ->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_ڳنڍ(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو) {
    UBool has_قدر=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unit_اشاريو==get_Element_String_ڊيگهائي(شروعات)) {
        // An intermediate or final value.
        value=get_Element_قدر(شروعات++);
        إذا(شروعات==حد_بندي) {
            أرجع write_قدر_And_Final(value, سچ);  // final-value node
        }
        has_قدر=سچ;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروعات, unit_اشاريو);
    int32_t maxUnit=getElementUnit(حد_بندي-1, unit_اشاريو);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخري_Unit_اشاريو=get_حد_بندي_جو_لڪيري_ميلاپ(شروعات, حد_بندي-1, unit_اشاريو);
        write_ڳنڍ(شروعات, حد_بندي, آخري_Unit_اشاريو);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخري_Unit_اشاريو-unit_اشاريو;
        int32_t max_لڪيري_ميلاپ_ڊيگهائي=get_Max_لڪيري_ميلاپ_ڊيگهائي();
        طالما(length>max_لڪيري_ميلاپ_ڊيگهائي) {
            آخري_Unit_اشاريو-=max_لڪيري_ميلاپ_ڊيگهائي;
            length-=max_لڪيري_ميلاپ_ڊيگهائي;
            writeElementUnits(شروعات, آخري_Unit_اشاريو, max_لڪيري_ميلاپ_ڊيگهائي);
            اكتب(get_Min_لڪيري_ميلاپ()+max_لڪيري_ميلاپ_ڊيگهائي-1);
        }
        writeElementUnits(شروعات, unit_اشاريو, length);
        type=get_Min_لڪيري_ميلاپ()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=ڳڻپ_Element_Units(شروعات, حد_بندي, unit_اشاريو);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_ڳنڍ(شروعات, حد_بندي, unit_اشاريو, length);
        إذا(--length<get_Min_لڪيري_ميلاپ()) {
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
StringTrieBuilder::write_Branch_Sub_ڳنڍ(int32_t شروعات, int32_t حد_بندي, int32_t unit_اشاريو, int32_t length) {
    char16_t وچ_Units[k_Max_ورهايو_Branch_سطحون];
    int32_t lessThan[k_Max_ورهايو_Branch_سطحون];
    int32_t lt_ڊيگهائي=0;
    طالما(length>get_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروعات, unit_اشاريو, length/2);
        // Encode the less-than branch first.
        وچ_Units[lt_ڊيگهائي]=getElementUnit(i, unit_اشاريو);  // middle unit
        lessThan[lt_ڊيگهائي]=write_Branch_Sub_ڳنڍ(شروعات, i, unit_اشاريو, length/2);
        ++lt_ڊيگهائي;
        // Continue for the greater-or-equal branch.
        شروعات=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي];
    UBool isFinal[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي-1];
    int32_t unit_عدد=0;
    افعل {
        int32_t i=starts[unit_عدد]=شروعات;
        char16_t unit=getElementUnit(i++, unit_اشاريو);
        i=اشاريو_جو_Element_With_ايندڙ_Unit(i, unit_اشاريو, unit);
        isFinal[unit_عدد]= شروعات==i-1 && unit_اشاريو+1==get_Element_String_ڊيگهائي(شروعات);
        شروعات=i;
    } طالما(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_عدد]=شروعات;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي-1];
    افعل {
        --unit_عدد;
        إذا(!isFinal[unit_عدد]) {
            jumpTargets[unit_عدد]=write_ڳنڍ(starts[unit_عدد], starts[unit_عدد+1], unit_اشاريو+1);
        }
    } طالما(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    write_ڳنڍ(شروعات, حد_بندي, unit_اشاريو+1);
    int32_t وٿي=اكتب(getElementUnit(شروعات, unit_اشاريو));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_عدد>=0) {
        شروعات=starts[unit_عدد];
        int32_t value;
        إذا(isFinal[unit_عدد]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_قدر(شروعات);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=وٿي-jumpTargets[unit_عدد];
        }
        write_قدر_And_Final(value, isFinal[unit_عدد]);
        وٿي=اكتب(getElementUnit(شروعات, unit_اشاريو));
    }
    // Write the split-branch nodes.
    طالما(lt_ڊيگهائي>0) {
        --lt_ڊيگهائي;
        write_Delta_منزل(lessThan[lt_ڊيگهائي]);
        وٿي=اكتب(وچ_Units[lt_ڊيگهائي]);
    }
    أرجع وٿي;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_ڳنڍ(int32_t شروعات,
                            int32_t حد_بندي,
                            int32_t unit_اشاريو,
                            UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    UBool has_قدر=خطأ;
    int32_t value=0;
    إذا(unit_اشاريو==get_Element_String_ڊيگهائي(شروعات)) {
        // An intermediate or final value.
        value=get_Element_قدر(شروعات++);
        إذا(شروعات==حد_بندي) {
            أرجع رجسٽر_Final_قدر(value, error_ڪوڊ);
        }
        has_قدر=سچ;
    }
    Node *ڳنڍ;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروعات, unit_اشاريو);
    int32_t maxUnit=getElementUnit(حد_بندي-1, unit_اشاريو);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخري_Unit_اشاريو=get_حد_بندي_جو_لڪيري_ميلاپ(شروعات, حد_بندي-1, unit_اشاريو);
        Node *ايندڙ_ڳنڍ=make_ڳنڍ(شروعات, حد_بندي, آخري_Unit_اشاريو, error_ڪوڊ);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخري_Unit_اشاريو-unit_اشاريو;
        int32_t max_لڪيري_ميلاپ_ڊيگهائي=get_Max_لڪيري_ميلاپ_ڊيگهائي();
        طالما(length>max_لڪيري_ميلاپ_ڊيگهائي) {
            آخري_Unit_اشاريو-=max_لڪيري_ميلاپ_ڊيگهائي;
            length-=max_لڪيري_ميلاپ_ڊيگهائي;
            ڳنڍ=create_لڪيري_ميلاپ_ڳنڍ(شروعات, آخري_Unit_اشاريو, max_لڪيري_ميلاپ_ڊيگهائي, ايندڙ_ڳنڍ);
            ايندڙ_ڳنڍ=رجسٽر_ڳنڍ(ڳنڍ, error_ڪوڊ);
        }
        ڳنڍ=create_لڪيري_ميلاپ_ڳنڍ(شروعات, unit_اشاريو, length, ايندڙ_ڳنڍ);
    } وإلا {
        // Branch node.
        int32_t length=ڳڻپ_Element_Units(شروعات, حد_بندي, unit_اشاريو);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_ڳنڍ=make_Branch_Sub_ڳنڍ(شروعات, حد_بندي, unit_اشاريو, length, error_ڪوڊ);
        ڳنڍ=جديد Branch_سر_ڳنڍ(length, sub_ڳنڍ);
    }
    إذا(has_قدر && ڳنڍ!=خالي_اشارو) {
        إذا(ميلاپ_Nodes_Can_Have_قدرون()) {
            ((قدر_ڳنڍ *)ڳنڍ)->سيٽ_قدر(value);
        } وإلا {
            ڳنڍ=جديد Intermediate_قدر_ڳنڍ(value, رجسٽر_ڳنڍ(ڳنڍ, error_ڪوڊ));
        }
    }
    أرجع رجسٽر_ڳنڍ(ڳنڍ, error_ڪوڊ);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_ڳنڍ(int32_t شروعات,
                                     int32_t حد_بندي,
                                     int32_t unit_اشاريو,
                                     int32_t length,
                                     UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    char16_t وچ_Units[k_Max_ورهايو_Branch_سطحون];
    Node *lessThan[k_Max_ورهايو_Branch_سطحون];
    int32_t lt_ڊيگهائي=0;
    طالما(length>get_Max_Branch_لڪيري_Sub_ڳنڍ_ڊيگهائي()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروعات, unit_اشاريو, length/2);
        // Create the less-than branch.
        وچ_Units[lt_ڊيگهائي]=getElementUnit(i, unit_اشاريو);  // middle unit
        lessThan[lt_ڊيگهائي]=make_Branch_Sub_ڳنڍ(شروعات, i, unit_اشاريو, length/2, error_ڪوڊ);
        ++lt_ڊيگهائي;
        // Continue for the greater-or-equal branch.
        شروعات=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    List_Branch_ڳنڍ *list_ڳنڍ=جديد List_Branch_ڳنڍ();
    إذا(list_ڳنڍ==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_عدد=0;
    افعل {
        int32_t i=شروعات;
        char16_t unit=getElementUnit(i++, unit_اشاريو);
        i=اشاريو_جو_Element_With_ايندڙ_Unit(i, unit_اشاريو, unit);
        إذا(شروعات==i-1 && unit_اشاريو+1==get_Element_String_ڊيگهائي(شروعات)) {
            list_ڳنڍ->جوڙيو(unit, get_Element_قدر(شروعات));
        } وإلا {
            list_ڳنڍ->جوڙيو(unit, make_ڳنڍ(شروعات, i, unit_اشاريو+1, error_ڪوڊ));
        }
        شروعات=i;
    } طالما(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(شروعات, unit_اشاريو);
    إذا(شروعات==حد_بندي-1 && unit_اشاريو+1==get_Element_String_ڊيگهائي(شروعات)) {
        list_ڳنڍ->جوڙيو(unit, get_Element_قدر(شروعات));
    } وإلا {
        list_ڳنڍ->جوڙيو(unit, make_ڳنڍ(شروعات, حد_بندي, unit_اشاريو+1, error_ڪوڊ));
    }
    Node *ڳنڍ=رجسٽر_ڳنڍ(list_ڳنڍ, error_ڪوڊ);
    // Create the split-branch nodes.
    طالما(lt_ڊيگهائي>0) {
        --lt_ڊيگهائي;
        ڳنڍ=رجسٽر_ڳنڍ(
            جديد ورهايو_Branch_ڳنڍ(وچ_Units[lt_ڊيگهائي], lessThan[lt_ڊيگهائي], ڳنڍ), error_ڪوڊ);
    }
    أرجع ڳنڍ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٽر_ڳنڍ(Node* new_ڳنڍ, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        ميٽيو new_ڳنڍ;
        أرجع خالي_اشارو;
    }
    إذا(new_ڳنڍ==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    ثابت UHashElement *old=uhash_ڳوليو(nodes, new_ڳنڍ);
    إذا(old!=خالي_اشارو) {
        ميٽيو new_ڳنڍ;
        أرجع static_cast<Node*>(old->ڪنجي.اشارو);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#جيڪڏهن_پورو
    uhash_puti(nodes, new_ڳنڍ, 1, &error_ڪوڊ);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(error_ڪوڊ)) {
        ميٽيو new_ڳنڍ;
        أرجع خالي_اشارو;
    }
    أرجع new_ڳنڍ;
}

StringTrieBuilder::Node*
StringTrieBuilder::رجسٽر_Final_قدر(int32_t value, UError_ڪوڊ& error_ڪوڊ) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }
    Final_قدر_ڳنڍ ڪنجي(value);
    ثابت UHashElement *old=uhash_ڳوليو(nodes, &ڪنجي);
    إذا(old!=خالي_اشارو) {
        أرجع static_cast<Node*>(old->ڪنجي.اشارو);
    }
    Node *new_ڳنڍ=جديد Final_قدر_ڳنڍ(value);
    إذا(new_ڳنڍ==خالي_اشارو) {
        error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_قدر=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#جيڪڏهن_پورو
    uhash_puti(nodes, new_ڳنڍ, 1, &error_ڪوڊ);
    U_ASSERT(old_قدر==0);
    إذا(U_FAILURE(error_ڪوڊ)) {
        ميٽيو new_ڳنڍ;
        أرجع خالي_اشارو;
    }
    أرجع new_ڳنڍ;
}

int32_t
StringTrieBuilder::hash_ڳنڍ(ثابت فراغ *ڳنڍ) {
    أرجع static_cast<ثابت Node*>(ڳنڍ)->hash_ڪوڊ();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *کاٻي, ثابت فراغ *ساڄي) {
    أرجع *static_cast<ثابت Node*>(کاٻي) == *static_cast<ثابت Node*>(ساڄي);
}

منطقي
StringTrieBuilder::Node::عامل==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        وٿي=ڪنارو_عدد;
    }
    أرجع ڪنارو_عدد;
}

منطقي
StringTrieBuilder::Final_قدر_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Final_قدر_ڳنڍ &o=static_cast<ثابت Final_قدر_ڳنڍ &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_قدر_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    وٿي=builder.write_قدر_And_Final(value, سچ);
}

منطقي
StringTrieBuilder::قدر_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت قدر_ڳنڍ &o=static_cast<ثابت قدر_ڳنڍ &>(other);
    أرجع has_قدر==o.has_قدر && (!has_قدر || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_قدر_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_ڳنڍ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_قدر_ڳنڍ &o=static_cast<ثابت Intermediate_قدر_ڳنڍ &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_قدر_ڳنڍ::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        وٿي=ڪنارو_عدد=next->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد);
    }
    أرجع ڪنارو_عدد;
}

فراغ
StringTrieBuilder::Intermediate_قدر_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    وٿي=builder.write_قدر_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::لڪيري_ميلاپ_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_ڳنڍ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت لڪيري_ميلاپ_ڳنڍ &o=static_cast<ثابت لڪيري_ميلاپ_ڳنڍ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::لڪيري_ميلاپ_ڳنڍ::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        وٿي=ڪنارو_عدد=next->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد);
    }
    أرجع ڪنارو_عدد;
}

منطقي
StringTrieBuilder::List_Branch_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_ڳنڍ &o=static_cast<ثابت List_Branch_ڳنڍ &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || قدرون[i]!=o.قدرون[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع سچ;
}

int32_t
StringTrieBuilder::List_Branch_ڳنڍ::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        پهريون_ڪنارو_عدد=ڪنارو_عدد;
        int32_t قدم=0;
        int32_t i=length;
        افعل {
            Node *ڪنارو=equal[--i];
            إذا(ڪنارو!=خالي_اشارو) {
                ڪنارو_عدد=ڪنارو->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد-قدم);
            }
            // For all but the rightmost edge, decrement the edge number.
            قدم=1;
        } طالما(i>0);
        وٿي=ڪنارو_عدد;
    }
    أرجع ڪنارو_عدد;
}

فراغ
StringTrieBuilder::List_Branch_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_عدد=length-1;
    Node *ساڄو_ڪنارو=equal[unit_عدد];
    int32_t ساڄو_ڪنارو_عدد= ساڄو_ڪنارو==خالي_اشارو ? پهريون_ڪنارو_عدد : ساڄو_ڪنارو->get_وٿي();
    افعل {
        --unit_عدد;
        إذا(equal[unit_عدد]!=خالي_اشارو) {
            equal[unit_عدد]->write_Unless_Inside_ساڄو_ڪنارو(پهريون_ڪنارو_عدد, ساڄو_ڪنارو_عدد, builder);
        }
    } طالما(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    إذا(ساڄو_ڪنارو==خالي_اشارو) {
        builder.write_قدر_And_Final(قدرون[unit_عدد], سچ);
    } وإلا {
        ساڄو_ڪنارو->اكتب(builder);
    }
    وٿي=builder.اكتب(units[unit_عدد]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_عدد>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_عدد]==خالي_اشارو) {
            // Write the final value for the one string ending with this unit.
            value=قدرون[unit_عدد];
            isFinal=سچ;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_عدد]->get_وٿي()>0);
            value=وٿي-equal[unit_عدد]->get_وٿي();
            isFinal=خطأ;
        }
        builder.write_قدر_And_Final(value, isFinal);
        وٿي=builder.اكتب(units[unit_عدد]);
    }
}

منطقي
StringTrieBuilder::ورهايو_Branch_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!Node::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت ورهايو_Branch_ڳنڍ &o=static_cast<ثابت ورهايو_Branch_ڳنڍ &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::ورهايو_Branch_ڳنڍ::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        پهريون_ڪنارو_عدد=ڪنارو_عدد;
        ڪنارو_عدد=greaterOrEqual->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد);
        وٿي=ڪنارو_عدد=lessThan->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد-1);
    }
    أرجع ڪنارو_عدد;
}

فراغ
StringTrieBuilder::ورهايو_Branch_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_ساڄو_ڪنارو(پهريون_ڪنارو_عدد, greaterOrEqual->get_وٿي(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->get_وٿي()>0);
    builder.write_Delta_منزل(lessThan->get_وٿي());  // less-than
    وٿي=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_سر_ڳنڍ::عامل==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع سچ;
    }
    إذا(!قدر_ڳنڍ::عامل==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_سر_ڳنڍ &o=static_cast<ثابت Branch_سر_ڳنڍ &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_سر_ڳنڍ::mark_ساڄو_ڪنارا_پهريون(int32_t ڪنارو_عدد) {
    إذا(وٿي==0) {
        وٿي=ڪنارو_عدد=next->mark_ساڄو_ڪنارا_پهريون(ڪنارو_عدد);
    }
    أرجع ڪنارو_عدد;
}

فراغ
StringTrieBuilder::Branch_سر_ڳنڍ::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.get_Min_لڪيري_ميلاپ()) {
        وٿي=builder.write_قدر_And_Type(has_قدر, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        وٿي=builder.write_قدر_And_Type(has_قدر, value, 0);
    }
}

U_NAMESPACE_پڇاڙي
