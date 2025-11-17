#include "ResourceDecompiler.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    printf("=== HoverRace Resource Decompiler ===\n");
    printf("Version 1.0\n\n");

    if (argc < 3)
    {
        printf("Usage: ResourceDecompiler <input_library> <output_directory>\n\n");
        printf("Examples:\n");
        printf("  ResourceDecompiler game.dat output/\n");
        printf("  ResourceDecompiler actors.dat decompiled/actors/\n\n");
        printf("Supported input formats:\n");
        printf("  - HoverRace binary resource libraries (.dat files)\n\n");
        printf("Output formats:\n");
        printf("  - Meshes: .msh (text-based, HoverRace format)\n");
        printf("  - Bitmaps: .tga (bitmap images)\n");
        printf("  - Audio: .wav (WAV format)\n");
        printf("  - Sprites: .dat (raw sprite data)\n");
        return 1;
    }

    const char* lInputFile = argv[1];
    const char* lOutputDir = argv[2];

    printf("Input file: %s\n", lInputFile);
    printf("Output directory: %s\n\n", lOutputDir);

    MR_ResourceDecompiler lDecompiler;
    if (lDecompiler.Decompile(lInputFile, lOutputDir))
    {
        printf("\nDecompilation completed successfully!\n");
        return 0;
    }
    else
    {
        printf("\nDecompilation FAILED!\n");
        return 1;
    }
}
