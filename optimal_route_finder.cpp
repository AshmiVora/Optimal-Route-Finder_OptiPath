#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <iomanip> // For setw and setprecision

using namespace std;

// ANSI escape codes for terminal formatting
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string ITALIC = "\033[3m";
const string UNDERLINE = "\033[4m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";

struct Edge {
    string destination;
    double distance;
    double baseTime;
    mutable double traffic;

    double getAdjustedTime() const {
        return baseTime * (1 + traffic / 100.0);
    }
};

struct PathInfo {
    double totalDistance;
    double totalTime;
    double totalTraffic;
    vector<string> path;
    int pathNumber;
};

// Comparison functions for sorting PathInfo objects
bool comparePathsByDistance(const PathInfo& a, const PathInfo& b) {
    return a.totalDistance < b.totalDistance;
}

bool comparePathsByTime(const PathInfo& a, const PathInfo& b) {
    return a.totalTime < b.totalTime;
}

bool comparePathsByTraffic(const PathInfo& a, const PathInfo& b) {
    return a.totalTraffic < b.totalTraffic;
}

// Merge function for merge sort
void merge(vector<PathInfo>& arr, int l, int m, int r, bool (*compare)(const PathInfo&, const PathInfo&)) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector<PathInfo> L(n1);
    vector<PathInfo> R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;

    while (i < n1 && j < n2) {
        if (compare(L[i], R[j])) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Merge sort function
void mergeSort(vector<PathInfo>& arr, int l, int r, bool (*compare)(const PathInfo&, const PathInfo&)) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m, compare);
        mergeSort(arr, m + 1, r, compare);
        merge(arr, l, m, r, compare);
    }
}

class Graph {
public:
    unordered_map<string, vector<Edge>> adjList;
    void printAllLocations() const {
    cout << "\n" << string(30, '=') << "\n"; // Top border
    cout << "  Available Locations in the Network  \n";
    cout << string(30, '-') << "\n"; // Separator

    if (adjList.empty()) {
        cout << "  No locations currently in the network.\n";
    } else {
        int count = 1;
        for (const auto& pair : adjList) {
            cout << "  " << count++ << ". " << pair.first << "\n";
        }
    }

    cout << string(30, '=') << "\n"; // Bottom border
}

    void addEdge(const string& src, const string& dest, double distance, double baseTime, double trafficFactor = 1.0) {
        double traffic = getRandomTraffic();
        adjList[src].push_back({dest, distance, baseTime * trafficFactor, traffic});
        adjList[dest].push_back({src, distance, baseTime * trafficFactor, traffic});
    }

    void ensureConnectivity() {
        if (adjList.empty()) return;

        set<string> visited;
        queue<string> q;
        string startNode = adjList.begin()->first;

        q.push(startNode);
        visited.insert(startNode);

        while (!q.empty()) {
            string node = q.front();
            q.pop();

            for (const auto& edge : adjList[node]) {
                if (visited.count(edge.destination) == 0) {
                    visited.insert(edge.destination);
                    q.push(edge.destination);
                }
            }
        }

        // If some nodes are still unvisited, forcefully connect them
        for (const auto& pair : adjList) {
            if (visited.count(pair.first) == 0) {
                double randomDistance = (rand() % 10) + 5;
                double randomTime = randomDistance * 1.5;
                addEdge(startNode, pair.first, randomDistance, randomTime, 0.8);
                visited.insert(pair.first);
            }
        }
    }


    void updateTraffic() {
        for (auto& entry : adjList) {
            auto& edges = entry.second;
            for (auto& edge : edges) {
                edge.traffic = getRandomTraffic();
                edge.baseTime = edge.distance * (1.5 - edge.traffic / 100.0);
            }
        }
    }

    void findAllPathsBacktracking(string start, string destination) {
        ensureConnectivity();
        updateTraffic();
        vector<string> path;
        set<string> visited;
        vector<PathInfo> allPaths;
        PathInfo bestTimePath = {1e9, 1e9, 1e9, {}, -1};
        PathInfo bestDistancePath = {1e9, 1e9, 1e9, {}, -1};
        PathInfo bestTrafficPath = {1e9, 1e9, 1e9, {}, -1};

        backtrack(start, destination, path, visited, 0, 0, 0, allPaths, bestTimePath, bestDistancePath, bestTrafficPath);

        if (allPaths.empty()) {
            cout << YELLOW << "No valid path found from " << BOLD << start << RESET << YELLOW << " to " << BOLD << destination << RESET << "\n";
            return;
        }

        cout << "\n" << BLUE << BOLD << "All Possible Paths from " << start << " to " << destination << ":" << RESET << "\n";
        for (size_t i = 0; i < allPaths.size(); i++) {
            allPaths[i].pathNumber = i + 1;
            cout << "  " << GREEN << BOLD << i + 1 << "." << RESET << " ";
            displayPath(allPaths[i]);
        }

        // Ask user if they want to filter by a location
        char filterChoice;
        cout << "\n" << CYAN << "Do you want to filter paths by including a specific location? (y/n): " << RESET;
        cin >> filterChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Consume the newline character

        if (tolower(filterChoice) == 'y') {
            string mustInclude;
            cout << CYAN << "Enter a location that must be included in the path: " << RESET;
            getline(cin, mustInclude);

            if (!mustInclude.empty()) {
                vector<PathInfo> filteredPaths;
                // The "search part" - iterating and checking
                for (const auto& pathInfo : allPaths) {
                    bool found = false;
                    for (const auto& location : pathInfo.path) {
                        if (location == mustInclude) {
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        filteredPaths.push_back(pathInfo);
                    }
                }

                if (!filteredPaths.empty()) {
                    cout << "\n" << MAGENTA << BOLD << "Paths that include \"" << mustInclude << "\":" << RESET << "\n";
                    for (const auto& p : filteredPaths) {
                        cout << "  ";
                        displayPath(p);
                    }
                } else {
                    cout << YELLOW << "No paths include \"" << mustInclude << "\" along with start and destination." << RESET << "\n";
                }
            } else {
                cout << YELLOW << "Location to include cannot be empty." << RESET << "\n";
            }
        }

        cout << "\n" << BOLD << "Shortest path (based on time) " << GREEN << "[Path " << bestTimePath.pathNumber << "]" << RESET << ":\n  ";
        displayPath(bestTimePath);

        cout << "\n" << BOLD << "Shortest path (based on distance) " << BLUE << "[Path " << bestDistancePath.pathNumber << "]" << RESET << ":\n  ";
        displayPath(bestDistancePath);

        cout << "\n" << BOLD << "Shortest path (based on traffic) " << YELLOW << "[Path " << bestTrafficPath.pathNumber << "]" << RESET << ":\n  ";
        displayPath(bestTrafficPath);

        // Offer sorting of all paths
        char sortChoice;
        cout << "\n" << CYAN << "Do you want to sort all the paths? (y/n): " << RESET;
        cin >> sortChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (tolower(sortChoice) == 'y') {
            char sortBy;
            cout << CYAN << "Sort by (d: distance, t: time, f: traffic): " << RESET;
            cin >> sortBy;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (tolower(sortBy) == 'd') {
                mergeSort(allPaths, 0, allPaths.size() - 1, comparePathsByDistance);
                cout << "\n" << MAGENTA << BOLD << "All Paths sorted by Distance:" << RESET << "\n";
            } else if (tolower(sortBy) == 't') {
                mergeSort(allPaths, 0, allPaths.size() - 1, comparePathsByTime);
                cout << "\n" << MAGENTA << BOLD << "All Paths sorted by Time:" << RESET << "\n";
            } else if (tolower(sortBy) == 'f') {
                mergeSort(allPaths, 0, allPaths.size() - 1, comparePathsByTraffic);
                cout << "\n" << MAGENTA << BOLD << "All Paths sorted by Traffic:" << RESET << "\n";
            } else {
                cout << YELLOW << "Invalid sorting option." << RESET << "\n";
            }

            for (size_t i = 0; i < allPaths.size(); i++) {
                allPaths[i].pathNumber = i + 1;
                cout << "  " << GREEN << BOLD << i + 1 << "." << RESET << " ";
                displayPath(allPaths[i]);
            }
        }

        int choice;
        cout << "\n" << CYAN << "Select the preferred path (enter number): " << RESET;
        cin >> choice;

        if (choice > 0 && choice <= allPaths.size()) {
            cout << "\n" << GREEN << BOLD << "You selected the following path:" << RESET << "\n  ";
            displayPath(allPaths[choice - 1]);
        } else {
            cout << YELLOW << "Invalid choice." << RESET << "\n";
        }
    }

    PathInfo findShortestPathDijkstra(const string& start, const string& destination) {
        ensureConnectivity();
        updateTraffic();

        unordered_map<string, double> dist;
        unordered_map<string, string> prev;
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

        for (const auto& pair : adjList) {
            dist[pair.first] = numeric_limits<double>::infinity();
        }

        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            string u = pq.top().second;
            pq.pop();

            if (u == destination) {
                break;
            }

            for (const Edge& edge : adjList[u]) {
                double adjustedTime = edge.getAdjustedTime();
                if (dist[u] + adjustedTime < dist[edge.destination]) {
                    dist[edge.destination] = dist[u] + adjustedTime;
                    prev[edge.destination] = u;
                    pq.push({dist[edge.destination], edge.destination});
                }
            }
        }

        if (dist[destination] == numeric_limits<double>::infinity()) {
            return {0, 0, 0, {}, -1}; // No path found
        }

        vector<string> path;
        string current = destination;
        double totalDistance = 0;
        double totalTime = dist[destination];
        double totalTraffic = 0;

        while (current != start) {
            path.push_back(current);
            string previous = prev[current];
            for(auto edge: adjList[previous]){
                if(edge.destination == current){
                    totalDistance += edge.distance;
                    totalTraffic += edge.traffic;
                    break;
                }
            }
            current = previous;
        }

        path.push_back(start);
        reverse(path.begin(), path.end());

        return {totalDistance, totalTime, totalTraffic, path, 0};
    }

    void displayPath(const PathInfo& path) {
        cout << "[Path " << BOLD << path.pathNumber << RESET << "] ";
        for (size_t i = 0; i < path.path.size(); i++) {
            cout << path.path[i];
            if (i != path.path.size() - 1) cout << " " << YELLOW << "=>" << RESET << " ";
        }
        cout << " | " << BOLD << "Distance:" << RESET << " " << fixed << setprecision(2) << path.totalDistance << " km";
        cout << " | " << BOLD << "Time:" << RESET << " " << fixed << setprecision(2) << path.totalTime << " min";
        cout << " | " << BOLD << "Traffic:" << RESET << " " << fixed << setprecision(2) << path.totalTraffic << RESET << "\n";
    }

private:
    double getRandomTraffic() {
        return (rand() % 101) / 10.0;
    }

    void backtrack(const string& current, const string& destination, vector<string>& path, set<string>& visited,
                   double totalDistance, double totalTime, double totalTraffic,
                   vector<PathInfo>& allPaths, PathInfo& bestTimePath, PathInfo& bestDistancePath, PathInfo& bestTrafficPath) {

        path.push_back(current);
        visited.insert(current);

        if (current == destination) {
            allPaths.push_back({totalDistance, totalTime, totalTraffic, path, static_cast<int>(allPaths.size()) + 1});
            if (totalTime < bestTimePath.totalTime) {
                bestTimePath = {totalDistance, totalTime, totalTraffic, path, static_cast<int>(allPaths.size())};
            }
            if (totalDistance < bestDistancePath.totalDistance) {
                bestDistancePath = {totalDistance, totalTime, totalTraffic, path, static_cast<int>(allPaths.size())};
            }
            if (totalTraffic < bestTrafficPath.totalTraffic) {
                bestTrafficPath = {totalDistance, totalTime, totalTraffic, path, static_cast<int>(allPaths.size())};
            }
        } else {
            for (const Edge& neighbor : adjList[current]) {
                if (!visited.count(neighbor.destination)) {
                    double adjustedTime = neighbor.getAdjustedTime();
                    backtrack(neighbor.destination, destination, path, visited,
                              totalDistance + neighbor.distance,
                              totalTime + adjustedTime,
                              totalTraffic + neighbor.traffic,
                              allPaths, bestTimePath, bestDistancePath, bestTrafficPath);
                }
            }
        }

        path.pop_back();
        visited.erase(current);
    }
};

int main() {
    srand(time(0));
    Graph g;
    g.addEdge("CST", "Dadar", 6, 12);
    g.addEdge("Dadar", "Bandra", 4, 8);
    g.addEdge("Bandra", "Juhu Beach", 3, 6);
    g.addEdge("CST", "Gateway of India", 2, 4);
    g.addEdge("Gateway of India", "Marine Drive", 3, 6);
    g.addEdge("Marine Drive", "Haji Ali", 3, 6);
    g.addEdge("Haji Ali", "Worli", 2, 4);
    g.addEdge("Worli", "Bandra", 8, 10, 0.7);
    g.addEdge("Juhu Beach", "Sanjay Gandhi National Park", 10, 25);
    g.addEdge("Bandra", "Shree Siddhivinayak Temple", 5, 12);
    g.addEdge("Shree Siddhivinayak Temple", "Worli", 3, 8);
    g.addEdge("Marine Drive", "Colaba Causeway", 2, 6);
    g.addEdge("Colaba Causeway", "Gateway of India", 1, 3);
    g.addEdge("CST", "Crawford Market", 1, 4);
    g.addEdge("Crawford Market", "Colaba Causeway", 2, 5);
    g.addEdge("Gateway of India", "Elephanta Caves", 1, 6);
    g.addEdge("Bandra", "Film City", 8, 20);
    g.addEdge("Bandra", "Bandstand", 1, 3);
    g.addEdge("Worli", "Jehangir Art Gallery", 4, 10);
    g.addEdge("Jehangir Art Gallery", "Prithvi Theatre", 5, 12);
    g.addEdge("Prithvi Theatre", "Juhu Beach", 1, 4);
    g.ensureConnectivity();

    g.printAllLocations(); // Show all available locations


    string start, destination;
    cout << BLUE << BOLD << "Enter start location: " << RESET;
    getline(cin, start);
    cout << MAGENTA << BOLD << "Enter destination: " << RESET;
    getline(cin, destination);

    g.findAllPathsBacktracking(start, destination);

    PathInfo shortestPath = g.findShortestPathDijkstra(start, destination);
    cout << "\n" << BOLD << CYAN << "Shortest Path (Dijkstra):" << RESET << "\n  ";
    g.displayPath(shortestPath);

    return 0;
}
