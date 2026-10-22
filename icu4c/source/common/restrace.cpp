// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ U_ENABLE_TRACING

#ಅನ್ತರ್ಭಾವ "restrace.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "util.h"

U_NAMESPACE_BEGIN

ResourceTracer::~ResourceTracer() = ಸ್ವಭಾವ;

ಶೂನ್ಯ ResourceTracer::trace(ಧ್ರುವ ವರ್ಣ* resType) ಧ್ರುವ {
    U_ASSERT(fResB || fParent);
    UTRACE_ENTRY(UTRACE_UDATA_RESOURCE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    getFilePath(filePath, status);

    CharString resPath;
    getResPath(resPath, status);

    // The longest type ("intvector") is 9 chars
    ಧ್ರುವ ವರ್ಣ kSpaces[] = "         ";
    CharString format;
    format.append(kSpaces, ಪರಿಮಾಣ(kSpaces) - 1 - uprv_strlen(resType), status);
    format.append("(%s) %s @ %s", status);

    UTRACE_DATA3(UTRACE_VERBOSE,
        format.data(),
        resType,
        filePath.data(),
        resPath.data());
    UTRACE_EXIT_STATUS(status);
}

ಶೂನ್ಯ ResourceTracer::traceOpen() ಧ್ರುವ {
    U_ASSERT(fResB);
    UTRACE_ENTRY(UTRACE_UDATA_BUNDLE);
    UErrorCode status = U_ZERO_ERROR;

    CharString filePath;
    UTRACE_DATA1(UTRACE_VERBOSE, "%s", getFilePath(filePath, status).data());
    UTRACE_EXIT_STATUS(status);
}

CharString& ResourceTracer::getFilePath(CharString& output, UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (fResB) {
        // Note: if you get a segfault around here, check that ResourceTable and
        // ResourceArray instances outlive ResourceValue instances referring to
        // their contents:
        output.append(fResB->fData->fPath, status);
        output.append('/', status);
        output.append(fResB->fData->fName, status);
        output.append(".res", status);
    } ಅನ್ಯಥಾ {
        fParent->getFilePath(output, status);
    }
    ಪ್ರತ್ಯಾಗಮ output;
}

CharString& ResourceTracer::getResPath(CharString& output, UErrorCode& status) ಧ್ರುವ {
    ಯದಿ (fResB) {
        output.append('/', status);
        output.append(fResB->fResPath, status);
        // removing the trailing /
        U_ASSERT(output[output.length()-1] == '/');
        output.truncate(output.length()-1);
    } ಅನ್ಯಥಾ {
        fParent->getResPath(output, status);
    }
    ಯದಿ (f_ಕೀಲಿ) {
        output.append('/', status);
        output.append(f_ಕೀಲಿ, status);
    }
    ಯದಿ (fIndex != -1) {
        output.append('[', status);
        UnicodeString indexString;
        ICU_Utility::append_ಸಂಖ್ಯೆ(indexString, fIndex);
        output.appendInvariantChars(indexString, status);
        output.append(']', status);
    }
    ಪ್ರತ್ಯಾಗಮ output;
}

ಶೂನ್ಯ FileTracer::traceOpen(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* type, ಧ್ರುವ ವರ್ಣ* name) {
    ಯದಿ (uprv_strcmp(type, "res") == 0) {
        traceOpenResFile(path, name);
    } ಅನ್ಯಥಾ {
        traceOpenDataFile(path, type, name);
    }
}

ಶೂನ್ಯ FileTracer::traceOpenDataFile(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* type, ಧ್ರುವ ವರ್ಣ* name) {
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

ಶೂನ್ಯ FileTracer::traceOpenResFile(ಧ್ರುವ ವರ್ಣ* path, ಧ್ರುವ ವರ್ಣ* name) {
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
