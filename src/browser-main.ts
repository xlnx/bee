// function skybox() {
// 	let camera = scene.createViewport<PerspectiveViewport>(new PerspectiveViewport()).get();
// 	camera.position = glm.vec3(4, 0, 1.2);
// 	camera.target = glm.vec3(-4, 0, -0.8);

// 	gl.clearColor(0, 0, 0, 0);
// 	gl.clearDepth(1);

// 	gl.enable(gl.DEPTH_TEST);
// 	// gl.enable(gl.BLEND);
// 	// gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

// 	let ambient = scene.createTechnique<AmbientCube>(new AmbientCube());
// 	// let skybox = scene.createObject<Skybox>(new Skybox());

// 	let off = scene.createObject<TestScreen>(new TestScreen()).get();

// 	let cameraPositions = [
// 		glm.vec3(1, 0, 0),
// 		glm.vec3(-1, 0, 0),
// 		glm.vec3(0, 1, 0),
// 		glm.vec3(0, -1, 0),
// 		glm.vec3(0, 0, 1),
// 		glm.vec3(0, 0, -1)
// 	];

// 	renderer.dispatch("keydown", (e: KeyboardEvent) => {
// 		if (!e.altKey && !e.shiftKey && !e.ctrlKey && !e.metaKey) {
// 			if ("012345".indexOf(e.key) >= 0) {
// 				off.face = cameraPositions[+e.key];
// 			}
// 		}
// 	});

// 	renderer.dispatch("render", (time: number) => {
// 		scene.render();
// 	});

// 	renderer.start();
// }

import { Game } from "./game/game";

new Game().start();