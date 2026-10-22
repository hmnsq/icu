// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#समावेश "unicode/utypes.h"

#अगर U_ENABLE_TRACING

#समावेश "restrace.h"
#समावेश "charstr.h"
#समावेश "cstring.h"
#समावेश "utracimp.h"
#समावेश "uresimp.h"
#समावेश "uassert.h"
#समावेश "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = शेष;

व्योम ResourceTracer::trace(स्थिर अक्षर* resType) स्थिर {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    स्थिर अक्षर kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, माप(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.तथ्य(),
        resType,
        filePath.तथ्य(),
        resPath.तथ्य());
    UTRACE_EXIT_STATUS(status);
}

व्योम ResourceTracer::traceOpen() स्थिर {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).तथ्य());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) स्थिर {
    अगर (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->f_तथ्य->fPath, status);
        output.append('/', status);
        output.append(fResB->f_तथ्य->fName, status);
        output.append(".res", status);
    } अन्यथा {
        fParent->getFilePath(output, status);
    }
    वापस output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) स्थिर {
    अगर (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } अन्यथा {
        fParent->getResPath(output, status);
    }
    अगर (f_कुंजी) {
        output.append('/', status);
        output.append(f_कुंजी, status);
    }
    अगर (f_सूचकांक != -1) {
        output.append('[', status);
        UnicodeString सूचकांक_String;
        ICU_Utility::append_संख्या(सूचकांक_String, f_सूचकांक);
        output.appendInvariantChars(सूचकांक_String, status);
        output.append(']', status);
    }
    वापस output;
}

व्योम FileTracer::traceOpen(स्थिर अक्षर* path, स्थिर अक्षर* type, स्थिर अक्षर* name) {
    अगर (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } अन्यथा {
        trace_Open_तथ्य_File(path, type, name);
    }
}

व्योम FileTracer::trace_Open_तथ्य_File(स्थिर अक्षर* path, स्थिर अक्षर* type, स्थिर अक्षर* name) {
    UTRACE_ENTRY(UTRACE_UDATA_तथ्य_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.तथ्य());
    UTRACE_EXIT_STATUS(status);
}

व्योम FileTracer::traceOpenResFile(स्थिर अक्षर* path, स्थिर अक्षर* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.तथ्य());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#पूर्ण_अगर // U_ENABLE_TRACING
