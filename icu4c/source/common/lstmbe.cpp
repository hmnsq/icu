// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#অন্তর্ভাব <complex>
#অন্তর্ভাব <utility>

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_BREAK_ITERATION

#অন্তর্ভাব "brkeng.h"
#অন্তর্ভাব "charstr.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "lstmbe.h"
#অন্তর্ভাব "putilimp.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ubrkimpl.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uvectr32.h"
#অন্তর্ভাব "uvector.h"

#অন্তর্ভাব "unicode/brkiter.h"
#অন্তর্ভাব "unicode/resbund.h"
#অন্তর্ভাব "unicode/ubrk.h"
#অন্তর্ভাব "unicode/uniset.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
বর্গ ReadArray1D {
সার্বজনিক:
    আভাস ~ReadArray1D();
    আভাস int32_t d1() ধ্রুব = 0;
    আভাস ভিন্ন get(int32_t i) ধ্রুব = 0;

#ifdef LSTM_DEBUG
    শূন্য print() ধ্রুব {
        printf("\n[");
        প্রতি (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           যদি (i % 4 == 3) printf("\n");
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
বর্গ ReadArray2D {
সার্বজনিক:
    আভাস ~ReadArray2D();
    আভাস int32_t d1() ধ্রুব = 0;
    আভাস int32_t d2() ধ্রুব = 0;
    আভাস ভিন্ন get(int32_t i, int32_t j) ধ্রুব = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
বর্গ ConstArray1D : সার্বজনিক ReadArray1D {
সার্বজনিক:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(ধ্রুব ভিন্ন* data, int32_t d1) : data_(data), d1_(d1) {}

    আভাস ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    শূন্য init(ধ্রুব int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ধ্রুব ভিন্ন*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    আভাস int32_t d1() ধ্রুব override { প্রত্যাগম d1_; }
    আভাস ভিন্ন get(int32_t i) ধ্রুব override {
        U_ASSERT(i < d1_);
        প্রত্যাগম data_[i];
    }

গুপ্ত:
    ধ্রুব ভিন্ন* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
বর্গ ConstArray2D : সার্বজনিক ReadArray2D {
সার্বজনিক:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(ধ্রুব ভিন্ন* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    আভাস ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    শূন্য init(ধ্রুব int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ধ্রুব ভিন্ন*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    অন্তঃস্থ int32_t d1() ধ্রুব override { প্রত্যাগম d1_; }
    অন্তঃস্থ int32_t d2() ধ্রুব override { প্রত্যাগম d2_; }
    ভিন্ন get(int32_t i, int32_t j) ধ্রুব override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        প্রত্যাগম data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    অন্তঃস্থ ConstArray1D row(int32_t i) ধ্রুব {
        U_ASSERT(i < d1_);
        প্রত্যাগম ConstArray1D(data_ + i * d2_, d2_);
    }

গুপ্ত:
    ধ্রুব ভিন্ন* data_;
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
বর্গ Array1D : সার্বজনিক ReadArray1D {
সার্বজনিক:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * পরিমাণ(ভিন্ন))),
          data_(static_cast<ভিন্ন*>(memory_)), d1_(d1) {
        যদি (U_SUCCESS(status)) {
            যদি (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            clear();
        }
    }

    আভাস ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(ভিন্ন* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    আভাস int32_t d1() ধ্রুব override { প্রত্যাগম d1_; }
    আভাস ভিন্ন get(int32_t i) ধ্রুব override {
        U_ASSERT(i < d1_);
        প্রত্যাগম data_[i];
    }

    // Return the index which point to the max data in the array.
    অন্তঃস্থ int32_t maxIndex() ধ্রুব {
        int32_t index = 0;
        ভিন্ন max = data_[0];
        প্রতি (int32_t i = 1; i < d1_; i++) {
            যদি (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        প্রত্যাগম index;
    }

    // Slice part of the array to a new one.
    অন্তঃস্থ Array1D slice(int32_t from, int32_t size) ধ্রুব {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        প্রত্যাগম Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    অন্তঃস্থ Array1D& addDotProduct(ধ্রুব ReadArray1D& a, ধ্রুব ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        প্রতি (int32_t i = 0; i < d1(); i++) {
            প্রতি (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        প্রত্যাগম *এতত্;
    }

    // Hadamard Product the values of another array of the same size into this one.
    অন্তঃস্থ Array1D& hadamardProduct(ধ্রুব ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        প্রতি (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        প্রত্যাগম *এতত্;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    অন্তঃস্থ Array1D& addHadamardProduct(ধ্রুব ReadArray1D& a, ধ্রুব ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        প্রতি (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        প্রত্যাগম *এতত্;
    }

    // Add the values of another array of the same size into this one.
    অন্তঃস্থ Array1D& add(ধ্রুব ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        প্রতি (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        প্রত্যাগম *এতত্;
    }

    // Assign the values of another array of the same size into this one.
    অন্তঃস্থ Array1D& assign(ধ্রুব ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        প্রতি (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        প্রত্যাগম *এতত্;
    }

    // Apply tanh to all the elements in the array.
    অন্তঃস্থ Array1D& tanh() {
        প্রত্যাগম tanh(*এতত্);
    }

    // Apply tanh of a and store into this array.
    অন্তঃস্থ Array1D& tanh(ধ্রুব Array1D& a) {
        U_ASSERT(a.d1() == d1());
        প্রতি (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        প্রত্যাগম *এতত্;
    }

    // Apply sigmoid to all the elements in the array.
    অন্তঃস্থ Array1D& sigmoid() {
        প্রতি (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        প্রত্যাগম *এতত্;
    }

    অন্তঃস্থ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * পরিমাণ(ভিন্ন));
        প্রত্যাগম *এতত্;
    }

গুপ্ত:
    শূন্য* memory_;
    ভিন্ন* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

বর্গ Array2D : সার্বজনিক ReadArray2D {
সার্বজনিক:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * পরিমাণ(ভিন্ন))),
          data_(static_cast<ভিন্ন*>(memory_)), d1_(d1), d2_(d2) {
        যদি (U_SUCCESS(status)) {
            যদি (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                প্রত্যাগম;
            }
            clear();
        }
    }
    আভাস ~Array2D();

    // ReadArray2D methods.
    আভাস int32_t d1() ধ্রুব override { প্রত্যাগম d1_; }
    আভাস int32_t d2() ধ্রুব override { প্রত্যাগম d2_; }
    আভাস ভিন্ন get(int32_t i, int32_t j) ধ্রুব override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        প্রত্যাগম data_[i * d2_ + j];
    }

    অন্তঃস্থ Array1D row(int32_t i) ধ্রুব {
        U_ASSERT(i < d1_);
        প্রত্যাগম Array1D(data_ + i * d2_, d2_);
    }

    অন্তঃস্থ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * পরিমাণ(ভিন্ন));
        প্রত্যাগম *এতত্;
    }

গুপ্ত:
    শূন্য* memory_;
    ভিন্ন* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

প্রকারনির্দেশ গণনা {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

প্রকারনির্দেশ গণনা {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

সংরচনা LSTMData : সার্বজনিক UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ধ্রুব char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

গুপ্ত:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম;
    }
    যদি (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        প্রত্যাগম;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_চাবি(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_চাবি(rb, "hunits", nullptr, &status));
    যদি (U_FAILURE(status)) প্রত্যাগম;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    ধ্রুব char16_t* type = ures_get_String_By_চাবি(rb, "type", nullptr, &status);
    যদি (U_FAILURE(status)) প্রত্যাগম;
    যদি (u_strCompare(type, -1, u"codepoints", -1, অসত্য) == 0) {
        fType = CODE_POINTS;
    } অন্যথা যদি (u_strCompare(type, -1, u"graphclust", -1, অসত্য) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_চাবি(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_চাবি(rb, "data", nullptr, &status));
    যদি (U_FAILURE(status)) প্রত্যাগম;
    int32_t data_len = 0;
    ধ্রুব int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    স্তূপ_UResource_Bundle স্তূপ_Temp_Bundle;
    Resource_Data_মান value;
    ures_get_মান_With_Fallback(rb, "dict", স্তূপ_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t অঙ্ক_index = stringArray.getSize();
    যদি (U_FAILURE(status)) { প্রত্যাগম; }

    // put dict into hash
    int32_t stringLength;
    প্রতি (int32_t idx = 0; idx < অঙ্ক_index; idx++) {
        stringArray.get_মান(idx, value);
        ধ্রুব char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (শূন্য*)str, idx, &status);
        যদি (U_FAILURE(status)) প্রত্যাগম;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        যাবত্ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (অঙ্ক_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#যদি U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (অঙ্ক_index + 1), embedding_size);
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

বর্গ Vectorizer : সার্বজনিক UMemory {
সার্বজনিক:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    আভাস ~Vectorizer();
    আভাস শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুব = 0;
রক্ষিত:
    int32_t stringToIndex(ধ্রুব char16_t* str) ধ্রুব {
        UBool found = অসত্য;
        int32_t ret = uhash_getiAndFound(fDict, (ধ্রুব শূন্য*)str, &found);
        যদি (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        যাবত্ (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        প্রত্যাগম ret;
    }

গুপ্ত:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

বর্গ CodePointsVectorizer : সার্বজনিক Vectorizer {
সার্বজনিক:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    আভাস ~CodePointsVectorizer();
    আভাস শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুব override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

শূন্য CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ধ্রুব
{
    যদি (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        যদি (U_FAILURE(status)) প্রত্যাগম;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        যাবত্ (U_SUCCESS(status) &&
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

বর্গ GraphemeClusterVectorizer : সার্বজনিক Vectorizer {
সার্বজনিক:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    আভাস ~GraphemeClusterVectorizer();
    আভাস শূন্য vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ধ্রুব override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

শূন্য GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ধ্রুব
{
    যদি (U_FAILURE(status)) প্রত্যাগম;
    যদি (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        প্রত্যাগম;
    }
    যদি (U_FAILURE(status)) প্রত্যাগম;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    যদি (U_FAILURE(status)) প্রত্যাগম;
    graphemeIter->setText(text, status);
    যদি (U_FAILURE(status)) প্রত্যাগম;

    যদি (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    যাবত্ ((current = graphemeIter->next()) != BreakIterator::DONE) {
        যদি (current >= endPos) {
            ভঙ্গ;
        }
        যদি (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            যদি (U_FAILURE(status)) প্রত্যাগম;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            যদি (U_FAILURE(status)) প্রত্যাগম;
        }
        last = current;
    }
    যদি (U_FAILURE(status) || last >= endPos) {
        প্রত্যাগম;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    যদি (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
শূন্য compute(
    int32_t hunits,
    ধ্রুব ReadArray2D& W, ধ্রুব ReadArray2D& U, ধ্রুব ReadArray1D& b,
    ধ্রুব ReadArray1D& x, Array1D& h, Array1D& c,
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
স্থির ধ্রুব int32_t MIN_WORD = 2;

// Minimum number of characters for two words
স্থির ধ্রুব int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ধ্রুব {
    যদি (U_FAILURE(status)) প্রত্যাগম 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    যদি (utext_getNativeIndex(text) >= endPos) {
        প্রত্যাগম 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    যদি (U_FAILURE(status)) প্রত্যাগম 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    যদি (U_FAILURE(status)) প্রত্যাগম 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t প্রবেশ_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(প্রবেশ_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    যদি (U_FAILURE(status)) প্রত্যাগম 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    প্রতি (int32_t i = প্রবেশ_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        যদি (i != প্রবেশ_seq_len - 1) {
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
    প্রতি (int32_t i = 0; i < প্রবেশ_seq_len; i++) {
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
        যদি (current == BEGIN || current == SINGLE) {
            যদি (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                যদি (U_FAILURE(status)) প্রত্যাগম 0;
            }
        }
    }
    প্রত্যাগম foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(ধ্রুব LSTMData* data, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }
    বিকল্প (data->fType) {
        পক্ষ CODE_POINTS:
            প্রত্যাগম নূতন CodePointsVectorizer(data->fDict);
            ভঙ্গ;
        পক্ষ GRAPHEME_CLUSTER:
            প্রত্যাগম নূতন GraphemeClusterVectorizer(data->fDict);
            ভঙ্গ;
        স্বভাব:
            ভঙ্গ;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ধ্রুব LSTMData* data, ধ্রুব UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    যদি (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      প্রত্যাগম;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

ধ্রুব char16_t* LSTMBreakEngine::name() ধ্রুব {
    প্রত্যাগম fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_চাবি_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_চাবি(b, uscript_getShortName(script), &status);
    ures_close(b);
    প্রত্যাগম result;
}

U_CAPI ধ্রুব LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    যদি (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        প্রত্যাগম nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    যদি (U_FAILURE(status)) প্রত্যাগম nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    যদি (U_FAILURE(status)) প্রত্যাগম nullptr;

    প্রত্যাগম CreateLSTMData(rb.orphan(), status);
}

U_CAPI ধ্রুব LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    যদি (U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব LSTMData* result = নূতন LSTMData(rb, status);
    যদি (U_FAILURE(status)) {
        delete result;
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম result;
}

U_CAPI ধ্রুব LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, ধ্রুব LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    বিকল্প(script) {
        পক্ষ USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            ভঙ্গ;
        পক্ষ USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            ভঙ্গ;
        স্বভাব:
            delete data;
            প্রত্যাগম nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    ধ্রুব LanguageBreakEngine* engine = নূতন LSTMBreakEngine(data, unicodeSet, status);
    যদি (U_FAILURE(status) || engine == nullptr) {
        যদি (engine != nullptr) {
            delete engine;
        } অন্যথা {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম engine;
}

U_CAPI শূন্য U_EXPORT2 DeleteLSTMData(ধ্রুব LSTMData* data)
{
    delete data;
}

U_CAPI ধ্রুব char16_t* U_EXPORT2 LSTMDataName(ধ্রুব LSTMData* data)
{
    প্রত্যাগম data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
