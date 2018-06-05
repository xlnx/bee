import { glm } from "../../util/glm"
import { Shader } from "../../gl/shader";
import { Rect } from "../../techniques/rect";
import { gl, Renderer } from "../../renderer/renderer";
import { Texture2D } from "../../gl/texture";
import { Submarine } from "../vessel/submarine";

class PeriScreen {
	private rects: Rect[] = [];
	private main: Rect;
	private launchers: Rect[] = [];
	private knobs: Rect[] = [];
	private knob: Rect;
	private indicator: Rect;
	private indicators: Rect[] = [];

	private bg = new Texture2D("./assets/periscope.png");

	constructor() {
		let w = Renderer.instance.canvas.width;
		let h = Renderer.instance.canvas.height;
		this.rects.push(
			new Rect({
				left: 0, top: 0, width: w, height: h, 
				tex: [0, 1, 1, 1, 1, 0.25, 0, 0.25]
			}), 

			new Rect({
				left: 1090 + 14, top: 582 + 42, width: 60, height: 60,
				tex: [0.477, 0.182, 0.528, 0.182, 0.528, 0.131, 0.477, 0.131]
			}),
			new Rect({
				left: 1090 - 109 + 14, top: 582 + 42, width: 60, height: 60,
				tex: [0.477, 0.182 + 0.051, 0.528, 0.182 + 0.051, 0.528, 0.131 + 0.051, 0.477, 0.131 + 0.051]
			})
		);
		this.main = new Rect({
			left: 0, top: 0, width: w, height: h, 
			tex: [0.15, 0.95, 1.15, 0.95, 1.15, -0.05, 0.15, -0.05]
		});
		this.knobs = [
			new Rect({
				left: 858 + 14, top: 572 + 42, width: 80, height: 80,
				tex: [0.168, 0.103, 0.248, 0.103, 0.248, 0.033, 0.168, 0.033]
			}),
			new Rect({
				left: 858 + 14, top: 572 + 42, width: 80, height: 80,
				tex: [0.168 + 0.086, 0.103, 0.248 + 0.086, 0.103, 0.248 + 0.086, 0.033, 0.168 + 0.086, 0.033]
			}),
			new Rect({
				left: 858 + 14, top: 572 + 42, width: 80, height: 80,
				tex: [0.168 + 0.086 * 2, 0.103, 0.248 + 0.086 * 2, 0.103, 0.248 + 0.086 * 2, 0.033, 0.168 + 0.086 * 2, 0.033]
			}),
		];
		this.periscopeState(2);
	}

	configureUboat(uboat: Submarine) {
		this.launchers = [];
		let iu = 0, iv = 0;
		for (let i in uboat.torpedoLauncher) {
			let dx = 0.04285 * +i;
			if (uboat.torpedoLauncher[i].orient[1] > 0) {
				this.launchers.push(new Rect({
					left: 976 + 14 + 49 * iu++, top: 420 + 42, width: 47, height: 47,
					tex: [0.168 + dx, 0.145, 0.21 + dx, 0.145, 0.21 + dx, 0.104, 0.168 + dx, 0.104]
				}));
			} else {
				this.launchers.push(new Rect({
					left: 976 + 14 + 49 * iv++, top: 420 + 47 + 26 + 42, width: 47, height: 47,
					tex: [0.168 + dx, 0.145, 0.21 + dx, 0.145, 0.21 + dx, 0.104, 0.168 + dx, 0.104]
				}));
			}
		}
		this.indicator = new Rect({
			left: 780 + 14, top: 500 + 42, width: 22, height: 32,
			tex: [0.459, 0.031, 0.48, 0.031, 0.48, 0, 0.459, 0]
		});
	}

	periscopeState(state: number) {
		this.knob = this.knobs[state];
	}

	indicatorState(state: number) {
		this.indicator.displacement["+="](glm.vec2(0, state * 2 * (508 - 374) / Renderer.instance.canvas.height));
	}

	render(world: Texture2D) {
		gl.disable(gl.DEPTH_TEST);
		gl.clear(gl.COLOR_BUFFER_BIT);
		gl.enable(gl.BLEND);
		gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
		gl.depthMask(false);

			world.use("Texture");
			Rect.bindShader();

				this.main.render();

			Rect.unbindShader();
			world.unuse();

			this.bg.use("Texture");
			Rect.bindShader();

				for (let i = 0; i != this.rects.length; ++i) {
					this.rects[i].render();
				}

				for (let x of this.launchers) {
					x.render();
				}

				this.knob.render();
				this.indicator.render();

			Rect.unbindShader();
			this.bg.unuse();

		gl.depthMask(true);
		gl.disable(gl.BLEND);
	}
}

export {
	PeriScreen
}