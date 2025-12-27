import { viewParams } from './view-params.js'
import { Socket } from '../common/socket.js'
import './gl-matrix-min.js';
import {WebGLDebugUtils} from './webgl-debug.js';

let time = 0.0;
let deltaTime = 0;
let runtimeSessionId;

const DEBUG = false;

const getScene = () => {
  if (window.scene)
      return Promise.resolve(window.scene)
  return fetch("./mapping.json").then(i=>i.json())
}

getScene().then(main);

function main(scene) {
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

//   gl.enable(gl.BLEND);
//   gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
//   gl.clearDepth(1.0); // Clear everything
//   gl.depthFunc(gl.LEQUAL); // Near things obscure far things
//   gl.clearColor(...viewParams.clearColor, 1.0); //TODO from view-params
//   gl.enable(gl.DEPTH_TEST);
//   gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

gl.clearColor(...viewParams.clearColor); // Clear to black, fully opaque
gl.clearDepth(1.0); // Clear everything
gl.enable(gl.DEPTH_TEST); // Enable depth testing
gl.depthFunc(gl.LEQUAL); // Near things obscure fa

  const vertexShaderSource = `
  attribute vec4 vertexPosition;
  attribute vec3 sphereOffset;    
  attribute vec4 sphereColor;     

  uniform mat4 uModelViewMatrix;
  uniform mat4 uProjectionMatrix;

  varying vec4 v_color;
  varying vec4 v_texcoord;

  void main() {    
    mat4 translation = mat4(
      vec4(0.01, 0, 0, 0),
      vec4(0, 0.01, 0, 0),
      vec4(0, 0, 0.01, 0),
      vec4(sphereOffset, 1));

    //https://www.geeks3d.com/20140807/billboarding-vertex-shader-glsl/
    //Set the rotation part of the matrix to identity, so the quad always faces the camera
    mat4 billboard = uModelViewMatrix * translation;
    // billboard[0][0] = .01; 
    // billboard[0][1] = 0.0; 
    // billboard[0][2] = 0.0; 
    // billboard[1][0] = 0.0; 
    // billboard[1][1] = .01; 
    // billboard[1][2] = 0.0; 
    // billboard[2][0] = 0.0; 
    // billboard[2][1] = 0.0; 
    // billboard[2][2] = .01; 

    gl_Position = uProjectionMatrix * billboard * vertexPosition;

    v_color = sphereColor;
    v_texcoord = vertexPosition;
  }`

  const fragmentShaderSource = `
  precision mediump float;
  varying vec4 v_color;
  varying vec4 v_texcoord;

  void main() {
    // float mask = 1.0 - step(1.0, length(v_texcoord.xy * 2. - 1.));
    // if (mask <= 0.0001) discard;
    // gl_FragColor = vec4(v_color.rgb * mask, v_color.a * mask);
    gl_FragColor = v_color;
  }
  `;

  // Initialize a shader program; this is where all the lighting
  // for the vertices and so forth is established.
  const shaderProgram = initShaderProgram(gl, vertexShaderSource, fragmentShaderSource);

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
    },
    uniformLocations: {
      projectionMatrix: gl.getUniformLocation(shaderProgram,"uProjectionMatrix"),
      modelViewMatrix: gl.getUniformLocation(shaderProgram, "uModelViewMatrix"),
    },
  };


  //Billboard vertex positions
  const positionBuffer = gl.createBuffer();
  // gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
  setAttribute(gl, positionBuffer, programInfo.attribLocations.position, {size:2});
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
     // one face
      -1, -1, 
       1, -1,
      -1,  1,
      -1,  1,
       1, -1,
       1,  1,
    ],
  ), gl.STATIC_DRAW);
  // gl.enableVertexAttribArray(programInfo.attribLocations.position);
  // {
  //   const size = 2;  // 2 values per vertex
  //   const type = gl.FLOAT;
  //   const normalize = false;
  //   const stride = 0;
  //   const offset = 0;
  //   gl.vertexAttribPointer(programInfo.attribLocations.position, size, type, normalize, stride, offset);
  // }

  const totalLights = scene.map(s=>s.positions.length).reduce((a, b) => a + b,0);

  // light positions (offsets)
  const offsetBuffer = gl.createBuffer();
  //gl.bindBuffer(gl.ARRAY_BUFFER, offsetBuffer);
  setAttribute(gl, offsetBuffer, programInfo.attribLocations.offset, {size:3, divisor:1});
  gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(totalLights*3), gl.STATIC_DRAW);    
  // gl.enableVertexAttribArray(programInfo.attribLocations.offset);
  // {
  //   const size = 3;  // 3 values per vertex
  //   const type = gl.FLOAT;
  //   const normalize = false;
  //   const stride = 0;
  //   const offset = 0;
  //   gl.vertexAttribPointer(programInfo.attribLocations.offset, size, type, normalize, stride, offset);
  //   gl.vertexAttribDivisor(programInfo.attribLocations.offset, 1);
  // }

  // light colors
  const colorBuffer = gl.createBuffer();
  //gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
  setAttribute(gl, colorBuffer, programInfo.attribLocations.color, {size:3, divisor:1, type:gl.UNSIGNED_BYTE, normalize:true});
  gl.bufferData(gl.ARRAY_BUFFER, new Uint8Array(totalLights*3), gl.STATIC_DRAW);  
  // gl.enableVertexAttribArray(programInfo.attribLocations.color);
  // {
  //   const size = 3;  
  //   const type = gl.UNSIGNED_BYTE;
  //   const normalize = true;
  //   const stride = 0;
  //   const offset = 0;
  //   gl.vertexAttribPointer(programInfo.attribLocations.color, size, type, normalize, stride, offset);
  //   gl.vertexAttribDivisor(programInfo.attribLocations.color, 1);
  // }
  
  let startLight = 0
  scene.forEach(scenePart => {
    gl.bindBuffer(gl.ARRAY_BUFFER, offsetBuffer);
    gl.bufferSubData(
      gl.ARRAY_BUFFER, 
      startLight * 3 * 4,
      new Float32Array(scenePart.positions.map(p=>[p.x, p.y, p.z]).flat()), 
    );

    gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    const colors = Array(scenePart.positions.length).fill([0, 0, 0]).flat();
    gl.bufferSubData(
      gl.ARRAY_BUFFER, 
      startLight * 3,
      new Uint8Array(colors), 
    );

    const bufferIndex = startLight * 3;

    new Socket(scenePart.path, data => {
      if (!(data instanceof ArrayBuffer)){
          const json = JSON.parse(data)
          console.log("websocket json", json)
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

    //   window.requestAnimationFrame(() => {
    //     gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    //     gl.bufferSubData(
    //         gl.ARRAY_BUFFER, 
    //         bufferIndex,
    //         new Uint8Array(data), 
    //     );
    //   });
 
    // })

    startLight += scenePart.positions.length;
  });

  const grid = {indices: []}
  //////grid
  //TODO rename draw_grid to something like calc_grid 
//   const grid = draw_grid(viewParams.gridZ)

//   const gridPositionBuffer = gl.createBuffer();
//   setAttribute(gl, gridPositionBuffer, programInfo.attribLocations.position, {});
//   // gl.bindBuffer(gl.ARRAY_BUFFER, gridPositionBuffer);
//   gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(grid.vertices), gl.STATIC_DRAW);

//   const gridIndicesBuffer = gl.createBuffer();
//   gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gridIndicesBuffer);
//   gl.bufferData(
//     gl.ELEMENT_ARRAY_BUFFER,
//     new Uint16Array(grid.indices),
//     gl.STATIC_DRAW
//   );

  // const gridColorBuffer = gl.createBuffer();
  // const gridColors = Array(grid.vertices.length).fill([50, 50, 50]).flat();
  // gl.bindBuffer(gl.ARRAY_BUFFER, gridColorBuffer);
  // gl.bufferData(gl.ARRAY_BUFFER, new Uint8Array(gridColors), gl.STATIC_DRAW);





  gl.useProgram(shaderProgram);

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

  let then = 0;

  const fpsElem = document.querySelector("#fps");

  // Draw the scene repeatedly
  function render(now) {
    now *= 0.001; // convert to seconds
    deltaTime = now - then;
    then = now;

    //TODO ugly params
    updateScene(gl, time, programInfo, totalLights, grid.indices.length);
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

function updateScene(gl, time, programInfo, numInstances, gridIndicesCount)
{
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

  //clear
//   gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
gl.clear(gl.COLOR_BUFFER_BIT);


  //draw lights
  const offset = 0;
  const vertexCount = 6;  //todo derive from global vertices object
  gl.drawArraysInstanced(gl.TRIANGLES, offset, vertexCount, numInstances);
  gl.finish();

  //draw grid

    // // setPositionAttribute(gl, buffers.gridPosition, programInfo);
    // // setColorAttribute(gl, buffers.gridColor, programInfo);
    // gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, buffers.gridIndices);
    // // gl.useProgram(programInfo.program);
    // gl.drawElements(gl.LINES, gridIndicesCount, gl.UNSIGNED_SHORT, 0);
}


function draw_grid(gridz = 0) {
  let grid = { vertices: [], /*normals: [],*/ indices: [], indicesCount: 0 }

  const slices = 10
  // const normal = [0, 1, 0]

  for (let j = 0; j < slices + 1; j++) {
    for (let i = 0; i < slices + 1; i++) {
      const x = -1 + 2 * i / slices
      const y = gridz
      const z = -1 + 2 * j / slices
      grid.vertices.push(x, y, z);
      // grid.normals.push(...normal);
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

  // Tell WebGL how to pull out the positions from the position
  // buffer into the vertexPosition attribute.
  function setAttribute(gl, buffer, location, {size=3, divisor=0, type=gl.FLOAT, normalize=false}) {
    // const numComponents = 3;
    // const type = gl.FLOAT; // the data in the buffer is 32bit floats
    // const normalize = false; // don't normalize
    // const stride = 0; // how many bytes to get from one set of values to the next
    // // 0 = use type and numComponents above
    // const offset = 0; // how many bytes inside the buffer to start from
    // gl.bindBuffer(gl.ARRAY_BUFFER, bufferPosition);
    // gl.vertexAttribPointer(
    //   programInfo.attribLocations.vertexPosition,
    //   numComponents,
    //   type,
    //   normalize,
    //   stride,
    //   offset
    // );
    // gl.enableVertexAttribArray(programInfo.attribLocations.vertexPosition);



      gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
      // gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(totalLights*3), gl.STATIC_DRAW);    
      gl.enableVertexAttribArray(location);
      {
        //  const size = 3;  // 3 values per vertex
        // const type = gl.FLOAT;
        // const normalize = false;
        const stride = 0;
        const offset = 0;
        gl.vertexAttribPointer(location, size, type, normalize, stride, offset);
        gl.vertexAttribDivisor(location, divisor);
      }
  }
  
  // // Tell WebGL how to pull out the colors from the color buffer
  // // into the vertexColor attribute.
  // function setColorAttribute(gl, bufferColor, programInfo) {
  //   const numComponents = 3;
  //   const type = gl.UNSIGNED_BYTE;
  //   const normalize = true;
  //   const stride = 0;
  //   const offset = 0;
  //   gl.bindBuffer(gl.ARRAY_BUFFER, bufferColor);
  //   gl.vertexAttribPointer(
  //     programInfo.attribLocations.vertexColor,
  //     numComponents,
  //     type,
  //     normalize,
  //     stride,
  //     offset
  //   );
  //   gl.enableVertexAttribArray(programInfo.attribLocations.vertexColor);
  // }