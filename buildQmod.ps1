# Builds a .zip file for loading with BMBF
$NDKPath = Get-Content $PSScriptRoot/ndkpath.txt

$buildScript = "$NDKPath/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk -j 4
Compress-Archive -Path  "./extern/libbeatsaber-hook_1_3_3.so",`
                        "./libs/arm64-v8a/libbeatsaber-hook_1_3_5.so",`
                        "./libs/arm64-v8a/libcustom-types.so",`
                        "./libs/arm64-v8a/libgorillacosmetics.so",`
                        "./libs/arm64-v8a/libzip.so",`
                        "./libs/arm64-v8a/libquest-cosmetic-loader_0_1_4.so",`
                        "./ExtraFiles/default",`
                        "./ExtraFiles/Mirror",`
                        "./ExtraFiles/None",`
                        "./ExtraFiles/HatRack",`
                        "./ExtraFiles/Hats/Amongus.hat",`
                        "./ExtraFiles/Hats/Crown.hat",`
                        "./ExtraFiles/Hats/Top_Hat.hat",`
                        "./ExtraFiles/Materials/Camo.gmat",`
                        "./ExtraFiles/Materials/Gold.gmat",`
                        "./ExtraFiles/Materials/Matrix.gmat",`
                        "./ExtraFiles/Materials/Rainbow.gmat",`
                        "./ExtraFiles/Materials/Unity_Default_Material.gmat",`
                        "./cover.png",`
                        "./mod.json" -DestinationPath "./GorillaCosmetics.zip" -Update

& copy-item -Force "./GorillaCosmetics.zip" "./GorillaCosmetics.qmod"