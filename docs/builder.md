# Theater::Builder

The ` Theater::Builder ` is the Entry point of your Application.
It provides methods, to setup your stage. Therefore it should only be used in your ` main ` function.

Once the ` Theater::Builder ` is set up you can use its ` Play(Theater::Scene*) ` method, to start playing your Scene on the Stage

## Example
```c++

int main(int argc, char **argv) {

  DemoScene sc; // <-- this must come from you somewhere 
  //                   see ./scenes.md for more info

  Theater::Builder(480, 320, 2)
      .Title("RayLib - Window 🎉") // All Setters can be chained
      .Play(&sc);                  // if all is set, call Play

  return 0; // Then end main.
}
```

## Available None Setter Methods

## Available Setter Methods
All Setter can be chained.

### Title
```c++
Builder Title(const char *title);
```
Sets your Application-Windows title to the given ` title `.

#### Param:
| name    | type          | description                 |
|---------|---------------|-----------------------------|
| `title` | `const char*` | The new title of the Window |

---

### BorderColor
```c++
Builder BorderColor(Color color)
```
When the window is resized or maximised, its aspect ratio may 
not be the same as for the stage. Therefore some borders may 
appear, to keep the Stages aspect ratio in tact.

This function defines what color the shown borders will be.

#### Param:
| name    | type                                   | description              |
|---------|----------------------------------------|--------------------------|
| `color` | `Color`<br><small>(via RayLib)</small> | The Color fo the Borders |

---


### BackgroundColor
```c++
Builder BackgroundColor(Color color)
```
This is a color, that the stage will show for each pixel, that is 
not drawn to. All pixels are reset to this color each cycle.

#### Param:
| name    | type                                   | description                  |
|---------|----------------------------------------|------------------------------|
| `color` | `Color`<br><small>(via RayLib)</small> | The Color for the Background |

---