#!/usr/bin/env python3
"""
Fix missing _AFXDLL in Release configurations for MFC projects
"""

import xml.etree.ElementTree as ET
from pathlib import Path

def fix_mfc_afxdll(vcxproj_path):
    """Add _AFXDLL to MFC projects that are missing it"""
    
    try:
        tree = ET.parse(vcxproj_path)
        root = tree.getroot()
        
        ns = {'ns': 'http://schemas.microsoft.com/developer/msbuild/2003'}
        ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')
        
        modified = False
        
        # Find all ItemDefinitionGroup elements
        for item_def_group in root.findall('.//ns:ItemDefinitionGroup', ns):
            # Check if this group is for Release configuration
            condition = item_def_group.get('Condition', '')
            if 'Release' not in condition:
                continue
            
            cl_compile = item_def_group.find('ns:ClCompile', ns)
            if cl_compile is None:
                continue
            
            # Check if _AFXDLL is already defined
            preproc = cl_compile.find('ns:PreprocessorDefinitions', ns)
            if preproc is not None and '_AFXDLL' not in (preproc.text or ''):
                # Add _AFXDLL
                if preproc.text:
                    preproc.text = f"_AFXDLL;{preproc.text}"
                else:
                    preproc.text = "_AFXDLL;%(PreprocessorDefinitions)"
                modified = True
                print(f"  Added _AFXDLL to {Path(vcxproj_path).name}")
        
        if modified:
            tree.write(str(vcxproj_path), encoding='utf-8', xml_declaration=True)
            return True
        
        return False
            
    except Exception as e:
        print(f"[ERROR] {Path(vcxproj_path).name}: {e}")
        return False

def main():
    """Fix all vcxproj files"""
    nettarget_dir = Path(r'c:\originalhr\HoverRace\NetTarget')
    
    vcxproj_files = sorted(nettarget_dir.glob('**/*.vcxproj'))
    
    print(f"[INFO] Checking {len(vcxproj_files)} projects for missing _AFXDLL...")
    print()
    
    fixed = 0
    for vcxproj_file in vcxproj_files:
        if fix_mfc_afxdll(vcxproj_file):
            fixed += 1
    
    print()
    print(f"[INFO] Fixed {fixed}/{len(vcxproj_files)} projects")
    return fixed > 0

if __name__ == '__main__':
    import sys
    success = main()
    sys.exit(0 if success else 1)
