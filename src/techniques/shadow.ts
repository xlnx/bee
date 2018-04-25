import Obj from "../scene/object"
import { Viewport } from "../scene/viewport";
import { Shader, Uniform } from "../gl/shader";
import { ulist_elem, ulist } from "../util/ulist"
import { Offscreen, RenderBuffer } from "../techniques/offscreen";
import { Communicator, Communicators } from "../gl/communicator";
import { gl, Renderer, gl2 } from "../renderer/renderer";
import { Texture2D, TextureCube } from "../gl/texture";
import { glm } from "../util/glm"
import { Technique } from "../scene/technique";
import { RBO } from "../gl/buffer";

class ShadowCommunicator extends Communicator {
	constructor() {
		super("Shadow");
		this.init({
			ShadowMap: {
				type: "int",
				init: 0
			}
		});
	}
}

class ShadowTechnique extends Technique {
	private offscreen = new Offscreen();

	constructor() {
		super();
		this.offscreen.bind();
			this.offscreen.set(gl.DEPTH_ATTACHMENT, new RenderBuffer(
					gl.DEPTH_COMPONENT16, Renderer.instance.canvas.height,
					Renderer.instance.canvas.height));
		this.offscreen.unbind();
	}

	render(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
		gl.cullFace(gl.FRONT);
		gl.clearColor(1, 1, 1, 1);
		this.offscreen.bind();
			communicators.use();
				for (let i = 0; i != 6; ++i) {
					this.offscreen.set(gl.COLOR_ATTACHMENT0, this.shadowTexture, i);
					this.offscreen.validate();

					gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
					[ this.lightCamera.target, this.lightCamera.up ] = ShadowTechnique.cameraPositions[i];
					objects.visit((e: ulist_elem<Obj>) => {
						e.get().render(this.lightCamera, this.shader);
					});
				}
			communicators.unuse();
			this.offscreen.unbind();
		gl.clearColor(0, 0, 0, 0);
		gl.cullFace(gl.BACK);
	}
	mainImage(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
		if (this.communicator != null) {
			this.communicator.remove();
		}
		this.communicator = communicators.add(this.gShadowMap);
	}

	private shader = Shader.create("shadow");

	private communicator: ulist_elem<Communicator> = null;
	private gShadowMap: ShadowCommunicator = new ShadowCommunicator();
	private shadowTexture: TextureCube = new TextureCube(gl.RGB);

	private lightCamera: Viewport = new Viewport();

	static cameraPositions = [
		[ glm.vec3(1, 0, 0), glm.vec3(0, -1, 0) ],
		[ glm.vec3(-1, 0, 0), glm.vec3(0, -1, 0) ],
		[ glm.vec3(0, 1, 0), glm.vec3(0, 0, 1) ],
		[ glm.vec3(0, -1, 0), glm.vec3(0, 0, -1) ],
		[ glm.vec3(0, 0, 1), glm.vec3(0, -1, 0) ],
		[ glm.vec3(0, 0, -1), glm.vec3(0, -1, 0) ]
	];
}

export {
	ShadowTechnique
}
