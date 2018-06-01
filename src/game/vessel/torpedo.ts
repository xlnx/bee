import { glm } from "../../util/glm";
import { Vessel } from "./vessel";

class Torpedo extends Vessel {
	set orient(velocity: glm.vec2) {
		// console.log(velocity.array);
		this.fspeedVec = velocity;
		this.rotate(glm.vec3(0, 0, -Math.acos(velocity.y)));
	}

	protected processProperty(data: { [key: string]: any}) {
		super.processProperty(data);
	}
}

export {
	Torpedo
}