TARGET = lib_pmp
TEMPLATE = lib

DEFINES += LIB_PMP_LIBRARY

HEADERS += inc/pmp/BoundingBox.h
HEADERS += inc/pmp/MatVec.h
HEADERS += inc/pmp/MemoryUsage.h
HEADERS += inc/pmp/Properties.h
HEADERS += inc/pmp/SurfaceMesh.h
#HEADERS += inc/pmp/SurfaceMeshIO.h
HEADERS += inc/pmp/Timer.h
HEADERS += inc/pmp/Types.h
HEADERS += inc/pmp/Version.h

HEADERS += inc/pmp/algorithms/BarycentricCoordinates.h
HEADERS += inc/pmp/algorithms/DifferentialGeometry.h
HEADERS += inc/pmp/algorithms/DistancePointTriangle.h
HEADERS += inc/pmp/algorithms/Heap.h
HEADERS += inc/pmp/algorithms/NormalCone.h
HEADERS += inc/pmp/algorithms/Quadric.h
HEADERS += inc/pmp/algorithms/SurfaceCurvature.h
HEADERS += inc/pmp/algorithms/SurfaceFairing.h
HEADERS += inc/pmp/algorithms/SurfaceFeatures.h
HEADERS += inc/pmp/algorithms/SurfaceGeodesic.h
HEADERS += inc/pmp/algorithms/SurfaceHoleFilling.h
HEADERS += inc/pmp/algorithms/SurfaceNormals.h
HEADERS += inc/pmp/algorithms/SurfaceParameterization.h
HEADERS += inc/pmp/algorithms/SurfaceRemeshing.h
HEADERS += inc/pmp/algorithms/SurfaceSimplification.h
HEADERS += inc/pmp/algorithms/SurfaceSmoothing.h
HEADERS += inc/pmp/algorithms/SurfaceSubdivision.h
HEADERS += inc/pmp/algorithms/SurfaceTriangulation.h
HEADERS += inc/pmp/algorithms/TriangleKdTree.h

SOURCES += src/SurfaceMesh.cpp
#SOURCES += src/SurfaceMeshIO.cpp
SOURCES += src/Version.cpp

SOURCES += src/algorithms/DifferentialGeometry.cpp
SOURCES += src/algorithms/DistancePointTriangle.cpp
SOURCES += src/algorithms/SurfaceCurvature.cpp
SOURCES += src/algorithms/SurfaceFairing.cpp
SOURCES += src/algorithms/SurfaceFeatures.cpp
SOURCES += src/algorithms/SurfaceGeodesic.cpp
SOURCES += src/algorithms/SurfaceHoleFilling.cpp
SOURCES += src/algorithms/SurfaceNormals.cpp
SOURCES += src/algorithms/SurfaceParameterization.cpp
SOURCES += src/algorithms/SurfaceRemeshing.cpp
SOURCES += src/algorithms/SurfaceSimplification.cpp
SOURCES += src/algorithms/SurfaceSmoothing.cpp
SOURCES += src/algorithms/SurfaceSubdivision.cpp
SOURCES += src/algorithms/SurfaceTriangulation.cpp
SOURCES += src/algorithms/TriangleKdTree.cpp
