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

// TODO: Add error checks.
// TODO: Add XDG_RUNTIME_DIR-based shared buffer directory.
// TODO: Refactor "create_wayland_buffer".

// NOTE: "create_wayland_buffer" uses lseek to make buffer grow only.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "stella.h"

wayland_display                *primary_wayland_display;
wayland_registry               *primary_wayland_registry;
wayland_registry_event_handler primary_wayland_registry_event_handler;
wayland_compositor             *primary_wayland_compositor;
xdg_shell                      *primary_xdg_shell;
wayland_shared_memory_manager  *primary_wayland_shared_memory_manager;

wayland_display *
open_primary_wayland_display()
{
	primary_wayland_display = wl_display_connect(PRIMARY_WAYLAND_DISPLAY_NAME);
	
	if (primary_wayland_display == INVALID_WAYLAND_DISPLAY)
	{
		printf("Failed to open Wayland display.\n");
		return INVALID_WAYLAND_DISPLAY;
	}
	
	primary_wayland_registry = wl_display_get_registry(primary_wayland_display);
	
	if (primary_wayland_registry == INVALID_WAYLAND_REGISTRY)
	{
		printf("Failed to open Wayland display.\n");
		return INVALID_WAYLAND_DISPLAY;
	}
	
	wl_registry_add_listener
		(primary_wayland_registry, &primary_wayland_registry_event_handler, NULL);
	
	wl_display_dispatch(primary_wayland_display);
	wl_display_roundtrip(primary_wayland_display);

	return primary_wayland_display;
}

wayland_buffer *
create_wayland_buffer
	(void **content, linear_dimension width, linear_dimension height)
{
	wayland_buffer *buffer;
	
	if (width == INVALID_LINEAR_DIMENSION)
	{
		return INVALID_WAYLAND_BUFFER;
	}
	if (height == INVALID_LINEAR_DIMENSION)
	{
		return INVALID_WAYLAND_BUFFER;
	}
	
	linear_dimension buffer_element_size = WAYLAND_ARGB8888_PIXEL_SIZE;
	linear_dimension       buffer_stride = width * buffer_element_size;
	quadratic_dimension      buffer_size = buffer_stride * height;

	file_descriptor buffer_file_descriptor;
	
	buffer_file_descriptor =
	open
		(TEMPORARY_BUFFER_FILE_NAME, OPEN_READ_WRITE_MODE | OPEN_CREATE_MODE,
		DEFAULT_OPEN_PERMISSIONS);
	
	if (buffer_file_descriptor == INVALID_FILE_DESCRIPTOR)
	{
		return INVALID_WAYLAND_BUFFER;
	}

	array_element_index last_buffer_element_index =
	calculate_last_array_element_index(buffer_size);
	
	lseek(buffer_file_descriptor, last_buffer_element_index, SEEK_SET);
	write(buffer_file_descriptor, "\0", 1);
	
	*content =
	mmap
		(DEFAULT_MEMORY_MAPPING_ADDRESS, buffer_size, PROT_READ | PROT_WRITE,
		MAP_SHARED, buffer_file_descriptor, NO_MEMORY_MAPPING_OFFSET);
	
	if (*content == USUCCESFUL_MEMORY_MAPPING)
	{
		munmap(*content, buffer_size);
		close(buffer_file_descriptor);
		return INVALID_WAYLAND_BUFFER;
	}
	
	memset(*content, DEFAULT_WAYLAND_BUFFER_CONTENT, buffer_size);
	
	wayland_shared_memory_pool_manager *pool = 
	wl_shm_create_pool
		(primary_wayland_shared_memory_manager, buffer_file_descriptor,
		buffer_size);
	
	close(buffer_file_descriptor);
	
	buffer = 
	wl_shm_pool_create_buffer
		(pool, NO_WAYLAND_BUFFER_CONTENT_OFFSET, width, height, buffer_stride,
		WL_SHM_FORMAT_ARGB8888);
	
	wl_shm_pool_destroy(pool);
	
	return buffer;
}

void
attach_wayland_buffer
	(wayland_surface* surface, wayland_buffer* buffer)
{
	if (surface == INVALID_WAYLAND_SURFACE)
	{
		return;
	}
	
	if (buffer == INVALID_WAYLAND_BUFFER)
	{
		return;
	}
	
	wl_surface_attach
		(surface, buffer,
		DEFAULT_WAYLAND_BUFFER_ATTACHMENT_X_OFFSET,
		DEFAULT_WAYLAND_BUFFER_ATTACHMENT_Y_OFFSET);
}

wayland_surface *
create_wayland_surface
	(wayland_buffer *buffer)
{
	wayland_surface *surface;

	if (primary_wayland_compositor == INVALID_WAYLAND_COMPOSITOR)
	{
		return INVALID_WAYLAND_SURFACE;
	}
	
	surface = wl_compositor_create_surface(primary_wayland_compositor);

	if (surface == INVALID_WAYLAND_SURFACE)
	{
		return INVALID_WAYLAND_SURFACE;
	}
	
	if (buffer == INVALID_WAYLAND_BUFFER)
	{
		return INVALID_WAYLAND_SURFACE;
	}
	
	wl_surface_attach
		(surface, buffer, NO_WAYLAND_BUFFER_ATTACHMENT_X_OFFSET, 
		NO_WAYLAND_BUFFER_ATTACHMENT_Y_OFFSET);
	
	damage_whole_wayland_surface(surface);
	
	wl_surface_commit(surface);
	
	return surface;
}

void
damage_whole_wayland_surface
	(wayland_surface *surface)
{
	if (surface == INVALID_WAYLAND_SURFACE)
	{
		return;
	}
	
	wl_surface_damage
		(surface,
		MINIMAL_WAYLAND_SURFACE_DAMAGE_X_OFFSET,
		MINIMAL_WAYLAND_SURFACE_DAMAGE_Y_OFFSET,
		MAXIMAL_WAYLAND_SURFACE_DAMAGE_WIDTH,
		MAXIMAL_WAYLAND_SURFACE_DAMAGE_HEIGHT);
}

void
add_wayland_surface_update_event_handler
	(wayland_surface *surface,
	wayland_surface_update_event_handler handle_update_event)
{
	wayland_callback_event_handler *surface_update_event_handler; 
	
	surface_update_event_handler =
	malloc(sizeof *surface_update_event_handler);
	
	*surface_update_event_handler =
	(wayland_callback_event_handler){ handle_update_event };
	
	wayland_callback *repaint_callback = wl_surface_frame(surface);
	
	wl_callback_add_listener
		(repaint_callback, surface_update_event_handler,
		NO_WAYLAND_REQUEST_USER_DATA);
}

xdg_surface *
create_xdg_surface
	(wayland_surface* surface, coordinate x, coordinate y,
	linear_dimension width, linear_dimension height)
{
	xdg_surface *xdg_surface;

	if (primary_xdg_shell == INVALID_XDG_SHELL)
	{
		return INVALID_XDG_SURFACE;
	}
	
	if (surface == INVALID_WAYLAND_SURFACE) 
	{
		return INVALID_XDG_SURFACE;
	}
	
	xdg_surface = xdg_shell_get_xdg_surface(primary_xdg_shell, surface);

	if (xdg_surface == INVALID_XDG_SURFACE)
	{
		return INVALID_XDG_SURFACE;
	}
	
	xdg_surface_set_window_geometry(xdg_surface, x, y, width, height);
	
	return xdg_surface;
}

void
ignore_event()
{

}

array_element_index
calculate_last_array_element_index
	(linear_dimension array_size)
{
	return array_size - 1;
}

void
handle_xdg_shell_ping_event
	(void *data, xdg_shell *shell, uint32_t serial)
{
	xdg_shell_pong(shell, serial);
}

xdg_shell_event_handler
primary_xdg_shell_event_handler = 
{
	handle_xdg_shell_ping_event
};

void
handle_new_wayland_buffer_format_discovered_event
	(void *data, wayland_shared_memory_manager *shared_memory_manager,
	uint32_t buffer_format)
{
	ignore_event();
}

wayland_shared_memory_manager_event_handler
primary_wayland_shared_memory_manager_event_handler =
{ 
	handle_new_wayland_buffer_format_discovered_event
};


void
handle_wayland_registry_object_removed_event
	(void *data, wayland_registry *registry, uint32_t id)
{
	ignore_event();
}

void
handle_new_wayland_registry_object_discovered_event
	(void *data, wayland_registry *registry, uint32_t id,
	const char *interface_name, uint32_t version)
{
	if 
	((strcmp(interface_name, WAYLAND_COMPOSITOR_INTERFACE_NAME)) ==
	(SUCCESFUL_STRING_COMPARISON))
	{
		primary_wayland_compositor =
		wl_registry_bind(registry, id, &wl_compositor_interface, version);
	}
	else if
	((strcmp(interface_name, XDG_SHELL_INTERFACE_NAME)) ==
	(SUCCESFUL_STRING_COMPARISON))
	{
		primary_xdg_shell =
		wl_registry_bind(registry, id, &xdg_shell_interface, version);
		
		xdg_shell_add_listener
			(primary_xdg_shell, &primary_xdg_shell_event_handler,
			 NO_WAYLAND_REQUEST_USER_DATA);
			
		xdg_shell_use_unstable_version
			(primary_xdg_shell,
			 XDG_SHELL_CURRENT_UNSTABLE_VERSION);
	}
	else if
	((strcmp(interface_name, WAYLAND_SHARED_MEMORY_MANAGER_INTERFACE_NAME)) ==
	(SUCCESFUL_STRING_COMPARISON))
	{
		primary_wayland_shared_memory_manager =
		wl_registry_bind(registry, id, &wl_shm_interface, version);
		
		wl_shm_add_listener
			(primary_wayland_shared_memory_manager,
			&primary_wayland_shared_memory_manager_event_handler,
			NO_WAYLAND_REQUEST_USER_DATA);
	}
}

wayland_registry_event_handler
primary_wayland_registry_event_handler =
{
	handle_new_wayland_registry_object_discovered_event,
	handle_wayland_registry_object_removed_event
};
