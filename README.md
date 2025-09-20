# ObjectPool Unreal Engine Module

**ObjectPool** is a robust Unreal Engine module that provides efficient runtime object pooling for Actors and Components. It streamlines the dynamic reuse of in-game objects, significantly optimizing performance, especially in scenarios with frequent spawning and destruction such as VFX, projectiles, or temporary gameplay objects.

This implementation offers easy setup, expandability, time-based shrinking, and can be integrated either directly via components or by spawning pool proxies.

***

## Features

- Actor & component pooling based on Unreal’s Actor/Component system
- Blueprint and C++ support for initialization, spawning, releasing, and pool shrinking
- Dynamic expand and shrink support with initial/max pool size control
- Time-based and usage-based cleanup management
- Multicast delegate notifications for pool events
- Proxy actor for advanced pooling/sharing scenarios
- Debug/validation/diagnostics utilities

***

## Tech Stack

- **Language**: C++
- **Framework**: Unreal Engine (tested with 4.x and 5.x)
- **Key Classes**: `UOPPoolableActorComponent`, `AOPPoolProxy`, `AOPPoolableActor`
- **Module**: Declared and loaded as a standard Unreal Engine `IModuleInterface` plugin

***

## Installation

### Prerequisites

- Unreal Engine 4.x or 5.x
- Include this module in your project's plugins or as source

### Setup Steps

1. **Add Module**: Copy the source folder of this module into your project's `Plugins` directory (or add to your source tree as desired).
2. **Register Module**: Amend your `.uproject` or `.uplugin` to include `ObjectPool` as a dependency.
3. **Regenerate Project Files**: Right-click your `.uproject` file and select “Generate Visual Studio project files”.
4. **Compile**: Open in your C++ IDE and build the project.

***

## Usage

### Blueprint Usage

1. **Add Component**: Attach `OPPoolableActorComponent` to an Actor.
2. **Configure**: Set the pooled actor class, initial pool size, max pool size, and expansion/shrink settings.
3. **Initialize Pool**:
   ```cpp
   PoolComponent->InitializePool();
   ```
4. **Spawn/Acquire**:
   ```cpp
   auto* Actor = PoolComponent->GetPooledActor();
   ```
5. **Release**:
   ```cpp
   PoolComponent->ReleaseActor(Actor);
   ```

### C++ Example

```cpp
UOPPoolableActorComponent* PoolComp = CreateDefaultSubobject<UOPPoolableActorComponent>(TEXT("MyPoolComp"));
PoolComp->PooledActorClass = AMyProjectile::StaticClass();
PoolComp->InitialPoolSize = 20;
PoolComp->MaxPoolSize = 100;
PoolComp->bDynamicallyExpandable = true;

PoolComp->InitializePool();

// Acquiring a pooled actor
AOPPoolableActor* Actor = PoolComp->GetPooledActor();
if (Actor)
{
    // Use your actor here
}

// When done
PoolComp->ReleaseActor(Actor);
```

***

## API Documentation

### Main Components

| Class                        | Purpose/Usage                                                      |
|------------------------------|--------------------------------------------------------------------|
| `UOPPoolableActorComponent`  | Attach to any actor. Manages lifecycle, pooling logic, exposes methods for creation/acquire/release/resize callbacks. |
| `AOPPoolProxy`               | Standalone pooled Actor for advanced or shared object pool usage.   |
| `AOPPoolableActor`           | Base class for pooled actors. Implements `OnAcquired`, `OnReleased`, `IsActive`, etc.         |
| `UOPObjectPoolSettings`      | Project-wide default pooling settings configuration.                |

#### Key Methods
- `InitializePool()`
- `GetPooledActor(EOPGetPooledActorErrorMode InErrorMode = EOPGetPooledActorErrorMode::Ignore)`
- `ReleaseActor(AOPPoolableActor* ActorToRelease)`
- `AdjustPoolSize(int32 NewInitialSize, int32 NewMaxSize)`
- `ForceShrinkPool()`
- `IsInitialized() const`
- `LogCurrentPoolableActors()`

#### Important Properties
- `InitialPoolSize`: Initial pool allocation
- `MaxPoolSize`: Hard limit (unless dynamically expandable)
- `bDynamicallyExpandable`: Allows pool growth beyond initial size
- `ShrinkCheckInterval` and `InactivityThreshold`: Fine-tune cleanup timing

***

## Contributing

Contributions welcome! Please follow typical Unreal module conventions:

- Fork the repo and create feature branch
- Adhere to Unreal coding standards & documentation style
- Submit clean pull requests with detailed descriptions
- Include tests or example use if possible

***

## License

See the LICENSE.MD file. (If based on Epic Games code or for Epic Marketplace distribution, observe relevant copyright.)

***

**Note:** The module heavily follows Unreal Engine standards for plugin/module structure and leverages both Blueprint and C++ extensibility points.
