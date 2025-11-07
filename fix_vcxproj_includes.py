#!/usr/bin/env python3
"""
Fix vcxproj files by adding MSVC standard include and library directories
"""

import xml.etree.ElementTree as ET
from pathlib import Path
import glob

def fix_vcxproj_includes(vcxproj_path):
    """Add standard MSVC includes to vcxproj file"""
    
    try:
        # MSVC paths
        msvc_include = r'C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\include'
        msvc_lib = r'C:\Program Files\Microsoft Visual Studio\2022\Community\SDK\ScopeCppSDK\vc15\VC\lib\x86'
        
        # Parse the file
        tree = ET.parse(vcxproj_path)
        root = tree.getroot()
        
        ns = {'ns': 'http://schemas.microsoft.com/developer/msbuild/2003'}
        ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')
        
        # Find all ItemDefinitionGroup elements and add includes
        modified = False
        for item_def_group in root.findall('.//ns:ItemDefinitionGroup', ns):
            cl_compile = item_def_group.find('ns:ClCompile', ns)
            if cl_compile is None:
                cl_compile = ET.SubElement(item_def_group, '{http://schemas.microsoft.com/developer/msbuild/2003}ClCompile')
            
            # Check if AdditionalIncludeDirectories already exists
            add_includes = cl_compile.find('ns:AdditionalIncludeDirectories', ns)
            if add_includes is not None:
                # Append to existing
                if msvc_include not in add_includes.text:
                    add_includes.text = f"{msvc_include};{add_includes.text}"
                    modified = True
            else:
                # Create new
                new_elem = ET.SubElement(cl_compile, '{http://schemas.microsoft.com/developer/msbuild/2003}AdditionalIncludeDirectories')
                new_elem.text = f"{msvc_include};%(AdditionalIncludeDirectories)"
                modified = True
            
            # Do the same for library directories in Link element
            link = item_def_group.find('ns:Link', ns)
            if link is None:
                link = ET.SubElement(item_def_group, '{http://schemas.microsoft.com/developer/msbuild/2003}Link')
            
            add_lib_dirs = link.find('ns:AdditionalLibraryDirectories', ns)
            if add_lib_dirs is not None:
                if msvc_lib not in add_lib_dirs.text:
                    add_lib_dirs.text = f"{msvc_lib};{add_lib_dirs.text}"
                    modified = True
            else:
                new_elem = ET.SubElement(link, '{http://schemas.microsoft.com/developer/msbuild/2003}AdditionalLibraryDirectories')
                new_elem.text = f"{msvc_lib};%(AdditionalLibraryDirectories)"
                modified = True
        
        if modified:
            tree.write(str(vcxproj_path), encoding='utf-8', xml_declaration=True)
            print(f"[OK] Fixed: {Path(vcxproj_path).name}")
            return True
        else:
            return False
            
    except Exception as e:
        print(f"[ERROR] {Path(vcxproj_path).name}: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """Fix all vcxproj files"""
    nettarget_dir = Path(r'c:\originalhr\HoverRace\NetTarget')
    
    vcxproj_files = sorted(nettarget_dir.glob('**/*.vcxproj'))
    
    if not vcxproj_files:
        print("[INFO] No .vcxproj files found")
        return False
    
    print(f"[INFO] Found {len(vcxproj_files)} .vcxproj files")
    print()
    
    fixed = 0
    for vcxproj_file in vcxproj_files:
        if fix_vcxproj_includes(vcxproj_file):
            fixed += 1
    
    print()
    print(f"[INFO] Fixed {fixed}/{len(vcxproj_files)} projects")
    return fixed > 0

if __name__ == '__main__':
    import sys
    success = main()
    sys.exit(0 if success else 1)
