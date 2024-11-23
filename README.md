# MyFirstGame
Fast Little Game written in C for YouTube

## Setup Windows
- Download Visual Studio: https://visualstudio.microsoft.com/downloads/
- Download Cmake: https://cmake.org/download/

### Visual Studio Installation
![Visual Studio Installer](/relative/path/to/img.jpg?raw=true "Installation Options")

### CMake Installation
![Visual Studio Installer](/relative/path/to/img.jpg?raw=true "Installation Options")

### Project Setup
1. Clone the Repository: `git clone https://github.com/Cakez77/MyFirstGame.git` or download the `.zip` file
2. Inside the folder open up a `Terminal` and type in: `cmake -S . -B build`
3. Navigate into the `build` Folder and open the `game.sln` to start **Visual Studio**
4. In **Visual Studio** set the **game** Project to **Set as Startup Project**
5. Open **Properties** of **game**, **Debugging** -> **Working Directory** = `$(ProjectDir)\..`


Hope this little project helps you make games in C / C++.
