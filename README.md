# 3D Cad Modeling's final assignment of ZJU 2023-2024 fall
## Mainly implement :  
1、Define the half-side structure.  
2、Five basic Euler operations are implemented based on half-sided data structures (/src/algorithm/EulerOperation).   
3、Sweep operation based on Euler operation (allows multiple inner rings within a 2D area).   
4、Realistic rendering of the solid model built by the sweep operation.   
## How to start :  
Enter  your local empty folder, then type  
`git clone https://github.com/ArsNovaaa/EulerOperation_ZJU_CAD_ZC.git `  
Just use your **Visual Studio** (it's better to use the version 2022) to open the **CadModeling.sln** file, then you can compile it directly.
## Visualizations :  
A cube with one Square hole in it :  
![这是图片1](/cubic.png "one hole cube")  
A cube with one starlike hole in it(Triangulation of the inner ring of a non-convex polygon) :   
![这是图片2](/star.png "one hole cube")  
A cube with two  holes in it (to prove the multi-innerloop Topological relationships is correct) :    
![这是图片3](/2holes.png "one hole cube") 
## Other work :  
1、An algorithm for projecting points on a 3D plane onto a 2D plane was implemented using the eigen library.  
2、And based on the algorithm of the previous step, the triangulation of the inner ring of the plane band was completed by the earcutting algorithm.  
3、Correct display of topology information is realized Using ImGui.  


