# Iris Engine - Gold

![00](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/bd05829968a79637c50890ac1e7b4aeb/Logo_Dark.png)

## More information:
We had three weeks to work on Iris Engine's Gold, for a total project duration of three and a half months.
- The editor can be used to create a new project with an empty scene.
- The physics system and material system have been implemented.
- The Scripting system in C# has been implemented.
- A JSON saving system has been implemented to save scene.
- Skybox has been implemented.
- Shadow mapping has been implemented.
- The engine has only one insoluble memory leak caused by shaderc.

## How to use the engine:
The engine is compiled with CMake. You can open the directory with VS 2022 and launch the IrisEditor.exe.

You can also create a solution with the GenerateSolution.bat to use the project.

/!\ Remember to update submodules with the UpdateSubmodules.bat file to use the engine correctly. /!\

The engine requires the Vulkan SDK with at least the 1.4.304.0 version! You will also required all debug tools of the SDK that are not mandatory at the installation.

The project does not compile yet with visual studio solution.

![01](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/b713fc6ca9ca9969ecd32195efc04111/image.png)

If the "Game" folder doesn't exist, create a new project by clicking on File -> New.

## Controls:

|      Controls       |     Action		|
|:------------------: |     :---:		|
|         W           |  Move forward	|
|         A           |    Move left    |
|         S           |   Move right    |
|         D           | Move backward	|
|         E           | Move up			|
|         Q           | Move down 		|
| Right click + Mouse |      Look		|

## Screenshots:
![02](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/af1c5af048439b786f2c1703b06aa00c/Capture_d_%C3%A9cran_2025-05-08_191249.png)
![01](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/dfc7792d2b4bfd502ad6ec0a1b230cab/Capture_d_%C3%A9cran_2025-05-08_191608.png)
![03](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/067d9be9d24548d4801ed7cbf330db8f/Capture_d_%C3%A9cran_2025-05-08_191430.png)
![04](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/ade951d45fb465d51e2a9593cc2ca191/Capture_d_%C3%A9cran_2025-05-08_191703.png)
![05](https://gitlabstudents.isartintra.com/projets/2024_gp_2028_gp_2028_projet_moteur-irisengine/uploads/05e712d459f1d3e59fe0e7c78e43bc69/Capture_d_%C3%A9cran_2025-05-08_192042.png)

## Authors:
### Programmers:
- [BRAUD Loïc](https://github.com/loicbraud95)
- [FONTVERNE Antoine](https://github.com/Auzuras)
- [GRAU Liam](https://github.com/Liam-Grau)
- [POINTEAU Marius](https://github.com/MariusPointeau)

### Artist:
- [DE CHARRY Côme](https://www.artstation.com/comdec)