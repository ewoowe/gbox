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
 * @file        mesh.c
 * @ingroup     utils
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mesh.h"
#include "impl/mesh_edge.h"
#include "impl/mesh_face.h"
#include "impl/mesh_vertex.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the mesh impl type 
typedef struct __gb_mesh_impl_t
{
    // the edges
    gb_mesh_edge_list_ref_t         edges;

    // the faces
    gb_mesh_face_list_ref_t         faces;

    // the vertices
    gb_mesh_vertex_list_ref_t       vertices;

}gb_mesh_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t gb_mesh_orbit_org_set(gb_mesh_edge_ref_t edge, gb_mesh_vertex_ref_t org)
{
    // check
    tb_assert_abort(edge && org);

    // done
    gb_mesh_edge_ref_t scan = edge;
    do
    {
        // set org
        gb_mesh_edge_org_set(scan, org);

        // the next edge
        scan = gb_mesh_edge_onext(scan);
    }
    while (scan != edge);
}
#if 0
static tb_void_t gb_mesh_orbit_lface_set(gb_mesh_edge_ref_t edge, gb_mesh_face_ref_t lface)
{
    // check
    tb_assert_abort(edge && lface);

    // done
    gb_mesh_edge_ref_t scan = edge;
    do
    {
        // set lface
        gb_mesh_edge_lface_set(scan, lface);

        // the next edge
        scan = gb_mesh_edge_onext(scan);
    }
    while (scan != edge);
}
#endif
static gb_mesh_edge_ref_t gb_mesh_orbit_edge(gb_mesh_edge_ref_t edge, gb_mesh_face_ref_t lface)
{
    // check
    tb_assert_abort(edge && lface);

    // done
    gb_mesh_edge_ref_t scan = edge;
    do
    {
        // found?
        if (gb_mesh_edge_lface(scan) == lface) return scan;

        // the next edge
        scan = gb_mesh_edge_onext(scan);
    }
    while (scan != edge);

    // failed
    return tb_null;
}
/* Guibas and Stolfi, simplified since we don't use flips (p98) 
 */
static tb_void_t gb_mesh_splice(gb_mesh_edge_ref_t edge1, gb_mesh_edge_ref_t edge2)
{
    // check
    tb_assert_abort(edge1 && edge2);

	gb_mesh_edge_ref_t onext1 = gb_mesh_edge_onext(edge1);
	gb_mesh_edge_ref_t onext2 = gb_mesh_edge_onext(edge2);

	gb_mesh_edge_oprev_set(onext1, edge2);
	gb_mesh_edge_oprev_set(onext2, edge1);

    gb_mesh_edge_onext_set(edge1, onext2);
	gb_mesh_edge_onext_set(edge2, onext1);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
gb_mesh_ref_t gb_mesh_init(tb_item_func_t edge_func, tb_item_func_t face_func, tb_item_func_t vertex_func)
{
    // done
    tb_bool_t           ok = tb_false;
    gb_mesh_impl_t*     impl = tb_null;
    do
    {
        // make mesh
        impl = tb_malloc0_type(gb_mesh_impl_t);
        tb_assert_and_check_break(impl);
    
        // init edges
        impl->edges = gb_mesh_edge_list_init(edge_func);
        tb_assert_and_check_break(impl->edges);
  
        // init faces
        impl->faces = gb_mesh_face_list_init(face_func);
        tb_assert_and_check_break(impl->faces);

        // init vertices
        impl->vertices = gb_mesh_vertex_list_init(vertex_func);
        tb_assert_and_check_break(impl->vertices);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) gb_mesh_exit((gb_mesh_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (gb_mesh_ref_t)impl;
}
tb_void_t gb_mesh_exit(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl);
   
    // exit edges
    if (impl->edges) gb_mesh_edge_list_exit(impl->edges);
    impl->edges = tb_null;

    // exit faces
    if (impl->faces) gb_mesh_face_list_exit(impl->faces);
    impl->faces = tb_null;

    // exit vertices
    if (impl->vertices) gb_mesh_vertex_list_exit(impl->vertices);
    impl->vertices = tb_null;

    // exit it
    tb_free(impl);
}
tb_void_t gb_mesh_clear(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl);

    // clear edges
    if (impl->edges) gb_mesh_edge_list_clear(impl->edges);

    // clear faces
    if (impl->faces) gb_mesh_face_list_clear(impl->faces);

    // clear vertices
    if (impl->vertices) gb_mesh_vertex_list_clear(impl->vertices);
}
tb_iterator_ref_t gb_mesh_face_itor(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->faces, tb_null);

    // the face iterator
    return gb_mesh_face_list_itor(impl->faces);
}
tb_cpointer_t gb_mesh_face_data(gb_mesh_ref_t mesh, gb_mesh_face_ref_t face)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->faces && face, tb_null);

    // the face data
    return gb_mesh_face_list_data(impl->faces, face);
}
tb_void_t gb_mesh_face_data_set(gb_mesh_ref_t mesh, gb_mesh_face_ref_t face, tb_cpointer_t data)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl && impl->faces && face);

    // set the face data
    gb_mesh_face_list_data_set(impl->faces, face, data);
}
tb_iterator_ref_t gb_mesh_vertex_itor(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->vertices, tb_null);

    // the vertex iterator
    return gb_mesh_vertex_list_itor(impl->vertices);
}
tb_cpointer_t gb_mesh_vertex_data(gb_mesh_ref_t mesh, gb_mesh_vertex_ref_t vertex)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->vertices && vertex, tb_null);

    // the vertex data
    return gb_mesh_vertex_list_data(impl->vertices, vertex);
}
tb_void_t gb_mesh_vertex_data_set(gb_mesh_ref_t mesh, gb_mesh_vertex_ref_t vertex, tb_cpointer_t data)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl && impl->vertices && vertex);

    // set the vertex data
    gb_mesh_vertex_list_data_set(impl->vertices, vertex, data);
}
tb_iterator_ref_t gb_mesh_edge_itor(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->edges, tb_null);

    // the edge iterator
    return gb_mesh_edge_list_itor(impl->edges);
}
tb_cpointer_t gb_mesh_edge_data(gb_mesh_ref_t mesh, gb_mesh_edge_ref_t edge)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->edges && edge, tb_null);

    // the edge data
    return gb_mesh_edge_list_data(impl->edges, edge);
}
tb_void_t gb_mesh_edge_data_set(gb_mesh_ref_t mesh, gb_mesh_edge_ref_t edge, tb_cpointer_t data)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl && impl->edges && edge);

    // set the edge data
    gb_mesh_edge_list_data_set(impl->edges, edge, data);
}
gb_mesh_edge_ref_t gb_mesh_make_edge(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->vertices && impl->faces && impl->edges, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    gb_mesh_edge_ref_t      edge = tb_null;
    gb_mesh_face_ref_t      face = tb_null;
    gb_mesh_vertex_ref_t    org = tb_null;
    gb_mesh_vertex_ref_t    dst = tb_null;
    do
    {
        // make the org
        org = gb_mesh_vertex_list_make(impl->vertices);
        tb_assert_and_check_break(org);

        // make the dst
        dst = gb_mesh_vertex_list_make(impl->vertices);
        tb_assert_and_check_break(dst);

        // make the face
        face = gb_mesh_face_list_make(impl->faces);
        tb_assert_and_check_break(face);

        // make the edge
        edge = gb_mesh_edge_list_make(impl->edges);
        tb_assert_and_check_break(edge);

        // the sym edge
        gb_mesh_edge_ref_t edge_sym = gb_mesh_edge_sym(edge);
        tb_assert_and_check_break(edge_sym);

        // init the edge
        gb_mesh_edge_org_set  (edge, org);
        gb_mesh_edge_lface_set(edge, face);

        // init the sym edge
        gb_mesh_edge_org_set  (edge_sym, dst);
        gb_mesh_edge_lface_set(edge_sym, face);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // kill the org
        if (org) gb_mesh_vertex_list_kill(impl->vertices, org);
        org = tb_null;

        // kill the dst
        if (dst) gb_mesh_vertex_list_kill(impl->vertices, dst);
        dst = tb_null;

        // kill the face
        if (face) gb_mesh_face_list_kill(impl->faces, face);
        face = tb_null;

        // kill the edge
        if (edge) gb_mesh_edge_list_kill(impl->edges, edge);
        edge = tb_null;
    }

    // ok?
    return edge;
}
gb_mesh_edge_ref_t gb_mesh_make_loop_edge(gb_mesh_ref_t mesh, tb_bool_t is_ccw)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->vertices && impl->faces && impl->edges, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    gb_mesh_edge_ref_t      edge = tb_null;
    gb_mesh_face_ref_t      lface = tb_null;
    gb_mesh_face_ref_t      rface = tb_null;
    gb_mesh_vertex_ref_t    vertex = tb_null;
    do
    {
        // make the vertex
        vertex = gb_mesh_vertex_list_make(impl->vertices);
        tb_assert_and_check_break(vertex);

        // make the left face
        lface = gb_mesh_face_list_make(impl->faces);
        tb_assert_and_check_break(lface);

        // make the right face
        rface = gb_mesh_face_list_make(impl->faces);
        tb_assert_and_check_break(rface);

        // make the edge
        edge = gb_mesh_edge_list_make_loop(impl->edges, is_ccw);
        tb_assert_and_check_break(edge);

        // the sym edge
        gb_mesh_edge_ref_t edge_sym = gb_mesh_edge_sym(edge);
        tb_assert_and_check_break(edge_sym);

        // init the edge
        gb_mesh_edge_org_set  (edge, vertex);
        gb_mesh_edge_lface_set(edge, lface);

        // init the sym edge
        gb_mesh_edge_org_set  (edge_sym, vertex);
        gb_mesh_edge_lface_set(edge_sym, rface);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // kill the vertex
        if (vertex) gb_mesh_vertex_list_kill(impl->vertices, vertex);
        vertex = tb_null;

        // kill the left face
        if (lface) gb_mesh_face_list_kill(impl->faces, lface);
        lface = tb_null;

        // kill the right face
        if (rface) gb_mesh_face_list_kill(impl->faces, rface);
        rface = tb_null;

        // kill the edge
        if (edge) gb_mesh_edge_list_kill(impl->edges, edge);
        edge = tb_null;
    }

    // ok?
    return edge;
}
gb_mesh_face_ref_t gb_mesh_make_face_edge(gb_mesh_ref_t mesh, gb_mesh_face_ref_t face, gb_mesh_vertex_ref_t org, gb_mesh_vertex_ref_t dst, gb_mesh_edge_ref_t* pedge)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl, tb_null);

    return tb_null;
}
tb_void_t gb_mesh_kill_face_edge(gb_mesh_ref_t mesh, gb_mesh_edge_ref_t edge)
{
}
gb_mesh_vertex_ref_t gb_mesh_make_vertex_edge(gb_mesh_ref_t mesh, gb_mesh_vertex_ref_t vertex, gb_mesh_face_ref_t lface, gb_mesh_face_ref_t rface, gb_mesh_edge_ref_t* pedge)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return_val(impl && impl->vertices && impl->edges && vertex && lface && rface, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    gb_mesh_edge_ref_t      edge_new = tb_null;
    gb_mesh_vertex_ref_t    vertex_new = tb_null;
    do
    {
        // get the vertex edge
        gb_mesh_edge_ref_t edge = gb_mesh_vertex_edge(vertex);
        tb_assert_abort_and_check_break(edge);

        // get the edge with edge.lface == lface
        gb_mesh_edge_ref_t edge_lf = gb_mesh_orbit_edge(edge, lface);
        tb_assert_abort_and_check_break(edge_lf);

        // get the edge with edge.lface == rface
        gb_mesh_edge_ref_t edge_rf = gb_mesh_orbit_edge(edge, rface);
        tb_assert_abort_and_check_break(edge_rf);

        // make the new vertex
        vertex_new = gb_mesh_vertex_list_make(impl->vertices);
        tb_assert_and_check_break(vertex_new);

        // make the new clockwise self-loop edge
        edge_new = gb_mesh_edge_list_make_loop(impl->edges, tb_false);
        tb_assert_and_check_break(edge_new);

        // the new sym edge
        gb_mesh_edge_ref_t edge_sym_new = gb_mesh_edge_sym(edge_new);
        tb_assert_and_check_break(edge_sym_new);

        /*    
         * before:
         *
         *        edge_new
         *          ----
         *         |    |
         *          <---
         *
         *
         *      .                         lface
         *          .
         *              .    
         *      edge_rf     .               edge_lf
         *  <----------------- vertex ------------------->
         *                  .         .        
         *   rface     .                   . 
         *         .                            . 
         *     .                                     .
         *
         * splice(edge_lf, edge_new):
         *
         *
         *      .                         lface
         *          .         edge_new
         *              .       --->
         *      edge_rf     .  |    |        edge_lf
         *  <----------------- vertex ------------------->
         *                  .         .        
         *   rface     .                   . 
         *         .                            . 
         *     .                                     .
         *
         * splice(edge_rf, edge_sym_new):
         *
         *
         *
         *      .                                     lface
         *          .              edge_sym_new
         *              .       <----------------
         *      edge_rf     .  |                 |           edge_lf
         *  <----------------- vertex       vertex_new  ------------------->
         *                  .                           .        
         *   rface     .                                   . 
         *         .                                          . 
         *     .                                                 .
         *
         *
         *
         *      .                                     lface
         *          .               
         *              .        
         *      edge_rf     .          edge_new                   edge_lf
         *  <----------------- vertex ----------> vertex_new ------------------->
         *                  .                                  .        
         *   rface     .                                          . 
         *         .                                                 . 
         *     .                                                        .
         */       
        gb_mesh_splice(edge_lf, edge_new);
        gb_mesh_splice(edge_rf, edge_sym_new);

        // init the new edge
        gb_mesh_edge_org_set  (edge_new, vertex);
        gb_mesh_edge_lface_set(edge_new, lface);
        gb_mesh_edge_rface_set(edge_new, rface);

        // update origin for all edges leaving the destination orbit of the new edge
        gb_mesh_orbit_org_set(edge_sym_new, vertex_new);

        // save edge
        if (pedge) *pedge = edge_new;

        // ok
        ok = tb_true;

    } while (0);

    // failed
    if (!ok)
    {
        // kill the new edge
        if (edge_new) gb_mesh_edge_list_kill(impl->edges, edge_new);
        edge_new = tb_null;

        // kill the new vertex
        if (vertex_new) gb_mesh_vertex_list_kill(impl->vertices, vertex_new);
        vertex_new = tb_null;
    }

    // ok?
    return vertex_new;
}
tb_void_t gb_mesh_kill_vertex_edge(gb_mesh_ref_t mesh, gb_mesh_edge_ref_t edge)
{
}
#ifdef __gb_debug__
tb_void_t gb_mesh_dump(gb_mesh_ref_t mesh)
{
    // check
    gb_mesh_impl_t* impl = (gb_mesh_impl_t*)mesh;
    tb_assert_and_check_return(impl && impl->vertices && impl->edges && impl->faces);

    // trace
    tb_trace_i("");
    tb_trace_i("edges:");

    // dump edges
    tb_char_t data[8192];
    tb_for_all_if (gb_mesh_edge_ref_t, edge, gb_mesh_edge_itor(mesh), edge)
    {
        // trace
        tb_trace_i("    %s", gb_mesh_edge_list_info(impl->edges, edge, data, sizeof(data)));
    }

    // trace
    tb_trace_i("faces:");

    // dump faces
    tb_for_all_if (gb_mesh_face_ref_t, face, gb_mesh_face_itor(mesh), face)
    {
        // trace
        tb_trace_i("    face: %s", gb_mesh_face_list_info(impl->faces, face, data, sizeof(data)));

        // dump face.edges
        gb_mesh_edge_ref_t head = gb_mesh_face_edge(face);
        gb_mesh_edge_ref_t edge = head;
        do
        {
            // trace
            tb_trace_i("        %s", gb_mesh_edge_list_info(impl->edges, edge, data, sizeof(data)));

            // the next edge
            edge = gb_mesh_edge_lnext(edge);

        } while (edge != head);
    }

    // trace
    tb_trace_i("vertices:");

    // dump vertices
    tb_for_all_if (gb_mesh_vertex_ref_t, vertex, gb_mesh_vertex_itor(mesh), vertex)
    {
        // trace
        tb_trace_i("    vertex: %s", gb_mesh_vertex_list_info(impl->vertices, vertex, data, sizeof(data)));

        // dump vertex.edges
        gb_mesh_edge_ref_t head = gb_mesh_vertex_edge(vertex);
        gb_mesh_edge_ref_t edge = head;
        do
        {
            // trace
            tb_trace_i("        %s", gb_mesh_edge_list_info(impl->edges, edge, data, sizeof(data)));

            // the next edge
            edge = gb_mesh_edge_onext(edge);

        } while (edge != head);
    }
}
tb_void_t gb_mesh_check(gb_mesh_ref_t mesh)
{
    // check edges
    tb_for_all_if (gb_mesh_edge_ref_t, edge, gb_mesh_edge_itor(mesh), edge)
    {
        // check edge
        tb_assert_abort(edge);
        tb_assert_abort(gb_mesh_edge_sym(edge) != edge);
        tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_sym(edge)) == edge);
        tb_assert_abort(gb_mesh_edge_org(edge));
        tb_assert_abort(gb_mesh_edge_dst(edge));
        tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_onext(gb_mesh_edge_lnext(edge))) == edge);
        tb_assert_abort(gb_mesh_edge_lnext(gb_mesh_edge_sym(gb_mesh_edge_onext(edge))) == edge);
    }

    // check faces
    tb_for_all_if (gb_mesh_face_ref_t, face, gb_mesh_face_itor(mesh), face)
    {
        // check face.edges
        gb_mesh_edge_ref_t head = gb_mesh_face_edge(face);
        gb_mesh_edge_ref_t edge = head;
        do
        {
            // check edge
            tb_assert_abort(edge);
            tb_assert_abort(gb_mesh_edge_sym(edge) != edge);
            tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_sym(edge)) == edge);
            tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_onext(gb_mesh_edge_lnext(edge))) == edge);
            tb_assert_abort(gb_mesh_edge_lnext(gb_mesh_edge_sym(gb_mesh_edge_onext(edge))) == edge);
            tb_assert_abort(gb_mesh_edge_lface(edge) == face);

            // the next edge
            edge = gb_mesh_edge_lnext(edge);

        } while (edge != head);
    }

    // check vertices
    tb_for_all_if (gb_mesh_vertex_ref_t, vertex, gb_mesh_vertex_itor(mesh), vertex)
    {
        // check vertex.edges
        gb_mesh_edge_ref_t head = gb_mesh_vertex_edge(vertex);
        gb_mesh_edge_ref_t edge = head;
        do
        {
            // check edge
            tb_assert_abort(edge);
            tb_assert_abort(gb_mesh_edge_sym(edge) != edge);
            tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_sym(edge)) == edge);
            tb_assert_abort(gb_mesh_edge_sym(gb_mesh_edge_onext(gb_mesh_edge_lnext(edge))) == edge);
            tb_assert_abort(gb_mesh_edge_lnext(gb_mesh_edge_sym(gb_mesh_edge_onext(edge))) == edge);
            tb_assert_abort(gb_mesh_edge_org(edge) == vertex);

            // the next edge
            edge = gb_mesh_edge_onext(edge);

        } while (edge != head);
    }
}
#endif