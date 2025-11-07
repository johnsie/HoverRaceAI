$projects = @(
    @{Path="NetTarget\Util\Util.vcxproj"; GUID="{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}"; Name="Util"; Macro="MR_UTIL"; DepGUIDs=@()},
    @{Path="NetTarget\VideoServices\VideoServices.vcxproj"; GUID="{158869E8-53E8-4DC1-B41F-B1C996211E2B}"; Name="VideoServices"; Macro="MR_VIDEO_SERVICES"; DepGUIDs=@("{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}")},
    @{Path="NetTarget\Model\Model.vcxproj"; GUID="{C0254587-DA7D-4338-9ECB-B614B05E5E1A}"; Name="Model"; Macro="MR_MODEL"; DepGUIDs=@("{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}","{158869E8-53E8-4DC1-B41F-B1C996211E2B}")},
    @{Path="NetTarget\MainCharacter\MainCharacter.vcxproj"; GUID="{A55BC265-CFB8-46A7-958B-D3A48D9477A0}"; Name="MainCharacter"; Macro="MR_MAIN_CHARACTER"; DepGUIDs=@("{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}","{C0254587-DA7D-4338-9ECB-B614B05E5E1A}","{158869E8-53E8-4DC1-B41F-B1C996211E2B}")},
    @{Path="NetTarget\ObjFacTools\ObjFacTools.vcxproj"; GUID="{9A391771-FAFF-4D9D-9F6A-8AD3A9F73CA3}"; Name="ObjFacTools"; Macro="MR_OBJ_FAC_TOOLS"; DepGUIDs=@("{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}","{158869E8-53E8-4DC1-B41F-B1C996211E2B}","{C0254587-DA7D-4338-9ECB-B614B05E5E1A}")},
    @{Path="NetTarget\ColorTools\ColorTools.vcxproj"; GUID="{F589CA0C-83C4-4F07-A3C7-F1684DF88369}"; Name="ColorTools"; Macro="MR_COLOR_TOOLS"; DepGUIDs=@("{402CDE5A-4540-4DFE-A4E0-6950C0F2CA57}")}
)

# Delete corrupted files first
foreach ($proj in $projects) {
    $fullPath = "c:\originalhr\HoverRace\$($proj.Path)"
    if (Test-Path $fullPath) {
        Remove-Item -Path $fullPath -Force
        Write-Host "Deleted $($proj.Name).vcxproj"
    }
}

Write-Host "All corrupted files deleted. Ready for clean creation."
