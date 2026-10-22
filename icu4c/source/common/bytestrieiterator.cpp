// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  bytestrieiterator.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov03
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/stringpiece.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

ಬೈಟುಗಳು_Trie::Iterator::Iterator(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ* trie_ಬೈಟುಗಳು U_LIFETIME_BOUND,
                              int32_t max_String_ಉದ್ದಳತೆ,
                              UError_ಸಂಕೇತ& error_ಸಂಕೇತ)
        : ಬೈಟುಗಳು_2(static_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(trie_ಬೈಟುಗಳು)),
          pos_(ಬೈಟುಗಳು_2), initialPos_(ಬೈಟುಗಳು_2),
          ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1), initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1),
          str_(ಶೂನ್ಯಸೂಚಕ), max_ಉದ್ದಳತೆ(max_String_ಉದ್ದಳತೆ), ಮೌಲ್ಯ(0), ರಾಶಿ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=ಹೊಸ CharString();
    ರಾಶಿ=ಹೊಸ UVector32(error_ಸಂಕೇತ);
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ) && (str_==ಶೂನ್ಯಸೂಚಕ || ರಾಶಿ==ಶೂನ್ಯಸೂಚಕ)) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

ಬೈಟುಗಳು_Trie::Iterator::Iterator(ಸ್ಥಿರಾಂಕ ಬೈಟುಗಳು_Trie &trie, int32_t max_String_ಉದ್ದಳತೆ,
                              UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : ಬೈಟುಗಳು_2(trie.ಬೈಟುಗಳು_2), pos_(trie.pos_), initialPos_(trie.pos_),
          ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(trie.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ),
          initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(trie.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ),
          str_(ಶೂನ್ಯಸೂಚಕ), max_ಉದ್ದಳತೆ(max_String_ಉದ್ದಳತೆ), ಮೌಲ್ಯ(0), ರಾಶಿ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    str_=ಹೊಸ CharString();
    ರಾಶಿ=ಹೊಸ UVector32(error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(str_==ಶೂನ್ಯಸೂಚಕ || ರಾಶಿ==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಆದರೆ(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        ಆದರೆ(max_ಉದ್ದಳತೆ>0 && length>max_ಉದ್ದಳತೆ) {
            length=max_ಉದ್ದಳತೆ;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(pos_), length, error_ಸಂಕೇತ);
        pos_+=length;
        ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ-=length;
    }
}

ಬೈಟುಗಳು_Trie::Iterator::~Iterator() {
    ಅಳಿಸು str_;
    ಅಳಿಸು ರಾಶಿ;
}

ಬೈಟುಗಳು_Trie::Iterator&
ಬೈಟುಗಳು_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ+1;  // Remaining match length.
    ಆದರೆ(max_ಉದ್ದಳತೆ>0 && length>max_ಉದ್ದಳತೆ) {
        length=max_ಉದ್ದಳತೆ;
    }
    str_->truncate(length);
    pos_+=length;
    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ-=length;
    ರಾಶಿ->ಗಣ_ಗಾತ್ರ(0);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool
ಬೈಟುಗಳು_Trie::Iterator::has_ಮುಂದಿನ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ pos_!=ಶೂನ್ಯಸೂಚಕ || !ರಾಶಿ->is_ಖಾಲಿಗಳು(); }

UBool
ಬೈಟುಗಳು_Trie::Iterator::next(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ uint8_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(ರಾಶಿ->is_ಖಾಲಿಗಳು()) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ರಾಶಿ_ಗಾತ್ರ=ರಾಶಿ->size();
        int32_t length=ರಾಶಿ->elementAti(ರಾಶಿ_ಗಾತ್ರ-1);
        pos=ಬೈಟುಗಳು_2+ರಾಶಿ->elementAti(ರಾಶಿ_ಗಾತ್ರ-2);
        ರಾಶಿ->ಗಣ_ಗಾತ್ರ(ರಾಶಿ_ಗಾತ್ರ-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ಆದರೆ(length>1) {
            pos=branch_ಮುಂದಿನ(pos, length, error_ಸಂಕೇತ);
            ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ನಿಜ;  // Reached a final value.
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            str_->append(static_cast<ಅಕ್ಷರ>(*pos++), error_ಸಂಕೇತ);
        }
    }
    ಆದರೆ(ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        ಹಿಂತಿರುಗಿ truncateAndStop();
    }
    ಗಾಗಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಆದರೆ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
            ಮೌಲ್ಯ=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
            ಆದರೆ(isFinal || (max_ಉದ್ದಳತೆ>0 && str_->length()==max_ಉದ್ದಳತೆ)) {
                pos_=ಶೂನ್ಯಸೂಚಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                pos_=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
            }
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
        ಆದರೆ(max_ಉದ್ದಳತೆ>0 && str_->length()==max_ಉದ್ದಳತೆ) {
            ಹಿಂತಿರುಗಿ truncateAndStop();
        }
        ಆದರೆ(ಗಂಟು<k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ) {
            ಆದರೆ(ಗಂಟು==0) {
                ಗಂಟು=*pos++;
            }
            pos=branch_ಮುಂದಿನ(pos, ಗಂಟು+1, error_ಸಂಕೇತ);
            ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ನಿಜ;  // Reached a final value.
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            // Linear-match node, append length bytes to str_.
            int32_t length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ+1;
            ಆದರೆ(max_ಉದ್ದಳತೆ>0 && str_->length()+length>max_ಉದ್ದಳತೆ) {
                str_->append(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(pos),
                            max_ಉದ್ದಳತೆ-str_->length(), error_ಸಂಕೇತ);
                ಹಿಂತಿರುಗಿ truncateAndStop();
            }
            str_->append(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(pos), length, error_ಸಂಕೇತ);
            pos+=length;
        }
    }
}

StringPiece
ಬೈಟುಗಳು_Trie::Iterator::getString() ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
    ಹಿಂತಿರುಗಿ str_ == ಶೂನ್ಯಸೂಚಕ ? StringPiece() : str_->ಗಮ್ಯ_String_Piece();
}

UBool
ಬೈಟುಗಳು_Trie::Iterator::truncateAndStop() {
    pos_=ಶೂನ್ಯಸೂಚಕ;
    ಮೌಲ್ಯ=-1;  // no real value for str
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ಸ್ಥಿರಾಂಕ uint8_t*
ಬೈಟುಗಳು_Trie::Iterator::branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_ಸಂಕೇತ& error_ಸಂಕೇತ) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        ರಾಶಿ->ಕೂಡಿಸು_Element(static_cast<int32_t>(skipDelta(pos) - ಬೈಟುಗಳು_2), error_ಸಂಕೇತ);
        ರಾಶಿ->ಕೂಡಿಸು_Element(((length-(length>>1))<<16)|str_->length(), error_ಸಂಕೇತ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ಗಂಟು=*pos++;
    UBool isFinal = static_cast<UBool>(ಗಂಟು & k_ಮೌಲ್ಯ_Is_Final);
    int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು>>1);
    pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
    ರಾಶಿ->ಕೂಡಿಸು_Element(static_cast<int32_t>(pos - ಬೈಟುಗಳು_2), error_ಸಂಕೇತ);
    ರಾಶಿ->ಕೂಡಿಸು_Element(((length-1)<<16)|str_->length(), error_ಸಂಕೇತ);
    str_->append(static_cast<ಅಕ್ಷರ>(trieByte), error_ಸಂಕೇತ);
    ಆದರೆ(isFinal) {
        pos_=ಶೂನ್ಯಸೂಚಕ;
        ಮೌಲ್ಯ=value;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pos+value;
    }
}

U_NAMESPACE_ಕೊನೆ
