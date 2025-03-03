/*
* 
* Practica_02_base.js
* Videojuegos (30262) - Curso 2019-2020
* 
* Parte adaptada de: Alex Clarke, 2016, y Ed Angel, 2015.
* 
*/

class Camera {
    constructor() {
        this.position = new vec3(10, 0, 15);
        this.front = new vec3(0, 0, -1);
        this.up = new vec3(0, 1, 0);
        
        this.yaw = -90;   // Mirando hacia -Z
        this.pitch = 0;
        
        this.speed = 0.1;
        this.sensitivity = 0.2;
        this.fov = 45;    // Solo en perspectiva
        this.projectionType = 'perspective';

        this.mouseDown = false;
        this.lastMouseX = null;
        this.lastMouseY = null;

        this.initEventListeners();
    }

    getViewMatrix() {
        let target = vec3();
		target[0] = this.position[0] + this.front[0]
		target[1] = this.position[1] + this.front[1]
		target[2] = this.position[2] + this.front[2]

        //add(target, this.position, this.front);
        return lookAt(this.position, target, this.up);
    }

    getProjectionMatrix(aspectRatio) {
        if (this.projectionType === 'perspective') {
            return perspective(this.fov * Math.PI / 180, aspectRatio, 0.1, 100);
        } else {
            let orthoSize = 5;
            return ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1, 100);
        }
    }

    processKeyboard(key) {
        let velocity = this.speed;
        let right = vec3();
        right = cross(this.front, this.up);
        right = normalize(right);

        let move = vec3();
        if (key === 'ArrowUp') {
			move[0] += this.front[0] * velocity;
			move[1] += this.front[1] * velocity;
			move[2] += this.front[2] * velocity;
		}
		
		if (key === 'ArrowDown') {
			move[0] -= this.front[0] * velocity;
			move[1] -= this.front[1] * velocity;
			move[2] -= this.front[2] * velocity;
		}
		
		if (key === 'ArrowLeft') {
			move[0] -= right[0] * velocity;
			move[1] -= right[1] * velocity;
			move[2] -= right[2] * velocity;
		}
		
		if (key === 'ArrowRight') {
			move[0] += right[0] * velocity;
			move[1] += right[1] * velocity;
			move[2] += right[2] * velocity;
		}

		this.position[0] += move[0];
		this.position[1] += move[1];
		this.position[2] += move[2];
    }

    processMouseMovement(xOffset, yOffset) {
        this.yaw += xOffset * this.sensitivity;
        this.pitch -= yOffset * this.sensitivity;
        this.pitch = Math.max(-89, Math.min(89, this.pitch));

        let direction = vec3();
        direction[0] = Math.cos(this.yaw * Math.PI / 180) * Math.cos(this.pitch * Math.PI / 180);
        direction[1] = Math.sin(this.pitch * Math.PI / 180);
        direction[2] = Math.sin(this.yaw * Math.PI / 180) * Math.cos(this.pitch * Math.PI / 180);
        this.front = normalize(direction);
    }

    processScroll(delta) {
        if (this.projectionType === 'perspective') {
			console.log("NOW\nfov: ",this.fov);
            this.fov -= delta;
			console.log("NEXT\nfov: ",this.fov);
            this.fov = Math.max(20, Math.min(80, this.fov));
			console.log("THEN\nfov: ",this.fov);
        }
    }

    switchProjection(mode) {
        if (mode === 'p'){
			console.log("P");
			this.projectionType = 'perspective';
		}
        if (mode === 'o'){
			console.log("O");
			this.projectionType = 'ortho';
		}
    }

    initEventListeners() {
        window.addEventListener('keydown', (e) => {
			console.log(e.code);
            if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight'].includes(e.code)) {
                this.processKeyboard(e.code);
            } else if (e.code === 'KeyP') {
                this.switchProjection('p');
            } else if (e.code === 'KeyO') {
                this.switchProjection('o');
            } else if (e.code === "BracketRight") {
                this.processScroll(-5);
            } else if (e.code === "Slash") {
                this.processScroll(5);
            }
        });

        window.addEventListener('mousedown', (e) => {
            this.mouseDown = true;
            this.lastMouseX = e.clientX;
            this.lastMouseY = e.clientY;
        });

        window.addEventListener('mouseup', () => {
            this.mouseDown = false;
        });

        window.addEventListener('mousemove', (e) => {
            if (!this.mouseDown) return;
            let xOffset = e.clientX - this.lastMouseX;
            let yOffset = e.clientY - this.lastMouseY;
            this.processMouseMovement(xOffset, yOffset);
            this.lastMouseX = e.clientX;
            this.lastMouseY = e.clientY;
        });
    }
}


// Variable to store the WebGL rendering context
var gl;

//----------------------------------------------------------------------------
// MODEL DATA 
//----------------------------------------------------------------------------

//Define points' position vectors
const cubeVerts = [
	[ 0.5, 0.5, 0.5, 1], //0
	[ 0.5, 0.5,-0.5, 1], //1
	[ 0.5,-0.5, 0.5, 1], //2
	[ 0.5,-0.5,-0.5, 1], //3
	[-0.5, 0.5, 0.5, 1], //4
	[-0.5, 0.5,-0.5, 1], //5
	[-0.5,-0.5, 0.5, 1], //6
	[-0.5,-0.5,-0.5, 1], //7
];

const wireCubeIndices = [
//Wire Cube - use LINE_STRIP, starts at 0, 30 vertices
	0,4,6,2,0, //front
	1,0,2,3,1, //right
	5,1,3,7,5, //back
	4,5,7,6,4, //right
	4,0,1,5,4, //top
	6,7,3,2,6, //bottom
];

const cubeIndices = [	
//Solid Cube - use TRIANGLES, starts at 0, 36 vertices
	0,4,6, //front
	0,6,2,
	1,0,2, //right
	1,2,3, 
	5,1,3, //back
	5,3,7,
	4,5,7, //left
	4,7,6,
	4,0,1, //top
	4,1,5,
	6,7,3, //bottom
	6,3,2,
];

const pointsAxes = [];
pointsAxes.push([ 2.0, 0.0, 0.0, 1.0]); //x axis is green
pointsAxes.push([-2.0, 0.0, 0.0, 1.0]);
pointsAxes.push([ 0.0, 2.0, 0.0, 1.0]); //y axis is red
pointsAxes.push([ 0.0,-2.0, 0.0, 1.0]); 
pointsAxes.push([ 0.0, 0.0, 2.0, 1.0]); //z axis is blue
pointsAxes.push([ 0.0, 0.0,-2.0, 1.0]);

const pointsWireCube = [];
for (let i=0; i < wireCubeIndices.length; i++)
{
	pointsWireCube.push(cubeVerts[wireCubeIndices[i]]);
}

const pointsCube = [];
for (let i=0; i < cubeIndices.length; i++)
{
	pointsCube.push(cubeVerts[cubeIndices[i]]);
}

const shapes = {
	wireCube: {Start: 0, Vertices: 30},
	cube: {Start: 0, Vertices: 36},
	axes: {Start: 0, Vertices: 6}
};
	
const red =			[1.0, 0.0, 0.0, 1.0];
const green =		[0.0, 1.0, 0.0, 1.0];
const blue =		[0.0, 0.0, 1.0, 1.0];
const lightred =	[1.0, 0.5, 0.5, 1.0];
const lightgreen =	[0.5, 1.0, 0.5, 1.0];
const lightblue = 	[0.5, 0.5, 1.0, 1.0];
const white =		[1.0, 1.0, 1.0, 1.0];

const colorsAxes = [
	green, green, //x
	red, red,     //y
	blue, blue,   //z
];	

const colorsWireCube = [
	white, white, white, white, white,
	white, white, white, white, white,
	white, white, white, white, white,
	white, white, white, white, white,
	white, white, white, white, white,
	white, white, white, white, white,
];


function colorCubo(nivelRojo){
	return [
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
		[nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0], [nivelRojo, 0.0, 1 - nivelRojo, 1.0],
	]
};

const colorsCube = [	
	lightblue, lightblue, lightblue, lightblue, lightblue, lightblue,
	lightgreen, lightgreen, lightgreen, lightgreen, lightgreen, lightgreen,
	lightred, lightred, lightred, lightred, lightred, lightred,
	blue, blue, blue, blue, blue, blue,
	red, red, red, red, red, red,
	green, green, green, green, green, green,
];


function colorCubo(color){
	let colorRGB = rainbowColor(color);
	colorRGB = [colorRGB[0], colorRGB[1], colorRGB[2], 1.0];

	return [
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
		colorRGB, colorRGB, colorRGB,
	]
};


function rainbowColor(value) {
    if (value < 0 || value > 1) {
        throw new Error("El valor debe estar entre 0 y 1.");
    }

    let hue = value * 360; // El valor ahora se escala entre 0 y 360
    let saturation = 1.0; // Máxima saturación para colores vivos
    let lightness = 0.5;  // Luminosidad media

    return hslToRgb(hue, saturation, lightness);
}

// Función para convertir HSL a RGB
function hslToRgb(h, s, l) {
    let c = (1 - Math.abs(2 * l - 1)) * s;
    let x = c * (1 - Math.abs((h / 60) % 2 - 1));
    let m = l - c / 2;
    let r, g, b;

    if (h < 60) { r = c; g = x; b = 0; } 
    else if (h < 120) { r = x; g = c; b = 0; } 
    else if (h < 180) { r = 0; g = c; b = x; } 
    else if (h < 240) { r = 0; g = x; b = c; } 
    else if (h < 300) { r = x; g = 0; b = c; } 
    else { r = c; g = 0; b = x; }

    return [
        (r + m),
        (g + m),
        (b + m)
    ];
}

//----------------------------------------------------------------------------
// OTHER DATA 
//----------------------------------------------------------------------------

var model = new mat4();   		// create a model matrix and set it to the identity matrix
var view = new mat4();   		// create a view matrix and set it to the identity matrix
var projection = new mat4();	// create a projection matrix and set it to the identity matrix

var eye, target, up;			// for view matrix

var rotAngle = 0.0;
var trasAngle = 0.0;
var rotChange = 1.5;
var trasChange = 0.5;

var program;
var uLocations = {};
var aLocations = {};

var programInfo = {
			program,
			uniformLocations: {},
			attribLocations: {},
};

var numObjects = 20; // Número de objetos que quieres crear
var limitePosicion = 8;
var objectsToDraw = [
	{
	  programInfo: programInfo,
	  pointsArray: pointsAxes, 
	  colorsArray: colorsAxes, 
	  uniforms: {
		u_colorMult: [1.0, 1.0, 1.0, 1.0],
		u_model: new mat4(),
	  },
	  primType: "lines",
	},
	
];

var posicionesCubos = [];
var ejesTraslacion = [];
var rotaciones = [];
var velRotaciones = [];
var traslaciones = [];
var velTraslaciones = [];


function numAleatorioEntero(min, max) {
	return Math.floor(Math.random() * (max - min + 1) + min); // +1 para incluir 'n' en el rango
};

function numAleatorioFloat(min, max) {
	return Math.random() * (max - min + 1) + min; // +1 para incluir 'n' en el rango
};

function posicionAleatoria(){
	let rand = numAleatorioEntero(1, limitePosicion);
	let signo = numAleatorioEntero(0, 1);
	if(signo == 1){
		rand = -rand;
	}
	return rand;
}

for (let i = 0; i < numObjects; i++) {
	let num = numAleatorioEntero(0,2);
	ejesTraslacion.push(num);

	rotaciones.push(0);
	traslaciones.push(0);

	velRotaciones.push(numAleatorioFloat(0.5, 1.5));
	velTraslaciones.push(numAleatorioFloat(0.5, 1.0));


	if (num % 3 == 0) { // eje X
    	posicionesCubos.push(translate(0, posicionAleatoria(), 0));
	} else if (num % 3 == 1) { // eje Y
		posicionesCubos.push(translate(0, 0, posicionAleatoria()));
	} else { // eje Z
		posicionesCubos.push(translate(posicionAleatoria(), 0, 0));
	}
}

for (let i = 0; i < numObjects; i++) {
	
    let object = {
        programInfo: programInfo,
        pointsArray: pointsCube, // O cualquier otro conjunto de puntos que quieras usar
        colorsArray: colorCubo((1/numObjects)*i), // O colores específicos
        uniforms: {
            u_colorMult: [1.0, 1.0, 1.0, 1.0], // Diferentes colores dependiendo del índice
            u_model: new mat4(),  // Transformación del modelo, que se actualizará más tarde
        },
        primType: "triangles", // Tipo de primitiva (por ejemplo, triángulos)
    };

    // Puedes hacer cosas como modificar las transformaciones en la matriz de modelo dependiendo del índice
    object.uniforms.u_model = posicionesCubos[i]; // Mueve cada cubo a una posición diferente en el eje X

    objectsToDraw.push(object); // Agrega el objeto creado al array
}

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

	setPrimitive(objectsToDraw);

	// Set up a WebGL program
	// Load shaders and initialize attribute buffers
	program = initShaders(gl, "vertex-shader", "fragment-shader");
	  
	// Save the attribute and uniform locations
	uLocations.model = gl.getUniformLocation(program, "model");
	uLocations.view = gl.getUniformLocation(program, "view");
	uLocations.projection = gl.getUniformLocation(program, "projection");
	uLocations.colorMult = gl.getUniformLocation(program, "colorMult");
	aLocations.vPosition = gl.getAttribLocation(program, "vPosition");
	aLocations.vColor = gl.getAttribLocation(program, "vColor");

	programInfo.uniformLocations = uLocations;
	programInfo.attribLocations = aLocations;
	programInfo.program = program;

	gl.useProgram(programInfo.program);
	
	// Set up viewport 
	gl.viewport(0, 0, gl.drawingBufferWidth, gl.drawingBufferHeight);

	// Set up camera
	// Projection matrix
	projection = perspective( 45.0, gl.drawingBufferWidth/gl.drawingBufferHeight, 0.1, 100.0 );
	gl.uniformMatrix4fv( programInfo.uniformLocations.projection, gl.FALSE, projection ); // copy projection to uniform value in shader
    // View matrix (static cam)
	eye = vec3(-5.0, 5.0, 10.0);
	target =  vec3(0.0, 0.0, 0.0);
	up =  vec3(0.0, 1.0, 0.0);
	view = lookAt(eye,target,up);
	gl.uniformMatrix4fv(programInfo.uniformLocations.view, gl.FALSE, view); // copy view to uniform value in shader
	
	requestAnimFrame(render);
  
};

//----------------------------------------------------------------------------
// Rendering Event Function
//----------------------------------------------------------------------------

const camera = new Camera();

function render() {

	gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
	
	//----------------------------------------------------------------------------
	// MOVE STUFF AROUND
	//----------------------------------------------------------------------------
	let viewMatrix = camera.getViewMatrix();
	let projectionMatrix = camera.getProjectionMatrix(gl.drawingBufferWidth / gl.drawingBufferHeight);

	let ejeX = vec3(1.0, 0.0, 0.0);
	let ejeY = vec3(0.0, 1.0, 0.0);
	let ejeZ = vec3(0.0, 0.0, 1.0);

	for (let i = 1; i < numObjects + 1; i++) {
		if(ejesTraslacion[i-1] == 0){ // Traslacion eje Z (azul)
			// Rotación propia del cubo
			objectsToDraw[i].uniforms.u_model = rotate(rotaciones[i-1], ejeZ);
			
			// Traslación
			let posicionInicial = posicionesCubos[i-1]; 
			objectsToDraw[i].uniforms.u_model = mult(posicionInicial, objectsToDraw[i].uniforms.u_model);

			// Rotación orbital
			objectsToDraw[i].uniforms.u_model = mult(rotate(traslaciones[i-1], ejeX), objectsToDraw[i].uniforms.u_model);
		
		} else if (ejesTraslacion[i-1] == 1){ // Traslacion eje X (verde)
			// Rotación propia del cubo
			objectsToDraw[i].uniforms.u_model = rotate(rotaciones[i-1], ejeX);

			// Traslación
			let posicionInicial = posicionesCubos[i-1]; 
			objectsToDraw[i].uniforms.u_model = mult(posicionInicial, objectsToDraw[i].uniforms.u_model);

			// Rotación orbital
			objectsToDraw[i].uniforms.u_model = mult(rotate(traslaciones[i-1], ejeY), objectsToDraw[i].uniforms.u_model);

		} else { // Traslacion eje Y (rojo)
			// Rotación propia del cubo
			objectsToDraw[i].uniforms.u_model = rotate(rotaciones[i-1], ejeY);

			// Traslación
			let posicionInicial = posicionesCubos[i-1]; 
			objectsToDraw[i].uniforms.u_model = mult(posicionInicial, objectsToDraw[i].uniforms.u_model);
		
			// Rotación orbital
			objectsToDraw[i].uniforms.u_model = mult(rotate(traslaciones[i-1], ejeZ), objectsToDraw[i].uniforms.u_model);
		}
		
		rotaciones[i-1] += velRotaciones[i-1];
		traslaciones[i-1] += velTraslaciones[i-1];
	}


	//----------------------------------------------------------------------------
	// DRAW
	//----------------------------------------------------------------------------

	objectsToDraw.forEach(function(object) {

		gl.useProgram(object.programInfo.program);

		// Setup buffers and attributes
		setBuffersAndAttributes(object.programInfo, object.pointsArray, object.colorsArray);

		object.uniforms.u_view = viewMatrix;
		object.uniforms.u_projection = projectionMatrix;

		// Set the uniforms
		setUniforms(object.programInfo, object.uniforms);

		// Draw
		gl.drawArrays(object.primitive, 0, object.pointsArray.length);
    });	

	requestAnimationFrame(render);
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
	// Copy uniform model values to corresponding values in shaders
	gl.uniform4f(pInfo.uniformLocations.colorMult, uniforms.u_colorMult[0], uniforms.u_colorMult[1], uniforms.u_colorMult[2], uniforms.u_colorMult[3]);
	gl.uniformMatrix4fv(pInfo.uniformLocations.model, gl.FALSE, uniforms.u_model);
	gl.uniformMatrix4fv(pInfo.uniformLocations.view, gl.FALSE, uniforms.u_view);
}

function setBuffersAndAttributes(pInfo, ptsArray, colArray) {
	// Load the data into GPU data buffers
	// Vertices
	var vertexBuffer = gl.createBuffer();
	gl.bindBuffer( gl.ARRAY_BUFFER, vertexBuffer );
	gl.bufferData( gl.ARRAY_BUFFER,  flatten(ptsArray), gl.STATIC_DRAW );
	gl.vertexAttribPointer( pInfo.attribLocations.vPosition, 4, gl.FLOAT, gl.FALSE, 0, 0 );
	gl.enableVertexAttribArray( pInfo.attribLocations.vPosition );

	// Colors
	var colorBuffer = gl.createBuffer();
	gl.bindBuffer( gl.ARRAY_BUFFER, colorBuffer );
	gl.bufferData( gl.ARRAY_BUFFER,  flatten(colArray), gl.STATIC_DRAW );
	gl.vertexAttribPointer( pInfo.attribLocations.vColor, 4, gl.FLOAT, gl.FALSE, 0, 0 );
	gl.enableVertexAttribArray( pInfo.attribLocations.vColor );
}
