# Stella Development Conventions

## Versioning

Every version must consists of two numbers:

```
X.Z
```

"X" must be changed only in case if backward-compability broken. "Z" must be changed every release.

## Project Structure

Currently porject structure is as in the example.

```
 + project folder
 - + engine
 - - + sources
 - - + binaries
 - build script
 - run script
```

Project must have its building and running script.

Every project components should have own folder (binaries, sources, engine, documentation, resources etc). Another example will explain the strategy.

```
 + project folder
 - + engine
 - - + sources
 - - - - stella.c
 - - + binaries
 - - - - stella.o
 - + documentation
 - - + sources
 - - - - api-reference.md
 - - + binaries
 - - - - api-reference.html
 - + resources
 - - + sources
 - - - - logotype.svg
 - - + binaries
 - - - - logotype.png
 - build script
 - run script
```

## Coding Conventions

Some conventions may be not mentioned here. Be aware, analyze existing code. Sometimes it is needed to alias some stuff to fit the style, feel free to do it. Keep library as pimitive as possible.

### Comments

Every source file should have heading comments.

```
/*
	Some information should be here.
*/

// TODO: All the "TODO" comments must go here.

// NOTE: The same must "NOTE" comments.
```

"TODO" comments should be imperative and they should denote development plans. "NOTE" comments are used for providing some important information, they should be declarative.

These comments should be associated with the source file containing them.

```
// TODO: Add double buffering.

// NOTE: "SUCCESFUL_STRING_COMPARISON" macro should be used with "strcomp".
```

To logically separate parts of code section headers could be used.

```
/* Button Rendering Done Here */

draw_rectangle();
draw_label();
```

### Headers Inlcusion

System headers should be included firstly, internal ones - lastly.

```
#include <stdio.h>
#include <stdlib.h>

#include "stella.h"
```

### Naming

All the identifiers must follow "lower_dash" style.

```
this_is_more_readable;

ThisIsNot;
```

Do not use common names. Use prefixes instead. Notice how the code is aligned and indented.

```
/* Wrong */

surface s;
buffer  buff;

/* Correct */

surface primary_surface;
buffer  primary_surface_buffer;

```

For event-handling structures "event_handler" suffix must be used. Corresponding function names should have prefix "handle" and suffix "event".

```
struct surface_event_handler
{
	handle_close_event,
	handle_moved_event
};
```

Functions must have natural names. Do not use short names or inverted order of words. 

```
/* Wrong */

obj_property_set(new_value);
wl_display_open(display_name);

/* Correct */

set_object_property(new_value);
open_wayland_display(display_name);
```

Long names should be formatted as in the example.

```
set_surface_geometry
	(main_surface, DEFAULT_SURFACE_X, DEFAULT_SURFACE_Y,
	DEFAULT_SURFACE_WIDTH, DEFAULT_SURFACE_WIDTH);
```

### Macros

Every literal must have a semantic macro.

```
#define  ARGB8888_PIXEL_SIZE 4
#define DEFAULT_BUFFER_WIDTH 200
```

Use "ILLEGAL", "SUCCESFUL", "UNSUCCESFUL" prefixes for corresponding macros. Macro defenitions arre aligned in a bit different way.

```
#define              ILLEGAL_BUFFER NULL
#define SUCCESFUL_STRING_COMPARISON 0
```

### Errors Handling

Before calling a function, all the arguments must be validated. If they are invalid, function can return an error value, for example.

```

typedef uint32_t dimension;

#define INVALID_DIMENSION 0

void set_surface_geometry(dimension width, dimension height)
{
	if (width == INVALID_DIMENSION)
	{
		return;
	}

	...
}
```

### Output

Output messages must start with capital letter, end with dot, should be declarative.

```
printf("Wayland display connection failed to open.\n");
```
