# Contributing

## Compilation

- [C++20](https://en.cppreference.com/w/cpp/20) standard
- [CMake](https://cmake.org/) building tool
- Handle [GCC](https://gcc.gnu.org/), [CLang](https://clang.llvm.org/) and [MinGW](https://www.mingw-w64.org/) compilers
- Cross compilation for Windows with MinGW

## Libraries

- [OpenCV](https://github.com/opencv/opencv) - Computer vision library to read inputs via OCR on a given image
- [nlohmann-json](https://github.com/nlohmann/json) - Allow to use JSON representation of a game for archived and later
  reprocessing
- [quill](https://github.com/odygrd/quill) - Quill is a cross-platform low latency logging library based on C++14/C++17.
- [uwebsockets](https://github.com/uNetworking/uWebSockets) - Simple, secure & standards compliant web server for the most
  demanding of applications
- [GoogleTest](https://github.com/google/googletest) - Testing library for runnings services unit tests

## Docker

A [Docker](https://www.docker.com/) environment is provided to facilitate the build and deployment process. It includes several
builder images, each tailored for specific aspects of the project's build and deployment process.

All Docker images are hosted on
the [GitHub Container Registry](https://github.com/mister-good-deal?tab=packages&repo_name=poker-tracker-cpp) (GHCR) under
the `mister-good-deal/poker-tracker-cpp` repository.

You can find more information about the Docker environment in the [docker/README.md](docker/README.md) file.

### Development

To start developing, you can use the provided Docker environment. The `dev-env` image is tailored for development tasks and includes
additional development tools. It contains GCC, Clang, and MinGW compilers, as well as various libraries and dependencies.

To use the development environment, run the following command:

```bash
docker run -it --rm -v $(pwd):/app ghcr.io/mister-good-deal/poker-tracker-cpp/dev-env:latest
```

## Services

The tracker is composed of multiple services that work together to provide its functionality.

### Logger

This service is responsible for handling logging functionalities. It provides logging capabilities for the entire tracker.

*Services used*

- None

*Libraries used*

- quill

### Game Handler

This service represents the game status and history. It logs all the game's events and can serialize them in JSON format.

*Services used*

- Logger

*Libraries used*

- nlohmann-json

### Websockets

This service handles communication between the poker tracker via websockets and the frontend app. It enables real-time data exchange between
the
tracker and the frontend VueJs application.

*Services used*

- Logger

*Libraries used*

- uwebsockets

### OCR

This service reads inputs from a poker room screenshot and extracts all the necessary information from it using optical character
recognition (OCR) techniques.

*Services used*

- Logger
- Game Handler

*Libraries used*

- OpenCV

### Scrapper

This service scrapes data from the poker room's user interface to gather information about ongoing games, players, and other relevant data.

*Services used*

- Logger
- Game Handler

*Libraries used*

- OpenCV
- X11
- Xext

### Game Session

This service manages the game session and handles game-related functionalities such as starting new games, monitoring a whole game, and
determining game outcomes.

*Services used*

- Logger
- Game Handler
- Scrapper
- OCR

*Libraries used*

- nlohmann-json
- OpenCV

### Dependencies graph

Here the dependencies graph of the services with libraries.

```plantuml
!define AWSPUML https://raw.githubusercontent.com/awslabs/aws-icons-for-plantuml/v16.0/dist

package "Logger Service" {
    component Logger
    folder "Libraries (Logger)" {
        component Quill_Logger
    }
}

package "Game Handler Service" {
    component GameHandler
    folder "Libraries (Game Handler)" {
        component NlohmannJson_GameHandler
    }
}

package "Websockets Service" {
    component Websockets
    folder "Libraries (Websockets)" {
        component UWebsockets_Websockets
    } 
}

package "OCR Service" {
    component OCR
    folder "Libraries (OCR)" {
        component OpenCV_OCR
        component opencv_core_OCR
        component opencv_imgproc_OCR
        component opencv_text_OCR
    }
}

package "Scrapper Service" {
    component Scrapper
    folder "Libraries (Scrapper)" {
        component OpenCV_Scrapper
        component opencv_core_Scrapper
        component opencv_imgproc_Scrapper
        component opencv_imgcodecs_Scrapper
        component X11_Scrapper
        component Xext_Scrapper
    }
}

package "Game Session Service" {
    component GameSession
    folder "Libraries (Game Session)" {
        component NlohmannJson_GameSession
        component OpenCV_GameSession
    }
}

Logger --> GameHandler
Logger --> Websockets
Logger --> OCR
Logger --> GameSession
GameHandler --> OCR
GameHandler --> Scrapper
GameHandler --> GameSession
Scrapper --> GameSession
OCR --> GameSession
```

## Coding

### Git hook

A client side [git hook](https://git-scm.com/book/en/v2/Customizing-Git-Git-Hooks) can be setup to automatically check that your code
is correctly formatted before committing.

The actual setup hook is a `pre-commit` hook located in `hooks/pre-commit` that verify that C++ files are
correctly formatted and correct the files if possible.

To enable this hook run `init.sh` at the root project directory.

To bypass this hook you can commit with the following option `git commit --no-verify`.

### Apply on all the project source files

To apply the code formatting on all project source files at start, use the following command :

```bash
find services -regex '.*\.\(cpp\|hpp\)' -exec clang-format -style=file -i {} \;
```
