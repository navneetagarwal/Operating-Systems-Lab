cp ./PatchFiles/* ./
echo "Patching All Files...."
patch < patchdefs
patch < patchkalloc
patch < patchmake
patch < patchmmu
patch < patchproc
patch < patchsyscallc
patch < patchsyscallh
patch < patchsysproc
patch < patchusys
patch < patchtrap
patch < patchvm
patch < patchuser
echo "done Patching all files"