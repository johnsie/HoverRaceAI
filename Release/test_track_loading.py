#!/usr/bin/env python3
"""
Track Loading Verification Script
Tests if Game2.exe can successfully load and deserialize ClassicH.trk
"""

import subprocess
import time
import os
import sys

def main():
    print("=" * 70)
    print("HOVERRACE TRACK LOADING VERIFICATION TEST")
    print("=" * 70)
    print()
    
    release_dir = r"C:\originalhr\HoverRace\Release"
    log_file = os.path.join(release_dir, "Game2_TrackLoad.log")
    game_exe = os.path.join(release_dir, "Game2.exe")
    track_file = os.path.join(release_dir, "Tracks", "ClassicH.trk")
    
    # Verify files exist
    print("PRE-TEST VERIFICATION:")
    print(f"  Game2.exe: {'✓' if os.path.exists(game_exe) else '✗'} {game_exe}")
    print(f"  ClassicH.trk: {'✓' if os.path.exists(track_file) else '✗'} {track_file}")
    
    if os.path.exists(track_file):
        size_mb = os.path.getsize(track_file) / (1024 * 1024)
        print(f"    Size: {size_mb:.2f} MB")
    print()
    
    # Clean old log
    if os.path.exists(log_file):
        os.remove(log_file)
        print(f"Cleaned old log: {log_file}")
    
    # Launch Game2
    print("LAUNCHING GAME2.EXE...")
    try:
        proc = subprocess.Popen([game_exe], cwd=release_dir)
        print(f"  Process ID: {proc.pid}")
        print(f"  Waiting 15 seconds for initialization...")
        
        # Wait for game to initialize and create log
        time.sleep(15)
        
        # Check if log was created
        if os.path.exists(log_file):
            print(f"  ✓ Log file created: {log_file}")
            print()
            print("=" * 70)
            print("TRACK LOADING LOG CONTENTS:")
            print("=" * 70)
            with open(log_file, 'r') as f:
                content = f.read()
                print(content)
            print("=" * 70)
            
            # Analyze log for success/errors
            if "SUCCESS" in content:
                print("✓ SUCCESS: Track loading appears successful!")
            elif "ERROR" in content:
                print("✗ ERROR: Track loading failed with errors")
            else:
                print("⚠ No clear status found in log")
        else:
            print(f"  ✗ Log file not created yet")
            print("  (This may be normal if track loading happens on-demand)")
            print()
            print("Game is still running. Check the main window for track selection.")
            print("The Game2_TrackLoad.log will be created when you select Play Race.")
        
        # Keep game running for manual testing
        print()
        print("Game is still running (PID {}). You can test manually.".format(proc.pid))
        print("The log will be updated when tracks are loaded from the UI.")
        
    except Exception as e:
        print(f"✗ ERROR: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
