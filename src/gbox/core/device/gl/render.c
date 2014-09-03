/*!The Graphic Box Library
 * 
 * GBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * GBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with GBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2014 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        render.c
 * @ingroup     core
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "gl_render"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "render.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t gb_gl_render_init(gb_gl_device_ref_t device)
{
    // check
    tb_assert_and_check_return_val(device, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
#if 0
        // init for gl >= 2.0
        if (device->version >= 0x20)
        {	

        }
#endif

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
tb_void_t gb_gl_render_fill(gb_gl_device_ref_t device, gb_point_t const* points, tb_size_t const* counts)
{
}
tb_void_t gb_gl_render_stok(gb_gl_device_ref_t device, gb_point_t const* points, tb_size_t const* counts)
{
}
tb_void_t gb_gl_render_exit(gb_gl_device_ref_t device)
{
}

