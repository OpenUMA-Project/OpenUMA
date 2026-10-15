#include <stdio.h>
#include <stdint.h>
#include <windows.h>

int main(int argc, char* argv[]) {
    const char* filepath = (argc > 1) ? argv[1] : "stories15M.bin";

    // 1. Open the file
    HANDLE hFile = CreateFileA(
        filepath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file. Error code: %lu\n", GetLastError());
        return 1;
    }

    // 2. Check that the file is at least 28 bytes
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(hFile, &fileSize) || fileSize.QuadPart < (LONGLONG)(7 * sizeof(int32_t))) {
        fprintf(stderr, "File is too small to contain header.\n");
        CloseHandle(hFile);
        return 1;
    }

    // 3. Create a file mapping object
    HANDLE hMap = CreateFileMappingA(
        hFile,
        NULL,
        PAGE_READONLY,
        0, 0,           // 0, 0 means map the full file size
        NULL
    );
    if (hMap == NULL) {
        printf("CreateFileMapping failed. Error code: %lu\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // 4. Map the view into memory (gives you the pointer)
    void* data = MapViewOfFile(
        hMap,
        FILE_MAP_READ,
        0, 0,           // Offset 0
        0               // Number of bytes: 0 maps the entire file
    );

    // The mapping handle can be closed immediately; the view stays active
    CloseHandle(hMap);

    if (data == NULL) {
        printf("MapViewOfFile failed. Error code: %lu\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }

    // 5. Cast to int32_t* and read the header numbers
    int32_t* h = (int32_t*)data;

    printf("dim:         %d\n", h[0]);
    printf("hidden_dim:  %d\n", h[1]);
    printf("n_layers:    %d\n", h[2]);
    printf("n_heads:     %d\n", h[3]);
    printf("n_kv_heads:  %d\n", h[4]);
    printf("vocab_size:  %d\n", h[5]);
    printf("seq_len:     %d\n", h[6]);

    if (h[5] > 0) {
        printf("Classifier weights: shared (vocab_size %d > 0)\n", h[5]);
    }
    else {
        printf("Classifier weights: separate (vocab_size %d < 0)\n", h[5]);
    }

    // 6. Cleanup
    UnmapViewOfFile(data);
    CloseHandle(hFile);

    return 0;
}