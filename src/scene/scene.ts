import { ulist_elem, ulist } from "../util/ulist"
import Obj from "./object"
import { Viewport } from "./viewport";
import { Shader } from "../gl/shader";

export default class Scene {
	private objects = new ulist<Obj>();
	private viewports = new ulist<Viewport>();

	createObject<T>(object: Obj): ulist_elem<T> {
		return this.objects.push(object).translate<T>();
	}
	createViewport<T>(viewport: Viewport): ulist_elem<T> {
		return this.viewports.push(viewport).translate<T>();
	}

	renderPass() {
		this.viewports.visit((v: ulist_elem<Viewport>) => {
			this.objects.visit((e: ulist_elem<Obj>) => {
				e.get().render(v.get(), this.shader);
			});
		})
	}

	private shader = Shader.create("phong", "phong.vs", "phong.fs");
}
