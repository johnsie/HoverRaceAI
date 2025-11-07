powershell -Command @'
# Define projects
$projects = @{
    'Util' = @{ 
        GUID = '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}'
        Macro = 'MR_UTIL'
        Files = 'DllObjectFactory.cpp','FuzzyLogic.cpp','Profiler.cpp','RecordFile.cpp','StdAfx.cpp','StrRes.cpp','WorldCoordinates.cpp'
        Deps = @()
    }
    'VideoServices' = @{
        GUID = '{158869E8-53E8-4DC1-B41F-B1C996211E2B}'
        Macro = 'MR_VIDEO_SERVICES'
        Files = '2DViewPort.cpp','3DViewport.cpp','3DViewportRendering.cpp','Bitmap.cpp','ColorPalette.cpp','ColorTab.cpp','Patch.cpp','SoundServer.cpp','Sprite.cpp','StdAfx.cpp','VideoBuffer.cpp','VideoBuffer2DDraw.cpp'
        Deps = @('Util')
        LinkDeps = 'dsound.lib;winmm.lib;Util.lib'
    }
    'Model' = @{
        GUID = '{C0254587-DA7D-4338-9ECB-B614B05E5E1A}'
        Macro = 'MR_MODEL'
        Files = 'ConcreteShape.cpp','ContactEffect.cpp','FreeElementMovingHelper.cpp','GameSession.cpp','Level.cpp','MazeElement.cpp','PhysicalCollision.cpp','ShapeCollisions.cpp','Shapes.cpp','StdAfx.cpp'
        Deps = @('Util','VideoServices')
        LinkDeps = 'winmm.lib;Util.lib;VideoServices.lib'
    }
    'MainCharacter' = @{
        GUID = '{A55BC265-CFB8-46A7-958B-D3A48D9477A0}'
        Macro = 'MR_MAIN_CHARACTER'
        Files = 'MainCharacter.cpp','MainCharacterRenderer.cpp','StdAfx.cpp'
        Deps = @('Util','Model','VideoServices')
    }
    'ObjFacTools' = @{
        GUID = '{9A391771-FAFF-4D9D-9F6A-8AD3A9F73CA3}'
        Macro = 'MR_OBJ_FAC_TOOLS'
        Files = 'BitmapSurface.cpp','FreeElementBase.cpp','FreeElementBaseRenderer.cpp','ObjectFactoryData.cpp','ResActor.cpp','ResBitmap.cpp','ResourceLib.cpp','ResSound.cpp','ResSprite.cpp','SpriteHandle.cpp','StdAfx.cpp'
        Deps = @('Util','VideoServices','Model')
        LinkDeps = 'Model.lib;VideoServices.lib;Util.lib'
    }
    'ColorTools' = @{
        GUID = '{F589CA0C-83C4-4F07-A3C7-F1684DF88369}'
        Macro = 'MR_COLOR_TOOLS'
        Files = 'ColorTools.cpp','StdAfx.cpp'
        Deps = @('Util')
    }
}

$depGUIDs = @{
    'Util' = '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}'
    'VideoServices' = '{158869E8-53E8-4DC1-B41F-B1C996211E2B}'
    'Model' = '{C0254587-DA7D-4338-9ECB-B614B05E5E1A}'
    'MainCharacter' = '{A55BC265-CFB8-46A7-958B-D3A48D9477A0}'
    'ObjFacTools' = '{9A391771-FAFF-4D9D-9F6A-8AD3A9F73CA3}'
    'ColorTools' = '{F589CA0C-83C4-4F07-A3C7-F1684DF88369}'
}

foreach ($projName in $projects.Keys) {
    $proj = $projects[$projName]
    $projFile = "c:\originalhr\HoverRace\NetTarget\$projName\$projName.vcxproj"
    
    $xml = '<?xml version="1.0" encoding="utf-8"?>' + "`n"
    $xml += '<Project DefaultTargets="Build" ToolsVersion="15.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">' + "`n"
    $xml += '  <ItemGroup Label="ProjectConfigurations">' + "`n"
    $xml += '    <ProjectConfiguration Include="Debug|Win32"><Configuration>Debug</Configuration><Platform>Win32</Platform></ProjectConfiguration>' + "`n"
    $xml += '    <ProjectConfiguration Include="Release|Win32"><Configuration>Release</Configuration><Platform>Win32</Platform></ProjectConfiguration>' + "`n"
    $xml += '  </ItemGroup>' + "`n"
    $xml += "  <PropertyGroup Label=`"Globals`"><ProjectGuid>$($proj.GUID)</ProjectGuid><ProjectName>$projName</ProjectName><WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion></PropertyGroup>" + "`n"
    $xml += '  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />' + "`n"
    $xml += '  <PropertyGroup Label="Configuration" Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Debug|Win32'"'"'"><ConfigurationType>DynamicLibrary</ConfigurationType><PlatformToolset>v143</PlatformToolset><CharacterSet>MultiByte</CharacterSet><UseOfMfc>Dynamic</UseOfMfc></PropertyGroup>' + "`n"
    $xml += '  <PropertyGroup Label="Configuration" Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Release|Win32'"'"'"><ConfigurationType>DynamicLibrary</ConfigurationType><PlatformToolset>v143</PlatformToolset><CharacterSet>MultiByte</CharacterSet><UseOfMfc>Dynamic</UseOfMfc></PropertyGroup>' + "`n"
    $xml += '  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />' + "`n"
    $xml += '  <ImportGroup Label="PropertySheets"><Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('"'"'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props'"'"')" Label="LocalAppDataPlatform" /></ImportGroup>' + "`n"
    $xml += '  <PropertyGroup Label="UserMacros" />' + "`n"
    $xml += '  <PropertyGroup Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Debug|Win32'"'"'"><OutDir>$(SolutionDir)$(Configuration)\</OutDir><IntDir>$(Configuration)\</IntDir></PropertyGroup>' + "`n"
    $xml += '  <PropertyGroup Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Release|Win32'"'"'"><OutDir>$(SolutionDir)$(Configuration)\</OutDir><IntDir>$(Configuration)\</IntDir></PropertyGroup>' + "`n"
    $xml += '  <ItemDefinitionGroup Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Debug|Win32'"'"'"><ClCompile><PreprocessorDefinitions>WIN32;_DEBUG;_WINDOWS;' + $proj.Macro + ';_AFXDLL;%(PreprocessorDefinitions)</PreprocessorDefinitions><WarningLevel>Level4</WarningLevel><AdditionalIncludeDirectories>C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\include;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories><MultiProcessorCompilation>true</MultiProcessorCompilation></ClCompile><Link><AdditionalLibraryDirectories>c:\originalhr\HoverRace\Release;C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\lib\x86;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\lib\x86;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories></Link></ItemDefinitionGroup>' + "`n"
    $xml += '  <ItemDefinitionGroup Condition="'"'"'$(Configuration)|$(Platform)'"'"'=='"'"'Release|Win32'"'"'"><ClCompile><PreprocessorDefinitions>WIN32;NDEBUG;_WINDOWS;' + $proj.Macro + ';_AFXDLL;%(PreprocessorDefinitions)</PreprocessorDefinitions><WarningLevel>Level4</WarningLevel><AdditionalIncludeDirectories>C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\include;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories><MultiProcessorCompilation>true</MultiProcessorCompilation></ClCompile><Link><AdditionalDependencies>' + $proj.LinkDeps + '%(AdditionalDependencies)</AdditionalDependencies><AdditionalLibraryDirectories>c:\originalhr\HoverRace\Release;C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\lib\x86;C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\lib\x86;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories></Link></ItemDefinitionGroup>' + "`n"
    
    # Add project references
    if ($proj.Deps.Count -gt 0) {
        $xml += '  <ItemGroup>' + "`n"
        foreach ($dep in $proj.Deps) {
            $xml += "    <ProjectReference Include=`"..\$dep\$dep.vcxproj`"><Project>$($depGUIDs[$dep])</Project></ProjectReference>" + "`n"
        }
        $xml += '  </ItemGroup>' + "`n"
    }
    
    # Add source files
    $xml += '  <ItemGroup>' + "`n"
    foreach ($file in $proj.Files) {
        $xml += "    <ClCompile Include=`"$file`" />" + "`n"
    }
    $xml += '  </ItemGroup>' + "`n"
    $xml += '  <ItemGroup><ClInclude Include="*.h" /></ItemGroup>' + "`n"
    $xml += '  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />' + "`n"
    $xml += '  <ImportGroup Label="ExtensionTargets" />' + "`n"
    $xml += '</Project>' + "`n"
    
    # Write file WITHOUT BOM
    [System.IO.File]::WriteAllText($projFile, $xml, [System.Text.Encoding]::UTF8)
    Write-Host "Created $projName.vcxproj"
}
'@
