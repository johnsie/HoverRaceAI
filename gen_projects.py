#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import xml.dom.minidom as minidom
import os

def create_project_file(proj_path, guid, macro_name, source_files, project_deps, link_libs):
    """Generate a .vcxproj file with proper XML structure"""
    
    root = ET.Element('Project')
    root.set('DefaultTargets', 'Build')
    root.set('ToolsVersion', '15.0')
    root.set('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')
    
    # ProjectConfigurations
    ig_configs = ET.SubElement(root, 'ItemGroup')
    ig_configs.set('Label', 'ProjectConfigurations')
    
    for config_name in ['Debug', 'Release']:
        for platform in ['Win32']:
            pc = ET.SubElement(ig_configs, 'ProjectConfiguration')
            pc.set('Include', f'{config_name}|{platform}')
            ET.SubElement(pc, 'Configuration').text = config_name
            ET.SubElement(pc, 'Platform').text = platform
    
    # Globals
    pg_globals = ET.SubElement(root, 'PropertyGroup')
    pg_globals.set('Label', 'Globals')
    ET.SubElement(pg_globals, 'ProjectGuid').text = guid
    ET.SubElement(pg_globals, 'ProjectName').text = os.path.basename(os.path.dirname(proj_path))
    ET.SubElement(pg_globals, 'WindowsTargetPlatformVersion').text = '10.0'
    
    # Default props import
    ET.SubElement(root, 'Import').set('Project', '$(VCTargetsPath)\\Microsoft.Cpp.Default.props')
    
    # Configuration-specific settings
    for config_name in ['Debug', 'Release']:
        pg = ET.SubElement(root, 'PropertyGroup')
        pg.set('Label', 'Configuration')
        pg.set('Condition', f"'$(Configuration)|$(Platform)'=='{config_name}|Win32'")
        ET.SubElement(pg, 'ConfigurationType').text = 'DynamicLibrary'
        ET.SubElement(pg, 'PlatformToolset').text = 'v143'
        ET.SubElement(pg, 'CharacterSet').text = 'MultiByte'
        ET.SubElement(pg, 'UseOfMfc').text = 'Dynamic'
    
    # Props import
    ET.SubElement(root, 'Import').set('Project', '$(VCTargetsPath)\\Microsoft.Cpp.props')
    
    # PropertySheets
    igs = ET.SubElement(root, 'ImportGroup')
    igs.set('Label', 'PropertySheets')
    imp = ET.SubElement(igs, 'Import')
    imp.set('Project', '$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')
    imp.set('Condition', "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')")
    imp.set('Label', 'LocalAppDataPlatform')
    
    # UserMacros
    ET.SubElement(root, 'PropertyGroup').set('Label', 'UserMacros')
    
    # Output paths
    for config_name in ['Debug', 'Release']:
        pg = ET.SubElement(root, 'PropertyGroup')
        pg.set('Condition', f"'$(Configuration)|$(Platform)'=='{config_name}|Win32'")
        ET.SubElement(pg, 'OutDir').text = '$(SolutionDir)$(Configuration)\\'
        ET.SubElement(pg, 'IntDir').text = '$(Configuration)\\'
    
    # ItemDefinitionGroup (compiler/linker settings)
    for config_name in ['Debug', 'Release']:
        idg = ET.SubElement(root, 'ItemDefinitionGroup')
        idg.set('Condition', f"'$(Configuration)|$(Platform)'=='{config_name}|Win32'")
        
        cc = ET.SubElement(idg, 'ClCompile')
        if config_name == 'Release':
            preprocessor = f'WIN32;NDEBUG;_WINDOWS;{macro_name};_AFXDLL;%(PreprocessorDefinitions)'
        else:
            preprocessor = f'WIN32;_DEBUG;_WINDOWS;{macro_name};_AFXDLL;%(PreprocessorDefinitions)'
        ET.SubElement(cc, 'PreprocessorDefinitions').text = preprocessor
        ET.SubElement(cc, 'WarningLevel').text = 'Level4'
        ET.SubElement(cc, 'MultiProcessorCompilation').text = 'true'
        
        lk = ET.SubElement(idg, 'Link')
        if link_libs:
            ET.SubElement(lk, 'AdditionalDependencies').text = link_libs + ';%(AdditionalDependencies)'
        ET.SubElement(lk, 'AdditionalLibraryDirectories').text = 'c:\\originalhr\\HoverRace\\NetTarget\\Game2\\Release;%(AdditionalLibraryDirectories)'
    
    # Project dependencies
    if project_deps:
        ig_deps = ET.SubElement(root, 'ItemGroup')
        for dep_include, dep_guid in project_deps:
            pr = ET.SubElement(ig_deps, 'ProjectReference')
            pr.set('Include', dep_include)
            ET.SubElement(pr, 'Project').text = dep_guid
    
    # Source files
    ig_src = ET.SubElement(root, 'ItemGroup')
    for src_file in source_files:
        ET.SubElement(ig_src, 'ClCompile').set('Include', src_file)
    
    # Header files
    ig_hdr = ET.SubElement(root, 'ItemGroup')
    ET.SubElement(ig_hdr, 'ClInclude').set('Include', '*.h')
    
    # Targets import
    ET.SubElement(root, 'Import').set('Project', '$(VCTargetsPath)\\Microsoft.Cpp.targets')
    
    # Extension targets
    ig_ext = ET.SubElement(root, 'ImportGroup')
    ig_ext.set('Label', 'ExtensionTargets')
    
    # Write XML to file
    os.makedirs(os.path.dirname(proj_path), exist_ok=True)
    
    with open(proj_path, 'w', encoding='utf-8') as f:
        f.write('<?xml version="1.0" encoding="utf-8"?>\n')
        f.write(ET.tostring(root, encoding='unicode'))

# Define projects
projects_config = {
    'Util': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\Util\\Util.vcxproj',
        'guid': '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}',
        'macro': 'MR_UTIL',
        'sources': ['DllObjectFactory.cpp', 'FuzzyLogic.cpp', 'Profiler.cpp', 'RecordFile.cpp', 'StdAfx.cpp', 'StrRes.cpp', 'WorldCoordinates.cpp'],
        'deps': [],
        'linklibs': ''
    },
    'VideoServices': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\VideoServices\\VideoServices.vcxproj',
        'guid': '{158869E8-53E8-4DC1-B41F-B1C996211E2B}',
        'macro': 'MR_VIDEO_SERVICES',
        'sources': ['DllMain.cpp', 'StdAfx.cpp', 'VideoEngine.cpp'],
        'deps': [('..\\Util\\Util.vcxproj', '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}')],
        'linklibs': 'Util.lib'
    },
    'Model': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\Model\\Model.vcxproj',
        'guid': '{C0254587-DA7D-4338-9ECB-B614B05E5E1A}',
        'macro': 'MR_MODEL',
        'sources': ['DllMain.cpp', 'StdAfx.cpp'],
        'deps': [
            ('..\\Util\\Util.vcxproj', '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}'),
            ('..\\VideoServices\\VideoServices.vcxproj', '{158869E8-53E8-4DC1-B41F-B1C996211E2B}')
        ],
        'linklibs': 'Util.lib;VideoServices.lib'
    },
    'MainCharacter': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\MainCharacter\\MainCharacter.vcxproj',
        'guid': '{A55BC265-CFB8-46A7-958B-D3A48D9477A0}',
        'macro': 'MR_MAIN_CHARACTER',
        'sources': ['DllMain.cpp', 'StdAfx.cpp'],
        'deps': [
            ('..\\Util\\Util.vcxproj', '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}'),
            ('..\\Model\\Model.vcxproj', '{C0254587-DA7D-4338-9ECB-B614B05E5E1A}'),
            ('..\\VideoServices\\VideoServices.vcxproj', '{158869E8-53E8-4DC1-B41F-B1C996211E2B}')
        ],
        'linklibs': 'Util.lib;Model.lib;VideoServices.lib'
    },
    'ObjFacTools': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\ObjFacTools\\ObjFacTools.vcxproj',
        'guid': '{9A391771-FAFF-4D9D-9F6A-8AD3A9F73CA3}',
        'macro': 'MR_OBJ_FAC_TOOLS',
        'sources': ['DllMain.cpp', 'StdAfx.cpp'],
        'deps': [
            ('..\\Util\\Util.vcxproj', '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}'),
            ('..\\VideoServices\\VideoServices.vcxproj', '{158869E8-53E8-4DC1-B41F-B1C996211E2B}'),
            ('..\\Model\\Model.vcxproj', '{C0254587-DA7D-4338-9ECB-B614B05E5E1A}')
        ],
        'linklibs': 'Util.lib;VideoServices.lib;Model.lib'
    },
    'ColorTools': {
        'path': 'c:\\originalhr\\HoverRace\\NetTarget\\ColorTools\\ColorTools.vcxproj',
        'guid': '{F589CA0C-83C4-4F07-A3C7-F1684DF88369}',
        'macro': 'MR_COLOR_TOOLS',
        'sources': ['DllMain.cpp', 'StdAfx.cpp'],
        'deps': [('..\\Util\\Util.vcxproj', '{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}')],
        'linklibs': 'Util.lib'
    }
}

# Generate all project files
for proj_name, config in projects_config.items():
    try:
        create_project_file(
            config['path'],
            config['guid'],
            config['macro'],
            config['sources'],
            config['deps'],
            config['linklibs']
        )
        print(f'✓ Created {proj_name}.vcxproj')
    except Exception as e:
        print(f'✗ Failed to create {proj_name}.vcxproj: {e}')

print('Done!')
