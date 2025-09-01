import { viewParams } from './view-params.js'

function initBuffers(gl, scene) {

  const ledBuffers = []
  scene.forEach(scenePart => {

    let sphere = { vertices: [], normals: [], indices: [] }

    scenePart.positions.forEach(pos => {
      draw_sphere(sphere, pos.x, pos.y, pos.z, scenePart.size ?? 0.01);
    })


    const positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphere.vertices), gl.STATIC_DRAW);

    const indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bufferData(
      gl.ELEMENT_ARRAY_BUFFER,
      new Uint32Array(sphere.indices),
      gl.STATIC_DRAW
    );

    const colorBuffer = gl.createBuffer();
    const colors = Array(sphere.vertices.length).fill([0, 0, 255]).flat();
    gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Uint8Array(colors), gl.DYNAMIC_DRAW);

    ledBuffers.push({
      position: positionBuffer,
      color: colorBuffer,
      indices: indexBuffer,
      indicesCount: sphere.indices.length,
      verticesCount: sphere.vertices.length,
    })
  })


  //////grid

  let grid = { vertices: [], normals: [], indices: [] }
  draw_grid(grid, viewParams.gridZ)

  const gridPositionBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, gridPositionBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(grid.vertices), gl.STATIC_DRAW);

  const gridIndexBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gridIndexBuffer);
  gl.bufferData(
    gl.ELEMENT_ARRAY_BUFFER,
    new Uint16Array(grid.indices),
    gl.STATIC_DRAW
  );

  const gridColorBuffer = gl.createBuffer();
  const gridColors = Array(grid.vertices.length).fill([50, 50, 50]).flat();
  gl.bindBuffer(gl.ARRAY_BUFFER, gridColorBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, new Uint8Array(gridColors), gl.STATIC_DRAW);



  return {
    ledBuffers,

    gridPosition: gridPositionBuffer,
    gridColor: gridColorBuffer,
    gridIndices: gridIndexBuffer,
    gridIndicesCount: grid.indices.length,
  };
}

function draw_sphere(target, cx, cy, cz, r=0.01) {
  // http://www.songho.ca/opengl/gl_sphere.html
  const stacks = 2;
  const sectors = 4;
  const startVertex = target.vertices.length / 3;

  for (let i = 0; i <= stacks; i++) {
    const phi = Math.PI / 2 - Math.PI * i / stacks;

    for (let j = 0; j <= sectors; j++) {
      const theta = 2 * Math.PI * j / sectors;

      const x = Math.cos(phi) * Math.cos(theta);
      const y = Math.cos(phi) * Math.sin(theta);
      const z = Math.sin(phi);

      target.vertices.push(r * x + cx, r * y + cy, r * z + cz);
      target.normals.push(x, y, z);
    }
  }

  // Create a list of triangle indices.
  for (let i = 0; i < stacks; i++) {
    let k1 = i * (sectors + 1);
    let k2 = k1 + sectors + 1;
    for (let j = 0; j < sectors; j++) {
      if (i !== 0) {
        target.indices.push(startVertex + k1, startVertex + k2, startVertex + k1 + 1);
      }
      if (i !== stacks - 1) {
        target.indices.push(startVertex + k1 + 1, startVertex + k2, startVertex + k2 + 1);
      }
      k1++;
      k2++;
    }
  }
}

function draw_grid(target, gridz = 0) {
  const slices = 10
  const normal = [0, 1, 0]

  for (let j = 0; j < slices + 1; j++) {
    for (let i = 0; i < slices + 1; i++) {
      const x = -1 + 2 * i / slices
      const y = gridz
      const z = -1 + 2 * j / slices
      target.vertices.push(x, y, z);
      target.normals.push(...normal);
    }
  }

  for (let j = 0; j < slices; j++) {
    for (let i = 0; i < slices; i++) {
      const row1 = j * (slices + 1)
      const row2 = (j + 1) * (slices + 1)

      target.indices.push(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1)
      target.indices.push(row2 + i + 1, row2 + i, row2 + i, row1 + i)
    }
  }
}

export { initBuffers };