# RayTheater - UI - Label

Labels are a way to display Text on the Stage.

## Example
```c++
float posX = 2;
float posY = 2;
std::string text = "";

Theater::UI::Label myLabel;

myLabel.Text(text)
    ->Position(posX, posY)
    ->Style({
        fontSize: 10
    });
```



## Methods

### Constructors
```c++
Theater::UI::Label();
Theater::UI::Label(std::string);
```


### Position
```c++
Theater::UI::Label* Position(float x, float y);
```
Sets the Pixel-Position of the label on the Stage


### Text
```c++
Theater::UI::Label* Text(std::string);
```

Changes, what text the Label is displaying


### Style 
```c++
Theater::UI::Label *Style(Theater::UI::Label::UIStyle *);
```

The Labels appearance can be modified via a [UIStyle](./style.md).


