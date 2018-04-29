import { Technique } from "../scene/technique";
import Obj from "../scene/object";
import { ulist, ulist_elem } from "../util/ulist";
import { Viewport, PerspectiveViewport } from "../scene/viewport";
import { Communicators, Communicator } from "../gl/communicator";
import { Offscreen, RenderBuffer } from "./offscreen";
import { Shader, Uniform } from "../gl/shader";
import { gl, Renderer } from "../renderer/renderer";
import { TextureCube } from "../gl/texture";
import { glm } from "../util/glm"
import { VAO, VertexAttrs } from "../gl/vertexAttrs";

class AmbientMap extends Communicator {
	constructor(private skybox: TextureCube) {
		super("AmbientMap", false);
		this.init({
			Ambient: {
				type: "int",
				init: 0x3
			}
		});
	}
}

class AmbientCube extends Technique {
	constructor() {
		super();
		let vertices = new VertexAttrs(["pos2"]);
		vertices.set("pos2", [
			-1, 1, 1, 1, 1, -1, -1, -1
		]);
		this.vao = new VAO(vertices, [
			0, 1, 2, 0, 2, 3
		]);
	}

	render(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
		this.skybox.unbind(0x3);

		let dt = Renderer.instance.time - this.time;
		this.time = Renderer.instance.time;

		gl.disable(gl.DEPTH_TEST);
			this.viewport.use();
				this.offscreen.bind();
					for (let i = 0; i != 6; ++i) {
						this.offscreen.set(gl.COLOR_ATTACHMENT0, this.skybox, i);

						gl.clear(gl.COLOR_BUFFER_BIT);
						this.shader.use();
							this.gSpace.set(AmbientCube.spaceTrans[i]);
							// this.gSunPos.set(glm.vec3(1, 0, 0.1));
							let alpha = Renderer.instance.time;
							this.gSunPos.set(glm.vec3(Math.sin(alpha), 0, Math.cos(alpha)));

							this.vao.bind();
								this.vao.draw();
							this.vao.unbind();
						this.shader.unuse();
					}
				this.offscreen.unbind();
			this.viewport.unuse();
		gl.enable(gl.DEPTH_TEST);
	}

	mainImage(objects: ulist<Obj>, viewports: ulist<Viewport>, communicators: Communicators) {
		if (this.communicator != null) {
			this.communicator.remove();
		}
		this.communicator = communicators.add(this.gAmbient);
		this.skybox.bind(0x3);
	}

	private offscreen = new Offscreen();
	private skybox = new TextureCube(gl.RGBA);
	private communicator: ulist_elem<Communicator> = null;
	private vao: VAO;

	private time: number = 0;

	private gSpace: Uniform = Shader.uniform("mat4", "gSpace");
	private gSunPos: Uniform = Shader.uniform("vec3", "gSunPos");

	private gAmbient = new AmbientMap(this.skybox);

	private shader = Shader.create("autoSkybox");
	private viewport = new Viewport(0, 0, 
		Renderer.instance.canvas.height, Renderer.instance.canvas.height);
	
	static spaceTrans = [
		glm.mat4( 0, 0, -1, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 ),
		glm.mat4( 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0, 1 ),
		glm.mat4( 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1 ),
		glm.mat4( -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, 1 )
	];
}

export {
	AmbientCube
}
