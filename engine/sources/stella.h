/*
	Copyright 2016 Alexandr Gulak

	This file is part of Stella.

	Stella is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	Stella is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Stella. If not, see <http://www.gnu.org/licenses/>.
*/

// TODO: Add cairo operators macros.
// TODO: Add all the available Wayland interfaces names macros.
// TODO: Add file acces macros to use with "open".
// TODO: Review "wayland_callback" name.
// TODO: Add different buffer formats support.
// TODO: Add several displays handling.

// NOTE: "XDG_SHELL_CURRENT_UNSTABLE_VERSION" macro should be used with "xdg_shell_use_unstable_version".
// NOTE: "SUCCESFUL_STRING_COMPARISON" macro should be used with "strcomp".
// NOTE: Further documentation can be found in Wayland (and Wayland protcols) repository in Internet.

#ifndef STELLA
#define STELLA

#include "xdg-shell-client-protocol.h"
#include <cairo/cairo.h>

typedef struct           wl_display wayland_display;
typedef struct  wl_display_listener wayland_display_event_handler;
typedef struct          wl_registry wayland_registry;
typedef struct wl_registry_listener wayland_registry_event_handler;
typedef struct        wl_compositor wayland_compositor;
typedef struct               wl_shm wayland_shared_memory_manager;
typedef struct      wl_shm_listener wayland_shared_memory_manager_event_handler;
typedef struct          wl_shm_pool wayland_shared_memory_pool_manager;
typedef struct          wl_callback wayland_callback;
typedef struct wl_callback_listener wayland_callback_event_handler;
typedef struct            wl_buffer wayland_buffer;
typedef struct           wl_surface wayland_surface;
typedef struct             wl_array wayland_array;

typedef struct            xdg_shell xdg_shell;
typedef struct   xdg_shell_listener xdg_shell_event_handler;
typedef struct          xdg_surface xdg_surface;
typedef struct xdg_surface_listener xdg_surface_event_handler;

typedef                     cairo_t cairo_context;
typedef             cairo_surface_t cairo_surface; 

typedef                    uint32_t milliseconds;
typedef                         int coordinate;
typedef                         int file_descriptor;
typedef                    uint32_t array_element_index;
typedef                    uint32_t linear_dimension;
typedef                    uint32_t quadratic_dimension;

typedef void (*wayland_surface_update_event_handler)
	(void *data, wayland_callback *current_repaint_callback, 
	uint32_t callback_data);

#define                DEFAULT_WAYLAND_SURFACE_WIDTH 640
#define               DEFAULT_WAYLAND_SURFACE_HEIGHT 360
#define                    DEFAULT_WAYLAND_SURFACE_X 0
#define                    DEFAULT_WAYLAND_SURFACE_Y 0
#define   DEFAULT_WAYLAND_BUFFER_ATTACHMENT_X_OFFSET 0
#define   DEFAULT_WAYLAND_BUFFER_ATTACHMENT_Y_OFFSET 0
#define      DEFAULT_WAYLAND_SURFACE_DAMAGE_X_OFFSET 0
#define      DEFAULT_WAYLAND_SURFACE_DAMAGE_Y_OFFSET 0
#define         DEFAULT_WAYLAND_SURFACE_DAMAGE_WIDTH INT32_MAX
#define        DEFAULT_WAYLAND_SURFACE_DAMAGE_HEIGHT INT32_MAX
#define      DEFAULT_WAYLAND_EVENT_HANDLER_USER_DATA NULL

#define               DEFAULT_WAYLAND_BUFFER_CONTENT 0x20

#define                 NO_WAYLAND_REQUEST_USER_DATA NULL
#define        NO_WAYLAND_BUFFER_ATTACHMENT_X_OFFSET 0
#define        NO_WAYLAND_BUFFER_ATTACHMENT_Y_OFFSET 0

#define             NO_WAYLAND_BUFFER_CONTENT_OFFSET 0

#define      MINIMAL_WAYLAND_SURFACE_DAMAGE_X_OFFSET 0
#define      MINIMAL_WAYLAND_SURFACE_DAMAGE_Y_OFFSET 0
#define         MAXIMAL_WAYLAND_SURFACE_DAMAGE_WIDTH INT32_MAX
#define        MAXIMAL_WAYLAND_SURFACE_DAMAGE_HEIGHT INT32_MAX

#define                 PRIMARY_WAYLAND_DISPLAY_NAME NULL

#define                       INVALID_WAYLAND_BUFFER NULL
#define                      INVALID_WAYLAND_SURFACE NULL
#define                          INVALID_XDG_SURFACE NULL
#define                            INVALID_XDG_SHELL NULL
#define                      INVALID_WAYLAND_DISPLAY NULL
#define                   INVALID_WAYLAND_COMPOSITOR NULL
#define                     INVALID_WAYLAND_REGISTRY NULL
#define         UNSUCCESFUL_WAYLAND_DISPLAY_DISPATCH -1
#define                      INVALID_FILE_DESCRIPTOR -1

#define            WAYLAND_COMPOSITOR_INTERFACE_NAME "wl_compositor"
#define                     XDG_SHELL_INTERFACE_NAME "xdg_shell"
#define WAYLAND_SHARED_MEMORY_MANAGER_INTERFACE_NAME "wl_shm"

#define                  WAYLAND_ARGB8888_PIXEL_SIZE 4
#define                    CAIRO_ARGB8888_PIXEL_SIZE 4
#define                        CAIRO_ARGB8888_FORMAT CAIRO_FORMAT_ARGB32

#define                         OPEN_READ_WRITE_MODE O_RDWR
#define                             OPEN_CREATE_MODE O_CREAT
#define                     DEFAULT_OPEN_PERMISSIONS 0600

#define                   TEMPORARY_BUFFER_FILE_NAME "temporary_buffer_file"
	
#define                     NO_MEMORY_MAPPING_OFFSET 0
#define               DEFAULT_MEMORY_MAPPING_ADDRESS NULL
#define                    USUCCESFUL_MEMORY_MAPPING MAP_FAILED

#define                  SUCCESFUL_STRING_COMPARISON 0

#define           XDG_SHELL_CURRENT_UNSTABLE_VERSION 5
	
#define                     INVALID_LINEAR_DIMENSION 0

/* Interface */

wayland_display *open_primary_wayland_display();

wayland_buffer *create_wayland_buffer
	(void** content, linear_dimension width, linear_dimension height);
void attach_wayland_buffer(wayland_surface *surface, wayland_buffer *buffer);

wayland_surface *create_wayland_surface(wayland_buffer *buffer);
void damage_whole_wayland_surface(wayland_surface *surface);
void
add_wayland_surface_update_event_handler
	(wayland_surface *surface,
	wayland_surface_update_event_handler handle_update_event);

xdg_surface
*create_xdg_surface
	(wayland_surface *surface, coordinate x, coordinate y,
	linear_dimension width, linear_dimension height);

void
ignore_event();

array_element_index
calculate_last_array_element_index(linear_dimension array_size);

#endif
