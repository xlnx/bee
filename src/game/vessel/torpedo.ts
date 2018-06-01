import { glm } from "../../util/glm";
import { Vessel } from "./vessel";
import { Renderer } from "../../renderer/renderer";

class Torpedo extends Vessel {
	private flifeTime: number;

	set orient(velocity: glm.vec2) {
		this.fspeedVec = velocity;
		this.rotate(glm.vec3(0, 0, -Math.acos(velocity.y)));
	}

	protected processProperty(data: { [key: string]: any}) {
		super.processProperty(data);
		this.flifeTime = data.lifetime;
	}

	protected updateState() {
		super.updateState();
		this.flifeTime -= Renderer.dt;
	}

	protected jitter() {
		//
	}

	get lifetime(): number {
		return this.flifeTime;
	}
}

export {
	Torpedo
}