import './gl-matrix-min.js';
import { WebGLDebugUtils } from './webgl-debug.js';
import { Socket } from '../common/socket.js'

let time = 0.0;
let deltaTime = 0;
let runtimeSessionId;

const DEBUG = false;

export function main(scene, calcViewMatrix, grid = undefined) {
  const canvas = document.querySelector("#glcanvas");

  const gl = DEBUG ?
    WebGLDebugUtils.makeDebugContext(canvas.getContext("webgl2")) :
    canvas.getContext("webgl2");

  window.onresize = () => {
    canvas.width  = window.innerWidth;
    canvas.height = window.innerHeight;

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
  }
  window.onresize()

  if (!gl) {
    alert(
      "Unable to initialize WebGL. Your browser or machine may not support it."
    );
    return;
  }

  gl.enable(gl.BLEND);
  gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
  gl.clearDepth(1.0); // Clear everything
  gl.depthFunc(gl.LEQUAL); // Near things obscure far things
  gl.clearColor(0.1, 0.1, 0.1, 1.0); 
  gl.enable(gl.DEPTH_TEST);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  const vertexShaderSource = `
  attribute vec4 vertexPosition;
  attribute vec3 sphereOffset;    
  attribute vec4 sphereColor;     
  attribute float sphereSize;

  uniform mat4 uModelViewMatrix;
  uniform mat4 uProjectionMatrix;

  varying vec4 v_color;

  void main() {    
    gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(sphereOffset + vertexPosition.xyz * sphereSize, 1.0);
    v_color = sphereColor;
  }`

  const fragmentShaderSource = `
  varying lowp vec4 v_color;

  void main() {
    gl_FragColor = v_color;
  }
  `;

  // Initialize a shader program; this is where all the lighting
  // for the vertices and so forth is established.
  const shaderProgram = initShaderProgram(gl, vertexShaderSource, fragmentShaderSource);
  gl.useProgram(shaderProgram);

  // Collect all the info needed to use the shader program.
  // Look up which attributes our shader program is using
  // for aVertexPosition, aVertexColor and also
  // look up uniform locations.
  const programInfo = {
    program: shaderProgram,
    attribLocations: {
      position: gl.getAttribLocation(shaderProgram, "vertexPosition"),
      offset: gl.getAttribLocation(shaderProgram, "sphereOffset"),
      color: gl.getAttribLocation(shaderProgram, "sphereColor"),
      size: gl.getAttribLocation(shaderProgram, "sphereSize"),
    },
    uniformLocations: {
      projectionMatrix: gl.getUniformLocation(shaderProgram,"uProjectionMatrix"),
      modelViewMatrix: gl.getUniformLocation(shaderProgram, "uModelViewMatrix"),
    },
  };

  /* sphere instance vertices and indices */
  const sphereData = draw_sphere(0, 0, 0, 1);
  const totalLights = scene.map(s=>s.positions.length).reduce((a, b) => a + b,0);
  const sceneBuffers = {
    position: createBuffer(gl, new Float32Array(sphereData.vertices)),
    index: createBuffer(gl, new Uint16Array(sphereData.indices), gl.ELEMENT_ARRAY_BUFFER),
    offset: createBuffer(gl, new Float32Array(totalLights*3)),
    color: createBuffer(gl, new Uint8Array(Array(totalLights*3).fill(0)), gl.ARRAY_BUFFER, gl.DYNAMIC_DRAW),
    size: createBuffer(gl, new Float32Array(totalLights)),
    indicesCount: sphereData.indices.length,
    instancesCount: totalLights,
  }

  let startLight = 0
  scene.forEach(scenePart => {
    gl.bindBuffer(gl.ARRAY_BUFFER, sceneBuffers.offset);
    gl.bufferSubData(
      gl.ARRAY_BUFFER, 
      startLight * 3 * 4,
      new Float32Array(scenePart.positions.map(p=>[p.x, p.y, p.z ?? 0]).flat()), 
    );

    gl.bindBuffer(gl.ARRAY_BUFFER, sceneBuffers.size);
    const sizes = Array(scenePart.positions.length).fill(scenePart.size || 0.01);
    gl.bufferSubData(
      gl.ARRAY_BUFFER, 
      startLight * 4,
      new Float32Array(sizes), 
    );

    const bufferIndex = startLight * 3;

    new Socket(scenePart.path, data => {
      if (!(data instanceof ArrayBuffer)){
          const json = JSON.parse(data)
          if (json.type=='runtimeSessionId')
          {
              const newBuildId = json.value
              if (!runtimeSessionId) runtimeSessionId = newBuildId;
              else if (runtimeSessionId != newBuildId) {
                  delete window.scene
                  window.onBuildIdChange?.();
                  window.location.reload();
              }
          }
          return;
      }

      gl.bindBuffer(gl.ARRAY_BUFFER, sceneBuffers.color);
      gl.bufferSubData(gl.ARRAY_BUFFER, bufferIndex,new Uint8Array(data));
 
    })

    startLight += scenePart.positions.length;
  });

  const buffers = {
    sceneBuffers,
  };

  /* grid */ 
  if (grid)
  {
    const gridData = draw_grid(grid.z);
    const gridBuffers = {
      position: createBuffer(gl, new Float32Array(gridData.vertices)),
      index: createBuffer(gl, new Uint16Array(gridData.indices), gl.ELEMENT_ARRAY_BUFFER),
      offset: createBuffer(gl, new Float32Array([0,0,0])),
      color: createBuffer(gl, new Uint8Array([50, 50, 50])),
      size: createBuffer(gl, new Float32Array([1])),
      indicesCount: gridData.indices.length,
      instancesCount: 1,
    }
    buffers.gridBuffers = gridBuffers;
  }

  let then = 0;
  const fpsElem = document.querySelector("#fps");

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001; // convert to seconds
    deltaTime = now - then;
    then = now;

    updateScene(gl, time, programInfo, buffers, calcViewMatrix);
    time += deltaTime;
    const fps = 1 / deltaTime; 
    fpsElem.textContent = fps.toFixed(1);

    requestAnimationFrame(render);
  }
  requestAnimationFrame(render);
}


function initShaderProgram(gl, vsSource, fsSource) {
  const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
  const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

  const shaderProgram = gl.createProgram();
  gl.attachShader(shaderProgram, vertexShader);
  gl.attachShader(shaderProgram, fragmentShader);
  gl.linkProgram(shaderProgram);

  if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
    alert(
      `Unable to initialize the shader program: ${gl.getProgramInfoLog(
        shaderProgram
      )}`
    );
    return null;
  }

  return shaderProgram;
}

function loadShader(gl, type, source) {
  const shader = gl.createShader(type);
  gl.shaderSource(shader, source);
  gl.compileShader(shader);
  if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
    alert(
      `An error occurred compiling the shaders: ${gl.getShaderInfoLog(shader)}`
    );
    gl.deleteShader(shader);
    return null;
  }
  return shader;
}

function updateScene(gl, time, programInfo, buffers, calcViewMatrix)
{
  // calcViewMatrix3d(gl, time, viewParams, programInfo);
  calcViewMatrix(gl, time, programInfo);

  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  //draw spheres
  bindBuffers(gl, programInfo, buffers.sceneBuffers);
  gl.drawElementsInstanced(gl.TRIANGLES, buffers.sceneBuffers.indicesCount, gl.UNSIGNED_SHORT, 0, buffers.sceneBuffers.instancesCount);
  
  //draw grid
  if (!buffers.gridBuffers) 
    return;
  bindBuffers(gl, programInfo, buffers.gridBuffers);
  gl.drawElementsInstanced(gl.LINES, buffers.gridBuffers.indicesCount, gl.UNSIGNED_SHORT, 0, buffers.gridBuffers.instancesCount);
}

function createBuffer(gl, data, type=gl.ARRAY_BUFFER, usage=gl.STATIC_DRAW) {
  const buffer = gl.createBuffer();
  gl.bindBuffer(type, buffer);
  gl.bufferData(type, data, usage);
  return buffer;
}

function bindBuffers(gl, programInfo, buffers) {
  setAttribute(gl, buffers.position, programInfo.attribLocations.position, {size:3});
  setAttribute(gl, buffers.offset, programInfo.attribLocations.offset, {size:3, divisor:1});
  setAttribute(gl, buffers.color, programInfo.attribLocations.color, {size:3, divisor:1, type:gl.UNSIGNED_BYTE, normalize:true});
  setAttribute(gl, buffers.size, programInfo.attribLocations.size, {size:1, divisor:1, type:gl.FLOAT});
  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.index); 
}

// Tell WebGL how to pull out the positions from the position
// buffer into the vertexPosition attribute.
function setAttribute(gl, buffer, location, {size=3, divisor=0, type=gl.FLOAT, normalize=false}) {
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.enableVertexAttribArray(location);
    {
      const stride = 0;
      const offset = 0;
      gl.vertexAttribPointer(location, size, type, normalize, stride, offset);
      gl.vertexAttribDivisor(location, divisor);
    }
}

export function calcViewMatrix3d(viewParams) {
  return (gl, time, programInfo) => calcViewMatrix3d_(gl, time, programInfo, viewParams);
}

function calcViewMatrix3d_(gl, time, programInfo, viewParams) {
  {
    const fieldOfView = viewParams.fieldOfView; //(80 * Math.PI) / 180; // in radians
    const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
    const zNear = 0.1;
    const zFar = 255.0;
    const projectionMatrix = mat4.create();
    mat4.perspective(projectionMatrix, fieldOfView, aspect, zNear, zFar);  
    gl.uniformMatrix4fv(
      programInfo.uniformLocations.projectionMatrix,
      false,
      projectionMatrix
    );
  }

  const modelViewMatrix = mat4.create();
  viewParams.transform.forEach(transformation => {
    if (transformation.type==='translate'){
      mat4.translate(
        modelViewMatrix, // destination matrix
        modelViewMatrix, // matrix to translate
        transformation.amount
      );
    } else if (transformation.type==='rotate') {
      mat4.rotate(
        modelViewMatrix, // destination matrix
        modelViewMatrix, // matrix to rotate
        transformation.amount(time),
        transformation.vector
      ); 
    }
  })
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.modelViewMatrix,
    false,
    modelViewMatrix
  );
}

export function calcViewMatrix2d(gl, time, programInfo) {

  const projectionMatrix = mat4.create();
  const aspect = gl.canvas.clientWidth / gl.canvas.clientHeight;
  mat4.ortho(projectionMatrix, -aspect, aspect, 1, -1, 0, 255);
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.projectionMatrix,
    false,
    projectionMatrix
  );

  const modelViewMatrix = mat4.identity(mat4.create());
  gl.uniformMatrix4fv(
    programInfo.uniformLocations.modelViewMatrix,
    false,
    modelViewMatrix
  );
}

function draw_sphere(cx, cy, cz, r=0.01) {
  // http://www.songho.ca/opengl/gl_sphere.html
  const stacks = 4;
  const sectors = 8;
  const startVertex = 0;

  const target = {vertices: [], normals: [], indices: []}

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
  return target;
}

function draw_grid(gridz = 0) {
  const grid = { vertices: [], indices: [] }

  const slices = 10

  for (let j = 0; j < slices + 1; j++) {
    for (let i = 0; i < slices + 1; i++) {
      const x = -1 + 2 * i / slices
      const y = gridz
      const z = -1 + 2 * j / slices
      grid.vertices.push(x, y, z);
    }
  }

  for (let j = 0; j < slices; j++) {
    for (let i = 0; i < slices; i++) {
      const row1 = j * (slices + 1)
      const row2 = (j + 1) * (slices + 1)

      grid.indices.push(row1 + i, row1 + i + 1, row1 + i + 1, row2 + i + 1)
      grid.indices.push(row2 + i + 1, row2 + i, row2 + i, row1 + i)
    }
  }

  return grid;
}