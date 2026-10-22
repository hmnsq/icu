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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __BYTESTRIE_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __BYTESTRIE_H__

/**
 * \file
 * \brief C++ API: Trie for mapping byte sequences to integer values.
 */

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ U_SHOW_CPLUSPLUS_API

#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustringtrie.h"

class ಬೈಟುಗಳು_Trie_Test;

U_NAMESPACE_BEGIN

class ByteSink;
class ಬೈಟುಗಳು_Trie_Builder;
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
class U_COMMON_API ಬೈಟುಗಳು_Trie : ಸಾರ್ವಜನಿಕ UMemory {
ಸಾರ್ವಜನಿಕ:
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
    ಬೈಟುಗಳು_Trie(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* trie_ಬೈಟುಗಳು U_LIFETIME_BOUND)
            : ownedArray_(nullptr), ಬೈಟುಗಳು_2(static_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(trie_ಬೈಟುಗಳು)),
              pos_(ಬೈಟುಗಳು_2), ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1) {}

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ~ಬೈಟುಗಳು_Trie();

    /**
     * Copy constructor, copies the other trie reader object and its state,
     * but not the byte array which will be shared. (Shallow copy.)
     * @param other Another BytesTrie object.
     * @stable ICU 4.8
     */
    ಬೈಟುಗಳು_Trie(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie &other)
            : ownedArray_(nullptr), ಬೈಟುಗಳು_2(other.ಬೈಟುಗಳು_2),
              pos_(other.pos_), ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(other.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ) {}

    /**
     * Resets this trie to its initial state.
     * @return *this
     * @stable ICU 4.8
     */
    ಬೈಟುಗಳು_Trie& reset() U_LIFETIME_BOUND {
        pos_=ಬೈಟುಗಳು_2;
        ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=-1;
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Returns the state of this trie as a 64-bit integer.
     * The state value is never 0.
     *
     * @return opaque state value
     * @see resetToState64
     * @stable ICU 65
     */
    uint64_t getState64() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ (static_cast<uint64_t>(ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ + 2) << k_State64_ಉಳಿದ_ಸರಿಸು) |
            static_cast<uint64_t>(pos_ - ಬೈಟುಗಳು_2);
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
    ಬೈಟುಗಳು_Trie& reset_ಗಮ್ಯ_State64(uint64_t state) U_LIFETIME_BOUND {
        ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ = static_cast<int32_t>(state >> k_State64_ಉಳಿದ_ಸರಿಸು) - 2;
        pos_ = ಬೈಟುಗಳು_2 + (state & k_State64_Pos_ಮುಖವಾಡ);
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * BytesTrie state object, for saving a trie's current state
     * and resetting the trie back to this state later.
     * @stable ICU 4.8
     */
    class State : ಸಾರ್ವಜನಿಕ UMemory {
    ಸಾರ್ವಜನಿಕ:
        /**
         * Constructs an empty State.
         * @stable ICU 4.8
         */
        State() { ಬೈಟುಗಳು=nullptr; }
    ಖಾಸಗಿ:
        ಮಿತ್ರ class ಬೈಟುಗಳು_Trie;

        ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು;
        ಸ್ಥಿರಾಂಕ uint8_t *pos;
        int32_t ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ_2;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie& saveState(State& state) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
        state.ಬೈಟುಗಳು=ಬೈಟುಗಳು_2;
        state.pos=pos_;
        state.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ_2=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
        ಹಿಂತಿರುಗಿ *this;
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
    ಬೈಟುಗಳು_Trie& reset_ಗಮ್ಯ_State(ಸ್ಥಿರಾಂಕ State& state) U_LIFETIME_BOUND {
        ಆದರೆ(ಬೈಟುಗಳು_2==state.ಬೈಟುಗಳು && ಬೈಟುಗಳು_2!=nullptr) {
            pos_=state.pos;
            ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=state.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ_2;
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    /**
     * Determines whether the byte sequence so far matches, whether it has a value,
     * and whether another input byte can continue a matching byte sequence.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ಫಲಿತಾಂಶ current() ಸ್ಥಿರಾಂಕ;

    /**
     * Traverses the trie from the initial state for this input byte.
     * Equivalent to reset().next(inByte).
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    ಸಾಲೊಳಗೆ UString_Trie_ಫಲಿತಾಂಶ ಮೊದಲ(int32_t ಒಳ_Byte) {
        ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=-1;
        ಆದರೆ(ಒಳ_Byte<0) {
            ಒಳ_Byte+=0x100;
        }
        ಹಿಂತಿರುಗಿ ಮುಂದಿನ_Impl(ಬೈಟುಗಳು_2, ಒಳ_Byte);
    }

    /**
     * Traverses the trie from the current state for this input byte.
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_ಫಲಿತಾಂಶ ಮುಂದಿನ(int32_t ಒಳ_Byte);

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
    UString_Trie_ಫಲಿತಾಂಶ ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t ಉದ್ದಳತೆ);

    /**
     * Returns a matching byte sequence's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the byte sequence so far.
     * @stable ICU 4.8
     */
    ಸಾಲೊಳಗೆ int32_t get_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ {
        ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
        int32_t leadByte=*pos++;
        // U_ASSERT(leadByte>=kMinValueLead);
        ಹಿಂತಿರುಗಿ read_ಮೌಲ್ಯ(pos, leadByte>>1);
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
    ಸಾಲೊಳಗೆ UBool has_Unique_ಮೌಲ್ಯ(int32_t &unique_ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
        ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        ಹಿಂತಿರುಗಿ pos!=nullptr && ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(pos+ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ+1, false, unique_ಮೌಲ್ಯ);
    }

    /**
     * Finds each byte which continues the byte sequence from the current state.
     * That is, each byte b for which it would be next(b)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next byte is appended to this object.
     *            (Only uses the out.Append(s, length) method.)
     * @return the number of bytes which continue the byte sequence from here
     * @stable ICU 4.8
     */
    int32_t get_ಮುಂದಿನ_ಬೈಟುಗಳು(ByteSink &ಹೊರಹೋಗು) ಸ್ಥಿರಾಂಕ;

    /**
     * Iterator for all of the (byte sequence, value) pairs in a BytesTrie.
     * @stable ICU 4.8
     */
    class U_COMMON_API Iterator : ಸಾರ್ವಜನಿಕ UMemory {
    ಸಾರ್ವಜನಿಕ:
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
        Iterator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* trie_ಬೈಟುಗಳು U_LIFETIME_BOUND,
                 int32_t max_String_ಉದ್ದಳತೆ,
                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ);

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
        Iterator(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie &trie, int32_t max_String_ಉದ್ದಳತೆ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

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
        UBool has_ಮುಂದಿನ() ಸ್ಥಿರಾಂಕ;

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
        UBool ಮುಂದಿನ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ);

        /**
         * @return The NUL-terminated byte sequence for the last successful next().
         * @stable ICU 4.8
         */
        StringPiece getString() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND;
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ; }

    ಖಾಸಗಿ:
        UBool truncateAndStop();

        ಸ್ಥಿರಾಂಕ uint8_t* branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND,
                                  int32_t ಉದ್ದಳತೆ,
                                  UError_ಸಂಕೇತ& error_ಸಂಕೇತ);

        ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು_2;
        ಸ್ಥಿರಾಂಕ uint8_t *pos_;
        ಸ್ಥಿರಾಂಕ uint8_t *initialPos_;
        int32_t ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
        int32_t initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;

        CharString *str_;
        int32_t max_ಉದ್ದಳತೆ;
        int32_t ಮೌಲ್ಯ;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from bytes_.
        // The second integer has the str_->length() from before the node in bits 15..0,
        // and the remaining branch length in bits 24..16. (Bits 31..25 are unused.)
        // (We could store the remaining branch length minus 1 in bits 23..16 and not use bits 31..24,
        // but the code looks more confusing that way.)
        UVector32 *ರಾಶಿ;
    };

ಖಾಸಗಿ:
    ಮಿತ್ರ class ಬೈಟುಗಳು_Trie_Builder;
    ಮಿತ್ರ class ::ಬೈಟುಗಳು_Trie_Test;

    /**
     * Constructs a BytesTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    ಬೈಟುಗಳು_Trie(ಶೂನ್ಯ *adopt_ಬೈಟುಗಳು, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *trie_ಬೈಟುಗಳು)
            : ownedArray_(static_cast<uint8_t *>(adopt_ಬೈಟುಗಳು)),
              ಬೈಟುಗಳು_2(static_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(trie_ಬೈಟುಗಳು)),
              pos_(ಬೈಟುಗಳು_2), ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1) {}

    // No assignment operator.
    ಬೈಟುಗಳು_Trie &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie &other) = delete;

    ಸಾಲೊಳಗೆ ಶೂನ್ಯ stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadByte, and the lead byte is already shifted right by 1.
    ಸ್ಥಿರ int32_t read_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t leadByte);
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ uint8_t* skip_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND, int32_t leadByte) {
        // U_ASSERT(leadByte>=kMinValueLead);
        ಆದರೆ(leadByte>=(k_Min_Two_Byte_ಮೌಲ್ಯ_Lead<<1)) {
            ಆದರೆ(leadByte<(k_Min_Three_Byte_ಮೌಲ್ಯ_Lead<<1)) {
                ++pos;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(leadByte<(k_Four_Byte_ಮೌಲ್ಯ_Lead<<1)) {
                pos+=2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                pos+=3+((leadByte>>1)&1);
            }
        }
        ಹಿಂತಿರುಗಿ pos;
    }
    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ uint8_t* skip_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND) {
        int32_t leadByte=*pos++;
        ಹಿಂತಿರುಗಿ skip_ಮೌಲ್ಯ(pos, leadByte);
    }

    // Reads a jump delta and jumps.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t* jumpByDelta(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND);

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ uint8_t* skipDelta(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        ಆದರೆ(delta>=kMinTwoByteDeltaLead) {
            ಆದರೆ(delta<kMinThreeByteDeltaLead) {
                ++pos;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(delta<kFourByteDeltaLead) {
                pos+=2;
            } ಇಲ್ಲದಿದ್ದರೆ {
                pos+=3+(delta&1);
            }
        }
        ಹಿಂತಿರುಗಿ pos;
    }

    ಸ್ಥಿರ ಸಾಲೊಳಗೆ UString_Trie_ಫಲಿತಾಂಶ ಮೌಲ್ಯ_ಫಲಿತಾಂಶ(int32_t ಗಂಟು) {
        ಹಿಂತಿರುಗಿ static_cast<UString_Trie_ಫಲಿತಾಂಶ>(USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ - (ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final));
    }

    // Handles a branch node for both next(byte) and next(string).
    UString_Trie_ಫಲಿತಾಂಶ branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t ಉದ್ದಳತೆ, int32_t ಒಳ_Byte);

    // Requires remainingLength_<0.
    UString_Trie_ಫಲಿತಾಂಶ ಮುಂದಿನ_Impl(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t ಒಳ_Byte);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t* ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ_From_Branch(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND,
                                                    int32_t ಉದ್ದಳತೆ,
                                                    UBool have_Unique_ಮೌಲ್ಯ,
                                                    int32_t& unique_ಮೌಲ್ಯ);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead byte.
    ಸ್ಥಿರ UBool ಕಂಡುಹಿಡಿ_Unique_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ uint8_t *pos, UBool have_Unique_ಮೌಲ್ಯ, int32_t &unique_ಮೌಲ್ಯ);

    // Helper functions for getNextBytes().
    // getNextBytes() when pos is on a branch node.
    ಸ್ಥಿರ ಶೂನ್ಯ get_ಮುಂದಿನ_Branch_ಬೈಟುಗಳು(ಸ್ಥಿರಾಂಕ uint8_t *pos, int32_t ಉದ್ದಳತೆ, ByteSink &ಹೊರಹೋಗು);
    ಸ್ಥಿರ ಶೂನ್ಯ append(ByteSink &ಹೊರಹೋಗು, ಪೂರ್ಣಾಂಕ c);

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
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ=5;

    // 10..1f: Linear-match node, match 1..16 bytes and continue reading the next node.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ=0x10;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=0x10;

    // 20..ff: Variable-length value node.
    // If odd, the value is final. (Otherwise, intermediate value or jump delta.)
    // Then shift-right by 1 bit.
    // The remaining lead byte value indicates the number of following bytes (0..4)
    // and contains the value's top bits.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Min_ಮೌಲ್ಯ_Lead=k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ+k_Max_ರೇಖೀಯ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // 0x20
    // It is a final value if bit 0 is set.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_ಮೌಲ್ಯ_Is_Final=1;

    // Compact value: After testing bit 0, shift right by 1 and then use the following thresholds.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Min_One_Byte_ಮೌಲ್ಯ_Lead=k_Min_ಮೌಲ್ಯ_Lead/2;  // 0x10
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_One_Byte_ಮೌಲ್ಯ=0x40;  // At least 6 bits in the first byte.

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Min_Two_Byte_ಮೌಲ್ಯ_Lead=k_Min_One_Byte_ಮೌಲ್ಯ_Lead+k_Max_One_Byte_ಮೌಲ್ಯ+1;  // 0x51
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Two_Byte_ಮೌಲ್ಯ=0x1aff;

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Min_Three_Byte_ಮೌಲ್ಯ_Lead=k_Min_Two_Byte_ಮೌಲ್ಯ_Lead+(k_Max_Two_Byte_ಮೌಲ್ಯ>>8)+1;  // 0x6c
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Four_Byte_ಮೌಲ್ಯ_Lead=0x7e;

    // A little more than Unicode code points. (0x11ffff)
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Max_Three_Byte_ಮೌಲ್ಯ=((k_Four_Byte_ಮೌಲ್ಯ_Lead-k_Min_Three_Byte_ಮೌಲ್ಯ_Lead)<<16)-1;

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t k_Five_Byte_ಮೌಲ್ಯ_Lead=0x7f;

    // Compact delta integers.
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kMaxOneByteDelta=0xbf;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kMinTwoByteDeltaLead=kMaxOneByteDelta+1;  // 0xc0
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kMinThreeByteDeltaLead=0xf0;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kFourByteDeltaLead=0xfe;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kFiveByteDeltaLead=0xff;

    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kMaxTwoByteDelta=((kMinThreeByteDeltaLead-kMinTwoByteDeltaLead)<<8)-1;  // 0x2fff
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int32_t kMaxThreeByteDelta=((kFourByteDeltaLead-kMinThreeByteDeltaLead)<<16)-1;  // 0xdffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    ಸ್ಥಿರ constexpr int32_t k_State64_ಉಳಿದ_ಸರಿಸು = 59;
    ಸ್ಥಿರ constexpr uint64_t k_State64_Pos_ಮುಖವಾಡ = (UINT64_C(1) << k_State64_ಉಳಿದ_ಸರಿಸು) - 1;

    uint8_t *ownedArray_;

    // Fixed value referencing the BytesTrie bytes.
    ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು_2;

    // Iterator variables.

    // Pointer to next trie byte to read. nullptr if no more matches.
    ಸ್ಥಿರಾಂಕ uint8_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
};

U_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* U_SHOW_CPLUSPLUS_API */

#ಆದರೆ_ಅಂತ್ಯ  // __BYTESTRIE_H__
