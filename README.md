# 3DACTG - 3D Action Game

A 3D action game developed in C++ using DirectX 9 and DirectX 11 for rendering, featuring maze navigation, enemy AI, and real-time combat mechanics.

## Overview

3DACTG is a Windows-based 3D action game that demonstrates advanced graphics programming techniques using DirectX. The game features a player-controlled character navigating through dynamically generated mazes while engaging with enemies in a 3D environment.

## Features

### Core Gameplay
- **Player Control**: Radio-controlled character with movement states (idle, running, jumping, attacking)
- **Maze Navigation**: Procedurally generated maze system with collision detection
- **Combat System**: Real-time combat with shooting mechanics and enemy AI
- **Camera System**: Dynamic camera with yaw and pitch controls

### Graphics & Rendering
- **Dual DirectX Support**: Utilizes both DirectX 9 and DirectX 11 rendering pipelines
- **Skinned Mesh Animation**: Character animations using skeletal animation system
- **Particle Effects**: Powered by Effekseer particle system
- **Lighting & Fog**: Dynamic lighting system with configurable fog effects
- **Debug Visualization**: Bounding box and bounding sphere rendering for collision debugging

### Technical Features
- **Collision Detection**: 
  - Bounding sphere collision detection
  - Bounding cube collision detection
  - Ray casting for precision detection
- **Sound System**: Integrated sound manager for audio effects and music
- **UI System**: 2D sprite rendering for HUD and menu interfaces
- **Minimap**: Real-time minimap display for navigation
- **Scene Management**: Modular scene system (Title, Game, Result, Menu)

## Requirements

### Development Environment
- **Operating System**: Windows 10 or later
- **IDE**: Visual Studio 2017 or later
- **Build Tools**: Microsoft Visual C++ Build Tools

### Libraries & Dependencies
- DirectX 9 SDK
- DirectX 11 SDK
- DirectX 10 SDK (for D3DX types)
- Windows SDK
- Effekseer (particle effects library)

## Build Instructions

### Using Visual Studio

1. **Clone the repository**
   ```bash
   git clone https://github.com/dieghomj/3DACTG.git
   cd 3DACTG
   ```

2. **Open the solution**
   - Open `3DACTG.sln` in Visual Studio

3. **Configure build settings**
   - Select build configuration: `Debug` or `Release`
   - Select platform: `x86` or `x64`

4. **Build the project**
   - Build → Build Solution (Ctrl+Shift+B)
   - Or right-click on the project → Build

5. **Run the game**
   - Debug → Start Debugging (F5)
   - Or Debug → Start Without Debugging (Ctrl+F5)

## Project Structure

```
3DACTG/
├── 3DACTG/                    # Main source directory
│   ├── Data/                  # Game assets
│   │   ├── Collision/         # Collision meshes
│   │   ├── DebugText/         # Debug font resources
│   │   ├── Effekseer/         # Particle effect files
│   │   ├── Font/              # Font files
│   │   ├── Mesh/              # 3D models
│   │   │   ├── Skin/          # Skinned meshes (characters)
│   │   │   └── Static/        # Static meshes (environment)
│   │   ├── Shader/            # HLSL shader files
│   │   ├── Sound/             # Audio files
│   │   └── Texture/           # Texture assets
│   ├── WinMain.cpp            # Application entry point
│   ├── CMain.h/cpp            # Main application class
│   ├── CScene*.h/cpp          # Scene management
│   ├── CPlayer.h/cpp          # Player controller
│   ├── CBaseEnemy.h/cpp       # Enemy AI base class
│   ├── CMaze.h/cpp            # Maze generation
│   ├── CCamera.h/cpp          # Camera controller
│   ├── CDirectX9/11.h/cpp     # DirectX wrappers
│   ├── CCollider*.h/cpp       # Collision detection
│   ├── CSkinMesh*.h/cpp       # Skeletal animation system
│   ├── CStaticMesh*.h/cpp     # Static mesh rendering
│   ├── CSprite*.h/cpp         # 2D/3D sprite rendering
│   ├── CSound*.h/cpp          # Audio system
│   └── Global.h               # Global constants and structures
└── 3DACTG.sln                 # Visual Studio solution file
```

## Key Components

### Character System
- **CPlayer**: Player-controlled character with movement and combat
- **CAnimCharacter**: Base class for animated characters
- **CSkinMesh**: Skeletal mesh loader and renderer
- **CBaseEnemy**: Enemy AI behavior and control

### Rendering System
- **CDirectX9/CDirectX11**: DirectX initialization and management
- **CCamera**: Camera control and view matrix management
- **CSprite2D/CSprite3D**: Billboard and sprite rendering
- **CEffect**: Particle effects using Effekseer

### Game Systems
- **CSceneManager**: Scene lifecycle management
- **CMaze**: Procedural maze generation
- **CCollider**: Collision detection interfaces
- **CSoundManager**: Audio playback and management

## Controls

*(Controls should be documented here based on the game's input handling)*

## Configuration

The game window size and frame rate can be configured in `Global.h`:
- `WND_W`: Window width (default: 800)
- `WND_H`: Window height (default: 600)
- `FPS`: Target frame rate (default: 60)

## Technologies Used

- **Language**: C++
- **Graphics APIs**: DirectX 9, DirectX 10, DirectX 11
- **Particle System**: Effekseer
- **Build System**: MSBuild (Visual Studio)
- **Version Control**: Git

## Development Notes

- The project uses Japanese comments in the source code
- Debug visualization can be enabled for collision shapes
- The game supports both windowed and fullscreen modes
- Mesh data includes both skinned (animated) and static meshes

## License

*(License information should be added here)*

## Contributing

*(Contribution guidelines should be added here)*

## Acknowledgments

- Effekseer particle system library
- Microsoft DirectX SDK

---

**Note**: This is a learning/demonstration project showcasing 3D game development techniques with DirectX.
