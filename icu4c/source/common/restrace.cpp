// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#अन्तर्भाव "unicode/utypes.h"

#यदि U_ENABLE_TRACING

#अन्तर्भाव "restrace.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = स्वभाव;

शून्य ResourceTracer::trace(ध्रुव वर्ण* resType) ध्रुव {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ध्रुव वर्ण kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, परिमाण(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

शून्य ResourceTracer::traceOpen() ध्रुव {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ध्रुव {
    यदि (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } अन्यथा {
        fParent->getFilePath(output, status);
    }
    प्रत्यागम output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ध्रुव {
    यदि (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } अन्यथा {
        fParent->getResPath(output, status);
    }
    यदि (f_किल्ली) {
        output.append('/', status);
        output.append(f_किल्ली, status);
    }
    यदि (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_संख्या(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    प्रत्यागम output;
}

शून्य FileTracer::traceOpen(ध्रुव वर्ण* path, ध्रुव वर्ण* type, ध्रुव वर्ण* name) {
    यदि (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } अन्यथा {
        traceOpenDataFile(path, type, name);
    }
}

शून्य FileTracer::traceOpenDataFile(ध्रुव वर्ण* path, ध्रुव वर्ण* type, ध्रुव वर्ण* name) {
    UTRACE_ENTRY(UTRACE_UDATA_DATA_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append('.', status);
    filePath.append(type, status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

शून्य FileTracer::traceOpenResFile(ध्रुव वर्ण* path, ध्रुव वर्ण* name) {
    UTRACE_ENTRY(UTRACE_UDATA_RES_FILE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    filePath.append(path, status);
    filePath.append('/', status);
    filePath.append(name, status);
    filePath.append(".res", status);

    UTRACE_DATA1(UTRACE_VERBOSE, "%s", filePath.data());
    UTRACE_EXIT_STATUS(status);
}

U_NAMESPACE_END

#endif // U_ENABLE_TRACING
