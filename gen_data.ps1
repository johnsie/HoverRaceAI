@{
    Util = @{
        path = "c:\originalhr\HoverRace\NetTarget\Util\Util.vcxproj"
        guid = "{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}"
        macro = "MR_UTIL"
        files = "DllObjectFactory.cpp,FuzzyLogic.cpp,Profiler.cpp,RecordFile.cpp,StdAfx.cpp,StrRes.cpp,WorldCoordinates.cpp"
        deps = ""
        linkdeps = ""
    }
    VideoServices = @{
        path = "c:\originalhr\HoverRace\NetTarget\VideoServices\VideoServices.vcxproj"
        guid = "{158869E8-53E8-4DC1-B41F-B1C996211E2B}"
        macro = "MR_VIDEO_SERVICES"
        files = "2DViewPort.cpp,3DViewport.cpp,3DViewportRendering.cpp,Bitmap.cpp,ColorPalette.cpp,ColorTab.cpp,Patch.cpp,SoundServer.cpp,Sprite.cpp,StdAfx.cpp,VideoBuffer.cpp,VideoBuffer2DDraw.cpp"
        deps = "..\Util\Util.vcxproj{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}"
        linkdeps = "Util.lib;dsound.lib;winmm.lib;"
    }
    Model = @{
        path = "c:\originalhr\HoverRace\NetTarget\Model\Model.vcxproj"
        guid = "{C0254587-DA7D-4338-9ECB-B614B05E5E1A}"
        macro = "MR_MODEL"
        files = "ConcreteShape.cpp,ContactEffect.cpp,FreeElementMovingHelper.cpp,GameSession.cpp,Level.cpp,MazeElement.cpp,PhysicalCollision.cpp,ShapeCollisions.cpp,Shapes.cpp,StdAfx.cpp"
        deps = "..\Util\Util.vcxproj{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}|..\VideoServices\VideoServices.vcxproj{158869E8-53E8-4DC1-B41F-B1C996211E2B}"
        linkdeps = "winmm.lib;Util.lib;VideoServices.lib;"
    }
    MainCharacter = @{
        path = "c:\originalhr\HoverRace\NetTarget\MainCharacter\MainCharacter.vcxproj"
        guid = "{A55BC265-CFB8-46A7-958B-D3A48D9477A0}"
        macro = "MR_MAIN_CHARACTER"
        files = "MainCharacter.cpp,MainCharacterRenderer.cpp,StdAfx.cpp"
        deps = "..\Util\Util.vcxproj{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}|..\Model\Model.vcxproj{C0254587-DA7D-4338-9ECB-B614B05E5E1A}|..\VideoServices\VideoServices.vcxproj{158869E8-53E8-4DC1-B41F-B1C996211E2B}"
        linkdeps = ""
    }
    ObjFacTools = @{
        path = "c:\originalhr\HoverRace\NetTarget\ObjFacTools\ObjFacTools.vcxproj"
        guid = "{9A391771-FAFF-4D9D-9F6A-8AD3A9F73CA3}"
        macro = "MR_OBJ_FAC_TOOLS"
        files = "BitmapSurface.cpp,FreeElementBase.cpp,FreeElementBaseRenderer.cpp,ObjectFactoryData.cpp,ResActor.cpp,ResBitmap.cpp,ResourceLib.cpp,ResSound.cpp,ResSprite.cpp,SpriteHandle.cpp,StdAfx.cpp"
        deps = "..\Util\Util.vcxproj{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}|..\VideoServices\VideoServices.vcxproj{158869E8-53E8-4DC1-B41F-B1C996211E2B}|..\Model\Model.vcxproj{C0254587-DA7D-4338-9ECB-B614B05E5E1A}"
        linkdeps = "Model.lib;VideoServices.lib;Util.lib;"
    }
    ColorTools = @{
        path = "c:\originalhr\HoverRace\NetTarget\ColorTools\ColorTools.vcxproj"
        guid = "{F589CA0C-83C4-4F07-A3C7-F1684DF88369}"
        macro = "MR_COLOR_TOOLS"
        files = "ColorTools.cpp,StdAfx.cpp"
        deps = "..\Util\Util.vcxproj{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}"
        linkdeps = ""
    }
} | ConvertTo-Json | Out-File c:\originalhr\HoverRace\projects.json -Encoding UTF8
