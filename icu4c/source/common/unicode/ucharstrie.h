// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrie.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov14
*   created by: Markus W. Scherer
*/

#నిర్వచితం_కాకపోతే __UCHARSTRIE_H__
#నిర్వచించు __UCHARSTRIE_H__

/**
 * \file
 * \brief C++ API: Trie for mapping Unicode strings (or 16-bit-unit sequences)
 *                 to integer values.
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ustringtrie.h"

U_NAMESPACE_BEGIN

class Appendable;
class UCharsTrieBuilder;
class UVector32;

/**
 * Light-weight, non-const reader class for a UCharsTrie.
 * Traverses a char16_t-serialized data structure with minimal state,
 * for mapping strings (16-bit-unit sequences) to non-negative integer values.
 *
 * This class owns the serialized trie data only if it was constructed by
 * the builder's build() method.
 * The public constructor and the copy constructor only alias the data (only copy the pointer).
 * There is no assignment operator.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API UCharsTrie : బహిరంగ UMemory {
బహిరంగ:
    /**
     * Constructs a UCharsTrie reader instance.
     *
     * The trieUChars must contain a copy of a char16_t sequence from the UCharsTrieBuilder,
     * starting with the first char16_t of that sequence.
     * The UCharsTrie object will not read more char16_ts than
     * the UCharsTrieBuilder generated in the corresponding build() call.
     *
     * The array is not copied/cloned and must not be modified while
     * the UCharsTrie object is in use.
     *
     * @param trieUChars The char16_t array that contains the serialized trie.
     * @stable ICU 4.8
     */
    UCharsTrie(ConstChar16Ptr trieUChars U_LIFETIME_BOUND)
            : ownedArray_(nullptr), uchars_(trieUChars),
              pos_(uchars_), మిగిలిన_సరిపోలిక_నిడివి(-1) {}

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ~UCharsTrie();

    /**
     * Copy constructor, copies the other trie reader object and its state,
     * but not the char16_t array which will be shared. (Shallow copy.)
     * @param other Another UCharsTrie object.
     * @stable ICU 4.8
     */
    UCharsTrie(స్థిరరాశి UCharsTrie &other)
            : ownedArray_(nullptr), uchars_(other.uchars_),
              pos_(other.pos_), మిగిలిన_సరిపోలిక_నిడివి(other.మిగిలిన_సరిపోలిక_నిడివి) {}

    /**
     * Resets this trie to its initial state.
     * @return *this
     * @stable ICU 4.8
     */
    UCharsTrie& reset() U_LIFETIME_BOUND {
        pos_=uchars_;
        మిగిలిన_సరిపోలిక_నిడివి=-1;
        తిరిగి *this;
    }

    /**
     * Returns the state of this trie as a 64-bit integer.
     * The state value is never 0.
     *
     * @return opaque state value
     * @see resetToState64
     * @stable ICU 65
     */
    uint64_t getState64() స్థిరరాశి {
        తిరిగి (static_cast<uint64_t>(మిగిలిన_సరిపోలిక_నిడివి + 2) << k_State64_మిగిలిన_జరుపు) |
            static_cast<uint64_t>(pos_ - uchars_);
    }

    /**
     * Resets this trie to the saved state.
     * Unlike resetToState(State), the 64-bit state value
     * must be from getState64() from the same trie object or
     * from one initialized the exact same way.
     * Because of no validation, this method is faster.
     *
     * @param state The opaque trie state value from getState64().
     * @return *this
     * @see getState64
     * @see resetToState
     * @see reset
     * @stable ICU 65
     */
    UCharsTrie& reset_గమ్యం_State64(uint64_t state) U_LIFETIME_BOUND {
        మిగిలిన_సరిపోలిక_నిడివి = static_cast<int32_t>(state >> k_State64_మిగిలిన_జరుపు) - 2;
        pos_ = uchars_ + (state & k_State64_Pos_ముసుగు);
        తిరిగి *this;
    }

    /**
     * UCharsTrie state object, for saving a trie's current state
     * and resetting the trie back to this state later.
     * @stable ICU 4.8
     */
    class State : బహిరంగ UMemory {
    బహిరంగ:
        /**
         * Constructs an empty State.
         * @stable ICU 4.8
         */
        State() { uchars=nullptr; }
    అంతరంగ:
        మిత్ర class UCharsTrie;

        స్థిరరాశి char16_t *uchars;
        స్థిరరాశి char16_t *pos;
        int32_t మిగిలిన_సరిపోలిక_నిడివి_2;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    స్థిరరాశి UCharsTrie& saveState(State& state) స్థిరరాశి U_LIFETIME_BOUND {
        state.uchars=uchars_;
        state.pos=pos_;
        state.మిగిలిన_సరిపోలిక_నిడివి_2=మిగిలిన_సరిపోలిక_నిడివి;
        తిరిగి *this;
    }

    /**
     * Resets this trie to the saved state.
     * If the state object contains no state, or the state of a different trie,
     * then this trie remains unchanged.
     * @param state The State object which holds a saved trie state.
     * @return *this
     * @see saveState
     * @see reset
     * @stable ICU 4.8
     */
    UCharsTrie& reset_గమ్యం_State(స్థిరరాశి State& state) U_LIFETIME_BOUND {
        ఒకవేళ(uchars_==state.uchars && uchars_!=nullptr) {
            pos_=state.pos;
            మిగిలిన_సరిపోలిక_నిడివి=state.మిగిలిన_సరిపోలిక_నిడివి_2;
        }
        తిరిగి *this;
    }

    /**
     * Determines whether the string so far matches, whether it has a value,
     * and whether another input char16_t can continue a matching string.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం current() స్థిరరాశి;

    /**
     * Traverses the trie from the initial state for this input char16_t.
     * Equivalent to reset().next(uchar).
     * @param uchar Input char value. Values below 0 and above 0xffff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    అంతఃవరుస UString_Trie_ఫలితం మొదటి(int32_t uchar) {
        మిగిలిన_సరిపోలిక_నిడివి=-1;
        తిరిగి తదుపరి_Impl(uchars_, uchar);
    }

    /**
     * Traverses the trie from the initial state for the
     * one or two UTF-16 code units for this input code point.
     * Equivalent to reset().nextForCodePoint(cp).
     * @param cp A Unicode code point 0..0x10ffff.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం మొదటి_For_సంకేతం_బిందువు(UChar32 cp);

    /**
     * Traverses the trie from the current state for this input char16_t.
     * @param uchar Input char value. Values below 0 and above 0xffff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం తదుపరి(int32_t uchar);

    /**
     * Traverses the trie from the current state for the
     * one or two UTF-16 code units for this input code point.
     * @param cp A Unicode code point 0..0x10ffff.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం తదుపరి_For_సంకేతం_బిందువు(UChar32 cp);

    /**
     * Traverses the trie from the current state for this string.
     * Equivalent to
     * \code
     * Result result=current();
     * for(each c in s)
     *   if(!USTRINGTRIE_HAS_NEXT(result)) return USTRINGTRIE_NO_MATCH;
     *   result=next(c);
     * return result;
     * \endcode
     * @param s A string. Can be nullptr if length is 0.
     * @param length The length of the string. Can be -1 if NUL-terminated.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం తదుపరి(ConstChar16Ptr s, int32_t నిడివి);

    /**
     * Returns a matching string's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the string so far.
     * @stable ICU 4.8
     */
    అంతఃవరుస int32_t get_విలువ() స్థిరరాశి {
        స్థిరరాశి char16_t *pos=pos_;
        int32_t leadUnit=*pos++;
        // U_ASSERT(leadUnit>=kMinValueLead);
        తిరిగి leadUnit&k_విలువ_Is_Final ?
            read_విలువ(pos, leadUnit&0x7fff) : read_కణుపు_విలువ(pos, leadUnit);
    }

    /**
     * Determines whether all strings reachable from the current state
     * map to the same value.
     * @param uniqueValue Receives the unique value, if this function returns true.
     *                    (output-only)
     * @return true if all strings reachable from the current state
     *         map to the same value.
     * @stable ICU 4.8
     */
    అంతఃవరుస UBool has_Unique_విలువ(int32_t &unique_విలువ) స్థిరరాశి {
        స్థిరరాశి char16_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        తిరిగి pos!=nullptr && కనుగొను_Unique_విలువ(pos+మిగిలిన_సరిపోలిక_నిడివి+1, false, unique_విలువ);
    }

    /**
     * Finds each char16_t which continues the string from the current state.
     * That is, each char16_t c for which it would be next(c)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next char16_t is appended to this object.
     * @return the number of char16_ts which continue the string from here
     * @stable ICU 4.8
     */
    int32_t get_తదుపరి_UChars(Appendable &బహిర్గతం) స్థిరరాశి;

    /**
     * Iterator for all of the (string, value) pairs in a UCharsTrie.
     * @stable ICU 4.8
     */
    class U_COMMON_API Iterator : బహిరంగ UMemory {
    బహిరంగ:
        /**
         * Iterates from the root of a char16_t-serialized UCharsTrie.
         * @param trieUChars The trie char16_ts.
         * @param maxStringLength If 0, the iterator returns full strings.
         *                        Otherwise, the iterator returns strings with this maximum length.
         * @param errorCode Standard ICU error code. Its input value must
         *                  pass the U_SUCCESS() test, or else the function returns
         *                  immediately. Check for U_FAILURE() on output or use with
         *                  function chaining. (See User Guide for details.)
         * @stable ICU 4.8
         */
        Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_నిడివి, UError_సంకేతం& error_సంకేతం);

        /**
         * Iterates from the current state of the specified UCharsTrie.
         * @param trie The trie whose state will be copied for iteration.
         * @param maxStringLength If 0, the iterator returns full strings.
         *                        Otherwise, the iterator returns strings with this maximum length.
         * @param errorCode Standard ICU error code. Its input value must
         *                  pass the U_SUCCESS() test, or else the function returns
         *                  immediately. Check for U_FAILURE() on output or use with
         *                  function chaining. (See User Guide for details.)
         * @stable ICU 4.8
         */
        Iterator(స్థిరరాశి UCharsTrie &trie, int32_t max_String_నిడివి, UError_సంకేతం &error_సంకేతం);

        /**
         * Destructor.
         * @stable ICU 4.8
         */
        ~Iterator();

        /**
         * Resets this iterator to its initial state.
         * @return *this
         * @stable ICU 4.8
         */
        Iterator& reset() U_LIFETIME_BOUND;

        /**
         * @return true if there are more elements.
         * @stable ICU 4.8
         */
        UBool has_తదుపరి() స్థిరరాశి;

        /**
         * Finds the next (string, value) pair if there is one.
         *
         * If the string is truncated to the maximum length and does not
         * have a real value, then the value is set to -1.
         * In this case, this "not a real value" is indistinguishable from
         * a real value of -1.
         * @param errorCode Standard ICU error code. Its input value must
         *                  pass the U_SUCCESS() test, or else the function returns
         *                  immediately. Check for U_FAILURE() on output or use with
         *                  function chaining. (See User Guide for details.)
         * @return true if there is another element.
         * @stable ICU 4.8
         */
        UBool తదుపరి(UError_సంకేతం &error_సంకేతం);

        /**
         * @return The string for the last successful next().
         * @stable ICU 4.8
         */
        స్థిరరాశి UnicodeString& getString() స్థిరరాశి U_LIFETIME_BOUND { తిరిగి str_; }
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_విలువ() స్థిరరాశి { తిరిగి విలువ; }

    అంతరంగ:
        UBool truncateAndStop() {
            pos_=nullptr;
            విలువ=-1;  // no real value for str
            తిరిగి true;
        }

        స్థిరరాశి char16_t* branch_తదుపరి(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                   int32_t నిడివి,
                                   UError_సంకేతం& error_సంకేతం);

        స్థిరరాశి char16_t *uchars_;
        స్థిరరాశి char16_t *pos_;
        స్థిరరాశి char16_t *initialPos_;
        int32_t మిగిలిన_సరిపోలిక_నిడివి;
        int32_t initial_మిగిలిన_సరిపోలిక_నిడివి;
        UBool skip_విలువ_2;  // Skip intermediate value which was already delivered.

        UnicodeString str_;
        int32_t max_నిడివి;
        int32_t విలువ;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from uchars_.
        // The second integer has the str_.length() from before the node in bits 15..0,
        // and the remaining branch length in bits 31..16.
        // (We could store the remaining branch length minus 1 in bits 30..16 and not use the sign bit,
        // but the code looks more confusing that way.)
        UVector32 *దొంతర;
    };

అంతరంగ:
    మిత్ర class UCharsTrieBuilder;

    /**
     * Constructs a UCharsTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    UCharsTrie(char16_t *adoptUChars, స్థిరరాశి char16_t *trieUChars)
            : ownedArray_(adoptUChars), uchars_(trieUChars),
              pos_(uchars_), మిగిలిన_సరిపోలిక_నిడివి(-1) {}

    // No assignment operator.
    UCharsTrie &క్రియాకారకం=(స్థిరరాశి UCharsTrie &other) = delete;

    అంతఃవరుస శూన్య stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadUnit, and the lead unit has bit 15 reset.
    స్థిర అంతఃవరుస int32_t read_విలువ(స్థిరరాశి char16_t *pos, int32_t leadUnit) {
        int32_t విలువ_2;
        ఒకవేళ(leadUnit<k_Min_Two_Unit_విలువ_Lead) {
            విలువ_2=leadUnit;
        } లేకపోతే ఒకవేళ(leadUnit<k_Three_Unit_విలువ_Lead) {
            విలువ_2=((leadUnit-k_Min_Two_Unit_విలువ_Lead)<<16)|*pos;
        } లేకపోతే {
            విలువ_2=(pos[0]<<16)|pos[1];
        }
        తిరిగి విలువ_2;
    }
    స్థిర అంతఃవరుస స్థిరరాశి char16_t* skip_విలువ(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                            int32_t leadUnit) {
        ఒకవేళ(leadUnit>=k_Min_Two_Unit_విలువ_Lead) {
            ఒకవేళ(leadUnit<k_Three_Unit_విలువ_Lead) {
                ++pos;
            } లేకపోతే {
                pos+=2;
            }
        }
        తిరిగి pos;
    }
    స్థిర అంతఃవరుస స్థిరరాశి char16_t* skip_విలువ(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND) {
        int32_t leadUnit=*pos++;
        తిరిగి skip_విలువ(pos, leadUnit&0x7fff);
    }

    స్థిర అంతఃవరుస int32_t read_కణుపు_విలువ(స్థిరరాశి char16_t *pos, int32_t leadUnit) {
        // U_ASSERT(kMinValueLead<=leadUnit && leadUnit<kValueIsFinal);
        int32_t విలువ_2;
        ఒకవేళ(leadUnit<k_Min_Two_Unit_కణుపు_విలువ_Lead) {
            విలువ_2=(leadUnit>>6)-1;
        } లేకపోతే ఒకవేళ(leadUnit<k_Three_Unit_కణుపు_విలువ_Lead) {
            విలువ_2=(((leadUnit&0x7fc0)-k_Min_Two_Unit_కణుపు_విలువ_Lead)<<10)|*pos;
        } లేకపోతే {
            విలువ_2=(pos[0]<<16)|pos[1];
        }
        తిరిగి విలువ_2;
    }
    స్థిర అంతఃవరుస స్థిరరాశి char16_t* skip_కణుపు_విలువ(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                                int32_t leadUnit) {
        // U_ASSERT(kMinValueLead<=leadUnit && leadUnit<kValueIsFinal);
        ఒకవేళ(leadUnit>=k_Min_Two_Unit_కణుపు_విలువ_Lead) {
            ఒకవేళ(leadUnit<k_Three_Unit_కణుపు_విలువ_Lead) {
                ++pos;
            } లేకపోతే {
                pos+=2;
            }
        }
        తిరిగి pos;
    }

    స్థిర అంతఃవరుస స్థిరరాశి char16_t* jumpByDelta(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        ఒకవేళ(delta>=kMinTwoUnitDeltaLead) {
            ఒకవేళ(delta==kThreeUnitDeltaLead) {
                delta=(pos[0]<<16)|pos[1];
                pos+=2;
            } లేకపోతే {
                delta=((delta-kMinTwoUnitDeltaLead)<<16)|*pos++;
            }
        }
        తిరిగి pos+delta;
    }

    స్థిర స్థిరరాశి char16_t* skipDelta(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        ఒకవేళ(delta>=kMinTwoUnitDeltaLead) {
            ఒకవేళ(delta==kThreeUnitDeltaLead) {
                pos+=2;
            } లేకపోతే {
                ++pos;
            }
        }
        తిరిగి pos;
    }

    స్థిర అంతఃవరుస UString_Trie_ఫలితం విలువ_ఫలితం(int32_t కణుపు) {
        తిరిగి static_cast<UString_Trie_ఫలితం>(USTRINGTRIE_INTERMEDIATE_విలువ - (కణుపు >> 15));
    }

    // Handles a branch node for both next(uchar) and next(string).
    UString_Trie_ఫలితం branch_తదుపరి(స్థిరరాశి char16_t *pos, int32_t నిడివి, int32_t uchar);

    // Requires remainingLength_<0.
    UString_Trie_ఫలితం తదుపరి_Impl(స్థిరరాశి char16_t *pos, int32_t uchar);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    స్థిర స్థిరరాశి char16_t* కనుగొను_Unique_విలువ_From_Branch(స్థిరరాశి char16_t* pos U_LIFETIME_BOUND,
                                                     int32_t నిడివి,
                                                     UBool have_Unique_విలువ,
                                                     int32_t& unique_విలువ);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead unit.
    స్థిర UBool కనుగొను_Unique_విలువ(స్థిరరాశి char16_t *pos, UBool have_Unique_విలువ, int32_t &unique_విలువ);

    // Helper functions for getNextUChars().
    // getNextUChars() when pos is on a branch node.
    స్థిర శూన్య get_తదుపరి_Branch_UChars(స్థిరరాశి char16_t *pos, int32_t నిడివి, Appendable &బహిర్గతం);

    // UCharsTrie data structure
    //
    // The trie consists of a series of char16_t-serialized nodes for incremental
    // Unicode string/char16_t sequence matching. (char16_t=16-bit unsigned integer)
    // The root node is at the beginning of the trie data.
    //
    // Types of nodes are distinguished by their node lead unit ranges.
    // After each node, except a final-value node, another node follows to
    // encode match values or continue matching further units.
    //
    // Node types:
    //  - Final-value node: Stores a 32-bit integer in a compact, variable-length format.
    //    The value is for the string/char16_t sequence so far.
    //  - Match node, optionally with an intermediate value in a different compact format.
    //    The value, if present, is for the string/char16_t sequence so far.
    //
    //  Aside from the value, which uses the node lead unit's high bits:
    //
    //  - Linear-match node: Matches a number of units.
    //  - Branch node: Branches to other nodes according to the current input unit.
    //    The node unit is the length of the branch (number of units to select from)
    //    minus 1. It is followed by a sub-node:
    //    - If the length is at most kMaxBranchLinearSubNodeLength, then
    //      there are length-1 (key, value) pairs and then one more comparison unit.
    //      If one of the key units matches, then the value is either a final value for
    //      the string so far, or a "jump" delta to the next node.
    //      If the last unit matches, then matching continues with the next node.
    //      (Values have the same encoding as final-value nodes.)
    //    - If the length is greater than kMaxBranchLinearSubNodeLength, then
    //      there is one unit and one "jump" delta.
    //      If the input unit is less than the sub-node unit, then "jump" by delta to
    //      the next sub-node which will have a length of length/2.
    //      (The delta has its own compact encoding.)
    //      Otherwise, skip the "jump" delta to the next sub-node
    //      which will have a length of length-length/2.

    // Match-node lead unit values, after masking off intermediate-value bits:

    // 0000..002f: Branch node. If node!=0 then the length is node+1, otherwise
    // the length is one more than the next unit.

    // For a branch sub-node with at most this many entries, we drop down
    // to a linear search.
    స్థిర స్థిరరాశి int32_t k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి=5;

    // 0030..003f: Linear-match node, match 1..16 units and continue reading the next node.
    స్థిర స్థిరరాశి int32_t k_Min_రేఖీయ_సరిపోలిక=0x30;
    స్థిర స్థిరరాశి int32_t k_Max_రేఖీయ_సరిపోలిక_నిడివి=0x10;

    // Match-node lead unit bits 14..6 for the optional intermediate value.
    // If these bits are 0, then there is no intermediate value.
    // Otherwise, see the *NodeValue* constants below.
    స్థిర స్థిరరాశి int32_t k_Min_విలువ_Lead=k_Min_రేఖీయ_సరిపోలిక+k_Max_రేఖీయ_సరిపోలిక_నిడివి;  // 0x0040
    స్థిర స్థిరరాశి int32_t k_కణుపు_Type_ముసుగు=k_Min_విలువ_Lead-1;  // 0x003f

    // A final-value node has bit 15 set.
    స్థిర స్థిరరాశి int32_t k_విలువ_Is_Final=0x8000;

    // Compact value: After testing and masking off bit 15, use the following thresholds.
    స్థిర స్థిరరాశి int32_t k_Max_One_Unit_విలువ=0x3fff;

    స్థిర స్థిరరాశి int32_t k_Min_Two_Unit_విలువ_Lead=k_Max_One_Unit_విలువ+1;  // 0x4000
    స్థిర స్థిరరాశి int32_t k_Three_Unit_విలువ_Lead=0x7fff;

    స్థిర స్థిరరాశి int32_t k_Max_Two_Unit_విలువ=((k_Three_Unit_విలువ_Lead-k_Min_Two_Unit_విలువ_Lead)<<16)-1;  // 0x3ffeffff

    // Compact intermediate-value integer, lead unit shared with a branch or linear-match node.
    స్థిర స్థిరరాశి int32_t k_Max_One_Unit_కణుపు_విలువ=0xff;
    స్థిర స్థిరరాశి int32_t k_Min_Two_Unit_కణుపు_విలువ_Lead=k_Min_విలువ_Lead+((k_Max_One_Unit_కణుపు_విలువ+1)<<6);  // 0x4040
    స్థిర స్థిరరాశి int32_t k_Three_Unit_కణుపు_విలువ_Lead=0x7fc0;

    స్థిర స్థిరరాశి int32_t k_Max_Two_Unit_కణుపు_విలువ=
        ((k_Three_Unit_కణుపు_విలువ_Lead-k_Min_Two_Unit_కణుపు_విలువ_Lead)<<10)-1;  // 0xfdffff

    // Compact delta integers.
    స్థిర స్థిరరాశి int32_t kMaxOneUnitDelta=0xfbff;
    స్థిర స్థిరరాశి int32_t kMinTwoUnitDeltaLead=kMaxOneUnitDelta+1;  // 0xfc00
    స్థిర స్థిరరాశి int32_t kThreeUnitDeltaLead=0xffff;

    స్థిర స్థిరరాశి int32_t kMaxTwoUnitDelta=((kThreeUnitDeltaLead-kMinTwoUnitDeltaLead)<<16)-1;  // 0x03feffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    స్థిర constexpr int32_t k_State64_మిగిలిన_జరుపు = 59;
    స్థిర constexpr uint64_t k_State64_Pos_ముసుగు = (UINT64_C(1) << k_State64_మిగిలిన_జరుపు) - 1;

    char16_t *ownedArray_;

    // Fixed value referencing the UCharsTrie words.
    స్థిరరాశి char16_t *uchars_;

    // Iterator variables.

    // Pointer to next trie unit to read. nullptr if no more matches.
    స్థిరరాశి char16_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t మిగిలిన_సరిపోలిక_నిడివి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __UCHARSTRIE_H__
