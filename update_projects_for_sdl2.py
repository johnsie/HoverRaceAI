#!/usr/bin/env python3
"""
SDL2 Graphics Integration - Automatic Project File Updater

This script automatically configures Visual Studio project files (.vcxproj)
to link SDL2 and OpenGL libraries for the SDL2 graphics backend.

Usage:
    python3 update_projects_for_sdl2.py [--vcpkg-path PATH]

Example:
    python3 update_projects_for_sdl2.py --vcpkg-path C:\vcpkg\installed\x64-windows
"""

import os
import sys
import argparse
from pathlib import Path
import xml.etree.ElementTree as ET

# Register namespaces to preserve XML formatting
namespaces = {
    'Default': 'http://schemas.microsoft.com/developer/msbuild/2003'
}

for prefix, uri in namespaces.items():
    ET.register_namespace(prefix, uri)

class SDL2ProjectUpdater:
    def __init__(self, vcpkg_root):
        self.vcpkg_root = vcpkg_root
        self.include_dir = os.path.join(vcpkg_root, 'include')
        self.lib_dir = os.path.join(vcpkg_root, 'lib')
        self.debug_lib_dir = os.path.join(vcpkg_root, 'debug', 'lib')
        
    def verify_paths(self):
        """Verify that SDL2 and OpenGL libraries are installed."""
        required_files = [
            os.path.join(self.include_dir, 'SDL2', 'SDL.h'),
            os.path.join(self.lib_dir, 'SDL2.lib'),
            os.path.join(self.lib_dir, 'OpenGL32.lib'),
        ]
        
        missing = [f for f in required_files if not os.path.exists(f)]
        
        if missing:
            print("ERROR: Missing SDL2/OpenGL files:")
            for f in missing:
                print(f"  - {f}")
            return False
        
        print("✓ All SDL2/OpenGL files found")
        return True
    
    def update_vcxproj(self, proj_path):
        """Update a Visual Studio project file with SDL2 and OpenGL settings."""
        
        if not os.path.exists(proj_path):
            print(f"ERROR: Project file not found: {proj_path}")
            return False
        
        try:
            # Parse XML
            tree = ET.parse(proj_path)
            root = tree.getroot()
            ns = {'': 'http://schemas.microsoft.com/developer/msbuild/2003'}
            
            # Find all ItemDefinitionGroup elements
            updated = False
            
            for item_def in root.findall('.//ItemDefinitionGroup', ns):
                # Update ClCompile include directories
                cl_compile = item_def.find('ClCompile', ns)
                if cl_compile is None:
                    cl_compile = ET.SubElement(item_def, 'ClCompile')
                
                inc_dirs = cl_compile.find('AdditionalIncludeDirectories', ns)
                if inc_dirs is None:
                    inc_dirs = ET.SubElement(cl_compile, 'AdditionalIncludeDirectories')
                    inc_dirs.text = ''
                
                # Add SDL2 include path
                current_includes = (inc_dirs.text or '').strip()
                sdl2_include = os.path.join(self.include_dir, 'SDL2')
                
                if sdl2_include not in current_includes:
                    if current_includes:
                        inc_dirs.text = f"{current_includes};{self.include_dir};{sdl2_include};%(AdditionalIncludeDirectories)"
                    else:
                        inc_dirs.text = f"{self.include_dir};{sdl2_include};%(AdditionalIncludeDirectories)"
                    updated = True
                
                # Update Linker settings
                link = item_def.find('Link', ns)
                if link is None:
                    link = ET.SubElement(item_def, 'Link')
                
                lib_dirs = link.find('AdditionalLibraryDirectories', ns)
                if lib_dirs is None:
                    lib_dirs = ET.SubElement(link, 'AdditionalLibraryDirectories')
                    lib_dirs.text = ''
                
                current_lib_dirs = (lib_dirs.text or '').strip()
                if self.lib_dir not in current_lib_dirs:
                    if current_lib_dirs:
                        lib_dirs.text = f"{current_lib_dirs};{self.lib_dir};%(AdditionalLibraryDirectories)"
                    else:
                        lib_dirs.text = f"{self.lib_dir};%(AdditionalLibraryDirectories)"
                    updated = True
                
                # Add libraries
                deps = link.find('AdditionalDependencies', ns)
                if deps is None:
                    deps = ET.SubElement(link, 'AdditionalDependencies')
                    deps.text = ''
                
                current_deps = (deps.text or '').strip()
                required_libs = ['SDL2.lib', 'SDL2main.lib', 'OpenGL32.lib', 'glew32.lib']
                
                for lib in required_libs:
                    if lib not in current_deps:
                        if current_deps:
                            current_deps = f"{current_deps};{lib}"
                        else:
                            current_deps = lib
                        updated = True
                
                if updated:
                    deps.text = f"{current_deps};%(AdditionalDependencies)"
            
            if updated:
                tree.write(proj_path, encoding='utf-8', xml_declaration=True)
                print(f"✓ Updated: {proj_path}")
                return True
            else:
                print(f"✓ Already configured: {proj_path}")
                return True
            
        except Exception as e:
            print(f"ERROR: Failed to update {proj_path}: {e}")
            return False
    
    def run(self, projects):
        """Update multiple project files."""
        
        print("SDL2 Graphics Integration - Project File Updater")
        print("=" * 60)
        print(f"VCPKG Root: {self.vcpkg_root}")
        print(f"Include Dir: {self.include_dir}")
        print(f"Lib Dir: {self.lib_dir}")
        print()
        
        if not self.verify_paths():
            return False
        
        print()
        print("Updating project files...")
        print("-" * 60)
        
        success_count = 0
        for proj in projects:
            if self.update_vcxproj(proj):
                success_count += 1
        
        print("-" * 60)
        print()
        print(f"Updated {success_count}/{len(projects)} project files successfully")
        
        return success_count == len(projects)

def main():
    parser = argparse.ArgumentParser(description='Update Visual Studio projects for SDL2 graphics backend')
    parser.add_argument('--vcpkg-path', default=r'C:\vcpkg\installed\x64-windows',
                       help='Path to vcpkg installed SDL2/OpenGL libraries')
    parser.add_argument('--projects', nargs='+', 
                       help='Specific project files to update')
    
    args = parser.parse_args()
    
    # Default projects to update
    if not args.projects:
        workspace_root = Path(__file__).parent
        args.projects = [
            str(workspace_root / 'NetTarget' / 'NetTarget.vcxproj'),
            str(workspace_root / 'NetTarget' / 'MazeCompiler' / 'MazeCompiler.vcxproj'),
        ]
    
    updater = SDL2ProjectUpdater(args.vcpkg_path)
    
    if updater.run(args.projects):
        print("\n✓ SDL2 integration setup complete!")
        print("\nNext steps:")
        print("  1. Close Visual Studio if open")
        print("  2. Reopen the solution in Visual Studio")
        print("  3. Clean the solution (Build → Clean Solution)")
        print("  4. Rebuild the solution (Build → Rebuild Solution)")
        sys.exit(0)
    else:
        print("\n✗ SDL2 integration setup failed!")
        sys.exit(1)

if __name__ == '__main__':
    main()
