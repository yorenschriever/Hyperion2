export const defaultOrbit = {
    azimuth: 0,
    elevation: 0.25,
    distance: 2,
    panY: 0,
}

export const viewParams = {
    fieldOfView: (80 * Math.PI) / 180,
    gridZ: 0,
    // mutated interactively by drag/pinch controls in webgl-monitor.js
    orbit: { ...defaultOrbit },
}