// import Obj from "../scene/object"
// import { Viewport, PerspectiveViewport } from "../scene/viewport";
// import { Shader, Uniform } from "../gl/shader";
// import { ulist_elem, ulist } from "../util/ulist"
// import { Offscreen, RenderBuffer } from "../techniques/offscreen";
// import { Communicator, Communicators } from "../gl/communicator";
// import { gl, Renderer, gl2 } from "../renderer/renderer";
// import { Texture2D, TextureCube } from "../gl/texture";
// import { glm } from "../util/glm"
// import { Technique } from "../scene/technique";
// import { RBO } from "../gl/buffer";
// import { PointLight } from "./lighting";

// class ShadowCommunicator extends Communicator {
// 	constructor() {
// 		super("Shadow", false);
// 		this.init({
// 			ShadowMap: {
// 				type: "int",
// 				init: 0
// 			},
// 			LightWorldPos: {
// 				type: "vec3",
// 				init: glm.vec3(0, 0, 0)
// 			}
// 		});
// 	}
// }

// class ShadowTechnique extends Technique {
// 	private offscreen = new Offscreen();

// 	constructor() {
// 		super();
// 		Shader.require({
// 			Shadow: {
// 				fs: "shadow"
// 			}
// 		});
// 		this.offscreen.bind();
// 			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(
// 					gl.DEPTH_COMPONENT16, Renderer.instance.canvas.height,
// 					Renderer.instance.canvas.height));
// 		this.offscreen.unbind();
// 		this.lightCamera.fov = glm.radians(90);
// 		this.lightCamera.whratio = 1;
// 	}

// 	setLight(light: PointLight) {
// 		this.light = light;
// 	}
// 	render(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
// 		this.shadowTexture.unbind(0);

// 		gl.cullFace(gl.FRONT);
// 		gl.clearColor(1, 1, 1, 1);
// 		this.lightCamera.use();
// 			this.offscreen.bind();
// 				Shader.specify("Shadow");
// 					communicators.use((e: Communicator) => {
// 						return e.type.indexOf("Light") < 0;
// 					});
// 						this.lightCamera.position = this.light.get("position");
// 						for (let i = 0; i != 6; ++i) {
// 							this.offscreen.set(gl.COLOR_ATTACHMENT0, this.shadowTexture, i);
// 							// this.offscreen.check();

// 							gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
// 							[ this.lightCamera.target, this.lightCamera.up ] = ShadowTechnique.cameraPositions[i];
// 							objects.visit((e: ulist_elem<Obj>) => {
// 								if (!("isTerrain" in e.get()) && !("isSkybox" in e.get())) {
// 									e.get().render(this.lightCamera, this.shader);
// 								}
// 							});
// 						}
// 					communicators.unuse();
// 				Shader.unspecify();
// 			this.offscreen.unbind();
// 		this.lightCamera.unuse();
// 		gl.clearColor(0, 0, 0, 0);
// 		gl.cullFace(gl.BACK);
// 	}
// 	mainImage(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
// 		if (this.communicator != null) {
// 			this.communicator.remove();
// 		}
// 		this.communicator = communicators.add(this.gShadowMap);
// 		this.gShadowMap.set("LightWorldPos", this.lightCamera.position);
// 		this.shadowTexture.bind(0);
// 	}

// 	private shader = Shader.create("shadow");

// 	private communicator: ulist_elem<Communicator> = null;
// 	private gShadowMap: ShadowCommunicator = new ShadowCommunicator();
// 	private shadowTexture: TextureCube = new TextureCube(gl.RGBA);

// 	private lightCamera: PerspectiveViewport = new PerspectiveViewport(0, 0, 
// 			Renderer.instance.canvas.height, Renderer.instance.canvas.height);
// 	private light: PointLight;

// 	static cameraPositions = [
// 		[ glm.vec3(1, 0, 0), glm.vec3(0, -1, 0) ],
// 		[ glm.vec3(-1, 0, 0), glm.vec3(0, -1, 0) ],
// 		[ glm.vec3(0, 1, 0), glm.vec3(0, 0, 1) ],
// 		[ glm.vec3(0, -1, 0), glm.vec3(0, 0, -1) ],
// 		[ glm.vec3(0, 0, 1), glm.vec3(0, -1, 0) ],
// 		[ glm.vec3(0, 0, -1), glm.vec3(0, -1, 0) ]
// 	];
// }

// export {
// 	ShadowTechnique
// }
