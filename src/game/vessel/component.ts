import Obj from "../../object/object";
import { Vessel } from "./vessel";

abstract class Component extends Obj {
	constructor(private parent: Vessel) {
		super();
	}

	getTrans() {
		return this.parent.getTrans();
	}
}

export {
	Component
}