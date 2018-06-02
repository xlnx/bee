import { CameraBase } from "./cameraBase";
import { glm } from "../../util/glm"
import { Renderer, RendererEvent } from "../../renderer/renderer";
import { m2screen } from "../vessel/vessel";
import { Submarine } from "../vessel/submarine";

class Periscope extends CameraBase {
	private static speed: number = .5 * m2screen;
	private static maxH: number = 5 * m2screen;
	private static minH: number = 0.;

	private mousedown: RendererEvent;
	private mousemove: RendererEvent;
	private mouseup: RendererEvent;

	private prevx: number = 0;
	private isDragging: boolean = false;

	private risev: number;
	private diff: number = 0;

	private parent: Submarine;

	constructor() {
		super();
		this.zNear = 2e-2;
		this.fzFar = 8e2;
		// this.fov = glm.radians(5);
	}

	viewModified() {
		return true;
	}

	getView(): glm.mat4 {
		return super.getView()["*"](this.parent.getSpace());
	}

	getCameraPosition(): glm.vec3 {
		// let p = 
		// console.log(p.array);
		return this.parent.getTrival()["*"](glm.vec4(super.getCameraPosition(), 1)).xyz;
	}

	bind() {
		this.mousemove = Renderer.instance.dispatch("mousemove", (e: MouseEvent) => {
			if (this.isDragging) {
				if (Renderer.pause) {
					this.isDragging = false;
				} else {
					let c1 = Renderer.instance.canvas.height / Math.tan(this.ffov/2);
					let c2 = Renderer.instance.canvas.width;
					let h = 0.5 * Math.sqrt(c1 * c1 - c2 * c2);
					let dx1 = e.clientX - Renderer.instance.canvas.width / 2;
					let dx2 = this.prevx - Renderer.instance.canvas.width / 2;
					let thetax = Math.atan(dx1/h) - Math.atan(dx2/h);
					this.rotate(glm.vec3(0, 0, -thetax));
					this.prevx = e.clientX;
				}
			}
		});
		this.mouseup = Renderer.instance.dispatch("mouseup", (e: MouseEvent) => {
			this.isDragging = false;
		});
		this.mousedown = Renderer.instance.dispatch("mousedown", (e: MouseEvent) => {
			if ((e.clientX - 425) * (e.clientX - 425) + (e.clientY - 374) * (e.clientY - 374)
					<= 325 * 325) {
				this.prevx = e.clientX;
				this.isDragging = true;
			}
		});
	}

	unbind() {
		this.mousedown.cancel();
		this.mouseup.cancel();
		this.mousemove.cancel();
		this.prevx = 0;
		this.isDragging = false;
	}

	rise() {
		this.risev = 1;
	}

	down() {
		this.risev = -1;
	}

	stop() {
		this.risev = 0;
	}

	update(): number {
		if (this.risev) {
			let d = this.risev * 
				Periscope.speed * Renderer.dt;
			this.diff += d;
			let fix = this.checkRange();
			this.diff += fix;
			d += fix;
			this.translate(glm.vec3(0, 0, d));
			return d / (Periscope.maxH - Periscope.minH);
		}
		return 0;
	}

	private checkRange(): number { 
		if (this.diff > Periscope.maxH) {
			return Periscope.maxH - this.diff;
		} else if (this.diff < Periscope.minH) {
			return Periscope.minH - this.diff;
		} else {
			return 0;
		}
	}

	locate(pos: glm.vec3) {

	}

	configureUboat(uboat: Submarine) {
		this.parent = uboat;
		this.diff = 0;
		this.position = glm.vec3(0, m2screen * 3.8, m2screen * 5);
	}
}

export {
	Periscope
}