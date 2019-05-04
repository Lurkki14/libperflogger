/*
This file is part of libperflogger.

Copyright (c) 2019 Jussi Kuokkanen

Libperflogger is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Libperflogger is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libperflogger.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef VULKAN_HOOKER_H_
#define VULKAN_HOOKER_H_

void *get_real_vk_function(const char *name);
void *get_hooked_vk_function(const char *name);

#endif
