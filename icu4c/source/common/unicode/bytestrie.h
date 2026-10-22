// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrie.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010sep25
*   created by: Markus W. Scherer
*/

#నిర్వచితం_కాకపోతే __BYTESTRIE_H__
#నిర్వచించు __BYTESTRIE_H__

/**
 * \file
 * \brief C++ API: Trie for mapping byte sequences to integer values.
 */

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ U_SHOW_CPLUSPLUS_API

#అన్తర్భావ "unicode/stringpiece.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ustringtrie.h"

class బైట్లు_Trie_Test;

U_NAMESPACE_BEGIN

class ByteSink;
class బైట్లు_Trie_Builder;
class CharString;
class UVector32;

/**
 * Light-weight, non-const reader class for a BytesTrie.
 * Traverses a byte-serialized data structure with minimal state,
 * for mapping byte sequences to non-negative integer values.
 *
 * This class owns the serialized trie data only if it was constructed by
 * the builder's build() method.
 * The public constructor and the copy constructor only alias the data (only copy the pointer).
 * There is no assignment operator.
 *
 * This class is not intended for public subclassing.
 * @stable ICU 4.8
 */
class U_COMMON_API బైట్లు_Trie : బహిరంగ UMemory {
బహిరంగ:
    /**
     * Constructs a BytesTrie reader instance.
     *
     * The trieBytes must contain a copy of a byte sequence from the BytesTrieBuilder,
     * starting with the first byte of that sequence.
     * The BytesTrie object will not read more bytes than
     * the BytesTrieBuilder generated in the corresponding build() call.
     *
     * The array is not copied/cloned and must not be modified while
     * the BytesTrie object is in use.
     *
     * @param trieBytes The byte array that contains the serialized trie.
     * @stable ICU 4.8
     */
    బైట్లు_Trie(స్థిరరాశి శూన్య* trie_బైట్లు U_LIFETIME_BOUND)
            : ownedArray_(nullptr), బైట్లు_2(static_cast<స్థిరరాశి uint8_t *>(trie_బైట్లు)),
              pos_(బైట్లు_2), మిగిలిన_సరిపోలిక_నిడివి(-1) {}

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ~బైట్లు_Trie();

    /**
     * Copy constructor, copies the other trie reader object and its state,
     * but not the byte array which will be shared. (Shallow copy.)
     * @param other Another BytesTrie object.
     * @stable ICU 4.8
     */
    బైట్లు_Trie(స్థిరరాశి బైట్లు_Trie &other)
            : ownedArray_(nullptr), బైట్లు_2(other.బైట్లు_2),
              pos_(other.pos_), మిగిలిన_సరిపోలిక_నిడివి(other.మిగిలిన_సరిపోలిక_నిడివి) {}

    /**
     * Resets this trie to its initial state.
     * @return *this
     * @stable ICU 4.8
     */
    బైట్లు_Trie& reset() U_LIFETIME_BOUND {
        pos_=బైట్లు_2;
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
            static_cast<uint64_t>(pos_ - బైట్లు_2);
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
    బైట్లు_Trie& reset_గమ్యం_State64(uint64_t state) U_LIFETIME_BOUND {
        మిగిలిన_సరిపోలిక_నిడివి = static_cast<int32_t>(state >> k_State64_మిగిలిన_జరుపు) - 2;
        pos_ = బైట్లు_2 + (state & k_State64_Pos_ముసుగు);
        తిరిగి *this;
    }

    /**
     * BytesTrie state object, for saving a trie's current state
     * and resetting the trie back to this state later.
     * @stable ICU 4.8
     */
    class State : బహిరంగ UMemory {
    బహిరంగ:
        /**
         * Constructs an empty State.
         * @stable ICU 4.8
         */
        State() { బైట్లు=nullptr; }
    అంతరంగ:
        మిత్ర class బైట్లు_Trie;

        స్థిరరాశి uint8_t *బైట్లు;
        స్థిరరాశి uint8_t *pos;
        int32_t మిగిలిన_సరిపోలిక_నిడివి_2;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    స్థిరరాశి బైట్లు_Trie& saveState(State& state) స్థిరరాశి U_LIFETIME_BOUND {
        state.బైట్లు=బైట్లు_2;
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
    బైట్లు_Trie& reset_గమ్యం_State(స్థిరరాశి State& state) U_LIFETIME_BOUND {
        ఒకవేళ(బైట్లు_2==state.బైట్లు && బైట్లు_2!=nullptr) {
            pos_=state.pos;
            మిగిలిన_సరిపోలిక_నిడివి=state.మిగిలిన_సరిపోలిక_నిడివి_2;
        }
        తిరిగి *this;
    }

    /**
     * Determines whether the byte sequence so far matches, whether it has a value,
     * and whether another input byte can continue a matching byte sequence.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం current() స్థిరరాశి;

    /**
     * Traverses the trie from the initial state for this input byte.
     * Equivalent to reset().next(inByte).
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    అంతఃవరుస UString_Trie_ఫలితం మొదటి(int32_t లోపల_Byte) {
        మిగిలిన_సరిపోలిక_నిడివి=-1;
        ఒకవేళ(లోపల_Byte<0) {
            లోపల_Byte+=0x100;
        }
        తిరిగి తదుపరి_Impl(బైట్లు_2, లోపల_Byte);
    }

    /**
     * Traverses the trie from the current state for this input byte.
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం తదుపరి(int32_t లోపల_Byte);

    /**
     * Traverses the trie from the current state for this byte sequence.
     * Equivalent to
     * \code
     * Result result=current();
     * for(each c in s)
     *   if(!USTRINGTRIE_HAS_NEXT(result)) return USTRINGTRIE_NO_MATCH;
     *   result=next(c);
     * return result;
     * \endcode
     * @param s A string or byte sequence. Can be nullptr if length is 0.
     * @param length The length of the byte sequence. Can be -1 if NUL-terminated.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ఫలితం తదుపరి(స్థిరరాశి అక్షరం *s, int32_t నిడివి);

    /**
     * Returns a matching byte sequence's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the byte sequence so far.
     * @stable ICU 4.8
     */
    అంతఃవరుస int32_t get_విలువ() స్థిరరాశి {
        స్థిరరాశి uint8_t *pos=pos_;
        int32_t leadByte=*pos++;
        // U_ASSERT(leadByte>=kMinValueLead);
        తిరిగి read_విలువ(pos, leadByte>>1);
    }

    /**
     * Determines whether all byte sequences reachable from the current state
     * map to the same value.
     * @param uniqueValue Receives the unique value, if this function returns true.
     *                    (output-only)
     * @return true if all byte sequences reachable from the current state
     *         map to the same value.
     * @stable ICU 4.8
     */
    అంతఃవరుస UBool has_Unique_విలువ(int32_t &unique_విలువ) స్థిరరాశి {
        స్థిరరాశి uint8_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        తిరిగి pos!=nullptr && కనుగొను_Unique_విలువ(pos+మిగిలిన_సరిపోలిక_నిడివి+1, false, unique_విలువ);
    }

    /**
     * Finds each byte which continues the byte sequence from the current state.
     * That is, each byte b for which it would be next(b)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next byte is appended to this object.
     *            (Only uses the out.Append(s, length) method.)
     * @return the number of bytes which continue the byte sequence from here
     * @stable ICU 4.8
     */
    int32_t get_తదుపరి_బైట్లు(ByteSink &బహిర్గతం) స్థిరరాశి;

    /**
     * Iterator for all of the (byte sequence, value) pairs in a BytesTrie.
     * @stable ICU 4.8
     */
    class U_COMMON_API Iterator : బహిరంగ UMemory {
    బహిరంగ:
        /**
         * Iterates from the root of a byte-serialized BytesTrie.
         * @param trieBytes The trie bytes.
         * @param maxStringLength If 0, the iterator returns full strings/byte sequences.
         *                        Otherwise, the iterator returns strings with this maximum length.
         * @param errorCode Standard ICU error code. Its input value must
         *                  pass the U_SUCCESS() test, or else the function returns
         *                  immediately. Check for U_FAILURE() on output or use with
         *                  function chaining. (See User Guide for details.)
         * @stable ICU 4.8
         */
        Iterator(స్థిరరాశి శూన్య* trie_బైట్లు U_LIFETIME_BOUND,
                 int32_t max_String_నిడివి,
                 UError_సంకేతం& error_సంకేతం);

        /**
         * Iterates from the current state of the specified BytesTrie.
         * @param trie The trie whose state will be copied for iteration.
         * @param maxStringLength If 0, the iterator returns full strings/byte sequences.
         *                        Otherwise, the iterator returns strings with this maximum length.
         * @param errorCode Standard ICU error code. Its input value must
         *                  pass the U_SUCCESS() test, or else the function returns
         *                  immediately. Check for U_FAILURE() on output or use with
         *                  function chaining. (See User Guide for details.)
         * @stable ICU 4.8
         */
        Iterator(స్థిరరాశి బైట్లు_Trie &trie, int32_t max_String_నిడివి, UError_సంకేతం &error_సంకేతం);

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
         * Finds the next (byte sequence, value) pair if there is one.
         *
         * If the byte sequence is truncated to the maximum length and does not
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
         * @return The NUL-terminated byte sequence for the last successful next().
         * @stable ICU 4.8
         */
        StringPiece getString() స్థిరరాశి U_LIFETIME_BOUND;
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_విలువ() స్థిరరాశి { తిరిగి విలువ; }

    అంతరంగ:
        UBool truncateAndStop();

        స్థిరరాశి uint8_t* branch_తదుపరి(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND,
                                  int32_t నిడివి,
                                  UError_సంకేతం& error_సంకేతం);

        స్థిరరాశి uint8_t *బైట్లు_2;
        స్థిరరాశి uint8_t *pos_;
        స్థిరరాశి uint8_t *initialPos_;
        int32_t మిగిలిన_సరిపోలిక_నిడివి;
        int32_t initial_మిగిలిన_సరిపోలిక_నిడివి;

        CharString *str_;
        int32_t max_నిడివి;
        int32_t విలువ;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from bytes_.
        // The second integer has the str_->length() from before the node in bits 15..0,
        // and the remaining branch length in bits 24..16. (Bits 31..25 are unused.)
        // (We could store the remaining branch length minus 1 in bits 23..16 and not use bits 31..24,
        // but the code looks more confusing that way.)
        UVector32 *దొంతర;
    };

అంతరంగ:
    మిత్ర class బైట్లు_Trie_Builder;
    మిత్ర class ::బైట్లు_Trie_Test;

    /**
     * Constructs a BytesTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    బైట్లు_Trie(శూన్య *adopt_బైట్లు, స్థిరరాశి శూన్య *trie_బైట్లు)
            : ownedArray_(static_cast<uint8_t *>(adopt_బైట్లు)),
              బైట్లు_2(static_cast<స్థిరరాశి uint8_t *>(trie_బైట్లు)),
              pos_(బైట్లు_2), మిగిలిన_సరిపోలిక_నిడివి(-1) {}

    // No assignment operator.
    బైట్లు_Trie &క్రియాకారకం=(స్థిరరాశి బైట్లు_Trie &other) = delete;

    అంతఃవరుస శూన్య stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadByte, and the lead byte is already shifted right by 1.
    స్థిర int32_t read_విలువ(స్థిరరాశి uint8_t *pos, int32_t leadByte);
    స్థిర అంతఃవరుస స్థిరరాశి uint8_t* skip_విలువ(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND, int32_t leadByte) {
        // U_ASSERT(leadByte>=kMinValueLead);
        ఒకవేళ(leadByte>=(k_Min_Two_Byte_విలువ_Lead<<1)) {
            ఒకవేళ(leadByte<(k_Min_Three_Byte_విలువ_Lead<<1)) {
                ++pos;
            } లేకపోతే ఒకవేళ(leadByte<(k_Four_Byte_విలువ_Lead<<1)) {
                pos+=2;
            } లేకపోతే {
                pos+=3+((leadByte>>1)&1);
            }
        }
        తిరిగి pos;
    }
    స్థిర అంతఃవరుస స్థిరరాశి uint8_t* skip_విలువ(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND) {
        int32_t leadByte=*pos++;
        తిరిగి skip_విలువ(pos, leadByte);
    }

    // Reads a jump delta and jumps.
    స్థిర స్థిరరాశి uint8_t* jumpByDelta(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND);

    స్థిర అంతఃవరుస స్థిరరాశి uint8_t* skipDelta(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        ఒకవేళ(delta>=kMinTwoByteDeltaLead) {
            ఒకవేళ(delta<kMinThreeByteDeltaLead) {
                ++pos;
            } లేకపోతే ఒకవేళ(delta<kFourByteDeltaLead) {
                pos+=2;
            } లేకపోతే {
                pos+=3+(delta&1);
            }
        }
        తిరిగి pos;
    }

    స్థిర అంతఃవరుస UString_Trie_ఫలితం విలువ_ఫలితం(int32_t కణుపు) {
        తిరిగి static_cast<UString_Trie_ఫలితం>(USTRINGTRIE_INTERMEDIATE_విలువ - (కణుపు & k_విలువ_Is_Final));
    }

    // Handles a branch node for both next(byte) and next(string).
    UString_Trie_ఫలితం branch_తదుపరి(స్థిరరాశి uint8_t *pos, int32_t నిడివి, int32_t లోపల_Byte);

    // Requires remainingLength_<0.
    UString_Trie_ఫలితం తదుపరి_Impl(స్థిరరాశి uint8_t *pos, int32_t లోపల_Byte);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    స్థిర స్థిరరాశి uint8_t* కనుగొను_Unique_విలువ_From_Branch(స్థిరరాశి uint8_t* pos U_LIFETIME_BOUND,
                                                    int32_t నిడివి,
                                                    UBool have_Unique_విలువ,
                                                    int32_t& unique_విలువ);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead byte.
    స్థిర UBool కనుగొను_Unique_విలువ(స్థిరరాశి uint8_t *pos, UBool have_Unique_విలువ, int32_t &unique_విలువ);

    // Helper functions for getNextBytes().
    // getNextBytes() when pos is on a branch node.
    స్థిర శూన్య get_తదుపరి_Branch_బైట్లు(స్థిరరాశి uint8_t *pos, int32_t నిడివి, ByteSink &బహిర్గతం);
    స్థిర శూన్య append(ByteSink &బహిర్గతం, పూర్ణాంకం c);

    // BytesTrie data structure
    //
    // The trie consists of a series of byte-serialized nodes for incremental
    // string/byte sequence matching. The root node is at the beginning of the trie data.
    //
    // Types of nodes are distinguished by their node lead byte ranges.
    // After each node, except a final-value node, another node follows to
    // encode match values or continue matching further bytes.
    //
    // Node types:
    //  - Value node: Stores a 32-bit integer in a compact, variable-length format.
    //    The value is for the string/byte sequence so far.
    //    One node bit indicates whether the value is final or whether
    //    matching continues with the next node.
    //  - Linear-match node: Matches a number of bytes.
    //  - Branch node: Branches to other nodes according to the current input byte.
    //    The node byte is the length of the branch (number of bytes to select from)
    //    minus 1. It is followed by a sub-node:
    //    - If the length is at most kMaxBranchLinearSubNodeLength, then
    //      there are length-1 (key, value) pairs and then one more comparison byte.
    //      If one of the key bytes matches, then the value is either a final value for
    //      the string/byte sequence so far, or a "jump" delta to the next node.
    //      If the last byte matches, then matching continues with the next node.
    //      (Values have the same encoding as value nodes.)
    //    - If the length is greater than kMaxBranchLinearSubNodeLength, then
    //      there is one byte and one "jump" delta.
    //      If the input byte is less than the sub-node byte, then "jump" by delta to
    //      the next sub-node which will have a length of length/2.
    //      (The delta has its own compact encoding.)
    //      Otherwise, skip the "jump" delta to the next sub-node
    //      which will have a length of length-length/2.

    // Node lead byte values.

    // 00..0f: Branch node. If node!=0 then the length is node+1, otherwise
    // the length is one more than the next byte.

    // For a branch sub-node with at most this many entries, we drop down
    // to a linear search.
    స్థిర స్థిరరాశి int32_t k_Max_Branch_రేఖీయ_Sub_కణుపు_నిడివి=5;

    // 10..1f: Linear-match node, match 1..16 bytes and continue reading the next node.
    స్థిర స్థిరరాశి int32_t k_Min_రేఖీయ_సరిపోలిక=0x10;
    స్థిర స్థిరరాశి int32_t k_Max_రేఖీయ_సరిపోలిక_నిడివి=0x10;

    // 20..ff: Variable-length value node.
    // If odd, the value is final. (Otherwise, intermediate value or jump delta.)
    // Then shift-right by 1 bit.
    // The remaining lead byte value indicates the number of following bytes (0..4)
    // and contains the value's top bits.
    స్థిర స్థిరరాశి int32_t k_Min_విలువ_Lead=k_Min_రేఖీయ_సరిపోలిక+k_Max_రేఖీయ_సరిపోలిక_నిడివి;  // 0x20
    // It is a final value if bit 0 is set.
    స్థిర స్థిరరాశి int32_t k_విలువ_Is_Final=1;

    // Compact value: After testing bit 0, shift right by 1 and then use the following thresholds.
    స్థిర స్థిరరాశి int32_t k_Min_One_Byte_విలువ_Lead=k_Min_విలువ_Lead/2;  // 0x10
    స్థిర స్థిరరాశి int32_t k_Max_One_Byte_విలువ=0x40;  // At least 6 bits in the first byte.

    స్థిర స్థిరరాశి int32_t k_Min_Two_Byte_విలువ_Lead=k_Min_One_Byte_విలువ_Lead+k_Max_One_Byte_విలువ+1;  // 0x51
    స్థిర స్థిరరాశి int32_t k_Max_Two_Byte_విలువ=0x1aff;

    స్థిర స్థిరరాశి int32_t k_Min_Three_Byte_విలువ_Lead=k_Min_Two_Byte_విలువ_Lead+(k_Max_Two_Byte_విలువ>>8)+1;  // 0x6c
    స్థిర స్థిరరాశి int32_t k_Four_Byte_విలువ_Lead=0x7e;

    // A little more than Unicode code points. (0x11ffff)
    స్థిర స్థిరరాశి int32_t k_Max_Three_Byte_విలువ=((k_Four_Byte_విలువ_Lead-k_Min_Three_Byte_విలువ_Lead)<<16)-1;

    స్థిర స్థిరరాశి int32_t k_Five_Byte_విలువ_Lead=0x7f;

    // Compact delta integers.
    స్థిర స్థిరరాశి int32_t kMaxOneByteDelta=0xbf;
    స్థిర స్థిరరాశి int32_t kMinTwoByteDeltaLead=kMaxOneByteDelta+1;  // 0xc0
    స్థిర స్థిరరాశి int32_t kMinThreeByteDeltaLead=0xf0;
    స్థిర స్థిరరాశి int32_t kFourByteDeltaLead=0xfe;
    స్థిర స్థిరరాశి int32_t kFiveByteDeltaLead=0xff;

    స్థిర స్థిరరాశి int32_t kMaxTwoByteDelta=((kMinThreeByteDeltaLead-kMinTwoByteDeltaLead)<<8)-1;  // 0x2fff
    స్థిర స్థిరరాశి int32_t kMaxThreeByteDelta=((kFourByteDeltaLead-kMinThreeByteDeltaLead)<<16)-1;  // 0xdffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    స్థిర constexpr int32_t k_State64_మిగిలిన_జరుపు = 59;
    స్థిర constexpr uint64_t k_State64_Pos_ముసుగు = (UINT64_C(1) << k_State64_మిగిలిన_జరుపు) - 1;

    uint8_t *ownedArray_;

    // Fixed value referencing the BytesTrie bytes.
    స్థిరరాశి uint8_t *బైట్లు_2;

    // Iterator variables.

    // Pointer to next trie byte to read. nullptr if no more matches.
    స్థిరరాశి uint8_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t మిగిలిన_సరిపోలిక_నిడివి;
};

U_NAMESPACE_చివర

#అయితే_ముగింపు /* U_SHOW_CPLUSPLUS_API */

#అయితే_ముగింపు  // __BYTESTRIE_H__
