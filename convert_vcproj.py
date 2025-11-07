#!/usr/bin/env python3
"""
Convert Visual Studio .vcproj files to .vcxproj format
This is a basic conversion that maintains the structure well enough for building
"""

import xml.etree.ElementTree as ET
import re
from pathlib import Path
from shutil import copy2

def parse_vcproj(vcproj_path):
    """Parse a .vcproj XML file"""
    tree = ET.parse(vcproj_path)
    return tree.getroot()

def convert_vcproj_to_vcxproj(vcproj_path):
    """Convert a single .vcproj file to .vcxproj format"""
    
    try:
        vcproj_path = Path(vcproj_path)
        vcxproj_path = vcproj_path.with_suffix('.vcxproj')
        
        # Read the original XML as text first to preserve formatting and comments
        with open(vcproj_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Parse the XML
        root = parse_vcproj(vcproj_path)
        
        # Extract key information
        project_name = root.get('Name', 'Project')
        project_guid = root.get('ProjectGUID', '')
        
        # Create new vcxproj document
        ns = 'http://schemas.microsoft.com/developer/msbuild/2003'
        ET.register_namespace('', ns)
        
        Project = ET.Element('Project', {
            'DefaultTargets': 'Build',
            'ToolsVersion': '14.0',
            'xmlns': ns
        })
        
        # ProjectConfigurations
        configurations = root.findall('.//Configuration')
        if configurations:
            ProjectConfigurations = ET.SubElement(Project, 'ItemGroup', {'Label': 'ProjectConfigurations'})
            
            for cfg in configurations:
                cfg_name = cfg.get('Name', 'Debug|Win32')
                parts = cfg_name.split('|')
                plat = parts[0] if len(parts) > 0 else 'Win32'
                conf = parts[1] if len(parts) > 1 else 'Debug'
                
                ProjectConfiguration = ET.SubElement(ProjectConfigurations, 'ProjectConfiguration', {'Include': cfg_name})
                ET.SubElement(ProjectConfiguration, 'Configuration').text = conf
                ET.SubElement(ProjectConfiguration, 'Platform').text = plat
        
        # Globals
        Globals = ET.SubElement(Project, 'PropertyGroup', {'Label': 'Globals'})
        ET.SubElement(Globals, 'ProjectGuid').text = project_guid
        ET.SubElement(Globals, 'ProjectName').text = project_name
        ET.SubElement(Globals, 'WindowsTargetPlatformVersion').text = '10.0'
        
        # Import defaults
        ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.Default.props'})
        
        # Configuration property groups
        for cfg in configurations:
            cfg_name = cfg.get('Name', 'Debug|Win32')
            cfg_type = cfg.get('ConfigurationType', '1')
            
            prop_group = ET.SubElement(Project, 'PropertyGroup', {
                'Label': 'Configuration',
                'Condition': f"'$(Configuration)|$(Platform)'=='{cfg_name}'"
            })
            
            ET.SubElement(prop_group, 'ConfigurationType').text = cfg_type
            ET.SubElement(prop_group, 'PlatformToolset').text = 'v143'
            ET.SubElement(prop_group, 'CharacterSet').text = cfg.get('CharacterSet', '1')
        
        # Import props
        ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.props'})
        
        # Property sheets
        property_sheets = ET.SubElement(Project, 'ImportGroup', {'Label': 'PropertySheets'})
        ET.SubElement(property_sheets, 'Import', {
            'Project': r'$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props',
            'Condition': "exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')",
            'Label': 'LocalAppDataPlatform'
        })
        
        # Source files - look for cpp files in same directory
        source_dir = vcproj_path.parent
        cpp_files = list(source_dir.glob('*.cpp'))
        h_files = list(source_dir.glob('*.h'))
        
        if cpp_files or h_files:
            ClCompile = ET.SubElement(Project, 'ItemGroup')
            for cpp in cpp_files:
                ET.SubElement(ClCompile, 'ClCompile', {'Include': cpp.name})
            
            ClInclude = ET.SubElement(Project, 'ItemGroup')
            for h in h_files:
                ET.SubElement(ClInclude, 'ClInclude', {'Include': h.name})
        
        # Target imports
        ET.SubElement(Project, 'Import', {'Project': r'$(VCTargetsPath)\Microsoft.Cpp.targets'})
        ET.SubElement(Project, 'ImportGroup', {'Label': 'ExtensionTargets'})
        
        # Write the vcxproj file
        tree = ET.ElementTree(Project)
        tree.write(str(vcxproj_path), encoding='utf-8', xml_declaration=True)
        
        print(f"[OK] Converted: {vcxproj_path.name}")
        return str(vcxproj_path)
        
    except Exception as e:
        print(f"[ERROR] Converting {vcproj_path}: {e}")
        import traceback
        traceback.print_exc()
        return None

def update_solution_file(sln_path):
    """Update solution file to reference .vcxproj files instead of .vcproj"""
    try:
        sln_path = Path(sln_path)
        with open(sln_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Replace .vcproj with .vcxproj in solution file
        updated = content.replace('.vcproj"', '.vcxproj"')
        
        with open(sln_path, 'w', encoding='utf-8') as f:
            f.write(updated)
        
        print(f"[OK] Updated solution file: {sln_path.name}")
        
    except Exception as e:
        print(f"[ERROR] Updating solution file: {e}")

def main():
    """Find and convert all .vcproj files"""
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
        result = convert_vcproj_to_vcxproj(vcproj_file)
        if result:
            converted += 1
    
    print()
    print(f"[INFO] Successfully converted {converted}/{len(vcproj_files)} projects")
    
    if converted > 0:
        print("[INFO] Updating solution file...")
        update_solution_file(sln_file)
        return True
    
    return False

if __name__ == '__main__':
    import sys
    success = main()
    sys.exit(0 if success else 1)
