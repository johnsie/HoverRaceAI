// Simple TRK file analyzer
// Reads and verifies ClassicH.trk structure
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

#pragma pack(push, 1)

// Expected constants
#define MR_MAGIC_TRACK_NUMBER  82617  // 0x1427F (little-endian: 7F 14)
#define MR_FREE_TRACK          1

// CArchive write format in MFC stores integers with a leading byte for validation
// When you do archive >> variable, MFC reads:
//   - 1 byte: type/count indicator
//   - N bytes: data

// CString serialization: 
//   - Short strings (< 0xFFFF): writes 2-byte length + data
//   - Empty strings: 0xFFFF

struct RecordHeader {
    uint32_t offset;
    uint32_t size;
};

#pragma pack(pop)

void PrintHex(const uint8_t* data, int len, int lineLen = 16) {
    for (int i = 0; i < len; i++) {
        if (i % lineLen == 0 && i > 0) printf("\n");
        if (i % lineLen == 0) printf("%08X: ", i);
        printf("%02X ", data[i]);
    }
    printf("\n");
}

void PrintASCII(const uint8_t* data, int len) {
    for (int i = 0; i < len; i++) {
        if (data[i] >= 32 && data[i] < 127) {
            printf("%c", data[i]);
        } else {
            printf(".");
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* logFile = fopen("trk_analyzer.log", "w");
    if (!logFile) {
        fprintf(stderr, "Failed to open log file\n");
        return 1;
    }

    const char* filename = "Release\\Tracks\\ClassicH.trk";
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(logFile, "ERROR: Cannot open file: %s\n", filename);
        fprintf(stderr, "ERROR: Cannot open file: %s\n", filename);
        fclose(logFile);
        return 1;
    }

    fprintf(logFile, "=== TRK File Analyzer ===\n");
    fprintf(logFile, "File: %s\n\n", filename);

    // Read file size
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    rewind(f);

    fprintf(logFile, "File size: %ld bytes (0x%lX)\n\n", fileSize, fileSize);

    // Read header - should be copyright notice
    char headerBuf[256];
    fgets(headerBuf, sizeof(headerBuf), f);
    fprintf(logFile, "Header (first line): %s\n", headerBuf);
    fprintf(logFile, "Header length: %zu\n\n", strlen(headerBuf));

    // The actual file structure is a record table followed by records
    // Let me read the raw bytes and look for patterns

    rewind(f);
    uint8_t* fileData = (uint8_t*)malloc(fileSize);
    if (!fileData) {
        fprintf(logFile, "ERROR: Cannot allocate memory for file\n");
        fclose(f);
        fclose(logFile);
        return 1;
    }

    size_t bytesRead = fread(fileData, 1, fileSize, f);
    fprintf(logFile, "Bytes read: %zu\n", bytesRead);

    // Look for magic numbers
    uint32_t magicLE = ((MR_MAGIC_TRACK_NUMBER & 0xFF) << 8) | ((MR_MAGIC_TRACK_NUMBER >> 8) & 0xFF);  // Little endian
    fprintf(logFile, "Looking for magic number: 0x%X (little-endian: %02X %02X)\n\n", 
            MR_MAGIC_TRACK_NUMBER,
            MR_MAGIC_TRACK_NUMBER & 0xFF,
            (MR_MAGIC_TRACK_NUMBER >> 8) & 0xFF);

    fprintf(logFile, "First 512 bytes (hex):\n");
    PrintHex(fileData, (fileSize > 512) ? 512 : fileSize, 32);
    fprintf(logFile, "\n");

    // Try to find magic numbers in the file
    fprintf(logFile, "Searching for magic number %d (0x%X)...\n", MR_MAGIC_TRACK_NUMBER, MR_MAGIC_TRACK_NUMBER);
    int magicCount = 0;
    for (long i = 0; i < fileSize - 3; i++) {
        uint32_t val = *(uint32_t*)(fileData + i);
        if (val == MR_MAGIC_TRACK_NUMBER) {
            fprintf(logFile, "  Found at offset 0x%lX\n", i);
            magicCount++;
        }
    }
    fprintf(logFile, "Total occurrences: %d\n\n", magicCount);

    // Try to parse as MFC archive format
    // MFC archive starts with preamble: number of records, then record offsets
    fprintf(logFile, "=== Attempting to parse as Record File ===\n");
    
    // The RecordFile format seems to store:
    // [Header info] [Record 0 content] [Record 1 content] ...
    // With a table at the beginning

    // Let's look for patterns - typically files start with:
    // 1. Some header
    // 2. Number of records (4 bytes, little endian)
    // 3. Record offsets
    // 4. Record data

    fprintf(logFile, "\nBytes 20-40 (after copyright): ");
    PrintASCII(fileData + 20, 40);
    fprintf(logFile, "\n");

    fprintf(logFile, "\nAnalyzing content after copyright...\n");
    // Copyright ends around byte 23-25
    // After that should be record metadata

    // Let's dump more structured data
    fprintf(logFile, "\n=== Raw data analysis (first 256 bytes) ===\n");
    for (int i = 0; i < 256; i += 16) {
        fprintf(logFile, "Offset %04X: ", i);
        for (int j = 0; j < 16 && i + j < 256; j++) {
            fprintf(logFile, "%02X ", fileData[i + j]);
        }
        fprintf(logFile, " | ");
        for (int j = 0; j < 16 && i + j < 256; j++) {
            uint8_t c = fileData[i + j];
            fprintf(logFile, "%c", (c >= 32 && c < 127) ? c : '.');
        }
        fprintf(logFile, "\n");
    }

    // Check for specific patterns
    fprintf(logFile, "\n=== Pattern Analysis ===\n");
    
    // MFC CString serialization:
    // - Archive << stringVariable;
    // Writes: [length (4 bytes)] [characters]
    
    // Look for string "Level: Easy" which appears in track description
    const char* searchStr = "Level: Easy";
    char* found = (char*)memmem(fileData, fileSize, searchStr, strlen(searchStr));
    if (found) {
        long offset = found - (char*)fileData;
        fprintf(logFile, "Found string '%s' at offset 0x%lX\n", searchStr, offset);
    } else {
        fprintf(logFile, "String '%s' not found\n", searchStr);
    }

    free(fileData);
    fclose(f);

    fprintf(logFile, "\n=== Analysis Complete ===\n");
    fclose(logFile);

    // Print log to console
    f = fopen("trk_analyzer.log", "r");
    if (f) {
        char line[512];
        while (fgets(line, sizeof(line), f)) {
            printf("%s", line);
        }
        fclose(f);
    }

    return 0;
}
