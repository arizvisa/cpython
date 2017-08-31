#include <windows.h>
#include <shlwapi.h>

static DWORD
GetHInstancePathW(HINSTANCE hModule, wchar_t** result)
{
    WCHAR path[MAX_PATH+1]; DWORD cb;
    DWORD e; wchar_t* pw;

    if (result == NULL)
        return ERROR_INVALID_PARAMETER;

    cb = GetModuleFileNameW(hModule, path, MAX_PATH); e = GetLastError();
    if (e != ERROR_SUCCESS || cb >= MAX_PATH)
        return e;

    path[cb] = L"\0";
    pw = _wcsdup(path);
    if (pw == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    *result = pw;
    return ERROR_SUCCESS;
}

static DWORD
GetHInstanceWorkingDirectoryW(HINSTANCE hInstance, wchar_t** result)
{
    wchar_t* pw;
    DWORD e;

    if (result == NULL)
        return ERROR_INVALID_PARAMETER;

    e = GetHInstancePathW(hInstance, &pw);
    if (e != ERROR_SUCCESS)
        return e;

    if (!PathRemoveFileSpecW(pw))
        return ERROR_PATH_NOT_FOUND;

    *result = pw;
    return ERROR_SUCCESS;
}

static DWORD
GetHInstanceWorkingDirectory(HINSTANCE hInstance, char** result)
{
    wchar_t* pw = NULL; char* p = NULL;
    size_t pwlen; size_t plen;
    DWORD e; errno_t err;

    e = GetHInstanceWorkingDirectoryW(hInstance, &pw);
    if (e != ERROR_SUCCESS)
        goto fail;
    pwlen = wcslen(pw);

    e = ERROR_NOT_ENOUGH_MEMORY;
    plen = pwlen + strlen(localpath);
    p = calloc(plen, sizeof(*p));
    if (p == NULL)
        goto fail;

    e = ERROR_INVALID_DATA;
    err = wcstombs_s(&plen, p, plen, pw, pwlen);
    if (err)
        goto fail;

    *result = p;
    free(pw);
    return ERROR_SUCCESS;

fail:
    if (pw) free(pw);
    if (p) free(p);
    return e;
}

static DWORD
GetHInstanceRelativeDirectory(HINSTANCE hInstance, const char* localpath, char** result)
{
    wchar_t* pw = NULL; char* p = NULL;
    size_t pwlen; size_t plen;
    DWORD e; errno_t err;

    if (result == NULL)
        return ERROR_INVALID_PARAMETER;

    e = GetHInstanceWorkingDirectoryW(hInstance, &pw);
    if (e != ERROR_SUCCESS)
        goto fail;
    pwlen = wcslen(pw) + sizeof("");

    e = ERROR_NOT_ENOUGH_MEMORY;
    plen = pwlen + strlen(localpath);
    p = calloc(plen, sizeof(*p));
    if (p == NULL)
        goto fail;

    e = ERROR_INVALID_DATA;
    err = wcstombs_s(&plen, p, plen, pw, pwlen);
    if (err)
        goto fail;

    err = strncat_s(p, plen, localpath, _TRUNCATE);
    if (err)
        goto fail;

    *result = p;
    free(pw);
    return ERROR_SUCCESS;

fail:
    if (pw) free(pw);
    if (p) free(p);
    return e;
}
