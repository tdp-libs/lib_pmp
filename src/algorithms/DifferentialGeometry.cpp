// Copyright 2011-2020 the Polygon Mesh Processing Library developers.
// Distributed under a MIT-style license, see LICENSE.txt for details.

#include "pmp/algorithms/DifferentialGeometry.h"

#include <cmath>

#include <limits>

#ifndef M_PI
#define M_PI 3.141592653589793238
#endif

namespace pmp {

Scalar triangle_area(const Point& p0, const Point& p1, const Point& p2)
{
    return Scalar(0.5) * norm(cross(p1 - p0, p2 - p0));
}

Scalar triangle_area(const SurfaceMesh& mesh, Face f)
{
    assert(mesh.valence(f) == 3);

    auto fv = mesh.vertices(f);
    const auto& p0 = mesh.position(*fv);
    const auto& p1 = mesh.position(*(++fv));
    const auto& p2 = mesh.position(*(++fv));

    return triangle_area(p0, p1, p2);
}

Scalar surface_area(const SurfaceMesh& mesh)
{
    Scalar area(0);
    for (auto f : mesh.faces())
    {
        area += triangle_area(mesh, f);
    }
    return area;
}

Point centroid(const SurfaceMesh& mesh, Face f)
{
    Point c(0, 0, 0);
    Scalar n(0);
    for (auto v : mesh.vertices(f))
    {
        c += mesh.position(v);
        ++n;
    }
    c /= n;
    return c;
}

Point centroid(const SurfaceMesh& mesh)
{
    Point center(0, 0, 0), c;
    Scalar area(0), a;
    for (auto f : mesh.faces())
    {
        a = triangle_area(mesh, f);
        c = centroid(mesh, f);
        area += a;
        center += a * c;
    }
    center /= area;
    return center;
}

double cotan_weight(const SurfaceMesh& mesh, Edge e)
{
    double weight = 0.0;

    const Halfedge h0 = mesh.halfedge(e, 0);
    const Halfedge h1 = mesh.halfedge(e, 1);

    const dvec3 p0 = dvec3(mesh.position(mesh.to_vertex(h0)));
    const dvec3 p1 = dvec3(mesh.position(mesh.to_vertex(h1)));

    if (!mesh.is_boundary(h0))
    {
        const dvec3 p2 = dvec3(mesh.position(mesh.to_vertex(mesh.next_halfedge(h0))));
        const dvec3 d0 = p0 - p2;
        const dvec3 d1 = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            weight += clamp_cot(cot);
        }
    }

    if (!mesh.is_boundary(h1))
    {
        const dvec3 p2 = dvec3(mesh.position(mesh.to_vertex(mesh.next_halfedge(h1))));
        const dvec3 d0 = p0 - p2;
        const dvec3 d1 = p1 - p2;
        const double area = norm(cross(d0, d1));
        if (area > std::numeric_limits<double>::min())
        {
            const double cot = dot(d0, d1) / area;
            weight += clamp_cot(cot);
        }
    }

    assert(!std::isnan(weight));
    assert(!std::isinf(weight));

    return weight;
}

double voronoi_area(const SurfaceMesh& mesh, Vertex v)
{
    double area(0.0);

    if (!mesh.is_isolated(v))
    {
        Halfedge h0, h1, h2;
        dvec3 p, q, r, pq, qr, pr;
        double dotp, dotq, dotr, triArea;
        double cotq, cotr;

        for (auto h : mesh.halfedges(v))
        {
            h0 = h;
            h1 = mesh.next_halfedge(h0);
            h2 = mesh.next_halfedge(h1);

            if (mesh.is_boundary(h0))
                continue;

            // three vertex positions
            p = dvec3(mesh.position(mesh.to_vertex(h2)));
            q = dvec3(mesh.position(mesh.to_vertex(h0)));
            r = dvec3(mesh.position(mesh.to_vertex(h1)));

            // edge vectors
            (pq = q) -= p;
            (qr = r) -= q;
            (pr = r) -= p;

            // compute and check triangle area
            triArea = norm(cross(pq, pr));
            if (triArea <= std::numeric_limits<double>::min())
                continue;

            // dot products for each corner (of its two emanating edge vectors)
            dotp = dot(pq, pr);
            dotq = -dot(qr, pq);
            dotr = dot(qr, pr);

            // angle at p is obtuse
            if (dotp < 0.0)
            {
                area += 0.25 * triArea;
            }

            // angle at q or r obtuse
            else if (dotq < 0.0 || dotr < 0.0)
            {
                area += 0.125 * triArea;
            }

            // no obtuse angles
            else
            {
                // cot(angle) = cos(angle)/sin(angle) = dot(A,B)/norm(cross(A,B))
                cotq = dotq / triArea;
                cotr = dotr / triArea;

                // clamp cot(angle) by clamping angle to [1,179]
                area += 0.125 * (sqrnorm(pr) * clamp_cot(cotq) +
                                 sqrnorm(pq) * clamp_cot(cotr));
            }
        }
    }

    assert(!std::isnan(area));
    assert(!std::isinf(area));

    return area;
}

double voronoi_area_barycentric(const SurfaceMesh& mesh, Vertex v)
{
    double area(0.0);

    if (!mesh.is_isolated(v))
    {
        const Point p = mesh.position(v);
        Halfedge h0, h1;
        Point q, r, pq, pr;

        for (auto h : mesh.halfedges(v))
        {
            if (mesh.is_boundary(h))
                continue;

            h0 = h;
            h1 = mesh.next_halfedge(h0);

            pq = mesh.position(mesh.to_vertex(h0));
            pq -= p;

            pr = mesh.position(mesh.to_vertex(h1));
            pr -= p;

            area += double(norm(cross(pq, pr)) / Scalar(3.0));
        }
    }

    return area;
}

Point laplace(const SurfaceMesh& mesh, Vertex v)
{
    Point laplace(0.0, 0.0, 0.0);

    if (!mesh.is_isolated(v))
    {
        Scalar weight, sumWeights(0.0);

        for (auto h : mesh.halfedges(v))
        {
            weight = Scalar(cotan_weight(mesh, mesh.edge(h)));
            sumWeights += weight;
            laplace += weight * mesh.position(mesh.to_vertex(h));
        }

        laplace -= sumWeights * mesh.position(v);
        laplace /= Scalar(2.0) * Scalar(voronoi_area(mesh, v));
    }

    return laplace;
}

Scalar angle_sum(const SurfaceMesh& mesh, Vertex v)
{
    Scalar angles(0.0);

    if (!mesh.is_boundary(v))
    {
        const Point& p0 = mesh.position(v);

        for (auto h : mesh.halfedges(v))
        {
            const Point& p1 = mesh.position(mesh.to_vertex(h));
            const Point& p2 =
                mesh.position(mesh.to_vertex(mesh.ccw_rotated_halfedge(h)));

            const Point p01 = normalize(p1 - p0);
            const Point p02 = normalize(p2 - p0);

            Scalar cos_angle = Scalar(clamp_cos(double(dot(p01, p02))));

            angles += std::acos(cos_angle);
        }
    }

    return angles;
}

VertexCurvature vertex_curvature(const SurfaceMesh& mesh, Vertex v)
{
    VertexCurvature c;

    const Scalar area = Scalar(voronoi_area(mesh, v));
    if (area > std::numeric_limits<Scalar>::min())
    {
        c.mean = Scalar(0.5) * norm(laplace(mesh, v));
        c.gauss = (Scalar(2.0) * Scalar(M_PI) - angle_sum(mesh, v)) / area;

        const Scalar s = std::sqrt(std::max(Scalar(0.0), c.mean * c.mean - c.gauss));
        c.min = c.mean - s;
        c.max = c.mean + s;

        assert(!std::isnan(c.mean));
        assert(!std::isnan(c.gauss));
        assert(!std::isinf(c.mean));
        assert(!std::isinf(c.gauss));

        assert(c.min <= c.mean);
        assert(c.mean <= c.max);
    }

    return c;
}

} // namespace pmp
