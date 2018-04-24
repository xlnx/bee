import { ulist_elem, ulist } from "../util/ulist"
import Obj from "./object"
import { Viewport } from "./viewport";
import { Shader } from "../gl/shader";
import { Communicator, Communicators } from "../gl/communicator";

export default class Scene {
	private objects = new ulist<Obj>();
	private viewports = new ulist<Viewport>();
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

	renderPass() {
		this.communicators.use();
			this.viewports.visit((v: ulist_elem<Viewport>) => {
				this.objects.visit((e: ulist_elem<Obj>) => {
					e.get().render(v.get(), this.shader);
				});
			});
		this.communicators.unuse();
	}

	private shader = Shader.create("test");
}
