#include <DisjointSets.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>


// Map arbitrary labels to 0, ..., (n-1) labels
template <typename T, typename Dist>
std::vector<size_t> EnumerateLabels(const std::vector<T> clusterCenters,
                                    const std::vector<T> objects,
                                    Dist distance) {
    std::vector<size_t> labels(objects.size(), 0);

    for (size_t i = 0; i != objects.size(); ++i) {
        int search_nearest_center = -1;
        for (size_t j = 0; j != clusterCenters.size(); ++j) {
            int this_dist = distance(clusterCenters[j], objects[i]);
            if (this_dist < search_nearest_center || search_nearest_center == -1) {
                search_nearest_center = this_dist;
                labels[i] = j;
            }
        }
    }

    return labels;
}


template <typename T, typename Dist>
std::vector<double> PairwiseDistances(std::vector<T> clusterCenters,
                                      std::vector<T> objects,
                                      Dist distance) {
    std::vector<double> pdist;
    for (size_t i = 0; i < clusterCenters.size(); ++i) {
        for (size_t j = 0; j < objects.size(); ++j) {
            pdist.push_back(distance(clusterCenters[i], objects[j]));
        }
    }
    return pdist;
}

template <typename T>
Point2D FindNewClusterCenter(std::vector<T> objects,
                             std::vector<double> pdist) {
    double argmax = 0;
    Point2D newCenter = {};
    for (size_t j = 0; j != objects.size(); ++j) {
        if (argmax < pdist[j]) {
            newCenter = objects[j];
            argmax = pdist[j];
        }
    }

    return newCenter;
}

template <typename T, typename Dist>
Point2D rechooseFirstCenter(std::vector<T> clusterCenters,
                                   std::vector<T> objects,
                                   Dist distance) {
       std::vector<double> pdist;
    for (size_t i = 1; i < clusterCenters.size(); ++i) {
        for (size_t j = 0; j < objects.size(); ++j) {
            pdist.push_back(distance(clusterCenters[i], objects[j]));
        }
    }

    return FindNewClusterCenter(objects, pdist);
}


template <typename T, typename Dist>
std::vector<size_t> ClusterMinDistToCenter(const std::vector<T>& objects,
                                           Dist distance,
                                           size_t clusterCount)
{
    std::vector<Point2D> clusterCenters(1);
    int x = rand() % objects.size();

    std::cout << x << ' ' << objects[x].x << ' ' << objects[x].y << std::endl;

    clusterCenters[0] = objects[x];
    for (size_t i = 1; i != clusterCount; ++i) {
        std::vector<double> pdist(PairwiseDistances(clusterCenters, objects, distance));
        Point2D new_center = FindNewClusterCenter(clusterCenters, pdist);
        clusterCenters.push_back(new_center);
    }
    clusterCenters[0] = rechooseFirstCenter(clusterCenters, objects, distance);

    return EnumerateLabels(clusterCenters, objects, distance);
}



double distance(const Point2D& first, const Point2D& second) {
    return std::sqrt((first.x - second.x) * (first.x - second.x) +
                     (first.y - second.y) * (first.y - second.y));
}


std::vector<Point2D> Random2DClusters(const std::vector<Point2D>& centers,
                                      const std::vector<double>& xVariances,
                                      const std::vector<double>& yVariances,
                                      size_t pointsCount)
{
    auto baseGenerator = std::default_random_engine();
    auto generateCluster = std::uniform_int_distribution<size_t>(0, centers.size() - 1);
    auto generateDeviation = std::normal_distribution<double>();

    std::vector<Point2D> results;
    for (size_t i = 0; i < pointsCount; ++i) {
        size_t c = generateCluster(baseGenerator);
        double x = centers[c].x + generateDeviation(baseGenerator) * xVariances[c];
        double y = centers[c].y + generateDeviation(baseGenerator) * yVariances[c];
        results.push_back({x, y});
    }

    return results;
}


// Generate files for plotting in gnuplot
void GNUPlotClusters2D(const std::vector<Point2D>& points,
                       const std::vector<size_t>& labels,
                       size_t clustersCount,
                       const std::string& outFolder)
{
    std::ofstream scriptOut(outFolder + "/script.txt");
    scriptOut << "plot ";

    for (size_t cluster = 0; cluster < clustersCount; ++cluster) {
        std::string filename = std::to_string(cluster) + ".dat";
        std::ofstream fileOut(outFolder + "/" + filename);
        scriptOut << "\"" << filename << "\"" << " with points, ";

        for (size_t i = 0; i < points.size(); ++i) {
            if (labels[i] == cluster) {
                fileOut << points[i].x << "\t" << points[i].y << "\n";
            }
        }
    }
}


int main() {
    std::srand(time(NULL));

    auto points = Random2DClusters(
            {{0, 2}, {2, 0}, {3, 3}},
            {0.3, 0.1, 0.3},
            {0.2, 0.1, 0.1},
            1000);

    std::vector<size_t> labels(points.size(), 0);
    GNUPlotClusters2D(points, labels, 1, "./plot_base");

    size_t clustersCount = 3;

    labels = ClusterMinDistToCenter(points, distance, clustersCount);
    GNUPlotClusters2D(points, labels, clustersCount, "./plot_mdc");

    return 0;
}