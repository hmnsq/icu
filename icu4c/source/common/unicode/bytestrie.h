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

#ifndef __BYTESTRIE_H__
#define __BYTESTRIE_H__

/**
 * \file
 * \brief C++ API: Trie for mapping byte sequences to integer values.
 */

#অন্তর্ভাৱ "unicode/utypes.h"

#যদি U_SHOW_CPLUSPLUS_API

#অন্তর্ভাৱ "unicode/stringpiece.h"
#অন্তর্ভাৱ "unicode/uobject.h"
#অন্তর্ভাৱ "unicode/ustringtrie.h"

class BytesTrieTest;

U_NAMESPACE_BEGIN

class ByteSink;
class BytesTrieBuilder;
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
class U_COMMON_API BytesTrie : সার্ৱজনিক UMemory {
সার্ৱজনিক:
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
    BytesTrie(ধ্রুৱ শূন্য* trieBytes U_LIFETIME_BOUND)
            : ownedArray_(nullptr), bytes_(static_cast<ধ্রুৱ uint8_t *>(trieBytes)),
              pos_(bytes_), remainingMatchLength_(-1) {}

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ~BytesTrie();

    /**
     * Copy constructor, copies the other trie reader object and its state,
     * but not the byte array which will be shared. (Shallow copy.)
     * @param other Another BytesTrie object.
     * @stable ICU 4.8
     */
    BytesTrie(ধ্রুৱ BytesTrie &other)
            : ownedArray_(nullptr), bytes_(other.bytes_),
              pos_(other.pos_), remainingMatchLength_(other.remainingMatchLength_) {}

    /**
     * Resets this trie to its initial state.
     * @return *this
     * @stable ICU 4.8
     */
    BytesTrie& reset() U_LIFETIME_BOUND {
        pos_=bytes_;
        remainingMatchLength_=-1;
        ঘূৰাই *this;
    }

    /**
     * Returns the state of this trie as a 64-bit integer.
     * The state value is never 0.
     *
     * @return opaque state value
     * @see resetToState64
     * @stable ICU 65
     */
    uint64_t getState64() ধ্রুৱ {
        ঘূৰাই (static_cast<uint64_t>(remainingMatchLength_ + 2) << kState64RemainingShift) |
            static_cast<uint64_t>(pos_ - bytes_);
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
    BytesTrie& resetToState64(uint64_t state) U_LIFETIME_BOUND {
        remainingMatchLength_ = static_cast<int32_t>(state >> kState64RemainingShift) - 2;
        pos_ = bytes_ + (state & kState64PosMask);
        ঘূৰাই *this;
    }

    /**
     * BytesTrie state object, for saving a trie's current state
     * and resetting the trie back to this state later.
     * @stable ICU 4.8
     */
    class State : সার্ৱজনিক UMemory {
    সার্ৱজনিক:
        /**
         * Constructs an empty State.
         * @stable ICU 4.8
         */
        State() { bytes=nullptr; }
    গুপ্ত:
        friend class BytesTrie;

        ধ্রুৱ uint8_t *bytes;
        ধ্রুৱ uint8_t *pos;
        int32_t remainingMatchLength;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    ধ্রুৱ BytesTrie& saveState(State& state) ধ্রুৱ U_LIFETIME_BOUND {
        state.bytes=bytes_;
        state.pos=pos_;
        state.remainingMatchLength=remainingMatchLength_;
        ঘূৰাই *this;
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
    BytesTrie& resetToState(ধ্রুৱ State& state) U_LIFETIME_BOUND {
        যদি(bytes_==state.bytes && bytes_!=nullptr) {
            pos_=state.pos;
            remainingMatchLength_=state.remainingMatchLength;
        }
        ঘূৰাই *this;
    }

    /**
     * Determines whether the byte sequence so far matches, whether it has a value,
     * and whether another input byte can continue a matching byte sequence.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UStringTrieResult current() ধ্রুৱ;

    /**
     * Traverses the trie from the initial state for this input byte.
     * Equivalent to reset().next(inByte).
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    অন্তঃস্থ UStringTrieResult first(int32_t inByte) {
        remainingMatchLength_=-1;
        যদি(inByte<0) {
            inByte+=0x100;
        }
        ঘূৰাই nextImpl(bytes_, inByte);
    }

    /**
     * Traverses the trie from the current state for this input byte.
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UStringTrieResult next(int32_t inByte);

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
    UStringTrieResult next(ধ্রুৱ ৱর্ণ *s, int32_t length);

    /**
     * Returns a matching byte sequence's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the byte sequence so far.
     * @stable ICU 4.8
     */
    অন্তঃস্থ int32_t get_মান() ধ্রুৱ {
        ধ্রুৱ uint8_t *pos=pos_;
        int32_t leadByte=*pos++;
        // U_ASSERT(leadByte>=kMinValueLead);
        ঘূৰাই read_মান(pos, leadByte>>1);
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
    অন্তঃস্থ UBool has_Unique_মান(int32_t &unique_মান) ধ্রুৱ {
        ধ্রুৱ uint8_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        ঘূৰাই pos!=nullptr && find_Unique_মান(pos+remainingMatchLength_+1, false, unique_মান);
    }

    /**
     * Finds each byte which continues the byte sequence from the current state.
     * That is, each byte b for which it would be next(b)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next byte is appended to this object.
     *            (Only uses the out.Append(s, length) method.)
     * @return the number of bytes which continue the byte sequence from here
     * @stable ICU 4.8
     */
    int32_t getNextBytes(ByteSink &out) ধ্রুৱ;

    /**
     * Iterator for all of the (byte sequence, value) pairs in a BytesTrie.
     * @stable ICU 4.8
     */
    class U_COMMON_API Iterator : সার্ৱজনিক UMemory {
    সার্ৱজনিক:
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
        Iterator(ধ্রুৱ শূন্য* trieBytes U_LIFETIME_BOUND,
                 int32_t maxStringLength,
                 UErrorCode& errorCode);

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
        Iterator(ধ্রুৱ BytesTrie &trie, int32_t maxStringLength, UErrorCode &errorCode);

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
        UBool hasNext() ধ্রুৱ;

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
        UBool next(UErrorCode &errorCode);

        /**
         * @return The NUL-terminated byte sequence for the last successful next().
         * @stable ICU 4.8
         */
        StringPiece getString() ধ্রুৱ U_LIFETIME_BOUND;
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_মান() ধ্রুৱ { ঘূৰাই মান; }

    গুপ্ত:
        UBool truncateAndStop();

        ধ্রুৱ uint8_t* branchNext(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND,
                                  int32_t length,
                                  UErrorCode& errorCode);

        ধ্রুৱ uint8_t *bytes_;
        ধ্রুৱ uint8_t *pos_;
        ধ্রুৱ uint8_t *initialPos_;
        int32_t remainingMatchLength_;
        int32_t initialRemainingMatchLength_;

        CharString *str_;
        int32_t maxLength_;
        int32_t মান;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from bytes_.
        // The second integer has the str_->length() from before the node in bits 15..0,
        // and the remaining branch length in bits 24..16. (Bits 31..25 are unused.)
        // (We could store the remaining branch length minus 1 in bits 23..16 and not use bits 31..24,
        // but the code looks more confusing that way.)
        UVector32 *স্তূপ;
    };

গুপ্ত:
    friend class BytesTrieBuilder;
    friend class ::BytesTrieTest;

    /**
     * Constructs a BytesTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    BytesTrie(শূন্য *adoptBytes, ধ্রুৱ শূন্য *trieBytes)
            : ownedArray_(static_cast<uint8_t *>(adoptBytes)),
              bytes_(static_cast<ধ্রুৱ uint8_t *>(trieBytes)),
              pos_(bytes_), remainingMatchLength_(-1) {}

    // No assignment operator.
    BytesTrie &operator=(ধ্রুৱ BytesTrie &other) = delete;

    অন্তঃস্থ শূন্য stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadByte, and the lead byte is already shifted right by 1.
    স্থির int32_t read_মান(ধ্রুৱ uint8_t *pos, int32_t leadByte);
    স্থির অন্তঃস্থ ধ্রুৱ uint8_t* skip_মান(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND, int32_t leadByte) {
        // U_ASSERT(leadByte>=kMinValueLead);
        যদি(leadByte>=(k_Min_Two_Byte_মান_Lead<<1)) {
            যদি(leadByte<(k_Min_Three_Byte_মান_Lead<<1)) {
                ++pos;
            } অন্যথা যদি(leadByte<(k_Four_Byte_মান_Lead<<1)) {
                pos+=2;
            } অন্যথা {
                pos+=3+((leadByte>>1)&1);
            }
        }
        ঘূৰাই pos;
    }
    স্থির অন্তঃস্থ ধ্রুৱ uint8_t* skip_মান(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND) {
        int32_t leadByte=*pos++;
        ঘূৰাই skip_মান(pos, leadByte);
    }

    // Reads a jump delta and jumps.
    স্থির ধ্রুৱ uint8_t* jumpByDelta(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND);

    স্থির অন্তঃস্থ ধ্রুৱ uint8_t* skipDelta(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        যদি(delta>=kMinTwoByteDeltaLead) {
            যদি(delta<kMinThreeByteDeltaLead) {
                ++pos;
            } অন্যথা যদি(delta<kFourByteDeltaLead) {
                pos+=2;
            } অন্যথা {
                pos+=3+(delta&1);
            }
        }
        ঘূৰাই pos;
    }

    স্থির অন্তঃস্থ UStringTrieResult মান_Result(int32_t গ্ৰন্থি) {
        ঘূৰাই static_cast<UStringTrieResult>(USTRINGTRIE_INTERMEDIATE_মান - (গ্ৰন্থি & k_মান_Is_Final));
    }

    // Handles a branch node for both next(byte) and next(string).
    UStringTrieResult branchNext(ধ্রুৱ uint8_t *pos, int32_t length, int32_t inByte);

    // Requires remainingLength_<0.
    UStringTrieResult nextImpl(ধ্রুৱ uint8_t *pos, int32_t inByte);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    স্থির ধ্রুৱ uint8_t* find_Unique_মান_From_Branch(ধ্রুৱ uint8_t* pos U_LIFETIME_BOUND,
                                                    int32_t length,
                                                    UBool have_Unique_মান,
                                                    int32_t& unique_মান);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead byte.
    স্থির UBool find_Unique_মান(ধ্রুৱ uint8_t *pos, UBool have_Unique_মান, int32_t &unique_মান);

    // Helper functions for getNextBytes().
    // getNextBytes() when pos is on a branch node.
    স্থির শূন্য getNextBranchBytes(ধ্রুৱ uint8_t *pos, int32_t length, ByteSink &out);
    স্থির শূন্য append(ByteSink &out, পূর্ণাঙ্ক c);

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
    স্থির ধ্রুৱ int32_t k_Max_Branch_Linear_Sub_গ্ৰন্থি_Length=5;

    // 10..1f: Linear-match node, match 1..16 bytes and continue reading the next node.
    স্থির ধ্রুৱ int32_t kMinLinearMatch=0x10;
    স্থির ধ্রুৱ int32_t kMaxLinearMatchLength=0x10;

    // 20..ff: Variable-length value node.
    // If odd, the value is final. (Otherwise, intermediate value or jump delta.)
    // Then shift-right by 1 bit.
    // The remaining lead byte value indicates the number of following bytes (0..4)
    // and contains the value's top bits.
    স্থির ধ্রুৱ int32_t k_Min_মান_Lead=kMinLinearMatch+kMaxLinearMatchLength;  // 0x20
    // It is a final value if bit 0 is set.
    স্থির ধ্রুৱ int32_t k_মান_Is_Final=1;

    // Compact value: After testing bit 0, shift right by 1 and then use the following thresholds.
    স্থির ধ্রুৱ int32_t k_Min_One_Byte_মান_Lead=k_Min_মান_Lead/2;  // 0x10
    স্থির ধ্রুৱ int32_t k_Max_One_Byte_মান=0x40;  // At least 6 bits in the first byte.

    স্থির ধ্রুৱ int32_t k_Min_Two_Byte_মান_Lead=k_Min_One_Byte_মান_Lead+k_Max_One_Byte_মান+1;  // 0x51
    স্থির ধ্রুৱ int32_t k_Max_Two_Byte_মান=0x1aff;

    স্থির ধ্রুৱ int32_t k_Min_Three_Byte_মান_Lead=k_Min_Two_Byte_মান_Lead+(k_Max_Two_Byte_মান>>8)+1;  // 0x6c
    স্থির ধ্রুৱ int32_t k_Four_Byte_মান_Lead=0x7e;

    // A little more than Unicode code points. (0x11ffff)
    স্থির ধ্রুৱ int32_t k_Max_Three_Byte_মান=((k_Four_Byte_মান_Lead-k_Min_Three_Byte_মান_Lead)<<16)-1;

    স্থির ধ্রুৱ int32_t k_Five_Byte_মান_Lead=0x7f;

    // Compact delta integers.
    স্থির ধ্রুৱ int32_t kMaxOneByteDelta=0xbf;
    স্থির ধ্রুৱ int32_t kMinTwoByteDeltaLead=kMaxOneByteDelta+1;  // 0xc0
    স্থির ধ্রুৱ int32_t kMinThreeByteDeltaLead=0xf0;
    স্থির ধ্রুৱ int32_t kFourByteDeltaLead=0xfe;
    স্থির ধ্রুৱ int32_t kFiveByteDeltaLead=0xff;

    স্থির ধ্রুৱ int32_t kMaxTwoByteDelta=((kMinThreeByteDeltaLead-kMinTwoByteDeltaLead)<<8)-1;  // 0x2fff
    স্থির ধ্রুৱ int32_t kMaxThreeByteDelta=((kFourByteDeltaLead-kMinThreeByteDeltaLead)<<16)-1;  // 0xdffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    স্থির constexpr int32_t kState64RemainingShift = 59;
    স্থির constexpr uint64_t kState64PosMask = (UINT64_C(1) << kState64RemainingShift) - 1;

    uint8_t *ownedArray_;

    // Fixed value referencing the BytesTrie bytes.
    ধ্রুৱ uint8_t *bytes_;

    // Iterator variables.

    // Pointer to next trie byte to read. nullptr if no more matches.
    ধ্রুৱ uint8_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t remainingMatchLength_;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIE_H__
