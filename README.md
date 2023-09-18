# OpenGL Photo Mode
An interactive photography tool using OpenGL. A software where one can import models, build a set, set the blocking of the scene, and take snapshots. A variety of image filters are also available, to give the images an extra flair. Also, progress is saved in between sections, with users being able to reload their scene and easily resume their work.

## How to use

Go to the releases page, and get the latest version. Run CGJProject.exe.

The objects in the program are in a tree. Everything is linked together like that and you can create children. If you're ever lost as to which object you selected, just try to change the mesh or the texture. When you create a child, the default mesh is empty. Tip: Using the U and T keys is the easiest way to go through objects.
You can add your own Assets to the corresponding folders in Assets and import them with the controls described below.

Controls:
• Esc - Terminate
• F2 - Snapshot
• F3 - Change load folder to meshes
• F4 - Change load folder to textures
• F5 - Previous file
• F6 - Next file
• F7 - Load file
• F11 - Animate object
• F12 - Swap camera rotation type
• W - Move selected object forward
• S - Move selected object back
• A - Move selected object left
• D - Move selected object right
• Q - Move selected object up
• E - Move selected object down
• X - Delete object
• C - Create child
• T - Select child
• Y - Select parent
• U - Previous sibling
• I - Next sibling
• O - Previous image filter
• P - Next image filter
• G - Set object's shading to Phong shading
• H - Set object's shading to Cel shading
• J - Change object’s texture to previous one
• K - Change object's texture to next one
• L - Change object's mesh to previous one
• Ç - Change object's mesh to next one
• N - Save progress (Loading is currently not working, though you can create a save)
• M - Load progress

DO NOT DELETE FILE 2021-01-19-21-24-18.json
If you do, rename another save to have this name.
If you have no saves, create one with N and rename the file (there needs to be a save with this name, otherwise any attempt to load with M will crash the program).
