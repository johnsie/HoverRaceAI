#!/usr/bin/env python3
"""
Visualize HoverRace craft mesh from extracted .msh file
"""

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import re

def parse_msh_file(filename):
    """Parse a .msh mesh file and extract vertex data"""
    patches = []
    current_patch = None
    vertices = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        
        if line.startswith('PATCH'):
            if current_patch is not None and vertices:
                current_patch['vertices'] = vertices
                patches.append(current_patch)
            current_patch = {'grid': None, 'vertices': []}
            vertices = []
        
        elif line.startswith('BITMAP_ID'):
            match = re.search(r'BITMAP_ID (\d+)', line)
            if match and current_patch is not None:
                current_patch['bitmap_id'] = int(match.group(1))
        
        elif line.startswith('GRID'):
            match = re.search(r'GRID (\d+) (\d+)', line)
            if match and current_patch is not None:
                current_patch['grid'] = (int(match.group(1)), int(match.group(2)))
        
        elif line.startswith('VERTICES'):
            # Read vertices until VERTICES_END
            i += 1
            while i < len(lines):
                vline = lines[i].strip()
                if vline == 'VERTICES_END':
                    break
                # Parse vertex coordinates
                coords = vline.split()
                if len(coords) == 3:
                    try:
                        x, y, z = map(int, coords)
                        vertices.append((x, y, z))
                    except ValueError:
                        pass
                i += 1
            continue
        
        i += 1
    
    # Add last patch
    if current_patch is not None and vertices:
        current_patch['vertices'] = vertices
        patches.append(current_patch)
    
    return patches

def vertices_to_faces(vertices, grid_w, grid_h):
    """Convert grid of vertices to triangular faces"""
    faces = []
    
    # Grid vertices are arranged in a 2D grid (grid_w x grid_h)
    # We need to create quad faces and subdivide them into triangles
    if len(vertices) < grid_w * grid_h:
        # Not enough vertices for the specified grid
        return faces
    
    for row in range(grid_h - 1):
        for col in range(grid_w - 1):
            # Get the four corners of the quad
            idx0 = row * grid_w + col
            idx1 = row * grid_w + (col + 1)
            idx2 = (row + 1) * grid_w + col
            idx3 = (row + 1) * grid_w + (col + 1)
            
            if all(idx < len(vertices) for idx in [idx0, idx1, idx2, idx3]):
                # Create two triangles from the quad
                v0, v1, v2, v3 = vertices[idx0], vertices[idx1], vertices[idx2], vertices[idx3]
                faces.append([v0, v1, v2])
                faces.append([v1, v3, v2])
    
    return faces

def visualize_craft(patches, title="HoverRace Craft"):
    """Create 3D visualization of the craft"""
    fig = plt.figure(figsize=(14, 10))
    ax = fig.add_subplot(111, projection='3d')
    
    all_faces = []
    colors = ['cyan', 'magenta', 'yellow', 'red', 'green', 'blue', 'orange']
    
    for patch_idx, patch in enumerate(patches):
        if 'vertices' not in patch or not patch['vertices']:
            continue
        
        vertices = patch['vertices']
        grid = patch.get('grid', (4, 4))
        
        if grid and len(vertices) == grid[0] * grid[1]:
            faces = vertices_to_faces(vertices, grid[0], grid[1])
            
            for face in faces:
                face_array = np.array(face)
                all_faces.append(face_array)
    
    if all_faces:
        # Create a collection of all faces
        face_collection = Poly3DCollection(all_faces, linewidths=0.5, edgecolors='black', alpha=0.7)
        
        # Color faces by patch
        face_colors = []
        color_idx = 0
        for patch_idx, patch in enumerate(patches):
            if 'vertices' not in patch or not patch['vertices']:
                continue
            
            vertices = patch['vertices']
            grid = patch.get('grid', (4, 4))
            
            if grid and len(vertices) == grid[0] * grid[1]:
                faces = vertices_to_faces(vertices, grid[0], grid[1])
                color = colors[patch_idx % len(colors)]
                for _ in faces:
                    face_colors.append(color)
        
        face_collection.set_facecolor(face_colors)
        ax.add_collection3d(face_collection)
    
    # Set axis labels and limits
    if all_faces:
        all_vertices = np.vstack(all_faces)
        ax.set_xlabel('X (mm)')
        ax.set_ylabel('Y (mm)')
        ax.set_zlabel('Z (mm)')
        
        # Set limits with some padding
        padding = 500
        ax.set_xlim(all_vertices[:, 0].min() - padding, all_vertices[:, 0].max() + padding)
        ax.set_ylim(all_vertices[:, 1].min() - padding, all_vertices[:, 1].max() + padding)
        ax.set_zlim(all_vertices[:, 2].min() - padding, all_vertices[:, 2].max() + padding)
    
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.view_init(elev=20, azim=45)
    
    return fig

def main():
    # Parse the demo_fighter mesh file
    msh_file = r'C:\originalhr2\HoverRaceAI\test_output\actor_demo_fighter_10.msh'
    print(f"Loading mesh file: {msh_file}")
    
    patches = parse_msh_file(msh_file)
    print(f"Found {len(patches)} patches")
    
    for i, patch in enumerate(patches):
        grid = patch.get('grid', (0, 0))
        vertices_count = len(patch.get('vertices', []))
        bitmap = patch.get('bitmap_id', -1)
        print(f"  Patch {i}: Grid={grid}, Vertices={vertices_count}, Bitmap={bitmap}")
    
    # Create visualization
    print("Creating 3D visualization...")
    fig = visualize_craft(patches, "Demo Fighter 10 - Frame 0")
    
    # Save the figure
    import os
    output_dir = r'C:\originalhr2\HoverRaceAI\Release\craft_visualizations'
    os.makedirs(output_dir, exist_ok=True)
    
    output_file = os.path.join(output_dir, 'demo_fighter_10.png')
    print(f"Saving visualization to: {output_file}")
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print("Done!")
    
    # Also save a different angle view
    fig2 = visualize_craft(patches, "Demo Fighter 10 - Top View")
    ax = fig2.axes[0]
    ax.view_init(elev=90, azim=0)
    
    output_file2 = os.path.join(output_dir, 'demo_fighter_10_top.png')
    print(f"Saving top view to: {output_file2}")
    plt.savefig(output_file2, dpi=150, bbox_inches='tight')
    plt.close(fig2)
    print("Done!")
    
    # Side view
    fig3 = visualize_craft(patches, "Demo Fighter 10 - Side View")
    ax = fig3.axes[0]
    ax.view_init(elev=0, azim=0)
    
    output_file3 = os.path.join(output_dir, 'demo_fighter_10_side.png')
    print(f"Saving side view to: {output_file3}")
    plt.savefig(output_file3, dpi=150, bbox_inches='tight')
    plt.close(fig3)
    print("Done!")

if __name__ == '__main__':
    main()
