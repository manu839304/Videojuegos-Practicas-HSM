/*
* 
* Practica_02_base.js
* Videojuegos (30262) - Curso 2019-2020
* 
* Parte adaptada de: Alex Clarke, 2016, y Ed Angel, 2015.
* 
*/

// Variable to store the WebGL rendering context
var gl;

//----------------------------------------------------------------------------
// OTHER DATA 
//----------------------------------------------------------------------------

var model = new mat4();   		// create a model matrix and set it to the identity matrix
var view = new mat4();   		// create a view matrix and set it to the identity matrix
var projection = new mat4();	// create a projection matrix and set it to the identity matrix

var eye, target, up;			// for view matrix

var planeProgramInfo = {
	program: {},
	uniformLocations: {},
	attribLocations: {},
};

var sphereProgramInfo = {
	program: {},
	uniformLocations: {},
	attribLocations: {},
};

// List of objects to draw
var objectsToDraw = [
	{
		programInfo: planeProgramInfo,
		pointsArray: pointsPlane, 
		uniforms: {
			u_color: [1.0, 1.0, 1.0, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [1.0, 1.0, 1.0, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [1.0, 0.5, 0.5, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [0.5, 1.0, 0.5, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [0.5, 0.5, 1.0, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [1.0, 1.0, 0.5, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [1.0, 0.5, 1.0, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
	{
		programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
			u_color: [0.5, 1.0, 1.0, 1.0],
			u_model: new mat4(),
		},
		primType: "triangles",
	},
];

// List of spheres with their current state
var spheres = [
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
	{
		position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		radius: 1.5,
	},
]

//----------------------------------------------------------------------------
// Initialization function
//----------------------------------------------------------------------------

window.onload = function init() {
	// Set up a WebGL Rendering Context in an HTML5 Canvas
	var canvas = document.getElementById("gl-canvas");
	gl = WebGLUtils.setupWebGL(canvas);
	if (!gl) {
		alert("WebGL isn't available");
	}

	//  Configure WebGL
	gl.clearColor(0.0, 0.0, 0.0, 1.0);
	gl.enable(gl.DEPTH_TEST);
	gl.disable(gl.CULL_FACE);

	setPrimitive(objectsToDraw);

	// Set up a WebGL program for the plane
	// Load shaders and initialize attribute buffers
	planeProgramInfo.program = initShaders(gl, "plane-vertex-shader", "plane-fragment-shader");
	  
	// Save the attribute and uniform locations
	planeProgramInfo.uniformLocations.model = gl.getUniformLocation(planeProgramInfo.program, "model");
	planeProgramInfo.uniformLocations.view = gl.getUniformLocation(planeProgramInfo.program, "view");
	planeProgramInfo.uniformLocations.projection = gl.getUniformLocation(planeProgramInfo.program, "projection");
	planeProgramInfo.uniformLocations.baseColor = gl.getUniformLocation(planeProgramInfo.program, "baseColor");
	planeProgramInfo.attribLocations.vPosition = gl.getAttribLocation(planeProgramInfo.program, "vPosition");

	// Set up a WebGL program for spheres
	// Load shaders and initialize attribute buffers
	sphereProgramInfo.program = initShaders(gl, "sphere-vertex-shader", "sphere-fragment-shader");
	  
	// Save the attribute and uniform locations
	sphereProgramInfo.uniformLocations.model = gl.getUniformLocation(sphereProgramInfo.program, "model");
	sphereProgramInfo.uniformLocations.view = gl.getUniformLocation(sphereProgramInfo.program, "view");
	sphereProgramInfo.uniformLocations.projection = gl.getUniformLocation(sphereProgramInfo.program, "projection");
	sphereProgramInfo.uniformLocations.baseColor = gl.getUniformLocation(sphereProgramInfo.program, "baseColor");
	sphereProgramInfo.attribLocations.vPosition = gl.getAttribLocation(sphereProgramInfo.program, "vPosition");

	// Set up viewport 
	gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);

	// Set initial positions
	spheres.forEach(function(sphere, index) {
		sphere.position = [5*(2*Math.random() - 1), 5*(2*Math.random() - 1), sphere.radius/2.0];
		sphere.rotation = [Math.random()*360, Math.random()*360, Math.random()*360];
	});

	spheres[0].position = [0.0, 0.0, spheres[0].radius/2.0];
	
	lastTick = Date.now();
	requestAnimFrame(tick);
};

//----------------------------------------------------------------------------
// Tick Event Function
//----------------------------------------------------------------------------

var lastTick;

function tick(nowish) {
	var now = Date.now();

    var dt = now - lastTick;
    lastTick = now;

	update(dt)
	render(dt)

	window.requestAnimationFrame(tick)
}

//----------------------------------------------------------------------------
// Update Event Function
//----------------------------------------------------------------------------

function update(dt) {
	// Update state
	spheres.forEach(function(sphere, index) {
		// Update state (rotation) of the sphere
		sphere.rotation[0] = (sphere.rotation[0] + 0.02*dt) % 360;
		sphere.rotation[1] = (sphere.rotation[1] + 0.02*dt) % 360;

		// Update graphical representation
		let transform = scale(sphere.radius, sphere.radius, sphere.radius);

		let ejeX = vec3(1.0, 0.0, 0.0);
		transform = mult(rotate(sphere.rotation[0], ejeX), transform);
		let ejeY = vec3(0.0, 1.0, 0.0);
		transform = mult(rotate(sphere.rotation[1], ejeY), transform);
		let ejeZ = vec3(1.0, 0.0, 0.0);
		transform = mult(rotate(sphere.rotation[2], ejeZ), transform);

		transform = mult(translate(sphere.position[0], sphere.position[1], sphere.position[2]), transform);

		// Skip the plane
		index += 1;

		objectsToDraw[index].uniforms.u_model = transform;
	});
}

//----------------------------------------------------------------------------
// Rendering Event Function
//----------------------------------------------------------------------------

function render(dt) {
	// Clear the buffer and draw everything
	gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

	objectsToDraw.forEach(function(object) {
		gl.useProgram(object.programInfo.program);

		// Setup buffers and attributes
		setBuffersAndAttributes(object.programInfo, object);

		// Set the uniforms
		setUniforms(object.programInfo, object.uniforms);

		// Draw
		gl.drawArrays(object.primitive, 0, object.pointsArray.length);
	});
}

//----------------------------------------------------------------------------
// Utils functions
//----------------------------------------------------------------------------

function setPrimitive(objectsToDraw) {	
	objectsToDraw.forEach(function(object) {
		switch(object.primType) {
		  case "lines":
			object.primitive = gl.LINES;
			break;
		  case "line_strip":
			object.primitive = gl.LINE_STRIP;
			break;
		  case "triangles":
			object.primitive = gl.TRIANGLES;
			break;
		  default:
			object.primitive = gl.TRIANGLES;
		}
	});	
}	

function setUniforms(pInfo, uniforms) {
	var canvas = document.getElementById("gl-canvas");

	// Set up camera
	// Projection matrix
	projection = perspective( 45.0, canvas.width/canvas.height, 0.1, 100.0 );
	gl.uniformMatrix4fv( pInfo.uniformLocations.projection, gl.FALSE, projection ); // copy projection to uniform value in shader
	
	// View matrix (static cam)
	eye = vec3(10.0, 0.0, 10.0);
	target = vec3(0.0, 0.0, 0.0);
	up = vec3(0.0, 0.0, 1.0);
	view = lookAt(eye,target,up);
	
	gl.uniformMatrix4fv(pInfo.uniformLocations.view, gl.FALSE, view); // copy view to uniform value in shader

	// Copy uniform model values to corresponding values in shaders
	if (pInfo.uniformLocations.baseColor != null) {
		gl.uniform4f(pInfo.uniformLocations.baseColor, uniforms.u_color[0], uniforms.u_color[1], uniforms.u_color[2], uniforms.u_color[3]);
	}
	gl.uniformMatrix4fv(pInfo.uniformLocations.model, gl.FALSE, uniforms.u_model);
}

function setBuffersAndAttributes(pInfo, object) {
	// Load the data into GPU data buffers
	// Vertices
	var vertexBuffer = gl.createBuffer();
	gl.bindBuffer( gl.ARRAY_BUFFER, vertexBuffer );
	gl.bufferData( gl.ARRAY_BUFFER,  flatten(object.pointsArray), gl.STATIC_DRAW );
	gl.vertexAttribPointer( pInfo.attribLocations.vPosition, 4, gl.FLOAT, gl.FALSE, 0, 0 );
	gl.enableVertexAttribArray( pInfo.attribLocations.vPosition );
}
