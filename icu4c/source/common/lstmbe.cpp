// © 2021 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव <complex>
#अन्तर्भाव <utility>

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_BREAK_ITERATION

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
वर्ग ReadArray1D {
सार्वजनिक:
    आभास ~ReadArray1D();
    आभास int32_t d1() अचर = 0;
    आभास भिन्न get(int32_t i) अचर = 0;

#ifdef LSTM_DEBUG
    शून्य print() अचर {
        म_लिखो("\n[");
        लागि (int32_t i = 0; i < d1(); i++) {
           म_लिखो("%0.8e ", get(i));
           यदि (i % 4 == 3) म_लिखो("\n");
        }
        म_लिखो("]\n");
    }
#endif
};

ReadArray1D::~ReadArray1D()
{
}

/**
 * Interface for reading 2D array.
 */
वर्ग ReadArray2D {
सार्वजनिक:
    आभास ~ReadArray2D();
    आभास int32_t d1() अचर = 0;
    आभास int32_t d2() अचर = 0;
    आभास भिन्न get(int32_t i, int32_t j) अचर = 0;
};

ReadArray2D::~ReadArray2D()
{
}

/**
 * A class to index a float array as a 1D Array without owning the pointer or
 * copy the data.
 */
वर्ग ConstArray1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    ConstArray1D() : data_(nullptr), d1_(0) {}

    ConstArray1D(अचर भिन्न* data, int32_t d1) : data_(data), d1_(d1) {}

    आभास ~ConstArray1D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(अचर int32_t* data, int32_t d1) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<अचर भिन्न*>(data);
        d1_ = d1;
    }

    // ReadArray1D methods.
    आभास int32_t d1() अचर override { फर्काऊ d1_; }
    आभास भिन्न get(int32_t i) अचर override {
        U_ASSERT(i < d1_);
        फर्काऊ data_[i];
    }

गुप्त:
    अचर भिन्न* data_;
    int32_t d1_;
};

ConstArray1D::~ConstArray1D()
{
}

/**
 * A class to index a float array as a 2D Array without owning the pointer or
 * copy the data.
 */
वर्ग ConstArray2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    ConstArray2D() : data_(nullptr), d1_(0), d2_(0) {}

    ConstArray2D(अचर भिन्न* data, int32_t d1, int32_t d2)
        : data_(data), d1_(d1), d2_(d2) {}

    आभास ~ConstArray2D();

    // Init the object, the object does not own the data nor copy.
    // It is designed to directly use data from memory mapped resources.
    शून्य init(अचर int32_t* data, int32_t d1, int32_t d2) {
        U_ASSERT(IEEE_754 == 1);
        data_ = reinterpret_cast<अचर भिन्न*>(data);
        d1_ = d1;
        d2_ = d2;
    }

    // ReadArray2D methods.
    अन्तःस्थ int32_t d1() अचर override { फर्काऊ d1_; }
    अन्तःस्थ int32_t d2() अचर override { फर्काऊ d2_; }
    भिन्न get(int32_t i, int32_t j) अचर override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        फर्काऊ data_[i * d2_ + j];
    }

    // Expose the ith row as a ConstArray1D
    अन्तःस्थ ConstArray1D row(int32_t i) अचर {
        U_ASSERT(i < d1_);
        फर्काऊ ConstArray1D(data_ + i * d2_, d2_);
    }

गुप्त:
    अचर भिन्न* data_;
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
वर्ग Array1D : सार्वजनिक ReadArray1D {
सार्वजनिक:
    Array1D() : memory_(nullptr), data_(nullptr), d1_(0) {}
    Array1D(int32_t d1, UErrorCode &status)
        : memory_(uprv_malloc(d1 * परिमाण(भिन्न))),
          data_(static_cast<भिन्न*>(memory_)), d1_(d1) {
        यदि (U_SUCCESS(status)) {
            यदि (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
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
    आभास int32_t d1() अचर override { फर्काऊ d1_; }
    आभास भिन्न get(int32_t i) अचर override {
        U_ASSERT(i < d1_);
        फर्काऊ data_[i];
    }

    // Return the index which point to the max data in the array.
    अन्तःस्थ int32_t maxIndex() अचर {
        int32_t index = 0;
        भिन्न max = data_[0];
        लागि (int32_t i = 1; i < d1_; i++) {
            यदि (data_[i] > max) {
                max = data_[i];
                index = i;
            }
        }
        फर्काऊ index;
    }

    // Slice part of the array to a new one.
    अन्तःस्थ Array1D slice(int32_t from, int32_t size) अचर {
        U_ASSERT(from >= 0);
        U_ASSERT(from < d1_);
        U_ASSERT(from + size <= d1_);
        फर्काऊ Array1D(data_ + from, size);
    }

    // Add dot product of a 1D array and a 2D array into this one.
    अन्तःस्थ Array1D& addDotProduct(अचर ReadArray1D& a, अचर ReadArray2D& b) {
        U_ASSERT(a.d1() == b.d1());
        U_ASSERT(b.d2() == d1());
        लागि (int32_t i = 0; i < d1(); i++) {
            लागि (int32_t j = 0; j < a.d1(); j++) {
                data_[i] += a.get(j) * b.get(j, i);
            }
        }
        फर्काऊ *एतत्;
    }

    // Hadamard Product the values of another array of the same size into this one.
    अन्तःस्थ Array1D& hadamardProduct(अचर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        लागि (int32_t i = 0; i < d1(); i++) {
            data_[i] *= a.get(i);
        }
        फर्काऊ *एतत्;
    }

    // Add the Hadamard Product of two arrays of the same size into this one.
    अन्तःस्थ Array1D& addHadamardProduct(अचर ReadArray1D& a, अचर ReadArray1D& b) {
        U_ASSERT(a.d1() == d1());
        U_ASSERT(b.d1() == d1());
        लागि (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i) * b.get(i);
        }
        फर्काऊ *एतत्;
    }

    // Add the values of another array of the same size into this one.
    अन्तःस्थ Array1D& add(अचर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        लागि (int32_t i = 0; i < d1(); i++) {
            data_[i] += a.get(i);
        }
        फर्काऊ *एतत्;
    }

    // Assign the values of another array of the same size into this one.
    अन्तःस्थ Array1D& assign(अचर ReadArray1D& a) {
        U_ASSERT(a.d1() == d1());
        लागि (int32_t i = 0; i < d1(); i++) {
            data_[i] = a.get(i);
        }
        फर्काऊ *एतत्;
    }

    // Apply tanh to all the elements in the array.
    अन्तःस्थ Array1D& tanh() {
        फर्काऊ tanh(*एतत्);
    }

    // Apply tanh of a and store into this array.
    अन्तःस्थ Array1D& tanh(अचर Array1D& a) {
        U_ASSERT(a.d1() == d1());
        लागि (int32_t i = 0; i < d1_; i++) {
            data_[i] = std::tanh(a.get(i));
        }
        फर्काऊ *एतत्;
    }

    // Apply sigmoid to all the elements in the array.
    अन्तःस्थ Array1D& sigmoid() {
        लागि (int32_t i = 0; i < d1_; i++) {
            data_[i] = 1.0f/(1.0f + expf(-data_[i]));
        }
        फर्काऊ *एतत्;
    }

    अन्तःस्थ Array1D& clear() {
        uprv_memset(data_, 0, d1_ * परिमाण(भिन्न));
        फर्काऊ *एतत्;
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

वर्ग Array2D : सार्वजनिक ReadArray2D {
सार्वजनिक:
    Array2D() : memory_(nullptr), data_(nullptr), d1_(0), d2_(0) {}
    Array2D(int32_t d1, int32_t d2, UErrorCode &status)
        : memory_(uprv_malloc(d1 * d2 * परिमाण(भिन्न))),
          data_(static_cast<भिन्न*>(memory_)), d1_(d1), d2_(d2) {
        यदि (U_SUCCESS(status)) {
            यदि (memory_ == nullptr) {
                status = U_MEMORY_ALLOCATION_ERROR;
                फर्काऊ;
            }
            clear();
        }
    }
    आभास ~Array2D();

    // ReadArray2D methods.
    आभास int32_t d1() अचर override { फर्काऊ d1_; }
    आभास int32_t d2() अचर override { फर्काऊ d2_; }
    आभास भिन्न get(int32_t i, int32_t j) अचर override {
        U_ASSERT(i < d1_);
        U_ASSERT(j < d2_);
        फर्काऊ data_[i * d2_ + j];
    }

    अन्तःस्थ Array1D row(int32_t i) अचर {
        U_ASSERT(i < d1_);
        फर्काऊ Array1D(data_ + i * d2_, d2_);
    }

    अन्तःस्थ Array2D& clear() {
        uprv_memset(data_, 0, d1_ * d2_ * परिमाण(भिन्न));
        फर्काऊ *एतत्;
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
    अचर char16_t* fName;
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
    यदि (U_FAILURE(status)) {
        फर्काऊ;
    }
    यदि (IEEE_754 != 1) {
        status = U_UNSUPPORTED_ERROR;
        फर्काऊ;
    }
    LocalUResourceBundlePointer embeddings_res(
        ures_get_By_साँचो(rb, "embeddings", nullptr, &status));
    int32_t embedding_size = ures_getInt(embeddings_res.getAlias(), &status);
    LocalUResourceBundlePointer hunits_res(
        ures_get_By_साँचो(rb, "hunits", nullptr, &status));
    यदि (U_FAILURE(status)) फर्काऊ;
    int32_t hunits = ures_getInt(hunits_res.getAlias(), &status);
    अचर char16_t* type = ures_get_String_By_साँचो(rb, "type", nullptr, &status);
    यदि (U_FAILURE(status)) फर्काऊ;
    यदि (u_strCompare(type, -1, u"codepoints", -1, असत्य) == 0) {
        fType = CODE_POINTS;
    } अन्यथा यदि (u_strCompare(type, -1, u"graphclust", -1, असत्य) == 0) {
        fType = GRAPHEME_CLUSTER;
    }
    fName = ures_get_String_By_साँचो(rb, "model", nullptr, &status);
    LocalUResourceBundlePointer dataRes(ures_get_By_साँचो(rb, "data", nullptr, &status));
    यदि (U_FAILURE(status)) फर्काऊ;
    int32_t data_len = 0;
    अचर int32_t* data = ures_getIntVector(dataRes.getAlias(), &data_len, &status);
    fDict = uhash_open(uhash_hashUChars, uhash_compareUChars, nullptr, &status);

    थाक_UResource_Bundle थाक_Temp_Bundle;
    Resource_Data_मान value;
    ures_get_मान_With_Fallback(rb, "dict", थाक_Temp_Bundle.getAlias(), value, status);
    ResourceArray stringArray = value.getArray(status);
    int32_t अङ्क_index = stringArray.getSize();
    यदि (U_FAILURE(status)) { फर्काऊ; }

    // put dict into hash
    int32_t stringLength;
    लागि (int32_t idx = 0; idx < अङ्क_index; idx++) {
        stringArray.get_मान(idx, value);
        अचर char16_t* str = value.getString(stringLength, status);
        uhash_putiAllowZero(fDict, (शून्य*)str, idx, &status);
        यदि (U_FAILURE(status)) फर्काऊ;
#ifdef LSTM_VECTORIZER_DEBUG
        म_लिखो("Assign [");
        जबसम्म (*str != 0x0000) {
            म_लिखो("U+%04x ", *str);
            str++;
        }
        म_लिखो("] map to %d\n", idx-1);
#endif
    }
    int32_t mat1_size = (अङ्क_index + 1) * embedding_size;
    int32_t mat2_size = embedding_size * 4 * hunits;
    int32_t mat3_size = hunits * 4 * hunits;
    int32_t mat4_size = 4 * hunits;
    int32_t mat5_size = mat2_size;
    int32_t mat6_size = mat3_size;
    int32_t mat7_size = mat4_size;
    int32_t mat8_size = 2 * hunits * 4;
#यदि U_DEBUG
    int32_t mat9_size = 4;
    U_ASSERT(data_len == mat1_size + mat2_size + mat3_size + mat4_size + mat5_size +
        mat6_size + mat7_size + mat8_size + mat9_size);
#endif

    fEmbedding.init(data, (अङ्क_index + 1), embedding_size);
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

वर्ग Vectorizer : सार्वजनिक UMemory {
सार्वजनिक:
    Vectorizer(UHashtable* dict) : fDict(dict) {}
    आभास ~Vectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) अचर = 0;
रक्षित:
    int32_t stringToIndex(अचर char16_t* str) अचर {
        UBool found = असत्य;
        int32_t ret = uhash_getiAndFound(fDict, (अचर शून्य*)str, &found);
        यदि (!found) {
            ret = fDict->count;
        }
#ifdef LSTM_VECTORIZER_DEBUG
        म_लिखो("[");
        जबसम्म (*str != 0x0000) {
            म_लिखो("U+%04x ", *str);
            str++;
        }
        म_लिखो("] map to %d\n", ret);
#endif
        फर्काऊ ret;
    }

गुप्त:
    UHashtable* fDict;
};

Vectorizer::~Vectorizer()
{
}

वर्ग CodePointsVectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    CodePointsVectorizer(UHashtable* dict) : Vectorizer(dict) {}
    आभास ~CodePointsVectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) अचर override;
};

CodePointsVectorizer::~CodePointsVectorizer()
{
}

शून्य CodePointsVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) अचर
{
    यदि (offsets.ensureCapacity(endPos - startPos, status) &&
            indices.ensureCapacity(endPos - startPos, status)) {
        यदि (U_FAILURE(status)) फर्काऊ;
        utext_setNativeIndex(text, startPos);
        int32_t current;
        char16_t str[2] = {0, 0};
        जबसम्म (U_SUCCESS(status) &&
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

वर्ग GraphemeClusterVectorizer : सार्वजनिक Vectorizer {
सार्वजनिक:
    GraphemeClusterVectorizer(UHashtable* dict)
        : Vectorizer(dict)
    {
    }
    आभास ~GraphemeClusterVectorizer();
    आभास शून्य vectorize(UText *text, int32_t startPos, int32_t endPos,
                           UVector32 &offsets, UVector32 &indices,
                           UErrorCode &status) अचर override;
};

GraphemeClusterVectorizer::~GraphemeClusterVectorizer()
{
}

constexpr int32_t MAX_GRAPHEME_CLSTER_LENGTH = 10;

शून्य GraphemeClusterVectorizer::vectorize(
    UText *text, int32_t startPos, int32_t endPos,
    UVector32 &offsets, UVector32 &indices, UErrorCode &status) अचर
{
    यदि (U_FAILURE(status)) फर्काऊ;
    यदि (!offsets.ensureCapacity(endPos - startPos, status) ||
            !indices.ensureCapacity(endPos - startPos, status)) {
        फर्काऊ;
    }
    यदि (U_FAILURE(status)) फर्काऊ;
    LocalPointer<BreakIterator> graphemeIter(BreakIterator::createCharacterInstance(Locale(), status));
    यदि (U_FAILURE(status)) फर्काऊ;
    graphemeIter->setText(text, status);
    यदि (U_FAILURE(status)) फर्काऊ;

    यदि (startPos != 0) {
        graphemeIter->preceding(startPos);
    }
    int32_t last = startPos;
    int32_t current = startPos;
    char16_t str[MAX_GRAPHEME_CLSTER_LENGTH];
    जबसम्म ((current = graphemeIter->next()) != BreakIterator::DONE) {
        यदि (current >= endPos) {
            रोक;
        }
        यदि (current > startPos) {
            utext_extract(text, last, current, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
            यदि (U_FAILURE(status)) फर्काऊ;
            offsets.addElement(last, status);
            indices.addElement(stringToIndex(str), status);
            यदि (U_FAILURE(status)) फर्काऊ;
        }
        last = current;
    }
    यदि (U_FAILURE(status) || last >= endPos) {
        फर्काऊ;
    }
    utext_extract(text, last, endPos, str, MAX_GRAPHEME_CLSTER_LENGTH, &status);
    यदि (U_SUCCESS(status)) {
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
    अचर ReadArray2D& W, अचर ReadArray2D& U, अचर ReadArray1D& b,
    अचर ReadArray1D& x, Array1D& h, Array1D& c,
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
स्थिर अचर int32_t MIN_WORD = 2;

// Minimum number of characters for two words
स्थिर अचर int32_t MIN_WORD_SPAN = MIN_WORD * 2;

int32_t
LSTMBreakEngine::divideUpDictionaryRange( UText *text,
                                                int32_t startPos,
                                                int32_t endPos,
                                                UVector32 &foundBreaks,
                                                UBool /* isPhraseBreaking */,
                                                UErrorCode& status) अचर {
    यदि (U_FAILURE(status)) फर्काऊ 0;
    int32_t beginFoundBreakSize = foundBreaks.size();
    utext_setNativeIndex(text, startPos);
    utext_moveIndex32(text, MIN_WORD_SPAN);
    यदि (utext_getNativeIndex(text) >= endPos) {
        फर्काऊ 0;       // Not enough characters for two words
    }
    utext_setNativeIndex(text, startPos);

    UVector32 offsets(status);
    UVector32 indices(status);
    यदि (U_FAILURE(status)) फर्काऊ 0;
    fVectorizer->vectorize(text, startPos, endPos, offsets, indices, status);
    यदि (U_FAILURE(status)) फर्काऊ 0;
    int32_t* offsetsBuf = offsets.getBuffer();
    int32_t* indicesBuf = indices.getBuffer();

    int32_t आगत_seq_len = indices.size();
    int32_t hunits = fData->fForwardU.d1();

    // ----- Begin of all the Array memory allocation needed for this function
    // Allocate temp array used inside compute()
    Array1D ifco(4 * hunits, status);

    Array1D c(hunits, status);
    Array1D logp(4, status);

    // TODO: limit size of hBackward. If input_seq_len is too big, we could
    // run out of memory.
    // Backward LSTM
    Array2D hBackward(आगत_seq_len, hunits, status);

    // Allocate fbRow and slice the internal array in two.
    Array1D fbRow(2 * hunits, status);

    // ----- End of all the Array memory allocation needed for this function
    यदि (U_FAILURE(status)) फर्काऊ 0;

    // To save the needed memory usage, the following is different from the
    // Python or ICU4X implementation. We first perform the Backward LSTM
    // and then merge the iteration of the forward LSTM and the output layer
    // together because we only neetdto remember the h[t-1] for Forward LSTM.
    लागि (int32_t i = आगत_seq_len - 1; i >= 0; i--) {
        Array1D hRow = hBackward.row(i);
        यदि (i != आगत_seq_len - 1) {
            hRow.assign(hBackward.row(i+1));
        }
#ifdef LSTM_DEBUG
        म_लिखो("hRow %d\n", i);
        hRow.print();
        म_लिखो("indicesBuf[%d] = %d\n", i, indicesBuf[i]);
        म_लिखो("fData->fEmbedding.row(indicesBuf[%d]):\n", i);
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
    लागि (int32_t i = 0; i < आगत_seq_len; i++) {
#ifdef LSTM_DEBUG
        म_लिखो("forwardRow %d\n", i);
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
        म_लिखो("backwardRow %d\n", i);
        backwardRow.print();
        म_लिखो("logp %d\n", i);
        logp.print();
#endif  // LSTM_DEBUG

        // current = argmax(logp)
        LSTMClass current = static_cast<LSTMClass>(logp.maxIndex());
        // BIES logic.
        यदि (current == BEGIN || current == SINGLE) {
            यदि (i != 0) {
                foundBreaks.addElement(offsetsBuf[i], status);
                यदि (U_FAILURE(status)) फर्काऊ 0;
            }
        }
    }
    फर्काऊ foundBreaks.size() - beginFoundBreakSize;
}

Vectorizer* createVectorizer(अचर LSTMData* data, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    विकल्प (data->fType) {
        पक्ष CODE_POINTS:
            फर्काऊ नूतन CodePointsVectorizer(data->fDict);
            रोक;
        पक्ष GRAPHEME_CLUSTER:
            फर्काऊ नूतन GraphemeClusterVectorizer(data->fDict);
            रोक;
        स्वभाव:
            रोक;
    }
    UPRV_UNREACHABLE_EXIT;
}

LSTMBreakEngine::LSTMBreakEngine(अचर LSTMData* data, अचर UnicodeSet& set, UErrorCode &status)
    : DictionaryBreakEngine(), fData(data), fVectorizer(createVectorizer(fData, status))
{
    यदि (U_FAILURE(status)) {
      fData = nullptr;  // If failure, we should not delete fData in destructor because the caller will do so.
      फर्काऊ;
    }
    setCharacters(set);
}

LSTMBreakEngine::~LSTMBreakEngine() {
    delete fData;
    delete fVectorizer;
}

अचर char16_t* LSTMBreakEngine::name() अचर {
    फर्काऊ fData->fName;
}

UnicodeString defaultLSTM(UScriptCode script, UErrorCode& status) {
    // open root from brkitr tree.
    UResourceBundle *b = ures_open(U_ICUDATA_BRKITR, "", &status);
    b = ures_get_By_साँचो_With_Fallback(b, "lstm", b, &status);
    UnicodeString result = ures_get_Unicode_String_By_साँचो(b, uscript_getShortName(script), &status);
    ures_close(b);
    फर्काऊ result;
}

U_CAPI अचर LSTMData* U_EXPORT2 CreateLSTMDataForScript(UScriptCode script, UErrorCode& status)
{
    यदि (script != USCRIPT_KHMER && script != USCRIPT_LAO && script != USCRIPT_MYANMAR && script != USCRIPT_THAI) {
        फर्काऊ nullptr;
    }
    UnicodeString name = defaultLSTM(script, status);
    यदि (U_FAILURE(status)) फर्काऊ nullptr;
    CharString namebuf;
    namebuf.appendInvariantChars(name, status).truncate(namebuf.lastIndexOf('.'));

    LocalUResourceBundlePointer rb(
        ures_openDirect(U_ICUDATA_BRKITR, namebuf.data(), &status));
    यदि (U_FAILURE(status)) फर्काऊ nullptr;

    फर्काऊ CreateLSTMData(rb.orphan(), status);
}

U_CAPI अचर LSTMData* U_EXPORT2 CreateLSTMData(UResourceBundle* rb, UErrorCode& status)
{
    यदि (U_FAILURE(status)) {
        फर्काऊ nullptr;
    }
    अचर LSTMData* result = नूतन LSTMData(rb, status);
    यदि (U_FAILURE(status)) {
        delete result;
        फर्काऊ nullptr;
    }
    फर्काऊ result;
}

U_CAPI अचर LanguageBreakEngine* U_EXPORT2
CreateLSTMBreakEngine(UScriptCode script, अचर LSTMData* data, UErrorCode& status)
{
    UnicodeString unicodeSetString;
    विकल्प(script) {
        पक्ष USCRIPT_THAI:
            unicodeSetString = UnicodeString(u"[[:Thai:]&[:LineBreak=SA:]]");
            रोक;
        पक्ष USCRIPT_MYANMAR:
            unicodeSetString = UnicodeString(u"[[:Mymr:]&[:LineBreak=SA:]]");
            रोक;
        स्वभाव:
            delete data;
            फर्काऊ nullptr;
    }
    UnicodeSet unicodeSet;
    unicodeSet.applyPattern(unicodeSetString, status);
    अचर LanguageBreakEngine* engine = नूतन LSTMBreakEngine(data, unicodeSet, status);
    यदि (U_FAILURE(status) || engine == nullptr) {
        यदि (engine != nullptr) {
            delete engine;
        } अन्यथा {
            status = U_MEMORY_ALLOCATION_ERROR;
        }
        फर्काऊ nullptr;
    }
    फर्काऊ engine;
}

U_CAPI शून्य U_EXPORT2 DeleteLSTMData(अचर LSTMData* data)
{
    delete data;
}

U_CAPI अचर char16_t* U_EXPORT2 LSTMDataName(अचर LSTMData* data)
{
    फर्काऊ data->fName;
}

U_NAMESPACE_END

#endif /* #if !UCONFIG_NO_BREAK_ITERATION */
