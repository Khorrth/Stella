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

// TODO: Add localization infrastructure.
// TODO: Add error handling everywhere.
// TODO: Review scopes and acces-restrictions.
// TODO: Fix surface resizing. Why it always deos resize from the same size?
// TODO: Add doublebuffering.
// TODO: Add delta-timing;
// TODO: Reorganize rendering. Add colors macros.
// TODO: Restrict surface resizing.
// TODO: Fix possible memory leaks.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "stella.h"

#define UNSUCCESFUL_WAYLAND_DISPLAY_OPEN_RETURN_CODE 1

wayland_buffer   *main_wayland_buffer; 
void             *main_wayland_buffer_content;

cairo_surface    *main_cairo_surface;
cairo_context    *main_cairo_surface_context;

wayland_surface  *main_wayland_surface;
linear_dimension  main_wayland_surface_width;
linear_dimension  main_wayland_surface_height;

xdg_surface      *main_xdg_surface;

void handle_main_xdg_surface_close_event();

void handle_main_xdg_surface_configure_event
	(void *data, xdg_surface *surface, int width, int height,
	wayland_array *states, uint32_t serial);
	
void update_main_wayland_surface
	(void *data, wayland_callback *current_repaint_callback, milliseconds time);

void render_main_scene();

void handle_main_xdg_surface_close_event()
{
	ignore_event();
}

void handle_main_xdg_surface_configure_event
	(void *data, xdg_surface *surface, int width, int height,
	wayland_array *states, uint32_t serial)
{	
	if (abs(width) == INVALID_LINEAR_DIMENSION)
	{
		return;
	}
	if (abs(height) == INVALID_LINEAR_DIMENSION)
	{
		return;
	}
	
	main_wayland_surface_width  = abs(width);
	main_wayland_surface_height = abs(height);
	
	main_wayland_buffer = 
	create_wayland_buffer(&main_wayland_buffer_content, width, height);
	
	render_main_scene();
	
	if (main_wayland_buffer == INVALID_WAYLAND_BUFFER)
	{
		return;
	}
	
	attach_wayland_buffer(main_wayland_surface, main_wayland_buffer);
	
	damage_whole_wayland_surface(main_wayland_surface);
	
	xdg_surface_ack_configure(main_xdg_surface, serial);
	
	wl_surface_commit(main_wayland_surface);
}

xdg_surface_event_handler
main_xdg_surface_event_handler =
{
	handle_main_xdg_surface_configure_event,
	handle_main_xdg_surface_close_event
};

void update_main_wayland_surface
	(void *data, wayland_callback *current_repaint_callback, milliseconds time)
{
	wl_callback_destroy(current_repaint_callback);
	
	render_main_scene();
	attach_wayland_buffer(main_wayland_surface, main_wayland_buffer);
	damage_whole_wayland_surface(main_wayland_surface);
	
	add_wayland_surface_update_event_handler
		(main_wayland_surface, update_main_wayland_surface);
	
	wl_surface_commit(main_wayland_surface);
}

void render_main_scene()
{
	main_cairo_surface = 
	cairo_image_surface_create_for_data
		(main_wayland_buffer_content, CAIRO_ARGB8888_FORMAT,
		main_wayland_surface_width, main_wayland_surface_height, 
		main_wayland_surface_width * CAIRO_ARGB8888_PIXEL_SIZE);

	main_cairo_surface_context = cairo_create(main_cairo_surface);
	
	cairo_set_source_rgba
		(main_cairo_surface_context, 
		(rand()%256)/255.0, (rand()%256)/255.0, 1, 1);
	cairo_rectangle(main_cairo_surface_context, 0, 0, 100, 100);
	cairo_fill(main_cairo_surface_context);
	cairo_destroy(main_cairo_surface_context);
	cairo_surface_destroy(main_cairo_surface);
}

int main()
{	
	wayland_display *primary_wayland_display;
	
	primary_wayland_display = open_primary_wayland_display();
	
	if (primary_wayland_display == INVALID_WAYLAND_DISPLAY)
	{
		return UNSUCCESFUL_WAYLAND_DISPLAY_OPEN_RETURN_CODE;
	}
	
	main_wayland_surface_width  = DEFAULT_WAYLAND_SURFACE_WIDTH;
	main_wayland_surface_height = DEFAULT_WAYLAND_SURFACE_HEIGHT;
	
	main_wayland_buffer = 
	create_wayland_buffer
		(&main_wayland_buffer_content,
		DEFAULT_WAYLAND_SURFACE_WIDTH, DEFAULT_WAYLAND_SURFACE_HEIGHT);
		
	main_wayland_surface = create_wayland_surface(main_wayland_buffer);
	
	main_xdg_surface =
	create_xdg_surface
		(main_wayland_surface,
		DEFAULT_WAYLAND_SURFACE_X, DEFAULT_WAYLAND_SURFACE_Y,
		DEFAULT_WAYLAND_SURFACE_WIDTH, DEFAULT_WAYLAND_SURFACE_HEIGHT);
	
	xdg_surface_add_listener
		(main_xdg_surface, &main_xdg_surface_event_handler,
		NO_WAYLAND_REQUEST_USER_DATA);
	
	add_wayland_surface_update_event_handler
		(main_wayland_surface, update_main_wayland_surface);

	wl_surface_commit(main_wayland_surface);
	
	while
	((wl_display_dispatch(primary_wayland_display)) != 
	(UNSUCCESFUL_WAYLAND_DISPLAY_DISPATCH))
	{
		
	}
	
	wl_display_disconnect(primary_wayland_display);
	printf("Wayland display connection closed succesfuly.\n");
}
