# Optimal-Route-Finder_OptiPath

**OptiPath** is a path optimization system with two implementations:

1. **C++ Console Version** – Demonstrates route finding using classical graph algorithms.
2. **Interactive Web Version (HTML + Leaflet.js)** – A modern visual interface built to reflect the same logic in a map-based UI.

---

## Project Overview

### `optimal_route_finder.cpp`  
A terminal-based application that:
- Represents a city map as a weighted, bidirectional graph.
- Computes:
  - **Shortest path** using Dijkstra’s Algorithm (adjusted for simulated traffic).
  - **All possible paths** using backtracking with filtering options.
- Supports:
  - Sorting by distance, time, or traffic.
  - Colorized terminal output using ANSI escape codes.

### `OptiPath.html`  
A web-based interactive version that:
- Implements the **same logic** as the C++ version (Dijkstra + Backtracking) using JavaScript.
- Uses **Leaflet.js** to plot locations on a real-world map.
- Allows:
  - Adding locations by click or search (via OpenStreetMap).
  - Undoing added markers.
  - Visualizing shortest and alternate routes with traffic weight.
  - Estimating time dynamically based on route and traffic conditions.
 
---

## Project Structure
```yaml
optipath/
├── optimal_route_finder.cpp       # C++ backend algorithm
├── OptiPath.html                  # Frontend Leaflet.js map UI
├── LICENSE                        # CC BY-NC 4.0 License
└── README.md                      # You're reading this
```

---

##  Getting Started

### C++ Version
```bash
g++ optimal_route_finder.cpp -o route_finder
./route_finder
```

### Web Version
Simply open OptiPath.html in any modern browser. No server or installation required.

---
## License
This project is licensed under the
Creative Commons Attribution-NonCommercial 4.0 International License.
Commercial use is strictly prohibited.
You are free to:
- Share and adapt the code with credit
- Use it non-commercially

[Full license details](https://creativecommons.org/licenses/by-nc/4.0/)

---
## Acknowledgements

- This project was developed as part of an **academic demonstration of pathfinding algorithms**, combining both theoretical and practical implementations.
- [Leaflet.js](https://leafletjs.com/) – for the interactive map interface.
- [OpenStreetMap Nominatim API](https://nominatim.org/release-docs/latest/api/Search/) – for geolocation, search, and reverse geocoding.
- [ANSI Escape Codes](https://en.wikipedia.org/wiki/ANSI_escape_code) – for colored and styled terminal output in the C++ version.


