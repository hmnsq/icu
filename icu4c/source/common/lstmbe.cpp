// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव <complex>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/utypes.h"

#सचे !UCONFIG_NO_BREAK_ITERATION

#अन्तर्भाव "brkeng.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "lstmbe.h"
#अन्तर्भाव "putilimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ubrkimpl.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvectr32.h"
#अन्तर्भाव "uvector.h"

#अन्तर्भाव "unicode/brkiter.h"
#अन्तर्भाव "unicode/resbund.h"
#अन्तर्भाव "unicode/ubrk.h"
#अन्तर्भाव "unicode/uniset.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/utf.h"

U_NAMESPACE_BEGIN

// Uncomment the following #define to debug.
// #define LSTM_DEBUG 1
// #define LSTM_VECTORIZER_DEBUG 1

/**
 * Interface for reading 1D array.
 */
वग्ग ReadArray1D {
सार्वजनिक:
    आभास ~ReadArray1D();
    आभास int32_t d1() ध्रुव = 0;
    आभास भिन्न get(int32_t i) ध्रुव = 0;

#ifdef LSTM_DEBUG
    शून्य print() ध्रुव {
        printf("\n[");
        अत्थाय (int32_t i = 0; i < d1(); i++) {
           printf("%0.8e ", get(i));
           सचे (i % 4 == 3) printf("\n");
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
वग्ग ReadArray2D {
सार्वजनिक:
    आभास ~ReadArray2D();
    आभास int32_t d1() ध्रुव = 0;
    आभास int32_t d2() ध्रुव = 0;
    आभास भिन्न get(int32_t i, int32_t j) ध्रुव = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
वग्ग ConstArray1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(ध्रुव भिन्न* data, int32_t d1) : data_(data), d1_(d1) {}

    आभास ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(ध्रुव int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ध्रुव भिन्न*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    आभास int32_t d1() ध्रुव override { पटिनिवत्त d1_; }
    आभास भिन्न get(int32_t i) ध्रुव override {
        U_ASSERT(i < d1_);
        पटिनिवत्त data_[i];
    }

गुप्त:
    ध्रुव भिन्न* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
वग्ग ConstArray2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(ध्रुव भिन्न* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    आभास ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(ध्रुव int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<ध्रुव भिन्न*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    अन्तःस्थ int32_t d1() ध्रुव override { पटिनिवत्त d1_; }
    अन्तःस्थ int32_t d2() ध्रुव override { पटिनिवत्त d2_; }
    भिन्न get(int32_t i, int32_t j) ध्रुव override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        पटिनिवत्त data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    अन्तःस्थ ConstArray1D row(int32_t i) ध्रुव {
        U_ASSERT(i < d1_);
        पटिनिवत्त ConstArray1D(data_ + i * d2_, d2_);
    }

गुप्त:
    ध्रुव भिन्न* data_;
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
वग्ग Array1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * परिमाण(भिन्न))),
          data_(static_cast<भिन्न*>(memory_)), d1_(d1) {
        सचे (U_SUCCESS(status)) {
            सचे (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            clear();
        }
    }

    आभास ~Array1D();

    // A special constructor which does not own the memory but writeable
    // as a slice of an array.
    Array1D(भिन्न* data, int32_t d1)
        : memory_(nullptr), data_(data), d1_(d1) {}

    // ReadArray1D methods.
    आभास int32_t d1() ध्रुव override { पटिनिवत्त d1_; }
    आभास भिन्न get(int32_t i) ध्रुव override {
        U_ASSERT(i < d1_);
        पटिनिवत्त data_[i];
    }

    // Return the index which point to the max data in the array.
    अन्तःस्थ int32_t maxIndex() ध्रुव {
        int32_t index = 0;
        भिन्न max = data_[0];
        अत्थाय (int32_t i = 1; i < d1_; i++) {
            सचे (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        पटिनिवत्त index;
    }

    // Slice part of the array to a new one.
    अन्तःस्थ Array1D slice(int32_t from, int32_t size) ध्रुव {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        पटिनिवत्त Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    अन्तःस्थ Array1D& addDotProduct(ध्रुव ReadArray1D& a, ध्रुव ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        अत्थाय (int32_t i = 0; i < d1(); i++) {
            अत्थाय (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        पटिनिवत्त *एतत्;
    }

    // Hadamard Product the values of another array of the same size into this one.
    अन्तःस्थ Array1D& hadamardProduct(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        अत्थाय (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        पटिनिवत्त *एतत्;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    अन्तःस्थ Array1D& addHadamardProduct(ध्रुव ReadArray1D& a, ध्रुव ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        अत्थाय (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        पटिनिवत्त *एतत्;
    }

    // Add the values of another array of the same size into this one.
    अन्तःस्थ Array1D& add(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        अत्थाय (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        पटिनिवत्त *एतत्;
    }

    // Assign the values of another array of the same size into this one.
    अन्तःस्थ Array1D& assign(ध्रुव ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        अत्थाय (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        पटिनिवत्त *एतत्;
    }

    // Apply tanh to all the elements in the array.
    अन्तःस्थ Array1D& tanh() {
        पटिनिवत्त tanh(*एतत्);
    }

    // Apply tanh of a and store into this array.
    अन्तःस्थ Array1D& tanh(ध्रुव Array1D& a) {
        U_ASSERT(a.d1() == d1());
        अत्थाय (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        पटिनिवत्त *एतत्;
    }

    // Apply sigmoid to all the elements in the array.
    अन्तःस्थ Array1D& sigmoid() {
        अत्थाय (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        पटिनिवत्त *एतत्;
    }

    अन्तःस्थ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * परिमाण(भिन्न));
        पटिनिवत्त *एतत्;
    }

गुप्त:
    शून्य* memory_;
    भिन्न* data_;
    int32_t d1_;
};

Array1D::~Array1D()
{
    uprv_free(memory_);
}

वग्ग Array2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * परिमाण(भिन्न))),
          data_(static_cast<भिन्न*>(memory_)), d1_(d1), d2_(d2) {
        सचे (U_SUCCESS(status)) {
            सचे (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                पटिनिवत्त;
            }
            clear();
        }
    }
    आभास ~Array2D();

    // ReadArray2D methods.
    आभास int32_t d1() ध्रुव override { पटिनिवत्त d1_; }
    आभास int32_t d2() ध्रुव override { पटिनिवत्त d2_; }
    आभास भिन्न get(int32_t i, int32_t j) ध्रुव override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        पटिनिवत्त data_[i * d2_ + j];
    }

    अन्तःस्थ Array1D row(int32_t i) ध्रुव {
        U_ASSERT(i < d1_);
        पटिनिवत्त Array1D(data_ + i * d2_, d2_);
    }

    अन्तःस्थ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * परिमाण(भिन्न));
        पटिनिवत्त *एतत्;
    }

गुप्त:
    शून्य* memory_;
    भिन्न* data_;
    int32_t d1_;
    int32_t d2_;
};

Array2D::~Array2D()
{
    uprv_free(memory_);
}

प्रकारनिर्देश गणना {
    BEGIN,
    INSIDE,
    END,
    SINGLE
} LSTMClass;

प्रकारनिर्देश गणना {
    UNKNOWN,
    CODE_POINTS,
    GRAPHEME_CLUSTER,
} EmbeddingType;

संरचना LSTMData : सार्वजनिक UMemory {
    LSTMData(UResourceBundle* rb, UErrorCode &status);
    ~LSTMData();
    UHashtable* fDict;
    EmbeddingType fType;
    ध्रुव char16_t* fName;
    ConstArray2D fEmbedding;
    ConstArray2D fForwardW;
    ConstArray2D fForwardU;
    ConstArray1D fForwardB;
    ConstArray2D fBackwardW;
    ConstArray2D fBackwardU;
    ConstArray1D fBackwardB;
    ConstArray2D fOutputW;
    ConstArray1D fOutputB;

गुप्त:
    UResourceBundle* fBundle;
};

LSTMData::LSTMData(UResourceBundle* rb, UErrorCode &status)
    : fDict(nullptr), fType(UNKNOWN), fName(nullptr),
      fBundle(rb)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त;
    }
    सचे (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        पटिनिवत्त;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_कुञ्चिका(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_कुञ्चिका(rb, "hunits", nullptr, &status));
    सचे (U_FAILURE(status)) पटिनिवत्त;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    ध्रुव char16_t* type = ures_get_String_By_कुञ्चिका(rb, "type", nullptr, &status);
    सचे (U_FAILURE(status)) पटिनिवत्त;
    सचे (u_strCompare(type, -1, u"codepoints", -1, मिच्छा) == 0) {
        fType = CODE_POINTS;
    } अञ्ञथा सचे (u_strCompare(type, -1, u"graphclust", -1, मिच्छा) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_कुञ्चिका(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_कुञ्चिका(rb, "data", nullptr, &status));
    सचे (U_FAILURE(status)) पटिनिवत्त;
    int32_t data_len = 0;
    ध्रुव int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    राशिः_UResource_Bundle राशिः_Temp_Bundle;
    Resource_Data_मानम् value;
    ures_get_मानम्_With_Fallback(rb, "dict", राशिः_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t अङ्कः_index = stringArray.getSize();
    सचे (U_FAILURE(status)) { पटिनिवत्त; }

    // put dict into hash
    int32_t stringLength;
    अत्थाय (int32_t idx = 0; idx < अङ्कः_index; idx++) {
        stringArray.get_मानम्(idx, value);
        ध्रुव char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (शून्य*)str, idx, &status);
        सचे (U_FAILURE(status)) पटिनिवत्त;
#ifdef LSTM_VECTORIZER_DEBUG
        printf("Assign [");
        याव (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (अङ्कः_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#सचे U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (अङ्कः_index + 1), embedding_size);
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

वग्ग Vectorizer : सार्वजनिक UMemory {
सार्वजनिक:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    आभास ~Vectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ध्रुव = 0;
रक्षित:
    int32_t stringToIndex(ध्रुव char16_t* str) ध्रुव {
        UBool found = मिच्छा;
        int32_t ret = uhash_getiAndFound(fDict, (ध्रुव शून्य*)str, &found);
        सचे (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        printf("[");
        याव (*str != 0x0000) {
            printf("U+%04x ", *str);
            str++;
        }
        printf("] map to %d\n", ret);
#endif
        पटिनिवत्त ret;
    }

गुप्त:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

वग्ग CodePointsVectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    आभास ~CodePointsVectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ध्रुव override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

शून्य CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ध्रुव
{
    सचे (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        सचे (U_FAILURE(status)) पटिनिवत्त;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        याव (U_SUCCESS(status) &&
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

वग्ग GraphemeClusterVectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    आभास ~GraphemeClusterVectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) ध्रुव override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

शून्य GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) ध्रुव
{
    सचे (U_FAILURE(status)) पटिनिवत्त;
    सचे (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        पटिनिवत्त;
    }
    सचे (U_FAILURE(status)) पटिनिवत्त;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    सचे (U_FAILURE(status)) पटिनिवत्त;
    graphemeIter->setText(text, status);
    सचे (U_FAILURE(status)) पटिनिवत्त;

    सचे (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    याव ((current = graphemeIter->next()) != BreakIterator::DONE) {
        सचे (current >= endPos) {
            भङ्ग;
        }
        सचे (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            सचे (U_FAILURE(status)) पटिनिवत्त;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            सचे (U_FAILURE(status)) पटिनिवत्त;
        }
        last = current;
    }
    सचे (U_FAILURE(status) || last >= endPos) {
        पटिनिवत्त;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    सचे (U_SUCCESS(status)) {
        offsets.addElement(last, status);
        indices.addElement(stringToIndex(str), status);
    }
}

// Computing LSTM as stated in
// https://en.wikipedia.org/wiki/Long_short-term_memory#LSTM_with_a_forget_gate
// ifco is temp array allocate outside which does not need to be
// input/output value but could avoid unnecessary memory alloc/free if passing
// in.
शून्य compute(
    int32_t hunits,
    ध्रुव ReadArray2D& W, ध्रुव ReadArray2D& U, ध्रुव ReadArray1D& b,
    ध्रुव ReadArray1D& x, Array1D& h, Array1D& c,
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
स्थिर ध्रुव int32_t MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर ध्रुव int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) ध्रुव {
    सचे (U_FAILURE(status)) पटिनिवत्त 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    सचे (utext_getNativeIndex(text) >= endPos) {
        पटिनिवत्त 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    सचे (U_FAILURE(status)) पटिनिवत्त 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    सचे (U_FAILURE(status)) पटिनिवत्त 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t निवेशः_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(निवेशः_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    सचे (U_FAILURE(status)) पटिनिवत्त 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    अत्थाय (int32_t i = निवेशः_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        सचे (i != निवेशः_seq_len - 1) {
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
    अत्थाय (int32_t i = 0; i < निवेशः_seq_len; i++) {
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
        सचे (current == BEGIN || current == SINGLE) {
            सचे (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                सचे (U_FAILURE(status)) पटिनिवत्त 0;
            }
        }
    }
    पटिनिवत्त foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(ध्रुव LSTMData* data, UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    विकल्प (data->fType) {
        पक्ष CODE_POINTS:
            पटिनिवत्त नूतन CodePointsVectorizer(data->fDict);
            भङ्ग;
        पक्ष GRAPHEME_CLUSTER:
            पटिनिवत्त नूतन GraphemeClusterVectorizer(data->fDict);
            भङ्ग;
        स्वभाव:
            भङ्ग;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(ध्रुव LSTMData* data, ध्रुव UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    सचे (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      पटिनिवत्त;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

ध्रुव char16_t* LSTMBreakEngine::name() ध्रुव {
    पटिनिवत्त fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_कुञ्चिका_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_कुञ्चिका(b, uscript_getShortName(script), &status);
    ures_close(b);
    पटिनिवत्त result;
}

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    सचे (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        पटिनिवत्त nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    सचे (U_FAILURE(status)) पटिनिवत्त nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    सचे (U_FAILURE(status)) पटिनिवत्त nullptr;

    पटिनिवत्त CreateLSTMData(rb.orphan(), status);
}

U_CAPI ध्रुव LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    सचे (U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    ध्रुव LSTMData* result = नूतन LSTMData(rb, status);
    सचे (U_FAILURE(status)) {
        delete result;
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त result;
}

U_CAPI ध्रुव LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, ध्रुव LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    विकल्प(script) {
        पक्ष USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            भङ्ग;
        पक्ष USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            भङ्ग;
        स्वभाव:
            delete data;
            पटिनिवत्त nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    ध्रुव LanguageBreakEngine* engine = नूतन LSTMBreakEngine(data, unicodeSet, status);
    सचे (U_FAILURE(status) || engine == nullptr) {
        सचे (engine != nullptr) {
            delete engine;
        } अञ्ञथा {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त engine;
}

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(ध्रुव LSTMData* data)
{
    delete data;
}

U_CAPI ध्रुव char16_t* U_EXPORT2 LSTMDataName(ध्रुव LSTMData* data)
{
    पटिनिवत्त data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
