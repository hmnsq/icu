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

#यदि_अपरिभाषितम् __BYTESTRIE_H__
#परिभाषय __BYTESTRIE_H__

/**
 * \file
 * \brief C++ API: Trie for mapping byte sequences to integer values.
 */

#अन्तर्भाव "unicode/utypes.h"

#यदि U_SHOW_CPLUSPLUS_API

#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ustringtrie.h"

class अष्टकानि_Trie_Test;

U_NAMESPACE_BEGIN

class ByteSink;
class अष्टकानि_Trie_Builder;
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
class U_COMMON_API अष्टकानि_Trie : सार्वजनिक UMemory {
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
    अष्टकानि_Trie(ध्रुव शून्य* trie_अष्टकानि U_LIFETIME_BOUND)
            : ownedArray_(nullptr), अष्टकानि_2(static_cast<ध्रुव uint8_t *>(trie_अष्टकानि)),
              pos_(अष्टकानि_2), remaining_मेलनम्_दीर्घता(-1) {}

    /**
     * Destructor.
     * @stable ICU 4.8
     */
    ~अष्टकानि_Trie();

    /**
     * Copy constructor, copies the other trie reader object and its state,
     * but not the byte array which will be shared. (Shallow copy.)
     * @param other Another BytesTrie object.
     * @stable ICU 4.8
     */
    अष्टकानि_Trie(ध्रुव अष्टकानि_Trie &other)
            : ownedArray_(nullptr), अष्टकानि_2(other.अष्टकानि_2),
              pos_(other.pos_), remaining_मेलनम्_दीर्घता(other.remaining_मेलनम्_दीर्घता) {}

    /**
     * Resets this trie to its initial state.
     * @return *this
     * @stable ICU 4.8
     */
    अष्टकानि_Trie& reset() U_LIFETIME_BOUND {
        pos_=अष्टकानि_2;
        remaining_मेलनम्_दीर्घता=-1;
        प्रत्यागम *this;
    }

    /**
     * Returns the state of this trie as a 64-bit integer.
     * The state value is never 0.
     *
     * @return opaque state value
     * @see resetToState64
     * @stable ICU 65
     */
    uint64_t getState64() ध्रुव {
        प्रत्यागम (static_cast<uint64_t>(remaining_मेलनम्_दीर्घता + 2) << k_State64_Remaining_सरणम्) |
            static_cast<uint64_t>(pos_ - अष्टकानि_2);
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
    अष्टकानि_Trie& reset_गन्तव्यम्_State64(uint64_t state) U_LIFETIME_BOUND {
        remaining_मेलनम्_दीर्घता = static_cast<int32_t>(state >> k_State64_Remaining_सरणम्) - 2;
        pos_ = अष्टकानि_2 + (state & k_State64_Pos_आवरणम्);
        प्रत्यागम *this;
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
        State() { अष्टकानि=nullptr; }
    गुप्त:
        मित्रम् class अष्टकानि_Trie;

        ध्रुव uint8_t *अष्टकानि;
        ध्रुव uint8_t *pos;
        int32_t remaining_मेलनम्_दीर्घता_2;
    };

    /**
     * Saves the state of this trie.
     * @param state The State object to hold the trie's state.
     * @return *this
     * @see resetToState
     * @stable ICU 4.8
     */
    ध्रुव अष्टकानि_Trie& saveState(State& state) ध्रुव U_LIFETIME_BOUND {
        state.अष्टकानि=अष्टकानि_2;
        state.pos=pos_;
        state.remaining_मेलनम्_दीर्घता_2=remaining_मेलनम्_दीर्घता;
        प्रत्यागम *this;
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
    अष्टकानि_Trie& reset_गन्तव्यम्_State(ध्रुव State& state) U_LIFETIME_BOUND {
        यदि(अष्टकानि_2==state.अष्टकानि && अष्टकानि_2!=nullptr) {
            pos_=state.pos;
            remaining_मेलनम्_दीर्घता=state.remaining_मेलनम्_दीर्घता_2;
        }
        प्रत्यागम *this;
    }

    /**
     * Determines whether the byte sequence so far matches, whether it has a value,
     * and whether another input byte can continue a matching byte sequence.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_परिणाम current() ध्रुव;

    /**
     * Traverses the trie from the initial state for this input byte.
     * Equivalent to reset().next(inByte).
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    अन्तःस्थ UString_Trie_परिणाम first(int32_t अन्तर्_Byte) {
        remaining_मेलनम्_दीर्घता=-1;
        यदि(अन्तर्_Byte<0) {
            अन्तर्_Byte+=0x100;
        }
        प्रत्यागम अग्र_Impl(अष्टकानि_2, अन्तर्_Byte);
    }

    /**
     * Traverses the trie from the current state for this input byte.
     * @param inByte Input byte value. Values -0x100..-1 are treated like 0..0xff.
     *               Values below -0x100 and above 0xff will never match.
     * @return The match/value Result.
     * @stable ICU 4.8
     */
    UString_Trie_परिणाम अग्र(int32_t अन्तर्_Byte);

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
    UString_Trie_परिणाम अग्र(ध्रुव वर्ण *s, int32_t दीर्घता);

    /**
     * Returns a matching byte sequence's value if called immediately after
     * current()/first()/next() returned USTRINGTRIE_INTERMEDIATE_VALUE or USTRINGTRIE_FINAL_VALUE.
     * getValue() can be called multiple times.
     *
     * Do not call getValue() after USTRINGTRIE_NO_MATCH or USTRINGTRIE_NO_VALUE!
     * @return The value for the byte sequence so far.
     * @stable ICU 4.8
     */
    अन्तःस्थ int32_t get_मानम्() ध्रुव {
        ध्रुव uint8_t *pos=pos_;
        int32_t leadByte=*pos++;
        // U_ASSERT(leadByte>=kMinValueLead);
        प्रत्यागम read_मानम्(pos, leadByte>>1);
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
    अन्तःस्थ UBool has_Unique_मानम्(int32_t &unique_मानम्) ध्रुव {
        ध्रुव uint8_t *pos=pos_;
        // Skip the rest of a pending linear-match node.
        प्रत्यागम pos!=nullptr && find_Unique_मानम्(pos+remaining_मेलनम्_दीर्घता+1, false, unique_मानम्);
    }

    /**
     * Finds each byte which continues the byte sequence from the current state.
     * That is, each byte b for which it would be next(b)!=USTRINGTRIE_NO_MATCH now.
     * @param out Each next byte is appended to this object.
     *            (Only uses the out.Append(s, length) method.)
     * @return the number of bytes which continue the byte sequence from here
     * @stable ICU 4.8
     */
    int32_t get_अग्र_अष्टकानि(ByteSink &बहिर्गमनम्) ध्रुव;

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
        Iterator(ध्रुव शून्य* trie_अष्टकानि U_LIFETIME_BOUND,
                 int32_t max_String_दीर्घता,
                 UError_संहिता& error_संहिता);

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
        Iterator(ध्रुव अष्टकानि_Trie &trie, int32_t max_String_दीर्घता, UError_संहिता &error_संहिता);

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
        UBool has_अग्र() ध्रुव;

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
        UBool अग्र(UError_संहिता &error_संहिता);

        /**
         * @return The NUL-terminated byte sequence for the last successful next().
         * @stable ICU 4.8
         */
        StringPiece getString() ध्रुव U_LIFETIME_BOUND;
        /**
         * @return The value for the last successful next().
         * @stable ICU 4.8
         */
        int32_t get_मानम्() ध्रुव { प्रत्यागम मानम्; }

    गुप्त:
        UBool truncateAndStop();

        ध्रुव uint8_t* branch_अग्र(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                  int32_t दीर्घता,
                                  UError_संहिता& error_संहिता);

        ध्रुव uint8_t *अष्टकानि_2;
        ध्रुव uint8_t *pos_;
        ध्रुव uint8_t *initialPos_;
        int32_t remaining_मेलनम्_दीर्घता;
        int32_t initial_Remaining_मेलनम्_दीर्घता;

        CharString *str_;
        int32_t max_दीर्घता;
        int32_t मानम्;

        // The stack stores pairs of integers for backtracking to another
        // outbound edge of a branch node.
        // The first integer is an offset from bytes_.
        // The second integer has the str_->length() from before the node in bits 15..0,
        // and the remaining branch length in bits 24..16. (Bits 31..25 are unused.)
        // (We could store the remaining branch length minus 1 in bits 23..16 and not use bits 31..24,
        // but the code looks more confusing that way.)
        UVector32 *राशिः;
    };

गुप्त:
    मित्रम् class अष्टकानि_Trie_Builder;
    मित्रम् class ::अष्टकानि_Trie_Test;

    /**
     * Constructs a BytesTrie reader instance.
     * Unlike the public constructor which just aliases an array,
     * this constructor adopts the builder's array.
     * This constructor is only called by the builder.
     */
    अष्टकानि_Trie(शून्य *adopt_अष्टकानि, ध्रुव शून्य *trie_अष्टकानि)
            : ownedArray_(static_cast<uint8_t *>(adopt_अष्टकानि)),
              अष्टकानि_2(static_cast<ध्रुव uint8_t *>(trie_अष्टकानि)),
              pos_(अष्टकानि_2), remaining_मेलनम्_दीर्घता(-1) {}

    // No assignment operator.
    अष्टकानि_Trie &संक्रियकः=(ध्रुव अष्टकानि_Trie &other) = delete;

    अन्तःस्थ शून्य stop() {
        pos_=nullptr;
    }

    // Reads a compact 32-bit integer.
    // pos is already after the leadByte, and the lead byte is already shifted right by 1.
    स्थिर int32_t read_मानम्(ध्रुव uint8_t *pos, int32_t leadByte);
    स्थिर अन्तःस्थ ध्रुव uint8_t* skip_मानम्(ध्रुव uint8_t* pos U_LIFETIME_BOUND, int32_t leadByte) {
        // U_ASSERT(leadByte>=kMinValueLead);
        यदि(leadByte>=(k_Min_Two_Byte_मानम्_Lead<<1)) {
            यदि(leadByte<(k_Min_Three_Byte_मानम्_Lead<<1)) {
                ++pos;
            } अन्यथा यदि(leadByte<(k_Four_Byte_मानम्_Lead<<1)) {
                pos+=2;
            } अन्यथा {
                pos+=3+((leadByte>>1)&1);
            }
        }
        प्रत्यागम pos;
    }
    स्थिर अन्तःस्थ ध्रुव uint8_t* skip_मानम्(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
        int32_t leadByte=*pos++;
        प्रत्यागम skip_मानम्(pos, leadByte);
    }

    // Reads a jump delta and jumps.
    स्थिर ध्रुव uint8_t* jumpByDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND);

    स्थिर अन्तःस्थ ध्रुव uint8_t* skipDelta(ध्रुव uint8_t* pos U_LIFETIME_BOUND) {
        int32_t delta=*pos++;
        यदि(delta>=kMinTwoByteDeltaLead) {
            यदि(delta<kMinThreeByteDeltaLead) {
                ++pos;
            } अन्यथा यदि(delta<kFourByteDeltaLead) {
                pos+=2;
            } अन्यथा {
                pos+=3+(delta&1);
            }
        }
        प्रत्यागम pos;
    }

    स्थिर अन्तःस्थ UString_Trie_परिणाम मानम्_परिणाम(int32_t ग्रन्थिः) {
        प्रत्यागम static_cast<UString_Trie_परिणाम>(USTRINGTRIE_INTERMEDIATE_मानम् - (ग्रन्थिः & k_मानम्_Is_Final));
    }

    // Handles a branch node for both next(byte) and next(string).
    UString_Trie_परिणाम branch_अग्र(ध्रुव uint8_t *pos, int32_t दीर्घता, int32_t अन्तर्_Byte);

    // Requires remainingLength_<0.
    UString_Trie_परिणाम अग्र_Impl(ध्रुव uint8_t *pos, int32_t अन्तर्_Byte);

    // Helper functions for hasUniqueValue().
    // Recursively finds a unique value (or whether there is not a unique one)
    // from a branch.
    स्थिर ध्रुव uint8_t* find_Unique_मानम्_From_Branch(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                                    int32_t दीर्घता,
                                                    UBool have_Unique_मानम्,
                                                    int32_t& unique_मानम्);
    // Recursively finds a unique value (or whether there is not a unique one)
    // starting from a position on a node lead byte.
    स्थिर UBool find_Unique_मानम्(ध्रुव uint8_t *pos, UBool have_Unique_मानम्, int32_t &unique_मानम्);

    // Helper functions for getNextBytes().
    // getNextBytes() when pos is on a branch node.
    स्थिर शून्य get_अग्र_Branch_अष्टकानि(ध्रुव uint8_t *pos, int32_t दीर्घता, ByteSink &बहिर्गमनम्);
    स्थिर शून्य append(ByteSink &बहिर्गमनम्, पूर्णाङ्क c);

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
    स्थिर ध्रुव int32_t k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता=5;

    // 10..1f: Linear-match node, match 1..16 bytes and continue reading the next node.
    स्थिर ध्रुव int32_t k_Min_रैखिकम्_मेलनम्=0x10;
    स्थिर ध्रुव int32_t k_Max_रैखिकम्_मेलनम्_दीर्घता=0x10;

    // 20..ff: Variable-length value node.
    // If odd, the value is final. (Otherwise, intermediate value or jump delta.)
    // Then shift-right by 1 bit.
    // The remaining lead byte value indicates the number of following bytes (0..4)
    // and contains the value's top bits.
    स्थिर ध्रुव int32_t k_Min_मानम्_Lead=k_Min_रैखिकम्_मेलनम्+k_Max_रैखिकम्_मेलनम्_दीर्घता;  // 0x20
    // It is a final value if bit 0 is set.
    स्थिर ध्रुव int32_t k_मानम्_Is_Final=1;

    // Compact value: After testing bit 0, shift right by 1 and then use the following thresholds.
    स्थिर ध्रुव int32_t k_Min_One_Byte_मानम्_Lead=k_Min_मानम्_Lead/2;  // 0x10
    स्थिर ध्रुव int32_t k_Max_One_Byte_मानम्=0x40;  // At least 6 bits in the first byte.

    स्थिर ध्रुव int32_t k_Min_Two_Byte_मानम्_Lead=k_Min_One_Byte_मानम्_Lead+k_Max_One_Byte_मानम्+1;  // 0x51
    स्थिर ध्रुव int32_t k_Max_Two_Byte_मानम्=0x1aff;

    स्थिर ध्रुव int32_t k_Min_Three_Byte_मानम्_Lead=k_Min_Two_Byte_मानम्_Lead+(k_Max_Two_Byte_मानम्>>8)+1;  // 0x6c
    स्थिर ध्रुव int32_t k_Four_Byte_मानम्_Lead=0x7e;

    // A little more than Unicode code points. (0x11ffff)
    स्थिर ध्रुव int32_t k_Max_Three_Byte_मानम्=((k_Four_Byte_मानम्_Lead-k_Min_Three_Byte_मानम्_Lead)<<16)-1;

    स्थिर ध्रुव int32_t k_Five_Byte_मानम्_Lead=0x7f;

    // Compact delta integers.
    स्थिर ध्रुव int32_t kMaxOneByteDelta=0xbf;
    स्थिर ध्रुव int32_t kMinTwoByteDeltaLead=kMaxOneByteDelta+1;  // 0xc0
    स्थिर ध्रुव int32_t kMinThreeByteDeltaLead=0xf0;
    स्थिर ध्रुव int32_t kFourByteDeltaLead=0xfe;
    स्थिर ध्रुव int32_t kFiveByteDeltaLead=0xff;

    स्थिर ध्रुव int32_t kMaxTwoByteDelta=((kMinThreeByteDeltaLead-kMinTwoByteDeltaLead)<<8)-1;  // 0x2fff
    स्थिर ध्रुव int32_t kMaxThreeByteDelta=((kFourByteDeltaLead-kMinThreeByteDeltaLead)<<16)-1;  // 0xdffff

    // For getState64():
    // The remainingMatchLength_ is -1..14=(kMaxLinearMatchLength=0x10)-2
    // so we need at least 5 bits for that.
    // We add 2 to store it as a positive value 1..16=kMaxLinearMatchLength.
    स्थिर constexpr int32_t k_State64_Remaining_सरणम् = 59;
    स्थिर constexpr uint64_t k_State64_Pos_आवरणम् = (UINT64_C(1) << k_State64_Remaining_सरणम्) - 1;

    uint8_t *ownedArray_;

    // Fixed value referencing the BytesTrie bytes.
    ध्रुव uint8_t *अष्टकानि_2;

    // Iterator variables.

    // Pointer to next trie byte to read. nullptr if no more matches.
    ध्रुव uint8_t *pos_;
    // Remaining length of a linear-match node, minus 1. Negative if not in such a node.
    int32_t remaining_मेलनम्_दीर्घता;
};

U_NAMESPACE_अन्तः

#यदि_अन्तः /* U_SHOW_CPLUSPLUS_API */

#यदि_अन्तः  // __BYTESTRIE_H__
