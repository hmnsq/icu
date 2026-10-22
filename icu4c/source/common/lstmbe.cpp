// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#അന്തര്ഭാവ <complex>
#അന്തര്ഭാവ <utility>

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_BREAK_ITERATION

#അന്തര്ഭാവ "brkeng.h"
#അന്തര്ഭാവ "charstr.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "lstmbe.h"
#അന്തര്ഭാവ "putilimp.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ubrkimpl.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "uvectr32.h"
#അന്തര്ഭാവ "uvector.h"

#അന്തര്ഭാവ "unicode/brkiter.h"
#അന്തര്ഭാവ "unicode/resbund.h"
#അന്തര്ഭാവ "unicode/ubrk.h"
#അന്തര്ഭാവ "unicode/uniset.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
വർഗ്ഗം ReadArray1D {
പൊതു:
    മായാ ~ReadArray1D();
    മായാ int32_t d1() സ്ഥിരം = 0;
    മായാ ദശാംശം get(int32_t i) സ്ഥിരം = 0;

#ifdef LSTM_DEBUG
    ശൂന്യം print() സ്ഥിരം {
        printf("\n[");
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           എങ്കിൽ (i % 4 == 3) printf("\n");
        }
        printf("]\n");
    }
#endif
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
വർഗ്ഗം ReadArray2D {
പൊതു:
    മായാ ~ReadArray2D();
    മായാ int32_t d1() സ്ഥിരം = 0;
    മായാ int32_t d2() സ്ഥിരം = 0;
    മായാ ദശാംശം get(int32_t i, int32_t j) സ്ഥിരം = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
വർഗ്ഗം ConstArray1D : പൊതു ReadArray1D {
പൊതു:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(സ്ഥിരം ദശാംശം* data, int32_t d1) : data_(data), d1_(d1) {}

    മായാ ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ശൂന്യം init(സ്ഥിരം int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<സ്ഥിരം ദശാംശം*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    മായാ int32_t d1() സ്ഥിരം override { തിരികെ d1_; }
    മായാ ദശാംശം get(int32_t i) സ്ഥിരം override {
        U_ASSERT(i < d1_);
        തിരികെ data_[i];
    }

സ്വകാര്യ:
    സ്ഥിരം ദശാംശം* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
വർഗ്ഗം ConstArray2D : പൊതു ReadArray2D {
പൊതു:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(സ്ഥിരം ദശാംശം* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    മായാ ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    ശൂന്യം init(സ്ഥിരം int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<സ്ഥിരം ദശാംശം*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    വരിയിൽ int32_t d1() സ്ഥിരം override { തിരികെ d1_; }
    വരിയിൽ int32_t d2() സ്ഥിരം override { തിരികെ d2_; }
    ദശാംശം get(int32_t i, int32_t j) സ്ഥിരം override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        തിരികെ data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    വരിയിൽ ConstArray1D row(int32_t i) സ്ഥിരം {
        U_ASSERT(i < d1_);
        തിരികെ ConstArray1D(data_ + i * d2_, d2_);
    }

സ്വകാര്യ:
    സ്ഥിരം ദശാംശം* data_;
    int32_t d1_;
    int32_t d2_;
};

ConstArray2D::~ConstArray2D()
{
}

/**
 * A class to allocate data as a writable 1D array.
 * This is the main class implement matrix operation.
 */
വർഗ്ഗം Array1D : പൊതു ReadArray1D {
പൊതു:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * വലുപ്പം(ദശാംശം))),
          data_(static_cast<ദശാംശം*>(memory_)), d1_(d1) {
        എങ്കിൽ (U_SUCCESS(status)) {
            എങ്കിൽ (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            clear();
        }
    }

    മായാ ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ദശാംശം* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    മായാ int32_t d1() സ്ഥിരം override { തിരികെ d1_; }
    മായാ ദശാംശം get(int32_t i) സ്ഥിരം override {
        U_ASSERT(i < d1_);
        തിരികെ data_[i];
    }

    // Return the index which point to the max data in the array.
    വരിയിൽ int32_t maxIndex() സ്ഥിരം {
        int32_t index = 0;
        ദശാംശം max = data_[0];
        വേണ്ടി (int32_t i = 1; i < d1_; i++) {
            എങ്കിൽ (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        തിരികെ index;
    }

    // Slice part of the array to a new one.
    വരിയിൽ Array1D slice(int32_t from, int32_t size) സ്ഥിരം {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        തിരികെ Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    വരിയിൽ Array1D& addDotProduct(സ്ഥിരം ReadArray1D& a, സ്ഥിരം ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
            വേണ്ടി (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        തിരികെ *ഇത്;
    }

    // Hadamard Product the values of another array of the same size into this one.
    വരിയിൽ Array1D& hadamardProduct(സ്ഥിരം ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        തിരികെ *ഇത്;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    വരിയിൽ Array1D& addHadamardProduct(സ്ഥിരം ReadArray1D& a, സ്ഥിരം ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        തിരികെ *ഇത്;
    }

    // Add the values of another array of the same size into this one.
    വരിയിൽ Array1D& add(സ്ഥിരം ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        തിരികെ *ഇത്;
    }

    // Assign the values of another array of the same size into this one.
    വരിയിൽ Array1D& assign(സ്ഥിരം ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        വേണ്ടി (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        തിരികെ *ഇത്;
    }

    // Apply tanh to all the elements in the array.
    വരിയിൽ Array1D& tanh() {
        തിരികെ tanh(*ഇത്);
    }

    // Apply tanh of a and store into this array.
    വരിയിൽ Array1D& tanh(സ്ഥിരം Array1D& a) {
        U_ASSERT(a.d1() == d1());
        വേണ്ടി (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        തിരികെ *ഇത്;
    }

    // Apply sigmoid to all the elements in the array.
    വരിയിൽ Array1D& sigmoid() {
        വേണ്ടി (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        തിരികെ *ഇത്;
    }

    വരിയിൽ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * വലുപ്പം(ദശാംശം));
        തിരികെ *ഇത്;
    }

സ്വകാര്യ:
    ശൂന്യം* memory_;
    ദശാംശം* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

വർഗ്ഗം Array2D : പൊതു ReadArray2D {
പൊതു:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * വലുപ്പം(ദശാംശം))),
          data_(static_cast<ദശാംശം*>(memory_)), d1_(d1), d2_(d2) {
        എങ്കിൽ (U_SUCCESS(status)) {
            എങ്കിൽ (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                തിരികെ;
            }
            clear();
        }
    }
    മായാ ~Array2D();

    // ReadArray2D methods.
    മായാ int32_t d1() സ്ഥിരം override { തിരികെ d1_; }
    മായാ int32_t d2() സ്ഥിരം override { തിരികെ d2_; }
    മായാ ദശാംശം get(int32_t i, int32_t j) സ്ഥിരം override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        തിരികെ data_[i * d2_ + j];
    }

    വരിയിൽ Array1D row(int32_t i) സ്ഥിരം {
        U_ASSERT(i < d1_);
        തിരികെ Array1D(data_ + i * d2_, d2_);
    }

    വരിയിൽ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * വലുപ്പം(ദശാംശം));
        തിരികെ *ഇത്;
    }

സ്വകാര്യ:
    ശൂന്യം* memory_;
    ദശാംശം* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

തരനിർവചനം എണ്ണൽ {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

തരനിർവചനം എണ്ണൽ {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

ഘടന LSTMData : പൊതു UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    സ്ഥിരം char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

സ്വകാര്യ:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ;
    }
    എങ്കിൽ (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        തിരികെ;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_താക്കോൽ(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_താക്കോൽ(rb, "hunits", nullptr, &status));
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    സ്ഥിരം char16_t* type = ures_get_String_By_താക്കോൽ(rb, "type", nullptr, &status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    എങ്കിൽ (u_strCompare(type, -1, u"codepoints", -1, അസത്യം) == 0) {
        fType = CODE_POINTS;
    } അല്ലെങ്കിൽ എങ്കിൽ (u_strCompare(type, -1, u"graphclust", -1, അസത്യം) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_താക്കോൽ(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_താക്കോൽ(rb, "data", nullptr, &status));
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    int32_t data_len = 0;
    സ്ഥിരം int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    അട്ടി_UResource_Bundle അട്ടി_Temp_Bundle;
    Resource_Data_മൂല്യം value;
    ures_get_മൂല്യം_With_Fallback(rb, "dict", അട്ടി_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t അക്കം_index = stringArray.getSize();
    എങ്കിൽ (U_FAILURE(status)) { തിരികെ; }

    // put dict into hash
    int32_t stringLength;
    വേണ്ടി (int32_t idx = 0; idx < അക്കം_index; idx++) {
        stringArray.get_മൂല്യം(idx, value);
        സ്ഥിരം char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (ശൂന്യം*)str, idx, &status);
        എങ്കിൽ (U_FAILURE(status)) തിരികെ;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        ആകുമ്പോൾ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (അക്കം_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#എങ്കിൽ U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (അക്കം_index + 1), embedding_size);
    data += mat1_size;
    fForwardW.init(data, embedding_size, 4 * hunits);
    data += mat2_size;
    fForwardU.init(data, hunits, 4 * hunits);
    data += mat3_size;
    fForwardB.init(data, 4 * hunits);
    data += mat4_size;
    fBackwardW.init(data, embedding_size, 4 * hunits);
    data += mat5_size;
    fBackwardU.init(data, hunits, 4 * hunits);
    data += mat6_size;
    fBackwardB.init(data, 4 * hunits);
    data += mat7_size;
    fOutputW.init(data, 2 * hunits, 4);
    data += mat8_size;
    fOutputB.init(data, 4);
}

LSTMData::~LSTMData() {
    uhash_close(fDict);
    ures_close(fBundle);
}

വർഗ്ഗം Vectorizer : പൊതു UMemory {
പൊതു:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    മായാ ~Vectorizer();
    മായാ ശൂന്യം vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) സ്ഥിരം = 0;
സംരക്ഷിത:
    int32_t stringToIndex(സ്ഥിരം char16_t* str) സ്ഥിരം {
        UBool found = അസത്യം;
        int32_t ret = uhash_getiAndFound(fDict, (സ്ഥിരം ശൂന്യം*)str, &found);
        എങ്കിൽ (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        ആകുമ്പോൾ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        തിരികെ ret;
    }

സ്വകാര്യ:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

വർഗ്ഗം CodePointsVectorizer : പൊതു Vectorizer {
പൊതു:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    മായാ ~CodePointsVectorizer();
    മായാ ശൂന്യം vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) സ്ഥിരം override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

ശൂന്യം CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) സ്ഥിരം
{
    എങ്കിൽ (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        എങ്കിൽ (U_FAILURE(status)) തിരികെ;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        ആകുമ്പോൾ (U_SUCCESS(status) &&
               (current = static_cast<int32_t>(utext_getNativeIndex(text))) < endPos) {
            // Since the LSTMBreakEngine is currently only accept chars in BMP,
            // we can ignore the possibility of hitting supplementary code
            // point.
            str[0] = static_cast<char16_t>(utext_next32(text));
            U_ASSERT(!U_IS_SURROGATE(str[0]));
            offsets.addElement(current, status);
            indices.addElement(stringToIndex(str), status);
        }
    }
}

വർഗ്ഗം GraphemeClusterVectorizer : പൊതു Vectorizer {
പൊതു:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    മായാ ~GraphemeClusterVectorizer();
    മായാ ശൂന്യം vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) സ്ഥിരം override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

ശൂന്യം GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) സ്ഥിരം
{
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    എങ്കിൽ (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        തിരികെ;
    }
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;
    graphemeIter->setText(text, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ;

    എങ്കിൽ (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    ആകുമ്പോൾ ((current = graphemeIter->next()) != BreakIterator::DONE) {
        എങ്കിൽ (current >= endPos) {
            നിർത്തുക;
        }
        എങ്കിൽ (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            എങ്കിൽ (U_FAILURE(status)) തിരികെ;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            എങ്കിൽ (U_FAILURE(status)) തിരികെ;
        }
        last = current;
    }
    എങ്കിൽ (U_FAILURE(status) || last >= endPos) {
        തിരികെ;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    എങ്കിൽ (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
ശൂന്യം compute(
    int32_t hunits,
    സ്ഥിരം ReadArray2D& W, സ്ഥിരം ReadArray2D& U, സ്ഥിരം ReadArray1D& b,
    സ്ഥിരം ReadArray1D& x, Array1D& h, Array1D& c,
    Array1D& ifco)
{
    // ifco = x * W + h * U + b
    ifco.assign(b)
        .addDotProduct(x, W)
        .addDotProduct(h, U);

    ifco.slice(0*hunits, hunits).sigmoid();  // i: sigmod
    ifco.slice(1*hunits, hunits).sigmoid(); // f: sigmoid
    ifco.slice(2*hunits, hunits).tanh(); // c_: tanh
    ifco.slice(3*hunits, hunits).sigmoid(); // o: sigmod

    c.hadamardProduct(ifco.slice(hunits, hunits))
        .addHadamardProduct(ifco.slice(0, hunits), ifco.slice(2*hunits, hunits));

    h.tanh(c)
        .hadamardProduct(ifco.slice(3*hunits, hunits));
}

// Minimum word size
സ്ഥിതം സ്ഥിരം int32_t MIN_WORD = 2;

// Minimum number of characters for two words
സ്ഥിതം സ്ഥിരം int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) സ്ഥിരം {
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    എങ്കിൽ (utext_getNativeIndex(text) >= endPos) {
        തിരികെ 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t നിവേശം_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(നിവേശം_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    വേണ്ടി (int32_t i = നിവേശം_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        എങ്കിൽ (i != നിവേശം_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#ifdef LSTM_DEBUG
        printf("hRow %d\n", i);
        hRow.print();
        printf("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        printf("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
        fData->fEmbedding.row(indicesBuf[i]).print();
#endif  // LSTM_DEBUG
        compute(hunits,
                fData->fBackwardW, fData->fBackwardU, fData->fBackwardB,
                fData->fEmbedding.row(indicesBuf[i]),
                hRow, c, ifco);
    }


    Array1D forwardRow = fbRow.slice(0, hunits);  // point to first half of data in fbRow.
    Array1D backwardRow = fbRow.slice(hunits, hunits);  // point to second half of data n fbRow.

    // The following iteration merge the forward LSTM and the output layer
    // together.
    c.clear();  // reuse c since it is the same size.
    വേണ്ടി (int32_t i = 0; i < നിവേശം_seq_len; i++) {
#ifdef LSTM_DEBUG
        printf("forwardRow %d\n", i);
        forwardRow.print();
#endif  // LSTM_DEBUG
        // Forward LSTM
        // Calculate the result into forwardRow, which point to the data in the first half
        // of fbRow.
        compute(hunits,
                fData->fForwardW, fData->fForwardU, fData->fForwardB,
                fData->fEmbedding.row(indicesBuf[i]),
                forwardRow, c, ifco);

        // assign the data from hBackward.row(i) to second half of fbRowa.
        backwardRow.assign(hBackward.row(i));

        logp.assign(fData->fOutputB).addDotProduct(fbRow, fData->fOutputW);
#ifdef LSTM_DEBUG
        printf("backwardRow %d\n", i);
        backwardRow.print();
        printf("logp %d\n", i);
        logp.print();
#endif  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.maxIndex());
        // BIES logic.
        എങ്കിൽ (current == BEGIN || current == SINGLE) {
            എങ്കിൽ (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                എങ്കിൽ (U_FAILURE(status)) തിരികെ 0;
            }
        }
    }
    തിരികെ foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(സ്ഥിരം LSTMData* data, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    തിരഞ്ഞെടുപ്പ് (data->fType) {
        അവസ്ഥ CODE_POINTS:
            തിരികെ പുതിയ CodePointsVectorizer(data->fDict);
            നിർത്തുക;
        അവസ്ഥ GRAPHEME_CLUSTER:
            തിരികെ പുതിയ GraphemeClusterVectorizer(data->fDict);
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(സ്ഥിരം LSTMData* data, സ്ഥിരം UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    എങ്കിൽ (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      തിരികെ;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

സ്ഥിരം char16_t* LSTMBreakEngine::name() സ്ഥിരം {
    തിരികെ fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_താക്കോൽ_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_താക്കോൽ(b, uscript_getShortName(script), &status);
    ures_close(b);
    തിരികെ result;
}

U_CAPI സ്ഥിരം LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    എങ്കിൽ (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        തിരികെ nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    എങ്കിൽ (U_FAILURE(status)) തിരികെ nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    എങ്കിൽ (U_FAILURE(status)) തിരികെ nullptr;

    തിരികെ CreateLSTMData(rb.orphan(), status);
}

U_CAPI സ്ഥിരം LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    എങ്കിൽ (U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    സ്ഥിരം LSTMData* result = പുതിയ LSTMData(rb, status);
    എങ്കിൽ (U_FAILURE(status)) {
        delete result;
        തിരികെ nullptr;
    }
    തിരികെ result;
}

U_CAPI സ്ഥിരം LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, സ്ഥിരം LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    തിരഞ്ഞെടുപ്പ്(script) {
        അവസ്ഥ USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            നിർത്തുക;
        അവസ്ഥ USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            delete data;
            തിരികെ nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    സ്ഥിരം LanguageBreakEngine* engine = പുതിയ LSTMBreakEngine(data, unicodeSet, status);
    എങ്കിൽ (U_FAILURE(status) || engine == nullptr) {
        എങ്കിൽ (engine != nullptr) {
            delete engine;
        } അല്ലെങ്കിൽ {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        തിരികെ nullptr;
    }
    തിരികെ engine;
}

U_CAPI ശൂന്യം U_EXPORT2 DeleteLSTMData(സ്ഥിരം LSTMData* data)
{
    delete data;
}

U_CAPI സ്ഥിരം char16_t* U_EXPORT2 LSTMDataName(സ്ഥിരം LSTMData* data)
{
    തിരികെ data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
