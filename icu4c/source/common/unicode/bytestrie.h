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

#अन्तर्भाव "unicode/utypes.h"

#जर U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ustringtrie.h"

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
class U_COMMON_API BytesTrie : सार्वजनिक UMemory {
सार्वजनिक:
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
    BytesTrie(स्थिरांक शून्य* trieBytes U_LIFETIME_BOUND)
            : ownedArray_(nullptr), bytes_(static_cast<स्थिरांक uint8_t *>(trieBytes)),
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
    BytesTrie(स्थिरांक BytesTrie &other)
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
        परत *this;
    }

    /**
     * Returns the state of this trie as a 64-bit integer.
     * The state value is never 0.
     *
     * @return opaque state value
     * @see resetToState64
     * @stable ICU 65
     */
    uint64_t getState64() स्थिरांक {
        परत (static_cast<uint64_t>(remainingMatchLength_ + 2) << kState64RemainingShift) |
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
        परत *this;
    }

    /**
     * BytesTrie state object, for saving a trie's current state
     * and resetting the trie back to this state later.
     * @stable ICU 4.8
     */
    class State : सार्वजनिक UMemory {
    सार्वजनिक:
        /**
         * Constructs an empty State.
         * @stable ICU 4.8
         */
        State() { bytes=nullptr; }
    खाजगी:
        मित्र class BytesTrie;

        स्थिरांक uint8_t *bytes;
        स्थिरांक uint8_t *pos;
        int32_t remainingMatchLength;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    स्थिरांक BytesTrie& saveState(State& state) स्थिरांक U_LIFETIME_BOUND {
        state.bytes=bytes_;
        state.pos=pos_;
        state.remainingMatchLength=remainingMatchLength_;
        परत *this;
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
    BytesTrie& resetToState(स्थिरांक State& state) U_LIFETIME_BOUND {
        जर(bytes_==state.bytes && bytes_!=nullptr) {
            pos_=state.pos;
            remainingMatchLength_=state.remainingMatchLength;
        }
        परत *this;
    }

    /**
     * Determines whether the byte sequence so far matches, whether it has a value,
     * and whether another input byte can continue a matching byte sequence.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UStringTrieResult current() स्थिरांक;

    /**
     * Traverses the trie from the initial state for this input byte.
     * Equivalent to reset().next(inByte).
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    ओळीत UStringTrieResult first(int32_t inByte) {
        remainingMatchLength_=-1;
        जर(inByte<0) {
            inByte+=0x100;
        }
        परत nextImpl(bytes_, inByte);
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
    UStringTrieResult next(स्थिरांक अक्षर *s, int32_t length);

    /**
     * Returns a matching byte sequence's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the byte sequence so far.
     * @stable ICU 4.8
     */
    ओळीत int32_t get_मूल्य() स्थिरांक {
        स्थिरांक uint8_t *pos=pos_;
        int32_t leadByte=*pos++;
        // U_ASSERT(leadByte>=kMinValueLead);
        परत read_मूल्य(pos, leadByte>>1);
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
    ओळीत UBool has_Unique_मूल्य(int32_t &unique_मूल्य) स्थिरांक {
        स्थिरांक uint8_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        परत pos!=nullptr && find_Unique_मूल्य(pos+remainingMatchLength_+1, false, unique_मूल्य);
    }

    /**
     * Finds each byte which continues the byte sequence from the current state.
     * That is, each byte b for which it would be next(b)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next byte is appended to this object.
     *            (Only uses the out.Append(s, length) method.)
     * @return the number of bytes which continue the byte sequence from here
     * @stable ICU 4.8
     */
    int32_t getNextBytes(ByteSink &out) स्थिरांक;

    /**
     * Iterator for all of the (byte sequence, value) pairs in a BytesTrie.
     * @stable ICU 4.8
     */
    class U_COMMON_API Iterator : सार्वजनिक UMemory {
    सार्वजनिक:
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
        Iterator(स्थिरांक शून्य* trieBytes U_LIFETIME_BOUND,
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
        Iterator(स्थिरांक BytesTrie &trie, int32_t maxStringLength, UErrorCode &errorCode);

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
        UBool hasNext() स्थिरांक;

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
        StringPiece getString() स्थिरांक U_LIFETIME_BOUND;
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_मूल्य() स्थिरांक { परत मूल्य; }

    खाजगी:
        UBool truncateAndStop();

        स्थिरांक uint8_t* branchNext(स्थिरांक uint8_t* pos U_LIFETIME_BOUND,
                                  int32_t length,
                                  UErrorCode& errorCode);

        स्थिरांक uint8_t *bytes_;
        स्थिरांक uint8_t *pos_;
        स्थिरांक uint8_t *initialPos_;
        int32_t remainingMatchLength_;
        int32_t initialRemainingMatchLength_;

        CharString *str_;
        int32_t maxLength_;
        int32_t मूल्य;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from bytes_.
        // The second integer has the str_->length() from before the node in bits 15..0,
        // and the remaining branch length in bits 24..16. (Bits 31..25 are unused.)
        // (We could store the remaining branch length minus 1 in bits 23..16 and not use bits 31..24,
        // but the code looks more confusing that way.)
        UVector32 *चळत;
    };

खाजगी:
    मित्र class BytesTrieBuilder;
    मित्र class ::BytesTrieTest;

    /**
     * Constructs a BytesTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    BytesTrie(शून्य *adoptBytes, स्थिरांक शून्य *trieBytes)
            : ownedArray_(static_cast<uint8_t *>(adoptBytes)),
              bytes_(static_cast<स्थिरांक uint8_t *>(trieBytes)),
              pos_(bytes_), remainingMatchLength_(-1) {}

    // No assignment operator.
    BytesTrie &चालक=(स्थिरांक BytesTrie &other) = delete;

    ओळीत शून्य stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadByte, and the lead byte is already shifted right by 1.
    स्थिर int32_t read_मूल्य(स्थिरांक uint8_t *pos, int32_t leadByte);
    स्थिर ओळीत स्थिरांक uint8_t* skip_मूल्य(स्थिरांक uint8_t* pos U_LIFETIME_BOUND, int32_t leadByte) {
        // U_ASSERT(leadByte>=kMinValueLead);
        जर(leadByte>=(k_Min_Two_Byte_मूल्य_Lead<<1)) {
            जर(leadByte<(k_Min_Three_Byte_मूल्य_Lead<<1)) {
                ++pos;
            } अन्यथा जर(leadByte<(k_Four_Byte_मूल्य_Lead<<1)) {
                pos+=2;
            } अन्यथा {
                pos+=3+((leadByte>>1)&1);
            }
        }
        परत pos;
    }
    स्थिर ओळीत स्थिरांक uint8_t* skip_मूल्य(स्थिरांक uint8_t* pos U_LIFETIME_BOUND) {
        int32_t leadByte=*pos++;
        परत skip_मूल्य(pos, leadByte);
    }

    // Reads a jump delta and jumps.
    स्थिर स्थिरांक uint8_t* jumpByDelta(स्थिरांक uint8_t* pos U_LIFETIME_BOUND);

    स्थिर ओळीत स्थिरांक uint8_t* skipDelta(स्थिरांक uint8_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        जर(delta>=kMinTwoByteDeltaLead) {
            जर(delta<kMinThreeByteDeltaLead) {
                ++pos;
            } अन्यथा जर(delta<kFourByteDeltaLead) {
                pos+=2;
            } अन्यथा {
                pos+=3+(delta&1);
            }
        }
        परत pos;
    }

    स्थिर ओळीत UStringTrieResult मूल्य_Result(int32_t गाठ) {
        परत static_cast<UStringTrieResult>(USTRINGTRIE_INTERMEDIATE_मूल्य - (गाठ & k_मूल्य_Is_Final));
    }

    // Handles a branch node for both next(byte) and next(string).
    UStringTrieResult branchNext(स्थिरांक uint8_t *pos, int32_t length, int32_t inByte);

    // Requires remainingLength_<0.
    UStringTrieResult nextImpl(स्थिरांक uint8_t *pos, int32_t inByte);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    स्थिर स्थिरांक uint8_t* find_Unique_मूल्य_From_Branch(स्थिरांक uint8_t* pos U_LIFETIME_BOUND,
                                                    int32_t length,
                                                    UBool have_Unique_मूल्य,
                                                    int32_t& unique_मूल्य);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead byte.
    स्थिर UBool find_Unique_मूल्य(स्थिरांक uint8_t *pos, UBool have_Unique_मूल्य, int32_t &unique_मूल्य);

    // Helper functions for getNextBytes().
    // getNextBytes() when pos is on a branch node.
    स्थिर शून्य getNextBranchBytes(स्थिरांक uint8_t *pos, int32_t length, ByteSink &out);
    स्थिर शून्य append(ByteSink &out, पूर्णांक c);

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
    स्थिर स्थिरांक int32_t k_Max_Branch_Linear_Sub_गाठ_Length=5;

    // 10..1f: Linear-match node, match 1..16 bytes and continue reading the next node.
    स्थिर स्थिरांक int32_t kMinLinearMatch=0x10;
    स्थिर स्थिरांक int32_t kMaxLinearMatchLength=0x10;

    // 20..ff: Variable-length value node.
    // If odd, the value is final. (Otherwise, intermediate value or jump delta.)
    // Then shift-right by 1 bit.
    // The remaining lead byte value indicates the number of following bytes (0..4)
    // and contains the value's top bits.
    स्थिर स्थिरांक int32_t k_Min_मूल्य_Lead=kMinLinearMatch+kMaxLinearMatchLength;  // 0x20
    // It is a final value if bit 0 is set.
    स्थिर स्थिरांक int32_t k_मूल्य_Is_Final=1;

    // Compact value: After testing bit 0, shift right by 1 and then use the following thresholds.
    स्थिर स्थिरांक int32_t k_Min_One_Byte_मूल्य_Lead=k_Min_मूल्य_Lead/2;  // 0x10
    स्थिर स्थिरांक int32_t k_Max_One_Byte_मूल्य=0x40;  // At least 6 bits in the first byte.

    स्थिर स्थिरांक int32_t k_Min_Two_Byte_मूल्य_Lead=k_Min_One_Byte_मूल्य_Lead+k_Max_One_Byte_मूल्य+1;  // 0x51
    स्थिर स्थिरांक int32_t k_Max_Two_Byte_मूल्य=0x1aff;

    स्थिर स्थिरांक int32_t k_Min_Three_Byte_मूल्य_Lead=k_Min_Two_Byte_मूल्य_Lead+(k_Max_Two_Byte_मूल्य>>8)+1;  // 0x6c
    स्थिर स्थिरांक int32_t k_Four_Byte_मूल्य_Lead=0x7e;

    // A little more than Unicode code points. (0x11ffff)
    स्थिर स्थिरांक int32_t k_Max_Three_Byte_मूल्य=((k_Four_Byte_मूल्य_Lead-k_Min_Three_Byte_मूल्य_Lead)<<16)-1;

    स्थिर स्थिरांक int32_t k_Five_Byte_मूल्य_Lead=0x7f;

    // Compact delta integers.
    स्थिर स्थिरांक int32_t kMaxOneByteDelta=0xbf;
    स्थिर स्थिरांक int32_t kMinTwoByteDeltaLead=kMaxOneByteDelta+1;  // 0xc0
    स्थिर स्थिरांक int32_t kMinThreeByteDeltaLead=0xf0;
    स्थिर स्थिरांक int32_t kFourByteDeltaLead=0xfe;
    स्थिर स्थिरांक int32_t kFiveByteDeltaLead=0xff;

    स्थिर स्थिरांक int32_t kMaxTwoByteDelta=((kMinThreeByteDeltaLead-kMinTwoByteDeltaLead)<<8)-1;  // 0x2fff
    स्थिर स्थिरांक int32_t kMaxThreeByteDelta=((kFourByteDeltaLead-kMinThreeByteDeltaLead)<<16)-1;  // 0xdffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    स्थिर constexpr int32_t kState64RemainingShift = 59;
    स्थिर constexpr uint64_t kState64PosMask = (UINT64_C(1) << kState64RemainingShift) - 1;

    uint8_t *ownedArray_;

    // Fixed value referencing the BytesTrie bytes.
    स्थिरांक uint8_t *bytes_;

    // Iterator variables.

    // Pointer to next trie byte to read. nullptr if no more matches.
    स्थिरांक uint8_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t remainingMatchLength_;
};

U_NAMESPACE_END

#endif /* U_SHOW_CPLUSPLUS_API */

#endif  // __BYTESTRIE_H__
