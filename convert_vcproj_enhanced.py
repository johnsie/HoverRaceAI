#!/usr/bin/env python3
"""
Enhanced conversion of Visual Studio .vcproj files to .vcxproj format
with proper handling of compiler settings, includes, and MFC configuration
"""

import xml.etree.ElementTree as ET
from pathlib import Path
import re

def extract_vcproj_settings(vcproj_path):
    """Extract all settings from a .vcproj file"""
    root = ET.parse(vcproj_path).getroot()
    
    settings = {
        'name': root.get('Name', 'Project'),
        'guid': root.get('ProjectGUID', ''),
        'configs': {}
    }
    
    configurations = root.findall('.//Configuration')
    for cfg in configurations:
        cfg_name = cfg.get('Name', '')
        cfg_type = cfg.get('ConfigurationType', '1')
        use_mfc = cfg.get('UseOfMFC', '0')
        charset = cfg.get('CharacterSet', '0')
        
        cfg_info = {
            'name': cfg_name,
            'type': cfg_type,
            'use_mfc': use_mfc,  # 0=no, 1=static, 2=shared DLL
            'charset': charset,
            'includes': [],
            'defines': [],
            'libs': [],
            'lib_dirs': [],
            'runtime_lib': '1',  # default
        }
        
        # Extract compiler settings
        compiler = cfg.find('.//Tool[@Name="VCCLCompilerTool"]')
        if compiler is not None:
            # Additional includes
            includes = compiler.get('AdditionalIncludeDirectories', '')
            if includes:
                cfg_info['includes'].extend(includes.split(';'))
            
            # Preprocessor definitions
            defines = compiler.get('PreprocessorDefinitions', '')
            if defines:
                cfg_info['defines'].extend(defines.split(';'))
            
            # Runtime library
            cfg_info['runtime_lib'] = compiler.get('RuntimeLibrary', '1')
        
        # Extract linker settings
        linker = cfg.find('.//Tool[@Name="VCLinkerTool"]')
        if linker is not None:
            libs = linker.get('AdditionalDependencies', '')
            if libs:
                cfg_info['libs'].extend(libs.split(' '))
            
            lib_dirs = linker.get('AdditionalLibraryDirectories', '')
            if lib_dirs:
                cfg_info['lib_dirs'].extend(lib_dirs.split(';'))
        
        # Extract resource compiler
        rc = cfg.find('.//Tool[@Name="VCResourceCompilerTool"]')
        if rc is not None:
            defines = rc.get('PreprocessorDefinitions', '')
            if defines and '_AFXDLL' in defines and '_AFXDLL' not in cfg_info['defines']:
                cfg_info['defines'].append('_AFXDLL')
        
        settings['configs'][cfg_name] = cfg_info
    
    return settings

def create_vcxproj(settings, vcproj_path):
    """Create a .vcxproj file from extracted settings"""
    
    ns = 'http://schemas.microsoft.com/developer/msbuild/2003'
    ET.register_namespace('', ns)
    
    Project = ET.Element('Project', {
        'DefaultTargets': 'Build',
        'ToolsVersion': '14.0',
        'xmlns': ns
    })
    
    # ProjectConfigurations
    if settings['configs']:
        ProjectConfigurations = ET.SubElement(Project, 'ItemGroup', {'Label': 'ProjectConfigurations'})
        for cfg_name in sorted(settings['configs'].keys()):
            parts = cfg_name.split('|')
            plat = parts[0] if len(parts) > 0 else 'Win32'
            conf = parts[1] if len(parts) > 1 else 'Debug'
            
            ProjectConfiguration = ET.SubElement(ProjectConfigurations, 'ProjectConfiguration', 
                                                  {'Include': cfg_name})
            ET.SubElement(ProjectConfiguration, 'Configuration').text = conf
            ET.SubElement(ProjectConfiguration, 'Platform').text = plat
    
    # Globals
    Globals = ET.SubElement(Project, 'PropertyGroup', {'Label': 'Globals'})
    ET.SubElement(Globals, 'ProjectGuid').text = settings['guid']
    ET.SubElement(Globals, 'ProjectName').text = settings['name']
    ET.SubElement(Globals, 'WindowsTargetPlatformVersion').text = '10.0'
    
    # Import defaults
    ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.Default.props'})
    
    # Configuration property groups
    for cfg_name in sorted(settings['configs'].keys()):
        cfg = settings['configs'][cfg_name]
        
        prop_group = ET.SubElement(Project, 'PropertyGroup', {
            'Label': 'Configuration',
            'Condition': f"'$(Configuration)|$(Platform)'=='{cfg_name}'"
        })
        
        config_type = {
            '1': 'Application',
            '2': 'DynamicLibrary',
            '4': 'DynamicLibrary',
            '10': 'Utility',
        }.get(cfg['type'], 'Application')
        
        ET.SubElement(prop_group, 'ConfigurationType').text = config_type
        ET.SubElement(prop_group, 'PlatformToolset').text = 'v143'
        
        charset_map = {'0': 'NotSet', '1': 'Unicode', '2': 'MultiByte'}
        ET.SubElement(prop_group, 'CharacterSet').text = charset_map.get(cfg['charset'], 'MultiByte')
        
        if cfg['use_mfc'] == '2':  # Shared DLL
            ET.SubElement(prop_group, 'UseOfMfc').text = 'Dynamic'
        elif cfg['use_mfc'] == '1':  # Static
            ET.SubElement(prop_group, 'UseOfMfc').text = 'Static'
    
    # Import props
    ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.props'})
    
    # Property sheets
    property_sheets = ET.SubElement(Project, 'ImportGroup', {'Label': 'PropertySheets'})
    ET.SubElement(property_sheets, 'Import', {
        'Project': r'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props',
        'Condition': "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')",
        'Label': 'LocalAppDataPlatform'
    })
    
    # User macros
    ET.SubElement(Project, 'PropertyGroup', {'Label': 'UserMacros'})
    
    # Configuration-specific property groups for compiler settings
    for cfg_name in sorted(settings['configs'].keys()):
        cfg = settings['configs'][cfg_name]
        
        prop_group = ET.SubElement(Project, 'PropertyGroup', {
            'Condition': f"'$(Configuration)|$(Platform)'=='{cfg_name}'"
        })
        
        # Add output directories
        ET.SubElement(prop_group, 'OutDir').text = f'$(SolutionDir)$(Configuration)\\'
        ET.SubElement(prop_group, 'IntDir').text = f'$(Configuration)\\'
    
    # Item definition groups for compiler/linker settings
    for cfg_name in sorted(settings['configs'].keys()):
        cfg = settings['configs'][cfg_name]
        
        item_def_group = ET.SubElement(Project, 'ItemDefinitionGroup', {
            'Condition': f"'$(Configuration)|$(Platform)'=='{cfg_name}'"
        })
        
        # Compiler settings
        cl_compile = ET.SubElement(item_def_group, 'ClCompile')
        
        if cfg['includes']:
            inc_dirs = ';'.join(filter(None, cfg['includes']))
            ET.SubElement(cl_compile, 'AdditionalIncludeDirectories').text = f"{inc_dirs};%(AdditionalIncludeDirectories)"
        
        if cfg['defines']:
            defs = ';'.join(filter(None, cfg['defines']))
            ET.SubElement(cl_compile, 'PreprocessorDefinitions').text = f"{defs};%(PreprocessorDefinitions)"
        
        # Set warning level
        ET.SubElement(cl_compile, 'WarningLevel').text = 'Level4'
        
        # Linker settings
        link = ET.SubElement(item_def_group, 'Link')
        if cfg['libs']:
            libs = ' '.join(filter(None, cfg['libs']))
            ET.SubElement(link, 'AdditionalDependencies').text = f"{libs};%(AdditionalDependencies)"
        
        if cfg['lib_dirs']:
            lib_dirs = ';'.join(filter(None, cfg['lib_dirs']))
            ET.SubElement(link, 'AdditionalLibraryDirectories').text = f"{lib_dirs};%(AdditionalLibraryDirectories)"
    
    # Source files
    source_dir = Path(vcproj_path).parent
    cpp_files = sorted(source_dir.glob('*.cpp'))
    h_files = sorted(source_dir.glob('*.h'))
    
    if cpp_files:
        ClCompile = ET.SubElement(Project, 'ItemGroup')
        for cpp in cpp_files:
            ET.SubElement(ClCompile, 'ClCompile', {'Include': cpp.name})
    
    if h_files:
        ClInclude = ET.SubElement(Project, 'ItemGroup')
        for h in h_files:
            ET.SubElement(ClInclude, 'ClInclude', {'Include': h.name})
    
    # Target imports
    ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.targets'})
    ET.SubElement(Project, 'ImportGroup', {'Label': 'ExtensionTargets'})
    
    # Write the file
    vcxproj_path = Path(vcproj_path).with_suffix('.vcxproj')
    tree = ET.ElementTree(Project)
    tree.write(str(vcxproj_path), encoding='utf-8', xml_declaration=True)
    
    print(f"[OK] Converted: {vcxproj_path.name}")
    return str(vcxproj_path)

def convert_all_vcprojs():
    """Convert all vcproj files in the NetTarget directory"""
    nettarget_dir = Path(r'c:\originalhr\HoverRace\NetTarget')
    sln_file = Path(r'c:\originalhr\HoverRace\NetTarget.sln')
    
    vcproj_files = sorted(nettarget_dir.glob('**/*.vcproj'))
    
    if not vcproj_files:
        print("[INFO] No .vcproj files found")
        return False
    
    print(f"[INFO] Found {len(vcproj_files)} .vcproj files to convert")
    print()
    
    converted = 0
    for vcproj_file in vcproj_files:
        try:
            settings = extract_vcproj_settings(vcproj_file)
            create_vcxproj(settings, vcproj_file)
            converted += 1
        except Exception as e:
            print(f"[ERROR] {vcproj_file.name}: {e}")
    
    print()
    print(f"[INFO] Successfully converted {converted}/{len(vcproj_files)} projects")
    
    if converted > 0:
        print("[INFO] Updating solution file...")
        with open(sln_file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        updated = content.replace('.vcproj"', '.vcxproj"')
        
        with open(sln_file, 'w', encoding='utf-8') as f:
            f.write(updated)
        
        print(f"[OK] Updated solution file")
        return True
    
    return False

if __name__ == '__main__':
    import sys
    success = convert_all_vcprojs()
    sys.exit(0 if success else 1)
