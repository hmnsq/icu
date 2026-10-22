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
hash_String_Trie_گره(ثابت UHashTok کلید) {
    أرجع icu::StringTrieBuilder::hash_گره(کلید.اشاره_گر);
}

ساكن UBool U_CALLCONV
equalStringTrieNodes(ثابت UHashTok key1, ثابت UHashTok key2) {
    أرجع icu::StringTrieBuilder::equalNodes(key1.اشاره_گر, key2.اشاره_گر);
}

U_CDECL_انتها

U_NAMESPACE_BEGIN

StringTrieBuilder::StringTrieBuilder() : nodes(اشاره_تهی) {}

StringTrieBuilder::~StringTrieBuilder() {
    deleteCompactBuilder();
}

فراغ
StringTrieBuilder::createCompactBuilder(int32_t اندازه_Guess, UError_کد &error_کد) {
    إذا(U_FAILURE(error_کد)) {
        أرجع;
    }
    nodes=uhash_open_اندازه(hash_String_Trie_گره, equalStringTrieNodes, اشاره_تهی,
                         اندازه_Guess, &error_کد);
    إذا(U_SUCCESS(error_کد)) {
        إذا(nodes==اشاره_تهی) {
          error_کد=U_حافظه_تخصیص_ERROR;
        } وإلا {
          uhash_مجموعه_کلید_Deleter(nodes, uprv_deleteUObject);
        }
    }
}

فراغ
StringTrieBuilder::deleteCompactBuilder() {
    uhash_close(nodes);
    nodes=اشاره_تهی;
}

فراغ
StringTrieBuilder::build(UStringTrieBuildOption buildOption, int32_t elements_درازی,
                       UError_کد &error_کد) {
    إذا(buildOption==USTRINGTRIE_BUILD_FAST) {
        write_گره(0, elements_درازی, 0);
    } وإلا /* USTRINGTRIE_BUILD_SMALL */ {
        createCompactBuilder(2*elements_درازی, error_کد);
        Node *ریشه=make_گره(0, elements_درازی, 0, error_کد);
        إذا(U_SUCCESS(error_کد)) {
            ریشه->mark_راست_یالها_اولی(-1);
            ریشه->اكتب(*هذا);
        }
        deleteCompactBuilder();
    }
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
int32_t
StringTrieBuilder::write_گره(int32_t شروع, int32_t حد, int32_t unit_شاخص) {
    UBool has_مقدار=خطأ;
    int32_t value=0;
    int32_t type;
    إذا(unit_شاخص==get_Element_String_درازی(شروع)) {
        // An intermediate or final value.
        value=get_Element_مقدار(شروع++);
        إذا(شروع==حد) {
            أرجع write_مقدار_And_Final(value, درست);  // final-value node
        }
        has_مقدار=درست;
    }
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروع, unit_شاخص);
    int32_t maxUnit=getElementUnit(حد-1, unit_شاخص);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخرین_Unit_شاخص=get_حد_از_خطی_تطابق(شروع, حد-1, unit_شاخص);
        write_گره(شروع, حد, آخرین_Unit_شاخص);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخرین_Unit_شاخص-unit_شاخص;
        int32_t max_خطی_تطابق_درازی=get_Max_خطی_تطابق_درازی();
        طالما(length>max_خطی_تطابق_درازی) {
            آخرین_Unit_شاخص-=max_خطی_تطابق_درازی;
            length-=max_خطی_تطابق_درازی;
            writeElementUnits(شروع, آخرین_Unit_شاخص, max_خطی_تطابق_درازی);
            اكتب(get_Min_خطی_تطابق()+max_خطی_تطابق_درازی-1);
        }
        writeElementUnits(شروع, unit_شاخص, length);
        type=get_Min_خطی_تطابق()+length-1;
    } وإلا {
        // Branch node.
        int32_t length=شمار_Element_Units(شروع, حد, unit_شاخص);
        // length>=2 because minUnit!=maxUnit.
        write_Branch_Sub_گره(شروع, حد, unit_شاخص, length);
        إذا(--length<get_Min_خطی_تطابق()) {
            type=length;
        } وإلا {
            اكتب(length);
            type=0;
        }
    }
    أرجع write_مقدار_And_Type(has_مقدار, value, type);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
int32_t
StringTrieBuilder::write_Branch_Sub_گره(int32_t شروع, int32_t حد, int32_t unit_شاخص, int32_t length) {
    char16_t میانه_Units[k_Max_تقسیم_Branch_سطوح];
    int32_t lessThan[k_Max_تقسیم_Branch_سطوح];
    int32_t lt_درازی=0;
    طالما(length>get_Max_Branch_خطی_Sub_گره_درازی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروع, unit_شاخص, length/2);
        // Encode the less-than branch first.
        میانه_Units[lt_درازی]=getElementUnit(i, unit_شاخص);  // middle unit
        lessThan[lt_درازی]=write_Branch_Sub_گره(شروع, i, unit_شاخص, length/2);
        ++lt_درازی;
        // Continue for the greater-or-equal branch.
        شروع=i;
        length=length-length/2;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t starts[k_Max_Branch_خطی_Sub_گره_درازی];
    UBool isFinal[k_Max_Branch_خطی_Sub_گره_درازی-1];
    int32_t unit_عدد=0;
    افعل {
        int32_t i=starts[unit_عدد]=شروع;
        char16_t unit=getElementUnit(i++, unit_شاخص);
        i=شاخص_از_Element_With_بعدی_Unit(i, unit_شاخص, unit);
        isFinal[unit_عدد]= شروع==i-1 && unit_شاخص+1==get_Element_String_درازی(شروع);
        شروع=i;
    } طالما(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    starts[unit_عدد]=شروع;

    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t jumpTargets[k_Max_Branch_خطی_Sub_گره_درازی-1];
    افعل {
        --unit_عدد;
        إذا(!isFinal[unit_عدد]) {
            jumpTargets[unit_عدد]=write_گره(starts[unit_عدد], starts[unit_عدد+1], unit_شاخص+1);
        }
    } طالما(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    write_گره(شروع, حد, unit_شاخص+1);
    int32_t جابه_جایی=اكتب(getElementUnit(شروع, unit_شاخص));
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_عدد>=0) {
        شروع=starts[unit_عدد];
        int32_t value;
        إذا(isFinal[unit_عدد]) {
            // Write the final value for the one string ending with this unit.
            value=get_Element_مقدار(شروع);
        } وإلا {
            // Write the delta to the start position of the sub-node.
            value=جابه_جایی-jumpTargets[unit_عدد];
        }
        write_مقدار_And_Final(value, isFinal[unit_عدد]);
        جابه_جایی=اكتب(getElementUnit(شروع, unit_شاخص));
    }
    // Write the split-branch nodes.
    طالما(lt_درازی>0) {
        --lt_درازی;
        write_Delta_مقصد(lessThan[lt_درازی]);
        جابه_جایی=اكتب(میانه_Units[lt_درازی]);
    }
    أرجع جابه_جایی;
}

// Requires start<limit,
// and all strings of the [start..limit[ elements must be sorted and
// have a common prefix of length unitIndex.
StringTrieBuilder::Node*
StringTrieBuilder::make_گره(int32_t شروع,
                            int32_t حد,
                            int32_t unit_شاخص,
                            UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    UBool has_مقدار=خطأ;
    int32_t value=0;
    إذا(unit_شاخص==get_Element_String_درازی(شروع)) {
        // An intermediate or final value.
        value=get_Element_مقدار(شروع++);
        إذا(شروع==حد) {
            أرجع ثبات_Final_مقدار(value, error_کد);
        }
        has_مقدار=درست;
    }
    Node *گره;
    // Now all [start..limit[ strings are longer than unitIndex.
    int32_t minUnit=getElementUnit(شروع, unit_شاخص);
    int32_t maxUnit=getElementUnit(حد-1, unit_شاخص);
    إذا(minUnit==maxUnit) {
        // Linear-match node: All strings have the same character at unitIndex.
        int32_t آخرین_Unit_شاخص=get_حد_از_خطی_تطابق(شروع, حد-1, unit_شاخص);
        Node *بعدی_گره=make_گره(شروع, حد, آخرین_Unit_شاخص, error_کد);
        // Break the linear-match sequence into chunks of at most kMaxLinearMatchLength.
        int32_t length=آخرین_Unit_شاخص-unit_شاخص;
        int32_t max_خطی_تطابق_درازی=get_Max_خطی_تطابق_درازی();
        طالما(length>max_خطی_تطابق_درازی) {
            آخرین_Unit_شاخص-=max_خطی_تطابق_درازی;
            length-=max_خطی_تطابق_درازی;
            گره=create_خطی_تطابق_گره(شروع, آخرین_Unit_شاخص, max_خطی_تطابق_درازی, بعدی_گره);
            بعدی_گره=ثبات_گره(گره, error_کد);
        }
        گره=create_خطی_تطابق_گره(شروع, unit_شاخص, length, بعدی_گره);
    } وإلا {
        // Branch node.
        int32_t length=شمار_Element_Units(شروع, حد, unit_شاخص);
        // length>=2 because minUnit!=maxUnit.
        Node *sub_گره=make_Branch_Sub_گره(شروع, حد, unit_شاخص, length, error_کد);
        گره=جديد Branch_سر_گره(length, sub_گره);
    }
    إذا(has_مقدار && گره!=اشاره_تهی) {
        إذا(تطابق_Nodes_Can_Have_مقادیر()) {
            ((مقدار_گره *)گره)->مجموعه_مقدار(value);
        } وإلا {
            گره=جديد Intermediate_مقدار_گره(value, ثبات_گره(گره, error_کد));
        }
    }
    أرجع ثبات_گره(گره, error_کد);
}

// start<limit && all strings longer than unitIndex &&
// length different units at unitIndex
StringTrieBuilder::Node*
StringTrieBuilder::make_Branch_Sub_گره(int32_t شروع,
                                     int32_t حد,
                                     int32_t unit_شاخص,
                                     int32_t length,
                                     UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    char16_t میانه_Units[k_Max_تقسیم_Branch_سطوح];
    Node *lessThan[k_Max_تقسیم_Branch_سطوح];
    int32_t lt_درازی=0;
    طالما(length>get_Max_Branch_خطی_Sub_گره_درازی()) {
        // Branch on the middle unit.
        // First, find the middle unit.
        int32_t i=skipElementsBySomeUnits(شروع, unit_شاخص, length/2);
        // Create the less-than branch.
        میانه_Units[lt_درازی]=getElementUnit(i, unit_شاخص);  // middle unit
        lessThan[lt_درازی]=make_Branch_Sub_گره(شروع, i, unit_شاخص, length/2, error_کد);
        ++lt_درازی;
        // Continue for the greater-or-equal branch.
        شروع=i;
        length=length-length/2;
    }
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    List_Branch_گره *list_گره=جديد List_Branch_گره();
    إذا(list_گره==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    // For each unit, find its elements array start and whether it has a final value.
    int32_t unit_عدد=0;
    افعل {
        int32_t i=شروع;
        char16_t unit=getElementUnit(i++, unit_شاخص);
        i=شاخص_از_Element_With_بعدی_Unit(i, unit_شاخص, unit);
        إذا(شروع==i-1 && unit_شاخص+1==get_Element_String_درازی(شروع)) {
            list_گره->جمع_کن(unit, get_Element_مقدار(شروع));
        } وإلا {
            list_گره->جمع_کن(unit, make_گره(شروع, i, unit_شاخص+1, error_کد));
        }
        شروع=i;
    } طالما(++unit_عدد<length-1);
    // unitNumber==length-1, and the maxUnit elements range is [start..limit[
    char16_t unit=getElementUnit(شروع, unit_شاخص);
    إذا(شروع==حد-1 && unit_شاخص+1==get_Element_String_درازی(شروع)) {
        list_گره->جمع_کن(unit, get_Element_مقدار(شروع));
    } وإلا {
        list_گره->جمع_کن(unit, make_گره(شروع, حد, unit_شاخص+1, error_کد));
    }
    Node *گره=ثبات_گره(list_گره, error_کد);
    // Create the split-branch nodes.
    طالما(lt_درازی>0) {
        --lt_درازی;
        گره=ثبات_گره(
            جديد تقسیم_Branch_گره(میانه_Units[lt_درازی], lessThan[lt_درازی], گره), error_کد);
    }
    أرجع گره;
}

StringTrieBuilder::Node*
StringTrieBuilder::ثبات_گره(Node* new_گره, UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        حذف new_گره;
        أرجع اشاره_تهی;
    }
    إذا(new_گره==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    ثابت UHashElement *old=uhash_پیدا_کن(nodes, new_گره);
    إذا(old!=اشاره_تهی) {
        حذف new_گره;
        أرجع static_cast<Node*>(old->کلید.اشاره_گر);
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_مقدار=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#پایان_اگر
    uhash_puti(nodes, new_گره, 1, &error_کد);
    U_ASSERT(old_مقدار==0);
    إذا(U_FAILURE(error_کد)) {
        حذف new_گره;
        أرجع اشاره_تهی;
    }
    أرجع new_گره;
}

StringTrieBuilder::Node*
StringTrieBuilder::ثبات_Final_مقدار(int32_t value, UError_کد& error_کد) U_LIFETIME_BOUND {
    إذا(U_FAILURE(error_کد)) {
        أرجع اشاره_تهی;
    }
    Final_مقدار_گره کلید(value);
    ثابت UHashElement *old=uhash_پیدا_کن(nodes, &کلید);
    إذا(old!=اشاره_تهی) {
        أرجع static_cast<Node*>(old->کلید.اشاره_گر);
    }
    Node *new_گره=جديد Final_مقدار_گره(value);
    إذا(new_گره==اشاره_تهی) {
        error_کد=U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }
    // If uhash_puti() returns a non-zero value from an equivalent, previously
    // registered node, then uhash_find() failed to find that and we will leak newNode.
#إذا U_DEBUG
    int32_t old_مقدار=  // Only in debug mode to avoid a compiler warning about unused oldValue.
#پایان_اگر
    uhash_puti(nodes, new_گره, 1, &error_کد);
    U_ASSERT(old_مقدار==0);
    إذا(U_FAILURE(error_کد)) {
        حذف new_گره;
        أرجع اشاره_تهی;
    }
    أرجع new_گره;
}

int32_t
StringTrieBuilder::hash_گره(ثابت فراغ *گره) {
    أرجع static_cast<ثابت Node*>(گره)->hash_کد();
}

UBool
StringTrieBuilder::equalNodes(ثابت فراغ *چپ, ثابت فراغ *راست) {
    أرجع *static_cast<ثابت Node*>(چپ) == *static_cast<ثابت Node*>(راست);
}

منطقي
StringTrieBuilder::Node::عملگر==(ثابت Node &other) ثابت {
    أرجع هذا==&other || (typeid(*هذا)==typeid(other) && hash==other.hash);
}

int32_t
StringTrieBuilder::Node::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        جابه_جایی=یال_عدد;
    }
    أرجع یال_عدد;
}

منطقي
StringTrieBuilder::Final_مقدار_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!Node::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت Final_مقدار_گره &o=static_cast<ثابت Final_مقدار_گره &>(other);
    أرجع value==o.value;
}

فراغ
StringTrieBuilder::Final_مقدار_گره::اكتب(StringTrieBuilder &builder) {
    جابه_جایی=builder.write_مقدار_And_Final(value, درست);
}

منطقي
StringTrieBuilder::مقدار_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!Node::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت مقدار_گره &o=static_cast<ثابت مقدار_گره &>(other);
    أرجع has_مقدار==o.has_مقدار && (!has_مقدار || value==o.value);
}

منطقي
StringTrieBuilder::Intermediate_مقدار_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!مقدار_گره::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت Intermediate_مقدار_گره &o=static_cast<ثابت Intermediate_مقدار_گره &>(other);
    أرجع next==o.next;
}

int32_t
StringTrieBuilder::Intermediate_مقدار_گره::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        جابه_جایی=یال_عدد=next->mark_راست_یالها_اولی(یال_عدد);
    }
    أرجع یال_عدد;
}

فراغ
StringTrieBuilder::Intermediate_مقدار_گره::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    جابه_جایی=builder.write_مقدار_And_Final(value, خطأ);
}

منطقي
StringTrieBuilder::خطی_تطابق_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!مقدار_گره::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت خطی_تطابق_گره &o=static_cast<ثابت خطی_تطابق_گره &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::خطی_تطابق_گره::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        جابه_جایی=یال_عدد=next->mark_راست_یالها_اولی(یال_عدد);
    }
    أرجع یال_عدد;
}

منطقي
StringTrieBuilder::List_Branch_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!Node::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت List_Branch_گره &o=static_cast<ثابت List_Branch_گره &>(other);
    لكل(int32_t i=0; i<length; ++i) {
        إذا(units[i]!=o.units[i] || مقادیر[i]!=o.مقادیر[i] || equal[i]!=o.equal[i]) {
            أرجع خطأ;
        }
    }
    أرجع درست;
}

int32_t
StringTrieBuilder::List_Branch_گره::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        اولی_یال_عدد=یال_عدد;
        int32_t گام=0;
        int32_t i=length;
        افعل {
            Node *یال=equal[--i];
            إذا(یال!=اشاره_تهی) {
                یال_عدد=یال->mark_راست_یالها_اولی(یال_عدد-گام);
            }
            // For all but the rightmost edge, decrement the edge number.
            گام=1;
        } طالما(i>0);
        جابه_جایی=یال_عدد;
    }
    أرجع یال_عدد;
}

فراغ
StringTrieBuilder::List_Branch_گره::اكتب(StringTrieBuilder &builder) {
    // Write the sub-nodes in reverse order: The jump lengths are deltas from
    // after their own positions, so if we wrote the minUnit sub-node first,
    // then its jump delta would be larger.
    // Instead we write the minUnit sub-node last, for a shorter delta.
    int32_t unit_عدد=length-1;
    Node *راست_یال=equal[unit_عدد];
    int32_t راست_یال_عدد= راست_یال==اشاره_تهی ? اولی_یال_عدد : راست_یال->get_جابه_جایی();
    افعل {
        --unit_عدد;
        إذا(equal[unit_عدد]!=اشاره_تهی) {
            equal[unit_عدد]->write_Unless_Inside_راست_یال(اولی_یال_عدد, راست_یال_عدد, builder);
        }
    } طالما(unit_عدد>0);
    // The maxUnit sub-node is written as the very last one because we do
    // not jump for it at all.
    unit_عدد=length-1;
    إذا(راست_یال==اشاره_تهی) {
        builder.write_مقدار_And_Final(مقادیر[unit_عدد], درست);
    } وإلا {
        راست_یال->اكتب(builder);
    }
    جابه_جایی=builder.اكتب(units[unit_عدد]);
    // Write the rest of this node's unit-value pairs.
    طالما(--unit_عدد>=0) {
        int32_t value;
        UBool isFinal;
        إذا(equal[unit_عدد]==اشاره_تهی) {
            // Write the final value for the one string ending with this unit.
            value=مقادیر[unit_عدد];
            isFinal=درست;
        } وإلا {
            // Write the delta to the start position of the sub-node.
            U_ASSERT(equal[unit_عدد]->get_جابه_جایی()>0);
            value=جابه_جایی-equal[unit_عدد]->get_جابه_جایی();
            isFinal=خطأ;
        }
        builder.write_مقدار_And_Final(value, isFinal);
        جابه_جایی=builder.اكتب(units[unit_عدد]);
    }
}

منطقي
StringTrieBuilder::تقسیم_Branch_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!Node::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت تقسیم_Branch_گره &o=static_cast<ثابت تقسیم_Branch_گره &>(other);
    أرجع unit==o.unit && lessThan==o.lessThan && greaterOrEqual==o.greaterOrEqual;
}

int32_t
StringTrieBuilder::تقسیم_Branch_گره::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        اولی_یال_عدد=یال_عدد;
        یال_عدد=greaterOrEqual->mark_راست_یالها_اولی(یال_عدد);
        جابه_جایی=یال_عدد=lessThan->mark_راست_یالها_اولی(یال_عدد-1);
    }
    أرجع یال_عدد;
}

فراغ
StringTrieBuilder::تقسیم_Branch_گره::اكتب(StringTrieBuilder &builder) {
    // Encode the less-than branch first.
    lessThan->write_Unless_Inside_راست_یال(اولی_یال_عدد, greaterOrEqual->get_جابه_جایی(), builder);
    // Encode the greater-or-equal branch last because we do not jump for it at all.
    greaterOrEqual->اكتب(builder);
    // Write this node.
    U_ASSERT(lessThan->get_جابه_جایی()>0);
    builder.write_Delta_مقصد(lessThan->get_جابه_جایی());  // less-than
    جابه_جایی=builder.اكتب(unit);
}

منطقي
StringTrieBuilder::Branch_سر_گره::عملگر==(ثابت Node &other) ثابت {
    إذا(هذا==&other) {
        أرجع درست;
    }
    إذا(!مقدار_گره::عملگر==(other)) {
        أرجع خطأ;
    }
    ثابت Branch_سر_گره &o=static_cast<ثابت Branch_سر_گره &>(other);
    أرجع length==o.length && next==o.next;
}

int32_t
StringTrieBuilder::Branch_سر_گره::mark_راست_یالها_اولی(int32_t یال_عدد) {
    إذا(جابه_جایی==0) {
        جابه_جایی=یال_عدد=next->mark_راست_یالها_اولی(یال_عدد);
    }
    أرجع یال_عدد;
}

فراغ
StringTrieBuilder::Branch_سر_گره::اكتب(StringTrieBuilder &builder) {
    next->اكتب(builder);
    إذا(length<=builder.get_Min_خطی_تطابق()) {
        جابه_جایی=builder.write_مقدار_And_Type(has_مقدار, value, length-1);
    } وإلا {
        builder.اكتب(length-1);
        جابه_جایی=builder.write_مقدار_And_Type(has_مقدار, value, 0);
    }
}

U_NAMESPACE_انتها
