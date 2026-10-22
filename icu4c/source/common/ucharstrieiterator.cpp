// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2010-2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  ucharstrieiterator.h
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010nov15
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucharstrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "uvectr32.h"

U_NAMESPACE_BEGIN

UCharsTrie::Iterator::Iterator(ConstChar16Ptr trieUChars U_LIFETIME_BOUND, int32_t max_String_ಉದ್ದಳತೆ,
                               UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : uchars_(trieUChars),
          pos_(uchars_), initialPos_(uchars_),
          ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1), initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(-1),
          skip_ಮೌಲ್ಯ_2(ಸುಳ್ಳು),
          max_ಉದ್ದಳತೆ(max_String_ಉದ್ದಳತೆ), ಮೌಲ್ಯ(0), ರಾಶಿ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    // stack_ is a pointer so that it's easy to turn ucharstrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike UCharsTrie itself, its Iterator performs memory allocations anyway
    // via the UnicodeString and UVector32 implementations, so this additional
    // cost is minimal.
    ರಾಶಿ=ಹೊಸ UVector32(error_ಸಂಕೇತ);
    ಆದರೆ(ರಾಶಿ==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }
}

UCharsTrie::Iterator::Iterator(ಸ್ಥಿರಾಂಕ UCharsTrie &trie, int32_t max_String_ಉದ್ದಳತೆ,
                               UError_ಸಂಕೇತ &error_ಸಂಕೇತ)
        : uchars_(trie.uchars_), pos_(trie.pos_), initialPos_(trie.pos_),
          ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(trie.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ),
          initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ(trie.ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ),
          skip_ಮೌಲ್ಯ_2(ಸುಳ್ಳು),
          max_ಉದ್ದಳತೆ(max_String_ಉದ್ದಳತೆ), ಮೌಲ್ಯ(0), ರಾಶಿ(ಶೂನ್ಯಸೂಚಕ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ರಾಶಿ=ಹೊಸ UVector32(error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ರಾಶಿ==ಶೂನ್ಯಸೂಚಕ) {
        error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;  // Actual remaining match length minus 1.
    ಆದರೆ(length>=0) {
        // Pending linear-match node, append remaining UChars to str_.
        ++length;
        ಆದರೆ(max_ಉದ್ದಳತೆ>0 && length>max_ಉದ್ದಳತೆ) {
            length=max_ಉದ್ದಳತೆ;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_.append(pos_, length);
        pos_+=length;
        ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ-=length;
    }
}

UCharsTrie::Iterator::~Iterator() {
    ಅಳಿಸು ರಾಶಿ;
}

UCharsTrie::Iterator&
UCharsTrie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ=initial_ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ;
    skip_ಮೌಲ್ಯ_2=ಸುಳ್ಳು;
    int32_t length=ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ+1;  // Remaining match length.
    ಆದರೆ(max_ಉದ್ದಳತೆ>0 && length>max_ಉದ್ದಳತೆ) {
        length=max_ಉದ್ದಳತೆ;
    }
    str_.truncate(length);
    pos_+=length;
    ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ-=length;
    ರಾಶಿ->ಗಣ_ಗಾತ್ರ(0);
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool
UCharsTrie::Iterator::has_ಮುಂದಿನ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ pos_!=ಶೂನ್ಯಸೂಚಕ || !ರಾಶಿ->is_ಖಾಲಿಗಳು(); }

UBool
UCharsTrie::Iterator::next(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಸ್ಥಿರಾಂಕ char16_t *pos=pos_;
    ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(ರಾಶಿ->is_ಖಾಲಿಗಳು()) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t ರಾಶಿ_ಗಾತ್ರ=ರಾಶಿ->size();
        int32_t length=ರಾಶಿ->elementAti(ರಾಶಿ_ಗಾತ್ರ-1);
        pos=uchars_+ರಾಶಿ->elementAti(ರಾಶಿ_ಗಾತ್ರ-2);
        ರಾಶಿ->ಗಣ_ಗಾತ್ರ(ರಾಶಿ_ಗಾತ್ರ-2);
        str_.truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        ಆದರೆ(length>1) {
            pos=branch_ಮುಂದಿನ(pos, length, error_ಸಂಕೇತ);
            ಆದರೆ(pos==ಶೂನ್ಯಸೂಚಕ) {
                ಹಿಂತಿರುಗಿ ನಿಜ;  // Reached a final value.
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            str_.append(*pos++);
        }
    }
    ಆದರೆ(ಉಳಿದ_ಹೊಂದಿಕೆ_ಉದ್ದಳತೆ>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining units.
        ಹಿಂತಿರುಗಿ truncateAndStop();
    }
    ಗಾಗಿ(;;) {
        int32_t ಗಂಟು=*pos++;
        ಆದರೆ(ಗಂಟು>=k_Min_ಮೌಲ್ಯ_Lead) {
            ಆದರೆ(skip_ಮೌಲ್ಯ_2) {
                pos=skip_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                ಗಂಟು&=k_ಗಂಟು_Type_ಮುಖವಾಡ;
                skip_ಮೌಲ್ಯ_2=ಸುಳ್ಳು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                // Deliver value for the string so far.
                UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
                ಆದರೆ(isFinal) {
                    ಮೌಲ್ಯ=read_ಮೌಲ್ಯ(pos, ಗಂಟು&0x7fff);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಮೌಲ್ಯ=read_ಗಂಟು_ಮೌಲ್ಯ(pos, ಗಂಟು);
                }
                ಆದರೆ(isFinal || (max_ಉದ್ದಳತೆ>0 && str_.length()==max_ಉದ್ದಳತೆ)) {
                    pos_=ಶೂನ್ಯಸೂಚಕ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    // We cannot skip the value right here because it shares its
                    // lead unit with a match node which we have to evaluate
                    // next time.
                    // Instead, keep pos_ on the node lead unit itself.
                    pos_=pos-1;
                    skip_ಮೌಲ್ಯ_2=ನಿಜ;
                }
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }
        ಆದರೆ(max_ಉದ್ದಳತೆ>0 && str_.length()==max_ಉದ್ದಳತೆ) {
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
            // Linear-match node, append length units to str_.
            int32_t length=ಗಂಟು-k_Min_ರೇಖೀಯ_ಹೊಂದಿಕೆ+1;
            ಆದರೆ(max_ಉದ್ದಳತೆ>0 && str_.length()+length>max_ಉದ್ದಳತೆ) {
                str_.append(pos, max_ಉದ್ದಳತೆ-str_.length());
                ಹಿಂತಿರುಗಿ truncateAndStop();
            }
            str_.append(pos, length);
            pos+=length;
        }
    }
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ಸ್ಥಿರಾಂಕ char16_t*
UCharsTrie::Iterator::branch_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ char16_t* pos U_LIFETIME_BOUND,
                                 int32_t length,
                                 UError_ಸಂಕೇತ& error_ಸಂಕೇತ) {
    ಆಗ(length>k_Max_Branch_ರೇಖೀಯ_Sub_ಗಂಟು_ಉದ್ದಳತೆ) {
        ++pos;  // ignore the comparison unit
        // Push state for the greater-or-equal edge.
        ರಾಶಿ->ಕೂಡಿಸು_Element(static_cast<int32_t>(skipDelta(pos) - uchars_), error_ಸಂಕೇತ);
        ರಾಶಿ->ಕೂಡಿಸು_Element(((length-(length>>1))<<16)|str_.length(), error_ಸಂಕೇತ);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    char16_t trieUnit=*pos++;
    int32_t ಗಂಟು=*pos++;
    UBool isFinal = static_cast<UBool>(ಗಂಟು >> 15);
    int32_t value=read_ಮೌಲ್ಯ(pos, ಗಂಟು&=0x7fff);
    pos=skip_ಮೌಲ್ಯ(pos, ಗಂಟು);
    ರಾಶಿ->ಕೂಡಿಸು_Element(static_cast<int32_t>(pos - uchars_), error_ಸಂಕೇತ);
    ರಾಶಿ->ಕೂಡಿಸು_Element(((length-1)<<16)|str_.length(), error_ಸಂಕೇತ);
    str_.append(trieUnit);
    ಆದರೆ(isFinal) {
        pos_=ಶೂನ್ಯಸೂಚಕ;
        ಮೌಲ್ಯ=value;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ pos+value;
    }
}

U_NAMESPACE_ಕೊನೆ
