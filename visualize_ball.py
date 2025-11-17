#!/usr/bin/env python3
"""
Visualize all frames of HoverRace craft mesh
"""

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import re

def parse_msh_file_all_frames(filename):
    """Parse a .msh mesh file and extract ALL vertices from all sequences"""
    sequences = []
    current_sequence = None
    current_frame = None
    current_patch = None
    vertices = []
    
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    i = 0
    while i < len(lines):
        line = lines[i].strip()
        
        if line.startswith('SEQUENCE'):
            if current_sequence is not None:
                sequences.append(current_sequence)
            current_sequence = {'frames': []}
        
        elif line.startswith('FRAME'):
            if current_frame is not None and current_sequence is not None:
                current_sequence['frames'].append(current_frame)
            current_frame = {'patches': []}
        
        elif line.startswith('PATCH'):
            if current_patch is not None and vertices:
                current_patch['vertices'] = vertices
                if current_frame is not None:
                    current_frame['patches'].append(current_patch)
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
            i += 1
            while i < len(lines):
                vline = lines[i].strip()
                if vline == 'VERTICES_END':
                    break
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
    
    # Finalize last structures
    if current_patch is not None and vertices:
        current_patch['vertices'] = vertices
        if current_frame is not None:
            current_frame['patches'].append(current_patch)
    
    if current_frame is not None and current_sequence is not None:
        current_sequence['frames'].append(current_frame)
    
    if current_sequence is not None:
        sequences.append(current_sequence)
    
    return sequences

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

def visualize_frame(frame, title="Frame"):
    """Create 3D visualization of a single frame"""
    fig = plt.figure(figsize=(12, 10))
    ax = fig.add_subplot(111, projection='3d')
    
    all_faces = []
    colors = ['cyan', 'magenta', 'yellow', 'red', 'green', 'blue', 'orange', 'purple', 'brown', 'pink']
    
    patches = frame.get('patches', [])
    
    for patch_idx, patch in enumerate(patches):
        if 'vertices' not in patch or not patch['vertices']:
            continue
        
        vertices = patch['vertices']
        grid = patch.get('grid', (4, 4))
        
        if grid and len(vertices) == grid[0] * grid[1]:
            faces = vertices_to_faces(vertices, grid[0], grid[1])
            
            for face in faces:
                all_faces.append(np.array(face))
    
    if all_faces:
        face_collection = Poly3DCollection(all_faces, linewidths=0.3, edgecolors='black', alpha=0.8)
        
        face_colors = []
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
    
    if all_faces:
        all_vertices = np.vstack(all_faces)
        ax.set_xlabel('X (mm)')
        ax.set_ylabel('Y (mm)')
        ax.set_zlabel('Z (mm)')
        
        padding = 500
        ax.set_xlim(all_vertices[:, 0].min() - padding, all_vertices[:, 0].max() + padding)
        ax.set_ylim(all_vertices[:, 1].min() - padding, all_vertices[:, 1].max() + padding)
        ax.set_zlim(all_vertices[:, 2].min() - padding, all_vertices[:, 2].max() + padding)
    
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.view_init(elev=20, azim=45)
    
    return fig

def main():
    msh_file = r'C:\originalhr2\HoverRaceAI\test_output\actor_ball_12.msh'
    print(f"Loading mesh file: {msh_file}")
    
    sequences = parse_msh_file_all_frames(msh_file)
    print(f"Found {len(sequences)} sequences")
    
    for seq_idx, seq in enumerate(sequences):
        frames = seq.get('frames', [])
        print(f"  Sequence {seq_idx}: {len(frames)} frames")
        for frame_idx, frame in enumerate(frames):
            patches = frame.get('patches', [])
            total_verts = sum(len(p.get('vertices', [])) for p in patches)
            print(f"    Frame {frame_idx}: {len(patches)} patches, {total_verts} vertices")
    
    # Visualize first frame in isometric view
    if sequences and sequences[0]['frames']:
        first_frame = sequences[0]['frames'][0]
        
        import os
        output_dir = r'C:\originalhr2\HoverRaceAI\Release\craft_visualizations'
        os.makedirs(output_dir, exist_ok=True)
        
        print("\nCreating isometric view...")
        fig = visualize_frame(first_frame, "Ball Actor 12 - Isometric View")
        fig.axes[0].view_init(elev=20, azim=45)
        plt.savefig(os.path.join(output_dir, 'ball_12_isometric.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved: ball_12_isometric.png")
        
        print("Creating front view...")
        fig = visualize_frame(first_frame, "Ball Actor 12 - Front View")
        fig.axes[0].view_init(elev=0, azim=0)
        plt.savefig(os.path.join(output_dir, 'ball_12_front.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved: ball_12_front.png")
        
        print("Creating top view...")
        fig = visualize_frame(first_frame, "Ball Actor 12 - Top View")
        fig.axes[0].view_init(elev=90, azim=0)
        plt.savefig(os.path.join(output_dir, 'ball_12_top.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved: ball_12_top.png")
        
        print("Creating side view...")
        fig = visualize_frame(first_frame, "Ball Actor 12 - Side View")
        fig.axes[0].view_init(elev=0, azim=90)
        plt.savefig(os.path.join(output_dir, 'ball_12_side.png'), dpi=150, bbox_inches='tight')
        plt.close(fig)
        print("Saved: ball_12_side.png")

if __name__ == '__main__':
    main()
