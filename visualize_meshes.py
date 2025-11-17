#!/usr/bin/env python3
"""
Visualize HoverRace mesh files (.msh) as 3D rendered images
"""

import os
import sys
import re
from pathlib import Path
from collections import defaultdict
import numpy as np

try:
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
    from mpl_toolkits.mplot3d.art3d import Poly3DCollection
except ImportError:
    print("Installing required packages...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "matplotlib", "numpy"])
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
    from mpl_toolkits.mplot3d.art3d import Poly3DCollection


class MeshParser:
    """Parser for HoverRace mesh files"""
    
    def __init__(self, filepath):
        self.filepath = filepath
        self.sequences = []
        self.parse()
    
    def parse(self):
        """Parse mesh file"""
        with open(self.filepath, 'r') as f:
            content = f.read()
        
        # Extract sequences
        sequence_pattern = r'SEQUENCE (\d+)(.*?)(?=SEQUENCE|\Z)'
        sequences = re.finditer(sequence_pattern, content, re.DOTALL)
        
        for seq_match in sequences:
            seq_id = int(seq_match.group(1))
            seq_content = seq_match.group(2)
            frames = self._parse_frames(seq_content)
            self.sequences.append({
                'id': seq_id,
                'frames': frames
            })
    
    def _parse_frames(self, content):
        """Extract frames from sequence content"""
        frames = []
        frame_pattern = r'FRAME (\d+)(.*?)(?=FRAME|\Z)'
        frame_matches = re.finditer(frame_pattern, content, re.DOTALL)
        
        for frame_match in frame_matches:
            frame_id = int(frame_match.group(1))
            frame_content = frame_match.group(2)
            patches = self._parse_patches(frame_content)
            frames.append({
                'id': frame_id,
                'patches': patches
            })
        
        return frames
    
    def _parse_patches(self, content):
        """Extract patches from frame content"""
        patches = []
        patch_pattern = r'PATCH (\d+)(.*?)PATCH_END'
        patch_matches = re.finditer(patch_pattern, content, re.DOTALL)
        
        for patch_match in patch_matches:
            patch_id = int(patch_match.group(1))
            patch_content = patch_match.group(2)
            
            # Extract BITMAP_ID
            bitmap_match = re.search(r'BITMAP_ID (\d+)', patch_content)
            bitmap_id = int(bitmap_match.group(1)) if bitmap_match else -1
            
            # Extract GRID dimensions
            grid_match = re.search(r'GRID (\d+) (\d+)', patch_content)
            grid = tuple(map(int, grid_match.groups())) if grid_match else (0, 0)
            
            # Extract VERTICES
            vertices_match = re.search(r'VERTICES\s+(.*?)\s+VERTICES_END', patch_content, re.DOTALL)
            vertices = []
            if vertices_match:
                vertices_str = vertices_match.group(1)
                for line in vertices_str.strip().split('\n'):
                    line = line.strip()
                    if line:
                        coords = list(map(float, line.split()))
                        if len(coords) == 3:
                            vertices.append(coords)
            
            patches.append({
                'id': patch_id,
                'bitmap_id': bitmap_id,
                'grid': grid,
                'vertices': np.array(vertices) if vertices else np.array([])
            })
        
        return patches
    
    def get_all_vertices(self):
        """Get all vertices from all frames and patches"""
        all_vertices = []
        for seq in self.sequences:
            for frame in seq['frames']:
                for patch in frame['patches']:
                    if len(patch['vertices']) > 0:
                        all_vertices.extend(patch['vertices'])
        return np.array(all_vertices) if all_vertices else np.array([])
    
    def get_faces(self):
        """Generate faces from grid topology"""
        faces = []
        vertex_offset = 0
        
        for seq in self.sequences:
            for frame in seq['frames']:
                for patch in frame['patches']:
                    if len(patch['vertices']) > 0:
                        vertices = patch['vertices']
                        grid_w, grid_h = patch['grid']
                        
                        # Generate faces from grid
                        for i in range(grid_h - 1):
                            for j in range(grid_w - 1):
                                # Two triangles per grid cell
                                v0 = vertex_offset + i * grid_w + j
                                v1 = vertex_offset + i * grid_w + j + 1
                                v2 = vertex_offset + (i + 1) * grid_w + j + 1
                                v3 = vertex_offset + (i + 1) * grid_w + j
                                
                                if all(idx < vertex_offset + len(vertices) for idx in [v0, v1, v2, v3]):
                                    faces.append([v0, v1, v2])
                                    faces.append([v0, v2, v3])
                        
                        vertex_offset += len(vertices)
        
        return faces


def visualize_mesh(mesh_file, output_path):
    """Visualize a single mesh file"""
    try:
        parser = MeshParser(mesh_file)
        vertices = parser.get_all_vertices()
        faces = parser.get_faces()
        
        if len(vertices) == 0:
            print(f"  No vertices found in {mesh_file}")
            return False
        
        # Create figure
        fig = plt.figure(figsize=(10, 8))
        ax = fig.add_subplot(111, projection='3d')
        
        # Create mesh
        if len(faces) > 0:
            verts = [vertices[face] for face in faces if all(idx < len(vertices) for idx in face)]
            poly = Poly3DCollection(verts, alpha=0.8, edgecolor='k', linewidth=0.5)
            
            # Color by Z coordinate
            face_colors = []
            for face in faces:
                if all(idx < len(vertices) for idx in face):
                    z_avg = np.mean(vertices[face, 2])
                    face_colors.append([0.3 + 0.7 * (z_avg - vertices[:, 2].min()) / (vertices[:, 2].max() - vertices[:, 2].min() + 1), 0.5, 0.8])
            
            poly.set_facecolor(face_colors)
            ax.add_collection3d(poly)
        
        # Set axis limits and labels
        ax.set_xlabel('X')
        ax.set_ylabel('Y')
        ax.set_zlabel('Z')
        
        # Auto scale
        ax.auto_scale_xyz(vertices[:, 0], vertices[:, 1], vertices[:, 2])
        
        # Set a good viewing angle
        ax.view_init(elev=20, azim=45)
        
        plt.title(os.path.basename(mesh_file))
        plt.tight_layout()
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        plt.close()
        
        print(f"  [OK] Rendered {os.path.basename(mesh_file)} -> {os.path.basename(output_path)}")
        return True
        
    except Exception as e:
        print(f"  [ERROR] Error processing {mesh_file}: {e}")
        return False


def main():
    """Main entry point"""
    mesh_dir = r"C:\originalhr2\HoverRaceAI\Release\test_output"
    output_dir = os.path.join(mesh_dir, "actors")
    
    # Create output directory
    os.makedirs(output_dir, exist_ok=True)
    
    # Find all mesh files
    mesh_files = sorted(Path(mesh_dir).glob("*.msh"))
    
    if not mesh_files:
        print(f"No mesh files found in {mesh_dir}")
        return
    
    print(f"Found {len(mesh_files)} mesh files. Rendering...")
    print()
    
    success_count = 0
    for mesh_file in mesh_files:
        output_file = os.path.join(output_dir, f"{mesh_file.stem}.png")
        if visualize_mesh(str(mesh_file), output_file):
            success_count += 1
    
    print()
    print(f"[SUCCESS] Successfully rendered {success_count}/{len(mesh_files)} meshes")
    print(f"Images saved to: {output_dir}")


if __name__ == "__main__":
    main()
