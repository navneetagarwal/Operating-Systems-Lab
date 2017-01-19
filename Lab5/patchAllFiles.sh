cp ./PatchFiles/* ./
echo "Patching All Files...."
patch < makeFilePatch.diff
patch < proccPatch.diff
patch < prochPatch.diff
patch < syscallcPatch.diff
patch < syscallhPatch.diff	
patch < sysproccPatch.diff
patch <  userhPatch.diff
patch < usyssPatch.diff
echo "done Patching all files"
