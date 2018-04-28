import { ulist_elem, ulist } from "../util/ulist"
import Obj from "./object"
import { Viewport } from "./viewport";
import { Shader } from "../gl/shader";
import { Communicator, Communicators } from "../gl/communicator";
import { gl, Renderer, gl2 } from "../renderer/renderer";
import { glm } from "../util/glm"
import { Technique } from "./technique";

export default class Scene {
	private objects = new ulist<Obj>();
	private viewports = new ulist<Viewport>();
	private techniques = new ulist<Technique>();
	private communicators = new Communicators();

	createObject<T>(object: Obj): ulist_elem<T> {
		return this.objects.push(object).translate<T>();
	}
	createViewport<T>(viewport: Viewport): ulist_elem<T> {
		return this.viewports.push(viewport).translate<T>();
	}
	createCommunicator<T>(communicator: Communicator): ulist_elem<T> {
		return this.communicators.add(communicator).translate<T>();
	}
	createTechnique<T>(technique: Technique): ulist_elem<T> {
		return this.techniques.push(technique).translate<T>();
	}

	render() {
		this.techniques.visit((e: ulist_elem<Technique>) => {
			e.get().render(this.objects, this.viewports, this.communicators);
		});
		gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
		this.techniques.visit((e: ulist_elem<Technique>) => {
			e.get().mainImage(this.objects, this.viewports, this.communicators);
		});
		this.communicators.use();
			this.viewports.visit((v: ulist_elem<Viewport>) => {
				v.get().use();
					this.objects.visit((e: ulist_elem<Obj>) => {
						e.get().render(v.get(), this.shader);
					});
				v.get().unuse();
			});
		this.communicators.unuse();
	}

	private shader = Shader.create("test");
}
