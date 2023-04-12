function initBuffers(gl, scene) {

    //console.log(scene)

    let sphere = {vertices:[], normals:[], indices:[]}

    scene.forEach(scenePart => {
        scenePart.positions.forEach(pos => {
            //console.log('drawing',pos)
            draw_sphere(sphere,pos.x,pos.y,pos.z);
        })
    })

    // draw_sphere(sphere,0,0,0);
    // draw_sphere(sphere,1,1,1);
    
    console.log(sphere,sphere.indices.length)

    const positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphere.vertices), gl.STATIC_DRAW);

    const indexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
    gl.bufferData(
      gl.ELEMENT_ARRAY_BUFFER,
      new Uint16Array(sphere.indices),
      gl.STATIC_DRAW
    );

    // const normalBuffer = gl.createBuffer();
    // gl.bindBuffer(gl.ARRAY_BUFFER, normalBuffer);
    // gl.bufferData(
    //     gl.ARRAY_BUFFER,
    //     new Float32Array(sphere.normals),
    //     gl.STATIC_DRAW
    // );

    //const positionBuffer = initPositionBuffer(gl);
    //const colorBuffer = initColorBuffer(gl);
    //const indexBuffer = initIndexBuffer(gl);

    const colorBuffer = gl.createBuffer();
    const colors = Array(sphere.vertices.length).fill([0,0,255]).flat();
    gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Uint8Array(colors), gl.STATIC_DRAW);


    return {
      position: positionBuffer,
      color: colorBuffer,
      indices: indexBuffer,
      //normals: normalBuffer,
      indicesCount: sphere.indices.length,
      verticesCount: sphere.vertices.length
    };
  }
  
  function draw_sphere(target, cx, cy, cz) {
    // http://www.songho.ca/opengl/gl_sphere.html
    //const vertices = [];
    //const normals = [];
  
    const r = 0.01;
    const stacks = 2;
    const sectors = 4;
    const startVertex = target.vertices.length/3;
  
    //console.log({startVertex})

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
    //const indices = [];
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
  
    //return {vertices, normals, indices};
  }

export { initBuffers };