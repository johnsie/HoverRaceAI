# HoverRace Game2 - ObjFac1 Rendering Requirements

## CRITICAL RULE: Always Use ObjFac1

**This is a binding directive: Never suggest or use SimpleRenderer as a fallback.**

### Rationale
- ObjFac1 provides proper 3D hovercraft rendering
- SimpleRenderer is only for legacy/fallback purposes
- The game requires 3D rendering to function properly

---

## ObjFac1 Integration Rules

### Rule 1: No Fallback to SimpleRenderer
❌ **FORBIDDEN**: Using SimpleRenderer as a fallback
✅ **REQUIRED**: Always attempt to use ObjFac1 HoverRender

**Code Pattern (CORRECT):**
```cpp
MR_ObjectFromFactoryId lHoverRenderId = { 1, 100 };
MR_ObjectFromFactory* pHoverRender = MR_DllObjectFactory::CreateObject(lHoverRenderId);
if (pHoverRender != NULL) {
    return pHoverRender;  // Use ObjFac1
} else {
    // Log error and return NULL - do NOT create SimpleRenderer
    return NULL;
}
```

**Code Pattern (WRONG):**
```cpp
// This is FORBIDDEN:
if (pHoverRender == NULL) {
    // DO NOT DO THIS:
    return new MR_SimpleMainCharacterRenderer(simpleId);
}
```

### Rule 2: ObjFac1 Must Be Fixed, Not Replaced
If ObjFac1 fails:
1. ✅ Add defensive programming to ObjFac1 (null checks, bounds checking)
2. ✅ Fix initialization order issues
3. ✅ Fix resource loading failures
4. ❌ Do NOT revert to SimpleRenderer

### Rule 3: Aggressive Error Reporting
When ObjFac1 creation fails, provide detailed diagnostics:
- Which DLL class ID failed
- Resource file existence and validity
- gObjectFactoryData initialization status
- All possible root causes listed

### Rule 4: Position Bounds Checking (Active)
- Viewing character position: ±100,000 units
- Actor position: ±500,000 units
- Corrupted positions: SKIP rendering, log occurrence

---

## ObjFac1 Build Verification

### Before Each Build
```powershell
# Verify ObjFac1.dll exists
Test-Path "c:\originalhr\HoverRace\Release\ObjFac1.dll"

# Verify ObjFac1.dat exists (resource file)
Test-Path "c:\originalhr\HoverRace\Release\ObjFac1.dat"

# Verify all dependencies
Test-Path "c:\originalhr\HoverRace\Release\VideoServices.dll"
Test-Path "c:\originalhr\HoverRace\Release\Model.dll"
Test-Path "c:\originalhr\HoverRace\Release\ObjFacTools.dll"
```

### Build Order (REQUIRED)
1. VideoServices.dll
2. Model.dll
3. ObjFacTools.dll
4. ObjFac1.dll (depends on 1-3)
5. MainCharacter.dll (must know ObjFac1 is available)
6. Game2.exe

**Never break this dependency chain.**

---

## ObjFac1 Initialization Sequence

### 1. DLL Loading
```cpp
ObjFac1.dll loads
  ├─ DllMain called with DLL_PROCESS_ATTACH
  ├─ MR_InitModule called by factory system
  │  └─ gObjectFactoryData = new MR_ObjectFactoryData(pModule, "ObjFac1.dat")
  └─ All classes registered with factory
```

### 2. Object Creation
```cpp
Game2 requests HoverRender (Class 100)
  ├─ MR_DllObjectFactory::CreateObject called
  ├─ ObjFac1 DLL entry point queried
  ├─ MR_GetObject(100) called
  ├─ MR_HoverRender constructor invoked
  │  ├─ Check gObjectFactoryData != NULL
  │  ├─ Load actor resources (3 car models)
  │  └─ Load sound resources (10 sounds)
  └─ Return MR_HoverRender* to Game2
```

### 3. Rendering
```cpp
Each frame:
  ├─ MainCharacter::Render called
  ├─ Position bounds checked
  ├─ mRenderer->Render called (MR_HoverRender::Render)
  ├─ 3D model rendered to viewport
  └─ Frame displayed
```

---

## Debugging ObjFac1 Failures

### Log Files to Monitor
- `Game2_FactoryFunc.log` - Factory function calls
- `Game2_ActorRender.log` - Rendering status
- `Game2_ViewingCharacterRender.log` - Viewing character rendering
- Windows Event Viewer - DLL load failures

### If ObjFac1 Creation Fails

**Check List:**
1. [ ] ObjFac1.dll present and 32-bit
2. [ ] ObjFac1.dat resource file exists
3. [ ] VideoServices.dll, Model.dll, ObjFacTools.dll present
4. [ ] All DLLs are compatible (built with same toolset)
5. [ ] Resource file path is correct ("ObjFac1.dat")
6. [ ] gObjectFactoryData initialization succeeds
7. [ ] No exceptions thrown in DllMain or MR_InitModule

### Diagnostic Output Expected
```
========== CREATING HOVERRENDER ==========
Attempting to create HoverRender from ObjFac1(DLL=1, Class=100)
CreateObject returned: <address>
========== SUCCESS: Got HoverRender from ObjFac1 =========
HoverRender object: <address>
Using PROPER 3D hovercraft rendering
```

---

## Code Review Checklist

Before any change to rendering:
- [ ] ObjFac1 is used for HoverRender (not SimpleRenderer)
- [ ] No fallback logic to SimpleRenderer exists
- [ ] Position bounds checking is active
- [ ] Exception handling uses MFC TRY/CATCH_ALL
- [ ] All null pointers checked before dereferencing
- [ ] Resource loading failures are handled gracefully
- [ ] Detailed logging for debugging

---

## Summary

**ObjFac1 must work. Not SimpleRenderer.**

- Use ObjFac1 for all hovercraft rendering
- Fix ObjFac1 issues by adding defenses
- Never revert to SimpleRenderer
- Report failures clearly with diagnostics
- Verify build order and dependencies
- Monitor logs for initialization issues

**The game requires 3D rendering. Make ObjFac1 work.**
