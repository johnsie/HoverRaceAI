#!/usr/bin/env python3
"""
Visualize all actor mesh files from test_output folder
"""

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import re
import os
import glob

def parse_msh_file_complete(filename):
    """Parse ALL patches from mesh file including multiple frames/sequences"""
    all_patches = []
    
    with open(filename, 'r') as f:
        content = f.read()
    
    # Split by SEQUENCE markers
    sequence_blocks = re.findall(r'SEQUENCE \d+.*?(?=SEQUENCE|\Z)', content, re.DOTALL)
    
    for seq_block in sequence_blocks:
        # Split by FRAME markers
        frame_blocks = re.findall(r'FRAME \d+.*?(?=FRAME|$)', seq_block, re.DOTALL)
        
        for frame_block in frame_blocks:
            # Split by PATCH markers to get all patches
            patch_blocks = re.findall(r'PATCH \d+.*?PATCH_END', frame_block, re.DOTALL)
            
            for patch_idx, block in enumerate(patch_blocks):
                # Extract GRID dimensions
                grid_match = re.search(r'GRID (\d+) (\d+)', block)
                if not grid_match:
                    continue
                
                grid_w, grid_h = int(grid_match.group(1)), int(grid_match.group(2))
                
                # Extract BITMAP_ID
                bitmap_match = re.search(r'BITMAP_ID (\d+)', block)
                bitmap_id = int(bitmap_match.group(1)) if bitmap_match else -1
                
                # Extract vertices
                vertices = []
                vertex_lines = re.findall(r'(-?\d+) (-?\d+) (-?\d+)', block)
                for x, y, z in vertex_lines:
                    vertices.append((int(x), int(y), int(z)))
                
                if vertices:
                    all_patches.append({
                        'grid': (grid_w, grid_h),
                        'bitmap_id': bitmap_id,
                        'vertices': vertices
                    })
    
    return all_patches

def vertices_to_faces(vertices, grid_w, grid_h):
    """Convert grid of vertices to triangular faces"""
    faces = []
    
    if len(vertices) < grid_w * grid_h:
        return faces
    
    for row in range(grid_h - 1):
        for col in range(grid_w - 1):
            idx0 = row * grid_w + col
            idx1 = row * grid_w + (col + 1)
            idx2 = (row + 1) * grid_w + col
            idx3 = (row + 1) * grid_w + (col + 1)
            
            if all(idx < len(vertices) for idx in [idx0, idx1, idx2, idx3]):
                v0, v1, v2, v3 = vertices[idx0], vertices[idx1], vertices[idx2], vertices[idx3]
                faces.append([v0, v1, v2])
                faces.append([v1, v3, v2])
    
    return faces

def visualize_all_patches(patches, title="All Patches", view_angles=(20, 45)):
    """Visualize all patches from all frames"""
    fig = plt.figure(figsize=(14, 12))
    ax = fig.add_subplot(111, projection='3d')
    
    all_faces = []
    colors = ['cyan', 'magenta', 'yellow', 'red', 'green', 'blue', 'orange', 'purple', 'brown', 'pink', 'lightgreen', 'lightblue', 'lightcoral', 'gold']
    
    face_colors = []
    
    for patch_idx, patch in enumerate(patches):
        vertices = patch['vertices']
        grid = patch['grid']
        
        if len(vertices) == grid[0] * grid[1]:
            faces = vertices_to_faces(vertices, grid[0], grid[1])
            
            color = colors[patch_idx % len(colors)]
            for face in faces:
                all_faces.append(np.array(face))
                face_colors.append(color)
    
    if all_faces:
        face_collection = Poly3DCollection(all_faces, linewidths=0.2, edgecolors='black', alpha=0.7)
        face_collection.set_facecolor(face_colors)
        ax.add_collection3d(face_collection)
    
    if all_faces:
        all_vertices = np.vstack(all_faces)
        ax.set_xlabel('X (mm)', fontsize=10)
        ax.set_ylabel('Y (mm)', fontsize=10)
        ax.set_zlabel('Z (mm)', fontsize=10)
        
        padding = 500
        ax.set_xlim(all_vertices[:, 0].min() - padding, all_vertices[:, 0].max() + padding)
        ax.set_ylim(all_vertices[:, 1].min() - padding, all_vertices[:, 1].max() + padding)
        ax.set_zlim(all_vertices[:, 2].min() - padding, all_vertices[:, 2].max() + padding)
    
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.view_init(elev=view_angles[0], azim=view_angles[1])
    
    return fig

def main():
    test_output_dir = r'C:\originalhr2\HoverRaceAI\test_output'
    output_dir = r'C:\originalhr2\HoverRaceAI\Release\craft_visualizations'
    os.makedirs(output_dir, exist_ok=True)
    
    # Find all .msh files
    msh_files = sorted(glob.glob(os.path.join(test_output_dir, 'actor_*.msh')))
    
    print(f"Found {len(msh_files)} actor mesh files\n")
    
    for msh_file in msh_files:
        filename = os.path.basename(msh_file)
        actor_name = filename.replace('actor_', '').replace('.msh', '')
        
        print(f"Processing: {filename}")
        
        all_patches = parse_msh_file_complete(msh_file)
        print(f"  {len(all_patches)} patches found")
        
        views = [
            ('_isometric.png', (20, 45), 'Isometric'),
            ('_front.png', (0, 0), 'Front'),
            ('_top.png', (90, 0), 'Top'),
            ('_side.png', (0, 90), 'Side'),
        ]
        
        for suffix, (elev, azim), label in views:
            fig = visualize_all_patches(all_patches, f"{actor_name} - {label} ({len(all_patches)} patches)")
            fig.axes[0].view_init(elev=elev, azim=azim)
            output_filename = actor_name + suffix
            output_path = os.path.join(output_dir, output_filename)
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
            plt.close(fig)
        
        print(f"  Saved 4 views\n")
    
    print("All visualizations complete!")
    
    # Summary
    print(f"\nOutput files saved to: {output_dir}")
    files = sorted(glob.glob(os.path.join(output_dir, '*.png')))
    print(f"Total PNG files: {len(files)}")

if __name__ == '__main__':
    main()
