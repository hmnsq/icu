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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/stringpiece.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "uvectr32.h"

U_NAMESPACE_BEGIN

अष्टकानि_Trie::Iterator::Iterator(ध्रुव शून्य* trie_अष्टकानि U_LIFETIME_BOUND,
                              int32_t max_String_दीर्घता,
                              UError_संहिता& error_संहिता)
        : अष्टकानि_2(static_cast<ध्रुव uint8_t *>(trie_अष्टकानि)),
          pos_(अष्टकानि_2), initialPos_(अष्टकानि_2),
          remaining_मेलनम्_दीर्घता(-1), initial_Remaining_मेलनम्_दीर्घता(-1),
          str_(शून्यसूचकः), max_दीर्घता(max_String_दीर्घता), मानम्(0), राशिः(शून्यसूचकः) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    // str_ and stack_ are pointers so that it's easy to turn bytestrie.h into
    // a public API header for which we would want it to depend only on
    // other public headers.
    // Unlike BytesTrie itself, its Iterator performs memory allocations anyway
    // via the CharString and UVector32 implementations, so this additional
    // cost is minimal.
    str_=नूतन CharString();
    राशिः=नूतन UVector32(error_संहिता);
    यदि(U_SUCCESS(error_संहिता) && (str_==शून्यसूचकः || राशिः==शून्यसूचकः)) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }
}

अष्टकानि_Trie::Iterator::Iterator(ध्रुव अष्टकानि_Trie &trie, int32_t max_String_दीर्घता,
                              UError_संहिता &error_संहिता)
        : अष्टकानि_2(trie.अष्टकानि_2), pos_(trie.pos_), initialPos_(trie.pos_),
          remaining_मेलनम्_दीर्घता(trie.remaining_मेलनम्_दीर्घता),
          initial_Remaining_मेलनम्_दीर्घता(trie.remaining_मेलनम्_दीर्घता),
          str_(शून्यसूचकः), max_दीर्घता(max_String_दीर्घता), मानम्(0), राशिः(शून्यसूचकः) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    str_=नूतन CharString();
    राशिः=नूतन UVector32(error_संहिता);
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम;
    }
    यदि(str_==शून्यसूचकः || राशिः==शून्यसूचकः) {
        error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    int32_t length=remaining_मेलनम्_दीर्घता;  // Actual remaining match length minus 1.
    यदि(length>=0) {
        // Pending linear-match node, append remaining bytes to str_.
        ++length;
        यदि(max_दीर्घता>0 && length>max_दीर्घता) {
            length=max_दीर्घता;  // This will leave remainingMatchLength>=0 as a signal.
        }
        str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos_), length, error_संहिता);
        pos_+=length;
        remaining_मेलनम्_दीर्घता-=length;
    }
}

अष्टकानि_Trie::Iterator::~Iterator() {
    विलोपय str_;
    विलोपय राशिः;
}

अष्टकानि_Trie::Iterator&
अष्टकानि_Trie::Iterator::reset() U_LIFETIME_BOUND {
    pos_=initialPos_;
    remaining_मेलनम्_दीर्घता=initial_Remaining_मेलनम्_दीर्घता;
    int32_t length=remaining_मेलनम्_दीर्घता+1;  // Remaining match length.
    यदि(max_दीर्घता>0 && length>max_दीर्घता) {
        length=max_दीर्घता;
    }
    str_->truncate(length);
    pos_+=length;
    remaining_मेलनम्_दीर्घता-=length;
    राशिः->समुच्चयः_आकार(0);
    प्रत्यागम *एतत्;
}

UBool
अष्टकानि_Trie::Iterator::has_अग्र() ध्रुव { प्रत्यागम pos_!=शून्यसूचकः || !राशिः->is_रिक्तानि(); }

UBool
अष्टकानि_Trie::Iterator::next(UError_संहिता &error_संहिता) {
    यदि(U_FAILURE(error_संहिता)) {
        प्रत्यागम असत्य;
    }
    ध्रुव uint8_t *pos=pos_;
    यदि(pos==शून्यसूचकः) {
        यदि(राशिः->is_रिक्तानि()) {
            प्रत्यागम असत्य;
        }
        // Pop the state off the stack and continue with the next outbound edge of
        // the branch node.
        int32_t राशिः_आकार=राशिः->size();
        int32_t length=राशिः->elementAti(राशिः_आकार-1);
        pos=अष्टकानि_2+राशिः->elementAti(राशिः_आकार-2);
        राशिः->समुच्चयः_आकार(राशिः_आकार-2);
        str_->truncate(length&0xffff);
        length = static_cast<int32_t>(static_cast<uint32_t>(length) >> 16);
        यदि(length>1) {
            pos=branch_अग्र(pos, length, error_संहिता);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम सत्य;  // Reached a final value.
            }
        } अन्यथा {
            str_->append(static_cast<वर्ण>(*pos++), error_संहिता);
        }
    }
    यदि(remaining_मेलनम्_दीर्घता>=0) {
        // We only get here if we started in a pending linear-match node
        // with more than maxLength remaining bytes.
        प्रत्यागम truncateAndStop();
    }
    प्रति(;;) {
        int32_t ग्रन्थिः=*pos++;
        यदि(ग्रन्थिः>=k_Min_मानम्_Lead) {
            // Deliver value for the byte sequence so far.
            UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
            मानम्=read_मानम्(pos, ग्रन्थिः>>1);
            यदि(isFinal || (max_दीर्घता>0 && str_->length()==max_दीर्घता)) {
                pos_=शून्यसूचकः;
            } अन्यथा {
                pos_=skip_मानम्(pos, ग्रन्थिः);
            }
            प्रत्यागम सत्य;
        }
        यदि(max_दीर्घता>0 && str_->length()==max_दीर्घता) {
            प्रत्यागम truncateAndStop();
        }
        यदि(ग्रन्थिः<k_Min_रैखिकम्_मेलनम्) {
            यदि(ग्रन्थिः==0) {
                ग्रन्थिः=*pos++;
            }
            pos=branch_अग्र(pos, ग्रन्थिः+1, error_संहिता);
            यदि(pos==शून्यसूचकः) {
                प्रत्यागम सत्य;  // Reached a final value.
            }
        } अन्यथा {
            // Linear-match node, append length bytes to str_.
            int32_t length=ग्रन्थिः-k_Min_रैखिकम्_मेलनम्+1;
            यदि(max_दीर्घता>0 && str_->length()+length>max_दीर्घता) {
                str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos),
                            max_दीर्घता-str_->length(), error_संहिता);
                प्रत्यागम truncateAndStop();
            }
            str_->append(reinterpret_cast<ध्रुव वर्ण *>(pos), length, error_संहिता);
            pos+=length;
        }
    }
}

StringPiece
अष्टकानि_Trie::Iterator::getString() ध्रुव U_LIFETIME_BOUND {
    प्रत्यागम str_ == शून्यसूचकः ? StringPiece() : str_->गन्तव्यम्_String_Piece();
}

UBool
अष्टकानि_Trie::Iterator::truncateAndStop() {
    pos_=शून्यसूचकः;
    मानम्=-1;  // no real value for str
    प्रत्यागम सत्य;
}

// Branch node, needs to take the first outbound edge and push state for the rest.
ध्रुव uint8_t*
अष्टकानि_Trie::Iterator::branch_अग्र(ध्रुव uint8_t* pos U_LIFETIME_BOUND,
                                int32_t length,
                                UError_संहिता& error_संहिता) {
    यावत्(length>k_Max_Branch_रैखिकम्_Sub_ग्रन्थिः_दीर्घता) {
        ++pos;  // ignore the comparison byte
        // Push state for the greater-or-equal edge.
        राशिः->सङ्कलय_Element(static_cast<int32_t>(skipDelta(pos) - अष्टकानि_2), error_संहिता);
        राशिः->सङ्कलय_Element(((length-(length>>1))<<16)|str_->length(), error_संहिता);
        // Follow the less-than edge.
        length>>=1;
        pos=jumpByDelta(pos);
    }
    // List of key-value pairs where values are either final values or jump deltas.
    // Read the first (key, value) pair.
    uint8_t trieByte=*pos++;
    int32_t ग्रन्थिः=*pos++;
    UBool isFinal = static_cast<UBool>(ग्रन्थिः & k_मानम्_Is_Final);
    int32_t value=read_मानम्(pos, ग्रन्थिः>>1);
    pos=skip_मानम्(pos, ग्रन्थिः);
    राशिः->सङ्कलय_Element(static_cast<int32_t>(pos - अष्टकानि_2), error_संहिता);
    राशिः->सङ्कलय_Element(((length-1)<<16)|str_->length(), error_संहिता);
    str_->append(static_cast<वर्ण>(trieByte), error_संहिता);
    यदि(isFinal) {
        pos_=शून्यसूचकः;
        मानम्=value;
        प्रत्यागम शून्यसूचकः;
    } अन्यथा {
        प्रत्यागम pos+value;
    }
}

U_NAMESPACE_अन्तः
