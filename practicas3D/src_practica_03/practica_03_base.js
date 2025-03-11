/*
* 
* Practica_02_base.js
* Videojuegos (30262) - Curso 2019-2020
* 
* Parte adaptada de: Alex Clarke, 2016, y Ed Angel, 2015.
* 
*/

// Eje X (izquierda)
// Eje Y (delante)
// Eje Z (arriba)

// Variable to store the WebGL rendering context
var gl;

//----------------------------------------------------------------------------
// OTHER DATA 
//----------------------------------------------------------------------------

var model = new mat4();   		// create a model matrix and set it to the identity matrix
var view = new mat4();   		// create a view matrix and set it to the identity matrix
var projection = new mat4();	// create a projection matrix and set it to the identity matrix

var eye, target, up;			// for view matrix

const gravity = 9.8;
const vmax = 3;
const fuerzaMovimiento = 5;  // Cuanta fuerza aplicamos
const masaEsfera = 1.0;        // Asumimos masa de 1 kg
let controlForces = [0, 0, 0]; // Almacena la fuerza aplicada

// Detectar teclas presionadas
window.addEventListener("keydown", (event) => {
    switch (event.key) {
        case "ArrowUp":    controlForces[1] = fuerzaMovimiento; break;  // ↑ Mueve en Y+
        case "ArrowDown":  controlForces[1] = -fuerzaMovimiento; break; // ↓ Mueve en Y-
        case "ArrowLeft":  controlForces[0] = -fuerzaMovimiento; break; // ← Mueve en X-
        case "ArrowRight": controlForces[0] = fuerzaMovimiento; break;  // → Mueve en X+
    }
});
/*
// Detectar cuando se suelta la tecla (detener la fuerza)
window.addEventListener("keyup", (event) => {
    switch (event.key) {
        case "ArrowUp":   
        case "ArrowDown":  controlForces[1] = 0; break;
        case "ArrowLeft":  
        case "ArrowRight": controlForces[0] = 0; break;
    }
});*/


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

var numObjects = 20;

// Crea un número aleatorio entero entre 'min' y 'max'
function numAleatorioEntero(min, max) {
	return Math.floor(Math.random() * (max - min + 1) + min); // +1 para incluir 'n' en el rango
};

function posicionAleatoria(){
	let rand = numAleatorioEntero(1, limitePosicion);
	let signo = numAleatorioEntero(0, 1);
	if(signo == 1){
		rand = -rand;
	}
	return rand;
}


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

// Devuelve un array con los colores del cubo
function colorEsfera(color){
	let colorRGB = rainbowColor(color);
	colorRGB = [colorRGB[0], colorRGB[1], colorRGB[2], 1.0];

	return colorRGB;
};


/* Si más de un plano, usar funciones

// Function to calculate the center of the plane
function calculatePlaneCenter(points) {
    let center = [0, 0, 0];
    points.forEach(point => {
        center[0] += point[0];
        center[1] += point[1];
        center[2] += point[2];
    });
    center[0] /= points.length;
    center[1] /= points.length;
    center[2] /= points.length;
    return center;
}

// Function to calculate the normal of the plane
function calculatePlaneNormal(points) {
    let v1 = subtract(points[1], points[0]);
    let v2 = subtract(points[2], points[0]);
    let normal = cross(v1, v2);
    return normalize(normal);
}

// Function to calculate the size of the plane
function calculatePlaneSize(points) {
    let size = distance(points[0], points[2]);
    return size;
}

// Calculate the plane properties
let planeCenter = calculatePlaneCenter(pointsPlane);
let planeNormal = calculatePlaneNormal(pointsPlane);
let planeSize = calculatePlaneSize(pointsPlane);

// Update the plane object
plane.position = planeCenter;
plane.normal = planeNormal;
plane.size = planeSize;

*/


var plane = {
	position: [0.0, 0.0, 0.0],
	size: 10,
	normal: [0.0, 0.0, 1.0],
};

var spheres = []
for (let i = 0; i < numObjects; i++) {
	
    let object = {
        position: [0.0, 0.0, 0.0],
		rotation: [0.0, 0.0, 0.0],
		velocity: [0.0, 0.0, 0.0],		
		radius: 1.5,
    };

    spheres.push(object); // Agrega el objeto creado al array
}

var objectsToDraw = []


// Metemos el plano
objectsToDraw.push({
	programInfo: planeProgramInfo,
	pointsArray: pointsPlane, 
	uniforms: {
		u_color: [1.0, 1.0, 1.0, 1.0],
		u_model: new mat4(),
	},
	primType: "triangles",
});


for (let i = 0; i < numObjects; i++) {
	colorAux=[1.0, 1.0, 1.0, 1.0];
	if (i > 0){
		colorAux = colorEsfera((1/numObjects)*i);
	}
    let object = {
        programInfo: sphereProgramInfo,
		pointsArray: pointsSphere, 
		uniforms: {
            u_color: colorAux, // Diferentes colores dependiendo del índice
			u_model: spheres[i].position,
		},
		primType: "triangles",
    };

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
		if(index !== 0){ // Saltamos la blanca
			sphere.position = [5*(2*Math.random() - 1), 5*(2*Math.random() - 1), 5*(4*Math.random() - 1)];
			/*if(sphere.uniforms.u_color[0] !== 1.0 || sphere.uniforms.u_color[1] !== 1.0 || sphere.uniforms.u_color[2] !== 1.0){
				sphere.position = [5*(2*Math.random() - 1), 5*(2*Math.random() - 1), 5*(4*Math.random() - 1)];
			}
			else{
				sphere.position = [5*(2*Math.random() - 1), 5*(2*Math.random() - 1), sphere.radius/2];
			}*/
			sphere.rotation = [Math.random()*360, Math.random()*360, Math.random()*360];
		}
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
	// Skip de frames para que cuando estes fuera no corra el tiempo y meta un rebote loco
	dt = Math.min(dt, 16*4);
    lastTick = now;

	update(dt)
	render(dt)

	window.requestAnimationFrame(tick)
}

//----------------------------------------------------------------------------
// Update Event Function
//----------------------------------------------------------------------------

function update(dt) {
	// Actualizamos plano
	objectsToDraw[0].uniforms.u_model = scale(plane.size, plane.size, plane.size);
	
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
		let ejeZ = vec3(0.0, 0.0, 1.0);
		transform = mult(rotate(sphere.rotation[2], ejeZ), transform);

		// Update position of the sphere based in velocity
		if(index!==0){
			sphere.position[0] += sphere.velocity[0]*dt/1000;
			sphere.position[1] += sphere.velocity[1]*dt/1000;
			sphere.position[2] += sphere.velocity[2]*dt/1000; 
			
			// 2.1 Gravity: Update velocity of the sphere based in gravity
			sphere.velocity[2] -= gravity * dt/1000;
			
			// 2.2 Check sphere-plane collision

			let sphereToPlane = Array.from(subtract(sphere.position, plane.position));

			// Distancia de la esfera al plano				
			let d = (sphereToPlane[0] * plane.normal[0]) +
					(sphereToPlane[1] * plane.normal[1]) +
					(sphereToPlane[2] * plane.normal[2]);
			
			if (d <= sphere.radius){

				/* 
				
				*/
				sphere.velocity[2] = -sphere.velocity[2]*0.95;
				sphere.position[2] = sphere.radius;
				/*
				let withinPlaneBounds = Math.abs(sphere.position[0] - plane.position[0]) <= plane.size  &&
                        Math.abs(sphere.position[1] - plane.position[1]) <= plane.size; 
				*/

				// Punto interseccion
				let intersectionPoint = [
					sphere.position[0] - (d - sphere.radius) * plane.normal[0],
					sphere.position[1] - (d - sphere.radius) * plane.normal[1],
					sphere.position[2] - (d - sphere.radius) * plane.normal[2]
				];
				
			}
		}
		/*
		else{
			// Cálculo de aceleración (F = m * a  →  a = F/m)
			let acceleration = [
				controlForces[0] / masaEsfera,
				controlForces[1] / masaEsfera,
				0  // No aplicamos fuerza en Z
			];
		
			// Actualizar velocidad
			sphere.velocity[0] += acceleration[0] * dt / 1000;
			sphere.velocity[1] += acceleration[1] * dt / 1000;
		
			// Limitar la velocidad máxima
			sphere.velocity[0] = Math.max(-vmax, Math.min(vmax, sphere.velocity[0]));
			sphere.velocity[1] = Math.max(-vmax, Math.min(vmax, sphere.velocity[1]));
		
			// Actualizar posición basada en la velocidad
			sphere.position[0] += sphere.velocity[0] * dt / 1000;
			sphere.position[1] += sphere.velocity[1] * dt / 1000;
			sphere.position[2] += sphere.velocity[2] * dt / 1000;
		
			// Aplicar gravedad en Z
			sphere.velocity[2] -= gravity * dt / 1000;
		}*/
		

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
