# UI-Styles

`UIStyle` is a struct with a list of properties, that can be passed to various [`Theater::UI`](../ui.md) elements.
not every [`Theater::UI`](../ui.md) element supports every property though.

| UI-Style prop     | Type                                     | Description                                                       | UI::Button | UI::Label |
|-------------------|------------------------------------------|-------------------------------------------------------------------|------------|-----------|
| `textColor`       | `Color`<br><small>(via RayLib)</small>   | Color of the elements Text                                        | ✔          | ✔         |
| `font`            | `Font`<br><small>(via RayLib)</small>    | Font used to draw the Element elements Text                       | ✔          | ✔         |
| `fontSize`        | `float`                                  | Size of the elements Text                                         | ✔          | ✔         |
| `backgroundColor` | `Color`<br><small>(via RayLib)</small>   | Background rendered behind the Text                               | ✔          |           |
| `labelOffset`     | `Vector2`<br><small>(via RayLib)</small> | Text Pixel-Offset from the Top-Left Cornor<br>of the Element      | ✔          |           |
| `cornorRadius`    | `float`                                  | how round the cornors are<br>(0.0-1.0, with 0 being no roundness) | ✔          |           |
| `roundTL`         | `bool`                                   | Round the Top-Left cornor                                         | ✔          |           |
| `roundTR`         | `bool`                                   | Round the Top-Right cornor                                        | ✔          |           |
| `roundBL`         | `bool`                                   | Round the Bottom-Left cornor                                      | ✔          |           |
| `roundBR`         | `bool`                                   | Round the Bottom-Right cornor                                     | ✔          |           |
